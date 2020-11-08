
#include "client_de.h"
#include "physics_lt.h"
#include "CMoveMgr.h"
#include "RiotClientShell.h"
#include "SharedMovement.h"
#include "RiotCommandIDs.h"
#include "ClientServerShared.h"
#include "RiotMsgIDs.h"


#define MIN_SWIM_ON_SURFACE_TIME	0.5f
#define SPECTATOR_ACCELERATION		100000.0f

#define LADDER_STOP_TIME			0.2f
#define SWIM_STOP_TIME				1.0f
#define SLIDE_TO_STOP_TIME			0.5f


class Pusher
{
public:
			Pusher()
			{
				m_Link.m_pData = this;
			}
	
	DVector	m_Pos;
	float	m_Radius;
	float	m_Delay;	// How long before it starts actually pushing.
	float	m_TimeLeft; // Time left for this sphere.
	float	m_Strength;
	DLink	m_Link;
};


CMoveMgr::CMoveMgr(CRiotClientShell *pShell)
{
	VEC_SET(m_WantedDims, 1, 1, 1);
	m_nStillAnims = 0;
	m_bSwimmingJump = DFALSE;
	m_eLastContainerCode = CC_NONE;
	m_nContainers = 0;
	m_hObject = DNULL;
	m_pClientDE = DNULL;
	m_pClientShell = pShell;
	dl_TieOff(&m_Pushers);
	m_bTBOn = DFALSE;
	m_dwControlFlags = 0;
	m_dwLastControlFlags = 0;
}


CMoveMgr::~CMoveMgr()
{
	DLink *pCur, *pNext;

	for(pCur=m_Pushers.m_pNext; pCur != &m_Pushers; pCur=pNext)
	{
		pNext = pCur->m_pNext;
		delete (Pusher*)pCur->m_pData;
	}
	dl_TieOff(&m_Pushers);
}


DBOOL CMoveMgr::Init(ClientDE *pClientDE)
{
	m_pClientDE = pClientDE;
	m_pPhysicsLT = pClientDE->Physics();

	// Init some defaults.  These should NEVER get used because we don't
	// have our object until the server sends the physics update.
	m_fSwimVel = 0.0f;
	m_fWalkVel = 0.0f;
	m_fRunVel = 400.0f;
	m_fJumpVel = 550.0f;
	m_fMoveMultiplier = 1.0f;
	m_bBodyInLiquid = DFALSE;
	m_bBodyOnLadder = DFALSE;
	m_bOnGround = DTRUE;
	m_bSpectatorMode = DFALSE;
	m_nMouseStrafeFlags = 0;
	m_bSwimmingOnSurface = DFALSE;
	m_bCanSwimJump = DFALSE;

	m_fLeashLen = 100.0f;
	m_fBaseMoveAccel = 0.0f;
	m_fMoveAccelMultiplier = 1.0f;
	m_fJumpMultiplier = 1.0f;

	return DTRUE;
}


void CMoveMgr::UpdateMouseStrafeFlags(float *pAxisOffsets)
{
	m_nMouseStrafeFlags = 0;

	if (pAxisOffsets[0] < 0.0f)
	{
		m_nMouseStrafeFlags |= SF_LEFT;
	}
	else if (pAxisOffsets[0] > 0.0f)
	{
		m_nMouseStrafeFlags |= SF_RIGHT;
	}

	if (pAxisOffsets[1] < 0.0f)
	{
		m_nMouseStrafeFlags |= SF_FORWARD;
	}
	else if (pAxisOffsets[1] > 0.0f)
	{
		m_nMouseStrafeFlags |= SF_BACKWARD;
	}

	pAxisOffsets[0] = 0.0f;
	pAxisOffsets[1] = 0.0f;
}


void CMoveMgr::UpdateAxisMovement(DBOOL bUseAxisForwardBackward, float fForwardBackward, float fForwardBackwardDeadZone,
								 DBOOL bUseAxisLeftRight, float fLeftRight, float fLeftRightDeadZone )
{
	m_bUseAxisForwardBackward = bUseAxisForwardBackward;
	m_fAxisForwardBackwardVel = fForwardBackward;
	m_fAxisForwardBackwardDeadZone = fForwardBackwardDeadZone;
	m_bUseAxisLeftRight = bUseAxisLeftRight;
	m_fAxisLeftRightVel = fLeftRight;
	m_fAxisLeftRightDeadZone = fLeftRightDeadZone;
/*
	if (m_bUseAxisForwardBackward)
	{
		if (m_fAxisForwardBackwardVel > m_fAxisForwardBackwardDeadZone)
		{
			m_pClientDE->SetCommandOn(COMMAND_ID_FORWARD);
		}
		else if (m_fAxisForwardBackwardVel < -m_fAxisForwardBackwardDeadZone)
		{
			m_pClientDE->SetCommandOn(COMMAND_ID_REVERSE);
		}
	}

	if (m_bUseAxisLeftRight)
	{
		if (m_fAxisLeftRightVel > m_fAxisLeftRightDeadZone)
		{
			m_pClientDE->SetCommandOn(COMMAND_ID_STRAFE_RIGHT);
		}
		else if (m_fAxisLeftRightVel < -m_fAxisLeftRightDeadZone)
		{
			m_pClientDE->SetCommandOn(COMMAND_ID_STRAFE_LEFT);
		}
	}
*/
}


void CMoveMgr::UpdateControlFlags()
{
	// Clear control flags...

	m_dwLastControlFlags = m_dwControlFlags;
	m_dwControlFlags = 0; 

	if (!m_hObject) 
		return;


	// Determine what commands are currently on...

	if (m_pClientDE->IsCommandOn(COMMAND_ID_RUN) || m_pClientShell->GetSettings()->RunLock())
	{
		m_dwControlFlags |= BC_CFLG_RUN;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_DUCK))
	{
		m_dwControlFlags |= BC_CFLG_DUCK;
	}

	// Only process jump if we aren't ducking...

	if (!(m_dwControlFlags & BC_CFLG_DUCK))
	{
		if (m_pClientDE->IsCommandOn(COMMAND_ID_JUMP))
		{
			m_dwControlFlags |= BC_CFLG_JUMP;
		}

		if (m_pClientDE->IsCommandOn(COMMAND_ID_DOUBLEJUMP))
		{
			// Can only double jump in MCA mode...
			if (m_pClientShell->IsOnFoot())
			{
				m_dwControlFlags |= BC_CFLG_JUMP;
			}
			else
			{
				m_dwControlFlags |= BC_CFLG_DOUBLEJUMP;
			}
		}
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_FORWARD))
	{
		m_dwControlFlags |= BC_CFLG_FORWARD;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_REVERSE))
	{
		m_dwControlFlags |= BC_CFLG_REVERSE;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_LEFT))
	{
		m_dwControlFlags |= BC_CFLG_LEFT;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_RIGHT))
	{
		m_dwControlFlags |= BC_CFLG_RIGHT;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_STRAFE))
	{
		m_dwControlFlags |= BC_CFLG_STRAFE;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_POSE))
	{
		m_dwControlFlags |= BC_CFLG_POSING;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_STRAFE_RIGHT))
	{
		m_dwControlFlags |= BC_CFLG_STRAFE_RIGHT;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_STRAFE_LEFT))
	{
		m_dwControlFlags |= BC_CFLG_STRAFE_LEFT;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_FIRING))
	{
		m_dwControlFlags |= BC_CFLG_FIRING;
	}

	if (m_pClientDE->IsCommandOn(COMMAND_ID_SPECIAL_MOVE))
	{
		m_dwControlFlags |= BC_CFLG_SPECIAL_MOVE;
	}

	// Check for strafe left and strafe right.
	if ((m_dwControlFlags & BC_CFLG_RIGHT) && (m_dwControlFlags & BC_CFLG_STRAFE))
	{
		m_dwControlFlags |= BC_CFLG_STRAFE_RIGHT;
	}

	if ((m_dwControlFlags & BC_CFLG_LEFT) && (m_dwControlFlags & BC_CFLG_STRAFE))
	{
		m_dwControlFlags |= BC_CFLG_STRAFE_LEFT;
	}
}


void CMoveMgr::UpdateInLiquid(CContainerInfo *pInfo)
{
	if(pInfo->m_bHidden)
		return;
	
	m_bBodyInLiquid = DTRUE;

	DBOOL bHeadInLiquid = IsLiquid(pInfo->m_ContainerCode);


	// Do REAL friction dampening (i.e., actually change our velocity)...

	DVector vVel;
	m_pPhysicsLT->GetVelocity(m_hObject, &vVel);

	DFLOAT fTimeDelta = m_pClientDE->GetFrameTime();

	DVector vCurVel;
	VEC_COPY(vCurVel, vVel);

	if (VEC_MAG(vCurVel) > 1.0f)
	{
		DVector vDir;
		VEC_COPY(vDir, vCurVel);
		VEC_NORM(vDir);

		DFLOAT fAdjust = m_pClientDE->GetFrameTime()*(m_fSwimVel/SWIM_STOP_TIME);

		VEC_MULSCALAR(vVel, vDir, fAdjust);

		if (VEC_MAG(vVel) < VEC_MAG(vCurVel))
		{
			VEC_SUB(vVel, vCurVel, vVel);
		}
		else
		{
			VEC_INIT(vVel);
		}
	}


	DVector curAccel;
	
	m_pPhysicsLT->GetAcceleration(m_hObject, &curAccel);

	// Handle floating around on the surface...

	if (m_bSwimmingOnSurface)
	{
		DBOOL bMoving = ((VEC_MAG(curAccel) > 0.01f) ||
						 (VEC_MAG(vVel) > 0.01f));
	
		// Disable gravity.
		m_pClientDE->SetObjectFlags(m_hObject, 
			m_pClientDE->GetObjectFlags(m_hObject) & ~FLAG_GRAVITY);

		if (bMoving)  // Turn off y acceleration and velocity
		{
			if (vVel.y > 0.0f || curAccel.y > 0.0f)
			{
				vVel.y = 0.0f;
				curAccel.y = 0.0f;
			}
		}
		else // Pull us down if we're not moving (fast enough)
		{
			curAccel.y += pInfo->m_fGravity;
		}
	}
	else if (IsLiquid(pInfo->m_ContainerCode))
	{
		m_pClientDE->SetObjectFlags(m_hObject, 
			m_pClientDE->GetObjectFlags(m_hObject) & ~FLAG_GRAVITY);

		curAccel.y += pInfo->m_fGravity;
	}

	m_pPhysicsLT->SetVelocity(m_hObject, &vVel);
	m_pPhysicsLT->SetAcceleration(m_hObject, &curAccel);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseCharacter::UpdateOnLadder
//
//	PURPOSE:	Update movement when on a ladder
//
// ----------------------------------------------------------------------- //

void CMoveMgr::UpdateOnLadder(CContainerInfo *pInfo)
{
	if(pInfo->m_bHidden)
		return;

	m_bBodyOnLadder = DTRUE;

	// Do REAL friction dampening (i.e., actually change our velocity)...

	DVector vVel;
	m_pPhysicsLT->GetVelocity(m_hObject, &vVel);

	DVector vCurVel;
	VEC_COPY(vCurVel, vVel);

	if (VEC_MAG(vCurVel) > 1.0f)
	{
		DVector vDir;
		VEC_COPY(vDir, vCurVel);
		VEC_NORM(vDir);

		DFLOAT fAdjust = m_pClientDE->GetFrameTime()*(m_fLadderVel/LADDER_STOP_TIME);

		VEC_MULSCALAR(vVel, vDir, fAdjust);

		if (VEC_MAG(vVel) < VEC_MAG(vCurVel))
		{
			VEC_SUB(vVel, vCurVel, vVel);
		}
		else
		{
			VEC_INIT(vVel);
		}

		m_pPhysicsLT->SetVelocity(m_hObject, &vVel);
	}

	m_pClientDE->SetObjectFlags(m_hObject, 
		m_pClientDE->GetObjectFlags(m_hObject) & ~FLAG_GRAVITY);
}


void CMoveMgr::UpdateOnGround( DVector *pvAccel )
{
	// Lets see if we are in the ground or in the air.

	CollisionInfo Info;
	m_pPhysicsLT->GetStandingOn(m_hObject, &Info);

	if (Info.m_hObject) 
	{
		m_bOnGround = DTRUE;

		if (Info.m_Plane.m_Normal.y < 0.76)
		{
			DVector vCross1, vCross2;
			VEC_SET(vCross1, 0.0, 1.0, 0.0);
			VEC_CROSS(vCross2, Info.m_Plane.m_Normal, vCross1);
			VEC_NORM(vCross2);
			VEC_CROSS(vCross1, vCross2, Info.m_Plane.m_Normal);
			VEC_MULSCALAR(vCross1, vCross1, -2000.0f);
			VEC_ADD(*pvAccel, *pvAccel, vCross1);

			m_bOnGround = DFALSE;

			// Force us down...

			DVector vVel;
			m_pPhysicsLT->GetVelocity(m_hObject, &vVel);

			vVel.y = - VEC_MAG(vVel);
			vVel.x = vVel.z = 0.0f;
			m_pPhysicsLT->SetVelocity(m_hObject, &vVel);
		}
	} 
	else 
	{
		m_bOnGround = DFALSE;
	}
}


DBOOL CMoveMgr::IsBodyInLiquid()
{
	DDWORD i;

	for(i=0; i < m_nContainers; i++)
	{
		if(IsLiquid(m_Containers[i].m_ContainerCode))
			return DTRUE;
	}
	return DFALSE;
}


DBOOL CMoveMgr::IsMecha()
{
	return !m_pClientShell->IsOnFoot();
}


DBOOL CMoveMgr::IsDead()
{
	return m_pClientShell->GetPlayerState() != PS_ALIVE;
}


void CMoveMgr::UpdateMotion()
{
	DFLOAT fTime = m_pClientDE->GetTime();
	DBOOL bDidLiquid, bDidLadder;
	DDWORD i, objFlags;
	float fMoveAccel;
	DVector curVel, myPos;
	DVector myDims;


	// Zero out the acceleration to start with.
	DVector zeroVec;
	zeroVec.Init();
	m_pPhysicsLT->SetAcceleration(m_hObject, &zeroVec);


	// Normally we have gravity on, but the containers might turn it off.
	objFlags = m_pClientDE->GetObjectFlags(m_hObject) | FLAG_GRAVITY;
	if(m_pClientShell->IsSpectatorMode() || m_bTBOn)
		objFlags &= ~FLAG_GRAVITY;
	m_pClientDE->SetObjectFlags(m_hObject, objFlags);
	
	m_bBodyInLiquid = m_bBodyOnLadder = DFALSE;
	bDidLiquid = bDidLadder = DFALSE;
	for(i=0; i < m_nContainers; i++)
	{
		if(IsLiquid(m_Containers[i].m_ContainerCode) && !bDidLiquid)
		{
			UpdateInLiquid(&m_Containers[i]);
			bDidLiquid = DTRUE;
		}
		else if(m_Containers[i].m_ContainerCode == CC_LADDER && !bDidLadder)
		{
			UpdateOnLadder(&m_Containers[i]);
			bDidLadder = DTRUE;
		}
	
		if(!m_Containers[i].m_bHidden)
		{
			m_pPhysicsLT->GetVelocity(m_hObject, &curVel);
			curVel += m_Containers[i].m_Current * m_pClientDE->GetFrameTime();
			m_pPhysicsLT->SetVelocity(m_hObject, &curVel);
		}
	}


	// See if we can run.
	DBOOL bAllowRun = !m_pClientShell->IsZoomed();


	// Get our current rotation.
	DRotation myRot;
	m_pClientShell->GetCameraRotation(&myRot);
	m_pClientDE->SetObjectRotation(m_hObject, &myRot);



	DVector myVel;
	m_pPhysicsLT->GetVelocity(m_hObject, &myVel);
			
	DVector moveVel;			
	VEC_COPY(moveVel, myVel);
	moveVel.y = 0;

	float fMaxVel;
	float fMaxVelLimit;
	DBOOL bHeadInLiquid = IsLiquid(m_pClientShell->GetCurContainerCode());
	DBOOL bInLiquid	    = (bHeadInLiquid || IsBodyInLiquid());
	DBOOL bFreeMovement = (IsFreeMovement(m_pClientShell->GetCurContainerCode()) || m_bBodyOnLadder ||
		m_pClientShell->IsSpectatorMode());

	// Check for running...

	if (((m_dwControlFlags & BC_CFLG_RUN) || (m_bUseAxisForwardBackward || m_bUseAxisLeftRight)) && bAllowRun)
	{
		fMaxVel = (m_bBodyInLiquid ? m_fSwimVel : (m_bBodyOnLadder ? m_fLadderVel : m_fRunVel));
	}
	else
	{
		fMaxVel = (m_bBodyInLiquid ? m_fSwimVel/2.0f : (m_bBodyOnLadder ? m_fLadderVel/2.0f : m_fWalkVel));
	}

	fMaxVel *= m_fMoveMultiplier;


	DBOOL bJumping = DFALSE;
	if ((m_dwControlFlags & BC_CFLG_JUMP || m_dwControlFlags & BC_CFLG_DOUBLEJUMP))
	{
		bJumping = DTRUE;
	}

	DBOOL bJumpedLastFrame = DFALSE;
	if ((m_dwLastControlFlags & BC_CFLG_JUMP || m_dwLastControlFlags & BC_CFLG_DOUBLEJUMP))
	{
		bJumpedLastFrame = DTRUE;
	}

	// Cap Velocity for joystick or other velocity related input device
	{
		DBOOL OverrideForwardBackward = ((m_dwControlFlags & BC_CFLG_FORWARD) || (m_nMouseStrafeFlags & SF_FORWARD)
				|| (m_dwControlFlags & BC_CFLG_REVERSE) || (m_nMouseStrafeFlags & SF_BACKWARD));
		DBOOL OverrideLeftRight = ((m_dwControlFlags & BC_CFLG_STRAFE_LEFT) || (m_nMouseStrafeFlags & SF_LEFT)
				|| (m_dwControlFlags & BC_CFLG_STRAFE_RIGHT) || (m_nMouseStrafeFlags & SF_RIGHT));

		if ((m_bUseAxisForwardBackward || m_bUseAxisLeftRight) && !(OverrideForwardBackward && OverrideLeftRight))
		{
			float fLeftRightCap = fMaxVel;
			float fForwardBackwardCap = fMaxVel;

			if (m_bUseAxisForwardBackward && !OverrideForwardBackward)
			{
				fForwardBackwardCap = fMaxVel * m_fAxisForwardBackwardVel;
			}

			if (m_bUseAxisLeftRight && !OverrideLeftRight)
			{
				fLeftRightCap = fMaxVel * m_fAxisLeftRightVel;
			}

			fMaxVelLimit = (float)sqrt(fForwardBackwardCap*fForwardBackwardCap + fLeftRightCap*fLeftRightCap);
		}
		else fMaxVelLimit = fMaxVel;
	}


	// Limit velocity...

	
	if (m_bBodyInLiquid || m_bBodyOnLadder)
	{
		// Cap the velocity in all directions...

		VEC_COPY(moveVel, myVel);

		if (VEC_MAG(moveVel) > fMaxVelLimit)
		{
			VEC_NORM(moveVel);
			VEC_MULSCALAR(moveVel, moveVel, fMaxVelLimit);

			m_pPhysicsLT->SetVelocity(m_hObject, &moveVel);
		}
	}
	else if (m_bOnGround && !m_bSpectatorMode && !bJumping) 
	{
		float curLen = (float)sqrt(myVel.x*myVel.x + myVel.z*myVel.z);
		if(curLen > fMaxVelLimit)
		{
			VEC_MULSCALAR(myVel, myVel, (fMaxVelLimit/curLen));

			m_pPhysicsLT->SetVelocity(m_hObject, &myVel);
		}
	}
	else if (VEC_MAG(moveVel) > fMaxVelLimit)
	{
		// Don't cap velocity in the y direction...

		VEC_NORM(moveVel);
		VEC_MULSCALAR(moveVel, moveVel, fMaxVelLimit);

		myVel.x = moveVel.x;
		myVel.z = moveVel.z;

		m_pPhysicsLT->SetVelocity(m_hObject, &myVel);
	}


	// Zero our acceleration to start with.
	// Even if we are dead we will keep moving in whatever
	// direction we were moving to begin with...
	
	DVector vAccel, vUp, vRight, vForward, vOriginalForward;
	
	m_pPhysicsLT->GetAcceleration(m_hObject, &vAccel);

	// Update m_bOnGround data member...

	UpdateOnGround( &vAccel );


	// See if we just broke the surface of water...

	if ((IsLiquid(m_eLastContainerCode) && !bHeadInLiquid) && !m_bOnGround)
	{
		m_fSwimmingOnSurfaceStart = fTime;
		m_bSwimmingOnSurface	  = DTRUE;
	}
	else if (bHeadInLiquid)  // See if we went back under...
	{
		m_fSwimmingOnSurfaceStart = -1.0f;
		m_bSwimmingOnSurface	  = DFALSE;
		m_bCanSwimJump			  = DTRUE;
	}	


	// If we're doing a swimming jump, keep jumping while we're not out of 
	// the water (and we're still trying to get out)...

	if (m_bSwimmingJump)
	{
		m_bSwimmingJump = (m_bBodyInLiquid && bJumping);
	}


	DRotation rRotation;
	if(m_bSpectatorMode || m_bSwimmingOnSurface || m_bBodyOnLadder ||
		IsFreeMovement(m_pClientShell->GetCurContainerCode()))
	{
		m_pClientShell->GetCameraRotation(&rRotation);
	}
	else
	{
		m_pClientDE->GetObjectRotation(m_hObject, &rRotation);
	}

	m_pClientDE->GetRotationVectors(&rRotation, &vUp, &vRight, &vOriginalForward);

	VEC_COPY(vForward, vOriginalForward);

	if (!IsMecha())
	{
		vRight.y = 0;
		VEC_NORM(vRight);
	}
	

	fMoveAccel = (m_fBaseMoveAccel * m_fMoveAccelMultiplier);

	if (m_bSpectatorMode)
	{
		DVector v;
		VEC_INIT(v);
		m_pPhysicsLT->SetVelocity(m_hObject, &v);
		fMoveAccel = SPECTATOR_ACCELERATION;
	}
	else if (!bInLiquid && !bFreeMovement)  // Can only move forward in x and z directions
	{
		vForward.y = 0.0;
		VEC_NORM(vForward);
	}


	DFLOAT fJumpVel = (m_fJumpVel * m_fJumpMultiplier);

	// If we have the speed key down and we aren't dead
	// make us move faster...

	if ((m_dwControlFlags & BC_CFLG_RUN) && !IsDead())
	{
		if (!IsMecha())
		{
			fMoveAccel *= 2.0f;
		}
		else
		{
			fMoveAccel *= 2.5f;
			fJumpVel     *= 1.1f;
		}
	}

	if ((m_dwControlFlags & BC_CFLG_DUCK) && !m_bBodyInLiquid && !m_bBodyOnLadder)
	{
		fMoveAccel /= 2.0f;
	}


	// If we aren't dead we can walk around

	if (!IsDead()) 
	{
		DVector vTemp;

		if ( (m_dwControlFlags & BC_CFLG_FORWARD) || (m_nMouseStrafeFlags & SF_FORWARD) )
		{
			VEC_MULSCALAR(vTemp, vForward, fMoveAccel);
			VEC_ADD(vAccel, vAccel, vTemp);
		}
		
		if ( (m_dwControlFlags & BC_CFLG_REVERSE) || (m_nMouseStrafeFlags & SF_BACKWARD) )
		{
			VEC_MULSCALAR(vTemp, vForward, -fMoveAccel);
			VEC_ADD(vAccel, vAccel, vTemp);
		}
/*
		// set the movement for velocity axis
		if (m_bUseAxisForwardBackward || m_bUseAxisLeftRight)
		{
			// get current velocity
			DVector velCurrent;
			m_pPhysicsLT->GetVelocity(m_hObject, &velCurrent);

			if (m_bUseAxisForwardBackward)
			{
				// figure out current velocity component forward
				float fVelCurrentForward;
				fVelCurrentForward = VEC_DOT(vForward, velCurrent);

				// is joystick pointing forward?
				if (m_fAxisForwardBackwardVel > m_fAxisForwardBackwardDeadZone)
				{
					// check if we are going to apply acceleration forward
					if (fVelCurrentForward < (m_fAxisForwardBackwardVel * fMaxVel))
					{
						VEC_MULSCALAR(vTemp, vForward, fMoveAccel);
						VEC_ADD(vAccel, vAccel, vTemp);
					}
				}

				// is joystick pointing backward?
				else if (m_fAxisForwardBackwardVel < -m_fAxisForwardBackwardDeadZone)
				{
					// check if we are going to apply acceleration backward
					if (fVelCurrentForward > (m_fAxisForwardBackwardVel * fMaxVel))
					{
						VEC_MULSCALAR(vTemp, vForward, -fMoveAccel);
						VEC_ADD(vAccel, vAccel, vTemp);
					}
				}
			}

			if (m_bUseAxisLeftRight)
			{
				// figure out current velocity component to the right
				float fVelCurrentRight;
				fVelCurrentRight = VEC_DOT(vRight, velCurrent);

				// is joystick pointing right?
				if (m_fAxisLeftRightVel > m_fAxisLeftRightDeadZone)
				{
					// check if we are going to apply acceleration right
					if (fVelCurrentRight < (m_fAxisLeftRightVel * fMaxVel))
					{
						VEC_MULSCALAR(vTemp, vRight, fMoveAccel);
						VEC_ADD(vAccel, vAccel, vTemp);
					}
				}

				// is joystick pointing left?
				else if (m_fAxisLeftRightVel < -m_fAxisLeftRightDeadZone)
				{
					// check if we are going to apply acceleration left
					if (fVelCurrentRight > (m_fAxisLeftRightVel * fMaxVel))
					{
						VEC_MULSCALAR(vTemp, vRight, -fMoveAccel);
						VEC_ADD(vAccel, vAccel, vTemp);
					}
				}
			}
		}
*/

		// If we are in a container that supports free movement, see if we are 
		// moving up or down...

		if (bInLiquid || bFreeMovement)
		{
			if (bJumping)
			{
				if (bInLiquid)
				{
					if (bHeadInLiquid || m_bSwimmingOnSurface)
					{
						vAccel.y = (m_dwControlFlags & BC_CFLG_DOUBLEJUMP) ? 1.5f * fMoveAccel : fMoveAccel;
					}
				}	
				else
				{
					vAccel.y += (m_dwControlFlags & BC_CFLG_DOUBLEJUMP) ? 1.5f * fMoveAccel : fMoveAccel;
				}
			}
			if (m_dwControlFlags & BC_CFLG_DUCK)
			{
				vAccel.y = -fMoveAccel;
			}
		}


		if ( (m_dwControlFlags & BC_CFLG_STRAFE_LEFT) || (m_nMouseStrafeFlags & SF_LEFT) )
		{
			VEC_MULSCALAR(vTemp, vRight, -fMoveAccel);
			VEC_ADD(vAccel, vAccel, vTemp);
		}
			
		if ( (m_dwControlFlags & BC_CFLG_STRAFE_RIGHT) || (m_nMouseStrafeFlags & SF_RIGHT) )
		{
			VEC_MULSCALAR(vTemp, vRight, fMoveAccel);
			VEC_ADD(vAccel, vAccel, vTemp);
		}
	}

	// reset the mouse strafe flags in case they are set

	m_nMouseStrafeFlags = 0;

	m_pPhysicsLT->SetAcceleration(m_hObject, &vAccel);

	DVector vel;
	m_pPhysicsLT->GetVelocity(m_hObject, &vel);
	
	// We can jump if we are not dead...

	if (bJumping && !IsDead())
	{
		if (!bHeadInLiquid && m_bBodyInLiquid)
		{
			if (m_bCanSwimJump)
			{
				m_bSwimmingJump = DTRUE;
				m_bCanSwimJump  = DFALSE;
			}
			// If our head is out of the liquid and we're standing on the
			// ground, let us jump out of the water...
			else if (m_bOnGround)
			{
				m_bSwimmingJump = DTRUE;
			}
		}

		DBOOL bOkayToJump = (m_bOnGround && !m_bBodyInLiquid && !bJumpedLastFrame);

		if (bOkayToJump || m_bSwimmingJump)
		{
			m_bSwimmingOnSurface = DFALSE;
			vel.y = (m_dwControlFlags & BC_CFLG_DOUBLEJUMP) ? 1.5f * fJumpVel : fJumpVel;
		}
	}


	// If in spectator mode, dampen velocity.
	if(m_pClientShell->IsSpectatorMode())
	{
		vel *= 0.9f;
		if(vel.Mag() < 0.1f)
			vel.Init();
	}

	
	// If tractor beam is on, move us towards it.
	if(m_bTBOn)
	{
		m_pClientDE->GetObjectPos(m_hObject, &myPos);
		vel = m_TBPos - myPos;

		// If we're pretty close to the destination, don't set it.
		m_pPhysicsLT->GetObjectDims(m_hObject, &myDims);
		if(vel.Mag() < myDims.Mag())
		{
			vel.Init();
		}
		else
		{
			vel.Norm();
			vel *= m_TBSpeed;
		}
	}


	m_pPhysicsLT->SetVelocity(m_hObject, &vel);

	// If we're dead, we can't move.
	if(m_pClientShell->GetPlayerState() == PS_DEAD || 
		m_pClientShell->GetPlayerState() == PS_DYING || 
		!m_pClientShell->IsPlayerMovementAllowed() ||
		IsInStillAnim())
	{
		DVector zeroVec;

		zeroVec.Init();
		m_pPhysicsLT->SetAcceleration(m_hObject, &zeroVec);
		m_pPhysicsLT->SetVelocity(m_hObject, &zeroVec);
		//MoveToClientObj();
	}

	m_eLastContainerCode = m_pClientShell->GetCurContainerCode();
}


void CMoveMgr::UpdatePushers()
{
	DLink *pCur, *pNext;
	Pusher *pPusher;
	DVector myPos, pushVec, vel;
	float dist, velocity, frameTime;
	CollisionInfo info;
	ClientIntersectQuery iQuery;
	ClientIntersectInfo iInfo;


	if(!m_hObject || !m_pClientDE || !m_pPhysicsLT)
		return;

	frameTime = m_pClientDE->GetFrameTime();

	m_pClientDE->GetObjectPos(m_hObject, &myPos);
	for(pCur=m_Pushers.m_pNext; pCur != &m_Pushers; pCur=pNext)
	{
		pNext = pCur->m_pNext;

		pPusher = (Pusher*)pCur->m_pData;

		pPusher->m_Delay -= frameTime;
		if(pPusher->m_Delay <= 0.0f)
		{
			pPusher->m_TimeLeft -= frameTime;
			if(pPusher->m_TimeLeft <= 0.0f)
			{
				// Expired..
				dl_Remove(&pPusher->m_Link);
				delete pPusher;
			}
			else
			{
				// Are we within range?
				dist = VEC_DIST(pPusher->m_Pos, myPos);
				if(dist < pPusher->m_Radius)
				{
					memset(&iQuery, 0, sizeof(iQuery));
					iQuery.m_From = pPusher->m_Pos;
					iQuery.m_To = myPos;
					if(!m_pClientDE->IntersectSegment(&iQuery, &iInfo))
					{
						velocity = 1.0f - (dist / pPusher->m_Radius);
						velocity *= pPusher->m_Strength;

						// If we're in the air, apply less (since there's no friction).
						m_pPhysicsLT->GetStandingOn(m_hObject, &info);					
						if(!info.m_hObject) 
						{
							velocity /= 10.0f;
						}

						pushVec = myPos - pPusher->m_Pos;
						pushVec.Norm(velocity);

						m_pPhysicsLT->GetVelocity(m_hObject, &vel);
						vel += pushVec;
						m_pPhysicsLT->SetVelocity(m_hObject, &vel);
					}
				}
			}
		}
	}
}


void CMoveMgr::UpdatePlayerAnimation()
{
	HOBJECT hClientObj;
	DDWORD modelAnim, curModelAnim, curFlags;
	DVector oldDims, offset;

	
	if(!(hClientObj = m_pClientDE->GetClientObject()))
		return;
	
	// Make sure our solid object is on the same animation.
	modelAnim = m_pClientDE->GetModelAnimation(hClientObj);
	curModelAnim = m_pClientDE->GetModelAnimation(m_hObject);
	if(modelAnim != curModelAnim)
	{
		// Kind of wierd what we do here.. the engine sets the dims automatically when
		// we change animations but it doesn't do collision detection (and we don't want
		// it to) so we may end up clipping into the world so we set it to a small cube
		// and resize the dims with collision detection.
		curFlags = m_pClientDE->GetObjectFlags(m_hObject);
		m_pClientDE->SetObjectFlags(m_hObject, (curFlags|FLAG_GOTHRUWORLD) & ~FLAG_SOLID);

		m_pClientDE->SetModelAnimation(m_hObject, modelAnim);
		
		// Get our wanted dims.
		oldDims = m_WantedDims;
		VEC_SET(m_WantedDims, 1, 1, 1);
		m_pClientDE->Common()->GetModelAnimUserDims(m_hObject, &m_WantedDims, modelAnim);
		m_WantedDims *= m_DimsScale[MS_NORMAL];

		// Figure out a position offset.
		VEC_INIT(offset);
		if (m_WantedDims.y < oldDims.y)
		{
			offset.y = -(oldDims.y - m_WantedDims.y);
			offset.y += .01f; // Fudge factor
		}

		m_pClientDE->SetObjectFlags(m_hObject, curFlags);
		
		// This makes you small before setting the dims so you don't clip thru stuff.
		ResetDims(&offset);
	}
}


DBOOL CMoveMgr::AreDimsCorrect()
{
	DVector curDims;

	if(!m_hObject || !m_pPhysicsLT)
		return DTRUE;

	m_pPhysicsLT->GetObjectDims(m_hObject, &curDims);
	return fabs(curDims.x-m_WantedDims.x) < 0.1f && fabs(curDims.y-m_WantedDims.y) < 0.1f &&
		fabs(curDims.z-m_WantedDims.z) < 0.1f;
}


void CMoveMgr::ResetDims(DVector *pOffset)
{
	DVector smallDims, pos;

	smallDims.Init(0.5f, 0.5f, 0.5f);
	m_pPhysicsLT->SetObjectDims(m_hObject, &smallDims, 0);
	
	// Move them if they want.
	if(pOffset)
	{
		m_pClientDE->GetObjectPos(m_hObject, &pos);
		pos += *pOffset;
		m_pPhysicsLT->MoveObject(m_hObject, &pos, 0);
	}
	
	m_pPhysicsLT->SetObjectDims(m_hObject, &m_WantedDims, SETDIMS_PUSHOBJECTS);
}


void CMoveMgr::MoveLocalSolidObject()
{
	MoveInfo info;
	DVector newPos, curPos;

	info.m_hObject = m_hObject;
	info.m_dt = m_FrameTime;
	m_pPhysicsLT->UpdateMovement(&info);

	if(info.m_Offset.MagSqr() > 0.01f)
	{
		m_pClientDE->GetObjectPos(m_hObject, &curPos);
		newPos = curPos + info.m_Offset;
		m_pPhysicsLT->MoveObject(m_hObject, &newPos, 0);
	}
}


void CMoveMgr::UpdateVelMagnitude()
{
	DVector vel;

	if(!m_pPhysicsLT || !m_pClientShell || !m_hObject)
		return;

	m_pPhysicsLT->GetVelocity(m_hObject, &vel);
	m_pClientShell->SetVelMagnitude(vel.Mag());
}


void CMoveMgr::SetClientObjNonsolid()
{
	HOBJECT hObj;

	if(hObj = m_pClientDE->GetClientObject())
	{
		m_pClientDE->SetObjectFlags(hObj, m_pClientDE->GetObjectFlags(hObj)|FLAG_CLIENTNONSOLID);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerObj::UpdateFriction
//
//	PURPOSE:	Update player fricton
//
// ----------------------------------------------------------------------- //

void CMoveMgr::UpdateFriction()
{
	// If we're in bipedal mode, dampen our velocity so we don't slide 
	// around too much...

	DDWORD dwTestFlags = BC_CFLG_MOVING | BC_CFLG_JUMP | BC_CFLG_DOUBLEJUMP;
	if (!(m_dwControlFlags & dwTestFlags) && m_bOnGround && 
		(m_pClientShell->IsOnFoot() || !m_pClientShell->IsVehicleMode()) && 
		(m_pClientShell->GetCurContainerCode() == CC_NONE))
	{
		DVector vCurVel;
		m_pPhysicsLT->GetVelocity(m_hObject, &vCurVel);

		DFLOAT fYVal = vCurVel.y;
		vCurVel.y = 0.0f;

		if (VEC_MAG(vCurVel) > 1.0f)
		{
			DVector vDir, vVel;
			VEC_COPY(vDir, vCurVel);
			VEC_NORM(vDir);

			DFLOAT fAdjust = m_pClientDE->GetFrameTime()*(m_fRunVel/SLIDE_TO_STOP_TIME);

			VEC_MULSCALAR(vVel, vDir, fAdjust);

			if (VEC_MAG(vVel) < VEC_MAG(vCurVel))
			{
				VEC_SUB(vVel, vCurVel, vVel);
			}
			else
			{
				VEC_INIT(vVel);
			}

			vVel.y = fYVal;
			m_pPhysicsLT->SetVelocity(m_hObject, &vVel);
		}
	}
}


void CMoveMgr::ShowPos(char *pBlah)
{
	DVector pos;
	m_pClientDE->GetObjectPos(m_hObject, &pos);
	m_pClientDE->CPrint("%s: %.1f %.1f %.1f", pBlah, VEC_EXPAND(pos));
}

void CMoveMgr::Update()
{
	HOBJECT hObj;

	hObj = m_pClientDE->GetClientObject();
	if(!m_hObject || !hObj)
		return;
			   
	// m_pClientDE->CPrint("On Ground: %s", m_bOnGround ? "TRUE" : "FALSE");

	m_FrameTime = m_pClientDE->GetFrameTime();
	if(m_FrameTime > 0.1f)
		m_FrameTime = 0.1f;

	// We don't want to hit the real client object.
	SetClientObjNonsolid();

	UpdatePlayerAnimation();

	UpdateControlFlags();

	UpdateMotion();


	// Update friction...
	UpdateFriction();

	UpdatePushers();

	// Make sure we have desired dims.
	if(!AreDimsCorrect())
		ResetDims();
	
	MoveLocalSolidObject();
	
	PostUpdate();

	UpdateVelMagnitude();

	// Clear these and they'll get set if mouse is moving.
	m_nMouseStrafeFlags = 0;
}


void CMoveMgr::OnPhysicsUpdate(HMESSAGEREAD hRead)
{
	ObjectCreateStruct theStruct;
	D_WORD changeFlags;
	DVector grav;
	HOBJECT hClientObj;
	DDWORD i, nDimsScales;
	float coeff;


	if(!m_pClientDE)
		return;

	changeFlags = m_pClientDE->ReadFromMessageWord(hRead);

	// Change our model file?
	if(changeFlags & PSTATE_MODELFILENAMES)
	{
		if(m_hObject)
			m_pClientDE->DeleteObject(m_hObject);

		INIT_OBJECTCREATESTRUCT(theStruct);
		SAFE_STRCPY(theStruct.m_Filename, m_pClientDE->ReadFromMessageString(hRead));
		SAFE_STRCPY(theStruct.m_SkinName, m_pClientDE->ReadFromMessageString(hRead));
		theStruct.m_ObjectType = OT_MODEL;
		theStruct.m_Flags = FLAG_SOLID | FLAG_GRAVITY | FLAG_STAIRSTEP;
		
		if(hClientObj = m_pClientDE->GetClientObject())
		{
			m_pClientDE->GetObjectPos(hClientObj, &theStruct.m_Pos);
		}

		m_hObject = m_pClientDE->CreateObject(&theStruct);
		if(m_hObject)
		{
			m_pClientDE->SetObjectClientFlags(m_hObject, CF_DONTSETDIMS|CF_NOTIFYREMOVE);
		}

		// Read still animations.
		m_nStillAnims = m_pClientDE->ReadFromMessageByte(hRead);
		if(m_nStillAnims > MAX_STILL_ANIMATIONS)
		{
			m_nStillAnims = 0;
			return;
		}

		for(i=0; i < m_nStillAnims; i++)
		{
			strncpy(m_StillAnimNames[i], m_pClientDE->ReadFromMessageString(hRead), MAX_STILL_ANIM_NAME_LEN-1);
			m_hStillAnims[i] = m_pClientDE->GetAnimIndex(m_hObject, m_StillAnimNames[i]);
		}

		// Read dims scale.
		nDimsScales = m_pClientDE->ReadFromMessageByte(hRead);
		if(nDimsScales != NUM_MODELSIZES)
			return;

		for(i=0; i < NUM_MODELSIZES; i++)
		{
			m_DimsScale[i] = m_pClientDE->ReadFromMessageFloat(hRead);
		}
	}

	if(changeFlags & PSTATE_TRACTORBEAM)
	{
		m_bTBOn = m_pClientDE->ReadFromMessageByte(hRead);
		m_pClientDE->ReadFromMessageCompPosition(hRead, &m_TBPos);
		m_TBSpeed = m_pClientDE->ReadFromMessageFloat(hRead);
	}
	
	if(changeFlags & PSTATE_GRAVITY)
	{
		m_pClientDE->ReadFromMessageVector(hRead, &grav);
		m_pPhysicsLT->SetGlobalForce(grav);
	}

	if(changeFlags & PSTATE_CONTAINERTYPE)
	{
		m_nContainers = m_pClientDE->ReadFromMessageByte(hRead);
		if(m_nContainers >= MAX_TRACKED_CONTAINERS)
			return;

		for(i=0; i < m_nContainers; i++)
		{
			m_Containers[i].m_ContainerCode = (ContainerCode)m_pClientDE->ReadFromMessageByte(hRead);
			m_pClientDE->ReadFromMessageVector(hRead, &m_Containers[i].m_Current);
			m_Containers[i].m_fGravity = m_pClientDE->ReadFromMessageFloat(hRead);
			m_Containers[i].m_bHidden = m_pClientDE->ReadFromMessageByte(hRead);
		}
	}

	if(changeFlags & PSTATE_SPEEDS)
	{
		m_fWalkVel = m_pClientDE->ReadFromMessageFloat(hRead);
		m_fRunVel = m_pClientDE->ReadFromMessageFloat(hRead);
		m_fSwimVel = m_pClientDE->ReadFromMessageFloat(hRead);
		m_fJumpVel = m_pClientDE->ReadFromMessageFloat(hRead);

		m_fMoveMultiplier = m_pClientDE->ReadFromMessageFloat(hRead);
		m_fMoveAccelMultiplier = m_pClientDE->ReadFromMessageFloat(hRead);

		m_fLeashLen = m_pClientDE->ReadFromMessageFloat(hRead);
		m_fBaseMoveAccel = m_pClientDE->ReadFromMessageFloat(hRead);
		m_fJumpMultiplier = m_pClientDE->ReadFromMessageFloat(hRead);
		m_fLadderVel = m_pClientDE->ReadFromMessageFloat(hRead);
		
		coeff = m_pClientDE->ReadFromMessageFloat(hRead);
		m_pPhysicsLT->SetFrictionCoefficient(m_hObject, coeff);
	}
}


DRESULT CMoveMgr::OnObjectMove(HOBJECT hObj, DBOOL bTeleport, DVector *pPos)
{
	HOBJECT hClientObj;
	DDWORD type;

	if(!m_hObject || !m_pClientDE)
		return LT_OK;

	hClientObj = m_pClientDE->GetClientObject();

	// If it's a solid world model moving, do a regular MoveObject on it so it
	// can carry/push the player object around.
	if(!bTeleport && hObj != hClientObj && hObj != m_hObject)
	{
		type = m_pClientDE->GetObjectType(hObj);
		if(type == OT_WORLDMODEL)
		{
			if(m_pClientDE->GetObjectFlags(hObj) & FLAG_SOLID)
			{
				m_pPhysicsLT->MovePushObjects(hObj, *pPos, &m_hObject, 1);
			}
		}	
	}

	return LT_OK;
}


DRESULT CMoveMgr::OnObjectRotate(HOBJECT hObj, DBOOL bTeleport, DRotation *pNewRot)
{
	HOBJECT hClientObj;
	DDWORD type;

	if(!m_hObject || !m_pClientDE)
		return LT_OK;

	hClientObj = m_pClientDE->GetClientObject();

	// If it's a solid world model moving, do a regular MoveObject on it so it
	// can carry/push the player object around.
	if(!bTeleport && hObj != hClientObj && hObj != m_hObject)
	{
		type = m_pClientDE->GetObjectType(hObj);
		if(type == OT_WORLDMODEL)
		{
			if(m_pClientDE->GetObjectFlags(hObj) & FLAG_SOLID)
			{
				m_pPhysicsLT->RotatePushObjects(hObj, *pNewRot, &m_hObject, 1);
			}
		}	
	}

	return LT_OK;
}


void CMoveMgr::OnObjectRemove(HOBJECT hObj)
{
	if(hObj == m_hObject)
	{
		m_hObject = DNULL;
	}
}


DRESULT CMoveMgr::AddPusher(DVector &pos, float radius, float startDelay, float duration, float strength)
{
	Pusher *pPusher;

	if(!(pPusher = new Pusher))
		return LT_ERROR;
	
	pPusher->m_Pos = pos;
	pPusher->m_Radius = radius;
	pPusher->m_Delay = startDelay;
	pPusher->m_TimeLeft = duration;
	pPusher->m_Strength = strength;
	dl_Insert(&m_Pushers, &pPusher->m_Link);
	
	return LT_OK;
}


void CMoveMgr::MoveToClientObj()
{
	DVector cObjPos;
	HOBJECT hObj;

	if(!m_hObject || !(hObj = m_pClientDE->GetClientObject()))
		return;

	m_pClientDE->GetObjectPos(hObj, &cObjPos);
	m_pPhysicsLT->MoveObject(m_hObject, &cObjPos, MOVEOBJECT_TELEPORT);
}


void CMoveMgr::PostUpdate()
{
/*
	HOBJECT hObj;
	DVector myPos;
	DRotation myRot;


	// Move the server's object to our position (the server object is going to be off a little).
	if(!m_pClientShell || !m_hObject || !(hObj = m_pClientDE->GetClientObject()))
		return;

	m_pClientDE->GetObjectPos(m_hObject, &myPos);
	m_pClientDE->SetObjectPos(hObj, &myPos);

	m_pClientShell->GetCameraRotation(&myRot);
	m_pClientDE->SetObjectRotation(hObj, &myRot);
*/
}


void CMoveMgr::SetSpectatorMode(DBOOL bSet)
{
	DVector vPos, vZero;
	DDWORD curFlags;

	if(!m_hObject || !m_pPhysicsLT)
		return;

	if(bSet)
	{
		// Move up.
		m_pClientDE->GetObjectPos(m_hObject, &vPos);
		vPos.y += 50;
		m_pPhysicsLT->MoveObject(m_hObject, &vPos, MOVEOBJECT_TELEPORT);

		curFlags = m_pClientDE->GetObjectFlags(m_hObject);
		curFlags |= FLAG_GOTHRUWORLD;
		curFlags &= ~FLAG_SOLID;
		m_pClientDE->SetObjectFlags(m_hObject, curFlags);
		
		vZero.Init();
		m_pPhysicsLT->SetVelocity(m_hObject, &vZero);
		m_pPhysicsLT->SetAcceleration(m_hObject, &vZero);
	}
	else
	{
		curFlags = m_pClientDE->GetObjectFlags(m_hObject);
		curFlags &= ~FLAG_GOTHRUWORLD;
		curFlags |= FLAG_SOLID;
		m_pClientDE->SetObjectFlags(m_hObject, curFlags);
	}
}


void CMoveMgr::OnTractorBeamPos(HMESSAGEREAD hRead)
{
	if(!m_pClientDE)
		return;

	m_pClientDE->ReadFromMessageCompPosition(hRead, &m_TBPos);
}


void CMoveMgr::OnServerForcePos(HMESSAGEREAD hRead)
{
	DVector zeroVec, tempDims, pos, curDims;

	if(!m_pClientDE || !m_pPhysicsLT)
		return;

	m_ClientMoveCode = m_pClientDE->ReadFromMessageByte(hRead);

	// Teleport to where it says.
	if(m_hObject)
	{
		SetClientObjNonsolid();

		// Move there.  We make our object a point first and then resize the dims so 
		// we don't teleport clipping into the world.
		m_pClientDE->ReadFromMessageVector(hRead, &pos);
		
		tempDims.Init(0.5f, 0.5f, 0.5f);
		m_pPhysicsLT->GetObjectDims(m_hObject, &curDims);
		m_pPhysicsLT->SetObjectDims(m_hObject, &tempDims, 0);
		m_pPhysicsLT->MoveObject(m_hObject, &pos, MOVEOBJECT_TELEPORT);
		m_pPhysicsLT->SetObjectDims(m_hObject, &curDims, SETDIMS_PUSHOBJECTS);

		// Clear our velocity and acceleration.
		zeroVec.Init();
		m_pPhysicsLT->SetVelocity(m_hObject, &zeroVec);
		m_pPhysicsLT->SetAcceleration(m_hObject, &zeroVec);
	}
}


void CMoveMgr::WritePositionInfo(HMESSAGEWRITE hWrite)
{
	DVector myPos, myVel;
	short compVel[3];

	if(!m_pClientDE)
		return;

	if(m_hObject)
	{
		m_pClientDE->GetObjectPos(m_hObject, &myPos);
		m_pPhysicsLT->GetVelocity(m_hObject, &myVel);
	}
	else
	{
		myPos.Init();
		myVel.Init();
	}
	
	m_pClientDE->WriteToMessageByte(hWrite, m_ClientMoveCode);
	m_pClientDE->WriteToMessageVector(hWrite, &myPos);
	
	compVel[0] = (short)myVel.x;
	compVel[1] = (short)myVel.y;
	compVel[2] = (short)myVel.z;
	m_pClientDE->WriteToMessageWord(hWrite, compVel[0]);
	m_pClientDE->WriteToMessageWord(hWrite, compVel[1]);
	m_pClientDE->WriteToMessageWord(hWrite, compVel[2]);
	
	m_pClientDE->WriteToMessageByte(hWrite, (DBYTE)m_bOnGround);
}


DBOOL CMoveMgr::IsInStillAnim()
{
	DDWORD i;
	HOBJECT hClientObj;
	HMODELANIM curAnim;


	if(!(hClientObj = m_pClientDE->GetClientObject()))
		return DFALSE;
	
	// Make sure our solid object is on the same animation.
	curAnim = m_pClientDE->GetModelAnimation(hClientObj);

	// Is our current animation a still animation?
	for(i=0; i < m_nStillAnims; i++)
	{
		if(curAnim == m_hStillAnims[i])
			return DTRUE;
	}
	return DFALSE;
}


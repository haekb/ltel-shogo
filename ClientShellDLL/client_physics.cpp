
#include "de_client.h"
#include "client_physics.h"
#include "ClientServerShared.h"
#include "ContainerCodes.h"


extern PhysicsState	  g_normalPhysicsState;
extern PhysicsState	  g_waterPhysicsState;

void InitMovingObject(MovingObject *pObject, DVector *pPos, DVector *pVelocity)
{
	memset(pObject, 0, sizeof(MovingObject));
	VEC_COPY(pObject->m_Pos, *pPos);
	VEC_COPY(pObject->m_LastPos, *pPos);
	VEC_COPY(pObject->m_Velocity, *pVelocity);

	ClientDE* pClientDE = g_normalPhysicsState.m_pClientDE;
	if (!pClientDE) return;

	// Determine if we are in any containers (like liquid) that would
	// affect physics...

	DDWORD dwUserFlags;
	HLOCALOBJ objList[1];
	DDWORD dwNum = pClientDE->GetPointContainers(&pObject->m_Pos, objList, 1);

	if (dwNum > 0 && objList[0])
	{
		pClientDE->GetObjectUserFlags(objList[0], &dwUserFlags);
	}

	if (dwNum > 0 && (dwUserFlags & USRFLG_VISIBLE))
	{
		D_WORD code;
		if (pClientDE->GetContainerCode(objList[0], &code))
		{
			ContainerCode eCode = (ContainerCode)code;

			if (IsLiquid(eCode))
			{
				pObject->m_PhysicsFlags |= MO_LIQUID;
			}
		}
	}
}


void SetPhysicsStateTimeStep(PhysicsState *pState, float timeStep)
{
	pState->m_TimeStep = timeStep;
	pState->m_TimeStepIntegral = timeStep*timeStep*0.5f;
}


DBOOL UpdateMovingObject(PhysicsState *pUserState, MovingObject *pObject, DVector *pNewPos)
{
	if (!pObject || !pNewPos) return DFALSE;

	PhysicsState* pState = pUserState ? pUserState : GetCurPhysicsState(pObject);
	if (!pState) return DFALSE;

	DVector vTemp, velocityDelta, posDelta;

	if(pObject->m_PhysicsFlags & MO_RESTING)
		return DFALSE;
	
	// Prevent tiny movements.
	if(VEC_MAGSQR(pObject->m_Acceleration) < 0.01f)
	{
		VEC_INIT(pObject->m_Acceleration);
	}

	if(VEC_MAGSQR(pObject->m_Velocity) < 0.01f)
	{
		VEC_INIT(pObject->m_Velocity);
	}

	// velocityDelta = ( acceleration + accelDelta * 0.5 ) * dt;
	VEC_INIT(vTemp);
	if (!(pObject->m_PhysicsFlags & MO_NOGRAVITY))
	{
		DFLOAT fScale = 0.5f;
		if (pObject->m_PhysicsFlags & MO_HALFGRAVITY)
		{
			fScale = 0.25f;
		}
		VEC_MULSCALAR(vTemp, pState->m_GravityAccel, fScale);
	}
	VEC_ADD(vTemp, vTemp, pObject->m_Acceleration);
	VEC_MULSCALAR(velocityDelta, vTemp, pState->m_TimeStep);

	// Apply the velocity to the position (p = p + vt + 0.5a(t^2)).
	VEC_MULSCALAR(posDelta, pObject->m_Acceleration, pState->m_TimeStepIntegral);
	VEC_ADDSCALED(posDelta, posDelta, pObject->m_Velocity, pState->m_TimeStep);

	// Add the final velocity to the new velocity.
	VEC_ADD(pObject->m_Velocity, pObject->m_Velocity, velocityDelta);

	if(!pNewPos)
		pNewPos = &pObject->m_Pos;

	VEC_ADD(*pNewPos, pObject->m_Pos, posDelta);
	
	// Zero out the acceleration.
	VEC_INIT(pObject->m_Acceleration);

	return DTRUE;
}


DBOOL BounceMovingObject(PhysicsState *pUserState, MovingObject *pObject, 
						 DVector *pNewPos, ClientIntersectInfo* pInfo)
{
	if (!pObject || !pNewPos || !pInfo) return DFALSE;

	PhysicsState* pState = pUserState ? pUserState : GetCurPhysicsState(pObject);
	if (!pState) return DFALSE;

	ClientIntersectQuery query;
	float dot;

	// Only do an intersection test if the line is long enough (sometimes the 
	// intersection test will fail on really short lines).
	memset(&query, 0, sizeof(query));
	VEC_COPY(query.m_From, pObject->m_Pos);
	VEC_COPY(query.m_To, *pNewPos);

	// Move the to point out to fix the "short line" bug...

	//DVector vDir;
	//VEC_SUB(vDir, query.m_From, query.m_To);
	//VEC_NORM(vDir);
	//VEC_MULSCALAR(vDir, vDir, 50.0f);
	//VEC_ADD(query.m_To, query.m_From, vDir);


	if (pState->m_pClientDE->IntersectSegment(&query, pInfo))
	{
		// Move the dest point a little in front of the plane.
		VEC_ADDSCALED(*pNewPos, pInfo->m_Point, pInfo->m_Plane.m_Normal, 0.3f);
	
		// Reflect the velocity.
		dot = VEC_DOT(pObject->m_Velocity, pInfo->m_Plane.m_Normal);
		dot *= -2.0f;

		VEC_ADDSCALED(pObject->m_Velocity, pObject->m_Velocity, pInfo->m_Plane.m_Normal, dot);

		// Dampen it.
		VEC_MULSCALAR(pObject->m_Velocity, pObject->m_Velocity, pState->m_VelocityDampen);
		
		// This assumes we're updating at 40 fps and our minimum velocity is 15 pixels per second
		// (15*15*40 is the maximum magnitude).
		if(pInfo->m_Plane.m_Normal.y > 0.6f && (VEC_MAGSQR(pObject->m_Velocity) < (15.0f*15.0f*40.0f)))
		{
			pObject->m_PhysicsFlags |= MO_RESTING;
		}

		return DTRUE;
	}

	return DFALSE;
}

// Based on the current position of the object, determine the physics state
// we should use...

PhysicsState* GetCurPhysicsState(MovingObject *pObject)
{
	if (!pObject) return DNULL;

	return ((pObject->m_PhysicsFlags & MO_LIQUID) ? &g_waterPhysicsState : &g_normalPhysicsState);
}






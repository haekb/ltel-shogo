
// Client side movement manager for Shogo.

#ifndef __CMOVEMGR_H__
#define __CMOVEMGR_H__

	
	#include "client_de.h"
	#include "ContainerCodes.h"
	#include "SharedMovement.h"
	#include "ModelFuncs.h"

	
	class CPhysicsLT;
	class CRiotClientShell;

	
	class CContainerInfo
	{
	public:
		float			m_fGravity;
		DVector			m_Current;
		ContainerCode	m_ContainerCode;
		DBOOL			m_bHidden;
	};

	
	class CMoveMgr
	{
	public:

					CMoveMgr(CRiotClientShell *pShell);
					~CMoveMgr();
		
		DBOOL		Init(ClientDE *pClientDE);
		
		void		Update();
		void		PostUpdate();
		void		OnPhysicsUpdate(HMESSAGEREAD hRead);
		void		UpdateMouseStrafeFlags(float *pAxisOffsets);
		void		UpdateAxisMovement(DBOOL bUseAxisForwardBackward, float fForwardBackward, float fForwardBackwardDeadZone,
									   DBOOL bUseAxisLeftRight,float fLeftRight, float fLeftRightDeadZone );

		DRESULT		OnObjectMove(HOBJECT hObj, DBOOL bTeleport, DVector *pPos);
		DRESULT		OnObjectRotate(HOBJECT hObj, DBOOL bTeleport, DRotation *pNewRot);

		void		OnObjectRemove(HOBJECT hObj);

		// CMoveMgr keeps a list of spheres that repel the player object.
		// These are created from explosions.
		DRESULT		AddPusher(DVector &pos, float radius, 
			float startDelay, float duration, float strength);

		HOBJECT		GetObject() {return m_hObject;}

		void		SetSpectatorMode(DBOOL bSet);
		void		OnTractorBeamPos(HMESSAGEREAD hRead);
		void		OnServerForcePos(HMESSAGEREAD hRead);

		void		WritePositionInfo(HMESSAGEWRITE hWrite);

	protected:

		void		ShowPos(char *pBlah);
		void		UpdateControlFlags();
		void		UpdateMotion();
		void		UpdatePushers();
		void		UpdatePlayerAnimation();
		
		DBOOL		AreDimsCorrect();
		void		ResetDims(DVector *pOffset=NULL);
		
		void		UpdateFriction();
		
		void		UpdateOnLadder(CContainerInfo *pInfo);
		void		UpdateInLiquid(CContainerInfo *pInfo);
		void		UpdateOnGround( DVector *pvAccel );
		
		void		MoveLocalSolidObject();
		void		UpdateVelMagnitude();
		void		SetClientObjNonsolid();
		void		MoveToClientObj();

		DBOOL		IsMecha();
		DBOOL		IsBodyInLiquid();
		DBOOL		IsDead();

		DBOOL		IsInStillAnim();

		
		DBYTE		m_ClientMoveCode;

		// The object representing our movement.  
		HOBJECT		m_hObject;

		ClientDE			*m_pClientDE;
		CPhysicsLT			*m_pPhysicsLT;
		CRiotClientShell	*m_pClientShell;

		float		m_DimsScale[NUM_MODELSIZES];
		DLink		m_Pushers;


		DVector		m_WantedDims;

		// Tractor beam info.
		DVector		m_TBPos;
		float		m_TBSpeed;
		DBOOL		m_bTBOn;
		
		// Movement state.
		DDWORD		m_dwControlFlags;
		DDWORD		m_dwLastControlFlags;
		DDWORD		m_nMouseStrafeFlags;

		DBOOL		m_bBodyInLiquid;
		DBOOL		m_bSwimmingOnSurface;
		DBOOL		m_bCanSwimJump;
		float		m_fSwimmingOnSurfaceStart;
		
		DBOOL		m_bBodyOnLadder;
		DBOOL		m_bSpectatorMode;
		DBOOL		m_bForceJump;

		DBOOL		m_bOnGround;
		
		float		m_fBaseMoveAccel;
		float		m_fMoveAccelMultiplier;

		float		m_fLeashLen;
		float		m_FrameTime;


		// Movement speeds.
		float		m_fJumpVel;
		float		m_fJumpMultiplier;

		float		m_fSwimVel;
		float		m_fWalkVel;
		float		m_fRunVel;
		float		m_fLadderVel;
		float		m_fMoveMultiplier;

		// special movement axis information
		DBOOL		m_bUseAxisLeftRight;
		DBOOL		m_bUseAxisForwardBackward;
		float		m_fAxisLeftRightVel; // Right is positive
		float		m_fAxisLeftRightDeadZone; 
		float		m_fAxisForwardBackwardVel; // Forward is positive ?????????
		float		m_fAxisForwardBackwardDeadZone; 
		
		DBOOL		m_bSwimmingJump;

		ContainerCode	m_eLastContainerCode;
		CContainerInfo	m_Containers[MAX_TRACKED_CONTAINERS];
		DDWORD			m_nContainers;

		// Still animations.
		char			m_StillAnimNames[MAX_STILL_ANIMATIONS][MAX_STILL_ANIM_NAME_LEN];
		HMODELANIM		m_hStillAnims[MAX_STILL_ANIMATIONS];
		DDWORD			m_nStillAnims;
	};


#endif  // __CMOVEMGR_H__



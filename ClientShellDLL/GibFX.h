// ----------------------------------------------------------------------- //
//
// MODULE  : GibFX.h
//
// PURPOSE : Gib - Definition
//
// CREATED : 6/15/98
//
// ----------------------------------------------------------------------- //

#ifndef __GIB_FX_H__
#define __GIB_FX_H__

#include "SpecialFX.h"
#include "client_physics.h"
#include "ModelFuncs.h"
#include "GibTypes.h"
#include "ContainerCodes.h"

#define MAX_GIB 20

struct GIBCREATESTRUCT : public SFXCREATESTRUCT
{
	GIBCREATESTRUCT::GIBCREATESTRUCT();

	DRotation	rRot;
	DVector		vPos;
	DVector		vMinVel;
	DVector		vMaxVel;
	DFLOAT		fLifeTime;
	DFLOAT		fFadeTime;
	DBYTE		nGibFlags;
	DBYTE		nCode;
	DBOOL		bRotate;
	DBYTE		nModelId;
	DBYTE		nSize;
	DBYTE		nCharacterClass;
	DBOOL		bSubGibs;
	DBOOL		bBloodSplats;
	DBYTE		nNumGibs;
	GibType		eGibTypes[MAX_GIB];
};

inline GIBCREATESTRUCT::GIBCREATESTRUCT()
{
	memset(this, 0, sizeof(GIBCREATESTRUCT));
	ROT_INIT(rRot);
}

class CGibFX : public CSpecialFX
{
	public :

		CGibFX() : CSpecialFX() 
		{
			VEC_INIT(m_vMinVel);
			VEC_INIT(m_vMaxVel);

			m_fLifeTime		= 0.0f;
			m_fFadeTime		= 0.0f;

			m_bFirstUpdate	= DTRUE;
			m_fLastTime		= -1.0f;
			m_fStartTime	= -1.0f;

			memset(m_Emmitters, 0, sizeof(MovingObject)*MAX_GIB);
			memset(m_ActiveEmmitters, 0, sizeof(DBOOL)*MAX_GIB);
			memset(m_BounceCount, 0, sizeof(DBYTE)*MAX_GIB);
			memset(m_hGib, 0, sizeof(HOBJECT)*MAX_GIB);
			memset(m_pGibTrail, 0, sizeof(CSpecialFX*)*MAX_GIB);
			memset(m_eGibTypes, 0, sizeof(GibType)*MAX_GIB);
			memset(m_fGibLife, 0, sizeof(DFLOAT)*MAX_GIB);
			m_nNumGibs		= 0;
			m_nGibFlags		= 0;
			m_bSubGibs		= DFALSE;
			m_bBloodSplats	= DFALSE;
			
			m_nNumRandomGibs = 2;

			m_nModelId			= MI_UNDEFINED;
			m_eCode				= CC_NONE;
			m_eSize				= MS_NORMAL;
			m_eCharacterClass	= UNKNOWN;

			m_bCurGibOnGround	= DFALSE;
			m_bPlayBounceSound  = DTRUE;

			m_bRotate			= DFALSE;
			m_fPitch			= 0.0f;
			m_fYaw				= 0.0f;
			m_fPitchVel			= 0.0f;
			m_fYawVel			= 0.0f;
		}

		~CGibFX()
		{
			RemoveAllFX();
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update();

	private :

		DRotation m_rRot;		// Direction of velocities
		DVector	m_vPos;			// Where Gib starts

		DVector	m_vMinVel;		// Minimum emmitter velocity
		DVector	m_vMaxVel;		// Maximum emmitter velocity

		DFLOAT	m_fFadeTime;	// When system should start to fade
		DFLOAT	m_fLifeTime;	// How long system stays around
		DFLOAT	m_fStartTime;	// When did we start this crazy thing

		DFLOAT	m_fLastTime;	// Last time we created some particles
		DBOOL	m_bFirstUpdate;	// First update

		DBYTE			m_nModelId;			// Model
		ContainerCode	m_eCode;			// Container code
		ModelSize		m_eSize;			// Size of model
		CharacterClass	m_eCharacterClass;	// Character class of model

		ModelType		m_eModelType;		// Type of model gibbed

		MovingObject	m_Emmitters[MAX_GIB];			// Gib emmitters
		DBYTE			m_nNumGibs;						// Num in array
		DBYTE			m_nGibFlags;					// MoveObject flags
		DBOOL			m_ActiveEmmitters[MAX_GIB];		// Active?	
		DBYTE			m_BounceCount[MAX_GIB];			// Number of bounces
		HLOCALOBJ		m_hGib[MAX_GIB];				// Gib models
		CSpecialFX*		m_pGibTrail[MAX_GIB];			// Blood trails
		GibType			m_eGibTypes[MAX_GIB];			// Types of gibs
		DFLOAT			m_fGibLife[MAX_GIB];			// Life time of the gib
		DBYTE			m_nNumRandomGibs;				// Num random gibs

		DBOOL			m_bCurGibOnGround;
		DBOOL			m_bPlayBounceSound;
		DBOOL			m_bSubGibs;
		DBOOL			m_bBloodSplats;

		// Emmitter rotation stuff...
		
		DBOOL			m_bRotate;
		DFLOAT			m_fPitch;		
		DFLOAT			m_fYaw;
		DFLOAT			m_fRoll;
		DFLOAT			m_fPitchVel;
		DFLOAT			m_fYawVel;
		DFLOAT			m_fRollVel;

		ClientIntersectInfo m_info;  // Last bounce info

		DBOOL		UpdateEmmitter(MovingObject* pObject);
		void		UpdateGib(int nIndex, DBOOL bBounced);
		HLOCALOBJ	CreateGib(GibType eType);
		HLOCALOBJ	CreateRandomGib();
		CSpecialFX*	CreateGibTrail(HLOCALOBJ hObj);

		void		CreateBloodSpray();
		void		CreateMiniBloodExplosion(int nIndex);
		void		CreateLingeringSmoke(int nIndex);
		void		HandleBounce(int nIndex);
		void		HandleDoneBouncing(int nIndex);
		char*		GetBounceSound();
		char*		GetGibDieSound();

		void		RemoveAllFX();
		DBOOL		OkToRemoveGib(HLOCALOBJ hGib);
};

#endif // __GIB_FX_H__
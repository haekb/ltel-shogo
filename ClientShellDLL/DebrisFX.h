// ----------------------------------------------------------------------- //
//
// MODULE  : DebrisFX.h
//
// PURPOSE : Debris - Definition
//
// CREATED : 5/31/98
//
// ----------------------------------------------------------------------- //

#ifndef __DEBRIS_FX_H__
#define __DEBRIS_FX_H__

#include "SpecialFX.h"
#include "client_physics.h"
#include "DebrisTypes.h"

#define MAX_DEBRIS 20

struct DEBRISCREATESTRUCT : public SFXCREATESTRUCT
{
	DEBRISCREATESTRUCT::DEBRISCREATESTRUCT();

	DRotation	rRot;
	DVector		vPos;
	DVector		vMinVel;
	DVector		vMaxVel;
	DFLOAT		fLifeTime;
	DFLOAT		fFadeTime;
	DBYTE		nNumDebris;
	DBYTE		nDebrisFlags;
	DBOOL		bRotate;
	DBYTE		nDebrisType;
	DFLOAT		fMinScale;
	DFLOAT		fMaxScale;
	DBOOL		bPlayBounceSound;
	DBOOL		bPlayExplodeSound;
	DBOOL		bForceRemove;
};

inline DEBRISCREATESTRUCT::DEBRISCREATESTRUCT()
{
	memset(this, 0, sizeof(DEBRISCREATESTRUCT));
	ROT_INIT(rRot);
}

class CDebrisFX : public CSpecialFX
{
	public :

		CDebrisFX() : CSpecialFX() 
		{
			VEC_INIT(m_vMinVel);
			VEC_INIT(m_vMaxVel);

			m_fLifeTime		= 0.0f;
			m_fFadeTime		= 0.0f;

			m_bFirstUpdate	= DTRUE;
			m_fLastTime		= -1.0f;
			m_fStartTime	= -1.0f;

			memset(m_Emmitters, 0, sizeof(MovingObject)*MAX_DEBRIS);
			memset(m_ActiveEmmitters, 0, sizeof(DBOOL)*MAX_DEBRIS);
			memset(m_BounceCount, 0, sizeof(DBYTE)*MAX_DEBRIS);
			memset(m_hDebris, 0, sizeof(HOBJECT)*MAX_DEBRIS);
			memset(m_fPitch, 0, sizeof(DFLOAT)*MAX_DEBRIS);
			memset(m_fYaw, 0, sizeof(DFLOAT)*MAX_DEBRIS);
			memset(m_fRoll, 0, sizeof(DFLOAT)*MAX_DEBRIS);
			memset(m_fPitchVel, 0, sizeof(DFLOAT)*MAX_DEBRIS);
			memset(m_fYawVel, 0, sizeof(DFLOAT)*MAX_DEBRIS);
			memset(m_fRollVel, 0, sizeof(DFLOAT)*MAX_DEBRIS);
			memset(m_fDebrisLife, 0, sizeof(DFLOAT)*MAX_DEBRIS);

			m_nNumDebris		= 0;
			m_nDebrisFlags		= 0;
			m_eDebrisType		= DBT_GENERIC;

			m_bRotate			= DFALSE;
			m_fMinScale			= 1.0f;
			m_fMaxScale			= 1.0f;
			m_bForceRemove		= DFALSE;

			m_bPlayBounceSound	= DFALSE;
			m_bPlayExplodeSound	= DFALSE;
		}

		~CDebrisFX()
		{
			for (int i=0; i < m_nNumDebris; i++)
			{
				if (m_hDebris[i] && m_pClientDE)
				{
					m_pClientDE->DeleteObject(m_hDebris[i]);
				}
			}
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update();

	private :

		DRotation m_rRot;		// Direction of velocities
		DVector	m_vPos;			// Where debris starts

		DVector	m_vLastPos;		// Last Particle particle position
		DVector	m_vMinVel;		// Minimum emmitter velocity
		DVector	m_vMaxVel;		// Maximum emmitter velocity

		DFLOAT	m_fFadeTime;	// When system should start to fade
		DFLOAT	m_fLifeTime;	// How long system stays around
		DFLOAT	m_fStartTime;	// When did we start this crazy thing

		DFLOAT	m_fLastTime;	// Last time we created some particles
		DBOOL	m_bFirstUpdate;	// First update

		MovingObject	m_Emmitters[MAX_DEBRIS];			// Debris emmitters
		DBYTE			m_nNumDebris;						// Num in array
		DBYTE			m_nDebrisFlags;						// MoveObject flags
		DBOOL			m_ActiveEmmitters[MAX_DEBRIS];		// Active?	
		DBYTE			m_BounceCount[MAX_DEBRIS];			// Number of bounces
		HLOCALOBJ		m_hDebris[MAX_DEBRIS];
		DFLOAT			m_fDebrisLife[MAX_DEBRIS];

		DebrisType		m_eDebrisType;		// Debris type
		DFLOAT			m_fMinScale;		// Min model scale
		DFLOAT			m_fMaxScale;		// Max model scale
	
		DBOOL			m_bPlayBounceSound;	 // Play a sound when we bounce
		DBOOL			m_bPlayExplodeSound; // Play a sound when we explode

		DBOOL			m_bForceRemove;

		// Emmitter rotation stuff...
		
		DBOOL			m_bRotate;
		DFLOAT			m_fPitch[MAX_DEBRIS];		
		DFLOAT			m_fYaw[MAX_DEBRIS];
		DFLOAT			m_fRoll[MAX_DEBRIS];
		DFLOAT			m_fPitchVel[MAX_DEBRIS];
		DFLOAT			m_fYawVel[MAX_DEBRIS];
		DFLOAT			m_fRollVel[MAX_DEBRIS];

		DBOOL		UpdateEmmitter(MovingObject* pObject);
		HLOCALOBJ	CreateDebris();
		DBOOL		OkToRemoveDebris(HLOCALOBJ hDebris);
};

#endif // __DEBRIS_FX_H__
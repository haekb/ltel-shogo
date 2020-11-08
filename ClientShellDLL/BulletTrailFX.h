// ----------------------------------------------------------------------- //
//
// MODULE  : BulletTrailFX.h
//
// PURPOSE : SmokeTrail segment special fx class - Definition
//
// CREATED : 3/1/98
//
// ----------------------------------------------------------------------- //

#ifndef __BULLET_TRAIL_FX_H__
#define __BULLET_TRAIL_FX_H__

#include "BaseParticleSystemFX.h"


struct BTCREATESTRUCT : public SFXCREATESTRUCT
{
	BTCREATESTRUCT::BTCREATESTRUCT();

	DVector vStartPos;
	DVector vDir;
	DVector vColor1;
	DVector vColor2;
	DFLOAT  fLifeTime;
	DFLOAT	fFadeTime;
	DFLOAT	fRadius;
	DFLOAT  fGravity;
	DFLOAT  fNumParticles;
};

inline BTCREATESTRUCT::BTCREATESTRUCT()
{
	memset(this, 0, sizeof(BTCREATESTRUCT));
}

class CBulletTrailFX : public CBaseParticleSystemFX
{
	public :

		CBulletTrailFX() : CBaseParticleSystemFX() 
		{
			VEC_INIT(m_vStartPos);
			VEC_INIT(m_vLastPos);
			VEC_INIT(m_vDir);

			m_fLifeTime		= 0.0f;
			m_fFadeTime		= 0.0f;
			m_fNumParticles	= 100.0f;

			m_bFirstUpdate	= DTRUE;
			m_fDistance		= 0.0f;
			m_fDistTraveled	= 0.0f;
			m_fTrailVel		= 0.0f;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update();

	private :

		DVector	m_vLastPos;		// Last bubble particle position
		DVector	m_vStartPos;	// Starting position of trail
		DVector	m_vDir;			// Direction of trail

		DFLOAT	m_fFadeTime;	// When system should start to fade
		DFLOAT	m_fLifeTime;	// How long system stays around
		DFLOAT	m_fNumParticles;// Total number of particles in system

		DBOOL	m_bFirstUpdate;	// First update
		DFLOAT	m_fDistance;	// Length of trail
		DFLOAT	m_fDistTraveled;// How far have we gone?
		DFLOAT	m_fTrailVel;	// Speed of trail

		DFLOAT	m_fStartTime;	// When did we start
		DFLOAT	m_fLastTime;	// When was the last update
};

#endif // __BULLET_TRAIL_FX_H__
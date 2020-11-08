// ----------------------------------------------------------------------- //
//
// MODULE  : ParticleTrailSegmentFX.h
//
// PURPOSE : ParticleTrail segment special fx class - Definition
//
// CREATED : 4/27/98
//
// ----------------------------------------------------------------------- //

#ifndef __PARTICLE_TRAIL_SEGMENT_FX_H__
#define __PARTICLE_TRAIL_SEGMENT_FX_H__

#include "BaseParticleSystemFX.h"


struct PTSCREATESTRUCT : public SFXCREATESTRUCT
{
	PTSCREATESTRUCT::PTSCREATESTRUCT();

	DBYTE	nType;
	DVector vColor1;
	DVector vColor2;
	DVector vDriftOffset;
	DBOOL   bSmall;
	DFLOAT  fLifeTime;
	DFLOAT	fFadeTime;
	DFLOAT  fOffsetTime;
	DFLOAT	fRadius;
	DFLOAT  fGravity;
	DBYTE   nNumPerPuff;
};

inline PTSCREATESTRUCT::PTSCREATESTRUCT()
{
	memset(this, 0, sizeof(PTSCREATESTRUCT));
}

class CParticleTrailSegmentFX : public CBaseParticleSystemFX
{
	public :

		CParticleTrailSegmentFX() : CBaseParticleSystemFX() 
		{
			VEC_INIT(m_vLastPos);
			VEC_INIT(m_vDriftOffset);

			m_fLifeTime		= 0.0f;
			m_fFadeTime		= 0.0f;
			m_fOffsetTime	= 0.0f;
			m_nNumPerPuff	= 1;
			m_bSmall		= DFALSE;
			m_nType			= 0;

			m_bFirstUpdate	= DTRUE;
			m_fLastTime		= -1.0f;
			m_fStartTime	= -1.0f;

			m_bIgnoreWind	= DFALSE;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update();

	private :

		DVector	m_vLastPos;		// Last Particle particle position
		DVector	m_vDriftOffset;	// Particle drift offset

		DFLOAT	m_fFadeTime;	// When system should start to fade
		DFLOAT	m_fLifeTime;	// How long system stays around
		DFLOAT	m_fStartTime;	// When did we start this crazy thing
		DFLOAT	m_fOffsetTime;	// Time between particles

		DBYTE	m_nNumPerPuff;	// Number of particles per Particle puff
		DBOOL	m_bSmall;		// Relative size of Particle
		DBYTE	m_nType;		// Type of particle

		DFLOAT	m_fLastTime;	// Last time we created some particles
		DBOOL	m_bFirstUpdate;	// First update
		DBOOL	m_bIgnoreWind;	// Ignore world wind?
};

#endif // __PARTICLE_TRAIL_SEGMENT_FX_H__
// ----------------------------------------------------------------------- //
//
// MODULE  : RocketFlareFX.h
//
// PURPOSE : RocketFlare special fx class - Definition
//
// CREATED : 11/11/97
//
// ----------------------------------------------------------------------- //

#ifndef __ROCKET_FLARE_FX_H__
#define __ROCKET_FLARE_FX_H__

#include "BaseParticleSystemFX.h"


struct RFCREATESTRUCT : public SFXCREATESTRUCT
{
	RFCREATESTRUCT::RFCREATESTRUCT();

	DVector vVel;
	DBOOL   bSmall;
};

inline RFCREATESTRUCT::RFCREATESTRUCT()
{
	memset(this, 0, sizeof(RFCREATESTRUCT));
}


class CRocketFlareFX : public CBaseParticleSystemFX
{
	public :

		CRocketFlareFX() : CBaseParticleSystemFX() 
		{
			VEC_INIT(m_vVel);
			m_bSmall = DFALSE;
		}

		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update();

	private :

		DBOOL AddFlareParticles();

		DVector	m_vVel;			// Velocity of smoking projectile
		DBOOL	m_bSmall;		// Relative size of smoke
};

#endif // __ROCKET_FLARE_FX_H__
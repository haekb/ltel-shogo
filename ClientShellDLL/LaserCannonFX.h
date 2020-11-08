// ----------------------------------------------------------------------- //
//
// MODULE  : LaserCannonFX.h
//
// PURPOSE : LaserCannon special fx class - Definition
//
// CREATED : 1/20/97
//
// ----------------------------------------------------------------------- //

#ifndef __LASER_CANNON_FX_H__
#define __LASER_CANNON_FX_H__

#include "SpecialFX.h"

struct LCCREATESTRUCT : public SFXCREATESTRUCT
{
	LCCREATESTRUCT::LCCREATESTRUCT();

	DDWORD	dwClientID;
};

inline LCCREATESTRUCT::LCCREATESTRUCT()
{
	memset(this, 0, sizeof(LCCREATESTRUCT));
}


class CLaserCannonFX : public CSpecialFX
{
	public :

		CLaserCannonFX() : CSpecialFX() 
		{
			m_dwClientID	= -1;

			m_bFirstUpdate	= DTRUE;
			m_fLastRotTime	= 0.0f;
			m_fRotPerSec	= 1.0f;
			m_fRotAmount	= 0.0f;
	
			m_fNextLightTime = 0.0f;
			m_fLightWaitTime = 2.0f;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update();
		virtual DBOOL CreateObject(CClientDE* pClientDE);

	protected :

		void CreateLightFX(DVector* pvPos);

		DDWORD		m_dwClientID;

		DBOOL		m_bFirstUpdate;
		DFLOAT		m_fLastRotTime;
		DFLOAT		m_fRotPerSec;
		DFLOAT		m_fRotAmount;

		DFLOAT		m_fNextLightTime;
		DFLOAT		m_fLightWaitTime;
};

#endif // __LASER_CANNON_FX_H__
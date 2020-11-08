// ----------------------------------------------------------------------- //
//
// MODULE  : DynamicLightFX.h
//
// PURPOSE : Dynamic Light special fx class - Definition
//
// CREATED : 2/25/98
//
// ----------------------------------------------------------------------- //

#ifndef __DYNAMIC_LIGHT_FX_H__
#define __DYNAMIC_LIGHT_FX_H__

#include "BaseParticleSystemFX.h"


struct DLCREATESTRUCT : public SFXCREATESTRUCT
{
	DLCREATESTRUCT::DLCREATESTRUCT();

	DVector vColor;
	DVector vPos;
	DFLOAT	fMinRadius;
	DFLOAT	fMaxRadius;
	DFLOAT	fRampUpTime;
	DFLOAT	fMaxTime;
	DFLOAT	fMinTime;
	DFLOAT	fRampDownTime;
	DDWORD  dwFlags;
};

inline DLCREATESTRUCT::DLCREATESTRUCT()
{
	memset(this, 0, sizeof(DLCREATESTRUCT));
}


class CDynamicLightFX : public CSpecialFX
{
	public :

		CDynamicLightFX() : CSpecialFX() 
		{
			VEC_SET(m_vColor, 1.0f, 1.0f, 1.0f);
			VEC_INIT(m_vPos);
			m_fMinRadius	= 100.0f;
			m_fMaxRadius	= 300.0f;
			m_fRampUpTime	= 1.0f;
			m_fMaxTime		= 1.0f;
			m_fMinTime		= 1.0f;
			m_fRampDownTime	= 1.0f;
			m_dwFlags		= 0;
			
			m_fStartTime	= 0.0f;
		}

		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update();

	private :

		DVector m_vColor;	
		DVector m_vPos;
		DFLOAT	m_fMinRadius;
		DFLOAT	m_fMaxRadius;
		DFLOAT	m_fRampUpTime;
		DFLOAT	m_fMaxTime;
		DFLOAT	m_fMinTime;
		DFLOAT	m_fRampDownTime;
		DDWORD  m_dwFlags;

		DFLOAT	m_fStartTime;		// When did we start
};

#endif // __DYNAMIC_LIGHT_FX_H__
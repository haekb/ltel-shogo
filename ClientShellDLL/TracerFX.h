// ----------------------------------------------------------------------- //
//
// MODULE  : TracerFX.h
//
// PURPOSE : Tracer special fx class - Definition
//
// CREATED : 1/21/97
//
// ----------------------------------------------------------------------- //

#ifndef __TRACER_FX_H__
#define __TRACER_FX_H__

#include "BaseLineSystemFX.h"
#include "WeaponDefs.h"

struct TRCREATESTRUCT : public SFXCREATESTRUCT
{
	TRCREATESTRUCT::TRCREATESTRUCT();

	DRotation	rRot;
	DVector		vPos;
	DVector		vVel;
	DVector		vStartColor;
	DVector		vEndColor;
	DVector		vStartPos;
	DFLOAT		fStartAlpha;
	DFLOAT		fEndAlpha;
	DBYTE		nWeaponId;
};

inline TRCREATESTRUCT::TRCREATESTRUCT()
{
	memset(this, 0, sizeof(TRCREATESTRUCT));
	ROT_INIT(rRot);
}


class CTracerFX : public CBaseLineSystemFX
{
	public :

		CTracerFX() : CBaseLineSystemFX() 
		{
			ROT_INIT(m_rRot);
			VEC_INIT(m_vVel);
			VEC_SET(m_vStartColor, 1.0f, 1.0f, 1.0f);
			VEC_SET(m_vEndColor, 1.0f, 1.0f, 1.0f);
			VEC_INIT(m_vStartPos);
			m_fStartAlpha	= 1.0f;
			m_fEndAlpha		= 1.0f;

			m_bFirstUpdate	= DTRUE;
			m_fStartTime	= 0.0f;
			m_fDuration		= 0.0f;
			m_nWeaponId		= GUN_NONE;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update();

	protected :

		DRotation	m_rRot;
		DVector		m_vVel;
		DVector		m_vStartColor;
		DVector		m_vEndColor;
		DVector		m_vStartPos;
		DFLOAT		m_fStartAlpha;
		DFLOAT		m_fEndAlpha;

		DBOOL		m_bFirstUpdate;
		DFLOAT		m_fStartTime;
		DFLOAT		m_fDuration;
		DBYTE		m_nWeaponId;
};

#endif // __TRACER_FX_H__
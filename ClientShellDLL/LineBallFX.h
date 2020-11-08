// ----------------------------------------------------------------------- //
//
// MODULE  : LineBallFX.h
//
// PURPOSE : LineBall special fx class - Definition
//
// CREATED : 9/06/97
//
// ----------------------------------------------------------------------- //

#ifndef __LINE_BALL_FX_H__
#define __LINE_BALL_FX_H__

#include "BaseLineSystemFX.h"

struct LBCREATESTRUCT : public SFXCREATESTRUCT
{
	LBCREATESTRUCT::LBCREATESTRUCT();

	DRotation	rRot;
	DVector		vPos;
	DVector		vStartColor;
	DVector		vEndColor;
	DVector		vInitialScale;
	DVector		vFinalScale;
	DFLOAT		fSystemStartAlpha;
	DFLOAT		fSystemEndAlpha;
	DFLOAT		fStartAlpha;
	DFLOAT		fEndAlpha;
	DFLOAT		fOffset;
	DFLOAT		fLifeTime;
	DFLOAT		fLineLength;
};

inline LBCREATESTRUCT::LBCREATESTRUCT()
{
	memset(this, 0, sizeof(LBCREATESTRUCT));
	ROT_INIT(rRot);
}


class CLineBallFX : public CBaseLineSystemFX
{
	public :

		CLineBallFX() : CBaseLineSystemFX() 
		{
			ROT_INIT(m_rRot);
			VEC_SET(m_vStartColor, 1.0f, 1.0f, 1.0f);
			VEC_SET(m_vEndColor, 1.0f, 1.0f, 1.0f);
			VEC_SET(m_vInitialScale, 1.0f, 1.0f, 1.0f);
			VEC_SET(m_vFinalScale, 1.0f, 1.0f, 1.0f);

			m_fSystemStartAlpha	= 1.0f;
			m_fSystemEndAlpha	= 1.0f;
			m_fStartAlpha		= 1.0f;
			m_fEndAlpha			= 1.0f;
			m_fStartTime		= 0.0f;
			m_fLifeTime			= 0.0f;
			m_fOffset			= 10.0f;
			m_fLineLength		= 200.0f;
			m_bFirstUpdate		= DTRUE;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update();

	protected :

		DRotation	m_rRot;
		DVector		m_vStartColor;
		DVector		m_vEndColor;
		DVector		m_vInitialScale;
		DVector		m_vFinalScale;
		DFLOAT		m_fStartAlpha;
		DFLOAT		m_fEndAlpha;

		DBOOL		m_bFirstUpdate;
		DFLOAT		m_fStartTime;
		DFLOAT		m_fLifeTime;
		DFLOAT		m_fOffset;		// In degrees
		DFLOAT		m_fLineLength;
		DFLOAT		m_fSystemStartAlpha;
		DFLOAT		m_fSystemEndAlpha;

		void		CreateLines();
		void		UpdateAlpha(DFLOAT fTimeDelta);
		void		UpdateScale(DFLOAT fTimeDelta);
		void		UpdateRotation(DFLOAT fTimeDelta);
};

#endif // __LINE_BALL_FX_H__
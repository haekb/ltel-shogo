 // ----------------------------------------------------------------------- //
//
// MODULE  : BaseScaleFX.h
//
// PURPOSE : BaseScale special fx class - Definition
//
// CREATED : 5/27/98
//
// ----------------------------------------------------------------------- //

#ifndef __BASE_SCALE_FX_H__
#define __BASE_SCALE_FX_H__

#include "SpecialFX.h"
#include "client_physics.h"

struct BSCREATESTRUCT : public SFXCREATESTRUCT
{
	BSCREATESTRUCT::BSCREATESTRUCT();

	DRotation	rRot;
	DVector		vPos;
	DVector		vVel;
	DVector		vInitialScale;
	DVector		vFinalScale;
	DVector		vInitialColor;
	DVector		vFinalColor;
	DFLOAT		fLifeTime;
	DFLOAT		fDelayTime;
	DFLOAT		fInitialAlpha;
	DFLOAT		fFinalAlpha;
	char*		pFilename;
	char*		pSkin;
	DDWORD		dwFlags;
	DBOOL		bUseUserColors;
	DBOOL		bLoop;
};

inline BSCREATESTRUCT::BSCREATESTRUCT()
{
	memset(this, 0, sizeof(BSCREATESTRUCT));
	ROT_INIT(rRot);
	VEC_SET(vInitialColor, -1.0f, -1.0f, -1.0f);
	VEC_SET(vFinalColor, -1.0f, -1.0f, -1.0f);
}


class CBaseScaleFX : public CSpecialFX
{
	public :

		CBaseScaleFX()
		{
			ROT_INIT(m_rRot);
			VEC_INIT(m_vPos);
			VEC_INIT(m_vVel);
			VEC_SET(m_vInitialScale, 1.0f, 1.0f, 1.0f);
			VEC_SET(m_vFinalScale, 1.0f, 1.0f, 1.0f);
			VEC_SET(m_vInitialColor, 1.0f, 1.0f, 1.0f);
			VEC_SET(m_vFinalColor, 1.0f, 1.0f, 1.0f);

			m_fLifeTime		= 1.0f;
			m_fInitialAlpha	= 1.0f;
			m_fFinalAlpha	= 1.0f;
			m_pFilename		= DNULL;
			m_pSkin			= DNULL;

			m_fStartTime	= 0.0f;
			m_fDelayTime	= 0.0f;
			m_fEndTime		= 0.0f;
			
			m_dwFlags		= 0;
			m_nType			= 0;
			m_nRotDir		= 1;
			m_bLoop			= DFALSE;

			m_bUseUserColors = DFALSE;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update();
		virtual DBOOL CreateObject(CClientDE* pClientDE);

	protected :

		DRotation	m_rRot;
		DVector		m_vPos;
		DVector		m_vVel;
		DVector		m_vInitialScale;
		DVector		m_vFinalScale;
		DVector		m_vInitialColor;
		DVector		m_vFinalColor;
		DFLOAT		m_fLifeTime;
		DFLOAT		m_fDelayTime;
		DFLOAT		m_fInitialAlpha;
		DFLOAT		m_fFinalAlpha;
		char*		m_pFilename;
		char*		m_pSkin;
		DDWORD		m_dwFlags;
		DBOOL		m_bUseUserColors;
		DBOOL		m_bLoop;
		
		int				m_nRotDir;
		unsigned short	m_nType;
		DFLOAT			m_fStartTime;
		DFLOAT			m_fEndTime;
		MovingObject	m_movingObj;

		virtual void UpdateAlpha(DFLOAT fTimeDelta);
		virtual void UpdateScale(DFLOAT fTimeDelta);
		virtual void UpdatePos(DFLOAT fTimeDelta);
		virtual void UpdateRot(DFLOAT fTimeDelta);
};

#endif // __BASE_SCALE_FX_H__
// ----------------------------------------------------------------------- //
//
// MODULE  : CameraFX.h
//
// PURPOSE : Camera special fx class - Definition
//
// CREATED : 5/20/98
//
// ----------------------------------------------------------------------- //

#ifndef __CAMERA_FX_H__
#define __CAMERA_FX_H__

#include "SpecialFX.h"

struct CAMCREATESTRUCT : public SFXCREATESTRUCT
{
	CAMCREATESTRUCT::CAMCREATESTRUCT();

	DBOOL bAllowPlayerMovement;
	DBYTE nCameraType;
	DBOOL bIsListener;
};

inline CAMCREATESTRUCT::CAMCREATESTRUCT()
{
	memset(this, 0, sizeof(CAMCREATESTRUCT));
}

class CCameraFX : public CSpecialFX
{
	public :

		CCameraFX() : CSpecialFX() 
		{
			m_bAllowPlayerMovement  = DFALSE;
			m_nCameraType			= 0; 
			m_bIsListener			= DFALSE;
		}

		virtual DBOOL Update() { return !m_bWantRemove; }

		DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct)
		{
			if (!CSpecialFX::Init(psfxCreateStruct)) return DFALSE;

			CAMCREATESTRUCT* pCAM = (CAMCREATESTRUCT*)psfxCreateStruct;

			m_bAllowPlayerMovement	= pCAM->bAllowPlayerMovement;
			m_nCameraType			= pCAM->nCameraType;
			m_bIsListener			= pCAM->bIsListener;

			return DTRUE;
		}

		DBOOL	AllowPlayerMovement()	const { return m_bAllowPlayerMovement; }
		DBYTE	GetType()				const { return m_nCameraType; }
		DBOOL	IsListener()			const { return m_bIsListener; }

	protected :

		DBOOL	m_bAllowPlayerMovement;
		DBYTE	m_nCameraType;
		DBOOL	m_bIsListener;
};

#endif // __CAMERA_FX_H__
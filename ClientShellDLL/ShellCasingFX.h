//----------------------------------------------------------
//
// MODULE  : SHELLCASINGFX.H
//
// PURPOSE : defines classes for ejected shells
//
// CREATED : 5/1/98
//
//----------------------------------------------------------

#ifndef __SHELLCASING_FX_H__
#define __SHELLCASING_FX_H__

#include "SpecialFX.h"
#include "dlink.h"
#include "client_physics.h"


struct SHELLCREATESTRUCT : public SFXCREATESTRUCT
{
	SHELLCREATESTRUCT::SHELLCREATESTRUCT();

	DRotation	rRot;
	DVector		vStartPos;
	DBYTE		nWeaponId;
};

inline SHELLCREATESTRUCT::SHELLCREATESTRUCT()
{
	memset(this, 0, sizeof(SHELLCREATESTRUCT));
}

class CShellCasingFX : public CSpecialFX
{
	public :

		CShellCasingFX();

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update();
		virtual DBOOL CreateObject(CClientDE* pClientDE);

	private:

		DFLOAT		m_fExpireTime;
		DVector		m_vLastPos;
		
		DFLOAT		m_fPitchVel;
		DFLOAT		m_fYawVel;
		DFLOAT		m_fPitch;
		DFLOAT		m_fYaw;

        DBOOL       m_bInVisible;
        int         m_nVisibleUpdate;
		int			m_nBounceCount;        

		DRotation	m_rRot;
		DVector		m_vStartPos;
		DBYTE		m_nWeaponId;
		DBOOL		m_bResting;

		DFLOAT		m_fDieTime;
		DVector		m_vScale;

		MovingObject m_movingObj;

		char* GetModelName();
		char* GetSkinName();
};


#endif  // __SHELLCASING_FX_H__
// ----------------------------------------------------------------------- //
//
// MODULE  : WeaponSoundFX.h
//
// PURPOSE : Weapon Sound fx - Definition
//
// CREATED : 10/28/98
//
// ----------------------------------------------------------------------- //

#ifndef __WEAPON_SOUND_FX_H__
#define __WEAPON_SOUND_FX_H__

#include "SpecialFX.h"

struct WSOUNDCREATESTRUCT : public SFXCREATESTRUCT
{
	WSOUNDCREATESTRUCT::WSOUNDCREATESTRUCT();

	DVector		vPos;
	DBYTE		nClientId;
	DBYTE		nType;
	DBYTE		nWeaponId;
	HSTRING		hSound;
};

inline WSOUNDCREATESTRUCT::WSOUNDCREATESTRUCT()
{
	memset(this, 0, sizeof(WSOUNDCREATESTRUCT));
}

class CWeaponSoundFX : public CSpecialFX
{
	public :

		CWeaponSoundFX() : CSpecialFX() 
		{
			VEC_INIT(m_vPos);
			m_nClientId	= 0;
			m_nType		= 0;
			m_nWeaponId	= 0;
			m_hSound	= 0;
		}

		~CWeaponSoundFX()
		{
			if (m_pClientDE && m_hSound)
			{
				m_pClientDE->FreeString(m_hSound);
			}
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update() { return DFALSE; }

	private :

		DVector	m_vPos;
		DBYTE	m_nClientId;
		DBYTE	m_nType;
		DBYTE	m_nWeaponId;
		HSTRING m_hSound;
};

#endif // __WEAPON_SOUND_FX_H__
 // ----------------------------------------------------------------------- //
//
// MODULE  : MarkSFX.h
//
// PURPOSE : Mark special fx class - Definition
//
// CREATED : 11/6/97
//
// ----------------------------------------------------------------------- //

#ifndef __MARKSFX_H__
#define __MARKSFX_H__

#include "SpecialFX.h"
#include "dlink.h"

struct MARKCREATESTRUCT : public SFXCREATESTRUCT
{
	MARKCREATESTRUCT::MARKCREATESTRUCT();

	DRotation	m_Rotation;
	DVector		m_vPos;
	DFLOAT		m_fScale;
	HSTRING		m_hstrSprite;
};

inline MARKCREATESTRUCT::MARKCREATESTRUCT()
{
	memset(this, 0, sizeof(MARKCREATESTRUCT));
}


class CMarkSFX : public CSpecialFX
{
	public :

		CMarkSFX()
		{
			ROT_INIT( m_Rotation );
			VEC_INIT( m_vPos );
			m_fScale = 1.0f;
			m_hstrSprite = DNULL;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update() { return DTRUE; }
		virtual DBOOL CreateObject(CClientDE* pClientDE);

	private :

		DRotation	m_Rotation;
		DVector		m_vPos;
		DFLOAT		m_fScale;
		HSTRING		m_hstrSprite;
};

#endif // __MARKSFX_H__
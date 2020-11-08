// ----------------------------------------------------------------------- //
//
// MODULE  : CriticalHitFX.h
//
// PURPOSE : Critical hit - Definition
//
// CREATED : 7/28/98
//
// ----------------------------------------------------------------------- //

#ifndef __CRITICAL_HIT_FX_H__
#define __CRITICAL_HIT_FX_H__

#include "SpecialFX.h"

struct CHCREATESTRUCT : public SFXCREATESTRUCT
{
	CHCREATESTRUCT::CHCREATESTRUCT();

	DFLOAT	fClientIDHitter;
	DFLOAT	fClientIDHittee;
	DVector	vPos;
};

inline CHCREATESTRUCT::CHCREATESTRUCT()
{
	VEC_INIT(vPos);
	fClientIDHitter = fClientIDHittee = -1.0f;
}

class CCriticalHitFX : public CSpecialFX
{
	public :

		CCriticalHitFX() : CSpecialFX() 
		{
			VEC_INIT(m_vPos);
			m_nClientIDHitter = m_nClientIDHittee = -1;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update() { return DFALSE; }

	private :

		DVector	m_vPos;
		int		m_nClientIDHitter;
		int		m_nClientIDHittee;
};

#endif // __CRITICAL_HIT_FX_H__
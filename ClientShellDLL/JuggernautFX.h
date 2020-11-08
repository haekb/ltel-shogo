// ----------------------------------------------------------------------- //
//
// MODULE  : JuggernautFX.h
//
// PURPOSE : Juggernaut special fx class - Definition
//
// CREATED : 4/29/98
//
// ----------------------------------------------------------------------- //

#ifndef __JUGGERNAUT_FX_H__
#define __JUGGERNAUT_FX_H__

#include "SpecialFX.h"

struct JNCREATESTRUCT : public SFXCREATESTRUCT
{
	JNCREATESTRUCT::JNCREATESTRUCT();

	DVector		vFirePos;
	DVector		vEndPos;
	DFLOAT		fFadeTime;
};

inline JNCREATESTRUCT::JNCREATESTRUCT()
{
	memset(this, 0, sizeof(JNCREATESTRUCT));
}


class CJuggernautFX : public CSpecialFX
{
	public :

		CJuggernautFX() : CSpecialFX() 
		{
			VEC_INIT(m_vFirePos);
			VEC_INIT(m_vEndPos);
			m_fFadeTime = 1.0f;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL Update();
		virtual DBOOL CreateObject(CClientDE* pClientDE);

	protected :

		DVector		m_vFirePos;
		DVector		m_vEndPos;
		DFLOAT		m_fFadeTime;
		DFLOAT		m_fStartTime;
};

#endif // __JUGGERNAUT_FX_H__
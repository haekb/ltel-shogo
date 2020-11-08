// ----------------------------------------------------------------------- //
//
// MODULE  : DeathFX.h
//
// PURPOSE : Death special fx class - Definition
//
// CREATED : 6/14/98
//
// ----------------------------------------------------------------------- //

#ifndef __DEATH_FX_H__
#define __DEATH_FX_H__

#include "SpecialFX.h"
#include "ContainerCodes.h"
#include "CharacterAlignment.h"
#include "ModelFuncs.h"
#include "GibFX.h"

struct DEATHCREATESTRUCT : public SFXCREATESTRUCT
{
	DEATHCREATESTRUCT::DEATHCREATESTRUCT();

	DBYTE	nModelId;
	DBYTE	nDeathType;
	DBYTE	nSize;
	DBYTE	nCharacterClass;
	DVector vPos;
	DVector vDir;
};

inline DEATHCREATESTRUCT::DEATHCREATESTRUCT()
{
	memset(this, 0, sizeof(DEATHCREATESTRUCT));
}


class CDeathFX : public CSpecialFX
{
	public :

		CDeathFX() : CSpecialFX() 
		{
			m_eCode				= CC_NONE;
			m_nDeathType		= 0;
			m_nModelId			= MI_UNDEFINED;
			m_nSize				= MS_NORMAL;
			m_nCharacterClass	= UNKNOWN;
			VEC_INIT(m_vPos);
			VEC_INIT(m_vDir);
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update() { return DFALSE; }

	protected :
	
		ContainerCode	m_eCode;			// Container effect is in
		DVector			m_vPos;				// Effect position
		DVector			m_vDir;				// Direction damage came from
		DBYTE			m_nDeathType;		// Type of death
		DBYTE			m_nModelId;			// Type of model
		DBYTE			m_nSize;			// Size of model
		DBYTE			m_nCharacterClass;	// Character class

		void CreateDeathFX();
		void CreateVehicleDeathFX();
		void CreateMechaDeathFX();
		void CreateHumanDeathFX();

		void SetupGibTypes(GIBCREATESTRUCT & gib);
};

#endif // __DEATH_FX_H__
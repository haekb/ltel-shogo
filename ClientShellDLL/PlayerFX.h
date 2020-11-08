// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerFX.h
//
// PURPOSE : Player special fx class - Definition
//
// CREATED : 8/24/98
//
// ----------------------------------------------------------------------- //

#ifndef __PLAYER_FX_H__
#define __PLAYER_FX_H__

#include "SpecialFX.h"

struct PLAYERCREATESTRUCT : public SFXCREATESTRUCT
{
	PLAYERCREATESTRUCT::PLAYERCREATESTRUCT();
};

inline PLAYERCREATESTRUCT::PLAYERCREATESTRUCT()
{
	memset(this, 0, sizeof(PLAYERCREATESTRUCT));
}


class CPlayerFX : public CSpecialFX
{
	public :

		CPlayerFX() : CSpecialFX() 
		{
			m_hTears			= DNULL;
			m_pBubbles			= DNULL;
			m_hVehicleSprite	= DNULL;

			m_fNextBubbleTime	= -1.0f;
		}

		~CPlayerFX()
		{
			RemoveUnderwaterFX();
			RemoveVehicleFX();
			RemoveTearsFX();
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update();

	protected :
	
		void CreateUnderwaterFX(DVector & vPos);
		void CreateTearsFX(DVector & vPos);
		void CreateVehicleFX(DVector & vPos);

		void UpdateUnderwaterFX(DVector & vPos);
		void UpdateTearsFX(DVector & vPos);
		void UpdateVehicleFX(DVector & vPos);

		void RemoveUnderwaterFX();
		void RemoveVehicleFX();
		void RemoveTearsFX();

		HLOCALOBJ		m_hTears;			// Tears fx
		CSpecialFX*		m_pBubbles;			// Bubbles fx
		HLOCALOBJ		m_hVehicleSprite;	// Vehicle sprite

		DFLOAT			m_fNextBubbleTime;
};

#endif // __PLAYER_FX_H__
// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerInventory.h
//
// PURPOSE : Definition of PlayerInventory class
//
// CREATED : 3/3/98
//
// ----------------------------------------------------------------------- //

#ifndef __PLAYERINVENTORY_H
#define __PLAYERINVENTORY_H

#include "WeaponDefs.h"

class CClientDE;
class CRiotClientShell;

class CPlayerInventory
{
public:

	CPlayerInventory();
	~CPlayerInventory();

	DBOOL		Init (CClientDE* pClientDE, CRiotClientShell* pClientShell);
	void		Term();

	void		Reset();
	void		Update();

	void		GunPickup (DBYTE nType, DBOOL bDisplayMessage=DTRUE);
	void		UpdateAmmo (DDWORD nType, DDWORD nAmount);

	void		ShogoPowerupPickup (PickupItemType ePowerup);
	void		ShogoPowerupClear();

	void		Draw (DBOOL bDrawOrdinance);

	void		Save(HMESSAGEWRITE hWrite);
	void		Load(HMESSAGEREAD hRead);

	DBOOL		CanDrawGun(DBYTE nWeaponId);

protected:

	DBOOL		InitSurfaces();

protected:
	
	CClientDE*			m_pClientDE;
	CRiotClientShell*	m_pClientShell;

	DBOOL				m_bHaveGun[GUN_MAX_NUMBER];
	DBOOL				m_bHaveShogoLetter[5];
	HSURFACE			m_hShogoLetter[5];
	
	DBOOL				m_bAmmoChanged;
	DDWORD				m_nNewAmmoType;
	DDWORD				m_nNewAmmoAmount;

	HDEFONT				m_hAmmoCountFont;

	HSURFACE			m_hCurrentMessage;
	DDWORD				m_cxCurrentMessage;
	DDWORD				m_cyCurrentMessage;

	DFLOAT				m_fDisplayTimeLeft;

	HSURFACE			m_hOrdinance;

	HSURFACE			m_hGunIcon[GUN_MAX_NUMBER];
	DDWORD				m_cxGunIcon[GUN_MAX_NUMBER];
	DDWORD				m_cyGunIcon[GUN_MAX_NUMBER];

	HSURFACE			m_hGunName[GUN_MAX_NUMBER];
	HSURFACE			m_hAmmoCount[GUN_MAX_NUMBER];

};

#endif
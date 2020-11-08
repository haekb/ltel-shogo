// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerStats.h
//
// PURPOSE : Definition of PlayerStats class
//
// CREATED : 10/9/97
//
// ----------------------------------------------------------------------- //

#ifndef __PLAYERSTATS_H
#define __PLAYERSTATS_H

#include "WeaponDefs.h"
#include "basedefs_de.h"

#define FLASH_HEALTH	0x01
#define FLASH_ARMOR		0x02
#define FLASH_AMMO		0x04

class CRiotClientShell;

class CPlayerStats
{
public:

	CPlayerStats();
	~CPlayerStats();

	DBOOL		Init (CClientDE* pClientDE, CRiotClientShell* pClientShell);
	void		Term();

	void		OnEnterWorld(DBOOL bRestoringGame=DFALSE);
	void		OnExitWorld();

	void		Draw (DBOOL bStatsSizedOff, DBOOL bDrawHud);

	void		Clear();
	void		Update();
	void		UpdatePlayerMode (DBYTE nNewMode, DBOOL bForce=DFALSE);
	void		UpdatePlayerWeapon (DBYTE nWeapon, DBOOL bForce=DFALSE);
	void		UpdateHealth (DDWORD nHealth);
	void		UpdateArmor (DDWORD nArmor);
	void		UpdateAmmo (DDWORD nType, DDWORD nAmmo);
	void		UpdateAir (DFLOAT nPercent);

	void		Save(HMESSAGEWRITE hWrite);
	void		Load(HMESSAGEREAD hRead);

	void		ToggleCrosshair();
	void		EnableCrosshair(DBOOL b=DTRUE) { m_bCrosshairEnabled = b; }
	DBOOL		CrosshairEnabled() const { return m_bCrosshairEnabled; }
	DBOOL		CrosshairOn() const { return (DBOOL)(m_nCrosshairLevel != 0); }

	void		SetDrawAmmo(DBOOL bVal=DTRUE) { m_bDrawAmmo = bVal; }
	
	DBYTE		GetCurWeapon() const { return m_nCurrentWeapon; }

	DDWORD		GetAmmoCount (DBYTE nWeapon) const { if (nWeapon >= GUN_MAX_NUMBER) return 0; return m_nAmmo[nWeapon]; }

protected:

	void		InitOnFoot();
	void		InitEnforcer();
	void		InitAkuma();
	void		InitOrdog();
	void		InitPredator();

	void		DrawOnFoot (HSURFACE hScreen, int nLeft, int nTop, int nRight, int nBottom, DBOOL bStatsSizedOff);
	void		DrawEnforcer (HSURFACE hScreen, int nLeft, int nTop, int nRight, int nBottom, DBOOL bStatsSizedOff);
	void		DrawAkuma (HSURFACE hScreen, int nLeft, int nTop, int nRight, int nBottom, DBOOL bStatsSizedOff);
	void		DrawOrdog (HSURFACE hScreen, int nLeft, int nTop, int nRight, int nBottom, DBOOL bStatsSizedOff);
	void		DrawPredator (HSURFACE hScreen, int nLeft, int nTop, int nRight, int nBottom, DBOOL bStatsSizedOff);

	void		UpdateOnFootHealth();
	void		UpdatePredatorHealth();	
	void		UpdateEnforcerHealth();
	void		UpdateOrdogHealth();
	void		UpdateAkumaHealth();
	
	void		UpdateOnFootArmor();
	void		UpdatePredatorArmor();
	void		UpdateEnforcerArmor();
	void		UpdateOrdogArmor();
	void		UpdateAkumaArmor();
	
	void		UpdateOnFootAmmo();
	void		UpdatePredatorAmmo();
	void		UpdateEnforcerAmmo();
	void		UpdateOrdogAmmo();
	void		UpdateAkumaAmmo();

	void		UpdateHUDEffect();

		
protected:

	CClientDE*	m_pClientDE;
	CRiotClientShell* m_pClientShell;

	DDWORD		m_nHealth;						// current health
	DDWORD		m_nArmor;						// current armor
	DDWORD		m_nAmmo[GUN_MAX_NUMBER];		// current ammo
	DBYTE		m_nCurrentWeapon;				// current weapon

	DBOOL		m_bHealthChanged;
	DBOOL		m_bArmorChanged;
	DBOOL		m_bAmmoChanged;

	HSURFACE	m_hAirMeter;
	DRect		m_rcAirBar;
	DDWORD		m_cxAirMeter;
	DDWORD		m_cyAirMeter;
	DFLOAT		m_fAirPercent;

	HDEFONT		m_hNumberFont;					// font for number display

	int			m_nCrosshairLevel;
	DBOOL		m_bCrosshairEnabled;
	DBOOL		m_bDrawHud;

	DBYTE		m_nPlayerMode;

	HSURFACE	m_hCrosshair1;
	HSURFACE	m_hCrosshair2;
	HSURFACE	m_hCrosshair3;
	HSURFACE	m_hZoomCrosshair;

	HSURFACE	m_hCleanHUDLeft;
	HSURFACE	m_hCleanHUDMiddle;
	HSURFACE	m_hCleanHUDRight;
	HSURFACE	m_hHUDDataLeft;
	HSURFACE	m_hHUDDataMiddle;
	HSURFACE	m_hHUDDataRight;
	HSURFACE	m_hHUDOverlayLeft;
	HSURFACE	m_hHUDOverlayMiddle;
	HSURFACE	m_hHUDOverlayRight;
	HSURFACE	m_hHUDLeft;
	HSURFACE	m_hHUDMiddle;
	HSURFACE	m_hHUDRight;
	HSURFACE	m_hHUDEffect;
	HSURFACE	m_hHUDNumbers;
	int			m_nNumWidths[10];
	int			m_nNumOffsets[10];

	DFLOAT		m_nEffectPos;

	HSURFACE	m_hAmmoIcon;
	DDWORD		m_cxAmmoIcon;
	DDWORD		m_cyAmmoIcon;

	DBOOL		m_bDrawAmmo;
};

#endif

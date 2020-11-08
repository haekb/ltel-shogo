// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerStats.cpp
//
// PURPOSE : Implementation of PlayerStats class
//
// CREATED : 10/9/97
//
// ----------------------------------------------------------------------- //

#include <stdio.h>
#include "RiotClientShell.h"
#include "client_de.h"
#include "ClientRes.h"
#include "PlayerStats.h"
#include "TextHelper.h"
#include "PlayerModeTypes.h"

#define ONFOOT_NUMBER_COLOR				m_pClientDE->SetupColor1 (0.98f, 0.317647f, 0.0f, DFALSE)
#define PREDATOR_NUMBER_COLOR			m_pClientDE->SetupColor1 (0.98f, 0.317647f, 0.0f, DFALSE)
#define ENFORCER_NUMBER_COLOR			m_pClientDE->SetupColor1 (0.98f, 0.317647f, 0.0f, DFALSE)
#define ORDOG_NUMBER_COLOR				m_pClientDE->SetupColor1 (0.98f, 0.317647f, 0.0f, DFALSE)
#define AKUMA_NUMBER_COLOR				m_pClientDE->SetupColor1 (0.98f, 0.317647f, 0.0f, DFALSE)

CPlayerStats::CPlayerStats()
{
	m_pClientDE = DNULL;
	m_pClientShell = DNULL;
	m_nHealth = 0;
	m_nArmor = 0;
	m_nCurrentWeapon = 0;
	memset (&m_nAmmo, 0, sizeof (DDWORD) * GUN_MAX_NUMBER);

	m_bHealthChanged = DFALSE;
	m_bArmorChanged = DFALSE;
	m_bAmmoChanged = DFALSE;

	m_hAirMeter = DNULL;
	m_cxAirMeter = 0;
	m_cyAirMeter = 0;
	m_fAirPercent = 100.0f;
	
	m_hNumberFont = DNULL;
	
	m_nCrosshairLevel = 2;
	m_bCrosshairEnabled = DTRUE;

	m_nPlayerMode = PM_MODE_UNDEFINED;

	m_hCrosshair1 = DNULL;
	m_hCrosshair2 = DNULL;
	m_hCrosshair3 = DNULL;
	m_hZoomCrosshair = DNULL;
	m_hCleanHUDLeft = DNULL;
	m_hCleanHUDMiddle = DNULL;
	m_hCleanHUDRight = DNULL;
	m_hHUDDataLeft = DNULL;
	m_hHUDDataMiddle = DNULL;
	m_hHUDDataRight = DNULL;
	m_hHUDOverlayLeft = DNULL;
	m_hHUDOverlayMiddle = DNULL;
	m_hHUDOverlayRight = DNULL;
	m_hHUDLeft = DNULL;
	m_hHUDMiddle = DNULL;
	m_hHUDRight = DNULL;
	m_hHUDEffect = DNULL;
	m_hHUDNumbers = DNULL;
	memset (m_nNumWidths, 0, sizeof (int) * 10);
	memset (m_nNumOffsets, 0, sizeof (int) * 10);

	m_nEffectPos = 7.0f;

	m_hAmmoIcon = DNULL;
	m_cxAmmoIcon = 0;
	m_cyAmmoIcon = 0;

	m_bDrawAmmo = DTRUE;
}

CPlayerStats::~CPlayerStats()
{
	Term();
}

DBOOL CPlayerStats::Init (CClientDE* pClientDE, CRiotClientShell* pClientShell)
{
	if (!pClientDE) return DFALSE;

	m_pClientDE = pClientDE;
	m_pClientShell = pClientShell;

	// init the font

	HSTRING hstrFont = m_pClientDE->FormatString (IDS_INGAMEFONT);
	m_hNumberFont = m_pClientDE->CreateFont (m_pClientDE->GetStringData(hstrFont), 10, 32, DFALSE, DFALSE, DFALSE);
	m_pClientDE->FreeString (hstrFont);

	// init the air meter

	m_hAirMeter = m_pClientDE->CreateSurfaceFromBitmap ("interface/AirMeter.pcx");
	m_pClientDE->GetSurfaceDims (m_hAirMeter, &m_cxAirMeter, &m_cyAirMeter);
	m_rcAirBar.left = 3;
	m_rcAirBar.top = 3;
	m_rcAirBar.right = 13;
	m_rcAirBar.bottom = 61;
	
	return DTRUE;
}

void CPlayerStats::Term()
{
	if (!m_pClientDE) return;

	if (m_hNumberFont) m_pClientDE->DeleteFont (m_hNumberFont);

	if (m_hCrosshair1)		m_pClientDE->DeleteSurface (m_hCrosshair1);
	if (m_hCrosshair2)		m_pClientDE->DeleteSurface (m_hCrosshair2);
	if (m_hCrosshair3)		m_pClientDE->DeleteSurface (m_hCrosshair3);
	if (m_hZoomCrosshair)	m_pClientDE->DeleteSurface (m_hZoomCrosshair);
	if (m_hCleanHUDLeft)	m_pClientDE->DeleteSurface (m_hCleanHUDLeft);
	if (m_hCleanHUDMiddle)	m_pClientDE->DeleteSurface (m_hCleanHUDMiddle);
	if (m_hCleanHUDRight)	m_pClientDE->DeleteSurface (m_hCleanHUDRight);
	if (m_hHUDOverlayLeft)	m_pClientDE->DeleteSurface (m_hHUDOverlayLeft);
	if (m_hHUDOverlayMiddle)m_pClientDE->DeleteSurface (m_hHUDOverlayMiddle);
	if (m_hHUDOverlayRight)	m_pClientDE->DeleteSurface (m_hHUDOverlayRight);
	if (m_hHUDLeft)			m_pClientDE->DeleteSurface (m_hHUDLeft);
	if (m_hHUDMiddle)		m_pClientDE->DeleteSurface (m_hHUDMiddle);
	if (m_hHUDRight)		m_pClientDE->DeleteSurface (m_hHUDRight);
	if (m_hHUDEffect)		m_pClientDE->DeleteSurface (m_hHUDEffect);
	if (m_hHUDNumbers)		m_pClientDE->DeleteSurface (m_hHUDNumbers);
	if (m_hAmmoIcon)		m_pClientDE->DeleteSurface (m_hAmmoIcon);
	if (m_hAirMeter)		m_pClientDE->DeleteSurface (m_hAirMeter);
	
	m_hCrosshair1		= DNULL;
	m_hCrosshair2		= DNULL;
	m_hCrosshair3		= DNULL;
	m_hCleanHUDLeft		= DNULL;
	m_hCleanHUDMiddle	= DNULL;
	m_hCleanHUDRight	= DNULL;
	m_hHUDOverlayLeft	= DNULL;
	m_hHUDOverlayMiddle = DNULL;
	m_hHUDOverlayRight	= DNULL;
	m_hHUDLeft			= DNULL;
	m_hHUDMiddle		= DNULL;
	m_hHUDRight			= DNULL;
	m_hHUDEffect		= DNULL;
	m_hHUDNumbers		= DNULL;
	m_hAmmoIcon			= DNULL;
	m_hAirMeter			= DNULL;

	m_pClientDE		= NULL;
	m_pClientShell	= NULL;
	m_nHealth		= 0;
	m_nArmor		= 0;
	m_hNumberFont	= NULL;
	
	m_cxAmmoIcon = 0;
	m_cyAmmoIcon = 0;
	
	m_nPlayerMode	= 0;
}

void CPlayerStats::OnEnterWorld(DBOOL bRestoringGame)
{
	if (!m_pClientDE || !m_pClientShell) return;

	// find out what mode we are in and make sure that mode is set

	UpdatePlayerMode(m_pClientShell->GetPlayerMode(), bRestoringGame);

	if (!bRestoringGame)
	{
		// clear the values
	
		Clear();
	}
}

void CPlayerStats::OnExitWorld()
{
	if (!m_pClientDE) return;
}

void CPlayerStats::Draw (DBOOL bStatsSizedOff, DBOOL bDrawHud)
{
	if (!m_pClientDE || !m_pClientShell || !m_pClientShell->GetCamera()) return;

	float m_nCurrentTime = m_pClientDE->GetTime();

	m_bDrawHud = bDrawHud;

	// get the screen size

	DDWORD nWidth = 0;
	DDWORD nHeight = 0;
	HSURFACE hScreen = m_pClientDE->GetScreenSurface();
	m_pClientDE->GetSurfaceDims (hScreen, &nWidth, &nHeight);

	// draw air meter if we're on foot and we have less than 100% air

	if (m_fAirPercent < 1.0f && m_nPlayerMode == PM_MODE_FOOT)
	{
		int x = nWidth - m_cxAirMeter - 10;
		int y = nHeight - 150;
		
		m_pClientDE->DrawSurfaceToSurface (hScreen, m_hAirMeter, DNULL, x, y);
	}

	// draw custom HUD for each mode

	switch (m_nPlayerMode)
	{
		case PM_MODE_FOOT:		DrawOnFoot (hScreen, 0, 0, nWidth, nHeight, bStatsSizedOff);	break;

		case PM_MODE_MCA_SA:
		case PM_MODE_MCA_AO:
		case PM_MODE_MCA_AP:
		case PM_MODE_MCA_UE:	DrawEnforcer (hScreen, 0, 0, nWidth, nHeight, bStatsSizedOff);	break;
	}
}
	
void CPlayerStats::Clear()
{
	UpdateHealth (0);
	UpdateArmor (0);
	for (DBYTE i = 0; i < GUN_MAX_NUMBER; i++)
	{
		UpdateAmmo (i, 0);
	}
}

void CPlayerStats::Update()
{
	// did the player's health change?

	if (m_bHealthChanged)
	{
		switch (m_nPlayerMode)
		{
			case PM_MODE_FOOT:		UpdateOnFootHealth();	break;

			case PM_MODE_MCA_AP:
			case PM_MODE_MCA_AO:
			case PM_MODE_MCA_SA:
			case PM_MODE_MCA_UE:	UpdateEnforcerHealth();	break;
		}

		m_bHealthChanged = DFALSE;
	}

	// did the player's armor change?

	if (m_bArmorChanged)
	{
		switch (m_nPlayerMode)
		{
			case PM_MODE_FOOT:		UpdateOnFootArmor();	break;

			case PM_MODE_MCA_AO:
			case PM_MODE_MCA_AP:
			case PM_MODE_MCA_SA:
			case PM_MODE_MCA_UE:	UpdateEnforcerArmor();	break;
		}

		m_bArmorChanged = DFALSE;
	}

	// did the player's ammo count change?

	if (m_bAmmoChanged && m_bDrawAmmo)
	{
		switch (m_nPlayerMode)
		{
			case PM_MODE_FOOT:		UpdateOnFootAmmo();		break;

			case PM_MODE_MCA_AP:
			case PM_MODE_MCA_AO:
			case PM_MODE_MCA_SA:
			case PM_MODE_MCA_UE:	UpdateEnforcerAmmo();	break;
		}

		m_bAmmoChanged = DFALSE;
	}

//	if (m_nPlayerMode == PM_MODE_MCA_AP || m_nPlayerMode == PM_MODE_MCA_AO || m_nPlayerMode == PM_MODE_MCA_SA || m_nPlayerMode == PM_MODE_MCA_UE)
//	{
//		UpdateHUDEffect();
//	}
}

void CPlayerStats::UpdatePlayerMode (DBYTE nNewMode, DBOOL bForce)
{
	if (nNewMode == m_nPlayerMode && !bForce) return;
	
	// set the new mode

	m_nPlayerMode = nNewMode;

	// remove all currently loaded surfaces

	if (m_hCrosshair1)		m_pClientDE->DeleteSurface (m_hCrosshair1);
	if (m_hCrosshair2)		m_pClientDE->DeleteSurface (m_hCrosshair2);
	if (m_hCrosshair3)		m_pClientDE->DeleteSurface (m_hCrosshair3);
	if (m_hCleanHUDLeft)	m_pClientDE->DeleteSurface (m_hCleanHUDLeft);
	if (m_hCleanHUDMiddle)	m_pClientDE->DeleteSurface (m_hCleanHUDMiddle);
	if (m_hCleanHUDRight)	m_pClientDE->DeleteSurface (m_hCleanHUDRight);
	if (m_hHUDOverlayLeft)	m_pClientDE->DeleteSurface (m_hHUDOverlayLeft);
	if (m_hHUDOverlayMiddle)m_pClientDE->DeleteSurface (m_hHUDOverlayMiddle);
	if (m_hHUDOverlayRight) m_pClientDE->DeleteSurface (m_hHUDOverlayRight);
	if (m_hHUDLeft)			m_pClientDE->DeleteSurface (m_hHUDLeft);
	if (m_hHUDMiddle)		m_pClientDE->DeleteSurface (m_hHUDMiddle);
	if (m_hHUDRight)		m_pClientDE->DeleteSurface (m_hHUDRight);
	if (m_hHUDEffect)		m_pClientDE->DeleteSurface (m_hHUDEffect);
	if (m_hHUDNumbers)		m_pClientDE->DeleteSurface (m_hHUDNumbers);
	
	m_hCrosshair1		= DNULL;
	m_hCrosshair2		= DNULL;
	m_hCrosshair3		= DNULL;
	m_hZoomCrosshair	= DNULL;
	m_hCleanHUDLeft		= DNULL;
	m_hCleanHUDMiddle	= DNULL;
	m_hCleanHUDRight	= DNULL;
	m_hHUDOverlayLeft	= DNULL;
	m_hHUDOverlayMiddle	= DNULL;
	m_hHUDOverlayRight	= DNULL;
	m_hHUDLeft			= DNULL;
	m_hHUDMiddle		= DNULL;
	m_hHUDRight			= DNULL;
	m_hHUDEffect		= DNULL;
	m_hHUDNumbers		= DNULL;

	memset (m_nNumWidths, 0, sizeof(int) * 10);
	memset (m_nNumOffsets, 0, sizeof(int) * 10);
	
	// load new surfaces

	switch (m_nPlayerMode)
	{
		case PM_MODE_FOOT:		InitOnFoot();	break;

		case PM_MODE_MCA_AP:
		case PM_MODE_MCA_AO:
		case PM_MODE_MCA_SA:
		case PM_MODE_MCA_UE:	InitEnforcer();	break;
	}
}

void CPlayerStats::UpdatePlayerWeapon(DBYTE nWeapon, DBOOL bForce)
{
	if (!m_pClientShell) return;
	if (m_nCurrentWeapon == nWeapon && !bForce) return;
	
	if (GetWeaponType((RiotWeaponId)nWeapon) == MELEE && m_pClientShell)
	{
		// we will not be drawing the ammo count, so make sure all of it gets removed...
		m_pClientShell->AddToClearScreenCount();
	}

	m_nCurrentWeapon = nWeapon;

	if (m_hAmmoIcon)
	{
		m_pClientDE->DeleteSurface (m_hAmmoIcon);
		m_hAmmoIcon = DNULL;
	}
	
	char strFilename[MAX_CS_FILENAME_LEN];
	switch (nWeapon)
	{
		case GUN_PULSERIFLE_ID:			SAFE_STRCPY(strFilename, "interface/Ammo_PulseRifle.pcx"); break;
		case GUN_SPIDER_ID:				SAFE_STRCPY(strFilename, "interface/Ammo_Spider.pcx"); break;
		case GUN_BULLGUT_ID:			SAFE_STRCPY(strFilename, "interface/Ammo_Bullgut.pcx"); break;
		case GUN_SNIPERRIFLE_ID:		SAFE_STRCPY(strFilename, "interface/Ammo_SniperRifle.pcx"); break;
		case GUN_JUGGERNAUT_ID:			SAFE_STRCPY(strFilename, "interface/Ammo_Juggernaut.pcx"); break;
		case GUN_SHREDDER_ID:			SAFE_STRCPY(strFilename, "interface/Ammo_Shredder.pcx"); break;
		case GUN_REDRIOT_ID:			SAFE_STRCPY(strFilename, "interface/Ammo_RedRiot.pcx"); break;
		
		case GUN_COLT45_ID:				SAFE_STRCPY(strFilename, "interface/Ammo_Colt45.pcx"); break;
		case GUN_SHOTGUN_ID:			SAFE_STRCPY(strFilename, "interface/Ammo_Shotgun.pcx"); break;
		case GUN_ASSAULTRIFLE_ID:		SAFE_STRCPY(strFilename, "interface/Ammo_AssaultRifle.pcx"); break;
		case GUN_ENERGYGRENADE_ID:		SAFE_STRCPY(strFilename, "interface/Ammo_EnergyGrenade.pcx"); break;
		case GUN_KATOGRENADE_ID:		SAFE_STRCPY(strFilename, "interface/Ammo_KatoGrenade.pcx"); break;
		case GUN_MAC10_ID:				SAFE_STRCPY(strFilename, "interface/Ammo_Mac10.pcx"); break;
		case GUN_TOW_ID:				SAFE_STRCPY(strFilename, "interface/Ammo_Tow.pcx"); break;
		case GUN_LASERCANNON_ID:		SAFE_STRCPY(strFilename, "interface/Ammo_LaserCannon.pcx"); break;

		// handle the melee weapons (turn off ammo display)

		case GUN_SQUEAKYTOY_ID:
		case GUN_ENERGYBATON_ID:
		case GUN_ENERGYBLADE_ID:
		case GUN_KATANA_ID:
		case GUN_MONOKNIFE_ID:
		case GUN_TANTO_ID:
		{
			SetDrawAmmo (DFALSE);
			return;
		}

		default:						SAFE_STRCPY(strFilename, ""); break;
	}
	
	if (!m_pClientShell->IsVehicleMode())
	{
		SetDrawAmmo (DTRUE);
	}

	if (strFilename[0] != '\0') m_hAmmoIcon = m_pClientDE->CreateSurfaceFromBitmap (strFilename);
	if (m_hAmmoIcon)
	{
		m_pClientDE->GetSurfaceDims (m_hAmmoIcon, &m_cxAmmoIcon, &m_cyAmmoIcon);
	}
	
	m_bAmmoChanged = DTRUE;
	Update();
}

void CPlayerStats::UpdateHealth (DDWORD nHealth)
{ 
	if (m_nHealth == nHealth) return;
	if (!m_pClientDE) return;

	// update the member variable

	m_nHealth = nHealth;
	m_bHealthChanged = DTRUE;
}

void CPlayerStats::UpdateArmor (DDWORD nArmor)
{ 
	if (m_nArmor == nArmor) return;
	if (!m_pClientDE) return;

	// update the member variable

	m_nArmor = nArmor; 
	m_bArmorChanged = DTRUE;
}

void CPlayerStats::UpdateAmmo (DDWORD nType, DDWORD nAmmo)
{ 
	if (!m_pClientDE) return;

	if (nType < 0 || nType > GUN_MAX_NUMBER) return;

	m_nAmmo[nType] = nAmmo;
	if (nType == m_nCurrentWeapon) m_bAmmoChanged = DTRUE;
}

void CPlayerStats::UpdateAir (DFLOAT fPercent)
{
	if (!m_pClientDE) return;
	if (m_fAirPercent == fPercent) return;

	m_fAirPercent = fPercent;

	// clear the meter

	m_pClientDE->FillRect (m_hAirMeter, &m_rcAirBar, DNULL);

	// draw blue up to the correct percent mark
	
	HDECOLOR hColor = m_pClientDE->SetupColor1(1.0f - fPercent, 0.0f, fPercent, DFALSE);
	HDECOLOR hTransColor = m_pClientDE->SetupColor2(0.0f, 1.0f, 0.0f, DFALSE);
	
	DRect rcPower = m_rcAirBar;
	rcPower.top = m_rcAirBar.bottom - (int)((float)(m_rcAirBar.bottom - m_rcAirBar.top) * fPercent);
	m_pClientDE->FillRect (m_hAirMeter, &rcPower, hColor);
	
	hTransColor = m_pClientDE->SetupColor2 (0.0f, 1.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hAirMeter, hTransColor);
}

void CPlayerStats::ToggleCrosshair()
{
	int nMaxLevel = 2;

	if (m_nPlayerMode == PM_MODE_FOOT) nMaxLevel = 1;
	
	if (--m_nCrosshairLevel < 0) m_nCrosshairLevel = nMaxLevel;
}

void CPlayerStats::InitOnFoot()
{
	// make sure crosshair level is set correctly

	m_nCrosshairLevel = 1;

	// load the surfaces

	m_hCrosshair2 = m_pClientDE->CreateSurfaceFromBitmap ("interface/OnFoot/Cross_Middle.pcx");
	HDECOLOR hTransColor = m_pClientDE->SetupColor1(0.0f, 0.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hCrosshair2, hTransColor);

	m_hZoomCrosshair = m_pClientDE->CreateSurfaceFromBitmap ("interface/OnFoot/Cross_Zoom.pcx");
	m_pClientDE->OptimizeSurface (m_hZoomCrosshair, hTransColor);

	m_hCleanHUDLeft = m_pClientDE->CreateSurfaceFromBitmap ("interface/OnFoot/HUD_Left.pcx");
	m_hCleanHUDRight = m_pClientDE->CreateSurfaceFromBitmap ("interface/OnFoot/HUD_Right.pcx");
	m_hHUDNumbers = m_pClientDE->CreateSurfaceFromBitmap ("interface/OnFoot/HUD_Nums.pcx");

	// init number widths and offsets

	m_nNumWidths[0] = 14;
	m_nNumWidths[1] = 8;
	m_nNumWidths[2] = 12;
	m_nNumWidths[3] = 12;
	m_nNumWidths[4] = 14;
	m_nNumWidths[5] = 13;
	m_nNumWidths[6] = 13;
	m_nNumWidths[7] = 13;
	m_nNumWidths[8] = 14;
	m_nNumWidths[9] = 14;

	int nOffset = 0;
	for (int i = 0; i < 10; i++)
	{
		m_nNumOffsets[i] = nOffset;
		nOffset += m_nNumWidths[i];
	}

	// create the "dirty" HUD surfaces

	DDWORD nWidth = 0;
	DDWORD nHeight = 0;

	m_pClientDE->GetSurfaceDims (m_hCleanHUDLeft, &nWidth, &nHeight);
	m_hHUDLeft = m_pClientDE->CreateSurface (nWidth, nHeight);
	
	m_pClientDE->GetSurfaceDims (m_hCleanHUDRight, &nWidth, &nHeight);
	m_hHUDRight = m_pClientDE->CreateSurface (nWidth, nHeight);

	// force updating of the surfaces

	UpdateOnFootHealth();
	UpdateOnFootArmor();
	UpdateOnFootAmmo();
}

void CPlayerStats::InitEnforcer()
{
	// make sure crosshair level is set correctly

	m_nCrosshairLevel = 2;

	// load the surfaces

	m_hCrosshair1 = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/Cross_Left.pcx");
	m_hCrosshair2 = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/Cross_Middle.pcx");
	m_hCrosshair3 = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/Cross_Right.pcx");
	m_hZoomCrosshair = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/Cross_Zoom.pcx");

	HDECOLOR hTransColor = m_pClientDE->SetupColor1(0.0f, 0.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hCrosshair1, hTransColor);
	m_pClientDE->OptimizeSurface (m_hCrosshair2, hTransColor);
	m_pClientDE->OptimizeSurface (m_hCrosshair3, hTransColor);
	m_pClientDE->OptimizeSurface (m_hZoomCrosshair, hTransColor);

	m_hCleanHUDLeft = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/HUD_Left.pcx");
	m_hCleanHUDMiddle = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/HUD_Mid.pcx");
	m_hCleanHUDRight = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/HUD_Right.pcx");
	m_hHUDOverlayLeft = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/HUD_LeftOverlay.pcx");
	m_hHUDOverlayMiddle = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/HUD_MidOverlay.pcx");
	m_hHUDOverlayRight = m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/HUD_RightOverlay.pcx");
	m_hHUDEffect = 	m_pClientDE->CreateSurfaceFromBitmap ("interface/Enforcer/HUD_Effect.pcx");
	m_hHUDNumbers = m_pClientDE->CreateSurfaceFromBitmap ("interface/OnFoot/HUD_Nums.pcx");

	// init number widths and offsets

	m_nNumWidths[0] = 14;
	m_nNumWidths[1] = 8;
	m_nNumWidths[2] = 12;
	m_nNumWidths[3] = 12;
	m_nNumWidths[4] = 14;
	m_nNumWidths[5] = 13;
	m_nNumWidths[6] = 13;
	m_nNumWidths[7] = 13;
	m_nNumWidths[8] = 14;
	m_nNumWidths[9] = 14;

	int nOffset = 0;
	for (int i = 0; i < 10; i++)
	{
		m_nNumOffsets[i] = nOffset;
		nOffset += m_nNumWidths[i];
	}
	
	// create the "dirty" HUD surfaces

	DDWORD nWidth = 0;
	DDWORD nHeight = 0;

	m_pClientDE->GetSurfaceDims (m_hCleanHUDLeft, &nWidth, &nHeight);
	m_hHUDDataLeft = m_pClientDE->CreateSurface (nWidth, nHeight);
	m_hHUDLeft = m_pClientDE->CreateSurface (nWidth, nHeight);
	
	m_pClientDE->GetSurfaceDims (m_hCleanHUDMiddle, &nWidth, &nHeight);
	m_hHUDDataMiddle = m_pClientDE->CreateSurface (nWidth, nHeight);
	m_hHUDMiddle = m_pClientDE->CreateSurface (nWidth, nHeight);
	
	m_pClientDE->GetSurfaceDims (m_hCleanHUDRight, &nWidth, &nHeight);
	m_hHUDDataRight = m_pClientDE->CreateSurface (nWidth, nHeight);
	m_hHUDRight = m_pClientDE->CreateSurface (nWidth, nHeight);
	
	// force updating of the surfaces

	UpdateEnforcerHealth();
	UpdateEnforcerArmor();
	UpdateEnforcerAmmo();
}

void CPlayerStats::DrawOnFoot (HSURFACE hScreen, int nLeft, int nTop, int nRight, int nBottom, DBOOL bStatsSizedOff)
{
	if (!m_pClientDE) return;

	int nScreenWidth		= nRight - nLeft;
	int nScreenHeight		= nBottom - nTop;

	DBOOL bAdjustSizes		= nScreenWidth < 512;
	float fAdjustment		= 1.0f;

	if (nScreenWidth == 400)
	{
		fAdjustment = 0.6f;
	}
	else if (nScreenWidth == 320)
	{
		fAdjustment = 0.73f;
	}

	DBOOL bZoomed = m_pClientShell->IsZoomed();

	int cxCrosshairMiddle	= bZoomed ? 256: 13;
	int cyCrosshairMiddle	= cxCrosshairMiddle;
	
	int xHUDLeft			= 0;
	int yHUDLeft			= nBottom - (int)(bAdjustSizes ? 44.0f * fAdjustment : 44);
	int xHUDRight			= nRight - (int)(bAdjustSizes ? 116.0f * fAdjustment : 116);
	int yHUDRight			= nBottom - (int)(bAdjustSizes ? 44.0f * fAdjustment : 44);

	DDWORD cxHUDLeft		= 0;
	DDWORD cyHUDLeft		= 0;
	DDWORD cxHUDRight		= 0;
	DDWORD cyHUDRight		= 0;

	m_pClientDE->GetSurfaceDims (m_hHUDLeft, &cxHUDLeft, &cyHUDLeft);
	m_pClientDE->GetSurfaceDims (m_hHUDRight, &cxHUDRight, &cyHUDRight);
	
	// set up the transparent color

	HDECOLOR hTransColor = DNULL;

	// draw the crosshair - only one level when on foot

	if (m_bCrosshairEnabled && m_nCrosshairLevel && !m_pClientShell->IsDialogVisible())
	{
		int x = nLeft + ((nScreenWidth - cxCrosshairMiddle) >> 1);
		int y = nTop + ((nScreenHeight - cyCrosshairMiddle) >> 1);

		HSURFACE hCross = bZoomed ? m_hZoomCrosshair : m_hCrosshair2; 
		m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, hCross, NULL, x, y, hTransColor);
	}
	
	// draw the HUD

	if (bAdjustSizes)
	{
		DRect rcDest;
		hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);
		if (!bStatsSizedOff && m_bDrawHud)
		{
			rcDest.left = xHUDLeft;
			rcDest.top = yHUDLeft;
			rcDest.right = xHUDLeft + (int)((float)cxHUDLeft * fAdjustment);
			rcDest.bottom = yHUDLeft + (int)((float)cyHUDLeft * fAdjustment);
			
			m_pClientDE->ScaleSurfaceToSurfaceTransparent (hScreen, m_hHUDLeft, &rcDest, NULL, hTransColor);

			if (m_bDrawAmmo)
			{
				rcDest.left = xHUDRight;
				rcDest.top = yHUDRight;
				rcDest.right = xHUDRight + (int)((float)cxHUDRight * fAdjustment);
				rcDest.bottom = yHUDRight + (int)((float)cyHUDRight * fAdjustment);
				
				m_pClientDE->ScaleSurfaceToSurfaceTransparent (hScreen, m_hHUDRight, &rcDest, NULL, hTransColor);
			}
		}
	}
	else
	{
		hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);
		if (!bStatsSizedOff && m_bDrawHud)
		{
			m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hHUDLeft, NULL, xHUDLeft, yHUDLeft, hTransColor);

			if (m_bDrawAmmo)
			{
				m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hHUDRight, NULL, xHUDRight, yHUDRight, hTransColor);
			}
		}
	}
}

void CPlayerStats::DrawEnforcer (HSURFACE hScreen, int nLeft, int nTop, int nRight, int nBottom, DBOOL bStatsSizedOff)
{
	if (!m_pClientDE) return;

	int nScreenWidth		= nRight - nLeft;
	int nScreenHeight		= nBottom - nTop;

	DBOOL bAdjustSizes		= nScreenWidth < 512;
	float fAdjustment		= 1.0f;

	if (nScreenWidth == 400)
	{
		fAdjustment = 0.8f;
	}
	else if (nScreenWidth == 320)
	{
		fAdjustment = 0.8f;
	}

	int cxCrosshairLeft		= 31;
	int cyCrosshairLeft		= 69;

	DBOOL bZoomed = m_pClientShell->IsZoomed();

	int cxCrosshairMiddle	= bZoomed ? 256: 9;
	int cyCrosshairMiddle	= cxCrosshairMiddle;

	int cxCrosshairRight	= 31;
	int cyCrosshairRight	= 69;
	int xHUDLeft			= (int) (bAdjustSizes ? 10.0f * fAdjustment : 10);
	int yHUDLeft			= nBottom - (int) (bAdjustSizes ? 49.0f * fAdjustment : 49);
	int xHUDMiddle			= (int) (bAdjustSizes ? 84.0f * fAdjustment : 84);
	int yHUDMiddle			= nBottom - (int) (bAdjustSizes ? 49.0f * fAdjustment : 49);
	int xHUDRight			= nRight - (int) (bAdjustSizes ? 110.0f * fAdjustment : 110);
	int yHUDRight			= nBottom - (int) (bAdjustSizes ? 49.0f * fAdjustment : 49);

	DDWORD cxHUDLeft		= 0;
	DDWORD cyHUDLeft		= 0;
	DDWORD cxHUDMiddle		= 0;
	DDWORD cyHUDMiddle		= 0;
	DDWORD cxHUDRight		= 0;
	DDWORD cyHUDRight		= 0;

	m_pClientDE->GetSurfaceDims (m_hHUDLeft, &cxHUDLeft, &cyHUDLeft);
	m_pClientDE->GetSurfaceDims (m_hHUDMiddle, &cxHUDMiddle, &cyHUDMiddle);
	m_pClientDE->GetSurfaceDims (m_hHUDRight, &cxHUDRight, &cyHUDRight);
	
	// set up the transparent color

	HDECOLOR hTransColor = DNULL;

	// draw the crosshair

	if (m_bCrosshairEnabled && m_nCrosshairLevel && !m_pClientShell->IsDialogVisible())
	{
		if (bZoomed)
		{
			int x = nLeft + ((nScreenWidth - cxCrosshairMiddle) >> 1);
			int y = nTop + ((nScreenHeight - cyCrosshairMiddle) >> 1);

			m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hZoomCrosshair, NULL, x, y, hTransColor);
		}
		else
		{
			if (m_nCrosshairLevel == 2)
			{
				int x = (nScreenWidth >> 1) - 68;
				int y = nTop + ((nScreenHeight - cyCrosshairLeft) >> 1);
				m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hCrosshair1, NULL, x, y, hTransColor);
			
				x = (nScreenWidth >> 1) + 36;
				y = nTop + ((nScreenHeight - cyCrosshairRight) >> 1);
				m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hCrosshair3, NULL, x, y, hTransColor);
			}
	
			int x = nLeft + ((nScreenWidth - cxCrosshairMiddle) >> 1);
			int y = nTop + ((nScreenHeight - cyCrosshairMiddle) >> 1);

			m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hCrosshair2, NULL, x, y, hTransColor);
		}
	}

	hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DTRUE);

	if (!bStatsSizedOff && m_bDrawHud)
	{
		// draw the HUD

		if (bAdjustSizes)
		{
			DRect rcDest;

			rcDest.left = xHUDLeft;
			rcDest.top = yHUDLeft;
			rcDest.right = xHUDLeft + (int)((float)cxHUDLeft * fAdjustment);
			rcDest.bottom = yHUDLeft + (int)((float)cyHUDLeft * fAdjustment);

			m_pClientDE->ScaleSurfaceToSurfaceTransparent (hScreen, m_hHUDLeft, &rcDest, NULL, hTransColor);
			
			rcDest.left = xHUDMiddle;
			rcDest.top = yHUDMiddle;
			rcDest.right = xHUDMiddle + (int)((float)cxHUDMiddle * fAdjustment);
			rcDest.bottom = yHUDMiddle + (int)((float)cyHUDMiddle * fAdjustment);

			m_pClientDE->ScaleSurfaceToSurfaceTransparent (hScreen, m_hHUDMiddle, &rcDest, NULL, hTransColor);

			if (m_bDrawAmmo)
			{
				rcDest.left = xHUDRight;
				rcDest.top = yHUDRight;
				rcDest.right = xHUDRight + (int)((float)cxHUDRight * fAdjustment);
				rcDest.bottom = yHUDRight + (int)((float)cyHUDRight * fAdjustment);
				
				m_pClientDE->ScaleSurfaceToSurfaceTransparent (hScreen, m_hHUDRight, &rcDest, NULL, hTransColor);
			}
		}
		else
		{
			m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hHUDLeft, NULL, xHUDLeft, yHUDLeft, hTransColor);
			m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hHUDMiddle, NULL, xHUDMiddle, yHUDMiddle, hTransColor);

			if (m_bDrawAmmo)
			{
				m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hHUDRight, NULL, xHUDRight, yHUDRight, hTransColor);
			}
		}
	}
}

void CPlayerStats::UpdateOnFootHealth()
{
	if (!m_pClientDE) return;

	// create the transparent color
	
	HDECOLOR hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);
	
	// clean the surface

	DRect rcSrc;
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = 76;
	rcSrc.bottom = 44;
	m_pClientDE->DrawSurfaceToSurface (m_hHUDLeft, m_hCleanHUDLeft, &rcSrc, 0, 0);
	
	// draw the health value to the surface

	int nTempNum = m_nHealth;
	int nNum = 0;
	int x = 67;	
	rcSrc.top = 0;
	rcSrc.bottom = 44;
	
	int j = 1;
	for (int i = 10; i <= 10000 && (i == 10 || nTempNum > 0); i *= 10, j *= 10)
	{
		nNum = (nTempNum % i) / j;
		nTempNum -= (nNum * j);
		x -= m_nNumWidths[nNum];
		rcSrc.left = m_nNumOffsets[nNum];
		rcSrc.right = rcSrc.left + m_nNumWidths[nNum];
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDLeft, m_hHUDNumbers, &rcSrc, x, 13, hTransColor);
	}

	hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hHUDLeft, hTransColor);
}

void CPlayerStats::UpdateEnforcerHealth()
{
	if (!m_pClientDE) return;

	// create the transparent color
	
	HDECOLOR hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);
	
	// clean the surface

	m_pClientDE->DrawSurfaceToSurface (m_hHUDLeft, m_hCleanHUDLeft, DNULL, 0, 0);

	// draw the effect and overlay to the surface

	DRect rcDst;
	rcDst.top = (int)m_nEffectPos;
	rcDst.bottom = (int)m_nEffectPos + 17;
	rcDst.left = 1;
	rcDst.right = 63;
	m_pClientDE->ScaleSurfaceToSurface (m_hHUDLeft, m_hHUDEffect, &rcDst, DNULL);

	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDLeft, m_hHUDOverlayLeft, DNULL, 0, 0, hTransColor);

	// draw the health value to the data surface

	m_pClientDE->FillRect (m_hHUDDataLeft, DNULL, DNULL);

	int nTempNum = m_nHealth;
	int nNum = 0;
	int x = 57;	
	DRect rcSrc;
	rcSrc.top = 0;
	rcSrc.bottom = 44;
	
	int j = 1;
	for (int i = 10; i <= 10000 && (i == 10 || nTempNum > 0); i *= 10, j *= 10)
	{
		nNum = (nTempNum % i) / j;
		nTempNum -= (nNum * j);
		x -= m_nNumWidths[nNum];
		rcSrc.left = m_nNumOffsets[nNum];
		rcSrc.right = rcSrc.left + m_nNumWidths[nNum];
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDDataLeft, m_hHUDNumbers, &rcSrc, x, 10, hTransColor);
	}

	// draw the data surface to the real surface
	
	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDLeft, m_hHUDDataLeft, DNULL, 0, 0, DNULL);
	
	hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hHUDLeft, hTransColor);
}
	
void CPlayerStats::UpdateOnFootArmor()
{
	if (!m_pClientDE) return;

	// create the transparent color
	
	HDECOLOR hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);
	
	// clean the surface

	DRect rcSrc;
	rcSrc.left = 76;
	rcSrc.top = 0;
	rcSrc.right = 152;
	rcSrc.bottom = 44;
	m_pClientDE->DrawSurfaceToSurface (m_hHUDLeft, m_hCleanHUDLeft, &rcSrc, 76, 0);
	
	// draw the armor value to the surface

	int nTempNum = m_nArmor;
	int nNum = 0;
	int x = 140;	
	rcSrc.top = 0;
	rcSrc.bottom = 44;
	
	int j = 1;
	for (int i = 10; i <= 10000 && (i == 10 || nTempNum > 0); i *= 10, j *= 10)
	{
		nNum = (nTempNum % i) / j;
		nTempNum -= (nNum * j);
		x -= m_nNumWidths[nNum];
		rcSrc.left = m_nNumOffsets[nNum];
		rcSrc.right = rcSrc.left + m_nNumWidths[nNum];
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDLeft, m_hHUDNumbers, &rcSrc, x, 13, hTransColor);
	}

	hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hHUDLeft, hTransColor);
}

void CPlayerStats::UpdateEnforcerArmor()
{
	if (!m_pClientDE) return;

	// create the transparent color
	
	HDECOLOR hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);
	
	// clean the surface

	m_pClientDE->DrawSurfaceToSurface (m_hHUDMiddle, m_hCleanHUDMiddle, DNULL, 0, 0);

	// draw the effect and overlay to the surface

	DRect rcDst;
	rcDst.top = (int)m_nEffectPos;
	rcDst.bottom = (int)m_nEffectPos + 17;
	rcDst.left = 1;
	rcDst.right = 63;
	m_pClientDE->ScaleSurfaceToSurface (m_hHUDMiddle, m_hHUDEffect, &rcDst, DNULL);

	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDMiddle, m_hHUDOverlayMiddle, DNULL, 0, 0, hTransColor);

	// draw the armor value to the data surface

	m_pClientDE->FillRect (m_hHUDDataMiddle, DNULL, DNULL);

	int nTempNum = m_nArmor;
	int nNum = 0;
	int x = 57;	
	DRect rcSrc;
	rcSrc.top = 0;
	rcSrc.bottom = 44;
	
	int j = 1;
	for (int i = 10; i <= 10000 && (i == 10 || nTempNum > 0); i *= 10, j *= 10)
	{
		nNum = (nTempNum % i) / j;
		nTempNum -= (nNum * j);
		x -= m_nNumWidths[nNum];
		rcSrc.left = m_nNumOffsets[nNum];
		rcSrc.right = rcSrc.left + m_nNumWidths[nNum];
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDDataMiddle, m_hHUDNumbers, &rcSrc, x, 10, hTransColor);
	}

	// draw the data surface to the real surface
	
	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDMiddle, m_hHUDDataMiddle, DNULL, 0, 0, DNULL);
	
	hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hHUDMiddle, hTransColor);
}


void CPlayerStats::UpdateOnFootAmmo()
{
	if (!m_pClientDE) return;

	// Don't display any ammo for melee weapons...

	if (GetWeaponType((RiotWeaponId)m_nCurrentWeapon) == MELEE ||
		m_nCurrentWeapon == GUN_SQUEAKYTOY_ID) return;
	
	// create the transparent color
	
	HDECOLOR hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);
	
	// clean the surface

	m_pClientDE->DrawSurfaceToSurface (m_hHUDRight, m_hCleanHUDRight, DNULL, 0, 0);
	
	// draw the ammo amount to the surface

	DRect rcSrc;
	memset (&rcSrc, 0, sizeof (DRect));

	int nTempNum = m_nAmmo[m_nCurrentWeapon];
	int nNum = 0;
	int x = 102;
	rcSrc.top = 0;
	rcSrc.bottom = 44;
	
	int j = 1;
	for (int i = 10; i <= 10000 && (i == 10 || nTempNum > 0); i *= 10, j *= 10)
	{
		nNum = (nTempNum % i) / j;
		nTempNum -= (nNum * j);
		x -= m_nNumWidths[nNum];
		rcSrc.left = m_nNumOffsets[nNum];
		rcSrc.right = rcSrc.left + m_nNumWidths[nNum];
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDRight, m_hHUDNumbers, &rcSrc, x, 13, hTransColor);
	}

	// if the ammo icon exists, draw it to the surface

	if (m_hAmmoIcon)
	{
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDRight, m_hAmmoIcon, DNULL, 3 + ((37 - (int)m_cxAmmoIcon) / 2), 5 + ((25 - (int)m_cyAmmoIcon) / 2), DNULL);
	}

	hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hHUDRight, hTransColor);
}

void CPlayerStats::UpdateEnforcerAmmo()
{
	if (!m_pClientDE) return;

	// Don't display any ammo for melee weapons...

	if (GetWeaponType((RiotWeaponId)m_nCurrentWeapon) == MELEE) return;


	// create the transparent color
	
	HDECOLOR hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);
	
	// clean the surface

	m_pClientDE->DrawSurfaceToSurface (m_hHUDRight, m_hCleanHUDRight, DNULL, 0, 0);

	// draw the effect and overlay to the surface

	m_pClientDE->DrawSurfaceToSurface (m_hHUDRight, m_hHUDEffect, DNULL, 1, (int)m_nEffectPos);
	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDRight, m_hHUDOverlayRight, DNULL, 0, 0, hTransColor);

	// draw the ammo count to the data surface

	m_pClientDE->FillRect (m_hHUDDataRight, DNULL, DNULL);

	int nTempNum = m_nAmmo[m_nCurrentWeapon];
	int nNum = 0;
	int x = 92;	
	DRect rcSrc;
	rcSrc.top = 0;
	rcSrc.bottom = 44;
	
	int j = 1;
	for (int i = 10; i <= 10000 && (i == 10 || nTempNum > 0); i *= 10, j *= 10)
	{
		nNum = (nTempNum % i) / j;
		nTempNum -= (nNum * j);
		x -= m_nNumWidths[nNum];
		rcSrc.left = m_nNumOffsets[nNum];
		rcSrc.right = rcSrc.left + m_nNumWidths[nNum];
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDDataRight, m_hHUDNumbers, &rcSrc, x, 10, hTransColor);
	}

	// if the ammo icon exists, draw it to the data surface

	if (m_hAmmoIcon)
	{
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDDataRight, m_hAmmoIcon, DNULL, 5 + ((32 - (int)m_cxAmmoIcon) / 2), 10 + ((12 - (int)m_cyAmmoIcon) / 2), DNULL);
	}

	// draw the data surface to the real surface

	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDRight, m_hHUDDataRight, DNULL, 0, 0, DNULL);

	hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hHUDRight, hTransColor);
}


void CPlayerStats::UpdateHUDEffect()
{
	if (!m_pClientDE) return;

	m_nEffectPos -= (60.0f * m_pClientDE->GetFrameTime());
	if (m_nEffectPos < -15.0f) m_nEffectPos = 50.0f;//37;

	HDECOLOR hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);
	
	// Left HUD

	m_pClientDE->DrawSurfaceToSurface (m_hHUDLeft, m_hCleanHUDLeft, DNULL, 0, 0);
	DRect rcDst;
	rcDst.top = __max ((int)m_nEffectPos, 2);
	rcDst.bottom = (int)m_nEffectPos + 17;	// if scalesurface... bug gets fixed this should be __min (m_nEffectPos + 17, 37)
	rcDst.left = 1;
	rcDst.right = 63;
	m_pClientDE->ScaleSurfaceToSurface (m_hHUDLeft, m_hHUDEffect, &rcDst, DNULL);
	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDLeft, m_hHUDOverlayLeft, DNULL, 0, 0, hTransColor);
	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDLeft, m_hHUDDataLeft, DNULL, 0, 0, DNULL);

	// Middle HUD

	m_pClientDE->DrawSurfaceToSurface (m_hHUDMiddle, m_hCleanHUDMiddle, DNULL, 0, 0);
	rcDst.top = __max ((int)m_nEffectPos, 2);
	rcDst.bottom = (int)m_nEffectPos + 17;		
	rcDst.left = 1;
	rcDst.right = 63;
	m_pClientDE->ScaleSurfaceToSurface (m_hHUDMiddle, m_hHUDEffect, &rcDst, DNULL);
	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDMiddle, m_hHUDOverlayMiddle, DNULL, 0, 0, hTransColor);
	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDMiddle, m_hHUDDataMiddle, DNULL, 0, 0, DNULL);
	
	// Right HUD

	DRect rcSrc;
	rcSrc.top = __max (2 - (int)m_nEffectPos, 0);
	rcSrc.bottom = 38 - (int)m_nEffectPos;
	rcSrc.left = 0;
	rcSrc.right = 98;
	m_pClientDE->DrawSurfaceToSurface (m_hHUDRight, m_hCleanHUDRight, DNULL, 0, 0);
	m_pClientDE->DrawSurfaceToSurface (m_hHUDRight, m_hHUDEffect, &rcSrc, 1, __max ((int)m_nEffectPos, 2));
	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDRight, m_hHUDOverlayRight, DNULL, 0, 0, hTransColor);
	m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hHUDRight, m_hHUDDataRight, DNULL, 0, 0, DNULL);

	// Optimize surfaces

	hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (m_hHUDLeft, hTransColor);
	m_pClientDE->OptimizeSurface (m_hHUDMiddle, hTransColor);
	m_pClientDE->OptimizeSurface (m_hHUDRight, hTransColor);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::Save
//
//	PURPOSE:	Save the player stats info
//
// --------------------------------------------------------------------------- //

void CPlayerStats::Save(HMESSAGEWRITE hWrite)
{
	if (!m_pClientDE) return;

	m_pClientDE->WriteToMessageDWord(hWrite, m_nHealth);
	m_pClientDE->WriteToMessageDWord(hWrite, m_nArmor);
	m_pClientDE->WriteToMessageDWord(hWrite, m_nCrosshairLevel);

	for (int i=0; i < GUN_MAX_NUMBER; i++)
	{
		m_pClientDE->WriteToMessageDWord(hWrite, m_nAmmo[i]);
	}

	m_pClientDE->WriteToMessageByte(hWrite, m_nCurrentWeapon);
	m_pClientDE->WriteToMessageByte(hWrite, m_nPlayerMode);
	m_pClientDE->WriteToMessageByte(hWrite, m_bCrosshairEnabled);
	m_pClientDE->WriteToMessageByte(hWrite, m_bDrawHud);
	m_pClientDE->WriteToMessageByte(hWrite, m_bDrawAmmo);
	m_pClientDE->WriteToMessageFloat(hWrite, m_fAirPercent);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::Load
//
//	PURPOSE:	Load the player stats info
//
// --------------------------------------------------------------------------- //

void CPlayerStats::Load(HMESSAGEREAD hRead)
{
	if (!m_pClientDE) return;

	m_nHealth			= m_pClientDE->ReadFromMessageDWord(hRead);
	m_nArmor			= m_pClientDE->ReadFromMessageDWord(hRead);
	m_nCrosshairLevel	= (int) m_pClientDE->ReadFromMessageDWord(hRead);

	for (int i=0; i < GUN_MAX_NUMBER; i++)
	{
		m_nAmmo[i] = m_pClientDE->ReadFromMessageDWord(hRead);
	}

	m_nCurrentWeapon	= m_pClientDE->ReadFromMessageByte(hRead);
	m_nPlayerMode		= m_pClientDE->ReadFromMessageByte(hRead);
	m_bCrosshairEnabled	= (DBOOL) m_pClientDE->ReadFromMessageByte(hRead);
	m_bDrawHud			= (DBOOL) m_pClientDE->ReadFromMessageByte(hRead);
	m_bDrawAmmo			= (DBOOL) m_pClientDE->ReadFromMessageByte(hRead);
	m_fAirPercent		= m_pClientDE->ReadFromMessageFloat(hRead);

	UpdatePlayerWeapon(m_nCurrentWeapon, DTRUE);
}
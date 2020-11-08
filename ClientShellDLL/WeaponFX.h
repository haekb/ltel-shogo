// ----------------------------------------------------------------------- //
//
// MODULE  : WeaponFX.h
//
// PURPOSE : Weapon special fx class - Definition
//
// CREATED : 2/22/98
//
// ----------------------------------------------------------------------- //

#ifndef __WEAPON_FX_H__
#define __WEAPON_FX_H__

#include "SpecialFX.h"
#include "SurfaceTypes.h"
#include "TracerFX.h"
#include "SmokeFX.h"
#include "ContainerCodes.h"
#include "WeaponDefs.h"
#include "ModelFuncs.h"
#include "RiotSettings.h"

struct WCREATESTRUCT : public SFXCREATESTRUCT
{
	WCREATESTRUCT::WCREATESTRUCT();

	DBYTE		nWeaponId;
	DBYTE		nSurfaceType;
	DBYTE		nIgnoreFX;
	DVector		vFirePos;
	DVector		vPos;
	DRotation	rRot;
	DBYTE		nShooterId;
	DBOOL		bLocal;
};

inline WCREATESTRUCT::WCREATESTRUCT()
{
	memset(this, 0, sizeof(WCREATESTRUCT));
	nShooterId = -1;
}


class CWeaponFX : public CSpecialFX
{
	public :

		CWeaponFX() : CSpecialFX() 
		{
			m_nWeaponId		= GUN_NONE;
			m_eSurfaceType	= ST_UNKNOWN;
			m_nFX			= 0;
			m_nIgnoreFX		= 0;
			m_fDamage		= 0;
			m_nShooterId	= -1;
			m_nLocalId		= -1;
			m_bLocal		= DFALSE;

			VEC_INIT(m_vFirePos);
			VEC_INIT(m_vPos);
			VEC_INIT(m_vDir);
			VEC_INIT(m_vSurfaceNormal);
			VEC_SET(m_vLightColor, 1.0f, 1.0f, 1.0f);

			ROT_INIT(m_rRotation);
			ROT_INIT(m_rSurfaceRot);
			ROT_INIT(m_rDirRot);

			m_eSize			= MS_NORMAL;
			m_eCode			= CC_NONE;
			m_eFirePosCode	= CC_NONE;

			m_bExplosionWeapon = DFALSE;
			m_fFireDistance	   = 100.0f;

			m_nDetailLevel	= RS_HIGH;
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update() { return DFALSE; }

	protected :
	
		RiotWeaponId	m_nWeaponId;		// Id of weapon fired
		SurfaceType		m_eSurfaceType;		// Surface hit by bullet
		DBYTE			m_nFX;				// FX to create
		DBYTE			m_nIgnoreFX;		// FX to ignore
		DFLOAT			m_fDamage;			// Damage done by bullet
		DRotation		m_rRotation;		// Norval of surface (as rotation)
		DVector			m_vFirePos;			// Position bullet was fired from
		DVector			m_vPos;				// Impact pos

		DBYTE			m_nDetailLevel;		// Current detail level setting

		ModelSize		m_eSize;			// Size of weapon fx
		ContainerCode	m_eCode;			// Container effect is in
		ContainerCode	m_eFirePosCode;		// Container fire pos is in

		DBOOL			m_bExplosionWeapon;	// Did the weapon go boom?
		DFLOAT			m_fFireDistance;	// Distance from fire pos to pos
		DVector			m_vDir;				// Direction from fire pos to pos
		DVector			m_vSurfaceNormal;	// Normal of surface of impact
		DRotation		m_rSurfaceRot;		// Rotation based on m_vSurfaceNormal
		DRotation		m_rDirRot;			// Rotation based on m_vDir

		DBYTE			m_nShooterId;		// Client id of the shooter
		DBYTE			m_nLocalId;			// Local client id
		DBOOL			m_bLocal;			// Is this a local fx (only done on this client?)

		DVector			m_vLightColor;		// Impact light color

		void CreateMark();
		void CreateSparks();
		void CreateSmoke();
		void CreateTracer();
		void CreateBulletTrail(DVector* pvStartPos);
		void TintScreen();

		void CreateWeaponSpecificFX();
		void CreateBeamFX();
		void CreateMuzzleFX();
		void CreateShell();
		void CreateMuzzleLight();
		void PlayImpactSound();
		void PlayFireSound();

		char* GetSparkTexture(RiotWeaponId nWeaponId, SurfaceType eSurfType,
							  DVector* pvColor1, DVector* pvColor2);
		void SetTracerValues(RiotWeaponId nWeaponId, TRCREATESTRUCT* pTR);
		void SetupSmoke(RiotWeaponId nWeaponId, SurfaceType eSurfType, SMCREATESTRUCT* pSM);
		void CreateLightFX();
		void CreateVectorBloodFX(DVector & vVelMin, DVector & vVelMax, DFLOAT fRange);
		void CreateLowVectorBloodFX(DVector & vVelMin, DVector & vVelMax, DFLOAT fRange);
		void CreateMedVectorBloodFX(DVector & vVelMin, DVector & vVelMax, DFLOAT fRange);
		void CreateBlastMark();

		DBOOL IsBulletTrailWeapon(RiotWeaponId nWeaponId);

		DBOOL DetermineDamageFX(DBOOL & bOnlyDoVectorDamageFX, 
								DBOOL & bDoVectorDamageFX, 
								DBOOL & bOnlyDoProjDamageFX,
								DBOOL & bCreateSparks);

		void CreatePulseRifleFX();
		void CreateSpiderFX();
		void CreateBullgutFX();
		void CreateSniperRifleFX();
		void CreateJuggernautFX();
		void CreateShredderFX();
		void CreateRedRiotFX();
		void CreateColt45FX();
		void CreateShotgunFX();
		void CreateAssaultRifleFX();
		void CreateEnergyGrenadeFX();
		void CreateKatoGrenadeFX();
		void CreateMac10FX();
		void CreateTOWFX();
		void CreateTantoFX();

		void CreateMedPulseRifleFX();
		void CreateMedSpiderFX();
		void CreateMedBullgutFX();
		void CreateMedSniperRifleFX();
		void CreateMedJuggernautFX();
		void CreateMedShredderFX();
		void CreateMedRedRiotFX();
		void CreateMedColt45FX();
		void CreateMedShotgunFX();
		void CreateMedAssaultRifleFX();
		void CreateMedEnergyGrenadeFX();
		void CreateMedKatoGrenadeFX();
		void CreateMedMac10FX();
		void CreateMedTOWFX();
		void CreateMedTantoFX();

		void CreateLowPulseRifleFX();
		void CreateLowSpiderFX();
		void CreateLowBullgutFX();
		void CreateLowSniperRifleFX();
		void CreateLowJuggernautFX();
		void CreateLowShredderFX();
		void CreateLowRedRiotFX();
		void CreateLowColt45FX();
		void CreateLowShotgunFX();
		void CreateLowAssaultRifleFX();
		void CreateLowEnergyGrenadeFX();
		void CreateLowKatoGrenadeFX();
		void CreateLowMac10FX();
		void CreateLowTOWFX();
		void CreateLowTantoFX();

		void CreateEnergyBatonFX()	{}
		void CreateEnergyBladeFX()	{}
		void CreateKatanaFX()		{}
		void CreateMonoKnifeFX()	{}
		void CreateLaserCannonFX()	{}

		void CreateRedRiotBeam();
		void CreateJuggernautBeam();
		void CreateShredderBeam();
		void CreateLaserCannonBeam();
};

#endif // __WEAPON_FX_H__
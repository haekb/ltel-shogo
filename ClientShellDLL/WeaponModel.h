// ----------------------------------------------------------------------- //
//
// MODULE  : WeaponModel.h
//
// PURPOSE : Generic client-side WeaponModel wrapper class - Definition
//
// CREATED : 9/27/97
//
// ----------------------------------------------------------------------- //

#ifndef __WEAPON_MODEL_H__
#define __WEAPON_MODEL_H__

#include "cpp_clientshell_de.h"
#include "WeaponDefs.h"
#include "SurfaceTypes.h"

class CWeaponModel
{
	public :

		CWeaponModel();
		virtual ~CWeaponModel();

		DBOOL Create(CClientDE* pClientDE, DBYTE nWeaponId);
		void ChangeWeapon(DBYTE nCommandId);
		void ChangeToPrevWeapon();	
		void ChangeToNextWeapon();	
		void ToggleVehicleMode();

		DBOOL IsDemoWeapon(DBYTE nCommandId);

		WeaponState UpdateWeaponModel(DRotation rRot, DVector vPos, DBOOL bFire);
		void HandleStateChange(HMESSAGEREAD hMessage);

		void Reset();
		HLOCALOBJ GetHandle() const { return m_hObject; }

		void UpdateBob(DFLOAT fWidth, DFLOAT fHeight);

		void SetVisible(DBOOL bVis=DTRUE);
		void SetZoom(DBOOL b) { m_bZoomView = b; }

		DVector GetFlashPos() const { return m_vFlashPos; }
		DVector GetModelPos() const;

		DVector GetOffset()			const { return m_vOffset; }
		void SetOffset(DVector v)	{ VEC_COPY(m_vOffset, v); }

		DVector GetMuzzleOffset()			const { return m_vMuzzleOffset; }
		void SetMuzzleOffset(DVector v)		{ VEC_COPY(m_vMuzzleOffset, v); }

		RiotWeaponId GetId() const { return m_nWeaponId; }

		void OnModelKey(HLOCALOBJ hObj, ArgList *pArgs);

	protected : 

		void CreateFlash();
		void CreateModel();
		void RemoveModel();
		void StartFlash();
		void UpdateFlash(WeaponState eState);
		
		WeaponState Fire();
		WeaponState UpdateModelState(DBOOL bFire);

		void	SendFireMsg();
		void	UpdateFiring();
		void	UpdateNonFiring();
		DBOOL	PlaySelectAnimation();
		DBOOL	PlayDeselectAnimation();
		DBOOL	PlayStartFireAnimation();
		DBOOL	PlayFireAnimation();
		DBOOL	PlayReloadAnimation();
		DBOOL	PlayStopFireAnimation();
		DBOOL	PlayIdleAnimation();
		void	InitAnimations();

		void	Deselect();
		void	Select();

		void	ClientFire(DVector & vPath, DVector & vFirePos);
		void	DoProjectile();
		void	DoVector();

		HLOCALOBJ CreateServerObj();

		void	AddImpact(HLOCALOBJ hObj, DVector & vInpactPoint, 
						  DVector & vNormal, SurfaceType eType);
		void	HandleVectorImpact(IntersectQuery & qInfo, IntersectInfo & iInfo);

		
		void	HandleInternalWeaponChange(DBYTE nWeaponId);
		void	DoWeaponChange(DBYTE nWeaponId);
		DBOOL	CanChangeToWeapon(DBYTE nCommandId);
		void	AutoSelectWeapon();
		DBYTE	PrevWeapon(DBYTE nPlayerMode);
		DBYTE	NextWeapon(DBYTE nPlayerMode);
		DBYTE	DemoPrevWeapon(DBYTE nPlayerMode);
		DBYTE	DemoNextWeapon(DBYTE nPlayerMode);


		CClientDE*	m_pClientDE;	// The clientDE
		HLOCALOBJ	m_hObject;		// Handle of WeaponModel model

		RiotWeaponId m_nWeaponId;

		DVector		m_vOffset;
		DVector		m_vMuzzleOffset;

		DVector		m_vFlashPos;
		HLOCALOBJ	m_hFlashObject;		// Muzzle flash object
		DFLOAT		m_fFlashStartTime;	// When did flash start

		// Bobbin' and Swayin' - Blood 2 style ;)
		DFLOAT		m_fBobHeight;
		DFLOAT		m_fBobWidth;

	
		
		DFLOAT		m_fLastIdleTime;
		DFLOAT		m_fTimeBetweenIdles;
		DBOOL		m_bFire;
		int			m_nAmmoInClip;
		WeaponState m_eState;			// What are we currently doing
		WeaponState	m_eLastWeaponState;

		HMODELANIM	m_nSelectAni;		// Select weapon
		HMODELANIM	m_nDeselectAni;		// Deselect weapon
		HMODELANIM	m_nStartFireAni;	// Start firing
		HMODELANIM	m_nFireAni;			// Fire
		HMODELANIM	m_nFireAni2;		// Fire2
		HMODELANIM	m_nFireZoomAni;		// Fire when zoomed in
		HMODELANIM	m_nLastFireAni;		// What fire ani was played last
		HMODELANIM	m_nStopFireAni;		// Stop firing
		HMODELANIM	m_nIdleAni1;		// Idle one
		HMODELANIM	m_nIdleAni2;		// Idle two
		HMODELANIM	m_nReloadAni;		// Reload weapon

		DVector			m_vPath;		// Path of current vector/projectile
		DVector			m_vFirePos;		// Fire position of current vector/projectile
		DVector			m_vEndPos;		// Impact location of current vector
		DBYTE			m_nIgnoreFX;	// FX to ignore for current vector/projectile
		ProjectileType	m_eType;		// Type of weapon
		DBOOL			m_bZoomView;	// Is the weapon zoomed?

		DBYTE			m_nRequestedWeaponId;	// Id of weapon to select
		DBOOL			m_bWeaponDeselected;	// Did we just deselect the weapon
};

#endif // __WEAPON_MODEL_H__
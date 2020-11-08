// ----------------------------------------------------------------------- //
//
// MODULE  : ProjectileFX.h
//
// PURPOSE : Projectile special fx class - Definition
//
// CREATED : 7/6/98
//
// ----------------------------------------------------------------------- //

#ifndef __PROJECTILE_FX_H__
#define __PROJECTILE_FX_H__

#include "SpecialFX.h"
#include "WeaponDefs.h"
#include "client_physics.h"


struct PROJECTILECREATESTRUCT : public SFXCREATESTRUCT
{
	PROJECTILECREATESTRUCT::PROJECTILECREATESTRUCT();

	DBYTE	nWeaponId;
	DBYTE	nShooterId;
	DBOOL	bLocal;
};

inline PROJECTILECREATESTRUCT::PROJECTILECREATESTRUCT()
{
	memset(this, 0, sizeof(PROJECTILECREATESTRUCT));
	nShooterId = -1;
}


class CProjectileFX : public CSpecialFX
{
	public :

		CProjectileFX() : CSpecialFX() 
		{
			m_nWeaponId		= GUN_NONE;
			m_eSize			= MS_NORMAL;
			m_nFX			= 0;

			m_pSmokeTrail	= DNULL;
			m_hFlare		= DNULL;
			m_hLight		= DNULL;
			m_hProjectile	= DNULL;
			m_hFlyingSound	= DNULL;

			m_nShooterId	= -1;
			m_bLocal		= DFALSE;

			m_bFirstSnake	= DTRUE;
			m_fSnakeUpVel	= 0.0f;
			m_fSnakeDir		= 1.0f;

			m_fStartTime	= 0.0f;
			m_bDetonated	= DFALSE;
		}

		~CProjectileFX()
		{
			RemoveFX();
		}

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update();

		void HandleTouch(CollisionInfo *pInfo, float forceMag);

	protected :
	
		RiotWeaponId	m_nWeaponId;		// Id of weapon fired
		ModelSize		m_eSize;			// Size of projectile
		DBYTE			m_nFX;				// FX associated with projectile
		DBYTE			m_nShooterId;		// Client Id of shooter
		DBOOL			m_bLocal;			// Did local client create this fx

		MovingObject	m_mover;			// Mover (m_bLocal only)

		void CreateSmokeTrail(DVector & vPos, DRotation & rRot);
		void CreateFlare(DVector & vPos, DRotation & rRot);
		void CreateLight(DVector & vPos, DRotation & rRot);
		void CreateProjectile(DVector & vPos, DRotation & rRot);
		void CreateFlyingSound(DVector & vPos, DRotation & rRot);
		
		void RemoveFX();

		DBOOL MoveServerObj();
		void  Detonate(CollisionInfo* pInfo);

		CSpecialFX*			m_pSmokeTrail;		// Smoke trail fx
		HLOCALOBJ			m_hFlare;			// Flare fx
		HLOCALOBJ			m_hLight;			// Light fx
		HLOCALOBJ			m_hProjectile;		// The Model/sprite
		HSOUNDDE			m_hFlyingSound;		// Sound of the projectile

		// Special anime snaking variables...

		DBOOL			m_bFirstSnake;
		DFLOAT			m_fSnakeUpVel;
		DFLOAT			m_fSnakeDir;

		DFLOAT			m_fStartTime;

		DVector			m_vFirePos;
		DVector			m_vPath;

		DBOOL			m_bDetonated;

};

#endif // __PROJECTILE_FX_H__
// ----------------------------------------------------------------------- //
//
// MODULE  : ClientWeaponUtils.cpp
//
// PURPOSE : Client-side firing helper functions
//
// CREATED : 11/2/98
//
// ----------------------------------------------------------------------- //

#include "ClientWeaponUtils.h"
#include "ClientUtilities.h"
#include "RiotClientShell.h"
#include "ClientServerShared.h"
#include "WeaponFX.h"

extern CRiotClientShell* g_pRiotClientShell;
extern CClientDE* g_pClientDE;


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	IsMoveable()
//
//	PURPOSE:	Determine if the passed in object is moveable
//
// ----------------------------------------------------------------------- //

DBOOL IsMoveable(HLOCALOBJ hObj)
{
	if (!g_pClientDE || !hObj) return DFALSE;

	DDWORD dwUserFlags;
	g_pClientDE->GetObjectUserFlags(hObj, &dwUserFlags);

	return !!(dwUserFlags & USRFLG_MOVEABLE);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GetSurfaceType()
//
//	PURPOSE:	Determine the type of surface associated with a poly
//
// ----------------------------------------------------------------------- //

SurfaceType GetSurfaceType(HPOLY hPoly)
{
	if (!g_pClientDE || !hPoly) return ST_UNKNOWN;

	SurfaceType eType = ST_UNKNOWN;

	if (hPoly != INVALID_HPOLY)
	{
		// Get the flags associated with the poly...

		DDWORD dwTextureFlags;
		g_pClientDE->GetPolyTextureFlags(hPoly, &dwTextureFlags);

		eType = (SurfaceType)dwTextureFlags;
	}
	
	return eType;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GetSurfaceType()
//
//	PURPOSE:	Determine the type of surface associated with an object
//
// ----------------------------------------------------------------------- //

SurfaceType GetSurfaceType(HLOCALOBJ hObj)
{
	if (!g_pClientDE || !hObj) return ST_UNKNOWN;

	SurfaceType eType = ST_UNKNOWN;

	D_WORD code;
	DDWORD dwUserFlags;
	g_pClientDE->GetObjectUserFlags(hObj, &dwUserFlags);

	if (dwUserFlags & USRFLG_CHARACTER)
	{
		eType = ST_FLESH;
	}
	else if (g_pClientDE->GetContainerCode(hObj, &code))
	{
		ContainerCode eCode = (ContainerCode)code;

		if (IsLiquid(eCode))
		{
			eType = ST_LIQUID;
		}
	}
	else 
	{
		eType = UserFlagToSurface(dwUserFlags);
	}

	return eType;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ObjListFilterFn()
//
//	PURPOSE:	Filter specific objects out of CastRay and/or 
//				IntersectSegment calls.
//
// ----------------------------------------------------------------------- //

DBOOL ObjListFilterFn(HLOCALOBJ hTest, void *pUserData)
{
	HOBJECT *hList;

	// Filters out objects for a raycast.  pUserData is a list of HOBJECTS terminated
	// with a NULL HOBJECT.
	hList = (HOBJECT*)pUserData;
	while(*hList)
	{
		if(hTest == *hList)
			return DFALSE;
		++hList;
	}
	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	SpecificObjectFilterFn()
//
//	PURPOSE:	Filter a specific object out of CastRay and/or 
//				IntersectSegment calls.
//
// ----------------------------------------------------------------------- //

DBOOL SpecificObjectFilterFn(HLOCALOBJ hObj, void *pUserData)
{
	if (!hObj) return DFALSE;

	return (hObj != (HLOCALOBJ)pUserData);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AttackerLiquidFilterFn()
//
//	PURPOSE:	Filter the attacker out of CastRay and/or 
//				IntersectSegment calls (so you don't shot yourself).
//				However, we want to ignore liquid as well...
//
// ----------------------------------------------------------------------- //

DBOOL AttackerLiquidFilterFn(HLOCALOBJ hObj, void *pUserData)
{
	// We're not attacking our self...

	if (SpecificObjectFilterFn(hObj, pUserData))
	{
		// Return DTRUE to keep this object (not liquid), or DFALSE to ignore
		// this object (is liquid)...

		D_WORD code;
		if (g_pClientDE && g_pClientDE->GetContainerCode(hObj, &code))
		{
			ContainerCode eCode = (ContainerCode)code;

			if (IsLiquid(eCode))
			{
				return DFALSE;
			}
		}

		return DTRUE;
	}

	return DFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AddLocalImpactFX
//
//	PURPOSE:	Add a weapon impact special fx
//
// ----------------------------------------------------------------------- //

void AddLocalImpactFX(HLOCALOBJ hObj, DVector & vFirePos, DVector & vImpactPoint, 
					  DVector & vNormal, SurfaceType eType, DVector & vPath, 
					  DBYTE nWeaponId, DBYTE nIgnoreFX)
{
	if (!g_pClientDE || !g_pRiotClientShell) return;

	CSFXMgr* psfxMgr = g_pRiotClientShell->GetSFXMgr();
	if (!psfxMgr) return;

	DVector vPos, vTemp;
	VEC_COPY(vPos, vImpactPoint);
	VEC_MULSCALAR(vTemp, vPath, -1.0f);
	VEC_ADD(vPos, vPos, vTemp);

	DRotation rRot;
	g_pClientDE->AlignRotation(&rRot, &vNormal, DNULL);

	DDWORD dwId;
	g_pClientDE->GetLocalClientID(&dwId);

	DBYTE nIgnFX = nIgnoreFX;

	if (IsMoveable(hObj))
	{
		nIgnFX |= WFX_MARK | WFX_SMOKE;	 
	}

	WCREATESTRUCT w;

	w.nWeaponId		= nWeaponId;
	w.nSurfaceType	= eType;
	w.nIgnoreFX 	= nIgnFX;
	w.nShooterId 	= (DBYTE)dwId;
	w.bLocal		= DTRUE;

	VEC_COPY(w.vFirePos, vFirePos);
	VEC_COPY(w.vPos, vPos);
	ROT_COPY(w.rRot, rRot);

	psfxMgr->CreateSFX(SFX_WEAPON_ID, &w);
}



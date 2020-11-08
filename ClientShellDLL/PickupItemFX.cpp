// ----------------------------------------------------------------------- //
//
// MODULE  : PickupItemFX.cpp
//
// PURPOSE : PickupItem - Implementation
//
// CREATED : 8/20/98
//
// ----------------------------------------------------------------------- //

#include "PickupItemFX.h"
#include "cpp_client_de.h"
#include "ClientUtilities.h"
#include "ClientServerShared.h"
#include "RiotClientShell.h"
#include "SFXMsgIds.h"

extern CRiotClientShell* g_pRiotClientShell;

#define PICKUPITEM_ROTVEL	0.3333f * MATH_CIRCLE

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPickupItemFX::Init
//
//	PURPOSE:	Init the fx
//
// ----------------------------------------------------------------------- //

DBOOL CPickupItemFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CSpecialFX::Init(psfxCreateStruct)) return DFALSE;

	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPickupItemFX::CreateObject
//
//	PURPOSE:	Create object associated the fx
//
// ----------------------------------------------------------------------- //

DBOOL CPickupItemFX::CreateObject(CClientDE *pClientDE)
{
	DBOOL bRet = CSpecialFX::CreateObject(pClientDE);
	if (!bRet) return bRet;

	return bRet;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPickupItemFX::Update
//
//	PURPOSE:	Update the pickupitem
//
// ----------------------------------------------------------------------- //

DBOOL CPickupItemFX::Update()
{
	if (!m_pClientDE || m_bWantRemove || !m_hServerObject) return DFALSE;

	DDWORD dwUsrFlags;
	m_pClientDE->GetObjectUserFlags(m_hServerObject, &dwUsrFlags);

	DFLOAT fDeltaTime = m_pClientDE->GetFrameTime();

	if (dwUsrFlags & USRFLG_PICKUP_ROTATE)
	{
		DRotation rRot;
		m_pClientDE->GetObjectRotation(m_hServerObject, &rRot);
		m_pClientDE->EulerRotateY(&rRot, PICKUPITEM_ROTVEL * fDeltaTime);
		m_pClientDE->SetObjectRotation(m_hServerObject, &rRot);
	}

	if (dwUsrFlags & USRFLG_PICKUP_BOUNCE)
	{

	}


	if (dwUsrFlags & USRFLG_PICKUP_RESPAWN)
	{

	}

	return DTRUE;
}
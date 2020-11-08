// ----------------------------------------------------------------------- //
//
// MODULE  : MarkSFX.cpp
//
// PURPOSE : Mark special FX - Implementation
//
// CREATED : 10/13/97
//
// ----------------------------------------------------------------------- //

#include "MarkSFX.h"
#include "cpp_client_de.h"
#include "dlink.h"
#include "RiotClientShell.h"


extern CRiotClientShell* g_pRiotClientShell;

#define REGION_DIAMETER			100.0f  // Squared distance actually
#define MAX_MARKS_IN_REGION		10

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMarkSFX::Init
//
//	PURPOSE:	Create the mark
//
// ----------------------------------------------------------------------- //

DBOOL CMarkSFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!psfxCreateStruct) return DFALSE;

	CSpecialFX::Init(psfxCreateStruct);

	MARKCREATESTRUCT* pMark = (MARKCREATESTRUCT*)psfxCreateStruct;

	ROT_COPY(m_Rotation, pMark->m_Rotation);
	VEC_COPY(m_vPos, pMark->m_vPos);
	m_fScale	 = pMark->m_fScale;
	m_hstrSprite = pMark->m_hstrSprite;

	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMarkSFX::CreateObject
//
//	PURPOSE:	Create object associated with the mark
//
// ----------------------------------------------------------------------- //

DBOOL CMarkSFX::CreateObject(CClientDE *pClientDE)
{
	DVector vScale;

	if (!CSpecialFX::CreateObject(pClientDE) || 
		!g_pRiotClientShell || !m_hstrSprite) return DFALSE;

	CSFXMgr* psfxMgr = g_pRiotClientShell->GetSFXMgr();
	if (!psfxMgr) return DFALSE;


	// Before we create a new buillet hole see if there is already another
	// bullet hole close by that we could use instead...

	CSpecialFXList* pList = psfxMgr->GetBulletHoleList();
	if (!pList) return DFALSE;

	int nNumBulletHoles = pList->GetSize();

	HOBJECT hMoveObj		 = DNULL;
	HOBJECT hObj			 = DNULL;
	DFLOAT	fClosestMarkDist = REGION_DIAMETER;
	DBYTE	nNumInRegion	 = 0;

	DVector vPos;

	for (int i=0; i < nNumBulletHoles; i++)
	{
		if ((*pList)[i])
		{
			hObj = (*pList)[i]->GetObject();
			if (hObj)
			{
				pClientDE->GetObjectPos(hObj, &vPos);
				
				DFLOAT fDist = VEC_DISTSQR(vPos, m_vPos);
				if (fDist < REGION_DIAMETER)
				{
					if (fDist < fClosestMarkDist)
					{
						fClosestMarkDist = fDist;
						hMoveObj = hObj;
					}

					if (++nNumInRegion > MAX_MARKS_IN_REGION)
					{
						// Just move this bullet-hole to the correct pos, and
						// remove thyself...

						pClientDE->SetObjectPos(hMoveObj, &m_vPos);
						return DFALSE;
					}
				}
			}
		}
	}


	// Setup the mark...

	ObjectCreateStruct createStruct;
	INIT_OBJECTCREATESTRUCT(createStruct);

	createStruct.m_ObjectType = OT_SPRITE;
	SAFE_STRCPY( createStruct.m_Filename, m_pClientDE->GetStringData( m_hstrSprite ));
	createStruct.m_Flags = FLAG_VISIBLE | FLAG_NOLIGHT | FLAG_ROTATEABLESPRITE; //  | FLAG_SPRITECHROMAKEY;
	VEC_COPY(createStruct.m_Pos, m_vPos);
	ROT_COPY(createStruct.m_Rotation, m_Rotation);

	m_hObject = pClientDE->CreateObject(&createStruct);

	VEC_SET( vScale, m_fScale, m_fScale, m_fScale );
	m_pClientDE->SetObjectScale(m_hObject, &vScale);

	m_pClientDE->FreeString(m_hstrSprite);

	return DTRUE;
}

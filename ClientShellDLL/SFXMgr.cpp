// ----------------------------------------------------------------------- //
//
// MODULE  : CSFXMgr.cpp
//
// PURPOSE : Special FX Mgr	- Implementation
//
// CREATED : 10/24/97
//
// ----------------------------------------------------------------------- //

#include "SFXMgr.h"
#include "SpecialFX.h"
#include "PolyGridFX.h"
#include "ParticleTrailFX.h"
#include "ParticleSystemFX.h"
#include "MarkSFX.h"
#include "CastLineFX.h"
#include "SparksFX.h"
#include "TracerFX.h"
#include "WeaponFX.h"
#include "DynamicLightFX.h"
#include "ParticleTrailSegmentFX.h"
#include "SmokeFX.h"
#include "BulletTrailFX.h"
#include "VolumeBrushFX.h"
#include "ShellCasingFX.h"
#include "RiotCommonUtilities.h"
#include "WeaponFXTypes.h"
#include "CameraFX.h"
#include "ParticleExplosionFX.h"
#include "SpriteFX.h"
#include "ExplosionFX.h"
#include "DebrisFX.h"
#include "DeathFX.h"
#include "GibFX.h"
#include "ProjectileFX.h"
#include "LightFX.h"
#include "PickupItemFX.h"
#include "CriticalHitFX.h"
#include "RiotClientShell.h"
#include "PlayerFX.h"
#include "LineBallFX.h"
#include "AnimeLineFX.h"
#include "WeaponSoundFX.h"
#include "SFXReg.h"


extern CRiotClientShell* g_pRiotClientShell;

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::Init()
//
//	PURPOSE:	Init the CSFXMgr
//
// ----------------------------------------------------------------------- //

DBOOL CSFXMgr::Init(CClientDE *pClientDE)
{
	if (!pClientDE) return DFALSE;

	m_pClientDE = pClientDE;

	DBOOL bRet = DTRUE;

	for (int i=0; i < DYN_ARRAY_SIZE; i++)
	{
		bRet = m_dynSFXLists[i].Create(GetDynArrayMaxNum(i));
		if (!bRet) return DFALSE;
	}

	bRet = m_cameraSFXList.Create(CAMERA_LIST_SIZE);

	return bRet;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::HandleSFXMsg()
//
//	PURPOSE:	Handle a special fx message
//
// ----------------------------------------------------------------------- //

void CSFXMgr::HandleSFXMsg(HLOCALOBJ hObj, HMESSAGEREAD hMessage)
{
	D_WORD wColor;

	if (!m_pClientDE) return;

	DBYTE nId = m_pClientDE->ReadFromMessageByte(hMessage);

	switch(nId)
	{
		case SFX_WEAPON_ID :
		{
			WCREATESTRUCT w;

			w.hServerObj	= hObj;
			w.nWeaponId		= m_pClientDE->ReadFromMessageByte(hMessage);
			w.nSurfaceType	= m_pClientDE->ReadFromMessageByte(hMessage);
			w.nIgnoreFX 	= m_pClientDE->ReadFromMessageByte(hMessage);
			w.nShooterId 	= m_pClientDE->ReadFromMessageByte(hMessage);
			m_pClientDE->ReadFromMessageCompPosition(hMessage, &(w.vFirePos));
			m_pClientDE->ReadFromMessageCompPosition(hMessage, &(w.vPos));
			m_pClientDE->ReadFromMessageRotation(hMessage, &(w.rRot));

			CreateSFX(nId, &w);
		}
		break;

		case SFX_WEAPONSOUND_ID :
		{
			WSOUNDCREATESTRUCT w;

			w.hServerObj	= hObj;
			w.nType			= m_pClientDE->ReadFromMessageByte(hMessage);
			w.nWeaponId		= m_pClientDE->ReadFromMessageByte(hMessage);
			m_pClientDE->ReadFromMessageCompPosition(hMessage, &(w.vPos));
			w.hSound		= m_pClientDE->ReadFromMessageHString(hMessage);
			w.nClientId		= m_pClientDE->ReadFromMessageByte(hMessage);

			CreateSFX(nId, &w);
		}
		break;

		case SFX_PROJECTILE_ID :
		{
			PROJECTILECREATESTRUCT projectile;

			projectile.hServerObj = hObj;
			projectile.nWeaponId  = m_pClientDE->ReadFromMessageByte(hMessage);
			projectile.nShooterId = m_pClientDE->ReadFromMessageByte(hMessage);

			CreateSFX(nId, &projectile);
		}
		break;

		case SFX_DEBRIS_ID :
		{
			DEBRISCREATESTRUCT debris;

			debris.hServerObj	= hObj;
			m_pClientDE->ReadFromMessageRotation(hMessage, &(debris.rRot));
			m_pClientDE->ReadFromMessageCompPosition(hMessage, &(debris.vPos));
			m_pClientDE->ReadFromMessageCompVector(hMessage, &(debris.vMinVel));
			m_pClientDE->ReadFromMessageCompVector(hMessage, &(debris.vMaxVel));
			debris.fLifeTime	= m_pClientDE->ReadFromMessageFloat(hMessage);
			debris.fFadeTime	= m_pClientDE->ReadFromMessageFloat(hMessage);
			debris.fMinScale	= m_pClientDE->ReadFromMessageFloat(hMessage);
			debris.fMaxScale	= m_pClientDE->ReadFromMessageFloat(hMessage);
			debris.nNumDebris	= m_pClientDE->ReadFromMessageByte(hMessage);
			debris.nDebrisFlags = m_pClientDE->ReadFromMessageByte(hMessage);
			debris.nDebrisType  = m_pClientDE->ReadFromMessageByte(hMessage);
			debris.bRotate		= (DBOOL)m_pClientDE->ReadFromMessageByte(hMessage);
			debris.bPlayBounceSound = (DBOOL)m_pClientDE->ReadFromMessageByte(hMessage);
			debris.bPlayExplodeSound = (DBOOL)m_pClientDE->ReadFromMessageByte(hMessage);
			
			CreateSFX(nId, &debris);
		}
		break;

		case SFX_DEATH_ID :
		{
			DEATHCREATESTRUCT d;

			d.hServerObj		= hObj;
			d.nModelId			= m_pClientDE->ReadFromMessageByte(hMessage);
			d.nSize				= m_pClientDE->ReadFromMessageByte(hMessage);
			d.nDeathType		= m_pClientDE->ReadFromMessageByte(hMessage);
			d.nCharacterClass	= m_pClientDE->ReadFromMessageByte(hMessage);
			m_pClientDE->ReadFromMessageVector(hMessage, &(d.vPos));
			m_pClientDE->ReadFromMessageVector(hMessage, &(d.vDir));

			CreateSFX(nId, &d);
		}
		break;

		case SFX_VOLUMEBRUSH_ID :
		{
			VBCREATESTRUCT vb;

			vb.hServerObj	= hObj;
			vb.bFogEnable	= (DBOOL)m_pClientDE->ReadFromMessageByte(hMessage);
			vb.fFogFarZ		= m_pClientDE->ReadFromMessageFloat(hMessage);
			vb.fFogNearZ	= m_pClientDE->ReadFromMessageFloat(hMessage);
			wColor = m_pClientDE->ReadFromMessageWord(hMessage);
			Color255WordToVector( wColor, &vb.vFogColor);

			CreateSFX(nId, &vb);
		}
		break;

		case SFX_CAMERA_ID :
		{
			CAMCREATESTRUCT cam;

			cam.hServerObj			 = hObj;
			cam.bAllowPlayerMovement = (DBOOL) m_pClientDE->ReadFromMessageByte(hMessage);
			cam.nCameraType			 = m_pClientDE->ReadFromMessageByte(hMessage);
			cam.bIsListener			 = (DBOOL) m_pClientDE->ReadFromMessageByte(hMessage);

			CreateSFX(nId, &cam);
		}
		break;

		case SFX_TRACER_ID :
		{
			TRCREATESTRUCT tr;

			tr.hServerObj = hObj;
			m_pClientDE->ReadFromMessageVector(hMessage, &(tr.vVel));
			m_pClientDE->ReadFromMessageVector(hMessage, &(tr.vStartColor));
			m_pClientDE->ReadFromMessageVector(hMessage, &(tr.vEndColor));
			m_pClientDE->ReadFromMessageVector(hMessage, &(tr.vStartPos));
			tr.fStartAlpha	= m_pClientDE->ReadFromMessageFloat(hMessage);
			tr.fEndAlpha	= m_pClientDE->ReadFromMessageFloat(hMessage);

			CreateSFX(nId, &tr);
		}
		break;

		case SFX_SPARKS_ID :
		{
			SCREATESTRUCT sp;

			sp.hServerObj = hObj;
			m_pClientDE->ReadFromMessageCompPosition(hMessage, &(sp.vPos));
			m_pClientDE->ReadFromMessageVector(hMessage, &(sp.vDir));
			wColor = m_pClientDE->ReadFromMessageWord(hMessage);
			Color255WordToVector( wColor, &sp.vColor1 );
			wColor = m_pClientDE->ReadFromMessageWord(hMessage);
			Color255WordToVector( wColor, &sp.vColor2 );
			sp.hstrTexture		= m_pClientDE->ReadFromMessageHString(hMessage);
			sp.nSparks			= m_pClientDE->ReadFromMessageByte(hMessage);
			sp.fDuration		= m_pClientDE->ReadFromMessageWord(hMessage);
			sp.fEmissionRadius	= m_pClientDE->ReadFromMessageFloat(hMessage);
			sp.fRadius			= ( DFLOAT )m_pClientDE->ReadFromMessageWord(hMessage);

			CreateSFX(nId, &sp);
		}
		break;

		case SFX_CASTLINE_ID :
		{
			CLCREATESTRUCT cl;

			cl.hServerObj = hObj;
			m_pClientDE->ReadFromMessageVector(hMessage, &(cl.vStartColor));
			m_pClientDE->ReadFromMessageVector(hMessage, &(cl.vEndColor));
			cl.fStartAlpha	= m_pClientDE->ReadFromMessageFloat(hMessage);
			cl.fEndAlpha	= m_pClientDE->ReadFromMessageFloat(hMessage);

			CreateSFX(nId, &cl);
		}
		break;

		case SFX_POLYGRID_ID :
		{
			PGCREATESTRUCT pg;

			pg.hServerObj = hObj;
			m_pClientDE->ReadFromMessageVector(hMessage, &(pg.vDims));
			wColor = m_pClientDE->ReadFromMessageWord(hMessage);
			Color255WordToVector(wColor, &(pg.vColor1));
			wColor = m_pClientDE->ReadFromMessageWord(hMessage);
			Color255WordToVector(wColor, &(pg.vColor2));
			pg.fXScaleMin = m_pClientDE->ReadFromMessageFloat(hMessage); 
			pg.fXScaleMax = m_pClientDE->ReadFromMessageFloat(hMessage); 
			pg.fYScaleMin = m_pClientDE->ReadFromMessageFloat(hMessage); 
			pg.fYScaleMax = m_pClientDE->ReadFromMessageFloat(hMessage); 
			pg.fXScaleDuration = m_pClientDE->ReadFromMessageFloat(hMessage);
			pg.fYScaleDuration = m_pClientDE->ReadFromMessageFloat(hMessage);
			pg.fXPan = m_pClientDE->ReadFromMessageFloat(hMessage);
			pg.fYPan = m_pClientDE->ReadFromMessageFloat(hMessage);
			pg.fAlpha = m_pClientDE->ReadFromMessageFloat(hMessage);
			pg.hstrSurfaceSprite = m_pClientDE->ReadFromMessageHString(hMessage);
			pg.dwNumPolies = (DDWORD)m_pClientDE->ReadFromMessageWord(hMessage);
			pg.nPlasmaType = m_pClientDE->ReadFromMessageByte(hMessage);
			pg.nRingRate[0] = m_pClientDE->ReadFromMessageByte(hMessage);
			pg.nRingRate[1] = m_pClientDE->ReadFromMessageByte(hMessage);
			pg.nRingRate[2] = m_pClientDE->ReadFromMessageByte(hMessage);
			pg.nRingRate[3] = m_pClientDE->ReadFromMessageByte(hMessage);

			CreateSFX(nId, &pg);
		}
		break;

		case SFX_PARTICLETRAIL_ID :
		{
			PTCREATESTRUCT pt;

			pt.hServerObj = hObj;
			pt.nType  = m_pClientDE->ReadFromMessageByte(hMessage);
			pt.bSmall = ( pt.nType & PT_SMALL ) ? DTRUE : DFALSE;
			pt.nType &= ~PT_SMALL;

			CreateSFX(nId, &pt);
		}
		break;
		
		case SFX_PARTICLESYSTEM_ID :
		{
			PSCREATESTRUCT ps;

			ps.hServerObj = hObj;
			m_pClientDE->ReadFromMessageVector(hMessage, &(ps.vColor1));
			m_pClientDE->ReadFromMessageVector(hMessage, &(ps.vColor2));
			ps.dwFlags			   = (DDWORD) m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fBurstWait		   = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fParticlesPerSecond = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fEmissionRadius	   = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fMinimumVelocity	   = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fMaximumVelocity    = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fVelocityOffset	   = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fParticleLifetime   = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fParticleRadius	   = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fGravity			   = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.fRotationVelocity   = m_pClientDE->ReadFromMessageFloat(hMessage);
			ps.hstrTextureName	   = m_pClientDE->ReadFromMessageHString(hMessage);

			CreateSFX(nId, &ps);
		}
		break;

		case SFX_MARK_ID :
		{
			MARKCREATESTRUCT mark;

			mark.hServerObj = hObj;
			m_pClientDE->ReadFromMessageRotation(hMessage, &(mark.m_Rotation));
			mark.m_fScale = m_pClientDE->ReadFromMessageFloat(hMessage);
			mark.m_hstrSprite = m_pClientDE->ReadFromMessageHString( hMessage );

			CreateSFX(nId, &mark);
		}
		break;

		case SFX_CRITICALHIT_ID :
		{
			CHCREATESTRUCT ch;

			m_pClientDE->ReadFromMessageCompPosition(hMessage, &(ch.vPos));
			ch.fClientIDHitter = m_pClientDE->ReadFromMessageFloat(hMessage);
			ch.fClientIDHittee = m_pClientDE->ReadFromMessageFloat(hMessage);

			CreateSFX(nId, &ch);
		}
		break;

		case SFX_LIGHT_ID :
		{
			LIGHTCREATESTRUCT light;

			light.hServerObj	= hObj;

			m_pClientDE->ReadFromMessageVector(hMessage, &light.vColor);
			light.dwLightFlags = m_pClientDE->ReadFromMessageDWord(hMessage);
			light.fIntensityMin = m_pClientDE->ReadFromMessageFloat(hMessage);
			light.fIntensityMax = m_pClientDE->ReadFromMessageFloat(hMessage);
			light.nIntensityWaveform = m_pClientDE->ReadFromMessageByte(hMessage);
			light.fIntensityFreq = m_pClientDE->ReadFromMessageFloat(hMessage);
			light.fIntensityPhase = m_pClientDE->ReadFromMessageFloat(hMessage);
			light.fRadiusMin = m_pClientDE->ReadFromMessageFloat(hMessage);
			light.fRadiusMax = m_pClientDE->ReadFromMessageFloat(hMessage);
			light.nRadiusWaveform = m_pClientDE->ReadFromMessageByte(hMessage);
			light.fRadiusFreq = m_pClientDE->ReadFromMessageFloat(hMessage);
			light.fRadiusPhase = m_pClientDE->ReadFromMessageFloat(hMessage);
			light.hstrRampUpSound = m_pClientDE->ReadFromMessageHString(hMessage);
			light.hstrRampDownSound = m_pClientDE->ReadFromMessageHString(hMessage);

			CreateSFX(nId, &light);
		}
		break;

		case SFX_PICKUPITEM_ID :
		{
			PICKUPITEMCREATESTRUCT pickupitem;

			pickupitem.hServerObj	= hObj;
			CreateSFX(nId, &pickupitem);
		}
		break;

		case SFX_PLAYER_ID :
		{
			PLAYERCREATESTRUCT player;

			player.hServerObj = hObj;
			CreateSFX(nId, &player);
		}
		break;

		case SFX_ANIMELINES_ID:
		{
			ALCREATESTRUCT cs;
			DBYTE theAngle;
			float fAngle;
			DRotation tempRot;
			DVector up, right;

			m_pClientDE->ReadFromMessageCompPosition(hMessage, &cs.m_Pos);
			
			theAngle = m_pClientDE->ReadFromMessageByte(hMessage);
			fAngle = ((float)theAngle / 255.0f) * MATH_CIRCLE;
			m_pClientDE->SetupEuler(&tempRot, 0.0f, fAngle, 0.0f);
			m_pClientDE->GetRotationVectors(&tempRot, &up, &right, &cs.m_DirVec);
			CreateSFX(nId, &cs);
		}
		break;

		case SFX_AUTO_ID:
		{
			CreateAutoSFX(hObj, hMessage);
		}
		break;

		default : break;
	}
}


CSpecialFX* CSFXMgr::CreateAutoSFX(HOBJECT hServerObj, HMESSAGEREAD hMessage)
{
	SFXReg *pReg;
	CAutoSpecialFX *pAuto;
	DBYTE sfxID;


	sfxID = m_pClientDE->ReadFromMessageByte(hMessage);

	if(pReg = FindSFXReg(sfxID))
	{
		if(pAuto = pReg->m_Fn())
		{
			if(pAuto->InitAuto(hServerObj, hMessage))
			{
				if (!m_pClientDE->IsConnected())
				{
					delete pAuto;
					return DNULL;
				}

				if(AddDynamicSpecialFX(pAuto, SFX_AUTO_ID))
					return pAuto;
			}
		}
	}
	
	return NULL;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::CreateSFX()
//
//	PURPOSE:	Create the special fx
//
// ----------------------------------------------------------------------- //

CSpecialFX* CSFXMgr::CreateSFX(DBYTE nId, SFXCREATESTRUCT *psfxCreateStruct)
{
	if (!m_pClientDE) return DNULL;

	CSpecialFX* pSFX = DNULL;

	switch(nId)
	{
		case SFX_WEAPON_ID :
		{
			pSFX = new CWeaponFX();
		}
		break;

		case SFX_WEAPONSOUND_ID :
		{
			pSFX = new CWeaponSoundFX();
		}
		break;

		case SFX_PROJECTILE_ID :
		{
			pSFX = new CProjectileFX();
		}
		break;

		case SFX_SPRITE_ID :
		{
			pSFX = new CSpriteFX();
		}
		break;

		case SFX_TRACER_ID :
		{
			pSFX = new CTracerFX();
		}
		break;

		case SFX_EXPLOSION_ID :
		{
			pSFX = new CExplosionFX();
		}
		break;

		case SFX_ANIMELINES_ID:
		{
			pSFX = new CAnimeLineFX();
		}
		break;

		case SFX_LINEBALL_ID :
		{
			pSFX = new CLineBallFX();
		}
		break;

		case SFX_PARTICLETRAIL_ID :
		{
			pSFX = new CParticleTrailFX();
		}
		break;

		case SFX_SPARKS_ID :
		{
			pSFX = new CSparksFX();
		}
		break;

		case SFX_PARTICLETRAILSEG_ID :
		{
			pSFX = new CParticleTrailSegmentFX();
		}
		break;

		case SFX_BULLETTRAIL_ID :
		{
			pSFX = new CBulletTrailFX();
		}
		break;

		case SFX_MARK_ID :
		{
			pSFX = new CMarkSFX();
		}
		break;

		case SFX_SHELLCASING_ID :
		{
			pSFX = new CShellCasingFX();
		}
		break;

		case SFX_PARTICLEEXPLOSION_ID :
		{
			pSFX = new CParticleExplosionFX();
		}
		break;

		case SFX_DEBRIS_ID :
		{
			pSFX = new CDebrisFX();
		}
		break;

		case SFX_CRITICALHIT_ID :
		{
			pSFX = new CCriticalHitFX();
		}
		break;

		case SFX_GIB_ID :
		{
			pSFX = new CGibFX();
		}
		break;

		case SFX_DYNAMICLIGHT_ID :
		{
			pSFX = new CDynamicLightFX();
		}
		break;

		case SFX_SMOKE_ID :
		{
			pSFX = new CSmokeFX();
		}
		break;
		
		case SFX_DEATH_ID :
		{
			pSFX = new CDeathFX();
		}
		break;

		case SFX_CAMERA_ID :
		{
			pSFX = new CCameraFX();
			if (pSFX)
			{
				if (pSFX->Init(psfxCreateStruct))
				{
					if (m_pClientDE->IsConnected())
					{
						if (pSFX->CreateObject(m_pClientDE))
						{
							m_cameraSFXList.Add(pSFX);
						}
						else
						{
							delete pSFX;
							pSFX = DNULL;
						}
					}
					else
					{
						delete pSFX;
						pSFX = DNULL;
					}
				}
			}

			return pSFX;
		}
		break;

		case SFX_CASTLINE_ID :
		{
			pSFX = new CCastLineFX();
		}
		break;

		case SFX_POLYGRID_ID :
		{
			pSFX = new CPolyGridFX();
		}
		break;
		
		case SFX_VOLUMEBRUSH_ID :
		{
			pSFX = new CVolumeBrushFX();
		}
		break;

		case SFX_PARTICLESYSTEM_ID :
		{
			pSFX = new CParticleSystemFX();
		}
		break;

		case SFX_LIGHT_ID :
		{
			pSFX = new CLightFX();
		}
		break;

		default : return DNULL;

		case SFX_PICKUPITEM_ID :
		{
			pSFX = new CPickupItemFX();
		}
		break;

		case SFX_PLAYER_ID :
		{
			pSFX = new CPlayerFX();
		}
		break;
	}


	// Initialize the sfx, and add it to the appropriate array...

	if (!pSFX) return DNULL;


	if (!pSFX->Init(psfxCreateStruct))
	{
		delete pSFX;
		return DNULL;
	}

	// Only create the sfx if we are connected to the server...

	if (!m_pClientDE->IsConnected())
	{
		delete pSFX;
		return DNULL;
	}

	if (!pSFX->CreateObject(m_pClientDE))
	{
		delete pSFX;
		return DNULL;
	}

	AddDynamicSpecialFX(pSFX, nId);

	return pSFX;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::UpdateSpecialFX()
//
//	PURPOSE:	Update any the special FX
//
// ----------------------------------------------------------------------- //

void CSFXMgr::UpdateSpecialFX()
{
	if (!m_pClientDE) return;

	DFLOAT fTime = m_pClientDE->GetTime();

	// Update dynamic sfx...

	UpdateDynamicSpecialFX();


	// Update camera sfx...

	int nNumSFX = m_cameraSFXList.GetSize();
	
	for (int i=0; i < nNumSFX; i++)
	{
		if (m_cameraSFXList[i]) 
		{
			if (fTime >= m_cameraSFXList[i]->m_fNextUpdateTime)
			{
				if (!m_cameraSFXList[i]->Update())
				{
					m_cameraSFXList.Remove(m_cameraSFXList[i]);
				}
				else
				{
					m_cameraSFXList[i]->m_fNextUpdateTime = fTime + m_cameraSFXList[i]->GetUpdateDelta();
				}
			}
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::RemoveSpecialFX()
//
//	PURPOSE:	Remove the specified special fx
//
// ----------------------------------------------------------------------- //

void CSFXMgr::RemoveSpecialFX(HLOCALOBJ hObj)
{
	if (!m_pClientDE) return;

	// Remove the dynamic special fx associated with this object..

	RemoveDynamicSpecialFX(hObj);


	// See if this was a camera...

	int nNumSFX = m_cameraSFXList.GetSize();

	for (int i=0; i < nNumSFX; i++)
	{
		if (m_cameraSFXList[i] && m_cameraSFXList[i]->GetServerObj() == hObj)
		{
			m_cameraSFXList[i]->WantRemove();
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::RemoveAll()
//
//	PURPOSE:	Remove all the special fx
//
// ----------------------------------------------------------------------- //

void CSFXMgr::RemoveAll()
{
	RemoveAllDynamicSpecialFX();

	int nNumSFX = m_cameraSFXList.GetSize();

	for (int i=0; i < nNumSFX; i++)
	{
		m_cameraSFXList.Remove(m_cameraSFXList[i]);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::AddDynamicSpecialFX()
//
//	PURPOSE:	Add a dyanamic special fx to our lists
//
// ----------------------------------------------------------------------- //

DBOOL CSFXMgr::AddDynamicSpecialFX(CSpecialFX* pSFX, DBYTE nId)
{
	int nIndex = GetDynArrayIndex(nId);

	if (0 <= nIndex && nIndex < DYN_ARRAY_SIZE)
	{
		m_dynSFXLists[nIndex].Add(pSFX);
		return DTRUE;
	}

	return DFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::UpdateDynamicSpecialFX()
//
//	PURPOSE:	Update the dyanamic special fxs
//
// ----------------------------------------------------------------------- //

void CSFXMgr::UpdateDynamicSpecialFX()
{
	if (!m_pClientDE) return;

	DFLOAT fTime = m_pClientDE->GetTime();

	for (int j=0; j < DYN_ARRAY_SIZE; j++)
	{
		int nNumSFX  = m_dynSFXLists[j].GetSize();

		for (int i=0; i < nNumSFX; i++)
		{
			if (m_dynSFXLists[j][i]) 
			{
				if (fTime >= m_dynSFXLists[j][i]->m_fNextUpdateTime)
				{
					if (!m_dynSFXLists[j][i]->Update())
					{
						m_dynSFXLists[j].Remove(m_dynSFXLists[j][i]);
					}
					else
					{
						m_dynSFXLists[j][i]->m_fNextUpdateTime = fTime + m_dynSFXLists[j][i]->GetUpdateDelta();
					}
				}
			}
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::RemoveDynamicSpecialFX()
//
//	PURPOSE:	Remove the specified special fx
//
// ----------------------------------------------------------------------- //

void CSFXMgr::RemoveDynamicSpecialFX(HOBJECT hObj)
{
	for (int j=0; j < DYN_ARRAY_SIZE; j++)
	{
		int nNumSFX  = m_dynSFXLists[j].GetSize();

		for (int i=0; i < nNumSFX; i++)
		{
			// More than one sfx may have the same server handle, so let them
			// all have an opportunity to remove themselves...

			if (m_dynSFXLists[j][i] && m_dynSFXLists[j][i]->GetServerObj() == hObj)
			{
				m_dynSFXLists[j][i]->WantRemove();
			}
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::RemoveAllDynamicSpecialFX()
//
//	PURPOSE:	Remove all dynamic special fx
//
// ----------------------------------------------------------------------- //

void CSFXMgr::RemoveAllDynamicSpecialFX()
{
	for (int j=0; j < DYN_ARRAY_SIZE; j++)
	{
		int nNumSFX  = m_dynSFXLists[j].GetSize();

		for (int i=0; i < nNumSFX; i++)
		{
			m_dynSFXLists[j].Remove(m_dynSFXLists[j][i]);
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::FindSpecialFX()
//
//	PURPOSE:	Find the specified special fx type associated with the 
//				object (see SFXMsgIds.h for valid types)
//
// ----------------------------------------------------------------------- //

CSpecialFX* CSFXMgr::FindSpecialFX(DBYTE nType, HLOCALOBJ hObj)
{
	if (0 <= nType && nType < DYN_ARRAY_SIZE)
	{
		int nNumSFX  = m_dynSFXLists[nType].GetSize();

		for (int i=0; i < nNumSFX; i++)
		{
			if (m_dynSFXLists[nType][i] && m_dynSFXLists[nType][i]->GetServerObj() == hObj)
			{
				return m_dynSFXLists[nType][i];
			}
		}
	}

	return DNULL;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::GetDynArrayIndex()
//
//	PURPOSE:	Get the array index associated with a particular type of
//				dynamic special fx
//
// ----------------------------------------------------------------------- //

int	CSFXMgr::GetDynArrayIndex(DBYTE nFXId)
{
	// All valid fxids should map directly to the array index...If this is
	// an invalid id, use the general fx index (i.e., 0)...

	if (nFXId < 0 || nFXId >= DYN_ARRAY_SIZE)
	{
		return 0;
	}

	return nFXId;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::GetDynArrayMaxNum()
//
//	PURPOSE:	Find a dyanamic special fx associated with an object
//
// ----------------------------------------------------------------------- //

unsigned int CSFXMgr::GetDynArrayMaxNum(DBYTE nIndex)
{
	if (0 <= nIndex && nIndex < DYN_ARRAY_SIZE)
	{
		// NOTE:  The indexes should map EXACTLY to the SFX ids defined
		// in SFXMsgIds.h...

		unsigned int s_nDynArrayMaxNums[DYN_ARRAY_SIZE] =
		{
			100,	// General fx
			10,		// Polygrid
			20,		// Particle trails
			20,		// Particle systems
			5,		// Cast line
			20,		// Sparks
			10,		// Tracers
			20,		// Weapons
			10,		// Dynamic lights
			50,		// Particle trail segments
			20,		// Smoke
			20,		// Bullet trail
			15,		// Volume brush
			200,	// Shell casings
			1,		// Camera - Unused, it has its own list
			10,		// Particle explosions
			150,	// Sprites
			10,		// Explosions
			100,	// Debris
			20,		// Death
			50,		// Gibs
			20,		// Projectile
			300,	// Marks - bullet holes
			10,		// Light
			5,		// Critical hits
			1,		// Unused
			10,		// Pickup item
			10,		// Player
			5,		// Line ball
			5,		// Anime lines
			15,		// Weapon sounds
			5		// Auto
		};

		// Use detail setting for bullet holes...

		if (nIndex == SFX_MARK_ID)
		{
			CRiotSettings* pSettings = g_pRiotClientShell->GetSettings();
			if (pSettings)
			{
				return int(pSettings->NumBulletHoles() + 1);
			}
		}

		return s_nDynArrayMaxNums[nIndex];
	}

	return 0;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::OnTouchNotify()
//
//	PURPOSE:	Handle client-side touch notify
//
// ----------------------------------------------------------------------- //

void CSFXMgr::OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo, float forceMag)
{
	if (!hMain) return;

	CSpecialFX* pFX = FindSpecialFX(SFX_PROJECTILE_ID, hMain);

	if (pFX)
	{
		pFX->HandleTouch(pInfo, forceMag);
	}
}

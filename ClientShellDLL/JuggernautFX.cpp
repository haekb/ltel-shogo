// ----------------------------------------------------------------------- //
//
// MODULE  : JuggernautFX.cpp
//
// PURPOSE : Juggernaut special FX - Implementation
//
// CREATED : 4/29/98
//
// ----------------------------------------------------------------------- //

#include "JuggernautFX.h"
#include "cpp_client_de.h"
#include "ClientUtilities.h"
#include "RiotClientShell.h"
#include "WeaponModel.h"
#include "DynamicLightFX.h"
#include "RiotMsgIds.h"

#define MAX_RANGE	5000.0f

extern CRiotClientShell* g_pRiotClientShell;

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CJuggernautFX::Init
//
//	PURPOSE:	Init the laser cannon fx
//
// ----------------------------------------------------------------------- //

DBOOL CJuggernautFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CSpecialFX::Init(psfxCreateStruct)) return DFALSE;

	JNCREATESTRUCT* pJN = (JNCREATESTRUCT*)psfxCreateStruct;
	VEC_COPY(m_vFirePos, pJN->vFirePos);
	VEC_COPY(m_vEndPos, pJN->vEndPos);
	m_fFadeTime = pJN->fFadeTime;

	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CJuggernautFX::CreateObject
//
//	PURPOSE:	Create object associated the line system.
//
// ----------------------------------------------------------------------- //

DBOOL CJuggernautFX::CreateObject(CClientDE *pClientDE)
{
	if (!CSpecialFX::CreateObject(pClientDE)) return DFALSE;

	DVector vDir;
	VEC_SUB(vDir, m_vEndPos, m_vFirePos);

	DFLOAT fDistance = VEC_MAG(vDir);
	if (fDistance > MAX_RANGE) fDistance = MAX_RANGE;
	
	VEC_NORM(vDir);

	DVector vTemp, vPos;
	VEC_MULSCALAR(vTemp, vDir, fDistance/2.0f);
	VEC_ADD(vPos, m_vFirePos, vTemp);

	// VEC_MULSCALAR(vDir, vDir, -1.0f);

	DRotation rRot;
	pClientDE->AlignRotation(&rRot, &vDir, DNULL);

	// Setup the model...

	ObjectCreateStruct createStruct;
	INIT_OBJECTCREATESTRUCT(createStruct);

	createStruct.m_ObjectType = OT_MODEL;
	//strcpy(createStruct.m_Filename, "Models\\PV_Weapons\\cone.abc");
	SAFE_STRCPY(createStruct.m_Filename, "Models\\Powerups\\beam.abc");
	//strcpy(createStruct.m_SkinName, "SpriteTextures\\Bullgut_smoke_1.dtx");
	SAFE_STRCPY(createStruct.m_SkinName, "SpecialFX\\Explosions\\Juggernaut.dtx");
	createStruct.m_Flags = FLAG_VISIBLE | FLAG_MODELGOURAUDSHADE | FLAG_NOLIGHT;
	VEC_COPY(createStruct.m_Pos, vPos);
	ROT_COPY(createStruct.m_Rotation, rRot);

	m_hObject = m_pClientDE->CreateObject(&createStruct);

	pClientDE->SetObjectColor(m_hObject, 1.0f, 1.0f, 1.0f, 0.8f);

	DVector vScale;
	VEC_SET(vScale, 1.8f, 1.8f, fDistance);

	m_pClientDE->SetObjectScale(m_hObject, &vScale);

	m_fStartTime = m_pClientDE->GetTime();

	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CJuggernautFX::Update
//
//	PURPOSE:	Update the laser cannon fx (recalculate end point)
//
// ----------------------------------------------------------------------- //

DBOOL CJuggernautFX::Update()
{
	if (!m_hObject || !m_pClientDE) return DFALSE;

	// Fade over time...
#define FADING
#ifdef FADING
	DFLOAT fTime = m_pClientDE->GetTime();
	DFLOAT fStopTime = (m_fStartTime + m_fFadeTime);

	if (fTime > fStopTime)
	{
		return DFALSE;
	}
	else
	{
		DFLOAT fNewAlpha = 0.99f * (fStopTime - fTime)/m_fFadeTime; 
		m_pClientDE->SetObjectColor(m_hObject, fNewAlpha, fNewAlpha, fNewAlpha, fNewAlpha);
	}
#endif

	return DTRUE;
}



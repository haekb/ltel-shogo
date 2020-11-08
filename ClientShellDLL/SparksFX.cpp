// ----------------------------------------------------------------------- //
//
// MODULE  : SparksFX.cpp
//
// PURPOSE : Sparks special FX - Implementation
//
// CREATED : 1/17/98
//
// ----------------------------------------------------------------------- //

#include "SparksFX.h"
#include "cpp_client_de.h"
#include "ClientUtilities.h"


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSparksFX::Init
//
//	PURPOSE:	Init the sparks
//
// ----------------------------------------------------------------------- //

DBOOL CSparksFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CBaseParticleSystemFX::Init(psfxCreateStruct)) return DFALSE;

	SCREATESTRUCT* pSS = (SCREATESTRUCT*)psfxCreateStruct;
	VEC_COPY(m_vPos, pSS->vPos);
	VEC_COPY(m_vDir, pSS->vDir);
	VEC_COPY(m_vColor1, pSS->vColor1);
	VEC_COPY(m_vColor2, pSS->vColor2);
	m_nSparks			= pSS->nSparks;
	m_fDuration			= pSS->fDuration;
	m_fEmissionRadius	= pSS->fEmissionRadius;
	m_hstrTexture		= pSS->hstrTexture;
	m_fRadius			= pSS->fRadius;
	m_fGravity			= pSS->fGravity;

	m_pTextureName = "SpecialFX\\ParticleTextures\\Spark_yellow_1.dtx";

	return DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSparksFX::CreateObject
//
//	PURPOSE:	Create object associated the particle system.
//
// ----------------------------------------------------------------------- //

DBOOL CSparksFX::CreateObject(CClientDE *pClientDE)
{
	if (!pClientDE ) return DFALSE;

	if (m_hstrTexture)
	{
		m_pTextureName = pClientDE->GetStringData(m_hstrTexture);
	}

	DBOOL bRet = CBaseParticleSystemFX::CreateObject(pClientDE);

	if (bRet)
	{
		bRet = AddSparks();
	}

	return bRet;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSparksFX::Update
//
//	PURPOSE:	Update the smoke trail (add smoke)
//
// ----------------------------------------------------------------------- //

DBOOL CSparksFX::Update()
{
	if(!m_hObject || !m_pClientDE) return DFALSE;

	DFLOAT fTime = m_pClientDE->GetTime();

	// Check to see if we should go away...

	if (fTime > m_fStartTime + m_fDuration)
	{
		return DFALSE;
	}


	// Fade sparks over duration...

	DFLOAT fScale = (m_fDuration - (fTime - m_fStartTime)) / m_fDuration;

	// m_pClientDE->SetParticleSystemColorScale(m_hObject, fScale);

	DFLOAT r, g, b, a;
	m_pClientDE->GetObjectColor(m_hObject, &r, &g, &b, &a);
	m_pClientDE->SetObjectColor(m_hObject, r, g, b, fScale);

	return DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSparksFX::AddSparks
//
//	PURPOSE:	Make the sparks
//
// ----------------------------------------------------------------------- //```
DBOOL CSparksFX::AddSparks()
{
	if(!m_hObject || !m_pClientDE) return DFALSE;

	DVector vMinOffset, vMaxOffset, vMinVel, vMaxVel;
	VEC_SET(vMinOffset, -m_fEmissionRadius, -m_fEmissionRadius, -m_fEmissionRadius);
	VEC_SET(vMaxOffset, m_fEmissionRadius, m_fEmissionRadius, m_fEmissionRadius);

	DFLOAT fVelOffset = VEC_MAG(m_vDir);
	VEC_NORM(m_vDir);

	DRotation rRot;
	m_pClientDE->AlignRotation(&rRot, &m_vDir, DNULL);

	DVector vF, vU, vR;
	m_pClientDE->GetRotationVectors(&rRot, &vU, &vR, &vF);

	if (vF.y <= -0.95f || vF.y >= 0.95f)
	{
		vF.y = vF.y > 0.0f ? 1.0f : -1.0f;
		VEC_SET(vR, 1.0f, 0.0f, 0.0f);
		VEC_SET(vU, 0.0f, 0.0f, 1.0f);
	}
	else if (vF.x <= -0.95f || vF.x >= 0.95f)
	{
		vF.x = vF.x > 0.0f ? 1.0f : -1.0f;
		VEC_SET(vR, 0.0f, 1.0f, 0.0f);
		VEC_SET(vU, 0.0f, 0.0f, 1.0f);
	}
	else if (vF.z <= -0.95f || vF.z >= 0.95f)
	{
		vF.z = vF.z > 0.0f ? 1.0f : -1.0f;
		VEC_SET(vR, 1.0f, 0.0f, 0.0f);
		VEC_SET(vU, 0.0f, 1.0f, 0.0f);
	}

	DVector vTemp;

	VEC_MULSCALAR(vMinVel, vF, fVelOffset * .025f); 
	VEC_MULSCALAR(vMaxVel, vF, fVelOffset); 

	VEC_MULSCALAR(vTemp, vR, -fVelOffset);
	VEC_ADD(vMinVel, vMinVel, vTemp);

	VEC_MULSCALAR(vTemp, vR, fVelOffset);
	VEC_ADD(vMaxVel, vMaxVel, vTemp);

	VEC_MULSCALAR(vTemp, vU, -fVelOffset);
	VEC_ADD(vMinVel, vMinVel, vTemp);

	VEC_MULSCALAR(vTemp, vU, fVelOffset);
	VEC_ADD(vMaxVel, vMaxVel, vTemp);

	m_pClientDE->AddParticles(m_hObject, m_nSparks,
							  &vMinOffset, &vMaxOffset,		// Position offset
							  &vMinVel, &vMaxVel,			// Velocity
							  &m_vColor1, &m_vColor2,		// Color
							  m_fDuration, m_fDuration);
	
	m_fStartTime = m_pClientDE->GetTime();

	return DTRUE;
}
// ----------------------------------------------------------------------- //
//
// MODULE  : RocketFlareFX.cpp
//
// PURPOSE : RocketFlare special FX - Implementation
//
// CREATED : 11/11/97
//
// ----------------------------------------------------------------------- //

#include "RocketFlareFX.h"
#include "cpp_client_de.h"
#include "ClientUtilities.h"


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRocketFlareFX::Init
//
//	PURPOSE:	Init the flare
//
// ----------------------------------------------------------------------- //

DBOOL CRocketFlareFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CBaseParticleSystemFX::Init(psfxCreateStruct)) return DFALSE;

	RFCREATESTRUCT* pST = (RFCREATESTRUCT*)psfxCreateStruct;
	VEC_COPY(m_vVel, pST->vVel);
	m_bSmall = pST->bSmall;
	
	m_fRadius  = 700.0f;		// pST->fRadius
	m_fGravity = 0.0f;

	if (m_bSmall)
	{
		m_fRadius /= 2.0f;
	}

	return DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRocketFlareFX::CreateObject
//
//	PURPOSE:	Create object associated the particle system.
//
// ----------------------------------------------------------------------- //

DBOOL CRocketFlareFX::CreateObject(CClientDE *pClientDE)
{
	DBOOL bRet = CBaseParticleSystemFX::CreateObject(pClientDE);

	if (bRet)
	{
		bRet = AddFlareParticles();
	}

	return bRet;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRocketFlareFX::Update
//
//	PURPOSE:	Update the smoke trail (add smoke)
//
// ----------------------------------------------------------------------- //

DBOOL CRocketFlareFX::Update()
{
	if(!m_hObject || !m_pClientDE || !m_hServerObject) return DFALSE;


	// Check to see if we should go away...

	if (m_bWantRemove)
	{
		return DFALSE;
	}


	// Calculate flare position...

	DVector vPos;
	m_pClientDE->GetObjectPos(m_hServerObject, &vPos);

	// Move the flare forward a bit, to put it in the correct position...

	DVector vAdjust;
	VEC_MULSCALAR(vAdjust, m_vVel, .035f);
	VEC_ADD(vPos, vPos, vAdjust);

	m_pClientDE->SetObjectPos(m_hObject, &vPos);

	
	// Rotate flare???

	//DRotation rRot;
	//m_pClientDE->GetObjectRotation(m_hObject, &rRot);
	//m_pClientDE->RotateAroundAxis(&rRot, &m_vVel, .333f);
	//m_pClientDE->SetObjectRotation(m_hObject, &rRot);

	return DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRocketFlareFX::AddFlareParticles
//
//	PURPOSE:	Make the flare
//
// ----------------------------------------------------------------------- //

DBOOL CRocketFlareFX::AddFlareParticles()
{
	if(!m_hObject || !m_pClientDE) return DFALSE;

	DVector vPos, vCurPos, vDelta, vDriftVel, vColor;
	VEC_INIT(vDriftVel);
	VEC_INIT(vPos);
	VEC_SET(vColor, 255.0f, 0.0f, 0.0f);

	// Calculate delta offsets for flare...opposite direction of velocity...

	VEC_COPY(vDelta, m_vVel);
	VEC_NORM(vDelta);
	VEC_MULSCALAR(vDelta, vDelta, -1.5f);


	// Determine offset used for flare thickness...

	DFLOAT fOriginalOffset = 1.0f;

	if (m_bSmall)
	{
		fOriginalOffset /= 2.0f;
	}

	DFLOAT fOffset = fOriginalOffset;


	int nNumSteps   = 6;	// Number of "steps" in the flare
	int nNumPerStep = 6;	// Number of particles per step

	for (int i=0; i < nNumSteps; i++)
	{
		// Go from Red to yellow...

		vColor.y = float(i)*(255.0f/float(nNumSteps-1));


		// Shrink to a point...

		fOffset  = fOriginalOffset - float(i)*(fOriginalOffset/float(nNumSteps-1));
		

		// Build nNumStep sections of the flare

		for (int j=0; j < nNumPerStep; j++)
		{
			VEC_COPY(vCurPos, vPos);

			vCurPos.x += GetRandom(-fOffset, fOffset);
			vCurPos.y += GetRandom(-fOffset, fOffset);
			vCurPos.z += GetRandom(-fOffset, fOffset);

			m_pClientDE->AddParticle(m_hObject, &vCurPos, &vDriftVel, &vColor, 100000);
		}

		VEC_ADD(vPos, vPos, vDelta);
	}

	return DTRUE;
}
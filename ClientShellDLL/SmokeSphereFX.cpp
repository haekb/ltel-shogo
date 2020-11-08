// ----------------------------------------------------------------------- //
//
// MODULE  : SmokeSphereFX.cpp
//
// PURPOSE : SmokeSphere special FX - Implementation
//
// CREATED : 12/15/97
//
// ----------------------------------------------------------------------- //

#include "SmokeSphereFX.h"
#include "cpp_client_de.h"
#include "ClientUtilities.h"


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSmokeSphereFX::Init
//
//	PURPOSE:	Init the smoke trail
//
// ----------------------------------------------------------------------- //

DBOOL CSmokeSphereFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CBaseParticleSystemFX::Init(psfxCreateStruct)) return DFALSE;

	SSCREATESTRUCT* pSS = (SSCREATESTRUCT*)psfxCreateStruct;
	m_fSphereRadius = pSS->m_fRadius;
	m_fLifeTime = pSS->m_fLifeTime;

	// Particles are create as a shell, so the number must be proportional to the area of the shell,
	// which is proportional to radius^2...
	m_nNumParticles = ( DDWORD )( 0.1f * m_fSphereRadius * m_fSphereRadius );
	if( !pSS->m_bDense )
		m_nNumParticles >>= 1;

	m_fGravity = 0.0f;			// Float 'em
	m_fRadius = 1000.0f;
	m_pTextureName	= "SpriteTextures\\bullgut_smoke_1.dtx";

	VEC_SET(m_vColor1, 100.0f, 100.0f, 100.0f);
	VEC_SET(m_vColor2, 150.0f, 150.0f, 150.0f);

	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSmokeSphereFX::Update
//
//	PURPOSE:	Update the smoke
//
// ----------------------------------------------------------------------- //

DBOOL CSmokeSphereFX::Update()
{
	DFLOAT fRadius;

	if(!m_hObject || !m_pClientDE || !m_hServerObject) return DFALSE;

	DFLOAT fTime = m_pClientDE->GetTime();


	// Check to see if we should just wait for last smoke puff to go away...

	if (m_bWantRemove)
	{
		if (fTime > m_fStartTime + m_fLifeTime)
		{
			return DFALSE;
		}
		
		return DTRUE;
	}

	// Create the smoke...
	if ((m_fStartTime < 0))
	{
		DVector vTemp, vDriftVel, vColor, vPos;
		DRotation rRot;

		// What is the range of colors?
		DFLOAT fRange = m_vColor2.x - m_vColor1.x;
		DFLOAT fRadiusMin;

		fRadiusMin = m_fSphereRadius - m_fSphereRadius * 0.10f;

		// Build the individual smoke puffs...
		for (DDWORD j=0; j < m_nNumParticles; j++)
		{
			VEC_SET(vDriftVel, GetRandom(-3.0f, 3.0f), 
							   GetRandom( 3.0f, 10.0f), 
							   GetRandom(-3.0f, 3.0f));

			// Put point somewhere in sphere...
			m_pClientDE->SetupEuler( &rRot, GetRandom( 0.0f, ( DFLOAT )MATH_CIRCLE ), 
				GetRandom( 0.0f, ( DFLOAT )MATH_CIRCLE ), GetRandom( 0.0f, ( DFLOAT )MATH_CIRCLE ));
			m_pClientDE->GetRotationVectors( &rRot, &vTemp, &vTemp, &vPos );
			fRadius = GetRandom( fRadiusMin, m_fSphereRadius );
			VEC_MULSCALAR( vPos, vPos, fRadius );

			GetRandomColorInRange(vColor);

			m_pClientDE->AddParticle(m_hObject, &vPos, &vDriftVel, &vColor, m_fLifeTime);
		}

		m_fStartTime = fTime;
	}

	return DTRUE;
}

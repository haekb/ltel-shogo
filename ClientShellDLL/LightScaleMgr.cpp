#include "client_de.h"
#include "cpp_client_de.h"
#include "LightScaleMgr.h"
#include "TextHelper.h"

DBOOL CLightScaleMgr::Init (CClientDE* pClientDE)
{
	if (!pClientDE) return DFALSE;

	m_pClientDE = pClientDE;
	VEC_SET(m_TimeOfDayScale, 1, 1, 1);

	return DTRUE;
}

void CLightScaleMgr::Term()
{
	LS_EFFECT* pEffect = m_pEffects;
	LS_EFFECT* pPrev = DNULL;
	while (pEffect)
	{
		pPrev = pEffect;
		pEffect = pEffect->pNext;
		delete pPrev;
	}

	m_pClientDE = DNULL;
	m_pEffects = DNULL;
}

void CLightScaleMgr::SetTimeOfDayScale(DVector &scale)
{
	m_TimeOfDayScale = scale;
	SetLightScale();
}

void CLightScaleMgr::SetLightScale (DFLOAT nRed, DFLOAT nGreen, DFLOAT nBlue, LightEffectType eType)
{
	// add this to the list of light scales

	LS_EFFECT* pEffect = new LS_EFFECT;
	pEffect->nRed = nRed;
	pEffect->nGreen = nGreen;
	pEffect->nBlue = nBlue;
	pEffect->eType = eType;

	pEffect->pNext = m_pEffects;
	m_pEffects = pEffect;

	// set the correct global light scale

	SetLightScale();
}

void CLightScaleMgr::ClearLightScale (DFLOAT nRed, DFLOAT nGreen, DFLOAT nBlue, LightEffectType eType)
{
	LS_EFFECT* pEffect = m_pEffects;
	LS_EFFECT* pPrev = DNULL;
	while (pEffect)
	{
		if (pEffect->nRed == nRed && pEffect->nGreen == nGreen && pEffect->nBlue == nBlue && pEffect->eType == eType)
		{
			if (pPrev)
			{
				pPrev->pNext = pEffect->pNext;
				delete pEffect;
			}
			else
			{
				m_pEffects = pEffect->pNext;
				delete pEffect;
			}

			break;
		}

		pPrev = pEffect;
		pEffect = pEffect->pNext;
	}

	SetLightScale();
}

void CLightScaleMgr::SetLightScale()
{
	if (!m_pClientDE) return;

	// go though looking for the first interface type

	LS_EFFECT* pEffect = m_pEffects;
	while (pEffect)
	{
		if (pEffect->eType == LightEffectInterface)
		{
			DVector vLightScale;
			vLightScale.x = pEffect->nRed;
			vLightScale.y = pEffect->nGreen;
			vLightScale.z = pEffect->nBlue;
			m_pClientDE->SetGlobalLightScale (&vLightScale);
			return;
		}
		pEffect = pEffect->pNext;
	}

	// go though looking for the first powerup type

	pEffect = m_pEffects;
	while (pEffect)
	{
		if (pEffect->eType == LightEffectPowerup)
		{
			DVector vLightScale;
			vLightScale.x = pEffect->nRed;
			vLightScale.y = pEffect->nGreen;
			vLightScale.z = pEffect->nBlue;
			m_pClientDE->SetGlobalLightScale (&vLightScale);
			return;
		}
		pEffect = pEffect->pNext;
	}

	// go through looking for the first environment type

	pEffect = m_pEffects;
	while (pEffect)
	{
		if (pEffect->eType == LightEffectEnvironment)
		{
			DVector vLightScale;
			vLightScale.x = pEffect->nRed;
			vLightScale.y = pEffect->nGreen;
			vLightScale.z = pEffect->nBlue;
			
			vLightScale.x *= m_TimeOfDayScale.x;
			vLightScale.y *= m_TimeOfDayScale.y;
			vLightScale.z *= m_TimeOfDayScale.z;
			
			m_pClientDE->SetGlobalLightScale (&vLightScale);
			return;
		}
		pEffect = pEffect->pNext;
	}

	// go through looking for the first world type

	pEffect = m_pEffects;
	while (pEffect)
	{
		if (pEffect->eType == LightEffectWorld)
		{
			DVector vLightScale;
			vLightScale.x = pEffect->nRed;
			vLightScale.y = pEffect->nGreen;
			vLightScale.z = pEffect->nBlue;

			vLightScale.x *= m_TimeOfDayScale.x;
			vLightScale.y *= m_TimeOfDayScale.y;
			vLightScale.z *= m_TimeOfDayScale.z;

			m_pClientDE->SetGlobalLightScale (&vLightScale);
			return;
		}
		pEffect = pEffect->pNext;
	}

	// we got all the way through without finding a suitable light effect - just set the light scale to (1,1,1)

	DVector vec;
	VEC_SET (vec, 1.0f, 1.0f, 1.0f);
	m_pClientDE->SetGlobalLightScale (&vec);
}

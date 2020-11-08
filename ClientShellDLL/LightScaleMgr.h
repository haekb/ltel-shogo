#ifndef __LIGHTSCALEMGR_H
#define __LIGHTSCALEMGR_H

#include "cpp_client_de.h"

enum LightEffectType
{
	LightEffectWorld = 0,
	LightEffectEnvironment,
	LightEffectPowerup,
	LightEffectInterface
};

struct LS_EFFECT
{
	LS_EFFECT()			{ nRed = 0.0f; nGreen = 0.0f; nBlue = 0.0f; eType = LightEffectWorld; }

	DFLOAT				nRed;
	DFLOAT				nGreen;
	DFLOAT				nBlue;
	LightEffectType		eType;

	LS_EFFECT*			pNext;
};

class CLightScaleMgr
{
public:

	CLightScaleMgr()	{ m_pClientDE = DNULL; m_pEffects = DNULL; }
	~CLightScaleMgr()	{ Term(); }

	DBOOL		Init (CClientDE* pClientDE);
	void		Term();

	DVector		GetTimeOfDayScale() {return m_TimeOfDayScale;}
	void		SetTimeOfDayScale(DVector &scale);

	void		SetLightScale (DVector* pVec, LightEffectType eType)		{ SetLightScale (pVec->x, pVec->y, pVec->z, eType); }
	void		SetLightScale (DFLOAT nRed, DFLOAT nGreen, DFLOAT nBlue, LightEffectType eType);

	void		ClearLightScale (DVector* pVec, LightEffectType eType)		{ ClearLightScale (pVec->x, pVec->y, pVec->z, eType); }
	void		ClearLightScale (DFLOAT nRed, DFLOAT nGreen, DFLOAT nBlue, LightEffectType eType);

protected:

	void		SetLightScale();

protected:

	CClientDE*	m_pClientDE;
	LS_EFFECT*	m_pEffects;
	DVector		m_TimeOfDayScale;
};

#endif

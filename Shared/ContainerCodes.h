// ----------------------------------------------------------------------- //
//
// MODULE  : ContainerCodes.h
//
// PURPOSE : Container code definitions
//
// CREATED : 1/29/98
//
// ----------------------------------------------------------------------- //

#ifndef __CONTAINER_CODES_H__
#define __CONTAINER_CODES_H__

enum ContainerCode {
	CC_NONE=0,
	CC_BLUE_WATER,
	CC_DIRTY_WATER,
	CC_CLEAR_WATER,
	CC_CORROSIVE_FLUID,
	CC_KATO,
	CC_LIQUID_NITROGEN,
	CC_POISON_GAS,
	CC_SMOKE,
	CC_ELECTRICITY,
	CC_ENDLESS_FALL,
	CC_WIND,
	CC_ZERO_GRAVITY,
	CC_VACUUM,
	CC_LADDER,
	CC_TOTAL_RED,
	CC_TINT_SCREEN,
	CC_VOLUME,
	CC_MAX_CONTAINER_CODES
};

inline DBOOL IsLiquid(ContainerCode code)
{
	DBOOL bRet = DFALSE;

	switch (code)
	{
		case CC_BLUE_WATER:
		case CC_DIRTY_WATER:
		case CC_CLEAR_WATER:
		case CC_CORROSIVE_FLUID:
		case CC_KATO:
		case CC_LIQUID_NITROGEN:
			bRet = DTRUE;
		break;

		default : break;
	}

	return bRet;
}

inline DBOOL GetLiquidColorRange(ContainerCode code, DVector* pvC1, DVector* pvC2)
{
	if (!IsLiquid(code) || !pvC1 || !pvC2) return DFALSE;

	switch (code)
	{
		case CC_BLUE_WATER:
			VEC_SET(*pvC1, 235.0f, 235.0f, 235.0f);
			VEC_SET(*pvC2, 235.0f, 235.0f, 255.0f);
		break;

		case CC_DIRTY_WATER:
			VEC_SET(*pvC1, 200.0f, 200.0f, 50.0f);
			VEC_SET(*pvC2, 255.0f, 200.0f, 50.0f);
		break;
		
		case CC_CLEAR_WATER:
			VEC_SET(*pvC2, 255.0f, 255.0f, 255.0f);
			VEC_SET(*pvC2, 255.0f, 255.0f, 255.0f);
		break;
		
		case CC_CORROSIVE_FLUID:
			VEC_SET(*pvC2, 200.0f, 200.0f, 0.0f);
			VEC_SET(*pvC2, 255.0f, 255.0f, 0.0f);
		break;
		
		case CC_KATO:
			VEC_SET(*pvC2, 200.0f, 0.0f, 0.0f);
			VEC_SET(*pvC2, 255.0f, 0.0f, 0.0f);
		break;
		
		case CC_LIQUID_NITROGEN:
			VEC_SET(*pvC2, 200.0f, 200.0f, 255.0f);
			VEC_SET(*pvC2, 200.0f, 200.0f, 255.0f);
		break;

		default : break;
	}

	return DTRUE;
}

inline DBOOL IsZeroGravity(ContainerCode code)
{
	DBOOL bRet = DFALSE;

	switch (code)
	{
		case CC_ZERO_GRAVITY:
		case CC_VACUUM:
			bRet = DTRUE;
		break;

		default : break;
	}

	return bRet;
}

inline DBOOL IsFreeMovement(ContainerCode code)
{
	return(IsLiquid(code) || IsZeroGravity(code) || (code == CC_LADDER));
}

#endif // __CONTAINER_CODES_H__

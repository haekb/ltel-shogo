// ----------------------------------------------------------------------- //
//
// MODULE  : ModelFuncs.h
//
// PURPOSE : Model related utility functions
//
// CREATED : 6/14/98
//
// ----------------------------------------------------------------------- //

#ifndef __MODEL_FUNCS_H__
#define __MODEL_FUNCS_H__

#include "basetypes_de.h"
#include "ModelIds.h"
#include "CharacterAlignment.h"
#include "GibTypes.h"
#include "PlayerModeTypes.h"

#define MODEL_LARGE_FLAG	0x80
#define MODEL_SMALL_FLAG	0x40
#define MODEL_FLAG_MASK		0xC0

enum ModelSize { MS_NORMAL=0, MS_SMALL, MS_LARGE, NUM_MODELSIZES };
enum ModelType { MT_UNSPECIFIED=0, MT_MECHA, MT_HUMAN, MT_VEHICLE, MT_PROP_GENERIC };

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GetModel
//
//	PURPOSE:	Return the model associated with a particular id
//
// ----------------------------------------------------------------------- //

char* GetModel(DBYTE nId, ModelSize size=MS_NORMAL);

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GetSkin
//
//	PURPOSE:	Return the skin associated with a particular id
//
// ----------------------------------------------------------------------- //

char* GetSkin(DBYTE nId, CharacterClass cc, ModelSize size=MS_NORMAL, 
			  DBOOL bMulti=DFALSE);

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GetGibModel
//
//	PURPOSE:	Return the gib model associated with a particular id
//
// ----------------------------------------------------------------------- //

char* GetGibModel(DBYTE nId, GibType eType, ModelSize size=MS_NORMAL);

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GetGibModelScale
//
//	PURPOSE:	Return the scale of the particular gib model
//
// ----------------------------------------------------------------------- //

inline DVector GetGibModelScale(DBYTE nId, ModelSize size=MS_NORMAL)
{
	DVector vScale;
	VEC_SET(vScale, 1.0f, 1.0f, 1.0f);

	if (size == MS_SMALL)
	{
		VEC_MULSCALAR(vScale, vScale, 0.2f);
	}
	else if (size == MS_LARGE)
	{
		VEC_MULSCALAR(vScale, vScale, 5.0f);
	}

	return vScale;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GetModelType
//
//	PURPOSE:	Return the type of the model
//
// ----------------------------------------------------------------------- //

ModelType GetModelType(DBYTE nId, ModelSize size=MS_NORMAL);

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GetTurretFilename
//
//	PURPOSE:	Return the turret model associated with a particular id
//
// ----------------------------------------------------------------------- //

char* GetTurretFilename(DBYTE nId, ModelSize size=MS_NORMAL);

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GetModelName
//
//	PURPOSE:	Return the model name associated with a particular id
//
// ----------------------------------------------------------------------- //

char* GetModelName(DBYTE nId, ModelSize size=MS_NORMAL);


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	PlayerModeToModelID
//
//	PURPOSE:	Returns model id for given player mode.
//
// ----------------------------------------------------------------------- //

inline DBYTE PlayerModeToModelID( DDWORD dwMode )
{
	switch( dwMode )
	{
		default:
		case PM_MODE_FOOT:
			return MI_PLAYER_ONFOOT_ID;
		case PM_MODE_KID:
			return MI_PLAYER_KID_ID;
		case PM_MODE_MCA_AP:
			return MI_PLAYER_PREDATOR_ID;
		case PM_MODE_MCA_UE:
			return MI_PLAYER_ENFORCER_ID;
		case PM_MODE_MCA_AO:
			return MI_PLAYER_ORDOG_ID;
		case PM_MODE_MCA_SA:
			return MI_PLAYER_AKUMA_ID;
	}
}

#endif // __MODEL_IDS_H__

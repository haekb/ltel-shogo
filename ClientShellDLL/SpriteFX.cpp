// ----------------------------------------------------------------------- //
//
// MODULE  : SpriteFX.cpp
//
// PURPOSE : Sprite special FX - Implementation
//
// CREATED : 5/22/98
//
// ----------------------------------------------------------------------- //

#include "SpriteFX.h"
#include "cpp_client_de.h"

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSpriteFX::Init
//
//	PURPOSE:	Create the sprite
//
// ----------------------------------------------------------------------- //

DBOOL CSpriteFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	return CBaseScaleFX::Init(psfxCreateStruct);
}

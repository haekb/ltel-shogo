// ----------------------------------------------------------------------- //
//
// MODULE  : PickupItem.h
//
// PURPOSE : PickupItem - Definition
//
// CREATED : 8/20/98
//
// ----------------------------------------------------------------------- //

#ifndef __PICKUPITEM_FX_H__
#define __PICKUPITEM_FX_H__

#include "SpecialFX.h"
#include "ModelFuncs.h"

struct PICKUPITEMCREATESTRUCT : public SFXCREATESTRUCT
{
	PICKUPITEMCREATESTRUCT::PICKUPITEMCREATESTRUCT();
};

inline PICKUPITEMCREATESTRUCT::PICKUPITEMCREATESTRUCT()
{
	memset(this, 0, sizeof(PICKUPITEMCREATESTRUCT));
}

class CPickupItemFX : public CSpecialFX
{
	public :

		virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual DBOOL CreateObject(CClientDE* pClientDE);
		virtual DBOOL Update();

};

#endif // __PICKUPITEM_FX_H__
#ifndef __NEWGAMEMENU_H
#define __NEWGAMEMENU_H

#include "BaseMenu.h"

class CNewGameMenu : public CBaseMenu
{
public:

	virtual DBOOL		Init (CClientDE* pClientDE, CRiotMenu* pRiotMenu, CBaseMenu* pParent, int nScreenWidth, int nScreenHeight);
	virtual void		Reset();
	virtual void		ScreenDimsChanged (int nScreenWidth, int nScreenHeight);

	virtual DBOOL		LoadAllSurfaces()		{ return LoadSurfaces(); }
	virtual void		UnloadAllSurfaces()		{ UnloadSurfaces(); }

	virtual void		Return();

protected:

	virtual DBOOL		LoadSurfaces();
	virtual void		UnloadSurfaces();

protected:

};

#endif

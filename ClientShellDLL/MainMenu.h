#ifndef __MAINMENU_H
#define __MAINMENU_H

#include "BaseMenu.h"
#include "SinglePlayerMenu.h"
#include "OptionsMenu.h"

class CMainMenu : public CBaseMenu
{
public:

	CMainMenu();

	virtual DBOOL		Init (CClientDE* pClientDE, CRiotMenu* pRiotMenu, CBaseMenu* pParent, int nScreenWidth, int nScreenHeight);
	virtual void		ScreenDimsChanged (int nScreenWidth, int nScreenHeight);
	virtual void		Reset();
	
	virtual DBOOL		LoadAllSurfaces()		{ if (!m_SinglePlayerMenu.LoadAllSurfaces() || !m_OptionsMenu.LoadAllSurfaces()) return DFALSE; return DTRUE; }
	virtual void		UnloadAllSurfaces()		{ m_SinglePlayerMenu.UnloadAllSurfaces(); m_OptionsMenu.UnloadAllSurfaces(); }
	
	virtual void		OnEnterWorld()		{ 
											m_SinglePlayerMenu.OnEnterWorld();
											m_OptionsMenu.OnEnterWorld();
											CBaseMenu::OnEnterWorld();
											}
	
	virtual void		OnExitWorld()		{ 
											m_SinglePlayerMenu.OnExitWorld();
											m_OptionsMenu.OnExitWorld();
											CBaseMenu::OnExitWorld();
											}
	
	CSinglePlayerMenu*	GetSinglePlayerMenu()	{ return &m_SinglePlayerMenu; }

	virtual void		Return();
	virtual void		Esc();

	virtual void		Draw (HSURFACE hScreen, int nScreenWidth, int nScreenHeight, int nTextOffset = 0);

			DBOOL		DoMultiplayer(DBOOL bMinimize);

protected:

	virtual DBOOL		LoadSurfaces();
	virtual void		UnloadSurfaces();

protected:

	DBOOL				m_bFirstDraw;

	float				m_fVersionDisplayTimeLeft;
	HSURFACE			m_hVersion;
	CSize				m_szVersion;

	CSinglePlayerMenu	m_SinglePlayerMenu;
	COptionsMenu		m_OptionsMenu;
};

#endif

#ifndef __OPTIONSMENU_H
#define __OPTIONSMENU_H

#include "BaseMenu.h"
#include "DisplayOptionsMenu.h"
#include "SoundOptionsMenu.h"
#include "KeyboardMenu.h"
#include "MouseMenu.h"
#include "JoystickMenu.h"

class COptionsMenu : public CBaseMenu
{
public:

	virtual DBOOL		Init (CClientDE* pClientDE, CRiotMenu* pRiotMenu, CBaseMenu* pParent, int nScreenWidth, int nScreenHeight);
	virtual void		ScreenDimsChanged (int nScreenWidth, int nScreenHeight);
	
	virtual DBOOL		LoadAllSurfaces()		{ if (!m_DisplayOptionsMenu.LoadAllSurfaces() || !m_SoundOptionsMenu.LoadAllSurfaces() ||
													  !m_KeyboardMenu.LoadAllSurfaces() || !m_MouseMenu.LoadAllSurfaces() || 
													  (m_JoystickMenu.JoystickEnabled() && !m_JoystickMenu.LoadAllSurfaces())) return DFALSE; return LoadSurfaces(); }
	virtual void		UnloadAllSurfaces()		{ m_DisplayOptionsMenu.UnloadAllSurfaces(); m_SoundOptionsMenu.UnloadAllSurfaces(); 
												  m_KeyboardMenu.UnloadAllSurfaces(); m_MouseMenu.UnloadAllSurfaces();
												  m_JoystickMenu.UnloadAllSurfaces(); UnloadSurfaces(); }
	virtual void		Return();

	virtual void		OnEnterWorld()		{ 
											m_DisplayOptionsMenu.OnEnterWorld();
											m_SoundOptionsMenu.OnEnterWorld();
											m_KeyboardMenu.OnEnterWorld();
											m_MouseMenu.OnEnterWorld();
											m_JoystickMenu.OnEnterWorld();
											CBaseMenu::OnEnterWorld();
											}

	virtual void		OnExitWorld()		{ 
											m_DisplayOptionsMenu.OnExitWorld();
											m_SoundOptionsMenu.OnExitWorld();
											m_KeyboardMenu.OnExitWorld();
											m_MouseMenu.OnExitWorld();
											m_JoystickMenu.OnExitWorld();
											CBaseMenu::OnExitWorld();
											}
protected:

	virtual DBOOL		LoadSurfaces();
	virtual void		UnloadSurfaces();

protected:

	CDisplayOptionsMenu	m_DisplayOptionsMenu;
	CSoundOptionsMenu	m_SoundOptionsMenu;
	CKeyboardMenu		m_KeyboardMenu;
	CMouseMenu			m_MouseMenu;
	CJoystickMenu		m_JoystickMenu;
};

#endif

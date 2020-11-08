#ifndef __JOYSTICKMENU_H
#define __JOYSTICKMENU_H

#include "BaseMenu.h"
#include "Slider.h"

#define NUM_JOYBUTTONS		10

class CJoystickMenu : public CBaseMenu
{
public:

	CJoystickMenu();

	virtual DBOOL		Init (CClientDE* pClientDE, CRiotMenu* pRiotMenu, CBaseMenu* pParent, int nScreenWidth, int nScreenHeight);
	virtual void		ScreenDimsChanged (int nScreenWidth, int nScreenHeight);
	virtual void		Reset();
	
	virtual DBOOL		LoadAllSurfaces()		{ return LoadSurfaces(); }
	virtual void		UnloadAllSurfaces()		{ UnloadSurfaces(); }
	
	virtual void		Up();
	virtual void		Down();
	virtual void		Left();
	virtual void		Right();
	virtual void		PageUp();
	virtual void		PageDown();
	virtual void		Home();
	virtual void		End();
	virtual void		Return();
	virtual void		Esc();

	virtual void		Draw (HSURFACE hScreen, int nScreenWidth, int nScreenHeight, int nTextOffset = 0);

	DBOOL				JoystickEnabled()		{ return m_bJoystickEnabled; }
	DBOOL				JoystickMenuDisabled()	{ return m_bJoystickMenuDisabled; };

protected:

	virtual DBOOL		LoadSurfaces();
	virtual void		UnloadSurfaces();

	virtual void		PostCalculateMenuDims();

	DBOOL				ChangeButtonSettingSurface (int nSelection, int* pSelection, int nChange);
	DBOOL				BindButtonToCommand (int nButton, int nSelection);
	DBOOL				ReBindAxis (int nDirection);
	DBOOL				ImplementJoyLook (CClientDE* pClientDE, DBOOL bJoyLook);

protected:

	int					m_nSecondColumn;

	DBOOL				m_bJoystickEnabled;

	DBOOL				m_bJoystickMenuDisabled;

	float				m_nXAxisMin;
	float				m_nXAxisMax;
	float				m_nYAxisMin;
	float				m_nYAxisMax;

	int					m_nJoyUpSelection;
	int					m_nJoyDownSelection;
	int					m_nJoyLeftSelection;
	int					m_nJoyRightSelection;
	int					m_nButtonSelections[NUM_JOYBUTTONS];

	GENERIC_ITEM		m_JoySettings[7 + NUM_JOYBUTTONS];
};

#endif

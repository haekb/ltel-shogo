#ifndef __MOUSEMENU_H
#define __MOUSEMENU_H

#include "BaseMenu.h"
#include "Slider.h"

class CMouseMenu : public CBaseMenu
{
public:

	CMouseMenu();

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

protected:

	virtual DBOOL		LoadSurfaces();
	virtual void		UnloadSurfaces();

	virtual void		PostCalculateMenuDims();
	
	DBOOL				ChangeButtonSettingSurface (int nSelection, int* pSelection, int nChange);
	DBOOL				BindButtonToCommand (int nButton, int nSelection);

protected:

	int					m_nSecondColumn;

	int					m_nLeftButtonSelection;
	int					m_nRightButtonSelection;
	int					m_nMiddleButtonSelection;

	GENERIC_ITEM		m_MouseSettings[9];
	CSlider				m_sliderMouseSensitivity;
	CSlider				m_sliderMouseInputRate;
};

#endif

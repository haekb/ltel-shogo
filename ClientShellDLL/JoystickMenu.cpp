#include "cpp_client_de.h"
#include "JoystickMenu.h"
#include "TextHelper.h"
#include "ClientRes.h"
#include "RiotMenu.h"
#include "ClientUtilities.h"
#include "RiotClientShell.h"
#include <stdio.h>

extern CommandID g_CommandArray[];

#define NUM_JOYBUTTONS		10

#define ID_JOYLOOK			0
#define ID_INVERTYAXIS		1
#define ID_JOYUP			2
#define ID_JOYDOWN			3
#define ID_JOYLEFT			4
#define ID_JOYRIGHT			5
#define ID_JOYBUTTONZERO	6
#define ID_BACK				(ID_JOYBUTTONZERO + NUM_JOYBUTTONS)

// check if a binding with the given action name exists (only checks first action!)
DBOOL JoystickMenuDoesBindingExist(CClientDE *pClientDE, char* lpszActionName)
{
	DBOOL bBindingExists = DFALSE;

	// Get the bindings
	DeviceBinding *pBinding=pClientDE->GetDeviceBindings(DEVICETYPE_JOYSTICK);

	// Iterate through each binding look for the one with our action name
	DeviceBinding *pCurrentBinding=pBinding;
	while (pCurrentBinding != NULL)
	{
		if (pCurrentBinding->pActionHead != NULL)
		{
			char *lpszFoundActionName=pCurrentBinding->pActionHead->strActionName;
			if (lpszFoundActionName != NULL)
			{
				if (stricmp(lpszActionName, lpszFoundActionName) == 0) bBindingExists = DTRUE;
			}
		}
		pCurrentBinding=pCurrentBinding->pNext;
		if (pCurrentBinding == pBinding) break;
	}

	// Free the device bindings
	pClientDE->FreeDeviceBindings(pBinding);

	return bBindingExists;
}

CJoystickMenu::CJoystickMenu() : CBaseMenu()
{
	m_nSecondColumn = 0;

	m_bJoystickEnabled = DFALSE;
	m_bJoystickMenuDisabled = DFALSE;

	m_nXAxisMin = 0.0f;
	m_nXAxisMax = 0.0f;
	m_nYAxisMin = 0.0f;
	m_nYAxisMax = 0.0f;
	
	m_nJoyUpSelection = 0;
	m_nJoyDownSelection = 0;
	m_nJoyLeftSelection = 0;
	m_nJoyRightSelection = 0;
	memset (m_nButtonSelections, 0, NUM_JOYBUTTONS * sizeof(int));
}

DBOOL CJoystickMenu::Init (CClientDE* pClientDE, CRiotMenu* pRiotMenu, CBaseMenu* pParent, int nScreenWidth, int nScreenHeight)
{
	if (!pClientDE || !pRiotMenu) return DFALSE;

	// first check to see if the joystick is enabled...if not, try once more
	// if still not enabled, set parent menu and return

	CRiotClientShell* pClientShell = pRiotMenu->GetClientShell();
	if (!pClientShell) return DFALSE;

	if (!pClientShell->IsJoystickEnabled() && !pClientShell->EnableJoystick())
	{
		m_bJoystickEnabled = DFALSE;
		return DTRUE;
	}

	m_bJoystickEnabled = DTRUE;

	// read in the JOYSTICK.CFG file
	pClientDE->ReadConfigFile ("joystick.cfg");

	// check if the joystick menu is disabled
	m_bJoystickMenuDisabled = DFALSE;
	{
		HCONSOLEVAR hVar;
		hVar = pClientDE->GetConsoleVar( "JoystickMenuDisabled");
		if (hVar != NULL) 
		{
			if (pClientDE->GetVarValueFloat(hVar) == 1) m_bJoystickMenuDisabled = DTRUE;
		}
	}

	// if the fixedaxispitch variable is set make sure that the AxisPitch binding is present or clear the variable
	{
		HCONSOLEVAR hVar = pClientDE->GetConsoleVar( "FixedAxisPitch");
		if (hVar != NULL) if (pClientDE->GetVarValueFloat(hVar) == 1) 
		{
			if (!JoystickMenuDoesBindingExist(pClientDE, "AxisPitch"))
			{
				pClientDE->RunConsoleString("FixedAxisPitch 0.0");
			};
		}
	}

	// if menu is disabled exit
	if (m_bJoystickMenuDisabled) 
	{
//		return DTRUE;
	}

	// start the rest of the initialization

	CRiotSettings* pSettings = pRiotMenu->GetSettings();
	if (!pSettings) return DFALSE;
	
	m_nJoyUpSelection = NUM_COMMANDS - 1;
	m_nJoyDownSelection = NUM_COMMANDS - 1;
	m_nJoyLeftSelection = NUM_COMMANDS - 1;
	m_nJoyRightSelection = NUM_COMMANDS - 1;
	
	// init button selections

	for (int i = 0; i < NUM_JOYBUTTONS; i++)
	{
		m_nButtonSelections[i] = NUM_COMMANDS - 1;
	}

	// get button and axis object names

	char strButton[NUM_JOYBUTTONS][64];
	memset (strButton, 0, 64 * NUM_JOYBUTTONS);

	char strXAxis[64];
	char strYAxis[64];
	memset (strXAxis, 0, 64);
	memset (strYAxis, 0, 64);

	DeviceObject* pObjects = pClientDE->GetDeviceObjects (DEVICETYPE_JOYSTICK);
	DeviceObject* pObj = pObjects;
	int nButton = 0;
	while (pObj)
	{
		if (pObj->m_ObjectType == CONTROLTYPE_BUTTON)
		{
			if(nButton < NUM_JOYBUTTONS)
			{
				strncpy (strButton[nButton], pObj->m_ObjectName, sizeof(strButton[nButton])-1);
				nButton++;
			}
		}

		if (pObj->m_ObjectType == CONTROLTYPE_XAXIS)
		{
			strncpy (strXAxis, pObj->m_ObjectName, sizeof(strXAxis)-1);
			m_nXAxisMin = pObj->m_RangeLow;
			m_nXAxisMax = pObj->m_RangeHigh;
		}

		if (pObj->m_ObjectType == CONTROLTYPE_YAXIS)
		{
			strncpy (strYAxis, pObj->m_ObjectName, sizeof(strYAxis)-1);
			m_nYAxisMin = pObj->m_RangeLow;
			m_nYAxisMax = pObj->m_RangeHigh;
		}

		pObj = pObj->m_pNext;
	}
	pClientDE->FreeDeviceObjects (pObjects);
	
	// load the current bindings

	DeviceBinding* pBindings = pClientDE->GetDeviceBindings (DEVICETYPE_JOYSTICK);
	DeviceBinding* pBinding = pBindings;
	while (pBinding)
	{
		// check the buttons

		for (int i = 0; i < NUM_JOYBUTTONS; i++)
		{
			if (stricmp (pBinding->strTriggerName, strButton[i]) == 0 && pBinding->pActionHead)
			{
				m_nButtonSelections[i] = CommandToArrayPos (pBinding->pActionHead->nActionCode);
			}
		}
		
		// now check the axes

		if (stricmp (pBinding->strTriggerName, strXAxis) == 0)
		{
			GameAction* pAction = pBinding->pActionHead;
			while (pAction)
			{
				if (pAction->nRangeLow == m_nXAxisMin)
				{
					m_nJoyLeftSelection = CommandToArrayPos (pAction->nActionCode);
				}
				else if (pAction->nRangeHigh == m_nXAxisMax)
				{
					m_nJoyRightSelection = CommandToArrayPos (pAction->nActionCode);
				}
				pAction = pAction->pNext;
			}
		}

		if (stricmp (pBinding->strTriggerName, strYAxis) == 0)
		{
			GameAction* pAction = pBinding->pActionHead;
			while (pAction)
			{
				if (pAction->nRangeLow == m_nYAxisMin)
				{
					m_nJoyUpSelection = CommandToArrayPos (pAction->nActionCode);
				}
				else if (pAction->nRangeHigh == m_nYAxisMax)
				{
					m_nJoyDownSelection = CommandToArrayPos (pAction->nActionCode);
				}
				pAction = pAction->pNext;
			}
		}

		pBinding = pBinding->pNext;
	}
	pClientDE->FreeDeviceBindings (pBindings);

	// implement the joy look command

	ImplementJoyLook (pClientDE, pSettings->JoyLook());
	
	// call the base class Init() function

	DBOOL bSuccess = CBaseMenu::Init (pClientDE, pRiotMenu, pParent, nScreenWidth, nScreenHeight);
	
	if (nScreenWidth < 512)
	{
		m_nSecondColumn = 110;
	}
	else if (nScreenWidth < 640)
	{
		m_nSecondColumn = 120;
	}
	else
	{
		m_nSecondColumn = 155;
	}

	return bSuccess;
}

void CJoystickMenu::ScreenDimsChanged (int nScreenWidth, int nScreenHeight)
{
	CBaseMenu::ScreenDimsChanged (nScreenWidth, nScreenHeight);

	if (nScreenWidth < 512)
	{
		m_nSecondColumn = 110;
	}
	else if (nScreenWidth < 640)
	{
		m_nSecondColumn = 120;
	}
	else
	{
		m_nSecondColumn = 155;
	}
}

void CJoystickMenu::Reset()
{
	if (!m_pRiotMenu || !m_pClientDE) return;

	CBaseMenu::Reset();
}

void CJoystickMenu::Up()
{
	CBaseMenu::Up();
}

void CJoystickMenu::Down()
{
	CBaseMenu::Down();
}

void CJoystickMenu::Left()
{
	if (!m_pClientDE || !m_pRiotMenu) return;

	CRiotSettings* pSettings = m_pRiotMenu->GetSettings();
	if (!pSettings) return;

	CBitmapFont* pFontNormal = m_pRiotMenu->GetFont08n();
	CBitmapFont* pFontSelected = m_pRiotMenu->GetFont08s();

	if (m_nSelection == ID_JOYLOOK)
	{
		pSettings->Control[RS_CTRL_JOYLOOK].nValue = !pSettings->Control[RS_CTRL_JOYLOOK].nValue;

		if (m_JoySettings[ID_JOYLOOK].hMenuItem) m_pClientDE->DeleteSurface (m_JoySettings[ID_JOYLOOK].hMenuItem);
		if (m_JoySettings[ID_JOYLOOK].hMenuItemSelected) m_pClientDE->DeleteSurface (m_JoySettings[ID_JOYLOOK].hMenuItemSelected);

		int nStringID = pSettings->JoyLook() ? IDS_ON : IDS_OFF;
		m_JoySettings[ID_JOYLOOK].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, nStringID);
		m_JoySettings[ID_JOYLOOK].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, nStringID);
		
		// crop the joylook surface...
		//m_JoySettings[ID_JOYLOOK].hMenuItem = CropMenuItemTop (m_JoySettings[ID_JOYLOOK].hMenuItem);
		//m_JoySettings[ID_JOYLOOK].hMenuItemSelected = CropMenuItemTop (m_JoySettings[ID_JOYLOOK].hMenuItemSelected);

		m_pClientDE->GetSurfaceDims (m_JoySettings[ID_JOYLOOK].hMenuItem, &m_JoySettings[ID_JOYLOOK].szMenuItem.cx, &m_JoySettings[ID_JOYLOOK].szMenuItem.cy);

		ImplementJoyLook (m_pClientDE, pSettings->JoyLook());
	}
	else if (m_nSelection == ID_INVERTYAXIS)
	{
		pSettings->Control[RS_CTRL_JOYINVERTY].nValue = !pSettings->Control[RS_CTRL_JOYINVERTY].nValue;

		if (m_JoySettings[ID_INVERTYAXIS].hMenuItem) m_pClientDE->DeleteSurface (m_JoySettings[ID_INVERTYAXIS].hMenuItem);
		if (m_JoySettings[ID_INVERTYAXIS].hMenuItemSelected) m_pClientDE->DeleteSurface (m_JoySettings[ID_INVERTYAXIS].hMenuItemSelected);

		int nStringID = pSettings->JoyInvertY() ? IDS_YES : IDS_NO;
		m_JoySettings[ID_INVERTYAXIS].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, nStringID);
		m_JoySettings[ID_INVERTYAXIS].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, nStringID);

		m_pClientDE->GetSurfaceDims (m_JoySettings[ID_INVERTYAXIS].hMenuItem, &m_JoySettings[ID_INVERTYAXIS].szMenuItem.cx, &m_JoySettings[ID_INVERTYAXIS].szMenuItem.cy);
	}
	else if (m_nSelection == ID_JOYLEFT)
	{
		ChangeButtonSettingSurface (m_nSelection, &m_nJoyLeftSelection, -1);
		ReBindAxis (m_nSelection);
	}
	else if (m_nSelection == ID_JOYRIGHT)
	{
		ChangeButtonSettingSurface (m_nSelection, &m_nJoyRightSelection, -1);
		ReBindAxis (m_nSelection);
	}
	else if (m_nSelection == ID_JOYUP)
	{
		ChangeButtonSettingSurface (m_nSelection, &m_nJoyUpSelection, -1);
		ReBindAxis (m_nSelection);
	}
	else if (m_nSelection == ID_JOYDOWN)
	{
		ChangeButtonSettingSurface (m_nSelection, &m_nJoyDownSelection, -1);
		ReBindAxis (m_nSelection);
	}
	else if (m_nSelection >= ID_JOYBUTTONZERO && m_nSelection < ID_JOYBUTTONZERO + NUM_JOYBUTTONS)
	{
		int nButton = m_nSelection - ID_JOYBUTTONZERO;
		ChangeButtonSettingSurface (m_nSelection, &m_nButtonSelections[nButton], -1);
		BindButtonToCommand (nButton, m_nButtonSelections[nButton]);
	}
	
	CBaseMenu::Left();
}

void CJoystickMenu::Right()
{
	if (!m_pClientDE) return;

	CRiotSettings* pSettings = m_pRiotMenu->GetSettings();
	if (!pSettings) return;

	if (m_nSelection == ID_JOYLOOK || m_nSelection == ID_INVERTYAXIS)
	{
		Left();
		return;
	}
	else if (m_nSelection == ID_JOYLEFT)
	{
		ChangeButtonSettingSurface (m_nSelection, &m_nJoyLeftSelection, 1);
		ReBindAxis (m_nSelection);
	}
	else if (m_nSelection == ID_JOYRIGHT)
	{
		ChangeButtonSettingSurface (m_nSelection, &m_nJoyRightSelection, 1);
		ReBindAxis (m_nSelection);
	}
	else if (m_nSelection == ID_JOYUP)
	{
		ChangeButtonSettingSurface (m_nSelection, &m_nJoyUpSelection, 1);
		ReBindAxis (m_nSelection);
	}
	else if (m_nSelection == ID_JOYDOWN)
	{
		ChangeButtonSettingSurface (m_nSelection, &m_nJoyDownSelection, 1);
		ReBindAxis (m_nSelection);
	}
	else if (m_nSelection >= ID_JOYBUTTONZERO && m_nSelection < ID_JOYBUTTONZERO + NUM_JOYBUTTONS)
	{
		int nButton = m_nSelection - ID_JOYBUTTONZERO;
		ChangeButtonSettingSurface (m_nSelection, &m_nButtonSelections[nButton], 1);
		BindButtonToCommand (nButton, m_nButtonSelections[nButton]);
	}

	CBaseMenu::Right();
}

void CJoystickMenu::PageUp()
{
	CBaseMenu::PageUp();
}

void CJoystickMenu::PageDown()
{
	CBaseMenu::PageDown();
}

void CJoystickMenu::Home()
{
	CBaseMenu::Home();
}

void CJoystickMenu::End()
{
	CBaseMenu::End();
}

void CJoystickMenu::Return()
{
	if (!m_pRiotMenu) return;

	if (m_nSelection == ID_BACK)
	{
		m_pRiotMenu->SetCurrentMenu (m_pParent);
		CBaseMenu::Return();
	}
}

void CJoystickMenu::Esc()
{
	CBaseMenu::Esc();
}

void CJoystickMenu::Draw (HSURFACE hScreen, int nScreenWidth, int nScreenHeight, int nTextOffset)
{
	if (!m_pClientDE) return;

	CBaseMenu::Draw (hScreen, nScreenWidth, nScreenHeight, nTextOffset);
	
	int y = m_nMenuY + m_szMenuTitle.cy + m_nMenuTitleSpacing;
	for (int i = m_nTopItem; i < ID_BACK + 1; i++)
	{
		if (m_JoySettings[i].hMenuItem)
		{
			m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_nSelection == i ? m_JoySettings[i].hMenuItemSelected : m_JoySettings[i].hMenuItem, DNULL, m_nMenuX + m_nSecondColumn, y, DNULL);
		}
		y += m_GenericItem[i].szMenuItem.cy + m_nMenuSpacing;
		if (y > GetMenuAreaBottom() - (int)m_GenericItem[i].szMenuItem.cy) break;
	}
}

DBOOL CJoystickMenu::LoadSurfaces()
{
	if (!m_pClientDE || !m_pRiotMenu) return DFALSE;

	// get the settings class

	CRiotSettings* pSettings = m_pRiotMenu->GetSettings();
	if (!pSettings) return DFALSE;
	
	// create the menu surfaces

	CBitmapFont* pFontNormal = m_pRiotMenu->GetFont08n();
	CBitmapFont* pFontSelected = m_pRiotMenu->GetFont08s();
	CBitmapFont* pFontTitle = m_pRiotMenu->GetFont12n();

	m_GenericItem[ID_JOYLOOK].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, IDS_JOY_JOYLOOK);
	m_GenericItem[ID_INVERTYAXIS].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, IDS_JOY_INVERTYAXIS);
	m_GenericItem[ID_JOYUP].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, IDS_JOY_UP);
	m_GenericItem[ID_JOYDOWN].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, IDS_JOY_DOWN);
	m_GenericItem[ID_JOYLEFT].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, IDS_JOY_LEFT);
	m_GenericItem[ID_JOYRIGHT].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, IDS_JOY_RIGHT);
	for (int i = ID_JOYBUTTONZERO; i < ID_JOYBUTTONZERO + NUM_JOYBUTTONS; i++)
	{
		m_GenericItem[i].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, IDS_JOY_BUTTON1 + (i - ID_JOYBUTTONZERO));
	}
	m_GenericItem[ID_BACK].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, IDS_BACK);

	m_GenericItem[ID_JOYLOOK].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, IDS_JOY_JOYLOOK);
	m_GenericItem[ID_INVERTYAXIS].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, IDS_JOY_INVERTYAXIS);
	m_GenericItem[ID_JOYUP].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, IDS_JOY_UP);
	m_GenericItem[ID_JOYDOWN].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, IDS_JOY_DOWN);
	m_GenericItem[ID_JOYLEFT].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, IDS_JOY_LEFT);
	m_GenericItem[ID_JOYRIGHT].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, IDS_JOY_RIGHT);
	for (i = ID_JOYBUTTONZERO; i < ID_JOYBUTTONZERO + NUM_JOYBUTTONS; i++)
	{
		m_GenericItem[i].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, IDS_JOY_BUTTON1 + (i - ID_JOYBUTTONZERO));
	}
	m_GenericItem[ID_BACK].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, IDS_BACK);
	
	// take care of the settings surfaces

	int nStringID = pSettings->JoyLook() ? IDS_ON : IDS_OFF;
	m_JoySettings[ID_JOYLOOK].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, nStringID);
	m_JoySettings[ID_JOYLOOK].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, nStringID);

	// crop the joylook surface...
	//m_JoySettings[ID_JOYLOOK].hMenuItem = CropMenuItemTop (m_JoySettings[ID_JOYLOOK].hMenuItem);
	//m_JoySettings[ID_JOYLOOK].hMenuItemSelected = CropMenuItemTop (m_JoySettings[ID_JOYLOOK].hMenuItemSelected);
	
	m_pClientDE->GetSurfaceDims (m_JoySettings[ID_JOYLOOK].hMenuItem, &m_JoySettings[ID_JOYLOOK].szMenuItem.cx, &m_JoySettings[ID_JOYLOOK].szMenuItem.cy);

	nStringID = pSettings->JoyInvertY() ? IDS_YES : IDS_NO;
	m_JoySettings[ID_INVERTYAXIS].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, nStringID);
	m_JoySettings[ID_INVERTYAXIS].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, nStringID);
	m_pClientDE->GetSurfaceDims (m_JoySettings[ID_INVERTYAXIS].hMenuItem, &m_JoySettings[ID_INVERTYAXIS].szMenuItem.cx, &m_JoySettings[ID_INVERTYAXIS].szMenuItem.cy);

	// create button settings surfaces

	m_JoySettings[ID_JOYLEFT].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, g_CommandArray[m_nJoyLeftSelection].nStringID);
	m_JoySettings[ID_JOYLEFT].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, g_CommandArray[m_nJoyLeftSelection].nStringID);
	m_pClientDE->GetSurfaceDims (m_JoySettings[ID_JOYLEFT].hMenuItem, &m_JoySettings[ID_JOYLEFT].szMenuItem.cx, &m_JoySettings[ID_JOYLEFT].szMenuItem.cy);
	
	m_JoySettings[ID_JOYRIGHT].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, g_CommandArray[m_nJoyRightSelection].nStringID);
	m_JoySettings[ID_JOYRIGHT].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, g_CommandArray[m_nJoyRightSelection].nStringID);
	m_pClientDE->GetSurfaceDims (m_JoySettings[ID_JOYRIGHT].hMenuItem, &m_JoySettings[ID_JOYRIGHT].szMenuItem.cx, &m_JoySettings[ID_JOYRIGHT].szMenuItem.cy);

	m_JoySettings[ID_JOYUP].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, g_CommandArray[m_nJoyUpSelection].nStringID);
	m_JoySettings[ID_JOYUP].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, g_CommandArray[m_nJoyUpSelection].nStringID);
	m_pClientDE->GetSurfaceDims (m_JoySettings[ID_JOYUP].hMenuItem, &m_JoySettings[ID_JOYUP].szMenuItem.cx, &m_JoySettings[ID_JOYUP].szMenuItem.cy);

	m_JoySettings[ID_JOYDOWN].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, g_CommandArray[m_nJoyDownSelection].nStringID);
	m_JoySettings[ID_JOYDOWN].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, g_CommandArray[m_nJoyDownSelection].nStringID);
	m_pClientDE->GetSurfaceDims (m_JoySettings[ID_JOYDOWN].hMenuItem, &m_JoySettings[ID_JOYDOWN].szMenuItem.cx, &m_JoySettings[ID_JOYDOWN].szMenuItem.cy);

	for (i = ID_JOYBUTTONZERO; i < ID_JOYBUTTONZERO + NUM_JOYBUTTONS; i++)
	{
		m_JoySettings[i].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, g_CommandArray[m_nButtonSelections[i - ID_JOYBUTTONZERO]].nStringID);
		m_JoySettings[i].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, g_CommandArray[m_nButtonSelections[i - ID_JOYBUTTONZERO]].nStringID);
		m_pClientDE->GetSurfaceDims (m_JoySettings[i].hMenuItem, &m_JoySettings[i].szMenuItem.cx, &m_JoySettings[i].szMenuItem.cy);
	}

	m_hMenuTitle = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontTitle, IDS_TITLE_JOYSTICK);
	m_pClientDE->GetSurfaceDims (m_hMenuTitle, &m_szMenuTitle.cx, &m_szMenuTitle.cy);
	
	// check to see that all the surfaces were created successfully

	for (i = 0; i <= ID_BACK; i++)
	{
		if (!m_GenericItem[i].hMenuItem || !m_GenericItem[i].hMenuItemSelected)
		{
			UnloadSurfaces();
			return DFALSE;
		}

		if (i == ID_BACK) continue;

		if (!m_JoySettings[i].hMenuItem || !m_JoySettings[i].hMenuItemSelected)
		{
			UnloadSurfaces();
			return DFALSE;
		}
	}
	
	// get the main surface sizes

	for (i = 0; i <= ID_BACK; i++)
	{
		m_pClientDE->GetSurfaceDims (m_GenericItem[i].hMenuItem, &m_GenericItem[i].szMenuItem.cx, &m_GenericItem[i].szMenuItem.cy);
	}
	
	return CBaseMenu::LoadSurfaces();
}

void CJoystickMenu::UnloadSurfaces()
{
	if (!m_pClientDE) return;

	for (int i = 0; i < ID_BACK; i++)
	{
		if (m_GenericItem[i].hMenuItem) m_pClientDE->DeleteSurface (m_GenericItem[i].hMenuItem);
		if (m_GenericItem[i].hMenuItemSelected) m_pClientDE->DeleteSurface (m_GenericItem[i].hMenuItemSelected);
		m_GenericItem[i].hMenuItem = DNULL;
		m_GenericItem[i].hMenuItemSelected = DNULL;
		m_GenericItem[i].szMenuItem.cx = m_GenericItem[i].szMenuItem.cy = 0;

		if (m_JoySettings[i].hMenuItem) m_pClientDE->DeleteSurface (m_JoySettings[i].hMenuItem);
		if (m_JoySettings[i].hMenuItemSelected) m_pClientDE->DeleteSurface (m_JoySettings[i].hMenuItemSelected);
		m_JoySettings[i].hMenuItem = DNULL;
		m_JoySettings[i].hMenuItemSelected = DNULL;
	}

	if (m_hMenuTitle) m_pClientDE->DeleteSurface (m_hMenuTitle);
	m_hMenuTitle = DNULL;
	
	CBaseMenu::UnloadSurfaces();
}

void CJoystickMenu::PostCalculateMenuDims()
{
	if (!m_pClientDE) return;

	// get the maximum width of the menu

	int nMenuMaxWidth = 0;
	DDWORD nSettingWidth, nSettingHeight;
	for (int i = ID_JOYLOOK; i <= ID_BACK; i++)
	{
		m_pClientDE->GetSurfaceDims (m_JoySettings[i].hMenuItem, &nSettingWidth, &nSettingHeight);
		if (m_nSecondColumn + (int)nSettingWidth > nMenuMaxWidth)
		{
			nMenuMaxWidth = m_nSecondColumn + nSettingWidth;
		}
	}

	m_nMenuX = 0;
	//if (m_pRiotMenu->InWorld() || m_szScreen.cx < 512)
	//{
		m_nMenuX = GetMenuAreaLeft() + ((int)m_szMenuArea.cx - nMenuMaxWidth) / 2;
	//}
	//else
	//{
	//	m_nMenuX = GetMenuAreaLeft() + ((int)m_szMenuArea.cx / 2);
	//}
}

DBOOL CJoystickMenu::ChangeButtonSettingSurface (int nMenuSelection, int* pSelection, int nChange)
{
	if (!pSelection || !m_pClientDE || !m_pRiotMenu) return DFALSE;

	CBitmapFont* pFontNormal = m_pRiotMenu->GetFont08n();
	CBitmapFont* pFontSelected = m_pRiotMenu->GetFont08s();
	
	if (m_JoySettings[nMenuSelection].hMenuItem) m_pClientDE->DeleteSurface (m_JoySettings[nMenuSelection].hMenuItem);
	if (m_JoySettings[nMenuSelection].hMenuItemSelected) m_pClientDE->DeleteSurface (m_JoySettings[nMenuSelection].hMenuItemSelected);
	
	*pSelection += nChange;
	if (*pSelection < 0) *pSelection = NUM_COMMANDS - 1;
	if (*pSelection >= NUM_COMMANDS) *pSelection = 0;

	m_JoySettings[nMenuSelection].hMenuItem = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontNormal, g_CommandArray[*pSelection].nStringID);
	m_JoySettings[nMenuSelection].hMenuItemSelected = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontSelected, g_CommandArray[*pSelection].nStringID);
	
	m_pClientDE->GetSurfaceDims (m_JoySettings[nMenuSelection].hMenuItem, &m_JoySettings[nMenuSelection].szMenuItem.cx, &m_JoySettings[nMenuSelection].szMenuItem.cy);
	
	return (m_JoySettings[nMenuSelection].hMenuItem != DNULL && m_JoySettings[nMenuSelection].hMenuItemSelected != DNULL);
}

DBOOL CJoystickMenu::ReBindAxis (int nDirection)
{
	if (!m_pClientDE) return DFALSE;
	
	// find the axis

	DeviceObject* pObjects = m_pClientDE->GetDeviceObjects (DEVICETYPE_JOYSTICK);
	DeviceObject* pObj = pObjects;
	DBOOL bFoundIt = DFALSE;
	while (pObj)
	{
		switch (nDirection)
		{
			case ID_JOYUP:		
			case ID_JOYDOWN:		if (pObj->m_ObjectType == CONTROLTYPE_YAXIS) bFoundIt = DTRUE; break;

			case ID_JOYLEFT:
			case ID_JOYRIGHT:		if (pObj->m_ObjectType == CONTROLTYPE_XAXIS) bFoundIt = DTRUE; break;
		}

		if (bFoundIt) break;

		pObj = pObj->m_pNext;
	}
	if (!pObj)
	{
		m_pClientDE->FreeDeviceObjects (pObjects);
		return DFALSE;
	}

	// set new binding

	float nRange = pObj->m_RangeHigh - pObj->m_RangeLow;
	float nHalfRange = nRange / 2.0f;
	float nActiveRange = nHalfRange - (0.16f * nRange);

	char str[128];
	sprintf (str, "rangebind \"%s\" \"%s\" 0 0 \"\"", pObj->m_DeviceName, pObj->m_ObjectName);
	if (!JoystickMenuDisabled())	m_pClientDE->RunConsoleString (str);
	
	char strCommand1[64];
	char strCommand2[64];
	switch (nDirection)
	{
		case ID_JOYUP:
		case ID_JOYDOWN:
		{
			SAFE_STRCPY(strCommand1, CommandName (g_CommandArray[m_nJoyUpSelection].nCommandID));
			SAFE_STRCPY(strCommand2, CommandName (g_CommandArray[m_nJoyDownSelection].nCommandID));
			sprintf (str, "rangebind \"%s\" \"%s\" \"%li\" \"%li\" \"%s\" \"%li\" \"%li\" \"%s\"", pObj->m_DeviceName, pObj->m_ObjectName,
					 (long)pObj->m_RangeLow, (long)(pObj->m_RangeLow + nActiveRange), strCommand1,
					 (long)(pObj->m_RangeHigh - nActiveRange), (long)pObj->m_RangeHigh, strCommand2);
			if (!JoystickMenuDisabled())	m_pClientDE->RunConsoleString (str);   
		}
		break;

		case ID_JOYLEFT:
		case ID_JOYRIGHT:
		{
			SAFE_STRCPY(strCommand1, CommandName (g_CommandArray[m_nJoyLeftSelection].nCommandID));
			SAFE_STRCPY(strCommand2, CommandName (g_CommandArray[m_nJoyRightSelection].nCommandID));
			sprintf (str, "rangebind \"%s\" \"%s\" \"%li\" \"%li\" \"%s\" \"%li\" \"%li\" \"%s\"", pObj->m_DeviceName, pObj->m_ObjectName, 
					 (long)pObj->m_RangeLow, (long)(pObj->m_RangeLow + nActiveRange), strCommand1,
					 (long)(pObj->m_RangeHigh - nActiveRange), (long)pObj->m_RangeHigh, strCommand2);
			if (!JoystickMenuDisabled())	m_pClientDE->RunConsoleString (str);   
		}
		break;
	}

	m_pClientDE->FreeDeviceObjects (pObjects);

	return DTRUE;
}

DBOOL CJoystickMenu::BindButtonToCommand (int nButton, int nSelection)
{
	if (!m_pClientDE) return DFALSE;

	// find the button

	DeviceObject* pObjects = m_pClientDE->GetDeviceObjects (DEVICETYPE_JOYSTICK);
	DeviceObject* pObj = pObjects;
	int nButtonCount = 0;
	while (pObj)
	{
		if (pObj->m_ObjectType == CONTROLTYPE_BUTTON)
		{
			if (nButtonCount == nButton) break;
			nButtonCount++;
		}

		pObj = pObj->m_pNext;
	}
	if (!pObj) 
	{
		m_pClientDE->FreeDeviceObjects (pObjects);
		return DFALSE;
	}

	// remove old binding and set new binding

	char str[128];
	sprintf (str, "rangebind \"%s\" \"%s\" 0 0 \"\"", pObj->m_DeviceName, pObj->m_ObjectName);
	if (!JoystickMenuDisabled())	m_pClientDE->RunConsoleString (str);   
	
	sprintf (str, "rangebind \"%s\" \"%s\" 0 0 \"%s\"", pObj->m_DeviceName, pObj->m_ObjectName, CommandName (g_CommandArray[nSelection].nCommandID));
	if (!JoystickMenuDisabled())	m_pClientDE->RunConsoleString (str);   
	
	m_pClientDE->FreeDeviceObjects (pObjects);

	return DTRUE;
}

DBOOL CJoystickMenu::ImplementJoyLook (CClientDE* pClientDE, DBOOL bJoyLook)
{
	if (!pClientDE) return DFALSE;

	// get device name

	char strJoystick[64];
	memset (strJoystick, 0, 64);
	pClientDE->GetDeviceName (DEVICETYPE_JOYSTICK, strJoystick, 63);
	if (strJoystick[0] == '\0') return DFALSE;

	// get axis object names

	char strXAxis[64];
	char strYAxis[64];
	memset (strXAxis, 0, 64);
	memset (strYAxis, 0, 64);

	DeviceObject* pObjects = pClientDE->GetDeviceObjects (DEVICETYPE_JOYSTICK);
	DeviceObject* pObj = pObjects;
	int nButton = 0;
	while (pObj)
	{
		if (pObj->m_ObjectType == CONTROLTYPE_XAXIS)
		{
			SAFE_STRCPY(strXAxis, pObj->m_ObjectName);
		}

		if (pObj->m_ObjectType == CONTROLTYPE_YAXIS)
		{
			SAFE_STRCPY(strYAxis, pObj->m_ObjectName);
		}

		pObj = pObj->m_pNext;
	}
	pClientDE->FreeDeviceObjects (pObjects);

	if (strYAxis[0] == '\0' || strXAxis[0] == '\0') return DFALSE;

	// implement joy look command

	float nXRange = m_nXAxisMax - m_nXAxisMin;
	float nXHalfRange = nXRange / 2.0f;
	float nXActiveRange = nXHalfRange - (0.16f * nXRange);

	float nYRange = m_nYAxisMax - m_nYAxisMin;
	float nYHalfRange = nYRange / 2.0f;
	float nYActiveRange = nYHalfRange - (0.16f * nYRange);
	
	char str[128];
	if (bJoyLook)
	{
		// clear bindings for the axes
		sprintf (str, "rangebind \"%s\" \"%s\" 0 0 \"\"", strJoystick, strXAxis);
		if (!JoystickMenuDisabled())	pClientDE->RunConsoleString (str);  
		sprintf (str, "rangebind \"%s\" \"%s\" 0 0 \"\"", strJoystick, strYAxis);
		if (!JoystickMenuDisabled())	pClientDE->RunConsoleString (str);  

		char strCommand1[64];
		char strCommand2[64];
		
		// implement new bindings
		SAFE_STRCPY(strCommand1, CommandName (g_CommandArray[CommandToArrayPos(COMMAND_ID_LEFT)].nCommandID));
		SAFE_STRCPY(strCommand2, CommandName (g_CommandArray[CommandToArrayPos(COMMAND_ID_RIGHT)].nCommandID));
		sprintf (str, "rangebind \"%s\" \"%s\" \"%li\" \"%li\" \"%s\" \"%li\" \"%li\" \"%s\"", strJoystick, strXAxis, 
				 (long)m_nXAxisMin, (long)(m_nXAxisMin + nXActiveRange), strCommand1,
				 (long)(m_nXAxisMax - nXActiveRange), (long)m_nXAxisMax, strCommand2);
		if (!JoystickMenuDisabled())	pClientDE->RunConsoleString (str);  
		
		SAFE_STRCPY(strCommand1, CommandName (g_CommandArray[CommandToArrayPos(COMMAND_ID_LOOKUP)].nCommandID));
		SAFE_STRCPY(strCommand2, CommandName (g_CommandArray[CommandToArrayPos(COMMAND_ID_LOOKDOWN)].nCommandID));
		sprintf (str, "rangebind \"%s\" \"%s\" \"%li\" \"%li\" \"%s\" \"%li\" \"%li\" \"%s\"", strJoystick, strYAxis, 
				 (long)m_nYAxisMin, (long)(m_nYAxisMin + nYActiveRange), strCommand1,
				 (long)(m_nYAxisMax - nYActiveRange), (long)m_nYAxisMax, strCommand2);
		if (!JoystickMenuDisabled())	pClientDE->RunConsoleString (str);  
	}
	else
	{
		// clear bindings for the axes
		sprintf (str, "rangebind \"%s\" \"%s\" 0 0 \"\"", strJoystick, strXAxis);
		if (!JoystickMenuDisabled())	pClientDE->RunConsoleString (str);
		sprintf (str, "rangebind \"%s\" \"%s\" 0 0 \"\"", strJoystick, strYAxis);
		if (!JoystickMenuDisabled())	pClientDE->RunConsoleString (str);

		char strCommand1[64];
		char strCommand2[64];
		
		// implement new bindings
		SAFE_STRCPY(strCommand1, CommandName (g_CommandArray[m_nJoyLeftSelection].nCommandID));
		SAFE_STRCPY(strCommand2, CommandName (g_CommandArray[m_nJoyRightSelection].nCommandID));
		sprintf (str, "rangebind \"%s\" \"%s\" \"%li\" \"%li\" \"%s\" \"%li\" \"%li\" \"%s\"", strJoystick, strXAxis, 
				 (long)m_nXAxisMin, (long)(m_nXAxisMin + nXActiveRange), strCommand1,
				 (long)(m_nXAxisMax - nXActiveRange), (long)m_nXAxisMax, strCommand2);
		if (!JoystickMenuDisabled())	pClientDE->RunConsoleString (str);  
		
		SAFE_STRCPY(strCommand1, CommandName (g_CommandArray[m_nJoyUpSelection].nCommandID));
		SAFE_STRCPY(strCommand2, CommandName (g_CommandArray[m_nJoyDownSelection].nCommandID));
		sprintf (str, "rangebind \"%s\" \"%s\" \"%li\" \"%li\" \"%s\" \"%li\" \"%li\" \"%s\"", strJoystick, strYAxis,
				 (long)m_nYAxisMin, (long)(m_nYAxisMin + nYActiveRange), strCommand1,
				 (long)(m_nYAxisMax - nYActiveRange), (long)m_nYAxisMax, strCommand2);
		if (!JoystickMenuDisabled())	pClientDE->RunConsoleString (str);
	}

	return DTRUE;
}


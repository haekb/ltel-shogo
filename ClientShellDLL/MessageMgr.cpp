//------------------------------------------------------------------------
//
// MODULE  : MessageMgr.h
//
// PURPOSE : Riot Messaging system - ruthlessly stolen from Greg Kettell
//
// CREATED : 10/22/97
//
//------------------------------------------------------------------------

#include "RiotClientShell.h"
#include "MessageMgr.h"
#include "VKDefs.h"
#include "stdio.h"
#include "RiotMsgIDs.h"
#include "RiotSoundTypes.h"
#include "ClientRes.h"

#define FONT_HEIGHT 12
#define FONT_WIDTH 6

CMessageMgr*	g_pMessageMgr = DNULL;
CCheatMgr*		g_pCheatMgr = DNULL;

extern CRiotClientShell*	g_pRiotClientShell;


CCheatMgr::CheatInfo CCheatMgr::s_CheatInfo[] = {
	{ "LPDKA",			DFALSE },	// mpgod
	{ "LPNIHI",			DFALSE },	// mpammo
	{ "LPNPHI]",		DFALSE },	// mparmor
	{ "LPWALH_R",		DFALSE },	// mphealth
	{ "LP@JT\\",		DFALSE },	// mpclip
	{ "LPSAIO[W[\\",	DFALSE },	// mpteleport
	{ "LPLLCIH^",		DFALSE },	// mponfoot
	{ "LPUAUSLVL",		DFALSE },	// mpvehicle
	{ "LPJANT",			DFALSE },	// mpmech
	{ "LP_K^",			DFALSE },	// mppos
	{ "LPHDL",			DFALSE },	// mpkfa
	{ "LPJUHSVQ",		DFALSE },	// mpmimimi
	{ "LPT^J]",			DFALSE },	// mpwpos	- toggle position weapon
	{ "LPTI]I\\",		DFALSE },	// mpwmpos	- toggle position weapon muzzle
	{ "LP_I",			DFALSE },	// mppm		- toggle player movement tweak
	{ "LP_M",			DFALSE },	// mppa		- toggle player movement accel
	{ "LP@MHO]I",		DFALSE },	// mpcamera	- toggle camera offset adjustment
	{ "LPKUBT_[JWSI",	DFALSE },	// mplightscale	- toggle light scale adjustment
	{ "LPAUBA^HZ",		DFALSE },	// mpbigguns	- toggle big guns cheat
	{ "LPSAL^\\",		DFALSE },	// mptears		- All weapons, infinite ammo
	{ "LPAKdS\\]JQ",	DFALSE },	// mpboyisuck	- Remove all AI in the level
	{ "LPSPTA@M[Y",		DFALSE },	// mptriggers	- Toggle trigger boxes on/off
	{ "LPNLTWN",		DFALSE },	// mpanime		- Toggle anime special fx on/off

	// Special Thanks cheats....

	{ "LP@V_S\\RLLIIgI",		DFALSE },	// mpchrishedberg
	{ "LPPMH]HH",				DFALSE },	// mpsamson
	{ "LPJANTJUHV",				DFALSE },	// mpmechaman
	{ "LPBT@^bVP\\[RHX[Y_U",	DFALSE },	// mpeverylittlething
	{ "LPCMSSOHL_S",			DFALSE },	// mpdavidneil
	{ "LPJUIS_IW\\",			DFALSE },	// mpmilitant
	{ "LPNBJ",					DFALSE },	// mpado
	{ "LPSV@VHSLZ",				DFALSE },	// mpthejoker
	{ "LPJMBScHPM_Z",			DFALSE },	// mpmagixnight
	{ "LPAMOK]ISWQY_g",			DFALSE },	// mpbabarajajuju
	{ "LPSe_SJH",				DFALSE },	// mptyrian
	{ "LP_MIOAIJK",				DFALSE },	// mppaleface
	{ "LPSV@TJHM",				DFALSE },	// mpthehand
	{ "LPCM_cW[\\ZVZ",			DFALSE },	// mpdarylsurat	
	{ "LPHM_OW",				DFALSE },	// mpkarel
	{ "LPBRUKIL\\\\[IgJZU]T",	DFALSE },	// mpethanbutterfield
	{ "LPHMWS",					DFALSE },	// mpkaji
	{ "LPHMJ_W",				DFALSE },	// mpkaoul
	{ "LPPVTJSQRWYY",			DFALSE },	// mpshinhikaru

	// Others...

	{ "LPQKOO]^",			DFALSE },	// mprobert
};	


DBOOL CCheatMgr::m_bPlayerCheated = DFALSE;



/*******************************************************************************

	CMessageMgr

*******************************************************************************/

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMessageMgr::CMessageMgr()
//
//	PURPOSE:	constructor
//
// ----------------------------------------------------------------------- //
			
CMessageMgr::CMessageMgr()
{
	g_pMessageMgr = this;
	m_x	= 15;
	m_y	= 0;

	m_pClientDE = DNULL;
	m_pClientShell = DNULL;

	m_nMaxMessages	= kDefaultMaxMessages;
	m_fMessageTime	= kDefaultMessageTime;
	m_dwMessageFlags = MESSAGEFLAGS_MASK;

	m_nMessageCount	= 0;
	m_nFirstMessage	= 0;
	m_nNextMessage	= m_nFirstMessage;
	m_bEditing = DFALSE;

	m_bEnabled = DFALSE;

	m_hFont = DNULL;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMessageMgr::Init (CClientDE* pClientDE)
//
//	PURPOSE:	Initializes the Message manager
//
// ----------------------------------------------------------------------- //

DBOOL CMessageMgr::Init (CClientDE* pClientDE, CRiotClientShell* pClientShell)
{
	if (!pClientDE || !pClientShell) return DFALSE;

	m_pClientDE = pClientDE;
	m_pClientShell = pClientShell;
	if (!m_InputLine.Init(pClientDE, pClientShell)) return DFALSE;

	HSTRING hstrFont = pClientDE->FormatString (IDS_INGAMEFONT);
	m_hFont = m_pClientDE->CreateFont(pClientDE->GetStringData (hstrFont), FONT_WIDTH, FONT_HEIGHT, DFALSE, DFALSE, DFALSE);
	pClientDE->FreeString (hstrFont);

	return DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMessageMgr::Enable()
//
//	PURPOSE:	Sets the enabled flag
//
// ----------------------------------------------------------------------- //
			
void CMessageMgr::Enable( DBOOL bEnabled )
{
	if (m_bEnabled && !bEnabled)
		Clear();

	m_bEnabled = bEnabled;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMessageMgr::AddLine()
//
//	PURPOSE:	Adds a new line to the Message buffer
//
// ----------------------------------------------------------------------- //
			
void CMessageMgr::AddLine( char *szMsg, DBYTE uMessageFlag )
{
	if (szMsg)
	{
		HSTRING hString = m_pClientDE->CreateString(szMsg);
		AddLine(hString, uMessageFlag);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMessageMgr::AddLine()
//
//	PURPOSE:	Adds a new line to the Message buffer
//
// ----------------------------------------------------------------------- //
			
void CMessageMgr::AddLine( HSTRING hMsg, DBYTE uMessageFlag )
{
	// don't bother adding Messages disallowed by the user
	if ( !(uMessageFlag && m_dwMessageFlags) )
		return;

	HDECOLOR hForeground = m_pClientDE->SetupColor1 (1.0f, 1.0f, 1.0f, DFALSE);

	Message *pMsg = &m_Messages[ m_nNextMessage ];

	// Set next Message pointer
	m_nNextMessage = (m_nNextMessage + 1) % kMaxMessages;

	// don't overflow the maximum number of Messages
	if ( ++m_nMessageCount > m_nMaxMessages )
	{
		// refresh the screen to get rid of oldest message
		g_pRiotClientShell->AddToClearScreenCount();

		// expire the oldest Message and recalculate the first Message in the buffer
		Message *pOld = &m_Messages[ m_nFirstMessage ];

		DeleteMessageData(pOld);

		m_nFirstMessage = (m_nFirstMessage + 1) % kMaxMessages;

		// reset the Message count to max
		m_nMessageCount = m_nMaxMessages;
	}

	pMsg->hSurface = m_pClientDE->CreateSurfaceFromString (m_hFont, hMsg, hForeground, NULL, 10, 0);
	if (!pMsg->hSurface)  // Error
	{
		return;
	}

	HDECOLOR hTransColor = m_pClientDE->SetupColor1(0.0f, 0.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (pMsg->hSurface, hTransColor);

	pMsg->hMessage = hMsg;
	pMsg->fExpiration = m_pClientDE->GetTime() + m_fMessageTime;
	
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMessageMgr::Draw()
//
//	PURPOSE:	Displays the Message list
//
// ----------------------------------------------------------------------- //
			
void CMessageMgr::Draw( void )
{
	// early return if there are no Messages in the queue,
	// or Messages are disabled, or while typing Messages
	if ( !m_bEditing && (m_nMessageCount == 0 || !m_bEnabled) )
		return;

	int nCount = m_nMessageCount;	// use temporary values
	int nFirst = m_nFirstMessage;	// because the real ones may change
	int i;

//	int nShade = ClipHigh(nCount * 8, 48);
//	g_pRiotClientShell->AddToClearScreenCount();

	HSURFACE hScreen = m_pClientDE->GetScreenSurface();
	DDWORD nScreenHeight, nScreenWidth;
	m_pClientDE->GetSurfaceDims (hScreen, &nScreenWidth, &nScreenHeight);

	float xRatio			= (float)nScreenWidth / 640.0f;
	float yRatio			= (float)nScreenHeight / 480.0f;
	
	int y = 0;
	int x = 3;

	for ( i = 0; i < nCount; i++)
	{
		Message *pMsg = &m_Messages[ (nFirst + i) % kMaxMessages ];

		// expire Messages that have overstayed their welcome
		if ( m_pClientDE->GetTime() >= pMsg->fExpiration )
		{
			g_pRiotClientShell->AddToClearScreenCount();

			DeleteMessageData(pMsg);
			// expire the oldest Message and recalculate the Message position and counter
			m_nFirstMessage = (m_nFirstMessage + 1) % kMaxMessages;
			m_nMessageCount--;

			// don't draw the expired Message
			continue;
		}

		// draw the text
		m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, pMsg->hSurface, NULL, x, y, NULL);
		y += FONT_HEIGHT;
	}

	if (m_bEditing)
	{
		int y = (int) (yRatio * 420.0f);
		HDECOLOR hForeground = m_pClientDE->SetupColor1 (1.0f, 1.0f, 1.0f, DFALSE);
		HDECOLOR hBackground = m_pClientDE->SetupColor1 (0.0f, 0.0f, 0.0f, DTRUE);
		
		//m_InputLine.Draw(hScreen, m_hFont, hForeground, NULL, m_x, y - FONT_HEIGHT);
		
		// {MD 9/13/98}
		m_InputLine.Draw(hScreen, m_hFont, hForeground, SETRGB_T(0,0,0), m_x, y - FONT_HEIGHT);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMessageMgr::Clear()
//
//	PURPOSE:	removes all Messages from the array
//
// ----------------------------------------------------------------------- //
			
void CMessageMgr::Clear( void )
{
	int i;
	for ( i = 0; i < m_nMessageCount; i++)
	{
		Message *pMsg = &m_Messages[ (m_nFirstMessage + i) % kMaxMessages ];
		DeleteMessageData(pMsg);
	}

	m_nFirstMessage = m_nNextMessage = m_nMessageCount = 0;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMessageMgr::SetMaxMessages()
//
//	PURPOSE:	Sets the maximum number of Messages to display
//
// ----------------------------------------------------------------------- //
			
void CMessageMgr::SetMaxMessages( int nMaxMessages )
{
	if (nMaxMessages < 1) nMaxMessages = 1;
	if (nMaxMessages > kMaxMessages) nMaxMessages = kMaxMessages;

	m_nMaxMessages = nMaxMessages;
}


void CMessageMgr::SetCoordinates( int x, int y )
{
	m_x = x;
	m_y = y;
}

void CMessageMgr::SetMessageTime( DFLOAT fSeconds )
{
	m_fMessageTime = fSeconds;
}

void CMessageMgr::SetMessageFlags( DDWORD dwFlags )
{
	m_dwMessageFlags = dwFlags & MESSAGEFLAGS_MASK;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMessageMgr::HandleKeyDown()
//
//	PURPOSE:	Processes keystrokes
//
// ----------------------------------------------------------------------- //
			
DBOOL CMessageMgr::HandleKeyDown(int key, int rep)
{
	if (m_bEditing)
		return m_InputLine.HandleKeyDown(key, rep);
	else
		return DFALSE;
}



/*******************************************************************************

	CInputLine

*******************************************************************************/


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::Init (CClientDE* pClientDE)
//
//	PURPOSE:	Initializes the player Message
//
// ----------------------------------------------------------------------- //

DBOOL CInputLine::Init (CClientDE* pClientDE, CRiotClientShell* pClientShell)
{
	if (!pClientDE || !pClientShell) return DFALSE;

	m_pClientDE = pClientDE;
	m_pClientShell = pClientShell;

	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::CInputLine()
//
//	PURPOSE:	constructor
//
// ----------------------------------------------------------------------- //
			
void CInputLine::Clear( void )
{
	*m_zText = '\0';	// nil the Message
	m_nTextLen = 0;	// zero the length
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::CInputLine()
//
//	PURPOSE:	constructor
//
// ----------------------------------------------------------------------- //
			
void CInputLine::Term( void )
{
	Clear();
	g_pMessageMgr->SetEditingState(DFALSE);
	m_pClientDE->SetInputState(DTRUE);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::CInputLine()
//
//	PURPOSE:	constructor
//
// ----------------------------------------------------------------------- //
			
void CInputLine::Draw(HSURFACE hDest, HDEFONT hFont, HDECOLOR hForeColor, HDECOLOR hBackColor, long x, long y)
{
	if (!m_pClientDE) return;

	char zTemp[ kMaxInputLine + 2 + 4];
	
	HSTRING hStr = m_pClientDE->FormatString (IDS_SAY);
	SAFE_STRCPY(zTemp, m_pClientDE->GetStringData (hStr));		// PUT IN STRING RESOURCE!!
	m_pClientDE->FreeString (hStr);

	strcat(zTemp, m_zText);

	double fTime = m_pClientDE->GetTime();
	double tmp;

	fTime = modf(fTime, &tmp);
	if (fTime > 0.5f)
		strcat(zTemp,"_");	// add a cursor

	HSTRING hString = m_pClientDE->CreateString(zTemp);


	// Draws the string.
	HSURFACE hSurface = m_pClientDE->CreateSurfaceFromString(hFont, hString, hForeColor, hBackColor, 0, 0);
	m_pClientDE->OptimizeSurface (hSurface, hBackColor);
	m_pClientDE->DrawSurfaceToSurfaceTransparent(hDest, hSurface, NULL, x, y, hBackColor);
	m_pClientDE->DeleteSurface(hSurface);
	m_pClientDE->FreeString(hString);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::CInputLine()
//
//	PURPOSE:	constructor
//
// ----------------------------------------------------------------------- //
			
DBOOL CInputLine::AddChar( DBYTE ch )
{
	if (m_nTextLen < kMaxInputLine)	// space left in the Message
	{
		m_zText[m_nTextLen] = ch;	// add a character
		m_nTextLen++;				// increment the length
		m_zText[m_nTextLen] = '\0';	// terminate the string
		return DTRUE;				// indicate success
	}
	return DFALSE;	// indicate failure
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::CInputLine()
//
//	PURPOSE:	constructor
//
// ----------------------------------------------------------------------- //
			
void CInputLine::DelChar( void )
{
	if (m_nTextLen > 0)	// text in the Message?
	{
		m_nTextLen--;				// back up a character
		m_zText[m_nTextLen] = '\0';	// terminate the string
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::CInputLine()
//
//	PURPOSE:	constructor
//
// ----------------------------------------------------------------------- //
			
void CInputLine::Set( char *pzText )
{
	strncpy(m_zText, pzText, kMaxInputLine);				// copy the text
	m_zText[kMaxInputLine] = '\0';							// enforce null termination
	m_nTextLen = strlen(pzText);							// enforce max length
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::CInputLine()
//
//	PURPOSE:	constructor
//
// ----------------------------------------------------------------------- //
			
void CInputLine::Send( void )
{
	if (!m_pClientDE || !m_pClientShell) return;

	// get the user's name
	char strName[128];
	memset (strName, 0, 128);

	DDWORD nLocalID = 0;
	m_pClientDE->GetLocalClientID (&nLocalID);

	int nGameMode = 0;
	m_pClientDE->GetGameMode(&nGameMode);
	if (nGameMode == STARTGAME_NORMAL || nGameMode == GAMEMODE_NONE || nLocalID == DE_NOTCONNECTED) 
	{
		HSTRING hStr = m_pClientDE->FormatString (IDS_SANJURO);
		strncpy (strName, m_pClientDE->GetStringData (hStr), 127);
		m_pClientDE->FreeString (hStr);
	}
	else
	{
		CClientInfoMgr* pClientInfoMgr = m_pClientShell->GetClientInfoMgr();
		strncpy (strName, pClientInfoMgr->GetPlayerName (nLocalID), 127);
	}

	
	// create the message
	char strMessage[kMaxInputLine + 56];
	strncpy (strMessage, strName, sizeof(strMessage)-1);
	strcat (strMessage, ": ");
	strcat (strMessage, m_zText);

	// Send the Message to the server
	HMESSAGEWRITE hMsg = m_pClientDE->StartMessage(MID_PLAYER_MESSAGE);
	m_pClientDE->WriteToMessageString(hMsg, strMessage);
	m_pClientDE->EndMessage(hMsg);

	Term();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::CInputLine()
//
//	PURPOSE:	constructor
//
// ----------------------------------------------------------------------- //
			
DBOOL CInputLine::HandleKeyDown(int key, int rep)
{
	switch( key )
	{
		case VK_ESCAPE:
			{
				Term();
			}
			break;

		case VK_HOME:
			{
				Clear();
			}
			break;

		case VK_BACK:
			{
				DelChar();
			}
			break;

		case VK_SHIFT:
			{
				m_bShift = DTRUE;
			}
			break;

		case VK_UP:
			{
				if (m_nCurrentRecentLine == m_nBaseRecentLine + 1 ||
					(m_nCurrentRecentLine == 0 && m_nBaseRecentLine == NUM_RECENT_LINES - 1))
				{
					break;
				}
				
				int nTest = 0;
				if (m_nCurrentRecentLine == -1)
				{
					nTest = m_nBaseRecentLine;
				}
				else
				{
					nTest = m_nCurrentRecentLine - 1;
				}
				if (nTest < 0) nTest = NUM_RECENT_LINES - 1;
				
				if (!m_bRecentLineUsed[nTest]) break;
				m_nCurrentRecentLine = nTest;
				SAFE_STRCPY(m_zText, m_pRecentLines[m_nCurrentRecentLine]);
			}
			break;

		case VK_DOWN:
			{
				if (m_nCurrentRecentLine == m_nBaseRecentLine || m_nCurrentRecentLine == -1)
				{
					break;
				}

				m_nCurrentRecentLine++;
				if (m_nCurrentRecentLine == NUM_RECENT_LINES) m_nCurrentRecentLine = 0;
				SAFE_STRCPY(m_zText, m_pRecentLines[m_nCurrentRecentLine]);
			}
			break;

		case VK_RETURN:
			{
				AddToRecentList();

				if ( g_pCheatMgr->Check(m_zText) )
				{
					PlaySoundLocal( "Sounds\\Interface\\Cheat.wav", SOUNDPRIORITY_MISC_MEDIUM);
					Term();
				}
				else
				{
					Send();
				}
				g_pClientDE->ClearInput(); // Don't process the key they hit...
			}
			break;

		default:
			{
				char ch;
				if (ch = AsciiXlate(key))
				{
					if (!AddChar(ch) )
					{
					}
				}
			}
			break;
	}
	return DTRUE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::HandleKeyUp()
//
//	PURPOSE:	Handles a key up Message, used for tracking shift keys
//
// ----------------------------------------------------------------------- //
			
void CInputLine::HandleKeyUp(int key)
{
	switch( key )
	{
		case VK_SHIFT:
			m_bShift = DFALSE;
			break;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::AddToRecentList()
//
//	PURPOSE:	Adds the current string to the recent-string list
//
// ----------------------------------------------------------------------- //
			
void CInputLine::AddToRecentList()
{
	// add the current line to the array of past input lines

	m_nBaseRecentLine++;
	if (m_nBaseRecentLine >= NUM_RECENT_LINES) 
		m_nBaseRecentLine = 0;
	else if(m_nBaseRecentLine < 0)
		m_nBaseRecentLine = 0;
	
	SAFE_STRCPY(m_pRecentLines[m_nBaseRecentLine], m_zText);
	m_bRecentLineUsed[m_nBaseRecentLine] = DTRUE;
	m_nCurrentRecentLine = -1;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInputLine::AsciiXlate()
//
//	PURPOSE:	Translates a VK_ code into something viewable.
//
// ----------------------------------------------------------------------- //
			
char CInputLine::AsciiXlate(int key)
{
	char ch = 0;
	char *zNumShift = ")!@#$%^&*(";

	// Check for a letter
	if (key >= VK_A && key <= VK_Z)
	{
		ch = m_bShift ? key : key - 'A' + 'a';
	}
	// Check for a number
	else if (key >= VK_0 && key <= VK_9)
	{
		ch = m_bShift ? zNumShift[key - VK_0] : key;
	}
	// Now check for the remaining usable keys
	else
	{
		switch(key)
		{
			case VK_SPACE: ch = ' '; break;
			case 189: ch = m_bShift ? '_' : '-'; break;
			case 187: ch = m_bShift ? '+' : '='; break;
			case 219: ch = m_bShift ? '{' : '['; break;
			case 221: ch = m_bShift ? '}' : ']'; break;
			case 220: ch = m_bShift ? '|' : '\\'; break;
			case 186: ch = m_bShift ? ':' : ';'; break;
			case 222: ch = m_bShift ? '"' : '\''; break;
			case 188: ch = m_bShift ? '<' : ','; break;
			case 190: ch = m_bShift ? '>' : '.'; break;
			case 191: ch = m_bShift ? '?' : '/'; break;
		}
	}
	return ch;

}



/*******************************************************************************

	CCheatMgr

*******************************************************************************/

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::Init()
//
//	PURPOSE:	Initializes the cheat manager
//
// ----------------------------------------------------------------------- //
			
void CCheatMgr::Init(CClientDE* pClientDE)
{
	m_pClientDE = pClientDE;
	g_pCheatMgr = this;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::Check()
//
//	PURPOSE:	See if a string is a cheat code
//
// ----------------------------------------------------------------------- //
			
DBOOL CCheatMgr::Check( char *pzText )
{
	char buf[100];


#ifdef _DEMO
	return DFALSE;		// Don't do cheats in the demo...
#endif


	// copy their text
	strncpy(buf, pzText, sizeof(buf)-1);

	// It should start with "MP"
	if ( strncmp(buf, "mp", 2) != 0 )
		return DFALSE;

	// convert it to cheat compatible text
	for ( unsigned i = 0; i < strlen(pzText); i++ )
		buf[i] = ((buf[i] ^ 38) + i) ^ 7;

	// then compare the converted text
	for ( i = 0; i < CHEAT_MAX; i++ )
	{
		if ( strcmp( buf, s_CheatInfo[i].pzText ) == 0)
		{
			Process( (CheatCode)i );
			return DTRUE;
		}
	}
	return DFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::Process()
//
//	PURPOSE:	Calls the appropriate cheat function
//
// ----------------------------------------------------------------------- //
			
void CCheatMgr::Process( CheatCode nCheatCode )
{
	if ( nCheatCode <= CHEAT_NONE || nCheatCode >= CHEAT_MAX )
		return;

	if (g_pRiotClientShell)
	{
		g_pRiotClientShell->ProcessCheat(nCheatCode);
	}
	
	// process cheat codes
	switch ( nCheatCode )
	{
		case CHEAT_GOD:			// god mode toggle
			SetGodMode(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_AMMO:		// full ammo
			SetAmmo();
			break;

		case CHEAT_ARMOR:		// full armor
			SetArmor();
			break;

		case CHEAT_HEALTH:		// full health
			SetHealth();
			break;

		case CHEAT_CLIP:		// toggle clipping mode
			SetClipMode(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_TELEPORT:	// teleport to beginning
			Teleport();
			break;

		case CHEAT_ONFOOT:		// switch to on-foot mode
			SetOnFoot();
			break;

		case CHEAT_VEHICLE:		// switch to vehicle mode
			SetVehicle();
			break;

		case CHEAT_MECH:		// switch to mech mode
			SetMech();
			break;

		case CHEAT_POS:		    // show/hide player position 
			SetPos(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_KFA:			// give em everything
			SetKFA();
			break;
	
		case CHEAT_FULL_WEAPONS:   // give all weapons
			SetFullWeapons();
			break;

		case CHEAT_POSWEAPON:		    // toggle adjust of weapon pos
			PosWeapon(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_POSWEAPON_MUZZLE:	// toggle adjust of weapon muzzle pos
			PosWeaponMuzzle(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_PLAYERMOVEMENT:		// toggle player movement speed adjust 
			PlayerMovement(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_PLAYERACCEL:		   // toggle player acceleration adjust
			PlayerAccel(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_CAMERAOFFSET:	   // toggle adjust of camera offset
			CameraOffset(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_LIGHTSCALE:	      // toggle client light scale offset
			LightScale(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_BIGGUNS:	      // toggle guns cheat
			BigGuns(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_TEARS:	      // toggle tears cheat
			Tears(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_REMOVEAI:	  // remove all ai
			RemoveAI(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_TRIGGERBOX:	  // toggle trigger boxes on/off
			TriggerBox(!s_CheatInfo[nCheatCode].bActive);
			break;

		case CHEAT_ANIME:
			Anime(!s_CheatInfo[nCheatCode].bActive);
		break;

		case CHEAT_CHRISH:
		case CHEAT_SAMSON:
		case CHEAT_MECHAMAN:
		case CHEAT_EYT:
		case CHEAT_DAVIDN:
		case CHEAT_MILITANT:
		case CHEAT_ADO:
		case CHEAT_JOKER:
		case CHEAT_MAGIX:
		case CHEAT_BABA:
		case CHEAT_TYRIAN:
		case CHEAT_PALEFACE:
		case CHEAT_THEHAND:
		case CHEAT_DARYLS:
		case CHEAT_KAREL:
		case CHEAT_ETHANB:
		case CHEAT_KAJI:
		case CHEAT_KAOUL:
		case CHEAT_SHINH:
			Thanks(!s_CheatInfo[nCheatCode].bActive, nCheatCode);
		break;

		case CHEAT_ROBERT:
			Robert(!s_CheatInfo[nCheatCode].bActive);
		break;

		default:
			return;				// skip setting global cheat indicator for unhandled cheats
	}

	m_bPlayerCheated = DTRUE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SendCheatMessage()
//
//	PURPOSE:	sends a cheat to the server
//
// ----------------------------------------------------------------------- //
			
void CCheatMgr::SendCheatMessage( CheatCode nCheatCode, DBYTE nData )
{
	// Send the Message to the server
	HMESSAGEWRITE hMsg = m_pClientDE->StartMessage(MID_PLAYER_CHEAT);
	m_pClientDE->WriteToMessageByte(hMsg, (DBYTE)nCheatCode);
	m_pClientDE->WriteToMessageByte(hMsg, nData);
	m_pClientDE->EndMessage(hMsg);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetGodMode()
//
//	PURPOSE:	Sets/resets God mode
//
// ----------------------------------------------------------------------- //
			
void CCheatMgr::SetGodMode(DBOOL bMode)
{
	s_CheatInfo[CHEAT_GOD].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_GOD, bMode);

	if (bMode)
		g_pMessageMgr->AddLine("You are in god mode");
	else
		g_pMessageMgr->AddLine("You are no longer in god mode");
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetAmmo()
//
//	PURPOSE:	Gives full ammo
//
// ----------------------------------------------------------------------- //
			
void CCheatMgr::SetAmmo()
{
	s_CheatInfo[CHEAT_AMMO].bActive = DTRUE;

	// Tell the server
	SendCheatMessage(CHEAT_AMMO, DTRUE);

	g_pMessageMgr->AddLine("You have full ammo.");
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetArmor()
//
//	PURPOSE:	Gives full armor
//
// ----------------------------------------------------------------------- //

void CCheatMgr::SetArmor()
{
	s_CheatInfo[CHEAT_ARMOR].bActive = DTRUE;

	// Tell the server
	SendCheatMessage(CHEAT_ARMOR, DTRUE);

	g_pMessageMgr->AddLine("You have full armor.");
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetHealth()
//
//	PURPOSE:	Gives full health
//
// ----------------------------------------------------------------------- //
			
void CCheatMgr::SetHealth()
{
	s_CheatInfo[CHEAT_HEALTH].bActive = DTRUE;

	// Tell the server
	SendCheatMessage(CHEAT_HEALTH, DTRUE);

	g_pMessageMgr->AddLine("You have full health.");
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetClipMode()
//
//	PURPOSE:	Sets/resets Clip mode
//
// ----------------------------------------------------------------------- //
			
void CCheatMgr::SetClipMode(DBOOL bMode)
{
	// Can't do this in multiplayer.
	if(g_pRiotClientShell && g_pRiotClientShell->IsMultiplayerGame())
		return;

	s_CheatInfo[CHEAT_CLIP].bActive = bMode;

	if (g_pRiotClientShell) 
	{
		g_pRiotClientShell->SetSpectatorMode(bMode);
	}

	// Tell the server
	SendCheatMessage(CHEAT_CLIP, bMode);

	if (bMode)
		g_pMessageMgr->AddLine("Spectator mode enabled.");
	else
		g_pMessageMgr->AddLine("Spectator mode disabled.");
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::Teleport()
//
//	PURPOSE:	Teleports to beginning of level
//
// ----------------------------------------------------------------------- //

void CCheatMgr::Teleport()
{
	SendCheatMessage(CHEAT_TELEPORT, DTRUE);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetOnFoot()
//
//	PURPOSE:	Sets on foot mode
//
// ----------------------------------------------------------------------- //

void CCheatMgr::SetOnFoot()
{
	// Tell the server
	SendCheatMessage(CHEAT_ONFOOT, DTRUE);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetVehicle()
//
//	PURPOSE:	Sets vehicle mode
//
// ----------------------------------------------------------------------- //

void CCheatMgr::SetVehicle()
{
	// Tell the server
	SendCheatMessage(CHEAT_VEHICLE, DTRUE);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetMech()
//
//	PURPOSE:	Sets mech mode
//
// ----------------------------------------------------------------------- //

void CCheatMgr::SetMech()
{
	// Tell the server
	SendCheatMessage(CHEAT_MECH, DTRUE);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetFullWeapons()
//
//	PURPOSE:	Give us all the weapons
//
// ----------------------------------------------------------------------- //

void CCheatMgr::SetFullWeapons()
{
	s_CheatInfo[CHEAT_FULL_WEAPONS].bActive = DTRUE;

	// Tell the server
	SendCheatMessage(CHEAT_FULL_WEAPONS, DTRUE);

	g_pMessageMgr->AddLine("Mi Mi Mi - Can't find any weapons? <sniff>");
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetFullWeapons()
//
//	PURPOSE:	Give us all the weapons
//
// ----------------------------------------------------------------------- //

void CCheatMgr::SetKFA()
{
	s_CheatInfo[CHEAT_KFA].bActive = DTRUE;

	// Give us all weapons, ammo, armor, and health...
	SetFullWeapons();
	SetHealth();
	SetArmor();
	SetAmmo();

	g_pMessageMgr->AddLine("I'm coming out of the BOOOOOOTH!");
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::SetPos()
//
//	PURPOSE:	Toggle displaying of position on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::SetPos(DBOOL bMode)
{
	s_CheatInfo[CHEAT_POS].bActive = bMode;

	if (g_pRiotClientShell) 
	{
		g_pRiotClientShell->ShowPlayerPos(bMode);
	}

	if (bMode)
	{
		g_pMessageMgr->AddLine("Show position enabled.");
	}
	else
	{
		g_pMessageMgr->AddLine("Show position disabled.");
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::PosWeapon()
//
//	PURPOSE:	Toggle positioning of player view weapon on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::PosWeapon(DBOOL bMode)
{
	s_CheatInfo[CHEAT_POSWEAPON].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_POSWEAPON, bMode);

	if (g_pRiotClientShell) 
	{
		g_pRiotClientShell->ToggleDebugCheat(CHEAT_POSWEAPON);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::PosWeaponMuzzle()
//
//	PURPOSE:	Toggle positioning of player view weapon muzzle on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::PosWeaponMuzzle(DBOOL bMode)
{
	s_CheatInfo[CHEAT_POSWEAPON_MUZZLE].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_POSWEAPON_MUZZLE, bMode);

	if (g_pRiotClientShell) 
	{
		g_pRiotClientShell->ToggleDebugCheat(CHEAT_POSWEAPON_MUZZLE);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::PlayerMovement()
//
//	PURPOSE:	Toggle adjusting of player movement speed on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::PlayerMovement(DBOOL bMode)
{
	s_CheatInfo[CHEAT_PLAYERMOVEMENT].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_PLAYERMOVEMENT, bMode);

	if (bMode)
	{
		g_pMessageMgr->AddLine("Adjust player movement: ON");
	}
	else
	{
		g_pMessageMgr->AddLine("Adjust player movement: OFF");
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::PlayerAccel()
//
//	PURPOSE:	Toggle adjustment of player acceleration on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::PlayerAccel(DBOOL bMode)
{
	s_CheatInfo[CHEAT_PLAYERACCEL].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_PLAYERACCEL, bMode);

	if (bMode)
	{
		g_pMessageMgr->AddLine("Adjust player acceleration: ON");
	}
	else
	{
		g_pMessageMgr->AddLine("Adjust player acceleration: OFF");
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::CameraOffset()
//
//	PURPOSE:	Toggle adjustment of camera offset on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::CameraOffset(DBOOL bMode)
{
	s_CheatInfo[CHEAT_CAMERAOFFSET].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_CAMERAOFFSET, bMode);

	if (bMode)
	{
		g_pMessageMgr->AddLine("Adjust camera: ON");
	}
	else
	{
		g_pMessageMgr->AddLine("Adjust camera: OFF");
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::LightScale()
//
//	PURPOSE:	Toggle adjustment of light scale offset on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::LightScale(DBOOL bMode)
{
	s_CheatInfo[CHEAT_LIGHTSCALE].bActive = bMode;

	if (g_pRiotClientShell) 
	{
		g_pRiotClientShell->ToggleDebugCheat(CHEAT_LIGHTSCALE);
	}

	if (bMode)
	{
		g_pMessageMgr->AddLine("Adjust Light Scale: ON");
	}
	else
	{
		g_pMessageMgr->AddLine("Adjust Light Scale: OFF");
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::BigGuns()
//
//	PURPOSE:	Toggle big guns cheat on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::BigGuns(DBOOL bMode)
{
	s_CheatInfo[CHEAT_BIGGUNS].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_BIGGUNS, bMode);

	if (bMode)
	{
		g_pMessageMgr->AddLine("This is my BOOM stick!");
	}
	else
	{
		g_pMessageMgr->AddLine("Hmmm, now I feel inferior...");
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::Tears()
//
//	PURPOSE:	Toggle tears cheat on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::Tears(DBOOL bMode)
{
	s_CheatInfo[CHEAT_TEARS].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_TEARS, bMode);

	if (bMode)
		g_pMessageMgr->AddLine("<sniff>...Here, have some tissue...");
	else
		g_pMessageMgr->AddLine("All better now?");
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::RemoveAI()
//
//	PURPOSE:	Remove all AI in the level
//
// ----------------------------------------------------------------------- //

void CCheatMgr::RemoveAI(DBOOL bMode)
{
	s_CheatInfo[CHEAT_REMOVEAI].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_REMOVEAI, bMode);

	if (bMode)
		g_pMessageMgr->AddLine("Cheaters never prosper...");
	else
		g_pMessageMgr->AddLine("Sheeze, you really ARE pathetic...");
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::TriggerBox()
//
//	PURPOSE:	Toggle trigger boxes on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::TriggerBox(DBOOL bMode)
{
	s_CheatInfo[CHEAT_TRIGGERBOX].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_TRIGGERBOX, bMode);

	if (bMode)
		g_pMessageMgr->AddLine("Ah shucks, that takes all the fun out of it...");
	else
		g_pMessageMgr->AddLine("That's better sport!");
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::Robert()
//
//	PURPOSE:	Toggle robert cheat
//
// ----------------------------------------------------------------------- //

void CCheatMgr::Robert(DBOOL bMode)
{
	s_CheatInfo[CHEAT_ROBERT].bActive = bMode;

	// Tell the server
	SendCheatMessage(CHEAT_ROBERT, bMode);

	if (bMode)
		g_pMessageMgr->AddLine("Yeah...yeah...yeah...");
	else
		g_pMessageMgr->AddLine("Yeah...");
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::Anime()
//
//	PURPOSE:	Toggle Anime cheat on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::Anime(DBOOL bMode)
{
	s_CheatInfo[CHEAT_ANIME].bActive = bMode;

	if (bMode)
		g_pMessageMgr->AddLine("Marketing smarketing...we know what you want...");
	else
		g_pMessageMgr->AddLine("Quick!  Here comes marketing!");
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCheatMgr::Thanks()
//
//	PURPOSE:	Toggle Thanks cheat on/off
//
// ----------------------------------------------------------------------- //

void CCheatMgr::Thanks(DBOOL bMode, DBYTE nCode)
{
	s_CheatInfo[nCode].bActive = bMode;

	char* pMsg = DNULL;

	switch (nCode)
	{
		case CHEAT_CHRISH:
		case CHEAT_MECHAMAN:
		case CHEAT_EYT:
		case CHEAT_DAVIDN:
		case CHEAT_MILITANT:
			g_pMessageMgr->AddLine("Thank you from the Shogo Team!");
			g_pMessageMgr->AddLine("You *really* helped to keep us going!");
		break;

		case CHEAT_TYRIAN:
			g_pMessageMgr->AddLine("Thank you from the Shogo Team!");
			g_pMessageMgr->AddLine("You *really* helped to keep us going!");
			g_pMessageMgr->AddLine("p.s.  Stop stalking me ;)");
		break;

		case CHEAT_SHINH:
		case CHEAT_SAMSON:
		case CHEAT_ADO:
		case CHEAT_JOKER:
		case CHEAT_MAGIX:
		case CHEAT_BABA:
		case CHEAT_PALEFACE:
		case CHEAT_THEHAND:
		case CHEAT_DARYLS:
		case CHEAT_KAREL:
		case CHEAT_ETHANB:
		case CHEAT_KAJI:
		case CHEAT_KAOUL:
			g_pMessageMgr->AddLine("Thank you from the Shogo Team!");
		break;

		default : break;
	}
}
//-------------------------------------------------------------------------
//
// MODULE  : MessageMgr.h
//
// PURPOSE : Riot Messaging system - ruthlessly stolen from Greg Kettell
//
// CREATED : 10/22/97
//
//-------------------------------------------------------------------------

#ifndef __MESSAGEMGR_H__
#define __MESSAGEMGR_H__


#include "cpp_clientshell_de.h"
#include "client_de.h"
#include "DynArray.h"
#include "CheatDefs.h"

// defines
#define MESSAGEFLAGS_INCOMING		0x01
#define MESSAGEFLAGS_SELFGET		0x02
#define MESSAGEFLAGS_OTHERGET		0x04
#define MESSAGEFLAGS_RESPAWN		0x08
#define MESSAGEFLAGS_MASK			0x0F

#define	NUM_RECENT_LINES			16

// Input line class
class CInputLine
{
	public:

		CInputLine() 
		{ 
			m_pClientDE = DNULL;
			m_pClientShell = DNULL;

			m_nTextLen = 0; 
			*m_zText = '\0'; 
			m_bShift = DFALSE;
			memset (m_pRecentLines, 0, NUM_RECENT_LINES * (kMaxInputLine + 2));
			memset (m_bRecentLineUsed, 0, NUM_RECENT_LINES * sizeof (DBOOL));
			m_nBaseRecentLine = 0;
			m_nCurrentRecentLine = -1;
		}

		DBOOL		Init (CClientDE* pClientDE, CRiotClientShell* pClientShell);
		void		Clear( void );
		void		Term( void );
		void		Draw(HSURFACE hDest, HDEFONT hFont, HDECOLOR hForeColor, HDECOLOR hBackColor, long x, long y);
		DBOOL		AddChar( DBYTE ch );
		void		DelChar( void );
		void		Set( char *pzText );
		void		Send( void );
		DBOOL		HandleKeyDown(int key, int rep);
		void		HandleKeyUp(int key);

	private:

		enum { kMaxInputLine = 80 };

		char				AsciiXlate(int key);	// Translates VK_ values to ascii
		void				AddToRecentList();		// adds current string to recent-string list

		CClientDE*			m_pClientDE;			// the client interface
		CRiotClientShell*	m_pClientShell;			// the client shell interface
		HSURFACE			m_hSurface;				// The surface to draw to
		DBOOL				m_bShift;				// Shift is active
		int					m_nTextLen;				// Current length of input string
		char				m_zText[ kMaxInputLine + 2 ]; // The buffer
		char				m_pRecentLines[ NUM_RECENT_LINES ][ kMaxInputLine + 2 ];	// recent lines (for doskey effect)
		DBOOL				m_bRecentLineUsed[ NUM_RECENT_LINES ];						// has this recent line been used?
		int					m_nBaseRecentLine;		// current most-recent-line
		int					m_nCurrentRecentLine;	// selected recent line
};

// Message structure
struct Message
{
	Message()
	{
		hMessage = DNULL;
		hSurface = DNULL;
	}
	DFLOAT fExpiration;
	HSTRING  hMessage;
	HSURFACE hSurface;
};


// Message manager class
class CMessageMgr
{
	public:

		// Local constants
		enum
		{
			kMaxMessages		= 20,
			kMaxMessageSize		= 128,

			kDefaultMaxMessages	= 4,
			kDefaultMessageTime	= 5,	// seconds
		};

	public:

		CMessageMgr();
	
		DBOOL		Init (CClientDE* pClientDE, CRiotClientShell* pClientShell);
		void		Term ()  { Clear(); if (m_hFont) m_pClientDE->DeleteFont(m_hFont); }

		void		Enable( DBOOL bEnabled );

		void		AddLine( char *szMsg, DBYTE dbMessageFlag = MESSAGEFLAGS_MASK );
		void		AddLine( HSTRING hMsg, DBYTE dbMessageFlag = MESSAGEFLAGS_MASK );
		void		Clear( void );
		void		Draw( void );
		DBOOL		HandleKeyDown(int key, int rep);
		void		HandleKeyUp(int key) { m_InputLine.HandleKeyUp(key); }

		void		SetCoordinates( int x, int y );
		void		SetMaxMessages( int nMaxMessages );
		void		SetMessageTime( DFLOAT fSeconds );
		void		SetMessageFlags( DDWORD dwFlags );
		void		SetEditingState(DBOOL bEditing) { m_bEditing = bEditing; }

		DBOOL		GetState( void )		{ return m_bEnabled; }
		int			GetMaxMessages( void )	{ return m_nMaxMessages; }
		DFLOAT		GetMessageTime( void )	{ return m_fMessageTime; }
		DBOOL		GetEditingState() { return m_bEditing; }

	private:

		void				DeleteMessageData(Message *pMsg);

		CClientDE*			m_pClientDE;	// the client interface
		CRiotClientShell*	m_pClientShell;	// client shell interface
		CInputLine			m_InputLine;	// Current input message
		HDEFONT				m_hFont;		// menu font

		DBOOL				m_bEnabled;
		DBOOL				m_bEditing;

		int					m_x;
		int					m_y;

		int					m_nMaxMessages;
		DFLOAT				m_fMessageTime;
		DDWORD				m_dwMessageFlags;

		int					m_nMessageCount;
		int					m_nFirstMessage;
		int					m_nNextMessage;

		Message				m_Messages[ kMaxMessages ];
};


inline void CMessageMgr::DeleteMessageData(Message *pMsg)
{
	if (pMsg)
	{
		if (pMsg->hSurface)
		{
			m_pClientDE->DeleteSurface(pMsg->hSurface);
			pMsg->hSurface = DNULL;
		}
		if (pMsg->hMessage)
		{
			m_pClientDE->FreeString(pMsg->hMessage);
			pMsg->hMessage = DNULL;
		}
	}
}


// Cheat Manager class
class CCheatMgr
{
	public:
		CCheatMgr() {}

		void	Init(CClientDE* pClientDE);

		DBOOL	Check( char *pzText );
		void	ClearCheater() { m_bPlayerCheated = DFALSE; }
		DBOOL	IsCheater() { return m_bPlayerCheated; }

	protected:
		void	Process( CheatCode nCheatCode );

	private:

		void	SendCheatMessage( CheatCode nCheatCode, DBYTE nData );

		CClientDE*	m_pClientDE;			// the client interface

		struct CheatInfo
		{
			char			*pzText;
			DBOOL			bActive;
		};

		static CheatInfo s_CheatInfo[];

		static DBOOL m_bPlayerCheated;

		// Helper functions
		void	SetGodMode(DBOOL bMode);
		void	SetAmmo();
		void	SetArmor();
		void	SetHealth();
		void	SetClipMode(DBOOL bMode);
		void	Teleport();
		void	SetOnFoot();
		void	SetVehicle();
		void	SetMech();
		void	SetPos(DBOOL bMode);
		void	SetFullWeapons();
		void	SetKFA();
		void	PosWeapon(DBOOL bMode);
		void	PosWeaponMuzzle(DBOOL bMode);
		void	PlayerMovement(DBOOL bMode);
		void	PlayerAccel(DBOOL bMode);
		void	CameraOffset(DBOOL bMode);
		void	LightScale(DBOOL bMode);
		void	BigGuns(DBOOL bMode);
		void	Tears(DBOOL bMode);
		void	RemoveAI(DBOOL bMode);
		void	TriggerBox(DBOOL bMode);
		void	Anime(DBOOL bMode);
		void	Robert(DBOOL bMode);
		void	Thanks(DBOOL bMode, DBYTE nCode);
};



#endif	// __MESSAGEMGR_H__

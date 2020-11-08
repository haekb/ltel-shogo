// ----------------------------------------------------------------------- //
//
// MODULE  : RiotClientShell.h
//
// PURPOSE : Riot's Client Shell - Definition
//
// CREATED : 9/18/97
//
// ----------------------------------------------------------------------- //

#ifndef __RIOTCLIENTSHELL_H__
#define __RIOTCLIENTSHELL_H__

#include "cpp_clientshell_de.h"
#include "RiotMenu.h"
#include "PlayerStats.h"
#include "PlayerCamera.h"
#include "WeaponDefs.h"
#include "ClientServerShared.h"
#include "MessageMgr.h"
#include "SFXMgr.h"
#include "Music.h"
#include "ContainerCodes.h"
#include "PlayerModeTypes.h"
#include "ClientInfoMgr.h"
#include "MissionObjectives.h"
#include "PlayerInventory.h"
#include "WeaponModel.h"
#include "PolygridFX.h"
#include "Credits.h"
#include "MessageBox.h"
#include "TextHelper.h"
#include "InfoDisplay.h"
#include "LightScaleMgr.h"

// These 
#define CS_MFLG_FORWARD			(1<<0)
#define CS_MFLG_REVERSE			(1<<1)
#define CS_MFLG_RIGHT			(1<<2)
#define CS_MFLG_LEFT			(1<<3)
#define CS_MFLG_JUMP			(1<<4)
#define CS_MFLG_DOUBLEJUMP		(1<<5)
#define CS_MFLG_DUCK			(1<<6)
#define CS_MFLG_STRAFE			(1<<7)
#define CS_MFLG_STRAFE_LEFT		(1<<8)
#define CS_MFLG_STRAFE_RIGHT	(1<<9)
#define CS_MFLG_RUN				(1<<10)
#define CS_MFLG_FIRING			(1<<11)
#define CS_MFLG_LOOKUP			(1<<12)
#define CS_MFLG_LOOKDOWN		(1<<13)

// game states
#define GS_UNDEFINED			0
#define GS_PLAYING				1
#define GS_MENU					2
#define GS_MOVIES				3
#define GS_BUMPER				4
#define GS_LOADINGLEVEL			5
#define GS_CREDITS				6
#define GS_MPLOADINGLEVEL		7
#define GS_INTRO				8
#define GS_PAUSED				9
#define GS_DEMO_MULTIPLAYER		10
#define GS_DEMO_INFO			11

// world menu items (temp)
#define ID_BASEWORLD			256
#define ID_BASEWORLDMULTI		512

typedef void (*YESNOPROC)(DBOOL, DDWORD);

class CSpecialFX;
class CPopupMenu;
class CMoveMgr;

class CRiotClientShell : public CClientShellDE
{
	public:

		CRiotClientShell();
		~CRiotClientShell();

		void  ProcessCheat(CheatCode nCode);
		void  ShakeScreen(DVector vAmount);

		void  HandleRecord(int argc, char **argv);
		void  HandlePlaydemo(int argc, char **argv);

		DBOOL LoadWorld(char* pWorldFile, char* pCurWorldSaveFile=DNULL,
					    char* pRestoreWorldFile=DNULL, DBYTE nFlags=LOAD_NEW_GAME);
		DBOOL LoadGame(char* pWorld, char* pObjectsFile);
		DBOOL SaveGame(char* pObjectsFile);

		DBOOL QuickSave();
		DBOOL QuickLoad();

		DBOOL IsAnime()	const { return m_bAnime; }

		DBOOL GetNiceWorldName(char* pWorldFile, char* pRetName, int nRetLen);
		DBOOL StartGame(GameDifficulty eDifficulty);

		void  SetDifficulty(GameDifficulty e) { m_eDifficulty = e; }

		CMoveMgr* GetMoveMgr() {return m_MoveMgr;}
		ContainerCode	GetCurContainerCode() const { return m_eCurContainerCode; }	
		CPlayerStats* GetPlayerStats()		  { return &m_stats; }
		CInfoDisplay* GetInfoDisplay()		  { return &m_infoDisplay; }
		CRiotSettings* GetSettings()		  { return m_menu.GetSettings(); }
		DBOOL		IsVehicleMode()			  { CRiotSettings* pSettings = m_menu.GetSettings(); if (pSettings) return pSettings->VehicleMode(); return DFALSE; }
		CRiotMenu*	GetMenu()				  { return &m_menu; }
		HLOCALOBJ	GetCamera()			const { return m_hCamera; }
		DBOOL		IsOnFoot()			const { return (m_nPlayerMode == PM_MODE_FOOT); }
		DBYTE		GetPlayerMode()		const { return m_nPlayerMode; }
		DBOOL		IsUnderwater()		const { return m_bUnderwater; }
		DDWORD		GetPlayerFlags()	const { return m_dwPlayerFlags; }
		CWeaponModel* GetWeaponModel()		  { return &m_weaponModel; }
		DBOOL		HaveSilencer()		const { return m_bHaveSilencer; }
		DBOOL		IsInWorld( )		const { return m_bInWorld; }
		DBOOL		IsFirstUpdate( )	const { return m_bFirstUpdate; }
		DBOOL		IsDialogVisible()	const { return !!m_pIngameDialog; }
		char*		GetCurrentWorldName()	  { return m_strCurrentWorldName; }
		void		SetGameState(int nState)  { m_nGameState = nState; }
		CClientInfoMgr* GetClientInfoMgr()	  { return &m_ClientInfo; }
		CPlayerInventory* GetInventory()	  { return &m_inventory; }

		DBOOL		SoundInited()			  { return ( m_resSoundInit == LT_OK ) ? DTRUE : DFALSE; }
		void		MainWindowMinimized()	  { m_bMainWindowMinimized = DTRUE; }
		DBOOL		IsMainWindowMinimized()	  { return m_bMainWindowMinimized; }
		void		MainWindowRestored()	  { m_bMainWindowMinimized = DFALSE; }
		DBOOL		IsSpectatorMode()			{return m_bSpectatorMode;}
		DBOOL		IsChaseView()				{return m_playerCamera.IsChaseView(); }

		DBOOL		AdvancedDisableMusic()			{ return m_bAdvancedDisableMusic; }
		DBOOL		AdvancedDisableSound()			{ return m_bAdvancedDisableSound; }
		DBOOL		AdvancedDisableMovies()			{ return m_bAdvancedDisableMovies; }
		DBOOL		AdvancedEnableOptSurf()			{ return m_bAdvancedEnableOptSurf; }
		DBOOL		AdvancedDisableLightMap()		{ return m_bAdvancedDisableLightMap; }
		DBOOL		AdvancedEnableTripBuf()			{ return m_bAdvancedEnableTripBuf; }
		DBOOL		AdvancedDisableDx6Cmds()		{ return m_bAdvancedDisableDx6Cmds; }
		DBOOL		AdvancedEnableTJuncs()			{ return m_bAdvancedEnableTJuncs; }
		DBOOL		AdvancedDisableFog()			{ return m_bAdvancedDisableFog; }
		DBOOL		AdvancedDisableLines()			{ return m_bAdvancedDisableLines; }
		DBOOL		AdvancedDisableModelFB()		{ return m_bAdvancedDisableModelFB; }
		DBOOL		AdvancedEnablePixelDoubling()	{ return m_bAdvancedEnablePixelDoubling; }
		DBOOL		AdvancedEnableMultiTexturing()	{ return m_bAdvancedEnableMultiTexturing; }
		DBOOL		AdvancedDisableJoystick()		{ return m_bAdvancedDisableJoystick; }
		
		DBOOL		IsPlayerMovementAllowed() {return m_bAllowPlayerMovement;}
		DBOOL		IsZoomed()			const { return m_bZoomView; }

		void		SetVelMagnitude(float fVelMagnitude) {m_fVelMagnitude = fVelMagnitude;}

		void		DoUpdateLoop (int nTimes = 1)	{ for (int i = 0; i < nTimes; i++) {PreUpdate();Update();PostUpdate();} }
		void		ResetMenuRestoreCamera (int nLeft, int nTop, int nRight, int nBottom) { if (m_rcMenuRestoreCamera.right != 0 && m_rcMenuRestoreCamera.bottom != 0)
																							{
																								m_rcMenuRestoreCamera.left = nLeft;
																								m_rcMenuRestoreCamera.top = nTop;
																								m_rcMenuRestoreCamera.right = nRight;
																								m_rcMenuRestoreCamera.bottom = nBottom;
																							} }

		void		ClearScreenAlways (DBOOL bYes = DTRUE)	{ m_bClearScreenAlways = bYes; }
		void		AddToClearScreenCount()					{ m_nClearScreenCount = 3; }
		void		ZeroClearScreenCount()					{ m_nClearScreenCount = 0; }
		
		CMusic*		GetMusic()		{ return &m_Music; }
		void		InitSound();

		void		ToggleDebugCheat(CheatCode eCheat);
		void		ShowPlayerPos(DBOOL bShow=DTRUE)	{ m_bDebugInfoChanged = DTRUE; m_bShowPlayerPos = bShow; }
		void		SetSpectatorMode(DBOOL bOn=DTRUE);

		void		ClearAllScreenBuffers();
		DBOOL		SetMenuMode (DBOOL bMenuUp = DTRUE, DBOOL bLoadingLevel = DFALSE);
		DBOOL		SetMenuMusic(DBOOL bMusicOn);

		DBOOL		DoMessageBox (int nStringID, int nAlignment = TH_ALIGN_LEFT, DBOOL bCrop = DTRUE);
		DBOOL		DoYesNoMessageBox (int nStringID, YESNOPROC pYesNoProc, DDWORD nUserData, int nAlignment = TH_ALIGN_LEFT, DBOOL bCrop = DTRUE);

		DBOOL		IsJoystickEnabled();
		DBOOL		EnableJoystick();

		CSFXMgr*	GetSFXMgr() { return &m_sfxMgr; }

		void		TintScreen(DVector vTintColor, DVector vPos, DFLOAT fTintRange,
							   DFLOAT fTime, DFLOAT fRampUp, DFLOAT fRampDown,
							   DBOOL bForce=DFALSE);

		DBOOL		m_bRecordingDemo;	// Are we recording a demo

		void		UpdateModelGlow();
		DVector&	GetModelGlow() {return m_vCurModelGlow;}

		void		PrintError(char*);

		DBOOL		IsMultiplayerGame();
		DBOOL		IsPlayerInWorld();

		PlayerState	GetPlayerState()	const { return m_ePlayerState; }
		DBOOL		IsPlayerDead()		const { return (m_ePlayerState == PS_DEAD || m_ePlayerState == PS_DYING); }

		void		GetCameraRotation(DRotation *pRot);

		void		ChangeWeapon(DBYTE nWeaponId, DBOOL bZoom, DDWORD dwAmmo);
		void		UpdatePlayerStats(DBYTE nThing, DBYTE nType, DFLOAT fAmount);
		
		void		DemoSerialize(DStream *pStream, DBOOL bLoad);

		
		DBOOL		m_bSwitchingModes;

	protected :

		void		CSPrint (char* msg, ...);
		void		ShowSplash();
		DDWORD		OnEngineInitialized(struct RMode_t *pMode, DGUID *pAppGuid);
		void		OnEngineTerm();
		void		OnEvent(DDWORD dwEventID, DDWORD dwParam);
		DRESULT		OnObjectMove(HOBJECT hObj, DBOOL bTeleport, DVector *pPos);
		DRESULT		OnObjectRotate(HOBJECT hObj, DBOOL bTeleport, DRotation *pNewRot);
		DRESULT		OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo, float forceMag);
		void		PreLoadWorld(char *pWorldName);
		void		OnEnterWorld();
		void		OnExitWorld();
		void		PreUpdate();
		void		Update();
		void		PostUpdate();
		void		OnCommandOn(int command);
		void		OnCommandOff(int command);
		void		OnKeyDown(int key, int rep);
		void		OnKeyUp(int key);
		void		SpecialEffectNotify(HLOCALOBJ hObj, HMESSAGEREAD hMessage);
		void		OnObjectRemove(HLOCALOBJ hObj);
		void		OnMessage(DBYTE messageID, HMESSAGEREAD hMessage);
		void		OnModelKey(HLOCALOBJ hObj, ArgList *pArgs);
		
		CPopupMenu* CreateIngameDialog (HMESSAGEREAD hMessage);
		void		HandleObjectives (HMESSAGEREAD hMessage);
		void		PlayIntroMovies (CClientDE* pClientDE);
		void		PauseGame (DBOOL bPause, DBOOL bPauseSound=DFALSE);
		void		SetMouseInput(DBOOL bAllowInput);

		void		ShowPlayer(DBOOL bShow=DTRUE);

		void		UpdateServerPlayerModel();
		void		RenderCamera(DBOOL bDrawInterface = DTRUE);
		void		DrawInterface();
		void		DrawTransmission();
		void		DoRenderLoop(DBOOL bDrawInterface = DTRUE);

		void		SetLoadGameMenu();

		void		UpdateSoundReverb( );

	private :

		CMoveMgr		*m_MoveMgr;			// Always around...

		DBOOL			m_bUseWorldFog;		// Tells if we should use global fog settings or
											// let the container handling do it.
		DBOOL			m_bTintScreen;		// Are we tinting the screen
		DFLOAT			m_fTintTime;		// Time screen stays at tint color
		DFLOAT			m_fTintStart;		// When did the tinting start
		DFLOAT			m_fTintRampUp;		// Ramp up time
		DFLOAT			m_fTintRampDown;	// Ramp down time
		DVector			m_vTintColor;		// Tint color

		DFLOAT			m_fYawBackup;
		DFLOAT			m_fPitchBackup;

		// Player movement variables...

		DDWORD			m_dwPlayerFlags;	// What is the player doing
		DBYTE			m_nPlayerMode;		// What mode is the player in
		DBOOL			m_bSpectatorMode;	// Are we in spectator mode
		DBOOL			m_bMoving;			// Is the player moving
		DBOOL			m_bMovingSide;		// Is the player moving sideways
		DBOOL			m_bOnGround;		// Is the player on the ground
		PlayerState		m_ePlayerState;		// What is the state of the player

		CMusic::EMusicLevel	m_eMusicLevel;		// Level of IMA

		// Player update stuff...

		DFLOAT		m_fLastSentYaw;
		DFLOAT		m_fLastSentCamCant;
		DVector		m_vLastSentFlashPos;
		DVector		m_vLastSentModelPos;
		DBYTE		m_nLastSentCode;
		DBOOL		m_bLastSent3rdPerson;

		DRotation		m_rRotation;		// Player view rotation
		DVector			m_vCameraOffset;	// Offsets to the camera position
		DFLOAT			m_fPitch;			// Pitch of camera
		DFLOAT			m_fYaw;				// Yaw of camera
		DBOOL			m_bCenterView;		// Center the view?
		DFLOAT			m_fFireJitterPitch;	// Weapon firing jitter pitch adjust

		DBOOL			m_bAllowPlayerMovement;		// External camera stuff
		DBOOL			m_bLastAllowPlayerMovement;
		DBOOL			m_bWasUsingExternalCamera;	// so we can detect when we start using it
		DBOOL			m_bUsingExternalCamera;

		DBOOL			m_bMovieCameraRect;
		int				m_nOldCameraLeft;
		int				m_nOldCameraTop;
		int				m_nOldCameraRight;
		int				m_nOldCameraBottom;

		// Container FX helpers...

		ContainerCode	m_eCurContainerCode;	// Code of container currently in
		DFLOAT			m_fContainerStartTime;	// Time we entered current container
		DBOOL			m_bUnderwater;			// Are we underwater?
		DFLOAT			m_fFovXFXDir;			// Variable use in UpdateUnderWaterFX()
		DFLOAT			m_fLastTime;			// Variable use in UpdateUnderWaterFX()

		// Camera bobbing variables...
		// Bobbin' and Swayin' - Blood 2 style ;)

		DFLOAT			m_fBobHeight;
		DFLOAT			m_fBobWidth;
		DFLOAT			m_fBobAmp;
		DFLOAT			m_fBobPhase;
		DFLOAT			m_fSwayPhase;
		DFLOAT			m_fVelMagnitude;	// Player's velocity (magnitude)
		
		// Camera canting variables...

		DFLOAT			m_fCantIncrement;	// In radians
		DFLOAT			m_fCantMaxDist;		// In radians
		DFLOAT			m_fCamCant;			// Camera cant amount


		// Camera zoom related variables...

		DFLOAT			m_fCurrentFovX;		// The current (non-zoomed) fovX
		DBOOL			m_bZoomView;		// Are we in zoom mode
		DBOOL			m_bOldZoomView;		// Were we zoom modelast frame
		DBOOL			m_bZooming;			// Are we zooming in/out
		DFLOAT			m_fSaveLODScale;	// LOD Scale value before zooming


		// Camera ducking variables...

		DBOOL	m_bStartedDuckingDown;		// Have we started ducking down
		DBOOL	m_bStartedDuckingUp;		// Have we started back up
		DFLOAT	m_fCamDuck;					// How far to move camera
		DFLOAT	m_fDuckDownV;				// Ducking down velocity
		DFLOAT	m_fDuckUpV;					// Ducking up velocity
		DFLOAT	m_fMaxDuckDistance;			// Max distance we can duck
		DFLOAT	m_fStartDuckTime;			// When duck up/down started


		GameDifficulty	m_eDifficulty;		// Difficulty of this game

		CPlayerStats		m_stats;			// duh - player statistics (health, ammo, armor, etc.)
		CMissionObjectives	m_objectives;		// mission objectives class
		CPlayerInventory	m_inventory;		// inventory class


	// NOTE:  The following data members do not need to be saved / loaded
	// when saving games.  Any data members that don't need to be saved
	// should be added here (to keep them together)...
	
		DBOOL		m_bGameOver;
		DBOOL		m_bAnime;
		DBOOL		m_bStartedLevel;
		D_WORD		m_nPlayerInfoChangeFlags;
		DFLOAT		m_fPlayerInfoLastSendTime;

		DBOOL		m_bCameraPosInited;		// Make sure the position is valid	
		DBOOL		m_bRestoringGame;		// Are we restoring a saved game

		DRESULT		m_resSoundInit;			// Was sound initialized ok?
		DBOOL		m_bGameMusicPaused;		// Is the game muisc paused?

		DBOOL		m_bMainWindowMinimized;	// Is the main window minimized?

		DBOOL		m_bStrafing;			// Are we strafing?  This used to implement mouse strafing.
		DBOOL		m_bHoldingMouseLook;	// Is the user holding down the mouselook key?

		DRect		m_rcMenuRestoreCamera;	// Camera rect to restore after leaving menus
		DBOOL		m_bMenuRestoreFullScreen;	// was the camera rect full-screen before going to the menus?

		DBOOL		m_bMusicOriginallyEnabled;	// was music originally enabled?
		DBOOL		m_bSoundOriginallyEnabled;	// was sound originally enabled?
		DBOOL		m_bLightmappingOriginallyEnabled;	// was lightmapping originally enabled?
		DBOOL		m_bModelFBOriginallyEnabled;		// were model fullbrights originally enabled?

		// Advanced console options...

		DBOOL		m_bAdvancedDisableMusic;		// Disable music
		DBOOL		m_bAdvancedDisableSound;		// Disable sound
		DBOOL		m_bAdvancedDisableMovies;		// Disable movies
		DBOOL		m_bAdvancedEnableOptSurf;		// Enable optimizing surfaces
		DBOOL		m_bAdvancedDisableLightMap;		// Disable light mapping
		DBOOL		m_bAdvancedEnableTripBuf;		// Enable triple buffering
		DBOOL		m_bAdvancedDisableDx6Cmds;		// Disable DX6 commands
		DBOOL		m_bAdvancedEnableTJuncs;		// Enable T-Junction sealing (gaps between polies)
		DBOOL		m_bAdvancedDisableFog;			// Disable fog
		DBOOL		m_bAdvancedDisableLines;		// Disable line systems
		DBOOL		m_bAdvancedDisableModelFB;		// Disable model fullbrights
		DBOOL		m_bAdvancedEnablePixelDoubling;	// Enable pixel doubling
		DBOOL		m_bAdvancedEnableMultiTexturing;// Enable single-pass multi-texturing
		DBOOL		m_bAdvancedDisableJoystick;		// Disable joystick
		
		// Glowing models...

		DVector		m_vCurModelGlow;		// Current glowing model light color 
		DVector		m_vMaxModelGlow;		// Max glowing model light color
		DVector		m_vMinModelGlow;		// Min glowing model light color
		DFLOAT		m_fModelGlowCycleTime;	// Current type throught 1/2 cycle
		DBOOL		m_bModelGlowCycleUp;	// Cycle color up or down?

		// Panning sky...

		DBOOL		m_bPanSky;				// Should we pan the sky
		DFLOAT		m_fPanSkyOffsetX;		// How much do we pan in X/frame
		DFLOAT		m_fPanSkyOffsetZ;		// How much do we pan in Z/frame
		DFLOAT		m_fPanSkyScaleX;		// How much do we scale the texture in X
		DFLOAT		m_fPanSkyScaleZ;		// How much do we scale the texutre in Z
		DFLOAT		m_fCurSkyXOffset;		// What is the current x offset
		DFLOAT		m_fCurSkyZOffset;		// What is the current z offset

		CWeaponModel	m_weaponModel;			// Current weapon model
		WeaponState		m_eWeaponState;			// State of the weapon
		DBOOL			m_bTweakingWeapon;		// Helper, move weapon around
		DBOOL			m_bTweakingWeaponMuzzle;// Helper, move weapon muzzle around

		DVector			m_vShakeAmount;			// Amount to shake screen
		DDWORD			m_nGameState;			// Current game state

		CMusic			m_Music;				// Music helper variable

		DVector			m_vDefaultLightScale;	// Level default light scale

		char			m_strCurrentWorldName[256];		// current world that's running
		char			m_strMoviesDir[256];			// location of movies on CDROM drive

		DBOOL			m_bGamePaused;			// Is the game paused?

		// Interface stuff...

		CMessageMgr			m_messageMgr;		// message display/sending mgr
		CCheatMgr			m_cheatMgr;			// cheat message mgr
		CRiotMenu			m_menu;				// pretty self-explanatory isn't it?
		CClientInfoMgr		m_ClientInfo;		// info on all clients connected to server
		CCredits			m_credits;			// class to display credits
		CInfoDisplay		m_infoDisplay;		// temporary information display class
		CLightScaleMgr		m_LightScaleMgr;	// class to handle light scale changes

		DVector				m_vLightScaleInfrared;		// default light scale for infrared powerup
		DVector				m_vLightScaleNightVision;	// default light scale for night vision powerup
		DVector				m_vLightScaleObjectives;	// default light scale for mission objectives
		DVector				m_vCurContainerLightScale;	// light scale values of current container

		CPolyGridFX*		m_pMenuPolygrid;		// Polygrid menu background
		DFLOAT				m_fMenuSaveFOVx;		// Fov before entering menu		
		DFLOAT				m_fMenuSaveFOVy;		// Fov before entering menu

		DDWORD			m_nClearScreenCount;		// how many frames to clear the screen
		DBOOL			m_bClearScreenAlways;		// should we always clear the screen?
		DBOOL			m_bDrawInterface;			// should we draw the interface (health, ammo, armor, etc.)
		DBOOL			m_bOldDrawInterface;		// Saved value of above
		DBOOL			m_bDrawHud;					// should we draw the interface hud
		DBOOL			m_bDrawMissionLog;			// should we draw the mission log?
		DBOOL			m_bDrawOrdinance;			// should we draw the ordinance screen?
		DBOOL			m_bDrawFragCount;			// should we draw the frag counts?
		DBOOL			m_bStatsSizedOff;			// have the stats been sized off the screen?
		DBOOL			m_bWaitingForMLClosure;		// are we waiting for the mission log to close?
		DBOOL			m_bNewObjective;			// have we received a new objective?
		HSURFACE		m_hNewObjective;			// new objective notice
		DBOOL			m_bUpdateStats;				// do we need to update the player's stats?
		DBOOL			m_bMissionLogKeyStillDown;	// are they holding down F1?
		DBOOL			m_bCrosshairOn;				// Is the crosshair on

		HSURFACE		m_hGameMessage;				// a game message surface, if one exists
		DFLOAT			m_nGameMessageRemoveTime;	// time at which to remove the message
		DRect			m_rcGameMessage;			// game message rectangle (0,0,width,height)

		CPopupMenu*		m_pIngameDialog;			// in-game dialog spawned by a CDialogTrigger
		CMessageBox*	m_pMessageBox;
		YESNOPROC		m_pYesNoProc;
		DDWORD			m_nYesNoUserData;

		HSURFACE		m_hBumperText;				// bumper screen text 
		char*			m_pPressAnyKeySound;		// Sound played when "press any key" text appears

		DBOOL			m_bAllowMouseInput;			// is mouse input ok?
		DBOOL			m_bRestoreOrientation;
		HLOCALOBJ		m_h3rdPersonCrosshair;
		HLOCALOBJ		m_hBoundingBox;

		DBOOL			m_bHaveNightVision;			// does this player currently have the NightVision powerup?
		DBOOL			m_bHaveInfrared;			// does this player currently have the Infrared powerup?
		DBOOL			m_bHaveSilencer;			// does this player currently have the Silencer powerup?
		DBOOL			m_bHaveStealth;				// does this player currently have the Stealth powerup?
		DFLOAT			m_fNormalWeaponAlpha;		// normal weapon alpha (for resetting after stealth expires)

		HSURFACE		m_hGamePausedSurface;		// "Game Paused" message

		HSOUNDDE		m_hMenuMusic;				// handle to music playing while menu is up

		DFLOAT			m_fTransmissionTimeLeft;		// time left to display transmission
		HSURFACE		m_hTransmissionImage;			// image to display in transmission
		HSURFACE		m_hTransmissionText;			// text surface for transmission
		HSOUNDDE		m_hTransmissionSound;			// sound handle of tranmission sound
		DBOOL			m_bAnimatingTransmissionOn;		// are we animating the transmission onto the screen?
		DBOOL			m_bAnimatingTransmissionOff;	// are we animating the transmission off of the screen?
		DFLOAT			m_xTransmissionImage;
		DFLOAT			m_yTransmissionImage;
		DFLOAT			m_cxTransmissionImage;
		DFLOAT			m_cyTransmissionImage;
		DFLOAT			m_xTransmissionText;
		DFLOAT			m_yTransmissionText;
		DFLOAT			m_cxTransmissionText;
		DFLOAT			m_cyTransmissionText;

		HSURFACE		m_hLoadingWorld;
		HSURFACE		m_hPressAnyKey;
		HSURFACE		m_hWorldName;
		DDWORD			m_cxPressAnyKey;
		DDWORD			m_cyPressAnyKey;

		DBOOL			m_bLoadingWorld;

		HSURFACE		m_hLoading;
		DDWORD			m_cxLoading;
		DDWORD			m_cyLoading;
	
		DBOOL			m_bQuickSave;

		DFLOAT			m_fEarliestRespawnTime;

		// Camera variables...

		HLOCALOBJ		m_hCamera;			// The camera
		CPlayerCamera	m_playerCamera;		// Handle 3rd person view

		DBOOL			m_bHandledStartup;	// Have we handled start up conditions
		DBOOL			m_bFirstUpdate;		// Is this the first update
		DBOOL			m_bInWorld;			// Are we in a world

		DBOOL			m_bDemoPlayback;	// Are we playing back a demo

		DBOOL			m_bPlayerPosSet;	// Has the server sent us the player pos?


		// Container FX helpers...

		HSOUNDDE		m_hContainerSound;	// Container sound...
	

		// Reverb parameters
		DBOOL			m_bUseReverb;
		float			m_fReverbLevel;
		float			m_fNextSoundReverbTime;
		DVector			m_vLastReverbPos;

		// Special FX management...

		CSFXMgr		m_sfxMgr;



		// Debugging variables...

		DBOOL		m_bDebugInfoChanged;	// Has the info changed?
		HSURFACE	m_hDebugInfo;			// The degug info surface
		DRect		m_rcDebugInfo;			// Debug info rect. (0,0,width,height)
		HLOCALOBJ	m_hLeakLines;			// Leak finder lines

		DBOOL		m_bShowPlayerPos;		// Display player's position.
		DBOOL		m_bAdjustLightScale;	// Adjusting the global light scale


		// Private helper functions...

		void	UpdateLoadingLevel();
		void	StartNewWorld(HSTRING hstrWorld);
		void	FirstUpdate();
		void	ChangeWeapon(HMESSAGEREAD hMessage);
		void	ShowGameMessage(char* strMessage);
		void	CreateDebugSurface(char* strMessage);
		void	UpdateContainerFX();
		void	ResetGlobalFog();
		void	UpdateUnderWaterFX(DBOOL bUpdate=DTRUE);
		void	UpdateBreathingFX(DBOOL bUpdate=DTRUE);
		void	UpdateWeaponModel();
		void	UpdatePlayerFlags();
		void	UpdateHeadBob();
		void	UpdateHeadCant();
		void	UpdateDuck();
		void	UpdatePlayer();
		void	UpdateWeaponPosition();
		void	UpdateWeaponMuzzlePosition();
		void	Update3rdPersonCrossHair(DFLOAT fDistance);

		void	UpdateMoviesState();
		void	UpdateCreditsState();
		void	UpdateIntroState();
		void	UpdateMenuState();
		void	UpdateBumperState();
		void	UpdateLoadingLevelState();
		void	UpdatePausedState();
		void	UpdateGameOver();
		void	UpdateDemoMultiplayerState();
		void	UpdateDemoInfoState();

		void	AdjustLightScale();
		void	UpdateDebugInfo();
		void	HandlePlayerStateChange(HMESSAGEREAD hMessage);
		void	HandlePlayerDamage(HMESSAGEREAD hMessage);
		void	HandleExitLevel(HMESSAGEREAD hMessage);
		void	HandleServerError(HMESSAGEREAD hMessage);
		void	HandleTransmission(HMESSAGEREAD hMessage);

		void	DoPickupItemScreenTint(PickupItemType eType);
		void	DisplayGenericPickupMessage(PickupItemType eType);
		void	HandleItemPickedup(PickupItemType eType);
		void	HandleItemExpired(PickupItemType eType);
		void	HandleRespawn();
		void	CreateBumperScreen(char* pPCXName, DDWORD dwBumperTextID);
		void	RemoveBumperScreen();
		void	HandleMPChangeLevel();

		DBOOL	ProcessMenuCommand (int nID);
		void	SetInputState(DBOOL bAllowInput);

		void	InitMultiPlayer();
		void	InitSinglePlayer();

		void	DoStartGame();
		DBOOL   DoLoadWorld(char* pWorldFile, char* pCurWorldSaveFile=DNULL,
					        char* pRestoreWorldFile=DNULL, DBYTE nFlags=LOAD_NEW_GAME,
							char *pRecordFile=DNULL, char *pPlaydemoFile=NULL);

		void	AutoSave(HMESSAGEREAD hMessage);

		void	StartLevel();

		// Load Save functionality...

		void	BuildClientSaveMsg(HMESSAGEWRITE hMessage);
		void	UnpackClientSaveMsg(HMESSAGEREAD hRead);


		// Camera helper functions...

		void	UpdateCamera();
		void	UpdateCameraZoom();
		void	UpdateCameraShake();
		void	UpdateScreenTint();
		void	ClearScreenTint();
		DBOOL	UpdatePlayerCamera();
		void	UpdateCameraPosition();
		void	CalculateCameraRotation();
		DBOOL	UpdateCameraRotation();
		void	UpdatePlayerInfo();
		DBOOL	UpdateAlternativeCamera();
		void	TurnOffAlternativeCamera();
		void	SetExternalCamera(DBOOL bExternal=DTRUE);
		void	HandleZoomChange(DBYTE nWeaponId);
		void	Update3rdPersonInfo();

		void	CreateMenuPolygrid();
		void	RemoveMenuPolygrid();
		void	UpdateMenuPolygrid();

		void	CreateBoundingBox();
		void	UpdateBoundingBox();
};


#endif  // __RIOTCLIENTSHELL_H__


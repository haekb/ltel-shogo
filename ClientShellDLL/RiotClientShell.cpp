// ----------------------------------------------------------------------- //
//
// MODULE  : RiotClientShell.cpp
//
// PURPOSE : Riot's Client Shell - Implementation
//
// CREATED : 9/18/97
//
// ----------------------------------------------------------------------- //

#include "RiotClientShell.h"
#include "RiotMsgIds.h"
#include "RiotCommandIds.h"
#include "WeaponModel.h"
#include "WeaponDefs.h"
#include "ClientUtilities.h"
#include "vkdefs.h"
#include "ClientRes.h"
#include "RiotSoundTypes.h"
#include "Music.h"
#include "TextHelper.h"
#include "PopupMenu.h"
#include "VolumeBrushFX.h"
#include "client_physics.h"
#include "CameraFX.h"
#include "WinUtil.h"
#include "Font28.h"
#include "Font08.h"
#include "NetStart.h"
#include "WeaponStringDefs.h"
#include "CMoveMgr.h"
#include "physics_lt.h"
#include "VarTrack.h"
#include "ClientWeaponUtils.h"
#include "SFXReg.h"
#include <mbstring.h>

#include <stdarg.h>
#include <stdio.h>

#define WAIT_FOR_DEBUGGER

#define min(a,b)	(a < b ? a : b)
#define max(a,b)	(a > b ? a : b)


#define BOBV_WALKING	24.0f
#define BOBH_WALKING	16.0f
#define SWAYV_WALKING	32.0f
#define SWAYH_WALKING	80.0f

#define BOBV_CRAWLING	24.0f
#define BOBH_CRAWLING	16.0f
#define SWAYV_CRAWLING	16.0f
#define SWAYH_CRAWLING	40.0f

#define FOV_NORMAL		90
#define FOV_ZOOMED		10

#define DEG2RAD(x)		(((x)*MATH_PI)/180.0f)
#define RAD2DEG(x)		(((x)*180.0f)/MATH_PI)

#define LETTERBOX_ADJUST	20

#define TRANSMISSION_IMAGE_ANIM_RATE	1000
#define TRANSMISSION_TEXT_ANIM_RATE		400

#define	FIRE_JITTER_DECAY_DELTA			0.1f
#define FIRE_JITTER_MAX_PITCH_DELTA		1.0f

#define DEFAULT_LOD_SCALE				1.1f
#define LODSCALE_MULTIPLIER				5.0f

#define MODELGLOW_HALFCYCLE				1.0f

#define MAX_PLAYER_INFO_SEND_PER_SECOND	20.0f

#define RESPAWN_WAIT_TIME					1.0f
#define MULTIPLAYER_RESPAWN_WAIT_TIME		0.2f

#define PICKUPITEM_TINT_UNKNOWN			DVector(0.0f, 0.0f, 0.0f)
#define PICKUPITEM_TINT_ARMOR			DVector(0.25f, 0.25f, 0.25f)
#define PICKUPITEM_TINT_HEALTH			DVector(0.25f, 0.25f, 0.25f)
#define PICKUPITEM_TINT_WEAPON			DVector(0.25f, 0.25f, 0.4f)

#define DEFAULT_CSENDRATE	7.0f

#define SOUND_REVERB_UPDATE_PERIOD		0.33f


#define DEFAULT_NORMAL_TURN_SPEED 1.5f
#define DEFAULT_FAST_TURN_SPEED 2.3f
#define NORMAL_TURN_RATE_VAR "NormalTurnRate"
#define FAST_TURN_RATE_VAR "FastTurnRate"


CRiotClientShell* g_pRiotClientShell = DNULL;
DVector			  g_vWorldWindVel;
PhysicsState	  g_normalPhysicsState;
PhysicsState	  g_waterPhysicsState;

VarTrack g_CV_CSendRate; // The SendRate console variable.

DFLOAT s_fDemoTime   = 0.0f;

void IRModelHook (struct ModelHookData_t *pData, void *pUser);
void NVModelHook (struct ModelHookData_t *pData, void *pUser);
void DefaultModelHook (struct ModelHookData_t *pData, void *pUser);

// Guids...

#ifdef _DEMO

DGUID SHOGOGUID = { 
	0x67aade60,	0xab3, 0x22d4, 0xcd, 0x69, 0x0, 0x40, 0x5, 0x92, 0x34, 0x31
};

#else

DGUID SHOGOGUID = { /* 87EEDE80-0ED4-11d2-BA96-006008904776 */
	0x87eede80,	0xed4, 0x11d2, 0xba, 0x96, 0x0, 0x60, 0x8, 0x90, 0x47, 0x76
};

#endif


// Setup..

SETUP_CLIENTSHELL();

ClientShellDE* CreateClientShell(ClientDE *pClientDE)
{
	g_pClientDE = pClientDE;
	return (ClientShellDE*)(new CRiotClientShell);
}

void DeleteClientShell(ClientShellDE *pInputShell)
{
	if (pInputShell)
	{
		delete ((CRiotClientShell*)pInputShell);
	}
}

static DBOOL LoadLeakFile(ClientDE *pClientDE, char *pFilename);

void LeakFileFn(int argc, char **argv)
{
	if (argc < 0)
	{
		g_pRiotClientShell->GetClientDE()->CPrint("LeakFile <filename>");
		return;
	}

	if (LoadLeakFile(g_pRiotClientShell->GetClientDE(), argv[0]))
	{
		g_pRiotClientShell->GetClientDE()->CPrint("Leak file %s loaded successfully!", argv[0]);
	}
	else
	{
		g_pRiotClientShell->GetClientDE()->CPrint("Unable to load leak file %s", argv[0]);
	}
}

DBOOL ConnectToTcpIpAddress(CClientDE* pClientDE, char* sAddress);

void ConnectFn(int argc, char **argv)
{

//#ifdef _DEMO
//	g_pRiotClientShell->GetClientDE()->CPrint("Multiplayer is not available in the demo");
//	return;
//#endif

	if (argc <= 0)
	{
		g_pRiotClientShell->GetClientDE()->CPrint("Connect <tcpip address> (use '*' for local net)");
		return;
	}

	ConnectToTcpIpAddress(g_pRiotClientShell->GetClientDE(), argv[0]);
}

void FragSelfFn(int argc, char **argv)
{
	ClientDE *pClientDE;
	HMESSAGEWRITE hWrite;

	pClientDE = g_pRiotClientShell->GetClientDE();
	hWrite = pClientDE->StartMessage(MID_FRAG_SELF);
	pClientDE->EndMessage(hWrite);
}


void RecordFn(int argc, char **argv)
{
	if(g_pRiotClientShell)
		g_pRiotClientShell->HandleRecord(argc, argv);
}

void PlayDemoFn(int argc, char **argv)
{
	if(g_pRiotClientShell)
		g_pRiotClientShell->HandlePlaydemo(argc, argv);
}


void ExitGame(DBOOL bResponse, DDWORD nUserData)
{
	if (bResponse)
	{
		g_pRiotClientShell->GetClientDE()->Shutdown();
	}
}


void InitSoundFn(int argc, char **argv)
{
	if( g_pRiotClientShell )
		g_pRiotClientShell->InitSound( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::CRiotClientShell()
//
//	PURPOSE:	Initialization
//
// ----------------------------------------------------------------------- //
			
CRiotClientShell::CRiotClientShell()
{
	g_pRiotClientShell = this;

	m_MoveMgr = DNULL;

	VEC_INIT(g_vWorldWindVel);
	memset(&m_rcMenuRestoreCamera, 0, sizeof(m_rcMenuRestoreCamera));

	m_hCamera		= DNULL;
	m_nGameState	= GS_PLAYING;

	m_bUseWorldFog = DTRUE;
	m_bMainWindowMinimized	= DFALSE;

	m_bStrafing			= DFALSE;
	m_bHoldingMouseLook	= DFALSE;
	
	m_bMusicOriginallyEnabled	= DFALSE;
	m_bSoundOriginallyEnabled	= DFALSE;
	m_bLightmappingOriginallyEnabled = DTRUE;
	m_bModelFBOriginallyEnabled		 = DTRUE;

	m_bGamePaused		= DFALSE;
	m_bRecordingDemo	= DFALSE;

	m_bSwitchingModes = DFALSE;
	m_nClearScreenCount		= 0;
	m_bClearScreenAlways	= DFALSE;
	m_bOldDrawInterface		= DTRUE;
	m_bDrawHud				= DTRUE;
	m_bDrawInterface		= DTRUE;
	m_bDrawMissionLog		= DFALSE;
	m_bDrawOrdinance		= DFALSE;
	m_bDrawFragCount		= DFALSE;
	m_bStatsSizedOff		= DFALSE;
	m_bWaitingForMLClosure	= DFALSE;
	m_bNewObjective			= DFALSE;
	m_hNewObjective			= DNULL;
	m_bUpdateStats			= DFALSE;
	m_bMissionLogKeyStillDown = DFALSE;
	m_bCrosshairOn			= DTRUE;
	
	m_hGameMessage			 = NULL;
	m_nGameMessageRemoveTime = 0.0f;
	m_pMenuPolygrid			 = DNULL;
	m_fMenuSaveFOVx			 = 0.0f;
	m_fMenuSaveFOVy			 = 0.0f;

	m_pIngameDialog		= DNULL;
	m_pMessageBox		= DNULL;
	m_pYesNoProc		= DNULL;
	m_nYesNoUserData	= DNULL;

	m_hBumperText		= DNULL;
	m_pPressAnyKeySound = "Sounds\\Interface\\pak.wav";

	m_fYawBackup		= 0.0f;
	m_fPitchBackup		= 0.0f;
	m_bRestoreOrientation		= DFALSE;
	m_bAllowPlayerMovement		= DTRUE;
	m_bLastAllowPlayerMovement	= DTRUE;
	m_bWasUsingExternalCamera	= DFALSE;
	m_bUsingExternalCamera		= DFALSE;
	m_bMovieCameraRect			= DFALSE;

	m_bHaveNightVision	= DFALSE;
	m_bHaveInfrared		= DFALSE;
	m_bHaveSilencer		= DFALSE;
	m_bHaveStealth		= DFALSE;
	m_fNormalWeaponAlpha = 0.0f;

	m_hGamePausedSurface	= DNULL;
	VEC_SET(m_vDefaultLightScale, 1.0f, 1.0f, 1.0f);

	m_hMenuMusic		= DNULL;
	
	m_nPlayerInfoChangeFlags	= 0;
	m_fPlayerInfoLastSendTime	= 0.0f;

	m_fTransmissionTimeLeft = 0.0f;
	m_hTransmissionImage = DNULL;
	m_hTransmissionText = DNULL;
	m_hTransmissionSound = DNULL;
	m_bAnimatingTransmissionOn = DFALSE;
	m_bAnimatingTransmissionOff = DFALSE;
	m_xTransmissionImage = 0;
	m_yTransmissionImage = 0;
	m_cxTransmissionImage = 0;
	m_cyTransmissionImage = 0;
	m_xTransmissionText = 0;
	m_yTransmissionText = 0;
	m_cxTransmissionText = 0;
	m_cyTransmissionText = 0;
	m_hPressAnyKey = DNULL;
	m_hLoadingWorld = DNULL;
	m_hWorldName	= DNULL;
	m_cxPressAnyKey = 0;
	m_cyPressAnyKey = 0;
	m_hLoading = DNULL;
	m_cxLoading = 0;
	m_cyLoading = 0;

	VEC_INIT(m_vCameraOffset);
	ROT_INIT(m_rRotation);

	m_fPitch			= 0.0f;
	m_fYaw				= 0.0f;

	m_fFireJitterPitch	= 0.0f;

	m_dwPlayerFlags		= 0;
	m_nPlayerMode		= PM_MODE_FOOT;
	m_ePlayerState		= PS_UNKNOWN;
	m_bSpectatorMode	= DFALSE;
	m_bMoving			= DFALSE;
	m_bMovingSide		= DFALSE;
	m_bOnGround			= DTRUE;
	m_bTweakingWeapon	= DFALSE;
	m_bTweakingWeaponMuzzle	= DFALSE;

	m_fBobHeight		= 0.0f;
	m_fBobWidth			= 0.0f;
	m_fBobAmp			= 0.0f;
	m_fBobPhase			= 0.0f;
	m_fSwayPhase		= 0.0f;
	m_fVelMagnitude		= 0.0f;

	VEC_INIT(m_vShakeAmount);

	m_bTintScreen		= DFALSE;
	m_fTintTime			= 0.0f;
	m_fTintStart		= 0.0f;
	m_fTintRampUp		= 0.0f;
	m_fTintRampDown		= 0.0f;
	VEC_INIT(m_vTintColor);

	memset (m_strCurrentWorldName, 0, 256);
	memset (m_strMoviesDir, 0, 256);

	VEC_SET (m_vLightScaleInfrared, 0.8f, 0.1f, 0.1f);
	VEC_SET (m_vLightScaleNightVision, 0.1f, 1.0f, 0.1f);
	VEC_SET (m_vLightScaleObjectives, 0.3f, 0.3f, 0.3f);
	VEC_SET (m_vCurContainerLightScale, -1.0f, -1.0f, -1.0f);
	
	m_fCantIncrement	= 0.009f; // in radians (.5 degrees)
	m_fCantMaxDist		= 0.035f; // in radians (2.0 degrees)
	m_fCamCant			= 0.0f;
	
	m_fCurrentFovX			= DEG2RAD(FOV_NORMAL);
	m_bZoomView				= DFALSE;
	m_bOldZoomView			= DFALSE;
	m_bZooming				= DFALSE;
	m_fSaveLODScale			= DEFAULT_LOD_SCALE;
	m_bHandledStartup		= DFALSE;
	m_bInWorld				= DFALSE;
	m_bLoadingWorld			= DFALSE;
	m_bStartedLevel			= DFALSE;

	m_bStartedDuckingDown	= DFALSE;
	m_bStartedDuckingUp		= DFALSE;
	m_fCamDuck				= 0.0f;
	m_fDuckDownV			= -75.0f;		
	m_fDuckUpV				= 75.0f;
	m_fMaxDuckDistance		= -20.0f;
	m_fStartDuckTime		= 0.0f;

	m_h3rdPersonCrosshair	= DNULL;
	m_hContainerSound		= DNULL;
	m_eCurContainerCode		= CC_NONE;
	m_bUnderwater			= DFALSE;

	m_bShowPlayerPos		= DFALSE;
	m_hDebugInfo			= DNULL;
	m_bDebugInfoChanged		= DFALSE;
	m_hLeakLines			= DNULL;

	m_bAdjustLightScale		= DFALSE;
	m_fContainerStartTime	= -1.0f;
	m_fFovXFXDir			= 1.0f;
	m_fLastTime				= 0.0f;

	m_bAdvancedDisableMusic			= DFALSE;
	m_bAdvancedDisableSound			= DFALSE;
	m_bAdvancedDisableMovies		= DFALSE;
	m_bAdvancedEnableOptSurf		= DFALSE;
	m_bAdvancedDisableLightMap		= DFALSE;
	m_bAdvancedEnableTripBuf		= DFALSE;
	m_bAdvancedDisableDx6Cmds		= DFALSE;
	m_bAdvancedEnableTJuncs			= DFALSE;
	m_bAdvancedDisableFog			= DFALSE;
	m_bAdvancedDisableLines			= DFALSE;
	m_bAdvancedDisableModelFB		= DFALSE;
	m_bAdvancedEnablePixelDoubling	= DFALSE;
	m_bAdvancedEnableMultiTexturing = DFALSE;
	m_bAdvancedDisableJoystick		= DFALSE;
	
	m_bPanSky				= DFALSE;
	m_fPanSkyOffsetX		= 1.0f;
	m_fPanSkyOffsetZ		= 1.0f;
	m_fPanSkyScaleX			= 1.0f;
	m_fPanSkyScaleZ			= 1.0f;
	m_fCurSkyXOffset		= 0.0f;
	m_fCurSkyZOffset		= 0.0f;

	VEC_SET(m_vCurModelGlow, 127.0f, 127.0f, 127.0f);
	VEC_SET(m_vMaxModelGlow, 255.0f, 255.0f, 255.0f);
	VEC_SET(m_vMinModelGlow, 50.0f, 50.0f, 50.f);
	m_fModelGlowCycleTime	= 0.0f;
	m_bModelGlowCycleUp		= DTRUE;

	m_bFirstUpdate			= DFALSE;
	m_bRestoringGame		= DFALSE;
	m_bQuickSave			= DFALSE;
	m_bGameMusicPaused		= DFALSE;

	m_bCameraPosInited		= DFALSE;

	m_resSoundInit			= LT_ERROR;
	m_eDifficulty			= GD_NORMAL;

	m_eMusicLevel			= CMusic::MUSICLEVEL_SILENCE;
	m_bAnime				= DFALSE;
	m_bGameOver				= DFALSE;

	m_fEarliestRespawnTime	= 0.0f;
	m_hBoundingBox			= DNULL;

	m_bPlayerPosSet			= DFALSE;

	m_fNextSoundReverbTime	= 0.0f;
	m_bUseReverb			= DFALSE;
	m_fReverbLevel			= 0.0f;
	VEC_INIT( m_vLastReverbPos );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::~CRiotClientShell()
//
//	PURPOSE:	Destruction
//
// ----------------------------------------------------------------------- //
			
CRiotClientShell::~CRiotClientShell()
{
	g_pRiotClientShell = DNULL;

	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if(m_MoveMgr)
	{
		delete m_MoveMgr;
		m_MoveMgr = DNULL;
	}

	if (m_hGameMessage)
	{
		pClientDE->DeleteSurface(m_hGameMessage);
		m_hGameMessage = NULL;
	}

	RemoveBumperScreen();
	SetMenuMusic(DFALSE);

	if (m_hDebugInfo)
	{
		pClientDE->DeleteSurface(m_hDebugInfo);
		m_hDebugInfo = NULL;
	}

	if (m_hBoundingBox)
	{
		pClientDE->DeleteObject(m_hBoundingBox);
	}

	if (m_hTransmissionImage) 
	{
		pClientDE->DeleteSurface (m_hTransmissionImage);
	}

	if (m_hTransmissionText) 
	{
		pClientDE->DeleteSurface (m_hTransmissionText);
	}

	if (m_pMenuPolygrid)
	{
		HLOCALOBJ hObj = m_pMenuPolygrid->GetServerObj();
		if (hObj)
		{
			pClientDE->DeleteObject(hObj);
		}

		delete m_pMenuPolygrid;
		m_pMenuPolygrid = DNULL;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::InitSound
//
//	PURPOSE:	Initialize the sounds
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::InitSound()
{
	Sound3DProvider *pSound3DProviderList, *pSound3DProvider;
	InitSoundInfo soundInfo;
	ReverbProperties reverbProperties;
	HCONSOLEVAR hVar;
	DDWORD dwProviderID;
	char sz3dSoundProviderName[_MAX_PATH + 1];
	int nError;

	m_resSoundInit = LT_ERROR;

	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;

	if (AdvancedDisableSound())
		return;

	INITSOUNDINFO_INIT(soundInfo);

	// Reload the sounds if there are any
	soundInfo.m_dwFlags				= INITSOUNDINFOFLAG_RELOADSOUNDS;

	// Get the 3d sound provider id.
	hVar = pClientDE->GetConsoleVar( "3DSoundProvider" );
	if( hVar )
	{
		dwProviderID = ( DDWORD )pClientDE->GetVarValueFloat( hVar );
	}
	else
		dwProviderID = SOUND3DPROVIDERID_NONE;

	// Can also be set by provider name, in which case the id will be set to UNKNOWN.
	if( dwProviderID == SOUND3DPROVIDERID_NONE || dwProviderID == SOUND3DPROVIDERID_UNKNOWN )
	{
		sz3dSoundProviderName[0] = 0;
		hVar = pClientDE->GetConsoleVar( "3DSoundProviderName" );
		if( hVar )
		{
			SAFE_STRCPY( sz3dSoundProviderName, pClientDE->GetVarValueString( hVar ));
			dwProviderID = SOUND3DPROVIDERID_UNKNOWN;
		}
	}

	// See if the provider exists.
	if( dwProviderID != SOUND3DPROVIDERID_NONE )
	{
		pClientDE->GetSound3DProviderLists( pSound3DProviderList, DFALSE );
		if( !pSound3DProviderList )
		{
			m_resSoundInit = LT_NO3DSOUNDPROVIDER;
			return;
		}

		pSound3DProvider = pSound3DProviderList;
		while( pSound3DProvider )
		{
			// If the provider is selected by name, then compare the names.
			if( dwProviderID == SOUND3DPROVIDERID_UNKNOWN )
			{
				if( _mbscmp(( const unsigned char * )sz3dSoundProviderName, ( const unsigned char * )pSound3DProvider->m_szProvider ) == 0 )
					break;
			}
			// Or compare by the id's.
			else if( pSound3DProvider->m_dwProviderID == dwProviderID )
				break;

			// Not this one, try next one.
			pSound3DProvider = pSound3DProvider->m_pNextProvider;
		}

		// Check if we found one.
		if( pSound3DProvider )
		{
			// Use this provider.
			SAFE_STRCPY( soundInfo.m_sz3DProvider, pSound3DProvider->m_szProvider );

			// Get the maximum number of 3d voices to use.
			hVar = pClientDE->GetConsoleVar( "Max3DVoices" );
			if( hVar )
			{
				soundInfo.m_nNum3DVoices = ( DBYTE )g_pClientDE->GetVarValueFloat( hVar );
			}
			else
				soundInfo.m_nNum3DVoices = 16;
		}

		pClientDE->ReleaseSound3DProviderList( pSound3DProviderList );
	}

	// Get the maximum number of sw voices to use.
	hVar = pClientDE->GetConsoleVar( "MaxSWVoices" );
	if( hVar )
	{
		soundInfo.m_nNumSWVoices = ( DBYTE )g_pClientDE->GetVarValueFloat( hVar );
	}
	else
		soundInfo.m_nNumSWVoices = 32;

	soundInfo.m_nSampleRate			= 22050;
	soundInfo.m_nBitsPerSample		= 16;
	soundInfo.m_nVolume				= (unsigned short)pSettings->SoundVolume();
	
	if( !pSettings->Sound16Bit( ))
		soundInfo.m_dwFlags |= INITSOUNDINFOFLAG_CONVERT16TO8;

	DFLOAT fMinStreamSoundTime = 0.5f;
	hVar = pClientDE->GetConsoleVar ("MinStreamTime");
	if (hVar)
	{
		fMinStreamSoundTime = pClientDE->GetVarValueFloat (hVar);
		fMinStreamSoundTime = (fMinStreamSoundTime < 0.2f ? 0.2f :
							   (fMinStreamSoundTime > 1.5f ? 1.5f : fMinStreamSoundTime));
	}
	soundInfo.m_fMinStreamTime = fMinStreamSoundTime;
	soundInfo.m_fDistanceFactor = 1.0f / 64.0f;

	// Go initialize the sounds.
	m_bUseReverb = DFALSE;
	if(( m_resSoundInit = pClientDE->InitSound(&soundInfo)) == LT_OK )
	{
		if( soundInfo.m_dwResults & INITSOUNDINFORESULTS_REVERB )
		{
			m_bUseReverb = DTRUE;
		}

		hVar = pClientDE->GetConsoleVar( "ReverbLevel" );
		if( hVar )
		{
			m_fReverbLevel = g_pClientDE->GetVarValueFloat( hVar );
		}
		else
			m_fReverbLevel = 1.0f;

		reverbProperties.m_dwParams = REVERBPARAM_VOLUME;
		reverbProperties.m_fVolume = m_fReverbLevel;
		pClientDE->SetReverbProperties( &reverbProperties );
	}
	else
	{
		if( m_resSoundInit == LT_NO3DSOUNDPROVIDER )
		{
			nError = IDS_MESSAGE_INVALID3DSOUNDPROVIDER;
		}
		else
		{
			nError = IDS_SOUNDNOTINITED;
		}

		DoMessageBox( nError, TH_ALIGN_CENTER );
	}

	return;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::CSPrint
//
//	PURPOSE:	Displays a line of text on the client
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::CSPrint (char* msg, ...)
{
	CClientDE* pClientDE = GetClientDE();

	// parse the message

	char pMsg[256];
	va_list marker;
	va_start (marker, msg);
	int nSuccess = vsprintf (pMsg, msg, marker);
	va_end (marker);

	if (nSuccess < 0) return;
	
	// now display the message

	m_messageMgr.AddLine(pMsg);

	pClientDE->CPrint(pMsg);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ShowSplash
//
//	PURPOSE:	Called after engine is fully initialized
//				to show a splash screen
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::ShowSplash()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;	
	
	HSURFACE hSplash = pClientDE->CreateSurfaceFromBitmap ("interface/splash.pcx");
	if (!hSplash) return;

	HSURFACE hScreen = pClientDE->GetScreenSurface();
	DDWORD nWidth = 0;
	DDWORD nHeight = 0;
	
	pClientDE->GetSurfaceDims (hScreen, &nWidth, &nHeight);
	
	DRect rcDst;
	rcDst.left = rcDst.top = 0;
	rcDst.right = nWidth;
	rcDst.bottom = nHeight;

	pClientDE->GetSurfaceDims (hSplash, &nWidth, &nHeight);
	DRect rcSrc;
	rcSrc.left = rcSrc.top = 0;
	rcSrc.right = nWidth;
	rcSrc.bottom = nHeight;

	pClientDE->ClearScreen (DNULL, CLEARSCREEN_SCREEN);
	pClientDE->Start3D();
	pClientDE->RenderCamera (m_hCamera);
	pClientDE->StartOptimized2D();
	pClientDE->ScaleSurfaceToSurface (hScreen, hSplash, &rcDst, &rcSrc);
	pClientDE->EndOptimized2D();
	pClientDE->End3D();
	pClientDE->FlipScreen (FLIPSCREEN_CANDRAWCONSOLE);
	AddToClearScreenCount();

	pClientDE->DeleteSurface (hSplash);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnEngineInitialized
//
//	PURPOSE:	Called after engine is fully initialized
//				Handle object initialization here
//
// ----------------------------------------------------------------------- //

DDWORD CRiotClientShell::OnEngineInitialized(struct RMode_t *pMode, DGUID *pAppGuid)
{
	//CWinUtil::DebugBreak();

	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return LT_OK;	

	*pAppGuid = SHOGOGUID;


	float nStartTime = CWinUtil::GetTime();

	m_MoveMgr = new CMoveMgr(this);
	if(!m_MoveMgr)
		return LT_ERROR;

	// Init our client send rate var.
	g_CV_CSendRate.Init(pClientDE, "CSendRate", NULL, DEFAULT_CSENDRATE);
	

	HCONSOLEVAR hIsSet = pClientDE->GetConsoleVar("UpdateRateInitted");
	if(hIsSet && pClientDE->GetVarValueFloat(hIsSet) == 1.0f)
	{
		// Ok it's already initialized.
	}
	else
	{
		// Initialize the update rate.
		pClientDE->RunConsoleString("+UpdateRateInitted 1");
		pClientDE->RunConsoleString("+UpdateRate 6");
	}


	m_MoveMgr->Init(pClientDE);

	pClientDE->RegisterConsoleProgram("LeakFile", LeakFileFn);
	pClientDE->RegisterConsoleProgram("Connect", ConnectFn);
	pClientDE->RegisterConsoleProgram("FragSelf", FragSelfFn);
	pClientDE->RegisterConsoleProgram("Record", RecordFn);
	pClientDE->RegisterConsoleProgram("PlayDemo", PlayDemoFn);
	pClientDE->RegisterConsoleProgram("InitSound", InitSoundFn);

	pClientDE->SetModelHook((ModelHookFn)DefaultModelHook, this);

	// Make sure the save directory exists...

	if (!CWinUtil::DirExist ("Save"))
	{
		CWinUtil::CreateDir ("Save");
	}


	// Add to NumRuns count...
	
	float nNumRuns = 0.0f;
	HCONSOLEVAR hVar = pClientDE->GetConsoleVar ("NumRuns");
	if (hVar)
	{
		nNumRuns = pClientDE->GetVarValueFloat (hVar);
	}
	nNumRuns++;
	
	char strConsole[64];
	sprintf (strConsole, "+NumRuns %f", nNumRuns);
	pClientDE->RunConsoleString (strConsole);

	// check advanced options...

	hVar = pClientDE->GetConsoleVar ("DisableMusic");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedDisableMusic = DTRUE;
	hVar = pClientDE->GetConsoleVar ("DisableSound");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedDisableSound = DTRUE;
	hVar = pClientDE->GetConsoleVar ("DisableMovies");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedDisableMovies = DTRUE;
	hVar = pClientDE->GetConsoleVar ("EnableOptSurf");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedEnableOptSurf = DTRUE;
	hVar = pClientDE->GetConsoleVar ("DisableLightMap");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedDisableLightMap = DTRUE;
	hVar = pClientDE->GetConsoleVar ("EnableTripBuf");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedEnableTripBuf = DTRUE;
	hVar = pClientDE->GetConsoleVar ("DisableDx6Cmds");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedDisableDx6Cmds = DTRUE;
	hVar = pClientDE->GetConsoleVar ("EnableTJuncs");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedEnableTJuncs = DTRUE;
	hVar = pClientDE->GetConsoleVar ("DisableFog");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedDisableFog = DTRUE;
	hVar = pClientDE->GetConsoleVar ("DisableLines");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedDisableLines = DTRUE;
	hVar = pClientDE->GetConsoleVar ("DisableModelFB");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedDisableModelFB = DTRUE;
	hVar = pClientDE->GetConsoleVar ("EnablePixDub");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedEnablePixelDoubling = DTRUE;
	hVar = pClientDE->GetConsoleVar ("EnableMultiTex");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedEnableMultiTexturing = DTRUE;
	hVar = pClientDE->GetConsoleVar ("DisableJoystick");
	if (hVar && pClientDE->GetVarValueFloat (hVar))		m_bAdvancedDisableJoystick = DTRUE;

	// record the original state of sound and music

	hVar = pClientDE->GetConsoleVar ("SoundEnable");
	m_bSoundOriginallyEnabled = (DBOOL) pClientDE->GetVarValueFloat (hVar);
	
	hVar = pClientDE->GetConsoleVar ("MusicEnable");
	m_bMusicOriginallyEnabled = (DBOOL) pClientDE->GetVarValueFloat (hVar);

	hVar = pClientDE->GetConsoleVar ("ModelFullbrite");
	m_bModelFBOriginallyEnabled	= (DBOOL) pClientDE->GetVarValueFloat (hVar);
	
	hVar = pClientDE->GetConsoleVar ("LightMap");
	m_bLightmappingOriginallyEnabled = (DBOOL) pClientDE->GetVarValueFloat (hVar);

	// implement any advanced options here (before renderer is started)

	hVar = pClientDE->GetConsoleVar ("SoundEnable");
	if (!hVar && !AdvancedDisableSound())
	{
		pClientDE->RunConsoleString ("SoundEnable 1");
	}
	hVar = pClientDE->GetConsoleVar ("MusicEnable");
	if (!hVar && !AdvancedDisableMusic())
	{
		pClientDE->RunConsoleString ("MusicEnable 1");
	}

	if (AdvancedEnableOptSurf())
	{
		pClientDE->RunConsoleString ("OptimizeSurfaces 1");
	}
	if (AdvancedEnableTripBuf())
	{
		pClientDE->RunConsoleString ("TripleBuffer 1");
	}
	if (AdvancedDisableDx6Cmds())
	{
		pClientDE->RunConsoleString ("UseDX6Commands 0");
	}
	if (AdvancedEnableTJuncs())
	{
		pClientDE->RunConsoleString ("FixTJunc 1");
	}
	if (AdvancedDisableFog())
	{
		pClientDE->RunConsoleString ("FogEnable 0");
	}
	if (AdvancedDisableLines())
	{
		pClientDE->RunConsoleString ("DrawLineSystems 0");
	}
	if (AdvancedDisableModelFB())
	{
		pClientDE->RunConsoleString ("ModelFullBrite 0");
	}
	if (AdvancedEnablePixelDoubling())
	{
		pClientDE->RunConsoleString ("PixelDouble 1");
	}
	if (AdvancedEnableMultiTexturing())
	{
		pClientDE->RunConsoleString ("Force1Pass 1");
	}
	if (AdvancedDisableJoystick())
	{
		pClientDE->RunConsoleString ("JoystickDisable 1");
	}

	// Determine if we were lobby launched...

	DBOOL bNetworkGameStarted = DFALSE;
	DBOOL bLobbyLaunched      = DFALSE;

	DRESULT dr = pClientDE->IsLobbyLaunched("dplay2");
	if (dr == LT_OK)
	{
		BOOL bRet = NetStart_DoLobbyLaunchWizard(pClientDE);
		if (bRet)
		{
			bLobbyLaunched      = DTRUE;
			bNetworkGameStarted = DTRUE;
		}
		else
		{
			pClientDE->Shutdown();
			return LT_ERROR;
		}
	}


	// Check for console vars that say we should do networking setup stuff...

	BOOL bDoNetStuff = DFALSE;

	hVar = pClientDE->GetConsoleVar ("Multiplayer");
	if (hVar)
	{
		if ((DBOOL)pClientDE->GetVarValueFloat(hVar)) bDoNetStuff = DTRUE;
	}
	pClientDE->RunConsoleString ("+Multiplayer 0");

	hVar = pClientDE->GetConsoleVar ("Connect");
	if (hVar)
	{
		char* sVar = pClientDE->GetVarValueString(hVar);
		if (sVar && sVar[0] != '\0' && strcmp(sVar, "0") != 0) bDoNetStuff = DTRUE;
	}

	hVar = pClientDE->GetConsoleVar ("ConnectPlr");
	if (hVar)
	{
		char* sVar = pClientDE->GetVarValueString(hVar);
		if (sVar && sVar[0] != '\0' && strcmp(sVar, "0") != 0) bDoNetStuff = DTRUE;
	}

	if (bLobbyLaunched) bDoNetStuff = DFALSE;

//#ifdef _DEMO
//	bDoNetStuff = DFALSE;
//#endif


	// Determine if we should display the networking dialogs...

	if (bDoNetStuff)
	{
		bNetworkGameStarted = NetStart_DoWizard (pClientDE);

		if (!bNetworkGameStarted)
		{
			pClientDE->Shutdown();
			return LT_ERROR;
		}
	}

	// Initialize the renderer

	DRESULT hResult = pClientDE->SetRenderMode(pMode);
	if (hResult != LT_OK)
	{
		pClientDE->DebugOut("Shogo Error: Couldn't set render mode!\n");

		RMode rMode;

		// If an error occurred, try 640x480x16...
		
		rMode.m_Width		= 640;
		rMode.m_Height		= 480;
		rMode.m_BitDepth	= 16;
		rMode.m_bHardware	= pMode->m_bHardware;

		sprintf(rMode.m_RenderDLL, "%s", pMode->m_RenderDLL);
		sprintf(rMode.m_InternalName, "%s", pMode->m_InternalName);
		sprintf(rMode.m_Description, "%s", pMode->m_Description);

		pClientDE->DebugOut("Setting render mode to 640x480x16...\n");
		
		if (pClientDE->SetRenderMode(&rMode) != LT_OK)
		{
			// Okay, that didn't work, looks like we're stuck with software...
		
			rMode.m_bHardware = DFALSE;

			sprintf(rMode.m_RenderDLL, "soft.ren");
			sprintf(rMode.m_InternalName, "");
			sprintf(rMode.m_Description, "");

			pClientDE->DebugOut("Setting render mode to software...\n");

			if (pClientDE->SetRenderMode(&rMode) != LT_OK)
			{
				pClientDE->DebugOut("Shogo Error: Couldn't set software render mode.\nShutting down Shogo...\n");
				pClientDE->ShutdownWithMessage("Shogo Error: Couldn't set software render mode.\nShutting down Shogo...\n");
				return LT_OK;
			} 
		}
	}

	// show the splash screen

	ShowSplash();

	// Create the camera...

	DDWORD dwWidth = 640, dwHeight = 480;
	pClientDE->GetSurfaceDims(pClientDE->GetScreenSurface(), &dwWidth, &dwHeight);

	ObjectCreateStruct theStruct;
	INIT_OBJECTCREATESTRUCT(theStruct);

	theStruct.m_ObjectType = OT_CAMERA;

	m_hCamera = pClientDE->CreateObject(&theStruct);
	pClientDE->SetCameraRect(m_hCamera, DFALSE, 0, 0, dwWidth, dwHeight);
	
	DFLOAT y = (m_fCurrentFovX * dwHeight) / dwWidth;
	pClientDE->SetCameraFOV(m_hCamera, m_fCurrentFovX, y);

	
	// Attempt to find the movies path

	CWinUtil::GetMoviesPath (m_strMoviesDir);

	// Initialize the physics states...

	g_normalPhysicsState.m_pClientDE = pClientDE;
	VEC_SET(g_normalPhysicsState.m_GravityAccel, 0.0f, -1000.0f, 0.0f);
	g_normalPhysicsState.m_VelocityDampen = 0.5f;

	g_waterPhysicsState.m_pClientDE = pClientDE;
	VEC_SET(g_waterPhysicsState.m_GravityAccel, 0.0f, -500.0f, 0.0f);
	g_waterPhysicsState.m_VelocityDampen = 0.25f;


	// Interface stuff...

	m_messageMgr.Init (pClientDE, this);
	m_messageMgr.Enable (DTRUE);
	m_cheatMgr.Init (pClientDE);
	m_ClientInfo.Init (pClientDE);
	m_inventory.Init (pClientDE, this);
	m_infoDisplay.Init (pClientDE);
	m_LightScaleMgr.Init (pClientDE);

	// Setup the music stuff...
	if (!m_Music.IsInitialized( ) && !AdvancedDisableMusic())
	{
		HCONSOLEVAR hConsoleVar;
		hConsoleVar = pClientDE->GetConsoleVar("musictype");

		// Default to ima if music never set before...
		if( !hConsoleVar )
		{
			m_Music.Init( pClientDE, DTRUE );
		}
		else
		{
			// See if they want cdaudio or ima...
			if( stricmp( pClientDE->GetVarValueString(hConsoleVar), "cdaudio") == 0 )
				m_Music.Init( pClientDE, DFALSE );
			else
				m_Music.Init( pClientDE, DTRUE );
		}
	}

	// back to interface stuff...

	if (!m_menu.Init (pClientDE, this))
	{
		CSPrint ("Could not init menu");
		
		// if we couldn't init, something critical must have happened (like no render dlls)
		pClientDE->ShutdownWithMessage("Could not initialize menu");
		return LT_OK;
	}

	if (!m_stats.Init (pClientDE, this))
	{
		CSPrint ("Could not init player stats");
	}

	m_objectives.Init (pClientDE, this);

	// player camera (non-1st person) stuff...

	if (!m_playerCamera.Init(pClientDE))
	{
		CSPrint ("Could not init player camera!");
	}
	else
	{
		DVector vOffset;
		VEC_SET(vOffset, 0.0f, 30.0, 0.0f);

		m_playerCamera.SetDistUp(10.0f);
		m_playerCamera.SetPointAtOffset(vOffset);
		m_playerCamera.SetChaseOffset(vOffset);
		m_playerCamera.SetCameraState(CPlayerCamera::SOUTH);
		m_playerCamera.GoFirstPerson();
	}


	// init the "press any key" surface

	m_hPressAnyKey = CTextHelper::CreateSurfaceFromString (pClientDE, m_menu.GetFont12s(), IDS_PRESSANYKEY);
	pClientDE->GetSurfaceDims (m_hPressAnyKey, &m_cxPressAnyKey, &m_cyPressAnyKey);
	
	// init the "loading" surface

	m_hLoading = pClientDE->CreateSurfaceFromBitmap ("interface/loading.pcx");
	pClientDE->GetSurfaceDims (m_hLoading, &m_cxLoading, &m_cyLoading);


	// Init the special fx mgr...

	if (!m_sfxMgr.Init(pClientDE)) return LT_OK;


	// Post an error if the sounds didn't init correctly

	// Okay, start the game...

	if (!bNetworkGameStarted)
	{
		StartGameRequest request;
		memset( &request, 0, sizeof( StartGameRequest ));

		NetStart_ClearGameStruct();  // Start with clean slate
		request.m_pGameInfo   = NetStart_GetGameStruct();
		request.m_GameInfoLen = sizeof(NetGame_t);

		HCONSOLEVAR hVar;

		if (hVar = pClientDE->GetConsoleVar("NumConsoleLines"))
		{
			DFLOAT fLines = pClientDE->GetVarValueFloat(hVar);
			//m_messageMgr.SetMaxMessages((int)fLines);
			pClientDE->RunConsoleString("+NumConsoleLines 0");
		}
		
		if (hVar = pClientDE->GetConsoleVar("runworld"))
		{
			strncpy (request.m_WorldName, pClientDE->GetVarValueString(hVar), MAX_SGR_STRINGLEN - 1);
			request.m_Type = STARTGAME_NORMAL;
			DRESULT dr = pClientDE->StartGame(&request);
			if (dr != LT_OK)
			{
				HSTRING hString = pClientDE->FormatString (IDS_NOLOADLEVEL);
				pClientDE->ShutdownWithMessage (pClientDE->GetStringData (hString));
				pClientDE->FreeString (hString);
				return LT_ERROR;
			}
		}
		else
		{
			// play the movies

			PlayIntroMovies (pClientDE);
		}
	}
	else
	{
		m_nGameState = GS_LOADINGLEVEL;
	}

	float nEndTime = CWinUtil::GetTime();
	char strTimeDiff[64];
	sprintf (strTimeDiff, "Game initialized in %f seconds.\n", nEndTime - nStartTime);
	CWinUtil::DebugOut (strTimeDiff);

	
	// Check for playdemo.
	if (hVar = pClientDE->GetConsoleVar("playdemo"))
	{
		DoLoadWorld("", NULL, NULL, LOAD_NEW_GAME, NULL, pClientDE->GetVarValueString(hVar));
	}

	return LT_OK;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnEngineTerm()
//
//	PURPOSE:	Called before the engine terminates itself
//				Handle object destruction here
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnEngineTerm()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
	
	if (m_hCamera)
	{
		pClientDE->DeleteObject(m_hCamera);
		m_hCamera = DNULL;
	}

	if (m_hPressAnyKey) 
	{
		pClientDE->DeleteSurface (m_hPressAnyKey);
		m_hPressAnyKey = DNULL;
	}

	if (m_hLoadingWorld) 
	{
		pClientDE->DeleteSurface (m_hLoadingWorld);
		m_hLoadingWorld = DNULL;
	}

	if (m_hWorldName) 
	{
		pClientDE->DeleteSurface (m_hWorldName);
		m_hWorldName = DNULL;
	}

	if (m_hLoading) 
	{
		pClientDE->DeleteSurface (m_hLoading);
		m_hLoading = DNULL;
	}

	m_messageMgr.Term();
	m_menu.Term();
	m_stats.Term();
	m_Music.Term( );
	m_infoDisplay.Term();
	m_LightScaleMgr.Term();
	
	if (m_bSoundOriginallyEnabled && AdvancedDisableSound())
	{
		pClientDE->RunConsoleString ("SoundEnable 1");
	}

	if (m_bMusicOriginallyEnabled && AdvancedDisableMusic())
	{
		pClientDE->RunConsoleString ("MusicEnable 1");
	}

	if (m_bModelFBOriginallyEnabled)
	{
		pClientDE->RunConsoleString ("ModelFullbrite 1");
	}

	if (m_bLightmappingOriginallyEnabled)
	{
		pClientDE->RunConsoleString ("LightMap 1");
	}

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnEvent()
//
//	PURPOSE:	Called for asynchronous errors that cause the server
//				to shut down
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnEvent(DDWORD dwEventID, DDWORD dwParam)
{
	CClientDE *pClientDE;
	DDWORD cxLoading, cyLoading, cxScreen, cyScreen;
	HSURFACE hLoading;
	HSURFACE hScreen;
	CBitmapFont *pFont;

	pClientDE = GetClientDE();
	if(!pClientDE)
		return;

	switch (dwEventID)
	{
		// Called when the renderer has switched into
		// the new mode but before it reloads all the textures
		// so you can display a loading screen.
		
		case LTEVENT_RENDERALMOSTINITTED :
			if(m_bSwitchingModes)
			{
				ClearAllScreenBuffers();
				
				hScreen = pClientDE->GetScreenSurface();
				pClientDE->GetSurfaceDims (hScreen, &cxScreen, &cyScreen);

				pClientDE->Start3D();
				pClientDE->StartOptimized2D();

				hLoading = pClientDE->CreateSurfaceFromBitmap ("interface/blanktag.pcx");
				if(hLoading)
				{
					pClientDE->GetSurfaceDims (hLoading, &cxLoading, &cyLoading);
					pClientDE->DrawSurfaceToSurface(hScreen, hLoading, DNULL, ((int)cxScreen - (int)cxLoading) / 2, ((int)cyScreen - (int)cyLoading) / 2);
					pClientDE->DeleteSurface(hLoading);
					
					pFont = m_menu.GetFont08n();
					if(pFont)
					{
						hLoading = CTextHelper::CreateSurfaceFromString(pClientDE,
							pFont, IDS_REINITIALIZING_RENDERER);
						if(hLoading)
						{
							pClientDE->GetSurfaceDims (hLoading, &cxLoading, &cyLoading);
							pClientDE->DrawSurfaceToSurfaceTransparent(hScreen, hLoading, 
								DNULL, ((int)cxScreen - (int)cxLoading) / 2, ((int)cyScreen - (int)cyLoading) / 2,
								SETRGB_T(0,0,0));
							pClientDE->DeleteSurface(hLoading);
						}
					}
				}

				pClientDE->EndOptimized2D();
				pClientDE->End3D();
				pClientDE->FlipScreen(0);
			}
		break;

		// Client disconnected from server.  dwParam will 
		// be a error flag found in de_codes.h.
		
		case LTEVENT_DISCONNECT :

			if((dwParam & ~ERROR_DISCONNECT) == LT_INVALIDNETVERSION)
			{
				if(IsMultiplayerGame())
				{
					ClearAllScreenBuffers();
					SetMenuMode(DTRUE, DFALSE);
					DoMessageBox(IDS_DISCONNECTED_WRONG_VERSION, TH_ALIGN_CENTER);
				}
			}
			else
			{
				if(IsMultiplayerGame())
				{
					ClearAllScreenBuffers();
					SetMenuMode(DTRUE, DFALSE);
	//#ifdef _DEMO
					//DoMessageBox(IDS_DEMODISCONNECT, TH_ALIGN_CENTER);
	//#else
					DoMessageBox(IDS_DISCONNECTED_FROM_SERVER, TH_ALIGN_CENTER);
	//#endif
				}
			}
			break;

		// Engine shutting down.  dwParam will be a error 
		// flag found in de_codes.h.

		case LTEVENT_SHUTDOWN :
		break;
		
		// The renderer was initialized.  This is called if 
		// you call SetRenderMode or if focus was lost and regained.

		case LTEVENT_RENDERINIT :
		break;

		// The renderer is being shutdown.  This happens when
		// ShutdownRender is called or if the app loses focus.

		case  LTEVENT_RENDERTERM :
		break;

		default :
		{
			DDWORD nStringID = IDS_UNSPECIFIEDERROR;
			SetMenuMode (DTRUE, DFALSE);
			DoMessageBox (nStringID, TH_ALIGN_CENTER);
		}
		break;
	}
}


DRESULT CRiotClientShell::OnObjectMove(HOBJECT hObj, DBOOL bTeleport, DVector *pPos)
{
	return m_MoveMgr->OnObjectMove(hObj, bTeleport, pPos);
}


DRESULT CRiotClientShell::OnObjectRotate(HOBJECT hObj, DBOOL bTeleport, DRotation *pNewRot)
{
	return m_MoveMgr->OnObjectRotate(hObj, bTeleport, pNewRot);
}


DRESULT	CRiotClientShell::OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo, float forceMag)
{
	m_sfxMgr.OnTouchNotify(hMain, pInfo, forceMag);
	return LT_OK;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::PreLoadWorld()
//
//	PURPOSE:	Called before world loads
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::PreLoadWorld(char *pWorldName)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
	
	if (IsMainWindowMinimized())
	{
		NetStart_RestoreMainWnd();
		MainWindowRestored();
	}

	char* pStrWorldOnly = &pWorldName[strlen(pWorldName) - 1];
	while (*pStrWorldOnly != '\\' && pStrWorldOnly != pWorldName) pStrWorldOnly--;
	if (pStrWorldOnly != pWorldName) pStrWorldOnly++;

	SAFE_STRCPY(m_strCurrentWorldName, pStrWorldOnly);
	m_objectives.SetLevelName (m_strCurrentWorldName);

	if (m_nGameState == GS_BUMPER || m_nGameState == GS_LOADINGLEVEL)
	{
		UpdateLoadingLevel();
	}

	return;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnEnterWorld()
//
//	PURPOSE:	Handle entering world
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnEnterWorld()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;


	// if we are in a multiplayer game, set the correct game state...

	if (m_nGameState == GS_MPLOADINGLEVEL)
	{
		m_nGameState = GS_PLAYING;
	}

	m_bFirstUpdate = DTRUE;
	m_ePlayerState = PS_UNKNOWN;

	m_bPlayerPosSet = DFALSE;

	m_bNewObjective	= DFALSE;
	m_bDrawHud		= DTRUE;

	AddToClearScreenCount();

	pClientDE->ClearInput();

	m_bHandledStartup = DFALSE;
	m_bInWorld		  = DTRUE;

	m_fVelMagnitude	= 0.0f;

	m_stats.OnEnterWorld(m_bRestoringGame);
	m_menu.OnEnterWorld();
	m_menu.ExitMenu(DTRUE);


	// If we're in loading level mode switch to bumper mode

	if (m_nGameState == GS_LOADINGLEVEL)
	{
		if (m_hBumperText)
		{
			PauseGame(DTRUE);
			m_nGameState = GS_BUMPER;
		}
	}

	SetExternalCamera(DFALSE);

	m_bRestoringGame		= DFALSE;
	m_bCameraPosInited		= DFALSE;
	m_nPlayerInfoChangeFlags |= CLIENTUPDATE_PLAYERROT;
	VEC_INIT( m_vLastReverbPos );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnExitWorld()
//
//	PURPOSE:	Handle exiting the world
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnExitWorld()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// See if we're changing levels if a multiplayer game (if OnExitWorld()
	// was called before we got the change level message)...

	if (IsMultiplayerGame() && m_nGameState != GS_MPLOADINGLEVEL)
	{
		HandleMPChangeLevel();
	}


	// Make sure credits are reset if playing...

	m_credits.Term();

	m_bInWorld		= DFALSE;
	m_bStartedLevel	= DFALSE;

	m_LightScaleMgr.ClearLightScale (&m_vDefaultLightScale, LightEffectWorld);

	memset (m_strCurrentWorldName, 0, 256);

	// Kill the music...
	pClientDE->StopMusic (MUSIC_IMMEDIATE);
	m_Music.TermPlayLists();
	m_eMusicLevel  = CMusic::MUSICLEVEL_SILENCE;

	if (m_h3rdPersonCrosshair)
	{
		pClientDE->DeleteObject(m_h3rdPersonCrosshair);
		m_h3rdPersonCrosshair = DNULL;
	}

	m_sfxMgr.RemoveAll();					// Remove all the sfx
	m_playerCamera.AttachToObject(DNULL);	// Detatch camera

	m_weaponModel.Reset();

	if (m_hContainerSound)
	{
		pClientDE->KillSound(m_hContainerSound);
		m_hContainerSound = DNULL;
	}

	if (m_hTransmissionSound)
	{
		pClientDE->KillSound (m_hTransmissionSound);
		m_hTransmissionSound = DNULL;
	}

	m_ClientInfo.RemoveAllClients();
	m_inventory.ShogoPowerupClear();

	m_stats.OnExitWorld();
	m_menu.OnExitWorld();

	if (m_hTransmissionImage)
	{
		pClientDE->DeleteSurface (m_hTransmissionImage);
		m_hTransmissionImage = DNULL;
	}

	if (m_hTransmissionText)
	{
		pClientDE->DeleteSurface (m_hTransmissionText);
		m_hTransmissionText = DNULL;
	}

	m_fTransmissionTimeLeft = 0.0f;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::PreUpdate()
//
//	PURPOSE:	Handle client pre-updates
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::PreUpdate()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// Conditions in which we don't want to clear the screen

	if ((m_ePlayerState == PS_UNKNOWN && m_bInWorld))
	{
		return;
	}
	
	// See if we're using an external camera now - if so, clear the screen
	// immediately, and add to the clearscreen count
	if (m_bUsingExternalCamera && !m_bWasUsingExternalCamera)
	{
		m_bWasUsingExternalCamera = DTRUE;
		AddToClearScreenCount();
	}
	else if (m_bWasUsingExternalCamera && !m_bUsingExternalCamera)
	{
		m_bWasUsingExternalCamera = DFALSE;
		AddToClearScreenCount();
	}

	if (m_bClearScreenAlways)
	{
		pClientDE->ClearScreen (DNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER);
	}
	else if (m_nClearScreenCount)
	{
		pClientDE->ClearScreen (DNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER);
		m_nClearScreenCount--;
	}
	else
	{
		pClientDE->ClearScreen (DNULL, CLEARSCREEN_RENDER);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::Update()
//
//	PURPOSE:	Handle client updates
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::Update()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// Handle first update...

	if (m_bFirstUpdate)
	{
		FirstUpdate();
	}


	// Update tint if applicable (always do this to make sure tinting
	// gets finished)...

	UpdateScreenTint();

	UpdateModelGlow();
	

	// Update any client-side special effects...

	m_sfxMgr.UpdateSpecialFX();

	
	// If there's a messagebox, draw it...

	if (m_pMessageBox)
	{
		m_pMessageBox->Draw();
		return;
	}


	// Make sure menu music is only playing in the menus (or loading a level)...

	if (m_nGameState != GS_MENU && 
		m_nGameState != GS_LOADINGLEVEL && 
		m_nGameState != GS_BUMPER)
	{
		SetMenuMusic(DFALSE);
	}


	// Make sure menu polygrid is/isn't around...

	if (m_nGameState == GS_MENU || m_nGameState == GS_PAUSED ||
		m_nGameState == GS_LOADINGLEVEL || m_nGameState == GS_BUMPER ||
		m_nGameState == GS_CREDITS || m_nGameState == GS_INTRO ||
		m_nGameState == GS_DEMO_MULTIPLAYER)
	{
		CreateMenuPolygrid();
	}
	else if (m_nGameState != GS_MENU && m_nGameState != GS_PAUSED && 
			 m_nGameState != GS_LOADINGLEVEL && m_nGameState != GS_BUMPER &&
			 m_nGameState != GS_CREDITS && m_nGameState != GS_INTRO &&
			 m_nGameState != GS_DEMO_MULTIPLAYER)
	{
		RemoveMenuPolygrid();
	}


	// Update based on the game state...

	switch (m_nGameState)
	{
		case GS_PLAYING :
		break;

		case GS_UNDEFINED:
		case GS_MPLOADINGLEVEL:
		{
			// Don't do anything
			return;
		}
		break;

		case GS_MOVIES:
		{
			UpdateMoviesState();
			return;
		}
		break;
		
		case GS_CREDITS :
		{
			UpdateCreditsState();
			return;
		}
		break;

		case GS_INTRO :
		{
			UpdateIntroState();
			return;
		}
		break;

		case GS_MENU :
		{
			UpdateMenuState();
			return;
		}
		break;

		case GS_BUMPER :
		{
			UpdateBumperState();
			return;
		}
		break;

		case GS_LOADINGLEVEL :
		{
			UpdateLoadingLevelState();
			return;
		}
		break;

		case GS_PAUSED:
		{
			UpdatePausedState();
			return;
		}
		break;

		case GS_DEMO_MULTIPLAYER :
		{
			UpdateDemoMultiplayerState();
			return;
		}
		break;

		default : break;
	}


	// Update client-side physics structs...

	SetPhysicsStateTimeStep(&g_normalPhysicsState, pClientDE->GetFrameTime());
	SetPhysicsStateTimeStep(&g_waterPhysicsState, pClientDE->GetFrameTime());
	
	

	// At this point we only want to proceed if the player is in the world...

	if (!IsPlayerInWorld()) return;


	m_MoveMgr->Update();

	UpdateSoundReverb( );
	
	// Tell the player to "start" the level...

	if (!m_bStartedLevel)
	{
		m_bStartedLevel = DTRUE;
		StartLevel();
	}


	// Update Player...
	
	UpdatePlayer();


	
	// Update sky-texture panning...

	if (m_bPanSky && !m_bGamePaused)
	{
		DFLOAT fFrameTime = pClientDE->GetFrameTime();

		m_fCurSkyXOffset += fFrameTime * m_fPanSkyOffsetX;
		m_fCurSkyZOffset += fFrameTime * m_fPanSkyOffsetZ;

		pClientDE->SetGlobalPanInfo(GLOBALPAN_SKYSHADOW, m_fCurSkyXOffset, m_fCurSkyZOffset, m_fPanSkyScaleX, m_fPanSkyScaleZ);
	}


	// update the player stats and inventory
	if (!m_bStatsSizedOff)
	{
		m_stats.Update();
	}

	// Keep track of what the player is doing...

	UpdatePlayerFlags();


	if (m_bAdjustLightScale)
	{
		AdjustLightScale();
	}


	// If in spectator mode, just do the camera stuff...
	if (m_bSpectatorMode || IsPlayerDead())
	{
		UpdateCamera();
		RenderCamera();
		return;
	}

	// Update weapon position if appropriated (Should probably remove this
	// at some point...although, it might be cool for end users???)...
	if (m_bTweakingWeapon)
	{
		UpdateWeaponPosition();
		UpdateCamera();
		RenderCamera();
		return;
	}
	else if (m_bTweakingWeaponMuzzle)
	{
		UpdateWeaponMuzzlePosition();
		UpdateCamera();
		RenderCamera();
		return;
	}

	// Update head-bob/head-cant camera offsets...
	if (m_bOnGround && !m_bZoomView) 
	{
		UpdateHeadBob();
		UpdateHeadCant();
	}				

	// Update duck camera offset...
	UpdateDuck();

	// Update the camera's position...
	UpdateCamera();

	// Render the camera and draw the interface first...then process everything
	RenderCamera();

	// Update container effects...
	UpdateContainerFX();

	// Update any debugging information...

	UpdateDebugInfo();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::PostUpdate()
//
//	PURPOSE:	Handle post updates - after the scene is rendered
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::PostUpdate()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if (m_bQuickSave)
	{
		SaveGame(QUICKSAVE_FILENAME);
		m_bQuickSave = DFALSE;
	}

	// conditions where we don't want to flip...

	if (m_nGameState == GS_MPLOADINGLEVEL)
	{
		return;
	}

	if (m_ePlayerState == PS_UNKNOWN && m_bInWorld)
	{
		return;
	}
	
	// See if the game is over...

	if (m_bGameOver && m_nGameState == GS_PLAYING)
	{
		UpdateGameOver();
	}

	pClientDE->FlipScreen (FLIPSCREEN_CANDRAWCONSOLE);

	// Check to see if we should start the world...(do after flip)...

	if (m_nGameState == GS_INTRO)
	{		
		if (m_credits.IsDone())
		{
			m_credits.Term();
			DoStartGame();
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateCamera()
//
//	PURPOSE:	Update the camera position/rotation
//
// ----------------------------------------------------------------------- //
void CRiotClientShell::UpdateSoundReverb( )
{
	DVector vPlayerDims;
	ClientIntersectInfo info;
	ClientIntersectQuery query;
	DVector vPos[6], vSegs[6];
	int i;
	float fVolume, fReverbLevel;
	DBOOL bOpen;
	HLOCALOBJ hPlayerObj;
	ReverbProperties reverbProperties;
	HCONSOLEVAR hVar;

	if( !m_bUseReverb )
		return;

	hPlayerObj = g_pClientDE->GetClientObject();
	if( !hPlayerObj )
		return;

	hVar = g_pClientDE->GetConsoleVar( "ReverbLevel" );
	if( hVar )
	{
		fReverbLevel = g_pClientDE->GetVarValueFloat( hVar );
	}
	else
		fReverbLevel = 1.0f;

	// Check if reverb was off and is still off
	if( fReverbLevel < 0.001f && m_fReverbLevel < 0.001f )
		return;

	m_fReverbLevel = fReverbLevel;

	// Check if it's time yet
	if( g_pClientDE->GetTime( ) < m_fNextSoundReverbTime )
		return;

	// Update timer
	m_fNextSoundReverbTime = g_pClientDE->GetTime( ) + SOUND_REVERB_UPDATE_PERIOD;

	HOBJECT hFilterList[] = {hPlayerObj, m_MoveMgr->GetObject(), DNULL};

	memset( &query, 0, sizeof( query ));
	query.m_Flags = INTERSECT_OBJECTS | IGNORE_NONSOLID;
	query.m_FilterFn = ObjListFilterFn;
	query.m_pUserData = hFilterList;

	g_pClientDE->GetObjectPos( hPlayerObj, &query.m_From );
	g_pClientDE->Physics( )->GetObjectDims( hPlayerObj, &vPlayerDims );

	// Player must move at least 2x his dims mag before reverb is checked again.
	if( VEC_DISTSQR( query.m_From, m_vLastReverbPos ) < 4 * VEC_MAGSQR( vPlayerDims ))
		return;

	VEC_COPY( m_vLastReverbPos, query.m_From );

	VEC_SET( vSegs[0], query.m_From.x + 2000.0f,	query.m_From.y,				query.m_From.z );
	VEC_SET( vSegs[1], query.m_From.x - 2000.0f,	query.m_From.y,				query.m_From.z );
	VEC_SET( vSegs[2], query.m_From.x,				query.m_From.y + 2000.0f,	query.m_From.z );
	VEC_SET( vSegs[3], query.m_From.x,				query.m_From.y - 2000.0f,	query.m_From.z );
	VEC_SET( vSegs[4], query.m_From.x,				query.m_From.y,				query.m_From.z + 2000.0f );
	VEC_SET( vSegs[5], query.m_From.x,				query.m_From.y,				query.m_From.z - 2000.0f );

	bOpen = DFALSE;
	for( i = 0; i < 6; i++ )
	{
		VEC_COPY( query.m_To, vSegs[i] );

		if( g_pClientDE->IntersectSegment( &query, &info ))
		{
			VEC_COPY( vPos[i], info.m_Point );
			if( info.m_SurfaceFlags == ST_AIR || info.m_SurfaceFlags == ST_SKY )
			{
				bOpen = DTRUE;
			}
		}
		else
		{
			VEC_COPY( vPos[i], vSegs[i] );
			bOpen = DTRUE;
		}
	}

	fVolume = VEC_DIST( vPos[0], vPos[1] );
	fVolume *= VEC_DIST( vPos[2], vPos[3] );
	fVolume *= VEC_DIST( vPos[4], vPos[5] );

	// Use room types that are not completely enclosed rooms
	if( bOpen )
	{
		if( fVolume < 100.0f*100.0f*100.0f )
		{
			reverbProperties.m_dwAcoustics = REVERB_ACOUSTICS_SEWERPIPE;
			reverbProperties.m_fReflectTime		= 0.005f;
			reverbProperties.m_fDecayTime		= 1.493f;
			reverbProperties.m_fVolume			= 0.1f * m_fReverbLevel;
		}
		else if( fVolume < 500.0f*500.0f*500.0f )
		{
			reverbProperties.m_dwAcoustics = REVERB_ACOUSTICS_PLAIN;
			reverbProperties.m_fReflectTime		= 0.005f;
			reverbProperties.m_fDecayTime		= 1.493f;
			reverbProperties.m_fVolume			= 0.2f * m_fReverbLevel;
		}
		else if( fVolume < 1000.0f*1000.0f*1000.0f )
		{
			reverbProperties.m_dwAcoustics = REVERB_ACOUSTICS_ARENA;
			reverbProperties.m_fReflectTime		= 0.01f;
			reverbProperties.m_fDecayTime		= 4.236f;
			reverbProperties.m_fVolume			= 0.1f * m_fReverbLevel;
		}
		else
		{
			reverbProperties.m_dwAcoustics = REVERB_ACOUSTICS_MOUNTAINS;
			reverbProperties.m_fReflectTime		= 0.0f;
			reverbProperties.m_fDecayTime		= 3.0f;
			reverbProperties.m_fVolume			= 0.1f * m_fReverbLevel;
		}
	} 
	// Use room types that are enclosed rooms
	else
	{
		if( fVolume < 100.0f*100.0f*100.0f )
		{
			reverbProperties.m_dwAcoustics = REVERB_ACOUSTICS_STONEROOM;
			reverbProperties.m_fReflectTime		= 0.005f;
			reverbProperties.m_fDecayTime		= 0.6f;
			reverbProperties.m_fVolume			= 0.3f * m_fReverbLevel;
		}
		else if( fVolume < 500.0f*500.0f*500.0f )
		{
			reverbProperties.m_dwAcoustics = REVERB_ACOUSTICS_HALLWAY;
			reverbProperties.m_fReflectTime		= 0.01f;
			reverbProperties.m_fDecayTime		= 1.3f;
			reverbProperties.m_fVolume			= 0.1f * m_fReverbLevel;
		}
		else if( fVolume < 1500.0f*1500.0f*1500.0f )
		{
			reverbProperties.m_dwAcoustics = REVERB_ACOUSTICS_CONCERTHALL;
			reverbProperties.m_fReflectTime		= 0.02f;
			reverbProperties.m_fDecayTime		= 2.2f;
			reverbProperties.m_fVolume			= 0.1f * m_fReverbLevel;
		}
		else
		{
			reverbProperties.m_dwAcoustics = REVERB_ACOUSTICS_AUDITORIUM;
			reverbProperties.m_fReflectTime		= 0.02f;
			reverbProperties.m_fDecayTime		= 3.5f;
			reverbProperties.m_fVolume			= 0.1f * m_fReverbLevel;
		}
	}

	// Override to water if in it.
	if( IsLiquid( m_eCurContainerCode ))
		reverbProperties.m_dwAcoustics = REVERB_ACOUSTICS_UNDERWATER;

	reverbProperties.m_dwParams = REVERBPARAM_VOLUME | REVERBPARAM_ACOUSTICS | REVERBPARAM_REFLECTTIME | REVERBPARAM_DECAYTIME;
	// If in mech, then lengthen the decaytime
	if( !((m_nPlayerMode == PM_MODE_FOOT) || (m_nPlayerMode == PM_MODE_KID)))
		reverbProperties.m_fDecayTime *= 2;
	g_pClientDE->SetReverbProperties( &reverbProperties );

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateCamera()
//
//	PURPOSE:	Update the camera position/rotation
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateCamera()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;


	// Update the camera's position and rotation..

	UpdateAlternativeCamera();


	// Update the player camera...

	if (!UpdatePlayerCamera()) return;


	// This is sort of a kludge.  Basically if this is the first update(s), make
	// sure the camera is positioned correctly before actually rendering the
	// scene.
	
	if (!m_bHandledStartup)
	{
		ShowPlayer(DFALSE);
	}


	if (!m_bUsingExternalCamera)
	{
		UpdateCameraPosition();
	}

	CalculateCameraRotation();
	UpdateCameraRotation();


	// If we're dead or dying, stop zooming...

	if (m_bUsingExternalCamera || IsPlayerDead())
	{
		m_bZoomView = DFALSE;
	}

	// Update zoom if applicable...

	if (m_bZooming || m_bZoomView != m_bOldZoomView)
	{
		UpdateCameraZoom();
	}

	m_bOldZoomView = m_bZoomView;


	// Update shake if applicable...

	UpdateCameraShake();



	
	// Make sure the player gets updated

	UpdatePlayerInfo();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateMoviesState()
//
//	PURPOSE:	Update movies
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateMoviesState()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// If we're playing movies, see if the current one is finished...
	
	pClientDE->UpdateVideo();
	
	if (pClientDE->IsVideoPlaying() != VIDEO_PLAYING)
	{
		PlayIntroMovies (pClientDE);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateCreditsState()
//
//	PURPOSE:	Update credits
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateCreditsState()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	int nMode = CM_CREDITS;

#ifdef _DEMO
	nMode = CM_DEMO_INFO;
#endif

	if (!m_credits.IsInited() && !m_credits.Init (pClientDE, this, nMode, DFALSE))
	{
		m_nGameState = GS_MENU;
		SetMenuMusic(DTRUE);
	}
	else
	{
		pClientDE->Start3D();
		UpdateMenuPolygrid();
		m_credits.Update();
		pClientDE->End3D();
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateGameOver()
//
//	PURPOSE:	Update game over
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateGameOver()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;	

	int nMode = CM_CREDITS;
#ifdef _DEMO
	nMode = CM_DEMO_INFO;
#endif

	if (!m_credits.IsInited() && !m_credits.Init(pClientDE, this, nMode, DFALSE))
	{
		return;
	}
	else
	{
		m_credits.Update();
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateIntroState()
//
//	PURPOSE:	Update intro
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateIntroState()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	int nMode = CM_INTRO;
#ifdef _DEMO
	nMode = CM_DEMO_INTRO;
#endif

	if (!m_credits.IsInited() && !m_credits.Init(pClientDE, this, nMode, DFALSE))
	{
		m_nGameState = GS_MENU;
		SetMenuMusic(DTRUE);
	}
	else
	{
		pClientDE->Start3D();
		UpdateMenuPolygrid();
		m_credits.Update();
		pClientDE->End3D();
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateMenuState()
//
//	PURPOSE:	Update menu state
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateMenuState()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	pClientDE->Start3D();

	UpdateMenuPolygrid();

	pClientDE->StartOptimized2D();
	m_menu.Draw();
	pClientDE->EndOptimized2D();

	pClientDE->End3D();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateBumperState()
//
//	PURPOSE:	Update bumper screen state
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateBumperState()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// Remove the loading text and play a sound so that they know they can
	// start the level...

	if (m_bLoadingWorld)
	{
		pClientDE->ClearInput();

		if (m_pPressAnyKeySound)
		{
			PlaySoundLocal(m_pPressAnyKeySound, SOUNDPRIORITY_PLAYER_HIGH);
		}
	}

	// World is done loading if we got called...

	m_bLoadingWorld = DFALSE;

	pClientDE->Start3D();

	UpdateMenuPolygrid();

	pClientDE->StartOptimized2D();

	UpdateLoadingLevel();

	HSURFACE hScreen = pClientDE->GetScreenSurface();
	DDWORD nWidth, nHeight;
	pClientDE->GetSurfaceDims (hScreen, &nWidth, &nHeight);
	pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hPressAnyKey, DNULL, ((int)nWidth - (int)m_cxPressAnyKey) / 2, (int)nHeight - (int)m_cyPressAnyKey, DNULL);

	pClientDE->EndOptimized2D();
	pClientDE->End3D();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateLoadingLevelState()
//
//	PURPOSE:	Update loading level state
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateLoadingLevelState()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	pClientDE->Start3D();

	UpdateMenuPolygrid();

	pClientDE->StartOptimized2D();

	UpdateLoadingLevel();

	pClientDE->EndOptimized2D();
	pClientDE->End3D();

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	if (m_bInWorld && hPlayerObj)
	{
		m_nGameState = GS_PLAYING;
		PauseGame(DFALSE);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdatePausedState()
//
//	PURPOSE:	Update paused state
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdatePausedState()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	pClientDE->Start3D();

	UpdateMenuPolygrid();

	pClientDE->StartOptimized2D();
	if (m_hGamePausedSurface)
	{
		DDWORD nSurfaceWidth = 0, nSurfaceHeight = 0;
		DDWORD nScreenWidth, nScreenHeight;

		HSURFACE hScreen = pClientDE->GetScreenSurface();
		pClientDE->GetSurfaceDims(hScreen, &nScreenWidth, &nScreenHeight);
		pClientDE->GetSurfaceDims(m_hGamePausedSurface, &nSurfaceWidth, &nSurfaceHeight);

		pClientDE->DrawSurfaceToSurfaceTransparent(hScreen, m_hGamePausedSurface, DNULL, 
												   ((int)(nScreenWidth - nSurfaceWidth)) / 2, 
												   (((int)(nScreenHeight - nSurfaceHeight)) / 2) + 70, DNULL);
	}
	pClientDE->EndOptimized2D();

	pClientDE->End3D();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateDemoMultiplayerState()
//
//	PURPOSE:	Update demo multiplayer state
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateDemoMultiplayerState()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if (!m_credits.IsInited() && !m_credits.Init (pClientDE, this, CM_DEMO_MULTI, DFALSE))
	{
		m_nGameState = GS_MENU;
		SetMenuMusic(DTRUE);
	}
	else
	{
		pClientDE->Start3D();
		UpdateMenuPolygrid();
		m_credits.Update();
		pClientDE->End3D();
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdatePlayerInfo()
//
//	PURPOSE:	Tell the player about the new camera stuff
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdatePlayerInfo()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	float sendRate;

	if (m_bAllowPlayerMovement != m_bLastAllowPlayerMovement)
	{
		SetInputState(m_bAllowPlayerMovement);
	}

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	if (!hPlayerObj) return;
	
	// Send the rotation and the current weapon flash/model position to the 
	// server to update the player.  The flash and model position are 
	// relative to the player position...

	DVector vFlashPos, vModelPos, vPlayerPos, vCameraPos;
	VEC_INIT(vFlashPos);
	VEC_INIT(vModelPos);
	VEC_INIT(vCameraPos);

	pClientDE->GetObjectPos(hPlayerObj, &vPlayerPos);

	VEC_SUB(vFlashPos, m_weaponModel.GetFlashPos(), vPlayerPos);
	VEC_SUB(vModelPos, m_weaponModel.GetModelPos(), vPlayerPos);


	DRotation rPlayerRot;
	DBYTE nCode	= (DBYTE)m_eCurContainerCode;

	m_nPlayerInfoChangeFlags |= CLIENTUPDATE_PLAYERROT;
	m_nPlayerInfoChangeFlags |= CLIENTUPDATE_WEAPONROT;

	// Set the player's rotation (don't allow model to rotate up/down).

	pClientDE->SetupEuler(&rPlayerRot, 0.0f, m_fYaw, m_fCamCant);

	if ( m_playerCamera.IsChaseView() != m_bLastSent3rdPerson )
	{
		m_nPlayerInfoChangeFlags |= CLIENTUPDATE_3RDPERSON;
		m_bLastSent3rdPerson = m_playerCamera.IsChaseView();
		if ( m_playerCamera.IsChaseView() )
			m_nPlayerInfoChangeFlags |= CLIENTUPDATE_3RDPERVAL;
	}
	if ( !m_bHandledStartup || m_bAllowPlayerMovement != m_bLastAllowPlayerMovement )
	{
		m_nPlayerInfoChangeFlags |= CLIENTUPDATE_ALLOWINPUT;
	}

	if (m_bUsingExternalCamera)
	{
		m_nPlayerInfoChangeFlags |= CLIENTUPDATE_EXTERNALCAMERA;
		pClientDE->GetObjectPos(m_hCamera, &vCameraPos);
	}

	if ( m_nPlayerInfoChangeFlags )
	{
		// Always send CLIENTUPDATE_ALLOWINPUT changes guaranteed.
		if(m_nPlayerInfoChangeFlags & CLIENTUPDATE_ALLOWINPUT)
		{
			HMESSAGEWRITE hMessage = pClientDE->StartMessage(MID_PLAYER_UPDATE);
			pClientDE->WriteToMessageWord(hMessage, CLIENTUPDATE_ALLOWINPUT);
			pClientDE->WriteToMessageByte(hMessage, (DBYTE)m_bAllowPlayerMovement);
			pClientDE->EndMessage(hMessage);
			m_nPlayerInfoChangeFlags &= ~CLIENTUPDATE_ALLOWINPUT;
		}
		
		sendRate = 1.0f / g_CV_CSendRate.GetFloat(DEFAULT_CSENDRATE);

		if(!IsMultiplayerGame() || 
			(pClientDE->GetTime() - m_fPlayerInfoLastSendTime) > sendRate)
		{
			HMESSAGEWRITE hMessage = pClientDE->StartMessage(MID_PLAYER_UPDATE);

			// Write rotation info.			
			pClientDE->WriteToMessageWord(hMessage, m_nPlayerInfoChangeFlags);
			if ( m_nPlayerInfoChangeFlags & CLIENTUPDATE_PLAYERROT )
			{
				//pClientDE->WriteToMessageRotation(hMessage, &rPlayerRot);
				pClientDE->WriteToMessageByte(hMessage, CompressRotationByte(pClientDE->Common(), &rPlayerRot));
			}
			
			if ( m_nPlayerInfoChangeFlags & CLIENTUPDATE_WEAPONROT )
				pClientDE->WriteToMessageRotation(hMessage, &m_rRotation);
			
			if ( m_nPlayerInfoChangeFlags & CLIENTUPDATE_EXTERNALCAMERA )
				pClientDE->WriteToMessageVector(hMessage, &vCameraPos);
			

			// Write position info.
			m_MoveMgr->WritePositionInfo(hMessage);

			pClientDE->EndMessage2(hMessage, 0); // Send unguaranteed.
			
			m_fLastSentYaw	= m_fYaw;
			m_fLastSentCamCant = m_fCamCant;
			m_fPlayerInfoLastSendTime = pClientDE->GetTime();
			m_nPlayerInfoChangeFlags = 0;
		}
	}

	m_bHandledStartup = DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::StartLevel()
//
//	PURPOSE:	Tell the player to start the level
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::StartLevel()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || IsMultiplayerGame()) return;

	HMESSAGEWRITE hMessage = pClientDE->StartMessage(MID_SINGLEPLAYER_START);
	pClientDE->EndMessage(hMessage);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdatePlayerCamera()
//
//	PURPOSE:	Update the player camera
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::UpdatePlayerCamera()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return DFALSE;

	// Make sure our player camera is attached...

	m_playerCamera.AttachToObject(m_MoveMgr->GetObject());


	if (m_playerCamera.IsChaseView())
	{
		Update3rdPersonInfo();
	}

	
	// Update our camera position based on the player camera...

	m_playerCamera.CameraUpdate(pClientDE->GetFrameTime());


	return DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::SetExternalCamera()
//
//	PURPOSE:	Turn on/off external camera mode
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::SetExternalCamera(DBOOL bExternal)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;
	
	if (bExternal && m_playerCamera.IsFirstPerson())
	{
		m_weaponModel.SetVisible(DFALSE);

		ShowPlayer(DTRUE);
		m_playerCamera.GoChaseMode();
		m_playerCamera.CameraUpdate(0.0f);

		m_bZoomView = DFALSE;			 // Can't zoom in 3rd person...
		m_stats.EnableCrosshair(DFALSE); // Disable cross hair in 3rd person...
	}
	else if (!bExternal && !m_playerCamera.IsFirstPerson()) // Go Internal
	{
		if (!pSettings->VehicleMode())
		{
			m_weaponModel.SetVisible(DTRUE);
			ShowPlayer(DFALSE);
		}

		m_playerCamera.GoFirstPerson();
		m_playerCamera.CameraUpdate(0.0f);

		if (m_nPlayerMode != PM_MODE_KID)
		{
			m_stats.EnableCrosshair(DTRUE); // Enable cross hair in 1st person...
		}

		if (m_h3rdPersonCrosshair)
		{
			pClientDE->DeleteObject(m_h3rdPersonCrosshair);
			m_h3rdPersonCrosshair = DNULL;
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateAlternativeCamera()
//
//	PURPOSE:	Update the camera using an alternative camera
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::UpdateAlternativeCamera()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return DFALSE;

	m_bLastAllowPlayerMovement = m_bAllowPlayerMovement;
	m_bAllowPlayerMovement	   = DTRUE;

	HOBJECT hObj = DNULL;

	// See if we should use an alternative camera position...

	CSpecialFXList* pCameraList = m_sfxMgr.GetCameraList();
	if (pCameraList)
	{
		int nNum = pCameraList->GetSize();

		for (int i=0; i < nNum; i++)
		{
			CCameraFX* pCamFX = (CCameraFX*)(*pCameraList)[i];
			if (!pCamFX) continue;

			hObj = pCamFX->GetServerObj();

			if (hObj)
			{
				DDWORD dwUsrFlags;
				pClientDE->GetObjectUserFlags(hObj, &dwUsrFlags);
				
				if (dwUsrFlags & USRFLG_CAMERA_LIVE)
				{
					m_bUsingExternalCamera = DTRUE;
					m_bDrawInterface	   = DFALSE;

					SetExternalCamera(DTRUE);

					m_bAllowPlayerMovement = pCamFX->AllowPlayerMovement();

					DVector vPos;
					pClientDE->GetObjectPos(hObj, &vPos);
					pClientDE->SetObjectPos(m_hCamera, &vPos);
					m_bCameraPosInited = DTRUE;

					DRotation rRot;
					pClientDE->GetObjectRotation(hObj, &rRot);
					pClientDE->SetObjectRotation(m_hCamera, &rRot);	
					
					if (pCamFX->IsListener())
					{
						pClientDE->SetListener(DFALSE, &vPos, &rRot);
					}

					// Set to movie camera rect, if not already set...

					if (!m_bMovieCameraRect)
					{
						// Make sure we clear whatever was on the screen before
						// we switch to this camera...

						ClearAllScreenBuffers();

						m_bMovieCameraRect = DTRUE;

						DDWORD dwWidth = 640, dwHeight = 480;
						pClientDE->GetSurfaceDims(pClientDE->GetScreenSurface(), &dwWidth, &dwHeight);

						DBOOL bFullScreen;
						pClientDE->GetCameraRect(m_hCamera, &bFullScreen, &m_nOldCameraLeft, &m_nOldCameraTop, 
												 &m_nOldCameraRight, &m_nOldCameraBottom);

						// Determine how to adjust camera rect/fov...

						DBYTE nCamType = pCamFX->GetType();
						DFLOAT fVal1 = (nCamType == CT_CINEMATIC) ? 1.0f/6.0f : 0.0f;
						DFLOAT fVal2 = (nCamType == CT_CINEMATIC) ? 2.0f/3.0f : 1.0f;

						int nOffset = int(dwHeight * fVal1);
						int nBottom = dwHeight - nOffset;
						
						// {MD 9/11/98} Force fullscreen to false otherwise the movie
						// won't be letterbox.
						bFullScreen = DFALSE;
						
						pClientDE->SetCameraRect(m_hCamera, bFullScreen, 0, nOffset, dwWidth, nBottom);

						DFLOAT y, x;
						pClientDE->GetCameraFOV(m_hCamera, &x, &y);
						y = (x * dwHeight * fVal2) / dwWidth;

						pClientDE->SetCameraFOV(m_hCamera, x, y);
					}

					return DTRUE;
				}
			}
		}
	}


	// Okay, we're no longer using an external camera...

	if (m_bUsingExternalCamera)
	{
		TurnOffAlternativeCamera();
	}


	return DFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::TurnOffAlternativeCamera()
//
//	PURPOSE:	Turn off the alternative camera mode
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::TurnOffAlternativeCamera()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	m_bUsingExternalCamera = DFALSE;
	m_bDrawInterface	   = DTRUE;

	// Set the listener back to the client...

	pClientDE->SetListener(DTRUE, DNULL, DNULL);
	
	
	// Force 1st person...

	SetExternalCamera(DFALSE); 


	// Set the camera back like it was...

	pClientDE->SetCameraRect(m_hCamera, DFALSE, m_nOldCameraLeft, m_nOldCameraTop, 
							 m_nOldCameraRight, m_nOldCameraBottom);
	m_bMovieCameraRect = DFALSE;

	DDWORD dwHeight = m_nOldCameraBottom - m_nOldCameraTop;
	DDWORD dwWidth  = m_nOldCameraRight - m_nOldCameraLeft;
	DFLOAT y, x;

	pClientDE->GetCameraFOV(m_hCamera, &x, &y);
	y = (x * dwHeight) / dwWidth;

	pClientDE->SetCameraFOV(m_hCamera, x, y);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateCameraPosition()
//
//	PURPOSE:	Update the camera position
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateCameraPosition()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	DVector vPos;
	VEC_COPY(vPos, m_playerCamera.GetPos());

	if (m_playerCamera.IsFirstPerson())
	{
		vPos.y += m_fBobHeight + m_fCamDuck;

		DVector vU, vR, vF;
		pClientDE->GetRotationVectors(&m_rRotation, &vU, &vR, &vF);

		VEC_MULSCALAR(vR, vR, m_fBobWidth)
		VEC_ADD(vPos, vPos, vR)
	}
	else
	{
		ROT_COPY(m_rRotation, m_playerCamera.GetRotation());
	}

	pClientDE->SetObjectPos(m_hCamera, &vPos);
	m_bCameraPosInited = DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::CalculateCameraRotation()
//
//	PURPOSE:	Calculate the new camera rotation
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::CalculateCameraRotation()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;

	DFLOAT fVal = m_bZoomView ? 10.0f : 1.0f;

	// Get axis offsets...

	float offsets[3];
	pClientDE->GetAxisOffsets(offsets);

	// update offsets with values from console variables

	HCONSOLEVAR hVar;
	float fAxisForwardBackward = 0.0f;
	float fAxisLeftRight = 0.0f;
	float fAxisYaw = 0.0f;
	float fAxisPitch = 0.0f;
	DBOOL bUseAxisForwardBackward = FALSE;
	DBOOL bUseAxisLeftRight = FALSE;
	DBOOL bFixedAxisPitch = FALSE;
	float fAxisYawDeadZone = 0.10f;
	float fAxisPitchDeadZone = 0.10f;
	float fAxisForwardBackwardDeadZone = 0.10f;
	float fAxisLeftRightDeadZone = 0.10f;

	if (IsJoystickEnabled())
	{
		hVar = pClientDE->GetConsoleVar( "AxisYawDeadZone");
		if (hVar != NULL) fAxisYawDeadZone = pClientDE->GetVarValueFloat(hVar);
		hVar = pClientDE->GetConsoleVar( "AxisPitchDeadZone");
		if (hVar != NULL) fAxisPitchDeadZone = pClientDE->GetVarValueFloat(hVar);
		hVar = pClientDE->GetConsoleVar( "AxisForwardBackwardDeadZone");
		if (hVar != NULL) fAxisForwardBackwardDeadZone = pClientDE->GetVarValueFloat(hVar);
		hVar = pClientDE->GetConsoleVar( "AxisLeftRightDeadZone");
		if (hVar != NULL) fAxisLeftRightDeadZone = pClientDE->GetVarValueFloat(hVar);

		hVar = pClientDE->GetConsoleVar( "AxisYaw");
		if (hVar != NULL)
		{
			fAxisYaw = pClientDE->GetVarValueFloat(hVar);
			if ((fAxisYaw > fAxisYawDeadZone) || ((fAxisYaw < -fAxisYawDeadZone))) offsets[0] += fAxisYaw;
		}
		hVar = pClientDE->GetConsoleVar( "AxisPitch");
		if (hVar != NULL) 
		{
			fAxisPitch = pClientDE->GetVarValueFloat(hVar);
			if ((fAxisPitch > fAxisPitchDeadZone) || (fAxisPitch < -fAxisPitchDeadZone)) offsets[1] += fAxisPitch;
		}
		hVar = pClientDE->GetConsoleVar( "AxisLeftRight");
		if (hVar != NULL) 
		{
			fAxisLeftRight = pClientDE->GetVarValueFloat(hVar);
			bUseAxisLeftRight = TRUE;
		}
		hVar = pClientDE->GetConsoleVar( "AxisForwardBackward");
		if (hVar != NULL)
		{
			fAxisForwardBackward = pClientDE->GetVarValueFloat(hVar);
			bUseAxisForwardBackward = TRUE;
		}

		// force pitch axis if we are using a joystick and the FixedAxisPitch variable is 1
		hVar = pClientDE->GetConsoleVar( "FixedAxisPitch");
		if (hVar != NULL) if (pClientDE->GetVarValueFloat(hVar) == 1) 
		{
			m_fPitch = fAxisPitch;
			bFixedAxisPitch = TRUE;
		}
	}

	m_MoveMgr->UpdateAxisMovement(bUseAxisForwardBackward, fAxisForwardBackward, fAxisForwardBackwardDeadZone, bUseAxisLeftRight, fAxisLeftRight, fAxisLeftRightDeadZone);


	if (m_bRestoreOrientation)
	{
		m_fYaw   = m_fYawBackup;
		m_fPitch = m_fPitchBackup;
		memset (offsets, 0, sizeof(float) * 3);

		m_bRestoreOrientation = DFALSE;
	}

	if (m_bStrafing)
	{
		m_MoveMgr->UpdateMouseStrafeFlags(offsets);
	}

	m_fYaw += offsets[0] / fVal;

	// get the turning speed
	
	DFLOAT nNormalTurnSpeed = DEFAULT_NORMAL_TURN_SPEED;
	DFLOAT nFastTurnSpeed = DEFAULT_FAST_TURN_SPEED;
	hVar = pClientDE->GetConsoleVar (NORMAL_TURN_RATE_VAR);
	if (hVar)
	{
		nNormalTurnSpeed = pClientDE->GetVarValueFloat (hVar);
	}
	hVar = pClientDE->GetConsoleVar (FAST_TURN_RATE_VAR);
	if (hVar)
	{
		nFastTurnSpeed = pClientDE->GetVarValueFloat (hVar);
	}

	if (!(m_dwPlayerFlags & CS_MFLG_STRAFE) && (m_dwPlayerFlags & CS_MFLG_LEFT))
	{
		m_fYaw -= pClientDE->GetFrameTime() * ((m_dwPlayerFlags & CS_MFLG_RUN) ? nFastTurnSpeed : nNormalTurnSpeed);
	}

	if (!(m_dwPlayerFlags & CS_MFLG_STRAFE) && (m_dwPlayerFlags & CS_MFLG_RIGHT))
	{
		m_fYaw += pClientDE->GetFrameTime() * ((m_dwPlayerFlags & CS_MFLG_RUN) ? nFastTurnSpeed : nNormalTurnSpeed);
	}

	
	if (pSettings->MouseLook() || (m_dwPlayerFlags & CS_MFLG_LOOKUP) || (m_dwPlayerFlags & CS_MFLG_LOOKDOWN) || m_bHoldingMouseLook)
	{
		if (pSettings->MouseLook() || m_bHoldingMouseLook)
		{
			if (pSettings->MouseInvertY())
			{
				m_fPitch -= offsets[1] / fVal;
			}
			else
			{
				m_fPitch += offsets[1] / fVal;
			}
		}

		if (m_dwPlayerFlags & CS_MFLG_LOOKUP)
		{
			m_fPitch -= 0.075f;
		}

		if (m_dwPlayerFlags & CS_MFLG_LOOKDOWN)
		{
			m_fPitch += 0.075f;
		}

		// Don't allow much movement up/down if 3rd person...

		if (!m_playerCamera.IsFirstPerson())
		{
			DFLOAT fMinY = DEG2RAD(45.0f) - 0.1f;

			if (m_fPitch < -fMinY) m_fPitch = -fMinY;
			if (m_fPitch > fMinY)  m_fPitch = fMinY;
		}
	}
	else if (m_fPitch != 0.0f && pSettings->Lookspring())
	{
		if (m_fPitch > 0.0f) m_fPitch -= min (0.075f, m_fPitch);
		if (m_fPitch < 0.0f) m_fPitch += min (0.075f, -m_fPitch);
	}

	DFLOAT fMinY = MATH_HALFPI - 0.1f;

	if (m_fPitch < -fMinY) m_fPitch = -fMinY;
	if (m_fPitch > fMinY)  m_fPitch = fMinY;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateCameraRotation()
//
//	PURPOSE:	Set the new camera rotation
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::UpdateCameraRotation()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return DFALSE;

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	if (!hPlayerObj) return DFALSE;

	if (m_bUsingExternalCamera)
	{
		// Just calculate the correct player rotation...

		pClientDE->SetupEuler(&m_rRotation, m_fPitch, m_fYaw, m_fCamCant);
	}
	else if (m_playerCamera.IsFirstPerson())
	{
		pClientDE->SetupEuler(&m_rRotation, m_fPitch, m_fYaw, m_fCamCant);
		pClientDE->SetObjectRotation(m_hCamera, &m_rRotation);
	}
	else
	{
		// Set the camera to use the rotation calculated by the player camera,
		// however we still need to calculate the correct rotation to be sent
		// to the player...

		pClientDE->EulerRotateX(&m_rRotation, m_fPitch);
		pClientDE->SetObjectRotation(m_hCamera, &m_rRotation);

		// Okay, now calculate the correct player rotation...

		pClientDE->SetupEuler(&m_rRotation, m_fPitch, m_fYaw, m_fCamCant);
	}

	return DTRUE;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateCameraZoom
//
//	PURPOSE:	Update the camera's field of view
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UpdateCameraZoom()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	DFLOAT zoomSpeed = 4 * pClientDE->GetFrameTime();

	char strConsole[30];
	DFLOAT fovX, fovY, oldFovX;
	DDWORD dwWidth = 640, dwHeight = 480;
	pClientDE->GetSurfaceDims(pClientDE->GetScreenSurface(), &dwWidth, &dwHeight);
	pClientDE->GetCameraFOV(m_hCamera, &fovX, &fovY);

	oldFovX = fovX;

	if (!fovX)
	{
		fovX = m_fCurrentFovX;
	}

	m_bZooming = DTRUE;
	
	DBOOL bZoomingIn = DTRUE;

	// Need to zoom camera
	if (m_bZoomView && fovX > DEG2RAD(FOV_ZOOMED))
	{
		fovX -= zoomSpeed;
		
		if (fovX < DEG2RAD(FOV_ZOOMED)) 
		{
			fovX = DEG2RAD(FOV_ZOOMED);
			CSPrint ("Zoom mode on");
			m_bZooming = DFALSE;

			// Set the lod scale to max value

			sprintf(strConsole, "+LODScale %f", m_fSaveLODScale * LODSCALE_MULTIPLIER);
			pClientDE->RunConsoleString(strConsole);
		}
	}
	else if (!m_bZoomView && fovX < m_fCurrentFovX)
	{
		bZoomingIn = DFALSE;

		fovX += zoomSpeed;

		if (fovX > m_fCurrentFovX) 
		{
			fovX = m_fCurrentFovX;
			CSPrint ("Zoom mode off");
			m_bZooming = DFALSE;

			// Set the lod scale for models back to saved value...

			sprintf(strConsole, "+LODScale %f", m_fSaveLODScale);
			pClientDE->RunConsoleString(strConsole);
		}
	}

	if (oldFovX != fovX && dwWidth && dwHeight)
	{
		fovY = (fovX * dwHeight) / dwWidth;

		pClientDE->SetCameraFOV(m_hCamera, fovX, fovY);

		// Update the lod scale for models...

		DFLOAT fVal1 = m_fCurrentFovX - fovX;
		DFLOAT fVal2 = (m_fSaveLODScale * LODSCALE_MULTIPLIER) - m_fSaveLODScale;
		DFLOAT fVal3 = m_fCurrentFovX - DEG2RAD(FOV_ZOOMED);

		DFLOAT fNewLODScale = fVal3 > 0.0f ? (m_fSaveLODScale + (fVal1 * fVal2) / fVal3) : (m_fSaveLODScale * LODSCALE_MULTIPLIER);

		sprintf(strConsole, "+LODScale %f", fNewLODScale);
		pClientDE->RunConsoleString(strConsole);

		//pClientDE->CPrint("Current FOV (%f, %f)", fovX, fovY);
		//pClientDE->CPrint("Current Zoom LODScale: %f", fNewLODScale);
	}

}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateCameraShake
//
//	PURPOSE:	Update the camera's shake
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UpdateCameraShake()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	// Decay...

	DFLOAT fDecayAmount = 2.0f * pClientDE->GetFrameTime();

	m_vShakeAmount.x -= fDecayAmount;
	m_vShakeAmount.y -= fDecayAmount;
	m_vShakeAmount.z -= fDecayAmount;

	if (m_vShakeAmount.x < 0.0f) m_vShakeAmount.x = 0.0f;
	if (m_vShakeAmount.y < 0.0f) m_vShakeAmount.y = 0.0f;
	if (m_vShakeAmount.z < 0.0f) m_vShakeAmount.z = 0.0f;


	if (m_vShakeAmount.x <= 0.0f && m_vShakeAmount.y <= 0.0f && m_vShakeAmount.z <= 0.0f) return;


	// Apply...

	DFLOAT faddX = GetRandom(-1.0f, 1.0f) * m_vShakeAmount.x * 3.0f;
	DFLOAT faddY = GetRandom(-1.0f, 1.0f) * m_vShakeAmount.y * 3.0f;
	DFLOAT faddZ = GetRandom(-1.0f, 1.0f) * m_vShakeAmount.z * 3.0f;

	DVector vPos, vAdd;
	VEC_SET(vAdd, faddX, faddY, faddZ);

	pClientDE->GetObjectPos(m_hCamera, &vPos);
	VEC_ADD(vPos, vPos, vAdd);

	pClientDE->SetObjectPos(m_hCamera, &vPos);

	HLOCALOBJ hWeapon = m_weaponModel.GetHandle();
	if (!hWeapon) return;

	pClientDE->GetObjectPos(hWeapon, &vPos);

	VEC_MULSCALAR(vAdd, vAdd, 0.95f);
	VEC_ADD(vPos, vPos, vAdd);
	pClientDE->SetObjectPos(hWeapon, &vPos);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateScreenTint
//
//	PURPOSE:	Update the screen tint
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UpdateScreenTint()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera || !m_bTintScreen) return;

	DVector vLightAdd, vCurLightAdd;
	VEC_SET(vLightAdd, 0.0f, 0.0f, 0.0f);

	pClientDE->GetCameraLightAdd(m_hCamera, &vCurLightAdd);

	DFLOAT fTime  = pClientDE->GetTime();
	if ((m_fTintRampUp > 0.0f) && (fTime < m_fTintStart + m_fTintRampUp))
	{
		DFLOAT fDelta = (fTime - m_fTintStart);
		vLightAdd.x = fDelta * (m_vTintColor.x) / m_fTintRampUp;
		vLightAdd.y = fDelta * (m_vTintColor.y) / m_fTintRampUp;
		vLightAdd.z = fDelta * (m_vTintColor.z) / m_fTintRampUp;
	}
	else if (fTime < m_fTintStart + m_fTintRampUp + m_fTintTime)
	{
		VEC_COPY(vLightAdd, m_vTintColor);
	}
	else if ((m_fTintRampDown > 0.0f) && (fTime < m_fTintStart + m_fTintRampUp + m_fTintTime + m_fTintRampDown))
	{
		DFLOAT fDelta = (fTime - (m_fTintStart + m_fTintRampUp + m_fTintTime));

		vLightAdd.x = m_vTintColor.x - (fDelta * (m_vTintColor.x) / m_fTintRampUp);
		vLightAdd.y = m_vTintColor.y - (fDelta * (m_vTintColor.y) / m_fTintRampUp);
		vLightAdd.z = m_vTintColor.z - (fDelta * (m_vTintColor.z) / m_fTintRampUp);
	}
	else
	{
		m_bTintScreen = DFALSE;
	}

	// Make sure values are in range...

	vLightAdd.x = (vLightAdd.x < 0.0f ? 0.0f : (vLightAdd.x > 1.0f ? 1.0f : vLightAdd.x));
	vLightAdd.y = (vLightAdd.y < 0.0f ? 0.0f : (vLightAdd.y > 1.0f ? 1.0f : vLightAdd.y));
	vLightAdd.z = (vLightAdd.z < 0.0f ? 0.0f : (vLightAdd.z > 1.0f ? 1.0f : vLightAdd.z));

	pClientDE->SetCameraLightAdd(m_hCamera, &vLightAdd);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ClearScreenTint
//
//	PURPOSE:	Clear any tint on the screen
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::ClearScreenTint()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if (m_bTintScreen)
	{
		m_bTintScreen = DFALSE;
		DVector vLightAdd;
		VEC_INIT(vLightAdd);
		pClientDE->SetCameraLightAdd(m_hCamera, &vLightAdd);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateWeaponModel()
//
//	PURPOSE:	Update the weapon model
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateWeaponModel()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_MoveMgr) return;

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	if (!hPlayerObj) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings || pSettings->VehicleMode()) return;


	// Decay firing jitter if necessary...

	if (m_fFireJitterPitch > 0.0f)
	{
		DFLOAT fVal = (pClientDE->GetFrameTime() * FIRE_JITTER_DECAY_DELTA);

		if (m_fFireJitterPitch - fVal < 0.0f)
		{
			fVal = m_fFireJitterPitch;
		}
		
		m_fFireJitterPitch -= fVal;
		m_fPitch += fVal;
	}


	// If possible, get these values from the camera, because it 
	// is more up-to-date...
	
	HLOCALOBJ hObj = hPlayerObj; // m_MoveMgr->GetObject();
	if (m_playerCamera.IsFirstPerson() && !m_bUsingExternalCamera)
	{
		hObj = m_hCamera;
	}

	DRotation rRot;
	DVector vPos;
	pClientDE->GetObjectPos(hObj, &vPos);
	pClientDE->GetObjectRotation(m_hCamera, &rRot);


	// If we aren't dead, and we aren't in the middle of changing weapons,
	// let us fire.
	
	DBOOL bFire = DFALSE;

	if ((m_dwPlayerFlags & CS_MFLG_FIRING) && (m_ePlayerState != PS_DEAD) && !m_bSpectatorMode)
	{
		bFire = DTRUE;
	}


	// Babies don't get to use their weapon...

	DDWORD dwUsrFlags;
	pClientDE->GetObjectUserFlags(hPlayerObj, &dwUsrFlags);
	if (dwUsrFlags & USRFLG_PLAYER_TEARS)
	{
		bFire = DFALSE;
	}


	// Update the model position and state...

	WeaponState eWeaponState = m_weaponModel.UpdateWeaponModel(rRot, vPos, bFire);


	// Set the correct flags...

	HLOCALOBJ hWeaponModel = m_weaponModel.GetHandle();
	if (m_bHaveStealth && !pClientDE->IsCommandOn(COMMAND_ID_FIRING))
	{
		DFLOAT r, g, b, a;
		pClientDE->GetObjectColor(hWeaponModel, &r, &g, &b, &a);
		pClientDE->SetObjectColor(hWeaponModel, r, g, b, 0.6f);
	}
	else
	{
		DFLOAT r, g, b, a;
		pClientDE->GetObjectColor(hWeaponModel, &r, &g, &b, &a);
		pClientDE->SetObjectColor(hWeaponModel, r, g, b, 1.0f);
	}


	// Do fire camera jitter...

	if (FiredWeapon(eWeaponState) && m_playerCamera.IsFirstPerson())
	{
		DVector vShake;
		VEC_SET(vShake, 0.1f, 0.1f, 0.1f);
		ShakeScreen(vShake);

		// Move view up a bit...

		DFLOAT fVal = (DFLOAT)DEG2RAD(FIRE_JITTER_MAX_PITCH_DELTA) - m_fFireJitterPitch;
		m_fFireJitterPitch += fVal;
		m_fPitch -= fVal;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::Update3rdPersonCrossHair()
//
//	PURPOSE:	Update the 3rd person crosshair pos
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::Update3rdPersonCrossHair(DFLOAT fDistance)
{
	CClientDE* pClientDE = GetClientDE();
	if (!m_MoveMgr || !pClientDE || !m_playerCamera.IsChaseView()) return;

	//HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	HLOCALOBJ hPlayerObj = m_MoveMgr->GetObject();
	if (!hPlayerObj) return;

	if (!m_h3rdPersonCrosshair)
	{
		// Create the 3rd person crosshair sprite...

		ObjectCreateStruct theStruct;
		INIT_OBJECTCREATESTRUCT(theStruct);

		theStruct.m_ObjectType = OT_SPRITE;
		SAFE_STRCPY(theStruct.m_Filename, "Sprites\\Crosshair.spr");
		theStruct.m_Flags = FLAG_VISIBLE | FLAG_GLOWSPRITE | FLAG_NOLIGHT;  
		m_h3rdPersonCrosshair = pClientDE->CreateObject(&theStruct);

		DVector vScale;
		VEC_SET(vScale, .5f, .5f, 1.0f);
		pClientDE->SetObjectScale(m_h3rdPersonCrosshair, &vScale);		
	}

	if (fDistance < 1.0f || m_bUsingExternalCamera || !m_bCrosshairOn)
	{
		DDWORD dwFlags = pClientDE->GetObjectFlags(m_h3rdPersonCrosshair);
		pClientDE->SetObjectFlags(m_h3rdPersonCrosshair, dwFlags & ~FLAG_VISIBLE);
		return;
	}
	else
	{
		DDWORD dwFlags = pClientDE->GetObjectFlags(m_h3rdPersonCrosshair);
		pClientDE->SetObjectFlags(m_h3rdPersonCrosshair, dwFlags | FLAG_VISIBLE);
	}

	DVector vU, vR, vF;
	pClientDE->GetRotationVectors(&m_rRotation, &vU, &vR, &vF);
	VEC_NORM(vF);

	DVector vPos;
	pClientDE->GetObjectPos(hPlayerObj, &vPos);

	DFLOAT fDist = fDistance > 100.0f ? fDistance - 20.0f : fDistance;

	DVector vTemp;
	VEC_MULSCALAR(vTemp, vF, fDist);
	VEC_ADD(vPos, vPos, vTemp);

	// Set the 3rd person crosshair to the correct position...

	pClientDE->SetObjectPos(m_h3rdPersonCrosshair, &vPos);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateContainerFX
//
//	PURPOSE:	Update any client side container fx
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UpdateContainerFX()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	DVector vPos;
	pClientDE->GetObjectPos(m_hCamera, &vPos);

	HLOCALOBJ objList[1];
	DDWORD dwNum = pClientDE->GetPointContainers(&vPos, objList, 1);

	DVector vScale, vLightAdd;
	VEC_SET(vScale, 1.0f, 1.0f, 1.0f);
	VEC_SET(vLightAdd, 0.0f, 0.0f, 0.0f);
	DBOOL bClearCurrentLightScale = DFALSE;

	char* pCurSound      = DNULL;
	ContainerCode eCode  = CC_NONE;
	DDWORD dwUserFlags	 = FLAG_VISIBLE;
	m_bUseWorldFog = DTRUE;

	// Get the user flags associated with the container, and make sure that
	// the container isn't hidden...

	if (dwNum > 0 && objList[0])
	{
		pClientDE->GetObjectUserFlags(objList[0], &dwUserFlags);
	}


	if (dwNum > 0 && (dwUserFlags & USRFLG_VISIBLE))
	{
		D_WORD code;
		if (pClientDE->GetContainerCode(objList[0], &code))
		{
			eCode = (ContainerCode)code;

			// See if we have entered/left a container...

			DFLOAT fTime = pClientDE->GetTime();

			if (m_eCurContainerCode != eCode)
			{
				m_fContainerStartTime = fTime;

				// See if this container has fog associated with it..

				CVolumeBrushFX* pFX = (CVolumeBrushFX*)m_sfxMgr.FindSpecialFX(SFX_VOLUMEBRUSH_ID, objList[0]);
				if (pFX)
				{
					DBOOL bFog = pFX->IsFogEnable();

					if (bFog)
					{
						m_bUseWorldFog = DFALSE;
	
						char buf[30];
						sprintf(buf, "FogEnable %d", (int)bFog);
						pClientDE->RunConsoleString(buf);

						sprintf(buf, "FogNearZ %d", (int)pFX->GetFogNearZ());
						pClientDE->RunConsoleString(buf);

						sprintf(buf, "FogFarZ %d", (int)pFX->GetFogFarZ());
						pClientDE->RunConsoleString(buf);

						DVector vFogColor = pFX->GetFogColor();

						sprintf(buf, "FogR %d", (int)vFogColor.x);
						pClientDE->RunConsoleString(buf);

						sprintf(buf, "FogG %d", (int)vFogColor.y);
						pClientDE->RunConsoleString(buf);

						sprintf(buf, "FogB %d", (int)vFogColor.z);
						pClientDE->RunConsoleString(buf);
					}
				}				
			}

			
			switch (eCode)
			{
				case CC_BLUE_WATER:
				{
					VEC_SET(vScale, 0.0f, 0.5f, 0.7f);
					pCurSound = "Sounds\\Player\\unwater2.wav";
				}
				break;
				case CC_DIRTY_WATER:
				{
					VEC_SET(vScale, 0.25f, 0.3f, 0.05f);
					pCurSound = "Sounds\\Player\\unwater2.wav";
				}
				break;
				case CC_CLEAR_WATER:
				{
					VEC_SET(vScale, 0.3f, 0.8f, 0.8f);
					pCurSound = "Sounds\\Player\\unwater2.wav";
				}
				break;
				case CC_CORROSIVE_FLUID:
				{
					VEC_SET(vScale, 0.3f, 0.4f, 0.0f);
					pCurSound = "Sounds\\Player\\unwater2.wav";
				}
				break;
				case CC_KATO:
				{
					VEC_SET(vScale, 0.5f, 0.15f, 0.0f);
					pCurSound = "Sounds\\Player\\unwater2.wav";
				}
				break;
				case CC_LIQUID_NITROGEN:
				{
					VEC_SET(vScale, 0.0f, 0.5f, 0.7f);
					VEC_SET(vLightAdd, .1f, .1f, .1f);
				}
				break;
				case CC_POISON_GAS:
				{
					VEC_SET(vScale, 1.0f, 1.0f, 0.3f);
				}
				break;
				case CC_SMOKE:
				{
					VEC_SET(vScale, 0.2f, 0.2f, 0.2f);
				}
				break;
				case CC_ELECTRICITY:
				{
					VEC_SET(vLightAdd, .1f, .1f, .1f);
				}
				break;
				case CC_ENDLESS_FALL:
				{
					DFLOAT fFallTime = 1.0f;

					if (fTime > m_fContainerStartTime + fFallTime)
					{
						VEC_SET(vScale, 0.0f, 0.0f, 0.0f);
					}
					else
					{
						DFLOAT fScaleStart = .3f;
						DFLOAT fTimeLeft = (m_fContainerStartTime + fFallTime) - fTime;
						DFLOAT fScalePercent = fTimeLeft/fFallTime;
						DFLOAT fScale = fScaleStart * fScalePercent;

						VEC_SET(vScale, fScale, fScale, fScale);
					}

					// special-case the light scale stuff for endless fall
					// if this is our first time in this case, don't clear the effect - otherwise clear it
					if (m_eCurContainerCode == CC_ENDLESS_FALL)
					{
						bClearCurrentLightScale = DTRUE;
					}
				}
				break;
				case CC_WIND:
				{
				}
				break;
				case CC_ZERO_GRAVITY:
				{
				}
				break;
				case CC_VACUUM:
				{
					VEC_SET(vScale, 0.8f, 0.8f, 0.8f);
				}
				break;
				case CC_LADDER:
				{
				}
				break;
				case CC_TOTAL_RED:
				{
					VEC_SET(vLightAdd, 1.0f, 0.0f, 0.0f);
					VEC_SET(vScale, 1.0f, 0.0f, 0.0f);
					pCurSound = "Sounds\\Voice\\totalred.wav";
				}
				break;
				case CC_TINT_SCREEN:
				{
					// Get the tint color out of the upper 3 bytes of the 
					// user flags...

					DBYTE r = (DBYTE)(dwUserFlags>>24);
					DBYTE g = (DBYTE)(dwUserFlags>>16);
					DBYTE b = (DBYTE)(dwUserFlags>>8);

					VEC_SET(vScale, (DFLOAT)r/255.0f, (DFLOAT)g/255.0f, (DFLOAT)b/255.0f);
				}
				break;

				default : break;
			}

		}
	}


	// See if we have entered/left a container...

	if (m_eCurContainerCode != eCode)
	{
		// See if the old container (if any) modified the light scale

		if (m_eCurContainerCode == CC_BLUE_WATER ||
			m_eCurContainerCode == CC_DIRTY_WATER ||
			m_eCurContainerCode == CC_CLEAR_WATER ||
			m_eCurContainerCode == CC_CORROSIVE_FLUID ||
			m_eCurContainerCode == CC_KATO ||
			m_eCurContainerCode == CC_LIQUID_NITROGEN ||
			m_eCurContainerCode == CC_POISON_GAS ||
			m_eCurContainerCode == CC_SMOKE ||
			m_eCurContainerCode == CC_ENDLESS_FALL ||
			m_eCurContainerCode == CC_VACUUM ||
			m_eCurContainerCode == CC_TOTAL_RED ||
			m_eCurContainerCode == CC_TINT_SCREEN)
		{
			bClearCurrentLightScale = DTRUE;
		}

		// Adjust Fog as necessary...
		
		if (m_bUseWorldFog)
		{
			ResetGlobalFog();
		}

		// See if we need to reset the current lightscale
		// If we entered a new container (or modified the light scale in the case of endless fall), then vScale won't be (1,1,1)
		// If we left a container that modified the light scale, then bClearCurrentLightScale will be DTRUE

		if (bClearCurrentLightScale)
		{
			m_LightScaleMgr.ClearLightScale (&m_vCurContainerLightScale, LightEffectEnvironment);
			VEC_SET (m_vCurContainerLightScale, -1.0f, -1.0f, -1.0f);
		}
		if (vScale.x != 1.0f || vScale.y != 1.0f || vScale.z != 1.0f)
		{
			m_LightScaleMgr.SetLightScale (&vScale, LightEffectEnvironment);
			VEC_COPY (m_vCurContainerLightScale, vScale);
		}

		// See if we are coming out of water...

		if (IsLiquid(m_eCurContainerCode) && !IsLiquid(eCode))
		{
			UpdateUnderWaterFX(DFALSE);
			pClientDE->RunConsoleString("+ModelWarble 0");
		}

		m_eCurContainerCode = eCode;

		if (m_hContainerSound)
		{
			pClientDE->KillSound(m_hContainerSound);
			m_hContainerSound = DNULL;
		}

		if (pCurSound)
		{
			PlaySoundInfo playSoundInfo;
			PLAYSOUNDINFO_INIT(playSoundInfo);

			playSoundInfo.m_dwFlags = PLAYSOUND_CLIENT | PLAYSOUND_LOCAL | PLAYSOUND_REVERB | PLAYSOUND_LOOP | PLAYSOUND_GETHANDLE;
			SAFE_STRCPY(playSoundInfo.m_szSoundName, pCurSound);
			pClientDE->PlaySound(&playSoundInfo);
			m_hContainerSound = playSoundInfo.m_hSound;
		}

		if (!m_bHaveNightVision && !m_bHaveInfrared)
		{
			if (!m_bTintScreen)
			{
				pClientDE->SetCameraLightAdd(m_hCamera, &vLightAdd);
			}
		}
	}


	// See if we are under water (under any liquid)...

	if (IsLiquid(m_eCurContainerCode)) 
	{
		pClientDE->RunConsoleString("ModelWarble 1");
		UpdateUnderWaterFX();
	}
	else
	{
		// UpdateBreathingFX();
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateUnderWaterFX
//
//	PURPOSE:	Update under water fx
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UpdateUnderWaterFX(DBOOL bUpdate)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera || m_bZoomView) return;

	DDWORD dwWidth = 640, dwHeight = 480;
	pClientDE->GetSurfaceDims(pClientDE->GetScreenSurface(), &dwWidth, &dwHeight);

	if (dwWidth < 0 || dwHeight < 0) return;


	// set under water flag...
	
	m_bUnderwater = bUpdate;

	// Initialize to default fov x and y...

	DFLOAT fFovX = m_fCurrentFovX;
	DFLOAT fFovY = (fFovX * dwHeight) / dwWidth;
	
	if (bUpdate)
	{
		pClientDE->GetCameraFOV(m_hCamera, &fFovX, &fFovY);

		DFLOAT fSpeed = .02f * pClientDE->GetFrameTime();

		if (m_fFovXFXDir > 0)
		{
			fFovX -= fSpeed;
			fFovY += fSpeed;

			if (fFovY > (m_fCurrentFovX * dwHeight) / dwWidth)
			{
				fFovY = (m_fCurrentFovX * dwHeight) / dwWidth;
				m_fFovXFXDir = -m_fFovXFXDir;
			}
		}
		else
		{
			fFovX += fSpeed;
			fFovY -= fSpeed;

			if (fFovY < (m_fCurrentFovX * dwHeight - 40) / dwWidth)
			{
				fFovY = (m_fCurrentFovX * dwHeight - 40) / dwWidth;
				m_fFovXFXDir = -m_fFovXFXDir;
			}
		}
	}

	pClientDE->SetCameraFOV(m_hCamera, fFovX, fFovY);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateBreathingFX
//
//	PURPOSE:	Update breathing fx
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UpdateBreathingFX(DBOOL bUpdate)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera || m_bZoomView) return;

	DDWORD dwWidth = 640, dwHeight = 480;
	pClientDE->GetSurfaceDims(pClientDE->GetScreenSurface(), &dwWidth, &dwHeight);

	if (dwWidth < 0 || dwHeight < 0) return;

	// Initialize to default fov x and y...

	DFLOAT fFovX = m_fCurrentFovX;
	DFLOAT fFovY = (fFovX * dwHeight) / dwWidth;
	
	if (bUpdate)
	{
		pClientDE->GetCameraFOV(m_hCamera, &fFovX, &fFovY);

		DFLOAT fSpeed = .005f * pClientDE->GetFrameTime();

		if (m_fFovXFXDir > 0)
		{
			fFovX -= fSpeed;
			fFovY -= fSpeed;

			if (fFovY < (m_fCurrentFovX * dwHeight - 10) / dwWidth)
			{
				fFovY = (m_fCurrentFovX * dwHeight - 10) / dwWidth;
				m_fFovXFXDir = -m_fFovXFXDir;
			}

		}
		else
		{
			fFovX += fSpeed;
			fFovY += fSpeed;

			if (fFovY > (m_fCurrentFovX * dwHeight) / dwWidth)
			{
				fFovY = (m_fCurrentFovX * dwHeight) / dwWidth;
				m_fFovXFXDir = -m_fFovXFXDir;
			}

		}
	}

	pClientDE->SetCameraFOV(m_hCamera, fFovX, fFovY);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ChangeWeapon()
//
//	PURPOSE:	Change the weapon model
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::ChangeWeapon(HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !hMessage) return;

	DBYTE nCommandId = pClientDE->ReadFromMessageByte(hMessage);
	DBYTE bAuto      = (DBOOL) pClientDE->ReadFromMessageByte(hMessage);

	DBYTE nWeaponId = GetWeaponId(nCommandId, GetPlayerMode());

#ifdef _DEMO
	if (!m_weaponModel.IsDemoWeapon(nCommandId))
	{
		// Tell the user the weapon isn't available...

		UpdatePlayerStats(IC_OUTOFAMMO_ID, nWeaponId, 1.0f);		
		return;
	}
#endif

	DBOOL bChange = DTRUE;

	// If this is an auto weapon change and this is a multiplayer game, see 
	// if the user really wants us to switch or not (we'll always switch in
	// single player games)...

	if (bAuto && IsMultiplayerGame())
	{
		bChange = DFALSE;

		HCONSOLEVAR hVar = pClientDE->GetConsoleVar("AutoWeaponSwitch");
		if (hVar)
		{
			bChange = (DBOOL) pClientDE->GetVarValueFloat(hVar);
		}
	}

	if (bChange)
	{
		// Force a change to the approprite weapon... 

		ChangeWeapon(nWeaponId, DFALSE, m_stats.GetAmmoCount(nWeaponId));
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ChangeWeapon()
//
//	PURPOSE:	Change the weapon model
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::ChangeWeapon(DBYTE nWeaponId, DBOOL bZoom, DDWORD dwAmmo)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

//	CSPrint("ChangeWeapon nWeaponId = %u", (DWORD) nWeaponId); // BLB Temp

	m_bZoomView = bZoom;

	HandleZoomChange(nWeaponId);

	if (!m_weaponModel.GetHandle() || nWeaponId != m_weaponModel.GetId())
	{
		m_weaponModel.Create(pClientDE, nWeaponId);
	}

	// Update the ammo display...

	m_stats.UpdateAmmo((DDWORD)nWeaponId, dwAmmo);
	m_stats.UpdatePlayerWeapon(nWeaponId);
		
	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;

	if (pSettings->VehicleMode() || m_playerCamera.IsChaseView())
	{
		m_weaponModel.SetVisible(DFALSE);
	}

	// Tell the server to change weapons...

	HMESSAGEWRITE hMessage = pClientDE->StartMessage(MID_WEAPON_CHANGE);
	pClientDE->WriteToMessageByte(hMessage, nWeaponId);
	pClientDE->EndMessage(hMessage);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandleZoomChange()
//
//	PURPOSE:	Handle a potential zoom change
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::HandleZoomChange(DBYTE nWeaponId)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
	
	if (m_bOldZoomView != m_bZoomView)
	{
		// Play zoom in/out sounds...

		char* pSound = (nWeaponId == GUN_SNIPERRIFLE_ID ? "Sounds\\Weapons\\SniperRifle\\zoomout.wav"
													    : "Sounds\\Weapons\\AssaultRifle\\zoomout.wav");
		if (m_bZoomView)
		{
			m_bDrawHud = DFALSE;

			pSound = (nWeaponId == GUN_SNIPERRIFLE_ID ? "Sounds\\Weapons\\SniperRifle\\zoomin.wav"
													  : "Sounds\\Weapons\\AssaultRifle\\zoomin.wav");
		}
		else
		{
			m_bDrawHud = DTRUE;
		}

		PlaySoundLocal(pSound, SOUNDPRIORITY_MISC_MEDIUM, DFALSE, DFALSE, 100, DTRUE );
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnCommandOn()
//
//	PURPOSE:	Handle client commands
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnCommandOn(int command)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// only allow input if not editing and not drawing the mission log

	if (m_messageMgr.GetEditingState())
	{
		return;
	}
	
	// take appropriate action

	switch (command)
	{
		case COMMAND_ID_PREV_WEAPON : 
		{
			m_weaponModel.ChangeToPrevWeapon(); 
		}
		break;
		
		case COMMAND_ID_NEXT_WEAPON : 
		{
			m_weaponModel.ChangeToNextWeapon();
		}
		break;

		case COMMAND_ID_WEAPON_1 :
		case COMMAND_ID_WEAPON_2 :
		case COMMAND_ID_WEAPON_3 :
		case COMMAND_ID_WEAPON_4 :
		case COMMAND_ID_WEAPON_5 :
		case COMMAND_ID_WEAPON_6 :
		case COMMAND_ID_WEAPON_7 :
		case COMMAND_ID_WEAPON_8 :
		case COMMAND_ID_WEAPON_9 :	
		case COMMAND_ID_WEAPON_10 :
		{
			m_weaponModel.ChangeWeapon(command);
		}
		break;

		case COMMAND_ID_SHOWORDINANCE :
		{
			m_bDrawOrdinance = (m_nPlayerMode != PM_MODE_KID) ? DTRUE : DFALSE;
		}
		break;
		
		case COMMAND_ID_DECSCREENRECT :
		case COMMAND_ID_INCSCREENRECT :
		{
			if (m_bUsingExternalCamera) break;

			DDWORD nWidth = 0;
			DDWORD nHeight = 0;
			HSURFACE hScreen = pClientDE->GetScreenSurface();
			pClientDE->GetSurfaceDims (hScreen, &nWidth, &nHeight);

			int dx = (int)(((float)nWidth * 0.1f) / 2.0f);		// 5% of screen width on either side
			int dy = (int)(((float)nHeight * 0.1f) / 2.0f);		// 5% of screen height on either side
			int nMinWidth = (int)nWidth >> 1;
			int nMinHeight = (int)nHeight >> 1;

			int nLeft = 0;
			int nTop = 0;
			int nRight = 0;
			int nBottom = 0;
			DBOOL bFullScreen = DFALSE;
			pClientDE->GetCameraRect (m_hCamera, &bFullScreen, &nLeft, &nTop, &nRight, &nBottom);

			// compute the actual resizing...
			
			if (command == COMMAND_ID_DECSCREENRECT)
			{
				// special case for letterbox format
				if (nTop == 0 && nBottom == (int)nHeight && !m_bStatsSizedOff)
				{
					dy = LETTERBOX_ADJUST;
					dx = 0;
				}
				else if (nTop == 0 && nBottom == (int)nHeight && m_bStatsSizedOff)
				{
					m_bStatsSizedOff = DFALSE;
					dx = 0;
					dy = 0;
				}
				else if (nTop == LETTERBOX_ADJUST && nLeft == 0)
				{
					nTop = 0;
					nBottom = nHeight;
				}
				
				if (((nRight - dx) - (nLeft + dx)) >= nMinWidth && ((nBottom - dy) - (nTop + dy)) >= nMinHeight)
				{
					nLeft += dx;
					nTop += dy;
					nRight -= dx;
					nBottom -= dy;
				}

				if (nLeft != 0 || nTop != 0 || nRight != (int)nWidth || nBottom != (int)nHeight) bFullScreen = DFALSE;
				pClientDE->SetCameraRect (m_hCamera, bFullScreen, nLeft, nTop, nRight, nBottom);
			}
			else
			{
				// special case for letterbox format
				if (nTop == LETTERBOX_ADJUST && nLeft == 0)
				{
					nTop = dy;
					nBottom = nHeight - dy;
				}
				else if (nLeft == 0 && nTop == 0 && !m_bStatsSizedOff)
				{
					m_bStatsSizedOff = DTRUE;
				}
				else if (nLeft - dx == 0 && nTop - dy == 0)
				{
					nTop += LETTERBOX_ADJUST;
					nBottom -= LETTERBOX_ADJUST;
				}

				nLeft -= dx;
				nTop -= dy;
				nRight += dx;
				nBottom += dy;

				if (nLeft < 0) nLeft = 0;
				if (nTop < 0) nTop = 0;
				if (nRight > (int)nWidth) nRight = (int)nWidth;
				if (nBottom > (int)nHeight) nBottom = (int)nHeight;

				if (nLeft == 0 && nTop == 0 && nRight == (int)nWidth && nBottom == (int)nHeight) bFullScreen = DTRUE;
				pClientDE->SetCameraRect (m_hCamera, bFullScreen, nLeft, nTop, nRight, nBottom);
			}

			AddToClearScreenCount();
		}
		break;

		case COMMAND_ID_FRAGCOUNT :
		{
			// make sure we're in multiplayer first
			
			if (!IsMultiplayerGame()) return;

			m_bDrawFragCount = DTRUE;
		}
		break;

		case COMMAND_ID_TURNAROUND :
		{
			m_fYaw += MATH_PI;
		}
		break;

		case COMMAND_ID_CAMERACIRCLE :
		{
			//if (!m_bSpectatorMode && !m_messageMgr.GetEditingState())
			//{
			//	m_playerCamera.StartCircle(0.0f, 75.0f, 0.0f, 3.0f);
			//}
		}
		break;
	
		case COMMAND_ID_CHASEVIEWTOGGLE :
		{
			if (m_ePlayerState == PS_ALIVE && !m_bZoomView)
			{
				SetExternalCamera(m_playerCamera.IsFirstPerson());
			}
		}
		break;

		case COMMAND_ID_CROSSHAIRTOGGLE :
		{
			m_stats.ToggleCrosshair();
			m_bCrosshairOn = m_stats.CrosshairOn();
		}
		break;

		case COMMAND_ID_MOUSEAIMTOGGLE :
		{
			CRiotSettings* pSettings = m_menu.GetSettings();
			if (!pSettings) return;
			
			if (m_playerCamera.IsFirstPerson())
			{
				if (!pSettings->MouseLook())
				{
					m_bHoldingMouseLook = DTRUE;
				}
			}
		}
		break;

		case COMMAND_ID_INTERFACETOGGLE:
		{
			// m_bDrawInterface = !m_bDrawInterface;
		}
		break;

		case COMMAND_ID_CENTERVIEW :
		{
			m_fPitch = 0.0f;
		}
		break;

		case COMMAND_ID_MESSAGE :
		{
			if (m_nGameState == GS_PLAYING)
			{
				m_messageMgr.SetEditingState(DTRUE);
				SetInputState(DFALSE);
			}
		}
		break;

		case COMMAND_ID_FIRING :
		{
			if (m_ePlayerState == PS_DEAD)
			{
				if (pClientDE->GetTime() > m_fEarliestRespawnTime)
				{
					HandleRespawn();
				}
			}
		}
		break;

		case COMMAND_ID_STRAFE:
		{
			m_bStrafing = DTRUE;
		}
		break;

		case COMMAND_ID_QUICKSAVE :
		{
			QuickSave();
		}
		break;

		case COMMAND_ID_QUICKLOAD :
		{
			QuickLoad();
		}
		break;

		default :
		break;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnCommandOff()
//
//	PURPOSE:	Handle command off notification
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnCommandOff(int command)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// only process if not editing a message
	if (!m_messageMgr.GetEditingState())
	{
		switch (command)
		{
			case COMMAND_ID_SHOWORDINANCE :
			{
				m_bDrawOrdinance = DFALSE;
			}
			break;

			case COMMAND_ID_FRAGCOUNT : 
			{
				m_bDrawFragCount = DFALSE;
			}
			break;

			case COMMAND_ID_STRAFE :
			{
				m_bStrafing = DFALSE;
			}
			break;

			case COMMAND_ID_MOUSEAIMTOGGLE :
			{
				CRiotSettings* pSettings = m_menu.GetSettings();
				if (!pSettings) return;
				
				m_bHoldingMouseLook = DFALSE;
			}
			break;

			default : break;
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnKeyDown(int key, int rep)
//
//	PURPOSE:	Handle key down notification
//				Try to avoid using OnKeyDown and OnKeyUp as they
//				are not portable functions
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnKeyDown(int key, int rep)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// get the vk codes for yes and no from cres.dll
	int nYesVKCode = TextHelperGetIntValFromStringID(pClientDE, IDS_YES_VK_CODE, VK_Y);
	int nNoVKCode = TextHelperGetIntValFromStringID(pClientDE, IDS_NO_VK_CODE, VK_N);


	//********  Are We Showing a MessageBox  ********//
	
	if (m_pMessageBox)
	{
		if ((key == VK_RETURN && !m_pMessageBox->IsYesNo()) || ((key == nYesVKCode || key == nNoVKCode) && m_pMessageBox->IsYesNo()))
		{
			DBOOL bYesNo = m_pMessageBox->IsYesNo();
			
			delete m_pMessageBox;
			m_pMessageBox = DNULL;

			if (bYesNo && m_pYesNoProc)
			{
				m_pYesNoProc (key == nYesVKCode, m_nYesNoUserData);
				m_pYesNoProc = DNULL;
				m_nYesNoUserData = 0;
			}

			if (m_nGameState == GS_MENU)
			{
				m_bClearScreenAlways = DTRUE;
			}
			else
			{
				PauseGame(DFALSE);
				AddToClearScreenCount();
			}
		}
		return;
	}


	//********  We are playing the credits  ********//

	if (m_bGameOver && m_nGameState == GS_PLAYING)
	{
		if (key != VK_ESCAPE)
		{
			m_credits.HandleInput (key);
			return;
		}
	}
	
	
	//********  Are We Broadcasting a Message  ********//
	
	if (m_messageMgr.GetEditingState())
	{
		m_messageMgr.HandleKeyDown (key, rep);
		return;
	}
	
	//********  Stopping Movies  ********//
	
	if (m_nGameState == GS_MOVIES)
	{
		if (key == VK_ESCAPE && pClientDE->IsVideoPlaying() == VIDEO_PLAYING)
		{
			if (pClientDE->StopVideo() == DE_OK)
			{
				PlayIntroMovies (pClientDE);
			}
		}
		return;
	}
	
	//********  Credits Mode Input  ********//

	if (m_nGameState == GS_CREDITS)
	{
		if (key == VK_ESCAPE)
		{
			m_nGameState = GS_MENU;
			SetMenuMusic(DTRUE);
			m_credits.Term();
		}
		else
		{
			m_credits.HandleInput (key);
		}

		return;
	}
	
	//********  Intro Mode Input  ********//

	if (m_nGameState == GS_INTRO)
	{
		if (key == VK_ESCAPE)
		{
			m_credits.Term();
			DoStartGame();
		}
		else
		{
			m_credits.HandleInput (key);
		}

		return;
	}

	//********  Stopping Bumper Screen Mode  ********//

	if (m_nGameState == GS_BUMPER)
	{
		// change our state to playing...

		m_nGameState = GS_PLAYING;
		PauseGame (DFALSE);

		// get rid of the bumper screen and text if we had one (there better be one if we got here!)
	
		RemoveBumperScreen();
		
		pClientDE->ClearInput(); // Don't process the key they hit...
		return;
	}
	
	//********  Menu Input  ********//
	
	if (m_nGameState == GS_MENU)
	{
		m_menu.HandleInput (key);
		return;
	}
	

	//********  Paused Input  ********//
	
	if (m_nGameState == GS_PAUSED)
	{
		// They pressed a key - unpause the game

		if (m_hGamePausedSurface) 
		{
			pClientDE->DeleteSurface (m_hGamePausedSurface);
			m_hGamePausedSurface = DNULL;
		}

		PauseGame (DFALSE);
		m_nGameState = GS_PLAYING;
		return;
	}

	
	//********  Demo Multiplayer Mode Input  ********//

	if (m_nGameState == GS_DEMO_MULTIPLAYER)
	{
		if (key == VK_ESCAPE)
		{
			m_nGameState = GS_MENU;
			SetMenuMusic(DTRUE);
			m_credits.Term();
		}
		else
		{
			m_credits.HandleInput (key);
		}

		return;
	}

	//********  Ingame Dialog Handling  ********//
	
	if (m_pIngameDialog)
	{
		switch (key)
		{
			case VK_DOWN:
			{
				m_pIngameDialog->ScrollDown();
				break;
			}

			case VK_UP:
			{
				m_pIngameDialog->ScrollUp();
				break;
			}

			case VK_NEXT:
			{
				m_pIngameDialog->PageDown();
				break;
			}

			case VK_PRIOR:
			{
				m_pIngameDialog->PageUp();
				break;
			}

			case VK_RETURN:
			{
				// get the current selection's string and target

				DFLOAT fSelection =  (DFLOAT)m_pIngameDialog->GetMenuSelection();
				POPUPMENUITEM* pItem = m_pIngameDialog->GetMenuItem ((int)fSelection);
				DDWORD nDlgObjHandle = pItem->nData;
				pItem->nData = 0;

				// delete the popup menu and send a message to the server

				delete m_pIngameDialog;
				m_pIngameDialog = DNULL;

				HMESSAGEWRITE hMessage = pClientDE->StartMessage(MID_DIALOG_CLOSE);
				pClientDE->WriteToMessageFloat (hMessage, fSelection);
				pClientDE->WriteToMessageByte (hMessage, (DBYTE) (nDlgObjHandle));
				pClientDE->WriteToMessageByte (hMessage, (DBYTE) (nDlgObjHandle >> 8));
				pClientDE->WriteToMessageByte (hMessage, (DBYTE) (nDlgObjHandle >> 16));
				pClientDE->WriteToMessageByte (hMessage, (DBYTE) (nDlgObjHandle >> 24));
				pClientDE->EndMessage(hMessage);

				// re-enable client-side input

				PauseGame (DFALSE);

				break;
			}
		}

		return;
	}
	
	//********  Mission Log Drawing  ********//
	
	if (m_bDrawMissionLog && !m_objectives.IsClosing() && !m_bMissionLogKeyStillDown)// && rep == 1)
	{
		if (key == VK_ESCAPE || key == VK_F1)
		{
			PlaySoundLocal ("Sounds\\Interface\\LogClose.wav", SOUNDPRIORITY_MISC_MEDIUM);
			
			if (key == VK_F1) m_bMissionLogKeyStillDown = DTRUE;
			m_objectives.StartCloseAnimation();
			m_bWaitingForMLClosure = DTRUE;
			
			m_objectives.ResetTop();
		}
		else if (key == VK_UP)
		{
			m_objectives.ScrollUp();
		}
		else if (key == VK_DOWN)
		{
			m_objectives.ScrollDown();
		}

		return;
	}


	if (key == VK_PAUSE)
	{
		if (IsMultiplayerGame() || m_nGameState != GS_PLAYING) return;
		
		if (!m_bGamePaused)
		{
			m_nGameState = GS_PAUSED;

			m_hGamePausedSurface = CTextHelper::CreateSurfaceFromString(pClientDE, m_menu.GetFont28n(), IDS_PAUSED);
		}

		PauseGame (!m_bGamePaused, DTRUE);
		return;
	}
	
	if (key == VK_F1 && !m_bDrawMissionLog && m_nPlayerMode != PM_MODE_KID && 
		!m_bMissionLogKeyStillDown && !m_bUsingExternalCamera)
	{
		// if we're in multiplayer just return
		
		if (IsMultiplayerGame()) return;

		PlaySoundLocal ("Sounds\\Interface\\LogOpen.wav", SOUNDPRIORITY_MISC_MEDIUM);
		
		m_objectives.ResetTop();

		m_bDrawMissionLog = DTRUE;
		m_bMissionLogKeyStillDown = DTRUE;
		m_bNewObjective = DFALSE;
		if (m_hNewObjective) pClientDE->DeleteSurface (m_hNewObjective);
		m_hNewObjective = DNULL;
		AddToClearScreenCount();

		PauseGame(DTRUE);

		m_LightScaleMgr.SetLightScale (&m_vLightScaleObjectives, LightEffectInterface);

		m_objectives.StartOpenAnimation();

		return;
	}

	//********  Escape Key Handling  ********//
	
	if (key == VK_ESCAPE)
	{
		if (m_nGameState == GS_PLAYING) //  && !m_bUsingExternalCamera)
		{
			SetMenuMode (DTRUE);
		}
		return;
	}

	//********  Show Player Stats  ********//
	
	if (key == VK_TAB)
	{
//		m_stats.ShowAllStats (DTRUE);
		return;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnKeyUp(int key, int rep)
//
//	PURPOSE:	Handle key down notification
//				Try to avoid using OnKeyDown and OnKeyUp as they
//				are not portable functions
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnKeyUp(int key)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// if it's the tilde (~) key then the console has been turned off
	// (this is the only event that causes this key to ever get processed)
	// so clear the back buffer to get rid of any part of the console still showing
	if (key == VK_TILDE)
	{
		AddToClearScreenCount();
	}

	if (m_messageMgr.GetEditingState())
	{
		m_messageMgr.HandleKeyUp (key);
	}

	if (key == VK_F1 && m_bMissionLogKeyStillDown)
	{
		m_bMissionLogKeyStillDown = DFALSE;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdatePlayerFlags
//
//	PURPOSE:	Update our copy of the movement flags
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdatePlayerFlags()
{
	// Clear movement flags...

	m_dwPlayerFlags = 0; 

	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;

	// only allow input if not editing and not drawing the mission log

	if (m_messageMgr.GetEditingState())
	{
		return;
	}

	// Determine what commands are currently on...

	if (pClientDE->IsCommandOn(COMMAND_ID_RUN) || pSettings->RunLock())
	{
		m_dwPlayerFlags |= CS_MFLG_RUN;
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_JUMP))
	{
		m_dwPlayerFlags |= CS_MFLG_JUMP;
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_DOUBLEJUMP))
	{
		m_dwPlayerFlags |= CS_MFLG_DOUBLEJUMP;
	}

	// Use the player's user flags instead of checking this flag directly
	// (this insures that the camera will be moved accurately...)
	//if (pClientDE->IsCommandOn(COMMAND_ID_DUCK))

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	if (hPlayerObj)
	{
		DDWORD dwUsrFlags;
		pClientDE->GetObjectUserFlags(hPlayerObj, &dwUsrFlags);
		if (dwUsrFlags & USRFLG_PLAYER_DUCK)
		{
			m_dwPlayerFlags |= CS_MFLG_DUCK;
		}
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_FORWARD))
	{
		m_dwPlayerFlags |= CS_MFLG_FORWARD;
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_REVERSE))
	{
		m_dwPlayerFlags |= CS_MFLG_REVERSE;
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_STRAFE))
	{
		m_dwPlayerFlags |= CS_MFLG_STRAFE;
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_RIGHT))
	{
		m_dwPlayerFlags |= CS_MFLG_RIGHT;
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_LEFT))
	{
		m_dwPlayerFlags |= CS_MFLG_LEFT;
	}

	if ( ((m_dwPlayerFlags & CS_MFLG_RIGHT) && 
		  (m_dwPlayerFlags & CS_MFLG_STRAFE)) ||
		  pClientDE->IsCommandOn(COMMAND_ID_STRAFE_RIGHT) )
	{
		m_dwPlayerFlags |= CS_MFLG_STRAFE_RIGHT;
	}

	if ( ((m_dwPlayerFlags & CS_MFLG_LEFT) && 
		  (m_dwPlayerFlags & CS_MFLG_STRAFE)) ||
		  pClientDE->IsCommandOn(COMMAND_ID_STRAFE_LEFT) )
	{
		m_dwPlayerFlags |= CS_MFLG_STRAFE_LEFT;
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_FIRING))
	{
		m_dwPlayerFlags |= CS_MFLG_FIRING;
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_LOOKUP))
	{
		m_dwPlayerFlags |= CS_MFLG_LOOKUP;
	}

	if (pClientDE->IsCommandOn(COMMAND_ID_LOOKDOWN))
	{
		m_dwPlayerFlags |= CS_MFLG_LOOKDOWN;
	}

	// Check to see if the player is moving...

	m_bMoving = m_bMovingSide = DFALSE;

	if (m_dwPlayerFlags & CS_MFLG_FORWARD ||
		m_dwPlayerFlags & CS_MFLG_REVERSE || 
		m_dwPlayerFlags & CS_MFLG_STRAFE_RIGHT ||
		m_dwPlayerFlags & CS_MFLG_STRAFE_LEFT)
	{
		m_bMoving = DTRUE;
	}

	if (m_dwPlayerFlags & CS_MFLG_STRAFE_RIGHT || 
		m_dwPlayerFlags & CS_MFLG_STRAFE_LEFT)
	{
		m_bMovingSide = DTRUE;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnMessage()
//
//	PURPOSE:	Handle client commands
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnMessage(DBYTE messageID, HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	DVector newColor;

	switch(messageID)
	{
		case MID_TIMEOFDAYCOLOR:
		{
			newColor.x = (float)pClientDE->ReadFromMessageByte(hMessage) / MAX_WORLDTIME_COLOR;
			newColor.y = (float)pClientDE->ReadFromMessageByte(hMessage) / MAX_WORLDTIME_COLOR;
			newColor.z = (float)pClientDE->ReadFromMessageByte(hMessage) / MAX_WORLDTIME_COLOR;
			m_LightScaleMgr.SetTimeOfDayScale(newColor);

			if(m_bUseWorldFog)
			{
				ResetGlobalFog();
			}
		}
		break;

		case MID_SERVERFORCEPOS:
		{
			if(m_MoveMgr)
				m_MoveMgr->OnServerForcePos(hMessage);

			m_bPlayerPosSet = DTRUE;
		}
		break;

		case MID_TRACTORBEAM_POS:
		{
			if(m_MoveMgr)
				m_MoveMgr->OnTractorBeamPos(hMessage);
		}
		break;

		case MID_PHYSICS_UPDATE:
		{
			if(m_MoveMgr)
				m_MoveMgr->OnPhysicsUpdate(hMessage);
		}
		break;

		case MID_PLAYER_ONGROUND :
		{
			m_bOnGround = (DBOOL)pClientDE->ReadFromMessageByte(hMessage);
		}
		break;


		case MID_PLAYER_INFOCHANGE :
		{
			int nThing = -1, nType = -1;
			DFLOAT fNewAmount = 0.0f;

			nThing = pClientDE->ReadFromMessageByte(hMessage);
			nType  = pClientDE->ReadFromMessageByte(hMessage);
			fNewAmount = pClientDE->ReadFromMessageFloat(hMessage);

			UpdatePlayerStats(nThing, nType, fNewAmount);
		}
		break;

		case MID_WEAPON_CHANGE :
		{
			ChangeWeapon(hMessage);
		}
		break;

		case MID_PLAYER_ADDED:
		{
			// only do something if we're in multiplayer
			if (!IsMultiplayerGame()) break;
			
			HSTRING hstrName = pClientDE->ReadFromMessageHString (hMessage);
			DDWORD nID = (DDWORD) pClientDE->ReadFromMessageFloat (hMessage);
			int nFrags = (int) pClientDE->ReadFromMessageFloat (hMessage);
			DBYTE r, g, b;
			r = pClientDE->ReadFromMessageByte(hMessage);
			g = pClientDE->ReadFromMessageByte(hMessage);
			b = pClientDE->ReadFromMessageByte(hMessage);
			m_ClientInfo.AddClient (hstrName, nID, nFrags, r, g, b);

			HSTRING hStr = pClientDE->FormatString (IDS_JOINEDGAME, pClientDE->GetStringData (hstrName));
			CSPrint (pClientDE->GetStringData (hStr));
			pClientDE->FreeString (hStr);
		}
		break;

		case MID_PLAYER_REMOVED:
		{
			// only do something if we're in multiplayer
			if (!IsMultiplayerGame()) break;
			
			DDWORD nID = (DDWORD) pClientDE->ReadFromMessageFloat (hMessage);
			
			HSTRING hStr = pClientDE->FormatString (IDS_LEFTGAME, m_ClientInfo.GetPlayerName (nID));
			CSPrint (pClientDE->GetStringData (hStr));
			pClientDE->FreeString (hStr);
			
			m_ClientInfo.RemoveClient (nID);
		}
		break;

		case MID_PINGTIMES:
		{
			while(1)
			{
				D_WORD id, ping;
				CLIENT_INFO *pClient;

				id = pClientDE->ReadFromMessageWord(hMessage);
				if(id == 0xFFFF)
					break;

				ping = pClientDE->ReadFromMessageWord(hMessage);
				if(pClient = m_ClientInfo.GetClientByID(id))
					pClient->m_Ping = ping / 1000.0f;
			}

			m_ClientInfo.ClearUpToDate();
		}
		break;

		case MID_PLAYER_FRAGGED:
		{
			// only do something if we're in multiplayer
			if (!IsMultiplayerGame()) break;

			DDWORD nLocalID = 0;
			pClientDE->GetLocalClientID (&nLocalID);

			DDWORD nVictim = (DDWORD) pClientDE->ReadFromMessageFloat (hMessage);
			DDWORD nKiller = (DDWORD) pClientDE->ReadFromMessageFloat (hMessage);

			if (nVictim == nKiller)
			{
				m_ClientInfo.RemoveFrag (nLocalID, nKiller);
			}
			else
			{
				m_ClientInfo.AddFrag (nLocalID, nKiller);
			}

			if (nVictim == nLocalID)
			{
				HSTRING hStr = DNULL;
				if (nVictim == nKiller)
				{
					hStr = pClientDE->FormatString (IDS_KILLEDMYSELF);
				}
				else
				{
					hStr = pClientDE->FormatString (IDS_HEKILLEDME, m_ClientInfo.GetPlayerName (nKiller));
				}
				CSPrint (pClientDE->GetStringData (hStr));
				pClientDE->FreeString (hStr);
			}
			else if (nKiller == nLocalID)
			{
				HSTRING hStr = DNULL;
				hStr = pClientDE->FormatString (IDS_IKILLEDHIM, m_ClientInfo.GetPlayerName (nVictim));
				CSPrint (pClientDE->GetStringData (hStr));
				pClientDE->FreeString (hStr);
			}
			else
			{
				HSTRING hStr = DNULL;

				if (nVictim == nKiller)
				{
					hStr = pClientDE->FormatString (IDS_HEKILLEDHIMSELF, m_ClientInfo.GetPlayerName(nKiller));
				}
				else
				{
					hStr = pClientDE->FormatString (IDS_HEKILLEDHIM, m_ClientInfo.GetPlayerName (nKiller), m_ClientInfo.GetPlayerName (nVictim));
				}
				CSPrint (pClientDE->GetStringData (hStr));
				pClientDE->FreeString (hStr);
			}
		}
		break;

		case MID_CHANGING_LEVELS :
		{
			HandleMPChangeLevel();
		}
		break;

		case MID_POWERUP_PICKEDUP :
		{
			PickupItemType eType = (PickupItemType)pClientDE->ReadFromMessageByte(hMessage);
			HandleItemPickedup(eType);
		}
		break;

		case MID_POWERUP_EXPIRED :
		{
			PickupItemType eType = (PickupItemType)pClientDE->ReadFromMessageByte(hMessage);
			HandleItemExpired(eType);
		}
		break;

		case MID_WEAPON_STATE :
		{
			m_weaponModel.HandleStateChange(hMessage);
		}
		break;

		case STC_BPRINT :
		{
			char msg[50];
			strncpy(msg, pClientDE->ReadFromMessageString(hMessage), 50);
			CSPrint (msg);
		}
		break;

		case MID_CAMERA_FOV :
		{
			DFLOAT x, y;
			x = pClientDE->ReadFromMessageFloat(hMessage);
			y = pClientDE->ReadFromMessageFloat(hMessage);

			pClientDE->SetCameraFOV(m_hCamera, x, y);

			// Save the current fovX...(used for zooming view)

			m_fCurrentFovX = x;
		}
		break;

		case MID_CAMERA_RECT :
		{
			int l, t, r, b;
			l = (int)pClientDE->ReadFromMessageFloat(hMessage);
			t = (int)pClientDE->ReadFromMessageFloat(hMessage);
			r = (int)pClientDE->ReadFromMessageFloat(hMessage);
			b = (int)pClientDE->ReadFromMessageFloat(hMessage);

			if (m_hCamera)
			{
				pClientDE->SetCameraRect(m_hCamera, DFALSE, l, t, r, b);
			}
		}
		break;

		case MID_PLAYER_MODECHANGE :
		{
			CRiotSettings* pSettings = m_menu.GetSettings();
			if (!pSettings) return;
			
			DBOOL  bOldVehicleMode = pSettings->VehicleMode();
			DDWORD nOldPlayerMode = m_nPlayerMode;
			DFLOAT x, y;

			pClientDE->ReadFromMessageVector(hMessage, &m_vCameraOffset);
			m_nPlayerMode  = pClientDE->ReadFromMessageByte(hMessage);
			pSettings->Misc[RS_MISC_VEHICLEMODE].nValue = (DBOOL)pClientDE->ReadFromMessageByte(hMessage);
			x			   = pClientDE->ReadFromMessageFloat(hMessage);
			y			   = pClientDE->ReadFromMessageFloat(hMessage);

			if (pSettings->VehicleMode() != bOldVehicleMode) 
			{
				// Going to vehicle mode...

				if (pSettings->VehicleMode())
				{
					// Assume we have the sniper rifle, so un-zoom (won't do
					// anything if we're not zoomed in)...

					m_bZoomView = DFALSE;
					HandleZoomChange(GUN_SNIPERRIFLE_ID);

					m_weaponModel.SetVisible(DFALSE);
					m_weaponModel.SetZoom(DFALSE);

					m_stats.SetDrawAmmo (DFALSE);
				}
				else 
				{
					if (m_playerCamera.IsFirstPerson())
					{
						m_weaponModel.SetVisible(DTRUE);
					}

					DBYTE nWeaponId = m_stats.GetCurWeapon();

					m_stats.SetDrawAmmo(DTRUE);
					m_stats.UpdatePlayerWeapon(nWeaponId, DTRUE);

					ChangeWeapon(nWeaponId, DFALSE, m_stats.GetAmmoCount(nWeaponId));
				}

				if (m_playerCamera.IsFirstPerson())
				{
					if (pSettings->VehicleMode()) ShowPlayer(DTRUE);
					else ShowPlayer(DFALSE);
				}
			}

			if (m_hCamera)
			{
				pClientDE->SetCameraFOV(m_hCamera, x, y);
			}

			m_playerCamera.SetFirstPersonOffset(m_vCameraOffset);

			if (nOldPlayerMode == PM_MODE_KID && m_nPlayerMode != PM_MODE_KID)
			{
				m_bDrawInterface = DTRUE;
			}
			else if (m_nPlayerMode == PM_MODE_KID)
			{
				m_bDrawInterface = DFALSE;
			}

			m_stats.UpdatePlayerMode (m_nPlayerMode);
		}
		break;

		case MID_PLAYER_EXITLEVEL :
		{	
			HandleExitLevel(hMessage);
		}
		break;

		case MID_PLAYER_STATE_CHANGE :
		{
			HandlePlayerStateChange(hMessage);
		}
		break;

		case MID_PLAYER_AUTOSAVE :
		{
			AutoSave(hMessage);
		}
		break;

		case MID_PLAYER_DAMAGE:
		{
			HandlePlayerDamage(hMessage);
		}
		break;

		case MID_PLAYER_MESSAGE :
		{
			// retrieve the string from the message, play the chat sound, and display the message
			
			char *pMessage = pClientDE->ReadFromMessageString (hMessage);

			HSTRING hstrChatSound = pClientDE->FormatString (IDS_CHATSOUND);
			PlaySoundInfo playSoundInfo;
			PLAYSOUNDINFO_INIT(playSoundInfo);
			playSoundInfo.m_dwFlags = PLAYSOUND_CLIENT | PLAYSOUND_LOCAL;
			SAFE_STRCPY(playSoundInfo.m_szSoundName, pClientDE->GetStringData (hstrChatSound));
			pClientDE->PlaySound(&playSoundInfo);

			pClientDE->FreeString(hstrChatSound);

			CSPrint (pMessage);
		}
		break;

		case MID_PLAYER_ORIENTATION :
		{
			// Set our pitch, yaw, and roll according to the players...

			DVector vVec;
			pClientDE->ReadFromMessageVector(hMessage, &vVec);

			m_fPitch		= vVec.x;
			m_fYaw			= vVec.y;
			m_fCamCant		= vVec.z;
			m_fYawBackup	= m_fYaw;
			m_fPitchBackup	= m_fPitch;
		}
		break;

		case MID_COMMAND_TOGGLE :
		{
			DBYTE nId = pClientDE->ReadFromMessageByte(hMessage);

			switch(nId)
			{
				case COMMAND_ID_RUNLOCK :
				{
					CRiotSettings* pSettings = m_menu.GetSettings();
					if (pSettings)
					{
						pSettings->SetRunLock((DBOOL)pClientDE->ReadFromMessageByte(hMessage));
					}
				}
				break;

				default : break;
			}
		}
		break;

		case MID_COMMAND_SHOWGAMEMSG :
		{
			char* strMessage = pClientDE->ReadFromMessageString (hMessage);
			ShowGameMessage (strMessage);
		}
		break;

		case MID_MUSIC:
		{
			if( m_Music.IsInitialized( ))
				m_Music.HandleMusicMessage( hMessage );
			break;
		}

		case MID_COMMAND_SHOWDLG:
		{
			m_pIngameDialog = CreateIngameDialog (hMessage);
			if (m_pIngameDialog)
			{
				// disable client input
				PauseGame (DTRUE);
			}
		}
		break;

		case MID_COMMAND_OBJECTIVE:
		{
			HandleObjectives (hMessage);
		}
		break;

		case MID_COMMAND_TRANSMISSION:
		{
			HandleTransmission(hMessage);
		}
		break;

		case MID_PLAYER_LOADCLIENT :
		{
			UnpackClientSaveMsg(hMessage);
		}
		break;

		case MID_PLAYER_MULTIPLAYER_INIT :
		{
			InitMultiPlayer();
		}
		break;

		case MID_PLAYER_SINGLEPLAYER_INIT :
		{
			InitSinglePlayer();
		}
		break;

		case MID_SERVER_ERROR :
		{
			HandleServerError(hMessage);
		}
		break;

		case MID_SFX_REG:
		{
			SFXReg *pReg;
			char *pClassName;
			DDWORD id;

			pClassName = pClientDE->ReadFromMessageString(hMessage);
			id = pClientDE->ReadFromMessageByte(hMessage); 

			if(pReg = FindSFXRegByName(pClassName))
			{
				pReg->m_ID = id;
			}
		}
		break;

		default : break;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ShakeScreen()
//
//	PURPOSE:	Shanke, rattle, and roll
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::ShakeScreen(DVector vShake)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// Add...

	VEC_ADD(m_vShakeAmount, m_vShakeAmount, vShake);

	if (m_vShakeAmount.x > 3.0f) m_vShakeAmount.x = 3.0f;
	if (m_vShakeAmount.y > 3.0f) m_vShakeAmount.y = 3.0f;
	if (m_vShakeAmount.z > 3.0f) m_vShakeAmount.z = 3.0f;
}


// ----------------------------------------------------------------------- //
// Console command handlers for recording and playing demos.
// ----------------------------------------------------------------------- //

void CRiotClientShell::HandleRecord(int argc, char **argv)
{
	ClientDE *pClientDE = GetClientDE();

	if(argc < 2)
	{
		pClientDE->CPrint("Record <world name> <filename>");
		return;
	}

	if(!DoLoadWorld(argv[0], NULL, NULL, LOAD_NEW_GAME, argv[1], NULL))
	{
		pClientDE->CPrint("Error starting world");
	}
}

void CRiotClientShell::HandlePlaydemo(int argc, char **argv)
{
	ClientDE *pClientDE = GetClientDE();

	if(argc < 1)
	{
		pClientDE->CPrint("Playdemo <filename>");
		return;
	}

	if(!DoLoadWorld("asdf", NULL, NULL, LOAD_NEW_GAME, NULL, argv[0]))
	{
		pClientDE->CPrint("Error starting world");
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::TintScreen()
//
//	PURPOSE:	Tint screen
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::TintScreen(DVector vTintColor, DVector vPos, DFLOAT fTintRange,
								  DFLOAT fTime, DFLOAT fRampUp, DFLOAT fRampDown, DBOOL bForce)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	CRiotSettings* pSettings = g_pRiotClientShell->GetSettings();
	if (!pSettings) return;
	
	if (!bForce && !pSettings->ScreenFlash()) return;

	DVector vCamPos;
	pClientDE->GetObjectPos(m_hCamera, &vCamPos);

	// Determine if we can see this...

	DVector vDir;
	VEC_SUB(vDir, vPos, vCamPos);
	DFLOAT fDirMag = VEC_MAG(vDir);
	if (fDirMag > fTintRange) return;

	// Okay, not adjust the tint based on the camera's angle to the tint pos.

	DRotation rRot;
	DVector vU, vR, vF;
	pClientDE->GetObjectRotation(m_hCamera, &rRot);
	pClientDE->GetRotationVectors(&rRot, &vU, &vR, &vF);

	VEC_NORM(vDir);
	VEC_NORM(vF);
	DFLOAT fMul = VEC_DOT(vDir, vF);
	if (fMul <= 0.0f) return;

	// {MD} See if we can even see this point.
	ClientIntersectQuery iQuery;
	ClientIntersectInfo iInfo;
	memset(&iQuery, 0, sizeof(iQuery));
	iQuery.m_From = vPos;
	iQuery.m_To = vCamPos;
	if(pClientDE->IntersectSegment(&iQuery, &iInfo))
	{
		// Something is in the way.
		return;
	}

	// Tint less if the pos was far away from the camera...

	DFLOAT fVal = 1.0f - (fDirMag/fTintRange);
	fMul *= (fVal <= 1.0f ? fVal : 1.0f);

	m_bTintScreen	= DTRUE;
	m_fTintStart	= pClientDE->GetTime();
	m_fTintTime		= fTime;
	m_fTintRampUp	= fRampUp;
	m_fTintRampDown	= fRampDown;
	VEC_COPY(m_vTintColor, vTintColor);
	VEC_MULSCALAR(m_vTintColor, m_vTintColor, fMul);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ShowGameMessage
//
//	PURPOSE:	Displays a message from the game in the center of the screen
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::ShowGameMessage(char* strMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if (m_hGameMessage)
	{
		pClientDE->DeleteSurface (m_hGameMessage);
		m_hGameMessage = NULL;
	}

	HDECOLOR hForeColor = pClientDE->SetupColor1 (1.0f, 1.0f, 1.0f, DFALSE);
	HDECOLOR hTrans = pClientDE->SetupColor2 (0.0f, 0.0f, 0.0f, DTRUE);
	
	HSTRING hstrFont = pClientDE->FormatString (IDS_INGAMEFONT);
	FONT fontdesc (pClientDE->GetStringData(hstrFont), 
					TextHelperGetIntValFromStringID(pClientDE, IDS_GAMEMESSAGETEXTWIDTH, 9),
					TextHelperGetIntValFromStringID(pClientDE, IDS_GAMEMESSAGETEXTHEIGHT, 18),
					DFALSE, DFALSE, DFALSE);
	pClientDE->FreeString (hstrFont);

	m_hGameMessage = CTextHelper::CreateSurfaceFromString (pClientDE, &fontdesc, strMessage, hForeColor);
	pClientDE->OptimizeSurface (m_hGameMessage, hTrans);

	DDWORD cx, cy;
	pClientDE->GetSurfaceDims (m_hGameMessage, &cx, &cy);
	m_rcGameMessage.left = 0;
	m_rcGameMessage.top = 0;
	m_rcGameMessage.right = (int)cx;
	m_rcGameMessage.bottom = (int)cy;
	
	m_nGameMessageRemoveTime = pClientDE->GetTime() + 2.0f;
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateHeadBob
//
//	PURPOSE:	Adjusts the head bobbing & swaying
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UpdateHeadBob()
{
	// don't update the head bob if we have a dialog up

	if (m_pIngameDialog || m_bGamePaused) return;

	// on with bobbing...

	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;
	
	const DFLOAT c_fMaxBobAmp  = 10.0f;
	const DFLOAT c_fDecayValue = 100.0f;

	DBOOL bMecha = !((m_nPlayerMode == PM_MODE_FOOT) || (m_nPlayerMode == PM_MODE_KID));

	DFLOAT	bobH  = BOBH_WALKING  / (bMecha ? (pSettings->VehicleMode() ? 500.0f : 165.0f) : 500.0f);
	DFLOAT  bobV  = BOBV_WALKING  / (bMecha ? (pSettings->VehicleMode() ? 500.0f : 50.0f)  : 100.0f); // 500.0f;
	DFLOAT  swayH = SWAYH_WALKING / 4000.0f;
	DFLOAT  swayV = SWAYV_WALKING / 4000.0f;

	DBOOL bRunning = (DBOOL)(m_dwPlayerFlags & CS_MFLG_RUN);
	DFLOAT moveDist = m_fVelMagnitude * pClientDE->GetFrameTime();

	// Running

	DFLOAT pace;
	if (bMecha && !pSettings->VehicleMode())
	{
		if (bRunning) pace = MATH_CIRCLE * 0.8f;
		else pace = MATH_CIRCLE * 0.6;
	}
	else
	{
		if (bRunning) pace = MATH_CIRCLE * 0.7f;
		else pace = MATH_CIRCLE * 0.5;
	}

	m_fBobPhase += (pClientDE->GetFrameTime() * pace);
	if (m_fBobPhase > MATH_CIRCLE) m_fBobPhase -= MATH_CIRCLE;

	if (bRunning) pace = MATH_CIRCLE * 0.8f;
	else pace = 5*MATH_CIRCLE/6;

	m_fSwayPhase += (pClientDE->GetFrameTime() * pace) / 2;
	if (m_fSwayPhase > MATH_CIRCLE) m_fSwayPhase -= MATH_CIRCLE;


	// decay the amplitude
	m_fBobAmp = m_fBobAmp - pClientDE->GetFrameTime() * c_fDecayValue;
	if (m_fBobAmp < 0) m_fBobAmp = 0;

	if (m_fBobAmp < c_fMaxBobAmp)
	{
		m_fBobAmp += moveDist;
	}
	if (m_fBobAmp > c_fMaxBobAmp) m_fBobAmp = c_fMaxBobAmp;

	m_fBobHeight = bobV * m_fBobAmp * (float)sin(m_fBobPhase * 2);
	m_fBobWidth  = bobH * m_fBobAmp * (float)sin(m_fBobPhase - (MATH_PI/4));

	DFLOAT fSwayHeight = -swayV * m_fBobAmp * (float)sin(m_fSwayPhase * 2);
	DFLOAT fSwayWidth  = swayH * m_fBobAmp * (float)sin(m_fSwayPhase - (MATH_PI/3));


	// Update the weapon model bobbing...

	m_weaponModel.UpdateBob(fSwayWidth, fSwayHeight);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateHeadCant()
//
//	PURPOSE:	Update head tilt when strafing
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateHeadCant()
{
	DFLOAT fMaxCantDist = m_fCantMaxDist;
	
	if (IsVehicleMode())
	{
		fMaxCantDist *= 1.5f;
	}
	else if (IsOnFoot())
	{
		fMaxCantDist *= 0.5f;
	}

		// If we are strafing, cant our head as we move..

	if (m_dwPlayerFlags & CS_MFLG_STRAFE_RIGHT)
	{
		m_fCamCant -= m_fCantIncrement;
		if (m_fCamCant < -fMaxCantDist)
			m_fCamCant = -fMaxCantDist;
	}
	else if (m_dwPlayerFlags & CS_MFLG_STRAFE_LEFT)
	{
		m_fCamCant += m_fCantIncrement;
		if (m_fCamCant > fMaxCantDist)
			m_fCamCant = fMaxCantDist;
	}
	else 
	{
		// We are not canting so move us toward zero...
		if (m_fCamCant != 0.0f)
		{
			if (m_fCamCant < 0.0f) 
			{
				m_fCamCant += m_fCantIncrement;
			} 
			else 
			{
				m_fCamCant -= m_fCantIncrement;
			}
			if (fabs(m_fCamCant) < m_fCantIncrement)
				m_fCamCant = 0.0f;
 		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateDuck()
//
//	PURPOSE:	Update ducking camera offset
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateDuck()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;


	// Can't duck when in liquid...

	if (IsLiquid(m_eCurContainerCode)) return;


	DFLOAT fTime = pClientDE->GetTime();

	if (m_dwPlayerFlags & CS_MFLG_DUCK)
	{
		m_bStartedDuckingUp = DFALSE;

		// See if the duck just started...

		if (!m_bStartedDuckingDown)
		{
			m_bStartedDuckingDown = DTRUE;
			m_fStartDuckTime = fTime;
		}

		m_fCamDuck = m_fDuckDownV * (fTime - m_fStartDuckTime);

		if (m_fCamDuck < m_fMaxDuckDistance)
		{
			m_fCamDuck = m_fMaxDuckDistance;
		}

	}
	else if (m_fCamDuck < 0.0) // Raise up
	{
		m_bStartedDuckingDown = DFALSE;

		if (!m_bStartedDuckingUp)
		{
			m_fStartDuckTime = fTime;
			m_bStartedDuckingUp = DTRUE;
		}

		m_fCamDuck += m_fDuckUpV * (fTime - m_fStartDuckTime);

		if (m_fCamDuck > 0.0f)
		{
			m_fCamDuck = 0.0f;
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ProcessMenuCommand()
//
//	PURPOSE:	Process a menu command
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::ProcessMenuCommand (int nID)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return DTRUE;
	
	return DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdatePlayerStats()
//
//	PURPOSE:	Update the player's stats
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdatePlayerStats(DBYTE nThing, DBYTE nType, DFLOAT fNewAmount)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
	
	switch (nThing)
	{
		case IC_WEAPON_PICKUP_ID :
		{
			m_inventory.GunPickup (nType);
			m_stats.UpdateAmmo ((DDWORD) nType, (DDWORD) fNewAmount);
			m_inventory.UpdateAmmo ((DDWORD) nType, (DDWORD) fNewAmount);
		}
		break;

		case IC_WEAPON_ID :
		{
			m_inventory.GunPickup (nType, DFALSE);
			m_stats.UpdateAmmo ((DDWORD) nType, (DDWORD) fNewAmount);
			m_inventory.UpdateAmmo ((DDWORD) nType, (DDWORD) fNewAmount);
		}
		break;

		case IC_AMMO_ID :
		{		
			m_stats.UpdateAmmo ((DDWORD) nType, (DDWORD) fNewAmount);
			m_inventory.UpdateAmmo ((DDWORD) nType, (DDWORD) fNewAmount);
		}
		break;

		case IC_HEALTH_ID :
		{		
			m_stats.UpdateHealth ((DDWORD) fNewAmount);
		}
		break;

		case IC_ARMOR_ID :
		{		
			m_stats.UpdateArmor ((DDWORD) fNewAmount);
		}
		break;

		case IC_AIRLEVEL_ID :
		{	
			m_stats.UpdateAir (fNewAmount);
		}
		break;

		case IC_ROCKETLOCK_ID :
		{
			// Play targeting sound..

			char* pSound = (nType == 1 ? "Sounds\\Weapons\\lockedon.wav"
									   : "Sounds\\Weapons\\lockingon.wav");
	
			HSTRING hStr = pClientDE->FormatString (nType == 1 ? IDS_ROCKETLOCKON : IDS_ROCKETLOCKDETECTED);

			pClientDE->CPrint(pClientDE->GetStringData (hStr));

			m_infoDisplay.AddInfo(pClientDE->GetStringData (hStr), m_menu.GetFont12s(), 1.5f, DI_CENTER | DI_TOP);

			PlaySoundLocal(pSound, SOUNDPRIORITY_MISC_MEDIUM );

			pClientDE->FreeString (hStr);
		}
		break;

		case IC_OUTOFAMMO_ID :
		{

#ifdef _DEMO
			// If the newamount is 1.0 and this is a demo, let user know this
			// weapon is not available...

			if (fNewAmount == 1.0f)
			{
				char buf[50];
				sprintf(buf, "Not Available in Demo");
				pClientDE->CPrint(buf);
				m_infoDisplay.AddInfo(buf, m_menu.GetFont12s(), 1.5f, DI_CENTER | DI_BOTTOM);
				break;
			}
#endif

			HSTRING hStr = pClientDE->FormatString (IDS_OUTOFAMMO, GetWeaponString((RiotWeaponId)nType));
			pClientDE->CPrint(pClientDE->GetStringData (hStr));
			m_infoDisplay.AddInfo(pClientDE->GetStringData (hStr), m_menu.GetFont12s(), 1.5f, DI_CENTER | DI_BOTTOM);
			pClientDE->FreeString (hStr);
		}
		break;

		default : break;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::CreateIngameDialog()
//
//	PURPOSE:	Create an ingame dialog
//
// ----------------------------------------------------------------------- //

CPopupMenu* CRiotClientShell::CreateIngameDialog (HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return DNULL;

	// get the strings we need for the dialog

	DDWORD nByte1 = (DDWORD) pClientDE->ReadFromMessageByte (hMessage);
	DDWORD nByte2 = (DDWORD) pClientDE->ReadFromMessageByte (hMessage);
	DDWORD nByte3 = (DDWORD) pClientDE->ReadFromMessageByte (hMessage);
	DDWORD nByte4 = (DDWORD) pClientDE->ReadFromMessageByte (hMessage);

	int nItems = (int)pClientDE->ReadFromMessageFloat (hMessage);
	if (nItems == 0) return DNULL;

	DDWORD nStringID[3];
	for (int i = 0; i < nItems; i++)
	{
		nStringID[i] = pClientDE->ReadFromMessageDWord (hMessage);
	}

	// create the string surfaces

	HSURFACE hSurf[3];
	HSURFACE hSurfSelected[3];	
	for (i = 0; i < nItems; i++)
	{
		hSurf[i] = CTextHelper::CreateSurfaceFromString (pClientDE, m_menu.GetFont08n(), nStringID[i]);
		hSurfSelected[i] = CTextHelper::CreateSurfaceFromString (pClientDE, m_menu.GetFont08s(), nStringID[i]);
	}
	
	// find the string with the largest width and largest height

	DDWORD nMaxHeight = 0;
	DDWORD nMaxWidth = 0;
	DDWORD nWidth, nHeight;
	for (i = 0; i < nItems; i++)
	{
		pClientDE->GetSurfaceDims (hSurf[i], &nWidth, &nHeight);
		if (nWidth > nMaxWidth) nMaxWidth = nWidth;
		if (nHeight > nMaxHeight) nMaxHeight = nHeight;
		
		pClientDE->GetSurfaceDims (hSurfSelected[i], &nWidth, &nHeight);
		if (nWidth > nMaxWidth) nMaxWidth = nWidth;
		if (nHeight > nMaxHeight) nMaxHeight = nHeight;
	}
	
	// make sure we're not wider than the screen width or 600, whichever is smaller

	DDWORD nScreenWidth, nScreenHeight;
	pClientDE->GetSurfaceDims (pClientDE->GetScreenSurface(), &nScreenWidth, &nScreenHeight);
	if (nScreenWidth <= 16)
	{
		// possible infinite loop below if nScreenWidth comes back as fucked-up value
		for (i = 0; i < nItems; i++)
		{
			pClientDE->DeleteSurface (hSurf[i]);
			pClientDE->DeleteSurface (hSurfSelected[i]);
		}
	}

	if (nMaxWidth + 16 > (min (nScreenWidth, 600)))
	{
		DDWORD nShortenedWidth = nMaxWidth / 2;
		while (nShortenedWidth + 16 > (min (nScreenWidth, 600)))
		{
			nShortenedWidth /= 2;
		}
		
		for (i = 0; i < nItems; i++)
		{
			pClientDE->DeleteSurface (hSurf[i]);
			pClientDE->DeleteSurface (hSurfSelected[i]);

			hSurf[i] = CTextHelper::CreateWrappedStringSurface (pClientDE, (int)nShortenedWidth, m_menu.GetFont08n(), nStringID[i], TH_ALIGN_CENTER, DFALSE);
			hSurfSelected[i] = CTextHelper::CreateWrappedStringSurface (pClientDE, (int)nShortenedWidth, m_menu.GetFont08s(), nStringID[i], TH_ALIGN_CENTER, DFALSE);
		}
	}

	nMaxWidth = 0;
	nMaxHeight = 0;
	int nTotalHeight = 0;
	for (i = 0; i < nItems; i++)
	{
		pClientDE->GetSurfaceDims (hSurf[i], &nWidth, &nHeight);
		if (nWidth > nMaxWidth) nMaxWidth = nWidth;
		if (nHeight > nMaxHeight) nMaxHeight = nHeight;

		nTotalHeight += nHeight;
		
		pClientDE->GetSurfaceDims (hSurfSelected[i], &nWidth, &nHeight);
		if (nWidth > nMaxWidth) nMaxWidth = nWidth;
		if (nHeight > nMaxHeight) nMaxHeight = nHeight;
	}
	
	// now determine the best location (center screen?)

	pClientDE->GetSurfaceDims (pClientDE->GetScreenSurface(), &nWidth, &nHeight);
	int x = ((int)nWidth - ((int)nMaxWidth + 16)) / 2;
	int y = ((int)nHeight - ((int)nMaxHeight + 16)) / 2;

	// create the popup menu and start adding the menu items
	
	if (m_pIngameDialog)
	{
		delete m_pIngameDialog;
	}

	CPopupMenu* pIngameDialog = new CPopupMenu (x, y, (int)nMaxWidth + 16, nTotalHeight + 16);
	if (!pIngameDialog) return DNULL;

	pIngameDialog->Init (pClientDE, DNULL);

	for (i = 0; i < nItems; i++)
	{
		POPUPMENUITEM* pItem = new POPUPMENUITEM;
		if (!pItem)
		{
			delete pIngameDialog;
			return DNULL;
		}

		pClientDE->GetSurfaceDims (hSurf[i], &nWidth, &nHeight);

		pItem->nHeight = (int)nHeight;
		pItem->nSelectionPoint = (int)nHeight / 2;
		pItem->hSurface = hSurf[i];
		pItem->hSelected = hSurfSelected[i];
		pItem->nType = List;
		pItem->nData = nByte1 | (nByte2 << 8) | (nByte3 << 16) | (nByte4 << 24);

		pIngameDialog->AddItem (pItem);
		pItem = DNULL;
	}

	return pIngameDialog;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandleObjectives()
//
//	PURPOSE:	Handles the mission objective message
//
// ----------------------------------------------------------------------- //
		
void CRiotClientShell::HandleObjectives (HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// get the strings we need 

	HSTRING hstrAdd = pClientDE->ReadFromMessageHString (hMessage);
	char* strAdd = pClientDE->GetStringData (hstrAdd);
	
	HSTRING hstrRemove = pClientDE->ReadFromMessageHString (hMessage);
	char* strRemove = pClientDE->GetStringData (hstrRemove);
	
	HSTRING hstrComplete = pClientDE->ReadFromMessageHString (hMessage);
	char* strComplete = pClientDE->GetStringData (hstrComplete);

	// play log updated sound if the log has been updated

	if (strAdd)
	{
		PlaySoundLocal ("Sounds\\Interface\\Update.wav", SOUNDPRIORITY_MISC_MEDIUM);
	}

	// parse out the string ids

	char* ptr = strtok (strAdd, " ");
	HDECOLOR hTrans = pClientDE->SetupColor2 (0.0f, 0.0f, 0.0f, DTRUE);
	while (ptr)
	{
		DDWORD nID = atoi (ptr);
		m_objectives.AddObjective (nID);
		m_bNewObjective = DTRUE;
		m_hNewObjective = pClientDE->CreateSurfaceFromBitmap ("interface/Log_Updated.pcx");
		pClientDE->OptimizeSurface (m_hNewObjective, hTrans);

		ptr = strtok (NULL, " ");
	}

	ptr = strtok (strRemove, " ");
	while (ptr)
	{
		DDWORD nID = atoi (ptr);
		m_objectives.RemoveObjective (nID);

		ptr = strtok (NULL, " ");
	}

	ptr = strtok (strComplete, " ");
	while (ptr)
	{
		DDWORD nID = atoi (ptr);
		m_objectives.CompleteObjective (nID);

		ptr = strtok (NULL, " ");
	}

	pClientDE->FreeString(hstrAdd);
	pClientDE->FreeString(hstrRemove);
	pClientDE->FreeString(hstrComplete);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandleTransmission()
//
//	PURPOSE:	Handles the transmission message
//
// ----------------------------------------------------------------------- //
		
void CRiotClientShell::HandleTransmission (HMESSAGEREAD hMessage)
{
	HMESSAGEWRITE hMsg;
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
	char *pszImage;

	DDWORD	nStringID = pClientDE->ReadFromMessageDWord(hMessage);
	pszImage = pClientDE->ReadFromMessageString (hMessage);
	
	DDWORD nScreenWidth, nScreenHeight;
	HSURFACE hScreen = pClientDE->GetScreenSurface();
	pClientDE->GetSurfaceDims (hScreen, &nScreenWidth, &nScreenHeight);

	if (m_hTransmissionImage) pClientDE->DeleteSurface (m_hTransmissionImage);
	if (m_hTransmissionText) pClientDE->DeleteSurface (m_hTransmissionText);

	char strImageFilename[256];
	SAFE_STRCPY(strImageFilename, "Interface\\TranPix\\");
	strcat (strImageFilename, pszImage );

	m_hTransmissionImage = pClientDE->CreateSurfaceFromBitmap (strImageFilename);
	HDECOLOR hTrans = pClientDE->SetupColor1 (0.0f, 0.0f, 1.0f, DTRUE);
	pClientDE->OptimizeSurface (m_hTransmissionImage, hTrans);

	HSTRING hstrFont = pClientDE->FormatString (IDS_INGAMEFONT);
	FONT fontdef (pClientDE->GetStringData (hstrFont), 
					TextHelperGetIntValFromStringID(pClientDE, IDS_TRANSMISSIONTEXTWIDTH, 6),
					TextHelperGetIntValFromStringID(pClientDE, IDS_TRANSMISSIONTEXTHEIGHT, 12));
	pClientDE->FreeString (hstrFont);

	HDECOLOR foreColor = pClientDE->SetupColor1 (1.0f, 1.0f, 1.0f, DFALSE);
	m_hTransmissionText = CTextHelper::CreateWrappedStringSurface (pClientDE, min (256, nScreenWidth - 90), &fontdef, nStringID, foreColor);
	hTrans = pClientDE->SetupColor1(0.0f, 0.0f, 0.0f, DTRUE);
	pClientDE->OptimizeSurface (m_hTransmissionText, hTrans);

	if (!m_hTransmissionImage || !m_hTransmissionText)
	{
		if (m_hTransmissionImage) pClientDE->DeleteSurface (m_hTransmissionImage);
		if (m_hTransmissionText) pClientDE->DeleteSurface (m_hTransmissionText);
		m_hTransmissionImage = DNULL;
		m_hTransmissionText = DNULL;
		m_fTransmissionTimeLeft = 0.0f;
		return;;
	}

	// Make sure we're not already playing a transmission sound...

	if (m_hTransmissionSound)
	{
		pClientDE->KillSound(m_hTransmissionSound);
		m_hTransmissionSound = DNULL;

		// Tell server the transmission ended.
		hMsg = pClientDE->StartMessage( MID_TRANSMISSIONENDED );
		pClientDE->EndMessage( hMsg );
	}

	// Play the sound streamed...

	char strSoundFilename[256];
	memset (strSoundFilename, 0, 256);
	SAFE_STRCPY(strSoundFilename, "Sounds\\Voice\\");
	ltoa ((long)nStringID, &strSoundFilename[13], 10);
	strcat (strSoundFilename, ".wav");

	PlaySoundInfo playSoundInfo;
	PLAYSOUNDINFO_INIT( playSoundInfo );

	playSoundInfo.m_dwFlags = PLAYSOUND_LOCAL | PLAYSOUND_FILESTREAM | PLAYSOUND_GETHANDLE;
	strncpy(playSoundInfo.m_szSoundName, strSoundFilename, _MAX_PATH);
	playSoundInfo.m_nPriority = SOUNDPRIORITY_MISC_HIGH;
	pClientDE->PlaySound(&playSoundInfo);
	m_hTransmissionSound = playSoundInfo.m_hSound;

	// Make sure the sound played.
	if( !m_hTransmissionSound )
	{
		// Tell server the transmission ended.
		hMsg = pClientDE->StartMessage( MID_TRANSMISSIONENDED );
		pClientDE->EndMessage( hMsg );
	}

	m_fTransmissionTimeLeft = 5.0f;
	m_bAnimatingTransmissionOn = DTRUE;
	m_bAnimatingTransmissionOff = DFALSE;

	DDWORD nWidth = 0;
	DDWORD nHeight = 0;

	pClientDE->GetSurfaceDims (m_hTransmissionImage, &nWidth, &nHeight);
	m_xTransmissionImage = -(DFLOAT)nWidth;
	m_yTransmissionImage = 6;
	m_cxTransmissionImage = (DFLOAT)nWidth;
	m_cyTransmissionImage = (DFLOAT)nHeight;
	
	pClientDE->GetSurfaceDims (m_hTransmissionText, &nWidth, &nHeight);
	m_xTransmissionText = 80;
	m_yTransmissionText = -(DFLOAT)nHeight;
	m_cxTransmissionText = (DFLOAT)nWidth;
	m_cyTransmissionText = (DFLOAT)nHeight;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::PlayIntroMovies()
//
//	PURPOSE:	Plays the intro movies in the correct order
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::PlayIntroMovies (CClientDE* pClientDE)
{
	if (!pClientDE) return;

	static int nMovie = 0;
	
	DRESULT nResult = DE_ERROR;
	DDWORD nFlags = PLAYBACK_FULLSCREEN;

	// Changed to look in current directory if the movie path is invalid...

	if (/*m_strMoviesDir[0] == '\0' ||*/ AdvancedDisableMovies())
	{
		nMovie = 2;	// fall through into default handler below
	}

#ifdef _DEMO
	nMovie = 2;
#endif

	switch (nMovie)
	{
		case 0:
		{
			nMovie++;

			// see if the movie exists
			char strPath[256];
			SAFE_STRCPY(strPath, m_strMoviesDir);
			strcat (strPath, "Logo.smk");
			if (CWinUtil::FileExist (strPath))
			{
				// attempt to play the movie
				nResult = pClientDE->StartVideo (strPath, nFlags);
			}

			if (nResult != DE_OK)
			{
				PlayIntroMovies (pClientDE);
				return;
			}
			
			m_nGameState = GS_MOVIES;
			return;
		}
		break;

		case 1:
		{
			nMovie++;

			// see if the movie exists
			char strPath[256];
			SAFE_STRCPY(strPath, m_strMoviesDir);
			strcat (strPath, "Intro.smk");
			if (CWinUtil::FileExist (strPath))
			{
				// attempt to play the movie
				nResult = pClientDE->StartVideo (strPath, nFlags);
			}
			
			if (nResult != DE_OK)
			{
				PlayIntroMovies (pClientDE);
				return;
			}
			
			m_nGameState = GS_MOVIES;
			return;
		}
		break;
		
		default:
		{
			SetMenuMode (DTRUE, DFALSE);
			return;
		}
		break;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::PauseGame()
//
//	PURPOSE:	Pauses/Unpauses the server
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::PauseGame (DBOOL bPause, DBOOL bPauseSound)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	m_bGamePaused = bPause;

	if (!IsMultiplayerGame())
	{
		HMESSAGEWRITE hMessage = pClientDE->StartMessage(bPause ? MID_GAME_PAUSE : MID_GAME_UNPAUSE);
		pClientDE->EndMessage(hMessage);
	}

	if (bPause && bPauseSound)
	{
		pClientDE->PauseSounds();
	}
	else
	{
		pClientDE->ResumeSounds();
	}

	SetInputState (!bPause);
	SetMouseInput (!bPause);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::SetInputState()
//
//	PURPOSE:	Allows/disallows input
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::SetInputState(DBOOL bAllowInput)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	pClientDE->SetInputState(bAllowInput);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::SetMouseInput()
//
//	PURPOSE:	Allows or disallows mouse input on the client
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::SetMouseInput (DBOOL bAllowInput)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if (bAllowInput)
	{
		m_bRestoreOrientation = DTRUE;
	}
	else
	{
		m_fYawBackup = m_fYaw;
		m_fPitchBackup = m_fPitch;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandlePlayerStateChange()
//
//	PURPOSE:	Update player state change
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::HandlePlayerStateChange(HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
	
	m_ePlayerState = (PlayerState) pClientDE->ReadFromMessageByte(hMessage);

	if (m_ePlayerState == PS_DYING)
	{
		ClearScreenTint();

		if (IsMultiplayerGame())
		{
			m_fEarliestRespawnTime = pClientDE->GetTime() + MULTIPLAYER_RESPAWN_WAIT_TIME;
		}
		else
		{
			m_fEarliestRespawnTime = pClientDE->GetTime() + RESPAWN_WAIT_TIME;
		}

		HLOCALOBJ hWeapon = m_weaponModel.GetHandle();
		if (hWeapon)
		{
			DDWORD dwFlags = pClientDE->GetObjectFlags(hWeapon);
			pClientDE->SetObjectFlags(hWeapon, dwFlags & ~FLAG_VISIBLE);
		}

		m_weaponModel.Reset();
		
		m_bDrawInterface = DFALSE;
		AddToClearScreenCount();
	
		if (m_playerCamera.IsFirstPerson())
		{
			SetExternalCamera(DTRUE);
		}

		HSTRING hStr = pClientDE->FormatString (IDS_YOUWEREKILLED);
		pClientDE->CPrint(pClientDE->GetStringData (hStr));

		m_infoDisplay.AddInfo(pClientDE->GetStringData (hStr), m_menu.GetFont12s(), 5.0f, DI_CENTER | DI_TOP);

		pClientDE->FreeString(hStr);
	}
	else if (m_ePlayerState == PS_ALIVE)
	{
		if (m_nPlayerMode == PM_MODE_KID)
		{
			m_bDrawInterface = DFALSE;
		}
		else
		{
			m_bDrawInterface = DTRUE;
		}

		m_bDrawHud = DTRUE;

		SetExternalCamera(DFALSE);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::AutoSave()
//
//	PURPOSE:	Autosave the game
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::AutoSave(HMESSAGEREAD hMessage)
{
	if (m_ePlayerState != PS_ALIVE || IsMultiplayerGame()) return;

	// Save the game for reloading if the player just changed levels
	// or if the player just started a game...

	CWinUtil::WinWritePrivateProfileString("Shogo", "Reload", m_strCurrentWorldName, SAVEGAMEINI_FILENAME);
	SaveGame(RELOADLEVEL_FILENAME);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandlePlayerDamage
//
//	PURPOSE:	Handle the player getting damaged
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::HandlePlayerDamage(HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera || !hMessage) return;

	DVector vDir;
	pClientDE->ReadFromMessageVector(hMessage, &vDir);

	DFLOAT fPercent = VEC_MAG(vDir);

	DFLOAT fRed = 0.5f + fPercent;
	fRed = fRed > 1.0f ? 1.0f : fRed;

	DFLOAT fRampDown = 0.5f + fPercent * 2.0f;

	DVector vTintColor;
	VEC_SET(vTintColor, fRed, 0.0f, 0.0f);
	DFLOAT fRampUp = 0.2f, fTintTime = 0.1f;
	
	DVector vCamPos;
	pClientDE->GetObjectPos(m_hCamera, &vCamPos);

	DRotation rRot;
	pClientDE->GetObjectRotation(m_hCamera, &rRot);

	DVector vU, vR, vF;
	pClientDE->GetRotationVectors(&rRot, &vU, &vR, &vF);

	VEC_MULSCALAR(vF, vF, 10.0f);
	VEC_ADD(vCamPos, vCamPos, vF);

	TintScreen(vTintColor, vCamPos, 1000.0f, fRampUp, fTintTime, fRampDown, DTRUE);


	// Shake the camera based on the direction the damage came from...
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandleServerError()
//
//	PURPOSE:	Handle any error messages sent from the server
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::HandleServerError(HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !hMessage) return;

	DBYTE nError = pClientDE->ReadFromMessageByte(hMessage);
	switch (nError)
	{
		case SERROR_SAVEGAME :
		{
			DoMessageBox(IDS_SAVEGAMEFAILED, TH_ALIGN_CENTER);
		}
		break;

		case SERROR_LOADGAME :
		{
			DoMessageBox(IDS_NOLOADLEVEL, TH_ALIGN_CENTER);
			m_menu.LoadAllSurfaces();
			m_nGameState = GS_MENU;  // Force the menu up
		}
		break;
		
		default : break;
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateWeaponMuzzlePosition()
//
//	PURPOSE:	Update the current weapon muzzle pos
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateWeaponMuzzlePosition()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;
	
	DFLOAT fIncValue = 0.1f;
	DBOOL bChanged = DFALSE;

	DVector vOffset;
	VEC_INIT(vOffset);


	// Move weapon faster if running...

	if (m_dwPlayerFlags &  CS_MFLG_RUN)
	{
		fIncValue = fIncValue * 2.0f;
	}


	vOffset = m_weaponModel.GetMuzzleOffset();


	// Move weapon forward or backwards...

	if ((m_dwPlayerFlags & CS_MFLG_FORWARD) || (m_dwPlayerFlags & CS_MFLG_REVERSE))
	{
		fIncValue = m_dwPlayerFlags & CS_MFLG_FORWARD ? fIncValue : -fIncValue;
		vOffset.z += fIncValue;
		bChanged = DTRUE;
	}


	// Move the weapon to the player's right or left...

	if ((m_dwPlayerFlags & CS_MFLG_STRAFE_RIGHT) || 
		(m_dwPlayerFlags & CS_MFLG_STRAFE_LEFT))
	{
		fIncValue = m_dwPlayerFlags & CS_MFLG_STRAFE_RIGHT ? fIncValue : -fIncValue;
		vOffset.x += fIncValue;
		bChanged = DTRUE;
	}


	// Move the weapon up or down relative to the player...

	if ((m_dwPlayerFlags & CS_MFLG_JUMP) || (m_dwPlayerFlags & CS_MFLG_DOUBLEJUMP) || (m_dwPlayerFlags & CS_MFLG_DUCK))
	{
		fIncValue = m_dwPlayerFlags & CS_MFLG_DUCK ? -fIncValue : fIncValue;
		vOffset.y += fIncValue;
		bChanged = DTRUE;
	}


	// Okay, set the offset...

	m_weaponModel.SetMuzzleOffset(vOffset);

	if (bChanged)
	{
		CSPrint ("Muzzle offset = %f, %f, %f", vOffset.x, vOffset.y, vOffset.z);
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateWeaponPosition()
//
//	PURPOSE:	Update the position of the current weapon
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateWeaponPosition()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;
	
	DFLOAT fIncValue = 0.1f;
	DBOOL bChanged = DFALSE;

	DVector vOffset;
	VEC_INIT(vOffset);


	// Move weapon faster if running...

	if (m_dwPlayerFlags &  CS_MFLG_RUN)
	{
		fIncValue = fIncValue * 2.0f;
	}


	vOffset = m_weaponModel.GetOffset();


	// Move weapon forward or backwards...

	if ((m_dwPlayerFlags & CS_MFLG_FORWARD) || (m_dwPlayerFlags & CS_MFLG_REVERSE))
	{
		fIncValue = m_dwPlayerFlags & CS_MFLG_FORWARD ? fIncValue : -fIncValue;
		vOffset.z += fIncValue;
		bChanged = DTRUE;
	}


	// Move the weapon to the player's right or left...

	if ((m_dwPlayerFlags & CS_MFLG_STRAFE_RIGHT) || 
		(m_dwPlayerFlags & CS_MFLG_STRAFE_LEFT))
	{
		fIncValue = m_dwPlayerFlags & CS_MFLG_STRAFE_RIGHT ? fIncValue : -fIncValue;
		vOffset.x += fIncValue;
		bChanged = DTRUE;
	}


	// Move the weapon up or down relative to the player...

	if ((m_dwPlayerFlags & CS_MFLG_JUMP) || (m_dwPlayerFlags & CS_MFLG_DOUBLEJUMP) || (m_dwPlayerFlags & CS_MFLG_DUCK))
	{
		fIncValue = m_dwPlayerFlags & CS_MFLG_DUCK ? -fIncValue : fIncValue;
		vOffset.y += fIncValue;
		bChanged = DTRUE;
	}


	// Okay, set the offset...

	m_weaponModel.SetOffset(vOffset);

	if (bChanged)
	{
		CSPrint ("Weapon offset = %f, %f, %f", vOffset.x, vOffset.y, vOffset.z);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::AdjustLightScale()
//
//	PURPOSE:	Update the current global light scale
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::AdjustLightScale()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	DFLOAT fIncValue = 0.01f;
	DBOOL bChanged = DFALSE;

	DVector vScale;
	VEC_INIT(vScale);

	pClientDE->GetGlobalLightScale(&vScale);

	// Move faster if running...

	if (m_dwPlayerFlags & CS_MFLG_RUN)
	{
		fIncValue = .5f;
	}


	// Move Red up/down...

	if ((m_dwPlayerFlags & CS_MFLG_FORWARD) || (m_dwPlayerFlags & CS_MFLG_REVERSE))
	{
		fIncValue = m_dwPlayerFlags & CS_MFLG_FORWARD ? fIncValue : -fIncValue;
		vScale.x += fIncValue;
		vScale.x = vScale.x < 0.0f ? 0.0f : (vScale.x > 1.0f ? 1.0f : vScale.x);

		bChanged = DTRUE;
	}


	// Move Green up/down...

	if ((m_dwPlayerFlags & CS_MFLG_STRAFE_RIGHT) || 
		(m_dwPlayerFlags & CS_MFLG_STRAFE_LEFT))
	{
		fIncValue = m_dwPlayerFlags & CS_MFLG_STRAFE_RIGHT ? fIncValue : -fIncValue;
		vScale.y += fIncValue;
		vScale.y = vScale.y < 0.0f ? 0.0f : (vScale.y > 1.0f ? 1.0f : vScale.y);

		bChanged = DTRUE;
	}


	// Move Blue up/down...

	if ((m_dwPlayerFlags & CS_MFLG_JUMP) || (m_dwPlayerFlags & CS_MFLG_DOUBLEJUMP) || (m_dwPlayerFlags & CS_MFLG_DUCK))
	{
		fIncValue = m_dwPlayerFlags & CS_MFLG_DUCK ? -fIncValue : fIncValue;
		vScale.z += fIncValue;
		vScale.z = vScale.z < 0.0f ? 0.0f : (vScale.z > 1.0f ? 1.0f : vScale.z);

		bChanged = DTRUE;
	}


	// Okay, set the light scale.

	pClientDE->SetGlobalLightScale(&vScale);

	if (bChanged)
	{
		CSPrint ("Light Scale = %f, %f, %f", vScale.x, vScale.y, vScale.z);
	}

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::SpecialEffectNotify()
//
//	PURPOSE:	Handle creation of a special fx
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::SpecialEffectNotify(HLOCALOBJ hObj, HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if (hObj)
	{
		pClientDE->SetObjectClientFlags(hObj, CF_NOTIFYREMOVE);
	}

	m_sfxMgr.HandleSFXMsg(hObj, hMessage);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnObjectRemove()
//
//	PURPOSE:	Handle removal of an object...
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::OnObjectRemove(HLOCALOBJ hObj)
{
	if (!hObj) return;

	m_sfxMgr.RemoveSpecialFX(hObj);
	
	if(m_MoveMgr)
	{
		m_MoveMgr->OnObjectRemove(hObj);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandleExitLevel()
//
//	PURPOSE:	Update player state change
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::HandleExitLevel(HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
	
	// Get the new world

	HSTRING	hstrWorld		 = pClientDE->ReadFromMessageHString(hMessage);
	HSTRING hstrBumperScreen = pClientDE->ReadFromMessageHString(hMessage);
	DDWORD  nBumperTextID	 = pClientDE->ReadFromMessageDWord(hMessage);
	DBOOL	bEndOfScenario	 = (DBOOL)pClientDE->ReadFromMessageByte(hMessage);

	if (!hstrWorld || !hstrBumperScreen)
	{
		PrintError("Invalid data in CRiotClientShell::HandleExitLevel()!");
		return;
	}

	pClientDE->ClearInput(); // Start next level with a clean slate

	char* pStr = pClientDE->GetStringData(hstrBumperScreen);

	TurnOffAlternativeCamera();

	CreateBumperScreen(pStr, nBumperTextID);
	StartNewWorld(hstrWorld);

	m_bInWorld = DFALSE;

	pClientDE->FreeString(hstrWorld);
	pClientDE->FreeString(hstrBumperScreen);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::CreateBumperScreen()
//
//	PURPOSE:	Create the bumper screen image
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::CreateBumperScreen(char* pPCXName, DDWORD dwBumperTextID)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !pPCXName) return;

	// Make sure we remove any old bumper screen stuff...

	RemoveBumperScreen();
	m_bLoadingWorld = DTRUE;


	// Get the screen width and height...

	if (dwBumperTextID)
	{
		DDWORD nWidth, nHeight;
		HSURFACE hScreen = pClientDE->GetScreenSurface();
		pClientDE->GetSurfaceDims(hScreen, &nWidth, &nHeight);
	
		if (nWidth < 640)
		{
			m_hBumperText = CTextHelper::CreateWrappedStringSurface(pClientDE, (min ((int)nWidth - 20, 600)), m_menu.GetFont08s(), dwBumperTextID);
		}
		else
		{
			m_hBumperText = CTextHelper::CreateWrappedStringSurface(pClientDE, (min ((int)nWidth - 40, 600)), m_menu.GetFont12s(), dwBumperTextID);
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::RemoveBumperScreen()
//
//	PURPOSE:	Remove the bumper screen image / text
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::RemoveBumperScreen()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if (m_hBumperText) 
	{
		pClientDE->DeleteSurface (m_hBumperText);
		m_hBumperText = DNULL;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::StartNewWorld()
//
//	PURPOSE:	Start the new world
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::StartNewWorld(HSTRING hstrWorld)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !hstrWorld) return;

	char* pWorld = pClientDE->GetStringData(hstrWorld);
	if (!pWorld)
	{
		PrintError("Error invalid world!");
		return;
	}


	// Load the new level...

	char newWorld[100];
	sprintf(newWorld, "worlds\\%s", pWorld);

	if (!LoadWorld(newWorld, DNULL, DNULL, LOAD_NEW_LEVEL))
	{
		SetMenuMode (DTRUE, DFALSE);
		DoMessageBox (IDS_NOLOADLEVEL, TH_ALIGN_CENTER);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateLoadingLevel()
//
//	PURPOSE:	Update the level loading screen
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateLoadingLevel()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || (!m_hBumperText && !m_hLoading)) return;

	// Start the loading level music...

	SetMenuMusic(DTRUE);


	HSURFACE hScreen = pClientDE->GetScreenSurface();
	DDWORD nScreenWidth = 0;
	DDWORD nScreenHeight = 0;
	pClientDE->GetSurfaceDims (hScreen, &nScreenWidth, &nScreenHeight);

	DDWORD nWidth = 0;
	DDWORD nHeight = 0;
	
	if (m_hBumperText)
	{
		if (m_hBumperText)
		{
			pClientDE->GetSurfaceDims(m_hBumperText, &nWidth, &nHeight);
			pClientDE->DrawSurfaceToSurfaceTransparent(hScreen, m_hBumperText, DNULL, ((int)(nScreenWidth - nWidth)) / 2, ((int)nScreenHeight - (int)nHeight) / 2, DNULL);
		}

		if (m_bLoadingWorld)
		{
			if (m_hWorldName)
			{
				pClientDE->GetSurfaceDims(m_hWorldName, &nWidth, &nHeight);
				pClientDE->DrawSurfaceToSurfaceTransparent(hScreen, m_hWorldName, DNULL, ((int)(nScreenWidth - nWidth)) / 2, (int)m_cyPressAnyKey, DNULL);
			}

			if (m_hLoadingWorld)
			{
				pClientDE->GetSurfaceDims(m_hLoadingWorld, &nWidth, &nHeight);
				pClientDE->DrawSurfaceToSurfaceTransparent(hScreen, m_hLoadingWorld, DNULL, ((int)(nScreenWidth - nWidth)) / 2, (int)nScreenHeight - (int)m_cyPressAnyKey, DNULL);
			}
		}
	}
	else if (m_hLoading)
	{
		pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hLoading, DNULL, ((int)(nScreenWidth - m_cxLoading)) / 2, ((int)(nScreenHeight - m_cyLoading)) / 2, DNULL);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::PrintError()
//
//	PURPOSE:	Display an error to the end-user
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::PrintError(char* pError)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !pError) return;

	CSPrint (pError);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::SetSpectatorMode()
//
//	PURPOSE:	Turn spectator mode on/off
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::SetSpectatorMode(DBOOL bOn)
{
	m_bSpectatorMode = bOn;

	if (m_playerCamera.IsFirstPerson())
	{
		m_weaponModel.SetVisible(!m_bSpectatorMode);
		ShowPlayer(DFALSE);
		
		if(m_MoveMgr)
			m_MoveMgr->SetSpectatorMode(bOn);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::LoadWorld()
//
//	PURPOSE:	Handles loading a world (with AutoSave)
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::LoadWorld(char* pWorldFile, char* pCurWorldSaveFile,
								  char* pRestoreObjectsFile, DBYTE nFlags)
{
	// Auto save the newly loaded level...

	return DoLoadWorld(pWorldFile, pCurWorldSaveFile, pRestoreObjectsFile, nFlags);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::DoLoadWorld()
//
//	PURPOSE:	Does actual work of loading a world
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::DoLoadWorld(char* pWorldFile, char* pCurWorldSaveFile,
								    char* pRestoreObjectsFile, DBYTE nFlags, 
									char *pRecordFile, char *pPlaydemoFile)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !pWorldFile) return DFALSE;
	

	// Make sure the FOV is set correctly...

	if (m_hCamera)
	{
		DDWORD dwWidth = 640, dwHeight = 480;
		pClientDE->GetSurfaceDims(pClientDE->GetScreenSurface(), &dwWidth, &dwHeight);

		pClientDE->SetCameraRect(m_hCamera, DFALSE, 0, 0, dwWidth, dwHeight);
		
		m_fCurrentFovX = DEG2RAD(FOV_NORMAL);

		DFLOAT y = (m_fCurrentFovX * dwHeight) / dwWidth;
		pClientDE->SetCameraFOV(m_hCamera, m_fCurrentFovX, y);
	}
	

	// See if the game is over...

	if (pWorldFile && (strcmpi(pWorldFile, "end") == 0 || strcmpi(pWorldFile, "worlds\\end") == 0))
	{
		m_bGameOver = DTRUE;
	}
	else
	{
		m_bGameOver = DFALSE;
	}


	// Set us to the loading state...

	m_nGameState = GS_LOADINGLEVEL;

	
	// Create the "loading" surface...

	if (m_hLoadingWorld)
	{
		pClientDE->DeleteSurface (m_hLoadingWorld);
		m_hLoadingWorld = DNULL;
	}

	if (m_hWorldName)
	{
		pClientDE->DeleteSurface (m_hWorldName);
		m_hWorldName = DNULL;
	}

	char worldname[51];
	worldname[0] = '\0';
	GetNiceWorldName(pWorldFile, worldname, 50);

	if (worldname[0])
	{
		m_hWorldName = CTextHelper::CreateSurfaceFromString(pClientDE, m_menu.GetFont12s(), worldname);
	}

	m_hLoadingWorld = CTextHelper::CreateSurfaceFromString(pClientDE, m_menu.GetFont12s(), IDS_BUMPER_LOADING);
	
	
	// Bring up the loading screen...

	pClientDE->ClearScreen (DNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER);
	pClientDE->Start3D();

	CreateMenuPolygrid();
	UpdateMenuPolygrid();

	pClientDE->StartOptimized2D();
	UpdateLoadingLevel();
	pClientDE->EndOptimized2D();

	pClientDE->End3D();
	pClientDE->FlipScreen (FLIPSCREEN_CANDRAWCONSOLE);

	
	// Check for special case of not being connected to a server or going to 
	// single player mode from multiplayer...

	int nGameMode = 0;
	pClientDE->GetGameMode(&nGameMode);
	if (pPlaydemoFile || 
		!pClientDE->IsConnected() || (nGameMode != STARTGAME_NORMAL && nGameMode != GAMEMODE_NONE))
	{
		StartGameRequest request;
		memset(&request, 0, sizeof(StartGameRequest));
	
		NetStart_ClearGameStruct();  // Start with clean slate
		request.m_pGameInfo   = NetStart_GetGameStruct();
		request.m_GameInfoLen = sizeof(NetGame_t);
		request.m_Type = STARTGAME_NORMAL;

		if(pRecordFile)
		{
			SAFE_STRCPY(request.m_RecordFilename, pRecordFile);
			SAFE_STRCPY(request.m_WorldName, pWorldFile);
		}

		if(pPlaydemoFile)
		{
			SAFE_STRCPY(request.m_PlaybackFilename, pPlaydemoFile);
		}

		DRESULT dr = pClientDE->StartGame(&request);
		if (dr != LT_OK)
		{
			return DFALSE;
		}

		if(pPlaydemoFile)
		{
			// If StartGameRequest::m_PlaybackFilename is filled in the engine fills in m_WorldName.
			pWorldFile = request.m_WorldName;
		}
	}

	// Since we're loading a game, reset the music level to silence...
	// (m_eMusicLevel is used in BuildClientSaveMsg())...

	m_eMusicLevel = CMusic::MUSICLEVEL_SILENCE;

	
	// Get rid of any mission objectives we currently have

	m_objectives.Reset();
	
	// Send a message to the server shell with the needed info...

	HSTRING hWorldFile			= pClientDE->CreateString(pWorldFile);
	HSTRING hCurWorldSaveFile	= pClientDE->CreateString(pCurWorldSaveFile ? pCurWorldSaveFile : " ");
	HSTRING hRestoreObjectsFile	= pClientDE->CreateString(pRestoreObjectsFile ? pRestoreObjectsFile : " ");

	HMESSAGEWRITE hMessage = pClientDE->StartMessage(MID_LOAD_GAME);
	pClientDE->WriteToMessageByte(hMessage, nFlags);
	pClientDE->WriteToMessageByte(hMessage, m_eDifficulty);
	pClientDE->WriteToMessageHString(hMessage, hWorldFile);
	pClientDE->WriteToMessageHString(hMessage, hCurWorldSaveFile);
	pClientDE->WriteToMessageHString(hMessage, hRestoreObjectsFile);

	BuildClientSaveMsg(hMessage);

	pClientDE->EndMessage(hMessage);

	pClientDE->FreeString(hWorldFile);
	pClientDE->FreeString(hCurWorldSaveFile);
	pClientDE->FreeString(hRestoreObjectsFile);


	return DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::LoadGame()
//
//	PURPOSE:	Handles loading a saved game
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::LoadGame(char* pWorld, char* pObjectsFile)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !pWorld || !pObjectsFile) return DFALSE;
		
	char fullWorldPath[100];
	sprintf(fullWorldPath,"worlds\\%s", pWorld);

	return DoLoadWorld(fullWorldPath, DNULL, pObjectsFile, LOAD_RESTORE_GAME);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::SaveGame()
//
//	PURPOSE:	Handles saving a game...
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::SaveGame(char* pObjectsFile)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !pObjectsFile || IsMultiplayerGame()) return DFALSE;
	
	DBYTE nFlags = 0;

	// Since we're saving the game, save the music level...
	// (m_eMusicLevel is used in BuildClientSaveMsg())...

	m_eMusicLevel = m_Music.GetMusicLevel();

	
	// Save the level objects...

	HSTRING hSaveObjectsName = pClientDE->CreateString(pObjectsFile);

	HMESSAGEWRITE hMessage = pClientDE->StartMessage(MID_SAVE_GAME);
	pClientDE->WriteToMessageByte(hMessage, nFlags);
	pClientDE->WriteToMessageHString(hMessage, hSaveObjectsName);

	BuildClientSaveMsg(hMessage);

	pClientDE->EndMessage(hMessage);

	pClientDE->FreeString(hSaveObjectsName);

	return DTRUE;
}


// ----------------------------------------------------------------------- //
// Clears the screen a few times so the backbuffer(s) get cleared.
// ----------------------------------------------------------------------- //
void CRiotClientShell::ClearAllScreenBuffers()
{
	ClientDE *pClientDE = GetClientDE();
	int i;

	for(i=0; i < 4; i++)
	{
		pClientDE->ClearScreen(DNULL, CLEARSCREEN_SCREEN);
		pClientDE->FlipScreen(0);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::SetMenuMode()
//
//	PURPOSE:	Turns menu mode on and off
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::SetMenuMode (DBOOL bMenuUp, DBOOL bLoadingLevel)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return DFALSE;

	DDWORD nWidth, nHeight;
	pClientDE->GetSurfaceDims (pClientDE->GetScreenSurface(), &nWidth, &nHeight);

	if (bMenuUp && m_nGameState != GS_MENU)
	{
		if (m_bUsingExternalCamera)
		{
			TurnOffAlternativeCamera();
			m_bCameraPosInited = DFALSE;
		}	

		// Make sure menus are full screen...

		memset (&m_rcMenuRestoreCamera, 0, sizeof (DRect));
		if (m_hCamera && !m_bMovieCameraRect)
		{
			pClientDE->GetCameraRect (m_hCamera, &m_bMenuRestoreFullScreen, &m_rcMenuRestoreCamera.left, &m_rcMenuRestoreCamera.top, &m_rcMenuRestoreCamera.right, &m_rcMenuRestoreCamera.bottom);
			pClientDE->SetCameraRect (m_hCamera, DTRUE, 0, 0, (int)nWidth, (int)nHeight);
		}
		
		m_nGameState = GS_MENU;
		PauseGame (DTRUE, DTRUE);
		SetMenuMusic (DTRUE);
		ClearScreenAlways();
		
		m_menu.LoadAllSurfaces();
	}
	else if (!bMenuUp)
	{
		if (m_hCamera && (m_rcMenuRestoreCamera.left != 0 || m_rcMenuRestoreCamera.top != 0 || m_rcMenuRestoreCamera.right != 0 || m_rcMenuRestoreCamera.bottom != 0))
		{
			pClientDE->SetCameraRect (m_hCamera, m_bMenuRestoreFullScreen, m_rcMenuRestoreCamera.left, m_rcMenuRestoreCamera.top, m_rcMenuRestoreCamera.right, m_rcMenuRestoreCamera.bottom);
		}

		int nGameMode = GAMEMODE_NONE;
		pClientDE->GetGameMode (&nGameMode);
		if (nGameMode == GAMEMODE_NONE) return DFALSE;

		ClearScreenAlways (DFALSE);
		AddToClearScreenCount();
		
		if (!bLoadingLevel)
		{
			m_nGameState = GS_PLAYING;
			PauseGame (DFALSE);
			SetMenuMusic (DFALSE);
		}
		
		m_menu.UnloadAllSurfaces();
	}

	return DTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::SetLoadGameMenu()
//
//	PURPOSE:	Turn the load game menu on
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::SetLoadGameMenu()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	SetMenuMode(DTRUE);

	// Make sure we're on the load menu...

	CMainMenu* pMain = m_menu.GetMainMenu();
	if (pMain)
	{
		CSinglePlayerMenu* pSingle = pMain->GetSinglePlayerMenu();
		if (pSingle)
		{
			CLoadSavedLevelMenu* pLoad = pSingle->GetLoadSavedLevelMenu();
			if (pLoad)
			{
				m_menu.SetCurrentMenu(pLoad);
			}
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::SetMenuMusic()
//
//	PURPOSE:	Turns menu / load level music on or off
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::SetMenuMusic(DBOOL bMusicOn)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return DFALSE;
	
	if (bMusicOn)
	{
		if (!m_bGameMusicPaused)
		{
			pClientDE->PauseMusic();
			m_bGameMusicPaused = DTRUE;
		}

		if (!m_hMenuMusic)
		{
			// Set up the music...

			PlaySoundInfo psi;
			PLAYSOUNDINFO_INIT (psi);
			psi.m_dwFlags = PLAYSOUND_LOCAL | PLAYSOUND_LOOP | PLAYSOUND_GETHANDLE | PLAYSOUND_CLIENT | PLAYSOUND_FILESTREAM;

			char* s_pLoadingMusic[] = 
			{
				"Sounds\\Interface\\loop1.wav",
				"Sounds\\Interface\\loop2.wav",
				"Sounds\\Interface\\loop3.wav",
				"Sounds\\Interface\\loop4.wav",
				"Sounds\\Interface\\loop5.wav",
				"Sounds\\Interface\\loop6.wav"
			};

#ifdef _DEMO
			char* pMusic = s_pLoadingMusic[0];
#else
			char* pMusic = s_pLoadingMusic[GetRandom(0, 5)];
#endif
			SAFE_STRCPY(psi.m_szSoundName, pMusic);

			psi.m_nPriority  = SOUNDPRIORITY_MISC_HIGH;

			pClientDE->PlaySound (&psi);
			m_hMenuMusic = psi.m_hSound;
		}

		return !!m_hMenuMusic;
	}
	else
	{
		if (m_bGameMusicPaused)
		{
			pClientDE->ResumeMusic();
			m_bGameMusicPaused = DFALSE;
		}

		if (m_hMenuMusic) 
		{
			pClientDE->KillSound(m_hMenuMusic);
			m_hMenuMusic = DNULL;
		}
	}

	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::DoMessageBox()
//
//	PURPOSE:	Displays a message box to the user in the center of the
//				screen
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::DoMessageBox (int nStringID, int nAlignment, DBOOL bCrop)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return DFALSE;

	if (m_pMessageBox) return DFALSE;

	m_pMessageBox = new CMessageBox;
	if (!m_pMessageBox->Init (pClientDE, nStringID, DFALSE, nAlignment, bCrop))
	{
		delete m_pMessageBox;
		m_pMessageBox = DNULL;
		return DFALSE;
	}

	if (m_nGameState != GS_MENU)
	{
		PauseGame(DTRUE, DTRUE);
	}
	m_bClearScreenAlways = DFALSE;
	ZeroClearScreenCount();
	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::DoYesNoMessageBox()
//
//	PURPOSE:	Displays a message box to the user in the center of the
//				screen
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::DoYesNoMessageBox (int nStringID, YESNOPROC pYesNoProc, DDWORD nUserData, int nAlignment, DBOOL bCrop)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return DFALSE;

	if (m_pMessageBox) return DFALSE;

	m_pMessageBox = new CMessageBox;
	if (!m_pMessageBox->Init (pClientDE, nStringID, DTRUE, nAlignment, bCrop))
	{
		delete m_pMessageBox;
		m_pMessageBox = DNULL;
		return DFALSE;
	}

	m_pYesNoProc = pYesNoProc;
	m_nYesNoUserData = nUserData;

	if (m_nGameState != GS_MENU)
	{
		PauseGame(DTRUE, DTRUE);
	}
	m_bClearScreenAlways = DFALSE;
	ZeroClearScreenCount();
	return DTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::IsJoystickEnabled()
//
//	PURPOSE:	Determines whether or not there is a joystick device 
//				enabled
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::IsJoystickEnabled()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return DFALSE;

	// first attempt to find a joystick device

	char strJoystick[128];
	memset (strJoystick, 0, 128);
	DRESULT result = pClientDE->GetDeviceName (DEVICETYPE_JOYSTICK, strJoystick, 127);
	if (result != LT_OK) return DFALSE;

	// ok - we found the device and have a name...see if it's enabled

	DBOOL bEnabled = DFALSE;
	pClientDE->IsDeviceEnabled (strJoystick, &bEnabled);

	return bEnabled;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::EnableJoystick()
//
//	PURPOSE:	Attempts to find and enable a joystick device
//
// ----------------------------------------------------------------------- //

DBOOL CRiotClientShell::EnableJoystick()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return DFALSE;

	// first attempt to find a joystick device

	char strJoystick[128];
	memset (strJoystick, 0, 128);
	DRESULT result = pClientDE->GetDeviceName (DEVICETYPE_JOYSTICK, strJoystick, 127);
	if (result != LT_OK) return DFALSE;

	// ok, now try to enable the device

	char strConsole[256];
	sprintf (strConsole, "EnableDevice \"%s\"", strJoystick);
	pClientDE->RunConsoleString (strConsole);

	DBOOL bEnabled = DFALSE;
	pClientDE->IsDeviceEnabled (strJoystick, &bEnabled);

	return bEnabled;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateDebugInfo()
//
//	PURPOSE:	Update debugging info.
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdateDebugInfo()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hCamera) return;

	if (m_hDebugInfo)
	{
		pClientDE->DeleteSurface(m_hDebugInfo);
		m_hDebugInfo = NULL;
	}

	char buf[100];
	buf[0] = '\0';
	
	// Check to see if we should show the player position...

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	if (m_bShowPlayerPos && hPlayerObj)
	{
		DVector vPos;
		pClientDE->GetObjectPos(hPlayerObj, &vPos);

		sprintf(buf, "Pos (%.0f, %.0f, %.0f)", vPos.x, vPos.y, vPos.z);
	}

	if (buf[0])
	{
		CreateDebugSurface(buf);
	}


	HCONSOLEVAR hVar = pClientDE->GetConsoleVar("PlayerDims");
	if (hVar)
	{
		if (pClientDE->GetVarValueFloat(hVar) > 0.0f)
		{
			CreateBoundingBox();
			UpdateBoundingBox();
		}
		else if (m_hBoundingBox)
		{
			pClientDE->DeleteObject(m_hBoundingBox);
			m_hBoundingBox = DNULL;
		}
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::CreateDebugSurface
//
//	PURPOSE:	Create a surface with debug info on it.
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::CreateDebugSurface(char* strMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !strMessage || strMessage[0] == '\0') return;

	HSTRING hstrFont = pClientDE->FormatString (IDS_INGAMEFONT);
	HDEFONT hFont = pClientDE->CreateFont (pClientDE->GetStringData(hstrFont), 9, 18, DFALSE, DFALSE, DFALSE);
	pClientDE->FreeString (hstrFont);

	HDECOLOR hFColor = pClientDE->SetupColor1(1.0f, 1.0f, 1.0f, DFALSE);
	HDECOLOR hTrans  = pClientDE->SetupColor2 (0.0f, 0.0f, 0.0f, DTRUE);
	m_hDebugInfo     = CTextHelper::CreateSurfaceFromString(pClientDE, hFont, strMessage, hFColor);
	pClientDE->OptimizeSurface (m_hDebugInfo, hTrans);

	pClientDE->DeleteFont(hFont);

	DDWORD cx, cy;
	pClientDE->GetSurfaceDims(m_hDebugInfo, &cx, &cy);
	m_rcDebugInfo.left   = 0;
	m_rcDebugInfo.top    = 0;
	m_rcDebugInfo.right  = (int)cx;
	m_rcDebugInfo.bottom = (int)cy;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ToggleDebugCheat
//
//	PURPOSE:	Handle debug cheat toggles
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::ToggleDebugCheat(CheatCode eCheat)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE ) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;
	
	switch (eCheat)
	{
		case CHEAT_POSWEAPON_MUZZLE :
		{
			if (!m_bSpectatorMode)
			{
				m_bTweakingWeaponMuzzle = !m_bTweakingWeaponMuzzle;
				m_bAllowPlayerMovement  = !m_bTweakingWeaponMuzzle;
			}
		}
		break;

		case CHEAT_POSWEAPON :
		{
			if (!m_bSpectatorMode)
			{
				m_bTweakingWeapon		= !m_bTweakingWeapon;
				m_bAllowPlayerMovement	= !m_bTweakingWeapon;
			}
		}
		break;

		case CHEAT_LIGHTSCALE :
		{
			m_bAdjustLightScale = !m_bAdjustLightScale;

			SetInputState(!m_bAdjustLightScale);
		}
		break;

		default : break;
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::FirstUpdate
//
//	PURPOSE:	Handle first update (each level)
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::FirstUpdate()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_bFirstUpdate) return;

	char buf[200];
	m_bFirstUpdate = DFALSE;


	// Initialize model warble sheeyot...

	pClientDE->RunConsoleString("+ModelWarble 0");
	pClientDE->RunConsoleString("+WarbleSpeed 15");
	pClientDE->RunConsoleString("+WarbleScale .95");

	
	// Set up the panning sky values

	m_bPanSky = (DBOOL) pClientDE->GetServerConVarValueFloat("PanSky");
	m_fPanSkyOffsetX = pClientDE->GetServerConVarValueFloat("PanSkyOffsetX");
	m_fPanSkyOffsetZ = pClientDE->GetServerConVarValueFloat("PanSkyOffsetX");
	m_fPanSkyScaleX = pClientDE->GetServerConVarValueFloat("PanSkyScaleX");
	m_fPanSkyScaleZ = pClientDE->GetServerConVarValueFloat("PanSkyScaleZ");
	char* pTexture  = pClientDE->GetServerConVarValueString("PanSkyTexture");

	if (m_bPanSky)
	{
		pClientDE->SetGlobalPanTexture(GLOBALPAN_SKYSHADOW, pTexture);
	}


	// Set up the environment map (chrome) texture...

	char* pEnvMap = pClientDE->GetServerConVarValueString("EnvironmentMap");
	if (pEnvMap)
	{
		char* pVal = pEnvMap[0] == '0' ? "Textures\\Chrome.dtx" : pEnvMap;
		sprintf(buf, "EnvMap %s", pVal);
		pClientDE->RunConsoleString(buf);
	}


	// Set up the global (per level) wind values...

	g_vWorldWindVel.x = pClientDE->GetServerConVarValueFloat("WindX");
	g_vWorldWindVel.y = pClientDE->GetServerConVarValueFloat("WindY");
	g_vWorldWindVel.z = pClientDE->GetServerConVarValueFloat("WindZ");


	// Set up the global (per level) light scale values...

	m_vDefaultLightScale.x = pClientDE->GetServerConVarValueFloat("LightScaleR") / 255.0f;
	m_vDefaultLightScale.y = pClientDE->GetServerConVarValueFloat("LightScaleG") / 255.0f;
	m_vDefaultLightScale.z = pClientDE->GetServerConVarValueFloat("LightScaleB") / 255.0f;

	m_LightScaleMgr.SetLightScale (&m_vDefaultLightScale, LightEffectWorld);

	
	// Set up the global (per level) far z value.

	//DFLOAT fVal = pClientDE->GetServerConVarValueFloat("FarZ");

	//if (fVal > 0.0f)
	//{
		// All levels should be vised, so make sure the far z is out there! :)

		sprintf(buf, "+FarZ %d", 100000 /*(int)fVal*/);
		pClientDE->RunConsoleString(buf);
	//}


	// Set up the global (per level) fog values...

	ResetGlobalFog();


	// Initialize the music playlists...

	if (m_Music.IsInitialized())
	{
//		CMusic::EMusicLevel level = m_Music.GetMusicLevel();
		m_Music.TermPlayLists();
		m_Music.InitPlayLists();

		if (!m_Music.UsingIMA())
		{
			m_Music.PlayCDList();
		}
		else
		{
			m_Music.PlayMusicLevel(m_eMusicLevel);
		}
	}

	// Set up the soft renderer sky map...

	char* pSoftSky = pClientDE->GetServerConVarValueString("SoftSky");
	if (pSoftSky)
	{
		sprintf(buf, "SoftSky %s", pSoftSky);
		pClientDE->RunConsoleString(buf);
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ResetGlobalFog
//
//	PURPOSE:	Reset the global fog value...
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::ResetGlobalFog()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// see if fog should be disabled

	if (AdvancedDisableFog())
	{
		pClientDE->RunConsoleString("FogEnable 0");
		return;
	}

	DFLOAT fVal = pClientDE->GetServerConVarValueFloat("EnableFog");
	
	char buf[30];
	DVector todScale;

	sprintf(buf, "FogEnable %d", (int)fVal);
	pClientDE->RunConsoleString(buf);
	
	todScale = m_LightScaleMgr.GetTimeOfDayScale();
	if (fVal)
	{
		fVal = pClientDE->GetServerConVarValueFloat("FogNearZ");
		sprintf(buf, "FogNearZ %d", (int)fVal);
		pClientDE->RunConsoleString(buf);

		fVal = pClientDE->GetServerConVarValueFloat("FogFarZ");
		sprintf(buf, "FogFarZ %d", (int)fVal);
		pClientDE->RunConsoleString(buf);

		fVal = pClientDE->GetServerConVarValueFloat("FogR") * todScale.x;
		sprintf(buf, "FogR %d", (int)fVal);
		pClientDE->RunConsoleString(buf);

		fVal = pClientDE->GetServerConVarValueFloat("FogG") * todScale.y;
		sprintf(buf, "FogG %d", (int)fVal);
		pClientDE->RunConsoleString(buf);

		fVal = pClientDE->GetServerConVarValueFloat("FogB") * todScale.z;
		sprintf(buf, "FogB %d", (int)fVal);
		pClientDE->RunConsoleString(buf);

		fVal = pClientDE->GetServerConVarValueFloat("SkyFog");
		sprintf(buf, "SkyFog %d", (int)fVal);
		pClientDE->RunConsoleString(buf);

		if (fVal)
		{
			fVal = pClientDE->GetServerConVarValueFloat("SkyFogNearZ");
			sprintf(buf, "SkyFogNearZ %d", (int)fVal);
			pClientDE->RunConsoleString(buf);

			fVal = pClientDE->GetServerConVarValueFloat("SkyFogFarZ");
			sprintf(buf, "SkyFogFarZ %d", (int)fVal);
			pClientDE->RunConsoleString(buf);
		}
	}

}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ShowPlayer()
//
//	PURPOSE:	Show/Hide the player object
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::ShowPlayer(DBOOL bShow)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	if (!hPlayerObj) return;

	DDWORD dwFlags = pClientDE->GetObjectFlags(hPlayerObj);
	if (bShow /*&& !(dwFlags & FLAG_VISIBLE)*/)
	{
		dwFlags |= FLAG_VISIBLE;
		pClientDE->SetObjectFlags(hPlayerObj, dwFlags);
	}
	else if (!bShow /*&& (dwFlags & FLAG_VISIBLE)*/)
	{
		dwFlags &= ~FLAG_VISIBLE;
		pClientDE->SetObjectFlags(hPlayerObj, dwFlags);
	}
}


// ----------------------------------------------------------------------- //
// Puts the server's player model where our invisible one is.
// ----------------------------------------------------------------------- //
void CRiotClientShell::UpdateServerPlayerModel()
{
	HOBJECT hClientObj, hRealObj;
	DRotation myRot;
	DVector myPos;
	ClientDE *pClientDE = GetClientDE();
	
	if(!pClientDE || !m_MoveMgr)
		return;
	
	if(!(hClientObj = pClientDE->GetClientObject()))
		return;

	if(!(hRealObj = m_MoveMgr->GetObject()))
		return;

	pClientDE->GetObjectPos(hRealObj, &myPos);
	pClientDE->SetObjectPos(hClientObj, &myPos);

	pClientDE->SetupEuler(&myRot, m_fPitch*0.1f, m_fYaw, m_fCamCant);
	pClientDE->SetObjectRotation(hClientObj, &myRot);	
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::RenderCamera()
//
//	PURPOSE:	Sets up the client and renders the camera
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::RenderCamera (DBOOL bDrawInterface)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_bCameraPosInited) return;
	
	// Make sure the rendered player object is right where it should be.

	UpdateServerPlayerModel();

	// Update anything attached to the player object if it's not going to be rendered.
	if(m_playerCamera.IsFirstPerson())
	{
		pClientDE->ProcessAttachments(pClientDE->GetClientObject());
	}

	// Make sure the weapon is updated before we render the camera...

	UpdateWeaponModel();


	pClientDE->Start3D();
	pClientDE->RenderCamera (m_hCamera);
	if (bDrawInterface) 
	{
		pClientDE->StartOptimized2D();
		DrawInterface();
		pClientDE->EndOptimized2D();
	}
	pClientDE->End3D();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::DrawInterface()
//
//	PURPOSE:	Draws any interface stuff that may need to be drawn
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::DrawInterface()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	// find out if we're in multiplayer
	DBOOL bMultiplayer = IsMultiplayerGame();
	
	// get the screen width and height
	HSURFACE hScreen = pClientDE->GetScreenSurface();
	DDWORD nScreenWidth, nScreenHeight;
	pClientDE->GetSurfaceDims (hScreen, &nScreenWidth, &nScreenHeight);

	// draw any temporary information stuff...
	m_infoDisplay.Draw();

	// draw the player stats (health,armor,ammo) if appropriate
	if (m_bDrawInterface) 
	{
		m_stats.Draw (m_bStatsSizedOff, m_bDrawHud);
	}

	// draw any frag counts (ours only and maybe all) if appropriate
	m_ClientInfo.Draw (m_bDrawInterface, m_bDrawFragCount || (m_ePlayerState == PS_DEAD && bMultiplayer));
	
	// draw any inventory pickup messages, and the ordinance screen if appropriate
	m_inventory.Draw (m_bDrawOrdinance);

	// draw mission log if we need to
	if (m_bDrawMissionLog)
	{
		m_objectives.Draw();
		if (m_bWaitingForMLClosure && !m_objectives.IsClosing())
		{
			m_bWaitingForMLClosure = DFALSE;
			m_bDrawMissionLog = DFALSE;
			PauseGame(DFALSE);
			m_LightScaleMgr.ClearLightScale (&m_vLightScaleObjectives, LightEffectInterface);
		}
	}

	// if we have a new objective that we haven't checked yet, draw the new objective notice
	if (m_bNewObjective && m_hNewObjective && !m_bUsingExternalCamera)
	{
		// draw the notice
		DDWORD nNewObjectiveWidth = 0;
		DDWORD nNewObjectiveHeight = 0;
		pClientDE->GetSurfaceDims (m_hNewObjective, &nNewObjectiveWidth, &nNewObjectiveHeight);

		pClientDE->DrawSurfaceToSurface (hScreen, m_hNewObjective, DNULL, nScreenWidth - nNewObjectiveWidth - 10, 25);
	}

	// draw transmission if needed
	if (m_fTransmissionTimeLeft > 0.0f) DrawTransmission();

	// if there's an ingame dialog, draw that
	if (m_pIngameDialog) m_pIngameDialog->Draw (pClientDE->GetScreenSurface());

	// handle message editing...
	m_messageMgr.Draw();


	///////////////////////////////////////////////////////////////////////////////
	//
	// WARNING: everything from here on down relies on the camera being created!
	//
	///////////////////////////////////////////////////////////////////////////////

	if (!m_hCamera)
	{
		return;
	}
	
	// get the camera dims
	DBOOL bFullScreen = DFALSE;
	int nCameraLeft = 0;
	int nCameraTop = 0;
	int nCameraRight = 0;
	int nCameraBottom = 0;
	pClientDE->GetCameraRect (m_hCamera, &bFullScreen, &nCameraLeft, &nCameraTop, &nCameraRight, &nCameraBottom);
	if (bFullScreen)
	{
		nCameraRight = (int)nScreenWidth;
		nCameraBottom = (int)nScreenHeight;
	}

	// if there is a game message (sent by a trigger) to display, display it
	if (m_hGameMessage)
	{
		float nCurrentTime = pClientDE->GetTime();
		if (nCurrentTime < m_nGameMessageRemoveTime)
		{
			int x = nCameraLeft + (((nCameraRight - nCameraLeft) - m_rcGameMessage.right) >> 1);
			int y = nCameraBottom - (m_rcGameMessage.bottom << 1);

			HDECOLOR hBlack = pClientDE->SetupColor1 (0.1f, 0.1f, 0.1f, DFALSE);
			pClientDE->DrawSurfaceSolidColor (hScreen, m_hGameMessage, &m_rcGameMessage, x+3, y+3, NULL, hBlack);
			pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hGameMessage, &m_rcGameMessage, x, y, NULL);
		}
		else
		{
			pClientDE->DeleteSurface (m_hGameMessage);
			m_hGameMessage = NULL;
		}
	}
	
	// Display any necessary debugging info...
	if (m_hDebugInfo)
	{
		int x = nScreenWidth  - (m_rcDebugInfo.right - m_rcDebugInfo.left);
		int y = nScreenHeight - 18;

		pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hDebugInfo, &m_rcDebugInfo, x, y, NULL);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::DrawTransmission()
//
//	PURPOSE:	Update the transmission if there is one
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::DrawTransmission()
{
	HMESSAGEWRITE hMsg;
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
	
	if (m_fTransmissionTimeLeft > 0.0f)
	{
		DFLOAT nFrameTime = pClientDE->GetFrameTime();
		if (nFrameTime > 1.0f) nFrameTime = 1.0f;
		m_fTransmissionTimeLeft -= nFrameTime;

		if (m_bAnimatingTransmissionOn)
		{
			m_xTransmissionImage += nFrameTime * TRANSMISSION_IMAGE_ANIM_RATE;
			m_yTransmissionText += nFrameTime * TRANSMISSION_TEXT_ANIM_RATE;

			if (m_xTransmissionImage > 6.0f || m_yTransmissionText > 32.0f)
			{
				m_xTransmissionImage = 6.0f;
				m_yTransmissionText = 32.0f;
				m_bAnimatingTransmissionOn = DFALSE;
			}
		}
		else if (m_bAnimatingTransmissionOff)
		{
			m_xTransmissionImage -= nFrameTime * TRANSMISSION_IMAGE_ANIM_RATE;
			m_yTransmissionText -= nFrameTime * TRANSMISSION_TEXT_ANIM_RATE;

			if (m_xTransmissionImage < -m_cxTransmissionImage && m_yTransmissionText < -m_cyTransmissionText)
			{
				m_bAnimatingTransmissionOff = DFALSE;
				m_fTransmissionTimeLeft = 0.0f;
			
				pClientDE->DeleteSurface (m_hTransmissionImage);
				pClientDE->DeleteSurface (m_hTransmissionText);
				m_hTransmissionImage = DNULL;
				m_hTransmissionText = DNULL;
			}
		}

		// Check if sound is done.
		if( m_hTransmissionSound )
		{
			if( pClientDE->IsDone( m_hTransmissionSound ))
			{
				pClientDE->KillSound( m_hTransmissionSound );
				m_hTransmissionSound = DNULL;
				
				// Tell server the transmission ended.
				hMsg = pClientDE->StartMessage( MID_TRANSMISSIONENDED );
				pClientDE->EndMessage( hMsg );
			}
		}

		if (m_hTransmissionImage)
		{
			HDECOLOR hTrans = pClientDE->SetupColor1 (0.0f, 0.0f, 1.0f, DFALSE);
			pClientDE->DrawSurfaceToSurfaceTransparent (pClientDE->GetScreenSurface(), m_hTransmissionText, DNULL, (int)m_xTransmissionText, (int)m_yTransmissionText, DNULL);
			pClientDE->DrawSurfaceToSurfaceTransparent (pClientDE->GetScreenSurface(), m_hTransmissionImage, DNULL, (int)m_xTransmissionImage, (int)m_yTransmissionImage, hTrans);

			if (m_fTransmissionTimeLeft <= 0.0f)
			{
				if( m_hTransmissionSound && !pClientDE->IsDone( m_hTransmissionSound ))
				{
					m_fTransmissionTimeLeft += nFrameTime;
				}
				else
				{
					m_fTransmissionTimeLeft = 100.0f;
					m_bAnimatingTransmissionOff = DTRUE;
				}
			}

			AddToClearScreenCount();
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::DoRenderLoop()
//
//	PURPOSE:	Forces exactly one update to occur
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::DoRenderLoop (DBOOL bDrawInterface)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
	
	PreUpdate();
	pClientDE->Start3D();
	pClientDE->RenderCamera (m_hCamera);
	if (bDrawInterface) 
	{
		pClientDE->StartOptimized2D();
		DrawInterface();
		pClientDE->EndOptimized2D();
	}
	pClientDE->End3D();
	pClientDE->FlipScreen (FLIPSCREEN_CANDRAWCONSOLE);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdatePlayer()
//
//	PURPOSE:	Update the player
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::UpdatePlayer()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	if (!hPlayerObj || m_ePlayerState == PS_DEAD) return;
	

	// This is pretty much a complete kludge, but I can't really think of
	// a better way to handle this...Okay, since the server can update the
	// player's flags at any time (and override anything that we set), we'll 
	// make sure that the player's flags are always what we want them to be :)

	DDWORD dwPlayerFlags = pClientDE->GetObjectFlags(hPlayerObj);
	if (m_playerCamera.IsFirstPerson())
	{
		if (dwPlayerFlags & FLAG_VISIBLE)
		{
			pClientDE->SetObjectFlags(hPlayerObj, dwPlayerFlags & ~FLAG_VISIBLE);
		}
	}
	else  // Third person
	{
		if (!(dwPlayerFlags & FLAG_VISIBLE))
		{
			pClientDE->SetObjectFlags(hPlayerObj, dwPlayerFlags | FLAG_VISIBLE);
		}
	}


	// Hide/Show our attachments...

	HLOCALOBJ attachList[20];
	DDWORD dwListSize = 0;
	DDWORD dwNumAttach = 0;

	pClientDE->GetAttachments(hPlayerObj, attachList, 20, &dwListSize, &dwNumAttach);
	int nNum = dwNumAttach <= dwListSize ? dwNumAttach : dwListSize;

	for (int i=0; i < nNum; i++)
	{
		DDWORD dwUsrFlags;
		pClientDE->GetObjectUserFlags(attachList[i], &dwUsrFlags);
		
		if (dwUsrFlags & USRFLG_ATTACH_HIDE1SHOW3)
		{
			DDWORD dwFlags = pClientDE->GetObjectFlags(attachList[i]);

			if (m_playerCamera.IsFirstPerson())
			{
				if (dwFlags & FLAG_VISIBLE)
				{
					dwFlags &= ~FLAG_VISIBLE;
					pClientDE->SetObjectFlags(attachList[i], dwFlags);
				}
			}
			else
			{
				if (!(dwFlags & FLAG_VISIBLE))
				{
					dwFlags |= FLAG_VISIBLE;
					pClientDE->SetObjectFlags(attachList[i], dwFlags);
				}
			}
		}
		else if (dwUsrFlags & USRFLG_ATTACH_HIDE1)
		{
			DDWORD dwFlags = pClientDE->GetObjectFlags(attachList[i]);

			if (m_playerCamera.IsFirstPerson())
			{
				if (dwFlags & FLAG_VISIBLE)
				{
					dwFlags &= ~FLAG_VISIBLE;
					pClientDE->SetObjectFlags(attachList[i], dwFlags);
				}
			}
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::Update3rdPersonInfo
//
//	PURPOSE:	Update the 3rd person cross hair / camera info
//
// ----------------------------------------------------------------------- //

void CRiotClientShell::Update3rdPersonInfo()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();
	if (!hPlayerObj || m_ePlayerState == PS_DEAD || !m_MoveMgr) return;

	HOBJECT hFilterList[] = {hPlayerObj, m_MoveMgr->GetObject(), DNULL};


	ClientIntersectInfo info;
	ClientIntersectQuery query;
	memset(&query, 0, sizeof(query));
	DVector vPlayerPos, vUp, vRight, vForward;

	pClientDE->GetObjectPos(hPlayerObj, &vPlayerPos);

	DFLOAT fCrosshairDist = -1.0f;
	DFLOAT fCameraOptZ = 110.0f;

	// Figure out crosshair distance...

	if (m_bCrosshairOn && m_weaponModel.GetHandle())
	{
		fCrosshairDist = GetWeaponRange(m_weaponModel.GetId());

		pClientDE->GetRotationVectors(&m_rRotation, &vUp, &vRight, &vForward);

		// Determine where the cross hair should be...

		DVector vStart, vEnd, vPos;
		VEC_COPY(vStart, vPlayerPos);
		VEC_MULSCALAR(vEnd, vForward, fCrosshairDist);
		VEC_ADD(vEnd, vEnd, vStart);

		VEC_COPY(query.m_From, vStart);
		VEC_COPY(query.m_To, vEnd);

		query.m_Flags = INTERSECT_OBJECTS | IGNORE_NONSOLID;
		query.m_FilterFn = ObjListFilterFn;
		query.m_pUserData = hFilterList;

		if (pClientDE->IntersectSegment (&query, &info))
		{
			VEC_COPY(vPos, info.m_Point);
		}
		else
		{
			VEC_COPY(vPos, vEnd);
		}

		DVector vTemp;
		VEC_SUB(vTemp, vPos, vStart);

		fCrosshairDist = VEC_MAG(vTemp);
	}


	// Figure out optinal camera distance...

	DRotation rRot;
	pClientDE->GetObjectRotation(hPlayerObj, &rRot);
	pClientDE->GetRotationVectors(&rRot, &vUp, &vRight, &vForward);
	VEC_NORM(vForward);

	// Determine how far behind the player the camera can go...

	DVector vEnd;
	VEC_MULSCALAR(vEnd, vForward, -fCameraOptZ);
	VEC_ADD(vEnd, vEnd, vPlayerPos);

	VEC_COPY(query.m_From, vPlayerPos);
	VEC_COPY(query.m_To, vEnd);

	query.m_Flags = INTERSECT_OBJECTS | IGNORE_NONSOLID;
	query.m_FilterFn = ObjListFilterFn;
	query.m_pUserData = hFilterList;

	if (pClientDE->IntersectSegment (&query, &info))
	{
		DVector vTemp;
		VEC_SUB(vTemp, info.m_Point, vPlayerPos);
		DFLOAT fDist = VEC_MAG(vTemp);

		fCameraOptZ = fDist < fCameraOptZ ? -(fDist - 5.0f) : -fCameraOptZ;
	}
	else
	{
		fCameraOptZ = -fCameraOptZ;
	}


	Update3rdPersonCrossHair(fCrosshairDist);
	m_playerCamera.SetOptZ(fCameraOptZ);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CreateMenuPolygrid
//
//	PURPOSE:	Create the polygrid used as a background for the menu
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::CreateMenuPolygrid()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || m_pMenuPolygrid || !m_hCamera) return;

	// Hide our hand-held weapon...

	HLOCALOBJ hWeapon = m_weaponModel.GetHandle();
	if (hWeapon)
	{
		DDWORD dwFlags = pClientDE->GetObjectFlags(hWeapon);
		pClientDE->SetObjectFlags(hWeapon, dwFlags & ~FLAG_VISIBLE);
	}


	// Create an object to serve as the "server" object for the polygrid.
	// The polygrid uses this object to determine its position, rotation,
	// and visibility...

	DVector vPos, vU, vR, vF, vTemp;
	DRotation rRot;

	pClientDE->GetObjectPos(m_hCamera, &vPos);
	pClientDE->GetObjectRotation(m_hCamera, &rRot);
	pClientDE->GetRotationVectors(&rRot, &vU, &vR, &vF);

	// Put the polygrid a little in front of the camera...

	VEC_MULSCALAR(vTemp, vF, /*25.0f*/10.0f);
	VEC_ADD(vPos, vPos, vTemp);

	// Need to orient the polygrid correctly...

	pClientDE->EulerRotateX(&rRot, MATH_HALFPI);

	ObjectCreateStruct theStruct;
	INIT_OBJECTCREATESTRUCT(theStruct);
	theStruct.m_ObjectType = OT_NORMAL;
	VEC_COPY(theStruct.m_Pos, vPos);
	ROT_COPY(theStruct.m_Rotation, rRot);

	HLOCALOBJ hObj = pClientDE->CreateObject(&theStruct);
	if (!hObj) return;


	// Need to set this to visible, or polygrid won't be drawn...

	pClientDE->SetObjectUserFlags(hObj, USRFLG_VISIBLE);


	// Save the current camera fov...

	pClientDE->GetCameraFOV(m_hCamera, &m_fMenuSaveFOVx, &m_fMenuSaveFOVy);

	// Set the menu fov...

	DDWORD dwWidth = 640, dwHeight = 480;
	pClientDE->GetSurfaceDims(pClientDE->GetScreenSurface(), &dwWidth, &dwHeight);

	DFLOAT y = (DEG2RAD(FOV_NORMAL) * dwHeight) / dwWidth;
	pClientDE->SetCameraFOV(m_hCamera, DEG2RAD(FOV_NORMAL), y);


	// Okay, since we now have a "server" object to associate with the
	// polygrid, create the polygrid...

	m_pMenuPolygrid = new CPolyGridFX();
	if (!m_pMenuPolygrid) return;

	PGCREATESTRUCT pg;

	pg.hServerObj = hObj;
	VEC_SET(pg.vDims, 10.6f, 0.5f, 10.6f);
	VEC_SET(pg.vColor1, 0.0f, 0.0f, 0.0f);
	VEC_SET(pg.vColor2, 255.0f, 255.0f, 255.0f);
	pg.fXScaleMin = .085f;
	pg.fXScaleMax = .085f;
	pg.fYScaleMin = .09f;
	pg.fYScaleMax = .09f;
	pg.fXScaleDuration = 0.0f;
	pg.fYScaleDuration = 0.0f;
	pg.fXPan = 0.0f;
	pg.fYPan = 0.0f;
	pg.fAlpha = 1.0f;
	pg.hstrSurfaceSprite = pClientDE->CreateString("Sprites\\Shogo.spr");
	pg.dwNumPolies = 500;
	pg.nPlasmaType = 1;
	pg.nRingRate[0] = 50;
	pg.nRingRate[1] = 10;
	pg.nRingRate[2] = 30;
	pg.nRingRate[3] = 20;

	m_pMenuPolygrid->Init(&pg);
	m_pMenuPolygrid->SetAlwaysUpdate(DTRUE);
	m_pMenuPolygrid->SetUseGlobalSettings(DFALSE);
	m_pMenuPolygrid->CreateObject(pClientDE);

	//HOBJECT hPolyObj = m_pMenuPolygrid->GetObject();
	//pClientDE->SetPolyGridEnvMap(hPolyObj, "Textures\\Chrome.dtx");
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	RemoveMenuPolygrid
//
//	PURPOSE:	Remove the polygrid used as a background for the menu
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::RemoveMenuPolygrid()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_pMenuPolygrid) return;

	// Set the FOV back to what it was...

	if (m_hCamera)
	{
		pClientDE->SetCameraFOV(m_hCamera, m_fMenuSaveFOVx, m_fMenuSaveFOVy);
	}


	// Hide our hand-held weapon...

	HLOCALOBJ hWeapon = m_weaponModel.GetHandle();
	if (hWeapon && m_playerCamera.IsFirstPerson())
	{
		DDWORD dwFlags = pClientDE->GetObjectFlags(hWeapon);
		pClientDE->SetObjectFlags(hWeapon, dwFlags | FLAG_VISIBLE);
	}

	HLOCALOBJ hObj = m_pMenuPolygrid->GetServerObj();
	pClientDE->DeleteObject(hObj);

	delete m_pMenuPolygrid;
	m_pMenuPolygrid = DNULL;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	UpdateMenuPolygrid
//
//	PURPOSE:	Update the polygrid used as a background for the menu
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UpdateMenuPolygrid()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_pMenuPolygrid || !m_hCamera) return;

	m_pMenuPolygrid->Update();

	HLOCALOBJ objs[1];
	objs[0] = m_pMenuPolygrid->GetObject();
	DRESULT dRes = pClientDE->RenderObjects(m_hCamera, objs, 1);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	UpdateModelGlow
//
//	PURPOSE:	Update the current model glow color
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UpdateModelGlow()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	DFLOAT fColor      = 0.0f;
	DFLOAT fColorRange = m_vMaxModelGlow.x - m_vMinModelGlow.x;

	m_fModelGlowCycleTime += pClientDE->GetFrameTime();

	if (m_bModelGlowCycleUp)
	{
		if (m_fModelGlowCycleTime < MODELGLOW_HALFCYCLE)
		{
			fColor = m_vMinModelGlow.x + (m_fModelGlowCycleTime * (fColorRange / MODELGLOW_HALFCYCLE));
			VEC_SET(m_vCurModelGlow, fColor, fColor, fColor);
		}
		else
		{
			m_fModelGlowCycleTime = 0.0f;
			VEC_COPY(m_vCurModelGlow, m_vMaxModelGlow);
			m_bModelGlowCycleUp = DFALSE;
		}
	}
	else 
	{
		if (m_fModelGlowCycleTime < MODELGLOW_HALFCYCLE)
		{
			fColor = m_vMaxModelGlow.x - (m_fModelGlowCycleTime * (fColorRange / MODELGLOW_HALFCYCLE));
			VEC_SET(m_vCurModelGlow, fColor, fColor, fColor);
		}
		else
		{
			m_fModelGlowCycleTime = 0.0f;
			VEC_COPY(m_vCurModelGlow, m_vMinModelGlow);
			m_bModelGlowCycleUp = DTRUE;
		}
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::InitSinglePlayer
//
//	PURPOSE:	Send the server the initial single player info
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::InitSinglePlayer()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;

	// init player variables on server...

	HMESSAGEWRITE hMessage = pClientDE->StartMessage(MID_PLAYER_INITVARS);
	pClientDE->WriteToMessageByte(hMessage, (DBYTE)pSettings->RunLock());
	pClientDE->EndMessage(hMessage);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::InitMultiPlayer
//
//	PURPOSE:	Send the server the initial multiplayer info
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::InitMultiPlayer()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if (!IsMultiplayerGame()) return;

	NetPlayer* pPlayerInfo = NetStart_GetPlayerStruct();
	if (!pPlayerInfo) return;

	HSTRING hstrName = pClientDE->CreateString(pPlayerInfo->m_sName);
	if (!hstrName) return;

	// Init multiplayer info on server...

	HMESSAGEWRITE hMessage = pClientDE->StartMessage(MID_PLAYER_MULTIPLAYER_INIT);
	pClientDE->WriteToMessageByte(hMessage, pPlayerInfo->m_byMech);
	pClientDE->WriteToMessageByte(hMessage, pPlayerInfo->m_byColor);
	pClientDE->WriteToMessageHString(hMessage, hstrName);
	pClientDE->EndMessage(hMessage);

	// Init player settings...

	CRiotSettings* pSettings = m_menu.GetSettings();
	if (!pSettings) return;

	hMessage = pClientDE->StartMessage(MID_PLAYER_INITVARS);
	pClientDE->WriteToMessageByte(hMessage, (DBYTE)pSettings->RunLock());
	pClientDE->EndMessage(hMessage);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandleItemPickedup
//
//	PURPOSE:	Handle an item being pickedup
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::HandleItemPickedup(PickupItemType eType)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;
 
	DoPickupItemScreenTint(eType);

	if (eType == PIT_ULTRA_NIGHTVISION)
	{
		m_bHaveNightVision = DTRUE;
		
		pClientDE->SetModelHook ((ModelHookFn)NVModelHook, this);
		m_LightScaleMgr.SetLightScale (&m_vLightScaleNightVision, LightEffectPowerup);
	}
	else if (eType == PIT_ULTRA_INFRARED)
	{
		m_bHaveInfrared = DTRUE;
		
		pClientDE->SetModelHook ((ModelHookFn)IRModelHook, this);
		m_LightScaleMgr.SetLightScale (&m_vLightScaleInfrared, LightEffectPowerup);
	}
	else if (eType == PIT_ULTRA_SILENCER)
	{
		m_bHaveSilencer = DTRUE;
	}
	else if (eType == PIT_ULTRA_STEALTH)
	{
		m_bHaveStealth = DTRUE;
	}
	else if (eType == PIT_SHOGO_S || eType == PIT_SHOGO_H || eType == PIT_SHOGO_O || eType == PIT_SHOGO_G)
	{
		m_inventory.ShogoPowerupPickup (eType);
	}
	else
	{
		DisplayGenericPickupMessage(eType);
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandleItemExpired
//
//	PURPOSE:	Handle an item expiring
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::HandleItemExpired(PickupItemType eType)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE) return;

	if (eType == PIT_ULTRA_NIGHTVISION)
	{
		m_bHaveNightVision = DFALSE;

		pClientDE->SetModelHook ((ModelHookFn)DefaultModelHook, this);
		m_LightScaleMgr.ClearLightScale (&m_vLightScaleNightVision, LightEffectPowerup);
	}
	else if (eType == PIT_ULTRA_INFRARED)
	{
		m_bHaveInfrared = DFALSE;
		
		pClientDE->SetModelHook ((ModelHookFn)DefaultModelHook, this);
		m_LightScaleMgr.ClearLightScale (&m_vLightScaleInfrared, LightEffectPowerup);
	}
	else if (eType == PIT_ULTRA_SILENCER)
	{
		m_bHaveSilencer = DFALSE;
	}
	else if (eType == PIT_ULTRA_STEALTH)
	{
		m_bHaveStealth = DFALSE;
	}
}





// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::BuildClientSaveMsg
//
//	PURPOSE:	Save all the necessary client-side info
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::BuildClientSaveMsg(HMESSAGEWRITE hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !hMessage) return;

	HMESSAGEWRITE hData = pClientDE->StartHMessageWrite();
	
	// Save complex data members...

	m_stats.Save(hData);
	m_inventory.Save(hData);
	m_objectives.Save(hData);


	// Save all necessary data members...
	DRotation dummyRotation;
	ROT_INIT(dummyRotation);

	pClientDE->WriteToMessageRotation(hData, &m_rRotation);
	pClientDE->WriteToMessageRotation(hData, &dummyRotation);
	pClientDE->WriteToMessageVector(hData, &m_vTintColor);
	pClientDE->WriteToMessageVector(hData, &m_vLastSentFlashPos);
	pClientDE->WriteToMessageVector(hData, &m_vLastSentModelPos);
	pClientDE->WriteToMessageVector(hData, &m_vCameraOffset);

	pClientDE->WriteToMessageByte(hData, m_eDifficulty);
	pClientDE->WriteToMessageByte(hData, m_nPlayerMode);
	pClientDE->WriteToMessageByte(hData, m_nLastSentCode);
	pClientDE->WriteToMessageByte(hData, m_bTintScreen);
	pClientDE->WriteToMessageByte(hData, m_bSpectatorMode);
	pClientDE->WriteToMessageByte(hData, m_bMoving);
	pClientDE->WriteToMessageByte(hData, m_bMovingSide);
	pClientDE->WriteToMessageByte(hData, m_bOnGround);
	pClientDE->WriteToMessageByte(hData, m_bLastSent3rdPerson);
	pClientDE->WriteToMessageByte(hData, m_bZoomView);
	pClientDE->WriteToMessageByte(hData, m_bOldZoomView);
	pClientDE->WriteToMessageByte(hData, m_bZooming);
	pClientDE->WriteToMessageByte(hData, m_bStartedDuckingDown);
	pClientDE->WriteToMessageByte(hData, m_bStartedDuckingUp);
	pClientDE->WriteToMessageByte(hData, m_bCenterView);
	pClientDE->WriteToMessageByte(hData, m_bAllowPlayerMovement);
	pClientDE->WriteToMessageByte(hData, m_bLastAllowPlayerMovement);
	pClientDE->WriteToMessageByte(hData, m_bWasUsingExternalCamera);
	pClientDE->WriteToMessageByte(hData, m_bUsingExternalCamera);
	pClientDE->WriteToMessageByte(hData, m_bMovieCameraRect);
	pClientDE->WriteToMessageByte(hData, m_bUnderwater);
	//pClientDE->WriteToMessageByte(hData, m_bGameOver);
	pClientDE->WriteToMessageByte(hData, m_ePlayerState);
	pClientDE->WriteToMessageByte(hData, m_eMusicLevel);

	pClientDE->WriteToMessageDWord(hData, m_dwPlayerFlags);
	pClientDE->WriteToMessageDWord(hData, m_nOldCameraLeft);
	pClientDE->WriteToMessageDWord(hData, m_nOldCameraTop);
	pClientDE->WriteToMessageDWord(hData, m_nOldCameraRight);
	pClientDE->WriteToMessageDWord(hData, m_nOldCameraBottom);

	pClientDE->WriteToMessageFloat(hData, m_fTintTime);
	pClientDE->WriteToMessageFloat(hData, m_fTintStart);
	pClientDE->WriteToMessageFloat(hData, m_fTintRampUp);
	pClientDE->WriteToMessageFloat(hData, m_fTintRampDown);
	pClientDE->WriteToMessageFloat(hData, m_fYaw);
	pClientDE->WriteToMessageFloat(hData, m_fPitch);
	pClientDE->WriteToMessageFloat(hData, m_fLastSentYaw);
	pClientDE->WriteToMessageFloat(hData, m_fLastSentCamCant);
	pClientDE->WriteToMessageFloat(hData, m_fPitch);
	pClientDE->WriteToMessageFloat(hData, m_fYaw);
	pClientDE->WriteToMessageFloat(hData, m_fFireJitterPitch);
	pClientDE->WriteToMessageFloat(hData, m_fContainerStartTime);
	pClientDE->WriteToMessageFloat(hData, m_fFovXFXDir);
	pClientDE->WriteToMessageFloat(hData, m_fLastTime);
	pClientDE->WriteToMessageFloat(hData, m_fBobHeight);
	pClientDE->WriteToMessageFloat(hData, m_fBobWidth);
	pClientDE->WriteToMessageFloat(hData, m_fBobAmp);
	pClientDE->WriteToMessageFloat(hData, m_fBobPhase);
	pClientDE->WriteToMessageFloat(hData, m_fSwayPhase);
	pClientDE->WriteToMessageFloat(hData, m_fVelMagnitude);
	pClientDE->WriteToMessageFloat(hData, m_fCantIncrement);
	pClientDE->WriteToMessageFloat(hData, m_fCantMaxDist);
	pClientDE->WriteToMessageFloat(hData, m_fCamCant);
	pClientDE->WriteToMessageFloat(hData, m_fCurrentFovX);
	pClientDE->WriteToMessageFloat(hData, m_fSaveLODScale);
	pClientDE->WriteToMessageFloat(hData, m_fCamDuck);
	pClientDE->WriteToMessageFloat(hData, m_fDuckDownV);
	pClientDE->WriteToMessageFloat(hData, m_fDuckUpV);
	pClientDE->WriteToMessageFloat(hData, m_fMaxDuckDistance);
	pClientDE->WriteToMessageFloat(hData, m_fStartDuckTime);
	
	pClientDE->WriteToMessageHMessageWrite(hMessage, hData);
	pClientDE->EndHMessageWrite(hData);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UnpackClientSaveMsg
//
//	PURPOSE:	Load all the necessary client-side info
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::UnpackClientSaveMsg(HMESSAGEREAD hMessage)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !hMessage) return;

	m_bRestoringGame = DTRUE;


	HMESSAGEREAD hData = pClientDE->ReadFromMessageHMessageRead(hMessage);


	// Load complex data members...

	// DO NOT CHANGE THE LOADING ORDER OF STATS AND INVENTORY
	// INVENTORY LOAD DEPENDS ON BEING LOADED AFTER STATS
	m_stats.Load(hData);
	m_inventory.Load(hData);
	m_objectives.Load(hData);


	// Load data members...
	DRotation dummyRotation;

	pClientDE->ReadFromMessageRotation(hData, &m_rRotation);
	pClientDE->ReadFromMessageRotation(hData, &dummyRotation);
	pClientDE->ReadFromMessageVector(hData, &m_vTintColor);
	pClientDE->ReadFromMessageVector(hData, &m_vLastSentFlashPos);
	pClientDE->ReadFromMessageVector(hData, &m_vLastSentModelPos);
	pClientDE->ReadFromMessageVector(hData, &m_vCameraOffset);

	// Reset our first-person camera offset...

	m_playerCamera.SetFirstPersonOffset(m_vCameraOffset);

	m_eDifficulty				= (GameDifficulty) pClientDE->ReadFromMessageByte(hData);
	m_nPlayerMode				= pClientDE->ReadFromMessageByte(hData);
	m_nLastSentCode				= pClientDE->ReadFromMessageByte(hData);
	m_bTintScreen 				= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bSpectatorMode			= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bMoving					= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bMovingSide				= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bOnGround					= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bLastSent3rdPerson		= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bZoomView					= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bOldZoomView				= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bZooming					= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bStartedDuckingDown		= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bStartedDuckingUp			= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bCenterView				= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bAllowPlayerMovement		= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bLastAllowPlayerMovement	= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bWasUsingExternalCamera	= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bUsingExternalCamera		= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bMovieCameraRect			= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_bUnderwater				= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	//m_bGameOver					= (DBOOL) pClientDE->ReadFromMessageByte(hData);
	m_ePlayerState				= (PlayerState) pClientDE->ReadFromMessageByte(hData);
	m_eMusicLevel				= (CMusic::EMusicLevel) pClientDE->ReadFromMessageByte(hData);

	m_dwPlayerFlags				= pClientDE->ReadFromMessageDWord(hData);
	m_nOldCameraLeft			= (int) pClientDE->ReadFromMessageDWord(hData);
	m_nOldCameraTop				= (int) pClientDE->ReadFromMessageDWord(hData);
	m_nOldCameraRight			= (int) pClientDE->ReadFromMessageDWord(hData);
	m_nOldCameraBottom			= (int) pClientDE->ReadFromMessageDWord(hData);

	m_fTintTime					= pClientDE->ReadFromMessageFloat(hData);
	m_fTintStart				= pClientDE->ReadFromMessageFloat(hData);
	m_fTintRampUp				= pClientDE->ReadFromMessageFloat(hData);
	m_fTintRampDown				= pClientDE->ReadFromMessageFloat(hData);
	m_fYawBackup				= pClientDE->ReadFromMessageFloat(hData);
	m_fPitchBackup				= pClientDE->ReadFromMessageFloat(hData);
	m_fLastSentYaw				= pClientDE->ReadFromMessageFloat(hData);
	m_fLastSentCamCant			= pClientDE->ReadFromMessageFloat(hData);
	m_fPitch					= pClientDE->ReadFromMessageFloat(hData);
	m_fYaw						= pClientDE->ReadFromMessageFloat(hData);
	m_fFireJitterPitch			= pClientDE->ReadFromMessageFloat(hData);
	m_fContainerStartTime		= pClientDE->ReadFromMessageFloat(hData);
	m_fFovXFXDir				= pClientDE->ReadFromMessageFloat(hData);
	m_fLastTime					= pClientDE->ReadFromMessageFloat(hData);
	m_fBobHeight				= pClientDE->ReadFromMessageFloat(hData);
	m_fBobWidth					= pClientDE->ReadFromMessageFloat(hData);
	m_fBobAmp					= pClientDE->ReadFromMessageFloat(hData);
	m_fBobPhase					= pClientDE->ReadFromMessageFloat(hData);
	m_fSwayPhase				= pClientDE->ReadFromMessageFloat(hData);
	m_fVelMagnitude				= pClientDE->ReadFromMessageFloat(hData);
	m_fCantIncrement			= pClientDE->ReadFromMessageFloat(hData);
	m_fCantMaxDist				= pClientDE->ReadFromMessageFloat(hData);
	m_fCamCant					= pClientDE->ReadFromMessageFloat(hData);
	m_fCurrentFovX				= pClientDE->ReadFromMessageFloat(hData);
	m_fSaveLODScale				= pClientDE->ReadFromMessageFloat(hData);
	m_fCamDuck					= pClientDE->ReadFromMessageFloat(hData);
	m_fDuckDownV				= pClientDE->ReadFromMessageFloat(hData);
	m_fDuckUpV					= pClientDE->ReadFromMessageFloat(hData);
	m_fMaxDuckDistance			= pClientDE->ReadFromMessageFloat(hData);
	m_fStartDuckTime			= pClientDE->ReadFromMessageFloat(hData);

	pClientDE->EndHMessageRead(hData);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::ProcessCheat
//
//	PURPOSE:	process a cheat.
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::ProcessCheat(CheatCode nCode)
{
	switch (nCode)
	{
		case CHEAT_ANIME:
			m_bAnime = !m_bAnime;
		break;

		default : break;
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandleRespawn
//
//	PURPOSE:	Handle player respawn
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::HandleRespawn()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || m_ePlayerState != PS_DEAD) return;


	// if we're in multiplayer send the respawn command...

	if (IsMultiplayerGame())
	{
		// send a message to the server telling it that it's ok to respawn us now...

		HMESSAGEWRITE hMsg = pClientDE->StartMessage(MID_PLAYER_RESPAWN);
		pClientDE->EndMessage(hMsg);
		return;
	}
	else  // Bring up load game menu...
	{
		SetLoadGameMenu();
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::HandleMPChangeLevel
//
//	PURPOSE:	Handle changing levels in a multiplayer game
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::HandleMPChangeLevel()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !IsMultiplayerGame() || m_nGameState == GS_MPLOADINGLEVEL) return;

	// Clear screen tint...

	if (m_bTintScreen)
	{
		m_bTintScreen = DFALSE;
		DVector vLightAdd;
		VEC_INIT(vLightAdd);
		pClientDE->SetCameraLightAdd(m_hCamera, &vLightAdd);
	}

	// Update the screen here with the current frag counts and no interface, and tell
	// the game we want to ignore any future updates until OnEnterWorld() is called...

	pClientDE->ClearScreen(DNULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER);
	pClientDE->Start3D();

	CreateMenuPolygrid();
	UpdateMenuPolygrid();

	pClientDE->StartOptimized2D();

	HSURFACE hLoadingWorld = CTextHelper::CreateSurfaceFromString(pClientDE, m_menu.GetFont12s(), IDS_BUMPER_LOADING);
	if (hLoadingWorld)
	{
		HSURFACE hScreen = pClientDE->GetScreenSurface();
		DDWORD nScreenWidth = 0, nScreenHeight = 0;
		pClientDE->GetSurfaceDims(hScreen, &nScreenWidth, &nScreenHeight);

		DDWORD nWidth = 0, nHeight = 0;
		pClientDE->GetSurfaceDims(hLoadingWorld, &nWidth, &nHeight);
		pClientDE->DrawSurfaceToSurfaceTransparent(hScreen, hLoadingWorld, DNULL, ((int)(nScreenWidth - nWidth)) / 2, 
												   (int)nScreenHeight - (int)m_cyPressAnyKey, DNULL);

		pClientDE->DeleteSurface(hLoadingWorld);
	}
	
	m_ClientInfo.Draw(DFALSE, DTRUE);
	pClientDE->EndOptimized2D();

	pClientDE->End3D();
	pClientDE->FlipScreen(FLIPSCREEN_CANDRAWCONSOLE);

	m_nGameState = GS_MPLOADINGLEVEL;
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::QuickSave
//
//	PURPOSE:	Quick save the game
//
// --------------------------------------------------------------------------- //

DBOOL CRiotClientShell::QuickSave()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || IsMultiplayerGame()) return DFALSE;

	if (m_ePlayerState == PS_DEAD || m_nGameState != GS_PLAYING || 
		m_bUsingExternalCamera) return DFALSE;

	// Do quick save...
	
	HSTRING hStr = pClientDE->FormatString (IDS_QUICKSAVING);
	pClientDE->CPrint(pClientDE->GetStringData (hStr));
	m_infoDisplay.AddInfo(pClientDE->GetStringData (hStr), m_menu.GetFont12s(), 0.5f, DI_CENTER | DI_TOP);
	pClientDE->FreeString (hStr);

	char strKey[32];
	SAFE_STRCPY(strKey, "SaveGame00");
	char strSaveGame[256];
	SAFE_STRCPY(strSaveGame, m_strCurrentWorldName);
	CWinUtil::WinWritePrivateProfileString ("Shogo", strKey, strSaveGame, SAVEGAMEINI_FILENAME);
	
	m_bQuickSave = DTRUE;

	return DTRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::QuickLoad
//
//	PURPOSE:	Quick load the game
//
// --------------------------------------------------------------------------- //

DBOOL CRiotClientShell::QuickLoad()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || IsMultiplayerGame()) return DFALSE;

	if (m_nGameState != GS_PLAYING || m_bUsingExternalCamera) return DFALSE;

	ClearScreenTint();

	char strSaveGameSetting[256];
	memset (strSaveGameSetting, 0, 256);
	char strKey[32];
	SAFE_STRCPY(strKey, "SaveGame00");
	CWinUtil::WinGetPrivateProfileString ("Shogo", strKey, "", strSaveGameSetting, 256, SAVEGAMEINI_FILENAME);
	
	if (!*strSaveGameSetting)
	{
		DoMessageBox (IDS_NOQUICKSAVEGAME, TH_ALIGN_CENTER);
		return DFALSE;
	}

	char* strWorldName = strSaveGameSetting;
	if (!LoadGame(strWorldName, QUICKSAVE_FILENAME))
	{
		DoMessageBox(IDS_LOADGAMEFAILED, TH_ALIGN_CENTER);
		return DFALSE;
	}

	return DTRUE;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::IsMultiplayerGame()
//
//	PURPOSE:	See if we are playing a multiplayer game
//
// --------------------------------------------------------------------------- //

DBOOL CRiotClientShell::IsMultiplayerGame()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE ) return DFALSE;

	int nGameMode = 0;
	pClientDE->GetGameMode(&nGameMode);
	if (nGameMode == STARTGAME_NORMAL || nGameMode == GAMEMODE_NONE) return DFALSE;

	return DTRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::IsPlayerInWorld()
//
//	PURPOSE:	See if the player is in the world
//
// --------------------------------------------------------------------------- //

DBOOL CRiotClientShell::IsPlayerInWorld()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE ) return DFALSE;

	HLOCALOBJ hPlayerObj = pClientDE->GetClientObject();

	if (!m_bPlayerPosSet || !m_bInWorld || m_ePlayerState == PS_UNKNOWN || !hPlayerObj) return DFALSE;

	return DTRUE;
}


void CRiotClientShell::GetCameraRotation(DRotation *pRot)
{
	GetClientDE()->SetupEuler(pRot, m_fPitch, m_fYaw, m_fCamCant);
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::StartGame
//
//	PURPOSE:	Start a new game
//
// --------------------------------------------------------------------------- //

DBOOL CRiotClientShell::StartGame(GameDifficulty eDifficulty)
{
	SetDifficulty(eDifficulty);

	// Play the intro first...

	SetGameState(GS_INTRO);
	SetMenuMusic(DFALSE);

	return DTRUE;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::DoStartGame
//
//	PURPOSE:	*Really* Start a new game
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::DoStartGame()
{
#ifdef _DEMO
	CreateBumperScreen("Demo", 1606);
	LoadWorld("Worlds\\demo_mca");
#else
	CreateBumperScreen("Ambush", 1600);
	LoadWorld("Worlds\\01_Ambush");
#endif
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::GetNiceWorldName
//
//	PURPOSE:	Get the nice (level designer set) world name...
//
// --------------------------------------------------------------------------- //

DBOOL CRiotClientShell::GetNiceWorldName(char* pWorldFile, char* pRetName, int nRetLen)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !pWorldFile || !pRetName || nRetLen < 2) return DFALSE;

	char buf[_MAX_PATH];
	buf[0] = '\0';
	DWORD len;

	char buf2[_MAX_PATH];
	sprintf(buf2, "%s.dat", pWorldFile);

	DRESULT dRes = pClientDE->GetWorldInfoString(buf2, buf, _MAX_PATH, &len);

	if (dRes != LT_OK || !buf[0] || len < 1)
	{
		// try pre-pending "worlds\" to the filename to see if it will find it then...

		sprintf (buf2, "worlds\\%s.dat", pWorldFile);
		dRes = pClientDE->GetWorldInfoString(buf2, buf, _MAX_PATH, &len);

		if (dRes != LT_OK || !buf[0] || len < 1)
		{
			return DFALSE;
		}
	}
	

	char tokenSpace[5*(PARSE_MAXTOKENSIZE + 1)];
	char *pTokens[5];
	int nArgs;

	char* pCurPos = buf;
	char* pNextPos;

	DBOOL bMore = DTRUE;
	while (bMore)
	{
		bMore = pClientDE->Parse(pCurPos, &pNextPos, tokenSpace, pTokens, &nArgs);
		if (nArgs < 2) break;

		if (_stricmp(pTokens[0], "WORLDNAME") == 0)
		{
			strncpy(pRetName, pTokens[1], nRetLen);
			return DTRUE;
		}

		pCurPos = pNextPos;
	}
	
	return DFALSE;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::DisplayGenericPickupMessage
//
//	PURPOSE:	Display a message when an item is picked up
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::DisplayGenericPickupMessage(PickupItemType eType)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || eType < 0 || eType >= PIT_MAX) return;

	static DDWORD s_pickupItemToStringIdMap[PIT_MAX] =
	{
		IDS_PIT_UNKNOWN, 
		
		// Armor...

		IDS_PIT_ARMOR_REPAIR100,
		IDS_PIT_ARMOR_REPAIR250,
		IDS_PIT_ARMOR_REPAIR500,
		IDS_PIT_ARMOR_BODY50,
		IDS_PIT_ARMOR_BODY100,
		IDS_PIT_ARMOR_BODY200,

		// Enhancements...

		IDS_PIT_ENHANCEMENT_DAMAGE,
		IDS_PIT_ENHANCEMENT_MELEEDAMAGE,
		IDS_PIT_ENHANCEMENT_PROTECTION,
		IDS_PIT_ENHANCEMENT_ENERGYPROTECTION,
		IDS_PIT_ENHANCEMENT_PROJECTILEPROTECTION,
		IDS_PIT_ENHANCEMENT_EXPLOSIVEPROTECTION,
		IDS_PIT_ENHANCEMENT_REGEN,
		IDS_PIT_ENHANCEMENT_HEALTH,
		IDS_PIT_ENHANCEMENT_ARMOR,

		// First aid...

		IDS_PIT_FIRSTAID_10,
		IDS_PIT_FIRSTAID_15,
		IDS_PIT_FIRSTAID_25,
		IDS_PIT_FIRSTAID_50,

		IDS_PIT_POWERSURGE_50,
		IDS_PIT_POWERSURGE_100,
		IDS_PIT_POWERSURGE_150,
		IDS_PIT_POWERSURGE_250,

		// Ultra Powerups...

		IDS_PIT_ULTRA_DAMAGE, 
		IDS_PIT_ULTRA_HEALTH, 
		IDS_PIT_ULTRA_POWERSURGE, 
		IDS_PIT_ULTRA_SHIELD, 
		IDS_PIT_ULTRA_STEALTH, 
		IDS_PIT_ULTRA_REFLECT, 
		IDS_PIT_ULTRA_NIGHTVISION, 
		IDS_PIT_ULTRA_INFRARED, 
		IDS_PIT_ULTRA_SILENCER, 
		IDS_PIT_ULTRA_RESTORE,

		// Uprades...

		IDS_PIT_UPGRADE_DAMAGE,
		IDS_PIT_UPGRADE_PROTECTION,
		IDS_PIT_UPGRADE_REGEN,
		IDS_PIT_UPGRADE_HEALTH,
		IDS_PIT_UPGRADE_ARMOR,
		IDS_PIT_UPGRADE_TARGETING,

		// Misc...

		IDS_PIT_CAT,
		IDS_PIT_SHOGO_S,
		IDS_PIT_SHOGO_H,
		IDS_PIT_SHOGO_O,
		IDS_PIT_SHOGO_G
	};

	DDWORD dwItemId = s_pickupItemToStringIdMap[eType];
	if (dwItemId == IDS_PIT_UNKNOWN) return;

	HSTRING hStr = pClientDE->FormatString(dwItemId);
	if (!hStr) return;

	char* pStr = pClientDE->GetStringData(hStr);
	if (!pStr) return;

	DDWORD nWidth, nHeight;
	HSURFACE hScreen = pClientDE->GetScreenSurface();
	pClientDE->GetSurfaceDims(hScreen, &nWidth, &nHeight);
	
	CBitmapFont* pFont = m_menu.GetFont12s();
	if (nWidth < 640)
	{
		pFont = m_menu.GetFont08s();
	}
	
	pClientDE->CPrint(pStr);
	m_infoDisplay.AddInfo(pStr, pFont, 2.0f, DI_CENTER | DI_BOTTOM);
	pClientDE->FreeString(hStr);
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::DoPickupItemScreenTint
//
//	PURPOSE:	Tint the screen when an item is picked up
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::DoPickupItemScreenTint(PickupItemType eType)
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || eType < 0 || eType >= PIT_MAX || !m_hCamera) return;

	static DVector s_vPickupItemTintColor[PIT_MAX] =
	{
		PICKUPITEM_TINT_UNKNOWN,		// Unknown 

		// Armor...

		PICKUPITEM_TINT_ARMOR,			// Armor repair 100
		PICKUPITEM_TINT_ARMOR,			// Armor repair 250
		PICKUPITEM_TINT_ARMOR,			// Armor repair 500
		PICKUPITEM_TINT_ARMOR,			// Body armor	50
		PICKUPITEM_TINT_ARMOR,			// Body armor	100
		PICKUPITEM_TINT_ARMOR,			// Body armor	200
	
		// Enhancements...

		PICKUPITEM_TINT_WEAPON,			// Damage Enhancement
		PICKUPITEM_TINT_WEAPON,			// Melee Damage Enhancement
		PICKUPITEM_TINT_ARMOR,			// Protection Enhancement
		PICKUPITEM_TINT_ARMOR,			// Energy Protection Enhancement
		PICKUPITEM_TINT_ARMOR,			// Projectile Protection Enhancement
		PICKUPITEM_TINT_ARMOR,			// Explosive Protection Enhancement
		PICKUPITEM_TINT_ARMOR,			// Regeneration Enhancement
		PICKUPITEM_TINT_HEALTH,			// Health Enhancement
		PICKUPITEM_TINT_ARMOR,			// Armor Enhancement

		// First aid...

		PICKUPITEM_TINT_HEALTH,			// First Aid 10
		PICKUPITEM_TINT_HEALTH,			// First Aid 15
		PICKUPITEM_TINT_HEALTH,			// First Aid 25
		PICKUPITEM_TINT_HEALTH,			// First Aid 50

		PICKUPITEM_TINT_HEALTH,			// Power Surge 50
		PICKUPITEM_TINT_HEALTH,			// Power Surge 100
		PICKUPITEM_TINT_HEALTH,			// Power Surge 150
		PICKUPITEM_TINT_HEALTH,			// Power Surge 250


		// Ultra Powerups...

		PICKUPITEM_TINT_WEAPON,			// Ultra Damage
		PICKUPITEM_TINT_HEALTH,			// Ultra Health
		PICKUPITEM_TINT_HEALTH,			// Ultra Power Surge
		PICKUPITEM_TINT_ARMOR,			// Ultra Shield
		PICKUPITEM_TINT_ARMOR,			// Ultra Stealth
		PICKUPITEM_TINT_ARMOR,			// Ultra Reflect
		PICKUPITEM_TINT_UNKNOWN,		// Ultra Night Vision
		PICKUPITEM_TINT_UNKNOWN,		// Ultra Infrared
		PICKUPITEM_TINT_WEAPON,			// Ultra Silencer
		PICKUPITEM_TINT_HEALTH,			// Ultra Restore

		// Uprades...

		PICKUPITEM_TINT_WEAPON,			// Damage Upgrade
		PICKUPITEM_TINT_ARMOR,			// Protection Upgrade
		PICKUPITEM_TINT_HEALTH,			// Regeneration Upgrade
		PICKUPITEM_TINT_HEALTH,			// Health Upgrade
		PICKUPITEM_TINT_ARMOR,			// Armor Upgrade
		PICKUPITEM_TINT_WEAPON,			// Targeting Upgrade


		// Misc...

		PICKUPITEM_TINT_UNKNOWN,		// Cat
		PICKUPITEM_TINT_UNKNOWN,		// S
		PICKUPITEM_TINT_UNKNOWN,		// H
		PICKUPITEM_TINT_UNKNOWN,		// O
		PICKUPITEM_TINT_UNKNOWN			// G
	};


	DVector vTintColor;
	VEC_COPY(vTintColor, s_vPickupItemTintColor[eType]);
	DFLOAT fRampDown = 2.0f;
	DFLOAT fRampUp = 0.2f, fTintTime = 0.1f;
	
	DVector vCamPos;
	pClientDE->GetObjectPos(m_hCamera, &vCamPos);

	DRotation rRot;
	pClientDE->GetObjectRotation(m_hCamera, &rRot);

	DVector vU, vR, vF;
	pClientDE->GetRotationVectors(&rRot, &vU, &vR, &vF);

	VEC_MULSCALAR(vF, vF, 10.0f);
	VEC_ADD(vCamPos, vCamPos, vF);

	TintScreen(vTintColor, vCamPos, 1000.0f, fRampUp, fTintTime, fRampDown, DTRUE);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::OnModelKey
//
//	PURPOSE:	Handle weapon model keys
//
// --------------------------------------------------------------------------- //

void CRiotClientShell::OnModelKey(HLOCALOBJ hObj, ArgList *pArgs)
{
	m_weaponModel.OnModelKey(hObj, pArgs);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::CreateBoundingBox
//
//	PURPOSE:	Create a box around the MoveMgr object
//
// --------------------------------------------------------------------------- //
	
void CRiotClientShell::CreateBoundingBox()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_MoveMgr || m_hBoundingBox) return;

	HLOCALOBJ hMoveMgrObj = m_MoveMgr->GetObject();
	if (!hMoveMgrObj) return;

	ObjectCreateStruct theStruct;
	INIT_OBJECTCREATESTRUCT(theStruct);

	DVector vPos;
	pClientDE->GetObjectPos(hMoveMgrObj, &vPos);
	VEC_COPY(theStruct.m_Pos, vPos);

	SAFE_STRCPY(theStruct.m_Filename, "Models\\Props\\1x1_square.abc");
	SAFE_STRCPY(theStruct.m_SkinName, "SpecialFX\\smoke.dtx");
	theStruct.m_ObjectType = OT_MODEL;
	theStruct.m_Flags = FLAG_VISIBLE | FLAG_MODELWIREFRAME;

	m_hBoundingBox = pClientDE->CreateObject(&theStruct);

	UpdateBoundingBox();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CRiotClientShell::UpdateBoundingBox()
//
//	PURPOSE:	Update the bounding box
//
// ----------------------------------------------------------------------- //
	
void CRiotClientShell::UpdateBoundingBox()
{
	CClientDE* pClientDE = GetClientDE();
	if (!pClientDE || !m_hBoundingBox) return;

	HLOCALOBJ hMoveMgrObj = m_MoveMgr->GetObject();
	if (!hMoveMgrObj) return;

	DVector vPos;
	pClientDE->GetObjectPos(hMoveMgrObj, &vPos);
	pClientDE->SetObjectPos(m_hBoundingBox, &vPos);

	DVector vDims;
	pClientDE->Physics()->GetObjectDims(hMoveMgrObj, &vDims);

	DVector vScale;
	VEC_DIVSCALAR(vScale, vDims, 0.5f);
	pClientDE->SetObjectScale(m_hBoundingBox, &vScale);
}


// --------------------------------------------------------------------------- //
// Called by the engine, saves all variables (console and member variables)
// related to demo playback.
// --------------------------------------------------------------------------- //
void LoadConVar(ClientDE *pClientDE, DStream *pStream, char *pVarName)
{
	float val;
	char cString[512];

	(*pStream) >> val;
	sprintf(cString, "%s %f", pVarName, val);
	pClientDE->RunConsoleString(cString);
}

void SaveConVar(ClientDE *pClientDE, DStream *pStream, char *pVarName, float defaultVal)
{
	HCONSOLEVAR hVar;
	float val;

	val = defaultVal;
	if(hVar = pClientDE->GetConsoleVar (pVarName))
	{
		val = pClientDE->GetVarValueFloat (hVar);
	}

	(*pStream) << val;
}

void CRiotClientShell::DemoSerialize(DStream *pStream, DBOOL bLoad)
{
	ClientDE *pClientDE = GetClientDE();
	CRiotSettings* pSettings = m_menu.GetSettings();
	DDWORD i;

	if(bLoad)
	{
		LoadConVar(pClientDE, pStream, NORMAL_TURN_RATE_VAR);
		LoadConVar(pClientDE, pStream, FAST_TURN_RATE_VAR);
		
		for(i=RS_CTRL_FIRST; i <= RS_CTRL_LAST; i++)
		{		
			(*pStream) >> pSettings->ControlSetting(i);
		}
	
		for(i=RS_DET_FIRST; i <= RS_DET_LAST; i++)
		{		
			(*pStream) >> pSettings->DetailSetting(i);
		}

		for(i=RS_SUBDET_FIRST; i <= RS_SUBDET_LAST; i++)
		{		
			(*pStream) >> pSettings->SubDetailSetting(i);
		}
	}
	else
	{
		SaveConVar(pClientDE, pStream, NORMAL_TURN_RATE_VAR, DEFAULT_NORMAL_TURN_SPEED);
		SaveConVar(pClientDE, pStream, FAST_TURN_RATE_VAR, DEFAULT_FAST_TURN_SPEED);

		for(i=RS_CTRL_FIRST; i <= RS_CTRL_LAST; i++)
		{		
			(*pStream) << pSettings->ControlSetting(i);
		}
	
		for(i=RS_DET_FIRST; i <= RS_DET_LAST; i++)
		{		
			(*pStream) << pSettings->DetailSetting(i);
		}

		for(i=RS_SUBDET_FIRST; i <= RS_SUBDET_LAST; i++)
		{		
			(*pStream) << pSettings->SubDetailSetting(i);
		}
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	LoadLeakFile
//
//	PURPOSE:	Loads a leak file and creates a line system for it.
//
// --------------------------------------------------------------------------- //

DBOOL LoadLeakFile(ClientDE *pClientDE, char *pFilename)
{
	FILE *fp;
	char line[256];
	HLOCALOBJ hObj;
	ObjectCreateStruct cStruct;
	DELine theLine;
	int nRead;

	fp = fopen(pFilename, "rt");
	if(fp)
	{
		INIT_OBJECTCREATESTRUCT(cStruct);
		cStruct.m_ObjectType = OT_LINESYSTEM;
		cStruct.m_Flags = FLAG_VISIBLE;
		hObj = pClientDE->CreateObject(&cStruct);
		if(!hObj)
		{
			fclose(fp);
			return DFALSE;
		}

		while(fgets(line, 256, fp))
		{
			nRead = sscanf(line, "%f %f %f %f %f %f", 
				&theLine.m_Points[0].m_Pos.x, &theLine.m_Points[0].m_Pos.y, &theLine.m_Points[0].m_Pos.z, 
				&theLine.m_Points[1].m_Pos.x, &theLine.m_Points[1].m_Pos.y, &theLine.m_Points[1].m_Pos.z);

			// White
			theLine.m_Points[0].r = theLine.m_Points[0].g = theLine.m_Points[0].b = 1;
			theLine.m_Points[0].a = 1;
			
			// Read
			theLine.m_Points[1].r = 1;
			theLine.m_Points[1].g = theLine.m_Points[1].b = 0;
			theLine.m_Points[1].a = 1;

			pClientDE->AddLine(hObj, &theLine);
		}

		fclose(fp);
		return DTRUE;
	}
	else
	{
		return DFALSE;
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	ConnectToTcpIpAddress
//
//	PURPOSE:	Connects (joins) to the given tcp/ip address
//
// --------------------------------------------------------------------------- //

DBOOL ConnectToTcpIpAddress(CClientDE* pClientDE, char* sAddress)
{
	// Sanity checks...

	if (!pClientDE) return(DFALSE);
	if (!sAddress) return(DFALSE);


	// Try to connect to the given address...

	DBOOL db = NetStart_DoConsoleConnect(pClientDE, sAddress);

	if (!db)
	{
		if (strlen(sAddress) <= 0) pClientDE->CPrint("Unable to connect");
		else pClientDE->CPrint("Unable to connect to %s", sAddress);
		return(DFALSE);
	}


	// All done...

	if (strlen(sAddress) > 0) pClientDE->CPrint("Connected to %s", sAddress);
	else pClientDE->CPrint("Connected");

	return(DTRUE);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	NVModelHook
//
//	PURPOSE:	Special Rendering Code for NightVision Powerup
//
// --------------------------------------------------------------------------- //

void NVModelHook (struct ModelHookData_t *pData, void *pUser)
{
	CRiotClientShell* pShell = (CRiotClientShell*) pUser;
	if (!pShell) return;
	
	CClientDE* pClientDE = (CClientDE*) pShell->GetClientDE();
	if (!pClientDE) return;
	
	DDWORD nUserFlags = 0;
	pClientDE->GetObjectUserFlags (pData->m_hObject, &nUserFlags);
	if (nUserFlags & USRFLG_NIGHT_INFRARED)
	{
		pData->m_Flags &= ~MHF_USETEXTURE;
		if (pData->m_LightAdd)
		{
			VEC_SET (*pData->m_LightAdd, 0.0f, 255.0f, 0.0f);
		}
	}
	else
	{
		DefaultModelHook(pData, pUser);
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	IRModelHook
//
//	PURPOSE:	Special Rendering Code for Infrared Powerup
//
// --------------------------------------------------------------------------- //

void IRModelHook (struct ModelHookData_t *pData, void *pUser)
{
	CRiotClientShell* pShell = (CRiotClientShell*) pUser;
	if (!pShell) return;
	
	CClientDE* pClientDE = (CClientDE*) pShell->GetClientDE();
	if (!pClientDE) return;

	DDWORD nUserFlags = 0;
	pClientDE->GetObjectUserFlags (pData->m_hObject, &nUserFlags);
	if (nUserFlags & USRFLG_NIGHT_INFRARED)
	{
		pData->m_Flags &= ~MHF_USETEXTURE;
		if (pData->m_LightAdd)
		{
			VEC_SET (*pData->m_LightAdd, 255.0f, 64.0f, 64.0f);
		}
	}
	else
	{
		DefaultModelHook(pData, pUser);
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	DefaultModelHook
//
//	PURPOSE:	Default model hook function
//
// --------------------------------------------------------------------------- //

void DefaultModelHook (struct ModelHookData_t *pData, void *pUser)
{
	CRiotClientShell* pShell = (CRiotClientShell*) pUser;
	if (!pShell) return;
	
	CClientDE* pClientDE = (CClientDE*) pShell->GetClientDE();
	if (!pClientDE) return;

	DDWORD nUserFlags = 0;
	pClientDE->GetObjectUserFlags (pData->m_hObject, &nUserFlags);

	if (nUserFlags & USRFLG_GLOW)
	{
		// MD {Updates model glow in Update}
		//pShell->UpdateModelGlow(vColor);

		if (pData->m_LightAdd)
		{
			*pData->m_LightAdd = pShell->GetModelGlow();
		}
	}
	else if (nUserFlags & USRFLG_MODELADD)
	{
		// Get the new color out of the upper 3 bytes of the 
		// user flags...

		DFLOAT r = (DFLOAT)(nUserFlags>>24);
		DFLOAT g = (DFLOAT)(nUserFlags>>16);
		DFLOAT b = (DFLOAT)(nUserFlags>>8);

		if (pData->m_LightAdd)
		{
			VEC_SET (*pData->m_LightAdd, r, g, b);
		}
	}
}




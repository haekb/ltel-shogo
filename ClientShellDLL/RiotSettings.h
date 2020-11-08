#ifndef __RIOTSETTINGS_H
#define __RIOTSETTINGS_H

//***********************************************
//
//		Structure and Class Definitions
//
//***********************************************

class CRiotClientShell;
class CRiotMenu;

// Settings struct...

struct Setting
{
	Setting()		{ bStringVar = DFALSE; memset (strVarName, 0, 32); hVar = DNULL; nValue = 0.0f; memset (strValue, 0, 32); }

	DBOOL			bStringVar;

	char			strVarName[32];
	HCONSOLEVAR		hVar;
	
	float			nValue;
	char			strValue[32];
};

// Values for 3 position options..

#define RS_LOW						0
#define	RS_MED						1
#define RS_HIGH						2

// MISC SETTINGS...

#define RS_MISC_FIRST				0
#define RS_MISC_VEHICLEMODE			0
#define RS_MISC_SCREENFLASH			1
#define RS_MISC_LAST				1

// CONTROL SETTINGS...

#define RS_CTRL_FIRST				0
#define RS_CTRL_MOUSELOOK			0
#define RS_CTRL_MOUSEINVERTY		1
#define RS_CTRL_MOUSESENSITIVITY	2
#define RS_CTRL_MOUSEINPUTRATE		3
#define RS_CTRL_JOYLOOK				4
#define RS_CTRL_JOYINVERTY			5
#define RS_CTRL_LOOKSPRING			6
#define RS_CTRL_RUNLOCK				7
#define RS_CTRL_LAST				7

// SOUND SETTINGS...

#define RS_SND_FIRST				0
#define RS_SND_MUSICENABLED			0
#define RS_SND_MUSICVOL				1
#define RS_SND_FX					2
#define RS_SND_SOUNDVOL				3
#define RS_SND_CHANNELS				4
#define RS_SND_16BIT				5
#define RS_SND_LAST					5

// TOP-LEVEL DETAIL SETTINGS...

#define RS_DET_FIRST				0
#define RS_DET_OVERALL				0
#define RS_DET_GORE					1
#define RS_DET_LAST					1

// LOW-LEVEL DETAIL SETTINGS...

#define RS_SUBDET_FIRST				0
#define RS_SUBDET_MODELLOD			0
#define RS_SUBDET_SHADOWS			1
#define RS_SUBDET_BULLETHOLES		2
#define RS_SUBDET_TEXTUREDETAIL		3
#define RS_SUBDET_DYNAMICLIGHTING	4
#define RS_SUBDET_LIGHTMAPPING		5
#define RS_SUBDET_SPECIALFX			6
#define RS_SUBDET_ENVMAPPING		7
#define RS_SUBDET_MODELFB			8
#define RS_SUBDET_CLOUDMAPLIGHT		9
#define RS_SUBDET_PVWEAPONS			10
#define RS_SUBDET_POLYGRIDS			11
#define RS_SUBDET_LAST				11


//***********************************************
//
//		Class Definition
//
//***********************************************

class CRiotSettings
{

public:

	CRiotSettings();
	~CRiotSettings()	{}

	DBOOL		Init (CClientDE* pClientDE, CRiotClientShell* pClientShell);
	
	// misc access functions

	DBOOL		VehicleMode()						{ return (DBOOL)Misc[RS_MISC_VEHICLEMODE].nValue; }
	DBOOL		ScreenFlash()						{ return (DBOOL)Misc[RS_MISC_SCREENFLASH].nValue; }
	
	// control access functions

	DBOOL		MouseLook()							{ return (DBOOL)Control[RS_CTRL_MOUSELOOK].nValue; }
	DBOOL		MouseInvertY()						{ return (DBOOL)Control[RS_CTRL_MOUSEINVERTY].nValue; }
	void		SetMouseLook(DBOOL bVal)			{ Control[RS_CTRL_MOUSELOOK].nValue = bVal; }
	void		SetMouseInvertY(DBOOL bVal)			{ Control[RS_CTRL_MOUSEINVERTY].nValue = bVal; }

	float&		ControlSetting(DDWORD val)			{ return Control[val].nValue; }
	float&		DetailSetting(DDWORD val)			{ return Detail[val].nValue; }
	float&		SubDetailSetting(DDWORD val)		{ return SubDetail[val].nValue; }

	float		MouseSensitivity()					{ return		Control[RS_CTRL_MOUSESENSITIVITY].nValue; }
	DBOOL		JoyLook()							{ return (DBOOL)Control[RS_CTRL_JOYLOOK].nValue; }
	DBOOL		JoyInvertY()						{ return (DBOOL)Control[RS_CTRL_JOYINVERTY].nValue; }
	DBOOL		Lookspring()						{ return (DBOOL)Control[RS_CTRL_LOOKSPRING].nValue; }
	DBOOL		RunLock()							{ return (DBOOL)Control[RS_CTRL_RUNLOCK].nValue; }
	
	void		SetRunLock (DBOOL bRunLock)			{ Control[RS_CTRL_RUNLOCK].nValue = bRunLock ? 1.0f : 0.0f; }

	// sound access functions

	DBOOL		MusicEnabled()						{ return (DBOOL)Sound[RS_SND_MUSICENABLED].nValue; }
	float		MusicVolume()						{ return		Sound[RS_SND_MUSICVOL].nValue; }
	DBOOL		SoundEnabled()						{ return (DBOOL)Sound[RS_SND_FX].nValue; }
	float		SoundVolume()						{ return		Sound[RS_SND_SOUNDVOL].nValue; }
	float		SoundChannels()						{ return		Sound[RS_SND_CHANNELS].nValue; }
	DBOOL		Sound16Bit()						{ return (DBOOL)Sound[RS_SND_16BIT].nValue; }

	// top-level detail access functions

	float		GlobalDetail()						{ return		Detail[RS_DET_OVERALL].nValue; }
	DBOOL		Gore()								
	{ 
		if (m_bAllowGore)
		{
			return (DBOOL)Detail[RS_DET_GORE].nValue;
		}
		else
		{
			return DFALSE;  // Never allow gore
		}
	}

	// low-level detail access functions

	float		ModelLOD()							{ return		SubDetail[RS_SUBDET_MODELLOD].nValue; }
	DBOOL		Shadows()							{ return (DBOOL)SubDetail[RS_SUBDET_SHADOWS].nValue; }
	float		NumBulletHoles()					{ return		SubDetail[RS_SUBDET_BULLETHOLES].nValue; }
	float		TextureDetailSetting()				{ return		SubDetail[RS_SUBDET_TEXTUREDETAIL].nValue; }
	float		DynamicLightSetting()				{ return		SubDetail[RS_SUBDET_DYNAMICLIGHTING].nValue; }
	DBYTE		SpecialFXSetting()					{ return (DBYTE)SubDetail[RS_SUBDET_SPECIALFX].nValue; }
	DBOOL		EnvironmentMapping()				{ return (DBOOL)SubDetail[RS_SUBDET_ENVMAPPING].nValue; }
	DBOOL		ModelFullBrights()					{ return (DBOOL)SubDetail[RS_SUBDET_MODELFB].nValue; }
	DBOOL		CloudMapLight()						{ return (DBOOL)SubDetail[RS_SUBDET_CLOUDMAPLIGHT].nValue; }
	DBYTE		PlayerViewWeaponSetting()			{ return (DBYTE)SubDetail[RS_SUBDET_PVWEAPONS].nValue; }
	DBOOL		PolyGrids()							{ return (DBOOL)SubDetail[RS_SUBDET_POLYGRIDS].nValue; }

	// display mode access functions

	RMode*		GetRenderMode()						{ return &CurrentRenderer; }
	DBOOL		Textures8Bit()						{ return (MasterPaletteMode.nValue == 1.0f) ? DTRUE : DFALSE; }

	// read/write functions
	
	DBOOL		ReadSettings();
	void		WriteSettings();

	void		ReadMiscSettings();
	void		WriteMiscSettings();

	void		ReadControlSettings();
	void		WriteControlSettings();

	void		ReadSoundSettings();
	void		WriteSoundSettings();

	void		ReadDetailSettings();
	void		WriteDetailSettings();

	void		ReadSubDetailSettings();
	void		WriteSubDetailSettings();

	void		ReadDisplayModeSettings();
	void		WriteDisplayModeSettings();
	
	// utility functions

	void		SetLowDetail();
	void		SetMedDetail();
	void		SetHiDetail();

	// settings implementation functions

	DBOOL		ImplementRendererSetting();
	void		ImplementMusicSource();
	void		ImplementMusicVolume();
	void		ImplementSoundEnabled();
	void		ImplementSoundVolume();
	void		ImplementSoundQuality();
	void		ImplementMouseSensitivity();
	void		ImplementInputRate();
	void		ImplementDetailSetting (int nSetting);
	void		ImplementBitDepth();

public:

	CClientDE*			m_pClientDE;
	CRiotClientShell*	m_pClientShell;

	// Default Low, Medium, and High Settings...

	Setting		DefLow[RS_SUBDET_LAST + 1];
	Setting		DefMed[RS_SUBDET_LAST + 1];
	Setting		DefHi[RS_SUBDET_LAST + 1];

	// Misc Settings...

	Setting		Misc[RS_MISC_LAST + 1];
	
	// Control Settings...

	Setting		Control[RS_CTRL_LAST + 1];

	// Sound Settings...

	Setting		Sound[RS_SND_LAST + 1];

	// Renderer Settings...

	RMode		CurrentRenderer;
	Setting		MasterPaletteMode;

	// Top-Level Detail Settings...

	Setting		Detail[RS_DET_LAST + 1];

	// Low-Level Detail Settings...

	Setting		SubDetail[RS_SUBDET_LAST + 1];

	// Allow any gore (even display of the option in game)
	DBOOL		m_bAllowGore;

};

#endif
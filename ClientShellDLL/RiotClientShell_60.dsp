# Microsoft Developer Studio Project File - Name="RiotClientShell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=RiotClientShell - Win32 Demo Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RiotClientShell.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RiotClientShell.mak" CFG="RiotClientShell - Win32 Demo Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RiotClientShell - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RiotClientShell - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RiotClientShell - Win32 Demo Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RiotClientShell - Win32 Demo Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RiotClientShell - Win32 Korean Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RiotClientShell - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\appheaders" /I "..\shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "RIOT_BUILD" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\Shogo\CShell.dll"

!ELSEIF  "$(CFG)" == "RiotClientShell - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\shared" /I "..\appheaders" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "RIOT_BUILD" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Shogo\CShell.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying                                                                                                                                                                    ClientShell.pdb                                                                                                                                                                                          to                                                                                                                                                    ..\ 
PostBuild_Cmds=copy Debug\CShell.pdb ..\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "RiotClientShell - Win32 Demo Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "RiotCli1"
# PROP BASE Intermediate_Dir "RiotCli1"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "RiotCli1"
# PROP Intermediate_Dir "RiotCli1"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\appheaders" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "RIOT_BUILD" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\appheaders" /I "..\shared" /D "_DEBUG" /D "_DEMO" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "RIOT_BUILD" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Shogo\CShell.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"\Games\ShogoDemo\ShogoDem\CShell.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying                                                                                                                                                                    ClientShell.pdb                                                                                                                                                                                          to                                                                                                                                                    ..\ 
PostBuild_Cmds=copy Debug\CShell.pdb ..\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "RiotClientShell - Win32 Demo Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RiotCli2"
# PROP BASE Intermediate_Dir "RiotCli2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RiotCli2"
# PROP Intermediate_Dir "RiotCli2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\appheaders" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "RIOT_BUILD" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\appheaders" /I "..\shared" /D "NDEBUG" /D "_DEMO" /D "WIN32" /D "_WINDOWS" /D "RIOT_BUILD" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\Shogo\CShell.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"\Games\ShogoDemo\ShogoDem\CShell.dll"

!ELSEIF  "$(CFG)" == "RiotClientShell - Win32 Korean Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RiotCli3"
# PROP BASE Intermediate_Dir "RiotCli3"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RiotCli3"
# PROP Intermediate_Dir "RiotCli3"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\appheaders" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "RIOT_BUILD" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\appheaders" /I "..\shared" /D "NDEBUG" /D "_KOREAN" /D "WIN32" /D "_WINDOWS" /D "RIOT_BUILD" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\Shogo\CShell.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\Shogo\CShell.dll"

!ENDIF 

# Begin Target

# Name "RiotClientShell - Win32 Release"
# Name "RiotClientShell - Win32 Debug"
# Name "RiotClientShell - Win32 Demo Debug"
# Name "RiotClientShell - Win32 Demo Release"
# Name "RiotClientShell - Win32 Korean Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c"
# Begin Group "Menu Stuff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BaseMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailSettingsMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayModeMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayOptionsMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\JoystickMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyboardMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadLevelMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadSavedLevelMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\MainMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGameMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveLevelMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\SinglePlayerMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundOptionsMenu.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AnimeLineFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseLineSystemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseParticleSystemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseScaleFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapFont.cpp
# End Source File
# Begin Source File

SOURCE=.\BulletTrailFX.cpp
# End Source File
# Begin Source File

SOURCE=.\CastLineFX.cpp
# End Source File
# Begin Source File

SOURCE=.\client_physics.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientInfoMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\CMoveMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Credits.cpp
# End Source File
# Begin Source File

SOURCE=.\CreditsWin.cpp
# End Source File
# Begin Source File

SOURCE=.\CriticalHitFX.cpp
# End Source File
# Begin Source File

SOURCE=.\DeathFX.cpp
# End Source File
# Begin Source File

SOURCE=.\DebrisFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DebrisTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicLightFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ExplosionFX.cpp
# End Source File
# Begin Source File

SOURCE=.\GibFX.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\IpMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\JuggernautFX.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyFixes.cpp
# End Source File
# Begin Source File

SOURCE=.\LaserCannonFX.cpp
# End Source File
# Begin Source File

SOURCE=.\LightFX.cpp
# End Source File
# Begin Source File

SOURCE=.\LightScaleMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\LineBallFX.cpp
# End Source File
# Begin Source File

SOURCE=.\MarkSFX.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MissionObjectives.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ModelFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\Music.cpp
# End Source File
# Begin Source File

SOURCE=.\NetInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\NetStart.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleExplosionFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleTrailFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleTrailSegmentFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PickupItemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\plasma1.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerInventory.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerStats.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyGridFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectileFX.cpp
# End Source File
# Begin Source File

SOURCE=.\RiotClientShell.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiotCommonUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\RiotMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\RiotSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\RocketFlareFX.cpp
# End Source File
# Begin Source File

SOURCE=.\SFXMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SFXReg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellCasingFX.cpp
# End Source File
# Begin Source File

SOURCE=.\Slider.cpp
# End Source File
# Begin Source File

SOURCE=.\SmokeFX.cpp
# End Source File
# Begin Source File

SOURCE=.\SmokeSphereFX.cpp
# End Source File
# Begin Source File

SOURCE=.\Sparam.cpp
# End Source File
# Begin Source File

SOURCE=.\SparksFX.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteFX.cpp
# End Source File
# Begin Source File

SOURCE=.\TextHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\TracerFX.cpp
# End Source File
# Begin Source File

SOURCE=.\VKDefs.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WeaponDefs.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponFX.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponFXLow.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponFXMed.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponModel.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponSoundFX.cpp
# End Source File
# Begin Source File

SOURCE=.\WinUtil.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "hpp;h"
# Begin Source File

SOURCE=.\AnimeLineFX.h
# End Source File
# Begin Source File

SOURCE=.\BaseLineSystemFX.h
# End Source File
# Begin Source File

SOURCE=.\BaseMenu.h
# End Source File
# Begin Source File

SOURCE=.\BaseParticleSystemFX.h
# End Source File
# Begin Source File

SOURCE=.\BaseScaleFX.h
# End Source File
# Begin Source File

SOURCE=.\BitmapFont.h
# End Source File
# Begin Source File

SOURCE=.\BulletTrailFX.h
# End Source File
# Begin Source File

SOURCE=.\CameraFX.h
# End Source File
# Begin Source File

SOURCE=.\CastLineFX.h
# End Source File
# Begin Source File

SOURCE=.\CharacterAlignment.h
# End Source File
# Begin Source File

SOURCE=.\CheatDefs.h
# End Source File
# Begin Source File

SOURCE=.\client_physics.h
# End Source File
# Begin Source File

SOURCE=.\ClientInfoMgr.h
# End Source File
# Begin Source File

SOURCE=.\ClientRes.h
# End Source File
# Begin Source File

SOURCE=.\ClientServerShared.h
# End Source File
# Begin Source File

SOURCE=.\ClientUtilities.h
# End Source File
# Begin Source File

SOURCE=.\CMoveMgr.h
# End Source File
# Begin Source File

SOURCE=.\ContainerCodes.h
# End Source File
# Begin Source File

SOURCE=.\ControlsMenu.h
# End Source File
# Begin Source File

SOURCE=.\CriticalHitFX.h
# End Source File
# Begin Source File

SOURCE=.\CustomSettingsMenu.h
# End Source File
# Begin Source File

SOURCE=.\DeathFX.h
# End Source File
# Begin Source File

SOURCE=.\DebrisFX.h
# End Source File
# Begin Source File

SOURCE=.\DebrisTypes.h
# End Source File
# Begin Source File

SOURCE=.\DisplayOptionsMenu.h
# End Source File
# Begin Source File

SOURCE=.\DynamicLightFX.h
# End Source File
# Begin Source File

SOURCE=.\DynArray.h
# End Source File
# Begin Source File

SOURCE=.\ExplosionFX.h
# End Source File
# Begin Source File

SOURCE=.\Font08.h
# End Source File
# Begin Source File

SOURCE=.\Font12.h
# End Source File
# Begin Source File

SOURCE=.\Font28.h
# End Source File
# Begin Source File

SOURCE=.\GibFX.h
# End Source File
# Begin Source File

SOURCE=.\GibTypes.h
# End Source File
# Begin Source File

SOURCE=.\JoystickMenu.h
# End Source File
# Begin Source File

SOURCE=.\JuggernautFX.h
# End Source File
# Begin Source File

SOURCE=.\KeyboardMenu.h
# End Source File
# Begin Source File

SOURCE=.\LaserCannonFX.h
# End Source File
# Begin Source File

SOURCE=.\LightFX.h
# End Source File
# Begin Source File

SOURCE=.\LineBallFX.h
# End Source File
# Begin Source File

SOURCE=.\LoadLevelMenu.h
# End Source File
# Begin Source File

SOURCE=.\MainMenu.h
# End Source File
# Begin Source File

SOURCE=.\MarkSFX.h
# End Source File
# Begin Source File

SOURCE=.\MessageMgr.h
# End Source File
# Begin Source File

SOURCE=.\MissionObjectives.h
# End Source File
# Begin Source File

SOURCE=.\ModelFuncs.h
# End Source File
# Begin Source File

SOURCE=.\ModelIds.h
# End Source File
# Begin Source File

SOURCE=.\MouseMenu.h
# End Source File
# Begin Source File

SOURCE=.\Music.h
# End Source File
# Begin Source File

SOURCE=.\NetInfo.h
# End Source File
# Begin Source File

SOURCE=.\NetStart.h
# End Source File
# Begin Source File

SOURCE=.\OptionsMenu.h
# End Source File
# Begin Source File

SOURCE=.\ParticleExplosionFX.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemFX.h
# End Source File
# Begin Source File

SOURCE=.\ParticleTrailFX.h
# End Source File
# Begin Source File

SOURCE=.\ParticleTrailSegmentFX.h
# End Source File
# Begin Source File

SOURCE=.\PickupItemFX.h
# End Source File
# Begin Source File

SOURCE=.\plasma.h
# End Source File
# Begin Source File

SOURCE=.\PlayerCamera.h
# End Source File
# Begin Source File

SOURCE=.\PlayerFX.h
# End Source File
# Begin Source File

SOURCE=.\PlayerInventory.h
# End Source File
# Begin Source File

SOURCE=.\PlayerModeTypes.h
# End Source File
# Begin Source File

SOURCE=.\PlayerStats.h
# End Source File
# Begin Source File

SOURCE=.\PolyGridFX.h
# End Source File
# Begin Source File

SOURCE=.\PopupMenu.h
# End Source File
# Begin Source File

SOURCE=.\PopupMenuIDs.h
# End Source File
# Begin Source File

SOURCE=.\ProjectileFX.h
# End Source File
# Begin Source File

SOURCE=.\ProjectileFXTypes.h
# End Source File
# Begin Source File

SOURCE=.\RiotClientShell.h
# End Source File
# Begin Source File

SOURCE=.\RiotCommandIDs.h
# End Source File
# Begin Source File

SOURCE=.\RiotCommonUtilities.h
# End Source File
# Begin Source File

SOURCE=.\RiotMenu.h
# End Source File
# Begin Source File

SOURCE=.\RiotMsgIDs.h
# End Source File
# Begin Source File

SOURCE=.\RiotSettings.h
# End Source File
# Begin Source File

SOURCE=.\RiotSoundTypes.h
# End Source File
# Begin Source File

SOURCE=.\RiotWeaponModels.h
# End Source File
# Begin Source File

SOURCE=.\RocketFlareFX.h
# End Source File
# Begin Source File

SOURCE=.\SFXMgr.h
# End Source File
# Begin Source File

SOURCE=.\SFXMsgIds.h
# End Source File
# Begin Source File

SOURCE=.\ShellCasingFX.h
# End Source File
# Begin Source File

SOURCE=.\SinglePlayerMenu.h
# End Source File
# Begin Source File

SOURCE=.\Slider.h
# End Source File
# Begin Source File

SOURCE=.\SmokeFX.h
# End Source File
# Begin Source File

SOURCE=.\SmokeSphereFX.h
# End Source File
# Begin Source File

SOURCE=.\SoundOptionsMenu.h
# End Source File
# Begin Source File

SOURCE=.\SparksFX.h
# End Source File
# Begin Source File

SOURCE=.\SpecialFX.h
# End Source File
# Begin Source File

SOURCE=.\SpecialFXList.h
# End Source File
# Begin Source File

SOURCE=.\SpriteFX.h
# End Source File
# Begin Source File

SOURCE=.\SurfaceTypes.h
# End Source File
# Begin Source File

SOURCE=.\TextHelper.h
# End Source File
# Begin Source File

SOURCE=.\TracerFX.h
# End Source File
# Begin Source File

SOURCE=.\VKDefs.h
# End Source File
# Begin Source File

SOURCE=.\VolumeBrushFX.h
# End Source File
# Begin Source File

SOURCE=.\WeaponDefs.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WeaponDefs.h
# End Source File
# Begin Source File

SOURCE=.\WeaponFX.h
# End Source File
# Begin Source File

SOURCE=.\WeaponFXTypes.h
# End Source File
# Begin Source File

SOURCE=.\WeaponModel.h
# End Source File
# Begin Source File

SOURCE=.\WeaponSoundFX.h
# End Source File
# Begin Source File

SOURCE=.\WinUtil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ClientShellDLL.rc
# End Source File
# End Target
# End Project

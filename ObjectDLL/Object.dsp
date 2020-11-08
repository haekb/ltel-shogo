# Microsoft Developer Studio Project File - Name="Object" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Object - Win32 Demo Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Object.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Object.mak" CFG="Object - Win32 Demo Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Object - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Object - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Object - Win32 Demo Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Object - Win32 Demo Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Object - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\appheaders" /I "..\shared" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "RIOT_BUILD" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\Shogo\Object.lto"

!ELSEIF  "$(CFG)" == "Object - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\appheaders" /I "..\shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "RIOT_BUILD" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Shogo\Object.lto" /pdbtype:sept
# Begin Special Build Tool
SOURCE=$(InputPath)
PostBuild_Desc=Copying \
                                                                                                                                                                                              Object.pdb \
                                                                                                                                                                                                                       to \
                                                                                                                                                                                   ..\ 
PostBuild_Cmds=copy Debug\Object.pdb ..\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Object - Win32 Demo Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Object_1"
# PROP BASE Intermediate_Dir "Object_1"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Object_1"
# PROP Intermediate_Dir "Object_1"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\appheaders" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "RIOT_BUILD" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\appheaders" /I "..\shared" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "RIOT_BUILD" /D "_DEMO" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\Shogo\Object.lto"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"\games\shogodemo\shogodem\Object.lto"

!ELSEIF  "$(CFG)" == "Object - Win32 Demo Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Object_2"
# PROP BASE Intermediate_Dir "Object_2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Object_2"
# PROP Intermediate_Dir "Object_2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\appheaders" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "RIOT_BUILD" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\appheaders" /I "..\shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "RIOT_BUILD" /D "_DEMO" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Shogo\Object.lto" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"\games\shogodemo\shogodem\Object.lto" /pdbtype:sept
# Begin Special Build Tool
SOURCE=$(InputPath)
PostBuild_Desc=Copying \
                                                                                                                                                                                              Object.pdb \
                                                                                                                                                                                                                       to \
                                                                                                                                                                                   ..\ 
PostBuild_Cmds=copy Debug\Object.pdb ..\*.*
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Object - Win32 Release"
# Name "Object - Win32 Debug"
# Name "Object - Win32 Demo Release"
# Name "Object - Win32 Demo Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c"
# Begin Source File

SOURCE=.\Activation.cpp
# End Source File
# Begin Source File

SOURCE=.\Admiral.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSound.cpp
# End Source File
# Begin Source File

SOURCE=.\AIKey.cpp
# End Source File
# Begin Source File

SOURCE=.\AIKeyData.cpp
# End Source File
# Begin Source File

SOURCE=.\AIPathMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\AISounds.cpp
# End Source File
# Begin Source File

SOURCE=.\Akuma.cpp
# End Source File
# Begin Source File

SOURCE=.\Andra10.cpp
# End Source File
# Begin Source File

SOURCE=.\Andra5.cpp
# End Source File
# Begin Source File

SOURCE=.\ArmorBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ArmorRepair.cpp
# End Source File
# Begin Source File

SOURCE=.\Assassin.cpp
# End Source File
# Begin Source File

SOURCE=.\AVC.cpp
# End Source File
# Begin Source File

SOURCE=.\Baku.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseAI.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\BodyArmor.cpp
# End Source File
# Begin Source File

SOURCE=.\BodyProp.cpp
# End Source File
# Begin Source File

SOURCE=.\Bouncer.cpp
# End Source File
# Begin Source File

SOURCE=.\CachedFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Cat.cpp
# End Source File
# Begin Source File

SOURCE=.\CharacterAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\CharacterMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Civilian1.cpp
# End Source File
# Begin Source File

SOURCE=.\Civilian2.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientCastLineSFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientDeathSFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientLightFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponSFX.cpp
# End Source File
# Begin Source File

SOURCE=.\Cothineal.cpp
# End Source File
# Begin Source File

SOURCE=..\appheaders\cpp_engineobjects_de.cpp
# End Source File
# Begin Source File

SOURCE=.\DebrisFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DebrisTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\Destructable.cpp
# End Source File
# Begin Source File

SOURCE=.\DestructableDoor.cpp
# End Source File
# Begin Source File

SOURCE=.\DestructableRotatingDoor.cpp
# End Source File
# Begin Source File

SOURCE=.\DestructableWorldModel.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Door.cpp
# End Source File
# Begin Source File

SOURCE=.\Dropship.cpp
# End Source File
# Begin Source File

SOURCE=.\EliteShockTrooper.cpp
# End Source File
# Begin Source File

SOURCE=.\EliteTrooper.cpp
# End Source File
# Begin Source File

SOURCE=.\Enforcer.cpp
# End Source File
# Begin Source File

SOURCE=.\EnhancementItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Enhancements.cpp
# End Source File
# Begin Source File

SOURCE=.\ExitTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Explosion.cpp
# End Source File
# Begin Source File

SOURCE=.\FirstAid.cpp
# End Source File
# Begin Source File

SOURCE=.\FirstAidBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Gabriel.cpp
# End Source File
# Begin Source File

SOURCE=.\GameStartPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\GlassProp.cpp
# End Source File
# Begin Source File

SOURCE=.\Hammerhead.cpp
# End Source File
# Begin Source File

SOURCE=.\Hank.cpp
# End Source File
# Begin Source File

SOURCE=.\Inventory.cpp
# End Source File
# Begin Source File

SOURCE=.\InventoryItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Kathryn.cpp
# End Source File
# Begin Source File

SOURCE=.\Key.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyData.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyFramer.cpp
# End Source File
# Begin Source File

SOURCE=.\Kura.cpp
# End Source File
# Begin Source File

SOURCE=.\LaserTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Lift.cpp
# End Source File
# Begin Source File

SOURCE=.\LittleKid.cpp
# End Source File
# Begin Source File

SOURCE=.\MajorCharacter.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ModelFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\Music.cpp
# End Source File
# Begin Source File

SOURCE=.\object_list.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectiveTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Officer.cpp
# End Source File
# Begin Source File

SOURCE=.\Ordog.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\PickupItem.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerMode.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerObj.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\PowerSurge.cpp
# End Source File
# Begin Source File

SOURCE=.\Powerup.cpp
# End Source File
# Begin Source File

SOURCE=.\Predator.cpp
# End Source File
# Begin Source File

SOURCE=.\Projectile.cpp
# End Source File
# Begin Source File

SOURCE=.\Prop.cpp
# End Source File
# Begin Source File

SOURCE=.\PVWeaponModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Raksha.cpp
# End Source File
# Begin Source File

SOURCE=.\Rascal.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiotCommonUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\RiotObjectUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\RiotProjectiles.cpp
# End Source File
# Begin Source File

SOURCE=.\RiotServerShell.cpp
# End Source File
# Begin Source File

SOURCE=.\RiotWeapons.cpp
# End Source File
# Begin Source File

SOURCE=.\RotatingDoor.cpp
# End Source File
# Begin Source File

SOURCE=.\RotatingWorldModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Ruin150.cpp
# End Source File
# Begin Source File

SOURCE=.\Ryo.cpp
# End Source File
# Begin Source File

SOURCE=.\Samantha.cpp
# End Source File
# Begin Source File

SOURCE=.\Sanjuro.cpp
# End Source File
# Begin Source File

SOURCE=.\ScaleSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\ShockTrooper.cpp
# End Source File
# Begin Source File

SOURCE=.\ShogoSecret.cpp
# End Source File
# Begin Source File

SOURCE=.\Sparam.cpp
# End Source File
# Begin Source File

SOURCE=.\Sparrowhawk.cpp
# End Source File
# Begin Source File

SOURCE=.\Spawner.cpp
# End Source File
# Begin Source File

SOURCE=.\SSFXReg.cpp
# End Source File
# Begin Source File

SOURCE=.\StoryTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Tenma.cpp
# End Source File
# Begin Source File

SOURCE=.\Toshiro.cpp
# End Source File
# Begin Source File

SOURCE=.\TractorBeam.cpp
# End Source File
# Begin Source File

SOURCE=.\TransmissionTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger.cpp
# End Source File
# Begin Source File

SOURCE=.\TriggerSound.cpp
# End Source File
# Begin Source File

SOURCE=.\Trooper.cpp
# End Source File
# Begin Source File

SOURCE=.\UhlanA3.cpp
# End Source File
# Begin Source File

SOURCE=.\UltraPowerupItem.cpp
# End Source File
# Begin Source File

SOURCE=.\UltraPowerups.cpp
# End Source File
# Begin Source File

SOURCE=.\UpgradeItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Upgrades.cpp
# End Source File
# Begin Source File

SOURCE=.\Vandal.cpp
# End Source File
# Begin Source File

SOURCE=.\Vehicle.cpp
# End Source File
# Begin Source File

SOURCE=.\Vigilance.cpp
# End Source File
# Begin Source File

SOURCE=.\VolumeBrush.cpp
# End Source File
# Begin Source File

SOURCE=.\VolumeBrushTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\Weapon.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WeaponDefs.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponPowerups.cpp
# End Source File
# Begin Source File

SOURCE=.\Weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldModelDebris.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldProperties.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp"
# Begin Source File

SOURCE=.\Activation.h
# End Source File
# Begin Source File

SOURCE=.\Admiral.h
# End Source File
# Begin Source File

SOURCE=.\AdvSound.h
# End Source File
# Begin Source File

SOURCE=.\AIKey.h
# End Source File
# Begin Source File

SOURCE=.\AIKeyData.h
# End Source File
# Begin Source File

SOURCE=.\AIPathList.h
# End Source File
# Begin Source File

SOURCE=.\AIPathMgr.h
# End Source File
# Begin Source File

SOURCE=.\AIScriptList.h
# End Source File
# Begin Source File

SOURCE=.\AISounds.h
# End Source File
# Begin Source File

SOURCE=.\Akuma.h
# End Source File
# Begin Source File

SOURCE=.\Andra10.h
# End Source File
# Begin Source File

SOURCE=.\Andra5.h
# End Source File
# Begin Source File

SOURCE=.\ArmorBase.h
# End Source File
# Begin Source File

SOURCE=.\ArmorRepair.h
# End Source File
# Begin Source File

SOURCE=.\Assassin.h
# End Source File
# Begin Source File

SOURCE=.\AVC.h
# End Source File
# Begin Source File

SOURCE=.\Baku.h
# End Source File
# Begin Source File

SOURCE=.\BaseAI.h
# End Source File
# Begin Source File

SOURCE=.\BaseCharacter.h
# End Source File
# Begin Source File

SOURCE=.\BodyArmor.h
# End Source File
# Begin Source File

SOURCE=.\BodyProp.h
# End Source File
# Begin Source File

SOURCE=.\Bouncer.h
# End Source File
# Begin Source File

SOURCE=.\CachedFiles.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Cat.h
# End Source File
# Begin Source File

SOURCE=.\CharacterAlignment.h
# End Source File
# Begin Source File

SOURCE=.\CharacterAttributes.h
# End Source File
# Begin Source File

SOURCE=.\CharacterMgr.h
# End Source File
# Begin Source File

SOURCE=.\CheatDefs.h
# End Source File
# Begin Source File

SOURCE=.\Civilian1.h
# End Source File
# Begin Source File

SOURCE=.\Civilian2.h
# End Source File
# Begin Source File

SOURCE=.\ClientCastLineSFX.h
# End Source File
# Begin Source File

SOURCE=.\ClientDeathSFX.h
# End Source File
# Begin Source File

SOURCE=.\ClientLightFX.h
# End Source File
# Begin Source File

SOURCE=.\ClientServerShared.h
# End Source File
# Begin Source File

SOURCE=.\ClientSFX.h
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponSFX.h
# End Source File
# Begin Source File

SOURCE=.\ContainerCodes.h
# End Source File
# Begin Source File

SOURCE=.\Cothineal.h
# End Source File
# Begin Source File

SOURCE=.\DamageTypes.h
# End Source File
# Begin Source File

SOURCE=.\DebrisFuncs.h
# End Source File
# Begin Source File

SOURCE=.\DebrisTypes.h
# End Source File
# Begin Source File

SOURCE=.\Destructable.h
# End Source File
# Begin Source File

SOURCE=.\DestructableDoor.h
# End Source File
# Begin Source File

SOURCE=.\DestructableRotatingDoor.h
# End Source File
# Begin Source File

SOURCE=.\DestructableWorldModel.h
# End Source File
# Begin Source File

SOURCE=.\DialogTrigger.h
# End Source File
# Begin Source File

SOURCE=.\Door.h
# End Source File
# Begin Source File

SOURCE=.\Dropship.h
# End Source File
# Begin Source File

SOURCE=.\EliteShockTrooper.h
# End Source File
# Begin Source File

SOURCE=.\EliteTrooper.h
# End Source File
# Begin Source File

SOURCE=.\Enforcer.h
# End Source File
# Begin Source File

SOURCE=.\EnhancementItem.h
# End Source File
# Begin Source File

SOURCE=.\Enhancements.h
# End Source File
# Begin Source File

SOURCE=.\ExitTrigger.h
# End Source File
# Begin Source File

SOURCE=.\Explosion.h
# End Source File
# Begin Source File

SOURCE=.\FirstAid.h
# End Source File
# Begin Source File

SOURCE=.\FirstAidBase.h
# End Source File
# Begin Source File

SOURCE=.\Gabriel.h
# End Source File
# Begin Source File

SOURCE=.\GameStartPoint.h
# End Source File
# Begin Source File

SOURCE=.\GibTypes.h
# End Source File
# Begin Source File

SOURCE=.\GlassProp.h
# End Source File
# Begin Source File

SOURCE=.\HammerHead.h
# End Source File
# Begin Source File

SOURCE=.\Hank.h
# End Source File
# Begin Source File

SOURCE=.\Inventory.h
# End Source File
# Begin Source File

SOURCE=.\InventoryItem.h
# End Source File
# Begin Source File

SOURCE=.\InventoryTypes.h
# End Source File
# Begin Source File

SOURCE=.\Kathryn.h
# End Source File
# Begin Source File

SOURCE=.\Key.h
# End Source File
# Begin Source File

SOURCE=.\KeyData.h
# End Source File
# Begin Source File

SOURCE=.\KeyFramer.h
# End Source File
# Begin Source File

SOURCE=.\Kura.h
# End Source File
# Begin Source File

SOURCE=.\LaserTrigger.h
# End Source File
# Begin Source File

SOURCE=.\Lift.h
# End Source File
# Begin Source File

SOURCE=.\LittleKid.h
# End Source File
# Begin Source File

SOURCE=.\MajorCharacter.h
# End Source File
# Begin Source File

SOURCE=.\ModelFuncs.h
# End Source File
# Begin Source File

SOURCE=.\ModelIds.h
# End Source File
# Begin Source File

SOURCE=.\ModelImpact.h
# End Source File
# Begin Source File

SOURCE=.\ModelNodes.h
# End Source File
# Begin Source File

SOURCE=.\ModelObject.h
# End Source File
# Begin Source File

SOURCE=.\Music.h
# End Source File
# Begin Source File

SOURCE=.\ObjectiveTrigger.h
# End Source File
# Begin Source File

SOURCE=.\ObjectUtilities.h
# End Source File
# Begin Source File

SOURCE=.\Officer.h
# End Source File
# Begin Source File

SOURCE=.\Ordog.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\PickupItem.h
# End Source File
# Begin Source File

SOURCE=.\PlayerMode.h
# End Source File
# Begin Source File

SOURCE=.\PlayerModeTypes.h
# End Source File
# Begin Source File

SOURCE=.\PlayerObj.h
# End Source File
# Begin Source File

SOURCE=.\PolyGrid.h
# End Source File
# Begin Source File

SOURCE=.\PowerSurge.h
# End Source File
# Begin Source File

SOURCE=.\Powerup.h
# End Source File
# Begin Source File

SOURCE=.\Predator.h
# End Source File
# Begin Source File

SOURCE=.\Projectile.h
# End Source File
# Begin Source File

SOURCE=.\ProjectileFXTypes.h
# End Source File
# Begin Source File

SOURCE=.\Prop.h
# End Source File
# Begin Source File

SOURCE=.\PVWeaponModel.h
# End Source File
# Begin Source File

SOURCE=.\Raksha.h
# End Source File
# Begin Source File

SOURCE=.\Rascal.h
# End Source File
# Begin Source File

SOURCE=.\RiotCommandIDs.h
# End Source File
# Begin Source File

SOURCE=.\RiotCommonUtilities.h
# End Source File
# Begin Source File

SOURCE=.\RiotMsgIDs.h
# End Source File
# Begin Source File

SOURCE=.\RiotObjectUtilities.h
# End Source File
# Begin Source File

SOURCE=.\RiotProjectiles.h
# End Source File
# Begin Source File

SOURCE=.\RiotServerShell.h
# End Source File
# Begin Source File

SOURCE=.\RiotSoundTypes.h
# End Source File
# Begin Source File

SOURCE=.\RiotWeapons.h
# End Source File
# Begin Source File

SOURCE=.\RotatingDoor.h
# End Source File
# Begin Source File

SOURCE=.\RotatingWorldModel.h
# End Source File
# Begin Source File

SOURCE=.\Ruin150.h
# End Source File
# Begin Source File

SOURCE=.\Ryo.h
# End Source File
# Begin Source File

SOURCE=.\Samantha.h
# End Source File
# Begin Source File

SOURCE=.\ScaleSprite.h
# End Source File
# Begin Source File

SOURCE=.\SFXMsgIds.h
# End Source File
# Begin Source File

SOURCE=.\ShockTrooper.h
# End Source File
# Begin Source File

SOURCE=.\ShogoSecret.h
# End Source File
# Begin Source File

SOURCE=.\SparrowHawk.h
# End Source File
# Begin Source File

SOURCE=.\Spawner.h
# End Source File
# Begin Source File

SOURCE=.\StoryTrigger.h
# End Source File
# Begin Source File

SOURCE=.\SurfaceFunctions.h
# End Source File
# Begin Source File

SOURCE=.\SurfaceTypes.h
# End Source File
# Begin Source File

SOURCE=.\TemplateList.h
# End Source File
# Begin Source File

SOURCE=.\Tenma.h
# End Source File
# Begin Source File

SOURCE=.\Toshiro.h
# End Source File
# Begin Source File

SOURCE=.\TractorBeam.h
# End Source File
# Begin Source File

SOURCE=.\TransmissionTrigger.h
# End Source File
# Begin Source File

SOURCE=.\Trigger.h
# End Source File
# Begin Source File

SOURCE=.\TriggerSound.h
# End Source File
# Begin Source File

SOURCE=.\Trooper.h
# End Source File
# Begin Source File

SOURCE=.\UhlanA3.h
# End Source File
# Begin Source File

SOURCE=.\UltraPowerupItem.h
# End Source File
# Begin Source File

SOURCE=.\UltraPowerups.h
# End Source File
# Begin Source File

SOURCE=.\UpgradeItem.h
# End Source File
# Begin Source File

SOURCE=.\Upgrades.h
# End Source File
# Begin Source File

SOURCE=.\Vandal.h
# End Source File
# Begin Source File

SOURCE=.\Vehicle.h
# End Source File
# Begin Source File

SOURCE=.\Vigilance.h
# End Source File
# Begin Source File

SOURCE=.\VolumeBrush.h
# End Source File
# Begin Source File

SOURCE=.\VolumeBrushTypes.h
# End Source File
# Begin Source File

SOURCE=.\Warrior.h
# End Source File
# Begin Source File

SOURCE=.\Weapon.h
# End Source File
# Begin Source File

SOURCE=.\WeaponFXTypes.h
# End Source File
# Begin Source File

SOURCE=.\WeaponPowerups.h
# End Source File
# Begin Source File

SOURCE=.\Weapons.h
# End Source File
# Begin Source File

SOURCE=.\WorldModelDebris.h
# End Source File
# End Group
# End Target
# End Project

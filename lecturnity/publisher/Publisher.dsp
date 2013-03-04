# Microsoft Developer Studio Project File - Name="Publisher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Publisher - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Publisher.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Publisher.mak" CFG="Publisher - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Publisher - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "Publisher - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Publisher - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "LPLibs" /I "..\common\MfcUtils" /I "..\common\lutils" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 LPLibs.lib MfcUtils.lib lutils.lib /nologo /subsystem:windows /machine:I386 /libpath:"LPLibs\Release" /libpath:"..\common\MfcUtils\Release" /libpath:"..\common\lutils\Release"

!ELSEIF  "$(CFG)" == "Publisher - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "LPLibs" /I "..\common\MfcUtils" /I "..\common\lutils" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 LPLibs.lib MfcUtils.lib lutils.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"LPLibs\Debug" /libpath:"..\common\MfcUtils\Debug" /libpath:"..\common\lutils\Debug"

!ENDIF 

# Begin Target

# Name "Publisher - Win32 Release"
# Name "Publisher - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DocumentPropsView.cpp
# End Source File
# Begin Source File

SOURCE=.\FrontpageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ProfileView.cpp
# End Source File
# Begin Source File

SOURCE=.\PublishEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Publisher.cpp
# End Source File
# Begin Source File

SOURCE=.\Publisher.rc
# End Source File
# Begin Source File

SOURCE=.\PublisherDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PublishProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsLpdPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsMediaPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsRmPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsScormPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsSlidesPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsTargetPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsTemplatePage.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsTransferPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsVideoClipsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsView.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsWmPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DocumentPropsView.h
# End Source File
# Begin Source File

SOURCE=.\FrontpageDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ProfileView.h
# End Source File
# Begin Source File

SOURCE=.\PublishEngine.h
# End Source File
# Begin Source File

SOURCE=.\Publisher.h
# End Source File
# Begin Source File

SOURCE=.\PublisherDoc.h
# End Source File
# Begin Source File

SOURCE=.\PublishProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SettingsLpdPage.h
# End Source File
# Begin Source File

SOURCE=.\SettingsMediaPage.h
# End Source File
# Begin Source File

SOURCE=.\SettingsRmPage.h
# End Source File
# Begin Source File

SOURCE=.\SettingsScormPage.h
# End Source File
# Begin Source File

SOURCE=.\SettingsSlidesPage.h
# End Source File
# Begin Source File

SOURCE=.\SettingsTargetPage.h
# End Source File
# Begin Source File

SOURCE=.\SettingsTemplatePage.h
# End Source File
# Begin Source File

SOURCE=.\SettingsTransferPage.h
# End Source File
# Begin Source File

SOURCE=.\SettingsVideoClipsPage.h
# End Source File
# Begin Source File

SOURCE=.\SettingsView.h
# End Source File
# Begin Source File

SOURCE=.\SettingsWmPage.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Frontpage.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Publisher.ico
# End Source File
# Begin Source File

SOURCE=.\res\Publisher.rc2
# End Source File
# Begin Source File

SOURCE=.\res\PublisherDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\manifest.xml
# End Source File
# Begin Source File

SOURCE=.\Publisher.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

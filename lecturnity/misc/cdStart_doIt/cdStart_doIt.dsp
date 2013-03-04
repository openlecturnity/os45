# Microsoft Developer Studio Project File - Name="cdStart_doIt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=cdStart_doIt - Win32 English Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "cdStart_doIt.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "cdStart_doIt.mak" CFG="cdStart_doIt - Win32 English Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "cdStart_doIt - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "cdStart_doIt - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "cdStart_doIt - Win32 English Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "cdStart_doIt - Win32 English Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cdStart_doIt - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEUD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Vfw32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"Release/autorun.exe"

!ELSEIF  "$(CFG)" == "cdStart_doIt - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEUD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Vfw32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/autorun_d.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "cdStart_doIt - Win32 English Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cdStart_doIt___Win32_English_Release"
# PROP BASE Intermediate_Dir "cdStart_doIt___Win32_English_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "English Release"
# PROP Intermediate_Dir "English Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU" /d "AFX_TARG_NEUD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Vfw32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"Release/autorun.exe"
# ADD LINK32 Vfw32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"English Release/autorun.exe"

!ELSEIF  "$(CFG)" == "cdStart_doIt - Win32 English Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cdStart_doIt___Win32_English_Debug"
# PROP BASE Intermediate_Dir "cdStart_doIt___Win32_English_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "English Debug"
# PROP Intermediate_Dir "English Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU" /d "AFX_TARG_NEUD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Vfw32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Vfw32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"English Debug/autorun_d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "cdStart_doIt - Win32 Release"
# Name "cdStart_doIt - Win32 Debug"
# Name "cdStart_doIt - Win32 English Release"
# Name "cdStart_doIt - Win32 English Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cdStart_doIt.cpp
# End Source File
# Begin Source File

SOURCE=.\cdStart_doIt.rc
# End Source File
# Begin Source File

SOURCE=.\cdStart_doItDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageButton.cpp
# End Source File
# Begin Source File

SOURCE=.\MCIWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiBitmapCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cdStart_doIt.h
# End Source File
# Begin Source File

SOURCE=.\cdStart_doItDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImageButton.h
# End Source File
# Begin Source File

SOURCE=.\MCIWnd.h
# End Source File
# Begin Source File

SOURCE=.\MultiBitmapCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\background.bmp
# End Source File
# Begin Source File

SOURCE=.\background_title.bmp
# End Source File
# Begin Source File

SOURCE=.\bestellform.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\button_cancel.bmp
# End Source File
# Begin Source File

SOURCE=.\button_cddemo.bmp
# End Source File
# Begin Source File

SOURCE=.\button_install.bmp
# End Source File
# Begin Source File

SOURCE=.\button_system.bmp
# End Source File
# Begin Source File

SOURCE=.\button_system_enabled.bmp
# End Source File
# Begin Source File

SOURCE=.\button_tutorial.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cdstart.ico
# End Source File
# Begin Source File

SOURCE=.\res\cdStart_doIt.ico
# End Source File
# Begin Source File

SOURCE=.\res\cdStart_doIt.rc2
# End Source File
# Begin Source File

SOURCE=.\res\icon_player.ico
# End Source File
# Begin Source File

SOURCE=.\skip_intro.bmp
# End Source File
# Begin Source File

SOURCE=.\text_buttonpressed.bmp
# End Source File
# Begin Source File

SOURCE=.\text_buttonpressed_en.bmp
# End Source File
# Begin Source File

SOURCE=.\title_buttonpressed.bmp
# End Source File
# Begin Source File

SOURCE=.\title_cddemo.bmp
# End Source File
# Begin Source File

SOURCE=.\title_cddemo_en.bmp
# End Source File
# Begin Source File

SOURCE=.\title_install.bmp
# End Source File
# Begin Source File

SOURCE=.\title_installation.bmp
# End Source File
# Begin Source File

SOURCE=.\title_sysreq.bmp
# End Source File
# Begin Source File

SOURCE=.\title_sysreq_en.bmp
# End Source File
# Begin Source File

SOURCE=.\title_tutorial.bmp
# End Source File
# Begin Source File

SOURCE=.\white.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=".\res\intro-kurz0911.mp3"
# End Source File
# Begin Source File

SOURCE=".\res\intro-kurz0911_mp3.wav"
# End Source File
# Begin Source File

SOURCE=".\res\intro-lang0911.mp3"
# End Source File
# Begin Source File

SOURCE=".\res\intro-lang0911_mp3.wav"
# End Source File
# Begin Source File

SOURCE=.\lecturnity_intro_cpak_mp3.avi
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\sample.wav
# End Source File
# Begin Source File

SOURCE=.\sample2.wav
# End Source File
# End Target
# End Project

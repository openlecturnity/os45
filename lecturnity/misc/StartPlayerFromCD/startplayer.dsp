# Microsoft Developer Studio Project File - Name="startplayer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=startplayer - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "startplayer.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "startplayer.mak" CFG="startplayer - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "startplayer - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "startplayer - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "startplayer - Win32 English Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "startplayer - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\common\keygenlib" /I "..\..\common\lutils" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_NEUD" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 lutils.lib keygenlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/autorun.exe" /libpath:"..\..\common\lutils\Release" /libpath:"..\..\common\keygenlib\Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\autorun.exe Release\startplayer.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "startplayer - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common\keygenlib" /I "..\..\common\lutils" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEUD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib lutils.lib keygenlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug\autorun.exe" /pdbtype:sept /libpath:"..\..\common\lutils\Debug" /libpath:"..\..\common\keygenlib\Debug"

!ELSEIF  "$(CFG)" == "startplayer - Win32 English Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "English_Release"
# PROP BASE Intermediate_Dir "English_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "English_Release"
# PROP Intermediate_Dir "English_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\lecturnity\common\keygenlib" /I "..\..\lecturnity\common\lutils" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\common\keygenlib" /I "..\..\common\lutils" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_TARG_NEUD" /d "AFX_TARG_ENU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 lutils.lib keygenlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/autorun.exe" /libpath:"..\..\lecturnity\common\keygenlib\Release" /libpath:"..\..\lecturnity\common\lutils\Release"
# ADD LINK32 lutils.lib keygenlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"English_Release/autorun.exe" /libpath:"..\..\common\lutils\Release" /libpath:"..\..\common\keygenlib\Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "English_Release\autorun.exe" "English_Release\startplayer.exe"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "startplayer - Win32 Release"
# Name "startplayer - Win32 Debug"
# Name "startplayer - Win32 English Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dialog.cpp
# ADD CPP /Yu"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\errormessages.cpp
# ADD CPP /Yu"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\lecturnity.rc
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# ADD CPP /Yu"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\queries.cpp
# ADD CPP /Yu"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\registry.cpp
# ADD CPP /Yu"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dialog.h
# End Source File
# Begin Source File

SOURCE=.\errormessages.h
# End Source File
# Begin Source File

SOURCE=.\queries.h
# End Source File
# Begin Source File

SOURCE=.\registry.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\Cancel.ico
# End Source File
# Begin Source File

SOURCE=.\cd_module_startup.bmp
# End Source File
# Begin Source File

SOURCE=.\desk_player.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\separator_patter.bmp
# End Source File
# End Group
# End Target
# End Project

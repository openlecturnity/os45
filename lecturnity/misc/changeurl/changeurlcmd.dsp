# Microsoft Developer Studio Project File - Name="changeurlcmd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=changeurlcmd - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "changeurlcmd.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "changeurlcmd.mak" CFG="changeurlcmd - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "changeurlcmd - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "changeurlcmd - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "changeurlcmd - Win32 English Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "changeurlcmd - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\common\ZipArchive" /I "..\..\common\lutils" /I "..\..\common\filesdk" /I "..\..\common\drawsdk" /I "..\..\common\MfcUtils" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /D "CHANGEURLCMD" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEUD" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 filesdk.lib lutils.lib ZipArchive.lib MfcUtils.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libcmt.lib" /out:"Release/locationupdatecmd.exe" /libpath:"..\..\common\lutils\Release" /libpath:"..\..\common\filesdk\Release" /libpath:"..\..\common\ZipArchive\Release" /libpath:"..\..\common\MfcUtils\Release"

!ELSEIF  "$(CFG)" == "changeurlcmd - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\common\ZipArchive" /I "..\..\common\lutils" /I "..\..\common\filesdk" /I "..\..\common\drawsdk" /I "..\..\common\MfcUtils" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /D "CHANGEURLCMD" /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEUD" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 filesdk.lib lutils.lib ZipArchive.lib MfcUtils.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/locationupdatecmd.exe" /pdbtype:sept /libpath:"..\..\common\lutils\Debug" /libpath:"..\..\common\filesdk\Debug" /libpath:"..\..\common\ZipArchive\Debug" /libpath:"..\..\common\MfcUtils\Debug"

!ELSEIF  "$(CFG)" == "changeurlcmd - Win32 English Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "changeurlcmd___Win32_English_Release"
# PROP BASE Intermediate_Dir "changeurlcmd___Win32_English_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "English_Release"
# PROP Intermediate_Dir "English_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\common\lutils" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /D "CHANGEURLCMD" /Yu"StdAfx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\common\lutils" /I "..\..\common\filesdk" /I "..\..\common\drawsdk" /I "..\..\common\MfcUtils" /I "..\..\common\ZipArchive" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /D "CHANGEURLCMD" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEUD" /d "AFX_TARG_ENU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 lutils.lib /nologo /subsystem:console /machine:I386 /out:"Release/locationupdatecmd.exe" /libpath:"..\..\common\lutils\Release"
# ADD LINK32 lutils.lib ZipArchive.lib MfcUtils.lib filesdk.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libcmt.lib" /out:"English_Release/locationupdatecmd.exe" /libpath:"..\..\common\lutils\Release" /libpath:"..\..\common\ZipArchive\Release" /libpath:"..\..\common\MfcUtils\Release" /libpath:"..\..\common\filesdk\Release"

!ENDIF 

# Begin Target

# Name "changeurlcmd - Win32 Release"
# Name "changeurlcmd - Win32 Debug"
# Name "changeurlcmd - Win32 English Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\changeurl.rc
# End Source File
# Begin Source File

SOURCE=.\changeurlcmd.cpp
# End Source File
# Begin Source File

SOURCE=.\changeurlengine.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\ZipProgressDlg.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\changeurlcmd.h
# End Source File
# Begin Source File

SOURCE=.\changeurlengine.h
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
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="changeurl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=changeurl - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "changeurl.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "changeurl.mak" CFG="changeurl - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "changeurl - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "changeurl - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "changeurl - Win32 English Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "changeurl - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\common\MfcUtils" /I "..\..\common\lutils" /I "..\..\common\filesdk" /I "..\..\common\drawsdk" /I "..\..\common\ZipArchive" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEUD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 MfcUtils.lib lutils.lib filesdk.lib ZipArchive.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"Release/locationupdate.exe" /libpath:"..\..\common\MfcUtils\Release" /libpath:"..\..\common\lutils\Release" /libpath:"..\..\common\filesdk\Release" /libpath:"..\..\common\ZipArchive\Release"

!ELSEIF  "$(CFG)" == "changeurl - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\common\MfcUtils" /I "..\..\common\lutils" /I "..\..\common\filesdk" /I "..\..\common\drawsdk" /I "..\..\common\ZipArchive" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEUD" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MfcUtils.lib lutils.lib filesdk.lib ZipArchive.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/locationupdate.exe" /pdbtype:sept /libpath:"..\..\common\MfcUtils\Debug" /libpath:"..\..\common\lutils\Debug" /libpath:"..\..\common\filesdk\Debug" /libpath:"..\..\common\ZipArchive\Debug"

!ELSEIF  "$(CFG)" == "changeurl - Win32 English Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "English Release"
# PROP BASE Intermediate_Dir "English Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "English_Release"
# PROP Intermediate_Dir "English_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\common\lutils" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\common\lutils" /I "..\..\common\filesdk" /I "..\..\common\drawsdk" /I "..\..\common\MfcUtils" /I "..\..\common\ZipArchive" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU" /d "AFX_TARG_NEUD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 lutils.lib /nologo /subsystem:windows /machine:I386 /out:"Release/locationupdate.exe" /libpath:"..\..\common\lutils\Release"
# ADD LINK32 filesdk.lib mfcutils.lib lutils.lib ZipArchive.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"English_Release/locationupdate.exe" /libpath:"..\..\common\filesdk\Release" /libpath:"..\..\common\MfcUtils\Release" /libpath:"..\..\common\lutils\Release" /libpath:"..\..\common\ZipArchive\Release"

!ENDIF 

# Begin Target

# Name "changeurl - Win32 Release"
# Name "changeurl - Win32 Debug"
# Name "changeurl - Win32 English Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\changeurl.cpp
# End Source File
# Begin Source File

SOURCE=.\changeurl.rc
# End Source File
# Begin Source File

SOURCE=.\changeurlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\changeurlengine.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ZipProgressDlg.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\changeurl.h
# End Source File
# Begin Source File

SOURCE=.\changeurlDlg.h
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
# Begin Source File

SOURCE=.\ZipProgressDlg.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\changeurl.ico
# End Source File
# Begin Source File

SOURCE=.\res\changeurl.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

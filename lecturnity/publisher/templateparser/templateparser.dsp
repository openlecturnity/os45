# Microsoft Developer Studio Project File - Name="templateparser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=templateparser - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "templateparser.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "templateparser.mak" CFG="templateparser - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "templateparser - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "templateparser - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "templateparser - Win32 English Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "templateparser - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TEMPLATEPARSER_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\jdk1.3.1\include" /I "\jdk1.3.1\include\win32" /I "..\..\common\lutils" /I "..\..\common\filesdk" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TEMPLATEPARSER_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 filesdk.lib lutils.lib drawsdk.lib /nologo /dll /machine:I386 /nodefaultlib:"libcmt.lib" /libpath:"..\..\common\lutils\Release" /libpath:"..\..\common\filesdk\Release" /libpath:"..\..\common\drawsdk\Release"

!ELSEIF  "$(CFG)" == "templateparser - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TEMPLATEPARSER_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\jdk1.3.1\include" /I "\jdk1.3.1\include\win32" /I "\Programme\jdk1.3.1\include" /I "\Programme\jdk1.3.1\include\win32" /I "..\..\common\lutils" /I "..\..\common\filesdk" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TEMPLATEPARSER_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 filesdk.lib lutils.lib drawsdk.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /pdbtype:sept /libpath:"..\..\common\lutils\Debug" /libpath:"..\..\common\filesdk\Debug" /libpath:"..\..\common\drawsdk\Debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\common\filesdk\Debug\filesdk.dll Debug	copy ..\..\common\drawsdk\Debug\drawsdk.dll Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "templateparser - Win32 English Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "English Release"
# PROP BASE Intermediate_Dir "English Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "English_Release"
# PROP Intermediate_Dir "English_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "\jdk1.3.1\include" /I "\jdk1.3.1\include\win32" /I "..\..\common\lutils" /I "..\..\common\filesdk" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TEMPLATEPARSER_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\jdk1.3.1\include" /I "\jdk1.3.1\include\win32" /I "..\..\common\lutils" /I "..\..\common\filesdk" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TEMPLATEPARSER_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 filesdk.lib lutils.lib drawsdk.lib /nologo /dll /machine:I386 /nodefaultlib:"libcmt.lib" /libpath:"..\..\common\lutils\Release" /libpath:"..\..\common\filesdk\Release" /libpath:"..\..\common\drawsdk\Release"
# ADD LINK32 filesdk.lib lutils.lib drawsdk.lib /nologo /dll /machine:I386 /nodefaultlib:"libcmt.lib" /libpath:"..\..\common\lutils\Release" /libpath:"..\..\common\filesdk\Release" /libpath:"..\..\common\drawsdk\Release"

!ENDIF 

# Begin Target

# Name "templateparser - Win32 Release"
# Name "templateparser - Win32 Debug"
# Name "templateparser - Win32 English Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\selecttemplatepage.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\templateparser.cpp
# End Source File
# Begin Source File

SOURCE=.\templateparser.rc
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\java_templateparser.h
# End Source File
# Begin Source File

SOURCE=.\selecttemplatepage.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\templateparser.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

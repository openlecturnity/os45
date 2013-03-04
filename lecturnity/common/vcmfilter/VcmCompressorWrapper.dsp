# Microsoft Developer Studio Project File - Name="VcmCompressorWrapper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VcmCompressorWrapper - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "VcmCompressorWrapper.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "VcmCompressorWrapper.mak" CFG="VcmCompressorWrapper - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "VcmCompressorWrapper - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "VcmCompressorWrapper - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VcmCompressorWrapper - Win32 Release"

# PROP BASE Use_MFC 2
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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /I "C:\DXSDK\Samples\C++\DirectShow\BaseClasses" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D nodebug=1 /D "STRICT" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 vfw32.lib strmbase.lib oleaut32.lib kernel32.lib user32.lib ole32.lib uuid.lib advapi32.lib winmm.lib /nologo /base:"0x1d200000" /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt.lib" /out:"Release/vcmfilter.dll" /libpath:"C:\DXSDK\Samples\C++\DirectShow\BaseClasses\Release"
# Begin Custom Build - Registering DirectX Media Filters...
TargetPath=.\Release\vcmfilter.dll
InputPath=.\Release\vcmfilter.dll
SOURCE="$(InputPath)"

"null" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\WINDOWS\System32\regsvr32.exe /s /c "$(TargetPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "VcmCompressorWrapper - Win32 Debug"

# PROP BASE Use_MFC 2
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MDd /W3 /Gm /GX /ZI /Od /I "C:\DXSDK\Samples\C++\DirectShow\BaseClasses" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "DEBUG" /D "STRICT" /FR /YX /FD /GZ /LDd /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vfw32.lib strmbasD.lib oleaut32.lib kernel32.lib user32.lib ole32.lib uuid.lib advapi32.lib winmm.lib /nologo /base:"0x1d200000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"Debug/vcmfilter.dll" /pdbtype:sept /libpath:"C:\DXSDK\Samples\C++\DirectShow\BaseClasses\Debug"
# Begin Custom Build - Registering DirectX Media Filters...
TargetPath=.\Debug\vcmfilter.dll
InputPath=.\Debug\vcmfilter.dll
SOURCE="$(InputPath)"

"null" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\WINDOWS\System32\regsvr32.exe /c "$(TargetPath)"

# End Custom Build

!ENDIF 

# Begin Target

# Name "VcmCompressorWrapper - Win32 Release"
# Name "VcmCompressorWrapper - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\VcmCompressorWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\VcmCompressorWrapper.DEF
# End Source File
# Begin Source File

SOURCE=.\VcmCompressorWrapper.RC
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\VcmCompressorWrapper.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

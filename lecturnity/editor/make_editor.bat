@echo off
if "%1"=="" goto warning

devenv editor.sln /rebuild "Release"
if errorlevel 1 goto error
REM devenv editor.sln /rebuild "English Release"
REM if errorlevel 1 goto error
devenv ..\common\ladfilter\ladfilter.sln /rebuild "Release"
if errorlevel 1 goto error
devenv ..\common\lresizer\lresizer.sln /rebuild "Release"
if errorlevel 1 goto error


:copy

if not exist "S:\Releases\Editor\%1" mkdir S:\Releases\Editor\%1
if not exist "S:\Releases\Editor\%1\en" mkdir S:\Releases\Editor\%1\en
if not exist "S:\Releases\Editor\%1\de" mkdir S:\Releases\Editor\%1\de
if not exist "S:\Releases\Editor\latest" mkdir S:\Releases\Editor\latest
if not exist "S:\Releases\Editor\latest\de" mkdir S:\Releases\Editor\latest\de
if not exist "S:\Releases\Editor\latest\en" mkdir S:\Releases\Editor\latest\en

del /q S:\Releases\Editor\%1\*.*
del /q S:\Releases\Editor\%1\en\*.*
del /q S:\Releases\Editor\%1\de\*.*

copy Release\editor.exe S:\Releases\Editor\%1\en
copy Release\editor.exe S:\Releases\Editor\%1\de
copy Release\drawsdk.dll S:\Releases\Editor\%1
copy Release\avedit.dll S:\Releases\Editor\%1
copy Release\lsutl32.dll S:\Releases\Editor\%1
copy ..\common\filesdk\Release\filesdk.dll S:\Releases\Editor\%1\en
copy ..\common\filesdk\Release\filesdk.dll S:\Releases\Editor\%1\de
copy ..\common\ladfilter\Release\ladfilter.dll S:\Releases\Editor\%1
copy ..\common\lresizer\Release\lresizer.dll S:\Releases\Editor\%1
REM copy ..\common\XT_Toolkit\WorkSpace\XTPToolkit_Dll\Release\XTP8510Lib.dll S:\Releases\Editor\%1\
copy ..\common\XT_Toolkit_2009\Workspace\ToolkitPro\Release\xtp2009.dll S:\Releases\Editor\%1\
copy register.bat S:\Releases\Editor\%1
REM xcopy /y /s S:\Releases\Editor\static\*.* S:\Releases\Editor\%1

del /q S:\Releases\Editor\latest\*.*
del /q S:\Releases\Editor\latest\en\*.*
del /q S:\Releases\Editor\latest\de\*.*

copy S:\Releases\Editor\%1\*.* S:\Releases\Editor\latest
copy S:\Releases\Editor\%1\en\*.* S:\Releases\Editor\latest\en
copy S:\Releases\Editor\%1\de\*.* S:\Releases\Editor\latest\de

goto ende

:error
echo Es ist ein Fehler aufgetreten.
echo Beim Erstellen des Editors oder eines der Zusatzpakete ist ein Fehler aufgetreten. > \Sandbox\make\error.txt
goto ende

:warning
echo Benutzung: make_editor.bat ^<version^>

:ende
echo Editor: Operation abgeschlossen.
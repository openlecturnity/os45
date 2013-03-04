@echo off

if "%1"=="" goto warning
if "%2"=="" goto warning

set SANDBOX=%1

devenv Studio.sln /rebuild "Release"
if errorlevel 1 goto error

devenv ..\common\ladfilter\ladfilter.sln /rebuild "Release"
if errorlevel 1 goto error
devenv ..\common\lresizer\lresizer.sln /rebuild "Release"
if errorlevel 1 goto error


:copy

if not exist "S:\Releases\Studio\%2" mkdir S:\Releases\Studio\%2
if not exist "S:\Releases\Studio\latest" mkdir S:\Releases\Studio\latest

del /q S:\Releases\Studio\%2\*.*

copy Release\Studio.exe S:\Releases\Studio\%2\
copy %SANDBOX%\bin\Release\drawsdk.dll S:\Releases\Studio\%2\
copy %SANDBOX%\bin\Release\filesdk.dll S:\Releases\Studio\%2\
copy %SANDBOX%\bin\Release\lsutl32.dll S:\Releases\Studio\%2\
copy %SANDBOX%\bin\Release\xtp2009.dll S:\Releases\Studio\%2\
copy %SANDBOX%\bin\Release\avedit.dll S:\Releases\Studio\%2\
copy %SANDBOX%\bin\Release\avgrabber.dll S:\Releases\Studio\%2\
copy %SANDBOX%\bin\Release\iohook.dll S:\Releases\Studio\%2\
copy ..\common\ladfilter\Release\ladfilter.dll S:\Releases\Studio\%2\
copy ..\common\lresizer\Release\lresizer.dll S:\Releases\Studio\%2\

REM copy ..\..\epresenter\smartboard\Release\smartboard.dll S:\Releases\Assistant2\%2\
REM copy S:\Releases\Assistant2\static\SBSDK.dll S:\Releases\Studio\%2\
copy S:\Releases\Assistant2\static\click.wav S:\Releases\Studio\%2\

del /q S:\Releases\Studio\latest\*.*

copy S:\Releases\Studio\%2\*.* S:\Releases\Studio\latest\

goto ende

:error
echo Es ist ein Fehler aufgetreten.
echo Beim Erstellen des Studios oder eines der Zusatzpakete ist ein Fehler aufgetreten. > %SANDBOX%\make\error.txt
goto ende

:warning
echo Benutzung: make_studio.bat ^<sandbox path^> ^<version^>

:ende
echo Studio: Operation abgeschlossen.
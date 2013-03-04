@echo off

if "%1"=="" goto Warnung
if "%2"=="" goto Warnung

set SANDBOX=%1

echo "make_publisher_libs started... %SANDBOX%"
:vs2005

cd %SANDBOX%\libs\realexport
devenv realexport.sln /rebuild "Release"
if errorlevel 1 goto Fehler

cd %SANDBOX%\libs\wm_export
devenv wm_export.sln /rebuild "Release"
if errorlevel 1 goto Fehler

cd %SANDBOX%\lecturnity\publisher\LPLibs
devenv LPLibs.sln /rebuild "Release"
if errorlevel 1 goto Fehler

cd %SANDBOX%\lecturnity\publisher\Uploader
devenv Uploader.sln /rebuild "Release"
if errorlevel 1 goto Fehler
devenv Uploader.sln /build "Exe_Release"
if errorlevel 1 goto Fehler

:copy
if not exist "S:\Releases\Publisher\latest" mkdir S:\Releases\Publisher\latest

copy %SANDBOX%\libs\realexport\Release\avcnvt.dll S:\Releases\Publisher\latest
if errorlevel 1 goto Fehler
copy %SANDBOX%\libs\wm_export\Release\avitowmv.dll S:\Releases\Publisher\latest
if errorlevel 1 goto Fehler
copy %SANDBOX%\lecturnity\publisher\LPLibs\Release\LPLibs.dll S:\Releases\Publisher\latest
if errorlevel 1 goto Fehler
copy %SANDBOX%\LPming\win32\Release\libming.dll S:\Releases\Publisher\latest
if errorlevel 1 goto Fehler
copy %SANDBOX%\lecturnity\common\filesdk\Release\filesdk.dll S:\Releases\Publisher\latest
if errorlevel 1 goto Fehler
copy %SANDBOX%\lecturnity\common\drawsdk\Release\drawsdk.dll S:\Releases\Publisher\latest
if errorlevel 1 goto Fehler
copy %SANDBOX%\lecturnity\publisher\Uploader\Release\Uploader.dll S:\Releases\Publisher\latest\
if errorlevel 1 goto Fehler
copy %SANDBOX%\lecturnity\publisher\Uploader\Exe_Release\Uploader.exe S:\Releases\Publisher\latest\
if errorlevel 1 goto Fehler
copy %SANDBOX%\lecturnity\publisher\Uploader\Release\Uploader.dll S:\Releases\Publisher\latest\
if errorlevel 1 goto Fehler

goto Ende

:Fehler
echo *** Es ist ein Fehler aufgetreten.
echo Beim Erstellen der Publisher-Bibliotheken ist ein Fehler aufgetreten. > %SANDBOX%\make\error.txt
goto Ende

:Warnung
echo Benutzung: make_publisher_libs.bat ^<sandbox path^> ^<version^>
echo   Die Libs fuer den Publisher werden erstellt und
echo   anschliessend in S:\Releases\Publisher\latest kopiert.

:Ende
echo === Operation abgeschlossen.
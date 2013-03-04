@echo off

if "%1"=="" goto Warning
if "%2"=="" goto Warning

set SANDBOX=%1

echo "make_flash_player2 started... %SANDBOX%"
if "%FLEX_SDK%"=="" echo Env variable FLEX_SDK is not set.
if "%FLEX_SDK%"=="" goto Error

if not exist "S:\Releases\FlashPlayer2\%2" mkdir S:\Releases\FlashPlayer2\%2
if not exist "S:\Releases\FlashPlayer2\latest" mkdir S:\Releases\FlashPlayer2\latest


cd %SANDBOX%\lecturnity\publisher\FlashPlayer\
"%FLEX_SDK%\bin\mxmlc" -library-path+=libs -source-path+=locale\{locale} -locale=en_US,de_DE -allow-source-path-overlap=true -use-network=false -debug=false src\LecturnityFlashPlayer.mxml
if errorlevel 1 goto error

REM Player app file:
copy src\LecturnityFlashPlayer.swf S:\Releases\FlashPlayer2\%2\

REM Static files:
copy src\LecturnityProxieAS2.swf S:\Releases\FlashPlayer2\%2\
copy html-template\AC_OETags.js S:\Releases\FlashPlayer2\%2\
copy html-template\playerProductInstall.swf S:\Releases\FlashPlayer2\%2\
copy src\LecturnityFlashPlayer.html.tmpl S:\Releases\FlashPlayer2\%2\


REM the start.exe:
cd %SANDBOX%\lecturnity\publisher\FlashStarter\
devenv FlashStarter.sln /rebuild "Release"
if errorlevel 1 goto error

copy Release\FlashStarter.exe S:\Releases\FlashPlayer2\%2\start.exe


REM Fill latest:
copy S:\Releases\FlashPlayer2\%2\*.* S:\Releases\FlashPlayer2\latest\


goto End

:Warning
echo This file compiles and copies the Flash player. Call with <version> as first parameter.
goto End

:Error
echo *** There was an error
echo upon creation of the Flash player files. > %SANDBOX%\make\error.txt

:End
echo === Flash Player 2: Operation finished.
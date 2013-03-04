@echo off
if "%1"=="" goto warning

:vs2005

devenv Assistant.sln /rebuild "Release"
if errorlevel 1 goto error
REM devenv Assistant.sln /rebuild "English Release"
REM if errorlevel 1 goto error


:copy
if not exist "S:\Releases\Assistant2\%1" mkdir S:\Releases\Assistant2\%1
if not exist "S:\Releases\Assistant2\%1\en" mkdir S:\Releases\Assistant2\%1\en
if not exist "S:\Releases\Assistant2\%1\de" mkdir S:\Releases\Assistant2\%1\de
if not exist "S:\Releases\Assistant2\latest" mkdir S:\Releases\Assistant2\latest
if not exist "S:\Releases\Assistant2\latest\en" mkdir S:\Releases\Assistant2\latest\en
if not exist "S:\Releases\Assistant2\latest\de" mkdir S:\Releases\Assistant2\latest\de

del /q S:\Releases\Assistant2\%1\*.*
del /q S:\Releases\Assistant2\%1\en\*.*
del /q S:\Releases\Assistant2\%1\de\*.*

copy Release\Assistant.exe S:\Releases\Assistant2\%1\en
copy Release\Assistant.exe S:\Releases\Assistant2\%1\de
copy ..\common\drawsdk\Release\drawsdk.dll S:\Releases\Assistant2\%1
copy ..\..\imc\native\avgrabber\Release\avgrabber.dll S:\Releases\Assistant2\%1
copy ..\..\imc\native\iohook\Release\iohook.dll S:\Releases\Assistant2\%1
copy ..\common\lsutl32\Release\lsutl32.dll S:\Releases\Assistant2\%1
copy ..\common\filesdk\Release\filesdk.dll S:\Releases\Assistant2\%1\en
copy ..\common\filesdk\Release\filesdk.dll S:\Releases\Assistant2\%1\de
copy ..\common\XT_Toolkit_2009\Workspace\ToolkitPro\Release\xtp2009.dll S:\Releases\Assistant2\%1\
REM copy ..\common\XT_Toolkit\WorkSpace\XTPToolkit_Dll\Release\XTP8510Lib.dll S:\Releases\Assistant2\%1\en
REM copy ..\common\XT_Toolkit\WorkSpace\XTPToolkit_Dll\Release\XTP8510Lib.dll S:\Releases\Assistant2\%1\de
copy ..\..\epresenter\smartboard\Release\smartboard.dll S:\Releases\Assistant2\%1
REM xcopy /y /s S:\Releases\Assistant2\static\*.* S:\Releases\Assistant2\%1
copy S:\Releases\Assistant2\static\SBSDK.dll S:\Releases\Assistant2\%1
copy S:\Releases\Assistant2\static\click.wav S:\Releases\Assistant2\%1

del /q S:\Releases\Assistant2\latest\*.*
del /q S:\Releases\Assistant2\latest\en\*.*
del /q S:\Releases\Assistant2\latest\de\*.*

copy S:\Releases\Assistant2\%1\*.* S:\Releases\Assistant2\latest
copy S:\Releases\Assistant2\%1\en\*.* S:\Releases\Assistant2\latest\en
copy S:\Releases\Assistant2\%1\de\*.* S:\Releases\Assistant2\latest\de

goto ende

:error
echo Es ist ein Fehler aufgetreten.
echo Beim Erstellen des Assistant oder eines der Zusatzpakete ist ein Fehler aufgetreten. > \Sandbox\make\error.txt
goto ende

:warning
echo Benutzung: make_assistant.bat ^<version^>

:ende
echo Assistant: Operation abgeschlossen.
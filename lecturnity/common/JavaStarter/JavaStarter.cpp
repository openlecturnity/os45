#include "stdafx.h"
#include "JavaStarter.h"
#include "JavaStarterDlg.h"

#include "KeyGenerator.h"   // keygenlib
#include "lutils.h"         // lutils

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "CRC_32.h"

#define CONNECT_TIMEOUT_MS 60000

BEGIN_MESSAGE_MAP(CJavaStarterApp, CWinApp)
	//{{AFX_MSG_MAP(CJavaStarterApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


/**
 * This application's purpose is to start Java.
 * As there are 2 programs (Player and Publisher) and 4 executables
 * (player.exe, startplayer.exe, autorun.exe, publisher.exe) it combines
 * the behavior of all these programs. The actual behavior is 
 * determined depending on the executable's name.
 */



CJavaStarterApp::CJavaStarterApp()
{
}

CJavaStarterApp theApp;


//
// Lots of forward declaration; main method is at the end.
//

#ifdef _DEBUG

void FillRandomly(BYTE* buffer, int len, int base)
{
   for (int i=0; i<len; ++i)
      buffer[i] = (rand() >> 3) % base;
}

void CreateLecturnityDat()
{
   _TCHAR tszCurrentDir[MAX_PATH];
   ::GetCurrentDirectory(MAX_PATH, tszCurrentDir);

   _tcscat(tszCurrentDir, _T("\\*.lpd"));
   WIN32_FIND_DATA sFindData;
   ZeroMemory(&sFindData, sizeof WIN32_FIND_DATA);
   HANDLE hSearch = ::FindFirstFile(tszCurrentDir, &sFindData);

   if (hSearch != INVALID_HANDLE_VALUE)
   {
      const int KEY_LENGTH = 512;
      const int CALC_LENGTH = 256;
      
      srand(GetTickCount());
      BYTE aKey[512];
      FillRandomly(aKey, 512, 256);

   
      aKey[0] = (BYTE)'d';
      aKey[1] = (BYTE)'e'; // this should be the language code
      aKey[2] = (BYTE)0x1e;
      aKey[3] = (BYTE)0xcd;
      aKey[4] = (BYTE)0xad; // identifier
      aKey[5] = (BYTE)2; // version

      int iKeyPointer = 0;

      HANDLE hDatFile = ::CreateFile(_T("lecturnity.dat"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
         FILE_ATTRIBUTE_NORMAL, NULL);
   
      DWORD dwWrittenBytes = 0;
      if (hDatFile != INVALID_HANDLE_VALUE)
         ::WriteFile(hDatFile, aKey, KEY_LENGTH, &dwWrittenBytes, NULL);

      bool bFileFound = hDatFile != INVALID_HANDLE_VALUE;

      while (bFileFound)
      {
         HANDLE hLpdFile = ::CreateFile(sFindData.cFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
         
         if (hLpdFile != INVALID_HANDLE_VALUE)
         {
            BYTE aFileStart[CALC_LENGTH];
            ::SetFilePointer(hLpdFile, 4, 0, FILE_BEGIN);
            
            DWORD dwReadBytes = 0;
            ::ReadFile(hLpdFile, aFileStart, CALC_LENGTH, &dwReadBytes, NULL);
            
            CRC_32 crc32;

            DWORD crc = crc32.CalcCRC(aFileStart, CALC_LENGTH);
            char hexCrc[8];
            memset(hexCrc, 0, 8);
            ultoa(crc, hexCrc, 16);
            
            int i = 0;
            for (i=0; i<8; ++i)
               hexCrc[i] ^= aKey[iKeyPointer++ % KEY_LENGTH];
            
            ::WriteFile(hDatFile, hexCrc, 8, &dwWrittenBytes, NULL);
            
            
            char szNameUtf8[MAX_PATH * 3];
            ZeroMemory(szNameUtf8, MAX_PATH * 3);
            int iByteLength = 0;
#ifdef _UNICODE
            iByteLength = ::WideCharToMultiByte(CP_UTF8, 0, sFindData.cFileName, -1, szNameUtf8, MAX_PATH * 3, NULL, NULL);
#else
            WCHAR wszTemp[MAX_PATH];
            int iLength = ::MultiByteToWideChar(CP_ACP, 0, sFindData.cFileName, -1, wszTemp, MAX_PATH);
            wszTemp[iLength] = 0;
            iByteLength = ::WideCharToMultiByte(CP_UTF8, 0, wszTemp, iLength, szNameUtf8, MAX_PATH * 3, NULL, NULL);
#endif
            szNameUtf8[iByteLength] = 0;
            ++iByteLength; // include null byte

            for (i=0; i<iByteLength; ++i)
               szNameUtf8[i] ^= aKey[iKeyPointer++ % KEY_LENGTH];
            
            ::WriteFile(hDatFile, szNameUtf8, iByteLength, &dwWrittenBytes, NULL);

                     
            ::CloseHandle(hLpdFile);
         }
         
         bFileFound = (::FindNextFile(hSearch, &sFindData) != FALSE);
      }

      if (hDatFile != INVALID_HANDLE_VALUE)
         ::CloseHandle(hDatFile);
   
   
      ::FindClose(hSearch);
   }
}
#endif // _DEBUG

void SplitCommandLine(CArray<CString, CString> *paCommandParts)
{
   if (paCommandParts != NULL)
   {
      CString csCommandLine = ::GetCommandLine();
      csCommandLine.TrimLeft();
      csCommandLine.TrimRight();
      csCommandLine += _T(' '); // make sure the algorithm below stops correctly
      
      bool bApostropheActive = false;
      int  iStart = 0;
      for (int i=0; i<csCommandLine.GetLength(); ++i)
      {
         _TCHAR tchCurrent = csCommandLine.GetAt(i);
         
         if (tchCurrent == _T('"'))
         {
            bApostropheActive = !bApostropheActive;
         }
         else if (tchCurrent == _T(' ') && (!bApostropheActive || i == csCommandLine.GetLength() - 1))
         {
            CString csPart = csCommandLine.Mid(iStart, i - iStart);
            if (csPart.GetLength() > 0 && csPart[0] == _T('"')) {
                csPart.TrimLeft(_T('"'));
                csPart.TrimRight(_T('"'));
            }
            
            if (csPart.GetLength() > 0)
               paCommandParts->Add(csPart);
            
            iStart = i + 1;
         }
      }
   }
}

CString GetProgramName(CString &csProgramPath)
{
   CString csProgramName = csProgramPath;
   if (csProgramName.ReverseFind(_T('\\')) > -1)
      csProgramName = csProgramName.Right(csProgramName.GetLength() - csProgramName.ReverseFind(_T('\\')) - 1);

   return csProgramName;
}

CString GetJavaCommand(CString &csProgramPath, bool bDoDebug)
{
   CString csJavaName = _T("javaw.exe");
   if (bDoDebug)
      csJavaName = _T("java.exe");

   CString csDirectory = csProgramPath;
   if (csDirectory.ReverseFind(_T('\\')) > -1)
      csDirectory = csDirectory.Left(csDirectory.ReverseFind(_T('\\'))); // "xyz.exe"

   // TODO this assumes that csProgramPath indeed ends with a file

   CString csJavaPath1 = csDirectory;
   if (csJavaPath1.ReverseFind(_T('\\')) > -1)
      csJavaPath1 = csJavaPath1.Left(csJavaPath1.ReverseFind(_T('\\'))); // ".."
   csJavaPath1 += _T("\\jre5\\bin\\"); // normally "installed"
   csJavaPath1 += csJavaName;
 
   CString csJavaPath2 = csDirectory;
   csJavaPath2 += _T("\\bin\\jre\\bin\\"); // on cd
   csJavaPath2 += csJavaName;

   CString csJavaPathReturn = csJavaPath1;
   if (_taccess(csJavaPathReturn, 04) != 0)
      csJavaPathReturn = csJavaPath2;
   
   if (_taccess(csJavaPathReturn, 04) != 0) // is not readable locally
   {
      // try to find an installed LECTURNITY (and the java therein)
      HKEY hkKey   = NULL;
      LONG lResult = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\imc AG\\LECTURNITY"), 0, KEY_READ, &hkKey);
      if (lResult == ERROR_SUCCESS)
      {
         _TCHAR tszInstallDir[MAX_PATH];
         DWORD dwBufferSize = MAX_PATH * sizeof _TCHAR;
         ZeroMemory(tszInstallDir, dwBufferSize);
         DWORD dwType = 0;
         lResult = ::RegQueryValueEx(hkKey, _T("InstallDir"), NULL, &dwType, (LPBYTE)tszInstallDir, &dwBufferSize);
         
         if (lResult == ERROR_SUCCESS)
         {
            csJavaPathReturn = tszInstallDir;
            if (csJavaPathReturn.GetAt(csJavaPathReturn.GetLength() - 1) != _T('\\'))
               csJavaPathReturn += _T('\\');
            csJavaPathReturn += _T("jre5\\bin\\");
            csJavaPathReturn += csJavaName;
         }
      }
      
      if (_taccess(csJavaPathReturn, 04) != 0) // is not installed with LECTURNITY
      {
         csJavaPathReturn = csJavaName;
      }
   }

   return csJavaPathReturn;
}

bool IsPathValid8Bit(CString csPath)
{
#ifdef _UNICODE

   int iLength = csPath.GetLength();
   char szPath[MAX_PATH];
   BOOL bUseDefaultChar = TRUE;
   ::WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, csPath, iLength, szPath, MAX_PATH, "?", &bUseDefaultChar);
   
   bool bPathValid8Bit = true;
   for (int i=0; i<iLength; ++i)
   {
      if (szPath[i] == '?')
         bPathValid8Bit = false;
   }

   szPath[iLength] = 0;
   
   return bPathValid8Bit;

#else
   return true;
#endif
}

CString GetPlayerJarName(CString &csProgramPath)
{
   CString csDirectory = csProgramPath;
   if (csDirectory.GetAt(csDirectory.GetLength() - 1) != _T('\\') && csDirectory.ReverseFind(_T('\\')) > -1)
      csDirectory = csDirectory.Left(csDirectory.ReverseFind(_T('\\'))); // "xyz.exe"

   CString csJarPath = csProgramPath;
   csJarPath += _T("\\player.jar"); // normal installation

   if (_taccess(csJarPath, 04) != 0)
   {
      csJarPath = csProgramPath;
      csJarPath += _T("\\bin\\player.jar"); // from cd

      if (_taccess(csJarPath, 04) != 0)
         csJarPath = _T("");
   }

   /* Then there are still several dlls (Publisher) missing...
   // Do an additional check because Java (java.exe, javaw.exe) cannot handle Unicode
   // on the command line and in processed env variables (like CLASSPATH).
   //
   if (csJarPath.GetLength() > 0)
   {
      // File seems to exist; check if the directory is accessible from Java (which is non-Unicode)

      if (!IsPathValid8Bit(csJarPath))
      {
         // Try to copy the player.jar to the temp directory (hopefully without evil characters).

         _TCHAR tszTempPath[MAX_PATH];
         ZeroMemory(tszTempPath, MAX_PATH * sizeof _TCHAR);

         DWORD dwChars = ::GetTempPath(MAX_PATH, tszTempPath);

         if (dwChars > 0)
         {
            CString csTemp = tszTempPath;

            if (IsPathValid8Bit(csTemp))
            {
               CString csNewJarPath = csTemp;
               csNewJarPath += _T("\\");
               csNewJarPath += _T("player8394xz48.jar"); // some random name
               BOOL bSuccess = ::CopyFile(csJarPath, csNewJarPath, FALSE);

               if (bSuccess)
                  csJarPath = csNewJarPath;
            }
         }

         // all else cases: unexpected/unhandled error
      }
   }
   */

   return csJarPath;
}

int FindJavaMemoryValue()
{
   // existance of environment variable overrides system values:
   _TCHAR *tszMaxJavaEnv = _tgetenv(_T("LECTURNITY_XMX"));
   if (tszMaxJavaEnv != NULL)
      return _ttoi(tszMaxJavaEnv);

   MEMORYSTATUSEX sMemoryStatus;
   sMemoryStatus.dwLength = sizeof MEMORYSTATUSEX; // important
   BOOL bMemorySuccess = ::GlobalMemoryStatusEx(&sMemoryStatus);

   int iTotalMemoryMB = 256;
   if (bMemorySuccess)
      iTotalMemoryMB = (int)(sMemoryStatus.ullTotalPhys/(1024i64*1024i64));

   // do not use all memory
   int iMaxJavaMB = (iTotalMemoryMB * 3) / 4;
   if (iTotalMemoryMB > 512)
      iMaxJavaMB = iTotalMemoryMB / 2;

   if (iMaxJavaMB < 64)
      iMaxJavaMB = 64;
   if (iMaxJavaMB > 880) // 1.5GB seems to be a limit of Java; stay way below it
      iMaxJavaMB = 880;

   return iMaxJavaMB;
}

void GetInstalledPublisherComponents(bool &bHasCdPublisher,
                                     bool &bHasRealMedia,
                                     bool &bHasWindowsMedia,
                                     bool &bHasMp4Video)
{
   // TODO could or should read the security entry instead?


   // Default: Nothing!
   bHasCdPublisher  = false;
   bHasRealMedia    = false;
   bHasWindowsMedia = false;
   bHasMp4Video     = false;

   HKEY key;
   DWORD dwResult = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                   _T("SOFTWARE\\imc AG\\LECTURNITY"),
                                   0, KEY_READ, &key);
   
   if (dwResult != ERROR_SUCCESS)
      return;

   DWORD dwType = REG_DWORD;
   DWORD dwValue = 0;
   DWORD dwSize = sizeof(DWORD);

   dwResult = ::RegQueryValueEx(key, _T("CdPublisher"), NULL,
                                &dwType, (LPBYTE) &dwValue, &dwSize);
   if (dwResult == ERROR_SUCCESS)
      bHasCdPublisher = (dwValue == 0x00000001);

   dwResult = ::RegQueryValueEx(key, _T("RealMedia"), NULL,
                                &dwType, (LPBYTE) &dwValue, &dwSize);
   if (dwResult == ERROR_SUCCESS)
      bHasRealMedia = (dwValue == 0x00000001);

   dwResult = ::RegQueryValueEx(key, _T("WindowsMedia"), NULL,
                                &dwType, (LPBYTE) &dwValue, &dwSize);
   if (dwResult == ERROR_SUCCESS)
      bHasWindowsMedia = (dwValue == 0x00000001);

   dwResult = ::RegQueryValueEx(key, _T("Mp4Video"), NULL,
                                &dwType, (LPBYTE) &dwValue, &dwSize);
   if (dwResult == ERROR_SUCCESS)
      bHasMp4Video = (dwValue == 0x00000001);

   RegCloseKey(key);

}

void SendJavaCommand(SOCKET hSocket, CArray<CString, CString> *paParameters)
{
   if (hSocket == INVALID_SOCKET || hSocket == NULL)
      return;

   if (paParameters == NULL || paParameters->GetSize() <= 0)
      return;

   int iCharCount = 0;
   int i = 0;
   for (i=0; i<paParameters->GetSize(); ++i)
      iCharCount += paParameters->GetAt(i).GetLength() + 1;

   int iArrayLength = iCharCount * 3 + 3;
   char *szCommandLine = new char[iArrayLength]; // maybe contains some utf8 stuff

   int iCommandLinePointer = 0;
   bool bWillBeFilename = false;
   for (i=0; i<paParameters->GetSize(); ++i)
   {
      int iLength = paParameters->GetAt(i).GetLength();
      WCHAR *wszOneParameter = new WCHAR[paParameters->GetAt(i).GetLength() + 1];

#ifdef _UNICODE
      wcsncpy(wszOneParameter, paParameters->GetAt(i), iLength);
      wszOneParameter[iLength] = 0;
#else
      ::MultiByteToWideChar(CP_ACP, 0, paParameters->GetAt(i), iLength, wszOneParameter, iLength);
#endif

      bool bIsFilename = false;
      if (i == 0 && iLength > 0 && wszOneParameter[0] != L'-')
          bIsFilename = true;

      // NOTE: There is also another mechanism at work for specifying paths.
      //       This other one had not these problems with spaces. But where is the other one?
      //       It is used for exotic characters (non iso8859-1).

      if (bIsFilename || bWillBeFilename)
          szCommandLine[iCommandLinePointer++] = '\"';
      
      int iCopied = ::WideCharToMultiByte(CP_UTF8, 0, wszOneParameter, iLength, 
         szCommandLine + iCommandLinePointer, iArrayLength - iCommandLinePointer, NULL, NULL);
      iCommandLinePointer += iCopied;

      if (bIsFilename || bWillBeFilename) {
          szCommandLine[iCommandLinePointer++] = '\"';
          bWillBeFilename = false;
      }

      szCommandLine[iCommandLinePointer++] = ' ';

      if (i == 0 && iLength > 0  && wcscmp(L"-load", wszOneParameter) == 0)
          bWillBeFilename = true;

      delete[] wszOneParameter;
   }

   szCommandLine[iCommandLinePointer] = 0;

   char *szToSend = szCommandLine;
   if (iCommandLinePointer > 0)
   {
      if (szToSend[0] == '-')
         ++szToSend;

      int iLength = strlen(szToSend);

      if (szToSend[iLength - 1] != '\n')
      {
         szToSend[iLength - 1] = '\n';
         iLength++;
         szToSend[iLength] = 0;
      }

      int iSent = ::send(hSocket, szToSend, iLength, 0);
      // can be SOCKET_ERROR; is ignored

      if (iSent != SOCKET_ERROR) {
          char szRecvDummy[32] = "";
          ::recv(hSocket, szRecvDummy, 32, 0);
          // Receive anything, especially a WSAECONNRESET; = waits for connection finish
      }
   }


   delete[] szCommandLine;
}

bool ConnectToPlayer(HANDLE hJavaHandle, CArray<CString, CString> *paParameters)
{
   bool bHandleValid = hJavaHandle != INVALID_HANDLE_VALUE && hJavaHandle != NULL;

   if (!bHandleValid && paParameters == NULL)
      return false; // something must be set

   WSADATA wsdata;
   WORD wVersionRequested = MAKEWORD(1,1);     
   int iError = ::WSAStartup(wVersionRequested, &wsdata);
   int iWsaError = iError;

   SOCKET hSocket = INVALID_SOCKET;
   if (iError == S_OK)
      hSocket = ::socket(PF_INET, SOCK_STREAM, 0);

   if (iError == S_OK && hSocket == INVALID_SOCKET)
      iError = ::WSAGetLastError();

   sockaddr_in sAddress;
   sAddress.sin_family      = PF_INET;
   sAddress.sin_port        = htons(21777);
   sAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

   bool bConnectSuccessful = false;

   int iLoopCounter = 0;
   bool bConnectedDirectly = false;

   DWORD dwStartMs = ::timeGetTime();
   while (iError == S_OK && !bConnectSuccessful)
   {
      DWORD dwCurrentMs = ::timeGetTime();

      if (dwCurrentMs - dwStartMs > CONNECT_TIMEOUT_MS)
         break;
 
      int iConnectError = ::connect(hSocket, (struct sockaddr *)&sAddress, sizeof(sAddress));

      if (iConnectError == SOCKET_ERROR)
      {
         int iSpecificError = ::WSAGetLastError();

         if (iSpecificError != WSAECONNREFUSED)
            break; // some real error
      }

      if (bHandleValid)
      {
         // Java was started here; check if it is still running

         DWORD dwJavaExitCode = 0;
         if (::GetExitCodeProcess(hJavaHandle, &dwJavaExitCode) != 0)
         {
            if (dwJavaExitCode != STILL_ACTIVE)
               break; // java process has already (?) finished
         }
      }

      if (iConnectError == S_OK)
      {
         //::Sleep(1000);
         //::MessageBox(NULL, _T("Some connection"), NULL, MB_OK);

         if (iLoopCounter == 0)
            bConnectedDirectly = true;

         char aBuffer[200];
         ZeroMemory(aBuffer, 200);
         int iBytesReceived = ::recv(hSocket, aBuffer, 199, 0);

         if (iBytesReceived < 0)
            iError = ::WSAGetLastError();
         else
         {
            aBuffer[iBytesReceived] = 0;

            char *szAuthValid = "LECTURNITY Player"; // might also contain a \n at the end
            if (strncmp(aBuffer, szAuthValid, strlen(szAuthValid)) == 0) {
               bConnectSuccessful = true;
            }
            else
               break; // something other waiting on this port; is this an error?

            if (paParameters != NULL && paParameters->GetSize() > 0)
               SendJavaCommand(hSocket, paParameters);
         }

         ::closesocket(hSocket);
      }
      else if (bHandleValid)
         ::Sleep(100);
      else // no Java there; but do not linger this was only a check (bHandleValid == false)
         break;

      ++iLoopCounter;
   }

   if (iWsaError == S_OK)
      ::WSACleanup();

   return bConnectSuccessful;
}

bool StartProgram(CString csCommand, HANDLE *phProgram, CString csParameters, bool bDoDebug, DWORD &dwProcessID)
{
   if (bDoDebug)
      ::MessageBox(NULL, csCommand, _T("Starting program:\n"), MB_OK);

   PROCESS_INFORMATION sProcessInfo;
   ZeroMemory(&sProcessInfo, sizeof PROCESS_INFORMATION);

   STARTUPINFO sStartupInfo;
   ZeroMemory(&sStartupInfo, sizeof STARTUPINFO);
   sStartupInfo.cb = sizeof STARTUPINFO;
   //? ::GetStartupInfo(&startUpInfo);

   if (csParameters.GetLength() > 0)
      ::SetEnvironmentVariable(_T("LECTURNITY_PARAMETERS"), csParameters);

   DWORD dwCreateFlags = CREATE_DEFAULT_ERROR_MODE;
   if (bDoDebug)
      dwCreateFlags |= CREATE_NEW_CONSOLE;

   // NULL is important for first parameter otherwise the search path is not used.
   BOOL bSuccess = ::CreateProcess(NULL, csCommand.GetBuffer(csCommand.GetLength()),
      NULL, NULL, false, dwCreateFlags,
      NULL, NULL, &sStartupInfo, &sProcessInfo);
   csCommand.ReleaseBuffer();

   if (bSuccess && phProgram) {
      *phProgram = sProcessInfo.hProcess;
      dwProcessID = sProcessInfo.dwProcessId;
   }

   return bSuccess == TRUE;
}

BOOL CJavaStarterApp::InitInstance()
{
   CArray<CString, CString> aCommandParts;
   SplitCommandLine(&aCommandParts);

   CString csProgramPathAndName;
   if (aCommandParts.GetSize() > 0)
      csProgramPathAndName = aCommandParts[0];

   CString csProgramPath = csProgramPathAndName;
   if (csProgramPath.ReverseFind(_T('\\')) > -1)
      csProgramPath = csProgramPath.Left(csProgramPath.ReverseFind(_T('\\')));
   else
      csProgramPath = _T(".");


   // TODO: Make sure the path is set correctly to find nativeutils.dll
   //   (must be with full path...)?


#ifdef _UNICODE
   if (csProgramPath.GetLength() > 0 && !IsPathValid8Bit(csProgramPath))
   {
      int iLength = csProgramPath.GetLength();
      char szProgramPath[MAX_PATH];
      BOOL bUseDefaultChar = TRUE;
      ::WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, csProgramPath, iLength, szProgramPath, iLength, "?", &bUseDefaultChar);
      szProgramPath[iLength] = 0;

      CString csMessage;
      csMessage.Format(IDS_ERR_PATH_NOT_8BIT, ::GetACP(), szProgramPath);

      ::MessageBox(NULL, csMessage, _T("Error"), MB_OK | MB_ICONERROR);
      return FALSE;
   }
#endif
   //else there shouldn't be a non-Unicode version...

   
   CString csProgramName = GetProgramName(csProgramPathAndName);
   csProgramName.MakeLower();
   
   bool bDoDebug = false;

   if (csProgramName.Find(_T("_debug")) > -1)
      bDoDebug = true;
   
   CString csJavaCommand = GetJavaCommand(csProgramPathAndName, bDoDebug);


   _TCHAR tszCurrentDir[MAX_PATH];
   ::GetCurrentDirectory(MAX_PATH, tszCurrentDir);

   CString csNewCurrent = csProgramPath;

   if (_tcscmp(tszCurrentDir, (LPCTSTR)csNewCurrent) != 0)
      ::SetCurrentDirectory(csNewCurrent); // e.g. LPLibs.dll can then be found

#ifdef _DEBUG
   if (csProgramName.Find(_T("datgen")) > -1)
   {
      CreateLecturnityDat();
      return FALSE;
   }
#endif

   int iMaxJavaMB = FindJavaMemoryValue();


   CJavaStarterDlg dlg; // for autorun case
   HWND hWndSplash = NULL;
  
   // "Xms20M": Workaround for Bug #838:
   // Audio replay seems to be influenced by (major) garbage collections
   // on certain computers, therefore increase initial heap size.

   CString csMaxMemory;
   if (csProgramName.Find(_T("publisher")) >= 0)
   {
      if (iMaxJavaMB > 512)
         iMaxJavaMB = 512;

      // Publisher with Flv conversion can take quite some memory for large video sizes;
      // avoid hitting the "java boundary" of 1500MB virtual size.
      //
      // Bugfix for #3857, see also #3718, #3731
   }

   csMaxMemory.Format(_T(" -Xmx%dM"), iMaxJavaMB); 

   CString csCommand;
   csCommand.Format(_T("\"%s\" -Xms20M"), csJavaCommand);

   csCommand += csMaxMemory;


   int iMaxPixelCount = ((iMaxJavaMB/20)*1000000);
   
   csCommand += _T(" -Dpixel.limit=");
   CString csDummy;
   csDummy.Format(_T("%d"), iMaxPixelCount);
   csCommand += csDummy;
   

   bool bIsJavaStart = false;
   bool bPlayerJarIsValid = true;
   bool bIsPublisher = false;

   CString csProgramParameters;

   if (csProgramName.Find(_T("publisher")) > -1)
   {
      bIsPublisher = true;

      CString csPublisherJar = csProgramPath;
      csPublisherJar += _T("\\publisher.jar");

      if (_taccess(csPublisherJar, 04) != 0)
      {
         // publisher.jar not found in directory of publisher.exe
         
         CString csMessage;
         csMessage.LoadString(IDS_ERR_PUBLISHER_NOT_FOUND);

         if (bDoDebug) {
             csMessage += _T("\n");
             csMessage += csPublisherJar;
         }
         
         ::MessageBox(hWndSplash, csMessage, _T("Error"), MB_OK | MB_ICONERROR);
         return FALSE;
      }

      CString csLecturnityPath = csProgramPath;
      if (csLecturnityPath.ReverseFind(_T('\\')) > -1)
         csLecturnityPath = csLecturnityPath.Left(csLecturnityPath.ReverseFind(_T('\\'))); // ".."

      // Find out which Publisher components are installed:
      bool bHasCdPublisher  = false;
      bool bHasRealMedia    = false;
      bool bHasWindowsMedia = false;
      bool bHasMp4Video     = false;
      GetInstalledPublisherComponents(bHasCdPublisher, bHasRealMedia, bHasWindowsMedia, bHasMp4Video);
      
      // Build the command line:
      if (bHasCdPublisher)
         csCommand += _T(" -Dopt.cd=true");
      if (bHasRealMedia)
         csCommand += _T(" -Dopt.real=true");
      if (bHasWindowsMedia)
         csCommand += _T(" -Dopt.wmt=true");
      if (bHasMp4Video)
         csCommand += _T(" -Dopt.mp4=true");
      
      csCommand += _T(" -Dinstall.path=\"");
      csCommand += csLecturnityPath;
      csCommand += _T("\"");

      // Change ordering of any -D parameters (needed before -jar)
      for (int i=aCommandParts.GetSize()-1; i>0; i--) {
          CString sSingleCommand = aCommandParts[i];
          if (sSingleCommand.GetLength() > 1 && sSingleCommand.Left(2) == _T("-D")) {
              csCommand += _T(" ");
              csCommand += sSingleCommand;

              aCommandParts.RemoveAt(i);
         }
      }


      csCommand += _T(" -jar \"");
      csCommand += csPublisherJar;
      csCommand += _T("\"");
      
      if (aCommandParts.GetSize() > 1)
      {
         for (int i=1; i<aCommandParts.GetSize(); ++i)
         {
            csCommand += _T(" \"");
            csCommand += aCommandParts[i];
            csCommand += _T("\"");

            csProgramParameters += _T("\"");
            csProgramParameters += aCommandParts[i];
            csProgramParameters += _T("\"");
            
            if (i < aCommandParts.GetSize() - 1)
               csProgramParameters += _T(" ");
         }
      }

      // In order to find LPLibs.dll anytime.
      // 

      DWORD dwEnvSize = 0;
      dwEnvSize = ::GetEnvironmentVariable(_T("PATH"), NULL, 0);
      _TCHAR *tszEnvPath = new _TCHAR[dwEnvSize];
      tszEnvPath[0] = 0;
      ::GetEnvironmentVariable(_T("PATH"), tszEnvPath, dwEnvSize);

      CString csNewEnvPath = tszEnvPath;
      csNewEnvPath += _T(";");
      csNewEnvPath += csProgramPath;

      ::SetEnvironmentVariable(_T("PATH"), csNewEnvPath);
   }
   else // default player and autorun
   //if (csProgramName.Find(_T("player")) > -1)
   {
      bIsJavaStart = true;
     
      CString csPlayerJarName = GetPlayerJarName(csProgramPath);
      if (csPlayerJarName.GetLength() < 1)
      {
         bPlayerJarIsValid = false;

         CString csPlayerSetup = csProgramPath;
         csPlayerSetup += _T("\\setup.exe");

         bool bIsOnEvalCd = csProgramName.Find(_T("startplayer")) > -1;

         if (bIsOnEvalCd || _taccess(csPlayerSetup, 04) != 0) 
         {
            // either on exported cd (player can exist) or on eval cd (player must exist)

            CString csMessage;
            csMessage.LoadString(IDS_ERR_PLAYER_NOT_FOUND);
         
            ::MessageBox(hWndSplash, csMessage, _T("Error"), MB_OK | MB_ICONERROR);
            return FALSE;
         }
         else
         {
            // start Player setup instead; its existance was checked above

            csCommand = csPlayerSetup;
         }
      }

      if (bPlayerJarIsValid)
      {
         /* Other possibility instead of "-cp"; but doesn't help with javaw.exe and Unicode
         CString csNewClasspath = csPlayerJarName;
         csNewClasspath += _T(";");
         csNewClasspath += _tgetenv(_T("CLASSPATH"));
         
         ::SetEnvironmentVariable(_T("CLASSPATH"), csNewClasspath);
         */

         csCommand += _T(" -cp \"");
         csCommand += csPlayerJarName;
         csCommand += _T(";");
         
         _TCHAR *tszClassPath = _tgetenv(_T("CLASSPATH"));
         if (tszClassPath == NULL)
            tszClassPath = _T("");
         
         csCommand += tszClassPath;
         csCommand += _T("\"");
         
         csCommand += _T(" -Dhelp.path=\"");
         csCommand += csProgramPath;
         if (csProgramName.Find(_T("startplayer")) < 0)
         {
            // normal mode: manual in program directory

            KerberokHandler kh;
            _TCHAR tszLanguageCode[10]; tszLanguageCode[0] = 0;
            kh.GetLanguageCode(tszLanguageCode);

            if (_tcslen(tszLanguageCode) <= 0)
                _tcscpy_s(tszLanguageCode, 10, _T("en"));

            CString csHelpFile;//
            csHelpFile.Format(_T("\\player_%s.pdf"), tszLanguageCode);
            
            CString csHelpPath = csProgramPath;
            csHelpPath += csHelpFile;

            if (_taccess(csHelpPath, 04) == 0) {
                csCommand += csHelpFile;
                csCommand += _T("\"");
            } else {
                // try a fallback (maybe state of 2.0)

                CString csHelpPath = csProgramPath;
                csHelpPath += _T("\\player.pdf");

                if (_taccess(csHelpPath, 04) == 0)
                    csCommand += _T("\\player.pdf\"");
                else
                    csCommand += _T("\""); // leave path empty; this is an error
            }
         }
         else
            csCommand += _T("\\bin\\player.pdf\""); // one level down on eval cd
         
         csCommand += _T(" imc.epresenter.player.Manager");
        
         aCommandParts.RemoveAt(0); // remove "xyz.exe"
         
         if (aCommandParts.GetSize() > 0)
         {
            if (aCommandParts[0].GetAt(0) != _T('-'))
            {
               // first is directly a file name; can also be "index.html"
               aCommandParts.InsertAt(0, _T("-load"));
            }
            
            bool bPlayerExists = ConnectToPlayer(NULL, &aCommandParts);
            
            if (bPlayerExists)
               return FALSE; // existing player is informed; no further action needed
         }

         if (csProgramName.Find(_T("autorun")) > -1)
         {
            
            CString csDocStartHtml = csProgramPath;
            csDocStartHtml += _T("\\docs\\start.html");
      
            aCommandParts.Add(csDocStartHtml);
         }
         
         // no player running; start it (later on)
         if (aCommandParts.GetSize() > 0)
         {
            if (aCommandParts[0].CompareNoCase(_T("-time")) == 0)
            {
               // reorder parameters: player startup has another format than player network interface
               
               if (aCommandParts.GetSize() > 2)
               {
                  CString csTime = aCommandParts[2];
                  aCommandParts.InsertAt(1, csTime);
                  aCommandParts.RemoveAt(3);
               }
            }
            
            for (int i=0; i<aCommandParts.GetSize(); ++i)
            {
               csCommand += _T(" \"");
               csCommand += aCommandParts[i];
               csCommand += _T("\"");
               
               csProgramParameters += _T("\"");
               csProgramParameters += aCommandParts[i];
               csProgramParameters += _T("\"");
               
               if (i < aCommandParts.GetSize() - 1)
                  csProgramParameters += _T(" ");
            }
         }
      }

      if (csProgramName.Find(_T("autorun")) > -1 || csProgramName.Find(_T("startplayer")) > -1)
      {
         AfxEnableControlContainer();
         
         // Standardinitialisierung
         
#ifdef _AFXDLL
         Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
         Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif
         
         m_pMainWnd = &dlg;
         dlg.Create(IDD_JAVASTARTER_DIALOG);
         
         dlg.ShowWindow(CWinApp::m_nCmdShow);
         hWndSplash = dlg.m_hWnd;
         dlg.UpdateWindow();

         CString csCdNativeUtilsPath = csProgramPath;
         csCdNativeUtilsPath += _T("\\bin");
         ::SetCurrentDirectory(csCdNativeUtilsPath);
      }
   }
 
   if (hWndSplash != NULL)
   {
      CWnd *hWndStatus = m_pMainWnd->GetDlgItem(IDC_STC_STATUS);
      if (hWndStatus != NULL)
      {
         CString csStartProcess;
         csStartProcess.LoadString(IDS_START_PROCESS);
         hWndStatus->SetWindowText(csStartProcess);
      }
   }

   //::MessageBox(NULL, csCommand, _T("Command line"), MB_OK);

   HANDLE hProcess = NULL;
   DWORD dwProcessID = 0;
   bool bSuccess = StartProgram(csCommand, &hProcess, csProgramParameters, bDoDebug, dwProcessID);

   if (bSuccess)
   {
       if (bIsPublisher) {

           // Send the id of the java process to a pipe to acknowledge the startup of jave
           if (INVALID_HANDLE_VALUE != hProcess) {
               CString csMessage;
               csMessage.Format(_T("JAVA-PROCESS %d"), dwProcessID);

               LIo::MessagePipe pipe;
               HRESULT hrPipe = pipe.Init(_T("publisher_exe_pipe"), false);
               if (SUCCEEDED(hrPipe))
                   pipe.WriteMessage(csMessage);
               //LIo::SendPipeMessage(_T("\\\\.\\pipe\\publisher_exe_pipe"), csMessage.GetBuffer(csMessage.GetLength()));
               csMessage.ReleaseBuffer();
           }


           bool bIsDpt = false;
           for (int i=0; i<aCommandParts.GetSize(); ++i) {
               if (aCommandParts[i] == _T("-dpt") || aCommandParts[i] == _T("\"-dpt\"")) {
                   bIsDpt = true;
                   break;
               }
           }

           if (bIsDpt && INVALID_HANDLE_VALUE != hProcess) {
               // Wait for process finish, Powertrainer waits for publisher.exe
               
               DWORD dwJavaExitCode = STILL_ACTIVE;
               while (::GetExitCodeProcess(hProcess, &dwJavaExitCode) != 0 && dwJavaExitCode == STILL_ACTIVE) {
                   ::Sleep(100); // wait some more if still active
               }
           }

       }


      if (bIsJavaStart && bPlayerJarIsValid)
      {
         // also waits for the Player to start properly
         
         if (hWndSplash != NULL)
         {
            CWnd *hWndStatus = m_pMainWnd->GetDlgItem(IDC_STC_STATUS);
            if (hWndStatus != NULL)
            {
               CString csWaitPlayer;
               csWaitPlayer.LoadString(IDS_WAIT_FOR_PLAYER);
               hWndStatus->SetWindowText(csWaitPlayer);
            }
         }
         

         bool bConnectionSuccessful = ConnectToPlayer(hProcess, NULL);
         
         if (!bConnectionSuccessful)
         {
            CString csMessage;
            csMessage.LoadString(IDS_ERR_PLAYER_CONNECT);
            
            //::MessageBox(hWndSplash, csMessage, _T("Error"), MB_OK | MB_ICONERROR);
            // Note: This would probably be a doubled error message: first "document not found"
            // (for example) and second this message.
         }
      }
   }
   else
   {
      DWORD dwErrorCode = ::GetLastError();
      _TCHAR tszErrorMessage[300];
      ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErrorCode, 0, tszErrorMessage, 300, NULL);

      CString csMessage;
      csMessage.LoadString(IDS_ERR_PROGRAM_START);
      csMessage += _T("\n");
      csMessage + tszErrorMessage;

      ::MessageBox(hWndSplash, csMessage, _T("Error"), MB_OK | MB_ICONERROR);
   }

	return FALSE; // do not start message system
}

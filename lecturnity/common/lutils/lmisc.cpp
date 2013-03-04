#include "stdafx.h"
#include <shlobj.h>
#include <math.h>
#include <Wincrypt.h>
#include <mmsystem.h>

#include "lutils.h"

// Windows NT: Administrator Rights?
BOOL LMisc::IsWindowsNT()
{
   if (GetVersion()<0x80000000) {
      return TRUE;
   } else { 
      return FALSE;
   };
}

bool LMisc::IsWindows7()
{
    OSVERSIONINFOEX osver;
    DWORDLONG dwlConditionMask = 0;
    ZeroMemory(&osver, sizeof(OSVERSIONINFOEX));
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    // Win 7: 6.1
    osver.dwMajorVersion = 6;
    osver.dwMinorVersion = 1;
    osver.wProductType = VER_NT_WORKSTATION;

    // Initialize the condition mask
    VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, 
        VER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, 
        VER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_PRODUCT_TYPE, 
        VER_EQUAL );

    // Perform the test
    if ( TRUE == VerifyVersionInfo(&osver, 
                                   VER_MAJORVERSION | VER_MINORVERSION | VER_PRODUCT_TYPE,
                                   dwlConditionMask) )
        return true;
    else
        return false;
}

bool LMisc::IsWindowsVista()
{
    OSVERSIONINFOEX osver;
    DWORDLONG dwlConditionMask = 0;
    ZeroMemory(&osver, sizeof(OSVERSIONINFOEX));
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    // Win Vista: 6.0
    osver.dwMajorVersion = 6;
    osver.dwMinorVersion = 0;
    osver.wProductType = VER_NT_WORKSTATION;

    // Initialize the condition mask
    VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, 
        VER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, 
        VER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_PRODUCT_TYPE, 
        VER_EQUAL );

    // Perform the test
    if ( TRUE == VerifyVersionInfo(&osver, 
                                   VER_MAJORVERSION | VER_MINORVERSION | VER_PRODUCT_TYPE,
                                   dwlConditionMask) )
        return true;
    else
        return false;
}

bool LMisc::IsWindowsVistaOrHigher()
{
    OSVERSIONINFOEX osver;
    DWORDLONG dwlConditionMask = 0;
    ZeroMemory(&osver, sizeof(OSVERSIONINFOEX));
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    // Win Vista/7: 6.0, 6.1 or higher (and possible higher major versions: 7.0, 7.1, ...)
    osver.dwMajorVersion = 6;
    osver.dwMinorVersion = 0;
    osver.wProductType = VER_NT_WORKSTATION;

    // Initialize the condition mask
    VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, 
        VER_GREATER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, 
        VER_GREATER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_PRODUCT_TYPE, 
        VER_EQUAL );

    // Perform the test
    if ( TRUE == VerifyVersionInfo(&osver, 
                                   VER_MAJORVERSION | VER_MINORVERSION | VER_PRODUCT_TYPE,
                                   dwlConditionMask) )
        return true;
    else
        return false;
}

bool LMisc::IsWindowsXp()
{
    OSVERSIONINFOEX osver;
    DWORDLONG dwlConditionMask = 0;
    ZeroMemory(&osver, sizeof(OSVERSIONINFOEX));
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    // Win XP: 5.1
    osver.dwMajorVersion = 5;
    osver.dwMinorVersion = 1;
    osver.wProductType = VER_NT_WORKSTATION;

    // Initialize the condition mask
    VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, 
        VER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, 
        VER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_PRODUCT_TYPE, 
        VER_EQUAL );

    // Perform the test
    if ( TRUE == VerifyVersionInfo(&osver, 
                                   VER_MAJORVERSION | VER_MINORVERSION | VER_PRODUCT_TYPE,
                                   dwlConditionMask) )
        return true;
    else
        return false;
}

bool LMisc::IsWindows2000()
{
    OSVERSIONINFOEX osver;
    DWORDLONG dwlConditionMask = 0;
    ZeroMemory(&osver, sizeof(OSVERSIONINFOEX));
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    // Win 2000: 5.0
    osver.dwMajorVersion = 5;
    osver.dwMinorVersion = 0;
    osver.wProductType = VER_NT_WORKSTATION;

    // Initialize the condition mask
    VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, 
        VER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, 
        VER_EQUAL );
    VER_SET_CONDITION( dwlConditionMask, VER_PRODUCT_TYPE, 
        VER_EQUAL );

    // Perform the test
    if ( TRUE == VerifyVersionInfo(&osver, 
                                   VER_MAJORVERSION | VER_MINORVERSION | VER_PRODUCT_TYPE,
                                   dwlConditionMask) )
        return true;
    else
        return false;
}

__int64 LMisc::GetLocalTimeMs() {
    SYSTEMTIME st;
    FILETIME ft;
    ::GetLocalTime(&st);             
    ::SystemTimeToFileTime(&st, &ft);

    __int64 lTimeMs = (__int64)ft.dwHighDateTime << 32 | ft.dwLowDateTime;
    lTimeMs /= 10000; // now ms

    return lTimeMs;
}


bool LMisc::IsUserAdmin() {
    // Does the current user have administrator rights?
    // Also see CanElevate() below.

    if (!IsWindowsNT()) {
        return true;
    }
   
    BOOL bSuccess = TRUE;
    bool bIsAdmin = false;

    // Bugfix 5299: 
    // From the example code of 
    // http://msdn.microsoft.com/en-us/library/aa376389(VS.85).aspx
    // to be able to detect admin rights using "CheckTokenMembership"
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup; 
    bSuccess = AllocateAndInitializeSid(&NtAuthority,
        2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &AdministratorsGroup); 

    if(bSuccess) {
        BOOL bUserAdmin = FALSE;
        bSuccess = CheckTokenMembership(NULL, AdministratorsGroup, &bUserAdmin);

        if (bSuccess && bUserAdmin == TRUE)
            bIsAdmin = true;

        FreeSid(AdministratorsGroup); 
    }

    return bIsAdmin;
}


bool LMisc::CanUserElevate() {
    // User can be become an admin (elevation)
    // From http://www.akiwa.de/CFLecture/Tipp/Tipp7_DE.htm
    //
    // Note for a restricted account on Windows Xp this also returns true.
    // So use IsUserAdmin() before (which returns false).

    bool bIsAdmin = false;
    HANDLE hToken = 0;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
	  DWORD retLength = 0;
	  GetTokenInformation(hToken, TokenGroups, 0, 0, &retLength);
	  TOKEN_GROUPS* ptg = (TOKEN_GROUPS*)malloc(retLength);
      if (ptg) {
          if (GetTokenInformation(hToken, TokenGroups, ptg, retLength, &retLength)) {
              SID_IDENTIFIER_AUTHORITY ntauth = SECURITY_NT_AUTHORITY;
              void* pAdminSid = 0;
              if (!AllocateAndInitializeSid(&ntauth, 2,
                  SECURITY_BUILTIN_DOMAIN_RID,
                  DOMAIN_ALIAS_RID_ADMINS,
                  0, 0, 0, 0, 0, 0, &pAdminSid)) {
                      return false;
              }

              SID_AND_ATTRIBUTES* const end = ptg->Groups + ptg->GroupCount;
              SID_AND_ATTRIBUTES* it;
              for (it = ptg->Groups; end != it; ++it) {
                  if (EqualSid( it->Sid, pAdminSid))
                      break;
              }

              bIsAdmin = end != it;

              FreeSid(pAdminSid);
          }
          free(ptg);
      }

      CloseHandle(hToken);
    }

    return bIsAdmin;
}

/*
BOOL LMisc::IsUacEnabled() {
    // User Access Control (UAC)
    // From http://www.akiwa.de/CFLecture/Tipp/Tipp7_DE.htm

    LPCTSTR subKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System");
    LPCTSTR keyForValue = _T("EnableLUA");
    DWORD dwType = 0;
    DWORD dwVal = 0;
    DWORD dwValSize = sizeof( DWORD );

    HKEY hSecKey = NULL;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hSecKey) != ERROR_SUCCESS)
	    return FALSE;
    if (hSecKey == NULL)
	    return FALSE;

    LONG lRet = RegQueryValueEx(hSecKey, keyForValue, NULL, &dwType, (LPBYTE)&dwVal, &dwValSize);
    RegCloseKey(hSecKey);
    if (lRet == ERROR_SUCCESS) {
	    return dwVal != 0;
    }
    return FALSE;
}
*/

/* Doesn't work on Vista and 7; use the above 2 methods.
bool LMisc::IsKerberokWritable()
{
   HKEY hkLec = NULL;
   DWORD dwDisposition = 0;
   LONG res = ::RegCreateKeyEx(HKEY_LOCAL_MACHINE,
      _T("SOFTWARE\\Kerberok"), NULL, _T(""), REG_OPTION_NON_VOLATILE, 
      KEY_ALL_ACCESS, NULL, &hkLec, &dwDisposition);

   if (res == ERROR_ACCESS_DENIED)
      return false;
   else
      ::RegCloseKey(hkLec);

   return true;
}
//*/


bool LMisc::LaunchProgram(const TCHAR *tszProgram, const TCHAR *tszParameter, const bool bRunAsVistaAdmin)
{
   _TCHAR *tszVerb = _T("open");
   if (bRunAsVistaAdmin && IsWindowsVistaOrHigher())
      tszVerb = _T("runas");

   DWORD dwResult = (DWORD) ::ShellExecute(NULL, tszVerb, tszProgram, tszParameter, NULL, SW_SHOW);

   return (dwResult > 32);
}

bool LMisc::WaitForProgramFinish(DWORD dwProcessId, DWORD *pdwExitCode) {
    
    Sleep(100);

    HANDLE hProcess = ::OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);

    // Loop while process is active
    DWORD dwExitCode = 0;
    if (hProcess != NULL)
        dwExitCode = STILL_ACTIVE;
    while (dwExitCode == STILL_ACTIVE) {
        ::Sleep(100);
        ::GetExitCodeProcess(hProcess, &dwExitCode);
    }

    if (pdwExitCode != NULL)
        *pdwExitCode = dwExitCode;

    return hProcess != NULL;
}

bool LMisc::WaitForPublisherJavaFinish(LIo::MessagePipe *pPipe,
                                       _TCHAR *tszMessageBuffer, int iBufferLength, DWORD *pdwExitCode) {
    
    static _TCHAR tszDummyBuffer[256];
    if (tszMessageBuffer == NULL) {
        tszMessageBuffer = tszDummyBuffer;
        iBufferLength = 256;
    }

    bool bSomethingReceived = false;
    //if (hPublisherExePipe != INVALID_HANDLE_VALUE) {
    //    bSomethingReceived = LIo::ReceivePipeMessage(hPublisherExePipe, tszMessageBuffer, iBufferLength);
    //}

    if (pPipe != NULL)
        bSomethingReceived = SUCCEEDED(pPipe->ReadMessage(tszMessageBuffer, iBufferLength));

    if (bSomethingReceived) {
        _TCHAR *tszIdBegin = _tcschr(tszMessageBuffer, _T(' '));
        DWORD dwProcessId = _ttoi(tszIdBegin);

        bSomethingReceived = WaitForProgramFinish(dwProcessId, pdwExitCode);
    }

    return bSomethingReceived;
}

bool LMisc::GetLecturnityHome(_TCHAR *tszLecturnityHome)
{
   bool success = true;

   HKEY hkLec = NULL;
   success = (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, 
                                              _T("SOFTWARE\\imc AG\\LECTURNITY"), 
                                              0, KEY_READ, &hkLec));
   DWORD dwSize = 0;
   DWORD dwType = REG_SZ;
   if (success)
   {
      success = (ERROR_SUCCESS == ::RegQueryValueEx(hkLec, _T("LecturnityHome"), NULL,
         &dwType, NULL, &dwSize));
   }

   if (success)
   {
      success = (ERROR_SUCCESS == ::RegQueryValueEx(hkLec, _T("LecturnityHome"), NULL,
         &dwType, (LPBYTE) tszLecturnityHome, &dwSize));
   }

   if (!success)
   {
      GetMyDocumentsDir(tszLecturnityHome);
      _tcscat(tszLecturnityHome, _T("\\Lecturnity"));
       
      BOOL bResult = TRUE;
      if (_taccess(tszLecturnityHome, 00) != 0)
         bResult = CreateDirectory(tszLecturnityHome, NULL);

      if (bResult == TRUE)
         success = true;
   }

   ::RegCloseKey(hkLec);

   return success;
}

bool LMisc::GetApplicationDataDir(_TCHAR *tszApplicationDataDir)
{
   HRESULT hr = SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, tszApplicationDataDir);

   if (SUCCEEDED(hr))
      return true;
   else
      return false;
}

bool LMisc::GetMyDocumentsDir(_TCHAR *tszApplicationDataDir)
{
   HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, tszApplicationDataDir);

   if (SUCCEEDED(hr))
      return true;
   else
      return false;
}


bool LMisc::CheckNeedsFinalization(const _TCHAR *tszLrdPathName, _TCHAR *tszNewLrdPathNameTarget)
{
    if (tszLrdPathName == NULL)
        return false;

    int iLen = _tcslen(tszLrdPathName);

    if (iLen < 4 || tszLrdPathName[iLen - 4] != _T('.')) // should have a suffix
        return false;
    // TODO: these both may lead to silent non-working

    _TCHAR tszLepPathName[MAX_PATH]; tszLepPathName[0] = 0;
    _tcsncpy_s(tszLepPathName, MAX_PATH, tszLrdPathName, iLen);
    tszLepPathName[iLen - 4] = 0;
    _tcscat_s(tszLepPathName, MAX_PATH, _T(".lep"));

    // The following line did lead to a stack error. Why?
    //_tcsncpy_s(tszLepPathName + (iLen - 4), MAX_PATH, _T(".lep"), 4);
    //tszLepPathName[min(iLen, MAX_PATH - 1)] = 0;

    LFile fileLep(tszLepPathName);
    if (fileLep.Exists(true)) {
        bool bExportFound = false;

        _TCHAR tszLrdDir[MAX_PATH]; tszLrdDir[0] = 0;
        int iLastSlashPos = 0;
        bool bSuccess = LIo::GetDir(tszLrdPathName, tszLrdDir, &iLastSlashPos);

        if (bSuccess) {
            const _TCHAR *tszLrdDocName = tszLrdPathName + iLastSlashPos + 1;

            const int iRawLength = 2048;

            LBuffer bufRaw(iRawLength);
            DWORD dwRead = 0;

            // NOTE: This depends on the "<lastexportms>" being at the start
            //       of the lep file. This is done in CEditorProject::SaveProject().
            fileLep.Open(); // closed automatically on delete
            fileLep.ReadRaw(&bufRaw, 0, iRawLength - 1, &dwRead); // read one byte less

            if (dwRead > 0) {
                // NOTE: Encoding is only taken into account for the file name

                char szBuffer[iRawLength];
                strncpy_s(szBuffer, iRawLength, (const char*)(LPVOID)bufRaw, dwRead);
                szBuffer[dwRead] = 0; // make the string end

                bool bIsUtf8 = (szBuffer[0] & 0xff) == 0xef 
                    && (szBuffer[1] & 0xff) == 0xbb && (szBuffer[2] & 0xff) == 0xbf;

                _ASSERT(bIsUtf8); // used for filename decoding below

                // Search for last export time and path in the lep file.
                // 

                char szTime[100]; szTime[0] = 0;
                bool bTimeFound = false;

                _TCHAR tszPathLrdFromExport[MAX_PATH + 1]; tszPathLrdFromExport[0] = 0;
                bool bPathAndFileFound = false;

                bool bIsOriginalRecording = false; // unchanged

                char *szTagMs = strstr(szBuffer, "<lastexportms>");
                if (szTagMs != NULL) {
                    char *szTagEndMs = strstr(szTagMs, "</lastexportms>");

                    if (szTagEndMs != NULL) {
                        // everything alright: a proper lastexportms tag was found

                        szTagMs += strlen("<lastexportms>");
                        int iLen = szTagEndMs - szTagMs;
                        strncpy_s(szTime, 100, szTagMs, iLen);
                        szTime[iLen] = 0;

                        bTimeFound = true;
                    }
                }

                char *szTagPath = strstr(szBuffer, "<lastexportpath>");
                if (szTagPath != NULL) {
                    char *szTagEndPath = strstr(szTagPath, "</lastexportpath>");

                    if (szTagEndPath != NULL) {
                        // everything alright: a proper lastexportpath tag was found

                        szTagPath += strlen("<lastexportpath>");
                        char szPath[MAX_PATH * 3]; // utf-8
                        int iLen = szTagEndPath - szTagPath;
                        strncpy_s(szPath, MAX_PATH * 3, szTagPath, iLen);
                        szPath[iLen] = 0;

#ifndef _UNICODE
                        _ASSERT(false); // the below will not work if no Unicode project
#endif

                        int iWritten = ::MultiByteToWideChar(CP_UTF8, 0, szPath, iLen, tszPathLrdFromExport, MAX_PATH);
                        if (iWritten > 0)
                            tszPathLrdFromExport[iWritten] = 0;


                        if (_tcsicmp(tszPathLrdFromExport, L"original") == 0)
                            bIsOriginalRecording = true;

                        if (_taccess(tszPathLrdFromExport, 04) == 0)
                            bPathAndFileFound = true;
                    }
                }

                if (bTimeFound) {

                    // Construct a file/dir of the following format:
                    // <given path>\Export_<last time>\<document name>.lrd
                    // This is also used in the Editor/Studio when exporting automatically.

                    _TCHAR tszNewLrdPathName[MAX_PATH];
                    _stprintf(tszNewLrdPathName, _T("%s\\Export_%hS\\%s"), 
                        tszLrdDir, szTime, tszLrdDocName);

                    LFile fileLrdNew(tszNewLrdPathName);

                    if (fileLrdNew.Exists(true)) {
                        bExportFound = true;
                        if (tszNewLrdPathNameTarget != NULL)
                            _tcscpy_s(tszNewLrdPathNameTarget, MAX_PATH, tszNewLrdPathName);
                    }

                } else if (bIsOriginalRecording || bPathAndFileFound) {
                    bExportFound = true;

                    if (tszNewLrdPathNameTarget != NULL) { // is requested

                        _tcscpy_s(tszNewLrdPathNameTarget, MAX_PATH, 
                            bIsOriginalRecording ? tszLrdPathName : tszPathLrdFromExport);
                    }
                } 
            }
        }

        if (!bExportFound) // lep found but either no export or lrd was found
            return true;
    }

    return false;

}


bool LMisc::GetPrivateBuild(_TCHAR *tszTarget) {
    if (tszTarget == NULL)
        return false;

    // on error an empty string returned/string left untouched

    bool bQuerySuccess = false;

    DWORD dwVersionSize = 0;
    DWORD hVersionInfo = 0;

    _TCHAR tszFilename[2 * MAX_PATH];
    ZeroMemory(tszFilename, 2 * MAX_PATH * sizeof _TCHAR);
    _stprintf(tszFilename, _T("..\\AutoUpdate\\lver.dll"));

    // the above path does not always work; so try to find out the absolute installation path
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
            _stprintf(tszFilename, tszInstallDir);
            if (tszFilename[_tcslen(tszFilename) - 1] != _T('\\'))
                _tcscat(tszFilename, _T("\\"));
            _tcscat(tszFilename, _T("AutoUpdate\\lver.dll"));
        }
        RegCloseKey(hkKey);
    }

    dwVersionSize = ::GetFileVersionInfoSize(tszFilename, &hVersionInfo);
    if (dwVersionSize > 0)
    {
        BYTE *pBuffer = new BYTE[dwVersionSize];

        BOOL bSuccess = ::GetFileVersionInfo(tszFilename, hVersionInfo, dwVersionSize, pBuffer);

        if (bSuccess)
        {
            unsigned short *subBlock;
            unsigned int iLength = 0;

            if (VerQueryValue(pBuffer, _T("\\VarFileInfo\\Translation"), (void **)&subBlock, &iLength))
            {
                _TCHAR tszInfoKey[512];
                _TCHAR *tszInfoValue = NULL;
                iLength = 0;

                _stprintf(tszInfoKey, _T("\\StringFileInfo\\%04x%04x\\"), subBlock[0], subBlock[1]);
                _tcscat(tszInfoKey, _T("PrivateBuild"));

                if (VerQueryValue(pBuffer, tszInfoKey, (void **)&tszInfoValue, &iLength))
                {
                    _tcscpy_s(tszTarget, 10, tszInfoValue); // 10 is a guessed/safety value

                    bQuerySuccess = true;
                }
            }
        }

        delete[] pBuffer;
    }

    return bQuerySuccess;
}
// Create a random 64 bit value
__int64 LMisc::GetRandomValue() {

    HRESULT hr = S_OK;

    //-------------------------------------------------------------------
    // Acquire a cryptographic provider context handle.
    BOOL bSuccess = TRUE;
    HCRYPTPROV   hCryptProv;
    bSuccess = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    if (!bSuccess)
        hr = E_FAIL;

    // Generate a random initialization vector.
    __int64 iRandom = 0;
    if (SUCCEEDED(hr)) {
        bSuccess = CryptGenRandom(hCryptProv, sizeof(__int64), (BYTE *)&iRandom);
        if (!bSuccess)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr))
        return iRandom;
    else
        return 0;

}
//////////////////////////////////////////////////////
// from twlc but also used by lsutl32 (SG extended)
//////////////////////////////////////////////////////

// moved from twlc
struct BGR
{
   unsigned char b;
   unsigned char g;
   unsigned char r;
};

bool operator== (BGR c1, BGR c2)
{
   return c1.b == c2.b && c1.g == c2.g && c1.r == c2.r;
}

bool operator!= (BGR c1, BGR c2)
{
   return c1.b != c2.b || c1.g != c2.g || c1.r != c2.r;
}

// moved from twlc
int LMisc::calcPadding(int width, int stride)
{
   int padding = width*stride % 4 == 0 ? 0 : 4-(width*stride % 4);
   //if (javaCalling())
   //   padding = 0;
   return padding;
}


// moved from twlc
// PZI: return value changed from bool to int - will return number of changed pixels now
//
// [in]  bm1, bm2: bitmaps of pixeldata
// [in]  width, height: dimensions of bitmaps
// [in]  stride: bytes per pixel, e.g. 2 for 16 bit or 3 for 24 bit
// [in]  result: rectangle describing in which area pixel changes should be determined
// [out] result: outer bounds of change area
// [return] number of changed pixels
// note: RECT* result is used for input AND output
// note: rect.right, rect.bottom is included (may be confused with width and height where -1 must be applied)
//       e.g. rect (0,0,1023,767) and width x height 1024x768
int LMisc::determineChangeArea(unsigned char* bm1, unsigned char* bm2, 
                                    int width, int height, int stride, RECT* result)
{
   // count number of changed pixels
   int count = 0;
   int minX = width, maxX = 0, minY = height, maxY = 0;

   int padding = calcPadding(width, stride);

   for (int y=result->top; y<=result->bottom; ++y)
   {
      bool changeInLineFound = false;
      bool pixelChanged;
      
      int offset = (y*width + result->left)*stride + y*padding;
      for (int x=result->left; x<=result->right; ++x)
      {
         pixelChanged = false;
         if (stride == 2)
         {
            if ((*((short *) (bm1+offset))) != (*((short *) (bm2+offset))))
               pixelChanged = true;
         }
         else
         {
            if (*((BGR*)(bm1+offset)) != *((BGR*)(bm2+offset)))
               pixelChanged = true;
         }

         if (pixelChanged)
         {
            // count number of changed pixels
            count++;
            if (x < minX)
               minX = x;
            if (x > maxX)
               maxX = x;
            changeInLineFound = true;
         }

         offset += stride;
      }
      
      if (changeInLineFound)
      {
         if (y < minY)
            minY = y;
         if (y > maxY)
            maxY = y;
         
      }
   
   }

   
   if (minX <= maxX)
   {
      result->left = minX;
      result->top = minY;
      result->right = maxX;
      result->bottom = maxY;
   }

   // return number of changed pixels
   return count;
}

// moved from twlc
void LMisc::splitChangeArea(unsigned char* bm1, unsigned char* bm2, 
                                int width, int height, int stride, RECT bounds, 
                                RECT *areas, int *count)
{
   int SPLIT_SIZE = 30;


   int boundsWidth = bounds.right-bounds.left+1;
   int boundsHeight = bounds.bottom-bounds.top+1;

   if (boundsWidth > SPLIT_SIZE && boundsHeight > SPLIT_SIZE && 
      (boundsWidth > 2*SPLIT_SIZE || boundsHeight > 2*SPLIT_SIZE))
   {

      //
      // determine appropriate number of splits (below a maximum)
      //
      int countW = boundsWidth / SPLIT_SIZE;
      int countH = boundsHeight / SPLIT_SIZE;


      //char euf[200];
      
      /*
      sprintf(euf, "Bounds are = %d,%d %dx%d; Split Counts = %d, %d\n",
         bounds.left, bounds.top, boundsWidth, boundsHeight, countW, countH);
      DEBUG(euf);
      //*/

      if (countW*countH > *count)
      {
         SPLIT_SIZE = ceil(sqrt((boundsWidth*boundsHeight)/(double)*count));

         countW = boundsWidth / SPLIT_SIZE;
         countH = boundsHeight / SPLIT_SIZE;
      }

      /*
      sprintf(euf, "New Counts = %d, %d; New Split Size = %d\n", countW, countH, SPLIT_SIZE);
      DEBUG(euf);
      //*/

      int normW = boundsWidth / countW;
      int endW = boundsWidth % countW;

      int normH = boundsHeight / countH;
      int endH = boundsHeight % countH;

      //std::vector<RECT> changeVector;
      //changeVector.reserve(40);
      int rectCount = 0;

      for (int y=0; y<countH; ++y)
      {
         int idx = y*countW;
         for (int x=0; x<countW; ++x)
         {
            RECT sample;
            sample.left = bounds.left+x*normW;
            sample.top = bounds.top+y*normH;

            sample.right = sample.left+normW-1;
            sample.bottom = sample.top+normH-1;

            if (x == countW-1 && endW > 0)
               sample.right += endW;

            if (y == countH-1 && endH > 0)
               sample.bottom += endH;

            /*
            sprintf(euf, "1 Split area: %d,%d %dx%d\n", sample.left, sample.top,
               w(sample), h(sample));
            DEBUG(euf);
            //*/
            

            bool changed = false;
            if (determineChangeArea(bm1, bm2, width, height, stride, &sample)>0) //PZI: return value changed from bool to int
            {
               //changeVector.push_back(sample);
               areas[rectCount++] = sample;
               changed = true;
            }
            
            /*
            sprintf(euf, "2 %d Split area: %d,%d %dx%d\n", changed, sample.left, sample.top,
               w(sample), h(sample));
            DEBUG(euf);
            //*/


            ++idx;
         } // for countW
      } // for countH

      //std::vector<RECT>::iterator iter;
      ////for (int i=0; i<changeVector.size(); ++i)
      ////   areas[i] = changeVector[i];
      //int i=0;
      //for (iter = changeVector.begin(); iter != changeVector.end(); ++iter)
      //   areas[i++] = *iter;
      //*count = changeVector.size();
      *count = rectCount;
   }
   else // change area (bounds) is not big enough to be split
   {
      areas[0] = bounds;
      *count = 1;
   }
}
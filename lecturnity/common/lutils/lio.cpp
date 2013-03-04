#include "StdAfx.h"
#include "lutils.h"

int LIo::CountSubDirs(const _TCHAR *szPath)
{
   int nCount = 0;
   int nLen = _tcslen(szPath);
   for (int i=0; i<nLen; ++i)
   {
      if (szPath[i] == _TCHAR('\\'))
         ++nCount;
   }

   return nCount-1;
}

bool LIo::GetSubDir(const _TCHAR *szPath, int nIndex, _TCHAR *szSubDir)
{
   int nPos = 3;
   int nSubDir = 0;
   int nLen = _tcslen(szPath);
   while (nSubDir < nIndex && nPos < nLen)
   {
      if (szPath[nPos] == _TCHAR('\\'))
         nSubDir++;
      ++nPos;
   }

   if (nSubDir == nIndex)
   {
      int nFirstPos = nPos;
      while (nSubDir == nIndex && nPos < nLen)
      {
         if (szPath[nPos] == _TCHAR('\\'))
            nSubDir++;
         else
            ++nPos;
      }

      if (nSubDir == nIndex + 1)
      {
         _tcsncpy(szSubDir, szPath + nFirstPos, nPos - nFirstPos);
         szSubDir[nPos - nFirstPos] = _TCHAR('\000');
      }
      else
         return false;
   }
   else
      return false;

   return true;
}

bool LIo::GetDir(const _TCHAR *tszPath,  _TCHAR *tszParentDir, int *piLastSlashPos)
{
    if (tszPath == NULL || tszParentDir == NULL)
        return false;

    int iLen = _tcslen(tszPath);
    if (iLen > 1) {
        int iSlashPos = -1;
        for (int i=iLen-2; i>=0; --i) { // ignore possible last '\' at last positon
            if (tszPath[i] == _T('\\')) {
                iSlashPos = i;
                break;
            }
        }

        if (iSlashPos > -1) {
            _tcsncpy_s(tszParentDir, MAX_PATH, tszPath, iSlashPos); // copies without '\'
            if (piLastSlashPos != NULL)
                *piLastSlashPos = iSlashPos;
            return true;
        }
    }


    return false;
}

bool LIo::MakeRelativePath(_TCHAR *szRelative, const _TCHAR *szSource, const _TCHAR *szTarget)
{
   if (szRelative == NULL ||
       szSource == NULL ||
       szTarget == NULL)
      return false;

   // The drive qualifier is at least 3 characters long
   bool success = _tcslen(szSource) > 3 && _tcslen(szTarget) > 3;

   if (success)
   {
      // Is there a drive?
      success = (szSource[1] == _TCHAR(':')) &&
         (szTarget[1] == _TCHAR(':'));
   }

   if (success)
   {
      // Is it the same drive?
      success = (szSource[0] == szTarget[0]);
   }

   if (success)
   {
      // Count the subdirs contained in each path
      int nSrcDirs = CountSubDirs(szSource);
      int nTrgDirs = CountSubDirs(szTarget);

      // Now we want to match as many subdirs as possible
      // from the two paths. The maximum number of matches
      // is obviously the minimum number of subdirs.
      int nMaxMatches = nSrcDirs < nTrgDirs ? nSrcDirs : nTrgDirs;

      _TCHAR szSrcDir[_MAX_PATH];
      _TCHAR szTrgDir[_MAX_PATH];

      int nMatches = 0;
      bool bDirsMatch = true;
      while (bDirsMatch && success && nMatches < nMaxMatches)
      {
         // Retrieve both subdirs with index nMatches
         // and compare them. If they match, continue,
         // otherwise stop.
         success = GetSubDir(szSource, nMatches, szSrcDir);
         if (success)
            success = GetSubDir(szTarget, nMatches, szTrgDir);
         if (success)
         {
            bDirsMatch = (_tcscmp(szSrcDir, szTrgDir) == 0);
            if (bDirsMatch)
               ++nMatches;
         }
      }

      if (success)
      {
         // nMatches now contains the number of common subdirectories.
         // E.g. C:\Documents and Settings\Martin\My Documents\test.doc and
         // C:\Documents and Settings\Ulrich\My Documents\more tests.doc
         // have a match level of 1, they share the "Documents and Settings"
         // subdirectory. In order to express the latter relatively to the
         // first directory, I have to go back (N - nMatches) directories 
         // (using ..\), where N is the subdirectory count of szSource. Then
         // I have to take the rest of the szTarget subdirectory and add
         // them to the path. At last, the file name is added from szTarget,
         // and we're done.
         int nGoBack = nSrcDirs - nMatches;
         int nGoForward = nTrgDirs - nMatches;
         _tcscpy(szRelative, _T(""));
         int i;
         for (i=0; i<nGoBack; ++i)
            _tcscat(szRelative, _T("..\\"));
         for (i=0; i<nGoForward; ++i)
         {
            _TCHAR tmp[_MAX_PATH];
            GetSubDir(szTarget, nMatches + i, tmp);
            _tcscat(szRelative, tmp);
            _tcscat(szRelative, _T("\\"));
         }

         _TCHAR *pLastSlash = _tcsrchr((_TCHAR *)szTarget, _TCHAR('\\'));
         _tcscat(szRelative, pLastSlash + 1);
      }
   }

   if (!success)
   {
      // If something went wrong, we'll copy szTarget
      // into szRelative. That can't be that wrong.
      _tcscpy(szRelative, szTarget);
   }

   return true;
}

bool LIo::MakeAbsolutePath(_TCHAR *szAbsolute, const _TCHAR *szSource, const _TCHAR *szRelTarget)
{
   if (szAbsolute == NULL ||
       szSource == NULL ||
       szRelTarget == NULL)
      return false;

   // Is szRelTarget a full path already?
   if (_tcslen(szRelTarget) > 1)
   {
      if (szRelTarget[1] == _TCHAR(':'))
      {
         // Assume an absolute path
         _tcscpy(szAbsolute, szRelTarget);
         return true;
      }
   }

   // Now we know that szRelPath contains a relative path
   _TCHAR szTmp[2*_MAX_PATH];
   // Assemble a path possibly with ..\ in it
   _TCHAR *pLastSlash = _tcsrchr((_TCHAR *)szSource, _TCHAR('\\'));
   if (pLastSlash == NULL)
      return false;

   int nSlashPos = pLastSlash - szSource + 1; // +1 : preserve the \
   // Copy the path of the source (excluding any file name)
   _tcsncpy(szTmp, szSource, nSlashPos);
   szTmp[nSlashPos] = _TCHAR('\000');
   // Then append the relative target
   _tcscat(szTmp, szRelTarget);

   
   // If szTmp is longer than 260 characters "_tfullpath" fails.
   if (_tcslen(szTmp) > 256)
   {
      // remove last sub dir in szSource and first "..\" in szRelTarget
      // until length of szSource plus length of szRelTarget is less than 256
      TCHAR szTmpSource[_MAX_PATH];
      _tcscpy(szTmpSource, szSource);
      // remove file name and last slash
      TCHAR *szPos = _tcsrchr(szTmpSource, _TCHAR('\\'));
      if (szPos)
         *(szPos) = _TCHAR('\000');

      int iOffset = 0;
      bool success = true;
      while (success && _tcslen(szTmp) > 256)
      {
         if (_tcsncmp(szRelTarget+iOffset, _T("..\\"), 3) == 0)
            iOffset += 3;
         else
            success = false;
         // Remove last sub directory
         if (success) 
         {
            szPos = _tcsrchr(szTmpSource, _TCHAR('\\'));
            if (szPos)
               *(szPos) = '\000';
            else
               success = false;
         }
         // append shortened relative path to shortened absolute path
         if (success) 
         {
            _tcscpy(szTmp, szTmpSource);
            _tcscat(szTmp, _T("\\"));
            _tcscat(szTmp, szRelTarget + iOffset);
         }
      }
      // The concatenated path is to long and 
      // can not be shortened any more 
      if (!success)
         return false;
   }

   // Use _tfullpath to create a full path; in stdlib.h
   _tfullpath(szAbsolute, szTmp, _MAX_PATH);

   return true;
}

bool LIo::CopyFile(const _TCHAR *tszSource, const _TCHAR *tszTarget)
{
   bool bSuccess = true;

   /* will//should be done before (for example FileSdk::ResolveFileNode() --> LIo::CreateDirs()).
   _TCHAR tszTargetPath[MAX_PATH];
   ZeroMemory(tszTargetPath, MAX_PATH * sizeof _TCHAR);

   _TCHAR *tszLastSlash = _tcsrchr(tszTarget, _T('\\'));

   if (tszLastSlash != NULL)
   {
      int iSlashPos = tszLastSlash - tszTarget;
      _tcsncpy(tszTargetPath, tszTarget, iSlashPos);
      tszTargetPath[iSlashPos] = 0;

      if (_taccess(tszTargetPath, 00) != 0)
         bSuccess = ::CreateDirectory(tszTargetPath, NULL) == TRUE;
   }
   */

   if (bSuccess)
      bSuccess = ::CopyFile(tszSource, tszTarget, FALSE) == TRUE;

   return bSuccess;
}

bool LIo::CopyDir(const _TCHAR *szSource, const _TCHAR *szTarget, bool bDoNotRecurse)
{
   _TCHAR tszSourceDir[MAX_PATH];
   int nLen = _tcslen(szSource);
   _tcscpy(tszSourceDir, szSource);
   // pFrom requires a double NULL character at the end.
   if (tszSourceDir[nLen-1] == _T('\\'))
      tszSourceDir[nLen-1] = _TCHAR(0);
   else
      tszSourceDir[nLen+1] = _TCHAR(0);

   _TCHAR tszTargetDir[MAX_PATH];
   int nLenTarget = _tcslen(szTarget);
   _tcscpy(tszTargetDir, szTarget);
   // pFrom requires a double NULL character at the end.
   if (tszTargetDir[nLenTarget-1] == _T('\\'))
      tszTargetDir[nLenTarget-1] = _TCHAR(0);
   else
      tszTargetDir[nLenTarget+1] = _TCHAR(0);

   bool success = true;
   SHFILEOPSTRUCT shFOp;
   memset(&shFOp, 0, sizeof SHFILEOPSTRUCT);
   shFOp.hwnd = NULL;
   shFOp.wFunc = FO_COPY;
   shFOp.pFrom = tszSourceDir;
   shFOp.pTo = tszTargetDir;
   shFOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
   if (bDoNotRecurse)
       shFOp.fFlags |= FOF_NORECURSION;
   if (szSource[nLen-1] == _T('*') && bDoNotRecurse) // ends with *.*
       shFOp.fFlags |= FOF_FILESONLY;

   success = (0 == ::SHFileOperation(&shFOp));

   return success;
}

bool LIo::CreateDirs(const _TCHAR *szBase, const _TCHAR *szDirs)
{
   bool success = true;

   _TCHAR szTemp[_MAX_PATH];
   _TCHAR szDir[_MAX_PATH];

   int nDirCount = 1;
   const _TCHAR *tszSlash = _tcschr(szDirs, _T('\\'));
   while (tszSlash)
   {
      nDirCount++;
      tszSlash = _tcschr(tszSlash + 1, _T('\\'));
   }

   if (nDirCount > 1)
      tszSlash = _tcschr(szDirs, _T('\\'));
   else
      tszSlash = szDirs + _tcslen(szDirs);
   for (int i=0; i<nDirCount && success; ++i)
   {
      int nLength = tszSlash - szDirs;
      _tcsncpy(szDir, szDirs, nLength);
      szDir[nLength] = _TCHAR(0);

      _stprintf(szTemp, _T("%s\\%s"), szBase, szDir);
      DWORD dwFileAtts = ::GetFileAttributes(szTemp);
      if (dwFileAtts == 0xFFFFFFFF)
      {
         success = (FALSE != ::CreateDirectory(szTemp, NULL));
         if (success)
            dwFileAtts = ::GetFileAttributes(szTemp);
      }

      if (dwFileAtts == 0xFFFFFFFF)
      {
         success = false;
      }
      else
      {
         if ((dwFileAtts & FILE_ATTRIBUTE_DIRECTORY) == 0)
            success = false;
      }

      if (i < nDirCount - 2)
         tszSlash = _tcschr(tszSlash + 1, _T('\\'));
      else
         tszSlash = szDirs + _tcslen(szDirs);
   }

   return success;
}

bool LIo::CreateTempDir(const _TCHAR *tszPrefix, _TCHAR *tszPath, _TCHAR *tszDir)
{
   if (tszPrefix == NULL)
      return false;

   ::GetTempPath(MAX_PATH, tszPath);
   UINT ret = ::GetTempFileName(tszPath, tszPrefix, 0, tszDir);
   bool success = (ret != 0);
   _TCHAR tszFileName[MAX_PATH];
   if (success)
   {
      ::DeleteFile(tszDir);
      success = MakeRelativePath(tszFileName, tszPath, tszDir);
   }

   if (success)
   {
      _tcscpy(tszDir, tszFileName);
      success = CreateDirs(tszPath, tszDir);
   }

   return success;
}

bool LIo::DeleteDir(const _TCHAR *tszDir)
{
   /*
   bool success = true;
   WIN32_FIND_DATA findData;
   memset(&findData, 0, sizeof WIN32_FIND_DATA);
   LString lsDir;
   lsDir = tszDir;
   lsDir += _T("*");

   HANDLE handle = 0;
   while (handle != INVALID_HANDLE_VALUE && success)
   {
      handle = ::FindFirstFile(lsDir, &findData);
      if (handle != INVALID_HANDLE_VALUE)
      {
         if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0)
         {
            LString lsSubDir;
            lsSubDir = tszDir;
            lsSubDir += findData.cFileName;
            lsSubDir += _T("\\");
            success = DeleteDir(lsSubDir);
            if (success)
              success = (FALSE != ::RemoveDirectory(lsSubDir));
         }
      }
      else
      {
         LString lsFile;
         lsFile = tszDir;
         lsFile += findData.cFileName;
         success = (FALSE != ::DeleteFile(lsFile));
      }
   }
   */

   _TCHAR tszDir2[MAX_PATH];
   int nLen = _tcslen(tszDir);
   _tcscpy(tszDir2, tszDir);
   // pFrom requires a double NULL character at the end.
   if (tszDir2[nLen-1] == _T('\\'))
      tszDir2[nLen-1] = _TCHAR(0);
   else
      tszDir2[nLen+1] = _TCHAR(0);
   bool success = true;
   SHFILEOPSTRUCT shFOp;
   memset(&shFOp, 0, sizeof SHFILEOPSTRUCT);
   shFOp.hwnd = NULL;
   shFOp.wFunc = FO_DELETE;
   shFOp.pFrom = tszDir2;
   shFOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;

   success = (0 == ::SHFileOperation(&shFOp));

   return success;
}

bool LIo::DeleteFile(const _TCHAR *tszFile)
{
   return (0 != ::DeleteFile(tszFile));
}

/*
bool LIo::CreateLink(const _TCHAR *tszSource, const _TCHAR *tszTarget, const _TCHAR *tszComment)
{
   HRESULT hr = ::CoInitialize(NULL); 

   IShellLink* psl;
   
   // Get a pointer to the IShellLink interface. 
   hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
      IID_IShellLink, (LPVOID*)&psl); 
   if (SUCCEEDED(hr)) 
   { 
      IPersistFile* ppf; 
      
      // Set the path to the shortcut target and add the description. 
      psl->SetPath(tszSource); 
      psl->SetDescription(tszComment != NULL ? tszComment : _T("")); 
      
      // Query IShellLink for the IPersistFile interface for saving the 
      // shortcut in persistent storage. 
      hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
      
      if (SUCCEEDED(hr)) 
      { 
         WCHAR wsz[MAX_PATH];
         
#ifdef _UNICODE
         wcscpy(wsz, tszTarget);
#else
         MultiByteToWideChar(CP_ACP, 0, tszTarget, -1, wsz, MAX_PATH); 
#endif
         
         // Save the link by calling IPersistFile::Save. 
         hr = ppf->Save(wsz, TRUE); 
         ppf->Release(); 
      } 
      psl->Release(); 
   } 

   ::CoUninitialize();

   return SUCCEEDED(hr);
}
*/

__int64 LIo::GetFileSize(const _TCHAR *tszFileName)
{
   HANDLE hFile = ::CreateFile(tszFileName, GENERIC_READ, FILE_SHARE_READ, 
      NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

   bool success = (hFile != INVALID_HANDLE_VALUE);
   __int64 i64FileSize = -1;
   if (success)
   {
      DWORD dwLowBits = 0;
      DWORD dwHiBits = 0;

      dwLowBits = ::GetFileSize(hFile, &dwHiBits);
      DWORD dwError = ::GetLastError();
      if (dwLowBits == 0xFFFFFFFF && dwError != NO_ERROR)
      {
         // Failure!
         success = false;
      }
      else
      {
         // Assemble file size
         __int64 i64Low = dwLowBits;
         __int64 i64Hi  = dwHiBits;
         i64Hi = (i64Hi << 32);
         i64FileSize = i64Low | i64Hi;
      }

      ::CloseHandle(hFile);
   }

   if (!success)
      return -1;

   return i64FileSize;
}

__int64 LIo::GetDiskSpace(const _TCHAR *tszPath)
{
   if (tszPath == NULL)
      return -1i64;
   _TCHAR tszTemp[5*_MAX_PATH];
   _tcscpy(tszTemp, tszPath);

   _TCHAR *pcSlash = _tcsrchr(tszTemp, _T('\\'));
   if (NULL == pcSlash)
      return -1i64;

   *(pcSlash + 1) = 0;

   ULARGE_INTEGER ulDiskFreeForUser;
   ulDiskFreeForUser.QuadPart = 0i64;
   if (0 != ::GetDiskFreeSpaceEx(tszTemp, &ulDiskFreeForUser, NULL, NULL))
   {
      return (signed __int64) ulDiskFreeForUser.QuadPart;
   }
   else
   {
      return -1i64;
   }
}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
{ 
    return 1;
}

//////////////////////////////////////
// MessagePipe
//////////////////////////////////////

LIo::MessagePipe::MessagePipe() {
    m_hPipe = INVALID_HANDLE_VALUE;
}

LIo::MessagePipe::~MessagePipe() {
    if (m_hPipe != INVALID_HANDLE_VALUE)
        ::CloseHandle(m_hPipe);
}

HRESULT LIo::MessagePipe::Init(_TCHAR *tszPipeId, bool bCreate, DWORD dwDesiredAccess) {
    if (tszPipeId == NULL)
        return E_POINTER;

    int iIdLength = _tcslen(tszPipeId);

    if (iIdLength <= 0)
        return E_INVALIDARG;

    _TCHAR tszPipeName[MAX_PATH];
    tszPipeName[0] = 0;
    if (_tcsncmp(tszPipeId, _T("\\\\.\\pipe\\"), 9) != 0) {
        _tcscat_s(tszPipeName, MAX_PATH, _T("\\\\.\\pipe\\"));
        _tcscat_s(tszPipeName, MAX_PATH, tszPipeId);

        iIdLength = _tcslen(tszPipeName);
    } else {
        _tcsncpy_s(tszPipeName, MAX_PATH, tszPipeId, iIdLength + 1);
    }

    m_hPipe = INVALID_HANDLE_VALUE;

    if (bCreate) {
        int iBufferSize = 512;

        m_hPipe = ::CreateNamedPipe( 
            tszPipeName,              // pipe name 
            PIPE_ACCESS_DUPLEX,       // read/write access 
            PIPE_TYPE_MESSAGE |       // message type pipe 
            PIPE_READMODE_MESSAGE |   // message-read mode 
            PIPE_WAIT,                // blocking mode 
            PIPE_UNLIMITED_INSTANCES, // max. instances  
            iBufferSize,              // output buffer size 
            iBufferSize,              // input buffer size 
            0,                        // client time-out 
            NULL);                    // default security attribute
    } else {
        m_hPipe = ::CreateFile( 
            tszPipeName,    // pipe name 
            dwDesiredAccess,  // default value in header function. write access only (Bugfix 5904: No read/write access with GENERIC_READ | GENERIC_WRITE)
            0,              // no sharing 
            NULL,           // default security attributes
            OPEN_EXISTING,  // opens existing pipe 
            0,              // default attributes 
            NULL);          // no template file
    }

    if (m_hPipe == INVALID_HANDLE_VALUE)
        return E_FAIL;
    else
        return S_OK;
}

HRESULT LIo::MessagePipe::ReadMessage(_TCHAR *tszMessage, int iBufferLength) {
    if (m_hPipe == INVALID_HANDLE_VALUE)
        return E_UNEXPECTED;
    if (tszMessage == NULL)
        return E_POINTER;

    if (iBufferLength > 0) {
        bool bSuccess = false;

        // This is necessary as a "server" should not make a blocking read call
        // until a client is/was connected.
        BOOL bConnected = ::ConnectNamedPipe(m_hPipe, NULL);

        DWORD dwLastError = ::GetLastError();
        if (bConnected || dwLastError == ERROR_PIPE_CONNECTED) {
            DWORD dwRead = 0;
            bSuccess = ::ReadFile(m_hPipe, tszMessage, iBufferLength * sizeof _TCHAR, &dwRead, NULL) == TRUE;
        }

        return bSuccess ? S_OK : E_FAIL;
    } else
        return E_INVALIDARG;

}

HRESULT LIo::MessagePipe::ReadMessage(char *szMessage, int iBufferLength, DWORD &dwRead, bool bReconnect) {
    if (m_hPipe == INVALID_HANDLE_VALUE)
        return E_UNEXPECTED;
    if (szMessage == NULL)
        return E_POINTER;

    if (iBufferLength > 0) {
        bool bSuccess = false;

        if (bReconnect) {
            // This is necessary as a "server" should not make a blocking read call
            // until a client is/was connected.
            BOOL bConnected = ::ConnectNamedPipe(m_hPipe, NULL);

            DWORD dwLastError = ::GetLastError();
            if (bConnected || dwLastError == ERROR_PIPE_CONNECTED) {
                bSuccess = ::ReadFile(m_hPipe, szMessage, iBufferLength, &dwRead, NULL) == TRUE;
            }
        } else {
            bSuccess = ::ReadFile(m_hPipe, szMessage, iBufferLength, &dwRead, NULL) == TRUE;
        }


        return bSuccess ? S_OK : E_FAIL;
    } else
        return E_INVALIDARG;

}

HRESULT LIo::MessagePipe::PeekMessage(char *szMessage, int iBufferLength, DWORD &dwRead) {
    if (m_hPipe == INVALID_HANDLE_VALUE)
        return E_UNEXPECTED;

    if (szMessage == NULL)
        return E_POINTER;

    dwRead = 0;
    bool bSuccess = ::PeekNamedPipe(m_hPipe, szMessage, iBufferLength, &dwRead, NULL, NULL) == TRUE;
   
    return bSuccess ? S_OK : E_FAIL;
}

HRESULT LIo::MessagePipe::WriteMessage(const _TCHAR *tszMessage) {
    if (m_hPipe == INVALID_HANDLE_VALUE)
        return E_UNEXPECTED;
    if (tszMessage == NULL)
        return E_POINTER;

    int iLen = _tcslen(tszMessage);
    if (iLen > 0) {
        
#ifndef _UNICODE
#pragma message("Fail if not Unicode: Remote side expects WCHAR...
#endif

        DWORD dwWritten = 0;
        bool bSuccess = WriteFile(m_hPipe, tszMessage, (iLen + 1) * sizeof _TCHAR, &dwWritten, NULL) == TRUE;

        return bSuccess ? S_OK : E_FAIL;
    }

    return S_FALSE;
}


HRESULT LIo::MessagePipe::WriteCharMessage(const char *szMessage) {
    if (m_hPipe == INVALID_HANDLE_VALUE)
        return E_UNEXPECTED;
    if (szMessage == NULL)
        return E_POINTER;

    int iLen =  strlen(szMessage);
    if (iLen > 0) {

#ifndef _UNICODE
#pragma message("Fail if not Unicode: Remote side expects WCHAR...
#endif

        DWORD dwWritten = 0;
        bool bSuccess = WriteFile(m_hPipe, szMessage, iLen, &dwWritten, NULL) == TRUE;
        return bSuccess ? S_OK : E_FAIL;
    }

    return S_FALSE;
}

//////////////////////////////////////
// Input/output streams
//////////////////////////////////////

LIo::InputStream::InputStream()
{
   m_pBaseStream = NULL;
}

HRESULT LIo::InputStream::Init(InputStream *pBaseInputStream)
{
   if (NULL == pBaseInputStream)
      return E_POINTER;

   m_pBaseStream = pBaseInputStream;

   return S_OK;
}

HRESULT LIo::InputStream::ReadByte(UINT *pnTarget)
{
   // dummy implementation

   if (NULL == pnTarget)
      return E_POINTER;

   LBuffer dummy(1);
   UINT nRead = 0;
   HRESULT hr = ReadBytes(&dummy, 0, 1, &nRead);
   if (SUCCEEDED(hr) && nRead != 0)
      *pnTarget = *dummy.GetByteAccess();

   return hr;
}

/* // dummy implementation
HRESULT LIo::InputStream::Skip(UINT nByteCount)
{
   HRESULT hr = S_OK;

   LBuffer garbageBuffer(min(4096, nByteCount));

   UINT nBytesLeft = nByteCount;
   do
   {
      UINT nTodoBytes = min((unsigned) garbageBuffer.GetSize(), nBytesLeft);
      UINT nRead = 0;
      hr = ReadBytes(&garbageBuffer, 0, nTodoBytes, &nRead);
      if (SUCCEEDED(hr))
         nBytesLeft -= nRead;
   }
   while (SUCCEEDED(hr) && nBytesLeft > 0);

   return hr;
}
*/



LIo::XorInputStream::XorInputStream()
{
   m_pKeyBuffer = NULL;
   m_nBufferPointer = 0;
}

LIo::XorInputStream::~XorInputStream()
{
   if (NULL != m_pKeyBuffer)
      delete m_pKeyBuffer;
   m_pKeyBuffer = NULL;
}

HRESULT LIo::XorInputStream::Init(InputStream *pBaseInputStream, LBuffer *pKeyBuffer)
{
   HRESULT hr = S_OK;

   if (NULL == pBaseInputStream || NULL == pKeyBuffer)
      return E_POINTER;

   if (pKeyBuffer->GetSize() <= 0 || pKeyBuffer->GetAddedSize() <= 0)
      return E_INVALIDARG;

   InputStream::Init(pBaseInputStream);

   m_pKeyBuffer = new LBuffer(pKeyBuffer);
   if (NULL == m_pKeyBuffer)
      hr = E_OUTOFMEMORY;
   m_nBufferPointer = 0;

   return hr;
}

HRESULT LIo::XorInputStream::ReadBytes(LBuffer *pBuffer, UINT nOff, UINT nLen, UINT *pnRead)
{
   HRESULT hr = S_OK;

   if (NULL == pBuffer || NULL == pnRead)
      return E_POINTER;

   DWORD nReadBytes = 0;
   DWORD nKeyBytes = m_pKeyBuffer->GetSize();
   // this is very unprecise, as LBuffer automatically grows its maximum
   // size as needed (and from outside LBuffer this can hardly be controlled)
   // different reasons but same result go for ->GetAddedSize()

   BYTE *pBytes = pBuffer->GetByteAccess();
   BYTE *pKey = m_pKeyBuffer->GetByteAccess();
   for (UINT b=nOff; SUCCEEDED(hr) && b<nOff+nLen; ++b)
   {
      UINT btRead = 0;
      hr = m_pBaseStream->ReadByte(&btRead);
      if (SUCCEEDED(hr))
      {
         pBytes[b] = btRead ^ pKey[m_nBufferPointer % nKeyBytes];
         m_nBufferPointer++;
         nReadBytes++;
      }
      else
      {
         break;
      }
   }
   
   if (SUCCEEDED(hr)) // to do this is slightly horrific (2x)
      pBuffer->UpdateAddedSize(nOff + nReadBytes - pBuffer->GetAddedSize());
   
   if (SUCCEEDED(hr))
      *pnRead = nReadBytes;

   return hr;
}


HRESULT LIo::XorInputStream::Skip(UINT nByteCount)
{
   HRESULT hr = S_OK;

   m_nBufferPointer += nByteCount;

   hr = m_pBaseStream->Skip(nByteCount);

   return hr;
}

HRESULT LIo::XorInputStream::ResetKeyPointer()
{
   m_nBufferPointer = 0;

   return S_OK;
}




LIo::FileInputStream::FileInputStream()
{
   m_pFile = NULL;
   m_bDidOpen = false;
}

LIo::FileInputStream::~FileInputStream()
{
   if (NULL != m_pFile && m_bDidOpen)
      m_pFile->Close();
   m_pFile = NULL;
}

HRESULT LIo::FileInputStream::Init(LFile *pFile)
{
   HRESULT hr = S_OK;

   if (NULL == pFile)
      return E_POINTER;

   m_pFile = pFile;

   LURESULT luResult = m_pFile->Open();
   if (S_OK == luResult || LFILE_ERR_ALREADY_OPEN == luResult)
   {
      if (LFILE_ERR_ALREADY_OPEN != luResult)
         m_bDidOpen = true;

      // everything alright
   }
   else
   {
      hr = E_FAIL;
   }

   return hr;
}

HRESULT LIo::FileInputStream::ReadByte(UINT *pnTarget)
{
   return m_pFile->ReadSingleByte();
}

HRESULT LIo::FileInputStream::ReadBytes(LBuffer *pBuffer, UINT nOff, UINT nLen, UINT *pnRead)
{
   HRESULT hr = S_OK;

   if (NULL == pBuffer || NULL == pnRead)
      return E_POINTER;

   ULONG nRead = 0;
   LURESULT luResult = m_pFile->ReadRaw(pBuffer, nOff, nLen, &nRead);
   if (luResult != S_OK)
      hr = E_FAIL;
   else
      *pnRead = (UINT)nRead;
   return hr;
}

HRESULT LIo::FileInputStream::Skip(UINT nByteCount)
{
   HRESULT hr = S_OK;

   LURESULT luResult = m_pFile->Skip(nByteCount);
   if (S_OK != luResult)
      hr = E_FAIL;

   return hr;
}



LIo::BufferedInputStream::BufferedInputStream()
{
   m_nBufferSize = 0;
   m_nBufferContent = 0;
   m_nBufferPointer = 0;
   m_plBuffer = NULL;
   m_pBuffer = NULL;
}

LIo::BufferedInputStream::~BufferedInputStream()
{
   if (NULL != m_plBuffer)
      delete m_plBuffer;
   m_plBuffer = NULL;

   //if (NULL != m_pBuffer) // not needed; part of the LBuffer
   //   delete[] m_pBuffer;
   m_pBuffer = NULL;
}

HRESULT LIo::BufferedInputStream::Init(InputStream *pBaseInputStream, UINT nBufferSize)
{
   HRESULT hr = S_OK;

   if (NULL == pBaseInputStream)
      return E_POINTER;

   if (0 == nBufferSize)
      return E_INVALIDARG;

   InputStream::Init(pBaseInputStream);

   m_nBufferSize = nBufferSize;
   m_nBufferContent = 0;
   m_nBufferPointer = 0;
   m_plBuffer = new LBuffer(m_nBufferSize);
   if (NULL != m_plBuffer)
      m_pBuffer = m_plBuffer->GetByteAccess(); // new BYTE[m_nBufferSize];
   else
      hr = E_OUTOFMEMORY;

   return hr;
}

HRESULT LIo::BufferedInputStream::ReadByte(UINT *pnTarget)
{
   HRESULT hr = S_OK;

   if (NULL == pnTarget)
      return E_POINTER;

   if (m_nBufferPointer >= m_nBufferContent)
   {
      hr = FillBuffer();
      // if (FAILED(hr) there was no data to be found
   }

   if (SUCCEEDED(hr))
      *pnTarget = (UINT)m_pBuffer[m_nBufferPointer++]; // must be a positive value

   return hr;
}

HRESULT LIo::BufferedInputStream::ReadBytes(LBuffer *pBuffer, UINT nOff, UINT nLen, UINT *pnRead)
{
   HRESULT hr = S_OK;

   if (NULL == pBuffer || NULL == pnRead)
      return E_POINTER;

   if (m_nBufferPointer >= m_nBufferContent)
   {
      hr = FillBuffer();
      // if (FAILED(hr) there was no data to be found
   }


   BYTE *pTargetBytes = (pBuffer->GetByteAccess()) + nOff;

   bool bEndReached = false; // apparently not yet
   UINT nBytesCopied = 0;
   do
   {
      UINT nBufferContains = m_nBufferContent - m_nBufferPointer;
      UINT nTodoBytes = min(nLen, nBufferContains);
      
      memcpy(pTargetBytes, m_pBuffer + m_nBufferPointer, nTodoBytes);

      m_nBufferPointer += nTodoBytes;
      nBytesCopied += nTodoBytes;

      if (m_nBufferPointer >= m_nBufferContent && nBytesCopied < nLen) 
      {
         // buffer empty AND more to do?
         hr = FillBuffer();
         // if FAILED(hr) this IS an error: less read than requested
      }
      else
      {
         // there is something left in the buffer
         // so less than that was requested

         bEndReached = true;
      }
   } 
   while (!bEndReached && SUCCEEDED(hr));

   if (SUCCEEDED(hr))
      *pnRead = nBytesCopied;

   return hr; // might be E_EOF
}

HRESULT LIo::BufferedInputStream::Skip(UINT nByteCount)
{
   HRESULT hr = S_OK;


   UINT nCurrentContentLength = m_nBufferContent - m_nBufferPointer;

   if (nCurrentContentLength <= nByteCount)
      m_nBufferPointer += nByteCount;
   else 
   {
      // declare the buffer "empty"
      m_nBufferPointer = 0;
      m_nBufferContent = 0;
   }


   if (SUCCEEDED(hr))
      hr = m_pBaseStream->Skip(nByteCount);

   return hr;
}

/* private */ HRESULT LIo::BufferedInputStream::FillBuffer()
{
   HRESULT hr = m_pBaseStream->ReadBytes(m_plBuffer, 0, m_nBufferSize, &m_nBufferContent);
   if (SUCCEEDED(hr) && m_nBufferContent > 0)
      m_nBufferPointer = 0;
   else
      m_nBufferPointer = m_nBufferContent;

   return hr;
}

LIo::CounterInputStream::CounterInputStream()
{
   m_nBytesRead = 0;
   m_bIncludeSkipped = false;
}

LIo::CounterInputStream::~CounterInputStream()
{
}

HRESULT LIo::CounterInputStream::Init(InputStream *pBaseInputStream, bool bIncludeSkipped)
{
   m_bIncludeSkipped = bIncludeSkipped;
   return InputStream::Init(pBaseInputStream);
}
      
HRESULT LIo::CounterInputStream::ReadByte(UINT *pTarget)
{
   HRESULT hr = m_pBaseStream->ReadByte(pTarget);
   if (SUCCEEDED(hr))
      m_nBytesRead++;
   return hr;
}

HRESULT LIo::CounterInputStream::ReadBytes(LBuffer *pBuffer, UINT nOff, UINT nLen, UINT *pnRead)
{
   HRESULT hr = m_pBaseStream->ReadBytes(pBuffer, nOff, nLen, pnRead);
   if (SUCCEEDED(hr))
      m_nBytesRead += *pnRead;
   return hr;
}

HRESULT LIo::CounterInputStream::Skip(UINT nByteCount)
{
   HRESULT hr = S_OK;

   hr = m_pBaseStream->Skip(nByteCount);
   if (SUCCEEDED(hr) && m_bIncludeSkipped)
      m_nBytesRead += nByteCount;

   return hr;
}

UINT LIo::CounterInputStream::GetReadBytes()
{
   return m_nBytesRead;
}





LIo::OutputStream::OutputStream()
{
   m_pBaseStream = NULL;
}

HRESULT LIo::OutputStream::Init(OutputStream *pBaseOutputStream)
{
   if (NULL == pBaseOutputStream)
      return E_POINTER;

   m_pBaseStream = pBaseOutputStream;

   return S_OK;
}



LIo::FileOutputStream::FileOutputStream()
{
   m_pFile = NULL;
   m_bDidOpen = false;
}

LIo::FileOutputStream::~FileOutputStream()
{
   if (NULL != m_pFile && m_bDidOpen)
      m_pFile->Close();
   m_pFile = NULL;
}

HRESULT LIo::FileOutputStream::Init(LFile *pFile)
{
   HRESULT hr = S_OK;

   if (NULL == pFile)
      return E_POINTER;

   m_pFile = pFile;

   LURESULT luResult = m_pFile->Create(); // binary and without BOM
   if (S_OK == luResult)
   {
      // everything alright
      m_bDidOpen = true;
   }
   else
   {
      hr = E_FAIL;
   }

   return hr;
}

HRESULT LIo::FileOutputStream::WriteBytes(LBuffer *pBuffer, UINT nOff, UINT nLen)
{
   HRESULT hr = S_OK;

   if (NULL == pBuffer)
      return E_POINTER;

   LURESULT luResult = m_pFile->WriteRaw(pBuffer->GetByteAccess() + nOff, nLen);
   if (luResult != S_OK)
      hr = E_FAIL;

   return hr;
}

LIo::XorOutputStream::XorOutputStream()
{
   m_pKeyBuffer = NULL;
   m_pHelperBuffer = NULL;
   m_nBufferPointer = 0;
}

LIo::XorOutputStream::~XorOutputStream()
{
   if (m_pKeyBuffer)
      delete m_pKeyBuffer;
   m_pKeyBuffer = NULL;
 
   if (m_pHelperBuffer)
      delete m_pHelperBuffer;
   m_pHelperBuffer = NULL;
}

      
HRESULT LIo::XorOutputStream::Init(OutputStream *pBaseOutputStream, LBuffer *pKeyBuffer)
{
   HRESULT hr = S_OK;

   if (NULL == pBaseOutputStream || NULL == pKeyBuffer)
      return E_POINTER;

   if (pKeyBuffer->GetSize() <= 0 || pKeyBuffer->GetAddedSize() <= 0)
      return E_INVALIDARG;

   OutputStream::Init(pBaseOutputStream);

   m_pHelperBuffer = new LBuffer(1);

   m_pKeyBuffer = new LBuffer(pKeyBuffer);
   if (NULL == m_pKeyBuffer)
      hr = E_OUTOFMEMORY;

   m_nBufferPointer = 0;


   return hr;
}

HRESULT LIo::XorOutputStream::WriteBytes(LBuffer *pBuffer, UINT nOff, UINT nLen)
{
   HRESULT hr = S_OK;

   if (NULL == pBuffer)
      return E_POINTER;

   if (NULL == m_pKeyBuffer)
      return E_FAIL;

   BYTE *pBytes = pBuffer->GetByteAccess() + nOff;
   BYTE *pKeyBytes = m_pKeyBuffer->GetByteAccess();
   UINT nKeySize = m_pKeyBuffer->GetSize();
   for (UINT b=0; b<nLen && SUCCEEDED(hr); ++b)
   {
      BYTE btToWrite = pBytes[b] ^ pKeyBytes[m_nBufferPointer % nKeySize];
      m_pHelperBuffer->GetByteAccess()[0] = btToWrite;

      hr = m_pBaseStream->WriteBytes(m_pHelperBuffer, 0, 1);

      if (SUCCEEDED(hr))
         m_nBufferPointer++;
   }


   return hr;
}

HRESULT LIo::XorOutputStream::ResetKeyPointer()
{
   m_nBufferPointer = 0;
   return S_OK;
}



LIo::BufferedOutputStream::BufferedOutputStream()
{
   m_plBuffer = NULL;
   m_nBufferPointer = 0;
}

LIo::BufferedOutputStream::~BufferedOutputStream()
{
   Flush();

   if (m_plBuffer)
      delete m_plBuffer;
   m_plBuffer = NULL;
}

      
HRESULT LIo::BufferedOutputStream::Init(OutputStream *pBaseOutputStream, UINT nBufferSize)
{
   HRESULT hr = S_OK;

   if (NULL == pBaseOutputStream)
      return E_POINTER;

   if (nBufferSize < 2)
      return E_INVALIDARG;

   m_plBuffer = new LBuffer(nBufferSize);
   if (NULL == m_plBuffer)
      hr = E_OUTOFMEMORY;

   m_nBufferPointer = 0;

   OutputStream::Init(pBaseOutputStream);

   return hr;
}

HRESULT LIo::BufferedOutputStream::WriteBytes(LBuffer *pBuffer, UINT nOff, UINT nLen)
{
   HRESULT hr = S_OK;

   if (NULL == pBuffer)
      return E_POINTER;

   if (nLen >= (unsigned)m_plBuffer->GetSize()/4)
   {
      // no buffering needed, write directly
      hr = Flush();

      if (SUCCEEDED(hr))
         hr = m_pBaseStream->WriteBytes(pBuffer, nOff, nLen);
   }
   else
   {
      // enough room to copy it in the buffer?
      UINT nSpaceLeft = m_plBuffer->GetSize() - m_nBufferPointer;
      if (nSpaceLeft < nLen)
         hr = Flush(); // this makes enough room for sure (see above)

      // copy to buffer
      if (nLen > 1)
      {
         memcpy(m_plBuffer->GetByteAccess() + m_nBufferPointer, pBuffer->GetByteAccess() + nOff, nLen);
      }
      else
      {
         m_plBuffer->GetByteAccess()[m_nBufferPointer] = pBuffer->GetByteAccess()[nOff];
      }

      m_nBufferPointer += nLen;
   }


   return hr;
}


HRESULT LIo::BufferedOutputStream::Flush()
{
   HRESULT hr = S_OK;

   if (m_nBufferPointer > 0)
      hr = m_pBaseStream->WriteBytes(m_plBuffer, 0, m_nBufferPointer);

   m_nBufferPointer = 0;

   return hr;
}

    

LIo::DataOutputStream::DataOutputStream()
{
   m_lbHelperBuffer.SetSize(8);
}

LIo::DataOutputStream::~DataOutputStream()
{
   // helper buffer is a stack object; does not need to be cleaned
}

HRESULT LIo::DataOutputStream::Init(OutputStream *pBaseOutputStream)
{
   if (NULL == pBaseOutputStream)
      return E_POINTER;

   OutputStream::Init(pBaseOutputStream);

   return S_OK;
}
   

HRESULT LIo::DataOutputStream::WriteBytes(LBuffer *pBuffer, UINT nOff, UINT nLen)
{
   return m_pBaseStream->WriteBytes(pBuffer, nOff, nLen);
}

HRESULT LIo::DataOutputStream::WriteByte(int iValue)
{
   if (iValue > 255 || iValue < -128) // 0xff unsigned and signed
      return E_INVALIDARG;

   m_lbHelperBuffer.GetByteAccess()[0] = iValue;

   return WriteBytes(&m_lbHelperBuffer, 0, 1);

}

HRESULT LIo::DataOutputStream::WriteLeShort(int iValue)
{
   if (iValue > 65535 || iValue < -32768) // 0xffff unsigned and signed
      return E_INVALIDARG;

   BYTE *pBytes = m_lbHelperBuffer.GetByteAccess();
   pBytes[0] = (iValue >> 8) & 0xff;
   pBytes[1] = iValue & 0xff;

   return WriteBytes(&m_lbHelperBuffer, 0, 2);
}

HRESULT LIo::DataOutputStream::WriteLeInt(int iValue)
{
   BYTE *pBytes = m_lbHelperBuffer.GetByteAccess();
   pBytes[0] = (iValue >> 24) & 0xff;
   pBytes[1] = (iValue >> 16) & 0xff;
   pBytes[2] = (iValue >> 8) & 0xff;
   pBytes[3] = iValue & 0xff;

   return WriteBytes(&m_lbHelperBuffer, 0, 4);
}

HRESULT LIo::DataOutputStream::WriteUtf8WithLength(LString *plsValue)
{
   HRESULT hr = S_OK;

   if (NULL == plsValue)
      return E_POINTER;

   char *pszUtf8Text = LString::TCharToUtf8((LPTSTR)*plsValue);
   if (NULL == pszUtf8Text)
      hr = E_FAIL;

   int nByteLength = strlen(pszUtf8Text);
   if (SUCCEEDED(hr))
      hr = WriteLeShort(nByteLength); // length in bytes
   if (SUCCEEDED(hr))
   {
      LBuffer buffer(nByteLength);
      buffer.AddBuffer(pszUtf8Text, nByteLength);
      hr = WriteBytes(&buffer, 0, nByteLength);
   }

   if (NULL != pszUtf8Text)
      delete[] pszUtf8Text;

   return hr;
}


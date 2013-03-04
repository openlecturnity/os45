#include "StdAfx.h"
#include "lutils.h"

LFile::LFile(const _TCHAR *tszFileName)
{
   m_lsFileName = tszFileName;
   m_dwFileType = LFILE_TYPE_BINARY;
   m_bWriteBOM  = FALSE; 
   m_hFile      = INVALID_HANDLE_VALUE;
}

LFile::~LFile()
{
   Close();
}

LURESULT LFile::Create(DWORD dwFileType, BOOL bWriteBOM)
{
   m_dwFileType = dwFileType;

   m_hFile = ::CreateFile((LPCTSTR) m_lsFileName,
      GENERIC_WRITE,
      0,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      NULL);

   if (INVALID_HANDLE_VALUE == m_hFile)
      return LFILE_ERR_CREATE;

   if (FALSE != bWriteBOM)
   {
      if (dwFileType == LFILE_TYPE_TEXT_UTF8)
      {
         char szBom[] = {(char) 0xef, (char) 0xbb, (char) 0xbf};
         WriteRaw(szBom, 3);
      }
      else if (dwFileType == LFILE_TYPE_TEXT_UNICODE)
      {
         char szBom[] = {(char) 0xff, (char) 0xfe};
         WriteRaw(szBom, 2);
      }
   }

   return S_OK;
}

LURESULT LFile::Open()
{
   if (INVALID_HANDLE_VALUE == m_hFile)
   {
      m_hFile = ::CreateFile((LPCTSTR) m_lsFileName,
         GENERIC_READ,
         FILE_SHARE_READ,
         NULL,
         OPEN_EXISTING,
         FILE_ATTRIBUTE_NORMAL,
         NULL);
      
      if (INVALID_HANDLE_VALUE == m_hFile)
         return LFILE_ERR_OPEN;

      return S_OK;
   }
   else
      return LFILE_ERR_ALREADY_OPEN;

}

LURESULT LFile::Close()
{
   if (INVALID_HANDLE_VALUE != m_hFile)
      ::CloseHandle(m_hFile);
   m_hFile = INVALID_HANDLE_VALUE;

   return S_OK;
}

LURESULT LFile::Delete()
{
   BOOL ret = ::DeleteFile((LPCTSTR) m_lsFileName);
   if (0 == ret)
      return LFILE_ERR_FAILED;
   return S_OK;
}

bool LFile::Exists(bool bWithSize)
{
    // this maybe doesn't work as expected
    // ie it returns false if the file cannot be opened (it is already open)

    //   bool bResult = Open() == S_OK;
    //   Close();
    //   return bResult;

    if( (_taccess( (LPCTSTR) m_lsFileName, 0 )) != -1 ) {
        if (!bWithSize || GetSize() > 0)
            return true;
        else
            return false;
    } else
        return false;
}

__int64 LFile::GetSize() {
    LARGE_INTEGER liSize;
    ZeroMemory(&liSize, sizeof LARGE_INTEGER);

    bool bIsOpen = INVALID_HANDLE_VALUE != m_hFile;

    if (!bIsOpen)
        Open();

    if (INVALID_HANDLE_VALUE != m_hFile) {
        BOOL bSuccess = ::GetFileSizeEx(m_hFile, &liSize);

        if (!bIsOpen)
            Close();
    }

    return liSize.QuadPart;
}

__int64 LFile::LastModifiedMs() {
    __int64 lModifiedTime = -1;

    bool bIsOpen = INVALID_HANDLE_VALUE != m_hFile;

    if (!bIsOpen)
        Open();

    if (INVALID_HANDLE_VALUE != m_hFile) {
        FILETIME sWriteTime;
        BOOL bSuccess = ::GetFileTime(m_hFile, NULL, NULL, &sWriteTime);
        if (bSuccess) {
            ULARGE_INTEGER liLastWriteTime;
            memcpy(&liLastWriteTime, &sWriteTime, 8);
            lModifiedTime = liLastWriteTime.QuadPart / 10000;
        }

        if (!bIsOpen)
            Close();
    }


    return lModifiedTime;
}

bool LFile::IsEqualOrNewerThan(LFile *pOther) {
    if (pOther == NULL)
        return false;

    __int64 lTime1 = LastModifiedMs();
    __int64 lTime2 = pOther->LastModifiedMs();

    if (lTime1 == -1 || lTime2 == -1)
        return false;

    return lTime1 >= lTime2;
}

INT LFile::ReadSingleByte()
{
   if (INVALID_HANDLE_VALUE == m_hFile)
      return LFILE_ERR_NOTOPEN;

   BYTE btTheSingle = 0;

   DWORD dwRead = 0;
   BOOL bRet = ::ReadFile(m_hFile, &btTheSingle, 1, &dwRead, NULL);

   if (bRet == FALSE || dwRead != 1)
      return -1;
   else
      return btTheSingle;
}

LURESULT LFile::ReadRaw(LBuffer *pBuffer, DWORD dwOff, DWORD dwLen, DWORD *pdwReadBytes)
{
   if (INVALID_HANDLE_VALUE == m_hFile)
      return LFILE_ERR_NOTOPEN;
   if (pBuffer == NULL)
      return LFILE_ERR_INVALIDPARAMS;
   
   DWORD dwRead = 0;
   BOOL bRet = ::ReadFile(m_hFile, ((BYTE *)pBuffer->GetBuffer()) + dwOff, dwLen, &dwRead, NULL);

   if (bRet == FALSE)
      return LFILE_ERR_READ;

   // to do this slightly horrific
   pBuffer->UpdateAddedSize(dwOff + dwRead - pBuffer->GetAddedSize());
   *pdwReadBytes = dwRead;

   return S_OK;
}

LURESULT LFile::WriteRaw(const void *pBuffer, DWORD dwSize)
{
   if (INVALID_HANDLE_VALUE == m_hFile)
      return LFILE_ERR_NOTOPEN;
   if (pBuffer == NULL)
      return LFILE_ERR_INVALIDPARAMS;

   DWORD dwWritten = 0;
   BOOL ret = ::WriteFile(m_hFile, pBuffer, dwSize, &dwWritten, NULL);

   if (ret == 0 || (dwWritten != dwSize))
      return LFILE_ERR_WRITE;

   return S_OK;
}

LURESULT LFile::WriteText(const _TCHAR *tszText, int nLen /* =-1 */)
{
   if (INVALID_HANDLE_VALUE == m_hFile)
      return LFILE_ERR_NOTOPEN;
   if (tszText == NULL)
      return LFILE_ERR_INVALIDPARAMS;

   if (m_dwFileType == LFILE_TYPE_BINARY)
      return LFILE_ERR_INVALIDPARAMS;

   if (nLen == -1)
      nLen = _tcslen(tszText);

   int nReturn = S_OK;

#ifdef _UNICODE
   // UNICODE case, tszText is wchar *
   switch (m_dwFileType)
   {
   case LFILE_TYPE_TEXT_UNICODE:
      nReturn = WriteRaw(tszText, nLen * (sizeof(wchar_t))); // each character has 2 bytes
      break;

   case LFILE_TYPE_TEXT_ANSI:
      // Convert Unicode to ANSI codepage (plain char *)
      {
         char *szTemp = new char[nLen + 1];
         memset(szTemp, 0, nLen + 1);
         ::WideCharToMultiByte(CP_ACP, 0, tszText, nLen, szTemp, nLen + 1, NULL, NULL);
         nReturn = WriteRaw(szTemp, nLen);
         delete[] szTemp;
      }
      break;

   case LFILE_TYPE_TEXT_UTF8:
      // Convert Unicode to UTF8
      {
         int nUtfSize = ::WideCharToMultiByte(CP_UTF8, 0, tszText, nLen, NULL, 0, NULL, NULL);
         char *szTemp = new char[nUtfSize + 1];
         memset(szTemp, 0, nUtfSize + 1);
         ::WideCharToMultiByte(CP_UTF8, 0, tszText, nLen, szTemp, nUtfSize + 1, NULL, NULL);
         nReturn = WriteRaw(szTemp, nUtfSize);
         delete[] szTemp;
      }
      break;
   }
#else
   // ANSI case, tszText is char *
   switch (m_dwFileType)
   {
   case LFILE_TYPE_TEXT_UNICODE:
      {
         // Convert ANSI to Unicode
         wchar_t *wszTemp = new wchar_t[nLen + 1];
         memset(wszTemp, 0, (nLen + 1) * (sizeof(wchar_t)));
         ::MultiByteToWideChar(CP_ACP, 0, tszText, nLen, wszTemp, nLen + 1);
         nReturn = WriteRaw(wszTemp, nLen * (sizeof(wchar_t)));
      }
      break;
   case LFILE_TYPE_TEXT_ANSI:
      nReturn = WriteRaw(tszText, nLen);
      break;
   case LFILE_TYPE_TEXT_UTF8:
      {
         // Difficult. First translate to Unicode, then
         // to UTF8 (there is not direct way to do this)
         wchar_t *wszTemp = new wchar_t[nLen + 1];
         memset(wszTemp, 0, (nLen + 1) * (sizeof(wchar_t)));
         ::MultiByteToWideChar(CP_ACP, 0, tszText, nLen, wszTemp, nLen + 1);
         int nUtfSize = ::WideCharToMultiByte(CP_UTF8, 0, wszTemp, nLen, NULL, 0, NULL, NULL);
         char *szTemp = new char[nUtfSize + 1];
         memset(szTemp, 0, nUtfSize + 1);
         ::WideCharToMultiByte(CP_UTF8, 0, wszTemp, nLen, szTemp, nUtfSize + 1, NULL, NULL);
         nReturn = WriteRaw(szTemp, nUtfSize);
         delete[] szTemp;
         delete[] wszTemp;
      }
      break;
   }
#endif

   return nReturn;
}

LURESULT LFile::Skip(UINT nByteCount)
{
   if (INVALID_HANDLE_VALUE == m_hFile)
      return LFILE_ERR_NOTOPEN;

   DWORD dwResult = ::SetFilePointer(m_hFile, nByteCount, NULL, FILE_CURRENT);

   if (dwResult != -1)
      return S_OK;
   else
      return LFILE_ERR_FAILED;

}
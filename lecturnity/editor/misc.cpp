#include "StdAfx.h"
#include "misc.h"

void Misc::FormatTickString(CString &string, int millis, STREAM_UNIT su, bool expanded)
{
   // hack
   if (expanded)
      su = SU_MINUTES;

   switch (su)
   {
   case SU_MINUTES:
      {
         int hundreds = (millis + 5) / 10;
         int secs = hundreds / 100;
         hundreds = hundreds % 100;
         int mins = secs / 60;
         secs = secs % 60;
         int hours = mins / 60;
         mins = mins % 60;
         string.Format(_T("%d:%02d:%02d,%02d"), hours, mins, secs, hundreds);
      }
      break;

   case SU_SECONDS:
      {
         int t = millis / 10;
         string.Format(_T("%0.2f s"), ((double) t) / 100.0);
      }
      break;

   case SU_MILLIS:
      {
         string.Format(_T("%d"), millis);
      }
      break;

   case SU_FRAMES:
      {
      }
      break;
   }
}

void Misc::EnableChildWindows(CWnd *pRoot, BOOL bEnable)
{
   HWND hWnd = pRoot->GetSafeHwnd();
   if (hWnd == NULL)
      return;

   EnumChildWindows(hWnd, EnumChildProc, (LPARAM) bEnable);
}

BOOL Misc::EnumChildProc(HWND hWnd, LPARAM lParam)
{
   BOOL b = (BOOL) lParam;
   EnableWindow(hWnd, b);

   return TRUE;
}

char Misc::MakeHex(char c)
{
   if (c < 10)
      return (char) (c + 0x30);
   else
      return (char) (c + 0x57);
}

char Misc::MakeInt(char c)
{
   if (c > 0x39)
      return (char) (c - 0x57);
   else
      return (char) (c - 0x30);
}

char Misc::MakeChar(char c1, char c2)
{
   return (MakeInt(c1) << 4) | MakeInt(c2);
}

void Misc::Make4Bit(char *pSrc, char *pDest, int length)
{
   int destIndex = 0;
   for (int i=0; i<length; ++i)
   {
      pDest[destIndex++] = MakeHex((pSrc[i] & 0xf0) >> 4);
      pDest[destIndex++] = MakeHex(pSrc[i] & 0x0f);
   }
}

void Misc::Make8Bit(char *pSrc, char *pDest, int length)
{
   int byteLen = length / 2;
   int srcIndex = 0;
   for (int i=0; i<byteLen; ++i)
   {
      char c1 = pSrc[srcIndex++];
      char c2 = pSrc[srcIndex++];
      pDest[i] = MakeChar(c1, c2);
   }
}

void Misc::GetStreamConfigDataSizes(AM_MEDIA_TYPE *pMT, int *pMediaSize, int *pVideoHeaderSize)
{
   ASSERT(pMediaSize && pVideoHeaderSize);
   GetStreamConfigData(pMT, NULL, pMediaSize, NULL, pVideoHeaderSize);
}

bool Misc::GetStreamConfigData(AM_MEDIA_TYPE *pMT,
                               char *pMedia, int *pMediaSize,
                               char *pVideoHeader, int *pVideoHeaderSize)
{
   int mediaSize = sizeof AM_MEDIA_TYPE;
   int videoHeaderSize = pMT->cbFormat;

   if (pMedia == NULL && pVideoHeader == NULL)
   {
      if (pMediaSize)
         *pMediaSize = mediaSize * 2 + 1;
      if (pVideoHeaderSize)
         *pVideoHeaderSize = videoHeaderSize * 2 + 1;
      return true;
   }

   ASSERT(pMediaSize && pVideoHeaderSize);

   if (pMedia == NULL || pVideoHeader == NULL)
      return false;

   if ((*pMediaSize < mediaSize * 2 + 1) ||
       (*pVideoHeaderSize < videoHeaderSize * 2 + 1))
      return false;

   ASSERT(pMedia && pVideoHeader);

   Make4Bit((char *) pMT, pMedia, mediaSize);
   pMedia[*pMediaSize - 1] = '\000';
   if (pMT->pbFormat)
   {
      Make4Bit((char *) pMT->pbFormat, pVideoHeader, videoHeaderSize);
   }
   else
   {
      ASSERT(videoHeaderSize == 0);
   }
   ASSERT(*pVideoHeaderSize > 0);
   pVideoHeader[*pVideoHeaderSize - 1] = '\000';

   return true;
}

bool Misc::SetStreamConfigData(AM_MEDIA_TYPE *pMT,
                               BYTE *pMediaHex, int inMediaSize,
                               BYTE *pVideoHeaderHex, int inVideoHeaderSize)
{
   int mediaSize = inMediaSize / 2;
   int videoHeaderSize = inVideoHeaderSize / 2;

   if (mediaSize != sizeof AM_MEDIA_TYPE)
      return false;

   LPVOID pNewVideoHeader = NULL;
   if (videoHeaderSize > 0)
   {
      pNewVideoHeader = CoTaskMemAlloc(videoHeaderSize);
      if (pNewVideoHeader)
      {
         Make8Bit((char *) pVideoHeaderHex, (char *) pNewVideoHeader, inVideoHeaderSize);
      }
      else
         return false;
   }
   // Release any previously allocated format tag
   FreeMediaType(*pMT);
   Make8Bit((char *) pMediaHex, (char *) pMT, inMediaSize);
   pMT->pbFormat = (BYTE *) pNewVideoHeader; // may be NULL under certain circumstances

   return true;
}

void Misc::GetImageHome(CString &csImageHome)
{
   csImageHome.Empty();

   GetShellFolders(_T("My Pictures"), csImageHome);
}

bool Misc::GetShellFolders(LPCTSTR tszSubFolder, CString &csReturnValue)
{
	DWORD dwType;
   DWORD dwBufSize = 0;
   HKEY  hKeyUser = NULL; 

   LONG regErr = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), NULL, KEY_READ, &hKeyUser); 

   if (regErr == ERROR_SUCCESS)
      regErr = ::RegQueryValueEx(hKeyUser, tszSubFolder, NULL, &dwType, NULL, &dwBufSize );

	if ( dwType != REG_SZ )
      regErr = E_FAIL;

	BYTE *binData = new BYTE[dwBufSize + 8];

	if (regErr == ERROR_SUCCESS)
      regErr = ::RegQueryValueEx(hKeyUser, tszSubFolder, NULL, &dwType, (unsigned char *)binData, &dwBufSize );

   if (regErr == ERROR_SUCCESS)
   {
      _TCHAR *tszReturn = (_TCHAR *)binData;
      int iCharCount = dwBufSize / sizeof _TCHAR;
      tszReturn[iCharCount] = 0;
      
      csReturnValue = tszReturn;
   }

   delete[] binData;

   if (hKeyUser != NULL)
      RegCloseKey(hKeyUser);

   return regErr == ERROR_SUCCESS;
}

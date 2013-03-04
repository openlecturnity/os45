#include "Stdafx.h"
#include "lutils.h"

LString::LString()
{
   m_szString = NULL;
}

LString::LString(_TCHAR *szSource, bool bCopySource)
{
   if (szSource)
      m_szString = bCopySource ? _tcsdup(szSource) : szSource;
   else
      m_szString = NULL;
}

LString::LString(int nSize)
{
   m_szString = NULL;
   Allocate(nSize);
}

LString::~LString()
{
   Release();
}

void LString::Release()
{
   if (m_szString)
      delete m_szString;
   m_szString = NULL;
}

void LString::operator=(LString &lsSource)
{
   Release();
   if (lsSource.m_szString)
      m_szString = _tcsdup(lsSource.m_szString);
}

void LString::operator=(const _TCHAR *szSource)
{
   Release();
   if (szSource)
      m_szString = _tcsdup(szSource);
}

void LString::operator+=(_TCHAR *szAdd)
{
   if (szAdd != NULL)
   {
      int addLen = _tcslen(szAdd);
      int thisLen = (m_szString != NULL ? _tcslen(m_szString) : 0);
      _TCHAR *szTmp = new _TCHAR[addLen + thisLen + 1];
      if (m_szString)
         memcpy(szTmp, m_szString, thisLen * sizeof _TCHAR);
      memcpy((void *) (((char *) szTmp) + (thisLen * sizeof _TCHAR)), szAdd, addLen * sizeof _TCHAR);
      szTmp[addLen + thisLen] = (_TCHAR) 0;
      Release();
      m_szString = szTmp;
   }
}

void LString::operator+=(LString &lsAdd)
{
   (*this) += (LPTSTR) lsAdd;
}

bool LString::operator ==(const LString &lsCompare)
{
   return (_tcscmp(m_szString, lsCompare.m_szString) == 0);
}

bool LString::operator ==(const _TCHAR *szCompare)
{
   return (_tcscmp(m_szString, szCompare) == 0);
}

void LString::Allocate(int size)
{
   Release();
   m_szString = new _TCHAR[size + 1];
   memset(m_szString, 0, (size + 1) * sizeof _TCHAR);
}

void LString::AllocateAndSet(char *szContent, int nStringLength)
{
   // no error checking yet!

   if (0 == nStringLength)
      nStringLength = strlen(szContent);
   Allocate(nStringLength);
   
#ifndef _UNICODE
   memcpy(m_szString, szContent, nStringLength);
#else
   ::MultiByteToWideChar(CP_ACP, 0, szContent, nStringLength + 1, (LPWSTR)m_szString, nStringLength + 1);
#endif
}

void LString::AllocateAndSetUtf8(char *sUtf8String, int nByteLength)
{
   // no error checking yet!

   if (0 == nByteLength)
      nByteLength = strlen(sUtf8String);

   int iWideStringLength = 
      ::MultiByteToWideChar(CP_UTF8, 0, sUtf8String, nByteLength, NULL, 0);

   Allocate(iWideStringLength);

#ifndef _UNICODE
   WCHAR *wsBuffer = new WCHAR[iWideStringLength];
   ::MultiByteToWideChar(
      CP_UTF8, 0, sUtf8String, nByteLength, wsBuffer, iWideStringLength);
   ::WideCharToMultiByte(
      CP_ACP, 0, wsBuffer, iWideStringLength, 
      m_szString, iWideStringLength + 1, NULL, NULL);
   delete[] wsBuffer;

#else
   ::MultiByteToWideChar(CP_UTF8, 0, sUtf8String, nByteLength, (LPWSTR)m_szString, iWideStringLength + 1);
#endif
}

bool LString::NextToken(int *nBegin, int *nEnd, _TCHAR *szDelimiters)
{
   int nLength = this->Length();

   if (*nBegin >= 0 && *nBegin < nLength && szDelimiters != NULL)
   {
      int nTokenCharactersFound = 0;
      int nDelimiterCharactersIgnored = 0;

      int iSearch = *nBegin;
      for (; iSearch<nLength; ++iSearch)
      {
         if (IsDelimiter(m_szString[iSearch], szDelimiters))
         {
            if (nTokenCharactersFound > 0)
               break;
            else
               nDelimiterCharactersIgnored++;
               // do nothing more, just ignore any leading delimiters
         }
         else
         {
            nTokenCharactersFound++;
         }
      }

      if (nTokenCharactersFound > 0)
      {
         *nBegin += nDelimiterCharactersIgnored;
         *nEnd = *nBegin+nTokenCharactersFound;
         return true;
      }
      else
      {
         // there are no more tokens 
         // (otherwise the search wouldn't have stopped)
         _ASSERT(iSearch == nLength);

         return false;
      }
   }
   else 
   {
      // input parameters bogus, or search exhausted
      return false;
   }
}

bool LString::Substring(int nBegin, int nEnd, LString *lsTarget)
{
   int nLength = this->Length();

   if (nBegin >= 0 && nBegin < nEnd && nEnd <= nLength && lsTarget != NULL)
   {
      int nCopyLength = nEnd-nBegin;
      lsTarget->Allocate(nCopyLength);
      memcpy(lsTarget->m_szString, m_szString+nBegin, nCopyLength*sizeof _TCHAR);
      // lsTarget->m_szString[nCopyLength] = _T('\0');
      // null termination already done by Allocate()
      
      return true;
   }
   else
   {
      // input parameters are bogus
      return false;
   }

}

int LString::FindString(const _TCHAR *szSearchString, int nBegin)
{
   if (m_szString == NULL)
      return -1;
   if (szSearchString == NULL)
      return -1;
   if (_tcslen(szSearchString) == 0)
      return -1;
   _TCHAR *tcPos = _tcsstr((m_szString + nBegin), szSearchString);
   if (tcPos == NULL)
      return -1;

   return (int) (tcPos - m_szString);
}

bool LString::Contains(const _TCHAR *szString)
{
   return (FindString(szString) != -1);
}

// only short-hand, not faster
bool LString::StartsWith(const _TCHAR *szString)
{
   return (FindString(szString) == 0);
}

// Don't forget to delete[] the result if != NULL.
char *LString::TCharToUtf8(const _TCHAR *tszString)
{
   if (tszString == NULL)
      return NULL;

   WCHAR *wszText = NULL;
#ifndef _UNICODE
   int nLen = _tcslen(tszString);
   wszText = new WCHAR[nLen + 1];
   ::MultiByteToWideChar(CP_ACP, 0, tszString, -1, wszText, nLen + 1);
#else
   wszText = wcsdup(tszString);
#endif
   char *utfText = NULL;
   int nUtfLen = ::WideCharToMultiByte(CP_UTF8, 0, wszText, -1, NULL, 0, NULL, NULL);
   utfText = new char[nUtfLen+1];
   ::WideCharToMultiByte(CP_UTF8, 0, wszText, -1, utfText, nUtfLen+1, NULL, NULL);
   
   delete[] wszText;
   wszText = NULL;

   return utfText;
}

// Don't forget to delete[] the result if != NULL.
char *LString::Utf8ToTChar(const _TCHAR *tszString)
{
   if (tszString == NULL)
      return NULL;

   WCHAR *wszText = NULL;
#ifndef _UNICODE
   int nLen = _tcslen(tszString);
   wszText = new WCHAR[nLen + 1];
   ::MultiByteToWideChar(CP_UTF8, 0, tszString, -1, wszText, nLen + 1);
#else
   wszText = wcsdup(tszString);
#endif
   char *szText = NULL;
   int nUtfLen = ::WideCharToMultiByte(CP_UTF8, 0, wszText, -1, NULL, 0, NULL, NULL);
   szText = new char[nUtfLen+1];
   ::WideCharToMultiByte(CP_ACP, 0, wszText, -1, szText, nUtfLen+1, NULL, NULL);
   
   delete[] wszText;
   wszText = NULL;

   return szText;
}

// Don't forget to delete[] the result if != NULL.
char *LString::WideChar2Ansi(const _TCHAR *document) {
    if (document == NULL)
        return NULL;

    WCHAR *wszText = NULL;
#ifndef _UNICODE
    int nLen = _tcslen(document);
    wszText = new WCHAR[nLen + 1];
    ::MultiByteToWideChar(CP_ACP, 0, document, -1, wszText, nLen + 1);
#else
    wszText = wcsdup(document);
#endif

    int nUtfLen = ::WideCharToMultiByte(CP_ACP, 0, wszText, -1, NULL, 0, NULL, NULL);
    if (nUtfLen == 0) {
        return NULL;
    }
    char *ansiText = new char[nUtfLen+1];
    if (::WideCharToMultiByte(CP_ACP, 0, wszText, -1, ansiText, nUtfLen+1, NULL, NULL) == 0) {
        delete []ansiText;
        ansiText = NULL;
        delete []wszText;
        return NULL;
    }
    delete []wszText;

    return ansiText;
}

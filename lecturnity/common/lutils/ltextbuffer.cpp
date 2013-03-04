#include "StdAfx.h"
#include "lutils.h"

class LReplaceContainer
{
public:
   LReplaceContainer(UINT nStart, UINT nEnd, LPCTSTR tszReplace);
   ~LReplaceContainer();

   int GetSourceLength() { return m_nEndIdx - m_nStartIdx; }
   int GetReplaceLength() { return m_nReplaceLength; }
   int GetLengthDiff();

   UINT    m_nStartIdx;
   UINT    m_nEndIdx;
   _TCHAR *m_tszReplace;
   int     m_nReplaceLength;

   LReplaceContainer *m_pNext;
};

LReplaceContainer::LReplaceContainer(UINT nStart, UINT nEnd, LPCTSTR tszReplace)
{
   m_nEndIdx = nEnd;
   m_nStartIdx = nStart;
   if (tszReplace != NULL)
   {
      m_tszReplace = _tcsdup(tszReplace);
      m_nReplaceLength = _tcslen(tszReplace);
   }
   else
   {
      m_tszReplace = NULL;
      m_nReplaceLength = 0;
   }
   m_pNext = NULL;
}

LReplaceContainer::~LReplaceContainer()
{
   if (m_tszReplace)
      delete[] m_tszReplace;
   m_tszReplace = NULL;
}

int LReplaceContainer::GetLengthDiff()
{
   return _tcslen(m_tszReplace) - (m_nEndIdx - m_nStartIdx);
}

/* =================================================== */

LTextBuffer::LTextBuffer() : LBuffer()
{
   m_pPointer = NULL;
   m_pHead    = NULL;
   m_pTail    = NULL;
}

LTextBuffer::LTextBuffer(int nInitialSize) : LBuffer(nInitialSize)
{
   m_pPointer = NULL;
   m_pHead    = NULL;
   m_pTail    = NULL;
}

LTextBuffer::LTextBuffer(const char *szInitialString)
{
   m_pPointer = NULL;
   m_pHead    = NULL;
   m_pTail    = NULL;

   int nLen = strlen(szInitialString);
   SetSize((nLen + 1) * (sizeof _TCHAR));
   _TCHAR *tszBuffer = (_TCHAR *) GetBuffer();

#ifdef _UNICODE
   ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szInitialString, -1, tszBuffer, nLen + 1);
#else
   memcpy((void *) tszBuffer, (void *) szInitialString, nLen);
#endif
   tszBuffer[nLen] = (_TCHAR) 0;
}

LTextBuffer::LTextBuffer(const wchar_t *wszInitialString)
{
   m_pPointer = NULL;
   m_pHead    = NULL;
   m_pTail    = NULL;

   int nLen = wcslen(wszInitialString);
   SetSize((nLen + 1) * (sizeof _TCHAR));
   _TCHAR *tszBuffer = (_TCHAR *) GetBuffer();
#ifdef _UNICODE
   memcpy((void *) tszBuffer, (void *) wszInitialString, (sizeof _TCHAR) * nLen);
#else
   ::WideCharToMultiByte(CP_ACP, 0, wszInitialString, -1, tszBuffer, nLen + 1, NULL, NULL);
#endif
   tszBuffer[nLen] = (_TCHAR) 0;
}

LTextBuffer::~LTextBuffer()
{
   ReleaseContainers();
}

void LTextBuffer::ReleaseContainers()
{
   if (m_pHead)
   {
      LReplaceContainer *pCurrent = m_pHead;
      LReplaceContainer *pNext = NULL;
      while (pCurrent)
      {
         pNext = pCurrent->m_pNext;
         delete pCurrent;
         pCurrent = pNext;
      }
   }

   m_pHead = NULL;
}

bool LTextBuffer::LoadFile(LPCTSTR tszFileName)
{
   if (_taccess(tszFileName, 04) != 0)
      return false;
   HANDLE hFile = ::CreateFile(tszFileName,
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL);
   if (hFile == INVALID_HANDLE_VALUE)
   {
      DWORD dwError = ::GetLastError();
      return false;
   }

   bool success = true;

   DWORD nSize = ::GetFileSize(hFile, NULL);
   if (nSize == 0xFFFFFFFF)
      success = false;

   if (success)
      SetSize((nSize + 1) * sizeof _TCHAR);

   DWORD nRead = 0;
   if (success)
   {
      BYTE *aBuffer = new BYTE[nSize];
      success = ::ReadFile(hFile, aBuffer, nSize, &nRead, NULL) == TRUE;

      if (success)
      {
#ifdef _UNICODE
         ::MultiByteToWideChar(CP_ACP, 0, (const char *)aBuffer, nSize, (WCHAR *)GetBuffer(), GetSize() / sizeof _TCHAR);
#else
         memcpy(GetBuffer(), aBuffer, nSize);
#endif
      }

      delete[] aBuffer;
   }

   if (success && nRead != nSize)
      success = false;

   // Let's NULL-Terminate the string
   if (success)
      ((_TCHAR *) GetBuffer())[nSize] = (_TCHAR) 0;

   if (hFile)
      ::CloseHandle(hFile);

   if (success)
      m_pPointer = (_TCHAR *) GetBuffer();

   return success;
}

void LTextBuffer::Commit()
{
   // Are there any replace operations pending?
   if (m_pHead == NULL)
      return;

   // First let's calculate the new size of the buffer
   LReplaceContainer *pCurrent = m_pHead;
   int iDeltaChars = 0;
   while (pCurrent)
   {
      iDeltaChars += pCurrent->GetLengthDiff(); //_tcslen(pCurrent->m_tszReplace) - (pCurrent->m_nEnd - pCurrent->m_nStart);
      pCurrent = pCurrent->m_pNext;
   }

   int nNewCharCount = GetTextLength() + iDeltaChars;
   _TCHAR *tszNewBuffer = new _TCHAR[nNewCharCount + 1];
   ZeroMemory(tszNewBuffer, (nNewCharCount + 1) * sizeof _TCHAR);
 
   _TCHAR *tszBuffer = (_TCHAR *) GetBuffer();

   pCurrent = m_pHead;

   int iSource = 0;
   int iTarget = 0;
   while (pCurrent != NULL)
   {
      int iLeadingCharCount = pCurrent->m_nStartIdx - iSource;
      _tcsncpy(tszNewBuffer + iTarget, tszBuffer + iSource, iLeadingCharCount);
      iTarget += iLeadingCharCount;
      iSource += iLeadingCharCount;

      _tcsncpy(tszNewBuffer + iTarget, pCurrent->m_tszReplace, pCurrent->GetReplaceLength());
      iTarget += pCurrent->GetReplaceLength();
      iSource += pCurrent->GetSourceLength();

      pCurrent = pCurrent->m_pNext;
   }

   if (iTarget < nNewCharCount)
      _tcsncpy(tszNewBuffer + iTarget, tszBuffer + iSource, nNewCharCount - iTarget);

  tszNewBuffer[nNewCharCount] = 0;

   delete[] m_pBuffer;
   m_pBuffer = (void *) tszNewBuffer;
   m_nBufferSize = (nNewCharCount + 1) * (sizeof _TCHAR);

   ResetBufferPointer();
   ReleaseContainers();
}

bool LTextBuffer::SaveFile(LPCTSTR tszFileName, DWORD dwFileType, bool bWriteBOM)
{
   if (tszFileName == NULL)
      return false;

   Commit();

   bool success = true;
   LURESULT lRes = 0;
   LFile lFile(tszFileName);
   lRes = lFile.Create(dwFileType, bWriteBOM ? TRUE : FALSE);
   if (LFILE_OK == lRes)
      lRes = lFile.WriteText((const _TCHAR *) GetBuffer(), (GetSize() - 1) / sizeof _TCHAR);

   if (lRes != LFILE_OK)
      success = false;
   
   return success;
}

void LTextBuffer::SetBufferPointer(int nBufferPointer)
{
   m_pPointer = ((_TCHAR *) GetBuffer()) + nBufferPointer;
}

bool LTextBuffer::FindLineContaining(LPCTSTR tszNeedle, LPTSTR tszLine, DWORD dwBufLen /* =0xffffffff*/)
{
   if (tszLine == NULL)
   {
      ResetBufferPointer();
      return false;
   }

   _TCHAR *tszBuffer = NULL;
   if (m_pPointer)
      tszBuffer = m_pPointer;
   else
      tszBuffer = (_TCHAR *) GetBuffer();

   _TCHAR *tszFound = _tcsstr(tszBuffer, tszNeedle);

   if (tszFound == NULL)
   {
      ResetBufferPointer();
      return false;
   }

   // Find beginning of line
   _TCHAR *tszBeginning = tszFound;
   while (*tszBeginning != (_TCHAR) 10 && tszBeginning > tszBuffer)
      --tszBeginning;
   if (tszBeginning > tszBuffer)
      ++tszBeginning;
   // Find end of line
   _TCHAR *tszEnd = tszFound;
   while (*tszEnd != (_TCHAR) 10 && *tszEnd != (_TCHAR) 13 && *tszEnd != (_TCHAR) 0)
      ++tszEnd;

   DWORD dwLen = tszEnd - tszBeginning;
   if (dwLen >= dwBufLen - 1)
      dwLen = dwBufLen - 1;

   _tcsncpy(tszLine, tszBeginning, dwLen);
   tszLine[dwLen] = (_TCHAR) 0;

   m_pPointer = tszEnd;

   return true;
}

bool LTextBuffer::Find(LPCTSTR tszNeedle)
{
   _TCHAR *tszBuffer = NULL;
   if (m_pPointer)
      tszBuffer = m_pPointer;
   else
      tszBuffer = (_TCHAR *) GetBuffer();

   int nLen = _tcslen(tszNeedle);
   _TCHAR *tszFound = _tcsstr(tszBuffer, tszNeedle);
   if (tszFound)
   {
      m_pPointer = tszFound + nLen;
      return true;
   }
   else
   {
      ResetBufferPointer();
      return false;
   }
}

bool LTextBuffer::ReplaceAfter(LPCTSTR tszNeedle, LPCTSTR tszDelim, LPCTSTR tszReplaceWith)
{
   if (tszNeedle == NULL)
   {
      ResetBufferPointer();
      return false;
   }

   _TCHAR *tszRealBuffer = (_TCHAR *) GetBuffer();

   _TCHAR *tszBuffer = NULL;
   if (m_pPointer)
      tszBuffer = m_pPointer;
   else
      tszBuffer = (_TCHAR *) GetBuffer();

   _TCHAR *tszFound = _tcsstr(tszBuffer, tszNeedle);
   if (tszFound != NULL)
   {
      int nLen = _tcslen(tszNeedle);
      tszFound += nLen;
   }
   else
   {
      ResetBufferPointer();
      return false;
   }

   _TCHAR *tszDelimPos = _tcsstr(tszFound, tszDelim);
   if (tszDelimPos != NULL)
   {
      LReplaceContainer *pContainer = new LReplaceContainer(
         (UINT) (tszFound - tszRealBuffer),
         (UINT) (tszDelimPos - tszRealBuffer),
         tszReplaceWith);
      InsertContainer(pContainer);

      // Set the seek pointer
      m_pPointer = tszDelimPos;
      m_pPointer += _tcslen(tszDelim);
   }
   else
   {
      ResetBufferPointer();
      return false;
   }

   return true;
}

bool LTextBuffer::ReplaceAll(LPCTSTR tszNeedle, LPCTSTR tszReplace)
{
   ResetBufferPointer();
   if (tszNeedle == NULL)
      return false;

   _TCHAR *tszRealBuffer = (_TCHAR *) GetBuffer();
   _TCHAR *tszBuffer = tszRealBuffer;

   int nReplaceLength = _tcslen(tszNeedle);

   _TCHAR *tszFound = _tcsstr(tszBuffer, tszNeedle);
   while (tszFound)
   {
      LReplaceContainer *pContainer = new LReplaceContainer(
         (UINT) (tszFound - tszRealBuffer),
         (UINT) (tszFound - tszRealBuffer + nReplaceLength),
         tszReplace);

      InsertContainer(pContainer);

      tszBuffer = tszFound + nReplaceLength;
      tszFound = _tcsstr(tszBuffer, tszNeedle);
   }

   return true;
}

void LTextBuffer::InsertContainer(LReplaceContainer *pContainer)
{
   if (m_pHead == NULL)
   {
      // Empty list
      m_pHead = pContainer;
      m_pTail = pContainer;
      pContainer->m_pNext = NULL;
   }
   else
   {
      // Find insertion point
      LReplaceContainer *pCurrent = m_pHead;
      // Before head?
      if (pContainer->m_nStartIdx < m_pHead->m_nStartIdx)
      {
         m_pHead = pContainer;
         pContainer->m_pNext = pCurrent;
      }
      else if (pContainer->m_nStartIdx > m_pTail->m_nStartIdx)
      {
         // After tail
         pCurrent = m_pTail;
         m_pTail = pContainer;
         pCurrent->m_pNext = pContainer;
         pContainer->m_pNext = NULL;
      }
      else
      {
         // Somewhere in between
         LReplaceContainer *pPrev = NULL;
         while (pCurrent->m_nStartIdx < pContainer->m_nStartIdx)
         {
            pPrev = pCurrent;
            pCurrent = pCurrent->m_pNext;
         }

         // Insert between pPrev and pCurrent
         pPrev->m_pNext = pContainer;
         pContainer->m_pNext = pCurrent;

         // Head and tail are still correct
      }
   }
}

_TCHAR *LTextBuffer::GetString()
{
   return (_TCHAR *) GetBuffer();
}

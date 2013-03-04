#include "stdafx.h"
#include "ArrayOutput.h"

CArrayOutput::CArrayOutput()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   m_aBuffer = NULL;
   m_nMaximumLength = 0;
   m_nCurrentCount = 0;
   m_bDoIndentation = false;
   m_tszIndentSpaces = NULL;
   m_nIndentLength = 0;
   m_bFirstAfterReturn = true;
   m_bDoUtf8 = false;
}

CArrayOutput::~CArrayOutput()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   Clear();
}

// private
HRESULT CArrayOutput::Clear()
{
   if (m_aBuffer != NULL)
      delete[] m_aBuffer;
   m_aBuffer = NULL;

   if (m_tszIndentSpaces != NULL)
      delete[] m_tszIndentSpaces;
   m_tszIndentSpaces = NULL;

   return S_OK;
}

HRESULT CArrayOutput::Init(UINT nInitialSize, UINT nIndentLength, bool bDoUtf8)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   if (nInitialSize == 0)
      return E_INVALIDARG;
   
   if (nIndentLength >= nInitialSize)
      return E_INVALIDARG;

   Clear();

   m_aBuffer = new BYTE[nInitialSize];
   m_nMaximumLength = nInitialSize;
   m_nCurrentCount = 0;

   if (nIndentLength > 0)
   {
      m_tszIndentSpaces = new _TCHAR[nIndentLength];
      for (int i=0; i<(signed)nIndentLength; ++i)
         m_tszIndentSpaces[i] = (int)_T(' ');
      
      m_bDoIndentation = true;
   }
   else
   {
      m_bDoIndentation = false;
   }
   m_nIndentLength = nIndentLength;
   
   m_bDoUtf8 = bDoUtf8;

   m_bFirstAfterReturn = true;

   return S_OK;
}

HRESULT CArrayOutput::Reset()
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_nCurrentCount = 0;
   return S_OK;
}


HRESULT CArrayOutput::Append(CString &csAppend)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return Append((LPCTSTR)csAppend, csAppend.GetLength());
}
 

HRESULT CArrayOutput::Append(LPCTSTR tszAppend, int iLength)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (tszAppend == NULL)
      return E_POINTER;

   // automatically checks for returns at the end and makes indentation

   if (m_bDoIndentation && m_bFirstAfterReturn)
   {
      m_bFirstAfterReturn = false;
      hr = Append(m_tszIndentSpaces, m_nIndentLength);
   }

   int iStringLength = iLength > -1 ? iLength : _tcslen(tszAppend);

   int iAppendBytes = iStringLength;
   char *szAppend = new char[iStringLength * 3];
#ifdef _UNICODE
   UINT nCodepage = m_bDoUtf8 ? CP_UTF8 : CP_ACP;
   iAppendBytes = ::WideCharToMultiByte(nCodepage, 0, tszAppend, iStringLength, szAppend, iStringLength * 3, NULL, NULL);
#else
   strncpy(szAppend, tszAppend, iStringLength);
#endif

   if (SUCCEEDED(hr))
      hr = Append((BYTE *)szAppend, iAppendBytes);

   if (SUCCEEDED(hr) && m_bDoIndentation)
   {
      if (szAppend[iAppendBytes - 1] == '\n')
      {
         m_bFirstAfterReturn = true;
      }
   }

   delete[] szAppend;

   return hr;
}

HRESULT CArrayOutput::Append(BYTE *aBytes, UINT nByteCount)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (nByteCount > m_nMaximumLength-m_nCurrentCount)
   {
      // enlarge array

      UINT nNewMaximumLength = m_nMaximumLength;
      do
      {
         nNewMaximumLength *= 2;
      }
      while (nByteCount > nNewMaximumLength-m_nCurrentCount);

      BYTE *aNewBuffer = new BYTE[nNewMaximumLength];
      m_nMaximumLength = nNewMaximumLength;

      memcpy(aNewBuffer, m_aBuffer, m_nCurrentCount);
      delete[] m_aBuffer;
      m_aBuffer = aNewBuffer;
   }

   memcpy(m_aBuffer + m_nCurrentCount, aBytes, nByteCount);
   m_nCurrentCount += nByteCount;

   return S_OK;
}

HRESULT CArrayOutput::Append(DrawSdk::DrawObject *pObject, bool bUseFullImagePath)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (pObject == NULL)
      return E_POINTER;

   // TODO indentation

   DrawSdk::CharArray array;
   pObject->Write(&array, bUseFullImagePath);

   return Append(array.GetBuffer(), array.GetLength());
}

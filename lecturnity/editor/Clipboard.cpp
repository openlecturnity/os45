// Clipboard.cpp: Implementierung der Klasse CClipboard.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Clipboard.h"

CClipboard::CClipboard()
{
   m_mapData.InitHashTable(37);
   m_mapData.RemoveAll();
}

CClipboard::~CClipboard()
{
   // must clear objects pointed to by value pointers
   Clear();
}

HRESULT CClipboard::Clear()
{
   POSITION iter = m_mapData.GetStartPosition();
   UINT nKey = 0;
   CByteArray *pByteArrayOld = NULL;

   while (iter != NULL)
   {
      m_mapData.GetNextAssoc(iter, nKey, pByteArrayOld);
      delete pByteArrayOld;
   }

   m_mapData.RemoveAll();

   return S_OK;
}


HRESULT CClipboard::AddData(BYTE *pData, UINT nDataLength, UINT nClipboardKey)
{
   if (pData == NULL && nDataLength > 0)
      return E_POINTER;
   if (nClipboardKey == 0)
      return E_INVALIDARG;

   CByteArray *pByteArray = new CByteArray();
   pByteArray->SetSize(nDataLength, 0);
   memcpy(pByteArray->GetData(), pData, nDataLength);

   CByteArray *pByteArrayOld = NULL;
   if (m_mapData.Lookup(nClipboardKey, pByteArrayOld))
   {
      m_mapData.RemoveKey(nClipboardKey);
      delete pByteArrayOld;
   }

   m_mapData.SetAt(nClipboardKey, pByteArray);

   return S_OK;
}

HRESULT CClipboard::GetData(BYTE *pData, UINT *pnDataLength, UINT nClipboardKey)
{
   HRESULT hr = S_OK;

   if (pnDataLength == NULL)
      return E_POINTER;
   if (nClipboardKey == 0)
      return E_INVALIDARG;

   CByteArray *pByteArray = NULL;
   if (m_mapData.Lookup(nClipboardKey, pByteArray))
   {
      if (pData == NULL)
         *pnDataLength = pByteArray->GetSize();
      else
      {
         UINT nToCopy = min(*pnDataLength, (unsigned)pByteArray->GetSize());
         memcpy(pData, pByteArray->GetData(), nToCopy);
         *pnDataLength = nToCopy;
      }
   }
   else
      hr = S_FALSE; // element not found


   return hr;
}
// BaseStream.cpp: Implementierung der Klasse CBaseStream.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseStream.h"

CBaseStream::CBaseStream()
{
   m_pEditorProject = NULL;
   m_pUndoManager = NULL;
   m_nStreamLength = 0xffffffff;
}

CBaseStream::~CBaseStream()
{

}


HRESULT CBaseStream::Init(CEditorProject *pProject, CUndoRedoManager *pUndoManager, UINT nStreamLength)
{
   if (pUndoManager == NULL || pProject == NULL)
      return E_POINTER;

   m_pEditorProject = pProject;
   m_pUndoManager = pUndoManager;
   m_nStreamLength = nStreamLength;

   return S_OK;
}


HRESULT CBaseStream::Cut(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard)
{
   HRESULT hr = CutCopyOrDelete(nFromMs, nLengthMs, pClipboard, true, true);
   if (SUCCEEDED(hr))
      m_nStreamLength -= nLengthMs;
   return hr;
}

HRESULT CBaseStream::Copy(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard)
{
   return CutCopyOrDelete(nFromMs, nLengthMs, pClipboard, true, false);
}

HRESULT CBaseStream::Paste(UINT nToMs, UINT nLengthMs, CClipboard *pClipboard)
{
   HRESULT hr = S_OK;

   if (pClipboard == NULL)
      return E_POINTER;

   UINT nByteCount = 0;
   UINT nClipboardKey = (UINT)this;
   
   hr = pClipboard->GetData(NULL, &nByteCount, nClipboardKey);

   // will be used for copying from clipboard AND for undo information
   BYTE *pData = NULL;
   
   if (SUCCEEDED(hr) && nByteCount > 0)
   {
      pData = new BYTE[nByteCount];

      hr = pClipboard->GetData(pData, &nByteCount, nClipboardKey);
   }

   if (SUCCEEDED(hr))
      hr = this->InsertRange(nToMs, nLengthMs, pData, nByteCount, true);

   if (pData)
      delete[] pData;

   if (SUCCEEDED(hr))
      m_nStreamLength += nLengthMs;
   
   return hr;
}

HRESULT CBaseStream::Delete(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = CutCopyOrDelete(nFromMs, nLengthMs, NULL, false, true);
   if (SUCCEEDED(hr))
      m_nStreamLength -= nLengthMs;
   return hr;   
}

HRESULT CBaseStream::EnlargeStreamLength(UINT nAdditionalMs)
{
   m_nStreamLength += nAdditionalMs;
   
   // in order to have proper undo information
   InsertRange(m_nStreamLength, nAdditionalMs, NULL, 0, true); 

   return S_OK;
}


HRESULT CBaseStream::UndoAction(UndoActionId idActionCode, 
                                BYTE *pData, UINT nDataLength, 
                                UINT nPositionMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (idActionCode == UNDO_REMOVE_RANGE)
   {
      hr = this->InsertRange(nPositionMs, nLengthMs, pData, nDataLength, true);
      if (SUCCEEDED(hr))
         m_nStreamLength += nLengthMs;
   }
   else if (idActionCode == UNDO_INSERT_RANGE)
   {
      hr = this->CutCopyOrDelete(nPositionMs, nLengthMs, NULL, false, true);
      if (SUCCEEDED(hr))
         m_nStreamLength -= nLengthMs;
   }
   else
   {
      hr = this->SpecificUndo(idActionCode, pData, nDataLength, nPositionMs, nLengthMs);
   }


   return hr;
}

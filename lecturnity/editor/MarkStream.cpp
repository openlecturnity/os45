// MarkStream.cpp: Implementierung der Klasse CMarkStream.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\Studio\resource.h"
#include "MainFrm.h"
#include "MarkStream.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMarkStream::CMarkStream()
{
   m_aAllMarks.RemoveAll();
   m_aAllMarks.SetSize(0, 160);
   // other than "0": the array will instantly contain 160 elements
   
   m_pUndoManager = NULL;
   m_pSelectedMark = NULL;
   m_nSelectedIndex = 0;
   m_bSelectedDisabled = false;

   m_nJumpMarkCounter = 0;
	
   m_stopmarkBitmap[0].LoadBitmap(IDB_ICON_STOPMARK);
   m_stopmarkBitmap[1].LoadBitmap(IDB_ICON_STOPMARK_DISABLED);
   m_stopmarkBitmap[2].LoadBitmap(IDB_ICON_STOPMARK_DEMO);
   m_stopmarkBitmap[3].LoadBitmap(IDB_ICON_STOPMARK_DISABLED);

   m_targetmarkBitmap[0].LoadBitmap(IDB_ICON_TARGETMARK);
   m_targetmarkBitmap[1].LoadBitmap(IDB_ICON_TARGETMARK_DISABLED);
}

CMarkStream::~CMarkStream()
{

}


bool CMarkStream::CheckTime(UINT *pnNewPositionMs, UINT nLastPositionMs)
{
   if (m_pSelectedMark != NULL)
      return false;

   if (m_aAllMarks.GetSize() == 0)
      return false;

   if (*pnNewPositionMs <= nLastPositionMs)
      return false;

   for (int i=0; i<m_aAllMarks.GetSize(); ++i)
   {
      if (m_aAllMarks[i].IsStopMark())
      {
         UINT nMarkPositionMs = m_aAllMarks[i].GetPosition();

         if (*pnNewPositionMs >= nMarkPositionMs && nLastPositionMs < nMarkPositionMs)
         {
            *pnNewPositionMs = nMarkPositionMs;

            return true;
         }
      }
   }

   return false;
}

bool CMarkStream::IsExactStopAt(UINT nTimeMs)
{
   if (m_pSelectedMark != NULL)
      return false;

   if (m_aAllMarks.GetSize() == 0)
      return false;

   for (int i=0; i<m_aAllMarks.GetSize(); ++i)
   {
      if (m_aAllMarks[i].IsStopMark())
      {
         int iMarkPosMs = m_aAllMarks[i].GetPosition();
         if (iMarkPosMs == nTimeMs)
            return true;
      }
   }

   return false;
}


HRESULT CMarkStream::InsertMarks(UINT nToMs, UINT nLength, CWhiteboardStream *pWhiteboard, bool bUndoWanted)
{
   HRESULT hr = S_OK;
   
   if (m_pSelectedMark)
      return E_MS_SELECTION_ACTIVE;

   if (pWhiteboard != NULL && pWhiteboard->ContainsMarks())
   {
      UINT nElementCount = 0;
      hr = pWhiteboard->ExtractMarks(NULL, &nElementCount);

      CStopJumpMark *aCopyMarks = new CStopJumpMark[nElementCount];
      
      if (SUCCEEDED(hr))
         hr = pWhiteboard->ExtractMarks(aCopyMarks, &nElementCount);

      if (SUCCEEDED(hr))
         hr = InsertRange(nToMs, nLength, (BYTE *)aCopyMarks, nElementCount * sizeof CStopJumpMark, bUndoWanted);

      if (aCopyMarks)
         delete[] aCopyMarks;
   }
   else
      hr = InsertRange(nToMs, nLength, NULL, 0, bUndoWanted); // move other marks

   if (SUCCEEDED(hr))
      m_nStreamLength += nLength;

   return hr;
}

HRESULT CMarkStream::IncludeMark(CStopJumpMark *pMark, bool bIsUserAction)
{
   HRESULT hr = S_OK;

   if (pMark == NULL)
      return E_POINTER;

   if (m_pSelectedMark)
      return E_MS_SELECTION_ACTIVE;

   UINT nToMs = pMark->GetPosition();

   bool bInserted = false;
   for (int i=0; i<m_aAllMarks.GetSize(); ++i)
   {
      if (pMark->GetPosition() <= m_aAllMarks[i].GetPosition())
      {
         bool bDifferentPosition = pMark->GetPosition() != m_aAllMarks[i].GetPosition();
         bool bDifferentType = pMark->IsJumpMark() != m_aAllMarks[i].IsJumpMark();
         bool bSecondPresent = false;
         if (!bDifferentPosition && i+1 < m_aAllMarks.GetSize())
         {
            if (pMark->GetPosition() == m_aAllMarks[i+1].GetPosition())
            {
               // there can be two marks with the same time when their type
               // is different but there can never be three with different types
               // at the same time; so return an error
               
               bSecondPresent = true;
            }
         }

         if (bDifferentPosition || bDifferentType && !bSecondPresent)
         {
            m_aAllMarks.InsertAt(i, *pMark);
            bInserted = true;
         }
         else
         {
            hr = E_MS_MARK_EXISTS;
         }
         break;
      }
   }
   if (!bInserted && SUCCEEDED(hr))
      m_aAllMarks.Add(*pMark);

   if (pMark->IsJumpMark() && SUCCEEDED(hr))
      ++m_nJumpMarkCounter;

   if (m_pUndoManager != NULL && SUCCEEDED(hr) && !pMark->IsDemoDocumentObject())
   {
      hr = m_pUndoManager->RegisterAction(this, UNDO_INSERT_OBJECT, 
         (BYTE *)pMark, sizeof CStopJumpMark, HINT_CHANGE_NOTHING, nToMs, 0);

      // this is an action only concerning this stream (at the moment)
      if (SUCCEEDED(hr) && bIsUserAction)
         hr = m_pUndoManager->SetRollbackPoint();
      // else !bIsUserAction: probably an undo which handles rollback points itself
   }

   return hr;
}

HRESULT CMarkStream::RemoveMark(UINT nFromMs, UINT nLengthMs, bool bIsUserAction)
{
   HRESULT hr = S_OK;

   if (m_pSelectedMark)
      return E_MS_SELECTION_ACTIVE;

   UINT nStartIndex;
	CArray<CStopJumpMark *, CStopJumpMark *> aMarks;
   hr = FindMarksInRegion(aMarks, nFromMs, nLengthMs, &nStartIndex, false);
   if (SUCCEEDED(hr) && hr == S_OK)
   {
       if (m_pUndoManager != NULL && SUCCEEDED(hr) && !aMarks.GetAt(0)->IsDemoDocumentObject())
      {
         hr = m_pUndoManager->RegisterAction(this, UNDO_REMOVE_OBJECT, 
            (BYTE *)aMarks[0], sizeof CStopJumpMark, HINT_CHANGE_NOTHING, aMarks[0]->GetPosition(), 1);

         // this is an action only concerning this stream (at the moment)
         if (SUCCEEDED(hr) && bIsUserAction)
            hr = m_pUndoManager->SetRollbackPoint();
         // else !bIsUserAction: probably an undo which handles rollback points itself
      }
      
      if (SUCCEEDED(hr))
         m_aAllMarks.RemoveAt(nStartIndex);
   }

   return hr;
}


CStopJumpMark* CMarkStream::GetFirstMarkInRange(UINT nFromMs, UINT nLengthMs)
{
   UINT nStartIndex;
	CArray<CStopJumpMark *, CStopJumpMark *> aMarks;
   HRESULT hr = FindMarksInRegion(aMarks, nFromMs, nLengthMs, &nStartIndex);
   
   if (SUCCEEDED(hr))
   {
      if (aMarks.GetSize() > 0)
         return aMarks[0];
   }
 
   return NULL;
}

CStopJumpMark* CMarkStream::GetFirstStopMarkInRange(UINT nFromMs, UINT nLengthMs)
{
   UINT nStartIndex;
	CArray<CStopJumpMark *, CStopJumpMark *> aMarks;
   HRESULT hr = FindMarksInRegion(aMarks, nFromMs, nLengthMs, &nStartIndex);
   
   if (SUCCEEDED(hr))
   {
      for (int i=0; i<aMarks.GetSize(); ++i)
      {
         if (aMarks[i]->IsStopMark())
            return aMarks[i];
      }
   }
 
   return NULL;
}

CStopJumpMark* CMarkStream::GetLastStopMarkInRange(UINT nFromMs, UINT nLengthMs) {
    UINT nStartIndex;
    CArray<CStopJumpMark *, CStopJumpMark *> aMarks;
    HRESULT hr = FindMarksInRegion(aMarks, nFromMs, nLengthMs, &nStartIndex);

    if (SUCCEEDED(hr))
    {
        int pos = -1;
        for (int i=0; i<aMarks.GetSize(); ++i)
        {
            if (aMarks[i]->IsStopMark())
                //return aMarks[i];
                pos = i;
        }
        if (pos != -1)
            return aMarks[pos];
    }

    return NULL;
}

CStopJumpMark* CMarkStream::GetFirstJumpMarkInRange(UINT nFromMs, UINT nLengthMs)
{
   UINT nStartIndex;
	CArray<CStopJumpMark *, CStopJumpMark *> aMarks;
   HRESULT hr = FindMarksInRegion(aMarks, nFromMs, nLengthMs, &nStartIndex);
   
   if (SUCCEEDED(hr))
   {
      for (int i=0; i<aMarks.GetSize(); ++i)
      {
         if (aMarks[i]->IsJumpMark())
            return aMarks[i];
      }
   }
 
   return NULL;
}

HRESULT CMarkStream::GetJumpMarksInRegion(CArray<CStopJumpMark *, CStopJumpMark *> &caMarks, 
														 UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = S_FALSE;
	
   if (nLengthMs == 0)
      return E_INVALIDARG;

   for (int i=0; i<m_aAllMarks.GetSize(); ++i)
   {
		if (m_aAllMarks[i].IsJumpMark() 
         && m_aAllMarks[i].GetPosition() >= nFromMs && m_aAllMarks[i].GetPosition() < nFromMs + nLengthMs)
		{
         caMarks.Add(&m_aAllMarks[i]);
         hr = S_OK;
      }
   }
	
   return hr;
}

UINT CMarkStream::GetTimeForId(UINT nId)
{
   for (int i=0; i<m_aAllMarks.GetSize(); ++i)
   {
      if (m_aAllMarks[i].GetId() != 0 && m_aAllMarks[i].GetId() == nId)
         return m_aAllMarks[i].GetPosition();
   }

   return 0xffffffff; // no mark found
}

CStopJumpMark* CMarkStream::GetMarkForId(UINT nId)
{
   for (int i=0; i<m_aAllMarks.GetSize(); ++i)
   {
      if (m_aAllMarks[i].GetId() != 0 && m_aAllMarks[i].GetId() == nId)
         return &m_aAllMarks[i];
   }

   return NULL; // no mark found
}


HRESULT CMarkStream::Draw(CDC *pDC, CRect& rect, UINT nFromMs, UINT nLengthMs)
{
	CRect rcStream(rect);

	// draw background
	rcStream.DeflateRect(0, 3, 0, 2);
	COLORREF clrBg = CMainFrameE::GetColor(CMainFrameE::COLOR_STREAM_BG);
	CBrush bgBrush;
   bgBrush.CreateHatchBrush(HS_VERTICAL, clrBg);
	pDC->SetBrushOrg(rect.left, rect.top);
	pDC->FillRect(rcStream, &bgBrush);
	bgBrush.DeleteObject();

	// draw marks
	UINT nStartIndex;
	CArray<CStopJumpMark *, CStopJumpMark *> caMarks;
	FindMarksInRegion(caMarks, nFromMs, nLengthMs, &nStartIndex, false);

	CRect rcLastTargetmark(0, 0, 0, 0);
	for (int i = 0; i < caMarks.GetSize(); ++i)
	{
		CStopJumpMark *pStopJumpMark = caMarks[i];
      bool bMarkDisabled = m_bSelectedDisabled && (caMarks[i] == &m_MovedMark || caMarks[i] == m_pSelectedMark);

		// Draw stop- and targetmarks
		
		// pixel position of stop / jump mark
		UINT nPositionMs = pStopJumpMark->GetPosition();
		int nPositionPx = Calculator::GetPixelFromMsec(nPositionMs, rect.Width(), nLengthMs, nFromMs);
			
		// draw text
		if (pStopJumpMark->IsJumpMark())
		{ 
			// test if there is an intersection between last 
			if (!rcLastTargetmark.IsRectNull())
			{
			}

			CFont font;
			font.CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
								 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
			CFont *pOldFont = pDC->SelectObject(&font);
			
			// calculate textWidth
			CRect rectText(0, rect.top, 0, rect.bottom);
			pDC->DrawText(pStopJumpMark->GetLabel(), rectText, DT_SINGLELINE | DT_TOP | DT_LEFT | DT_VCENTER | DT_CALCRECT);
			
			UINT nTextOffset = 4;

			// draw background rectangle
			int nOldBkMode = pDC->SetBkMode(OPAQUE);
			int nOldBkColor = pDC->SetBkColor(RGB(255, 255, 255));
			CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
			CPen *pOldPen = pDC->SelectObject(&blackPen);
			
			//pDC->Rectangle(&rectTextBackground);
			
			// set current x-Position and width for text
			int iTextY = rect.top + (int)(((double)rect.Height() / 2) - ((double)rectText.Height() / 2));
			rectText.SetRect(rect.left + nPositionPx + 9, iTextY, 
								  rect.left + nPositionPx + rectText.Width() + 9, iTextY + rectText.Height());
			
			// draw text
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(0x55, 0x55, 0x55));
			pDC->DrawText(pStopJumpMark->GetLabel(), rectText, DT_SINGLELINE | DT_TOP | DT_LEFT | DT_VCENTER);
			
			// reset values	
			pDC->SelectObject(pOldFont);
			font.DeleteObject();
			pDC->SetBkMode(nOldBkMode);
			pDC->SetBkColor(nOldBkColor);			
			pDC->SelectObject(pOldPen);
			blackPen.DeleteObject();
			
		}


		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		
		CRect rcBitmap(0, 0, 13/*17*/, 18/*15*/);		// dimension of bitmap
		UINT nBitmapOffset = 5/*8*/;				// offset from left border to black line
		nPositionPx -= nBitmapOffset;
		// part of stop / jump mark out of left border
		/*if (nPositionPx < 0)
		{
			rcBitmap.left = -nPositionPx;
			nPositionPx = 0;
		}*/

		CBitmap *pBitmap = NULL;
		int nOffset = bMarkDisabled ? 1 : 0;

        if (pStopJumpMark->IsStopMark()){
            if(pStopJumpMark->IsDemoDocumentObject())
                nOffset += 2;
			pBitmap = &m_stopmarkBitmap[0 + nOffset];
        }
		else
			pBitmap = &m_targetmarkBitmap[0 + nOffset];
	
		CDC bmpDC;
		bmpDC.CreateCompatibleDC(pDC);
		bmpDC.SelectObject(pBitmap->m_hObject);
		TransparentBlt(pDC->m_hDC, rect.left + nPositionPx, rect.top+3, 
							rcBitmap.Width(), rcBitmap.Height(), 
							bmpDC.m_hDC, 0, 0, 
							rcBitmap.Width(), rcBitmap.Height(), RGB(0, 0, 0)); 

	}

   return S_OK;
}

HRESULT CMarkStream::SelectMark(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (nLengthMs == 0)
      return E_INVALIDARG;

   if (m_pSelectedMark)
      return E_MS_SELECTION_ACTIVE;

   UINT nStartIndex = 0;
   CArray<CStopJumpMark *, CStopJumpMark *> aMarks;
   hr = FindMarksInRegion(aMarks, nFromMs, nLengthMs, &nStartIndex);
   
   if (SUCCEEDED(hr))
   {
      if (aMarks.GetSize() > 0)
      {
		  m_pSelectedMark = aMarks[0];
         m_pSelectedMark->CloneTo(&m_MovedMark);
         m_nSelectedIndex = nStartIndex;

         hr = S_OK;
      }
      else
      {
         ResetMoveSelected();

         hr = S_FALSE;
      }
   }

   return hr;
}

HRESULT CMarkStream::MoveSelectedMark(int iMoveMs)
{
   if (!m_pSelectedMark)
      return E_MS_NO_MARK_SELECTED;

   if (iMoveMs > 0)
      return m_MovedMark.IncrementPosition(iMoveMs, m_nStreamLength);
   else if (iMoveMs < 0)
      return m_MovedMark.DecrementPosition(-iMoveMs);
   
   return S_FALSE;
}

HRESULT CMarkStream::GetMovedMarkTimes(int &iFromMs, int &iToMs) {
    HRESULT hr = S_OK;

   if (!m_pSelectedMark)
      return E_MS_NO_MARK_SELECTED;

   UINT nFromMs = m_pSelectedMark->GetPosition();
   UINT nToMs = m_MovedMark.GetPosition();
   iFromMs = nFromMs;
   iToMs = nToMs;
   return hr;
}

HRESULT CMarkStream::CommitMoveSelected(bool bIsUserAction)
{
   HRESULT hr = S_OK;

   if (!m_pSelectedMark)
      return E_MS_NO_MARK_SELECTED;

   UINT nFromMs = m_pSelectedMark->GetPosition();
   UINT nToMs = m_MovedMark.GetPosition();

   if (nFromMs != nToMs)
      hr = RemoveAndIntegrateMovedElement();

   if (SUCCEEDED(hr) && m_pUndoManager != NULL)
   {
      int iMoveDistance = (signed)nToMs - (signed)nFromMs;

      hr = m_pUndoManager->RegisterAction(
         this, UNDO_MOVE_OBJECT, NULL, 0, HINT_CHANGE_NOTHING, nFromMs, (unsigned)iMoveDistance);

      // this is an action only concerning this stream (at the moment)
      if (SUCCEEDED(hr) && bIsUserAction)
         hr = m_pUndoManager->SetRollbackPoint();
      // else !bIsUserAction: probably an undo which handles rollback points itself
   
   }

   ResetMoveSelected();

   return hr;
}

HRESULT CMarkStream::ResetMoveSelected()
{
   m_pSelectedMark = NULL;
   m_nSelectedIndex = 0;
   m_bSelectedDisabled = false;

   return S_OK;
}

HRESULT CMarkStream::EnableSelectedMark(bool bEnable)
{
   if (!m_pSelectedMark)
      return E_MS_NO_MARK_SELECTED;

   m_bSelectedDisabled = !bEnable;

   return S_OK;
}

bool CMarkStream::SelectedMarkIsUsed()
{
   if (m_pSelectedMark != NULL)
      return m_pSelectedMark->IsUsed();
   else
      return false;
}

HRESULT CMarkStream::RemoveSelectedMark(bool bIsUserInteraction)
{
   if (!m_pSelectedMark)
      return E_MS_NO_MARK_SELECTED;

   UINT nRemovePosition = m_pSelectedMark->GetPosition();

   ResetMoveSelected();

   return RemoveMark(nRemovePosition, 1, bIsUserInteraction);
}


HRESULT CMarkStream::SpecificUndo(UndoActionId idActionCode, 
                                  BYTE *pData, UINT nDataLength,
                                  UINT nPositionMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (idActionCode == UNDO_MOVE_OBJECT)
   {
      hr = SelectMark(nPositionMs+(signed)nLengthMs, 1);

      if (hr == S_OK)
      {
         hr = MoveSelectedMark((-1)*(signed)nLengthMs);

         if (SUCCEEDED(hr))
            hr = CommitMoveSelected(false);
      }
   }
   else if (idActionCode == UNDO_INSERT_OBJECT)
   {
      hr = RemoveMark(nPositionMs, 1, false);
   }
   else if (idActionCode == UNDO_REMOVE_OBJECT)
   {
      hr = IncludeMark((CStopJumpMark *)pData, false);
   }
   else
      hr = S_FALSE;

   return hr;
}




HRESULT CMarkStream::CutCopyOrDelete(UINT nFromMs, UINT nLengthMs, 
                                     CClipboard *pClipboard, bool bDoCopy, bool bDoDelete)
{
   HRESULT hr = S_OK;

   if (bDoCopy && !pClipboard)
      return E_POINTER;

   if (nLengthMs == 0)
      return E_INVALIDARG;
  
   if (m_pSelectedMark)
      return E_MS_SELECTION_ACTIVE;


   UINT nElementCount = 0;
   // if there are no elements affected: have a proper stop condition for shifting below
   UINT nStartIndex = (unsigned)m_aAllMarks.GetSize();
   
   for (int i=0; i<m_aAllMarks.GetSize(); ++i)
   {
      UINT nPos = m_aAllMarks[i].GetPosition();
      if (nPos >= nFromMs && nPos < nFromMs+nLengthMs)
      {
         if (nElementCount == 0)
            nStartIndex = i;
         ++nElementCount;
      }
      else if (nPos >= nFromMs+nLengthMs)
      {
         if (nElementCount == 0)
            nStartIndex = i; // no element to be copied/deleted but maybe some to be shifted

         break; // nElementCount is a range of consecutive elements
      }
   }

   // will be used for copying to clipboard AND for undo information
   CStopJumpMark *aAffectedMarks = NULL;
   if (nElementCount > 0)
      aAffectedMarks = new CStopJumpMark[nElementCount];

   UINT nByteCount = nElementCount * sizeof CStopJumpMark;
   
   int iEndIndex = nStartIndex+nElementCount;
   for (i=nStartIndex; i<iEndIndex && SUCCEEDED(hr); ++i)
   {
      UINT nPos = m_aAllMarks[i].GetPosition();
      if (nPos >= nFromMs && nPos < nFromMs+nLengthMs)
      {
         m_aAllMarks[i].CloneTo(&aAffectedMarks[i-nStartIndex]);
         hr = aAffectedMarks[i-nStartIndex].DecrementPosition(nFromMs);
      }
   }
   
   
   if (bDoCopy && SUCCEEDED(hr))
   {
      // make another copy of the marks, this time with a new id (for Paste)
      CStopJumpMark *aCopyMarks = NULL;
      if (nElementCount > 0)
         aCopyMarks = new CStopJumpMark[nElementCount];
      for (i=0; i<(signed)nElementCount && SUCCEEDED(hr); ++i)
         hr = aAffectedMarks[i].CloneTo(&aCopyMarks[i], true);


      UINT nClipboardKey = (UINT)this;
      if (SUCCEEDED(hr))
         hr = pClipboard->AddData((BYTE *)aCopyMarks, nByteCount, nClipboardKey);
      // do this even with aCopyMarks == NULL in order overwrite possible data in the clipboard

      _ASSERT(nElementCount != 0 || nByteCount == 0);

      
      if (aCopyMarks != NULL)
         delete[] aCopyMarks;
   }
   
   
   if (bDoDelete && SUCCEEDED(hr) && nStartIndex < (unsigned)m_aAllMarks.GetSize())
   {
      // shift any remaining element after the ones to be removed
      for (i=nStartIndex+nElementCount; i<m_aAllMarks.GetSize(); ++i)
         m_aAllMarks[i].DecrementPosition(nLengthMs);

      if (nElementCount > 0)
         m_aAllMarks.RemoveAt(nStartIndex, nElementCount);
   }

   if (m_pUndoManager && bDoDelete && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(this, UNDO_REMOVE_RANGE, (BYTE *)aAffectedMarks, nByteCount, 
         HINT_CHANGE_NOTHING, nFromMs, nLengthMs);
   }

   if (aAffectedMarks != NULL)
      delete[] aAffectedMarks;



   return hr;
}

HRESULT CMarkStream::InsertRange(UINT nToMs, UINT nLengthMs, BYTE *pData, UINT nByteCount, bool bUndoWanted)
{
   // this must also be done if there are no elements to insert 
   // as at least the time shifting of other marks must be done

   HRESULT hr = S_OK;


   if (nByteCount > 0 && pData == NULL)
      return E_INVALIDARG;
   
   if (nByteCount % sizeof CStopJumpMark != 0)
      return E_MS_INVALID_DATA;
  
   if (m_pSelectedMark)
      return E_MS_SELECTION_ACTIVE;

 
   
   CStopJumpMark *aMarks = (CStopJumpMark*)pData;
   UINT nElementCount = nByteCount / sizeof CStopJumpMark;


   UINT nInsertPosition = (unsigned)m_aAllMarks.GetSize();
   if (m_aAllMarks.GetSize() > 0)
   {
      // hint: never use an unsigned variable as loop variable:
      // at least when doing "--" with it: it will do an overflow when going below 0
      for (int i=m_aAllMarks.GetSize()-1; i>=0; --i)
      {
         UINT nPos = m_aAllMarks[i].GetPosition();
         
         if (nToMs <= nPos)
            nInsertPosition = i;
         else
            break;
      }
   }
   
   // move the time of marks following after the insert position
   // this has to be done even if there are no marks to be inserted (nByteCount == 0)
   if (nInsertPosition < (unsigned)m_aAllMarks.GetSize())
   {
      for (int i=nInsertPosition; i<m_aAllMarks.GetSize() && SUCCEEDED(hr); ++i)
         hr = m_aAllMarks[i].IncrementPosition(nLengthMs);
   }
  
   for (int i=0; i<(int)nElementCount && SUCCEEDED(hr); ++i)
   {
      hr = aMarks[i].IncrementPosition(nToMs);
      
      if (SUCCEEDED(hr))
      {
         if (i == 0) // make room for nElementCount elements
            m_aAllMarks.InsertAt(nInsertPosition, aMarks[0], nElementCount);
         else
            hr = aMarks[i].CloneTo(&m_aAllMarks[nInsertPosition + i]);
      }
   }

   // register undo action; data can be NULL
   if (bUndoWanted && m_pUndoManager != NULL && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(this, UNDO_INSERT_RANGE, pData, nByteCount, 
         HINT_CHANGE_NOTHING, nToMs, nLengthMs);
   }

   return hr;
}

// private
HRESULT CMarkStream::FindMarksInRegion(CArray<CStopJumpMark *, CStopJumpMark *> &aMarks, 
                                       UINT nFromMs, UINT nLengthMs, UINT *pnStartIndex,
                                       bool bSelectedInOrder)
{
   HRESULT hr = S_FALSE;

   if (nLengthMs == 0)
      return E_INVALIDARG;

   if (pnStartIndex == NULL)
      return E_POINTER;

	*pnStartIndex = 0;
	bool bInsertMovedMark = false;
	bool bStartIndexSet = false;
   for (int i=0; i<m_aAllMarks.GetSize(); ++i)
   {
      UINT nPosMs = m_aAllMarks[i].GetPosition();
      if (nPosMs >= nFromMs && nPosMs < nFromMs+nLengthMs)
      {
		  if (!bStartIndexSet){
			*pnStartIndex = i;
			bStartIndexSet = true;
		  }
			
         if (m_pSelectedMark != &m_aAllMarks[i] || m_bSelectedDisabled)
            aMarks.Add(&m_aAllMarks[i]);
         else
            bInsertMovedMark = true; // should occur at most once

         hr = S_OK;
      }
   }

   if (bInsertMovedMark && m_pSelectedMark)
   {
      // this case will (should) happen only during the moving of a mark

      bool bInserted = false;

      if (bSelectedInOrder)
      {
         // then this moved mark should be at the right temporary/moved position

         for (i=0; i<aMarks.GetSize(); ++i)
         {
            if (m_MovedMark.GetPosition() <= aMarks[i]->GetPosition())
            {
               aMarks.InsertAt(i, &m_MovedMark);
               bInserted = true;
               break;
            }
         }
      }

      if (!bInserted)
         aMarks.Add(&m_MovedMark); // always the case if bSelectedInOrder == false
   }

   return hr;
}

// private
HRESULT CMarkStream::RemoveAndIntegrateMovedElement()
{
   HRESULT hr = S_OK;

   if (!m_pSelectedMark)
      return E_MS_NO_MARK_SELECTED;

   if (&m_aAllMarks[m_nSelectedIndex] != m_pSelectedMark)
      return E_MS_INVALID_DATA;

   UINT nNewPositionMs = m_MovedMark.GetPosition();
   UINT nOldPositionMs = m_pSelectedMark->GetPosition();

   if (nNewPositionMs > nOldPositionMs) // move up
   {
      bool bSwapDone = false;
      if (m_nSelectedIndex < (unsigned)m_aAllMarks.GetSize()-1) // there are elements after the selected
      {
         if (nNewPositionMs > m_aAllMarks[m_nSelectedIndex+1].GetPosition())
         {
            // search for the right position

            UINT nInsertIndex = m_nSelectedIndex+2;
            for (int i=m_nSelectedIndex+2; i<m_aAllMarks.GetSize(); ++i)
            {
               if (nNewPositionMs > m_aAllMarks[i].GetPosition())
                  nInsertIndex = i+1;
            }

            m_aAllMarks.InsertAt(nInsertIndex, m_MovedMark);
            m_aAllMarks.RemoveAt(m_nSelectedIndex);

            bSwapDone = true;
         }
      }
      
      if (!bSwapDone)
         m_MovedMark.CloneTo(m_pSelectedMark); // simply change the position
      
   }
   else if (nNewPositionMs < nOldPositionMs) // move down
   {
      bool bSwapDone = false;
      if (m_nSelectedIndex > 0) // there are elements before the selected
      {
         if (nNewPositionMs < m_aAllMarks[m_nSelectedIndex-1].GetPosition())
         {
            // search for the right position

            UINT nInsertIndex = m_nSelectedIndex-1;
            for (int i=m_nSelectedIndex-2; i>=0; --i)
            {
               if (nNewPositionMs < m_aAllMarks[i].GetPosition())
                  nInsertIndex = i;
            }

            m_aAllMarks.RemoveAt(m_nSelectedIndex);
            m_aAllMarks.InsertAt(nInsertIndex, m_MovedMark);

            bSwapDone = true;
         }
      }

      if (!bSwapDone)
         m_MovedMark.CloneTo(m_pSelectedMark); // simply change the position
   }
   else
   {
      // else no move specified, nothing needs to be done
      hr = S_FALSE;
   }

   return hr;
}


HRESULT CMarkStream::WriteLepEntry(CFileOutput *pOutput)
{
   HRESULT hr = S_OK;
  
   if (m_pSelectedMark)
      return E_MS_SELECTION_ACTIVE;

   _TCHAR tszOutput[100];
   tszOutput[0] = 0;

   _stprintf(tszOutput, _T("    <marks length=\"%d\">\n"), this->m_nStreamLength);
   hr = pOutput->Append(tszOutput);
   
   if (SUCCEEDED(hr))
      hr = WriteObjEntry(pOutput, true);

   _stprintf(tszOutput, _T("    </marks>\n"));
   if (SUCCEEDED(hr))
      hr = pOutput->Append(tszOutput);

   return hr;
}

HRESULT CMarkStream::WriteObjEntry(CFileOutput *pOutput, bool bDoIdent)
{
   HRESULT hr = S_OK;
  
   if (m_pSelectedMark)
      return E_MS_SELECTION_ACTIVE;

   _TCHAR tszOutput[200+MAX_MARK_LABEL_LENGTH];
   tszOutput[0] = 0;
   
   UINT nLastWrittenStop = 0;
   for (int i=0; i<m_aAllMarks.GetSize() && SUCCEEDED(hr); ++i)
   {
      bool bDoWrite = true;
      
      if (bDoIdent)
         pOutput->Append(_T("      "));

      // TODO: CStopJumpMark should write (and read) their data themselves
      if (m_aAllMarks[i].IsJumpMark())
      {
         LPCTSTR tszLabel = m_aAllMarks[i].GetLabel();
         _stprintf(tszOutput, _T("<MARK type=\"%s\" time=\"%d\" label=\"%s\"></MARK>\n"), 
            _T("jump"), m_aAllMarks[i].GetPosition(), tszLabel);

         pOutput->Append(tszOutput);
      }
      else
      {
         _stprintf(tszOutput, _T("<MARK type=\"%s\" time=\"%d\" demoDocumentObject=\"%s\"></MARK>\n"), 
             _T("stop"), m_aAllMarks[i].GetPosition(), m_aAllMarks[i].IsDemoDocumentObject() ? _T("true") : _T("false"));

         pOutput->Append(tszOutput);

         // filter same stop times
         if (i > 0 && nLastWrittenStop == m_aAllMarks[i].GetPosition())
            bDoWrite = false;
         else
            nLastWrittenStop = m_aAllMarks[i].GetPosition();
      }
   }

   return hr;
}
   
HRESULT CMarkStream::ReadLepEntry(SgmlElement *pSgml)
{
   HRESULT hr = S_OK;
  
   if (pSgml == NULL)
      return E_POINTER;

   if (m_pSelectedMark)
      return E_MS_SELECTION_ACTIVE;


   int iStreamLength = pSgml->GetIntValue(_T("length"));
   if (iStreamLength > 0)
      this->m_nStreamLength = iStreamLength;


   CMarkReaderWriter reader;

   CArray<SgmlElement *, SgmlElement *> aElements;
   pSgml->GetElements(aElements);
   for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
       SgmlElement *pSon = aElements[i];
       if (pSon != NULL) {
           if (_tcscmp(pSon->GetName(), _T("MARK")) == 0)
               hr = reader.Parse(pSon);
       }
   }

   UINT nCount = reader.GetCount();
   if (nCount > 0 && SUCCEEDED(hr))
   {
      CStopJumpMark *aCopyMarks = new CStopJumpMark[reader.GetCount()];

      hr = reader.Extract(aCopyMarks);

      if (SUCCEEDED(hr))
      {
         this->InsertRange(0, m_nStreamLength, (BYTE *)aCopyMarks, nCount * sizeof CStopJumpMark, false);
      }

      if (aCopyMarks)
         delete[] aCopyMarks;
   }

   return hr;
}

bool CMarkStream::SelectedStopMarkIsDemoDocumentObject(){
    return m_pSelectedMark->IsStopMark() && m_pSelectedMark->IsDemoDocumentObject();
}

void CMarkStream::RemoveAllEndOfPageStopMarks(){
    // validate required objects
    ASSERT(m_pEditorProject);
    ASSERT(m_pEditorProject->m_pDoc);
    if (m_pEditorProject == NULL || m_pEditorProject->m_pDoc == NULL){
        return;
    }
    // save currently selected mark
    CStopJumpMark *pSelectedMark = m_pSelectedMark;

    for(int i =0; i< m_aAllMarks.GetSize(); i++)
    {
        SelectMark(m_aAllMarks[i].GetPosition(), 1);
        // if we are in screen simultation and the selected mark is demo document object don't allow movement.
        if(m_aAllMarks[i].IsStopMark() && SelectedStopMarkIsDemoDocumentObject()){
            RemoveSelectedMark(false);
            //This can be done because CArray::RemoveAt in the process, it shifts down all the elements above the removed element(s)
            i--;
        }
        ResetMoveSelected();
    }
    // restore selected mark
    m_pSelectedMark = pSelectedMark;
}
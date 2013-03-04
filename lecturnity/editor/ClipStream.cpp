// ClipStream.cpp: Implementierung der Klasse CClipStream.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClipStream.h"
#include "MainFrm.h"
#include "..\Studio\resource.h"
#include "MiscFunctions.h" // fileSdk/VideoInformation
#include "avinfo.h"

#define CAPTION_COLOR RGB(85, 85, 85)
#define CAPTION_SPACING 4

CMergedClip::CMergedClip()
{
   m_aClipParts.RemoveAll();
}

CMergedClip::CMergedClip(const CMergedClip &other)
{
   m_aClipParts.RemoveAll();
   m_aClipParts.Append(other.m_aClipParts);
}
 
CMergedClip& CMergedClip::operator= (const CMergedClip &other)
{
   m_aClipParts.RemoveAll();
   m_aClipParts.Append(other.m_aClipParts);
   return (*this);
}

CMergedClip::~CMergedClip()
{
}

HRESULT CMergedClip::AddSegmentAsPointer(CClipSegment *pSegment)
{
   if (pSegment == NULL)
      return E_POINTER;

   if (!m_bIsInitialized)
   {
      CMediaSegment::Init(pSegment->GetFilename(), pSegment->GetTargetBegin(), pSegment->GetLength());
   }
   else
   {
      CMediaSegment::IncreaseLength(pSegment->GetLength());
   }

   m_aClipParts.Add(pSegment);
 
   return S_OK;
}

int CMergedClip::GetVideoStreamSelector()
{
   if (m_aClipParts.GetSize() > 0)
      return m_aClipParts[0]->GetVideoStreamSelector();
   else
      return -1;
}

// PZI: write clip structuring status to LRD
// appends output to output string
// returns true if some flag has been set; else otherwise
bool CMergedClip::WriteClipStructure(CString &output)
{
   output += _T("    <CLIP>\n");

   bool bStructureAvailable = false;
   bool bImagesAvailable = false;
   bool bFulltextAvailable = false;

   // combine flags of all segments
   for(int pos=0; pos<m_aClipParts.GetCount();pos++) {
      CClipSegment *clipSegment = m_aClipParts.GetAt(pos);
      bStructureAvailable |= clipSegment->GetStructureAvailable();
      bImagesAvailable |= clipSegment->GetImagesAvailable();
      bFulltextAvailable |= clipSegment->GetFulltextAvailable();
   }

   // set flags for LRD
   if(bStructureAvailable) 
      output += _T("      <STRUCTURE>available</STRUCTURE>\n");
   else
      output += _T("      <STRUCTURE></STRUCTURE>\n");

   if(bImagesAvailable) 
      output += _T("      <IMAGES>available</IMAGES>\n");
   else
      output += _T("      <IMAGES></IMAGES>\n");

   if(bFulltextAvailable) 
      output += _T("      <FULLTEXT>available</FULLTEXT>\n");
   else
      output += _T("      <FULLTEXT></FULLTEXT>\n");

   output += _T("    </CLIP>\n");

   // structure must be written if something is available
   return bStructureAvailable || bImagesAvailable || bFulltextAvailable;
}

bool CMergedClip::HasStructuredClips()
{
   bool bStructureAvailable = false;
   //bool bImagesAvailable = false;
   //bool bFulltextAvailable = false;

   // combine flags of all segments
   for(int pos=0; pos<m_aClipParts.GetCount();pos++) {
      CClipSegment *clipSegment = m_aClipParts.GetAt(pos);
      bStructureAvailable |= clipSegment->GetStructureAvailable();
      //bImagesAvailable |= clipSegment->GetImagesAvailable();
      //bFulltextAvailable |= clipSegment->GetFulltextAvailable();
   }
   return bStructureAvailable;
}

CClipStreamR::CClipStreamR()
{
   m_aAllClipSegments.RemoveAll();
   m_aMergedSegments.RemoveAll();

   
   m_nSelectionStart = 0;
   m_nSelectionLength = 0;
   m_iMoveDistanceMs = 0;
   m_nUpperBoundary = 0;
   m_nLowerBoundary = 0;

	m_bmpBackground.LoadBitmap(IDB_VIDEO_STREAM_BG_BLUE);

   FontManipulation::CreateDefaultFont(m_TextFont);

   m_pIcon =NULL;
   m_pImageBackgroundClip = NULL;
   
   m_bIconClip = LoadIconForClip(IDR_TIMELINE_ICON_VIDEO);
}

CClipStreamR::~CClipStreamR()
{

}

HRESULT CClipStreamR::FillClipInfo(UINT nClipNumber, CClipInfo *pClipInfo)
{
   if (nClipNumber < (unsigned)m_aAllClipSegments.GetSize())
      return m_aAllClipSegments[nClipNumber].FillClipInfo(pClipInfo);
   else
      return S_FALSE;
}

HRESULT CClipStreamR::FillClipInfoAt(UINT nPositionMs, CClipInfo *pClipInfo)
{
   // slightly double code below

   HRESULT hr = S_OK;

   if (pClipInfo == NULL)
      return E_POINTER;

   UINT nStartIndex = 0;
   UINT nElementCount = 0;

   hr = FindTotalClipAt(nPositionMs, &nStartIndex, &nElementCount);

   if (SUCCEEDED(hr) && nElementCount > 0)
   {
      hr = FillClipInfo(nStartIndex, pClipInfo);
   }

   return hr;
}

HRESULT CClipStreamR::ChangeClipInfoAt(UINT nPositionMs, CClipInfo *pClipInfo)
{
   // slightly double code above

   HRESULT hr = S_OK;

   if (pClipInfo == NULL)
      return E_POINTER;

   CString csKeywords;
   pClipInfo->GetKeywords(csKeywords);

   UINT nStartIndex = 0;
   UINT nElementCount = 0;

   hr = FindTotalClipAt(nPositionMs, &nStartIndex, &nElementCount);

   if (SUCCEEDED(hr) && nElementCount > 0)
   {
      // title and keywords did maybe change
      // copy them to all relevant clip segments

      CClipSegment source;
      hr = source.Init(pClipInfo);

      int iEndIndex = nStartIndex + nElementCount - 1;
      for (int i=nStartIndex; i<=iEndIndex && SUCCEEDED(hr); ++i)
         hr = source.CloneClipDataTo(&m_aAllClipSegments[i]);
   }

   return hr;
}

HRESULT CClipStreamR::ChangeAllClipInfo(CClipInfo *pClipInfo)
{
   HRESULT hr = S_OK;

   CClipSegment source;
   hr = source.Init(pClipInfo);

   for (int i=0; i<m_aAllClipSegments.GetSize() && SUCCEEDED(hr); ++i)
      hr = source.CloneClipDataTo(&m_aAllClipSegments[i]);

   return hr;
}

bool CClipStreamR::HasAudio()
{
   bool bHasAudio = false;
   
   for (int i=0; i<m_aAllClipSegments.GetSize(); ++i)
   {
      if (m_aAllClipSegments[i].HasAudio())
      {
         bHasAudio = true;
         break;
      }
   }

   return bHasAudio;
}
   
bool CClipStreamR::HasClipAudioAt(UINT nPositionMs)
{
   UINT nStartIndex = 0;
   UINT nElementCount = 0;

   HRESULT hr = FindTotalClipAt(nPositionMs, &nStartIndex, &nElementCount);

   if (SUCCEEDED(hr) && nElementCount > 0)
   {
      if (m_aAllClipSegments[nStartIndex].HasAudio())
         return true;
   }

   return false;
}

LPCTSTR CClipStreamR::GetFilenameAt(UINT nPositionMs)
{
   UINT nStartIndex = 0;
   UINT nElementCount = 0;

   HRESULT hr = FindTotalClipAt(nPositionMs, &nStartIndex, &nElementCount);

   if (SUCCEEDED(hr) && nElementCount > 0)
   {
      return m_aAllClipSegments[nStartIndex].GetFilename();
   }

   return NULL;
}

bool CClipStreamR::BelongsToNextSegment(UINT nClipNumber)
{
   if (nClipNumber + 1 < (unsigned)m_aAllClipSegments.GetSize())
      return m_aAllClipSegments[nClipNumber].BelongsTo(&m_aAllClipSegments[nClipNumber + 1]);
   else
      return false;
}
   

UINT CClipStreamR::GetSourceBegin(UINT nClipNumber)
{
   if (nClipNumber < (unsigned)m_aAllClipSegments.GetSize())
      return m_aAllClipSegments[nClipNumber].GetSourceBegin();
   else
      return 0xffffffff;
}

HRESULT CClipStreamR::GetClipTimesAt(UINT nClickMs, UINT *pnClipStartMs, UINT *pnClipLengthMs)
{
   HRESULT hr = S_OK;

   if (pnClipStartMs == NULL || pnClipLengthMs == NULL)
      return E_POINTER;

   UINT nStartIndex = 0;
   UINT nElementCount = 0;

   hr = FindTotalClipAt(nClickMs, &nStartIndex, &nElementCount);

   if (SUCCEEDED(hr) && nElementCount > 0)
   {
      *pnClipStartMs = m_aAllClipSegments[nStartIndex].GetTargetBegin();

      int iEndIndex = nStartIndex + nElementCount - 1;
      for (int i=nStartIndex; i<=iEndIndex; ++i)
         *pnClipLengthMs += m_aAllClipSegments[i].GetLength();
   }

   return hr;
}

bool CClipStreamR::IsEmpty()
{
   return m_aAllClipSegments.GetSize() == 0;
}

HRESULT CClipStreamR::MergeClipsForExport(CMap<CString, LPCTSTR, CAviSettings *, CAviSettings *> *pMapClipSettings)
{
   m_aMergedSegments.RemoveAll();

   for (int i=0; i<m_aAllClipSegments.GetSize(); ++i)
   {
      CMergedClip mergedClip;
      m_aMergedSegments.Add(mergedClip);

      m_aMergedSegments[m_aMergedSegments.GetSize()-1].AddSegmentAsPointer(&m_aAllClipSegments[i]);

      while (i+1 < m_aAllClipSegments.GetSize() && m_aAllClipSegments[i].BelongsTo(&m_aAllClipSegments[i+1]))
      {
         m_aMergedSegments[m_aMergedSegments.GetSize()-1].AddSegmentAsPointer(&m_aAllClipSegments[i+1]);
         ++i;
      }
   }


   for (int s=m_aMergedSegments.GetSize()-1; s>=0; --s)
   {
      // check if length is enough for input frame rate

      // TODO why use input frame rate here?
      AVInfo avInfo;
      HRESULT hr = avInfo.Initialize(m_aMergedSegments[s].GetFilename());
      if (SUCCEEDED(hr))
      {
         AM_MEDIA_TYPE clipMt;
         hr = avInfo.GetStreamFormat(m_aMergedSegments[s].GetVideoStreamSelector(), &clipMt);
         if (SUCCEEDED(hr))
         {
            if (clipMt.formattype == FORMAT_VideoInfo)
            {
               VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *) clipMt.pbFormat;
               ULONG lTimePerFrame = (ULONG) (pVih->AvgTimePerFrame / 10000i64);
               
               if (m_aMergedSegments[s].GetLength() < lTimePerFrame)
               {
                  m_aMergedSegments.RemoveAt(s);

                  
                  // TODO output error message here; have other check in CEditorProject::ExportClips();
                  /*
                  if (bRemoveClip && m_pExportProgressDialog)
                  {
                  CString csClipPos;
                  Misc::FormatTickString(csClipPos, nStartPosMs, SU_MINUTES, true);
                  CString csMsg;
                  csMsg.Format(IDS_MSG_CLIP_TOO_SHORT, csClipPos, nFileLengthMs);
                  CString csCap;
                  csCap.LoadString(IDS_INFORMATION);
                  
                    ::MessageBox(m_pExportProgressDialog->GetSafeHwnd(), csMsg, csCap, MB_OK | MB_ICONINFORMATION);
                    }
                  */
                  
               }
            }
         }
      }
  }

   return S_OK;
}

UINT CClipStreamR::GetExportClipFileCount()
{
   return m_aMergedSegments.GetSize();
}

HRESULT CClipStreamR::FillExportClipTimes(CArray<UINT, UINT> *paClipTimes)
{
   if (paClipTimes == NULL)
      return E_POINTER;

   if (paClipTimes->GetSize() > 0)
      return E_INVALIDARG;


   for (int i=0; i<m_aMergedSegments.GetSize(); ++i)
   {
      paClipTimes->Add(m_aMergedSegments[i].GetTargetBegin());
   }

   return S_OK;
}


HRESULT CClipStreamR::Draw(CDC *pDC, CRect& rctStreamArea, UINT nFromMs, UINT nLengthMs, CArray<CRect, CRect> *pClipAreas, int yOffset)
{
   HRESULT hr = S_OK;
   for (int i=0; i<m_aAllClipSegments.GetSize(); ++i)
   {
      // TODO? This is another "is this segment affected (visible)?" code block.
      // Here the possible movement (m_iMoveDistanceMs) is and must be accounted for.
      // But maybe we should join it with "FindTotalClipAt()" and "ContainsSegments()"?

      int iSelectionEnd = m_nSelectionStart + m_nSelectionLength - 1;
      bool bIsMovedSegment = i >= (signed)m_nSelectionStart && i <= iSelectionEnd;
      // if there is no selection and thus m_nSelectionLength == 0
      // this variable evaluates to false

      bool bIsSegmentVisible = false;
      if (!bIsMovedSegment)
         bIsSegmentVisible = m_aAllClipSegments[i].IntersectsRange(nFromMs, nLengthMs);
      else
         bIsSegmentVisible = m_aAllClipSegments[i].IntersectsRange(nFromMs, nLengthMs, m_iMoveDistanceMs);
         
      if (bIsSegmentVisible)
      {
         UINT nSegmentStart = m_aAllClipSegments[i].GetTargetBegin();

         if (bIsMovedSegment)
            nSegmentStart += m_iMoveDistanceMs;
         
         UINT nSegmentLength = m_aAllClipSegments[i].GetLength();

         while (i+1 < m_aAllClipSegments.GetSize())
         {
            // if there is a selection and thus maybe a movement
            // regard that movement with respect to the next element if
            // one of them is a moved element
            int iMoveMs = 0;
            if (m_nSelectionLength > 0)
            {
               bool bThisIsLastInSelection = i == iSelectionEnd;
               bool bNextIsFirstInSelection = i+1 == (signed)m_nSelectionStart;
            
               if (bThisIsLastInSelection)
                  iMoveMs = m_iMoveDistanceMs;
               else if (bNextIsFirstInSelection)
                  iMoveMs = -m_iMoveDistanceMs;
            }
            
            if (m_aAllClipSegments[i].BelongsTo(&m_aAllClipSegments[i+1], iMoveMs))
            {
               nSegmentLength += m_aAllClipSegments[i+1].GetLength();
               ++i;
            }
            else
               break; // end of clip found
         }

         bool bCutAtStart = false;
         bool bCutAtEnd = false;

         if (nSegmentStart < nFromMs)
         {
            // clip not fully visible at the beginning

            nSegmentLength -= (nFromMs - nSegmentStart);
            nSegmentStart = nFromMs;
            bCutAtStart = true;
         }

         if (nSegmentStart + nSegmentLength > nFromMs + nLengthMs)
         {
            // clip not fully visible at the end

            nSegmentLength = (nFromMs + nLengthMs) - nSegmentStart;
            bCutAtEnd = true;
         }

         double dStartPartage = (nSegmentStart - nFromMs) / (double)nLengthMs;
         double dLengthPartage = nSegmentLength / (double)nLengthMs;

         int iRectLeft = rctStreamArea.left+(int)(dStartPartage*rctStreamArea.Width());
         // initialize with width 0
         CRect rctSegment(iRectLeft, rctStreamArea.top, iRectLeft, rctStreamArea.bottom);
         rctSegment.right = rctStreamArea.left + (int)((dStartPartage+dLengthPartage) * rctStreamArea.Width());
         // Note: "dStartPartage+dLengthPartage" is more precise than rctSegment.left + dLengthPartage *....
         // as with the latter one you maybe round down two times thus losing a pixel.
         if (rctSegment.right == rctSegment.left)
            rctSegment.right = rctSegment.left+1;

         CRect rcClipArea(rctSegment);
         rcClipArea.OffsetRect(0, -yOffset);
         pClipAreas->Add(rcClipArea);

			// draw rectangle
			CRect rcClip(rctSegment);
			rcClip.DeflateRect(1, 1, 0, 1);
        	COLORREF clrPen = CMainFrameE::GetColor(CMainFrameE::COLOR_STREAM_LINE);
			CPen borderPen(PS_SOLID, 1, clrPen);
			CPen *pPenOld =  pDC->SelectObject(&borderPen);
         if(bCutAtEnd == true)
         {
            rcClip.right += 5;  
         }
         if(bCutAtStart==true)
         {
            rcClip.left -= 4;
         }
         CBrush bmpBrush;
			bmpBrush.CreatePatternBrush(&m_bmpBackground);
			pDC->SetBrushOrg(rcClip.left, rcClip.top);
         if (m_pImageBackgroundClip!=NULL)
         {
          m_pImageBackgroundClip->DrawImage(pDC, rcClip, m_pImageBackgroundClip->GetSource(), CRect(5, 5, 5, 5), COLORREF_NULL);
         }
         if ((m_bIconClip == true)&&(m_pIcon!=NULL))
         { 
            CSize sz(16, 16);
            UINT uiOffsetIcon = 0 ;
            if(( nFromMs < nLengthMs )&&(nFromMs > 0))
            {
              uiOffsetIcon= 4;
            }
            // set drawing point
            CPoint pt(rcClip.left + 10 + uiOffsetIcon ,rcClip.top+2);
            // draw an image (icon)
            m_pIcon->Draw(pDC, pt, m_pIcon->GetIcon(), sz);
         }
			bmpBrush.DeleteObject();

			// draw shadow
			CPen shadowPen(PS_SOLID, 1, RGB(190, 190, 190));
			pDC->SelectObject(&shadowPen);
			pDC->MoveTo(rcClip.left - 2, rcClip.bottom + 2);
			pDC->LineTo(rcClip.right + 2, rcClip.bottom + 2);
			
			pDC->SelectObject(pPenOld);

         // draw text
         if (_tcslen(m_aAllClipSegments[i].GetLabel()) > 0)
         {
            UINT uiOffsetText = 0 ;
            CFont font;
            font.CreateFontIndirect(&m_TextFont);
            CFont *pOldFont = pDC->SelectObject(&font);
            pDC->SetBkMode(TRANSPARENT);

            pDC->SetTextColor(CAPTION_COLOR);

            CRect rctText = rcClip;
            rctText.DeflateRect(CAPTION_SPACING, 0);
            if((m_bIconClip == true)&&(m_pIcon!=NULL))
            {
               if(bCutAtStart==true)
               {
                  uiOffsetText= 4;
               } 
               rctText.MoveToX(rcClip.left + 10 + m_iconHandle.GetExtent().cx+ 5 +uiOffsetText);
            }
          
            if (!rctText.IsRectEmpty())
            {
               pDC->DrawText(m_aAllClipSegments[i].GetLabel(), &rctText, 
                  DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOCLIP);
            }

            // reset values	
            pDC->SelectObject(pOldFont);
            font.DeleteObject();
         }
      }
   }
   return hr;
}

HRESULT CClipStreamR::InsertClips(UINT nToMs, UINT nLengthMs, CClipStream *pOldClipStream, bool bUndoWanted)
{
   // TODO: remove this translation (from old CClipStream) and read them directly

   HRESULT hr = S_OK;

   if (nLengthMs == 0)
      return E_INVALIDARG;

   if (m_nSelectionLength > 0)
      return E_CS_SELECTION_ACTIVE;


   int iClipCount = 0;
   CClipSegment *aClips = NULL;
   if (pOldClipStream != NULL && pOldClipStream->GetSize() > 0)
   {
      iClipCount = pOldClipStream->GetSize();
      aClips = new CClipSegment[iClipCount];

      for (int i=0; i<iClipCount && SUCCEEDED(hr); ++i)
      {
         CClipInfo *pInfo = pOldClipStream->FindClip(i);

         if (pInfo != NULL)
         {
            if (SUCCEEDED(hr))
               hr = aClips[i].Init(pInfo);

            if (SUCCEEDED(hr))
            {
               if (pInfo->GetTimestamp() > 0)
                  hr = aClips[i].IncrementTargetBegin(pInfo->GetTimestamp());
               // BUGFIX #4377 else it will be ignored
            }
         }
         else
            hr = E_INVALIDARG;
      }
   }

   if (SUCCEEDED(hr))
      hr = InsertRange(nToMs, nLengthMs, (BYTE *)aClips, iClipCount * sizeof CClipSegment, bUndoWanted);

   if (SUCCEEDED(hr))
      m_nStreamLength += nLengthMs;

   if (aClips != NULL)
      delete[] aClips;

   return hr;
}

HRESULT CClipStreamR::IncludeClip(UINT nToMs, CClipInfo *pClipInfo, bool bForceOverwrite)
{
   HRESULT hr = S_OK;

   if (m_nSelectionLength > 0)
      return E_CS_SELECTION_ACTIVE;

   
   CClipSegment sgmNewClip;
   if (SUCCEEDED(hr))
      hr = sgmNewClip.Init(pClipInfo);

   int iClipLength = 0;
   if (SUCCEEDED(hr))
      iClipLength = sgmNewClip.GetLength();

   if (SUCCEEDED(hr) && iClipLength <= 0)
      hr = E_CS_CLIP_INFO_FAILED;

   if (nToMs + iClipLength >= m_nStreamLength)
   {
      // clip is too long for stream; only insert begin part

      iClipLength -= (nToMs + iClipLength) - m_nStreamLength;
      sgmNewClip.TrimToRange(sgmNewClip.GetTargetBegin(), iClipLength);
   }
  
   // return an error if there are segments affected and bForceOverwrite is not true
   if (SUCCEEDED(hr) && !bForceOverwrite && ContainsSegments(nToMs, iClipLength))
      hr = E_CS_REPLACE_CLIP;
   
   // delete affected region: must be done even if there are no elements in it
   // otherwise InsertRange (which makes it longer) cannot be used
   if (SUCCEEDED(hr))
      hr = CutCopyOrDelete(nToMs, iClipLength, NULL, false, true);
   
   if (SUCCEEDED(hr))
      hr = InsertRange(nToMs, iClipLength, (BYTE *)&sgmNewClip, sizeof CClipSegment, true);
   
   if (m_pUndoManager != NULL && SUCCEEDED(hr))
      m_pUndoManager->SetRollbackPoint();
   
   return hr;
}

HRESULT CClipStreamR::DeleteClipAt(UINT nPositionMs)
{
   HRESULT hr = S_OK;

   if (m_nSelectionLength > 0)
      return E_CS_SELECTION_ACTIVE;


   UINT nStartIndex = 0;
   UINT nElementCount = 0;

   hr = FindTotalClipAt(nPositionMs, &nStartIndex, &nElementCount);

   UINT nLengthMs = 0;
   if (SUCCEEDED(hr))
   {
      int iNextIndex = nStartIndex+nElementCount;
      for (int i=nStartIndex; i<iNextIndex; ++i)
         nLengthMs += m_aAllClipSegments[i].GetLength();
   }

   if (SUCCEEDED(hr))
   {
      if (nLengthMs > 0)
      {
         UINT nBeginMs = m_aAllClipSegments[nStartIndex].GetTargetBegin();

         // now that we know the length of the total clip
         // delete it (its region) and add an empty region so that the length does not change
         
         if (SUCCEEDED(hr))
            hr = CutCopyOrDelete(nBeginMs, nLengthMs, NULL, false, true);
         
         if (SUCCEEDED(hr))
            hr = InsertRange(nBeginMs, nLengthMs, NULL, 0, true); 
         
         if (m_pUndoManager != NULL && SUCCEEDED(hr))
            m_pUndoManager->SetRollbackPoint();
      }
      else
         hr = S_FALSE;
   }

   return hr;
}


HRESULT CClipStreamR::SelectClipAt(UINT nPositionMs)
{
   HRESULT hr = S_OK;

   if (m_nSelectionLength > 0)
      return E_CS_SELECTION_ACTIVE;


   UINT nStartIndex = 0;
   UINT nElementCount = 0;

   hr = FindTotalClipAt(nPositionMs, &nStartIndex, &nElementCount);

   if (hr == S_OK)
   {
      // one or more clip segments were found
      m_nSelectionStart = nStartIndex;
      m_nSelectionLength = nElementCount;
      m_iMoveDistanceMs = 0;

      UINT nSegmentCount = (unsigned)m_aAllClipSegments.GetSize();
      bool bHasSegmentsBefore = nStartIndex > 0;
      bool bHasSegmentsAfter = nSegmentCount > 1 && nStartIndex+nElementCount < nSegmentCount;


      if (bHasSegmentsBefore)
      {
         CClipSegment *pSgmBefore = &m_aAllClipSegments[nStartIndex-1];
         m_nLowerBoundary = pSgmBefore->GetTargetBegin() + pSgmBefore->GetLength();
      }
      else
         m_nLowerBoundary = 0;

      if (bHasSegmentsAfter)
      {
         CClipSegment *pSgmAfter = &m_aAllClipSegments[nStartIndex+nElementCount];
         m_nUpperBoundary = pSgmAfter->GetTargetBegin();
      }
      else
         m_nUpperBoundary = m_nStreamLength+1;
   }
   
   return hr;
}

HRESULT CClipStreamR::MoveSelectedClip(int iDiffMs)
{
   HRESULT hr = S_OK;

   if (m_nSelectionLength == 0)
      return E_CS_NO_CLIP_SELECTED;


   if (iDiffMs < 0)
   {
      UINT nCurrentClipStart = m_aAllClipSegments[m_nSelectionStart].GetTargetBegin() + m_iMoveDistanceMs;
      int iNewClipStart = nCurrentClipStart + iDiffMs; // could be < 0

      if (iNewClipStart < (signed)m_nLowerBoundary)
         iDiffMs += m_nLowerBoundary - iNewClipStart;
   }
   else if (iDiffMs > 0)
   {
      CClipSegment *pSgmEnd = &m_aAllClipSegments[m_nSelectionStart+m_nSelectionLength-1];
      UINT nCurrentClipEnd = pSgmEnd->GetTargetBegin() + pSgmEnd->GetLength() + m_iMoveDistanceMs;
      UINT nNewClipEnd = nCurrentClipEnd + iDiffMs;

      if (nNewClipEnd > m_nUpperBoundary)
         iDiffMs -= nNewClipEnd - m_nUpperBoundary;
   }

   
   if (iDiffMs != 0)
      m_iMoveDistanceMs += iDiffMs;
   else
      hr = S_FALSE;

   return hr;
}

HRESULT CClipStreamR::CommitMoveSelected(bool bIsUserAction)
{
   HRESULT hr = S_OK;

   if (m_nSelectionLength == 0)
      return E_CS_NO_CLIP_SELECTED;


   // currently the movement ist bounded by the clips lying before and after 
   // these segments; so a simply change of begin times is sufficient,
   // no swaps are needed

   UINT nFromMs = m_aAllClipSegments[m_nSelectionStart].GetTargetBegin();
      
   if (m_iMoveDistanceMs != 0)
   {
      int iNextIndex = m_nSelectionStart+m_nSelectionLength;
      for (int i=m_nSelectionStart; i<iNextIndex; ++i)
      {
         if (m_iMoveDistanceMs > 0)
            hr = m_aAllClipSegments[i].IncrementTargetBegin(m_iMoveDistanceMs);
         else if (m_iMoveDistanceMs < 0)
            hr = m_aAllClipSegments[i].DecrementTargetBegin(-m_iMoveDistanceMs);
      }
      
   }
   
   
   if (m_pUndoManager && SUCCEEDED(hr))
   {
      // Remember the whole movement (of several elements) in one action.
      //
      // An alternative would be to register an action for every segment affected.

      // You cannot just simply remember position (of first segment) and
      // movement distance as you would have to use "FindTotalClipAt()" for
      // the undo then and that cannot
      // tell from the current situation if there was a (visual) join due to "BelongsTo"
      // AFTER the move.

      UINT anValues[2];
      anValues[0] = m_nSelectionStart;
      anValues[1] = m_nSelectionLength;

      hr = m_pUndoManager->RegisterAction(this, UNDO_MOVE_OBJECT, 
         (BYTE *)&anValues, sizeof anValues, HINT_CHANGE_STRUCTURE, nFromMs, (unsigned)m_iMoveDistanceMs);
   }

   // (maybe) this is an action only concerning this stream 
   if (SUCCEEDED(hr) && bIsUserAction)
      hr = m_pUndoManager->SetRollbackPoint();
   
   if (SUCCEEDED(hr) && m_iMoveDistanceMs == 0)
      hr = S_FALSE; // return something informative

   m_nSelectionStart = 0;
   m_nSelectionLength = 0;
   m_iMoveDistanceMs = 0;
   m_nLowerBoundary = 0;
   m_nUpperBoundary = 0;

   return hr;
}

HRESULT CClipStreamR::SpecificUndo(UndoActionId idActionCode, 
                                   BYTE *pData, UINT nDataLength,
                                   UINT nPositionMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;
   
   if (idActionCode == UNDO_MOVE_OBJECT)
   {
      UINT *pnValues = (UINT *)pData;
       
      if (nDataLength != 2 * sizeof UINT)
         hr = E_CS_INVALID_DATA;

      if (pnValues[0] + pnValues[1] > (unsigned)m_aAllClipSegments.GetSize())
         hr = E_CS_INVALID_DATA;
      
      if (SUCCEEDED(hr))
      {
         // TODO this is a major (maybe unwanted) side effect.

         m_nSelectionStart = pnValues[0];
         m_nSelectionLength = pnValues[1];
         m_iMoveDistanceMs = (-1)*(signed)nLengthMs;

         // also takes care of right undo/redo actions
         hr = CommitMoveSelected(false);
      }
   }
   else if (idActionCode == UNDO_INSERT_OBJECT)
   {
      // isn't used; IncludeClip() and DeleteClipAt() use normal range operations (two times)
   }
   else if (idActionCode == UNDO_REMOVE_OBJECT)
   {
      // isn't used; IncludeClip() and DeleteClipAt() use normal range operations (two times)
   }
   else
      hr = S_FALSE;


   return hr;
}


HRESULT CClipStreamR::CutCopyOrDelete(UINT nFromMs, UINT nLengthMs, 
                                      CClipboard *pClipboard, bool bDoCopy, bool bDoDelete)
{
   HRESULT hr = S_OK;

   if (bDoCopy && !pClipboard)
      return E_POINTER;

   if (nLengthMs == 0)
      return E_INVALIDARG;

   if (m_nSelectionLength > 0)
      return E_CS_SELECTION_ACTIVE;

   // find elements affected by this change
   UINT nStartIndex   = 0;
   UINT nElementCount = 0;
   ContainsSegments(nFromMs, nLengthMs, &nStartIndex, &nElementCount);


   // will be used for copying to clipboard AND for undo information
   CClipSegment *aSegments = NULL;
   if (nElementCount > 0)
      aSegments = new CClipSegment[nElementCount];
   
   UINT nByteCount = nElementCount * sizeof CClipSegment; // nElementCount maybe gets adjusted later on

   // compile new elements from the affected region
   int iEndIndex = nStartIndex+nElementCount;
   UINT nCopyIndex = 0;
   for (int i=nStartIndex; i<iEndIndex && SUCCEEDED(hr); ++i)
   {
      bool bContainsStart = m_aAllClipSegments[i].Contains(nFromMs-1);
      bool bContainsEnd   = m_aAllClipSegments[i].Contains(nFromMs+nLengthMs);

      CClipSegment sgmTwo;
      if (nFromMs > 0 && bContainsStart)
      {
         // this element has parts not affected at the beginning
         
         if (bDoDelete)
            hr = m_aAllClipSegments[i].Split(nFromMs, &aSegments[nCopyIndex]);
         else
         {
            m_aAllClipSegments[i].CloneTo(&sgmTwo);
            sgmTwo.Split(nFromMs, &aSegments[nCopyIndex]);
         }

         // do not delete it later
         ++nStartIndex;
         --nElementCount;

         if (nElementCount == 0 && bContainsEnd && SUCCEEDED(hr))
         {
            // and maybe it has also parts not affected at the end
            
            hr = aSegments[nCopyIndex].Split(nFromMs+nLengthMs, &sgmTwo);
           
            if (SUCCEEDED(hr) && bDoDelete)
            {
               // additionally remember the end part
               m_aAllClipSegments.InsertAt(i+1, sgmTwo);
            }
            
            
            // no join check needed here as it is one segment that was split in three parts
            // the remaining start and end parts (after delete) do surely not join


            // the loop should be finished here (only one element affected)
            // make sure it really stops after this
            i = iEndIndex;
         }
      }
      else if (bContainsEnd)
      {
         // this element has parts not affected at the end

         if (bDoDelete)
            hr = m_aAllClipSegments[i].Split(nFromMs+nLengthMs, &sgmTwo);
         else
         {
            m_aAllClipSegments[i].CloneTo(&aSegments[nCopyIndex]);

            // copy the begin part
            aSegments[nCopyIndex].TrimToRange(nFromMs, nLengthMs);
         }
         
         if (SUCCEEDED(hr) && bDoDelete)
         {
            m_aAllClipSegments[i].CloneTo(&aSegments[nCopyIndex]);

            // remember the end part
            m_aAllClipSegments.SetAt(i, sgmTwo);
         
            // do not delete it later on
            if (nElementCount > 0)
               --nElementCount;
         }
      }
      else
      {
         // element lies fully within range (and maybe will be deleted later on)
         hr = m_aAllClipSegments[i].CloneTo(&aSegments[nCopyIndex]);

         // make the segment shorter (maybe)
         aSegments[nCopyIndex].TrimToRange(nFromMs, nLengthMs); 
      }

      if (SUCCEEDED(hr))
         hr = aSegments[nCopyIndex].DecrementTargetBegin(nFromMs);

      ++nCopyIndex;
   }

   if (bDoCopy && SUCCEEDED(hr))
   {
      UINT nClipboardKey = (UINT)this;
      hr = pClipboard->AddData((BYTE *)aSegments, nByteCount, nClipboardKey);
      // Note: aSegments can be NULL if none are affected.
      // However you have to store 0 bytes in that case in order to reset
      // any old data in the clipboard.
      _ASSERT(aSegments != NULL || nByteCount == 0);
   }
   
   
   if (bDoDelete && SUCCEEDED(hr))
   {
      if (nElementCount > 0) // still something to delete?
         m_aAllClipSegments.RemoveAt(nStartIndex, nElementCount);

      // move the segments after the removed range
      for (i=nStartIndex; i < m_aAllClipSegments.GetSize() && SUCCEEDED(hr); ++i)
         hr = m_aAllClipSegments[i].DecrementTargetBegin(nLengthMs);
      
      
      if (SUCCEEDED(hr))
         hr = JoinSegments();
   }
   

   if (m_pUndoManager && bDoDelete && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(this, UNDO_REMOVE_RANGE, (BYTE *)aSegments, nByteCount, 
         HINT_CHANGE_STRUCTURE, nFromMs, nLengthMs);
   }


   if (aSegments != NULL)
      delete[] aSegments;


   return hr;
}


HRESULT CClipStreamR::InsertRange(UINT nToMs, UINT nLengthMs, BYTE *pData, UINT nByteCount, bool bUndoWanted)
{
   // this must also be done if there are no elements to insert 
   // as at least the time shifting of other marks must be done

   HRESULT hr = S_OK;


   if (nByteCount > 0 && pData == NULL || nLengthMs == 0)
      return E_INVALIDARG;

   if (m_nSelectionLength > 0)
      return E_CS_SELECTION_ACTIVE;

   if (nByteCount % sizeof CClipSegment != 0)
      return E_CS_INVALID_DATA;
 

   CClipSegment *aSegments = (CClipSegment*)pData;
   UINT nElementCount = nByteCount / sizeof CClipSegment;

   
   UINT nInsertIndex = FindInsertIndex(nToMs);
   if (nInsertIndex < (unsigned)m_aAllClipSegments.GetSize() && m_aAllClipSegments[nInsertIndex].Contains(nToMs))
   {
      if (m_aAllClipSegments[nInsertIndex].GetTargetBegin() != nToMs)
      {
         // an existing segment is split in two parts
         // shorten the first part and add the second part 
         // as a new segment and insert the elements before that

         CClipSegment sgmTwo;
         hr = m_aAllClipSegments[nInsertIndex].Split(nToMs, &sgmTwo);
      
         ++nInsertIndex;

         m_aAllClipSegments.InsertAt(nInsertIndex, sgmTwo);
      }
      // else: everything is alright; the found segment begins directly after the one to be inserted
   }


   // move the time of segments following after the insert position
   // this has to be done even if there are no segments to be inserted (nByteCount == 0)
   if (nInsertIndex < (unsigned)m_aAllClipSegments.GetSize() && SUCCEEDED(hr))
   {
      for (int i=nInsertIndex; i<m_aAllClipSegments.GetSize() && SUCCEEDED(hr); ++i)
         hr = m_aAllClipSegments[i].IncrementTargetBegin(nLengthMs);
   }


   for (int i=0; i<(int)nElementCount && SUCCEEDED(hr); ++i)
   {
      hr = aSegments[i].IncrementTargetBegin(nToMs);
      
      if (SUCCEEDED(hr))
      {
         if (i == 0) // make room for nElementCount elements
            m_aAllClipSegments.InsertAt(nInsertIndex, aSegments[0], nElementCount);
         else
            hr = aSegments[i].CloneTo(&m_aAllClipSegments[nInsertIndex + i]);
      }
   }

   if (SUCCEEDED(hr))
      hr = JoinSegments();

   // register undo action; data can be NULL
   if (bUndoWanted && m_pUndoManager != NULL && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(this, UNDO_INSERT_RANGE, pData, nByteCount,
         HINT_CHANGE_STRUCTURE, nToMs, nLengthMs);
   }


   return hr;
}


// private
HRESULT CClipStreamR::FindTotalClipAt(UINT nPositionMs, UINT *pnStartIndex, UINT *pnElementCount)
{
   HRESULT hr = S_OK;

   if (pnStartIndex == NULL || pnElementCount == NULL)
      return E_POINTER;

   
   UINT nStartIndex = FindInsertIndex(nPositionMs);
   UINT nElementCount = 0;

   if (nStartIndex < (unsigned)m_aAllClipSegments.GetSize() && m_aAllClipSegments[nStartIndex].Contains(nPositionMs))
   {
      nElementCount = 1;
      
      UINT nSegmentCount = (unsigned)m_aAllClipSegments.GetSize();
      bool bHasSegmentsBefore = nStartIndex > 0;
      bool bHasSegmentsAfter = nSegmentCount > 1 && nStartIndex < nSegmentCount-1;

      UINT nClickIndex = nStartIndex;
      
      // look for clip segments belonging to this one before
      if (bHasSegmentsBefore)
      {
         for (int i=nClickIndex-1; i>=0; --i)
         {
            if (m_aAllClipSegments[i].BelongsTo(&m_aAllClipSegments[i+1]))
            {
               --nStartIndex;
               ++nElementCount;
            }
            else
               break; // if there is a gap
         }
      }

      if (bHasSegmentsAfter)
      {
         for (int i=nClickIndex+1; i<(signed)nSegmentCount; ++i)
         {
            if (m_aAllClipSegments[i-1].BelongsTo(&m_aAllClipSegments[i]))
            {
               ++nElementCount;
            }
            else
               break; // if there is a gap
         }
      }

      _ASSERT(nStartIndex+nElementCount-1 < (unsigned)m_aAllClipSegments.GetSize());


      *pnStartIndex = nStartIndex;
      *pnElementCount = nElementCount;
   }
   else
      hr = S_FALSE;

   return hr;
}
  
// private
UINT CClipStreamR::FindInsertIndex(UINT nPositionMs)
{
   UINT nInsertIndex = 0;

   for (int i=0; i<m_aAllClipSegments.GetSize(); ++i)
   {
      if (m_aAllClipSegments[i].Contains(nPositionMs) || m_aAllClipSegments[i].BeginsAtOrAfter(nPositionMs))
         break;
      
      ++nInsertIndex;
   }
   
   return nInsertIndex;
}

// private
bool CClipStreamR::ContainsSegments(UINT nFromMs, UINT nLengthMs, UINT *pnStartIndex, UINT *pnElementCount)
{
   UINT nStartIndex   = FindInsertIndex(nFromMs);
   UINT nElementCount = 0;

   if (nLengthMs > 0)
   {
      for (int i=nStartIndex; i<m_aAllClipSegments.GetSize(); ++i)
      {
         if (m_aAllClipSegments[i].IntersectsRange(nFromMs, nLengthMs))
         {
            ++nElementCount;
         }
         else if (m_aAllClipSegments[i].BeginsAtOrAfter(nFromMs + nLengthMs))
         {
            break; // nElementCount is a range of consecutive elements
         }
      }

      if (pnStartIndex != NULL)
         *pnStartIndex = nStartIndex;
      
      if (pnElementCount != NULL)
         *pnElementCount = nElementCount;
   }

   return nElementCount > 0;
}

// private
HRESULT CClipStreamR::JoinSegments(UINT nStartIndex, UINT nRangeLength)
{
   HRESULT hr = S_OK;

   if (nRangeLength == 0)
   {
      nStartIndex = 0;
      nRangeLength = m_aAllClipSegments.GetSize();
   }

   if (nStartIndex+nRangeLength > (unsigned)m_aAllClipSegments.GetSize())
      return E_INVALIDARG;


   for (int i=nStartIndex+nRangeLength-2; i>=(signed)nStartIndex && SUCCEEDED(hr); --i)
   {
      if (m_aAllClipSegments[i].JoinsWith(&m_aAllClipSegments[i+1]))
      {
         hr = m_aAllClipSegments[i].Join(&m_aAllClipSegments[i+1]);
         m_aAllClipSegments.RemoveAt(i+1);
      }
   }

   return hr;
}


HRESULT CClipStreamR::WriteLrdEntry(CFileOutput *pOutput, CString &csPrefix, bool bIsDenver)
{
   HRESULT hr = S_OK;

   if (m_nSelectionLength > 0)
      return E_CS_SELECTION_ACTIVE;

   if (pOutput == NULL)
      return E_POINTER;

   _TCHAR tszOutput[200 + MAX_PATH];
   ZeroMemory(tszOutput, 200 + MAX_PATH);

   ULONG nWritten = 0;

   if (m_aMergedSegments.GetSize() > 0)
   {
      _stprintf(tszOutput, _T("        <OPT>-multi</OPT>\n"));
      hr = pOutput->Append(tszOutput);
      
      for (int i=0; i<m_aMergedSegments.GetSize() && SUCCEEDED(hr); ++i)
      {
         _TCHAR *tszEntry = tszOutput;

         _TCHAR tszOutputFilename[MAX_PATH];
         if (!bIsDenver)
            _stprintf(tszOutputFilename, _T("%s_clip_%d.avi"), csPrefix, m_aMergedSegments[i].GetTargetBegin());
         else
            _stprintf(tszOutputFilename, _T("%s.avi"), csPrefix); 
         // match the name of the exported denver clip: it is exported "as" the video

         _stprintf(tszEntry, _T("        <OPT>%s</OPT>\n"), tszOutputFilename); 
         tszEntry += _tcslen(tszEntry);
         _stprintf(tszEntry, _T("        <OPT>%d</OPT>\n"), m_aMergedSegments[i].GetTargetBegin());
      

         hr = pOutput->Append(tszOutput);

         if (bIsDenver)
            break; // a denver document contains only one clip

      }
   }

   return hr;
}



HRESULT CClipStreamR::WriteLepEntry(CFileOutput *pOutput, CString &csLepFileName)
{
   HRESULT hr = S_OK;
  
   if (m_nSelectionLength > 0)
      return E_CS_SELECTION_ACTIVE;

   if (pOutput == NULL)
      return E_POINTER;


   ULONG nWritten = 0;
   _TCHAR tszOutput[200+MAX_MARK_LABEL_LENGTH];
   
   _stprintf(tszOutput, _T("    <clips length=\"%d\">\n"), this->m_nStreamLength);
   hr = pOutput->Append(tszOutput);

   for (int i=0; i<m_aAllClipSegments.GetSize() && SUCCEEDED(hr); ++i)
   {
      _stprintf(tszOutput, _T("      "));
      hr = m_aAllClipSegments[i].AppendLepTextData(tszOutput+_tcslen(tszOutput), csLepFileName);
      _tcscat(tszOutput, _T("\n"));
      
      hr = pOutput->Append(tszOutput);
   }

   _stprintf(tszOutput, _T("    </clips>\n"));
   if (SUCCEEDED(hr))
   {
      hr = pOutput->Append(tszOutput);
   }

   return hr;
}

HRESULT CClipStreamR::ReadLepEntry(SgmlElement *pSgml, LPCTSTR tszLepFileName) {
    HRESULT hr = S_OK;

    if (pSgml == NULL)
        return E_POINTER;

    if (m_nSelectionLength > 0)
        return E_CS_SELECTION_ACTIVE;

    int iStreamLength = pSgml->GetIntValue(_T("length"));
    if (iStreamLength > 0)
        this->m_nStreamLength = iStreamLength;

    CArray<CClipSegment, CClipSegment> aClips;	 
    CArray<SgmlElement *, SgmlElement *> aElements;
    if (SUCCEEDED(hr)) {
        pSgml->GetElements(aElements);	  
        aClips.SetSize(aElements.GetCount(), 0);
        for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement != NULL) {
                if (_tcscmp(pElement->GetName(), _T("CLIP")) == 0)
                    hr = aClips[i].ReadFromSgml(pElement, tszLepFileName);
            }
        }
    }

    if (SUCCEEDED(hr))
        hr = InsertRange(0, m_nStreamLength, (BYTE*)aClips.GetData(), aClips.GetSize() * sizeof CClipSegment, false);

    return hr;
}

// PZI: write clip structuring status
HRESULT CClipStreamR::WriteClipStructure(CFileOutput *pOutput)
{
   HRESULT hr = S_OK;

   CString line = _T("  <CLIPSTRUCTURE>\n");

   bool bWriteClipStructures = false;
   for (int i=0; i<m_aMergedSegments.GetSize(); ++i)      
      bWriteClipStructures |= m_aMergedSegments[i].WriteClipStructure(line);

   line += _T("  </CLIPSTRUCTURE>\n");

   // structure must be written if something is available
   if(bWriteClipStructures)
      hr = pOutput->Append(line);

   return hr;
}

// PZI: check if there are structured clips
bool CClipStreamR::HasStructuredClips() {
    for (int i=0; i<m_aAllClipSegments.GetSize(); ++i) {
        if (m_aAllClipSegments[i].GetStructureAvailable()) {
            return true;
        }
    }
    return false;
}

bool CClipStreamR::LoadIconForClip(UINT uiID)
{
   bool bRetValue=false;
   // icon size
   CSize sz(16, 16);
   // load icon
   m_iconHandle = m_pIcon->LoadBitmapFromResource(MAKEINTRESOURCE(uiID /*IDR_TIMELINE_ICON_VIDEO*/), NULL);
   if(m_iconHandle!=NULL)
   {    
      m_pIcon = new CXTPImageManagerIcon(0, m_iconHandle.GetExtent().cx, m_iconHandle.GetExtent().cy);
      m_pIcon->SetNormalIcon(m_iconHandle);
      if(m_pIcon!=NULL)
      {
         bRetValue=true;
      }
   }
   return bRetValue;
}
CXTPImageManagerIcon* CClipStreamR::GetIconForClip()
{
   return m_pIcon;
}
void CClipStreamR::SetBackgroundImg(CXTPOffice2007Image* pImageBackgroundClip )
{
   m_pImageBackgroundClip = pImageBackgroundClip;
}

CMergedClip *CClipStreamR::GetClipToExport() {
    CMergedClip *pReturnClip = NULL;

    if (HasStructuredClips() && m_aMergedSegments.GetCount() == 1) {
        pReturnClip = &m_aMergedSegments[0];

    }

    return pReturnClip;
}
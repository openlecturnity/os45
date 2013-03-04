#include "stdafx.h"
#include "DynamicObject.h"
#include "editorDoc.h"
#include "InteractionStream.h"

IMPLEMENT_DYNCREATE(CDynamicObject, CInteractionAreaEx)

CDynamicObject::CDynamicObject()
{
   m_idType = DYNAMIC_NONE;
   m_bHasToBeChecked = false;
   m_bIsChecked = false;
   m_pEditorProject = NULL;
   m_pWbView = NULL;
   m_pEdit = NULL;
}

CDynamicObject::~CDynamicObject()
{
   if (m_pEdit != NULL)
   {
      if (m_pWbView != NULL)
         m_pWbView->RemoveTextField(m_pEdit);

      delete m_pEdit;
   }
}

HRESULT CDynamicObject::Init(CRect *prcSize, CTimePeriod *pVisibility, 
                             CTimePeriod *pActivity, CEditorProject *pProject, 
                             DynamicTypeId idType, bool bHasToBeChecked,
                             CArray<CString, CString> *paCorrectTexts)
{
   if (prcSize == NULL || pVisibility == NULL)
      return E_POINTER;
   
   if (idType == DYNAMIC_NONE)
      return E_IAX_INVALID_DATA;

   // only one of the two answer parameters has to be set
   if (idType == DYNAMIC_TEXTFIELD)
   {
      if (paCorrectTexts == NULL || paCorrectTexts->GetSize() == 0)
         return E_IAX_INVALID_DATA;
   }
   else
   {
      if (paCorrectTexts != NULL && paCorrectTexts->GetSize() > 0)
         return E_IAX_INVALID_DATA;
   }
 
   m_idType = idType;
   m_bHasToBeChecked = bHasToBeChecked;
   if (paCorrectTexts != NULL)
      m_aCorrectTexts.Append(*paCorrectTexts);

   if (idType == DYNAMIC_TEXTFIELD)
   {
      // native component will be added later
   }
   
   m_pEditorProject = pProject;
   
   HRESULT hr = CInteractionAreaEx::Init(prcSize, false,
      NULL, NULL, NULL, NULL, NULL, pVisibility, pActivity, pProject);

   CreateNameAndType();

   return hr;
}

HRESULT CDynamicObject::Init(CInteractionArea *pInteraction, CEditorProject *pProject)
{
   if (pInteraction == NULL || pProject == NULL)
      return E_POINTER;

   DynamicTypeId idType = DYNAMIC_NONE;
   if (pInteraction->IsCheckBox())
      idType = DYNAMIC_CHECKBOX;
   else if (pInteraction->IsRadioButton())
      idType = DYNAMIC_RADIOBUTTON;
   else if (pInteraction->IsTextField())
      idType = DYNAMIC_TEXTFIELD;
   else
      return E_IAX_INVALID_DATA;

   CTimePeriod periodVisible;
   HRESULT hr = periodVisible.Parse(pInteraction->GetVisibility());

   CTimePeriod periodActive;
   if (SUCCEEDED(hr))
      hr = periodActive.Parse(pInteraction->GetActivity());

   CRect rcSize = pInteraction->GetActivityArea();

   if (idType == DYNAMIC_TEXTFIELD)
   {
      // native component will be added later
   }
   
   m_pEditorProject = pProject;   

   CArray<CString, CString> aCorrectTexts;
   if (SUCCEEDED(hr))
      hr = pInteraction->GetCorrectTexts(&aCorrectTexts);

   if (SUCCEEDED(hr))
   {
      hr = Init(&rcSize, &periodVisible, &periodActive,
         pProject, idType, pInteraction->HasToBeChecked(), &aCorrectTexts);
   }

   return hr;
}

HRESULT CDynamicObject::Move(float fDiffX, float fDiffY)
{
   int iPosX = m_rcDimensions.left;
   int iPosY = m_rcDimensions.top;
   
   HRESULT hr = CInteractionAreaEx::Move(fDiffX, fDiffY);

   if (m_pEdit != NULL)
   {
      int iDiffX = m_rcDimensions.left - iPosX;
      int iDiffY = m_rcDimensions.top - iPosY;

      if (m_pWbView != NULL)
         m_pWbView->MoveTextField(m_pEdit, iDiffX, iDiffY);
   }

   return hr;
}

InteractionClassId CDynamicObject::GetClassId()
{
   return INTERACTION_CLASS_DYNAMIC;
}

HRESULT CDynamicObject::AddNativeToView()
{

   if (m_pEditorProject != NULL && m_pEdit == NULL)
   {
      CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

      if (pDoc != NULL) {
          m_pWbView = pDoc->GetWhiteboardView();
          if (m_pWbView != NULL)
            m_pEdit = m_pWbView->CreateTextField(GetArea());
          if (m_pEdit != NULL)
          {
              m_pEdit->EnableWindow(FALSE);

              CString csFirstCorrect;
              if (m_aCorrectTexts.GetSize() > 0)
                  csFirstCorrect = m_aCorrectTexts[0];
              m_pEdit->SetWindowText((LPCTSTR)csFirstCorrect); // correct text for display during not-preview

              if (m_Visibility.Contains(pDoc->m_curPosMs))
                  m_pEdit->ShowWindow(SW_SHOW);
          }
          return S_OK;
      }
   }

   return S_FALSE;
}

bool CDynamicObject::ShowHide(UINT nPositionMs, bool bPreviewing, bool bForceHide)
{
   bool bSomeChange = false;

   if (m_pEdit != NULL)
   {
      bool bIsVisible = m_pEdit->IsWindowVisible() == TRUE;
      bool bShouldBeVisible = m_Visibility.Contains(nPositionMs);
      if (bShouldBeVisible)
      {
         if (bForceHide)
            bShouldBeVisible = false;
         else if (m_pWbView != NULL && m_pWbView->IsFeedbackVisible())
            bShouldBeVisible = false; // not show it if there is feedback
      }
      
      if (bIsVisible != bShouldBeVisible)
      {
         m_pEdit->ShowWindow(bShouldBeVisible ? SW_SHOW : SW_HIDE);
         
         bSomeChange = true;
      }

      if (bShouldBeVisible)
      {
         bool bShouldBeEnabled = bPreviewing && !m_bDisabledForPreview;
         BOOL bEnabledC = bShouldBeEnabled ? TRUE : FALSE;
         
         if (m_pEdit->IsWindowEnabled() != bEnabledC)
         {
            m_pEdit->EnableWindow(bEnabledC);
            
            bSomeChange = true;

            if (bPreviewing)
            {
               if (bEnabledC)
                  m_pEdit->SetWindowText(_T(""));
            }
            else
            {
               CString csFirstCorrect;
               if (m_aCorrectTexts.GetSize() > 0)
                  csFirstCorrect = m_aCorrectTexts[0];
               m_pEdit->SetWindowText((LPCTSTR)csFirstCorrect);
            }
         }
      }
   }

   return bSomeChange;
}
   
bool CDynamicObject::Reset(bool bAfterPreview)
{
   bool bChange = CInteractionAreaEx::Reset(bAfterPreview);

   if (m_idType == DYNAMIC_RADIOBUTTON || m_idType == DYNAMIC_CHECKBOX)
   {
      bool bWasActive = m_bIsChecked;
      m_bIsChecked = false;
      if (bWasActive)
         bChange = true;
   }
   else if (m_idType == DYNAMIC_TEXTFIELD)
   {
      if (m_pEdit != NULL)
      {
         CString csFirstCorrect;
         if (bAfterPreview)
         {
            CEditorDoc *pDoc = m_pEditorProject != NULL ? m_pEditorProject->m_pDoc : NULL;
            if (pDoc != NULL)
               ShowHide(pDoc->m_curPosMs, false); // disable window
            else
               _ASSERT(false);

            if (m_aCorrectTexts.GetSize() > 0)
               csFirstCorrect = m_aCorrectTexts[0];
            m_pEdit->SetWindowText((LPCTSTR)csFirstCorrect); // restore correct text for display during not-preview
         }
         else
         {
            m_pEdit->SetWindowText(_T(""));
         }
         bChange = true;
      }
   }
   
   return bChange;
}

bool CDynamicObject::ResetRadio()
{
   if (m_idType == DYNAMIC_RADIOBUTTON)
      return Reset(false);
   else
      return false;
}  



bool CDynamicObject::IsAnswerCorrect()
{
   bool bAnswer = false;
   
   if (m_idType == DYNAMIC_RADIOBUTTON || m_idType == DYNAMIC_CHECKBOX)
   {
      bAnswer = m_bHasToBeChecked == m_bIsChecked;
   }
   else if (m_idType == DYNAMIC_TEXTFIELD)
   {
      if (m_pEdit != NULL)
      {
         CString csText;
         m_pEdit->GetWindowText(csText);

         bAnswer = false;
         for (int i=0; i<m_aCorrectTexts.GetSize(); ++i)
         {
            if (m_aCorrectTexts[i].Compare(csText) == 0)
               bAnswer = true;
         }
      }
   }
 
   return bAnswer;
}

   
bool CDynamicObject::InformMouseStatus(bool bMouseOver, bool bMousePressed)
{
   bool bChange = false;
   
   if (bMouseOver && !bMousePressed && m_bMousePressed)
   {
      // mouse is released after it was clicked
      m_bIsChecked = !m_bIsChecked;
      
      bChange = true;

      if (GetType() == DYNAMIC_RADIOBUTTON)
      {
         CInteractionStream *pInteractions = m_pEditorProject->GetInteractionStream(false);
         if (pInteractions != NULL)
            pInteractions->ResetOtherRadioButtons(this);
      }
   }
   
   CInteractionAreaEx::InformMouseStatus(bMouseOver, bMousePressed); // set variables correctly

   return bChange;
}


CInteractionAreaEx* CDynamicObject::MakeNew()
{
   return new CDynamicObject();
}

// protected
HRESULT CDynamicObject::CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey)
{
   HRESULT hr = S_OK;

   hr = CInteractionAreaEx::CloneTo(pTarget, bCopyHashKey);

   if (SUCCEEDED(hr))
   {
      if (pTarget->GetClassId() != GetClassId())
         return E_INVALIDARG;

      CDynamicObject *pOther = (CDynamicObject *)pTarget;

      pOther->m_idType = m_idType;
      pOther->m_bHasToBeChecked = m_bHasToBeChecked;
      pOther->m_aCorrectTexts.Append(m_aCorrectTexts);

      pOther->m_pEditorProject = m_pEditorProject;
      
      pOther->m_pWbView = NULL;
      pOther->m_pEdit = NULL;

      // Note: m_pEdit is not copied: each text field has its own window;
      // the new window will be created during first call of Draw().
   }
   
   return hr;
}

void CDynamicObject::Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing)
{
   if (!bPreviewing && m_bHiddenDuringEdit || bPreviewing && m_bHiddenDuringPreview)
      return;

   CRect rcObjectArea = GetArea();
   CRect rcPaintArea = rcObjectArea;
   if (pZoomManager != NULL /*&& pZoomManager->GetZoomFactor() != 1.0*/)
   {
      double dZoom = pZoomManager->GetZoomFactor();

      CWhiteboardView::ScaleAndOffsetRect(rcPaintArea, ptOffsets, dZoom);
   }
   
   bool bShouldBeEnabled = bPreviewing && !m_bDisabledForPreview;
        
   int iCheckState = m_bIsChecked ? DFCS_CHECKED : 0;
   int iEnableState = bShouldBeEnabled ? 0 : DFCS_INACTIVE;

   if (m_idType == DYNAMIC_RADIOBUTTON)
   {
      pDC->DrawFrameControl(rcPaintArea, DFC_BUTTON, DFCS_BUTTONRADIO | iCheckState | iEnableState);
   }
   else if (m_idType == DYNAMIC_CHECKBOX)
   {
      pDC->DrawFrameControl(rcPaintArea, DFC_BUTTON, DFCS_BUTTONCHECK | iCheckState | iEnableState);
   }
   else if (m_idType == DYNAMIC_TEXTFIELD)
   {
      // content is drawn as a native component

      CEditorDoc *pDoc = m_pEditorProject != NULL ? m_pEditorProject->m_pDoc : NULL;
      if (pDoc != NULL && m_pEdit == NULL)
         AddNativeToView();
     
      CPen penDarkRed;
      penDarkRed.CreatePen(PS_SOLID, 1, RGB(32, 0, 0));
      CPen *pOldPen = pDC->SelectObject(&penDarkRed);

#ifdef _DEBUG
      // make it instantly visible if the native window is missing or misplaced
      
      CBrush brshLightGray;
      brshLightGray.CreateSolidBrush(RGB(230, 230, 230));
      CBrush *pOldBrush = pDC->SelectObject(&brshLightGray);
#endif

      //pDC->Draw3dRect(rcPaintArea, RGB(0,0,0), RGB(0,0,0)); // visible on thumbnails
      pDC->Rectangle(rcPaintArea);

#ifdef _DEBUG
      if (pOldBrush != NULL)
         pDC->SelectObject(pOldBrush);
#endif

      if (pOldPen != NULL)
         pDC->SelectObject(pOldPen);
 
      if (m_pEdit != NULL)
      {
         CFont *pOldFont = pDC->SelectObject(((CExtEdit*)m_pEdit)->GetExtFont());

         CRect rcText = rcPaintArea;
         rcText.DeflateRect(5, 2); // arbitrary magic numbers that CEdit seems to be using as border

         CString csText;
         m_pEdit->GetWindowText(csText);

         int nMode = pDC->SetBkMode(TRANSPARENT); // do not paint text background

         pDC->DrawText(csText, csText.GetLength(), rcText, 0);

         pDC->SetBkMode(nMode);

         if (pOldFont != NULL)
            pDC->SelectObject(pOldFont);
      }

  }

   
   if (!bPreviewing && IsSelected())
   {
      DrawBorder(pDC, ptOffsets, pZoomManager);
   }
}

void CDynamicObject::DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager)
{
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   graphics.TranslateTransform((float)ptOffsets->x, (float)ptOffsets->y);
   graphics.ScaleTransform((float)pZoomManager->GetZoomFactor(), (float)pZoomManager->GetZoomFactor());

   Gdiplus::Color clrWhite(255, 255, 255, 255);
   Gdiplus::Color clrBlack(255, 0, 0, 0);
   Gdiplus::RectF rcOut((Gdiplus::REAL)m_rcDimensions.left, (Gdiplus::REAL)m_rcDimensions.top, 
      (Gdiplus::REAL)m_rcDimensions.Width(), (Gdiplus::REAL)m_rcDimensions.Height());
   
   Gdiplus::REAL fBoundingWidth = (Gdiplus::REAL)(6.0 / pZoomManager->GetZoomFactor());
   Gdiplus::HatchBrush brush(Gdiplus::HatchStyleLightUpwardDiagonal, clrBlack, clrWhite);
   Gdiplus::Pen selectionRectPen(&brush, fBoundingWidth);
   rcOut.Inflate((Gdiplus::REAL)fBoundingWidth / 2, (Gdiplus::REAL)fBoundingWidth / 2);
   graphics.DrawRectangle(&selectionRectPen, rcOut);
   
   
   /*/
   Gdiplus::Pen blackPen(clrBlack, 1.0f);
   Gdiplus::SolidBrush whiteBrush(clrWhite);
   
     CPoint ptSelected[8];
     ptSelected[0] = CPoint((int)rcOut.X, (int)rcOut.Y);
     ptSelected[1] = CPoint((int)(rcOut.X + rcOut.Width / 2), (int)rcOut.Y);
     ptSelected[2] = CPoint((int)rcOut.GetRight()-1, (int)rcOut.Y);
     ptSelected[3] = CPoint((int)rcOut.GetRight()-1, (int)(rcOut.Y + rcOut.Height / 2));
     ptSelected[4] = CPoint((int)rcOut.GetRight()-1, (int)rcOut.GetBottom()-1);
     ptSelected[5] = CPoint((int)(rcOut.X + rcOut.Width / 2), (int)rcOut.GetBottom()-1);
     ptSelected[6] = CPoint((int)rcOut.X, (int)rcOut.GetBottom()-1);
     ptSelected[7] = CPoint((int)rcOut.X, (int)(rcOut.Y + rcOut.Height / 2));
     Gdiplus::REAL fRadius = (Gdiplus::REAL)(3.0 / pZoomManager->GetZoomFactor());
     for (int i = 0; i < 8; ++i)
     {
     graphics.FillEllipse(&whiteBrush, 
     (Gdiplus::REAL)(ptSelected[i].x - fRadius), (Gdiplus::REAL)(ptSelected[i].y - fRadius), 
     2*fRadius, 2*fRadius);
     graphics.DrawEllipse(&blackPen, 
     (Gdiplus::REAL)(ptSelected[i].x - fRadius), (Gdiplus::REAL)(ptSelected[i].y - fRadius), 
     2*fRadius, 2*fRadius);
     }
     
   /*/ 
}


_TCHAR* CDynamicObject::GetSubClassIdent()
{
   if (m_idType == DYNAMIC_RADIOBUTTON)
   {
      return _T("RADIO");
   }
   else if (m_idType == DYNAMIC_CHECKBOX)
   {
      return _T("CHECK");
   }
   else if (m_idType == DYNAMIC_TEXTFIELD)
   {
      return _T("FIELD");
   }
   else
      return NULL; // shouldn't happen?
}

HRESULT CDynamicObject::AppendSubClassParams(_TCHAR *tszOutput)
{
   _tcscat(tszOutput, _T(" give=\""));

   if (m_idType == DYNAMIC_RADIOBUTTON || m_idType == DYNAMIC_CHECKBOX)
   {
      if (m_bHasToBeChecked)
         _tcscat(tszOutput, _T("w\""));
      else
         _tcscat(tszOutput, _T("n\""));
   }
   else if (m_idType == DYNAMIC_TEXTFIELD)
   {
      CString csAllTexts;
      for (int i=0; i<m_aCorrectTexts.GetSize(); ++i)
      {
         CString csOneText = m_aCorrectTexts[i];
         csOneText.Replace(_T(";"), _T("\\;"));
         csOneText.Replace(_T("\\"), _T("\\\\"));
         
         csAllTexts += csOneText;
         csAllTexts += _T(";"); // must also finish with a ";" -> easier parsing
      }
      // TODO this is no char*!
      _stprintf(tszOutput + _tcslen(tszOutput), _T("%s\""), csAllTexts);
   }
   else
      _tcscat(tszOutput, _T("\"")); // shouldn't happen?

   return S_OK;
}

HRESULT CDynamicObject::SetInteractionName(CString &csName, bool bChangeText)
{
   return CInteractionAreaEx::SetInteractionName(csName, false);
}
   
HRESULT CDynamicObject::Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
      LPCTSTR tszExportPrefix, CEditorProject *pProject)
{
   return CInteractionAreaEx::Write(pOutput, bDoIdent, bUseFullImagePath, mapImagePaths, tszExportPrefix, pProject);
}

// private
void CDynamicObject::CreateNameAndType()
{
   if (m_idType == DYNAMIC_RADIOBUTTON)
   {
      m_csDisplayName.LoadString(IDS_DYNAMIC_RADIOBUTTON);
      m_csDisplayType.LoadString(IDS_DYNAMIC_RADIOBUTTON);
   }
   else if (m_idType == DYNAMIC_CHECKBOX)
   {
      m_csDisplayName.LoadString(IDS_DYNAMIC_CHECKBOX);
      m_csDisplayType.LoadString(IDS_DYNAMIC_CHECKBOX);
   }
   else if (m_idType == DYNAMIC_TEXTFIELD)
   {
      m_csDisplayName.LoadString(IDS_DYNAMIC_TEXTFIELD);
      m_csDisplayType.LoadString(IDS_DYNAMIC_TEXTFIELD);
   }
   else
   {
      m_csDisplayName.Empty();
      m_csDisplayType.Empty();
   }
}
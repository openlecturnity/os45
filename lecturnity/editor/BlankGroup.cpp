#include "stdafx.h"
#include "BlankGroup.h"
#include "InteractionAreaEx.h"
#include "InteractionStream.h"
#include "MoveableObjects.h"
#include "..\Studio\resource.h"
#include "lutils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CBlankGroup, CMoveableObjects)

CBlankGroup::CBlankGroup()
{
   //m_pEditorDoc = NULL;
   m_aDrawObjects.SetSize(0, 25);
   m_aBlankObjects.SetSize(0, 10);
}

CBlankGroup::~CBlankGroup()
{
   for (int i=0; i<m_aDrawObjects.GetSize(); ++i)
      delete m_aDrawObjects[i];

   for (i=0; i<m_aBlankObjects.GetSize(); ++i)
      delete m_aBlankObjects[i];
}

HRESULT CBlankGroup::Init(CTimePeriod *pVisibility, CTimePeriod *pActivity, 
                          CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects, 
                          CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paBlanks, CEditorProject *pProject)
{
   if (paObjects == NULL || paBlanks == NULL || pProject == NULL || pVisibility == NULL || pActivity == NULL)
      return E_POINTER;

   //m_pEditorDoc = pDoc;
   
   for (int i=0; i<paObjects->GetSize(); ++i)
      m_aDrawObjects.Add(paObjects->GetAt(i)->Copy());
    
   for (i=0; i<paBlanks->GetSize(); ++i)
      m_aBlankObjects.Add(paBlanks->GetAt(i)->Copy());

   CRect rcTotalSize;
   CalculateDimensions(&m_aDrawObjects, &rcTotalSize);

   for (i=0; i<paBlanks->GetSize(); ++i)
   {
      if (i == 0 && m_aBlankObjects.GetSize() == 0)
         rcTotalSize = m_aBlankObjects[i]->GetArea();
      else
         rcTotalSize.UnionRect(rcTotalSize, m_aBlankObjects[i]->GetArea());
   }

   HRESULT hr = CInteractionAreaEx::Init(&rcTotalSize, false,
      NULL, NULL, NULL, NULL, NULL, pVisibility, pActivity, pProject);

   CreateNameAndType();

   return hr;
}
   
HRESULT CBlankGroup::Init(DrawSdk::DrawObject *pObject, CInteractionArea *pBlank, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (pObject == NULL && pBlank == NULL)
      return E_POINTER; // at least one must be set

   if (pObject != NULL && pBlank != NULL)
      return E_INVALIDARG; // at most one can be set

   if (pProject == NULL)
      return E_POINTER;

   if (pBlank != NULL && !pBlank->IsTextField())
      return E_INVALIDARG;


   //m_pEditorDoc = pDoc;

   CTimePeriod Visibility;
   CTimePeriod Activity;

   if (pBlank != NULL)
   {
      if (SUCCEEDED(hr))
         hr = AddDuringParsing(pBlank, pProject);

      if (SUCCEEDED(hr))
         hr = Visibility.Parse(pBlank->GetVisibility());
      if (SUCCEEDED(hr))
         hr = Activity.Parse(pBlank->GetActivity());
   }
   else if (pObject != NULL)
   {
      hr = AddDuringParsing(pObject);

      CString csVisibility = pObject->GetVisibility();
      if (SUCCEEDED(hr))
         hr = Visibility.Parse(csVisibility);
      if (SUCCEEDED(hr))
         hr = Visibility.CloneTo(&Activity);
   }

   if (SUCCEEDED(hr))
   {
      CRect rcDummy = m_rcDimensions;
      hr = CInteractionAreaEx::Init(&rcDummy, false,
         NULL, NULL, NULL, NULL, NULL, &Visibility, &Activity, pProject);
   }

   CreateNameAndType();

   return hr;
}

HRESULT CBlankGroup::AddDuringParsing(DrawSdk::DrawObject *pObject)
{
   HRESULT hr = S_OK;

   if (pObject == NULL)
      return E_POINTER;

   CRect rcSize;
   pObject->GetLogicalBoundingBox(&rcSize);

   if (m_rcDimensions.Width() == 0)
      m_rcDimensions = rcSize;
   else
      m_rcDimensions.UnionRect(m_rcDimensions, rcSize);

   m_aDrawObjects.Add(pObject->Copy());
  
   CreateNameAndType();

   return hr;
}

HRESULT CBlankGroup::AddDuringParsing(CInteractionArea *pBlank, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (pBlank == NULL)
      return E_POINTER;

   if (!pBlank->IsTextField())
      return E_INVALIDARG;

   CDynamicObject* pDynamic = new CDynamicObject();
   hr = pDynamic->Init(pBlank, pProject);
   
   if (m_rcDimensions.Width() == 0)
      m_rcDimensions = pBlank->GetActivityArea();
   else
      m_rcDimensions.UnionRect(m_rcDimensions, pBlank->GetActivityArea());

   m_aBlankObjects.Add(pDynamic);

   CreateNameAndType();

   return hr;
}


HRESULT CBlankGroup::SetArea(CRect *prcArea)
{
   if (prcArea == NULL)
      return E_POINTER;

   
   m_rcDimensions = *prcArea;
   CalculateNewPosition(m_rcDimensions);

   return S_OK;
}

HRESULT CBlankGroup::Move(float fDiffX, float fDiffY)
{
   CInteractionAreaEx::Move(fDiffX, fDiffY);
   
   for (int i=0; i<m_aDrawObjects.GetSize(); ++i)
      m_aDrawObjects[i]->Move(fDiffX, fDiffY);
   
   for (i=0; i<m_aBlankObjects.GetSize(); ++i)
      m_aBlankObjects[i]->Move(fDiffX, fDiffY);
  
   return (fDiffX != 0 || fDiffY != 0) ? S_OK : S_FALSE;
}

void CBlankGroup::Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing)
{
   int iOffsetX = 0;
   int iOffsetY = 0;
   if (ptOffsets != NULL)
   {
      iOffsetX = ptOffsets->x;
      iOffsetY = ptOffsets->y;
   }
   
   for (int i=0; i<m_aDrawObjects.GetSize(); ++i)
      m_aDrawObjects[i]->Draw(pDC->m_hDC, iOffsetX, iOffsetY, pZoomManager);

   for (i=0; i<m_aBlankObjects.GetSize(); ++i)
      m_aBlankObjects[i]->Draw(pDC, ptOffsets, pZoomManager, bPreviewing);

   if (!bPreviewing && IsSelected())
      DrawBorder(pDC, ptOffsets, pZoomManager);
}

// TODO only implement DrawBorder() in CInteractionAreaEx: there are only two variations...
void CBlankGroup::DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager)
{
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   graphics.TranslateTransform((float)ptOffsets->x, (float)ptOffsets->y);
   
   Gdiplus::Color clrWhite(255, 255, 255, 255);
   Gdiplus::Color clrBlack(255, 0, 0, 0);
   Gdiplus::RectF rcOut((Gdiplus::REAL)(m_rcDimensions.left * pZoomManager->GetZoomFactor()), 
                        (Gdiplus::REAL)(m_rcDimensions.top * pZoomManager->GetZoomFactor()), 
                        (Gdiplus::REAL)(m_rcDimensions.Width() * pZoomManager->GetZoomFactor()), 
                        (Gdiplus::REAL)(m_rcDimensions.Height() * pZoomManager->GetZoomFactor()));
   
   Gdiplus::REAL fLineWidth = 6.0;
   Gdiplus::REAL fRadius = 3.0;
   
   Gdiplus::HatchBrush brush(Gdiplus::HatchStyleLightUpwardDiagonal, clrBlack, clrWhite);
   Gdiplus::Pen selectionRectPen(&brush, fLineWidth);
   rcOut.Inflate((Gdiplus::REAL)fLineWidth / 2, (Gdiplus::REAL)fLineWidth / 2);
   graphics.DrawRectangle(&selectionRectPen, rcOut);
   
   Gdiplus::REAL fPenWidth = 0.5f;
   Gdiplus::Pen blackPen(clrBlack, fPenWidth);
   Gdiplus::SolidBrush whiteBrush(clrWhite);
   
   Gdiplus::PointF ptSelected[8];
   ptSelected[0] = Gdiplus::PointF(rcOut.X, rcOut.Y-0.5f);
   ptSelected[1] = Gdiplus::PointF((rcOut.X + rcOut.Width / 2.0f), rcOut.Y-0.5f);
   ptSelected[2] = Gdiplus::PointF(rcOut.GetRight()-0.5f, rcOut.Y-0.5f);
   ptSelected[3] = Gdiplus::PointF(rcOut.GetRight()-0.5f, (rcOut.Y + rcOut.Height / 2.0f));
   ptSelected[4] = Gdiplus::PointF(rcOut.GetRight()-0.5f, rcOut.GetBottom());
   ptSelected[5] = Gdiplus::PointF((rcOut.X + rcOut.Width / 2.0f), rcOut.GetBottom());
   ptSelected[6] = Gdiplus::PointF(rcOut.X-0.5f, rcOut.GetBottom());
   ptSelected[7] = Gdiplus::PointF(rcOut.X-0.5f, (rcOut.Y + rcOut.Height / 2.0f));
   
   for (int i = 0; i < 8; ++i)
   {
      graphics.FillEllipse(&whiteBrush, 
         (ptSelected[i].X - fRadius), (ptSelected[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
      graphics.DrawEllipse(&blackPen, 
         (ptSelected[i].X - fRadius), (ptSelected[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
   }
}

HRESULT CBlankGroup::SetInteractionName(CString &csName, bool bChangeText)
{
   return CInteractionAreaEx::SetInteractionName(csName, false);
}

HRESULT CBlankGroup::Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
                           CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, LPCTSTR tszExportPrefix, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (pOutput == NULL)
      return E_POINTER;

   UINT nIdentLength = bDoIdent ? 5 : 0;

   static _TCHAR tszOutput[2000];
   tszOutput[0] = 0;

   static _TCHAR tszTimes[200];
   tszTimes[0] = 0;
   m_Visibility.AppendTimes(tszTimes);

   // Note: the times of the single objects are set/corrected before writing.
   // These (single) times might be wrong as they are not adapted during stream operations.
   // Only the times of the CBlankGroup object are corrected.

   // write blanks first: they also have an activity
   for (int i=0; i<m_aBlankObjects.GetSize() && SUCCEEDED(hr); ++i)
   {
      m_aBlankObjects[i]->SetVisibilityPeriod(&m_Visibility);
      m_aBlankObjects[i]->SetActivityPeriod(&m_Activity);

      hr = m_aBlankObjects[i]->Write(pOutput, bDoIdent, 
         bUseFullImagePath, mapImagePaths, tszExportPrefix, pProject);
   }

   for (i=0; i<m_aDrawObjects.GetSize() && SUCCEEDED(hr); ++i)
   {
      m_aDrawObjects[i]->SetVisibility(tszTimes);

      DrawSdk::CharArray array;

      m_aDrawObjects[i]->Write(&array, bUseFullImagePath);

      hr = pOutput->Append(array.GetBuffer(), array.GetLength());
   }
   
   return hr;
}

bool CBlankGroup::Reset(bool bAfterPreview)
{
   bool bChange = CInteractionAreaEx::Reset(bAfterPreview);
   
   for (int i=0; i<m_aBlankObjects.GetSize(); ++i)
   {
      if (m_aBlankObjects[i]->GetClassId() == INTERACTION_CLASS_DYNAMIC)
      {
         bool bSomeInnerChange = 
            ((CDynamicObject *)m_aBlankObjects[i])->Reset(bAfterPreview);

         if (bSomeInnerChange)
            bChange = true;
      }
   }

   return bChange;
}

HRESULT CBlankGroup::AddNativesToView()
{
   HRESULT hr = S_OK;
   
   for (int i=0; i<m_aBlankObjects.GetSize() && SUCCEEDED(hr); ++i)
   {
      if (m_aBlankObjects[i]->GetClassId() == INTERACTION_CLASS_DYNAMIC)
         hr = ((CDynamicObject *)m_aBlankObjects[i])->AddNativeToView();
   }
   
   return hr;
}

bool CBlankGroup::ShowHide(UINT nPositionMs, bool bPreviewing, bool bForceHide)
{
   bool bSomeChange = false;

   if (m_Visibility.Contains(nPositionMs) && m_aBlankObjects.GetSize() > 0)
      nPositionMs = m_aBlankObjects[0]->GetVisibilityStart();
   // small hack as the objects in m_aBlankObjects might not have the right 
   // times (after stream operations)

   for (int i=0; i<m_aBlankObjects.GetSize(); ++i)
   {
      if (m_aBlankObjects[i]->GetClassId() == INTERACTION_CLASS_DYNAMIC)
      {
         
         bool bSomeInnerChange = 
            ((CDynamicObject *)m_aBlankObjects[i])->ShowHide(nPositionMs, bPreviewing, bForceHide);

         if (bSomeInnerChange)
            bSomeChange = true;
      }
   }

   return bSomeChange;
}

bool CBlankGroup::IsAnswerCorrect()
{
   bool bAnswerCorrect = true;

   for (int i=0; i<m_aBlankObjects.GetSize(); ++i)
   {
      if (m_aBlankObjects[i]->GetClassId() == INTERACTION_CLASS_DYNAMIC)
      {
         bool bInnerCorrect = ((CDynamicObject *)m_aBlankObjects[i])->IsAnswerCorrect();

         if (!bInnerCorrect)
            bAnswerCorrect = false;
      }
   }

   return bAnswerCorrect;
}

// protected
HRESULT CBlankGroup::CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey)
{
   HRESULT hr = S_OK;

   if (pTarget->GetClassId() != GetClassId())
      return E_INVALIDARG;

   hr = CInteractionAreaEx::CloneTo(pTarget, bCopyHashKey);

   if (SUCCEEDED(hr))
   {
      CBlankGroup *pOther = (CBlankGroup *)pTarget;

      //pOther->m_pEditorDoc = m_pEditorDoc;

      pOther->m_aDrawObjects.SetSize(0, m_aDrawObjects.GetSize());
      for (int i=0; i<m_aDrawObjects.GetSize(); ++i)
         pOther->m_aDrawObjects.Add(m_aDrawObjects[i]->Copy());
      
      pOther->m_aBlankObjects.SetSize(0, m_aBlankObjects.GetSize());
      for (i=0; i<m_aBlankObjects.GetSize(); ++i)
         pOther->m_aBlankObjects.Add(m_aBlankObjects[i]->Copy());
   }

   return hr;
}


// private
void CBlankGroup::CreateNameAndType()
{
   m_csDisplayName.Empty();
   if (m_aDrawObjects.GetSize() > 0 && m_aDrawObjects[0]->GetType() == DrawSdk::TEXT)
      m_csDisplayName = ((DrawSdk::Text *)m_aDrawObjects[0])->GetString();

   m_csDisplayType.LoadString(IDS_BLANK_GROUP_TYPE);
}

void CBlankGroup::CalculateNewPosition(CRect &rcArea)
{
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paTexts = GetDrawObjects();
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paBlanks = GetBlankObjects();
   
   CDynamicObject *pBlank = NULL;
   int iBlankIndex = 0;
   if (paBlanks->GetSize() > 0)
      pBlank = (CDynamicObject *)paBlanks->GetAt(0);
   int iBlankX = -1;
   int iBlankTop = -1;
   int iBlankBottom = -1;
   if (pBlank)
   {
      CRect rcBlank = pBlank->GetArea();
      iBlankX = rcBlank.left;
      iBlankTop = rcBlank.top;
      iBlankBottom = rcBlank.bottom;
   }
   
   DrawSdk::DrawObject *pText = NULL;
   int iTextIndex = 0;
   if (paTexts->GetSize() > 0)
      pText = paTexts->GetAt(0);
   
   LOGFONT lf;
   LFont::FillWithDefaultValues(&lf);
   
   int iTextX = -1;
   int iTextY = -1;
   if (pText)
   {
      ((DrawSdk::Text *)pText)->GetLogFont(&lf);
      iTextX = (int)pText->GetX();
      iTextY = (int)pText->GetY();
   }
   
   double dBlankWidth = DrawSdk::Text::GetTextWidth(_T(" "), 1, &lf);
   double dTextAscent = DrawSdk::Text::GetTextAscent(&lf); 
   double dTextHeight = abs(lf.lfHeight); 
   double dLineHeight = 2 * dTextHeight;
   double dYBorder = dTextHeight / 4;

   double dActX = rcArea.left;
   double dActY = rcArea.top;
   int iMaxX = rcArea.right;

   int dMaxY = 0;
   while (pText)
   {
      if (pBlank && 
          ((iTextY < iBlankBottom && iTextY > iBlankTop && iBlankX < iTextX) ||   // same line
           (iTextY > iBlankBottom)))                                              // text is in next line
      {
         CRect rcBlank = pBlank->GetArea();
         if (dActX + rcBlank.Width() > iMaxX)
         {
            dActY += dLineHeight;
            dActX = rcArea.left;
         }
         
         double dXWidth = 0;
         CArray<CString , CString > *paCorrectAnswers = pBlank->GetCorrectTexts();
         for (int k = 0; k < paCorrectAnswers->GetSize(); ++k)
         {
            Gdiplus::RectF bbox;
            CString csAnswer = paCorrectAnswers->GetAt(k);
            DrawSdk::Text::GetBoundingBox(csAnswer, csAnswer.GetLength(), &lf, bbox);
            if (bbox.Width > dXWidth)
               dXWidth = bbox.Width;
         }
               
         rcBlank.left = (int)dActX;
         rcBlank.top = (int)dActY;
         rcBlank.right = (int)(dActX + dXWidth);
         rcBlank.bottom = (int)(dActY + dTextHeight + dYBorder);
         
         pBlank->SetArea(&rcBlank);

         dActX += rcBlank.Width() + dBlankWidth;
         
         ++iBlankIndex;
         if (iBlankIndex < paBlanks->GetSize())
         {
            pBlank = (CDynamicObject *)paBlanks->GetAt(iBlankIndex);
            CRect rcBlank = pBlank->GetArea();
            iBlankX = rcBlank.left;
            iBlankTop = rcBlank.top;
            iBlankBottom = rcBlank.bottom;
         }
         else
         {
            if (pBlank)
            {
               CRect rcBBox;
               rcBBox = pBlank->GetArea();
               if (rcBBox.bottom > dMaxY)
                  dMaxY = rcBBox.bottom;
            }
            pBlank = NULL;
         }

      }
      else
      {
         if (dActX + pText->GetWidth() > iMaxX)
         {
            dActY = dActY + dLineHeight;
            dActX = rcArea.left;
         }

         pText->SetX(dActX);
         pText->SetY(dActY+dTextAscent);
         
         
         if (((DrawSdk::Text *)pText)->EndsWithReturn())
         {
            dActY = dActY + dLineHeight;
            dActX = rcArea.left;
         }
         else
            dActX += pText->GetWidth() + dBlankWidth;

         ++iTextIndex;
         if (iTextIndex < paTexts->GetSize())
         {
            pText = paTexts->GetAt(iTextIndex);
            iTextX = (int)pText->GetX();
            iTextY = (int)pText->GetY();
         }
         else
         {
            if (pText)
            {
               CRect rcBBox;
               pText->GetLogicalBoundingBox(&rcBBox);
               if (rcBBox.bottom > dMaxY)
                  dMaxY = rcBBox.bottom;
            }
            pText = NULL;
         }
         

      }
   }
   
   if (pBlank != NULL)
   {
      for (int j = iBlankIndex; j < paBlanks->GetSize(); ++j)
      {
         pBlank = (CDynamicObject *)paBlanks->GetAt(j);
         
         CRect rcBlank = pBlank->GetArea();
         if (dActX + rcBlank.Width() > iMaxX)
         {
            dActY = dActY + dLineHeight;
            dActX = rcArea.left;
         }

         double dXWidth = 0;
         CArray<CString , CString > *paCorrectAnswers = pBlank->GetCorrectTexts();
         for (int k = 0; k < paCorrectAnswers->GetSize(); ++k)
         {
            Gdiplus::RectF bbox;
            CString csAnswer = paCorrectAnswers->GetAt(k);
            DrawSdk::Text::GetBoundingBox(csAnswer, csAnswer.GetLength(), &lf, bbox);
            if (bbox.Width > dXWidth)
               dXWidth = bbox.Width;
         }
         
         rcBlank.left = (int)dActX;
         rcBlank.top = (int)dActY;
         rcBlank.right = (int)(dActX + dXWidth);
         rcBlank.bottom = (int)(dActY + dTextHeight + dYBorder);
         
         pBlank->SetArea(&rcBlank);
         
         dActX += rcBlank.Width() + dBlankWidth;
         
      }
      if (pBlank)
      {
         CRect rcBBox;
         rcBBox = pBlank->GetArea();
         if (rcBBox.bottom > dMaxY)
            dMaxY = rcBBox.bottom;
      }
   }

   rcArea.bottom = dMaxY;

}
#include "stdafx.h"
#include "..\Studio\resource.h"
#include "editorDoc.h"
#include "MoveableObjects.h"
#include <math.h>

IMPLEMENT_DYNCREATE(CMoveableObjects, CInteractionAreaEx)

CMoveableObjects::CMoveableObjects()
{
   m_fMoveForPreviewOnlyX = 0;
   m_fMoveForPreviewOnlyY = 0;
   m_fSnapInMoveX = 0;
   m_fSnapInMoveY = 0;
   m_bCorrectSnapping = false;

   m_idMoveableType = MOVEABLE_TYPE_NONE;
   m_nBelongsKey = 0;
}

CMoveableObjects::~CMoveableObjects()
{
   for (int i=0; i<m_aObjects.GetSize(); ++i)
      delete m_aObjects[i];
}

HRESULT CMoveableObjects::Init(CRect *prcSize, CTimePeriod *pVisibility, CEditorProject *pProject, 
                               CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects,
                               MoveableTypeId idMoveableType, UINT nBelongsKey)
{
   if (pVisibility == NULL || paObjects == NULL)
      return E_POINTER;

   if (paObjects->GetSize() <= 0)
      return E_INVALIDARG;

   if (pVisibility->GetPeriodId() == INTERACTION_PERIOD_NEVER)
      return E_IAX_INVALID_DATA;

   m_aObjects.SetSize(paObjects->GetSize(), 10);
   for (int i=0; i<paObjects->GetSize(); ++i)
      m_aObjects.SetAt(i, paObjects->GetAt(i)->Copy());

   m_idMoveableType = idMoveableType;
   m_nBelongsKey = nBelongsKey;

   CRect rcTotalSize;
   if (prcSize == NULL)
   {
      // calculate bounding box from objects

      CalculateDimensions(paObjects, &rcTotalSize);
      prcSize = &rcTotalSize;

      // TODO (inconsistency): no "+2" here (see other Init()); which is better?
   }


   HRESULT hrSuper = CInteractionAreaEx::Init(prcSize, false, 
      NULL, NULL, NULL, NULL, NULL, pVisibility, pVisibility, pProject);

   CreateNameAndType();

   return hrSuper;
}

HRESULT CMoveableObjects::Init(DrawSdk::DrawObject *pObject, CEditorProject *pProject)
{
   if (pObject == NULL || pProject == NULL)
      return E_POINTER;

   CRect rcSize;
   pObject->GetLogicalBoundingBox(&rcSize);

   //pObject->ResetVisibility();

   MoveableTypeId idMoveableType = MOVEABLE_TYPE_OTHER;

   if (pObject->IsDndMoveable())
      idMoveableType = MOVEABLE_TYPE_SOURCE;
   else if (pObject->GetType() == DrawSdk::TARGET)
      idMoveableType = MOVEABLE_TYPE_TARGET;

   UINT nBelongsKey = (unsigned)pObject->GetBelongsKey();

   CTimePeriod periodVisible;
   CString csVisibleDefine = pObject->GetVisibility();
   periodVisible.Parse(csVisibleDefine);
   
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aDummy;
   aDummy.SetSize(1, 0);
   aDummy.SetAt(0, pObject);

   // TODO key "0"
   return Init(&rcSize, &periodVisible, pProject, &aDummy, idMoveableType, nBelongsKey);
}


InteractionClassId CMoveableObjects::GetClassId()
{
   return INTERACTION_CLASS_MOVE;
}

CInteractionAreaEx* CMoveableObjects::MakeNew()
{
   return new CMoveableObjects();
}

// protected
HRESULT CMoveableObjects::CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey)
{
   HRESULT hr = S_OK;

   hr = CInteractionAreaEx::CloneTo(pTarget, bCopyHashKey);

   if (SUCCEEDED(hr))
   {
      if (pTarget->GetClassId() != GetClassId())
         return E_INVALIDARG;

      CMoveableObjects *pOther = (CMoveableObjects *)pTarget;

      for (int i=0; i<pOther->m_aObjects.GetSize(); ++i)
         delete pOther->m_aObjects[i];

      pOther->m_aObjects.SetSize(m_aObjects.GetSize(), 10);
      for (i=0; i<m_aObjects.GetSize(); ++i)
         pOther->m_aObjects.SetAt(i, m_aObjects[i]->Copy());
      
         
      pOther->m_idMoveableType = m_idMoveableType;
      pOther->m_nBelongsKey = m_nBelongsKey;
   }
   
   return hr;
}

CRect& CMoveableObjects::GetArea()
{
   static CRect rcMoved;

   rcMoved = m_rcDimensions;

   if (m_fSnapInMoveX != 0 || m_fSnapInMoveY != 0)
   {
      rcMoved.OffsetRect((int)m_fSnapInMoveX, (int)m_fSnapInMoveY);
   }

   return rcMoved;
}

void CMoveableObjects::Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing)
{
   if (!bPreviewing && m_bHiddenDuringEdit || bPreviewing && m_bHiddenDuringPreview)
      return;

   float fOffsetX = (float)ptOffsets->x;
   float fOffsetY = (float)ptOffsets->y;

   double dZoom = 1.0;
   if (pZoomManager != NULL)
      dZoom = pZoomManager->GetZoomFactor();

   if (m_fSnapInMoveX != 0 || m_fSnapInMoveY != 0)
   {
      fOffsetX += (float)(dZoom * m_fSnapInMoveX);
      fOffsetY += (float)(dZoom * m_fSnapInMoveY);
   }
   
   for (int i=0; i<m_aObjects.GetSize(); ++i)
      m_aObjects[i]->Draw(pDC->m_hDC, fOffsetX, fOffsetY, pZoomManager);

   if (!bPreviewing && IsSelected())
      DrawBorder(pDC, ptOffsets, pZoomManager);
}

void CMoveableObjects::DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager)
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
   
   if (IsMoveableSource() || IsMoveableTarget())
   {
      Gdiplus::REAL fRadius = 3.0;
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
}

HRESULT CMoveableObjects::Move(float fDiffX, float fDiffY)
{
   CInteractionAreaEx::Move(fDiffX, fDiffY);
   
   for (int i=0; i<m_aObjects.GetSize(); ++i)
   {
      m_aObjects[i]->SetX(m_aObjects[i]->GetX() + fDiffX);
      m_aObjects[i]->SetY(m_aObjects[i]->GetY() + fDiffY);
   }
   
   return (fDiffX != 0 || fDiffY != 0) ? S_OK : S_FALSE;
}

HRESULT CMoveableObjects::MoveForPreviewOnly(float fDiffX, float fDiffY, 
                                             CArray<CPoint, CPoint> *paAllTargets,
                                             int iTargetX, int iTargetY)
{
   m_fMoveForPreviewOnlyX += fDiffX;
   m_fMoveForPreviewOnlyY += fDiffY;

   HRESULT hr = Move(fDiffX, fDiffY);

   if (SUCCEEDED(hr) && paAllTargets != NULL)
   {
      // check for snap in
      
      float fSnapInDistance = (m_rcDimensions.Width() + m_rcDimensions.Height()) / 5.0f;
      float fObjectsCenterX = m_rcDimensions.left + m_rcDimensions.Width() / 2.0f;
      float fObjectsCenterY = m_rcDimensions.top + m_rcDimensions.Height() / 2.0f;

      if (m_aObjects.GetSize() == 1)
      {
         // m_rcDimensions is only integer but we need floating point
         // otherwise the object is moved continuously (floating point) but its center
         // is only moved stepwise (integer)
         fObjectsCenterX = (float)(m_aObjects[0]->GetX() + m_aObjects[0]->GetWidth() / 2.0);
         fObjectsCenterY = (float)(m_aObjects[0]->GetY() + m_aObjects[0]->GetHeight() / 2.0);
      }
      // TODO fix this for an arbitrary number of elements
      
      bool bSnapInFound = false;
      for (int i=0; i<paAllTargets->GetSize(); ++i)
      {
         float fdX = paAllTargets->GetAt(i).x - fObjectsCenterX + 0.5f;
         float fdY = paAllTargets->GetAt(i).y - fObjectsCenterY + 0.5f;
         // "+ 0.5f": the values in paAllTargets->GetAt(i) were probably rounded down
         float fDistance = (float)sqrt(fdX*fdX + fdY*fdY);
         
         if (fDistance < fSnapInDistance)
         {
            m_fSnapInMoveX = fdX;
            m_fSnapInMoveY = fdY;

            // will be regarded during painting
            
            bSnapInFound = true;

            if (iTargetX == -1 && iTargetY == -1)
            {
               m_bCorrectSnapping = true;
               // target undefined: always correct
            }
            else if (iTargetX == paAllTargets->GetAt(i).x && iTargetY == paAllTargets->GetAt(i).y)
            {
               m_bCorrectSnapping = true;
            }
            else
            {
               m_bCorrectSnapping = false;
            }

            break;
         }
      }
      
      if (!bSnapInFound)
      {
         m_fSnapInMoveX = 0;
         m_fSnapInMoveY = 0;
         m_bCorrectSnapping = false;
      }
   }
   
   return hr;
}

bool CMoveableObjects::Reset(bool bAfterPreview)
{
   // reset any glogal variables (at the moment: m_bDisabledForPreview)
   CInteractionAreaEx::Reset(bAfterPreview);
   // ignore return value of this at the moment

   bool bChange = false;

   m_bCorrectSnapping = false;

   if (m_fMoveForPreviewOnlyX != 0 || m_fMoveForPreviewOnlyY != 0)
   {
      HRESULT hr = Move(-m_fMoveForPreviewOnlyX, -m_fMoveForPreviewOnlyY);
      
      m_fMoveForPreviewOnlyX = 0;
      m_fMoveForPreviewOnlyY = 0;

      m_fSnapInMoveX = 0;
      m_fSnapInMoveY = 0;
      
      if (SUCCEEDED(hr) && hr != S_FALSE)
         bChange = true;
   }

   return bChange;
}

HRESULT CMoveableObjects::SetInteractionName(CString &csName, bool bChangeText)
{
   HRESULT hr = CInteractionAreaEx::SetInteractionName(csName, false);

   if (SUCCEEDED(hr) && bChangeText)
   {
      for (int i = 0; i < m_aObjects.GetSize(); ++i)
      {
         if (m_aObjects[i]->GetType() == DrawSdk::TEXT)
         {
            DrawSdk::Text *pText = (DrawSdk::Text *)m_aObjects[i];

            if (pText->GetType() != DrawSdk::TEXT_TYPE_BLANK_SUPPORT)
               pText->SetString(csName, csName.GetLength());
         }
      }
   }

   if (SUCCEEDED(hr))
   {
      CRect rcNewArea;
      hr = CalculateDimensions(&m_aObjects, &rcNewArea);
      hr = SetArea(&rcNewArea);
   }

   return hr;
}

bool CMoveableObjects::IsInteractionNameText()
{
   for (int i = 0; i < m_aObjects.GetSize(); ++i)
   {
      if (m_aObjects[i]->GetType() == DrawSdk::TEXT)
         return true;
   }
   
   return false;
}
   
HRESULT CMoveableObjects::Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
      LPCTSTR tszExportPrefix, CEditorProject *pProject)
{
   HRESULT hr = m_aObjects.GetSize() > 0 ? S_OK : S_FALSE;

   // TODO proper and general (done at one place/object) indentation

   if (pOutput == NULL)
      return E_POINTER;

   static _TCHAR tszTimes[200];
   tszTimes[0] = 0;
   m_Visibility.AppendTimes(tszTimes);
      
   for (int i=0; i<m_aObjects.GetSize() && SUCCEEDED(hr); ++i)
   {
      // TODO this side effect on m_objectCounter is very bad style.
      m_aObjects[i]->SetObjectNumber(CWhiteboardStream::m_objectCounter++);

      // make it correspond with the "container" object
      m_aObjects[i]->SetVisibility(tszTimes);
      
      if (m_idMoveableType == MOVEABLE_TYPE_SOURCE || m_idMoveableType == MOVEABLE_TYPE_TARGET)
      {
         m_aObjects[i]->SetBelongsKey(m_nBelongsKey);
      }

      if (tszExportPrefix != NULL && m_aObjects[i]->GetType() == DrawSdk::IMAGE)
      {
          CString csExportPrefix = tszExportPrefix;
          if (SUCCEEDED(hr)) {
              TCHAR *tszSourcePath = NULL;
              if (pProject != NULL) {
                  CString csProjectPath = pProject->GetFilename();
                  StringManipulation::GetPath(csProjectPath);
                  tszSourcePath = new TCHAR[csProjectPath.GetLength() + 1];
                  _tcscpy(tszSourcePath, csProjectPath);
              }
              hr = CWhiteboardStream::CopyImage((DrawSdk::Image *)m_aObjects[i], csExportPrefix, mapImagePaths, tszSourcePath);

              if (tszSourcePath != NULL)
                  delete tszSourcePath;
         }
      }
      
      int idType = m_aObjects[i]->GetType();
      
      if (SUCCEEDED(hr))
         hr = pOutput->Append(m_aObjects[i], bUseFullImagePath);

   }

   return hr;

}

// protected
HRESULT CMoveableObjects::CalculateDimensions(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects, CRect *prcTotal)
{
   if (paObjects == NULL || prcTotal == NULL)
      return E_POINTER;
   
   for (int i=0; i<paObjects->GetSize(); ++i)
   {
      CRect rcOneSize;
      paObjects->GetAt(i)->GetLogicalBoundingBox((LPRECT)rcOneSize);
      
      if (i == 0)
         *prcTotal = rcOneSize;
      else
         prcTotal->UnionRect(*prcTotal, rcOneSize);
   }
   
   return S_OK;
}

// private
void CMoveableObjects::CreateNameAndType()
{
   m_csDisplayType.LoadString(IDS_TESTOBJECT);
   m_csDisplayName.Empty();
   
   if (m_aObjects.GetSize() > 0)
   {
      
      if (m_aObjects[0]->GetType() == DrawSdk::TEXT)
      {
         DrawSdk::Text *pText = (DrawSdk::Text *)m_aObjects[0];
         
         if (pText->GetTextType() == DrawSdk::TEXT_TYPE_QUESTION_TITLE)
            m_csDisplayType.LoadString(IDS_QUESTION_TITLE);
         else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_QUESTION_TEXT)
            m_csDisplayType.LoadString(IDS_QUESTION_QUESTION);
         else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_NUMBER_SUPPORT)
            m_csDisplayType.LoadString(IDS_QUESTION_PROGRESS);
         else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_TRIES_SUPPORT)
            m_csDisplayType.LoadString(IDS_QUESTION_TRIES);
         else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_RADIO_SUPPORT)
            m_csDisplayType.LoadString(IDS_QUESTION_RADIOBUTTON);
         else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_BLANK_SUPPORT)
            m_csDisplayType.LoadString(IDS_QUESTION_ANSWER); // TODO
         else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_TARGET_SUPPORT)
            m_csDisplayType.LoadString(IDS_QUESTION_TARGET_TITLE);
         else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_TIME_SUPPORT)
            m_csDisplayType.LoadString(IDS_QUESTION_TIME);
      }
      else if (m_aObjects[0]->GetType() == DrawSdk::IMAGE)
      {
         m_csDisplayType.LoadString(IDS_QUESTION_IMAGE);
      }
      else if (m_aObjects[0]->GetType() == DrawSdk::TARGET)
      {
         m_csDisplayType.LoadString(IDS_QUESTION_TARGET_AREA);
      }
      
      
      if (m_aObjects[0]->GetType() == DrawSdk::TEXT)
      {
         m_csDisplayName = ((DrawSdk::Text *)m_aObjects[0])->GetString();
      }
      else if (m_aObjects[0]->GetType() == DrawSdk::IMAGE)
      {
         m_csDisplayName.LoadString(IDS_QUESTION_IMAGE);
      }
      else if (m_aObjects[0]->GetType() == DrawSdk::TARGET)
      {
         m_csDisplayName.LoadString(IDS_QUESTION_TARGET_AREA);
      }
      else
         m_csDisplayName.Empty();
   }  
}

#include "stdafx.h"
#include "..\Studio\resource.h"
#include "ChangeableObject.h"

IMPLEMENT_DYNCREATE(CChangeableObject, CInteractionAreaEx)

CChangeableObject::CChangeableObject()
{
   m_pObject = NULL;
}

CChangeableObject::~CChangeableObject()
{
   if (m_pObject != NULL)
      delete m_pObject;
}


HRESULT CChangeableObject::Init(CRect *prcSize, CTimePeriod *pVisibility, CEditorProject *pProject, 
                                DrawSdk::DrawObject *pObject)
{
   if (prcSize == NULL || pVisibility == NULL || pObject == NULL)
      return E_POINTER;
   
   if (pVisibility->GetPeriodId() == INTERACTION_PERIOD_NEVER)
      return E_IAX_INVALID_DATA;

   if (pObject->GetType() != DrawSdk::TEXT)
      return E_INVALIDARG;

   // TODO maybe even check for PERIOD_PAGE only

   m_pObject = pObject->Copy();

   HRESULT hr = CInteractionAreaEx::Init(prcSize, false, 
      NULL, NULL, NULL, NULL, NULL, pVisibility, pVisibility, pProject);
   
   CreateNameAndType();
   
   return hr;
}

HRESULT CChangeableObject::Init(DrawSdk::DrawObject *pObject, CEditorProject *pProject)
{
   if (pObject == NULL || pProject == NULL)
      return E_POINTER;

   if (pObject->GetType() != DrawSdk::TEXT)
      return E_INVALIDARG;

   CRect rcSize;
   pObject->GetLogicalBoundingBox(&rcSize);

   //pObject->ResetVisibility();

   CTimePeriod periodVisible;
   CString csVisibleDefine = pObject->GetVisibility();
   periodVisible.Parse(csVisibleDefine);
   
   HRESULT hr = Init(&rcSize, &periodVisible, pProject, pObject);
   
   CreateNameAndType();

   return hr;
}

InteractionClassId CChangeableObject::GetClassId()
{
   return INTERACTION_CLASS_CHANGE;
}


CInteractionAreaEx* CChangeableObject::MakeNew()
{
   return new CChangeableObject();
}

// protected
HRESULT CChangeableObject::CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey)
{
   HRESULT hr = S_OK;

   hr = CInteractionAreaEx::CloneTo(pTarget, bCopyHashKey);

   if (SUCCEEDED(hr))
   {
      if (pTarget->GetClassId() != GetClassId())
         return E_INVALIDARG;

      CChangeableObject *pOther = (CChangeableObject *)pTarget;

      if (pOther->m_pObject != NULL)
         delete pOther->m_pObject;

      pOther->m_pObject = m_pObject->Copy();      
   }
   
   return hr;
}

HRESULT CChangeableObject::Move(float fDiffX, float fDiffY)
{
   HRESULT hr = CInteractionAreaEx::Move(fDiffX, fDiffY);
   m_pObject->Move(fDiffX, fDiffY);

   return hr;
}

void CChangeableObject::Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing)
{
   if (!bPreviewing && m_bHiddenDuringEdit || bPreviewing && m_bHiddenDuringPreview)
      return;

   if (m_pObject != NULL)
      m_pObject->Draw(pDC->m_hDC, (float)ptOffsets->x, (float)ptOffsets->y, pZoomManager);
   
   
   if (!bPreviewing && IsSelected())
   {
      DrawBorder(pDC, ptOffsets, pZoomManager);
   }
}

void CChangeableObject::DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager)
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

HRESULT CChangeableObject::SetInteractionName(CString &csName, bool bChangeText)
{
   return CInteractionAreaEx::SetInteractionName(csName, false);
}

HRESULT CChangeableObject::Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
      LPCTSTR tszExportPrefix, CEditorProject *pProject)
{
   // TODO proper and general (done at one place/object) indentation

   HRESULT hr = S_FALSE;

   if (pOutput == NULL)
      return E_POINTER;

   if (m_pObject != NULL)
   {
      if (m_pObject->GetType() != DrawSdk::TEXT)
         return E_NOTIMPL;

      // TODO this side effect on m_objectCounter is very bad style.
      m_pObject->SetObjectNumber(CWhiteboardStream::m_objectCounter++);

      // make it correspond with the "container" object
      static _TCHAR tszTimes[200];
      tszTimes[0] = 0;
      m_Visibility.AppendTimes(tszTimes);
      m_pObject->SetVisibility(tszTimes);

      if (SUCCEEDED(hr))
         hr = pOutput->Append(m_pObject, bUseFullImagePath);
   }

   return hr;
}

// private
void CChangeableObject::CreateNameAndType()
{
   m_csDisplayName = ((DrawSdk::Text *)m_pObject)->GetString();
   
   DrawSdk::Text *pText = (DrawSdk::Text *)m_pObject;
   if (pText->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_TIME)
      m_csDisplayType.LoadString(IDS_CHANGEABLE_TIME);
   else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_NUMBER)
      m_csDisplayType.LoadString(IDS_CHANGEABLE_PROGRESS);
   else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_TRIES)
      m_csDisplayType.LoadString(IDS_CHANGEABLE_TRIES);
   else
      m_csDisplayType.Empty();
}

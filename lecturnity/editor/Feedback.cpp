#include "stdafx.h"               
#include "Feedback.h"
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

IMPLEMENT_DYNCREATE(CFeedback, CMoveableObjects)

#define MAX(a, b) (a > b) ? a : b
#define MIN(a, b) (a < b) ? a : b

CFeedback::CFeedback()
{
   m_idType = FEEDBACK_TYPE_NOTHING;
   m_bIsInherited = false;
   m_bIsDisabled = false;
   m_aObjects.SetSize(0, 8);
   m_pOkButton = NULL;
   m_nInteractionObjectHash = 0;

   m_bPassed = true;
}

CFeedback::~CFeedback()
{
   for (int i=0; i<m_aObjects.GetSize(); ++i)
      delete m_aObjects[i];

   if (m_pOkButton != NULL)
      delete m_pOkButton;
}


HRESULT CFeedback::Init(CFeedbackDefinition *pDefinition)
{
   if (pDefinition == NULL)
      return E_POINTER;

   CString csType = pDefinition->GetType();
   if (StringManipulation::StartsWith(csType, CString(_T("q-correct"))))
      m_idType = FEEDBACK_TYPE_Q_CORRECT;
   else if (StringManipulation::StartsWith(csType, CString(_T("q-wrong"))))
      m_idType = FEEDBACK_TYPE_Q_WRONG;
   else if (StringManipulation::StartsWith(csType, CString(_T("q-repeat"))))
      m_idType = FEEDBACK_TYPE_Q_REPEAT;
   else if (StringManipulation::StartsWith(csType, CString(_T("q-time"))))
      m_idType = FEEDBACK_TYPE_Q_TIME;
   else if (StringManipulation::StartsWith(csType, CString(_T("q-tries"))))
      m_idType = FEEDBACK_TYPE_Q_TRIES;
   else if (StringManipulation::StartsWith(csType, CString(_T("qq-passed"))))
      m_idType = FEEDBACK_TYPE_QQ_PASSED;
   else if (StringManipulation::StartsWith(csType, CString(_T("qq-failed"))))
      m_idType = FEEDBACK_TYPE_QQ_FAILED;
   else if (StringManipulation::StartsWith(csType, CString(_T("qq-eval"))))
      m_idType = FEEDBACK_TYPE_QQ_EVAL;
   else
      return E_INVALIDARG;

   m_bIsDisabled = pDefinition->IsDisabled();
   m_csText = pDefinition->GetText(); // can be empty
      
   if (!m_bIsDisabled)
   {
      m_bIsInherited = pDefinition->IsInherited();
      if (!m_bIsInherited)
      {
         pDefinition->ExtractObjects(&m_aObjects);

         // m_aObjects can be empty, eg for question feedback in the test/questionnaire (storing texts)
         //if (m_aObjects.GetSize() <= 0)
         //   return E_INVALIDARG; // there must be some objects in this case

         CInteractionArea *pOldArea = pDefinition->ExtractOkButton();
         if (pOldArea != NULL)
         {
            m_pOkButton = new CInteractionAreaEx();
            HRESULT hr = m_pOkButton->Init(pOldArea, NULL, 0);
            // Note: CEditorDoc* and nInsertPositionMs hopefully (?) not necessary for this button.
            
            if (FAILED(hr))
               delete m_pOkButton; // ignore error
            
            delete pOldArea;
         }
      }
   }

   // calculate bounding box from objects
   
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjectsDummy;
   aObjectsDummy.Append(m_aObjects);
   
   if (m_pOkButton != NULL)
      aObjectsDummy.Append(*(m_pOkButton->GetNormalObjects()));
   
   CalculateDimensions(&aObjectsDummy, &m_rcDimensions);
   // TODO (inconsistency): no "+2" here (see CMoveableObjects); which is better?
   
   m_bHiddenDuringEdit = true;
   m_bHiddenDuringPreview = true;

   CreateNameAndType();

   return S_OK;
}

HRESULT CFeedback::Init(FeedbackTypeId idType, bool bIsDisabled, bool bIsInherited)
{
   m_idType = idType;
   m_bIsDisabled = bIsDisabled;
   m_bIsInherited = bIsInherited;

   // the other objects here remain empty

   m_bHiddenDuringEdit = true;
   m_bHiddenDuringPreview = true;

   CreateNameAndType();

   return S_OK;
}

HRESULT CFeedback::Init(FeedbackTypeId idType, bool bIsDisabled, bool bIsInherited, CString &csText,
                        CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *pObjects, 
                        CInteractionAreaEx *pOkButton)
{
   HRESULT hr = Init(idType, bIsDisabled, bIsInherited);

   if (SUCCEEDED(hr))
   {
      m_csText = csText;
      
      if (!bIsDisabled && !bIsInherited)
      {
         // Note: more or less any combination of parameters is allowed here.
         // For example: bIsDisabled == true and pObjects->GetSize() > 0 is possible.
         // But this case here is an error.
      
         if (pObjects == NULL) // TODO require? || pOkButton == NULL)
            return E_POINTER;
      }
      
      //if (bIsInherited && pObjects != NULL && pObjects->GetSize() > 0)
      //   _ASSERT(false);

      if (pObjects != NULL)
      {
         for (int i=0; i<pObjects->GetSize(); ++i)
            m_aObjects.Add(pObjects->GetAt(i)->Copy());
      }
      
      if (pOkButton != NULL)
         m_pOkButton = pOkButton;


      // calculate bounding box from objects
     
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjectsDummy;
      aObjectsDummy.Append(m_aObjects);

      if (pOkButton != NULL)
         aObjectsDummy.Append(*(pOkButton->GetNormalObjects()));
      
      CalculateDimensions(&aObjectsDummy, &m_rcDimensions);
      // TODO (inconsistency): no "+2" here (see CMoveableObjects); which is better?

      CreateNameAndType();

      m_bHiddenDuringEdit = true;
      m_bHiddenDuringPreview = true;
   }

   return hr;
}

bool CFeedback::IsQuestionnaireOnly()
{
   if (m_idType == FEEDBACK_TYPE_QQ_PASSED
      || m_idType == FEEDBACK_TYPE_QQ_FAILED
      || m_idType == FEEDBACK_TYPE_QQ_EVAL)
      return true;
   else
      return false;
}  

HRESULT CFeedback::ChangeAppearance(CQuestionnaireEx *pQuestionnaire)
{
   HRESULT hr = S_OK;

   if (m_pOkButton != NULL)
   {
      CRect rcOldDimension = m_pOkButton->GetArea();
      hr = m_pOkButton->ChangeAppearance(pQuestionnaire);
      CRect rcNewDimension = m_pOkButton->GetArea();
      if (SUCCEEDED(hr) && rcOldDimension.Width() != rcNewDimension.Width())
      {
         // Re-calculate dimensions
         hr = RecalculateDimensions(&m_rcDimensions);
      }

      return hr;

   }
   else
      return S_FALSE;
}

HRESULT CFeedback::RecalculateDimensions(CRect *prcTotal)
{
   if (prcTotal == NULL)
      return E_POINTER;
   
   prcTotal->SetRectEmpty();

   // Define offsets --> see also CQuestionnaireEx::CreateFeedback(...)
   int OFFSET_X = 20;
   int OFFSET_Y = 5;

   // Calculate bounding box from objects
   // Note: The 2 border/background rectangles must be ignored here, because they 
   // can change their dimensions and must be re-calculated again (sse below)
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjectsDummy;
   aObjectsDummy.Append(m_aObjects);

   // Get dimensions from feedback text
   double dTextX = 0.0;
   double dTextY = 0.0;
   double dTextWidth = 0.0;
   double dTextHeight = 0.0;
   double dTextAscent = 0.0;
   for (int i=0; i<aObjectsDummy.GetSize(); ++i)
   {
      if (aObjectsDummy.GetAt(i)->GetType() == DrawSdk::TEXT)
      {
         dTextX = aObjectsDummy.GetAt(i)->GetX();
         dTextY = aObjectsDummy.GetAt(i)->GetY();
         dTextWidth = aObjectsDummy.GetAt(i)->GetWidth();
         dTextHeight = aObjectsDummy.GetAt(i)->GetHeight();

         LOGFONT lf;
         ((DrawSdk::Text *)aObjectsDummy.GetAt(i))->GetLogFont(&lf);
         dTextAscent = ((DrawSdk::Text *)aObjectsDummy.GetAt(i))->GetTextAscent(&lf);
      }
   }

   // Get dimensions from feedback button
   double dOkButtonX = 0.0;
   double dOkButtonY = 0.0;
   double dOkButtonWidth = 0.0;
   double dOkButtonHeight = 0.0;
   if (m_pOkButton != NULL)
   {
      dOkButtonX = (double)m_pOkButton->GetArea().left;
      dOkButtonY = (double)m_pOkButton->GetArea().top;
      dOkButtonWidth = (double)m_pOkButton->GetArea().Width();
      dOkButtonHeight = (double)m_pOkButton->GetArea().Height();
   }

   // Calculate the new dimension of the feedback
   double dMin = MIN((int)dTextX, (int)dOkButtonX);
   double rcX1 = dMin - OFFSET_X;
   double rcY1 = dTextY - (dTextHeight+dTextAscent);
   double dMax = MAX((int)dTextWidth, (int)dOkButtonWidth);
   double rcX2 = rcX1 + dMax + 2*OFFSET_X;
   double rcY2 = dOkButtonY + dOkButtonHeight + OFFSET_Y;

   // Set the new feedback dimension
   prcTotal->SetRect((int)rcX1, (int)rcY1, (int)rcX2, (int)rcY2);

   // Resize the 2 border/background rectangles
   HRESULT hr = ResizeBorderRectangles(*prcTotal);

   return hr;
}

HRESULT CFeedback::ResizeBorderRectangles(CRect rcNewDimension)
{
   for (int i=0; i<m_aObjects.GetSize(); ++i)
   {
      if (m_aObjects.GetAt(i)->GetType() == DrawSdk::RECTANGLE)
      {
         m_aObjects.GetAt(i)->SetX(rcNewDimension.left);
         m_aObjects.GetAt(i)->SetY(rcNewDimension.top);
         m_aObjects.GetAt(i)->SetWidth(rcNewDimension.Width());
         m_aObjects.GetAt(i)->SetHeight(rcNewDimension.Height());
      }
   }
   
   return S_OK;
}

HRESULT CFeedback::SetVisibility(CTimePeriod *pPageTime)
{
   if (pPageTime == NULL)
      return E_POINTER;

   return pPageTime->CloneTo(&m_Visibility);
}

HRESULT CFeedback::Move(float fDiffX, float fDiffY)
{
   for (int i=0; i<m_aObjects.GetSize(); ++i)
   {
      m_aObjects[i]->SetX(m_aObjects[i]->GetX() + fDiffX);
      m_aObjects[i]->SetY(m_aObjects[i]->GetY() + fDiffY);
   }
   
   if (m_pOkButton != NULL)
      m_pOkButton->Move(fDiffX, fDiffY);

   CInteractionAreaEx::Move(fDiffX, fDiffY);

   return S_OK;
}

// protected
HRESULT CFeedback::CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey)
{
   HRESULT hr = S_OK;

   hr = CInteractionAreaEx::CloneTo(pTarget, bCopyHashKey);

   if (SUCCEEDED(hr))
   {
      if (pTarget->GetClassId() != GetClassId())
         return E_INVALIDARG;

      CFeedback *pOther = (CFeedback *)pTarget;

      pOther->m_idType = m_idType;
      pOther->m_bIsInherited = m_bIsInherited;
      pOther->m_bIsDisabled = m_bIsDisabled;
      pOther->m_csText = m_csText;

      pOther->m_aObjects.SetSize(0, m_aObjects.GetSize());
      for (int i=0; i<m_aObjects.GetSize(); ++i)
         pOther->m_aObjects.Add(m_aObjects[i]->Copy());

      if (m_pOkButton != NULL)
         pOther->m_pOkButton = m_pOkButton->Copy();

      pOther->m_nInteractionObjectHash = 0; // the new object is not yet added anywhere

      pOther->m_bPassed = m_bPassed;
   }

   return hr;
}

void CFeedback::Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing)
{
   // TODO regard "text change"
   if (!IsDisabled() && pDC != NULL)
   {
      int iOffsetX = 0;
      int iOffsetY = 0;
      if (ptOffsets != NULL)
      {
         iOffsetX = ptOffsets->x;
         iOffsetY = ptOffsets->y;
      }

      for (int i=0; i<m_aObjects.GetSize(); ++i)
      {
         bool bDoPaint = true;
         
         if (m_aObjects[i]->GetType() == DrawSdk::TEXT)
         {      
            DrawSdk::Text *pText = (DrawSdk::Text *)m_aObjects[i];
            
            if (pText->HasChangeableType())
            {
               if (!m_bPassed && pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_PASSED)
                  bDoPaint = false;
               else if (m_bPassed && pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_FAILED)
                  bDoPaint = false;
            }
         }
         
         if (bDoPaint)
            m_aObjects[i]->Draw(pDC->m_hDC, iOffsetX, iOffsetY, pZoomManager);
         
      }

      if (m_pOkButton != NULL)
      {
         m_pOkButton->Activate(0, true, true);  // they are always active
         m_pOkButton->Draw(pDC, ptOffsets, pZoomManager, true);
      }
   }
}

HRESULT CFeedback::ToFront(CInteractionStream *pStream)
{
   if (pStream == NULL)
      return E_POINTER;

   return pStream->MoveToFront(GetHashKey());
}

HRESULT CFeedback::SetInteractionName(CString &csName, bool bChangeText)
{
   return CInteractionAreaEx::SetInteractionName(csName, false);
}

HRESULT CFeedback::Write(CFileOutput *pOutput, UINT nIdentLength, bool bUseFullImagePath, 
                         CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
                         LPCTSTR tszExportPrefix, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (pOutput == NULL)
      return E_POINTER;

   _TCHAR *tszType = NULL;

   if (m_idType == FEEDBACK_TYPE_Q_CORRECT)
      tszType = _T("q-correct");
   else if (m_idType == FEEDBACK_TYPE_Q_WRONG)
      tszType = _T("q-wrong");
   else if (m_idType == FEEDBACK_TYPE_Q_REPEAT)
      tszType = _T("q-repeat");
   else if (m_idType == FEEDBACK_TYPE_Q_TIME)
      tszType = _T("q-time");
   else if (m_idType == FEEDBACK_TYPE_Q_TRIES)
      tszType = _T("q-tries");
   else if (m_idType == FEEDBACK_TYPE_QQ_PASSED)
      tszType = _T("qq-passed");
   else if (m_idType == FEEDBACK_TYPE_QQ_FAILED)
      tszType = _T("qq-failed");
   else if (m_idType == FEEDBACK_TYPE_QQ_EVAL)
      tszType = _T("qq-eval");
   else
      tszType = _T("undefined");

   _TCHAR *tszValue = NULL;
   //const _TCHAR *tszText = _T("");
   //bool bHasText = false;
   
   if (m_bIsDisabled)
      tszValue = _T("disable");
   else
   {
      if (m_bIsInherited)
      {
         if (m_csText.GetLength() > 0)
            tszValue = _T("change-text");
         else
            tszValue = _T("inherit");
      }
      else
         tszValue = _T("enable");
   }

   CString csFeedbackText;
   StringManipulation::TransformForSgml(m_csText, csFeedbackText);

   _TCHAR tszOutput[2000];
   _stprintf(tszOutput, _T("<feedback type=\"%s\" value=\"%s\" text=\"%s\">"), tszType, tszValue, csFeedbackText);
   
   if(SUCCEEDED(hr))
   {
      if (!m_bIsDisabled && !m_bIsInherited)
      {
         _tcscat(tszOutput, _T("\n"));
         hr = pOutput->Append(tszOutput);
         
         DrawSdk::CharArray array;
         for (int i=0; i<m_aObjects.GetSize() && SUCCEEDED(hr); ++i)
         {
            m_aObjects[i]->Write(&array, bUseFullImagePath);
            hr = pOutput->Append(array.GetBuffer(), array.GetLength());
            array.Reset();
         }

         if (m_pOkButton != NULL)
         {
            m_pOkButton->Write(pOutput, nIdentLength > 0, 
               bUseFullImagePath, mapImagePaths, tszExportPrefix, pProject);
         }
         
         if (SUCCEEDED(hr))
            hr = pOutput->Append(_T("</feedback>\n"));
      }
      else
      {
         _tcscat(tszOutput, _T("</feedback>\n"));
         hr = pOutput->Append(tszOutput);
      }
   }  
  
   return hr;
}

// private
void CFeedback::CreateNameAndType()
{
   m_csDisplayName.Empty();

   for (int i=0; i<m_aObjects.GetSize(); ++i)
   {
      if (m_aObjects[i]->GetType() == DrawSdk::TEXT)
      {
         m_csDisplayName = ((DrawSdk::Text *)m_aObjects[i])->GetString();
         break;
      }
   }

   if (m_idType == FEEDBACK_TYPE_Q_CORRECT)
      m_csDisplayType.LoadString(IDS_TYPE_CORRECT);
   else if (m_idType == FEEDBACK_TYPE_Q_WRONG)
      m_csDisplayType.LoadString(IDS_TYPE_WRONG);
   else if (m_idType == FEEDBACK_TYPE_Q_REPEAT)
      m_csDisplayType.LoadString(IDS_TYPE_TRIES);
   else if (m_idType == FEEDBACK_TYPE_Q_TIME)
      m_csDisplayType.LoadString(IDS_TYPE_TIME);
   else if (m_idType == FEEDBACK_TYPE_QQ_PASSED)
      m_csDisplayType.LoadString(IDS_TYPE_PASSED);
   else if (m_idType == FEEDBACK_TYPE_QQ_FAILED)
      m_csDisplayType.LoadString(IDS_TYPE_FAILED);
   else if (m_idType == FEEDBACK_TYPE_QQ_EVAL)
      m_csDisplayType.LoadString(IDS_TYPE_SUMMARY);
   else
      m_csDisplayType.Empty();
}
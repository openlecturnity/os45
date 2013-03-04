#include "stdafx.h"
#include "QuestionEx.h"
#include "ChangeableObject.h"
#include "EditorDoc.h"
#include "QuestionnaireEx.h"
#include "InteractionStream.h"
#include "lutils.h"

IMPLEMENT_DYNCREATE(CQuestionEx, CObject)

CQuestionEx::CQuestionEx()
{
   m_idType = QUESTION_TYPE_NOTHING;
   m_pCorrectAction = NULL;
   m_pFailureAction = NULL;
   m_bJumpsResolved = false;

   m_nAchievablePoints = 0;
   m_nMaximumTries = 0;
   m_nAvailableSeconds = 0;

   m_pInteractionStream = NULL;
   m_aObjectsToCommit.SetSize(0, 40);
   m_pBelongsTo = NULL;
   
   static UINT s_nCounter = 0;
   m_nJoinId = ++s_nCounter;

   m_idDndAlignment = QUESTION_DND_ALIGNMENT_UNDEFINED;

   // initialize test engine values
   ResetAfterPreview();
}

CQuestionEx::~CQuestionEx()
{
   if (m_pCorrectAction != NULL)
      delete m_pCorrectAction;

   if (m_pFailureAction != NULL)
      delete m_pFailureAction;
   
   // the feedback and normal objects are deleted by the CInteractionStream

   if (m_aObjectsToCommit.GetSize() > 0)
   { 
      // there shoudln't be any elements left
      _ASSERT(false);

      for (int i=0; i<m_aObjectsToCommit.GetSize(); ++i)
         delete m_aObjectsToCommit[i];
   }
   

   // TODO what about m_pBelongsTo->UnregisterQuestion(this) here?
   // Probably you cannot know if m_pBelongsTo is still valid/existing.
}

HRESULT CQuestionEx::Init(QuestionTypeId idType, CString& csTitle, CString& csQuestion, CString& csScormId,
                          CTimePeriod *pPageVisibility, CTimePeriod *pObjectVisibility, CTimePeriod *pObjectActivity,
                          CMouseAction *pCorrectAction, CMouseAction *pFailureAction,
                          UINT nAchievablePoints, UINT nMaximumTries, UINT nAvailableTimeMs,
                          CEditorProject *pProject, //CArray<CInteractionAreaEx *, CInteractionAreaEx *> *pObjects,
                          CArray<CFeedback *, CFeedback *> *paSpecificFeedback)
{
   HRESULT hr = S_OK;

   if (pProject == NULL || pPageVisibility == NULL || pObjectVisibility == NULL || pObjectActivity == NULL)
      return E_POINTER;

   // check times: page times must be a superset of object times; and they may be at most one page

   if (pPageVisibility->GetPeriodId() != INTERACTION_PERIOD_PAGE)
      return E_INVALIDARG;

   if (!pObjectVisibility->EntirelyWithin(pPageVisibility))
      return E_INVALIDARG;

   if (!pObjectActivity->EntirelyWithin(pPageVisibility))
      return E_INVALIDARG;

   m_idType = idType;
   m_csTitle = csTitle;
   m_csQuestion = csQuestion;
   m_csScormId = csScormId;

   pPageVisibility->CloneTo(&m_PageVisibility);
   pObjectVisibility->CloneTo(&m_ObjectVisibility);
   pObjectActivity->CloneTo(&m_ObjectActivity);

   if (pCorrectAction != NULL)
      m_pCorrectAction = pCorrectAction->Copy();

   if (pFailureAction != NULL)
      m_pFailureAction = pFailureAction->Copy();

   m_nAchievablePoints = nAchievablePoints;
   m_nMaximumTries = nMaximumTries;
   m_nAvailableSeconds = nAvailableTimeMs/1000;


   CInteractionStream *pInteractionStream = pProject->GetInteractionStream(true);
   m_pInteractionStream = pInteractionStream;


   for (int i=0; i<paSpecificFeedback->GetSize() && SUCCEEDED(hr); ++i)
   {
      CFeedback *pFeedback = paSpecificFeedback->GetAt(i);

      if (pFeedback->IsQuestionnaireOnly())
      {
         _ASSERT(false);
         // not allowed for questions
         hr = E_INVALIDARG;
      }

      if (SUCCEEDED(hr))
         hr = pFeedback->SetVisibility(&m_PageVisibility);

      if (SUCCEEDED(hr))
         m_aObjectsToCommit.Add(pFeedback);

   }


   return hr;
}

HRESULT CQuestionEx::Init(CQuestion *pQuestion, UINT nStreamToMs, CQuestionnaireEx *pBelongsTo, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (pProject == NULL || pQuestion == NULL || pBelongsTo == NULL)
      return E_POINTER;

   CString csPageVisibility = pQuestion->GetPageVisibility();
   CString csObjectVisibility = pQuestion->GetObjectVisibility();
   CString csObjectActivity = pQuestion->GetObjectActivity();

   if (csPageVisibility.IsEmpty() || csObjectVisibility.IsEmpty() || csObjectActivity.IsEmpty())
      return E_QUE_INVALID_DATA;

   m_pBelongsTo = pBelongsTo;

   // TODO do these need to be defined?
   m_csTitle = pQuestion->GetTitle();
   m_csQuestion = pQuestion->GetQuestion();
   m_csScormId = pQuestion->GetScormId();

   CString csType = pQuestion->GetType();
   if (StringManipulation::StartsWith(csType, CString(_T("choice"))))
      m_idType = QUESTION_TYPE_CHOICE;
   else if (StringManipulation::StartsWith(csType, CString(_T("textgap"))))
      m_idType = QUESTION_TYPE_TEXTGAP;
   else if (StringManipulation::StartsWith(csType, CString(_T("dragndrop"))))
   {
      m_idType = QUESTION_TYPE_DRAGNDROP;

      if (pQuestion->GetDndAlignment().Compare(_T("horizontal")) == 0)
         m_idDndAlignment = QUESTION_DND_ALIGNMENT_HORIZONTAL;
      else
         m_idDndAlignment = QUESTION_DND_ALIGNMENT_VERTICAL;
   }
   else
      return E_QUE_INVALID_DATA; // it needs a valid type
   
   m_nAchievablePoints = pQuestion->GetAchievablePoints();
   m_nMaximumTries = pQuestion->GetMaximumAttempts();
   m_nAvailableSeconds = pQuestion->GetMaximumTime();

   CInteractionStream *pInteractionStream = pProject->GetInteractionStream(true);
   m_pInteractionStream = pInteractionStream;

   CString csCorrectAction = pQuestion->GetSuccessAction();
   CString csFailureAction = pQuestion->GetFailureAction();

   if (SUCCEEDED(hr))
      hr = m_PageVisibility.Parse(csPageVisibility);
   if (SUCCEEDED(hr))
      hr = m_ObjectVisibility.Parse(csObjectVisibility);
   if (SUCCEEDED(hr))
      hr = m_ObjectActivity.Parse(csObjectActivity);

   if (m_PageVisibility.GetPeriodId() != INTERACTION_PERIOD_PAGE)
      return E_QUE_INVALID_DATA;

   if (!m_ObjectVisibility.EntirelyWithin(&m_PageVisibility))
      return E_QUE_INVALID_DATA;

   if (!m_ObjectActivity.EntirelyWithin(&m_PageVisibility))
      return E_QUE_INVALID_DATA;

   
   if (SUCCEEDED(hr) && !csCorrectAction.IsEmpty())
   {
      m_pCorrectAction = new CMouseAction();
      hr = m_pCorrectAction->Parse(csCorrectAction, pProject, nStreamToMs);
   }

   if (SUCCEEDED(hr) && !csFailureAction.IsEmpty())
   {
      m_pFailureAction = new CMouseAction();
      hr = m_pFailureAction->Parse(csFailureAction, pProject, nStreamToMs);
   }

   if (SUCCEEDED(hr))
   {
      CArray<CFeedbackDefinition *, CFeedbackDefinition *> aFeedback;
      hr = pQuestion->ExtractFeedback(&aFeedback);

      for (int i=0; i<aFeedback.GetSize() && SUCCEEDED(hr); ++i)
      {
         CFeedback *pFeedback = new CFeedback();
         hr = pFeedback->Init(aFeedback[i]);
      
         if (SUCCEEDED(hr))
            hr = pFeedback->SetVisibility(&m_PageVisibility);

         if (SUCCEEDED(hr) && nStreamToMs > 0)
            hr = pFeedback->IncrementTimes(nStreamToMs);

         if (SUCCEEDED(hr))
            hr = pInteractionStream->InsertSingle(pFeedback, false);

         // Note insertion done directly: there will be made a copy of this question during InsertRange()
         // and m_aObjectsToCommit is not (yet?) copied.
         //if (SUCCEEDED(hr))
         //   m_aObjectsToCommit.Add(pFeedback);

         
         delete aFeedback[i];
      }
   }

   
   return hr;
}

CQuestionEx* CQuestionEx::Copy(bool bIncludeJoinId)
{
   CQuestionEx *pCopy = new CQuestionEx();
   HRESULT hr = CloneTo(pCopy, bIncludeJoinId);

   if (SUCCEEDED(hr))
      return pCopy;
   else
   {
      delete pCopy;
      return NULL;
   }
}

// private
HRESULT CQuestionEx::CloneTo(CQuestionEx *pOther, bool bIncludeJoinId)
{
   HRESULT hr = S_OK;

   if (pOther == NULL)
      return E_POINTER;

   pOther->m_idType = m_idType;
   pOther->m_csTitle = m_csTitle;
   pOther->m_csQuestion = m_csQuestion;
   pOther->m_csScormId = m_csScormId;

   m_PageVisibility.CloneTo(&pOther->m_PageVisibility);
   m_ObjectVisibility.CloneTo(&pOther->m_ObjectVisibility);
   m_ObjectActivity.CloneTo(&pOther->m_ObjectActivity);

   if (m_pCorrectAction != NULL)
   {
      pOther->m_pCorrectAction = new CMouseAction();
      m_pCorrectAction->CloneTo(pOther->m_pCorrectAction);
   }

   if (m_pFailureAction != NULL)
   {
      pOther->m_pFailureAction = new CMouseAction();
      m_pFailureAction->CloneTo(pOther->m_pFailureAction);
   }

   pOther->m_nAchievablePoints = m_nAchievablePoints;
   pOther->m_nMaximumTries = m_nMaximumTries;
   pOther->m_nAvailableSeconds = m_nAvailableSeconds;
  
   pOther->m_pBelongsTo = m_pBelongsTo;

   if (bIncludeJoinId)
      pOther->m_nJoinId = m_nJoinId;

   pOther->m_pInteractionStream = m_pInteractionStream;
   pOther->m_idDndAlignment = m_idDndAlignment;

   return hr;

}

bool CQuestionEx::HasDeleteButtonEnabled() {
    CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
    aObjects.SetSize(0, 40);
    HRESULT hr = GetObjectPointers(&aObjects);
    if (SUCCEEDED(hr)) {
        for (int i=0; i<aObjects.GetSize(); ++i) {
            if (aObjects[i]->GetButtonType() == BUTTON_TYPE_DELETE)
                return true;
        }
    }
    return false;
}

HRESULT CQuestionEx::ResolveJumpTimes(CEditorProject *pProject, UINT nMoveMs)
{
   HRESULT hr = S_OK;

   if (pProject == NULL)
      return E_POINTER;
   
   if (!m_bJumpsResolved)
   {
      if (m_pCorrectAction != NULL)
      {
         hr = m_pCorrectAction->ResolveJumpTimes(pProject, nMoveMs);
      }
      
      if (m_pFailureAction != NULL && SUCCEEDED(hr))
      {
         hr = m_pFailureAction->ResolveJumpTimes(pProject, nMoveMs);
      }

      m_bJumpsResolved = true;
      
      // TODO how about mark registering (see CInteractionAreaEx)?
   }

   return hr;
}

HRESULT CQuestionEx::SetBelongsTo(CQuestionnaireEx *pBelongsTo)
{ 
   if (pBelongsTo == NULL)
      return E_POINTER;

   m_pBelongsTo = pBelongsTo;

   return S_OK;
}

CFeedback* CQuestionEx::GetFeedback(FeedbackTypeId idType, bool bGetSuper)
{
   CArray<CFeedback *, CFeedback *> aFeedback;
   GetAllFeedback(&aFeedback);

   CFeedback *pFeedback = NULL;
   for (int i=0; i<aFeedback.GetSize(); ++i)
   {
      if (aFeedback[i]->GetType() == idType)
      {
         pFeedback = aFeedback[i];
         break;
      }
   }

   if ((pFeedback == NULL || pFeedback->IsInherited()) && m_pBelongsTo != NULL && bGetSuper)
      return m_pBelongsTo->GetFeedback(idType);
   else
      return pFeedback;
}

bool CQuestionEx::PageContains(UINT nPositionMs)
{
   return m_PageVisibility.Contains(nPositionMs);
}

bool CQuestionEx::Intersects(CQuestionEx *pOther)
{
   if (pOther == NULL)
      return false;

   return m_PageVisibility.IntersectsRange(&pOther->m_PageVisibility);
}
   

bool CQuestionEx::BeginsAtOrAfter(UINT nPositionMs)
{
   return m_PageVisibility.GetBegin() >= nPositionMs;
}

HRESULT CQuestionEx::IncrementTimes(UINT nDiffMs)
{
   HRESULT hr = m_PageVisibility.IncrementBegin(nDiffMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectVisibility.IncrementBegin(nDiffMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectActivity.IncrementBegin(nDiffMs);

   return hr;
}

HRESULT CQuestionEx::DecrementTimes(UINT nDiffMs)
{
   HRESULT hr = m_PageVisibility.DecrementBegin(nDiffMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectVisibility.DecrementBegin(nDiffMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectActivity.DecrementBegin(nDiffMs);

   return hr;
}

HRESULT CQuestionEx::TrimToRange(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = m_PageVisibility.TrimTo(nFromMs, nLengthMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectVisibility.TrimTo(nFromMs, nLengthMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectActivity.TrimTo(nFromMs, nLengthMs);

   return hr;
}

HRESULT CQuestionEx::RemovePart(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = m_PageVisibility.Subtract(nFromMs, nLengthMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectVisibility.Subtract(nFromMs, nLengthMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectActivity.Subtract(nFromMs, nLengthMs);

   return hr;
}

bool CQuestionEx::MeetsWith(CQuestionEx *pOther, UINT nMoveMs)
{
   if (pOther == NULL)
      return false;

   if (pOther->m_nJoinId != m_nJoinId)
      return false;

   UINT nOtherStartMs = pOther->GetPageBegin() + nMoveMs;
   UINT nOtherLengthMs = pOther->GetPageLength();

   bool bIntersects = m_PageVisibility.IntersectsRange(nOtherStartMs, nOtherLengthMs);
   if (!bIntersects)
      bIntersects = GetPageBegin() == nOtherStartMs + nOtherLengthMs || GetPageBegin() + GetPageLength() == nOtherStartMs;

   return bIntersects;
}

HRESULT CQuestionEx::Enlarge(CQuestionEx *pOther, UINT nMoveMs)
{
   HRESULT hr = S_OK;

   if (pOther == NULL)
      return E_POINTER;

   //if (!MeetsWith(pOther, nMoveMs))
   //   return E_INVALIDARG;

   UINT nOtherStartMs = pOther->GetPageBegin() + nMoveMs;
   UINT nOtherLengthMs = pOther->GetPageLength();

   return Enlarge(nOtherStartMs, nOtherLengthMs);
}

HRESULT CQuestionEx::Enlarge(UINT nOtherStartMs, UINT nOtherLengthMs)
{
   HRESULT hr = S_OK;
   
   bool bMeets = m_PageVisibility.IntersectsRange(nOtherStartMs, nOtherLengthMs);
   if (!bMeets)
      bMeets = nOtherStartMs + nOtherLengthMs == GetPageBegin();
   if (!bMeets)
      bMeets = GetPageBegin() + GetPageLength() == nOtherStartMs;
   
   if (!bMeets)
      return E_INVALIDARG;

   /* solution not sufficient
   // make all the question objects longer; they are ignored in CInteractionStream in this case (implicitly)
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aQuestionObjects;
   aQuestionObjects.SetSize(0, 20);
   hr = GetObjectPointers(&aQuestionObjects);

   for (int i=0; i<aQuestionObjects.GetSize() && SUCCEEDED(hr); ++i)
   {
      CInteractionAreaEx *pCopyArea = aQuestionObjects[i]->Copy(true);
      
      if (pCopyArea != NULL)
      {
         hr = pCopyArea->Enlarge(
            pOther->m_PageVisibility.GetBegin() + nMoveMs, pOther->m_PageVisibility.GetLength());

         if (SUCCEEDED(hr))
            m_pInteractionStream->ModifyArea(aQuestionObjects[i]->GetHashKey(), pCopyArea, false);
      }
      else
         hr = E_FAIL;
   }
   */

   if (SUCCEEDED(hr))
      hr = m_PageVisibility.EnlargeWith(nOtherStartMs, nOtherLengthMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectVisibility.EnlargeWith(nOtherStartMs, nOtherLengthMs);
   if (SUCCEEDED(hr))
      hr = m_ObjectActivity.EnlargeWith(nOtherStartMs, nOtherLengthMs);

   return hr;
}
   

CQuestionEx *CQuestionEx::Split(UINT nSplitMs)
{
   if (!m_PageVisibility.Contains(nSplitMs))
      return NULL;

   if (nSplitMs == GetPageBegin())
      return NULL;

   CQuestionEx *pCopyQuestion = Copy();
   if (pCopyQuestion != NULL)
   {
      UINT nNewLength1 = nSplitMs - GetPageBegin();
      UINT nNewLength2 = GetPageLength() - nNewLength1;

      HRESULT hr = TrimToRange(GetPageBegin(), nNewLength1);
      if (SUCCEEDED(hr))
         hr = pCopyQuestion->TrimToRange(nSplitMs, nNewLength2);

      if (FAILED(hr))
      {
         delete pCopyQuestion;
         pCopyQuestion = NULL;
      }
   }

   return pCopyQuestion;
}


HRESULT CQuestionEx::GetObjectPointers(CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paObjects)
{
   HRESULT hr = S_OK;

   if (paObjects == NULL)
      return E_POINTER;
   
   if (m_pInteractionStream == NULL)
      return E_FAIL;

   hr = m_pInteractionStream->FindInteractions(&m_ObjectVisibility, true, paObjects);

   if (SUCCEEDED(hr))
   {
      for (int i=paObjects->GetSize() - 1; i>=0; --i)
      {
         if (paObjects->GetAt(i)->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES)
         {
            // it is a questionnaire feedback

            paObjects->RemoveAt(i);
         }
      }
   }

   return hr;
}

bool CQuestionEx::IsMultipleChoice()
{
   if (m_idType != QUESTION_TYPE_CHOICE)
      return false;

   // try to find a radio button
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
   aObjects.SetSize(0, 40);

   HRESULT hr = GetObjectPointers(&aObjects);

   if (SUCCEEDED(hr))
   {
      for (int i=0; i<aObjects.GetSize(); ++i)
      {
         if (aObjects[i]->GetClassId() == INTERACTION_CLASS_DYNAMIC)
         {
            CDynamicObject *pDynamic = (CDynamicObject *)aObjects[i];

            if (pDynamic->GetType() == DYNAMIC_CHECKBOX)
               return true;
            else
               return false;
         }
      }
   }

   return false;
}
 

HRESULT CQuestionEx::AddObject(CInteractionAreaEx *pObject)
{
   HRESULT hr = S_OK;

   if (pObject == NULL)
      return E_POINTER;

   m_aObjectsToCommit.Add(pObject);

   return hr;
}

HRESULT CQuestionEx::CommitObjects()
{
   HRESULT hr = S_OK;

   for (int i=0; i<m_aObjectsToCommit.GetSize() && SUCCEEDED(hr); ++i)
      hr = m_pInteractionStream->InsertSingle(m_aObjectsToCommit[i], false);
   m_aObjectsToCommit.RemoveAll();

   // Note: CQuestionEx does not permanently store pointers to its CInteractionAreaEx.
   // They are handled (also for copy and changes) by CInteractionStream. Handling them
   // here would be double effort.
   // Instead GetObjectPointers() and m_aObjectsToCommit is used.

   return hr;
}
   

HRESULT CQuestionEx::RemoveObjects()
{
   HRESULT hr = S_OK;

   if (m_pInteractionStream == NULL)
      return E_POINTER;

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
   aObjects.SetSize(0, 40);
   hr = GetObjectPointers(&aObjects);

   for (int i=0; i<aObjects.GetSize() && SUCCEEDED(hr); ++i)
      hr = m_pInteractionStream->DeleteSingle(aObjects[i]->GetHashKey(), false);

   return hr;
}


HRESULT CQuestionEx::UpdateButtonAppearance(CQuestionnaireEx *pQuestionnaire, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
   aObjects.SetSize(0, 60);
   hr = GetObjectPointers(&aObjects);

   CArray<CFeedback *, CFeedback *> aFeedback;
   GetAllFeedback(&aFeedback);

   for (int i=0; i<aFeedback.GetSize(); ++i)
   {
      if (aFeedback[i]->GetOkButton() != NULL)
         aObjects.Add(aFeedback[i]);
   }

   CInteractionAreaEx *pSubmitButton = NULL;
   CInteractionAreaEx *pOldSubmitButton = NULL;
   CInteractionAreaEx *pDeleteButton = NULL;
   CInteractionAreaEx *pOldDeleteButton = NULL;
   for (i=0; i<aObjects.GetSize() && SUCCEEDED(hr); ++i)
   {
      if (aObjects[i]->GetClassId() == INTERACTION_CLASS_BUTTON 
         || aObjects[i]->GetClassId() == INTERACTION_CLASS_FEEDBACK)
      {
         CInteractionAreaEx *pNewArea = aObjects[i]->Copy(true);

         hr = pNewArea->ChangeAppearance(pQuestionnaire);

         if (SUCCEEDED(hr))
         {
            if (pNewArea->GetButtonType() == BUTTON_TYPE_SUBMIT)
            {
               pSubmitButton = pNewArea;
               pOldSubmitButton = aObjects[i];
            }
            else if (pNewArea->GetButtonType() == BUTTON_TYPE_DELETE)
            {
               pDeleteButton = pNewArea;
               pOldDeleteButton = aObjects[i];
            }
            else
               hr = m_pInteractionStream->ModifyArea(aObjects[i]->GetHashKey(), pNewArea, false);
         }
      }
   }

   if (pSubmitButton || pDeleteButton)
   {
      CRect rcSubmitButton;
      CRect rcDeleteButton;
      pQuestionnaire->CalculateDefaultButtonPosition(pProject, rcSubmitButton, rcDeleteButton);
      if (pSubmitButton && pOldSubmitButton && !rcSubmitButton.IsRectEmpty())
      {  
         CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects[4];
         paObjects[0] = pSubmitButton->GetInactiveObjects();
         paObjects[1] = pSubmitButton->GetNormalObjects();
         paObjects[2] = pSubmitButton->GetOverObjects();
         paObjects[3] = pSubmitButton->GetPressedObjects();

         for (int i = 0; i < 4; ++i)
         {
            for (int j = 0; j < paObjects[i]->GetSize(); ++j)
            {
               DrawSdk::DrawObject *pObject = paObjects[i]->GetAt(j);
               if (pObject->GetType() == DrawSdk::RECTANGLE)
               {
                  DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
                  pObject->SetX(rcSubmitButton.left);
                  pObject->SetY(rcSubmitButton.top);
                  pObject->SetWidth(rcSubmitButton.Width());
                  pObject->SetHeight(rcSubmitButton.Height());
               }

               if (pObject->GetType() == DrawSdk::IMAGE)
               {
                  DrawSdk::Image *pImage = (DrawSdk::Image *)pObject;
                  pImage->SetX(rcSubmitButton.left);
                  pImage->SetY(rcSubmitButton.top);
                  pImage->SetWidth(rcSubmitButton.Width());
                  pImage->SetHeight(rcSubmitButton.Height());
               }

               if (pObject->GetType() == DrawSdk::TEXT)
               {
                  CString csButtonText = ((DrawSdk::Text *)pObject)->GetString();
                  LOGFONT lf;
                  ((DrawSdk::Text *)pObject)->GetLogFont(&lf);

                  double dTextWidth = DrawSdk::Text::GetTextWidth(csButtonText, csButtonText.GetLength(), &lf);
                  double dTextHeight = DrawSdk::Text::GetTextHeight(csButtonText, csButtonText.GetLength(), &lf);
                  double dAscent = DrawSdk::Text::GetTextAscent(&lf);
                  if (rcSubmitButton.Width() < dTextWidth)
                     dTextWidth = rcSubmitButton.Width();
                  if (rcSubmitButton.Height() < dTextHeight)
                     dTextHeight = rcSubmitButton.Height();
                  double dXPos = rcSubmitButton.left + (rcSubmitButton.Width() - dTextWidth) / 2;
                  double dYPos = rcSubmitButton.top + (rcSubmitButton.Height() - dTextHeight) / 2 + dAscent + 1.5;

                  pObject->SetX(dXPos);
                  pObject->SetY(dYPos);
               }
            }
         }
         pSubmitButton->SetArea(&rcSubmitButton);
         m_pInteractionStream->ModifyArea(pOldSubmitButton->GetHashKey(), pSubmitButton, false);
      }
      if (pDeleteButton && pOldDeleteButton && !rcDeleteButton.IsRectEmpty())
      {
         CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects[4];
         paObjects[0] = pDeleteButton->GetInactiveObjects();
         paObjects[1] = pDeleteButton->GetNormalObjects();
         paObjects[2] = pDeleteButton->GetOverObjects();
         paObjects[3] = pDeleteButton->GetPressedObjects();

         for (int i = 0; i < 4; ++i)
         {
            for (int j = 0; j < paObjects[i]->GetSize(); ++j)
            {
               DrawSdk::DrawObject *pObject = paObjects[i]->GetAt(j);
               if (pObject->GetType() == DrawSdk::RECTANGLE)
               {
                  DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
                  pObject->SetX(rcDeleteButton.left);
                  pObject->SetY(rcDeleteButton.top);
                  pObject->SetWidth(rcDeleteButton.Width());
                  pObject->SetHeight(rcDeleteButton.Height());
               }

               if (pObject->GetType() == DrawSdk::IMAGE)
               {
                  DrawSdk::Image *pImage = (DrawSdk::Image *)pObject;
                  pImage->SetX(rcDeleteButton.left);
                  pImage->SetY(rcDeleteButton.top);
                  pImage->SetWidth(rcDeleteButton.Width());
                  pImage->SetHeight(rcDeleteButton.Height());
               }

               if (pObject->GetType() == DrawSdk::TEXT)
               {
                  CString csButtonText = ((DrawSdk::Text *)pObject)->GetString();
                  LOGFONT lf;
                  ((DrawSdk::Text *)pObject)->GetLogFont(&lf);

                  double dTextWidth = DrawSdk::Text::GetTextWidth(csButtonText, csButtonText.GetLength(), &lf);
                  double dTextHeight = DrawSdk::Text::GetTextHeight(csButtonText, csButtonText.GetLength(), &lf);
                  double dAscent = DrawSdk::Text::GetTextAscent(&lf);
                  if (rcDeleteButton.Width() < dTextWidth)
                     dTextWidth = rcDeleteButton.Width();
                  if (rcDeleteButton.Height() < dTextHeight)
                     dTextHeight = rcDeleteButton.Height();
                  double dXPos = rcDeleteButton.left + (rcDeleteButton.Width() - dTextWidth) / 2;
                  double dYPos = rcDeleteButton.top + (rcDeleteButton.Height() - dTextHeight) / 2 + dAscent + 1.5;

                  pObject->SetX(dXPos);
                  pObject->SetY(dYPos);
               }
            }
         }
         pDeleteButton->SetArea(&rcDeleteButton);
         m_pInteractionStream->ModifyArea(pOldDeleteButton->GetHashKey(), pDeleteButton, false);
      }
         
   }

   return hr;
}


HRESULT CQuestionEx::ChangeQuestionsShowProgress(CEditorProject *pProject, bool bShowProgress)
{
   HRESULT hr = S_OK;

   CInteractionAreaEx *pTimerText = NULL;
   CInteractionAreaEx *pTimerValue = NULL;
   CInteractionAreaEx *pPossibleTriesText = NULL;
   CInteractionAreaEx *pPossibleTriesValue = NULL;
   CInteractionAreaEx *pProgressText = NULL;
   CInteractionAreaEx *pProgressValue = NULL;
   GetQuestionStateInteractions(pTimerText, pTimerValue,
                                pPossibleTriesText, pPossibleTriesValue,
                                pProgressText, pProgressValue);

   

   if (bShowProgress)
   { 
      double dXPos;
      CalculateQuestionStateLeftPosition(pProject, dXPos);
      double dYPos;
      CalculateQuestionStateBottomPosition(pProject, dYPos);

      if (pPossibleTriesText != NULL)
      {
         CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *aDrawObjects;
         aDrawObjects = ((CMoveableObjects *)pPossibleTriesText)->GetObjects();
         DrawSdk::Text *pText = (DrawSdk::Text *)aDrawObjects->GetAt(0);
         dYPos = pText->GetY();
      }
      else if (pTimerText != NULL)
      {
         CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *aDrawObjects;
         aDrawObjects = ((CMoveableObjects *)pTimerText)->GetObjects();
         DrawSdk::Text *pText = (DrawSdk::Text *)aDrawObjects->GetAt(0);
         dYPos = pText->GetY();
      }
      
      CString csProgress;
      csProgress.LoadString(IDS_PROGRESS_TEXT);

      LOGFONT lf;
      GetQuestionStateDefaultFont(lf);
      COLORREF clrText;
      GetQuestionStateDefaultColor(clrText);

      AddProgressText(pProject, m_ObjectVisibility, 
                      dXPos, dYPos, 
                      csProgress, clrText, lf,
                      true);

      AddProgressObject(pProject, m_ObjectVisibility,
                        dXPos, dYPos,
                        clrText, lf,
                        true);
   }
   else if (pProgressText != NULL && pProgressValue != NULL)
   {
      m_pInteractionStream->DeleteSingle(pProgressText->GetHashKey(), false);
      m_pInteractionStream->DeleteSingle(pProgressValue->GetHashKey(), false);
      
   }

   return hr;

}

HRESULT CQuestionEx::GetQuestionStateInteractions(CInteractionAreaEx *&pTimerText, CInteractionAreaEx *&pTimerValue,
                                        CInteractionAreaEx *&pPossibleTriesText, CInteractionAreaEx *&pPossibleTriesValue,
                                        CInteractionAreaEx *&pProgressText, CInteractionAreaEx *&pProgressValue)
{
   HRESULT hr = S_OK;

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
   aObjects.SetSize(0, 60);
   hr = GetObjectPointers(&aObjects);

   for (int i=0; i<aObjects.GetSize() && SUCCEEDED(hr); ++i)
   {
      // PROGRESS text -> MoveableObject + text with type TEXT_TYPE_NUMBER_SUPPORT
      if (aObjects[i]->GetClassId() == INTERACTION_CLASS_MOVE)
      {
         CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *aMoveableObjects;
         aMoveableObjects = ((CMoveableObjects *)aObjects[i])->GetObjects();
         if (aMoveableObjects->GetSize() == 1)
         {
            if (aMoveableObjects->GetAt(0)->GetType() == DrawSdk::TEXT)
            {
               if (((DrawSdk::Text *)aMoveableObjects->GetAt(0))->GetTextType() == DrawSdk::TEXT_TYPE_TIME_SUPPORT)
               {
                  pTimerText = aObjects[i];
               }
               if (((DrawSdk::Text *)aMoveableObjects->GetAt(0))->GetTextType() == DrawSdk::TEXT_TYPE_TRIES_SUPPORT)
               {
                  pPossibleTriesText = aObjects[i];
               }
               if (((DrawSdk::Text *)aMoveableObjects->GetAt(0))->GetTextType() == DrawSdk::TEXT_TYPE_NUMBER_SUPPORT)
               {
                  pProgressText = aObjects[i];
               }
            }
         }
      }

      // PROGRESS -> ChangeableObject + text with type TEXT_TYPE_CHANGE_NUMBER
      if (aObjects[i]->GetClassId() == INTERACTION_CLASS_CHANGE)
      {
         DrawSdk::DrawObject *pObject = ((CChangeableObject *)aObjects[i])->GetObject();
         if (pObject->GetType() == DrawSdk::TEXT)
         {  
            if (((DrawSdk::Text *)pObject)->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_TIME)
            {
               pTimerValue = aObjects[i];
            }
            if (((DrawSdk::Text *)pObject)->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_TRIES)
            {
               pPossibleTriesValue = aObjects[i];
            }
            if (((DrawSdk::Text *)pObject)->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_NUMBER)
            {
               pProgressValue = aObjects[i];
            }
         }
      }

   }
   return hr;
}


/*
HRESULT CQuestionEx::MoveFeedbackToFront()
{
   POSITION pos = m_mapFeedback.GetStartPosition();
   while (pos != NULL)
   {
      FeedbackTypeId idKey;
      CFeedback *pFeedback = NULL;
      m_mapFeedback.GetNextAssoc(pos, idKey, pFeedback);

      if (!pFeedback->IsDisabled() && !pFeedback->IsInherited())
      {
         pFeedback->ToFront(m_pInteractionStream);
      }
   }

   return S_OK;
}
*/


HRESULT CQuestionEx::Write(CFileOutput *pOutput, UINT nIdentLength, bool bUseFullImagePath, 
                           CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
                           LPCTSTR tszExportPrefix, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (pOutput == NULL)
      return E_POINTER;

   _TCHAR *tszType = NULL;
   _TCHAR *tszDndAlignment = _T("");

   if (m_idType == QUESTION_TYPE_CHOICE)
      tszType = _T("choice");
   else if (m_idType == QUESTION_TYPE_TEXTGAP)
      tszType = _T("textgap");
   else if (m_idType == QUESTION_TYPE_DRAGNDROP)
   {
      tszType = _T("dragndrop");

      if (m_idDndAlignment == QUESTION_DND_ALIGNMENT_HORIZONTAL)
         tszDndAlignment = _T(" dnd-align=\"horizontal\"");
      else
         tszDndAlignment = _T(" dnd-align=\"vertical\"");
   }
   else
      tszType = _T("undefined");

   // BUGFIX #3122
   // The question text can have returns (\r\n from the text field).
   // The Player however does not expect returns in a parameter.

   CString csQuestionText = m_csQuestion;
   csQuestionText.Replace(_T("\r\n"), _T("\\\\r\\\\n"));

   // BUGFIX #3970, #3945
   // Also mask <, >, " otherwise they will lead to problems upon re-importing or in the Publisher

   CString csQuestionText2;
   StringManipulation::TransformForSgml(csQuestionText, csQuestionText2);

   CString csTitle;
   StringManipulation::TransformForSgml(m_csTitle, csTitle);

   CString csScormId;
   StringManipulation::TransformForSgml(m_csScormId, csScormId);

   _TCHAR tszOutput[3000];
   _stprintf(tszOutput, 
      _T("<QUESTION type=\"%s\" title=\"%s\" text=\"%s\" id=\"%s\" points=\"%d\" attempts=\"%d\" time=\"%d\"%s"), 
      tszType, csTitle, csQuestionText2, csScormId,
      m_nAchievablePoints, m_nMaximumTries, m_nAvailableSeconds, tszDndAlignment);

   if (SUCCEEDED(hr))
      hr = pOutput->Append(tszOutput);

   if (SUCCEEDED(hr))
   {
      if (m_pCorrectAction != NULL)
      {
         pOutput->Append(_T(" onSuccess=\""));
         m_pCorrectAction->AppendData(pOutput, tszExportPrefix, pProject);
         pOutput->Append(_T("\""));
      }
      
      if (m_pFailureAction != NULL)
      {
         pOutput->Append(_T(" onFailure=\""));
         m_pFailureAction->AppendData(pOutput, tszExportPrefix, pProject);
         pOutput->Append(_T("\""));
      }

      if (m_PageVisibility.GetPeriodId() != INTERACTION_PERIOD_NEVER)
      {
         pOutput->Append(_T(" page=\""));
         m_PageVisibility.AppendTimes(pOutput);
         pOutput->Append(_T("\""));
      }
      
      if (m_ObjectVisibility.GetPeriodId() != INTERACTION_PERIOD_NEVER)
      {
         pOutput->Append(_T(" visible=\""));
         m_ObjectVisibility.AppendTimes(pOutput);
         pOutput->Append(_T("\""));
      }
      
      if (m_ObjectActivity.GetPeriodId() != INTERACTION_PERIOD_NEVER)
      {
         pOutput->Append(_T(" active=\""));
         m_ObjectActivity.AppendTimes(pOutput);
         pOutput->Append(_T("\""));
      }
      
      pOutput->Append(_T(">\n"));
     
      CArray<CFeedback *, CFeedback *> aFeedback;
      GetAllFeedback(&aFeedback);
      
      for (int i=0; i<aFeedback.GetSize(); ++i)
      {
         hr = aFeedback[i]->Write(pOutput, nIdentLength, bUseFullImagePath, mapImagePaths, tszExportPrefix, pProject);
      }
      
      pOutput->Append(_T("</QUESTION>\n"));
   }

   return hr;
}



bool CQuestionEx::AreAttemptsLeft()
{
   if (m_nMaximumTries > 0)
      return m_nTakenAttempts < m_nMaximumTries;
   else
      return true;
}

HRESULT CQuestionEx::UpdateAnswered(bool bQuestionSuccess)
{
   if (AreAttemptsLeft())
      m_bAnswerCorrect = bQuestionSuccess;

   m_nTakenAttempts++;

   return S_OK;
}

HRESULT CQuestionEx::ShowFeedback(FeedbackTypeId idFeedback, CWhiteboardView *pView, bool bWaitForFinish)
{
   if (pView == NULL)
      return E_POINTER;

   if (idFeedback == FEEDBACK_TYPE_Q_TIME)
      m_bTimerMessageShown = true;

   CFeedback *pFeedback = GetFeedback(idFeedback, true);

   if (pFeedback != NULL && !pFeedback->IsDisabled())
      return pView->ShowFeedback(pFeedback, bWaitForFinish);
   else
      return S_FALSE;
}

bool CQuestionEx::IsFeedbackActive(FeedbackTypeId idFeedback)
{
   CFeedback *pFeedback = GetFeedback(idFeedback, true);

   return pFeedback != NULL && !pFeedback->IsDisabled();
}
   

HRESULT CQuestionEx::Disable()
{
   HRESULT hr = S_OK;

   m_bDisabled = true;

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aAreas;
   aAreas.SetSize(0, 60);
   hr = GetObjectPointers(&aAreas);

   for (int i=0; i<aAreas.GetSize() && SUCCEEDED(hr); ++i)
   {
      if (aAreas[i]->GetClassId() != INTERACTION_CLASS_FEEDBACK)
         bool bChange = aAreas[i]->DisableForPreview();
   }

   return hr;
}

HRESULT CQuestionEx::Execute(CEditorDoc *pDoc, bool bQuestionSuccess)
{
   HRESULT hr = S_OK;

   if (bQuestionSuccess && m_pCorrectAction != NULL)
      m_pCorrectAction->Execute(pDoc);

   if (!bQuestionSuccess && m_pFailureAction != NULL)
      m_pFailureAction->Execute(pDoc);

   return hr;
}

HRESULT CQuestionEx::ResetAfterPreview()
{
   m_bAnswerCorrect = false;
   m_nTakenAttempts = 0;
   m_bDisabled = false;
   m_bTimerMessageShown = false;
   m_nUsedSeconds = 0;
   m_nStartSeconds = 0;

   return S_OK;
}

// TODO triple code

bool CQuestionEx::IsTimeUp()
{
   if (IsTimeable())
   {
      UINT nCurrentSeconds = ::timeGetTime()/1000;
      
      if (m_nStartSeconds == 0) // has not been updated yet
         m_nStartSeconds = nCurrentSeconds;

      int iRemainingSeconds = m_nAvailableSeconds - (nCurrentSeconds - m_nStartSeconds) - m_nUsedSeconds;

      if (iRemainingSeconds > 0)
         return false;
      else
         return true;
   }
   else
      return false;
}

int CQuestionEx::UpdateTimer()
{
   if (IsTimeable() && !IsAnsweredCorrect() && IsEnabled())
   {
      UINT nCurrentSeconds = ::timeGetTime()/1000;

      if (m_nStartSeconds == 0)
         m_nStartSeconds = nCurrentSeconds;
      
      int iRemainingSeconds = m_nAvailableSeconds - (nCurrentSeconds - m_nStartSeconds) - m_nUsedSeconds;

      if (iRemainingSeconds > 0)
         return iRemainingSeconds;
      else
         return 0;
   }
   else
      return 0;
}

int CQuestionEx::StopTimer()
{
   if (IsTimeable())
   {
      UINT nCurrentSeconds = ::timeGetTime()/1000;
      
      if (m_nStartSeconds == 0) // has not been update yet; will be set to 0 below
         m_nStartSeconds = nCurrentSeconds;

      int iRemainingSeconds = m_nAvailableSeconds - (nCurrentSeconds - m_nStartSeconds) - m_nUsedSeconds;

      m_nUsedSeconds += nCurrentSeconds - m_nStartSeconds;
      m_nStartSeconds = 0;

      if (iRemainingSeconds > 0)
         return iRemainingSeconds;
      else
         return 0;
   }
   else
      return 0;
}

HRESULT CQuestionEx::UpdateChangeableTexts(int iRemainingSeconds)
{
   HRESULT hr = S_OK;

   int iSeconds = iRemainingSeconds%60;
   int iMinutes = iRemainingSeconds/60;
   _TCHAR tszText[20];
   tszText[0] = 0;
   int iAttempts = m_nMaximumTries - m_nTakenAttempts;
   if (iAttempts < 0)
      iAttempts = 0;

   int iQuestionNumber = 1;
   UINT iQuestionTotal = 1;
   if (m_pBelongsTo != NULL)
   {
      iQuestionNumber = m_pBelongsTo->GetQuestionNumber(this) + 1;
      iQuestionTotal = m_pBelongsTo->GetQuestionTotal();
      if (m_pBelongsTo->IsRandomTest()) {
          iQuestionTotal = m_pBelongsTo->GetNumberOfRandomQuestions();
          int iNoOfRandQuestion = m_pBelongsTo->GetRandomQuestionIndex(this);
          if (iNoOfRandQuestion >= 0)
            iQuestionNumber = iNoOfRandQuestion;
      }
   }

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aAreas;
   hr = GetObjectPointers(&aAreas);

   bool bTextChanged = false;
   for (int i=0; i<aAreas.GetSize() && SUCCEEDED(hr); ++i)
   {
      if (aAreas[i]->GetClassId() == INTERACTION_CLASS_CHANGE)
      {
         CChangeableObject *pChange = (CChangeableObject *)aAreas[i];

         DrawSdk::DrawObject *pObject = pChange->GetObject();

         if (pObject != NULL && pObject->GetType() == DrawSdk::TEXT)
         {
            DrawSdk::Text *pText = (DrawSdk::Text *)pObject;

            if (pText->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_TIME)
               _stprintf(tszText, _T("%02d:%02d"), iMinutes, iSeconds);
            else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_TRIES)
               _stprintf(tszText, _T("%d"), iAttempts);
            else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_NUMBER)
               _stprintf(tszText, _T("%d/%d"), iQuestionNumber, iQuestionTotal);
          
            
            if (_tcslen(tszText) > 0)
            {
               LPCTSTR tszCurrent = pText->GetString();

               if (_tcscmp(tszCurrent, tszText) != 0)
               {
                  pText->SetString(tszText, _tcslen(tszText));
                  bTextChanged = true;
               }
            }
         }
      }
   }

   if (SUCCEEDED(hr) && !bTextChanged)
      hr = S_FALSE;

   return hr;
}

// private
HRESULT CQuestionEx::GetAllFeedback(CArray<CFeedback *, CFeedback *> *paFeedback)
{
   HRESULT hr = S_OK;

   if (paFeedback == NULL)
      return E_POINTER;

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
   aObjects.SetSize(0, 40);

   hr = m_pInteractionStream->FindInteractions(&m_PageVisibility, true, &aObjects);

   if (SUCCEEDED(hr))
   {
      for (int i=0; i<aObjects.GetSize(); ++i)
      {
         if (aObjects[i]->GetClassId() == INTERACTION_CLASS_FEEDBACK)
         {
            CFeedback *pFeedback = (CFeedback *)aObjects[i];
            if (pFeedback->GetVisibilityPeriod() == INTERACTION_PERIOD_PAGE)
               paFeedback->Add(pFeedback);
         }
      }
   }

   _ASSERT(paFeedback->GetSize() > 0); // TODO remove

   return hr;
}

HRESULT CQuestionEx::AddTimerText(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                                  double &dXPos, double &dYPos, 
                                  CString &csTimer, COLORREF clrText, LOGFONT &lf, 
                                  bool bUseHashKey,UINT nHashKey)
{
   HRESULT hr = S_OK;

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;

   double dTextWidth = DrawSdk::Text::GetTextWidth(csTimer, csTimer.GetLength(), &lf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csTimer, csTimer.GetLength(), &lf);

   dYPos -= 1.1 * dTextHeight;

   DrawSdk::Text timerText(dXPos, dYPos, dTextWidth, dTextHeight, 
                           clrText, csTimer, csTimer.GetLength(), 
                           &lf, DrawSdk::TEXT_TYPE_TIME_SUPPORT);
   aObjects.Add(&timerText);

   CMoveableObjects *pTimerObject = new CMoveableObjects();
   pTimerObject->Init(NULL, &visibilityPeriod, pProject, &aObjects, MOVEABLE_TYPE_OTHER);
   if (bUseHashKey)
      pTimerObject->SetHashKey(nHashKey);

   AddObject(pTimerObject);

   aObjects.RemoveAll();
   
   dXPos += (dTextWidth + 5);

   return hr;
}

HRESULT CQuestionEx::AddTimerObject(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                                    double &dXPos, double &dYPos, 
                                    COLORREF clrText, LOGFONT &lf, 
                                    bool bUseHashKey,UINT nHashKey)
{
   HRESULT hr = S_OK;

   CString csTemp = _T("00:00");

   double dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);

   DrawSdk::Text timerCountText(dXPos, dYPos, dTextWidth, dTextHeight, 
                                clrText, csTemp, csTemp.GetLength(), 
                                &lf, DrawSdk::TEXT_TYPE_CHANGE_TIME);

   CChangeableObject *pTimerCountObject = new CChangeableObject();
   CRect rcTime;
   timerCountText.GetLogicalBoundingBox(&rcTime);
   pTimerCountObject->Init(&rcTime, &visibilityPeriod, pProject, &timerCountText);

   if (bUseHashKey)
      pTimerCountObject->SetHashKey(nHashKey);

   AddObject(pTimerCountObject);

   return hr;
}

HRESULT CQuestionEx::AddTriesText(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                                  double &dXPos, double &dYPos, 
                                  CString &csTries, COLORREF clrText, LOGFONT &lf, 
                                  bool bUseHashKey,UINT nHashKey)
{
   HRESULT hr = S_OK;

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;

   double dTextWidth = DrawSdk::Text::GetTextWidth(csTries, csTries.GetLength(), &lf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csTries, csTries.GetLength(), &lf);

   dYPos -= 1.1 * dTextHeight;

   DrawSdk::Text triesText(dXPos, dYPos, dTextWidth, dTextHeight, 
                           clrText, csTries, csTries.GetLength(), 
                           &lf, DrawSdk::TEXT_TYPE_TRIES_SUPPORT);

   aObjects.Add(&triesText);

   CMoveableObjects *pTriesObject = new CMoveableObjects();
   pTriesObject->Init(NULL, &visibilityPeriod, pProject, &aObjects, MOVEABLE_TYPE_OTHER);

   if (bUseHashKey)
      pTriesObject->SetHashKey(nHashKey);

   AddObject(pTriesObject);
   aObjects.RemoveAll();
   
   dXPos += (dTextWidth + 5);

   return hr;
}

HRESULT CQuestionEx::AddTriesObject(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                                    double &dXPos, double &dYPos, 
                                    COLORREF clrText, LOGFONT &lf, 
                                    bool bUseHashKey,UINT nHashKey)
{
   HRESULT hr = S_OK;

   CString csTemp = _T("0");

   double dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);

   DrawSdk::Text tryCountText(dXPos, dYPos, dTextWidth, dTextHeight, 
                              clrText, csTemp, csTemp.GetLength(), 
                              &lf, DrawSdk::TEXT_TYPE_CHANGE_TRIES);

   CChangeableObject *pTryCountObject = new CChangeableObject();
   CRect rcTry;
   tryCountText.GetLogicalBoundingBox(&rcTry);
   pTryCountObject->Init(&rcTry, &visibilityPeriod, pProject, &tryCountText);

   if (bUseHashKey)
      pTryCountObject->SetHashKey(nHashKey);

   AddObject(pTryCountObject);

   return hr;
}

HRESULT CQuestionEx::AddProgressText(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                                  double &dXPos, double &dYPos, 
                                  CString &csProgress, COLORREF clrText, LOGFONT &lf, 
                                  bool bInsertInStream, bool bUseHashKey, UINT nHashKey)
{
   HRESULT hr = S_OK;

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;

   double dTextWidth = DrawSdk::Text::GetTextWidth(csProgress, csProgress.GetLength(), &lf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csProgress, csProgress.GetLength(), &lf);

   dYPos -= 1.1 * dTextHeight;

   DrawSdk::Text progressText(dXPos, dYPos, dTextWidth, dTextHeight, 
                              clrText, csProgress, csProgress.GetLength(), 
                              &lf, DrawSdk::TEXT_TYPE_NUMBER_SUPPORT);

   aObjects.Add(&progressText);

   CMoveableObjects *pProgressObject = new CMoveableObjects();
   pProgressObject->Init(NULL, &visibilityPeriod, pProject, &aObjects, MOVEABLE_TYPE_OTHER);
   
   if (bUseHashKey)
      pProgressObject->SetHashKey(nHashKey);

   if (bInsertInStream)
   {
      CInteractionStream *pInteractionStream = pProject->GetInteractionStream();
      if (pInteractionStream != NULL)
      {
         pInteractionStream->InsertSingle(pProgressObject);
      }
   }
   else
      AddObject(pProgressObject);

   aObjects.RemoveAll();

   dXPos += (dTextWidth + 5);

   return hr;
}

HRESULT CQuestionEx::AddProgressObject(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                                    double &dXPos, double &dYPos, 
                                    COLORREF clrText, LOGFONT &lf, 
                                    bool bInsertInStream, bool bUseHashKey, UINT nHashKey)
{
   HRESULT hr = S_OK;

   CString csTemp = _T("0/0");

   double dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);

   DrawSdk::Text sumText(dXPos, dYPos, dTextWidth, dTextHeight, 
                         clrText, csTemp, csTemp.GetLength(), 
                         &lf, DrawSdk::TEXT_TYPE_CHANGE_NUMBER);

   CChangeableObject *pSumObject = new CChangeableObject();
   CRect rcSum;
   sumText.GetLogicalBoundingBox(&rcSum);
   pSumObject->Init(&rcSum, &visibilityPeriod, pProject, &sumText);

   if (bUseHashKey)
      pSumObject->SetHashKey(nHashKey);

   if (bInsertInStream)
   {
      CInteractionStream *pInteractionStream = pProject->GetInteractionStream();
      if (pInteractionStream != NULL)
      {
         pInteractionStream->InsertSingle(pSumObject);
      }
   }
   else
      AddObject(pSumObject);

   return hr;
}

HRESULT CQuestionEx::GetQuestionScale(CEditorProject *pProject, double &dScaleX, double &dScaleY)
{
   HRESULT hr = S_OK;

   CSize siPage;
   pProject->GetPageDimension(siPage);

   dScaleX = (double)siPage.cx / 720;
   if (dScaleX > 1.0)
      dScaleX = 1.0;
   dScaleY = (double)siPage.cy / 540;
   if (dScaleY > 1.0)
      dScaleY = 1.0;

   return hr;
}

HRESULT CQuestionEx::GetStateTextHeight(CEditorProject *pProject, double &dTextHeight)
{
   HRESULT hr = S_OK;

   LOGFONT lf;
   GetQuestionStateDefaultFont(lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(_T("Dummy"), 5, &lf);
   
   return hr;
}

HRESULT CQuestionEx::CalculateQuestionStateBottomPosition(CEditorProject *pProject, double &dYPos)
{
   HRESULT hr = S_OK;

   double dXScale, dYScale;
   GetQuestionScale(pProject, dXScale, dYScale);

   dYPos = (double)CQuestionnaireEx::BOTTOM_BORDER * dYScale + CQuestionnaireEx::BUTTON_HEIGHT;
   
   CSize siPage;
   pProject->GetPageDimension(siPage);

   dYPos = siPage.cy - dYPos;
   
   return hr;
}

HRESULT CQuestionEx::CalculateQuestionStateLeftPosition(CEditorProject *pProject, double &dXPos)
{
   HRESULT hr = S_OK;

   double dXScale, dYScale;
   GetQuestionScale(pProject, dXScale, dYScale);

   dXPos = (double)CQuestionnaireEx::LEFT_BORDER * dXScale;
   
   return hr;
}

HRESULT CQuestionEx::GetQuestionStateDefaultFont(LOGFONT &lf)
{
   HRESULT hr = S_OK;

   LFont::FillWithDefaultValues(&lf);
   lf.lfHeight = -14;
   lf.lfWeight = FW_NORMAL;

   return hr;
}
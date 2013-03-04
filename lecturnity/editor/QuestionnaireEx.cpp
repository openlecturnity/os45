#include "stdafx.h"
#include "lutils.h"
#include "..\Studio\resource.h"
#include "QuestionnaireEx.h"
#include "editorDoc.h"
#include "InteractionStream.h"
#include "QuestionStream.h"
#include "InteractionButtonDialog.h"
#include "ButtonAppearanceSettings.h"

/* REVIEW UK
 * There is large quantity of pProject->GetPageDimension(). Also in other classes.
 *     This should be better a parameter
 *     and in exchange CEditorProject should be removed.
 */

IMPLEMENT_DYNCREATE(CQuestionnaireEx, CObject)

CQuestionnaireEx::CQuestionnaireEx()
{
   m_pCorrectAction = NULL;
   m_pFailureAction = NULL;
   m_pCorrectActionBackup = NULL;
   m_pFailureActionBackup = NULL;
   m_bShowEvaluation = false;
   m_bShowProgress = false;
   m_bDoStopmarks = false;
   m_bPassTypeRelative = false;
   m_nNeededPoints = 0;
   m_mapFeedbackNotVisible.InitHashTable(17);
   m_aQuestionPointers.SetSize(0, 10);
   m_bQuestionPointersValid = true;
   m_pEditorProject = NULL;
   m_aFeedbackToCommit.SetSize(0, 5);
   m_bHasImageButtons = false;
   m_bJumpsResolved = false;
      
   m_csFilename[0].Empty();
   m_csFilename[1].Empty();
   m_csFilename[2].Empty();
   m_csFilename[3].Empty();
   ZeroMemory(m_clrFill, sizeof m_clrFill);
   ZeroMemory(m_clrLine, sizeof m_clrLine);
   ZeroMemory(m_clrText, sizeof m_clrText);
   ZeroMemory(m_lf, sizeof m_lf);

   m_bIsRandomTest = false;
   m_iNumberOfRandomQuestions = 0;
   SetRandomQuestionReplayIndex(0);
}

CQuestionnaireEx::~CQuestionnaireEx()
{
   if (m_pCorrectAction != NULL)
      delete m_pCorrectAction;

   if (m_pFailureAction != NULL)
      delete m_pFailureAction;

   POSITION pos = m_mapFeedbackNotVisible.GetStartPosition();
   while (pos != NULL)
   {
      FeedbackTypeId idKey;
      CFeedback *pFeedback = NULL;
      m_mapFeedbackNotVisible.GetNextAssoc(pos, idKey, pFeedback);
         
      delete pFeedback;
   }

   if (m_aFeedbackToCommit.GetSize() > 0)
   {
      _ASSERT(false);
      // shouldn't happen

      for (int i=0; i<m_aFeedbackToCommit.GetSize(); ++i)
         delete m_aFeedbackToCommit[i];
   }

    for (int i = 0; i< m_aQuestionMouseActions.GetSize(); i++) {
        delete m_aQuestionMouseActions.GetAt(i);
    }
    SAFE_DELETE(m_pCorrectActionBackup);
    SAFE_DELETE(m_pFailureActionBackup);
}

HRESULT CQuestionnaireEx::Init(CQuestionnaire *pQuestionnaire, bool bIgnoreQuestions, 
                               CEditorProject *pProject, UINT nInsertionMs)
{
   if (pQuestionnaire == NULL || pProject == NULL)
      return E_POINTER;

   if (bIgnoreQuestions == false)
      return E_INVALIDARG;

   m_csTitle = pQuestionnaire->GetTitle();
   m_bShowEvaluation = pQuestionnaire->IsShowEvaluation();
   m_bShowProgress = pQuestionnaire->IsShowProgress();
   m_bDoStopmarks = pQuestionnaire->IsDoStopmarks();
   m_bHasImageButtons = pQuestionnaire->IsImageButtons();
   m_bPassTypeRelative = pQuestionnaire->IsRelative();
   m_nNeededPoints = pQuestionnaire->GetPassPoints();
   m_bIsRandomTest = pQuestionnaire->IsRandomTest();
   m_iNumberOfRandomQuestions = pQuestionnaire->GetNumberOfRandomQuestions();

   CString csCorrectAction = pQuestionnaire->GetSuccessAction();
   if (!csCorrectAction.IsEmpty())
   {
      m_pCorrectAction = new CMouseAction();
      HRESULT hr = m_pCorrectAction->Parse(csCorrectAction, pProject, nInsertionMs);

      if (FAILED(hr))
      {
         delete m_pCorrectAction;
         return hr;
      }
   }

   CString csFailureAction = pQuestionnaire->GetFailureAction();
   if (!csFailureAction.IsEmpty())
   {
      m_pFailureAction = new CMouseAction();
      HRESULT hr = m_pFailureAction->Parse(csFailureAction, pProject, nInsertionMs);

      if (FAILED(hr))
      {
         delete m_pFailureAction;
         return hr;
      }
   }

   m_csOkText = pQuestionnaire->GetOkText();
   m_csSubmitText = pQuestionnaire->GetSubmitText();
   m_csDeleteText = pQuestionnaire->GetDeleteText();
 
   m_pEditorProject = pProject;

   CArray<CFeedbackDefinition *, CFeedbackDefinition *> aFeedback;
   pQuestionnaire->ExtractFeedback(&aFeedback);

   CInteractionStream *pInteractionStream = pProject->GetInteractionStream(true);
   CTimePeriod periodVisible;
   periodVisible.Init(INTERACTION_PERIOD_ALL_PAGES, 0, 1);

   if (pQuestionnaire->ContainsButtonConfig())
   {
      pQuestionnaire->GetInactiveButtonValues(m_clrFill[0], m_clrLine[0], m_clrText[0], m_lf[0], m_csFilename[0]);
      pQuestionnaire->GetNormalButtonValues(m_clrFill[1], m_clrLine[1], m_clrText[1], m_lf[1], m_csFilename[1]);
      pQuestionnaire->GetMouseoverButtonValues(m_clrFill[2], m_clrLine[2], m_clrText[2], m_lf[2], m_csFilename[2]);
      pQuestionnaire->GetPressedButtonValues(m_clrFill[3], m_clrLine[3], m_clrText[3], m_lf[3], m_csFilename[3]);
   }
   else
      SetDefaultFontAndColor(); 
   // TODO else only for old document from alpha and beta phase (2.0);
   // can be removed.

   for (int i=0; i<aFeedback.GetSize(); ++i)
   {
      CFeedback *pSingleFeedback = new CFeedback();
      HRESULT hr = pSingleFeedback->Init(aFeedback[i]);

      if (SUCCEEDED(hr))
      {
         if (pSingleFeedback->IsQuestionnaireOnly())
         {
            hr = pSingleFeedback->SetVisibility(&periodVisible);

            if (SUCCEEDED(hr))
               hr = pInteractionStream->InsertSingle(pSingleFeedback, false);
         }
         else
            m_mapFeedbackNotVisible.SetAt(pSingleFeedback->GetType(), pSingleFeedback);
         // for the 4 question feedbacks which just store the preset texts
      }

      if (FAILED(hr))
      {
         delete pSingleFeedback;
         return hr;
      }
   }

   // was "extracted"; delete here
   for (i=0; i<aFeedback.GetSize(); ++i)
      delete aFeedback[i];


   return S_OK;
}

HRESULT CQuestionnaireEx::Init(CString& csTitle, bool bShowEvaluation, bool bShowProgress, bool bDoStopmarks, bool bIsRandomTest, UINT iNumberOfRandomQuestions,
                               CMouseAction *pCorrectAction, CMouseAction *pFailureAction,
                               bool bPassTypeRelative, UINT nNeededPoints, CEditorProject *pProject,
                               CString& csOkText, CString& csSubmitText, CString& csDeleteText,
                               CArray<CFeedback *, CFeedback *> *paGeneralFeedback)
{
   HRESULT hr = S_OK;

   if (paGeneralFeedback == NULL)
      return E_POINTER;

   if (pCorrectAction != NULL)
      m_pCorrectAction = pCorrectAction->Copy();

   if (pFailureAction != NULL)
      m_pFailureAction = pFailureAction->Copy();

   m_csTitle = csTitle;
   m_bShowEvaluation = bShowEvaluation;
   m_bShowProgress = bShowProgress;
   m_bDoStopmarks = bDoStopmarks;
   m_bIsRandomTest = bIsRandomTest;
   m_iNumberOfRandomQuestions = iNumberOfRandomQuestions;
   m_bPassTypeRelative = bPassTypeRelative;
   m_nNeededPoints = nNeededPoints;
   
   m_csOkText = csOkText;
   m_csSubmitText = csSubmitText;
   m_csDeleteText = csDeleteText;

   m_pEditorProject = pProject;

   CInteractionStream *pInteractionStream = pProject->GetInteractionStream(true);
   CTimePeriod periodVisible;
   hr = periodVisible.Init(INTERACTION_PERIOD_ALL_PAGES, 0, 1);


   // TODO check for all necessary feedback to be defined
   for (int i=0; i<paGeneralFeedback->GetSize() && SUCCEEDED(hr); ++i)
   {
      CFeedback *pFeedback = paGeneralFeedback->GetAt(i);
      
      if (pFeedback->IsQuestionnaireOnly())
      {
         hr = pFeedback->SetVisibility(&periodVisible);

         if (SUCCEEDED(hr))
            m_aFeedbackToCommit.Add(pFeedback);
      }
      else
         m_mapFeedbackNotVisible.SetAt(pFeedback->GetType(), pFeedback);
   }

   return hr;
}

HRESULT CQuestionnaireEx::ResolveJumpTimes(CEditorProject *pProject, UINT nMoveMs)
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
   }

   return hr;
}

HRESULT CQuestionnaireEx::CommitFeedback()
{
   HRESULT hr = S_OK;

   CInteractionStream *pInteractionStream = m_pEditorProject->GetInteractionStream(false);
      
   for (int i=0; i<m_aFeedbackToCommit.GetSize() && SUCCEEDED(hr); ++i)
      hr = pInteractionStream->InsertSingle(m_aFeedbackToCommit[i], false);
   m_aFeedbackToCommit.RemoveAll();
   
   return hr;
}

HRESULT CQuestionnaireEx::RemoveFeedback()
{
   HRESULT hr = S_OK;

   CInteractionStream *pInteractionStream = m_pEditorProject->GetInteractionStream(false);

   if (pInteractionStream != NULL)
   {
      CArray<CFeedback *, CFeedback *> aFeedback;
      GetAllFeedback(&aFeedback);

      for (int i=0; i<aFeedback.GetSize() && SUCCEEDED(hr); ++i)
         hr = pInteractionStream->DeleteSingle(aFeedback[i]->GetHashKey(), false);
   }

   return hr;
}

 
/*
HRESULT CQuestionnaireEx::MoveFeedbackToFront()
{
   CInteractionStream *pInteractionStream = m_pProject->project.GetInteractionStream(false);

   if (pInteractionStream != NULL)
   {
      POSITION pos = m_mapFeedback.GetStartPosition();
      while (pos != NULL)
      {
         FeedbackTypeId idKey;
         CFeedback *pFeedback = NULL;
         m_mapFeedback.GetNextAssoc(pos, idKey, pFeedback);
         
         if (!pFeedback->IsDisabled() && !pFeedback->IsInherited())
         {
            pFeedback->ToFront(pInteractionStream);
         }
      }
   }

   return S_OK;
}
*/
 
CQuestionnaireEx* CQuestionnaireEx::Copy()
{
   if (HasQuestions())
      return NULL;
   // TODO copying a questionnaire that already contains questions
   // is not possible at the moment.
   // But this has to change for cut&paste for example.

   CQuestionnaireEx *pNewQuestionnaire = new CQuestionnaireEx();
   
   HRESULT hr = CloneTo(pNewQuestionnaire);

   // Note you cannot call Init() on the new object here as this would mess
   // up the feedback situation (insert it again and then 
   // delete the objects).

   if (SUCCEEDED(hr))
      return pNewQuestionnaire;
   else
   {
      delete pNewQuestionnaire;
      _ASSERT(false);
      return NULL;
   }
}

// private
HRESULT CQuestionnaireEx::CloneTo(CQuestionnaireEx *pTarget)
{
   if (pTarget == NULL)
      return E_POINTER;

   if (m_pCorrectAction != NULL)
      pTarget->m_pCorrectAction = m_pCorrectAction->Copy();

   if (m_pFailureAction != NULL)
      pTarget->m_pFailureAction = m_pFailureAction->Copy();

   pTarget->m_csTitle = m_csTitle;
   pTarget->m_bShowEvaluation = m_bShowEvaluation;
   pTarget->m_bShowProgress = m_bShowProgress;
   pTarget->m_bDoStopmarks = m_bDoStopmarks;
   pTarget->m_bPassTypeRelative = m_bPassTypeRelative;
   pTarget->m_nNeededPoints = m_nNeededPoints;
   pTarget->m_bIsRandomTest = m_bIsRandomTest;
   pTarget->m_iNumberOfRandomQuestions = m_iNumberOfRandomQuestions;

   pTarget->m_pEditorProject = m_pEditorProject;

   POSITION pos = m_mapFeedbackNotVisible.GetStartPosition();
   while (pos != NULL)
   {
      FeedbackTypeId idKey;
      CFeedback *pFeedback = NULL;
      m_mapFeedbackNotVisible.GetNextAssoc(pos, idKey, pFeedback);

      pTarget->m_mapFeedbackNotVisible.SetAt(idKey, (CFeedback *)pFeedback->Copy());
   }
   
   pTarget->SetButtonColor(m_clrFill, m_clrLine, m_clrText);
   pTarget->SetButtonFilenames(m_csFilename);
   pTarget->SetButtonFont(m_lf);
   pTarget->SetButtonsAreImageButtons(m_bHasImageButtons);
      
   pTarget->m_csOkText = m_csOkText;
   pTarget->m_csSubmitText = m_csSubmitText;
   pTarget->m_csDeleteText = m_csDeleteText;


   return S_OK;
}

HRESULT CQuestionnaireEx::UpdateButtonAppearance()
{
   HRESULT hr = S_OK;

   for (int i=0; i<m_aQuestionPointers.GetSize() && SUCCEEDED(hr); ++i)
   {
      hr = m_aQuestionPointers[i]->UpdateButtonAppearance(this, m_pEditorProject);
   }

   CInteractionStream *pInteractionStream = m_pEditorProject->GetInteractionStream(false);

   CArray<CFeedback *, CFeedback *> aFeedback;
   GetAllFeedback(&aFeedback);

   for (i=0; i<aFeedback.GetSize() && SUCCEEDED(hr); ++i)
   {
      if (aFeedback[i]->GetOkButton() != NULL)
      {
         CFeedback *pNewFeedback = (CFeedback *)aFeedback[i]->Copy(true);
         hr = pNewFeedback->GetOkButton()->ChangeAppearance(this);

         if (SUCCEEDED(hr))
            hr = pInteractionStream->ModifyArea(aFeedback[i]->GetHashKey(), pNewFeedback, false);
      }
   }
   
   return hr;
}

CFeedback* CQuestionnaireEx::GetFeedback(FeedbackTypeId idType)
{
   CFeedback *pFeedbackReturn = NULL;
   
   CArray<CFeedback *, CFeedback *> aFeedback;
   GetAllFeedback(&aFeedback);

   for (int i=0; i<aFeedback.GetSize(); ++i)
   {
      if (aFeedback[i]->GetType() == idType)
      {
         pFeedbackReturn = aFeedback[i];
         break;
      }
   }
  
   return pFeedbackReturn;
}

int CQuestionnaireEx::GetQuestionNumber(CQuestionEx *pQuestion)
{
   if (pQuestion == NULL)
      return -1;

   for (int i=0; i<m_aQuestionPointers.GetSize(); ++i)
   {
      if (pQuestion == m_aQuestionPointers[i])
         return i;
   }

   return -1;
}


HRESULT CQuestionnaireEx::RegisterQuestion(CQuestionEx *pQuestion)
{
   if (pQuestion != NULL)
   {
      bool bInserted = false;
      for (int i=0; i<m_aQuestionPointers.GetSize() + 1; ++i)
      {
         if (i == m_aQuestionPointers.GetSize() 
            || m_aQuestionPointers[i]->BeginsAtOrAfter(pQuestion->GetPageBegin()))
         {
            m_aQuestionPointers.InsertAt(i, pQuestion);
            bInserted = true;
            break;
         }
      }

      if (!bInserted) // fallback; shouldn't happen
         m_aQuestionPointers.Add(pQuestion);

      return S_OK;
   }
   else
      return E_POINTER;
}

HRESULT CQuestionnaireEx::UnregisterQuestion(CQuestionEx *pQuestion, CQuestionEx *pQuestionNew)
{
   if (pQuestion != NULL)
   {
      for (int i=0; i<m_aQuestionPointers.GetSize(); ++i)
      {
         if (pQuestion == m_aQuestionPointers[i])
         {
            if (pQuestionNew == NULL)
               m_aQuestionPointers.RemoveAt(i);
            else
               m_aQuestionPointers.SetAt(i, pQuestionNew);

            return S_OK;
         }
      }
      return S_FALSE; // not found
   }
   else
      return E_POINTER;
}

HRESULT CQuestionnaireEx::ExchangeQuestion(CQuestionEx *pQuestionOld, CQuestionEx *pQuestionNew)
{
   return UnregisterQuestion(pQuestionOld, pQuestionNew);
}
   

HRESULT CQuestionnaireEx::TransferQuestionPointers(CQuestionnaireEx *pTarget)
{
   if (pTarget == NULL)
      return E_POINTER;

   for (int i=0; i<m_aQuestionPointers.GetSize(); ++i)
      m_aQuestionPointers[i]->SetBelongsTo(pTarget);
   pTarget->m_aQuestionPointers.Append(m_aQuestionPointers);

   m_bQuestionPointersValid = false;

   return S_OK;
}

HRESULT CQuestionnaireEx::ClaimQuestionPointers()
{
   m_bQuestionPointersValid = true;

   return S_OK;
}

bool CQuestionnaireEx::HasQuestions()
{ 
   return m_aQuestionPointers.GetSize() > 0 && m_bQuestionPointersValid; 
}
   

HRESULT CQuestionnaireEx::ReplaceFeedback(CFeedback *pNewFeedback, bool bSetRollback)
{
   HRESULT hr = S_OK;

   if (pNewFeedback == NULL)
      return E_POINTER;

   FeedbackTypeId idType = pNewFeedback->GetType();

   if (idType != FEEDBACK_TYPE_QQ_EVAL)
   {
      _ASSERT(false);
      // only this can be changed externally
      return E_INVALIDARG;
   }

   CInteractionStream *pInteractionStream = m_pEditorProject->GetInteractionStream(true);

   CFeedback *pOldFeedback = GetFeedback(idType);
   if (pOldFeedback != NULL)
      hr = pInteractionStream->ModifyArea(pOldFeedback->GetHashKey(), pNewFeedback, bSetRollback);

   return hr;
}

HRESULT CQuestionnaireEx::Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
                                CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
                                LPCTSTR tszExportPrefix, LPCTSTR tszLepFilename, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (pOutput == NULL)
      return E_POINTER;
   
   // TODO can tszExportPrefix be NULL?

   bool bIsExport = tszLepFilename == NULL || _tcslen(tszLepFilename) == 0;
   if (bIsExport && m_aQuestionPointers.GetSize() <= 0)
      return S_FALSE; // do not write empty tests upon export

   CString csRandQuestionNumber;
   csRandQuestionNumber.Format(_T("%d"), m_iNumberOfRandomQuestions);

   UINT nIdentLength = bDoIdent ? 6 : 0; // number of spaces in LEP

   _TCHAR *tszTrue = _T("true");
   _TCHAR *tszFalse = _T("false");

   CString csTitle;
   StringManipulation::TransformForSgml(m_csTitle, csTitle);

   CString csOkText;
   StringManipulation::TransformForSgml(m_csOkText, csOkText);

   CString csSubmitText;
   StringManipulation::TransformForSgml(m_csSubmitText, csSubmitText);

   CString csDeleteText;
   StringManipulation::TransformForSgml(m_csDeleteText, csDeleteText);

   
   _TCHAR tszOutput[2000];
   _stprintf(tszOutput, _T("<QUESTIONNAIRE title=\"%s\" eval=\"%s\" progress=\"%s\" stops=\"%s\" image-buttons=\"%s\" ok-t=\"%s\" submit-t=\"%s\" delete-t=\"%s\" randomizedQuestions=\"%s\" randomizedQuestionsNumber=\"%s\""), 
      csTitle, m_bShowEvaluation ? tszTrue : tszFalse, 
      m_bShowProgress ? tszTrue : tszFalse, m_bDoStopmarks ? tszTrue : tszFalse,
      m_bHasImageButtons ? tszTrue : tszFalse,
      csOkText, csSubmitText, csDeleteText, m_bIsRandomTest ? tszTrue : tszFalse, csRandQuestionNumber);
   
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
     
      pOutput->Append(_T(">\n"));


      // TODO proper indentation?? all except QUESTIONNAIRE need a level more


      _TCHAR *tszType = _T("absolute");
      if (m_bPassTypeRelative)
         tszType = _T("relative");

      UINT nMaximumPoints = 0;
      for (int i=0; i<m_aQuestionPointers.GetSize(); ++i)
         nMaximumPoints += m_aQuestionPointers[i]->GetAchievablePoints();

      _stprintf(tszOutput, _T("<pass type=\"%s\" value=\"%d\" max=\"%d\"></pass>\n"), tszType, m_nNeededPoints, nMaximumPoints);
      pOutput->Append(tszOutput);
      
      hr = WriteDefaultButtonConfig(pOutput, tszLepFilename);
 
     
      CArray<CFeedback *, CFeedback *> aFeedback;
      GetAllFeedback(&aFeedback);
      
      for (i=0; i<aFeedback.GetSize() && SUCCEEDED(hr); ++i)
      {
         hr = aFeedback[i]->Write(pOutput, nIdentLength, bUseFullImagePath, mapImagePaths, tszExportPrefix, pProject);
      }
      
      for (i=0; i<m_aQuestionPointers.GetSize() && SUCCEEDED(hr); ++i)
      {
         hr = m_aQuestionPointers[i]->Write(pOutput, bDoIdent, 
            bUseFullImagePath, mapImagePaths, tszExportPrefix, pProject);
      }
      
      // also write maximum points
      
      pOutput->Append(_T("</QUESTIONNAIRE>\n"));
   }

   return hr;
}

bool CQuestionnaireEx::AnyQuestionsLeft()
{
   for (int i=0; i<m_aQuestionPointers.GetSize(); ++i)
   {
      if (m_aQuestionPointers[i]->IsEnabled())
         return true;
   }

   return false;
}

bool CQuestionnaireEx::CheckPassed(UINT *pnMax, UINT *pnGet, UINT *pnPass)
{
   UINT nMaximumPoints = 0;
   UINT nAchievedPoints = 0;

   for (int i=0; i<m_aQuestionPointers.GetSize(); ++i)
   {
      CQuestionEx *pQuestion = m_aQuestionPointers[i];
      UINT nMaxQuestionPoints = pQuestion->GetAchievablePoints();
      nMaximumPoints += nMaxQuestionPoints;
      if (pQuestion->IsAnsweredCorrect())
         nAchievedPoints += nMaxQuestionPoints;
   }

   UINT nPassPoints = m_nNeededPoints;
   if (m_bPassTypeRelative)
      nPassPoints = (UINT)(m_nNeededPoints/100.0 * nMaximumPoints); // rounded down; same as in the Player

   if (pnMax != NULL)
      *pnMax = nMaximumPoints;
   if (pnGet != NULL)
      *pnGet = nAchievedPoints;
   if (pnPass != NULL)
      *pnPass = nPassPoints;

   return nAchievedPoints >= nPassPoints;
}

bool CQuestionnaireEx::IsEvaluationDefined()
{
   if (m_bShowEvaluation)
   {
      CFeedback *pFeedback = GetFeedback(FEEDBACK_TYPE_QQ_EVAL);

      if (pFeedback != NULL && !pFeedback->IsDisabled())
         return true;
   }

   return false;
}

HRESULT CQuestionnaireEx::ShowEvaluation(CWhiteboardView *pView)
{
   HRESULT hr = S_FALSE;

   if (pView == NULL)
      return E_POINTER;

   if (m_bShowEvaluation)
   {
      CFeedback *pFeedback = GetFeedback(FEEDBACK_TYPE_QQ_EVAL);

      if (pFeedback != NULL && !pFeedback->IsDisabled())
      {
         UINT nMaximumPoints = 0;
         UINT nAchievedPoints = 0;
         UINT nPassPoints = 0;

         bool bSuccess = CheckPassed(&nMaximumPoints, &nAchievedPoints, &nPassPoints);

         pFeedback->SetPassed(bSuccess);

         // if there is no point at all: avoid divide by zero

         UINT nAchievedPercent = 100;
         if (nMaximumPoints > 0)
            nAchievedPercent = (nAchievedPoints * 100) / nMaximumPoints;
         CString csAchievedPercent;
         csAchievedPercent.Format(_T("%d%%"), nAchievedPercent);
         UINT nPassPercent = 100;
         if (nMaximumPoints > 0)
            nPassPercent = (nPassPoints * 100) / nMaximumPoints;
         CString csPassPercent;
         csPassPercent.Format(_T("%d%%"), nPassPercent);

         CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paFeebackObjects = pFeedback->GetObjects();         

         int iCorrectCounter = 0;
         int iPointsCounter = 0;
         for (int i=0; i<paFeebackObjects->GetSize(); ++i)
         {
            if (paFeebackObjects->GetAt(i)->GetType() == DrawSdk::TEXT)
            {
               DrawSdk::Text *pText = (DrawSdk::Text *)paFeebackObjects->GetAt(i);

               if (pText->HasChangeableType())
               {
                  // Do nothing on passed and failed message; instead it is not shown during painting.
                  //if (!bSuccess && pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_PASSED)
                  //   pText->SetString(_T(""), 0);
                  //else if (bSuccess && pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_FAILED)
                  //   pText->SetString(_T(""), 0);
                  
                  if (pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_ACHIEVED)
                     pText->SetStringNumber(nAchievedPoints);
                  else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT)
                     pText->SetString(csAchievedPercent, csAchievedPercent.GetLength());
                  else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_TOTAL)
                     pText->SetStringNumber(nMaximumPoints);
                  else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_REQUIRED)
                     pText->SetStringNumber(nPassPoints);
                  else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT)
                     pText->SetString(csPassPercent, csPassPercent.GetLength());
                  else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_Q_CORRECT)
                  {
                     if (iCorrectCounter < m_aQuestionPointers.GetSize())
                     {
                        CQuestionEx *pQuestion = m_aQuestionPointers[iCorrectCounter];
                        CString csMessage;
                        bool bSuccess = pQuestion->IsAnsweredCorrect();
                        if (bSuccess)
                           csMessage.LoadString(IDS_CORRECT);
                        else
                           csMessage.LoadString(IDS_WRONG);
                        pText->SetString((LPCTSTR)csMessage, csMessage.GetLength());
                        ++iCorrectCounter;
                     }
                  }
                  else if (pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_Q_POINTS)
                  {
                     if (iPointsCounter < m_aQuestionPointers.GetSize())
                     {
                        CQuestionEx *pQuestion = m_aQuestionPointers[iPointsCounter];
                        bool bSuccess = pQuestion->IsAnsweredCorrect();
                        if (bSuccess)
                           pText->SetStringNumber(pQuestion->GetAchievablePoints());
                        else
                           pText->SetStringNumber(0);
                        ++iPointsCounter;
                     }
                  }
               }
            }
         }

         pView->ShowFeedback(pFeedback);

         hr = S_OK;
      }
   }

   return hr;
}

HRESULT CQuestionnaireEx::Execute(CEditorDoc *pDoc, bool bQuestionnairePassed)
{
   HRESULT hr = S_OK;

   if (bQuestionnairePassed && m_pCorrectAction != NULL)
      m_pCorrectAction->Execute(pDoc);

   if (!bQuestionnairePassed && m_pFailureAction != NULL)
      m_pFailureAction->Execute(pDoc);

   return hr;
}

HRESULT CQuestionnaireEx::ResetAfterPreview()
{
   for (int i=0; i<m_aQuestionPointers.GetSize(); ++i)
   {
      m_aQuestionPointers[i]->ResetAfterPreview();
   }

   return S_OK;
}

void CQuestionnaireEx::SetButtonColor(COLORREF *pClrFill, COLORREF *pClrLine, COLORREF *pClrText)
{
   for (int i = 0; i < 4; ++i)
   {
      m_clrFill[i] = pClrFill[i];
      m_clrLine[i] = pClrLine[i];
      m_clrText[i] = pClrText[i];
   }
}

void CQuestionnaireEx::SetButtonFilenames(CString *csFilename)
{
   for (int i = 0; i < 4; ++i)
      m_csFilename[i] = csFilename[i];
}

void CQuestionnaireEx::SetButtonFont(LOGFONT *pLogFont)
{
   for (int i = 0; i < 4; ++i)
      m_lf[i] = pLogFont[i];
}

void CQuestionnaireEx::GetNormalButtonValues(COLORREF &clrFill, COLORREF &clrLine, 
                                             COLORREF &clrText, LOGFONT &lf, CString &csFilename)
{
   clrFill = m_clrFill[1];
   clrLine = m_clrLine[1];
   clrText = m_clrText[1];
   lf = m_lf[1];
   csFilename = m_csFilename[1];
}

void CQuestionnaireEx::GetMouseoverButtonValues(COLORREF &clrFill, COLORREF &clrLine, 
                                                COLORREF &clrText, LOGFONT &lf, CString &csFilename)
{
   clrFill = m_clrFill[2];
   clrLine = m_clrLine[2];
   clrText = m_clrText[2];
   lf = m_lf[2];
   csFilename = m_csFilename[2];
}

void CQuestionnaireEx::GetPressedButtonValues(COLORREF &clrFill, COLORREF &clrLine, 
                                              COLORREF &clrText, LOGFONT &lf, CString &csFilename)
{
   clrFill = m_clrFill[3];
   clrLine = m_clrLine[3];
   clrText = m_clrText[3];
   lf = m_lf[3];
   csFilename = m_csFilename[3];
}

void CQuestionnaireEx::GetInactiveButtonValues(COLORREF &clrFill, COLORREF &clrLine, 
                                               COLORREF &clrText, LOGFONT &lf, CString &csFilename)
{
   clrFill = m_clrFill[0];
   clrLine = m_clrLine[0];
   clrText = m_clrText[0];
   lf = m_lf[0];
   csFilename = m_csFilename[0];
}

bool operator!= (LOGFONT &a, LOGFONT &b)
{
   int iCompareSize = sizeof LOGFONT;

   BYTE *pData1 = (BYTE *)&a;
   BYTE *pData2 = (BYTE *)&b;

   for (int i=0; i<iCompareSize; ++i)
   {
      if (pData1[i] != pData2[i])
         return true;
   }

   return false;
}

bool CQuestionnaireEx::IsButtonConfigurationDifferent(CQuestionnaireEx *pOther)
{
   if (pOther == NULL)
      return false;

   // font different?
   if (m_lf[0] != pOther->m_lf[0])
      return true;
   if (m_lf[1] != pOther->m_lf[1])
      return true;
   if (m_lf[2] != pOther->m_lf[2])
      return true;
   if (m_lf[3] != pOther->m_lf[3])
      return true;

   if (m_bHasImageButtons != pOther->m_bHasImageButtons)
      return true;

   // text color different
   if (m_clrText[0] != pOther->m_clrText[0])
      return true;
   if (m_clrText[1] != pOther->m_clrText[1])
      return true;
   if (m_clrText[2] != pOther->m_clrText[2])
      return true;
   if (m_clrText[3] != pOther->m_clrText[3])
      return true;

   // button labels different
   if (m_csOkText.Compare(pOther->m_csOkText) != 0)
      return true;
   if (m_csSubmitText.Compare(pOther->m_csSubmitText) != 0)
      return true;
   if (m_csDeleteText.Compare(pOther->m_csDeleteText) != 0)
      return true;

   if (m_bHasImageButtons)
   {
      // image paths different
      if (m_csFilename[0].Compare(pOther->m_csFilename[0]) != 0)
         return true;
      if (m_csFilename[1].Compare(pOther->m_csFilename[1]) != 0)
         return true;
      if (m_csFilename[2].Compare(pOther->m_csFilename[2]) != 0)
         return true;
      if (m_csFilename[3].Compare(pOther->m_csFilename[3]) != 0)
         return true;
   }
   else
   {
      // other colors different

      if (m_clrLine[0] != pOther->m_clrLine[0])
         return true;
      if (m_clrLine[1] != pOther->m_clrLine[1])
         return true;
      if (m_clrLine[2] != pOther->m_clrLine[2])
         return true;
      if (m_clrLine[3] != pOther->m_clrLine[3])
         return true;

      if (m_clrFill[0] != pOther->m_clrFill[0])
         return true;
      if (m_clrFill[1] != pOther->m_clrFill[1])
         return true;
      if (m_clrFill[2] != pOther->m_clrFill[2])
         return true;
      if (m_clrFill[3] != pOther->m_clrFill[3])
         return true;
   }

   return false;
}


HRESULT CQuestionnaireEx::ChangeQuestionsShowProgress(bool bShowProgress)
{
   HRESULT hr = S_OK;

   for (int i=0; i<m_aQuestionPointers.GetSize() && SUCCEEDED(hr); ++i)
   {
      hr = m_aQuestionPointers[i]->ChangeQuestionsShowProgress(m_pEditorProject, bShowProgress);
   }
   
   return hr;
}
  
/////////////////////////////////////////////////////////////////////////////
// static functions

CFeedback *CQuestionnaireEx::CreateResultFeedback(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire) //, CArray<CQuestionEx *, CQuestionEx *> *paQuestionPointers)
{
   HRESULT hr = S_OK;

   LOGFONT lf;
   LFont::FillWithDefaultValues(&lf);

   int OFFSET_TOP = 15;
   int OFFSET_BOTTOM = 5;
   int OFFSET_X = 10;
   int COLUMN_OFFSET = 15;
   int OK_WIDTH = 80;
   int OK_HEIGHT = 20;
   int TITLE_FONT_HEIGHT = -16;
   int NORMAL_FONT_HEIGHT = -14;
   int SMALL_FONT_HEIGHT = -12;

   // Calculate width
   double dMaxXPos = 0;
   double dMaxYPos = 0;
   double dColumnWidth = 0;

   CString csTitle;
   csTitle.LoadString(IDS_RESULT);
   lf.lfHeight = TITLE_FONT_HEIGHT;
   lf.lfWeight = FW_NORMAL;

   double dTextWidth = DrawSdk::Text::GetTextWidth(csTitle, csTitle.GetLength(), &lf);
   dMaxXPos += dTextWidth;

   lf.lfHeight = NORMAL_FONT_HEIGHT;
   lf.lfWeight = FW_NORMAL;
   CString csTemp;

   double dMaxSubStringWidth = 0;
   csTemp.LoadString(IDS_TITLE_QUESTION);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   if (dMaxSubStringWidth < dTextWidth)
      dMaxSubStringWidth = dTextWidth;

   csTemp.LoadString(IDS_TITLE_ANSWER);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   if (dMaxSubStringWidth < dTextWidth)
      dMaxSubStringWidth = dTextWidth;

   csTemp.LoadString(IDS_TITLE_POINTS);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   if (dMaxSubStringWidth < dTextWidth)
      dMaxSubStringWidth = dTextWidth;

   dMaxSubStringWidth += 2 * COLUMN_OFFSET;
   dColumnWidth = dMaxSubStringWidth;

   if (dMaxXPos < 3*dMaxSubStringWidth)
      dMaxXPos = 3*dMaxSubStringWidth;

   
   CEditorDoc *pDoc = pProject->m_pDoc;

   CTimePeriod visualityPeriod;
   visualityPeriod.Init(INTERACTION_PERIOD_ALL_PAGES, 0, pDoc->m_displayEndMs);

   CRect rcTestOk(0, 0, OK_WIDTH, OK_HEIGHT);
   CMouseAction testClickAction;
   testClickAction.Init(pProject, INTERACTION_NO_ACTION);
   CString csTestOk = _T("OK");
   CInteractionAreaEx *pTestOkButton = CreateQuestionnaireButton(pProject, pQuestionnaire, BUTTON_TYPE_OK,
                                                                &visualityPeriod, NULL, testClickAction,
                                                                rcTestOk, csTestOk);
   if (rcTestOk.Width() > dMaxXPos)
      dMaxXPos = rcTestOk.Width();

   delete pTestOkButton;

   // create rectangle (width i scurrent, height have to be set later
   double dFeedbackWidth = dMaxXPos + 2*OFFSET_X;
   double dFeedbackHeight = 0;

   CRect rcFeedback(0, 0, (int)dFeedbackWidth, (int)dFeedbackHeight);

   double dXPos = OFFSET_X;
   double dYPos = 0;

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;


   DrawSdk::Rectangle *pBgFilled = 
      new DrawSdk::Rectangle(rcFeedback.left, rcFeedback.top, rcFeedback.Width(), rcFeedback.Height(),
                             RGB(22, 74, 122), RGB(236, 233, 216), 1.0, 0);
   aObjects.Add(pBgFilled);

   DrawSdk::Rectangle *pBgOutline = 
      new DrawSdk::Rectangle(rcFeedback.left, rcFeedback.top, rcFeedback.Width(), rcFeedback.Height(), 
                             RGB(22, 74, 122), 1.0, 0);
   aObjects.Add(pBgOutline);

   DrawSdk::Text *pText = NULL;

   // title
   csTitle.LoadString(IDS_RESULT);
   lf.lfHeight = TITLE_FONT_HEIGHT;
   lf.lfWeight = FW_NORMAL;
   dTextWidth = DrawSdk::Text::GetTextWidth(csTitle, csTitle.GetLength(), &lf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csTitle, csTitle.GetLength(), &lf);
   double dTextAscent = DrawSdk::Text::GetTextAscent(&lf);
   
   dXPos = (double)(rcFeedback.Width() - dTextWidth) / 2;
   dYPos = OFFSET_TOP + dTextAscent;

   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTitle, csTitle.GetLength(), &lf);
   aObjects.Add(pText);

   dYPos += dTextHeight + 5;

   // first line
   lf.lfHeight = NORMAL_FONT_HEIGHT;
   lf.lfWeight = FW_NORMAL;

   dXPos = OFFSET_X;
   csTemp.LoadString(IDS_TITLE_QUESTION);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                           csTemp, csTemp.GetLength(), &lf);
   aObjects.Add(pText);
   
   dXPos += dColumnWidth;
   csTemp.LoadString(IDS_TITLE_ANSWER);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                           csTemp, csTemp.GetLength(), &lf);
   aObjects.Add(pText);
   
   dXPos += dColumnWidth;
   csTemp.LoadString(IDS_TITLE_POINTS);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                        csTemp, csTemp.GetLength(), &lf);
   aObjects.Add(pText);
   dYPos += 3;
   
   double dLineWidth = rcFeedback.Width() - 2*OFFSET_X;

   DrawSdk::Line *pLine = new DrawSdk::Line(rcFeedback.left + OFFSET_X, dYPos, dLineWidth, 0, 
                                            RGB(22, 74, 122), 1, 0);
   aObjects.Add(pLine);
   dYPos += 3;

   lf.lfWeight = FW_NORMAL;
   dTextAscent = DrawSdk::Text::GetTextAscent(&lf);
   
   if (pQuestionnaire != NULL)
   {
      lf.lfHeight = SMALL_FONT_HEIGHT;
      UINT iEvaluationQuestionsNumber = pQuestionnaire->GetQuestionTotal();
      if (pQuestionnaire->IsRandomTest()) 
          iEvaluationQuestionsNumber = pQuestionnaire->GetNumberOfRandomQuestions();

      for (unsigned int i = 0; i < iEvaluationQuestionsNumber; ++i)
      {
         dXPos = rcFeedback.left + OFFSET_X;
         dYPos += dTextAscent;
         csTemp.Format(IDS_DEFAULT_QUESTION_TITLE, i + 1);
         dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
         dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
         pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
            csTemp, csTemp.GetLength(), &lf);
         aObjects.Add(pText);
         
         dXPos += dColumnWidth;
         csTemp.LoadString(IDS_CORRECT);
         dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
         dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
         pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
            csTemp, csTemp.GetLength(), &lf, DrawSdk::TEXT_TYPE_EVAL_Q_CORRECT);
         aObjects.Add(pText);
         
         dXPos += dColumnWidth;
         csTemp = _T("0");
         dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
         dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
         pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
            csTemp, csTemp.GetLength(), &lf, DrawSdk::TEXT_TYPE_EVAL_Q_POINTS);
         aObjects.Add(pText);
         
         dYPos += (dTextHeight-dTextAscent) + 3;
      }
   }
  
   pLine = new DrawSdk::Line(rcFeedback.left + OFFSET_X, dYPos, dLineWidth, 0, RGB(22, 74, 122), 1, 0);
   aObjects.Add(pLine);
   dYPos += 2;

   pLine = new DrawSdk::Line(rcFeedback.left + OFFSET_X, dYPos, dLineWidth, 0, RGB(22, 74, 122), 1, 0);
   aObjects.Add(pLine);
   dYPos += 3 + dTextAscent;

   lf.lfHeight = SMALL_FONT_HEIGHT;
   // maximal points
   dXPos = rcFeedback.left + OFFSET_X;
   csTemp.LoadString(IDS_POINTS_MAX);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf);
   aObjects.Add(pText);
   
   dXPos += 2*dColumnWidth;
   csTemp = _T("00");
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf, DrawSdk::TEXT_TYPE_EVAL_QQ_TOTAL);
   aObjects.Add(pText);

   // required points
   dXPos = rcFeedback.left + OFFSET_X;
   dYPos += dTextHeight + 3;
   lf.lfWeight = FW_NORMAL;
   csTemp.LoadString(IDS_POINTS_REQUIRED);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf);
   aObjects.Add(pText);
   
   dXPos += 2*dColumnWidth;
   csTemp = _T("0");
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf, DrawSdk::TEXT_TYPE_EVAL_QQ_REQUIRED);
   aObjects.Add(pText);

   // required percentage
   dXPos = rcFeedback.left + OFFSET_X;
   dYPos += dTextHeight + 3;
   lf.lfWeight = FW_NORMAL;
   csTemp.LoadString(IDS_POINTS_REQUIRED_PERCENTAGE);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf);
   aObjects.Add(pText);
   
   dXPos += 2*dColumnWidth;
   csTemp = _T("000%");
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf, DrawSdk::TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT);
   aObjects.Add(pText);

   // achieved points
   dXPos = rcFeedback.left + OFFSET_X;
   dYPos += dTextHeight + 3;
   lf.lfWeight = FW_NORMAL;
   csTemp.LoadString(IDS_POINTS_ACHIEVED);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf);
   aObjects.Add(pText);
   
   dXPos += 2*dColumnWidth;
   csTemp = _T("0");
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf, DrawSdk::TEXT_TYPE_EVAL_QQ_ACHIEVED);
   aObjects.Add(pText);

   dXPos = rcFeedback.left + OFFSET_X;
   dYPos += dTextHeight + 3;
   csTemp.LoadString(IDS_POINTS_ACHIEVED_PERCENTAGE);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf);
   aObjects.Add(pText);

   dXPos += 2*dColumnWidth;
   csTemp = _T("000%");
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf, DrawSdk::TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT);
   aObjects.Add(pText);


   lf.lfHeight = NORMAL_FONT_HEIGHT;
   dXPos = rcFeedback.left + OFFSET_X;
   dYPos += dTextHeight + 7;
   lf.lfWeight = FW_NORMAL;
   csTemp.LoadString(IDS_MESSAGE_SUCCESS);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf, DrawSdk::TEXT_TYPE_EVAL_QQ_PASSED);
   aObjects.Add(pText);
   
   csTemp.LoadString(IDS_MESSAGE_FAILED);
   dTextWidth = DrawSdk::Text::GetTextWidth(csTemp, csTemp.GetLength(), &lf);
   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, RGB(22, 74, 122), 
                             csTemp, csTemp.GetLength(), &lf, DrawSdk::TEXT_TYPE_EVAL_QQ_FAILED);
   aObjects.Add(pText);

   dTextHeight = DrawSdk::Text::GetTextHeight(csTemp, csTemp.GetLength(), &lf);
   dYPos += dTextHeight + 3;

   // create ok button objects 
   LOGFONT lfOk;
   LFont::FillWithDefaultValues(&lfOk);
   lfOk.lfHeight = -16;
   lfOk.lfWeight = FW_NORMAL;

   CString csOk = _T("OK");
   if (pQuestionnaire)
      csOk = pQuestionnaire->GetOkText();
   Gdiplus::RectF gdipRcOk;
   gdipRcOk.X = (float)(dFeedbackWidth - OK_WIDTH) / 2;
   gdipRcOk.Y = (float)dYPos;
   gdipRcOk.Width = (float)OK_WIDTH;
   gdipRcOk.Height = (float)OK_HEIGHT;
   CRect rcOk((int)gdipRcOk.X, (int)gdipRcOk.Y, (int)(gdipRcOk.X+gdipRcOk.Width), (int)(gdipRcOk.Y+gdipRcOk.Height));
   
   CMouseAction clickAction;
   clickAction.Init(pProject, INTERACTION_NO_ACTION);

   CInteractionAreaEx *pOkButton = CreateQuestionnaireButton(pProject, pQuestionnaire, BUTTON_TYPE_OK,
                                                             &visualityPeriod, NULL, clickAction,
                                                             rcOk, csOk);
   

   if (rcOk.Width() != OK_WIDTH)
   {
      float fOffsetX = (float)(OK_WIDTH - rcOk.Width()) / 2;
      pOkButton->Move(fOffsetX, 0);
   }
 

   dFeedbackHeight = dYPos + rcOk.Height() + OFFSET_BOTTOM;
   
   // center feedback
   CSize siPage;
   pProject->GetPageDimension(siPage);

   int iXOffset = 0;
   if (dMaxXPos < siPage.cx)
      iXOffset = (int)((siPage.cx - dMaxXPos) / 2);

   int iYOffset = 0;
   if (dMaxYPos < siPage.cy)
      iYOffset = (int)((siPage.cy - dFeedbackHeight) / 2);

   rcFeedback.OffsetRect(iXOffset, iYOffset);

   pBgFilled->SetHeight(dFeedbackHeight);
   pBgOutline->SetHeight(dFeedbackHeight);

   for (int j = 0; j < aObjects.GetSize(); ++j)
   {
      aObjects[j]->Move(iXOffset, iYOffset);
   }

   pOkButton->Move((float)iXOffset, (float)iYOffset);

   CFeedback *pEvalFeedback = new CFeedback();
   CString csDummy;
   csDummy.LoadString(IDS_RESULT);
   if (SUCCEEDED(hr))
      hr = pEvalFeedback->Init(FEEDBACK_TYPE_QQ_EVAL, false, false, csDummy, &aObjects, pOkButton);
   
   for (j = 0; j < aObjects.GetSize(); ++j)
      delete aObjects[j];
   aObjects.RemoveAll();
   
   //if (pOkButton)
   //   delete pOkButton;

   return pEvalFeedback;
}

void CQuestionnaireEx::CalculateDefaultButtonPosition(CEditorProject *pProject, CRect &rcSubmit, CRect &rcDelete)
{ 
   CSize siSubmit;
   CalculateButtonSize(BUTTON_TYPE_SUBMIT, siSubmit);
   CSize siDelete;
   CalculateButtonSize(BUTTON_TYPE_DELETE, siDelete);
   CSize siMaxButton = siSubmit;
   if (siDelete.cx > siMaxButton.cx)
      siMaxButton.cx = siDelete.cx;
   if (siDelete.cy > siMaxButton.cy)
      siMaxButton.cy = siDelete.cy;

   CRect rcSubmitButton(0, 0, 0, 0);
   CRect rcDeleteButton(0, 0, 0, 0);

   CSize siPage;
   pProject->GetPageDimension(siPage);
   double dScaleX = (double)siPage.cx / 720;
   if (dScaleX > 1.0)
      dScaleX = 1.0;
   double dScaleY = (double)siPage.cy / 540;
   if (dScaleY > 1.0)
      dScaleY = 1.0;

   int iLeftBorder = (int)((double)LEFT_BORDER * dScaleX);
   int iBottomBorder = (int)((double)BOTTOM_BORDER * dScaleY);

   rcSubmit.SetRect(iLeftBorder, 
                    siPage.cy - (iBottomBorder + BUTTON_HEIGHT), 
                    iLeftBorder + siMaxButton.cx, 
                    siPage.cy - (iBottomBorder + BUTTON_HEIGHT) + siMaxButton.cy);

   int iButtonOffset = (int)((double)BUTTON_OFFSET * dScaleX);
   rcDelete.SetRect(iLeftBorder + siMaxButton.cx + iButtonOffset, 
                    siPage.cy - (iBottomBorder + BUTTON_HEIGHT), 
                    iLeftBorder + siMaxButton.cx + iButtonOffset + siMaxButton.cx, 
                    siPage.cy - (iBottomBorder + BUTTON_HEIGHT) + siMaxButton.cy);

}

CQuestionnaireEx *CQuestionnaireEx::CreateDefaultQuestionnaire(CEditorProject *pProject)
{
   // TODO: Counter
   CQuestionStream *pQuestionStream = pProject->GetQuestionStream(false);
   UINT nCount = 1;
   if (pQuestionStream != NULL)
      nCount = pQuestionStream->GetQuestionnaireCount() + 1;

   CString csTitle;
   csTitle.Format(IDS_QUESTIONNAIRE_TITLE, nCount);
   
   CString csOk;
   csOk.Format(IDS_BUTTON_OK);
   
   CString csSubmit;
   csSubmit.Format(IDS_BUTTON_SUBMIT);
   
   CString csDelete;
   csDelete.Format(IDC_BUTTON_DELETE);

   bool bPassTypeRelative = true;
   UINT nMaximumPoints = 100;
   UINT nNeededPoints = 100;
   
   CMouseAction *pCorrectAction = new CMouseAction();
   pCorrectAction->Init(pProject, INTERACTION_JUMP_NEXT_PAGE, 0);

   CMouseAction *pFailureAction = new CMouseAction();
   pFailureAction->Init(pProject, INTERACTION_NO_ACTION, 0);

   CEditorDoc *pDoc = pProject->m_pDoc;

   CTimePeriod visualityPeriod;
   visualityPeriod.Init(INTERACTION_PERIOD_ALL_PAGES, 0, pDoc->m_displayEndMs);

   CArray<CFeedback *, CFeedback *> aFeedback;

   CString csSuccessFeedback;
   csSuccessFeedback.LoadString(IDS_ANSWER_SUCCESS);
   CFeedback *pSuccessFeedback = 
         CreateFeedback(pProject, NULL, FEEDBACK_TYPE_QQ_PASSED, csSuccessFeedback, 
                        false, visualityPeriod);  
   aFeedback.Add(pSuccessFeedback);
   
   CString csFailedFeedback;
   csFailedFeedback.LoadString(IDS_ANSWER_FAILED);
   CFeedback *pFailedFeedback = 
         CreateFeedback(pProject, NULL, FEEDBACK_TYPE_QQ_FAILED, csFailedFeedback, 
                        false, visualityPeriod);  
   aFeedback.Add(pFailedFeedback);

   CFeedback *pResultFeedback = CreateResultFeedback(pProject, NULL);
   aFeedback.Add(pResultFeedback);

   CQuestionnaireEx *pRetQuestionnaire = new CQuestionnaireEx();

   pRetQuestionnaire->Init(csTitle, true, true, true, false, 0,
      pCorrectAction, pFailureAction, bPassTypeRelative, nNeededPoints, 
      pProject, csOk, csSubmit, csDelete, &aFeedback);

   if (pCorrectAction)
      delete pCorrectAction;

   if (pFailureAction)
      delete pFailureAction;
  

   pRetQuestionnaire->SetDefaultFontAndColor();
   
   CString csImagePath = CButtonAppearanceSettings::GetButtonImagesPath();
   // TODO partly double code to InteractionButtonDialog.cpp and QuestionnaireButtonPage.cpp
   if (!csImagePath.IsEmpty())
   {
       csImagePath += _T("\\Aqua2\\Medium\\Blue\\");

       CString csFileNames[4];
       csFileNames[0] = csImagePath + _T("Passive.png");
       csFileNames[1] = csImagePath + _T("Normal.png");
       csFileNames[2] = csImagePath + _T("MouseOver.png");
       csFileNames[3] = csImagePath + _T("MouseDown.png");
       pRetQuestionnaire->SetButtonFilenames(csFileNames);
   }

   return pRetQuestionnaire;
}

////// static
CFeedback *CQuestionnaireEx::CreateFeedback(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire, 
                                     FeedbackTypeId feedbackType, 
                                     CString csFeedback, bool bDisabled, CTimePeriod &visualityPeriod)
{
   int OK_WIDTH = 80;
   int OK_HEIGHT = 20;
   int OFFSET_X = 20;
   int OFFSET_Y = 5;
   int MIN_FEEDBACK_WIDTH = OK_WIDTH + 2*OFFSET_X;

   CSize siPage;
   pProject->GetPageDimension(siPage);
   
   LOGFONT lf;
   LFont::FillWithDefaultValues(&lf);
   lf.lfHeight = -14;
   lf.lfWeight = FW_NORMAL;

   // create feedback rectangle and text          
   double dTextWidth = DrawSdk::Text::GetTextWidth(csFeedback, csFeedback.GetLength(), &lf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csFeedback, csFeedback.GetLength(), &lf);  
   double dTextAscent = DrawSdk::Text::GetTextAscent(&lf);


   double dRectangleWidth = dTextWidth + 2*OFFSET_X;
   if (dRectangleWidth < MIN_FEEDBACK_WIDTH)
      dRectangleWidth = MIN_FEEDBACK_WIDTH;

   double dRectangleHeight = 3*dTextHeight + OK_HEIGHT + OFFSET_Y;
   
   // test ok button
   
   CRect rcTestOk(0, 0, OK_WIDTH, OK_HEIGHT);
   
   CMouseAction testClickAction;
   testClickAction.Init(pProject, INTERACTION_NO_ACTION);
   CString csTestOk = _T("OK");
   CInteractionAreaEx *pTestOkButton = CreateQuestionnaireButton(pProject, pQuestionnaire, BUTTON_TYPE_OK,
                                                                &visualityPeriod, NULL, testClickAction,
                                                                rcTestOk, csTestOk);
   if (rcTestOk.Width() + 2*OFFSET_X > dRectangleWidth)
      dRectangleWidth = rcTestOk.Width() + 2*OFFSET_X;

   if (rcTestOk.Height() != OK_HEIGHT)
      dRectangleHeight = 3*dTextHeight + rcTestOk.Height() + OFFSET_Y;

   delete pTestOkButton;

   Gdiplus::RectF gdipRcFeedback;
   gdipRcFeedback.X = (float)(siPage.cx - dRectangleWidth) / 2;
   gdipRcFeedback.Y = (float)(siPage.cy - dRectangleHeight) / 2;
   gdipRcFeedback.Width = (float)dRectangleWidth;
   gdipRcFeedback.Height = (float)dRectangleHeight;
   
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;  
               
   DrawSdk::Rectangle *pFillRectangle = 
      new DrawSdk::Rectangle(gdipRcFeedback.X, gdipRcFeedback.Y, 
                             gdipRcFeedback.Width, gdipRcFeedback.Height,
                             RGB(22, 74, 122), RGB(236, 233, 216), 1.0, 0);
   aObjects.Add(pFillRectangle);
   
   DrawSdk::Rectangle *pLineRectangle = 
      new DrawSdk::Rectangle(gdipRcFeedback.X, gdipRcFeedback.Y, 
                             gdipRcFeedback.Width, gdipRcFeedback.Height,
                             RGB(22, 74, 122), 1.0, 0);
   aObjects.Add(pLineRectangle);
   
   double dXPos = gdipRcFeedback.X + (gdipRcFeedback.Width - dTextWidth) / 2;
   double dYPos = gdipRcFeedback.Y + dTextHeight + dTextAscent;
   DrawSdk::Text *pText = 
      new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight,
                        RGB(22, 74, 122), csFeedback, csFeedback.GetLength(), &lf);
   aObjects.Add(pText);
   
   // ok button

   CString csOk = _T("OK");
   if (pQuestionnaire)
      csOk = pQuestionnaire->GetOkText();
   Gdiplus::RectF gdipRcOk;
   gdipRcOk.X = (float)gdipRcFeedback.X + (gdipRcFeedback.Width - OK_WIDTH) / 2;
   gdipRcOk.Y = (float)(gdipRcFeedback.Y + 3 * dTextHeight);
   gdipRcOk.Width = (float)OK_WIDTH;
   gdipRcOk.Height = (float)OK_HEIGHT;

   CRect rcOk((int)gdipRcOk.X, (int)gdipRcOk.Y, (int)(gdipRcOk.X+gdipRcOk.Width), (int)(gdipRcOk.Y+gdipRcOk.Height));

   CMouseAction clickAction;
   clickAction.Init(pProject, INTERACTION_NO_ACTION);
   CInteractionAreaEx *pOkButton = CreateQuestionnaireButton(pProject, pQuestionnaire, BUTTON_TYPE_OK,
                                                             &visualityPeriod, NULL, clickAction,
                                                             rcOk, csOk);

   if (rcOk.Width() != OK_WIDTH)
   {
      float fOffsetX = (float)(OK_WIDTH - rcOk.Width()) / 2;
      pOkButton->Move(fOffsetX, 0);
   }
 
   
   CFeedback *pFeedback = new CFeedback();
   pFeedback->Init(feedbackType, bDisabled, false, csFeedback, &aObjects, pOkButton);
   
   for (int o=0; o<aObjects.GetSize(); ++o)
   {
      DrawSdk::DrawObject *pObject = aObjects[o];
      if (pObject)
         delete pObject;
   }

   return pFeedback;
}

CInteractionAreaEx *CQuestionnaireEx::CreateQuestionnaireButton(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire, 
                                                                ButtonTypeId idButtonType,
                                                                CTimePeriod *pVisualityPeriod, CTimePeriod *pActivityPeriod, 
                                                                CMouseAction &clickAction,
                                                                CRect &rcButton, CString &csButton)
{ 
   HRESULT hr = S_OK;
   
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aNormalObjects;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aMouseoverObjects;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aPressedObjects;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aPassivObjects;
  
   bool bIsImage = false;
   if (pQuestionnaire != NULL)
   {
      COLORREF clrFill, clrLine, clrText;
      LOGFONT lf;
      CString csFilename;
      
      CSize siImage(rcButton.Width(), rcButton.Height());
      CSize siButton;
      pQuestionnaire->CalculateButtonSize(BUTTON_TYPE_OTHER, siButton, (TCHAR *)(LPCTSTR)csButton);
      if (pQuestionnaire->ButtonsAreImageButtons())
      {
         bIsImage = true;
         rcButton.right = rcButton.left + siButton.cx;
         rcButton.bottom = rcButton.top + siButton.cy;
         siImage.SetSize(rcButton.Width(), rcButton.Height());
      }

      pQuestionnaire->GetNormalButtonValues(clrFill, clrLine, clrText, lf, csFilename);
      if (pQuestionnaire->ButtonsAreImageButtons())
      {
         hr = CInteractionAreaEx::CreateButtonObjects(&aNormalObjects, rcButton, siImage, 
            csFilename, csButton, clrText, &lf);
         
      }
      else
      {
	     CInteractionAreaEx::CalculateButtonSize(siButton, csButton, &lf);
		 if (siButton.cx > rcButton.Width())
			 rcButton.right = rcButton.left + siButton.cx;
		 if (siButton.cy > rcButton.Height())
			 rcButton.bottom = rcButton.top + siButton.cy;
         hr = CInteractionAreaEx::CreateButtonObjects(
            &aNormalObjects, rcButton, clrFill, clrLine, 1, 0, csButton, clrText, &lf);  
      }
      
      pQuestionnaire->GetMouseoverButtonValues(clrFill, clrLine, clrText, lf, csFilename);
      if (pQuestionnaire->ButtonsAreImageButtons())
      {
         hr = CInteractionAreaEx::CreateButtonObjects(&aMouseoverObjects, rcButton, siImage, 
            csFilename, csButton, clrText, &lf);
         
      }
      else
      {
		 if (siButton.cx > rcButton.Width())
			 rcButton.right = rcButton.left + siButton.cx;
		 if (siButton.cy > rcButton.Height())
			 rcButton.bottom = rcButton.top + siButton.cy;
         hr = CInteractionAreaEx::CreateButtonObjects(
            &aMouseoverObjects, rcButton, clrFill, clrLine, 1, 0, csButton, clrText, &lf);  
      }
      
      pQuestionnaire->GetPressedButtonValues(clrFill, clrLine, clrText, lf, csFilename);
      if (pQuestionnaire->ButtonsAreImageButtons())
      {
         hr = CInteractionAreaEx::CreateButtonObjects(&aPressedObjects, rcButton, siImage, 
            csFilename, csButton, clrText, &lf);
         
      }
      else
      {
		 if (siButton.cx > rcButton.Width())
			 rcButton.right = rcButton.left + siButton.cx;
		 if (siButton.cy > rcButton.Height())
			 rcButton.bottom = rcButton.top + siButton.cy;
         hr = CInteractionAreaEx::CreateButtonObjects(
            &aPressedObjects, rcButton, clrFill, clrLine, 1, 0, csButton, clrText, &lf);  
      }
      
      pQuestionnaire->GetInactiveButtonValues(clrFill, clrLine, clrText, lf, csFilename);
      if (pQuestionnaire->ButtonsAreImageButtons())
      {
         hr = CInteractionAreaEx::CreateButtonObjects(&aPassivObjects, rcButton, siImage, 
            csFilename, csButton, clrText, &lf);
         
      }
      else
      {
		 if (siButton.cx > rcButton.Width())
			 rcButton.right = rcButton.left + siButton.cx;
		 if (siButton.cy > rcButton.Height())
			 rcButton.bottom = rcButton.top + siButton.cy;
         hr = CInteractionAreaEx::CreateButtonObjects(
            &aPassivObjects, rcButton, clrFill, clrLine, 1, 0, csButton, clrText, &lf);  
      }
   }
   else
   {
      LOGFONT lf;
      LFont::FillWithDefaultValues(&lf);
      lf.lfHeight = -14;
      lf.lfWeight = FW_NORMAL;
      hr = CInteractionAreaEx::CreateButtonObjects(
         &aNormalObjects, rcButton, CQuestionnaireEx::NORMAL_BUTTON_BG, CQuestionnaireEx::NORMAL_BUTTON_LINE,
         1, 0, csButton, CQuestionnaireEx::NORMAL_BUTTON_TEXT, &lf);  
      hr = CInteractionAreaEx::CreateButtonObjects(
         &aMouseoverObjects, rcButton, CQuestionnaireEx::MOUSEO_BUTTON_BG, CQuestionnaireEx::MOUSEO_BUTTON_LINE,
         1, 0, csButton, CQuestionnaireEx::MOUSEO_BUTTON_TEXT, &lf);  
      hr = CInteractionAreaEx::CreateButtonObjects(
         &aPressedObjects, rcButton, CQuestionnaireEx::PRESSED_BUTTON_BG, CQuestionnaireEx::PRESSED_BUTTON_LINE,
         1, 0, csButton, CQuestionnaireEx::PRESSED_BUTTON_TEXT, &lf);  
      hr = CInteractionAreaEx::CreateButtonObjects(
         &aPassivObjects, rcButton, CQuestionnaireEx::PASSIV_BUTTON_BG, CQuestionnaireEx::PASSIV_BUTTON_LINE,
         1, 0, csButton, CQuestionnaireEx::PASSIV_BUTTON_TEXT, &lf);  
   }
   
   CInteractionAreaEx *pButton = new CInteractionAreaEx();

   pButton->Init(&rcButton, false, &clickAction, NULL, NULL, NULL, NULL, 
                   pVisualityPeriod, pActivityPeriod, pProject);  
   pButton->InitButton(idButtonType, &aNormalObjects, &aMouseoverObjects, &aPressedObjects, &aPassivObjects, bIsImage);
      
   for (int o=0; o<aPassivObjects.GetSize(); ++o)
   {
      DrawSdk::DrawObject *pObject = aPassivObjects[o];
      if (pObject)
         delete pObject;
   }
   for (o=0; o<aNormalObjects.GetSize(); ++o)
   {
      DrawSdk::DrawObject *pObject = aNormalObjects[o];
      if (pObject)
         delete pObject;
   }
   for (o=0; o<aMouseoverObjects.GetSize(); ++o)
   {
      DrawSdk::DrawObject *pObject = aMouseoverObjects[o];
      if (pObject)
         delete pObject;
   }
   for (o=0; o<aPressedObjects.GetSize(); ++o)
   {
      DrawSdk::DrawObject *pObject = aPressedObjects[o];
      if (pObject)
         delete pObject;
   }

   return pButton;
}


// private
HRESULT CQuestionnaireEx::SetDefaultFontAndColor()
{
   LOGFONT defaultFont;
   LFont::FillWithDefaultValues(&defaultFont);
   defaultFont.lfHeight = -14;
   defaultFont.lfWeight = FW_NORMAL;
   
   COLORREF clrFill[4];
   COLORREF clrLine[4];
   COLORREF clrText[4];
   LOGFONT lf[4];
   clrFill[0] = CQuestionnaireEx::PASSIV_BUTTON_BG;
   clrLine[0] = CQuestionnaireEx::PASSIV_BUTTON_LINE;
   clrText[0] = CQuestionnaireEx::PASSIV_BUTTON_TEXT;
   lf[0] = defaultFont;
   
   clrFill[1] = CQuestionnaireEx::NORMAL_BUTTON_BG;
   clrLine[1] = CQuestionnaireEx::NORMAL_BUTTON_LINE;
   clrText[1] = CQuestionnaireEx::NORMAL_BUTTON_TEXT;
   lf[1] = defaultFont;
   
   clrFill[2] = CQuestionnaireEx::MOUSEO_BUTTON_BG;
   clrLine[2] = CQuestionnaireEx::MOUSEO_BUTTON_LINE;
   clrText[2] = CQuestionnaireEx::MOUSEO_BUTTON_TEXT;
   lf[2] = defaultFont;
   
   clrFill[3] = CQuestionnaireEx::PRESSED_BUTTON_BG;
   clrLine[3] = CQuestionnaireEx::PRESSED_BUTTON_LINE;
   clrText[3] = CQuestionnaireEx::PRESSED_BUTTON_TEXT;
   lf[3] = defaultFont;

   SetButtonColor(clrFill, clrLine, clrText);
   SetButtonFont(lf);

   return S_OK;
}

// private
HRESULT CQuestionnaireEx::GetAllFeedback(CArray<CFeedback *, CFeedback *> *paFeedback)
{
   HRESULT hr = S_OK;

   if (paFeedback == NULL)
      return E_POINTER;

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
   aObjects.SetSize(0, 40);

   CInteractionStream *pInteractionStream = m_pEditorProject->GetInteractionStream(false);

   if (pInteractionStream != NULL)
   {
      CTimePeriod periodAll;
      periodAll.Init(INTERACTION_PERIOD_ALL_PAGES, 0, 1);

      hr = pInteractionStream->FindInteractions(&periodAll, true, &aObjects);
   }

   if (SUCCEEDED(hr))
   {
      for (int i=0; i<aObjects.GetSize(); ++i)
      {
         if (aObjects[i]->GetClassId() == INTERACTION_CLASS_FEEDBACK)
         {
            CFeedback *pFeedback = (CFeedback *)aObjects[i];
            if (pFeedback->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES)
               paFeedback->Add(pFeedback);
         }
      }
   }

   POSITION pos = m_mapFeedbackNotVisible.GetStartPosition();
   while (pos != NULL)
   {
      FeedbackTypeId idKey;
      CFeedback *pFeedback = NULL;
      m_mapFeedbackNotVisible.GetNextAssoc(pos, idKey, pFeedback);
      
      paFeedback->Add(pFeedback);
   }
   
   return hr;
}

HRESULT CQuestionnaireEx::WriteDefaultButtonConfig(CFileOutput *pFileOutput, LPCTSTR tszLepFilename)
{
   if (pFileOutput == NULL)
      return E_POINTER;

   _TCHAR tszOutput[1000];

   pFileOutput->Append(_T("<default>\n"));

   for (int i=0; i<4; ++i)
   {
      _TCHAR *tszButtonType = _T("passive");
      if (i == 1)
         tszButtonType = _T("normal");
      else if (i == 2)
         tszButtonType = _T("over");
      else if (i == 3)
         tszButtonType = _T("down");

      _stprintf(tszOutput, _T("<button-config type=\"%s\">\n"), tszButtonType);
      pFileOutput->Append(tszOutput);

      _stprintf(tszOutput, _T("<colors bg=\"%d\" line=\"%d\" text=\"%d\"></colors>\n"),
         m_clrFill[i], m_clrLine[i], m_clrText[i]);
      pFileOutput->Append(tszOutput);

      _stprintf(tszOutput,  _T("<font family=\"%s\" size=\"%d\" weight=\"%d\" slant=\"%d\"></font>\n"), 
         m_lf[i].lfFaceName, m_lf[i].lfHeight, m_lf[i].lfWeight, m_lf[i].lfItalic);
      pFileOutput->Append(tszOutput);

      if (!m_csFilename[i].IsEmpty())
      {
         // TODO path might contain WCHAR?

         CString csOutputPath;
         
         if (tszLepFilename != NULL)
            csOutputPath = StringManipulation::MakeRelativeAndSgml(tszLepFilename, m_csFilename[i]);
         else
            StringManipulation::TransformForSgml(m_csFilename[i], csOutputPath);
      
         _stprintf(tszOutput, _T("<file path=\"%s\"></file>\n"), csOutputPath);
         pFileOutput->Append(tszOutput);
      }

      pFileOutput->Append(_T("</button-config>\n"));

   }

   pFileOutput->Append(_T("</default>\n"));

   return S_OK;
}

HRESULT CQuestionnaireEx::CalculateButtonSize(ButtonTypeId idButtonType, CSize &siButton, TCHAR *szButton)
{
   HRESULT hr = S_OK;

   siButton.SetSize(0, 0);

   COLORREF clrFill, clrLine, clrText;
   LOGFONT lf;
   CString csFilename;
   if (ButtonsAreImageButtons())
   {
      CSize siTmpButton;
      GetNormalButtonValues(clrFill, clrLine, clrText, lf, csFilename);
      CInteractionAreaEx::CalculateButtonSize(siTmpButton, csFilename);
      siButton = siTmpButton;

      GetMouseoverButtonValues(clrFill, clrLine, clrText, lf, csFilename);
      CInteractionAreaEx::CalculateButtonSize(siTmpButton, csFilename);
      siButton.cx = 
         siTmpButton.cx > siButton.cx ? siTmpButton.cx : siButton.cx;
      siButton.cy = 
         siTmpButton.cy > siButton.cy ? siTmpButton.cy : siButton.cy;

      GetPressedButtonValues(clrFill, clrLine, clrText, lf, csFilename);
      CInteractionAreaEx::CalculateButtonSize(siTmpButton, csFilename);
      siButton.cx = 
         siTmpButton.cx > siButton.cx ? siTmpButton.cx : siButton.cx;
      siButton.cy = 
         siTmpButton.cy > siButton.cy ? siTmpButton.cy : siButton.cy;

      GetInactiveButtonValues(clrFill, clrLine, clrText, lf, csFilename);
      CInteractionAreaEx::CalculateButtonSize(siTmpButton, csFilename);
      siButton.cx = 
         siTmpButton.cx > siButton.cx ? siTmpButton.cx : siButton.cx;
      siButton.cy = 
         siTmpButton.cy > siButton.cy ? siTmpButton.cy : siButton.cy;
   }
   else
   {
      CSize siTmpButton;
      CString csButtonText;
      if (idButtonType == BUTTON_TYPE_SUBMIT)
         csButtonText = GetSubmitText();
      else if (idButtonType == BUTTON_TYPE_DELETE)
         csButtonText = GetDeleteText();
      else if (idButtonType == BUTTON_TYPE_OK)
         csButtonText = GetOkText();
      else if (szButton != NULL)
         csButtonText = szButton;
      else
         hr = E_FAIL;

      if (SUCCEEDED(hr))
      {
         // Normal button size
         GetNormalButtonValues(clrFill, clrLine, clrText, lf, csFilename);
         CInteractionAreaEx::CalculateButtonSize(siTmpButton, csButtonText, &lf);
         siButton = siTmpButton;

         // Mouse over button size
         GetMouseoverButtonValues(clrFill, clrLine, clrText, lf, csFilename);
         CInteractionAreaEx::CalculateButtonSize(siTmpButton, csButtonText, &lf);
         siButton.cx = 
            siTmpButton.cx > siButton.cx ? siTmpButton.cx : siButton.cx;
         siButton.cy = 
            siTmpButton.cy > siButton.cy ? siTmpButton.cy : siButton.cy;

         // Pressed button size
         GetPressedButtonValues(clrFill, clrLine, clrText, lf, csFilename);
         CInteractionAreaEx::CalculateButtonSize(siTmpButton, csButtonText, &lf);
         siButton.cx = 
            siTmpButton.cx > siButton.cx ? siTmpButton.cx : siButton.cx;
         siButton.cy = 
            siTmpButton.cy > siButton.cy ? siTmpButton.cy : siButton.cy;

         // Inactive button size
         GetInactiveButtonValues(clrFill, clrLine, clrText, lf, csFilename);
         CInteractionAreaEx::CalculateButtonSize(siTmpButton, csButtonText, &lf);
         siButton.cx = 
            siTmpButton.cx > siButton.cx ? siTmpButton.cx : siButton.cx;
         siButton.cy = 
            siTmpButton.cy > siButton.cy ? siTmpButton.cy : siButton.cy;

         if (idButtonType == BUTTON_TYPE_OK)
         {
            if (siButton.cx < OK_WIDTH)
               siButton.cx = OK_WIDTH;
            if (siButton.cy < OK_HEIGHT)
               siButton.cy = OK_HEIGHT;
         }
         else
         {
            siButton.cx += BUTTON_X_OFFSET;
            siButton.cy += BUTTON_Y_OFFSET;
         }
      }
   }

   return hr;
}

bool CQuestionnaireEx::RandomizeQuestionnaire() {
    if (m_aQuestionPointers.GetSize() <= 0) {
        return false;
    }

    ASSERT(m_bIsRandomTest);
    ASSERT(m_iNumberOfRandomQuestions > 0);
    ASSERT(m_iNumberOfRandomQuestions <= m_aQuestionPointers.GetSize());

    if (m_iNumberOfRandomQuestions > m_aQuestionPointers.GetSize()) {
        m_iNumberOfRandomQuestions = GetQuestionTotal();
    }
    if (m_iNumberOfRandomQuestions == 0)
        m_iNumberOfRandomQuestions = 1;

    // if there are any randomized questions remove them.
    m_aRandomizedQuestionPointers.RemoveAll();
    m_aUnPeekedRandomizedQuestions.RemoveAll();

    // Create m_aUnPeekedRandomizedQuestions as unique and not NULL question pointers. m_aQuestionPointers can contain a question pointer several times
    for (int i = 0; i < m_aQuestionPointers.GetSize(); i++) {
        if (m_aQuestionPointers.GetAt(i) != NULL) {
            bool bFound = false;
            for (int j = 0; j < m_aUnPeekedRandomizedQuestions.GetSize(); j++) {
                if (m_aUnPeekedRandomizedQuestions.GetAt(j) == m_aQuestionPointers.GetAt(i)) {
                    bFound = true;
                    break;
                }
            }
            if (bFound == false)
                m_aUnPeekedRandomizedQuestions.Add(m_aQuestionPointers.GetAt(i));
        }
        else 
            ASSERT(false); // question inside array must be valid
    }

    for (int i = 0; i < m_iNumberOfRandomQuestions; i++) {
        int iRandValue = rand();
        int randIndex = iRandValue % m_aUnPeekedRandomizedQuestions.GetSize();
        m_aRandomizedQuestionPointers.Add(m_aUnPeekedRandomizedQuestions.GetAt(randIndex));
        m_aUnPeekedRandomizedQuestions.RemoveAt(randIndex);
    }

    // Unregister questions that were not peeked during randomization from questionnaire.
    PrepareDocumentForRandomizePlay();

    // Change jump targets of all quesionnaire's questions
    for (int i = 0; i < m_aRandomizedQuestionPointers.GetSize(); i++) {
        CMouseAction *pCorrectAction = m_aRandomizedQuestionPointers.GetAt(i)->GetCorrectAction();
        CMouseAction *pFailureAction = m_aRandomizedQuestionPointers.GetAt(i)->GetFailureAction();
        if (!pCorrectAction) {
            pCorrectAction = new CMouseAction();
            m_aRandomizedQuestionPointers.GetAt(i)->SetCorrectAction(pCorrectAction);
        }
        pCorrectAction->SetActionId(INTERACTION_JUMP_RANDOM_QUESTION);

        if (pFailureAction)
            pFailureAction->SetActionId(INTERACTION_JUMP_RANDOM_QUESTION); 
        else {
            pFailureAction = pCorrectAction->Copy();
            m_aRandomizedQuestionPointers.GetAt(i)->SetFailureAction(pFailureAction);
        }
    }

    if (m_pCorrectAction != NULL) {
        m_pCorrectActionBackup = m_pCorrectAction->Copy();
        ConvertAction(m_pCorrectAction);
    }
    if (m_pFailureAction != NULL) {
        m_pFailureActionBackup = m_pFailureAction->Copy();
        ConvertAction(m_pFailureAction);
    }
    return true;
}

void CQuestionnaireEx::ConvertAction(CMouseAction *pAction) {
    ASSERT(m_bIsRandomTest);
    ASSERT(m_pEditorProject);
    if (!m_bIsRandomTest || !m_pEditorProject)
        return;

    CPage *pPage = NULL;
    switch (pAction->GetActionId()) {
    case INTERACTION_START_REPLAY:
    case INTERACTION_JUMP_RANDOM_QUESTION:
        ASSERT(false); // Currently user can not set this action.
        break;
    case INTERACTION_JUMP_NEXT_PAGE:
        {
            int iLastQuestionEndTimestamp = m_pEditorProject->GetLastQuestionEndTimeStamp() + 1;
            if (m_pEditorProject->m_pDoc && iLastQuestionEndTimestamp < m_pEditorProject->m_pDoc->m_docLengthMs) 
                pPage = m_pEditorProject->FindPageAt(iLastQuestionEndTimestamp);
            break;
        }
    case INTERACTION_JUMP_PREVIOUS_PAGE:
        {
            int iFirstQuestionBeginTimestamp = m_pEditorProject->GetFirstQuestionBeginTimeStamp();
            if (iFirstQuestionBeginTimestamp > 0)
                pPage = m_pEditorProject->FindPageAt(iFirstQuestionBeginTimestamp - 1);
            break;
        }
    default:
        return;
    }
    if (pPage) {
        pAction->SetActionId(INTERACTION_JUMP_SPECIFIC_PAGE); 
        pAction->SetPageMarkId(pPage->GetJoinId());
    } else {
        pAction->SetActionId(INTERACTION_NO_ACTION); 
    }
}

void CQuestionnaireEx::PrepareDocumentForRandomizePlay() {
    SetRandomQuestionReplayIndex(0);

    // backup only jump targets so we'll have a minimum impact on the entire project.
    // During play no change of structure is allowed so we can have a 1 to 1 map of arrays.
    for (int i = 0; i< m_aQuestionMouseActions.GetSize(); i++) {
        delete m_aQuestionMouseActions.GetAt(i);
    }
    m_aQuestionMouseActions.RemoveAll();
    for (int i = 0; i< m_aQuestionPointers.GetSize(); i++) {
        CQuestionEx *pQuestion = m_aQuestionPointers.GetAt(i);
        m_aQuestionMouseActions.Add(new QuestionMouseActions(pQuestion, // same question poiner
            pQuestion != NULL && pQuestion->GetCorrectAction() != NULL ? pQuestion->GetCorrectAction() : NULL,
            pQuestion != NULL && pQuestion->GetFailureAction() != NULL ? pQuestion->GetFailureAction() : NULL));
        ASSERT(m_aQuestionMouseActions.GetAt(i)->m_pBelongsTo == pQuestion);
    }

    for (int i = 0; i< m_aUnPeekedRandomizedQuestions.GetSize(); i++) {
        UnregisterQuestion(m_aUnPeekedRandomizedQuestions.GetAt(i));
    }
}

void CQuestionnaireEx::RestoreDocumentFromRandomizePlay() {
    // restore jump targets
    ASSERT(m_aQuestionMouseActions.GetSize() == m_aQuestionMouseActions.GetSize());
    for (int i = 0; i< m_aQuestionMouseActions.GetSize(); i++) {
        for (int j = 0; j < m_aQuestionPointers.GetSize(); j++) {
            CQuestionEx *pQuestion = m_aQuestionPointers.GetAt(j);
            QuestionMouseActions *pQuestionMouseAction = m_aQuestionMouseActions.GetAt(i);
            if (pQuestionMouseAction->m_pBelongsTo == pQuestion) {
                if (pQuestionMouseAction) {
                    if (pQuestionMouseAction->m_pCorrectAction)
                        pQuestionMouseAction->m_pCorrectAction->CloneTo(pQuestion->GetCorrectAction());
                    else {
                        CMouseAction *pAction = pQuestion->GetCorrectAction();
                        SAFE_DELETE(pAction);
                        pQuestion->SetCorrectAction(NULL);
                    }

                    if (pQuestionMouseAction->m_pFailureAction)
                        pQuestionMouseAction->m_pFailureAction->CloneTo(pQuestion->GetFailureAction());
                    else {
                        CMouseAction *pAction = pQuestion->GetFailureAction();
                        SAFE_DELETE(pAction);
                        pQuestion->SetFailureAction(NULL);
                    }
                }
                break;
            }
        }
    }

    for (int i = 0; i< m_aUnPeekedRandomizedQuestions.GetSize(); i++) {
        // Double check: see if question is not already registered.
        bool bAlreadyRegistered = false;
        for (int j=0; j<m_aQuestionPointers.GetSize(); ++j)
        {
            if (m_aUnPeekedRandomizedQuestions.GetAt(i) == m_aQuestionPointers.GetAt(j)) {
                bAlreadyRegistered = true;
                break;
            }
        }
        if (!bAlreadyRegistered)
            RegisterQuestion(m_aUnPeekedRandomizedQuestions.GetAt(i));
    }

    if (m_pCorrectActionBackup) {
        m_pCorrectActionBackup->CloneTo(m_pCorrectAction);
        SAFE_DELETE(m_pCorrectActionBackup);
    }
    if (m_pFailureActionBackup) {
        m_pFailureActionBackup->CloneTo(m_pFailureAction);
        SAFE_DELETE(m_pFailureActionBackup);
    }
    m_pEditorProject->StartRandomTest(false);
}

int CQuestionnaireEx::GetRandomQuestionIndex(CQuestionEx *pQuestion) {
    for (int i = 0; i< m_aRandomizedQuestionPointers.GetSize(); i++) {
        if (m_aRandomizedQuestionPointers.GetAt(i) == pQuestion) {
            return i + 1;
        }
    }
    return -1;
}

CQuestionEx *CQuestionnaireEx::GetRandomQuestionFromPosition(int iIndex) {
    if (iIndex >= 0 && iIndex < m_aRandomizedQuestionPointers.GetSize())
        return m_aRandomizedQuestionPointers.GetAt(iIndex);
    return NULL;
}

// Sets the random question number
void CQuestionnaireEx::SetNumberOfRandomQuestions(UINT nNumberOfRandomQuestions) {
    m_iNumberOfRandomQuestions = nNumberOfRandomQuestions; 
}

bool CQuestionnaireEx::IsRandomTest() {
    return m_bIsRandomTest && m_pEditorProject && m_pEditorProject->IsRandomTestEnabled();
}

int CQuestionnaireEx::GetRandomQuestionReplayIndex() {
    return m_iRandomQuestionReplayIndex;
}

void CQuestionnaireEx::SetRandomQuestionReplayIndex(int iRandomQuestionReplayIndex) {
    m_iRandomQuestionReplayIndex = iRandomQuestionReplayIndex;
}

CQuestionnaireEx::QuestionMouseActions::QuestionMouseActions (CQuestionEx *pBelongsTo, CMouseAction *pCorrectAction, CMouseAction *pFailureAction) {
    m_pBelongsTo = pBelongsTo;
    
    if (pCorrectAction) {
        m_pCorrectAction = new CMouseAction();
        pCorrectAction->CloneTo(m_pCorrectAction);
    } else {
        m_pCorrectAction = NULL;
    }

    if (pFailureAction) {
        m_pFailureAction = new CMouseAction();
        pFailureAction->CloneTo(m_pFailureAction);
    } else {
        m_pFailureAction = NULL;
    }
}

CQuestionnaireEx::QuestionMouseActions::~QuestionMouseActions (){
    m_pBelongsTo = NULL;
    SAFE_DELETE(m_pCorrectAction);
    SAFE_DELETE(m_pFailureAction);
}
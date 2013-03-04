#include "stdafx.h"
#include "QuestionStream.h"
#include "EditorDoc.h"
#include "BlankGroup.h"
#include "ChangeableObject.h"
#include "DynamicObject.h"
#include "MainFrm.h"
#include "MoveableObjects.h"
#include "LanguageSupport.h" // lsutl32


CQuestionStream::CQuestionStream()
{
   m_aQuestions.SetSize(0, 10);
   m_aQuestionnaires.SetSize(0, 5);
   m_bDoLoopTimer = false;
   m_pLastTimerQuestion = NULL;
   m_pThread = NULL;
   m_bQuestionSuccess = false;
   m_pAnsweredQuestion = NULL;
   m_ppLastClipboardData = NULL;
   m_nLastClipboardCount = 0;
   m_iFeedbackStartPosMs = 0;

   m_nProjectValid = ID_VALID_CHECK;
}

CQuestionStream::~CQuestionStream()
{
   m_nProjectValid = 0;

   if (m_bDoLoopTimer)
   {
      m_bDoLoopTimer = false;

      // happens when PausePreview() (and ResetAfterPreview()) was not called correctly

      ::Sleep(501); // wait for the thread to finish (dirty and maybe sometimes wrong solution)
   }

   for (int i=0; i<m_aQuestions.GetSize(); ++i)
      delete m_aQuestions[i];
   
   for (i=0; i<m_aQuestionnaires.GetSize(); ++i)
      delete m_aQuestionnaires[i];

   if (m_ppLastClipboardData != NULL)
   {
      for (i=0; i<(signed)m_nLastClipboardCount; ++i)
         delete m_ppLastClipboardData[i];
      delete[] m_ppLastClipboardData;
   }
}

HRESULT CQuestionStream::Insert(UINT nToMs, UINT nLength, CWhiteboardStream *pWhiteboard)
{
   HRESULT hr = S_OK;

   if (pWhiteboard != NULL && pWhiteboard->ContainsQuestions())
   {
      UINT nElementCount = 0;
      hr = pWhiteboard->ExtractQuestionnaires(NULL, &nElementCount);

      CQuestionnaire **apQuestionnaires = new CQuestionnaire*[nElementCount];
      ZeroMemory(apQuestionnaires, nElementCount * sizeof (CQuestionnaire*));
      
      if (SUCCEEDED(hr))
         hr = pWhiteboard->ExtractQuestionnaires(apQuestionnaires, &nElementCount);

      if (SUCCEEDED(hr))
         hr = Insert(nToMs, nLength, apQuestionnaires, nElementCount);

      if (apQuestionnaires != NULL)
      {
         if (SUCCEEDED(hr))
         {
            for (int i=0; i<(signed)nElementCount; ++i)
              delete apQuestionnaires[i];
         }

         delete[] apQuestionnaires;
      }
   }
   else
      hr = InsertRange(nToMs, nLength, NULL, 0, true); // move other questions
  
   if (SUCCEEDED(hr))
      m_nStreamLength += nLength;

   return hr;
}

HRESULT CQuestionStream::InsertDenver(UINT nToMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   for (int i=0; i<m_aQuestions.GetSize() && SUCCEEDED(hr); ++i)
   {
      if (m_aQuestions[i]->PageContains(nToMs))
      {
         CQuestionEx *pCopyQuestion = m_aQuestions[i]->Copy();
         
         if (pCopyQuestion != NULL)
         {
            hr = pCopyQuestion->Enlarge(nToMs, nLengthMs);
            if (SUCCEEDED(hr))
               hr = ChangeQuestion(m_aQuestions[i], pCopyQuestion, false, false);
         }
         else
            hr = E_FAIL;
      }
      else if (m_aQuestions[i]->BeginsAtOrAfter(nToMs))
      {
         hr = ChangeTimes(m_aQuestions[i], nLengthMs);
      }
   }

/* must also be moved
   CQuestionEx *pQuestion = FindQuestionForTime(nToMs);

   if (pQuestion != NULL)
   {
      CQuestionEx *pCopyQuestion = pQuestion->Copy();
      
      if (pCopyQuestion != NULL)
      {
         hr = pCopyQuestion->Enlarge(nToMs, nLengthMs);
         if (SUCCEEDED(hr))
            hr = ChangeQuestion(pQuestion, pCopyQuestion, false, false);
      }
      else
         hr = E_FAIL;
   }
   */
   
   if (SUCCEEDED(hr))
      m_nStreamLength += nLengthMs;
 
   
   if (m_pUndoManager != NULL && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(
         this, UNDO_INSERT_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nToMs, nLengthMs, false);
      // dummy entry to shorten the stream upon undo/redo
   }
   
   return hr;
}
   
 
HRESULT CQuestionStream::ResolveJumpTimes(UINT nMoveMs)
{
   HRESULT hr = S_OK;

   // BUGFIX #4259: also resolve the test actions
   for (int i=0; i<m_aQuestionnaires.GetSize() && SUCCEEDED(hr); ++i)
   {
      hr = m_aQuestionnaires[i]->ResolveJumpTimes(m_pEditorProject, nMoveMs);
   }
   
   for (int i=0; i<m_aQuestions.GetSize() && SUCCEEDED(hr); ++i)
   {
      hr = m_aQuestions[i]->ResolveJumpTimes(m_pEditorProject, nMoveMs);
   }

   return hr;
}  

HRESULT CQuestionStream::InsertSingle(CQuestionEx *pQuestion, CQuestionnaireEx *pBelongsTo, 
                                      bool bUserInteraction, bool bDoCollisionCheck)
{
   HRESULT hr = S_OK;

   if (pQuestion == NULL || pBelongsTo == NULL)
      return E_POINTER;

   if (pQuestion->GetBelongsTo() != NULL && pQuestion->GetBelongsTo() != pBelongsTo)
      return E_INVALIDARG;

   // Note: AddObjects() was and has to be done before and outside.

   
   for (int i=0; i<m_aQuestions.GetSize() + 1; ++i)
   {
      // Note: i == m_aQuestions.GetSize() is the "stop" element

      if (i == m_aQuestions.GetSize() || m_aQuestions[i]->BeginsAtOrAfter(pQuestion->GetPageBegin()))
      {
         if (bDoCollisionCheck && i < m_aQuestions.GetSize() && 
            (pQuestion->Intersects(m_aQuestions[i]) || (i > 0 && pQuestion->Intersects(m_aQuestions[i-1]))))
            return E_INVALIDARG; // collision
      
         // check for scorm id
         CString csId = pQuestion->GetScormId();
         bool bDummy = false;
         if (csId.IsEmpty() || m_mapScormIds.Lookup(csId, bDummy))
         {
            csId = CreateScormId(csId);
            pQuestion->SetScormId(csId);
         }

         m_mapScormIds.SetAt(csId, true);

         m_aQuestions.InsertAt(i, pQuestion);
         hr = pQuestion->CommitObjects();
         if (SUCCEEDED(hr))
         {
            pBelongsTo->RegisterQuestion(pQuestion);
            pQuestion->SetBelongsTo(pBelongsTo);

            if (pBelongsTo->IsRandomTest()) {
                if (pBelongsTo->GetNumberOfRandomQuestions() == 0)
                    pBelongsTo->SetNumberOfRandomQuestions(1);
                if (m_pEditorProject != NULL && m_pEditorProject->m_bOpening == false 
                    && pBelongsTo->GetNumberOfRandomQuestions() == pBelongsTo->GetQuestionTotal() - 1)
                    pBelongsTo->SetNumberOfRandomQuestions(pBelongsTo->GetNumberOfRandomQuestions() + 1);
            }
         }

         if (m_pUndoManager != NULL && SUCCEEDED(hr))
         {
            // pointers used: make a copy for the undo stack
            // use an array because of "ReleaseData()"
            CQuestionEx **paUndo = new CQuestionEx*[1];
            paUndo[0] = pQuestion;
            
            if (SUCCEEDED(hr))
            {
               hr = m_pUndoManager->RegisterAction(this, UNDO_INSERT_OBJECT, 
                  (BYTE *)paUndo, sizeof (CQuestionEx*), HINT_CHANGE_PAGE, 
                  pQuestion->GetPageBegin(), IUNDO_OBJECT_QUESTION, false);
            }

            // this is an action only concerning this stream
            if (SUCCEEDED(hr) && bUserInteraction)
               hr = m_pUndoManager->SetRollbackPoint();
            // else !bIsUserInteraction: probably an undo which handles rollback points itself
         }
         
         break;
      }
   }

   return hr;
}

HRESULT CQuestionStream::DeleteSingle(CQuestionEx *pQuestion, bool bRemoveObjects, bool bDoRollback)
{
   HRESULT hr = S_OK;

   if (pQuestion == NULL)
      return E_POINTER;

   if (pQuestion->GetBelongsTo() == NULL)
      return E_INVALIDARG;


   for (int i=0; i<m_aQuestions.GetSize(); ++i)
   {
      if (m_aQuestions[i] == pQuestion)
      {
         CString csId = pQuestion->GetScormId();
         m_mapScormIds.RemoveKey(csId);


         // TODO maybe delete empty questionnaires?
         
         if (m_pUndoManager != NULL)
         {
            // pointers used: make a copy for the undo stack
            // use an array because of "ReleaseData()"
            CQuestionEx **paUndo = new CQuestionEx*[1];
            paUndo[0] = m_aQuestions[i];
            
            HRESULT hr = S_OK;

            if (SUCCEEDED(hr))
            {
               hr = m_pUndoManager->RegisterAction(this, UNDO_REMOVE_OBJECT, 
                  (BYTE *)paUndo, sizeof (CQuestionEx*), 
                  HINT_CHANGE_PAGE, pQuestion->GetPageBegin(), IUNDO_OBJECT_QUESTION, false);
            }
            
            // Note: so the object pointed to by m_aQuestions[i] is not
            // really deleted as it is reused on the undo stack instead.
         }
         else
         {
            _ASSERT(false);
            // should not happen
         }

         m_aQuestions.RemoveAt(i);

         // objects have to be deleted after the question;
         // especially for undo information, otherwise you get the wrong
         // ordering upon reinsertion
         CQuestionnaireEx *pBelongsTo = pQuestion->GetBelongsTo();
         if (bRemoveObjects)
            pQuestion->RemoveObjects();    
         pBelongsTo->UnregisterQuestion(pQuestion);
         if (pBelongsTo->IsRandomTest() && pBelongsTo->GetNumberOfRandomQuestions() > pBelongsTo->GetQuestionTotal()) {
             if (pBelongsTo->GetQuestionTotal() > 0)
                pBelongsTo->SetNumberOfRandomQuestions(pBelongsTo->GetQuestionTotal());
         }
         
         
         // this is an action only concerning this stream
         if (SUCCEEDED(hr) && m_pUndoManager != NULL && bDoRollback)
            hr = m_pUndoManager->SetRollbackPoint();
         // else !bIsUserInteraction: probably an undo which handles rollback points itself

         return S_OK;
      }
   }

   return S_FALSE;
}

HRESULT CQuestionStream::ChangeQuestion(CQuestionEx *pOld, CQuestionEx *pNew,
                                        bool bUserInteraction, bool bDoCollisionCheck)
{
   HRESULT hr = S_OK;

   if (pOld == NULL || pNew == NULL)
      return E_POINTER;

   bool bFound = false;
   for (int i=0; i<m_aQuestions.GetSize(); ++i)
   {
      if (pOld == m_aQuestions[i])
      {
         bFound = true;

         CQuestionnaireEx *pNewBelongsTo = pNew->GetBelongsTo();
         if (pNewBelongsTo == NULL)
            pNewBelongsTo = m_aQuestions[i]->GetBelongsTo(); // take the old questionnaire
         
         if (pNewBelongsTo == NULL)
         {
            _ASSERT(false);
            return E_FAIL; // should not happen
         }

         UINT nOldJoinId = pOld->GetJoinId();

         if (m_ppLastClipboardData != NULL && bUserInteraction)
         {
            for (i=0; i<(signed)m_nLastClipboardCount; ++i)
            {
               if (m_ppLastClipboardData[i]->GetJoinId() == nOldJoinId)
               {
                  CString csMessage;
                  csMessage.LoadString(IDS_CLIPBOARD_DELETED);

                  
                  if (m_pEditorProject != NULL && m_pEditorProject->m_pDoc != NULL)
                  m_pEditorProject->m_pDoc->GetMainFrame()->ShowTimedStatusText(csMessage, 10000);

                  m_pEditorProject->ClearStreamClipboard();
               }
            }
         }

         if (SUCCEEDED(hr))
            hr = DeleteSingle(pOld, bUserInteraction, false);

         if (SUCCEEDED(hr))
            pNew->SetJoinId(nOldJoinId);

         if (SUCCEEDED(hr))
            hr = InsertSingle(pNew, pNewBelongsTo, bUserInteraction, bDoCollisionCheck);
         // maybe also sets a rollback point

         break;
      }
   }

   if (SUCCEEDED(hr) && !bFound)
      return E_INVALIDARG;

   return hr;
}

CQuestionEx* CQuestionStream::FindQuestionForTime(UINT nPositionMs)
{
   for (int i=0; i<m_aQuestions.GetSize(); ++i)
   {
      if (m_aQuestions[i]->PageContains(nPositionMs))
      {
         return m_aQuestions[i];
      }
   }

   return NULL;
}

CString& CQuestionStream::CreateScormId(CString& csExisting)
{
   static CString csReturn;

   if (csExisting.IsEmpty())
      csReturn.Format(_T("question%d"), GetQuestionCount() + 1);
   else
      csReturn = csExisting;

   bool bDummy = false;
   if (m_mapScormIds.Lookup(csReturn, bDummy))
   {
      CString csPrefix = csReturn;

      int iIdx = csReturn.ReverseFind(_T('_'));
      int iSubNumber = 0;
      
      if (iIdx > -1)
      {
         bool bEndsAsNumber = true;
         CString csNumber = csReturn.Right(csReturn.GetLength() - iIdx - 1);
         if (csNumber.GetLength() < 1)
            bEndsAsNumber = false;
         else
         {
            for (int i=0; i<csNumber.GetLength(); ++i)
            {
               if (csNumber[i] < _T('0') || csNumber[i] > _T('9'))
                  bEndsAsNumber = false;
            }
         }
         
         if (bEndsAsNumber)
         {
            iSubNumber = _ttoi(csNumber);
            csPrefix = csReturn.Left(iIdx);
         }
      }

      do
      {
         iSubNumber++;
         CString csAdd;
         csAdd.Format(_T("_%d"), iSubNumber);
         
         csReturn = csPrefix;
         csReturn += csAdd;
      } while (m_mapScormIds.Lookup(csReturn, bDummy));
      
   }

   return csReturn;
}


HRESULT CQuestionStream::AddQuestionnaire(CQuestionnaireEx *pQuestionnaire, bool bUserInteraction)
{
   HRESULT hr = S_OK;

   if (pQuestionnaire == NULL)
      return E_POINTER;

   if (m_aQuestionnaires.GetSize() > 0)
   {
      _ASSERT(false);

      return E_NOTIMPL;
   }


   m_aQuestionnaires.Add(pQuestionnaire);
   if (SUCCEEDED(hr))
      hr = pQuestionnaire->CommitFeedback(); // for initial insertion
   if (SUCCEEDED(hr))
      hr = pQuestionnaire->ClaimQuestionPointers(); // for undo/redo of remove
   
   if (m_pUndoManager != NULL && SUCCEEDED(hr))
   {
      // pointers used: make a copy for the undo stack
      // use an array because of "ReleaseData()"
      CQuestionnaireEx **paUndo = new CQuestionnaireEx*[1];
      paUndo[0] = pQuestionnaire;
            
      if (SUCCEEDED(hr))
      {
         hr = m_pUndoManager->RegisterAction(this, UNDO_INSERT_OBJECT, 
            (BYTE *)paUndo, sizeof (CQuestionnaireEx*), 
            HINT_CHANGE_NOTHING, 0, IUNDO_OBJECT_QUESTIONNAIRE, false);
      }
      
      if (SUCCEEDED(hr) && bUserInteraction)
         hr = m_pUndoManager->SetRollbackPoint();
   }
  
   return S_OK;
}

HRESULT CQuestionStream::GetQuestionnaires(CArray<CQuestionnaireEx *, CQuestionnaireEx *> *paTarget)
{
   if (paTarget == NULL)
      return E_POINTER;

   if (paTarget->GetSize() > 0)
      return E_INVALIDARG;

   for (int i=0; i<m_aQuestionnaires.GetSize(); ++i)
      paTarget->Add(m_aQuestionnaires[i]);

   return S_OK;
}

HRESULT CQuestionStream::ChangeQuestionnaire(CQuestionnaireEx *pOld, CQuestionnaireEx *pNew, bool bDoRollback)
{
   HRESULT hr = S_OK;

   if (pOld == NULL || pNew == NULL)
      return E_POINTER;

   if (pNew->HasQuestions())
      return E_INVALIDARG;

   bool bFound = false;
   for (int i=0; i<m_aQuestionnaires.GetSize(); ++i)
   {
      if (pOld == m_aQuestionnaires[i])
      {
         bFound = true;

         pOld->TransferQuestionPointers(pNew);

         bool bButtonsDifferent = pOld->IsButtonConfigurationDifferent(pNew);
         
         if (SUCCEEDED(hr))
            hr = RemoveQuestionnaire(pOld, false);

         if (SUCCEEDED(hr))
            hr = AddQuestionnaire(pNew, false);

         if (bButtonsDifferent && SUCCEEDED(hr))
            hr = pNew->UpdateButtonAppearance(); // change appearance for old (question) buttons

         if (pOld->IsShowProgress() != pNew->IsShowProgress())
         {
            pNew->ChangeQuestionsShowProgress(pNew->IsShowProgress());
         }

         if (bDoRollback && m_pUndoManager != NULL && SUCCEEDED(hr))
            hr = m_pUndoManager->SetRollbackPoint();
        
         //pNew->MoveFeedbackToFront();
         

         break;
      }
   }

   if (bFound)
      return hr;
   else
      return E_INVALIDARG;
}

HRESULT CQuestionStream::RemoveQuestionnaire(CQuestionnaireEx *pQuestionnaire, bool bUserInteraction)
{
   HRESULT hr = S_OK;

   if (pQuestionnaire == NULL)
      return E_POINTER;

   // during undo (of ChangeQuestionnaire()) you first remove the 
   // current question which still has questions...
   //if (pQuestionnaire->HasQuestions())
   //   return E_QS_CONTAINS_QUESTIONS;

   bool bFound = false;
   
   for (int i=0; i<m_aQuestionnaires.GetSize(); ++i)
   {
      if (pQuestionnaire == m_aQuestionnaires[i])
      {
         bFound = true;

         //pQuestionnaire->SetValidQuestionPointers(false);

         
         if (m_pUndoManager != NULL)
         {
            // pointers used: make a copy for the undo stack
            // use an array because of "ReleaseData()"
            CQuestionnaireEx **paUndo = new CQuestionnaireEx*[1];
            paUndo[0] = pQuestionnaire;
            
            if (SUCCEEDED(hr))
            {
               hr = m_pUndoManager->RegisterAction(this, UNDO_REMOVE_OBJECT, 
                  (BYTE *)paUndo, sizeof (CQuestionnaireEx*), HINT_CHANGE_NOTHING, 
                  0, IUNDO_OBJECT_QUESTIONNAIRE, false);

               // Note: the nLengthMs field is slightly abused for an object type identifier.
            }

            if (SUCCEEDED(hr) && bUserInteraction)
               hr = m_pUndoManager->SetRollbackPoint();
         }

         m_aQuestionnaires.RemoveAt(i);

         pQuestionnaire->RemoveFeedback();
      }
   }
   
   if (bFound)
      return hr;
   else
      return E_INVALIDARG;
}


HRESULT CQuestionStream::WriteLepEntry(CFileOutput *pOutput, LPCTSTR tszLepFilename)
{
   HRESULT hr = S_OK;

   if (tszLepFilename == NULL)
      return E_POINTER;

   if (pOutput == NULL)
      return E_POINTER;

   // use of char[] is deliberate here: data is plain text and always 8-bit

   _TCHAR tszOutput[100];
   _stprintf(tszOutput, _T("    <questions length=\"%d\">\n"), this->m_nStreamLength);
   hr = pOutput->Append(tszOutput);
   
   if (SUCCEEDED(hr))
      hr = WriteFileEntry(pOutput, true, true, NULL, tszLepFilename);

   _stprintf(tszOutput, _T("    </questions>\n"));
   if (SUCCEEDED(hr))
      hr = pOutput->Append(tszOutput);

   return hr;
}

HRESULT CQuestionStream::WriteFileEntry(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
                                        LPCTSTR tszExportPrefix, LPCTSTR tszLepFilename)
{
   HRESULT hr = S_OK;

   if (pOutput == NULL)
      return E_POINTER;

   // Note: tszExportPrefix can be NULL, it is non-null in the export case;
   // if there are images that maybe have to be copied to the export directory.

   // Note:
   // * Questions are written by their questionnaires
   // * The (drawsdk) objects were alreay written by the CInteractionStream

   CMap<CString, LPCTSTR, CString, CString&> mapImagePaths;

   for (int i=0; i<m_aQuestionnaires.GetSize() && SUCCEEDED(hr); ++i)
   {
      hr = m_aQuestionnaires[i]->Write(pOutput, bDoIdent, 
         bUseFullImagePath, mapImagePaths, tszExportPrefix, tszLepFilename, m_pEditorProject);
   }

   return hr;
}


HRESULT CQuestionStream::ReadLepEntry(SgmlElement *pSgml)
{
   HRESULT hr = S_OK;
  
   if (pSgml == NULL)
      return E_POINTER;

   int iStreamLength = pSgml->GetIntValue(_T("length"));
   if (iStreamLength > 0)
      m_nStreamLength = iStreamLength;

   CQuestionReaderWriter reader;

   CArray<SgmlElement *, SgmlElement *> aElements;
   pSgml->GetElements(aElements);
   for (int i = 0; i < aElements.GetCount() && SUCCEEDED(hr); ++i) {
       SgmlElement *pSon = aElements[i];
       if (pSon != NULL) {
           if (_tcscmp(pSon->GetName(), _T("QUESTIONNAIRE")) == 0) {
               // images are specified with an absolute (or LEP relative) path during
               // LEP saving: so there doesn't have to be one specified: ""
               hr = reader.Parse(pSon, CString(_T("")));
           }
       }
   }

   if (reader.GetCount() > 0 && SUCCEEDED(hr))
   {
      UINT nElementCount = reader.GetCount();

      // this part is double code to Insert()
      
      CQuestionnaire **apQuestionnaires = new CQuestionnaire*[nElementCount];
      ZeroMemory(apQuestionnaires, nElementCount * sizeof (CQuestionnaire*));
      
      if (SUCCEEDED(hr))
         hr = reader.Extract(apQuestionnaires);

      if (SUCCEEDED(hr))
         hr = Insert(0, m_nStreamLength, apQuestionnaires, nElementCount);

      if (apQuestionnaires != NULL)
      {
         if (SUCCEEDED(hr))
         {
            for (int i=0; i<(signed)nElementCount; ++i)
               delete apQuestionnaires[i];
         }

         delete[] apQuestionnaires;
      }
   }

   if (SUCCEEDED(hr) && reader.GetCount() == 0)
      hr = S_FALSE;

   // m_nStreamLength is set above

   return hr;
}


HRESULT CQuestionStream::OperateQuestion(UINT nPreviewMs, QuestionOperationId idOperation)
{
   HRESULT hr = S_FALSE;

   CQuestionEx *pQuestion = FindQuestionForTime(nPreviewMs);

   if (pQuestion != NULL)
   {
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> aQuestionAreas;
      hr = pQuestion->GetObjectPointers(&aQuestionAreas);

      if (SUCCEEDED(hr))
      {
         bool bChangeFound = false;
         bool bQuestionSuccess = true;

         for (int i=0; i<aQuestionAreas.GetSize(); ++i)
         {
            CInteractionAreaEx *pArea = aQuestionAreas[i];
            
            bool bDynamic = pArea->GetClassId() == INTERACTION_CLASS_DYNAMIC;
            bool bMoveable = pArea->GetClassId() == INTERACTION_CLASS_MOVE;
            bool bBlanks = pArea->GetClassId() == INTERACTION_CLASS_BLANKS;
            
            CDynamicObject *pDynamic = NULL;
            if (bDynamic)
               pDynamic = (CDynamicObject *)pArea;
            CMoveableObjects *pMoveable = NULL;
            if (bMoveable)
            {
               pMoveable = (CMoveableObjects *)pArea;

               if (!pMoveable->IsMoveableSource())
                  bMoveable = false; // check only real d&d objects
            }
            CBlankGroup *pBlanks = NULL;
            if (bBlanks)
               pBlanks = (CBlankGroup *)pArea;
            
            if (idOperation == QUESTION_OPERATION_CHECK)
            {
               if (bDynamic)
               {
                  if (!pDynamic->IsAnswerCorrect())
                     bQuestionSuccess = false;
               }
               else if (bMoveable)
               {
                  if (!pMoveable->IsAnswerCorrect())
                     bQuestionSuccess = false;
               }
               else if (bBlanks)
               {
                  if (!pBlanks->IsAnswerCorrect())
                     bQuestionSuccess = false;
               }
                
               if (!bQuestionSuccess)
                  break;
            }
            else if (idOperation == QUESTION_OPERATION_RESET)
            {
               HRESULT hrSingle = S_FALSE;
               
               bool bChange = pArea->Reset(false);
               
               //if (SUCCEEDED(hrSingle) && hrSingle != S_FALSE)
               if (bChange)
                  bChangeFound = true;
            }
         }

         if (idOperation == QUESTION_OPERATION_CHECK)
         {
            // store results, show feedback, execute actions (question and questionnaire)
            QuestionAnswered(pQuestion, bQuestionSuccess);
         }
      }
   }
  
   return hr;
}

HRESULT CQuestionStream::StartTimerThread()
{
   m_bDoLoopTimer = true;
   m_pThread = AfxBeginThread(TimerThreadLauncher, (LPVOID)this);
   
   if (m_pThread != NULL)
      return S_OK;
   else
      return E_FAIL;
}
   
HRESULT CQuestionStream::ResetAfterPreview()
{
   // Note: the question objects themself were resetted in CInteractionStream.

   m_bDoLoopTimer = false;
   // the thread will stop soon
   m_pThread = NULL;

   for (int i=0; i<m_aQuestionnaires.GetSize(); ++i)
      m_aQuestionnaires[i]->ResetAfterPreview();

   return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// protected or private and other helper methods


HRESULT CQuestionStream::UndoAction(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs)
{
   // handle all undo (especially INSERT and REMOVE_RANGE)
   return SpecificUndo(idActionCode, pData, nDataLength, nPositionMs, nLengthMs);
}

HRESULT CQuestionStream::ReleaseData(BYTE *pData, UINT nDataLength, bool bDeleteCompletely,
                                     UndoActionId idActionCode, UINT nPositionMs, UINT nLengthMs)
{
   if (pData == NULL && nDataLength > 0)
      return E_POINTER;

   if (nDataLength % sizeof (CQuestionEx*) != 0)
      return E_INVALIDARG;

   bool bIsQuestion = nLengthMs == IUNDO_OBJECT_QUESTION;

   int iElementSize = bIsQuestion ? sizeof (CQuestionEx*) : sizeof (CQuestionnaireEx*);

   UINT nElementCount = nDataLength / sizeof iElementSize;
   CQuestionEx **paQuestions = NULL;
   CQuestionnaireEx **paQuestionnaires = NULL;
   
   if (bIsQuestion)
      paQuestions = (CQuestionEx **)pData;
   else
      paQuestionnaires = (CQuestionnaireEx **)pData;

   if (bDeleteCompletely)
   {
      // this should be always an array; even for only one element (InsertSingle(), ...)
      for (int i=0; i<(signed)nElementCount; ++i)
      {
         if (bIsQuestion)
         {
            CQuestionEx *pQuestion = FindQuestionForTime(paQuestions[i]->GetPageBegin());

            if (paQuestions[i] != pQuestion) 
               delete paQuestions[i];
         }
         else
         {
            if (!IsQuestionnaireInUse(paQuestionnaires[i]))
               delete paQuestionnaires[i];
         }
      }
   }

   if (paQuestions != NULL)
      delete[] paQuestions;

   if (paQuestionnaires != NULL)
      delete[] paQuestionnaires;
   

   return S_OK;
}

HRESULT CQuestionStream::SpecificUndo(UndoActionId idActionCode, 
                                      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (idActionCode == UNDO_INSERT_OBJECT)
   {
      if (nLengthMs == IUNDO_OBJECT_QUESTION)
      {
         CQuestionEx **paQuestions = (CQuestionEx **)pData;
         hr = DeleteSingle(paQuestions[0], false, false);
      }
      else
      {
         CQuestionnaireEx **paQuestionnaires = (CQuestionnaireEx **)pData;
         hr = RemoveQuestionnaire(paQuestionnaires[0], false);
      }
   }
   else if (idActionCode == UNDO_REMOVE_OBJECT)
   {
      if (nLengthMs == IUNDO_OBJECT_QUESTION)
      {
         CQuestionEx **paQuestions = (CQuestionEx **)pData;
         hr = InsertSingle(paQuestions[0], paQuestions[0]->GetBelongsTo(), false, false);
      }
      else
      {
         CQuestionnaireEx **paQuestionnaires = (CQuestionnaireEx **)pData;
         hr = AddQuestionnaire(paQuestionnaires[0], false);
      }
   }
   else if (idActionCode == UNDO_INSERT_RANGE)
   {
      m_nStreamLength -= nLengthMs;

      if (m_pUndoManager != NULL && SUCCEEDED(hr))
      {
         hr = m_pUndoManager->RegisterAction(
            this, UNDO_REMOVE_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nPositionMs, nLengthMs, false);
         // dummy entry to shorten the stream upon undo/redo
      }
   }
   else if (idActionCode == UNDO_REMOVE_RANGE)
   {
      m_nStreamLength += nLengthMs;

      if (m_pUndoManager != NULL && SUCCEEDED(hr))
      {
         hr = m_pUndoManager->RegisterAction(
            this, UNDO_INSERT_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nPositionMs, nLengthMs, false);
         // dummy entry to shorten the stream upon undo/redo
      }
   }
   else if (idActionCode == UNDO_CHANGE_OBJECT)
   {
      if (nDataLength < 12) // at least id, pointer, value
         hr = E_IAX_INVALID_DATA;

      if (SUCCEEDED(hr))
      {
         int *pValues = (int *)pData;

         CQuestionEx *pQuestion = (CQuestionEx *)pValues[1];

         bool bObjectValid = false;
         for (int i=0; i<m_aQuestions.GetSize(); ++i)
         {
            if (m_aQuestions[i] == pQuestion)
            {
               bObjectValid = true;
               break;
            }
         }

         int iTimesChange = pValues[2];

         if (bObjectValid)
            hr = ChangeTimes(pQuestion, -iTimesChange);
         else
         {
            _ASSERT(false); // shouldn't happen
            hr = E_FAIL;
         }
      }
   }
   else
      hr = S_FALSE;

   return hr;
}

HRESULT CQuestionStream::FillCopyMouseActions(CArray<CMouseAction *, CMouseAction *> *paActions)
{
   if (paActions == NULL)
      return E_POINTER;

   int iBeginSize = paActions->GetSize();

   for (int i=0; i<m_aQuestions.GetSize(); ++i)
   {
      if (m_aQuestions[i]->GetCorrectAction() != NULL && m_aQuestions[i]->GetCorrectAction()->GetPath() != NULL)
         paActions->Add(m_aQuestions[i]->GetCorrectAction());

      if (m_aQuestions[i]->GetFailureAction() != NULL && m_aQuestions[i]->GetFailureAction()->GetPath() != NULL)
         paActions->Add(m_aQuestions[i]->GetFailureAction());
   }


   for (i=0; i<m_aQuestionnaires.GetSize(); ++i)
   {
      if (m_aQuestionnaires[i]->GetCorrectAction() != NULL && m_aQuestionnaires[i]->GetCorrectAction()->GetPath() != NULL)
         paActions->Add(m_aQuestionnaires[i]->GetCorrectAction());

      if (m_aQuestionnaires[i]->GetFailureAction() != NULL && m_aQuestionnaires[i]->GetFailureAction()->GetPath() != NULL)
         paActions->Add(m_aQuestionnaires[i]->GetFailureAction());
   }

   return paActions->GetSize() > iBeginSize ? S_OK : S_FALSE;

}

HRESULT CQuestionStream::CutCopyOrDelete(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard, bool bDoCopy, bool bDoDelete)
{
   HRESULT hr = S_OK;

   CArray<CQuestionEx *, CQuestionEx *> aAffectedQuestions;
   aAffectedQuestions.SetSize(0, 10);

   for (int i=0; i<m_aQuestions.GetSize() && SUCCEEDED(hr); ++i)
   {
      if (m_aQuestions[i]->GetPageVisibility()->IntersectsRange(nFromMs, nLengthMs))
      {
         aAffectedQuestions.Add(m_aQuestions[i]);
      }
      else if (bDoDelete && m_aQuestions[i]->BeginsAtOrAfter(nFromMs + nLengthMs))
      {
         hr = ChangeTimes(m_aQuestions[i], -(int)nLengthMs);
      }
   }
    
   if (bDoCopy && SUCCEEDED(hr))
   {
      if (pClipboard == NULL)
         return E_POINTER;

      CArray<CQuestionEx *, CQuestionEx *> aCopyQuestions;
      aCopyQuestions.SetSize(0, 10);

      for (i=0; i<aAffectedQuestions.GetSize() && SUCCEEDED(hr); ++i)
      {
         // Note: also questions without a matching in the object activity range
         // are allowed (and copied). For "END_PAGE" special care ist taken in CTimePeriod.

         CQuestionEx *pCopyQuestion = aAffectedQuestions[i]->Copy();
         
         if (pCopyQuestion != NULL)
         {
            hr = pCopyQuestion->TrimToRange(nFromMs, nLengthMs);
            if (SUCCEEDED(hr))
               hr = pCopyQuestion->DecrementTimes(nFromMs);
            if (SUCCEEDED(hr))
               aCopyQuestions.Add(pCopyQuestion);
            else
               delete pCopyQuestion;
         }
         else
            hr = E_FAIL;
         
      }

      //TRACE("To clipboard:\n");
      //PrintState(&aCopyQuestions);

      if (SUCCEEDED(hr))
      {
         if (m_ppLastClipboardData != NULL)
         {
            for (i=0; i<(signed)m_nLastClipboardCount; ++i)
               delete m_ppLastClipboardData[i];
            delete[] m_ppLastClipboardData;
         }
         
         UINT nElementCount = (unsigned)aCopyQuestions.GetSize();
         UINT nByteCount = nElementCount * sizeof (CQuestionEx*);
         m_ppLastClipboardData = new CQuestionEx*[nElementCount];
         m_nLastClipboardCount = nElementCount;
         
         for (int j=0; j<(signed)nElementCount; ++j)
            m_ppLastClipboardData[j] = aCopyQuestions[j];

         UINT nClipboardKey = (UINT)this;
         hr = pClipboard->AddData((BYTE *)m_ppLastClipboardData, nByteCount, nClipboardKey);
      }

   }

   if (bDoDelete && SUCCEEDED(hr))
   {
      for (i=0; i<aAffectedQuestions.GetSize() && SUCCEEDED(hr); ++i)
      {
         if (aAffectedQuestions[i]->GetObjectVisibility()->EntirelyWithin(nFromMs, nLengthMs))
         {
            hr = DeleteSingle(aAffectedQuestions[i], false, false);
         }
         else
         {
            CQuestionEx *pCopyQuestion = aAffectedQuestions[i]->Copy();

            if (pCopyQuestion != NULL)
            {
               hr = pCopyQuestion->RemovePart(nFromMs, nLengthMs);
               if (SUCCEEDED(hr))
                  hr = ChangeQuestion(aAffectedQuestions[i], pCopyQuestion, false, true);
            }
            else
               hr = E_FAIL;
         }
      }
   }

   if (m_pUndoManager && bDoDelete && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(
         this, UNDO_REMOVE_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nFromMs, nLengthMs, false);
      // dummy entry to enlarge the stream upon undo/redo
   }

   //TRACE("remaining:\n");
   //PrintState();

   return hr;
}

HRESULT CQuestionStream::InsertRange(UINT nToMs, UINT nLengthMs, BYTE *pData, UINT nByteCount, bool bUndoWanted)
{
   // ignore bUndoWanted: we need proper undo informations for memory management

   HRESULT hr = S_OK;

   if (pData == NULL && nByteCount > 0)
      return E_POINTER;
   // else despite pData == NULL stream must be made longer and elements must be moved

   if ((nByteCount % sizeof (CQuestionEx*)) != 0)
      return E_INVALIDARG;

   UINT nElementCount = nByteCount / sizeof (CQuestionEx*);
   CQuestionEx **apQuestions = (CQuestionEx **)pData;

   int  iInsertPosition = m_aQuestions.GetSize();
   bool bInsideQuestion = false;
   bool bStartQuestion = false;
   bool bOutsideQuestion = true;

   for (int i=0; i<m_aQuestions.GetSize() && SUCCEEDED(hr); ++i)
   {
      if (m_aQuestions[i]->BeginsAtOrAfter(nToMs) || m_aQuestions[i]->PageContains(nToMs))
      {
         iInsertPosition = i;

         if (m_aQuestions[i]->GetPageBegin() > nToMs)
            bOutsideQuestion = true;
         else if (m_aQuestions[i]->GetPageBegin() == nToMs)
            bStartQuestion = true;
         else
            bInsideQuestion = true;

         break;
      }
   }

   bool bOperationFinished = false;
   if (bInsideQuestion || bStartQuestion)
   {
      if (nElementCount == 1)
      {
         if (m_aQuestions[iInsertPosition]->MeetsWith(apQuestions[0], nToMs))
         {
            // enlarge existing question
            hr = EnlargeQuestion(iInsertPosition, apQuestions[0], nToMs);

            bOperationFinished = true;
            iInsertPosition += 1;
         }
         else if (iInsertPosition > 0 && m_aQuestions[iInsertPosition - 1]->MeetsWith(apQuestions[0], nToMs))
         {
            // enlarge existing question
            hr = EnlargeQuestion(iInsertPosition - 1, apQuestions[0], nToMs);

            bOperationFinished = true;
         }
      }
      
      if (!bOperationFinished && bInsideQuestion)
      {
         // split question: two new halves
         CQuestionEx *pCopyQuestion1 = m_aQuestions[iInsertPosition]->Copy();
         
         if (pCopyQuestion1 != NULL)
         {
            CQuestionEx *pCopyQuestion2 = pCopyQuestion1->Split(nToMs);
            if (pCopyQuestion2 != NULL)
            {
               hr = ChangeQuestion(m_aQuestions[iInsertPosition], pCopyQuestion1, false, true);
               if (SUCCEEDED(hr))
                  hr = InsertSingle(pCopyQuestion2, pCopyQuestion2->GetBelongsTo(), false);
            }
            else
               hr = E_FAIL;
         }
         else
            hr = E_FAIL;


         iInsertPosition += 1;
      }
   }

   // move any area after the insert range
   for (i=iInsertPosition; i<m_aQuestions.GetSize() && SUCCEEDED(hr); ++i)
      ChangeTimes(m_aQuestions[i], nLengthMs);
    
   // insert (or join) the elements from the clipboard
   if (!bOperationFinished && nElementCount > 0)
   {
      for (i=0; i<(signed)nElementCount && SUCCEEDED(hr); ++i)
      {
         if (i == 0 && iInsertPosition > 0 && m_aQuestions[iInsertPosition - 1]->MeetsWith(apQuestions[i], nToMs))
         {
            // enlarge existing question
            hr = EnlargeQuestion(iInsertPosition - 1, apQuestions[i], nToMs);
         }
         else if (i == (signed)nElementCount - 1 && iInsertPosition < m_aQuestions.GetSize() && m_aQuestions[iInsertPosition]->MeetsWith(apQuestions[i], nToMs))
         {
            // enlarge existing question
            hr = EnlargeQuestion(iInsertPosition, apQuestions[i], nToMs);
         }
         else
         {
            // insert as new

            CQuestionEx *pCopyQuestion = apQuestions[i]->Copy();

            if (pCopyQuestion != NULL)
            {
               if (nToMs > 0)
                  hr = pCopyQuestion->IncrementTimes(nToMs);
               if (SUCCEEDED(hr))
                  hr = InsertSingle(pCopyQuestion, pCopyQuestion->GetBelongsTo(), false);
            }
            else
               hr = E_FAIL; 
         }
      }
   }  

   _ASSERT(SUCCEEDED(hr));
   //TRACE("new:\n");
   //PrintState();


   if (m_pUndoManager != NULL && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(
         this, UNDO_INSERT_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nToMs, nLengthMs, false);
      // dummy entry to shorten the stream upon undo/redo
   }
   

   return hr;
}


// private
HRESULT CQuestionStream::EnlargeQuestion(UINT nExistingIndex, CQuestionEx *pToInclude, UINT nMoveMs)
{
   HRESULT hr = S_OK;

   if (nExistingIndex >= (unsigned)m_aQuestions.GetSize())
      return E_INVALIDARG;

   if (pToInclude == NULL)
      return E_POINTER;

   CQuestionEx *pCopyQuestion = m_aQuestions[nExistingIndex]->Copy();
   
   if (pCopyQuestion != NULL)
   {
      hr = pCopyQuestion->Enlarge(pToInclude, nMoveMs);
      if (SUCCEEDED(hr))
         hr = ChangeQuestion(m_aQuestions[nExistingIndex], pCopyQuestion, false, false);
   }
   else
      hr = E_FAIL; 
   
   return hr;
}


// private
HRESULT CQuestionStream::Insert(UINT nToMs, UINT nLength, 
                                CQuestionnaire **apQuestionnaires, UINT nCount)
{
   HRESULT hr = S_OK;

   if (apQuestionnaires == NULL)
      return E_POINTER;

   if (nCount > 0)
   {
      UINT nQuestionCount = 0;
      for (int i=0; i<(signed)nCount && SUCCEEDED(hr); ++i)
         nQuestionCount += apQuestionnaires[i]->GetQuestionCount();

      // now insert every questionnaire and then their respective questions
      if (SUCCEEDED(hr))
      {
         CQuestionEx** apQuestions = new CQuestionEx*[nQuestionCount];
         ZeroMemory(apQuestions, nQuestionCount * sizeof (CQuestionEx*));
         CArray<CQuestion *, CQuestion *> aExtractQuestions;

         UINT nInsertIndex = 0;
         for (int i=0; i<(signed)nCount && SUCCEEDED(hr); ++i)
         {
            CQuestionnaireEx *pQQex = NULL;
            
            if (GetQuestionnaireCount() == 0)
            {
               pQQex = new CQuestionnaireEx();
               hr = pQQex->Init(apQuestionnaires[i], true, m_pEditorProject, nToMs);

               if (SUCCEEDED(hr))
                  hr = AddQuestionnaire(pQQex, false);
            }
            else
               pQQex = m_aQuestionnaires[0];
            // TODO currently only one questionnaire supported; change that in future?


            if (SUCCEEDED(hr))
            {
               hr = apQuestionnaires[i]->ExtractQuestions(&aExtractQuestions);

               for (int j=0; j<aExtractQuestions.GetSize() && SUCCEEDED(hr); ++j)
               {
                  CQuestionEx *pQuestion = new CQuestionEx();
                  hr = pQuestion->Init(aExtractQuestions[j], nToMs, pQQex, m_pEditorProject);

                  if (SUCCEEDED(hr))
                     apQuestions[nInsertIndex++] = pQuestion;
                  
                  delete aExtractQuestions[j];
               }
               
               aExtractQuestions.RemoveAll();
            }
         }
         
         if (SUCCEEDED(hr))
         {
            hr = InsertRange(nToMs, nLength, (BYTE *)apQuestions,
               nQuestionCount * sizeof (CQuestionEx*), true);
            // makes a copy of the questions (special case)
         }
         

         if (apQuestions != NULL)
         {
            for (i=0; i<(signed)nQuestionCount; ++i)
               delete apQuestions[i];
            delete[] apQuestions;
         }
      }

   }

   return hr;
}

// private
HRESULT CQuestionStream::ChangeTimes(CQuestionEx *pQuestion, int iChangeMs)
{
   HRESULT hr = S_OK;

   if (pQuestion == NULL)
      return E_POINTER;
   
   if (iChangeMs < 0)
      hr = pQuestion->DecrementTimes((unsigned)-iChangeMs);
   else
      hr = pQuestion->IncrementTimes((unsigned)iChangeMs);
   
   if (SUCCEEDED(hr) && m_pUndoManager != NULL)
   {
      int aiValues[3];
      aiValues[0] = IUNDO_ACTION_TIMES_CHANGE;
      aiValues[1] = (int)pQuestion;
      aiValues[2] = iChangeMs;
      
      hr = m_pUndoManager->RegisterAction(this, UNDO_CHANGE_OBJECT, 
         (BYTE *)&aiValues, sizeof aiValues, HINT_CHANGE_NOTHING, 0, 0, true);
   }
   
   return hr;
}


//private
bool CQuestionStream::IsQuestionnaireInUse(CQuestionnaireEx *pQuestionnaire)
{
   bool bInUse = false;
   
   if (pQuestionnaire != NULL)
   {
      for (int i=0; i<m_aQuestionnaires.GetSize(); ++i)
      {
         if (m_aQuestionnaires[i] == pQuestionnaire)
         {
            bInUse = true;
            break;
         }
      }
   }
   
   return bInUse;
}


//private
HRESULT CQuestionStream::QuestionAnswered(CQuestionEx *pQuestion, bool bQuestionSuccess)
{
   if (pQuestion == NULL)
      return E_POINTER;

   if (m_pAnsweredQuestion != NULL)
      _ASSERT(false); // shouldn't happen; feedback is "modal"; action execution now delayed however...

   pQuestion->UpdateAnswered(bQuestionSuccess);

   if (bQuestionSuccess || !pQuestion->AreAttemptsLeft() || pQuestion->IsTimeUp())
      pQuestion->Disable();

   m_bQuestionSuccess = bQuestionSuccess;
   m_pAnsweredQuestion = pQuestion;

   CWinThread *pThread = AfxBeginThread(FeedbackThreadLauncher, (LPVOID)this);
   // ShowFeedback is (normally) blocking so that action execution is delayed.

   // NOTE the thread above is also not working fully correct:
   // a passing back to the ui thread loop is necessary in order to correctly perform jumps.
   
   if (pThread != NULL)
      return S_OK;
   else
      return E_FAIL;
}


// private, static
UINT CQuestionStream::TimerThreadLauncher(LPVOID pParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   if (pParam != NULL)
   {
      CQuestionStream *pThatStream = (CQuestionStream *)pParam;

      return pThatStream->TimerThreadLoop();
   }
   else
      return 1;
}

// private
UINT CQuestionStream::TimerThreadLoop()
{
   while (m_bDoLoopTimer && m_nProjectValid == ID_VALID_CHECK)
   {
      // at least the latter one fails if the object is deleted already

      CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

      CQuestionEx *pQuestion = FindQuestionForTime(pDoc->m_curPosMs);

      if (pQuestion != m_pLastTimerQuestion && m_pLastTimerQuestion != NULL)
      {
         m_pLastTimerQuestion->StopTimer();
         m_pLastTimerQuestion = NULL;
      }

      if (pQuestion != NULL)
      {
         int iRemainingSeconds = 0;
         if (pQuestion->IsTimeable())
         {
            iRemainingSeconds = pQuestion->UpdateTimer();
            m_pLastTimerQuestion = pQuestion; 
         }

         CWhiteboardView *pView = pDoc->GetWhiteboardView();

         HRESULT hrChange = pQuestion->UpdateChangeableTexts(iRemainingSeconds);
         if (SUCCEEDED(hrChange) && hrChange != S_FALSE && pView != NULL)
            pView->RedrawWindow(); // TODO maybe a bit large and during preview not necessary

         if (pQuestion->IsTimeable() && pQuestion->IsTimeUp() 
            && !pQuestion->IsTimerMessageShown() && pQuestion->IsEnabled())
         {
            // The time is up:

            QuestionAnswered(pQuestion, false);
         }
      }

      ::Sleep(500);
   }

   return 0;
}

// private, static
UINT CQuestionStream::FeedbackThreadLauncher(LPVOID pParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   if (pParam != NULL)
   {
      CQuestionStream *pThatStream = (CQuestionStream *)pParam;

      return pThatStream->FeedbackThreadLoop();
   }
   else
      return 1;
}

// private
UINT CQuestionStream::FeedbackThreadLoop() {

    CQuestionEx *pQuestion = m_pAnsweredQuestion;
    bool bQuestionSuccess = m_bQuestionSuccess;

    CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

    m_iFeedbackStartPosMs = pDoc->m_curPosMs;

    CWhiteboardView *pView = pDoc->GetWhiteboardView();
    if (pView == NULL)
    {
        _ASSERT(false); // shouldn't happen
        return E_FAIL;
    }

    bool bQuestionnaireOpen = true;
    CQuestionnaireEx *pQuestionnaire = pQuestion->GetBelongsTo();
    if (pQuestionnaire != NULL)
        bQuestionnaireOpen = pQuestionnaire->AnyQuestionsLeft();
    else
        _ASSERT(false); // shouldn't happen

    bool bSomeQuestionnaireFeedback = false;
    if (!bQuestionnaireOpen) {
        bool bQuestionnairePassed = pQuestionnaire->CheckPassed();

        bool bSomeQuestionnaireFeedback = pQuestionnaire->IsEvaluationDefined();

        if (!bSomeQuestionnaireFeedback) {
            if (bQuestionnairePassed)
                bSomeQuestionnaireFeedback = pQuestion->IsFeedbackActive(FEEDBACK_TYPE_QQ_PASSED);
            else
                bSomeQuestionnaireFeedback = pQuestion->IsFeedbackActive(FEEDBACK_TYPE_QQ_FAILED);
        }
    }

    // Note: If the questionnaire has no questions left then several feedbacks are enqueued
    // before waiting on the last feedback; otherwise a user jump could not 
    // remove all feedback but only the present one.

    bool bWaitForQuestionFeedback = bQuestionnaireOpen || !bSomeQuestionnaireFeedback;

    // Note: It is also possible that there is no questionnaire feedback activated.
    // In that case however one has to wait for the question feedback.

    if (bQuestionSuccess) {
        pQuestion->ShowFeedback(FEEDBACK_TYPE_Q_CORRECT, pView, bWaitForQuestionFeedback);
    } else {
        if (!pQuestion->IsTimeUp()) {
            if (pQuestion->AreAttemptsLeft())
                pQuestion->ShowFeedback(FEEDBACK_TYPE_Q_REPEAT, pView, bWaitForQuestionFeedback);
            else
                pQuestion->ShowFeedback(FEEDBACK_TYPE_Q_WRONG, pView, bWaitForQuestionFeedback);
        } else {
            pQuestion->ShowFeedback(FEEDBACK_TYPE_Q_TIME, pView, bQuestionnaireOpen);
        }
    }

    if (!bQuestionnaireOpen) {
        // TODO this feedback is shown even if a jump was performed beforehand
        // (is this the right behavior?): feedback is then visible while preview is running...

        bool bQuestionnairePassed = pQuestionnaire->CheckPassed();

        bool bWaitOnPassFeedback = !pQuestionnaire->IsEvaluationDefined();

        if (bQuestionnairePassed)
            pQuestion->ShowFeedback(FEEDBACK_TYPE_QQ_PASSED, pView, bWaitOnPassFeedback);
        else
            pQuestion->ShowFeedback(FEEDBACK_TYPE_QQ_FAILED, pView, bWaitOnPassFeedback);

        pQuestionnaire->ShowEvaluation(pView); // if defined
    }

    // Do the action execution (especially jumps):
    // TODO quite a hack! Change/remove/find out reason why jumping produces audio gaps
    // (at least it does when displaying the questionnaire feedback).
    CWnd *pWnd = pDoc->GetMainFrame();
    pWnd->PostMessage(WM_USER + MESSAGE_ID_DO_ACTIONS, (WPARAM)this, NULL);

    return 0;
}

void CQuestionStream::FeedbackLoopDoActions() {

    CQuestionEx *pQuestion = m_pAnsweredQuestion;
    bool bQuestionSuccess = m_bQuestionSuccess;

    CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

    bool bQuestionnaireOpen = true;
    CQuestionnaireEx *pQuestionnaire = pQuestion->GetBelongsTo();
    if (pQuestionnaire != NULL)
        bQuestionnaireOpen = pQuestionnaire->AnyQuestionsLeft();

    if (bQuestionnaireOpen) {
        pQuestionnaire->SetRandomQuestionReplayIndex(pQuestionnaire->GetRandomQuestionReplayIndex() + 1);

        if (m_iFeedbackStartPosMs == pDoc->m_curPosMs) {// no (user) jump yet
            if (bQuestionSuccess || !pQuestion->AreAttemptsLeft() || pQuestion->IsTimeUp()) {
                pQuestion->Execute(pDoc, bQuestionSuccess);
                // do nothing more; wait for other questions
            }
        } else {
            if (pQuestion->IsTimeUp()) // TODO why this special case?
                pQuestion->Execute(pDoc, bQuestionSuccess);
        }
    } else {
        bool bQuestionnairePassed = pQuestionnaire->CheckPassed();

        if (m_iFeedbackStartPosMs == pDoc->m_curPosMs) // no (user) jump yet
            pQuestionnaire->Execute(pDoc, bQuestionnairePassed);
    }
    m_pAnsweredQuestion = NULL;
}


void CQuestionStream::PrintState(CArray<CQuestionEx *, CQuestionEx *> *paQuestions)
{
   if (paQuestions == NULL)
      paQuestions = &m_aQuestions;

   for (int i=0; i<paQuestions->GetSize(); ++i)
   {
      CQuestionEx *pQuestion = paQuestions->GetAt(i);

      TRACE(_T("q %08x: %d - %d\n"), pQuestion, pQuestion->GetPageBegin(), pQuestion->GetPageBegin() + pQuestion->GetPageLength() - 1);
   }
}
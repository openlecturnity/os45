#include "stdafx.h"
#include "MouseAction.h"
#include "MarkReaderWriter.h"
#include "editorDoc.h"
#include "InteractionStream.h" // E_IAX_INVALID_DATA
#include "QuestionnaireEx.h"
#include "QuestionEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMouseAction, CObject)

CMouseAction::CMouseAction()
{
   m_pEditorProject = NULL;
   m_idAction = INTERACTION_NO_ACTION;
   m_nPageMarkId = 0;
   m_tszPath = NULL;
   m_nNameUsageCount = 0;
   m_pNextAction = NULL;
}

CMouseAction::~CMouseAction()
{
   if (m_idAction == INTERACTION_JUMP_TARGET_MARK && m_pEditorProject != NULL)
   {
      CStopJumpMark *pMark = m_pEditorProject->GetMarkWithId(m_nPageMarkId);
      if (pMark != NULL)
         pMark->UnregisterUser();
   }

   if (m_tszPath != NULL)
      delete[] m_tszPath;

   if (m_pNextAction != NULL)
      delete m_pNextAction;
}

HRESULT CMouseAction::Init(CEditorProject *pProject, AreaActionTypeId idAction, 
                           UINT nPageMarkId, LPCTSTR tszPath, AreaActionTypeId idActionFollowup)
{
   if (pProject == NULL)
      return E_POINTER;

   int iPathLength = 0;
   if (tszPath != NULL) 
      iPathLength = _tcslen(tszPath);

   if (idAction == INTERACTION_OPEN_URL || idAction == INTERACTION_OPEN_FILE)
   {
      if (tszPath == NULL)
         return E_POINTER;
      else if (iPathLength < 1)
         return E_INVALIDARG;
   }

   if (idAction == INTERACTION_JUMP_TARGET_MARK)
   {
      if (nPageMarkId == 0)
         return E_INVALIDARG;
   }

   if (idActionFollowup != INTERACTION_NO_ACTION)
   {
      if (idActionFollowup != INTERACTION_START_REPLAY && idActionFollowup != INTERACTION_STOP_REPLAY)
         return E_INVALIDARG; // others not implemented/specified yet
   }

   m_pEditorProject = pProject;

   m_idAction = idAction;
   m_nPageMarkId = nPageMarkId;

   if (m_idAction == INTERACTION_JUMP_TARGET_MARK)
   {
      CStopJumpMark *pMark = pProject->GetMarkWithId(m_nPageMarkId);
      if (pMark != NULL)
            pMark->RegisterUser();
   }

   if (m_tszPath != NULL)
      delete[] m_tszPath;
   m_tszPath = new _TCHAR[iPathLength + 1];
   ZeroMemory(m_tszPath, iPathLength + 1);

   _tcsncpy(m_tszPath, tszPath, iPathLength);
   m_tszPath[iPathLength] = 0; //destroyed by _tcsncpy()

   if (m_pNextAction != NULL)
      delete m_pNextAction;

   if (idActionFollowup != INTERACTION_NO_ACTION)
   {
      m_pNextAction = new CMouseAction();
      m_pNextAction->Init(pProject, idActionFollowup);
   }

 
   return S_OK;
}

HRESULT CMouseAction::Parse(CString& csDefinition, CEditorProject *pProject, UINT nInsertPositionMs)
{
   HRESULT hr = S_OK;

   CString csAction = csDefinition;

   if (!csAction.IsEmpty())
   {
      if (StringManipulation::StartsWith(csAction, CString(_T("start"))))
      {
         m_idAction = INTERACTION_START_REPLAY;
      }
      else if (StringManipulation::StartsWith(csAction, CString(_T("stop"))))
      {
         m_idAction = INTERACTION_STOP_REPLAY;
      }
      else if (StringManipulation::StartsWith(csAction, CString(_T("jump"))))
      {
         CString csParameter = csAction;
         StringManipulation::GetParameter(csParameter);

         if (StringManipulation::StartsWith(csParameter, CString(_T("next"))))
         {
            m_idAction = INTERACTION_JUMP_NEXT_PAGE;
         } 
         else if (StringManipulation::StartsWith(csParameter, CString(_T("question"))))
         {
            m_idAction = INTERACTION_JUMP_RANDOM_QUESTION;
         }
         else if (StringManipulation::StartsWith(csParameter, CString(_T("prev"))))
         {
            m_idAction = INTERACTION_JUMP_PREVIOUS_PAGE;
         }
         else if (StringManipulation::StartsWith(csParameter, CString(_T("page"))))
         {
            m_idAction = INTERACTION_JUMP_SPECIFIC_PAGE;

            StringManipulation::GetParameter(csParameter); // obtain second parameter
            UINT nMarkPosMs = _ttoi(csParameter);

            CPage *pPage = pProject->FindPageAt(nMarkPosMs + nInsertPositionMs);
            if (pPage != NULL)
               m_nPageMarkId = pPage->GetJumpId();
            else
               hr = E_INVALIDARG;
         }
         else if (StringManipulation::StartsWith(csParameter, CString(_T("mark"))))
         {
            m_idAction = INTERACTION_JUMP_TARGET_MARK;

            StringManipulation::GetParameter(csParameter); // obtain second parameter
            UINT nMarkPosMs = _ttoi(csParameter);

            // will get resolved later:
            m_nPageMarkId = nMarkPosMs;
            // TODO maybe use an additional variable to distinguish between
            // not specified/not found and not resolved (yet)
         }
         else
            hr = E_INVALIDARG;
      }
      else if (StringManipulation::StartsWith(csAction, CString(_T("open"))))
      {
         CString csSub = csAction.Mid(5, 3); // 5 -> "open-"
         if (StringManipulation::StartsWith(csSub, CString(_T("fil"))))
            m_idAction = INTERACTION_OPEN_FILE;
         else
            m_idAction = INTERACTION_OPEN_URL;

         CString csParameter = csAction;
         StringManipulation::GetParameter(csParameter);
         
         if (m_tszPath != NULL)
            delete[] m_tszPath;

         if (csParameter.GetLength() > 0)
         {
            CString csOpenPath = csParameter;

            if (m_idAction == INTERACTION_OPEN_FILE)
            {
               // Directly using the constructor will give an exception if the file is missing.
               //CFile file(csParameter, CFile::modeRead);
               
               CFile file;
               file.Open(csParameter, CFile::modeRead);
               csOpenPath = file.GetFilePath();
               // This converts local paths to global paths
               // and relies on the fact that the "current directory" is correctly set.
               // That is done in CEditorDoc::Import().
            }

            int iLength = csOpenPath.GetLength();

            m_tszPath = new _TCHAR[iLength + 1];
            ZeroMemory(m_tszPath, iLength + 1);

            _tcsncpy(m_tszPath, csOpenPath, iLength);
            m_tszPath[iLength] = 0; // is destroyed by _tcsncpy
         }
         else
            hr = E_INVALIDARG;
      }
      else if (StringManipulation::StartsWith(csAction, CString(_T("audio"))))
      {
         CString csParameter = csAction;
         StringManipulation::GetParameter(csParameter);

         if (StringManipulation::StartsWith(csParameter, CString(_T("on"))))
            m_idAction = INTERACTION_AUDIO_ON;
         else
            m_idAction = INTERACTION_AUDIO_OFF;
      }
      else if (StringManipulation::StartsWith(csAction, CString(_T("close"))))
      {
         m_idAction = INTERACTION_EXIT_PROCESS;
      }
      else if (StringManipulation::StartsWith(csAction, CString(_T("check"))))
      {
         m_idAction = INTERACTION_CHECK_QUESTION;
      }
      else if (StringManipulation::StartsWith(csAction, CString(_T("reset"))))
      {
         m_idAction = INTERACTION_RESET_QUESTION;
      }
      else
         hr = E_INVALIDARG;


      if (SUCCEEDED(hr))
      {
         bool bNext = StringManipulation::GetNextCommand(csAction);
         if (bNext)
         {
            // there are more commands, parse them, too
            CMouseAction followingAction;
            hr = followingAction.Parse(csAction, pProject, nInsertPositionMs);
            
            if (SUCCEEDED(hr))
               AddFollowingAction(&followingAction);
         }
      }
   }

   return hr;
}

HRESULT CMouseAction::ResolveJumpTimes(CEditorProject *pProject, UINT nInsertPositionMs)
{
   HRESULT hr = S_OK;

   if (m_idAction == INTERACTION_JUMP_TARGET_MARK)
   {
      CStopJumpMark *pMark = pProject->GetJumpMarkAt(m_nPageMarkId);
      if (pMark != NULL)
      {
         m_nPageMarkId = pMark->GetId();
         pMark->RegisterUser();
         
         // BUGFIX #4328: also obtain the label of this mark
         int iPathLength = 0;

         LPCTSTR tszMarkLabel = pMark->GetLabel();
         if (tszMarkLabel != NULL)
            iPathLength = _tcslen(tszMarkLabel);

         if (m_tszPath != NULL)
            delete[] m_tszPath;
         m_tszPath = new _TCHAR[iPathLength + 1]; // can be empty
         ZeroMemory(m_tszPath, iPathLength + 1);

         if (iPathLength > 0)
         {
            _tcsncpy(m_tszPath, tszMarkLabel, iPathLength);
            m_tszPath[iPathLength] = 0; //destroyed by _tcsncpy()
         }
      }
      else
         hr = E_IAX_INVALID_DATA;
   }

   if (SUCCEEDED(hr) && m_pNextAction != NULL)
      hr = m_pNextAction->ResolveJumpTimes(pProject, nInsertPositionMs);

   return hr;
}

HRESULT CMouseAction::CloneTo(CMouseAction *pTarget)
{
   HRESULT hr = S_OK;

   if (pTarget == NULL)
      return E_POINTER;

   pTarget->m_idAction = m_idAction;
   pTarget->m_nPageMarkId = m_nPageMarkId;

   if (pTarget->m_tszPath != NULL)
      delete[] pTarget->m_tszPath;

   if (pTarget->m_pNextAction != NULL)
      delete pTarget->m_pNextAction;

   if (m_tszPath != NULL)
   {
      int iPathLength = _tcslen(m_tszPath);
      pTarget->m_tszPath = new _TCHAR[iPathLength + 1];
      ZeroMemory(pTarget->m_tszPath, iPathLength + 1);

      _tcsncpy(pTarget->m_tszPath, m_tszPath, iPathLength);
      pTarget->m_tszPath[iPathLength] = 0; // destroyed by _tcsncpy()
   }
   else
      pTarget->m_tszPath = NULL;

   if (m_pNextAction != NULL)
   {
      pTarget->m_pNextAction = new CMouseAction;
      hr = m_pNextAction->CloneTo(pTarget->m_pNextAction);
   }
   else
      pTarget->m_pNextAction = NULL;

   return hr;
}

CMouseAction* CMouseAction::Copy()
{
   CMouseAction *pNewAction = new CMouseAction();
   HRESULT hr = CloneTo(pNewAction);

   if (SUCCEEDED(hr))
      return pNewAction;
   else
   {
      delete pNewAction;
      return NULL;
   }
}


HRESULT CMouseAction::AddFollowingAction(CMouseAction *pAction)
{
   HRESULT hr = S_OK;

   if (m_pNextAction != NULL)
      hr = m_pNextAction->AddFollowingAction(pAction);
   else
   {
      m_pNextAction = new CMouseAction;
      hr = pAction->CloneTo(m_pNextAction);
   }

   return hr;
}  
   
void CMouseAction::SetActionId(AreaActionTypeId actionId) 
{ 
    m_idAction = actionId; 
}

HRESULT CMouseAction::Execute(CEditorDoc *pDoc)
{
   HRESULT hr = S_OK;

   CEditorProject *pProject = &pDoc->project;

   UINT nPreviewMs = pDoc->GetCurrentPreviewPos();
   HINSTANCE hResult = 0;
   UINT nJumpMs = 0xffffffff;
   CString csMessage;

   switch(m_idAction)
   {
   case INTERACTION_JUMP_NEXT_PAGE:
       pDoc->JumpPreview(pProject->GetNextPageBegin(nPreviewMs));
       break;

   case INTERACTION_JUMP_PREVIOUS_PAGE:
       pDoc->JumpPreview(pProject->GetPrevPageBegin(nPreviewMs, true));
       break;

   case INTERACTION_JUMP_SPECIFIC_PAGE:
       nJumpMs = pProject->GetTimeForPageId(m_nPageMarkId);
       if (nJumpMs != 0xffffffff)
           pDoc->JumpPreview(nJumpMs);
       // else page undefined
       break;

   case INTERACTION_JUMP_TARGET_MARK:
       nJumpMs = pProject->GetTimeForMarkId(m_nPageMarkId);
       if (nJumpMs != 0xffffffff)
           pDoc->JumpPreview(nJumpMs);
       // else mark undefined
       break;
   case INTERACTION_JUMP_RANDOM_QUESTION:
       {
           pDoc->project.StartRandomTest(true);
           CQuestionnaireEx *pQuestionnaire = pProject->GetFirstQuestionnaireEx();
           if (!pQuestionnaire) {
               ASSERT(FALSE);
               break;
           }
           if (!pQuestionnaire->AnyQuestionsLeft()) {
               break;// There is no reason to make any jumps to random question after the test finished
           }
           if (pQuestionnaire->GetRandomQuestionReplayIndex() == pQuestionnaire->GetNumberOfRandomQuestions()) {
               pDoc->JumpPreview(pProject->GetLastQuestionEndTimeStamp() + 1);
           } else {
               CQuestionEx *pRandomQuestion = pQuestionnaire->GetRandomQuestionFromPosition(pQuestionnaire->GetRandomQuestionReplayIndex());
               pDoc->JumpPreview(pRandomQuestion->GetPageBegin());
           }
           break;
       }
   case INTERACTION_OPEN_URL:
   case INTERACTION_OPEN_FILE:
       // TODO m_tszPath could be local
       hResult = ::ShellExecute(NULL, _T("open"), m_tszPath, _T(""), _T(""), SW_SHOWNORMAL);
       if ((int)hResult <= 32)
       {
           csMessage.Format(IDS_MSG_FILE_FAILED, m_tszPath);
           pDoc->ShowErrorMessage(csMessage);
       }
       break;

   case INTERACTION_START_REPLAY:
       pDoc->StartPreview();
       break;

   case INTERACTION_STOP_REPLAY:
       if (pDoc->IsPreviewActive())
           pDoc->PausePreviewSpecial();
       break;

   case INTERACTION_AUDIO_ON:
       csMessage.LoadString(IDS_MSG_AUDIO_ON);
       pDoc->ShowWarningMessage(csMessage);
       break;

   case INTERACTION_AUDIO_OFF:
       csMessage.LoadString(IDS_MSG_AUDIO_OFF);
       pDoc->ShowWarningMessage(csMessage);
       break;

   case INTERACTION_EXIT_PROCESS:
       csMessage.LoadString(IDS_MSG_CLOSE_INSTANCE);
       pDoc->ShowWarningMessage(csMessage);
       break;

   case INTERACTION_RESET_QUESTION:
       pProject->ResetQuestion();
       break;

   case INTERACTION_CHECK_QUESTION:
       if (pDoc->IsPreviewActive())
           pDoc->PausePreviewSpecial();
       pProject->CheckQuestion();
       break;

   }


   if (m_pNextAction != NULL)
       hr = m_pNextAction->Execute(pDoc);


   return hr;
}

HRESULT CMouseAction::AppendData(CArrayOutput *pTarget, LPCTSTR tszExportPrefix, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (pTarget == NULL)
      return E_POINTER;

   static _TCHAR tszTarget[2000];
   _TCHAR *sTarget = tszTarget;
   tszTarget[0] = 0;

   UINT nTimeMs = 0;


   CString csOutputFileName = m_tszPath;
   if (csOutputFileName.GetLength() > 0)
   {
      bool bIsFile = _taccess(m_tszPath, 04) == 0;

      if (bIsFile)
      {
         if (tszExportPrefix == NULL)
         {
            // LEP output: preserve full path
            // it doesn't matter here if it is a URL or a path: only "\" are replaced
            
            _TCHAR tszEscapedPath[1200];
            
            // TODO remove/merge with other occurances (for exampel DrawSdk::Image::Write())
            ZeroMemory(tszEscapedPath, 1200);
            int iOutputPos = 0;
            int iLength = _tcslen(m_tszPath) + 1;
            for (int i=0; i<iLength; ++i)
            {
               if (m_tszPath[i] == '\\')
                  tszEscapedPath[iOutputPos++] = '\\';
               
               tszEscapedPath[iOutputPos++] = m_tszPath[i];
            }
            
            csOutputFileName = tszEscapedPath;
         }
         else
         {
            // this is an export:
            // make a copy of the file to target directory
            
            StringManipulation::GetFilename(csOutputFileName);

            if (m_nNameUsageCount > 0)
            {
               // there is more than one file with this name: change the output name

               CString csPrefix = csOutputFileName;
               CString csSuffix = csOutputFileName;
               CString csNumber;
               csNumber.Format(_T("_%03d."), m_nNameUsageCount);

               StringManipulation::GetFilePrefix(csPrefix);
               StringManipulation::GetFileSuffix(csSuffix);

               csOutputFileName = csPrefix;
               csOutputFileName += csNumber;
               csOutputFileName += csSuffix;
            }
            
            CString csSourceFile = m_tszPath;

            CString csTargetFile = tszExportPrefix;
            StringManipulation::GetPath(csTargetFile);
            csTargetFile += _T("\\");
            csTargetFile += csOutputFileName;

            bool bTargetExists = _taccess(csTargetFile, 00) == 0;
            if (bTargetExists)
            {
               bTargetExists = false; // copy anyway except if they are really the same

               if (csSourceFile.CompareNoCase(csTargetFile) != 0)
               {
                  CFile file1(csSourceFile, CFile::modeRead);
                  CFile file2(csTargetFile, CFile::modeRead);
                  
                  if (file1.GetLength() == file2.GetLength())
                  {
                     CFileStatus fileStatus1;
                     CFileStatus fileStatus2;
                     
                     file1.GetStatus(fileStatus1);
                     file2.GetStatus(fileStatus2);
                     
                     BOOL bTimesTheSame = fileStatus1.m_mtime == fileStatus2.m_mtime;
                     
                     if (bTimesTheSame)
                        bTargetExists = true;
                  }
               }
               else
               {
                  // same source and target path: make no copy/this is a strange special case
                  bTargetExists = true;
               }
            }

            if (!bTargetExists)
               ::CopyFile(csSourceFile, csTargetFile, FALSE);
            // TODO what about error here?
         }
      }
      // else: no file: just take the m_tszPath/csOutputFileName as address


      // TODO what about INTERACTION_OPEN_FILE (or URL) with a local file that 
      // cannot be copied??
      // What about the target file cannot be overwritten?
   }
   

   switch(m_idAction)
   {
   case INTERACTION_JUMP_NEXT_PAGE:
      _tcscat(sTarget, _T("jump next"));
      sTarget += _tcslen(sTarget);
      
      break;

   case INTERACTION_JUMP_PREVIOUS_PAGE:
      _tcscat(sTarget, _T("jump prev"));
      sTarget += _tcslen(sTarget);
      
      break;

   case INTERACTION_JUMP_SPECIFIC_PAGE:
      nTimeMs = pProject->GetTimeForPageId(m_nPageMarkId);
      if (nTimeMs != 0xffffffff)
      {
         _stprintf(sTarget, _T("jump page %d"), nTimeMs);
         sTarget += _tcslen(sTarget);
      }
      // else page undefined
      break;

   case INTERACTION_JUMP_TARGET_MARK:
      nTimeMs = pProject->GetTimeForMarkId(m_nPageMarkId);
      if (nTimeMs != 0xffffffff)
      {
         _stprintf(sTarget, _T("jump mark %d"), nTimeMs);
         sTarget += _tcslen(sTarget);
      }
      // else mark undefined

      break;
   case INTERACTION_JUMP_RANDOM_QUESTION:
      _tcscat(sTarget, _T("jump question"));
      sTarget += _tcslen(sTarget);
       break;
   case INTERACTION_OPEN_URL:
      // TODO what about TCHAR here? path/url only 8-bit? Maybe not!
      _stprintf(sTarget, _T("open-url %s"), csOutputFileName);
      sTarget += _tcslen(sTarget);
      
      break;
   case INTERACTION_OPEN_FILE:
      // TODO what about TCHAR here? path/url only 8-bit? Maybe not!


      _stprintf(sTarget, _T("open-file %s"), csOutputFileName);
      sTarget += _tcslen(sTarget);
      
      break;

   case INTERACTION_START_REPLAY:
      _tcscat(sTarget, _T("start"));
      sTarget += _tcslen(sTarget);
      
      break;

   case INTERACTION_STOP_REPLAY:
      _tcscat(sTarget, _T("stop"));
      sTarget += _tcslen(sTarget);
      
      break;

   case INTERACTION_AUDIO_ON:
      _tcscat(sTarget, _T("audio on"));
      sTarget += _tcslen(sTarget);
      
      break;
   case INTERACTION_AUDIO_OFF:
      _tcscat(sTarget, _T("audio off"));
      sTarget += _tcslen(sTarget);
      
      break;

   case INTERACTION_EXIT_PROCESS:
      _tcscat(sTarget, _T("close"));
      sTarget += _tcslen(sTarget);
      
      break;

   case INTERACTION_RESET_QUESTION:
      _tcscat(sTarget, _T("reset"));
      sTarget += _tcslen(sTarget);
      
      break;

   case INTERACTION_CHECK_QUESTION:
      _tcscat(sTarget, _T("check"));
      sTarget += _tcslen(sTarget);
      
      break;
   }

   if (SUCCEEDED(hr))
      hr = pTarget->Append(tszTarget); 

   //ASSERT(!(m_pNextAction != NULL && m_pNextAction->GetActionId() == INTERACTION_NO_ACTION && m_idAction == INTERACTION_NO_ACTION));
   if (m_pNextAction != NULL && m_pNextAction->GetActionId() != INTERACTION_NO_ACTION && m_idAction != INTERACTION_NO_ACTION)
   {
      pTarget->Append(_T(";"));
      
      hr = m_pNextAction->AppendData(pTarget, tszExportPrefix, pProject);
   }


   return hr;
}

void CMouseAction::GetActionStrings(CStringArray &aActionStrings, bool bIsRandomTest)
{
   CString csAction;
   csAction.LoadString(IDS_NO_ACTION);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_NEXT_PAGE);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_PREVIOUS_PAGE);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_SPECIAL_PAGE);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_TARGET_MARK);
	aActionStrings.Add(csAction);
    if (bIsRandomTest) {
        csAction.LoadString(IDS_NEXT_RANDOM_QUESTION);
        aActionStrings.Add(csAction);
    }
   csAction.LoadString(IDS_OPEN_URL);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_OPEN_FILE);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_START_REPLAY);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_STOP_REPLAY);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_SWITCH_AUDIO_ON);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_SWITCH_AUDIO_OFF);
	aActionStrings.Add(csAction);
   csAction.LoadString(IDS_EXIT_PROCESS);
	aActionStrings.Add(csAction);
}

AreaActionStringId CMouseAction::GetActionString(CMouseAction *pMouseAction)
{
   AreaActionTypeId nActionId = pMouseAction->GetActionId();

   CMouseAction *pNextAction = NULL;

   if (nActionId == INTERACTION_NO_ACTION)
      return NO_ACTION;
   else if (nActionId == INTERACTION_JUMP_NEXT_PAGE)
   {
      pNextAction = pMouseAction->GetNextAction();
      if (!pNextAction)
         return NEXT_PAGE;
      else if (pNextAction->GetActionId() == INTERACTION_START_REPLAY)
         return NEXT_PAGE_STARTREPLAY;
      else if (pNextAction->GetActionId() == INTERACTION_STOP_REPLAY)
         return NEXT_PAGE_STOPREPLAY;
   }
   else if (nActionId == INTERACTION_JUMP_PREVIOUS_PAGE)
   {
      pNextAction = pMouseAction->GetNextAction();
      if (!pNextAction)
         return PREVIOUS_PAGE;
      else if (pNextAction->GetActionId() == INTERACTION_START_REPLAY)
         return PREVIOUS_PAGE_STARTREPLAY;
      else if (pNextAction->GetActionId() == INTERACTION_STOP_REPLAY)
         return PREVIOUS_PAGE_STOPREPLAY;
   }
   else if (nActionId == INTERACTION_JUMP_SPECIFIC_PAGE)
   {
      pNextAction = pMouseAction->GetNextAction();
      if (!pNextAction)
         return SPECIFIC_PAGE;
      else if (pNextAction->GetActionId() == INTERACTION_START_REPLAY)
         return SPECIFIC_PAGE_STARTREPLAY;
      else if (pNextAction->GetActionId() == INTERACTION_STOP_REPLAY)
         return SPECIFIC_PAGE_STOPREPLAY;
   }
   else if (nActionId == INTERACTION_JUMP_TARGET_MARK)
   {
      pNextAction = pMouseAction->GetNextAction();
      if (!pNextAction)
         return TARGET_MARK;
      else if (pNextAction->GetActionId() == INTERACTION_START_REPLAY)
         return TARGET_MARK_STARTREPLAY;
      else if (pNextAction->GetActionId() == INTERACTION_STOP_REPLAY)
         return TARGET_MARK_STOPREPLAY;
   }
   else if (nActionId == INTERACTION_JUMP_RANDOM_QUESTION) {
      return RANDOM_QUESTION;
   }
   else if (nActionId == INTERACTION_OPEN_URL)
   {
      pNextAction = pMouseAction->GetNextAction();
      if (!pNextAction)
         return OPEN_URL;
      else if (pNextAction->GetActionId() == INTERACTION_START_REPLAY)
         return OPEN_URL_STARTREPLAY;
      else if (pNextAction->GetActionId() == INTERACTION_STOP_REPLAY)
         return OPEN_URL_STOPREPLAY;
   }
   else if (nActionId == INTERACTION_OPEN_FILE)
   {
      pNextAction = pMouseAction->GetNextAction();
      if (!pNextAction)
         return OPEN_FILE;
      else if (pNextAction->GetActionId() == INTERACTION_START_REPLAY)
         return OPEN_FILE_STARTREPLAY;
      else if (pNextAction->GetActionId() == INTERACTION_STOP_REPLAY)
         return OPEN_FILE_STOPREPLAY;
   }
   else if (nActionId == INTERACTION_START_REPLAY)
   {
      return START_REPLAY;
   }
   else if (nActionId == INTERACTION_STOP_REPLAY)
   {
      return STOP_REPLAY;
   }
   else if (nActionId == INTERACTION_AUDIO_ON)
   {
      pNextAction = pMouseAction->GetNextAction();
      if (!pNextAction)
         return AUDIO_ON;
      else if (pNextAction->GetActionId() == INTERACTION_START_REPLAY)
         return AUDIO_ON_STARTREPLAY;
      else if (pNextAction->GetActionId() == INTERACTION_STOP_REPLAY)
         return AUDIO_ON_STOPREPLAY;
   }
   else if (nActionId == INTERACTION_AUDIO_OFF)
   {
      pNextAction = pMouseAction->GetNextAction();
      if (!pNextAction)
         return AUDIO_OFF;
      else if (pNextAction->GetActionId() == INTERACTION_START_REPLAY)
         return AUDIO_OFF_STARTREPLAY;
      else if (pNextAction->GetActionId() == INTERACTION_STOP_REPLAY)
         return AUDIO_OFF_STOPREPLAY;
   }
   else if (nActionId == INTERACTION_EXIT_PROCESS)
   {
      return EXIT_PROCESS;
   }

   return NO_ACTION;
}

AreaActionTypeId CMouseAction::GetActionId(AreaActionStringId nActionStringId, AreaActionTypeId &nNextActionId)
{
   AreaActionTypeId nActionId = INTERACTION_NO_ACTION;

   switch (nActionStringId)
   {
   case NO_ACTION:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_NO_ACTION;
      break;
   case NEXT_PAGE:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_JUMP_NEXT_PAGE;
      break;
   case NEXT_PAGE_STARTREPLAY:
      nNextActionId = INTERACTION_START_REPLAY;
      return INTERACTION_JUMP_NEXT_PAGE;
      break;
   case NEXT_PAGE_STOPREPLAY:
      nNextActionId = INTERACTION_STOP_REPLAY;
      return INTERACTION_JUMP_NEXT_PAGE;
      break;      
   case PREVIOUS_PAGE:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_JUMP_PREVIOUS_PAGE;
      break;
   case RANDOM_QUESTION:
      nNextActionId = INTERACTION_START_REPLAY;
      return INTERACTION_JUMP_RANDOM_QUESTION;
      break;
   case PREVIOUS_PAGE_STARTREPLAY:
      nNextActionId = INTERACTION_START_REPLAY;
      return INTERACTION_JUMP_PREVIOUS_PAGE;
      break;
   case PREVIOUS_PAGE_STOPREPLAY:
      nNextActionId = INTERACTION_STOP_REPLAY;
      return INTERACTION_JUMP_PREVIOUS_PAGE;
      break; 
   case SPECIFIC_PAGE:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_JUMP_SPECIFIC_PAGE;
      break;
   case SPECIFIC_PAGE_STARTREPLAY:
      nNextActionId = INTERACTION_START_REPLAY;
      return INTERACTION_JUMP_SPECIFIC_PAGE;
      break;
   case SPECIFIC_PAGE_STOPREPLAY:
      nNextActionId = INTERACTION_STOP_REPLAY;
      return INTERACTION_JUMP_SPECIFIC_PAGE;
      break;
   case TARGET_MARK:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_JUMP_TARGET_MARK;
      break;
   case TARGET_MARK_STARTREPLAY:
      nNextActionId = INTERACTION_START_REPLAY;
      return INTERACTION_JUMP_TARGET_MARK;
      break;
   case TARGET_MARK_STOPREPLAY:
      nNextActionId = INTERACTION_STOP_REPLAY;
      return INTERACTION_JUMP_TARGET_MARK;
      break;
   case OPEN_URL:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_OPEN_URL;
      break;
   case OPEN_URL_STARTREPLAY:
      nNextActionId = INTERACTION_START_REPLAY;
      return INTERACTION_OPEN_URL;
      break;
   case OPEN_URL_STOPREPLAY:
      nNextActionId = INTERACTION_STOP_REPLAY;
      return INTERACTION_OPEN_URL;
      break; 
   case OPEN_FILE:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_OPEN_FILE;
      break;
   case OPEN_FILE_STARTREPLAY:
      nNextActionId = INTERACTION_START_REPLAY;
      return INTERACTION_OPEN_FILE;
      break;
   case OPEN_FILE_STOPREPLAY:
      nNextActionId = INTERACTION_STOP_REPLAY;
      return INTERACTION_OPEN_FILE;
      break;
   case START_REPLAY:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_START_REPLAY;
   case STOP_REPLAY:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_STOP_REPLAY;
      break;
   case AUDIO_ON:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_AUDIO_ON;
      break;
   case AUDIO_ON_STARTREPLAY:
      nNextActionId = INTERACTION_START_REPLAY;
      return INTERACTION_AUDIO_ON;
      break;
   case AUDIO_ON_STOPREPLAY:
      nNextActionId = INTERACTION_STOP_REPLAY;
      return INTERACTION_AUDIO_ON;
      break;
   case AUDIO_OFF:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_AUDIO_OFF;
      break;
   case AUDIO_OFF_STARTREPLAY:
      nNextActionId = INTERACTION_START_REPLAY;
      return INTERACTION_AUDIO_OFF;
      break;
   case AUDIO_OFF_STOPREPLAY:
      nNextActionId = INTERACTION_STOP_REPLAY;
      return INTERACTION_AUDIO_OFF;
      break;
   case EXIT_PROCESS:
      nNextActionId = INTERACTION_NO_ACTION;
      return INTERACTION_EXIT_PROCESS;
      break;
   }

   return INTERACTION_NO_ACTION;
}


//////////////////////////////////////////////////////////////////////////////

// AssistantDoc.cpp : implementation of the CAssistantDoc class
//

#include "stdafx.h"

#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "AssistantDoc.h"
#include "MainFrm.h"
#include "backend\mlb_misc.h"
#include "backend\la_project.h"
#include "DocumentManager.h"
#include "..\Studio\Studio.h"

#include "lutils.h"  // lutils.dll
#include "RegistrySettings.h"  // lutils.dll
#include "MiscFunctions.h"

#include "LanguageSupport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAssistantDoc

IMPLEMENT_DYNCREATE(CAssistantDoc, CDocument)

BEGIN_MESSAGE_MAP(CAssistantDoc, CDocument)
	//{{AFX_MSG_MAP(CAssistantDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// static
bool CAssistantDoc::IsDoRegistryProject() {
#ifdef _STUDIO
    return true;
#endif

    return false;

    // This should be done / or something similar:
    //CString csCommand = ::GetCommandLine();
    //csCommand.ToLowerCase();
    //return csCommand.Find(_T("studio.exe")) >= 0;
}

/////////////////////////////////////////////////////////////////////////////
// CAssistantDoc construction/destruction

CAssistantDoc::CAssistantDoc()
{
   Init();
   m_snapChecked = 0;
   m_displayGrid = 0;
}

CAssistantDoc::~CAssistantDoc()
{
    SaveSettings();
   Clear();
}

/////////////////////////////////////////////////////////////////////////////
// CAssistantDoc serialization

void CAssistantDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
      CString filePath = ar.GetFile()->GetFilePath();

      if (IsDoRegistryProject()) {
          if (OpenSourceDocument(filePath) != S_OK)
              throw new CArchiveException(); //("Krumpf");
      }
      else {
#ifndef _STUDIO
          if (!OpenProject(filePath))
              throw new CArchiveException(); //("Krumpf");
#endif
      }

	}
}

/////////////////////////////////////////////////////////////////////////////
// CAssistantDoc diagnostics

#ifdef _DEBUG
void CAssistantDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAssistantDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAssistantDoc commands

BOOL CAssistantDoc::SaveModified()
{
    bool bSaveSuccess = true; 

    if (ASSISTANT::Project::active != NULL)
        bSaveSuccess = ASSISTANT::Project::active->SaveModified();

    return bSaveSuccess;
}

void CAssistantDoc::Init()
{
   m_nToolId = ID_TOOL_SELECT;
   m_pProgress = NULL;
   m_hDialogInitEvent = 0;

#ifndef _STUDIO
   m_bFreezeImages = ((CAssistantApp *)AfxGetApp())->GetFreezeImages();
   m_bFreezeMaster = ((CAssistantApp *)AfxGetApp())->GetFreezeMaster();
   m_bFreezeDocument = ((CAssistantApp *)AfxGetApp())->GetFreezeDocument();
#else
   ReadSettings();
#endif

#ifdef _STUDIO
   ASSISTANT::Project::InitAvGrabber();
#endif

   if (IsDoRegistryProject()) {
       bool doWithSG = true;

       const _TCHAR *tszDesiredLrdName = NULL;
       if (CStudioApp::IsLiveContextMode())
           tszDesiredLrdName = CStudioApp::GetLiveContextRecordFile();

       ASSISTANT::Project::active = 
           new ASSISTANT::Project(doWithSG, m_bFreezeImages, 
           m_bFreezeMaster, m_bFreezeDocument, tszDesiredLrdName);
   }

   m_bIsFullScreenOn = false;
}

void CAssistantDoc::Clear()
{
#ifdef _STUDIO
    ASSISTANT::Project::DeleteAvGrabber();
#endif
}

void CAssistantDoc::ReadSettings()
{
   m_bShowImportWarning = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("DoNotDisplayImportWarning"), 0) == 0 ? true : false;

   m_bShowMissingFontsWarning = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("DoNotDisplayMissingFontsWarning"), 0) == 0 ? true : false;

   m_bShowAudioWarning = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("DoNotDisplayAudioWarning"), 0) == 0 ? true : false;

   m_bShowUseSmartBoard = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("DoNotShowUseSmartBoard"), 0) == 0 ? true : false;

   m_bFreezeImages = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("FreezeImages"), 0) == 0 ? false : true;
   
   m_bFreezeMaster = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("FreezeMaster"), 1) == 0 ? false : true;

   m_bFreezeDocument = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("FreezeDocument"), 1) == 0 ? false : true;
   

   m_bShowMouseCursor = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("ShowCursor"), 1) == 0 ? false : true;
   
   m_bShowStartupPage = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("ShowStartupPage"), 1) == 0 ? false : true;

   m_bSwitchAutomatically = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("SwitchAutomaticallyToEdit"), 0) == 0 ? false : true;

   m_bShowStatusBar = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("ShowStatusBar"), 1) == 0 ? false : true;

   m_bShowAVMonitor = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("ShowAVMonitor"), 1) == 0 ? false : true;

   m_bShowAudioMonitor = 
      AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("ShowAudioMonitor"), 1) == 0 ? false : true;
}

void CAssistantDoc::SaveSettings()
{
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("DoNotDisplayImportWarning"), !m_bShowImportWarning);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("DoNotDisplayMissingFontsWarning"), !m_bShowMissingFontsWarning);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("DoNotDisplayAudioWarning"), !m_bShowAudioWarning);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("DoNotShowUseSmartBoard"), !m_bShowUseSmartBoard);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("FreezeImages"), m_bFreezeImages);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("FreezeMaster"), m_bFreezeMaster);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("FreezeDocument"), m_bFreezeDocument);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("ShowCursor"), m_bShowMouseCursor);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("ShowStartupPage"), m_bShowStartupPage);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("SwitchAutomaticallyToEdit"), m_bSwitchAutomatically);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("ShowStatusBar"), m_bShowStatusBar);
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("ShowAVMonitor"), m_bShowAVMonitor);
}

BOOL CAssistantDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // Create new project if no project exists
    if (ASSISTANT::Project::active == NULL) {
        bool doWithSG = true;

       const _TCHAR *tszDesiredLrdName = NULL;
       if (CStudioApp::IsLiveContextMode())
           tszDesiredLrdName = CStudioApp::GetLiveContextRecordFile();

        ASSISTANT::Project::active = new ASSISTANT::Project(doWithSG, 
            m_bFreezeImages, m_bFreezeMaster, m_bFreezeDocument, tszDesiredLrdName);
    }

    // Close all document presentation contents
    if (ASSISTANT::Project::active != NULL) { 
        // 
        bool bClosed = ASSISTANT::Project::active->CloseDocumentsAndPresentations();
        if (!bClosed)
            return FALSE;
        
        CMainFrameA *pFrame = CMainFrameA::GetCurrentInstance();
        if (pFrame) {
            CDocumentStructureView *pStructureTree = pFrame->GetStructureTree();
            if (pStructureTree)
                pStructureTree->RemoveAll();
            CRecordingsView *pRecodingView = pFrame->GetRecordingView();
            if (pRecodingView)
                pRecodingView->RemoveAll();
        }

        ASSISTANT::Project::active->LoadDocumentsAndPresentations();
    
    }

    RebuildStructureTree();
    RebuildRecordingTree();

    return TRUE;
}

BOOL CAssistantDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
   CString fileSuffix = lpszPathName;
   StringManipulation::GetFileSuffix(fileSuffix);
   fileSuffix.MakeUpper();

   if (fileSuffix == _T("LSD"))
   {
       if (IsDoRegistryProject()) {
           OpenSourceDocument(lpszPathName);
       }
       else {
#ifndef _STUDIO
           bool doWithSG = true;
           if (ASSISTANT::Project::active == NULL)
           {
               CStringArray aLastProjects;
               CString csSection = _T("Recent File List");
               CString csText;
               int i = 1;
               do 
               {
                   CString csKey;
                   csKey.Format(_T("File%d"), i);
                   csText = AfxGetApp()->GetProfileString(csSection, csKey);
                   if (!csText.IsEmpty())
                       aLastProjects.Add(csText);
                   ++i;
               } while (!csText.IsEmpty());


               CStringArray aContainingList;
               if (aLastProjects.GetSize() > 0)
                   ASSISTANT::FindProject(lpszPathName, aContainingList, aLastProjects);

               if (aContainingList.GetSize() > 0)
               {
                   OpenProject(aContainingList[0]);
               }
               else
               {
                   ASSISTANT::Project::active = 
                       new ASSISTANT::Project(doWithSG, m_bFreezeImages, m_bFreezeMaster, m_bFreezeDocument);

                   OpenSourceDocument(lpszPathName);
               }
           }
#endif
       }
   }
   else
   {
       if (!CDocument::OnOpenDocument(lpszPathName))
           return FALSE;

   }

   UpdateAllViews(NULL);

   return TRUE;
}

#ifndef _STUDIO
bool CAssistantDoc::OpenProject(LPCTSTR szFilename)
{
   CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();

   if (pMainWnd == NULL)
       return true; // not yet ready; nothing to do

   if (pMainWnd != NULL)
   {
      // Clear recordings and Document structure views
      pMainWnd->RemoveViewsContent();
   }

   bool success = StartOpen(szFilename);
   
   if (pMainWnd != NULL)
   {
       RebuildStructureTree();
      
      CRecordingsView *pRecodingView = pMainWnd->GetRecordingView();
      if (pRecodingView && ASSISTANT::Project::active)
         ASSISTANT::Project::active->ReBuildRecordingTree(pRecodingView);
   }

   if (success) {
       if (pMainWnd != NULL) {
           pMainWnd->CheckAssistantMainView();
           pMainWnd->CloseStartPage();
       }
   }

   UpdateAllViews(NULL);


   return true; //success;
}
#endif

#ifndef _STUDIO
bool CAssistantDoc::StartOpen(LPCTSTR szFilename)
{
   m_hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   
   m_pProgress = new CProgressDialogA(AfxGetMainWnd(), m_hDialogInitEvent);
   
   if (ASSISTANT::Project::active != NULL)
   {
      delete ASSISTANT::Project::active;
      ASSISTANT::Project::active = NULL;
   }

   bool doWithSG = true;
   ASSISTANT::Project::active = 
      new ASSISTANT::Project(doWithSG, m_bFreezeImages, m_bFreezeMaster, m_bFreezeDocument);

   OpenProjectStruct openStruct;
   openStruct.pDoc = this;
   openStruct.pProject = ASSISTANT::Project::active;
   openStruct.szProjectName = szFilename;
   openStruct.pProgress = m_pProgress;
   openStruct.hr = S_OK;
   
   CWinThread *thread = AfxBeginThread(OpenThread, (LPVOID)&openStruct);

   if (m_pProgress)
      m_pProgress->DoModal();
   
   if (m_pProgress)
      delete m_pProgress;
   m_pProgress = NULL;

   CloseHandle(m_hDialogInitEvent);
   m_hDialogInitEvent = NULL;
   

   if (openStruct.hr != S_OK)
   {
      ASSISTANT::Project::active = NULL;
      return false;
   }
   else
   {
      ASSISTANT::Project::active->ActivatePage();
   }
   
   return true;
}
#endif

#ifndef _STUDIO
UINT CAssistantDoc::OpenThread(LPVOID pParam)
{
   HRESULT hr = S_OK;

   hr = CLanguageSupport::SetThreadLanguage();

   // necessary to prevent progree window with hwnd==NULL
   Sleep(100);

   OpenProjectStruct *pOpenStruct = (OpenProjectStruct *)pParam;

   CAssistantDoc *pDoc = pOpenStruct->pDoc;
   CProgressDialogA *pProgress = pOpenStruct->pProgress;
   ASSISTANT::Project *pProject = pOpenStruct->pProject;
   CString csProjectName = pOpenStruct->szProjectName;

   bool bOpenSuccess = true;
   bOpenSuccess = pProject->Open(csProjectName, pProgress);

   if (bOpenSuccess)
      pOpenStruct->hr = S_OK;
   else
      pOpenStruct->hr = E_FAIL;

   if (pProgress != NULL)
   {
      PostMessage(pProgress->GetSafeHwnd(), WM_USER, CProgressDialogA::END_DIALOG, NULL);
   }
   
   return 0;
}
#endif

void CAssistantDoc::UpdateAfterStructureChange()
{
    CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
    if (pMainWnd != NULL)
        pMainWnd->CheckAssistantMainView();

    RebuildStructureTree();
    UpdateAllViews(NULL);
}

BOOL CAssistantDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->Save(lpszPathName);

   SetModifiedFlag(FALSE);
	return TRUE;
}

void CAssistantDoc::OnCloseDocument() 
{
    // Note: Storing documents (lsd/lrd) in registry is done in la_project.cpp::Close()

    CDocument::OnCloseDocument();
}

BOOL CAssistantDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	CString csNewName = lpszPathName;

	if (csNewName.IsEmpty())
	{
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		csNewName = m_strPathName;
		if (bReplace && csNewName.IsEmpty())
		{
			csNewName = m_strTitle;
			// check for dubious filename
			int iBad = csNewName.FindOneOf(_T(" #%;/\\"));
			if (iBad != -1)
				csNewName.ReleaseBuffer(iBad);

			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
			  !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == _T('.'));
				csNewName += strExt;
			}
		}

      // This is the original MFC CDocument code:
      /*
		if (!AfxGetApp()->DoPromptFileName(newName,
		  bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate))
			return FALSE;       // don't even attempt to save
      */
      // Which was replaced with this code:

      ASSISTANT::GetSaveFilename(IDS_LAP_FILTER, csNewName, _T(".lap"), _T("LAP"));

      if (csNewName.IsEmpty())
         return FALSE;
	}

	CWaitCursor wait;

	if (!OnSaveDocument(csNewName))
	{
		if (lpszPathName == NULL)
		{
			// be sure to delete the file
			TRY
			{
				CFile::Remove(csNewName);
			}
			CATCH_ALL(e)
			{
				TRACE0("Warning: failed to delete file after failed SaveAs.\n");
				//DELETE_EXCEPTION(e);
			}
			END_CATCH_ALL
		}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
		SetPathName(csNewName);

	return TRUE;        // success
}


bool CAssistantDoc::HasOpenProject()
{
   if (ASSISTANT::Project::active == NULL)
      return false;
   else
      return true;
}

ASSISTANT::Project* CAssistantDoc::GetOpenProject() {
    // Currently not necessary ("active" is public static) but 
    // preparation for a better design. Introduced with CDocumentManager.
    return ASSISTANT::Project::active;
}

bool CAssistantDoc::HasLoadedDocuments()
{
    if (ASSISTANT::Project::active != NULL)
        return ASSISTANT::Project::active->HasLoadedDocuments();

    return false;
}

bool CAssistantDoc::ActiveDocumentIsLoaded()
{
    if (ASSISTANT::Project::active != NULL && 
        ASSISTANT::Project::active->GetActiveDocument() != NULL)
        return ASSISTANT::Project::active->GetActiveDocument()->IsLoaded();

    return false;
}

bool CAssistantDoc::IsSgOnlyDocument()
{
    if (ASSISTANT::Project::active != NULL)
        return ASSISTANT::Project::active->IsSgOnlyDocument();

    return false;
}

bool CAssistantDoc::RecordingIsPossible()
{
   if (ASSISTANT::Project::active == NULL)
      return false;

   return ASSISTANT::Project::active->RecordingIsPossible();
}

bool CAssistantDoc::HasActivePage()
{
   bool bRet = false;

   if (ASSISTANT::Project::active != NULL)
   {
      if (ASSISTANT::Project::active->GetActivePage() != NULL)
         bRet = true;
   }

   return bRet;
}

bool CAssistantDoc::PageIsChanged()
{
   bool bRet = false;

   if (ASSISTANT::Project::active != NULL)
   {
      if (ASSISTANT::Project::active->PageIsChanged())
         bRet = true;
   }

   return bRet;
}

bool CAssistantDoc::PasteBufferIsEmpty()
{
   bool bRet = false;

   if (ASSISTANT::Project::active != NULL)
   {
      if (ASSISTANT::Project::active->IsPasteBufferEmpty())
         bRet = true;
   }

   return bRet;
}

HRESULT CAssistantDoc::DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;


   ASSISTANT::Project::active->DrawWhiteboard(rcWhiteboard, pDC);

   return S_OK;
}


HRESULT CAssistantDoc::SetCurrentTool(UINT nToolId)
{
   m_nToolId = nToolId;

   return S_OK;
}

HRESULT CAssistantDoc::AddNewObject()
{
   switch (m_nToolId)
   {
   case ID_TOOL_SELECT:
      {
      }
   case ID_TOOL_MOVE:
      {
      }
   case ID_TOOL_COPY:
      {
      }
   }


   return S_OK;
}

HRESULT CAssistantDoc::ModifyObject()
{

   return S_OK;
}

HRESULT CAssistantDoc::CutObjects()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->CutObjects();

   return S_OK;
}

HRESULT CAssistantDoc::DeleteObjects(bool bDoUndo)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->DeleteObjects(bDoUndo);

   return S_OK;
}

HRESULT CAssistantDoc::CopyObjects()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->CopyObjects();

   return S_OK;
}

HRESULT CAssistantDoc::PasteObjects()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->PasteObjects();

   return S_OK;
}

CAssistantView *CAssistantDoc::GetWhiteboardView()
{
   CView *pView = NULL;
   POSITION pos = GetFirstViewPosition();
   while (pos)
   {
      pView = GetNextView(pos);
      if (pView && pView->GetRuntimeClass() == RUNTIME_CLASS(CAssistantView))
         return (CAssistantView *)pView;
   }

   return NULL;
}


UINT CAssistantDoc::GoToNextPage()
{
   UINT nActivePageId = -1;

   if (ASSISTANT::Project::active != NULL)
   {
      if (GetWhiteboardView())
         GetWhiteboardView()->EndCreateObjects();
      ASSISTANT::Project::active->NextPage();
      if (ASSISTANT::Project::active->GetActivePage())
         nActivePageId = ASSISTANT::Project::active->GetActivePage()->GetID();
   }

   return nActivePageId;
}

UINT CAssistantDoc::GoToPreviousPage()
{
   UINT nActivePageId = -1;

   if (ASSISTANT::Project::active != NULL)
   {     
      if (GetWhiteboardView())
         GetWhiteboardView()->EndCreateObjects();
      ASSISTANT::Project::active->PreviousPage();
      if (ASSISTANT::Project::active->GetActivePage())
         nActivePageId = ASSISTANT::Project::active->GetActivePage()->GetID();
   }

   return nActivePageId;
}

UINT CAssistantDoc::GoToFirstPage()
{
   UINT nActivePageId = -1;

   if (ASSISTANT::Project::active != NULL)
   {     
      if (GetWhiteboardView())
         GetWhiteboardView()->EndCreateObjects();
      ASSISTANT::Project::active->FirstPage();
      if (ASSISTANT::Project::active->GetActivePage())
         nActivePageId = ASSISTANT::Project::active->GetActivePage()->GetID();
   }

   return nActivePageId;
}

HRESULT CAssistantDoc::SelectContainer(UINT nPageId)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   
   if (ASSISTANT::Project::active != NULL)
   {     
      if (GetWhiteboardView())
         GetWhiteboardView()->EndCreateObjects();
      bool bFound = ASSISTANT::Project::active->SelectContainer(nPageId);
      if (bFound && GetWhiteboardView())
         GetWhiteboardView()->RedrawWindow();

      CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
      if (pMainWnd != NULL)
          pMainWnd->UpdateStatusPages();
   }

   return S_OK;
}

HRESULT CAssistantDoc::LoadDocument(UINT nDocumentId)
{
    if (ASSISTANT::Project::active == NULL)
        return E_FAIL;

    bool bLoadSuccess = ASSISTANT::Project::active->LoadDocument(nDocumentId);

    if (bLoadSuccess)
        UpdateAfterStructureChange();

    return S_OK;
}

HRESULT CAssistantDoc::CloseDocument(bool bAskFor)
{
    if (ASSISTANT::Project::active == NULL)
        return E_FAIL;

    HRESULT hr = S_OK;
    bool bCloseSuccess = ASSISTANT::Project::active->CloseDocument(bAskFor);

    if (bCloseSuccess) {
        UpdateAfterStructureChange();
    }
    else
        hr = E_FAIL;

    return hr;
}

HRESULT CAssistantDoc::CloseDocument(UINT nDocumentId)
{
    if (ASSISTANT::Project::active == NULL)
        return E_FAIL;

    ASSISTANT::Project::active->SetActiveDocument(nDocumentId);
    HRESULT hr = CloseDocument();

    return hr;
}


HRESULT CAssistantDoc::RemoveDocumentFromList(UINT nDocumentId)
{
    if (ASSISTANT::Project::active == NULL)
        return E_FAIL;

    HRESULT hr = S_OK;

    ASSISTANT::Project::active->SetActiveDocument(nDocumentId);
    ASSISTANT::Project::active->RemoveDocumentFromList(nDocumentId);

    UpdateAfterStructureChange();

    return hr;
}

HRESULT CAssistantDoc::RenameContainer(UINT nContainerId, LPCTSTR szNewContainerName)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   
   if (ASSISTANT::Project::active != NULL)
   {     
      bool bRenamed = ASSISTANT::Project::active->RenameContainer(nContainerId, szNewContainerName);
      if (bRenamed)
         UpdateAllViews(NULL);
   }
   
   return S_OK;
}

HRESULT CAssistantDoc::DeleteContainer(UINT nContainerId)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   
   if (ASSISTANT::Project::active != NULL)
   {     
      if (GetWhiteboardView())
         GetWhiteboardView()->EndCreateObjects();
      bool bDeleted = ASSISTANT::Project::active->DeleteContainer(nContainerId);
      if (bDeleted)
      {
          UpdateAfterStructureChange();
      }
   }
   
   return S_OK;
}

HRESULT CAssistantDoc::SaveContainer(UINT nContainerId, LPCTSTR szFileName)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   
   if (ASSISTANT::Project::active != NULL)
   {     
      bool bSaved = ASSISTANT::Project::active->SaveContainer(nContainerId, szFileName);
   }
   
   return S_OK;
}

HRESULT CAssistantDoc::MoveContainer(CDocumentStructureView *pStructureTree, UINT nMoveContainerId, UINT nContainerId, bool bInsertInChapter, bool bInsertFirst)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   
   if (ASSISTANT::Project::active != NULL)
   {     
      bool bSaved = ASSISTANT::Project::active->MoveContainer(nMoveContainerId, nContainerId, bInsertInChapter, bInsertFirst);
      RebuildStructureTree(pStructureTree);
   }
   
   return S_OK;
}

HRESULT CAssistantDoc::SelectObjectsInRange(CRect &rcObject)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SelectObjects(rcObject);
   
   return S_OK;
}

HRESULT CAssistantDoc::SelectObjectContainsRange(CRect &rcObject)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SelectSingleObject(rcObject);
   
   return S_OK;
}

HRESULT CAssistantDoc::SelectAllObjects()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SelectAllObjects();
   
   return S_OK;
}

HRESULT CAssistantDoc::UnSelectObjects()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->UnSelectObjects();
   
   return S_OK;
}

bool CAssistantDoc::HasSelectedObjects()
{
   if (ASSISTANT::Project::active != NULL)
      return ASSISTANT::Project::active->HasSelectedObjects();
   
   return false;
}

HRESULT CAssistantDoc::InsertNewDocument(CDocumentStructureView *pStructureTree)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   ASSISTANT::Project::active->NewDocument();

   UpdateAfterStructureChange();

   return S_OK;
}

HRESULT CAssistantDoc::InsertNewChapter(CDocumentStructureView *pStructureTree)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   ASSISTANT::Project::active->InsertNewFolder();
   RebuildStructureTree(pStructureTree);

   UpdateAllViews(NULL);

   return S_OK;
}

HRESULT CAssistantDoc::InsertNewPage(CDocumentStructureView *pStructureTree)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   if (GetWhiteboardView())
      GetWhiteboardView()->EndCreateObjects();

   ASSISTANT::Project::active->InsertNewPage();
   RebuildStructureTree(pStructureTree);

   UpdateAllViews(NULL);

   return S_OK;
}

HRESULT CAssistantDoc::DuplicatePage(CDocumentStructureView *pStructureTree)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   if (GetWhiteboardView())
      GetWhiteboardView()->EndCreateObjects();

   ASSISTANT::Project::active->DuplicatePage();
   RebuildStructureTree(pStructureTree);

   UpdateAllViews(NULL);

   return S_OK;
}

HRESULT CAssistantDoc::SetKeywords(UINT nContainerId, LPCTSTR szKeywords)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   ASSISTANT::Project::active->SetKeywords(nContainerId, szKeywords);

   return S_OK;
}

HRESULT CAssistantDoc::OpenSourceDocument(LPCTSTR szDocumentName)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   if (szDocumentName == NULL)
      return E_INVALIDARG;

   if (ASSISTANT::Project::active != NULL)
   {
       int iPos = ASSISTANT::Project::CURRENT;
       bool bShowProgress = true;
       bool bDoLoad = true;
       ASSISTANT::Project::active->AddDocument(szDocumentName, iPos, bShowProgress, bDoLoad);
   }
   
   UpdateAfterStructureChange();

   return S_OK;
}

HRESULT CAssistantDoc::SaveSourceDocument()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SaveDocument();

   RebuildStructureTree();
   
   return S_OK;
}

HRESULT CAssistantDoc::SaveSourceDocumentAs()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SaveDocumentAs();

   RebuildStructureTree();
   
   return S_OK;
}

HRESULT CAssistantDoc::GetLastSavedDocument(CString &csLastSavedDocument)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->GetLastSavedDocument(csLastSavedDocument);
   
   return S_OK;
}

bool CAssistantDoc::Is64BitPowerPointInstalled()
{
   bool bIs64BitPowerPoint = false;
   
   _TCHAR tszPowerPointPath[_MAX_PATH];
   DWORD dwSize = _MAX_PATH;
   bool success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
      _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Powerpnt.exe"), _T("Path"),
      tszPowerPointPath, &dwSize);

   if (success)
   {
      // Registry entry read successfully --> Office 2010 is installed
      CString csPowerPointPath(tszPowerPointPath);
      if (csPowerPointPath[csPowerPointPath.GetLength() - 1] != '\\')
         csPowerPointPath += CString(_T("\\"));
      csPowerPointPath += CString(_T("POWERPNT.EXE"));

      if (!csPowerPointPath.IsEmpty())
      {
         DWORD bin_type;
         BOOL res = GetBinaryType(csPowerPointPath, &bin_type);
         if (res != FALSE)
         {
            if (bin_type == SCS_64BIT_BINARY)
            {
               // 64bit PowerPoint detected!
               bIs64BitPowerPoint = true;
            }
         } 
      }
   }
   else
   {
      // either no PowerPoint installed or a common error
      bIs64BitPowerPoint = false;
   }

   return bIs64BitPowerPoint;
}

HRESULT CAssistantDoc::ImportPowerPoint(LPCTSTR szPowerPointName, bool bAskForLsd, bool bDoRecord)
{
    HRESULT hr = S_OK;

   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   if (szPowerPointName == NULL)
      return E_INVALIDARG;
   
   if (ASSISTANT::Project::active != NULL)
   {
	   CWaitCursor wait;
      bool bSuccess = ASSISTANT::Project::active->ImportPPT(szPowerPointName, bAskForLsd, bDoRecord);
      if (!bSuccess)
          hr = E_FAIL;

   }
   
   return hr;
}

HRESULT CAssistantDoc::ChangeLineColor(COLORREF clrLine, int iTransparency)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ChangeLineColor(clrLine, iTransparency);

   return S_OK;
}

HRESULT CAssistantDoc::ChangeFillColor(COLORREF clrFill, int iTransparency)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ChangeFillColor(clrFill, iTransparency);

   return S_OK;
}

HRESULT CAssistantDoc::ChangeFillColor()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ChangeFillColor();

   return S_OK;
}

HRESULT CAssistantDoc::ChangeLineWidth(double dLineWidth)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ChangeLineWidth(dLineWidth);
   
   return S_OK;
}

HRESULT CAssistantDoc::ChangeLineStyle(int iLineStyle)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ChangeLineStyle(iLineStyle);
   
   return S_OK;
}

HRESULT CAssistantDoc::ChangeArrowStyle(int iArrowConfig, LPCTSTR szArrowStyle)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ChangeArrowStyle(iArrowConfig, szArrowStyle);
   
   return S_OK;
}

HRESULT CAssistantDoc::ChangeFontFamily(LPCTSTR szFontFamily)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ChangeFontFamily(szFontFamily);
   
   return S_OK;
}

HRESULT CAssistantDoc::ChangeFontSize(int iFontSize)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ChangeFontSize(iFontSize);
   
   return S_OK;
}

HRESULT CAssistantDoc::ChangeTextColor(COLORREF clrText)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ChangeTextColor(clrText);
   
   return S_OK;
}

HRESULT CAssistantDoc::GetPageProperties(UINT &nPageWidth, UINT &nPageHeight, COLORREF &clrBackground)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->GetPageProperties(nPageWidth, nPageHeight, clrBackground);
   
   return S_OK;
}

HRESULT CAssistantDoc::GetPagePropertiesEx(CRect rcW, UINT &nPageWidth, UINT &nPageHeight, UINT &nOffX, UINT &nOffY)
{
	if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

	if(ASSISTANT::Project::active->GetPagePropertiesEx(rcW, nPageWidth, nPageHeight, nOffX, nOffY)) 
		return S_OK;
	else
		return S_FALSE;
}

HRESULT CAssistantDoc::Undo()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->Undo();
   
   return S_OK;
}

HRESULT CAssistantDoc::StartPackProject(LPCTSTR csProjectName)
{
   m_hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

   m_pProgress = new CProgressDialogA(AfxGetMainWnd(), m_hDialogInitEvent);
   
   OpenProjectStruct packStruct;
   packStruct.pDoc = this;
   packStruct.pProject = ASSISTANT::Project::active;
   packStruct.szProjectName = csProjectName;
   packStruct.pProgress = m_pProgress;
   packStruct.hr = S_OK;
   
   CWinThread *thread = AfxBeginThread(PackThread, &packStruct);

   if (m_pProgress)
      m_pProgress->DoModal();
   
   delete m_pProgress;
   m_pProgress = NULL;

   CloseHandle(m_hDialogInitEvent);
   m_hDialogInitEvent = NULL;

   if (packStruct.hr != S_OK)
      return false;
   
   return true;
}

UINT CAssistantDoc::PackThread(LPVOID pParam)
{
   HRESULT hr = S_OK;

   hr = CLanguageSupport::SetThreadLanguage();

   OpenProjectStruct *pPackStruct = (OpenProjectStruct *)pParam;

   CProgressDialogA *pProgress = pPackStruct->pProgress;
   ASSISTANT::Project *pProject = pPackStruct->pProject;
   CString csProjectName = pPackStruct->szProjectName;
   

   if (pProject == NULL)
      hr = E_FAIL;
   
   if (SUCCEEDED(hr))
   {
      hr = pProject->Copy(csProjectName, pProgress);
   }

   pPackStruct->hr = hr;
   if (pProgress)
      PostMessage(pProgress->GetSafeHwnd(), WM_USER, CProgressDialogA::END_DIALOG, NULL);
   
   return 0;
}

// record functions
HRESULT CAssistantDoc::StartRecording()
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->StartRecording();

   return hr;
}

HRESULT CAssistantDoc::PauseRecording()
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->PauseRecording();

   return hr;
}

HRESULT CAssistantDoc::UnPauseRecording()
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->UnPauseRecording();

   return hr;
}

HRESULT CAssistantDoc::StopRecording()
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->StopRecording();

   return hr;
}

HRESULT CAssistantDoc::StartScreengrabbing(CRect &rcSelection)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->StartSg(rcSelection);

   return hr;
}

HRESULT CAssistantDoc::StopScreengrabbing()
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->StopSg();

   return hr;
}

HRESULT CAssistantDoc::StartScreengrabbingSelection() {
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
       hr = ASSISTANT::Project::active->StartDefineArea();

   return hr;
}


// audio functions
HRESULT CAssistantDoc::GetAudioDevices(CStringArray &arDevices, UINT &nDevice)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->GetAudioDevices(arDevices, nDevice);
   
   return hr;
}

HRESULT CAssistantDoc::GetSampleWidths(UINT nAudioDevice, CUIntArray &arSampleWidths, UINT &nSampleWidth)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->GetSampleWidths(nAudioDevice, arSampleWidths, nSampleWidth);

   return hr;

}

HRESULT CAssistantDoc::GetSampleRates(UINT nAudioDevice, CUIntArray &arSampleRates, UINT &nSampleRate)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->GetSampleRates(nAudioDevice, arSampleRates, nSampleRate);

   return hr;

}

HRESULT CAssistantDoc::GetAudioSources(CStringArray &arSources, UINT &nSource)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->GetAudioSources(arSources, nSource);

   return hr;
   
}

HRESULT CAssistantDoc::ActivateAudioDevice()
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->ActivateAudioDevice();

   return hr;
   
}

HRESULT CAssistantDoc::SetAudioDevice(UINT nDevice)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->SetAudioDevice(nDevice);

   return hr;
   
}

HRESULT CAssistantDoc::SetSampleWidth(UINT nSampleWidth)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->SetSampleWidth(nSampleWidth);

   return hr;
   
}

HRESULT CAssistantDoc::SetSampleRate(UINT nSampleRate)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->SetSampleRate(nSampleRate);

   return hr;
   
}

HRESULT CAssistantDoc::SetAudioSource(UINT nSource)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->SetAudioSource(nSource);

   return hr;
   
}

UINT CAssistantDoc::GetAudioSource()
{
   UINT nSource = 0;

   if (ASSISTANT::Project::active != NULL)
      nSource = ASSISTANT::Project::active->GetAudioSource();

   return nSource;
}

double CAssistantDoc::GetAudioLevel()
{
   double dAudioLevel = 0.0;
   if (ASSISTANT::Project::active != NULL)
      dAudioLevel = ASSISTANT::Project::active->GetAudioLevel();

   return dAudioLevel;
}

UINT CAssistantDoc::GetMixerVolume()
{
   HRESULT hr = S_OK;

   UINT nMixerVolumen = 0;
   if (ASSISTANT::Project::active != NULL)
      nMixerVolumen = ASSISTANT::Project::active->GetMixerVolume();

   return nMixerVolumen;
}

HRESULT CAssistantDoc::SetMixerVolume(UINT nMixerVolume)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->SetMixerVolume(nMixerVolume);

   return hr;
}

HRESULT CAssistantDoc::WriteAudioSettings()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->WriteAudioSettings();
   
   return hr;
}

HRESULT CAssistantDoc::ReadAudioSettings()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   ASSISTANT::Project::active->ReadAudioSettings(true);
   
   return S_OK;
}


// video functions
HRESULT CAssistantDoc::GetVideoSettings(bool &bRecordVideo, bool &bShowMonitor, bool &bUseAlternativeSync,
                                        bool &bSetAvOffset, int &nAvOffset)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->GetVideoSettings(bRecordVideo, bShowMonitor, bUseAlternativeSync,
                                                bSetAvOffset, nAvOffset);

   return S_OK;
}

HRESULT CAssistantDoc::GetVideoParameter(bool &bUseVFW, CStringArray &arVFWSources, UINT &nVFWIndex,
                                         CStringArray &arWDMSources, UINT &nWDMIndex, 
                                         UINT &nXResolution, UINT &nYResolution, UINT &nBitPerColor, 
                                         CArray<float, float> &arFramerates, UINT &nFrameRateIndex)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->GetVideoParameter(bUseVFW, arVFWSources, nVFWIndex,
                                                 arWDMSources, nWDMIndex, 
                                                 nXResolution, nYResolution, nBitPerColor, 
                                                 arFramerates, nFrameRateIndex);

   return S_OK;
}

HRESULT CAssistantDoc::EnableVideoRecording(bool bEnable, bool bCheckCodecs)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   HRESULT hr = ASSISTANT::Project::active->EnableVideoRecording(bEnable, bCheckCodecs);

   return hr;
}


HRESULT CAssistantDoc::ShowVideoMonitor(bool bShow)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   HRESULT hr = ASSISTANT::Project::active->ShowVideoMonitor(bShow);

   return hr;
}

HRESULT CAssistantDoc::SetAlternativeAvSync(bool bUseAlternativeSync)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   HRESULT hr = ASSISTANT::Project::active->SetAlternativeAvSync(bUseAlternativeSync);

   return hr;
}

HRESULT CAssistantDoc::SetUseAvOffset(bool bUseAvOffset)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   HRESULT hr = ASSISTANT::Project::active->SetUseAvOffset(bUseAvOffset);

   return hr;
}

HRESULT CAssistantDoc::SetAvOffset(int iAvOffset)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   HRESULT hr = ASSISTANT::Project::active->SetAvOffset(iAvOffset);

   return hr;
}

HRESULT CAssistantDoc::SetUseVFW(bool bUseVFW)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   HRESULT hr = ASSISTANT::Project::active->SetUseVFW(bUseVFW);

   return hr;
}

HRESULT CAssistantDoc::ChangeWdmDevice(CString &csWdmDevice)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   HRESULT hr = ASSISTANT::Project::active->ChangeWdmDevice(csWdmDevice);

   return hr;
}

HRESULT CAssistantDoc::ShowVideoFormat()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->ShowVideoFormat();
   
   return hr;
}

HRESULT CAssistantDoc::ShowVideoSource()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->ShowVideoSource();
   
   return hr;
}

HRESULT CAssistantDoc::ShowVideoDisplay()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->ShowVideoDisplay();
   
   return hr;
}

HRESULT CAssistantDoc::WriteVideoSettings()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->WriteVideoSettings();
   
   return hr;
}

HRESULT CAssistantDoc::ReadVideoSettings(bool bShowWarning)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   ASSISTANT::Project::active->ReadVideoSettings(bShowWarning, true);
   
   return S_OK;
}

// codec functions
HRESULT CAssistantDoc::GetCodecSettings(CStringArray &arCodecs, UINT &nCodecIndex, UINT &nQuality, 
                                        bool &bUseKeyframes, UINT &nKeyframeDistance,
                                        bool &bUseDatarateAdaption, UINT &nDatarate,
                                        bool &bHasAbout, bool &bHasConfigure)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->GetCodecSettings(arCodecs, nCodecIndex, nQuality,
                                                             bUseKeyframes, nKeyframeDistance,
                                                             bUseDatarateAdaption, nDatarate,
                                                             bHasAbout, bHasConfigure);
   
   return hr;
}

HRESULT CAssistantDoc::SetCodec(CString &csCodec, bool bShowWarning)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetCodec(csCodec, bShowWarning);
   
   return hr;
}

HRESULT CAssistantDoc::SetUseKeyframes(bool bUseKeyframes)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetUseKeyframes(bUseKeyframes);
   
   return hr;
}

HRESULT CAssistantDoc::SetKeyframes(UINT nKeyframes)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetKeyframes(nKeyframes);
   
   return hr;
}

HRESULT CAssistantDoc::SetUseDatarate(bool bUseDatarate)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetUseDatarate(bUseDatarate);
   
   return hr;
}

HRESULT CAssistantDoc::SetDatarate(UINT nDatarate)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetDatarate(nDatarate);
   
   return hr;
}

HRESULT CAssistantDoc::SetCodecQuality(UINT nQuality)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetCodecQuality(nQuality);
   
   return hr;
}

bool CAssistantDoc::QualityPossible()
{
   if (ASSISTANT::Project::active == NULL)
      return false;
   
   return ASSISTANT::Project::active->QualityPossible();
}

bool CAssistantDoc::KeyframesPossible()
{
   if (ASSISTANT::Project::active == NULL)
      return false;
   
   return ASSISTANT::Project::active->KeyframesPossible();
}

bool CAssistantDoc::DataratePossible()
{
   if (ASSISTANT::Project::active == NULL)
      return false;
   
   return ASSISTANT::Project::active->DataratePossible();
}

bool CAssistantDoc::CodecHasAbout()
{
   if (ASSISTANT::Project::active == NULL)
      return false;
   
   return ASSISTANT::Project::active->CodecHasAbout();
}

bool CAssistantDoc::CodecCanBeConfigured()
{
   if (ASSISTANT::Project::active == NULL)
      return false;
   
   return ASSISTANT::Project::active->CodecCanBeConfigured();
}

// metadata functions
HRESULT CAssistantDoc::GetMetadata(CString &csTitle, CString &csAuthor, 
                       CString &csProducer, CString &csKeywords)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->GetMetadata(csTitle, csAuthor, csProducer, csKeywords);

   return hr;
}

HRESULT CAssistantDoc::SetMetadata(CString &csTitle, CString &csAuthor, 
                       CString &csProducer, CString &csKeywords)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetMetadata(csTitle, csAuthor, csProducer, csKeywords);

   return hr;
}

HRESULT CAssistantDoc::GetMouseEffects(bool &bMouseVisual, bool &bMouseAcoustic)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->GetMouseEffects(bMouseVisual, bMouseAcoustic);

   return hr;
}

HRESULT CAssistantDoc::SetMouseEffects(bool bMouseVisual, bool bMouseAcoustic)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetMouseEffects(bMouseVisual, bMouseAcoustic);
   
   return hr;
}

HRESULT CAssistantDoc::GetFramerateSettings(bool &bStaticFramerate, 
                                            float &fStaticFramerate, float &fAdaptiveFramerate,
                                            UINT &nWidth, UINT &nHeight)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->GetFramerateSettings(bStaticFramerate, 
                                                                 fStaticFramerate, fAdaptiveFramerate,
                                                                 nWidth, nHeight);
   
   return hr;
}

HRESULT CAssistantDoc::SetFramerateSettings(bool bStaticFramerate, 
                                            float fStaticFramerate, float fAdaptiveFramerate,
                                            UINT nWidth, UINT nHeight)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetFramerateSettings(bStaticFramerate, 
                                                                 fStaticFramerate, fAdaptiveFramerate,
                                                                 nWidth, nHeight);
   
   return hr;
}

HRESULT CAssistantDoc::GetRegionSettings(bool &bRegionFixed, UINT &nFixedX, UINT &nFixedY, 
                                         UINT &nFixedWidth, UINT &nFixedHeight)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->GetRegionSettings(bRegionFixed, nFixedX, nFixedY, 
                                                              nFixedWidth, nFixedHeight);
   
   return hr;
}

HRESULT CAssistantDoc::SetRegionSettings(bool bRegionFixed, UINT nFixedX, UINT nFixedY, 
                                         UINT nFixedWidth, UINT nFixedHeight)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetRegionSettings(bRegionFixed, nFixedX, nFixedY, 
                                                              nFixedWidth, nFixedHeight);
   
   return hr;
}



HRESULT CAssistantDoc::GetSgHandling(bool &bClearAnnotation, bool &bReportDroppedFrames, bool &bBorderPanning,
                                     bool &bDisableAcceleration, bool &bQuickCapture, bool &bSuspendVideo)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->GetSgHandling(bClearAnnotation, bReportDroppedFrames, bBorderPanning,
                                                          bDisableAcceleration, bQuickCapture, bSuspendVideo);
   
   return hr;
}

HRESULT CAssistantDoc::SetSgHandling(bool bClearAnnotation, bool bReportDroppedFrames, bool bBorderPanning,
                                     bool bDisableAcceleration, bool bQuickCapture, bool bSuspendVideo)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetSgHandling(bClearAnnotation, bReportDroppedFrames, bBorderPanning,
                                                          bDisableAcceleration, bQuickCapture, bSuspendVideo);
   
   return hr;
}

HRESULT CAssistantDoc::GetSgHotkeys(AVGRABBER::HotKey &hkStartStop, AVGRABBER::HotKey &hkReduce, AVGRABBER::HotKey &hkMinimize)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->GetSgHotkeys(hkStartStop, hkReduce, hkMinimize);
   
   return hr;
}

HRESULT CAssistantDoc::SetSgHotkeys(AVGRABBER::HotKey &hkStartStop, AVGRABBER::HotKey &hkReduce, AVGRABBER::HotKey &hkMinimize)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetSgHotkeys(hkStartStop, hkReduce, hkMinimize);
   
   return hr;
}

HRESULT CAssistantDoc::GetSgStartStopHotKey(AVGRABBER::HotKey &hkStartStop)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->GetSgStartStopHotKey(hkStartStop);
   
   return hr;
}

HRESULT CAssistantDoc::SetSgStartStopHotKey(AVGRABBER::HotKey &hkStartStop)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetSgStartStopHotKey(hkStartStop);
   
   return hr;
}

HRESULT CAssistantDoc::TestHotKey(AVGRABBER::HotKey &hkStartStop)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->TestHotKey(hkStartStop);
   
   return hr;
}

HRESULT CAssistantDoc::ActivateStartStopHotKey(bool bActivate)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->ActivateStartStopHotKey(bActivate);
   
   return hr;
}

void CAssistantDoc::GetLastSelectionRectangle(CRect &rcLastSelection, bool &bRecordFullScreen) {
    CRegistrySettings::GetLastSelectionRectangle(rcLastSelection);
    bRecordFullScreen = CRegistrySettings::GetRecordFullScreen();
}

void CAssistantDoc::SetLastSelectionRectangle(CRect &rcLastSelection, bool bRecordFullScreen) {
    CRegistrySettings::SetLastSelectionRectangle(rcLastSelection);
    CRegistrySettings::SetRecordFullScreen(bRecordFullScreen);
}

HRESULT CAssistantDoc::GetCodecSettings(CStringArray &arCodecList, UINT &nCodecIndex, bool &bHasAbout, bool &bHasConfigure,
                                        bool &bSupportsQuality, UINT &nCodecQuality, 
                                        bool &bSupportsKeyframes, bool &bUseKeyframes, UINT &nKeyframeDistance, 
                                        bool &bSupportsDatarate, bool &bUseDatarte, UINT &nDatarate)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->GetCodecSettings(arCodecList, nCodecIndex, bHasAbout, bHasConfigure,
                                                             bSupportsQuality, nCodecQuality,
                                                             bSupportsKeyframes, bUseKeyframes, nKeyframeDistance, 
                                                             bSupportsDatarate, bUseDatarte, nDatarate);
   
   return hr;
}

HRESULT CAssistantDoc::SetCodecSettings(CString &csCodec, UINT nCodecQuality, 
                                        bool bUseKeyframes, UINT nKeyframeDistance, bool &bUseDatarte, UINT nDatarate)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->SetCodecSettings(csCodec, nCodecQuality, 
                                                             bUseKeyframes, nKeyframeDistance, bUseDatarte, nDatarate);
   
   return hr;
}

HRESULT CAssistantDoc::ShowSgCodecAbout()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->ShowSgCodecAbout();
   
   return hr;
}

HRESULT CAssistantDoc::ShowSgCodecConfigure()
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;
   
   HRESULT hr = ASSISTANT::Project::active->ShowSgCodecConfigure();
   
   return hr;
}


// structure functions
HRESULT CAssistantDoc::RebuildStructureTree(CDocumentStructureView *pStructureTree)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
   if (pMainWnd != NULL)
       pMainWnd->UpdateStatusPages();

    CDocumentStructureView *pCurrentStructureTree = pStructureTree;
   if (pCurrentStructureTree == NULL)
       pCurrentStructureTree = pMainWnd->GetStructureTree();

   if (pCurrentStructureTree == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->ReBuildStructureTree(pCurrentStructureTree);

   return S_OK;
}

HRESULT CAssistantDoc::RebuildRecordingTree(CRecordingsView *pRecordingList)
{
    if (ASSISTANT::Project::active == NULL)
        return E_FAIL;

    CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
    if (pMainWnd != NULL)
        pMainWnd->UpdateStatusPages();

    CRecordingsView *pCurrentRecordingTree = pRecordingList;
    if (pCurrentRecordingTree == NULL)
        pCurrentRecordingTree = pMainWnd->GetRecordingView();

    if (pCurrentRecordingTree == NULL)
        return E_FAIL;

    ASSISTANT::Project::active->ReBuildRecordingTree(pCurrentRecordingTree);

    return S_OK;
}

HRESULT CAssistantDoc::RemovePresentation(ASSISTANT::PresentationInfo *pPresentation)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->RemovePresentation(pPresentation);
   RebuildRecordingTree(NULL);

   return S_OK;
}

HRESULT CAssistantDoc::DeletePresentation(ASSISTANT::PresentationInfo *pPresentation)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Project::active->DeletePresentation(pPresentation);
   RebuildRecordingTree(NULL);

   return S_OK;
}

HRESULT CAssistantDoc::TranslateScreenToObjectCoordinates(CRect &rcWhiteboard, CPoint &ptScreen, Gdiplus::PointF &ptObject)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   if (pPage == NULL)
      return E_FAIL;


   float fZoomFactor = pPage->CalculateZoomFactor(rcWhiteboard);
   UINT nOffsetX = 0;
   UINT nOffsetY = 0;
   pPage->CalculateOffset(rcWhiteboard, fZoomFactor, nOffsetX, nOffsetY);

   double dX = ((double)ptScreen.x - nOffsetX) / fZoomFactor;
   double dY = ((double)ptScreen.y - nOffsetY) / fZoomFactor;

   if (dX < 0)
      ptObject.X = 0;
   else if (dX > pPage->GetWidth())
      ptObject.X = pPage->GetWidth();
   else
      ptObject.X = dX;
   
   if (dY < 0)
      ptObject.Y = 0;
   else if (dY > pPage->GetHeight())
      ptObject.Y = pPage->GetHeight();
   else
      ptObject.Y = dY;

   return S_OK;
}

HRESULT CAssistantDoc::TranslateScreenToObjectCoordinates(CRect &rcWhiteboard,CRect &rcScreen, CRect &rcObject)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   if (pPage == NULL)
      return E_FAIL;


   float fZoomFactor = pPage->CalculateZoomFactor(rcWhiteboard);
   UINT nOffsetX = 0;
   UINT nOffsetY = 0;
   pPage->CalculateOffset(rcWhiteboard, fZoomFactor, nOffsetX, nOffsetY);

   rcObject.left = (int)((double)rcScreen.left - nOffsetX) / fZoomFactor;
   rcObject.right = (int)((double)rcScreen.right - nOffsetX) / fZoomFactor;
   rcObject.top = (int)((double)rcScreen.top - nOffsetY) / fZoomFactor;
   rcObject.bottom = (int)((double)rcScreen.bottom - nOffsetY) / fZoomFactor;

   if (rcObject.left < 0)
      rcObject.left = 0;
   else if (rcObject.left > pPage->GetWidth())
      rcObject.left = pPage->GetWidth();
   
   if (rcObject.right < 0)
      rcObject.right = 0;
   else if (rcObject.right > pPage->GetWidth())
      rcObject.right = pPage->GetWidth();

   if (rcObject.right < rcObject.left)
      rcObject.right = rcObject.left;
   
   if (rcObject.top < 0)
      rcObject.top = 0;
   else if (rcObject.top > pPage->GetHeight())
      rcObject.top = pPage->GetHeight();

   if (rcObject.bottom < 0)
      rcObject.bottom = 0;
   else if (rcObject.bottom > pPage->GetHeight())
      rcObject.bottom = pPage->GetHeight();

   if (rcObject.bottom < rcObject.top)
      rcObject.bottom = rcObject.top;
   
   rcObject.NormalizeRect();

   return S_OK;
}

ASSISTANT::DrawObject *CAssistantDoc::GetObject(Gdiplus::PointF &ptMouse)
{
   if (ASSISTANT::Project::active == NULL)
      return NULL;

   ASSISTANT::DrawObject *pObject = NULL;
   
   ASSISTANT::Page *pActivePage = ASSISTANT::Project::active->GetActivePage();
   if (pActivePage)
   {
      pObject = pActivePage->GetObject(ptMouse);
   }
   
   return pObject;
}

HRESULT CAssistantDoc::GetSelectedObjects(CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> &arSelectedObjects)
{
   if (ASSISTANT::Project::active == NULL)
      return E_FAIL;

   ASSISTANT::Page *pActivePage = ASSISTANT::Project::active->GetActivePage();

   if (pActivePage)
   {
      pActivePage->GetSelectedObjects(arSelectedObjects);
   }
   
   return S_OK;
}

bool CAssistantDoc::PointIsInObject(Gdiplus::PointF &ptMouse)
{
   if (ASSISTANT::Project::active == NULL)
      return false;

   ASSISTANT::Page *pActivePage = ASSISTANT::Project::active->GetActivePage();

   if (pActivePage->PointIsInObject(ptMouse))
      return true;
  
   return false;
}
void CAssistantDoc::SetShowImportWarning(bool bShowImportWarning)
{
   m_bShowImportWarning = bShowImportWarning;
   return;
}

void CAssistantDoc::SetShowMissingFontsWarning(bool bShowMissingFontsWarning)
{
   m_bShowMissingFontsWarning = bShowMissingFontsWarning;
   return;
}

void CAssistantDoc::SetShowAudioWarning(bool bShowAudioWarning)
{
   m_bShowAudioWarning = bShowAudioWarning;
   return;
}

void CAssistantDoc::SetShowUseSmartBoard(bool bShowUseSmartBoard)
{
   m_bShowUseSmartBoard = bShowUseSmartBoard;
   return;
}

void CAssistantDoc::SetFreezeImages(bool bFreezeImage)
{
   m_bFreezeImages = bFreezeImage;

   if (ASSISTANT::Project::active == NULL)
      return;

   ASSISTANT::Project::active->FreezeImages(bFreezeImage);
  
   return;
}

void CAssistantDoc::SetFreezeMaster(bool bFreezeMaster)
{
   m_bFreezeMaster = bFreezeMaster;

   if (ASSISTANT::Project::active == NULL)
      return;

   ASSISTANT::Project::active->FreezeMaster(bFreezeMaster);
  
   return;
}

void CAssistantDoc::SetFreezeDocument(bool bFreezeDocument)
{
   m_bFreezeDocument = bFreezeDocument;

   if (ASSISTANT::Project::active == NULL)
      return;

   ASSISTANT::Project::active->FreezeDocument(bFreezeDocument);
  
   return;
}

void CAssistantDoc::SetShowMouseCursor(bool bShowMouseCursor)
{
   m_bShowMouseCursor = bShowMouseCursor;
   return;
}

void CAssistantDoc::SetShowStatusBar(bool bShowStatusBar)
{
   m_bShowStatusBar = bShowStatusBar;
   return;
}

void CAssistantDoc::SetShowStartupPage(bool bShowStartupPage)
{
   m_bShowStartupPage = bShowStartupPage;
   return;
}

void CAssistantDoc::SetSwitchAutomaticallyToEdit(bool bSwitchAutomatically)
{
   m_bSwitchAutomatically = bSwitchAutomatically;
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("SwitchAutomaticallyToEdit"), m_bSwitchAutomatically);
   return;
}

void CAssistantDoc::SetShowAVMonitor(bool bShowAvMonitor)
{
   m_bShowAVMonitor = bShowAvMonitor;
}
// Audio monitor is visible just on screen recording
void CAssistantDoc::SetShowAudioMonitor(bool bShowAudioMonitor)
{
   m_bShowAudioMonitor = bShowAudioMonitor;
}

bool CAssistantDoc::IsRecording()
{
   if (ASSISTANT::Project::active == NULL)
      return false;

   return ASSISTANT::Project::active->IsRecording();
}

bool CAssistantDoc::IsRecordingPaused()
{
   if (ASSISTANT::Project::active == NULL)
      return false;

   return ASSISTANT::Project::active->IsRecordingPaused();
}

void CAssistantDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
   CString fileSuffix = lpszPathName;
   StringManipulation::GetFileSuffix(fileSuffix);
   fileSuffix.MakeUpper();

   if (fileSuffix != _T("LSD"))
	   CDocument::SetPathName(lpszPathName, bAddToMRU);
   else
   {
      CString newPathName = lpszPathName;
      int index = newPathName.ReverseFind(_T('.'));
      int count = newPathName.GetLength() - index;
      newPathName.Delete(index, count);
      newPathName += _T(".lap");
	   CDocument::SetPathName(newPathName, FALSE);
   }
}

void CAssistantDoc::SortRecordingsList(int nColumn, BOOL bAscending) 
{
   if (ASSISTANT::Project::active == NULL)
      return;

   ASSISTANT::Project::active->SortRecordingsList(nColumn, bAscending);
}

void CAssistantDoc::SetFullScreenOn(bool bState)
{
   m_bIsFullScreenOn = bState; 
}

bool CAssistantDoc::IsFullScreenOn()
{
	if (ASSISTANT::Project::active != NULL)
	   ASSISTANT::Project::active->SetFullScreenOn(m_bIsFullScreenOn);
   return m_bIsFullScreenOn;
}


HRESULT CAssistantDoc::GetGridSettings(UINT &snapChecked, UINT &spacingValue, UINT &displayGrid)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->GetGridSettings(snapChecked, spacingValue, displayGrid);
   
   return hr;
}

HRESULT CAssistantDoc::SetGridSettings(UINT snapChecked, UINT spacingValue, UINT displayGrid)
{
   HRESULT hr = S_OK;

   if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->SetGridSettings(snapChecked, spacingValue, displayGrid);

   return hr;
   
}

bool CAssistantDoc::IsGridVisible()
{
	HRESULT hr = S_OK;
	if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->GetGridSettings(m_snapChecked, m_spacingValue, m_displayGrid);
	if(m_displayGrid)
		return true;
	else 
		return false;
}

bool CAssistantDoc::IsSnapEnabled()
{
	HRESULT hr = S_OK;
	if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->GetGridSettings(m_snapChecked, m_spacingValue, m_displayGrid);
	if(m_snapChecked)
		return true;
	else 
		return false;
}

int CAssistantDoc::GetGridWidth()
{
	HRESULT hr = S_OK;
	if (ASSISTANT::Project::active != NULL)
      hr = ASSISTANT::Project::active->GetGridSettings(m_snapChecked, m_spacingValue, m_displayGrid);
	return m_spacingValue;
}

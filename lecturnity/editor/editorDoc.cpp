// editorDoc.cpp : Implementierung der Klasse CEditorDoc
//

/* REVIEW UK
 * Lengthy check for resetting report control content should be moved to CInteractionStructureView
 *   -> CInteractionStructureView::ResetReportContent()
 */
#include "stdafx.h"
#include "editorDoc.h"
#include "MainFrm.h"
#include "WhiteboardView.h"
#include "InteractionStream.h" // for integrity check
#include "lutils.h" // eg LFile
// PZI:
#include "ScreengrabbingExtendedExternal.h" // lsutl32

#include "..\Assistant2_0\DocumentManager.h"
#include "..\Studio\Studio.h"
#include "QuestionnaireEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CUpdateHint, CObject)

CUpdateHint::CUpdateHint()
{
   m_idUpdateHint = HINT_CHANGE_NOTHING;
   m_nPositionMs = 0;
}

CUpdateHint::~CUpdateHint()
{
}


/////////////////////////////////////////////////////////////////////////////
// CEditorDoc

IMPLEMENT_DYNCREATE(CEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CEditorDoc)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
    ON_COMMAND(ID_FILE_SAVE_AS, &CEditorDoc::OnFileSaveAs)
END_MESSAGE_MAP()


CEditorDoc *CEditorDoc::s_pEditorDoc = NULL;
CEditorDoc* CEditorDoc::GetCurrentInstance() {
    return s_pEditorDoc;
}

// static
int CEditorDoc::ShowInfoMessage(CString &csMessage, HWND hWndParent, UINT nButtonType) {
   if (hWndParent == NULL)
      hWndParent = AfxGetMainWnd()->GetSafeHwnd();

   CString csCap;
   csCap.LoadString(IDS_INFORMATION);
   return ::MessageBox(hWndParent, csMessage, csCap, nButtonType | MB_ICONINFORMATION);
}

// static
int CEditorDoc::ShowWarningMessage(CString &csMessage, HWND hWndParent, UINT nButtonType) {
   if (hWndParent == NULL)
      hWndParent = AfxGetMainWnd()->GetSafeHwnd();

   CString csCap;
   csCap.LoadString(IDS_WARNING_E);
   return ::MessageBox(hWndParent, csMessage, csCap, nButtonType | MB_ICONWARNING | MB_TOPMOST);

   // TODO using a proper hWndParent does not need MB_TOPMOST
   // (Was probably a quick hack for all MessageBoxes...)
   // Also below.
}

// static
int CEditorDoc::ShowErrorMessage(CString &csMessage, HWND hWndParent, UINT nButtonType) {
   if (hWndParent == NULL)
      hWndParent = AfxGetMainWnd()->GetSafeHwnd();

   CString csCap;
   csCap.LoadString(IDS_ERROR_E);
   return ::MessageBox(hWndParent, csMessage, csCap, nButtonType | MB_ICONERROR | MB_TOPMOST);
}

// static 
CString CEditorDoc::GetCorrespondingLep(CString& csLrdPath, HWND hwndParent) {
    CString csLepPath = csLrdPath;
    StringManipulation::GetPathFilePrefix(csLepPath);
    csLepPath += _T(".lep");

    LFile fileLep(csLepPath);
    if (fileLep.Exists(true)) { // exists with a size > 0
        LFile fileLrd(csLrdPath);

        if (fileLrd.IsEqualOrNewerThan(&fileLep)) {
            // warn and selection (?) dialog

            //CString csMessage;
            //csMessage.LoadString(IDS_LRD_NEWER_LEP);
            //ShowWarningMessage(csMessage, hwndParent);

            // TODO: this warning message (text) is not correct in all cases 
            //       (ie Export from Assistant mode)
        }

        return csLepPath;
    }
    
    return _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CEditorDoc Konstruktion/Destruktion

CEditorDoc::CEditorDoc()
{
    s_pEditorDoc = this;
   m_docLengthMs = -1; // no document
   m_displayStartMs = 0;
   m_displayEndMs   = m_docLengthMs;
   m_curPosMs = 0;
   m_markStartMs = -1; // no mark
   m_markEndMs = -1; // no mark

   m_streamUnit = SU_MINUTES;

   // Set the m_pDoc member of our project
   // to ourselves
   project.m_pDoc = this;

   m_bPreviewActive = false;
   m_bPausedSpecial = false;
   m_bJumpActive = false;
   m_bUserIsScrolling = false;

   m_pMainFrame = NULL;

   // Initialize GDI+.
   Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

CEditorDoc::~CEditorDoc()
{
   project.Close();
   
   // Stop GDI+
   Gdiplus::GdiplusShutdown(gdiplusToken);
}

BOOL CEditorDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
		return FALSE;

	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)
   m_docLengthMs = -1; // no document
   m_displayStartMs = 0;
   m_displayEndMs   = m_docLengthMs;
   m_curPosMs = 0;
   m_markStartMs = -1; // no mark
   m_markEndMs = -1; // no mark

   m_bPreviewActive = false;
   m_bPausedSpecial = false;
   m_bIsPreviewRebuildNeeded = TRUE;
   
   // BUGFIX #2427
   CMainFrameE *pFrame = GetMainFrame();
 
   // Note: (CMainFrameE *) AfxGetMainWnd();
   // This will return CSplashScreen on the very first start on some computers...

   if (pFrame != NULL)
   {
      if (pFrame->m_pStreamView != NULL)
         pFrame->m_pStreamView->ResetUserVariables();  
      
      if (pFrame->m_pWhiteboardView != NULL)
         pFrame->m_pWhiteboardView->ResetVariables();
      if(pFrame->GetStructureView() != NULL)
         pFrame->GetStructureView()->RemoveAll();
   }
   // else on some computers this is called during startup
   // and the main window is not yet created/set at this position (see above)


   if (pFrame != NULL)
      pFrame->AdjustToProjectMode();

   // to have all view properly repaint their empty state
   UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CEditorDoc Serialisierung

void CEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
		CString filePath = ar.GetFile()->GetFilePath();
		// MessageBox(NULL, filePath, "Lade...", MB_OK);

		//throw new CArchiveException(); //("Krumpf");

		//if (!Open(filePath))
		// throw new CArchiveException(); //("Krumpf");


		try
		{
			Open(filePath);
		}

		catch (CArchiveException* pEx)
		{
			pEx->Delete ();
		}


	}
}

BOOL CEditorDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	CString csNewName;
    
    if (lpszPathName != NULL)
        csNewName = lpszPathName;
    else
        csNewName = m_strPathName;

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
	if (bReplace && !csNewName.IsEmpty())
		SetPathName(csNewName);

    return TRUE;        // success
}

void CEditorDoc::UpdateDocument()
{
    m_docLengthMs    = project.GetTotalLength();
    m_markStartMs    = -1; // no mark
    m_markEndMs      = -1; // no mark
    if (m_curPosMs < 0)
        m_curPosMs = 0;
    if (m_curPosMs > m_docLengthMs)
        m_curPosMs = m_docLengthMs;
}

bool CEditorDoc::InsertLrd(LPCTSTR tszFilename)
{
    bool bHasInteractions = false;
    if (project.GetInteractionStream() != NULL)
        bHasInteractions = true;

    if (project.IsEmpty())
        project.StartOpen(tszFilename);
    else
        project.StartInsertLrd(tszFilename, m_curPosMs);

    // temporary solution:
    project.UpdateStructureAndStreams();
    UpdateDocument();
    m_displayStartMs = 0;
    m_displayEndMs   = m_docLengthMs;

    // Inserting something is definitely a change
    SetModifiedFlag();

    CMainFrameE *pMainWnd = GetMainFrame();
    if (pMainWnd != NULL)
        pMainWnd->AdjustToProjectMode();
    RebuildPreview();

    UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);


    return true;
}

bool CEditorDoc::Open(LPCTSTR tszFilename)
{
    // TODO When opening a document see if it must enter directly to SS. Initialize m_bScreenSimulation acordingly to this.
    CString csFileSuffix = tszFilename;
    StringManipulation::GetFileSuffix(csFileSuffix);
    csFileSuffix.MakeUpper();

    if (csFileSuffix == _T("LRD")) {
        // Check if a .lep exists. If so: load that instead.
        CString csLrdPath = tszFilename;
        CString csLepPath = GetCorrespondingLep(csLrdPath);

        bool bUseLrd = csLepPath.IsEmpty();

        if (!csLepPath.IsEmpty()) {
            // Some status message
            CString csMessageIn;
            csMessageIn.LoadString(IDS_USING_LEP);
            CString csMessage;
            csMessage.Format(csMessageIn, csLepPath);
            CMainFrameE::GetCurrentInstance()->ShowTimedStatusText(csMessage, 5000);
        }


        if (!bUseLrd) {
            bool bSuccess = Open(csLepPath);

            return bSuccess;
        } else {
            Import(tszFilename);

            return true; // ???
        }
    } else {

        CMainFrameE *pFrame = GetMainFrame();
        if (pFrame != NULL && pFrame->GetWhiteboardView() != NULL)
            pFrame->GetWhiteboardView()->GetSelectedObjects().RemoveAll();

        if (pFrame != NULL && pFrame->GetInteractionStructureView() != NULL )
        {
            if ( pFrame->GetInteractionStructureView()->GetReportCtrl().GetRecords() != NULL 
                && pFrame->GetInteractionStructureView()->GetReportCtrl().GetRecords()->GetCount() > 0 )
                pFrame->GetInteractionStructureView()->GetReportCtrl().ResetContent();
        }
        if (pFrame != NULL && pFrame->GetStructureView() != NULL )
        {
           pFrame->GetStructureView()->RemoveAll();
        }

        bool success = project.StartOpen(tszFilename);

        project.UpdateStructureAndStreams();
        UpdateDocument();
        m_curPosMs       = 0;
        m_displayStartMs = 0;
        m_displayEndMs   = m_docLengthMs;

        SetModifiedFlag();
        // We do not rebuild the preview yet, as some
        // variables may not have been set yet. See
        // MainFrm.cpp
        SetPreviewRebuildNeeded();

        UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);

        if (pFrame != NULL && pFrame->m_pStreamView != NULL)
            pFrame->m_pStreamView->ResetUserVariables();

        if (pFrame != NULL)
            pFrame->AdjustToProjectMode();

        return success;
    }
}

void CEditorDoc::Import(LPCTSTR tszFilename)
{
   // TODO: this is double code to Open()

   // make sure the current directory is set to the source directory 
   // so that files of actions can be found
   CString csFilePath = tszFilename;
   StringManipulation::GetPath(csFilePath);
   if (csFilePath.GetLength() > 0)
   ::SetCurrentDirectory(csFilePath);

   bool bHasInteractions = false;
   if (project.GetInteractionStream() != NULL)
      bHasInteractions = true;

   project.Import(tszFilename);


   SetPathName(tszFilename); // automatically "converts" to ".lep"
   // TODO: What about MRU in this case?

   project.UpdateStructureAndStreams();
   UpdateDocument();
   m_curPosMs       = 0;
   m_displayStartMs = 0;
   m_displayEndMs   = m_docLengthMs;

   SetModifiedFlag();
   // We do not rebuild the preview yet, as some
   // variables may not have been set yet. See
   // MainFrm.cpp
   SetPreviewRebuildNeeded();
   
   UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);

   CMainFrameE *pMainWnd = GetMainFrame();
   if (pMainWnd != NULL)
      pMainWnd->AdjustToProjectMode();

   CArray<CString, CString>acsUnsupportedFonts;
   project.GetUnsupportedFonts(acsUnsupportedFonts);
   if(acsUnsupportedFonts.GetCount() > 0)
   {
       CString msg;
       msg.LoadString(IDC_WARNING_MISSING_FONTS);
       msg += _T("\n\n");
       for(int i = 0; i < acsUnsupportedFonts.GetCount(); i++)
       {
           msg += acsUnsupportedFonts.GetAt(i) + _T("\n");
       }
     
       AfxMessageBox(msg, MB_TOPMOST);
   }

   //return true; // TODO: ??? What happens in case of error??
}

/////////////////////////////////////////////////////////////////////////////
// CEditorDoc Diagnose

#ifdef _DEBUG
void CEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CEditorDoc::SetModifiedFlag(BOOL bFlag)
{
   CDocument::SetModifiedFlag(bFlag);
//   if (TRUE == bFlag)
//      SetPreviewRebuildNeeded(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CEditorDoc Befehle

void CEditorDoc::OnCloseDocument() {

    CDocument::OnCloseDocument(); // must be called AFTER anything else (deletes objects)

}

BOOL CEditorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	project.Save(lpszPathName);

   SetModifiedFlag(FALSE);
   return (TRUE);
}

void CEditorDoc::OnFileSaveAs()
{
    CDocTemplate* pTemplate = GetDocTemplate();
    ASSERT(pTemplate != NULL);

    CString csNewName;
    CString csFilter;
    csFilter.LoadString(IDS_FILEFILTER);
    CString csInitialDir = GetMainFrame()->GetInitialLepDir();
    FileDialog::GetSaveFilename(AfxGetApp(), csNewName, AfxGetMainWnd()->GetSafeHwnd(),
        AfxGetResourceHandle(), csFilter, _T(".lrd"), _T("LRD"), csInitialDir);

    CWaitCursor wait;

    if (!csNewName.IsEmpty()) {
        bool bSuccess = project.SaveAs(csNewName);
        SetPathName(csNewName);
    }
}

void CEditorDoc::DeleteContents() 
{
    project.m_avEdit.StopPreview();
	project.Close();
   
    UpdateAllViews(NULL, UPDATE_FITVIDEO); // TODO what about this?
	CDocument::DeleteContents();
}

CMainFrameE *CEditorDoc::GetMainFrame() {
   return CMainFrameE::GetCurrentInstance();
}

CView *CEditorDoc::GetMainView() {
    CMainFrameE *pMainFrame = GetMainFrame();
    if (pMainFrame == NULL)
        return NULL;

    POSITION pos = GetFirstViewPosition();
    while (pos != NULL) {
        CView *pView = GetNextView(pos);

        if (pView->IsKindOf(RUNTIME_CLASS(CWhiteboardView)) && pView->IsWindowVisible())
            return pView;
        else if (pView->IsKindOf(RUNTIME_CLASS(CVideoView)) && pView->IsWindowVisible() && 
            pView->GetParent() == pMainFrame)
            return pView;
    }

    return NULL;
}

CWhiteboardView *CEditorDoc::GetWhiteboardView()
{
   POSITION pos = GetFirstViewPosition();
   while (pos != NULL)
   {
      CView *pView = GetNextView(pos);

      if (pView->IsKindOf(RUNTIME_CLASS(CWhiteboardView)))
         return (CWhiteboardView *)pView;
   }

   return NULL;
}

CVideoView *CEditorDoc::GetVideoView() {
    CMainFrameE *pMainFrame = GetMainFrame();
    if (pMainFrame == NULL)
        return NULL;

    POSITION pos = GetFirstViewPosition();
    CView *pVideoViewInMainWnd = NULL;
    CView *pVideoViewInPane = NULL;
    while (pos != NULL) {
        CView *pView = GetNextView(pos);
        if (pView->IsKindOf(RUNTIME_CLASS(CVideoView))) {
            if (pView->GetParent() == pMainFrame)
                pVideoViewInMainWnd = pView;
            else
                pVideoViewInPane = pView;
        }
    }

    bool bIsScreenGrabbing = pMainFrame->IsScreengrabbingDocument();
    if (bIsScreenGrabbing && pVideoViewInMainWnd != NULL)
        return (CVideoView *)pVideoViewInMainWnd;
    else
        return (CVideoView *)pVideoViewInPane;
}

void CEditorDoc::StartPreview(HWND hWndContainer, bool bDoSelection) {
    if (project.m_previewList.GetSize() == 0) {
        return;
    }

    AfxGetMainWnd()->DragAcceptFiles(FALSE);

    if (m_bPausedSpecial) {
        CWhiteboardView *pView = GetWhiteboardView();
        if (pView != NULL) {
            pView->RemoveAllFeedback();
        }

        m_bPausedSpecial = false;
    }

    CQuestionnaireEx *pFirstQuestionnaire = project.GetFirstQuestionnaireEx();
    if (pFirstQuestionnaire && pFirstQuestionnaire->IsRandomTest()
        && !project.IsRandomTestStarted()
        && pFirstQuestionnaire->GetQuestionTotal() > 0) {
        if (!m_bJumpActive) {
            if (m_curPosMs == 0) {
                pFirstQuestionnaire->RandomizeQuestionnaire();
            }
        }
    }

    if (!IsPreviewRebuildNeeded()) {
        HRESULT hr = S_OK;

        if (!bDoSelection) {
            hr = project.m_avEdit.StartPreview();
        } else {
            project.m_avEdit.StartPreview(m_markStartMs, m_markEndMs);
        }

        if (SUCCEEDED(hr)) {
            m_bPreviewActive = true;
        } else {
            project.HandleError(IDS_MSG_PREVIEWSTART_FAILED, hr);
        }
        CMainFrameE::GetCurrentInstance()->ChangeToPause();
        return;
    }

   if (BuildPreview(hWndContainer)) {
       HRESULT hr = S_OK;

       if (!bDoSelection) {
           hr = project.m_avEdit.JumpPreview(m_curPosMs, true);
       }

       if (SUCCEEDED(hr)) {
           if (!bDoSelection) {
               hr = project.m_avEdit.StartPreview();
           } else {
               hr = project.m_avEdit.StartPreview(m_markStartMs, m_markEndMs);
           }
       }

       if (SUCCEEDED(hr)) {
           m_bPreviewActive = true;
       } else {
           project.HandleError(IDS_MSG_PREVIEWSTART_FAILED, hr);
       }
   } else {
       // Note the usage of SetPreviewRebuildNeeded() here!
       // DO NOT USE RebuildPreview here!!
       SetPreviewRebuildNeeded(TRUE);
   }

   CMainFrameE::GetCurrentInstance()->ChangeToPause();

   //((CMainFrameE *)AfxGetMainWnd)->UpdateWhiteboardView();
}

void CEditorDoc::StartPreviewSelection(HWND hWndContainer)
{
   StartPreview(hWndContainer, true);
}

void CEditorDoc::JumpPreview(int millis)
{
   m_bJumpActive = true;

   if (millis > m_docLengthMs - 1)
      millis = m_docLengthMs - 1;
   else if (millis < 0)
      millis = 0;

   m_curPosMs = millis; 

   if (project.m_previewList.GetSize() == 0)
      return;

   bool bExactStopFound = false;
   CMarkStream *pMarkStream = project.GetMarkStream();
   if (pMarkStream != NULL)
   {
      bExactStopFound = pMarkStream->IsExactStopAt(millis);
      if (bExactStopFound)
         PausePreviewSpecial();
   }

   // in order to jump the preview you have to pause it first
   // this works for simple jumps (single clicks)
   // for something like scrolling you have to call PausePreview()
   // and some jumps later StartPreview from the outside (currently CTimelineCtrl)

   bool bWasPreviewing = m_bPreviewActive;
   if (m_bPreviewActive)
      PausePreview();
   
   // "true" means: update all streams (audio AND video)
   // during scrolling we could only update video ("false")
   // but that doesn't save much time
   project.m_avEdit.JumpPreview(millis, true);
   
   AdaptDisplayRange(); // if zoomed

   if (!bExactStopFound && (bWasPreviewing || m_bPausedSpecial))
      StartPreview();
   else
   {
      AsyncTriggerDocUpdate();

      // Note: invoking this directly does not work if it is another thread
      // (like QuestionStream::FeedbackThreadLoop()); results in an (mfc-) assertion.
      //
      //UpdateAllViews(NULL, HINT_CHANGE_POSITION);

      // Note: Sending some custom message does not work either
      // (it doesn't reach CMainFrameE somehow).
      //
      //::AfxGetMainWnd()->PostMessage(WM_UPDATE_EXT, HINT_CHANGE_POSITION);  
   }
   
   m_bJumpActive = false;
}

bool CEditorDoc::InformCurrentReplayTime(int iReplayMs)
{
   bool bChange = false;

   if (!m_bJumpActive && iReplayMs > m_curPosMs)
   {
      if (iReplayMs > m_docLengthMs - 1)
         iReplayMs = m_docLengthMs - 1;
      
      if (iReplayMs != m_curPosMs)
      {
         bChange = true;
         m_curPosMs = iReplayMs;

         AdaptDisplayRange();
      }
   }
   // Note: sometimes replay reports a time a few ms before a previous jump time
   // this is odd and it is ignored here.

   return bChange;
}

BOOL CEditorDoc::SwitchToView(CRuntimeClass* pNewViewClass) {
    // Get the active MDI child window.
    CXTPMDIChildWnd* pMainWnd = CMainFrameE::GetCurrentInstance();

    CView* pOldActiveView = GetMainView();

   // If we're already displaying this kind of view, no need to go further.

   if (pOldActiveView->IsKindOf(pNewViewClass))
      return TRUE;
	
   // Set the child window ID of the active view to AFX_IDW_PANE_FIRST.

   // This is necessary so that CFrameWnd::RecalcLayout will allocate

   // this "first pane" to that portion of the frame window's client

   // area not allocated to control bars.  Set the child ID of

   // the previously active view to some other ID.

   ::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, 0);

   // create the new view

   CCreateContext context;
   context.m_pNewViewClass = pNewViewClass;
   context.m_pCurrentDoc = this;
   CView* pNewView = STATIC_DOWNCAST(CView, pMainWnd->CreateView(&context));
   if (pNewView != NULL)
   {
      // the new view is there, but invisible and not active...

      pNewView->ShowWindow(SW_SHOW);
      pNewView->OnInitialUpdate();
      pMainWnd->SetActiveView(pNewView);
      pMainWnd->RecalcLayout();

      // destroy the old view...
      BOOL bAutoDelete = m_bAutoDelete;
      m_bAutoDelete = FALSE;
      pOldActiveView->DestroyWindow();   
      m_bAutoDelete = bAutoDelete;

      return TRUE;
   }

   return FALSE;
}

UINT CEditorDoc::GetBackendPreviewPos()
{
   return project.m_avEdit.GetPreviewPosition();
}
   

void CEditorDoc::RebuildPreview(bool bProhibitJump)
{   
	CWaitCursor wait;
   // First set the flag that we need to rebuild
   // the preview (in case something goes wrong
   // this variable will still be TRUE so that we
   // know that the preview is no longer valid)
   SetPreviewRebuildNeeded();
   // TIME: needs 1-2s
  
   HWND hWndContainer = GetMainFrame()->GetVideoHWND();

   BuildPreview(hWndContainer);
   // TIME: needs 1s

   if (m_curPosMs > 0 && !bProhibitJump)
      JumpPreview(m_curPosMs);
}

bool CEditorDoc::BuildPreview(HWND hWndContainer)
{
   CMainFrameE *pFrame = GetMainFrame();

   if (project.m_previewList.GetSize() == 0 ||
       !IsPreviewRebuildNeeded())
      return true;

   // If we really want to rebuild preview,
   // we have to make sure it's been torn
   // down first.
   project.m_avEdit.StopPreview();
   // TIME needs no time as it most likely has been called before (in StopPreview())

   m_bPreviewActive = false;

   // Calculate the correct slide size
   project.CalculateSlideSize();

   //HRESULT hr = project.InitAudioVideo2(true);
   HRESULT hr = project.PrepareRenderGraph(true, NULL);
   // TIME needs more or less no time

   if (SUCCEEDED(hr)) {
      HWND hwndNotify = NULL;
      if (pFrame != NULL)
          hwndNotify = pFrame->GetSafeHwnd();

      hr = project.m_avEdit.InitializePreview(hWndContainer, NULL, hwndNotify);
      // TIME: needs 0.3s
   }

   if (SUCCEEDED(hr))
      hr = project.m_avEdit.BuildPreview();
      // TIME: needs 0.5s

   if (SUCCEEDED(hr))
      SetPreviewRebuildNeeded(FALSE);
      // TIME: needs no time with the flag FALSE

   if (FAILED(hr))
      project.HandleError(IDS_MSG_PREVIEWFAILED, hr);

   if (SUCCEEDED(hr))
      project.VerifyLastPageEnd();
  
   if (SUCCEEDED(hr))
   {
      // TODO what about this??
      UpdateAllViews(NULL, UPDATE_FITVIDEO);
   }

   return SUCCEEDED(hr) ? true : false;
}

void CEditorDoc::SetPreviewRebuildNeeded(BOOL bFlag)
{
   m_bIsPreviewRebuildNeeded = bFlag;
   if (bFlag)
   {
      StopPreview();
   }
}

HRESULT CEditorDoc::GetPreviewVideoSize(SIZE *pSize)
{
   if (project.m_previewList.GetSize() == 0)
      return S_FALSE;

   return project.m_avEdit.GetPreviewVideoSize(pSize);
}

HRESULT CEditorDoc::ResizePreviewVideoWindow(RECT *pRect)
{
   return project.m_avEdit.ResizeVideoWindow(pRect);
}

/*
void CEditorDoc::CalculateCursorPos()
{
   if (m_curPosMs >= m_displayStartMs && m_curPosMs <= m_displayEndMs)
      return;

   int diffMs = m_displayEndMs - m_displayStartMs;
   int offset = (int)((double)diffMs / 2.0);

   m_displayEndMs = m_curPosMs + offset;
   if (m_displayEndMs > m_docLengthMs)
      m_displayEndMs = m_docLengthMs;

   m_displayStartMs = m_displayEndMs - diffMs;
   if (m_displayStartMs < 0)
   {
      m_displayStartMs = 0;
      m_displayEndMs = diffMs;
   }

   UpdateAllViews(NULL);
}*/

void CEditorDoc::StopPreview() {
    if (m_bPreviewActive) {
        project.m_avEdit.StopPreview();
        // TIME: needs 2s

        m_bPreviewActive = false;
        m_bPausedSpecial = false;

        UpdateAllViews(NULL, UPDATE_FITVIDEO); // TODO what about this?

        AfxGetMainWnd()->DragAcceptFiles(TRUE);
        CMainFrameE::GetCurrentInstance()->ChangeToPause();
    }
}

void CEditorDoc::PausePreview() {
    project.m_avEdit.PausePreview();

    CQuestionnaireEx *pFirstQuestionnaire = project.GetFirstQuestionnaireEx();
    if (!m_bJumpActive && pFirstQuestionnaire && pFirstQuestionnaire->IsRandomTest()
        && pFirstQuestionnaire->GetQuestionTotal() > 0) {
        pFirstQuestionnaire->RestoreDocumentFromRandomizePlay();
    }

    m_bPreviewActive = false;
    m_bPausedSpecial = false;

    //CalculateCursorPos();

    // TODO couple DragAcceptFiles() more closely to m_bPreviewActive and
    // the preview state. For example: what happens if
    // OnNewDocument() is called while previewing? Currently it is deactivated
    // during preview but that may change.

    AfxGetMainWnd()->DragAcceptFiles(TRUE);
    CMainFrameE::GetCurrentInstance()->ChangeToNormalPlay();
}

void CEditorDoc::PausePreviewSpecial() {
    if (m_bPreviewActive) {
        project.m_avEdit.PausePreview();
        m_bPreviewActive = false;
        m_bPausedSpecial = true;

        CMainFrameE::GetCurrentInstance()->StartPausePreviewSpecialTimer();

        // Bugfix #4545: make sure display is correct; also after jump + stop
        AsyncTriggerDocUpdate();
    } // else no action needs be done; especially no special pause is needed/correct
}

void CEditorDoc::Copy()
{
   if (m_markStartMs != -1 && m_markEndMs != -1 && (m_markStartMs != m_markEndMs))
   {
      // copy selected part of the streams 
      project.Copy(m_markStartMs, m_markEndMs, project.m_clipBoard, true);
   }
}

void CEditorDoc::Cut()
{
   if (m_markStartMs != -1 && m_markEndMs != -1 && (m_markStartMs != m_markEndMs))
   {
      // reposition the current position so that it matches the old position
      if (m_curPosMs > m_markStartMs && m_curPosMs < m_markEndMs)
         m_curPosMs = m_markStartMs;
      else if (m_curPosMs >= m_markEndMs)
         m_curPosMs = m_curPosMs - (m_markEndMs - m_markStartMs);
      
      // cut selected part of the streams and reset selection area
      project.Cut(m_markStartMs, m_markEndMs, true);
      project.UpdateStructureAndStreams();
      UpdateDocument();

      /* is done in RebuildPreview() -> AdaptDisplayRange()
      if (m_displayEndMs > m_docLengthMs)
         m_displayEndMs = m_docLengthMs;
      if (m_docLengthMs == 0)
      {
         m_displayStartMs = 0;
         m_displayEndMs   = 0;
      }
      */

      // Something has changed, ergo set the modified flag
      SetModifiedFlag();
      RebuildPreview();

      UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
   }
}

void CEditorDoc::Paste()
{
   // paste copied part of the streams at actual position
   int insertLength = 0;
   if (project.Paste(m_curPosMs, project.m_clipBoard, insertLength))
   {
      project.UpdateStructureAndStreams();

      UpdateDocument();
      if (m_displayStartMs == m_displayEndMs)
      {
         m_displayStartMs = 0;
         m_displayEndMs   = m_docLengthMs;
      }
      // set marker position to pasted area
      m_markStartMs = m_curPosMs;
      m_markEndMs   = m_curPosMs + insertLength;
      // Something has changed, ergo set the modified flag
      SetModifiedFlag();
      RebuildPreview();

#ifdef _DEBUG
      // RebuildPreview() probably adapts the last page end.
      if (project.GetInteractionStream(false) != NULL)
         project.GetInteractionStream(false)->CheckIntegrity(this);
#endif

      UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
   }
}

void CEditorDoc::Delete()
{
   if (m_markStartMs != -1 && m_markEndMs != -1 && (m_markStartMs != m_markEndMs))
   {
       // reposition the current position so that it matches the old position
      if (m_curPosMs > m_markStartMs && m_curPosMs < m_markEndMs)
         m_curPosMs = m_markStartMs;
      else if (m_curPosMs >= m_markEndMs)
         m_curPosMs = m_curPosMs - (m_markEndMs - m_markStartMs);

      // cut selected part of the streams and reset selection area
      project.Delete(m_markStartMs, m_markEndMs);

      project.UpdateStructureAndStreams();

      UpdateDocument();

      /* is done in JumpPreview() -> AdaptDisplayRange()
      if (m_displayEndMs > m_docLengthMs)
         m_displayEndMs = m_docLengthMs;
      if (m_docLengthMs == 0)
      {
         m_displayStartMs = 0;
         m_displayEndMs   = 0;
      }
      */

      // Something has changed, ergo set the modified flag
      SetModifiedFlag();
      RebuildPreview();
      
#ifdef _DEBUG
      if (project.GetInteractionStream(false) != NULL)
         project.GetInteractionStream(false)->CheckIntegrity(this);
#endif

      if (m_curPosMs > 0)
         JumpPreview(m_curPosMs);
      else
         AdaptDisplayRange();

      UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
   }
}

void CEditorDoc::Undo()
{
   UndoRedo(true);
}

void CEditorDoc::Redo()
{
   UndoRedo(false);
}

// private
void CEditorDoc::UndoRedo(bool bDoUndo)
{
   UpdateHintId idMaximumChange = HINT_CHANGE_NOTHING;
   CArray<UINT, UINT> aBeginTimes;
   
   bool bAnyChange = false;
   if (bDoUndo)
      bAnyChange = project.Undo(&idMaximumChange, &aBeginTimes);
   else
      bAnyChange = project.Redo(&idMaximumChange, &aBeginTimes);

   if (bAnyChange)
   {
      project.UpdateStructureAndStreams();
      UpdateDocument();

      /* is done in AdaptDisplayRange() below
      if (m_displayStartMs == m_displayEndMs)
      {
         m_displayStartMs = 0;
         m_displayEndMs   = m_docLengthMs;
      }
      if (m_docLengthMs == 0)
      {
         m_displayStartMs = 0;
         m_displayEndMs   = 0;
      }
      if (m_displayEndMs > m_docLengthMs)
         m_displayEndMs = m_docLengthMs;
         */
      
      SetModifiedFlag();
      if (project.WasUndoLegacy() || idMaximumChange == HINT_CHANGE_STRUCTURE)
      {
         RebuildPreview(true);
         
         m_curPosMs = 0;
      }

      AdaptDisplayRange();
      // TODO you don't know what undo did but if it was a structural change
      // you should and m_curPosMs should be set to right value (like for example in Cut())
      

      if (idMaximumChange != HINT_CHANGE_PAGE || aBeginTimes.GetSize() == 0)
         UpdateAllViews(NULL, idMaximumChange);
      else
      {
         for (int i=0; i<aBeginTimes.GetSize(); ++i)
         {
            CUpdateHint objUpdate;
            objUpdate.m_idUpdateHint = idMaximumChange;
            objUpdate.m_nPositionMs = aBeginTimes[i];
            UpdateAllViews(NULL, idMaximumChange, &objUpdate);
         }
      }

      UpdateScreenMode();


        // we must refresh the list, because the pvData pointer to CStructureInformation must be refreshed with it's clone.
        CStructureView *pStructureView = CMainFrameE::GetCurrentInstance()->GetStructureView();
        if (pStructureView) {
            pStructureView->RefreshList();
        }
   }
}


void CEditorDoc::ImportAudio(LPCTSTR tszFileName)
{
   if (!project.ImportAudio(tszFileName))
   {
      // TODO: Fehlermeldung //
      return;
   }

   // The new audio file could be longer
   UpdateDocument();

   // The export will tear down all render engines,
   // so they will have to be rebuilt after the export
   SetModifiedFlag(TRUE);
   RebuildPreview();

   UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
}

void CEditorDoc::ImportVideo(LPCTSTR tszFileName)
{
   HRESULT hr = AVInfo::StaticCheckRenderable(tszFileName);
   if (S_OK != hr)
   {
      CString csMsg;
      csMsg.Format(IDS_MSG_VIDEOFILE_NOT_RENDERABLE, hr);
      ShowErrorMessage(csMsg);
      return;
   }

   if (!project.ImportVideo(tszFileName))
   {
      // TODO?: Fehlermeldung //
      return;
   }

   // The new audio file could be longer
   UpdateDocument();

   SetModifiedFlag(TRUE);
   RebuildPreview();

   UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);

   {
      AVInfo avInfo;
      CString csFileName(tszFileName);
      CString csSuffix = csFileName;
      StringManipulation::GetFileSuffix(csSuffix);
      csSuffix.MakeLower();
      if (!(csSuffix.Find(_T("avi")) >= 0) ||
         !avInfo.HasVideoVfwCodec(csFileName))
      {
         CString csMsg;
         csMsg.LoadString(IDS_MSG_NOT_AVI_IMPORT);
         ShowInfoMessage(csMsg);
         // We are importing a WM/MPEG/whatever file.
         // The export format may have changed; tell the user
         GetMainFrame()->DisplayExportSettings(0);
      }
   }
}

void CEditorDoc::RemoveVideo()
{
   if (!project.RemoveVideo())
   {
      // TODO?: Fehlermeldung //
      return;
   }


   SetModifiedFlag(TRUE);
   RebuildPreview();

   UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
}

HRESULT CEditorDoc::InsertNewPage(UINT nPositionMs, CString &csPageTitle, UINT nLengthMs)
{
   HRESULT hr = project.InsertNewPage(nPositionMs, csPageTitle, nLengthMs);

   if (SUCCEEDED(hr) && hr != S_FALSE)
   {
      project.UpdateStructureAndStreams();
      
      UpdateDocument();
      m_displayStartMs = 0;
      m_displayEndMs   = m_docLengthMs;
         
      // Inserting something is definitely a change
      SetModifiedFlag();
      RebuildPreview();
         
      UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
   }

   return hr;
}

void CEditorDoc::InsertClip(LPCTSTR tszFilename)
{
   HRESULT hr = AVInfo::StaticCheckRenderable(tszFilename);
   if (S_OK != hr)
   {
      CString csMsg;
      csMsg.Format(IDS_MSG_VIDEOFILE_NOT_RENDERABLE, hr);
      ShowErrorMessage(csMsg);
      return;
   }

   bool bNeedUpdate;
   if (project.InsertClip(tszFilename, m_curPosMs, bNeedUpdate))
   {
      project.UpdateStructureAndStreams();
      if (bNeedUpdate)
      {
         UpdateDocument();
         m_displayStartMs = 0;
         m_displayEndMs   = m_docLengthMs;
         
         // Inserting something is definitely a change
         SetModifiedFlag();
         RebuildPreview();
         
         UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
      }
      else
      {
         SetModifiedFlag();
         RebuildPreview();

         UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
      }

      AVInfo avInfo;
      CString csFileName(tszFilename);
      CString csSuffix = csFileName;
      StringManipulation::GetFileSuffix(csSuffix);
      csSuffix.MakeLower();
      if (!(csSuffix.Find(_T("avi")) >= 0) ||
         !avInfo.HasVideoVfwCodec(csFileName))
      {
         CString csMsg;
         csMsg.LoadString(IDS_MSG_NOT_AVI_IMPORT);
         ShowInfoMessage(csMsg);
         // We are importing a WM/MPEG/whatever file.
         // The export format may have changed; tell the user
         GetMainFrame()->DisplayExportSettings(4, tszFilename);
      }
   }
}

void CEditorDoc::InsertClipFromLrd(LPCTSTR tszFilename)
{
   CString csClipFileName(_T(""));

   // InsertClipFromLrd checks if the extracted clip is
   // renderable, so we need not do that here.
   if (project.InsertClipFromLrd(tszFilename, m_curPosMs, csClipFileName))
   {
      project.UpdateStructureAndStreams();
      SetModifiedFlag();
      RebuildPreview();

      UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);

      AVInfo avInfo;
      CString csSuffix = csClipFileName;
      StringManipulation::GetFileSuffix(csSuffix);
      csSuffix.MakeLower();
      if (!(csSuffix.Find(_T("avi")) >= 0) ||
         !avInfo.HasVideoVfwCodec(csClipFileName))
      {
         CString csMsg;
         csMsg.LoadString(IDS_MSG_NOT_AVI_IMPORT);
         ShowInfoMessage(csMsg);
         // We are importing a WM/MPEG/whatever file.
         // The export format may have changed; tell the user
         GetMainFrame()->DisplayExportSettings(4, tszFilename);
      }
   }
}

void CEditorDoc::DeleteCurrentClip()
{
   DeleteClipAt(m_curPosMs);
}

void CEditorDoc::DeleteClipAt(int iPositionMs)
{
   bool bNeedUpdate = false;;
   if (project.DeleteClip(iPositionMs, bNeedUpdate))
   {
      project.UpdateStructureAndStreams();
      if (bNeedUpdate) 
      {
         // length changed
         UpdateDocument();
         m_displayStartMs = 0;
         m_displayEndMs   = m_docLengthMs;
         // Deleting something is definitely a change
      }
      
      SetModifiedFlag();
      RebuildPreview();

      UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
   }

}

void CEditorDoc::NotifyCommitMoveClip()
{
   project.UpdateStructureAndStreams();
   SetModifiedFlag();
   RebuildPreview();
   
   UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
   
}

void CEditorDoc::InsertStillImage(LPCTSTR tszFilename)
{
   if (project.InsertStillImage(tszFilename))
   {
      SetModifiedFlag();
      RebuildPreview();
      UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
   }
}

void CEditorDoc::RemoveStillImage()
{
   if (project.RemoveStillImage())
   {
      SetModifiedFlag();
      RebuildPreview();
      UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
   }
}

bool CEditorDoc::IsClipActive()
{
   if (project.FindClipAt(m_curPosMs) != NULL)
      return true;
   else
      return false;
}

BOOL CEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{					
   // PZI: moved here from EditorProject::Import() 
   // because the following CDocument::OnOpenDocument() blocks the LRD file (read-only)
   CString csFileSuffix = lpszPathName;
   StringManipulation::GetFileSuffix(csFileSuffix);
   csFileSuffix.MakeUpper();
   if (csFileSuffix == _T("LRD"))
      // TODO: error handling
      CScreengrabbingExtendedExternal::RunPostProcessing(lpszPathName, false);
	  

   // If some WAV files had to be created, it may be that
   // during the opening of the document, the document
   // is "dirty", i.e. has to be changed.
   m_bDirtyDuringOpen = false;

   if (!CDocument::OnOpenDocument(lpszPathName))
       return FALSE;

   if (m_bDirtyDuringOpen)
       SetModifiedFlag(TRUE);

   RebuildPreview();

   if (!CStudioApp::IsLiveContextMode()) {
       CDocumentManager docManager(AfxGetApp());
       docManager.AddRecordingToRegistry(lpszPathName);
   } // else // Do nothing related to media library (LC has its own)

   return TRUE;
}

void CEditorDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
    CString fileSuffix = lpszPathName;
    StringManipulation::GetFileSuffix(fileSuffix);
    fileSuffix.MakeUpper();

    if (fileSuffix != _T("LRD"))
        CDocument::SetPathName(lpszPathName, bAddToMRU);
    else
    {
        CString csLepPathName = lpszPathName;
        StringManipulation::GetPathFilePrefix(csLepPathName);
        csLepPathName += _T(".lep");

        CDocument::SetPathName(csLepPathName, FALSE); // TODO FALSE ???
    }
}

void CEditorDoc::MarkPageAt(int nTimeMs)
{
   CPage *pPage = project.FindPageAt(nTimeMs);
   if (pPage) {
       m_markStartMs = pPage->GetBegin();
       m_markEndMs = pPage->GetEnd();

       UpdateAllViews(NULL, HINT_CHANGE_SELECTION);
   }
}

void CEditorDoc::MarkClipAt(int nTimeMs)
{
   UINT nClipStartMs = 0;
   UINT nClipLengthMs = 0;

   CClipStreamR *pClipStream = project.GetClipStream();
   if (pClipStream != NULL)
   {
      HRESULT hr = pClipStream->GetClipTimesAt((unsigned)nTimeMs, &nClipStartMs, &nClipLengthMs);

      if (SUCCEEDED(hr) && nClipLengthMs > 0)
      {
         m_markStartMs = nClipStartMs;
         m_markEndMs = nClipStartMs + nClipLengthMs;
         
         UpdateAllViews(NULL, HINT_CHANGE_SELECTION);
         
      }
   }
}

// private
bool CEditorDoc::AdaptDisplayRange()
{
   // TODO use this mechanism also for all other changes to the display range
   // like zooming itself or insertions and deletions during zoom.


   // if the current position is outside of the zoomed range
   // adapt the range: 2/3 on one side and 1/3 of the range on the
   // other side of the position; according to where the range was

   bool bSomethingChanged = false;

   if (m_displayEndMs > m_docLengthMs)
   {
      // for example after deletion

      int iDiffMs = m_displayEndMs - m_docLengthMs;

      m_displayEndMs -= iDiffMs;
      m_displayStartMs -= iDiffMs;

      if (m_displayStartMs < 0)
         m_displayStartMs = 0;

      bSomethingChanged = true;
   }

   if (m_docLengthMs == 0 && (m_displayStartMs != 0 || m_displayEndMs != 0))
   {
      m_displayStartMs = 0;
      m_displayEndMs   = 0;

      bSomethingChanged = true;
   }
   
   bool bZoomed = m_displayStartMs > 0 || m_displayEndMs < m_docLengthMs;
   int iDisplayWidthMs = m_displayEndMs - m_displayStartMs;
   
   int iLowerBoundaryMs = 0;
   int iUpperBoundaryMs = m_docLengthMs;

   if (bZoomed)
   {
      iLowerBoundaryMs = m_displayStartMs + iDisplayWidthMs / 10;
      iUpperBoundaryMs = m_displayStartMs + (iDisplayWidthMs * 9) / 10;
   }

   if (m_curPosMs < iLowerBoundaryMs)
   {
      m_displayStartMs -= iLowerBoundaryMs - m_curPosMs;
      m_displayEndMs   = m_displayStartMs + iDisplayWidthMs;
     
      bSomethingChanged = true;
   }
   else if (m_curPosMs > iUpperBoundaryMs)
   {
      m_displayEndMs   += m_curPosMs - iUpperBoundaryMs;
      m_displayStartMs = m_displayEndMs - iDisplayWidthMs;
  
      bSomethingChanged = true;
   }

   if (bSomethingChanged)
   {
      if (m_displayEndMs > m_docLengthMs)
      {
         m_displayEndMs   = m_docLengthMs;
         m_displayStartMs = m_docLengthMs - iDisplayWidthMs;
      }
     
      if (m_displayStartMs < 0)
      {
         m_displayStartMs = 0;
         m_displayEndMs   = iDisplayWidthMs;
      }
   }

   return bSomethingChanged;
}


void CEditorDoc::ShowPageView()
{
   CMainFrameE *pFrame = GetMainFrame();
   
   if (pFrame != NULL)
   {
      pFrame->ShowPane(CMainFrameE::XTP_PAGEOBJECTS_VIEW);
      pFrame->UpdatePageObjectsView();
   }
}

BOOL CEditorDoc::SaveModified()
{
	if (!IsModified())
		return TRUE;        // ok to continue

	CString prompt;
    prompt.LoadString(IDS_SAVE_CHANGES);
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
        SetModifiedFlag(FALSE);
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

void CEditorDoc::AsyncTriggerDocUpdate()
{
   if (m_pMainFrame == NULL)
      m_pMainFrame = this->GetMainFrame(); // should only be necessary once

   if (m_pMainFrame != NULL)
         m_pMainFrame->TriggerDocUpdate();
}

bool CEditorDoc::ShowAsScreenSimulation() {
    bool bShowAsScreenSimulation = false;

    if (IsScreenGrabbingDocument() && !project.ShowScreenGrabbingAsRecording())
        bShowAsScreenSimulation = true;

    return bShowAsScreenSimulation;
}

HRESULT CEditorDoc::OpenAsScreenRecording() {
    HRESULT hr = S_OK;

    project.SetShowScreenGrabbingAsRecording(true);

    return hr;
}

HRESULT CEditorDoc::OpenAsScreenSimulation() {
    HRESULT hr = S_OK;

    project.SetShowScreenGrabbingAsRecording(false);

    return hr;
}

bool CEditorDoc::IsDemoDocument(){
    CInteractionStream *pInteractionStream = project.GetInteractionStream(false);
    if (pInteractionStream) {
        return IsScreenGrabbingDocument() && pInteractionStream->HasDemoDocumentObjects();
    }
    return false;
}

bool CEditorDoc::IsDemoModeRequired() {
    if (IsScreenGrabbingDocument()) {
        CInteractionStream *pInteractionStream = project.GetInteractionStream(false);
        CMarkStream *pMarkStream = project.GetMarkStream(false);
        if ((pInteractionStream != NULL && pInteractionStream->GetInteractionsCount() > 0 && !pInteractionStream->InteractionsAreOnlyClickInfos())
            || (pMarkStream != NULL && pMarkStream->GetMarksCount() > 0)) {
            return true;
        }
    }
    return false;
}

void CEditorDoc::UpdateScreenMode() {
    if (!IsScreenGrabbingDocument()) {
        return;
    }
    if (project.IsEmpty() || IsPreviewActive()) {
		return;
    }
    CInteractionStream *pInteractionStream = project.GetInteractionStream();
    if (!pInteractionStream) {
        return;
    }
    bool bExistInteractionObject = false;

    CTimePeriod ctpDocument;
    ctpDocument.Init(INTERACTION_PERIOD_ALL_PAGES, 0, m_docLengthMs);
    CArray<CInteractionAreaEx *, CInteractionAreaEx *> arAllObjects;
    project.GetInteractionStream()->FindInteractions(&ctpDocument, false, &arAllObjects);
    if (arAllObjects.GetSize() > 0){
        // we have 3 redundant feedback interactions in case a test was inserted and afterwards deleted.
        bool bOnlyFeedbackInteractions = true;
        for (int i = 0; i< arAllObjects.GetSize(); i++) {
            if(arAllObjects[i]->GetClassId() != INTERACTION_CLASS_FEEDBACK){
                bOnlyFeedbackInteractions = false;// this "else" condition shouldn't exist. But the feedbacks are not deleted, after the last test was deleted.
            }
        }
        if (!bOnlyFeedbackInteractions) {
            bExistInteractionObject = true;
        }
    }

    if (bExistInteractionObject) {
        if (!ShowAsScreenSimulation()) {
            CMainFrameE::GetCurrentInstance()->SwitchRecordingToSimulation();
        }
    } else if (ShowAsScreenSimulation()) {
        CMainFrameE::GetCurrentInstance()->SwitchSimulationToRecording();
    }
}
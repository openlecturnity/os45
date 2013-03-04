// MainFrm.cpp : Implementierung der Klasse CMainFrameE
//

#include "stdafx.h"

#include "MainFrm.h"
#include "exportsettings.h"
#include "editorview.h"
#include "lutils.h"
#include "misc.h"
#include "WhiteboardView.h"

#include "LanguageSupport.h"

#include "InsertStopmarkDialog.h"
#include "InsertTargetmarkDialog.h"
#include "ControlLineWidthSelector.h"
#include "ControlLineStyleSelector.h"
#include "GalleryItems.h"
#include "QuestionStream.h"
#include "QuestionnaireSettings.h"
#include "TestSettings.h"
#include "EditInteractionDialog.h"

#include "..\Studio\XTPMDIChildWnd.h"
#include "..\Studio\MainFrm.h"
#include "..\Studio\Studio.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GET_DOC(x) CEditorDoc *x = (CEditorDoc *) CMainFrameE::GetEditorDocument(); \
                   if (!x) \
                       return;

#define TIMER_ICON_ID 291
#define TIMER_REPLAY_ID 104
#define TIMER_REPLAY_TIMEOUT 50
#define EDITOR_REGISTRY _T("Software\\imc AG\\LECTURNITY\\Editor")
#define COMMAND_BARS_MODE_EDITOR _T("CommandBarsEditor")
/////////////////////////////////////////////////////////////////////////////
// CMainFrameE

IMPLEMENT_DYNCREATE(CMainFrameE, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CMainFrameE, CMDIChildWnd)
    //{{AFX_MSG_MAP(CMainFrameE)
    ON_WM_CREATE()
    ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileOpen)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
    ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE16, OnUpdateFileOpen)
    ON_COMMAND(ID_NEW_SLIDE, OnNewSlide)
    ON_UPDATE_COMMAND_UI(ID_NEW_SLIDE, OnUpdateNewSlide)
    ON_WM_CLOSE()
    ON_WM_SYSCOLORCHANGE()
    ON_COMMAND(ID_EDIT_IMPORTAUDIO, OnImportAudio)
    ON_COMMAND(ID_EDIT_IMPORT_VIDEO, OnImportVideo)
    //ON_COMMAND(IDC_UNDO, OnUndo)
    //ON_COMMAND(IDC_REDO, OnRedo)
    ON_COMMAND(IDC_EDIT_CUT, OnCut)
    ON_COMMAND(IDC_EDIT_COPY, OnCopy)
    ON_COMMAND(IDC_EDIT_PASTE, OnPaste)
    ON_COMMAND(IDC_EDIT_DELETE, OnDelete)
    ON_COMMAND(IDC_MARK_START, OnJumpMarkStart)
    ON_COMMAND(IDC_MARK_END, OnJumpMarkEnd)
    ON_COMMAND(IDC_MARK_ALL, OnMarkAll)
    ON_COMMAND(IDC_MARK_NONE, OnMarkNone)
    ON_COMMAND(ID_CREATESTRUCTURE, OnCreateSgStructure)
    ON_COMMAND(ID_INSERT_PRESENTATION, OnInsertPresentation)
    ON_COMMAND(ID_INSERT_PRESENTATION_BEGINNING, OnInsertPresentationBeginning)
    ON_COMMAND(ID_INSERT_PRESENTATION_END, OnInsertPresentationEnd)
    ON_COMMAND(ID_CLIP_INSERT, OnClipInsert)
    ON_COMMAND(ID_CLIP_INSERT_LRD, OnClipInsertFromLrd)
    ON_COMMAND(ID_STILLIMAGE, OnInsertStillImage)
    ON_COMMAND(IDC_INSERT_STOPMARK, OnInsertStopmark)
    ON_UPDATE_COMMAND_UI(IDC_INSERT_STOPMARK, OnUpdateInsertStopmark)
    ON_COMMAND(IDC_INSERT_TARGETMARK, OnInsertTargetmark)
    ON_UPDATE_COMMAND_UI(IDC_INSERT_TARGETMARK, OnUpdateInsertTargetmark)
    ON_COMMAND(IDC_STARTPREVIEW, OnStartPreview)
    ON_COMMAND(IDC_PAUSEPREVIEW, OnPausePreview)
    ON_COMMAND(IDC_STARTSTOPPREVIEW, OnStartStopPreview)
    ON_COMMAND(IDC_PREVIEWSELECTION, OnPreviewSelection)
    ON_COMMAND(IDC_JUMP_POS1, OnJumpPos1)
    ON_COMMAND(IDC_JUMP_END, OnJumpEnd)
    ON_COMMAND(IDC_JUMP_FORWARD, OnJumpForward)
    ON_COMMAND(IDC_JUMP_BACK, OnJumpBack)
    ON_COMMAND(ID_HELP, OnHelp)
    ON_COMMAND(ID_SHOW_IMC, CMainFrame::OnShowImc)
    ON_COMMAND_RANGE(IDS_SHOW_IMC_WEBPAGE_TUTORIALS,IDS_SHOW_IMC_WEBPAGE_BUYNOW, CMainFrame::OnShowImcPages)
    ON_UPDATE_COMMAND_UI(ID_HELP, OnUpdateHelp)
    ON_COMMAND(IDC_MARK_SETMARK_LEFT, OnSetLeftMark)
    ON_COMMAND(IDC_MARK_SETMARK_RIGHT, OnSetRightMark)
    ON_COMMAND(ID_EDIT_EXPORTAUDIO, OnExportAudio)
    ON_COMMAND(ID_EDIT_REMOVE_VIDEO, OnRemoveVideo)
    ON_COMMAND(ID_CLIP_DELETE, OnClipDelete)
    ON_COMMAND(ID_EDIT_DELETESTILLIMAGE, OnRemoveStillImage)
    ON_COMMAND(ID_VIEW_THUMB_BIG, OnThumbBig)
    ON_COMMAND(ID_VIEW_THUMB_MIDDLE, OnThumbMiddle)
    ON_COMMAND(ID_VIEW_THUMB_SMALL, OnThumbSmall)
    ON_COMMAND(ID_MARK_PAGE, OnMarkCurrentPage)
    ON_COMMAND(ID_MARK_CLIP, OnMarkCurrentClip)
    ON_COMMAND(ID_CLIP_TITLE, OnChangeCurrentClipTitle)
    ON_COMMAND(ID_PAGE_PROPERTIES, OnChangeCurrentPageTitle)
    ON_COMMAND(ID_CLIP_PROPERTIES, OnDisplayCurrentClipExportSettings)
    ON_COMMAND(ID_INDICATOR_ZOOM_SELECTED, OnZoomSelected)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOM_SELECTED, OnUpdateZoomSelected)
    ON_COMMAND(ID_INDICATOR_ZOOM_ALL, OnZoomAll)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOM_ALL, OnUpdateZoomAll)
    ON_UPDATE_COMMAND_UI(IDC_STARTPREVIEW, OnUpdateStartPreview)
    ON_UPDATE_COMMAND_UI(IDC_STARTSTOPPREVIEW, OnUpdateStartPreview)
    ON_UPDATE_COMMAND_UI(IDC_PAUSEPREVIEW, OnUpdatePausePreview)
    ON_UPDATE_COMMAND_UI(IDC_PREVIEWSELECTION, OnUpdatePreviewSelection)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_PASTE, OnUpdatePaste)
    ON_UPDATE_COMMAND_UI(IDC_UNDO, OnUpdateUndo)
    ON_UPDATE_COMMAND_UI(IDC_REDO, OnUpdateRedo)
    ON_MESSAGE(WM_AVEDIT, OnHandleAvEditMessage)
    ON_BN_CLICKED(IDC_LOGO, OnOpenWebpage)
    ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
    ON_COMMAND(ID_FILE_EXPORTSELECTION, OnFileExportSelection)
    ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, OnUpdateProjectCommand)
    ON_COMMAND(ID_FILE_EXPORT_SETTINGS, OnExportSettings)
    ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_SETTINGS, OnUpdateExportSettings)
    ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
    ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateFileImport)
    ON_UPDATE_COMMAND_UI(ID_CREATESTRUCTURE, OnUpdateCreateSgStructure)
    ON_UPDATE_COMMAND_UI(ID_INSERT_PRESENTATION, OnUpdateInsertPresentation)
    ON_UPDATE_COMMAND_UI(ID_CLIP_INSERT, OnUpdateClipInsert)
    ON_UPDATE_COMMAND_UI(ID_STILLIMAGE, OnUpdateInsertStillImage)
    ON_UPDATE_COMMAND_UI(ID_CLIP_DELETE, OnUpdateClipDelete)
    ON_UPDATE_COMMAND_UI(ID_EDIT_DELETESTILLIMAGE, OnUpdateRemoveStillImage)
    ON_UPDATE_COMMAND_UI(ID_CLIP_REMOVE, OnUpdateRemoveClip)

    ON_UPDATE_COMMAND_UI(ID_EDIT_IMPORT_VIDEO, OnUpdateImportVideo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REMOVE_VIDEO, OnUpdateRemoveVideo)
    ON_UPDATE_COMMAND_UI(ID_VIEW_THUMB_BIG, OnUpdateThumbDimension)
    ON_UPDATE_COMMAND_UI(ID_MARK_PAGE, OnUpdateMarkPage)
    ON_COMMAND(ID_EXPLORE_LECTURNITYHOME, OnExploreLecturnityHome)
    ON_UPDATE_COMMAND_UI(ID_EXPLORE_LECTURNITYHOME, OnUpdateExploreLecturnityHome)
    ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTSELECTION, OnUpdateExportSelection)
    ON_UPDATE_COMMAND_UI(ID_CLIP_INSERT_LRD, OnUpdateClipInsert)
    ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_SETTINGS, OnUpdateProjectCommand)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
    ON_UPDATE_COMMAND_UI(IDC_JUMP_BACK, OnUpdateJumpButtons)
    ON_UPDATE_COMMAND_UI(IDC_JUMP_FORWARD, OnUpdateJumpButtons)
    ON_UPDATE_COMMAND_UI(IDC_JUMP_END, OnUpdateJumpButtons)
    ON_UPDATE_COMMAND_UI(IDC_JUMP_POS1, OnUpdateJumpButtons)
    ON_UPDATE_COMMAND_UI(IDC_MARK_SETMARK_LEFT, OnUpdateSelectionButtons)
    ON_UPDATE_COMMAND_UI(IDC_MARK_SETMARK_RIGHT, OnUpdateSelectionButtons)
    ON_UPDATE_COMMAND_UI(IDC_MARK_START, OnUpdateJumpMark)
    ON_UPDATE_COMMAND_UI(IDC_MARK_END, OnUpdateJumpMark)
    ON_UPDATE_COMMAND_UI(IDC_MARK_NONE, OnUpdateCutCopy)
    ON_UPDATE_COMMAND_UI(IDC_MARK_ALL, OnUpdateSelectionButtons)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_CUT, OnUpdateCutCopy)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_COPY, OnUpdateCutCopy)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_DELETE, OnUpdateCutCopy)
    ON_UPDATE_COMMAND_UI(ID_INSERT_PRESENTATION_BEGINNING, OnUpdateInsertPresentation)
    ON_UPDATE_COMMAND_UI(ID_INSERT_PRESENTATION_END, OnUpdateInsertPresentation)
    ON_UPDATE_COMMAND_UI(ID_EDIT_EXPORTAUDIO, OnUpdateProjectCommand)
    ON_UPDATE_COMMAND_UI(ID_EDIT_IMPORTAUDIO, OnUpdateProjectCommand)
    ON_UPDATE_COMMAND_UI(ID_VIEW_THUMB_MIDDLE, OnUpdateThumbDimension)
    ON_UPDATE_COMMAND_UI(ID_VIEW_THUMB_SMALL, OnUpdateThumbDimension)
    ON_UPDATE_COMMAND_UI(ID_MARK_CLIP, OnUpdateClipChange)
    ON_UPDATE_COMMAND_UI(ID_PAGE_PROPERTIES, OnUpdateMarkPage)
    ON_UPDATE_COMMAND_UI(ID_CLIP_TITLE, OnUpdateClipChange)
    ON_UPDATE_COMMAND_UI(ID_CLIP_PROPERTIES, OnUpdateClipChange)
    ON_COMMAND(IDC_SHOW_STRUCTURE, OnShowStructure)
    ON_UPDATE_COMMAND_UI(IDC_SHOW_STRUCTURE, OnUpdateShowStructure)
    ON_COMMAND(IDC_SHOW_VIDEO, OnShowVideo)
    ON_UPDATE_COMMAND_UI(IDC_SHOW_VIDEO, OnUpdateShowVideo)
    ON_COMMAND(IDC_SHOW_STREAMS, OnShowStreams)
    ON_UPDATE_COMMAND_UI(IDC_SHOW_STREAMS, OnUpdateShowStreams)
    ON_COMMAND(IDC_SHOW_PAGEOBJECTS, OnShowPageobjects)
    ON_UPDATE_COMMAND_UI(IDC_SHOW_PAGEOBJECTS, OnUpdateShowPageobjects)
    ON_COMMAND(ID_ADD_BUTTON, OnAddButton)

    ON_UPDATE_COMMAND_UI(ID_STATUS_POSITION,OnUpdateStatusPosition)
    ON_UPDATE_COMMAND_UI(ID_STATUS_SELECTION,OnUpdateStatusSelection)
    ON_UPDATE_COMMAND_UI(ID_STATUSBAR_POSITION,OnUpdateStatusPositionCurrentTime)
    ON_UPDATE_COMMAND_UI(ID_STATUSBAR_LENGHT,OnUpdateStatusPositionTotalLenght)

    ON_COMMAND(ID_CLOSE, OnLrdClose)
    ON_UPDATE_COMMAND_UI(ID_CLOSE, OnUpdateLrdClose)

    ON_COMMAND(IDB_BACK_TO_BUTTON, OnLrdClose)
    ON_UPDATE_COMMAND_UI(IDB_BACK_TO_BUTTON, OnUpdateLrdClose)

    ON_WM_TIMER()
    //ribbon messages start
    ON_COMMAND(ID_MENU_EXPORT, OnMenuExport)
    ON_COMMAND(ID_MENU_AUDIO, OnMenuAudio)
    ON_COMMAND(ID_MENU_VIDEO, OnMenuVideo)
    ON_UPDATE_COMMAND_UI(ID_MENU_AUDIO, OnUpdateMenuAudio)
    ON_UPDATE_COMMAND_UI(ID_MENU_VIDEO, OnUpdateMenuVideo)
    ON_COMMAND(ID_MEDIA_LRD_BUTTON, OnButtonInsertLRD)
    ON_UPDATE_COMMAND_UI(ID_MEDIA_LRD_BUTTON, OnUpdateButtonInsertLRD)
    ON_COMMAND(ID_MEDIA_CLIP_BUTTON, OnButtonInsertClip)
    //ribbon messages end
    //}}AFX_MSG_MAP

    ON_NOTIFY(XTP_SBN_SCROLL, ID_INDICATOR_ZOOMSLIDER, OnZoomSliderScroll)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOMSLIDER, OnUpdateZoomSliderScroll)

    // BUGFIX 4832: remove partly meaningless customization options
    // ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)
    ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
    ON_XTP_EXECUTE_RANGE(XTP_ID_PIN_FILE_MRU_FIRST,XTP_ID_PIN_FILE_MRU_LAST,OnPinRangeClick)

    ON_XTP_CREATECOMMANDBAR()

    ON_WM_ACTIVATE()

    ON_COMMAND(ID_INTERACTION_AREA, OnInsertInteraction)
    ON_UPDATE_COMMAND_UI(ID_INTERACTION_AREA, OnUpdateInsertInteraction)
    ON_COMMAND(ID_ADD_QUESTION_DD, OnInsertTest)
    ON_UPDATE_COMMAND_UI(ID_ADD_QUESTION_DD, OnUpdateInsertTest)
    ON_COMMAND(ID_INTERACTION_BUTTON, OnInsertInteraction)
    ON_UPDATE_COMMAND_UI(ID_INTERACTION_BUTTON, OnUpdateInsertInteraction)
    ON_COMMAND(ID_ADD_QUESTION_FB, OnInsertTest)
    ON_UPDATE_COMMAND_UI(ID_ADD_QUESTION_FB, OnUpdateInsertTest)
    ON_COMMAND(ID_ADD_QUESTION_MC, OnInsertTest)
    ON_UPDATE_COMMAND_UI(ID_ADD_QUESTION_MC, OnUpdateInsertTest)
    ON_COMMAND(ID_GRAPHICALOBJECT_CALLOUT, OnButtonCallout)
    ON_UPDATE_COMMAND_UI(ID_GRAPHICALOBJECT_CALLOUT, OnUpdateButtonCallout)
    ON_UPDATE_COMMAND_UI(ID_GRAPHICALOBJECT_TEXT, OnUpdateInsertText)
    ON_COMMAND(ID_GRAPHICALOBJECT_TEXT, OnInsertText)
    ON_COMMAND(ID_DEMOOBJECT, OnButtonDemoObject)
    ON_UPDATE_COMMAND_UI(ID_DEMOOBJECT, OnUpdateButtonDemoObject)
    ON_COMMAND(ID_CREATE_DEMO_DOCUMENT, OnCreateDemoDocument)
    ON_UPDATE_COMMAND_UI(ID_CREATE_DEMO_DOCUMENT, OnUpdateCreateDemoDocument)
    ON_COMMAND(ID_EMBED_MOUSEPOINTER, OnHideMousePointer)
    ON_UPDATE_COMMAND_UI(ID_EMBED_MOUSEPOINTER, OnUpdateHideMousePointer)
    ON_XTP_EXECUTE(ID_CALLOUTS, OnXTPGalleryCallouts)
    ON_UPDATE_COMMAND_UI(ID_CALLOUTS, OnUpdateGalleryCallouts)
    ON_UPDATE_COMMAND_UI(ID_DEMOOBJECTS, OnUpdateGalleryDemoObjects)
    ON_XTP_EXECUTE(ID_DEMOOBJECTS, OnXTPGalleryDemoObjects)
    ON_COMMAND(ID_CREATE_MANUAL, OnCreateManual)
    ON_UPDATE_COMMAND_UI(ID_CREATE_MANUAL, OnUpdateCreateManual)
    ON_COMMAND(ID_GODEF_OPTIONS, OnGraphicalObjectDefaultSettings)
    ON_UPDATE_COMMAND_UI(ID_GODEF_OPTIONS, OnUpdateGraphicalObjectDefaultSettings)
END_MESSAGE_MAP()


// "static initialization"
COLORREF CMainFrameE::s_clrDark;
COLORREF CMainFrameE::s_clrLight;
COLORREF CMainFrameE::s_clrBackgroundLight;
COLORREF CMainFrameE::s_clrBackgroundDark;
COLORREF CMainFrameE::s_clrBorder;
COLORREF CMainFrameE::s_clrStreamBg;
COLORREF CMainFrameE::s_clrStreamLine;

// first two field in status bar
// the text in the status bar can be found in the string resources
// the text can be updated with OnUpdate...
static UINT indicators[] =
{
   ID_STATUSBAR_POSITION,
   ID_STATUSBAR_LENGHT,
	//ID_STATUS_POSITION,
	ID_STATUS_SELECTION
};

CDocTemplate *CMainFrameE::GetAssociatedTemplate()
{
    CDocTemplate *pEditorDocTemplate = NULL;

    CWinApp *pApp = AfxGetApp();
    if (pApp) {
        POSITION templPos = pApp->GetFirstDocTemplatePosition();

        while (templPos != NULL) {
            CDocTemplate *pDocTemplate = pApp->GetNextDocTemplate(templPos);
            CString csDocString;
            pDocTemplate->GetDocString(csDocString, CDocTemplate::windowTitle);
            if (csDocString == _T("Editor")) {
                pEditorDocTemplate = pDocTemplate;
                break;
            }
        }
    }

    return pEditorDocTemplate;
}

CEditorDoc *CMainFrameE::GetEditorDocument() {
    return CEditorDoc::GetCurrentInstance();
}

CWhiteboardView *CMainFrameE::GetEditorView() { 
    if (CEditorDoc::GetCurrentInstance() != NULL) 
        return CEditorDoc::GetCurrentInstance()->GetWhiteboardView();
    return NULL;
}

CMainFrameE *CMainFrameE::s_pMainFrame = NULL;

CMainFrameE* CMainFrameE::GetCurrentInstance() {
    return s_pMainFrame;
}

void CMainFrameE::RefreshPaneDrawing()
{
    m_paneManager.GetPaintManager()->RefreshMetrics();
    m_paneManager.RedrawPanes();
}

// static
CString CMainFrameE::GetInitialOpenDir()
{
   CString csDir;
   
   CSecurityUtils securityUtils;
   bool success = securityUtils.GetLecturnityHome(csDir);
   if (success)
   {
      CString csDir2 = csDir;
      csDir2 += _T("\\Recordings");

      if (_taccess(csDir2, 00) == 0)
         csDir = csDir2;
   }

   return csDir;
}

// static
CString CMainFrameE::GetInitialLepDir()
{
   CString csDir;

   CSecurityUtils securityUtils;
   bool success = securityUtils.GetLecturnityHome(csDir);

   return csDir;
}

// static
CString CMainFrameE::GetInitialExportDir()
{
   return GetInitialOpenDir();
}

// Constructor / Destructor

CMainFrameE::CMainFrameE() {
    s_pMainFrame = this;

    m_bHasCheckedForManual = false;
    m_bHasManual = false;

    m_pStreamView = NULL;
    m_pStructureView = NULL;
    m_pInteractionStructureView = NULL;
    m_pWhiteboardView = NULL;
    m_pVideoView = NULL;
    m_bVideoPaneClosedForSgLayout = false;

    m_bViewsAreInStandardLayout = true;
    m_nTimedStatusTimeout = 0;

    m_nTimer = 0;
    m_pSymbolBar = NULL;
    m_bIsDeactivated = false;

    m_bLastCopyToWb = false;
    m_nLastCheckedPreviewPos = 0;
    m_bLastUpdateTooLong = false;  
    m_bPositionJumped = false;

    m_pStandardLayout = NULL;
    m_pScreengrabbingLayout = NULL;

    m_bRecentFilelist = false;
    m_bPlusMinusKeyZoom = false;  

    m_pActiveView = NULL;
    m_iCurrentSelectedInteractionID = -1;
}

CMainFrameE::~CMainFrameE()
{
   if (m_pStandardLayout)
      delete m_pStandardLayout;
   m_pStandardLayout = NULL;

   if (m_pScreengrabbingLayout)
      delete m_pScreengrabbingLayout;
   m_pScreengrabbingLayout = NULL;
}

int CMainFrameE::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    ModifyStyle(WS_MINIMIZEBOX  | WS_MAXIMIZEBOX, 0);

    // get the current colors
    RefreshColors();

    if (!InitCommandBars())
        return -1;

    CXTPPaintManager::SetTheme(xtpThemeRibbon);

    CXTPCommandBars* pCommandBars = GetCommandBars();
    if(pCommandBars == NULL) {
        TRACE0("Failed to create command bars object.\n");
        return -1;      // fail to create
    }

    CreateSymbolbar();

    CXTPCommandBarsOptions* pOptions = pCommandBars->GetCommandBarsOptions();
    pOptions->bAlwaysShowFullMenus = TRUE;

    XTP_COMMANDBARS_ICONSINFO* pIconsInfo = XTPPaintManager()->GetIconsInfo();
    pIconsInfo->bUseDisabledIcons = TRUE;

    LoadCommandBars(_T("CommandBars"));

    DragAcceptFiles();

	CreatePanes();

	// the whiteboard view is part of the view list in CEditorDocument
    m_pWhiteboardView = GetEditorView();

    if (m_pWhiteboardView == NULL)
        return -1;

    SetTimer(TIMER_REPLAY_ID, TIMER_REPLAY_TIMEOUT, NULL);

    ::timeBeginPeriod(1); // for accurate delay informations lateron

    return 0;
}

int CMainFrameE::OnCreateCommandBar(LPCREATEBARSTRUCT lpCreatePopup)
{
	if (lpCreatePopup->bTearOffBar && 
		(lpCreatePopup->nID == IDR_LINECOLOR_POPUP || lpCreatePopup->nID == IDR_FILLCOLOR_POPUP))
	{
		CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, lpCreatePopup->pCommandBar);
		ASSERT(pToolBar);
		
		// Prevent docking.
		pToolBar->EnableDocking(xtpFlagFloating);
		// Prevent customization
		pToolBar->EnableCustomization(FALSE);
		return TRUE;		
	}
	return FALSE;
}

void CMainFrameE::OnCustomize()
{
	CXTPCustomizeSheet cs(GetCommandBars());
	
	CXTPCustomizeOptionsPage pageOptions(&cs);
	cs.AddPage(&pageOptions);
	
	CXTPCustomizeCommandsPage* pCommands = cs.GetCommandsPage();
	pCommands->AddCategories(IDR_MAINFRAME_E);

	pCommands->InsertAllCommandsCategory();
	pCommands->InsertBuiltInMenus(IDR_MAINFRAME_E);
	pCommands->InsertNewMenuCategory();
	
	cs.DoModal();
}

BOOL CMainFrameE::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.

    if (m_pWhiteboardView != NULL && 
        message == WM_XTP_COMMAND && wParam == IDC_FONT_SIZE) {

        NMXTPCHARHDR *pFnm = (NMXTPCHARHDR *)lParam;
        if (pFnm->hdr.code == XTP_FN_GETFORMAT) {
            m_pWhiteboardView->FillCharFormat(pFnm->cf);
        }
        return TRUE;
    }

	return CXTPFrameWndBase<CMDIChildWnd>::OnWndMsg(message, wParam, lParam, pResult);
}

BOOL CMainFrameE::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

   /*
   DWORD dwWindowStyle = AfxGetApp()->GetProfileInt(_T("Window"), _T("Style"), WS_VISIBLE);
   if ((dwWindowStyle & WS_MAXIMIZE) > 0)
      dwWindowStyle = WS_MAXIMIZE;
   else
      dwWindowStyle = WS_VISIBLE;
   
   cs.style |= dwWindowStyle;
   */

	return TRUE;
}

void CMainFrameE::OnUpdateFrameTitle(BOOL bAddToTitle)
{
    SetWindowText(_T(""));
}

void CMainFrameE::SwitchMode() {
    OnLrdClose();
}

BOOL CMainFrameE::CreateToolbar()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	
	CString csName;
	csName.LoadString(IDS_TOOLBAR_NAME);
	CXTPToolBar* pStandardBar = (CXTPToolBar*)pCommandBars->Add(csName, xtpBarTop);
	if (!pStandardBar ||
		!pStandardBar->LoadToolBar(IDR_STANDARD))
	{
      if(pStandardBar)
         pStandardBar->SetVisible(false);
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}
   pStandardBar->SetVisible(false);
	csName.LoadString(IDS_EDITBAR_NAME);
	CXTPToolBar* pEditBar = (CXTPToolBar*)pCommandBars->Add(csName, xtpBarTop);
	if (!pEditBar ||
		!pEditBar->LoadToolBar(IDR_EDIT))
	{
      if(pEditBar)
         pEditBar->SetVisible(false);
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}

	DockRightOf(pEditBar, pStandardBar);
   
   pEditBar->SetVisible(false);

	return TRUE;
}

BOOL CMainFrameE::CreateSymbolbar()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	
	CString csName;
	csName.LoadString(IDS_STREAMBAR_NAME);
	m_pSymbolBar = (CXTPToolBar*)pCommandBars->Add(csName, xtpBarBottom);
	if (!m_pSymbolBar ||
		!m_pSymbolBar->LoadToolBar(IDR_SYMBOLBAR))
	{
		TRACE0("Failed to create symbol bar\n");
		return FALSE;
	}
    if (CStudioApp::IsLiveContextPreview()) {
        CXTPControl *pControl = NULL;
        if((pControl = (CXTPControl *)m_pSymbolBar->GetControls()->FindControl(IDC_MARK_START)))
            pControl->SetHideFlags(xtpHideGeneric);
        if((pControl = (CXTPControl *)m_pSymbolBar->GetControls()->FindControl(IDC_MARK_END)))
            pControl->SetHideFlags(xtpHideGeneric);
        if((pControl = (CXTPControl *)m_pSymbolBar->GetControls()->FindControl(IDC_MARK_SETMARK_LEFT)))
            pControl->SetHideFlags(xtpHideGeneric);
        if((pControl = (CXTPControl *)m_pSymbolBar->GetControls()->FindControl(IDC_MARK_SETMARK_RIGHT)))
            pControl->SetHideFlags(xtpHideGeneric);
    }
   // BUGFIX 4832: disable closing because there is currently no option to enable toolbar again
   m_pSymbolBar->SetCloseable(false);

	return TRUE;
}

void CMainFrameE::CreatePanes()
{
    m_paneManager.InstallDockingPanes(this);
    m_paneManager.SetTheme(xtpPaneThemeOffice2007Ext);
    m_paneManager.SetThemedFloatingFrames(TRUE);
    m_paneManager.SetShowContentsWhileDragging(TRUE);

    if (!(CStudioApp::IsLiveContextMode() && CStudioApp::IsLiveContextPreview())) {
        CXTPDockingPane *pStructurePane = m_paneManager.CreatePane(XTP_STRUCTURE_VIEW,CRect(0, 0, 180, 200), xtpPaneDockLeft/*dockLeftOf*/);
        pStructurePane->SetOptions(xtpPaneNoHideable);
    }

    CXTPDockingPane *pVideoPane = m_paneManager.CreatePane(XTP_VIDEO_VIEW, CRect(0, 0, 180, 100), xtpPaneDockRight/*xtpPaneDockBottom*//*dockBottomOf*//*, pStructurePane*/);
    pVideoPane->SetOptions(xtpPaneNoHideable);

    CXTPDockingPane *pPageobjetsPane = m_paneManager.CreatePane(XTP_PAGEOBJECTS_VIEW, CRect(0, 0, 180, 200/*210, 300*/), xtpPaneDockBottom, pVideoPane/*dockRightOf*/);
    pPageobjetsPane->SetOptions(xtpPaneNoHideable);

    if (!(CStudioApp::IsLiveContextMode() && CStudioApp::IsLiveContextPreview())) {
        CXTPDockingPane *pStreamPane = m_paneManager.CreatePane(XTP_STREAM_VIEW, CRect(0, 0, 300, 200), xtpPaneDockBottom/*dockBottomOf*/);
        pStreamPane->SetOptions(xtpPaneNoHideable);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrameE Diagnose

#ifdef _DEBUG
void CMainFrameE::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CMainFrameE::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrameE Nachrichten-Handler


void CMainFrameE::OnCut() {
    BeginWaitCursor();

    // find out which view is active, whiteboard or stream view
    m_bLastCopyToWb = false;
    if (m_pWhiteboardView != NULL && m_pWhiteboardView->IsActive())	{
        // cut all selected whiteboard element
        m_pWhiteboardView->Cut();
        m_bLastCopyToWb = true;
    } else {
        // cut selected streams 
        CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
        if (pDoc != NULL)
            pDoc->Cut();
    }

    EndWaitCursor();
}

void CMainFrameE::OnCopy() {
    BeginWaitCursor();

    m_bLastCopyToWb = false;
    // find out which view is active, whiteboard or stream view
    if (m_pWhiteboardView != NULL && m_pWhiteboardView->IsActive())	{
        // cut all selected whiteboard element
        m_pWhiteboardView->Copy();
        m_bLastCopyToWb = true;
    } else {
        CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
        if (pDoc)
            pDoc->Copy();
    }

    EndWaitCursor();
}

void CMainFrameE::OnPaste() {
    BeginWaitCursor();

    // find out which view is active, whiteboard or stream view

    if (ProjectHasDataAndNotPreviewing()) {	
        CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();

        if (m_pWhiteboardView != NULL && m_bLastCopyToWb) {
            m_pWhiteboardView->Paste();
            pDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
        } else if (m_pActiveView && m_pStreamView == m_pActiveView) {
            bool updateVisibleStreams = false;
            // This is a workaround for the following problem:
            // If nothing is displayed, the streams should be
            // reenabled after the undo action.
            if (pDoc->m_displayStartMs == pDoc->m_displayEndMs)
                updateVisibleStreams = true;
            if (pDoc)
                pDoc->Paste();
            if (updateVisibleStreams)
                m_pStreamView->UpdateStreamVisibilities(pDoc);
        }
    }

    EndWaitCursor();
}

void CMainFrameE::OnDelete() {
    BeginWaitCursor();

    // find out which view is active, whiteboard or stream view
    if (m_pWhiteboardView != NULL && m_pWhiteboardView->IsActive()) {
        // cut all selected whiteboard element
        m_pWhiteboardView->Delete();
    } else {
        CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
        if (pDoc)
            pDoc->Delete();
    }

    EndWaitCursor();
}

void CMainFrameE::OnUndo() {
    return;
    BeginWaitCursor();
    CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
    if (pDoc) {
        pDoc->Undo();

        m_pStreamView->UpdateStreamVisibilities(pDoc);
        pDoc->UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
    }
    EndWaitCursor();
}

void CMainFrameE::OnRedo() {
    return;
    BeginWaitCursor();
    CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
    if (pDoc) {
        pDoc->Redo();

        m_pStreamView->UpdateStreamVisibilities(pDoc);
        pDoc->UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
    }
    EndWaitCursor();
}

void CMainFrameE::OnFileImport() 
{
   OnFileOpen();

   /*
   CString filter;
   filter.LoadString(IDS_LRD_FILEFILTER);

   CString fileName;
   FileDialog::GetOpenFilename(AfxGetApp(), fileName, AfxGetMainWnd()->GetSafeHwnd(), 
      AfxGetInstanceHandle(), filter, _T("LRDIMPORT"), GetInitialOpenDir());
 
   if (!fileName.IsEmpty())
   {
      CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
      if (pDoc)
      {
         BeginWaitCursor();
         pDoc->Import(fileName);
         
         // TODO this code is side effect or even double code
         // but it is necessary for creating the preview (and thus show video).
         // BUGFIX #2734
         pDoc->UpdateDocument();
         pDoc->m_curPosMs = 0;
         pDoc->m_displayStartMs = 0;
         pDoc->m_displayEndMs = pDoc->m_docLengthMs;
         m_pStreamView->UpdateStreamVisibilities(pDoc);
         pDoc->UpdateAllViews(NULL);

         AdjustToProjectMode();

         pDoc->BuildPreview(m_pVideoView->GetSafeVideoHwnd());

         
         EndWaitCursor();
      }
   }
   */
}

void CMainFrameE::OnFileExportSelection()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
   if (!pDoc)
      return;

   long nMarkStart = pDoc->m_markStartMs;
   long nMarkEnd   = pDoc->m_markEndMs;
   long nDocLength = pDoc->m_docLengthMs;

   long nDisplayStart = pDoc->m_displayStartMs;
   long nDisplayEnd   = pDoc->m_displayEndMs;

   long nActionCount = 0;

   if (nMarkStart > 0)
   {
      pDoc->m_markStartMs = 0;
      pDoc->m_markEndMs = nMarkStart;
      OnDelete();
      nActionCount++;
   }

   if (nMarkEnd < nDocLength)
   {
      pDoc->m_markStartMs = nMarkEnd - nMarkStart;
      pDoc->m_markEndMs = pDoc->m_docLengthMs;
      OnDelete();
      nActionCount++;
   }

   OnFileExport();

   for (int i=0; i<nActionCount; ++i)
      OnUndo();

   pDoc->m_markStartMs = nMarkStart;
   pDoc->m_markEndMs = nMarkEnd;
   pDoc->m_displayStartMs = nDisplayStart;
   pDoc->m_displayEndMs = nDisplayEnd;

   //pDoc->UpdateAllViews(NULL, UPDATE_ZOOM);
}

void CMainFrameE::OnUpdateExportSelection(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = false;
   
   if (ProjectHasDataAndNotPreviewing())
   {
		if (pDoc->m_markStartMs != -1 && pDoc->m_markEndMs != -1)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}


void CMainFrameE::OnFileExport() 
{
	// First check if the export settings dialog has
   // been visited
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
   if (!pDoc)
      return;

   // TODO: What about this here (shouldn't be here)? It is currently not duplicated in DoFileExport()
   if ((!pDoc->project.m_videoAviSettings.bIsVideoFormatInitialized && pDoc->project.HasVideo() && pDoc->project.m_videoStreamLength > 0) ||
        !pDoc->project.m_isAudioFormatInitialized)
   {
      OnExportSettings();
   }

   DoFileExport(this, &(pDoc->project), pDoc);
}

// static
bool CMainFrameE::DoFileExport(CWnd *pParent, CEditorProject *pProject, CEditorDoc *pDoc, LPCTSTR tszTargetFilename) {

    if (pProject == NULL)
        return false;
    // All other parameters are not necessary

    HWND hwndParent = NULL;
    if (pParent != NULL)
        hwndParent = pParent->GetSafeHwnd();
    else
        hwndParent = AfxGetMainWnd()->GetSafeHwnd();

    CString csFilter;
    csFilter.LoadString(IDS_LRD_FILEFILTER);

    CString csFileName;
    // Use given parameter for export filename
    if (tszTargetFilename != NULL)
        csFileName = tszTargetFilename;

    while (csFileName.IsEmpty())
    {
        FileDialog::GetSaveFilename(AfxGetApp(), csFileName, hwndParent, 
            AfxGetInstanceHandle(), csFilter, _T(".lrd"), _T("LRDEXPORT"), GetInitialExportDir());

        if (!csFileName.IsEmpty())
        { 

            // Check if output file is currently used
            bool isLrdUsed = pProject->IsLrdUsed(csFileName);
            if (isLrdUsed)
            {
                CString msg;
                msg.LoadString(IDS_MSG_LRD_USED);
                CString cap;
                cap.LoadString(IDS_INFORMATION);

                ::MessageBox(hwndParent, msg, cap, MB_OK | MB_ICONERROR);
                csFileName.Empty();
                continue;
            }

            // Check if a presentation is in specified directory
            CString csDirName = csFileName;
            StringManipulation::GetPath(csDirName);
            bool bDirectoryContainsLrd = FileInformation::DirectoryContainsLrd(csDirName, csFileName);
            if (bDirectoryContainsLrd)
            {
                CString msg;
                msg.LoadString(IDS_MSG_DIR_CONTAINS_LRD);
                CString cap;
                cap.LoadString(IDS_INFORMATION);

                int result = ::MessageBox(hwndParent, msg, cap, MB_YESNOCANCEL | MB_ICONQUESTION);
                if (result == IDCANCEL)
                    return false;
                else if (result == IDNO)
                {
                    csFileName.Empty();
                    continue;
                }
            }
        }
        else
            return false;
    }

    bool bSuccess = false;

    if (!csFileName.IsEmpty())
    {
        // The export will tear down all render engines,
        // so they will have to be rebuilt after the export
        if (pDoc != NULL)
            pDoc->SetPreviewRebuildNeeded(TRUE);

        if (pParent != NULL)
            pParent->BeginWaitCursor();

        _TCHAR tszExportedLrd[MAX_PATH+1];
        tszExportedLrd[0] = _T('\0');
        bool bExportNeeded = true;
        CString csLrdPath = pProject->GetFilename();
        StringManipulation::GetPathFilePrefix(csLrdPath);
        csLrdPath += _T(".lrd");

        bExportNeeded = LMisc::CheckNeedsFinalization(csLrdPath, tszExportedLrd);
        if (bExportNeeded || (pDoc && pDoc->IsModified()) || tszTargetFilename != NULL) {
            // There should be no message boxes in LiveContext mode
            if (CStudioApp::IsLiveContextMode())
                bSuccess = pProject->Export(csFileName, false, NULL, true);
            else
                bSuccess = pProject->Export(csFileName, false);

            // Rebuild the preview automatically after the export
            if (pDoc != NULL) {
                HWND hwndVideoView = NULL;
                if (pDoc->GetVideoView() != NULL)
                    hwndVideoView = pDoc->GetVideoView()->GetSafeVideoHwnd();
                pDoc->BuildPreview(hwndVideoView);
            }
        } else {
            bSuccess = pProject->CopyExportedDocument(csFileName, tszExportedLrd);
        }

        if (pParent != NULL)
            pParent->EndWaitCursor();
    }

    return bSuccess;
}

void CMainFrameE::OnFileOpen() 
{
   CString csFilter;
   csFilter.LoadString(IDS_LRD_FILEFILTER);

   CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();

   CString fileName;
   FileDialog::GetOpenFilename(AfxGetApp(), fileName, AfxGetMainWnd()->GetSafeHwnd(), 
      AfxGetInstanceHandle(), csFilter, _T("LRD"), GetInitialOpenDir());

   if (!fileName.IsEmpty())
   {
       AfxGetApp()->OpenDocumentFile(fileName);
   }
}

void CMainFrameE::OnCreateSgStructure() 
{
    // TODO: There is double code with "OnFileImport"

    CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();

    if (!pDoc)
        return;

    CString csFilename = pDoc->GetPathName();
    StringManipulation::GetPathFilePrefix(csFilename);
    csFilename += ".lrd";

    // TODO: Export if the document was edited

    BeginWaitCursor();

    pDoc->project.Close();

    pDoc->Import(csFilename);

    // TODO this code is side effect or even double code
    // but it is necessary for creating the preview (and thus show video).
    // BUGFIX #2734
    pDoc->UpdateDocument();
    pDoc->m_curPosMs = 0;
    pDoc->m_displayStartMs = 0;
    pDoc->m_displayEndMs = pDoc->m_docLengthMs;
    m_pStreamView->UpdateStreamVisibilities(pDoc);
    pDoc->UpdateAllViews(NULL);

    AdjustToProjectMode();

    pDoc->BuildPreview(GetVideoHWND());

    EndWaitCursor();
}

void CMainFrameE::OnInsertPresentation() 
{
   CString filter;
   filter.LoadString(IDS_LRD_FILEFILTER);

   CString fileName;
   FileDialog::GetOpenFilename(AfxGetApp(), fileName, AfxGetMainWnd()->GetSafeHwnd(), 
      AfxGetInstanceHandle(), filter, _T("LRDIMPORT"), GetInitialOpenDir());
   

   CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
   if (!fileName.IsEmpty() && pDoc)
   {
      bool wasDenverDocument = pDoc->project.IsDenverDocument();
      BeginWaitCursor();
      pDoc->InsertLrd(fileName);
      if (wasDenverDocument && !pDoc->project.IsDenverDocument())
         m_pStreamView->ShowSlideStream();
      //pDoc->BuildPreview(m_pVideoView->GetSafeVideoHwnd());
      
      m_pStreamView->UpdateStreamVisibilities(pDoc);

      EndWaitCursor();
   }
}

void CMainFrameE::OnInsertPresentationBeginning() 
{
	// TODO: Code für Befehlsbehandlungsroutine hier einfügen
	
   CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
   if (pDoc)
   {
      pDoc->m_curPosMs = 0;
      OnInsertPresentation();
   }
}

void CMainFrameE::OnInsertPresentationEnd() 
{
	// TODO: Code für Befehlsbehandlungsroutine hier einfügen
	
   CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
   if (pDoc)
   {
      pDoc->m_curPosMs = pDoc->m_docLengthMs;
      OnInsertPresentation();
   }
}

void CMainFrameE::OnClose() 
{
    SwitchMode();
}

void CMainFrameE::OnNewSlide() 
{
}

void CMainFrameE::OnUpdateNewSlide(CCmdUI *pCmdUI)
{
}

void CMainFrameE::OnStartPreview()
{
    CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
    if (!pDoc || pDoc->project.IsEmpty())
        return;

    if (pDoc->project.FirstQuestionnaireIsRandomTest()
        && pDoc->project.GetFirstQuestionnaireEx()->GetQuestionTotal() > 0) {
        if (pDoc->project.IsRandomTestStarted()) 
            OnPausePreview();
        pDoc->JumpPreview(0);
        pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
    }

    if (!pDoc->IsPreviewActive() || pDoc->IsPausedSpecial()) {
        BeginWaitCursor();

        HRESULT hrReset = pDoc->project.StartQuestionTimers();

        pDoc->StartPreview(GetVideoHWND());
        UpdateWhiteboardView();

        EndWaitCursor();
    }
}

void CMainFrameE::OnPausePreview()
{
   // Note: is called due to the button being clicked AND the end being reached.

   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
   if (!pDoc || pDoc->project.IsEmpty())
      return;

   pDoc->project.SetRandomTestEnabled(true);

	if (pDoc->IsPreviewActive())
	{
		BeginWaitCursor();
		pDoc->PausePreview();

      // restore original state of question objects (ie undo move)
      HRESULT hrReset = pDoc->project.ResetQuestionsAfterPreview();

      StopPausePreviewSpecialTimer();
      //ChangeToNormalPlay(); // is called in pDoc->PausePreview();
      
      // Do not check for real change (resetting question objects):
      // there may be other changes requiring a repaint (ie button appearance)
      //if (SUCCEEDED(hrReset) && hrReset != S_FALSE)
      UpdateWhiteboardView();
	  UpdateStreamView();

		EndWaitCursor();
	}
}

void CMainFrameE::OnStartStopPreview()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
   if (!pDoc || pDoc->project.IsEmpty())
      return;

   pDoc->project.SetRandomTestEnabled(true);

	if (!pDoc->IsPreviewActive() || pDoc->IsPausedSpecial())
	{
        if (pDoc->project.FirstQuestionnaireIsRandomTest()
            && pDoc->project.GetFirstQuestionnaireEx()->GetQuestionTotal() > 0) {
            if (pDoc->project.IsRandomTestStarted()) 
                OnPausePreview();
            pDoc->JumpPreview(0);
            pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
        }

		BeginWaitCursor();

      HRESULT hrReset = pDoc->project.StartQuestionTimers();
      
		pDoc->StartPreview(GetVideoHWND());
	}
   else
   {
		BeginWaitCursor();
		pDoc->PausePreview();

      // restore original state of question objects (ie undo move)
      HRESULT hrReset = pDoc->project.ResetQuestionsAfterPreview();
      
      // Do not check for real change (resetting question objects):
      // there may be other changes requiring a repaint (ie button appearance)
      //if (SUCCEEDED(hrReset) && hrReset != S_FALSE)
   }
   UpdateWhiteboardView();
   EndWaitCursor();
}

void CMainFrameE::OnPreviewSelection()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
   if (!pDoc)
      return;

 
   if (pDoc->IsPreviewActive())
   {
		pDoc->PausePreview();
      return;
   }

   // Start preview selection
   BeginWaitCursor();   

   pDoc->StartPreviewSelection(GetVideoHWND(false));
   EndWaitCursor();
}

LRESULT CMainFrameE::OnHandleAvEditMessage(WPARAM wParam, LPARAM lParam)
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

   switch (wParam)
   {
   case AVEDIT_STREAM_FINISHED:
      {
         // make sure stop marks at the end are found
         OnTimer(TIMER_REPLAY_ID);
         
         bool bIsPausedSpecial = false;
         if (pDoc != NULL)
            bIsPausedSpecial = pDoc->IsPausedSpecial();

         if (!bIsPausedSpecial)
            OnPausePreview(); // stop preview
         // else stop mark at the end
         // TODO maybe reset position to 0?
      }
      break;
   case AVEDIT_STREAMPOSITION:
      {
         // BUGFIX #3118, #3119
         // No messages anymore to be informed of the current replay time.
         // Instead a timer (OnTimer()) here is used.
      }
      break;
   }
   
   return 0;
}

void CMainFrameE::ShowHideView(UINT nPaneId, UINT nId)
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars == NULL)
	{
		TRACE0("Failed to create command bars object.\n");
		return;      // fail to create
	}
	bool bPaneVisible = IsPaneVisible(nPaneId);
	if (bPaneVisible)
      HidePane(nPaneId);
	else
      ShowPane(nPaneId);
	
}

void CMainFrameE::SetViewsToScreengrabbingLayout() {

    CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();

    bool bShowAsSimulation = pDoc->ShowAsScreenSimulation();
    if (bShowAsSimulation) {
        if (pDoc != NULL) {
            double dCurrentZoom = 1.0;
            if (m_pVideoView != NULL)
                dCurrentZoom = m_pVideoView->GetZoom();
            pDoc->SwitchToView(RUNTIME_CLASS(CWhiteboardView));
            m_pWhiteboardView = pDoc->GetWhiteboardView();
            m_pVideoView = pDoc->GetVideoView();
            if (m_pWhiteboardView != NULL)
                m_pWhiteboardView->SetZoomFactor(dCurrentZoom);
        }
    } else {
        if (pDoc != NULL) {
            int iMenuZoom = 100;
            if (m_pWhiteboardView != NULL) {
                iMenuZoom = m_pWhiteboardView->GetMenuZoomFactor();
            }
            pDoc->SwitchToView(RUNTIME_CLASS(CVideoView));
            m_pWhiteboardView = NULL;
            m_pVideoView = pDoc->GetVideoView();
            if (m_pVideoView != NULL) {
                if (iMenuZoom == 50)
                    m_pVideoView->SetZoom(0.5);
                else if (iMenuZoom == 100)
                    m_pVideoView->SetZoom(1.0);
                else if (iMenuZoom == 200)
                    m_pVideoView->SetZoom(2.0);
                else
                    m_pVideoView->SetZoom(100.0);
            }
        }
    }

    if (!m_bViewsAreInStandardLayout)
        return;

    // Save current (standard) layout
    m_pStandardLayout = new CXTPDockingPaneLayout(&m_paneManager);
    m_paneManager.GetLayout(m_pStandardLayout);

    // if there was a former screengrabbing layout use it
    if (m_pScreengrabbingLayout) {
        // Set layout to previous used screengrabbing layout
        m_paneManager.SetLayout(m_pScreengrabbingLayout);

        // The views are detached during SetLayout
        // They have to be attached again
        CXTPDockingPane *pVideoPane = m_paneManager.FindPane(XTP_VIDEO_VIEW); 
        if (pVideoPane != NULL) {
            if (!pVideoPane->IsClosed()) {
                pVideoPane->Close();
                m_bVideoPaneClosedForSgLayout = true;
            }
        }

        CXTPDockingPane *pStructurePane = m_paneManager.FindPane(XTP_STRUCTURE_VIEW);
        if (pStructurePane)
            pStructurePane->Attach(m_pStructureView);

        CXTPDockingPane *pStreamPane = m_paneManager.FindPane(XTP_STREAM_VIEW);
        if (pStreamPane)
            pStreamPane->Attach(m_pStreamView);

        CXTPDockingPane *pPageObjectsPane = m_paneManager.FindPane(XTP_PAGEOBJECTS_VIEW);
        if (pPageObjectsPane)
            pPageObjectsPane->Attach(m_pInteractionStructureView);

        // The screengrabbing layout is no longer needed
        delete m_pScreengrabbingLayout;
        m_pScreengrabbingLayout = NULL;
    }
    // The video view will be moved to the whiteboard position
    else { 
        CXTPDockingPane *pPageObjectsPane = m_paneManager.FindPane(XTP_PAGEOBJECTS_VIEW);
        if (pPageObjectsPane != NULL){
            if (bShowAsSimulation)
                m_paneManager.DockPane(pPageObjectsPane, xtpPaneDockRight);
        }
       

        // In the screengrabbing layout the video view will be displayed in the main view
        CXTPDockingPane *pVideoPane = m_paneManager.FindPane(XTP_VIDEO_VIEW);
        if (pVideoPane && !pVideoPane->IsClosed()) {
            pVideoPane->Close();
            m_bVideoPaneClosedForSgLayout = true;
        }

        CXTPDockingPane *pStructurePane = m_paneManager.FindPane(XTP_STRUCTURE_VIEW);
        if (pStructurePane)
           m_paneManager.DockPane(pStructurePane, xtpPaneDockLeft/*dockLeftOf*/);


        CXTPDockingPane *pStreamPane = m_paneManager.FindPane(XTP_STREAM_VIEW);
        if (pStreamPane)
           m_paneManager.DockPane(pStreamPane, xtpPaneDockBottom);

    }  

    m_bViewsAreInStandardLayout = false;
}

void CMainFrameE::SetViewsToStandardLayout()
{
    if (m_bViewsAreInStandardLayout)
        return;

    CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
    if (pDoc != NULL) {
        pDoc->SwitchToView(RUNTIME_CLASS(CWhiteboardView));
        m_pVideoView = pDoc->GetVideoView();
        m_pWhiteboardView = pDoc->GetWhiteboardView();
    }

    // if there was a former standard layout use it
    if (m_pStandardLayout) {
        // Save current (screengrabbing) layout
        if (m_pScreengrabbingLayout)
            delete m_pScreengrabbingLayout;
        m_pScreengrabbingLayout = new CXTPDockingPaneLayout(&m_paneManager);
        m_paneManager.GetLayout(m_pScreengrabbingLayout);

        // Reset to standard layout
        m_paneManager.SetLayout(m_pStandardLayout);

        // The views are detached during SetLayout
        // They have to be attached again
        CXTPDockingPane *pStructurePane = m_paneManager.FindPane(XTP_STRUCTURE_VIEW);
        if (pStructurePane)
            pStructurePane->Attach(m_pStructureView);

        CXTPDockingPane *pVideoPane = m_paneManager.FindPane(XTP_VIDEO_VIEW);
        if (pVideoPane) {
            // There should be no video pane visible if the document is a pure Screen grabbing
            if (IsScreengrabbingDocument()) {
                if (!pVideoPane->IsClosed()) {
                    pVideoPane->Close();
                    m_bVideoPaneClosedForSgLayout = true;
                }
            } else {
                if (m_bVideoPaneClosedForSgLayout)
                    pVideoPane->Select();
                if (pDoc != NULL)
                    m_pVideoView = pDoc->GetVideoView();
                pVideoPane->Attach(m_pVideoView);
                m_bVideoPaneClosedForSgLayout = false;
            }
        }

        CXTPDockingPane *pStreamPane = m_paneManager.FindPane(XTP_STREAM_VIEW);
        if (pStreamPane)
            pStreamPane->Attach(m_pStreamView);

        CXTPDockingPane *pPageObjectsPane = m_paneManager.FindPane(XTP_PAGEOBJECTS_VIEW);
        if (pPageObjectsPane)
            pPageObjectsPane->Attach(m_pInteractionStructureView);

        // The standard layout is no longer needed
        delete m_pStandardLayout;
        m_pStandardLayout = NULL;
    } 

    m_bViewsAreInStandardLayout = true;
}

void CMainFrameE::ShowHideInteractionTools(BOOL bShow)
{
    CArray<int, int> arCommands;
    // tools which should be visibe/unvisible
    arCommands.Add(ID_INTERACTION_BUTTON);
    arCommands.Add(ID_INTERACTION_AREA);
    arCommands.Add(ID_ADD_QUESTION_DD);
    arCommands.Add(ID_ADD_QUESTION_MC);
    arCommands.Add(ID_ADD_QUESTION_FB);

    CXTPCommandBars *pCommandBars = GetCommandBars();

    CXTPToolBar *pStandardBar = NULL;
    if (pCommandBars != NULL)
        pStandardBar = (CXTPToolBar*)pCommandBars->GetToolBar(IDR_STANDARD);

    // Search coresponding buttons in toolbar and set visibility mode
    CXTPControl *pControl;
    if (pStandardBar) {
        for (int i = 0; i < arCommands.GetSize(); ++i) {
            pControl = 
                (CXTPControl *)pStandardBar->GetControls()->FindControl(arCommands[i]);
            if (pControl)
                pControl->SetVisible(bShow);
        }

        // reposition the edit tool bar, if it is docked to the standard bar
        CXTPToolBar *pEditBar = NULL;
        pEditBar = (CXTPToolBar*)pCommandBars->GetToolBar(IDR_EDIT);
        if (pEditBar) {
            CRect rcStandard;
            pStandardBar->GetWindowRect(&rcStandard);
            CRect rcEditBar;
            pEditBar->GetWindowRect(&rcEditBar);

            // if the right border of the standard bar is equal the
            // left border of the edit bar, the bars are docked.
            if (rcEditBar.top == rcStandard.top && rcEditBar.left == rcStandard.right) {
                DockRightOf(pEditBar, pStandardBar);
            }
        }
    }

    /*
    CXTPCommandBar *pMenuBar = NULL;
    if (pCommandBars) {
        pMenuBar = pCommandBars->GetMenuBar();
    }

    if (pMenuBar) {
        CXTPControl *pInsertControl = (CXTPControl *)pMenuBar->GetControls()->GetAt(2);
        if (pInsertControl && pInsertControl->GetType() == xtpControlPopup) {
            CXTPCommandBar *pInsertMenu = ((CXTPControlPopup *)pInsertControl)->GetCommandBar();
            if (pInsertMenu) {
                for (int i = 0; i < arCommands.GetSize(); ++i) {
                    pControl = 
                        pInsertMenu->GetControls()->FindControl(arCommands[i]);
                    if (pControl)
                        pControl->SetVisible(bShow);
                }
            }
        }
    }
    */

}

void CMainFrameE::ShowHideEditTools(BOOL bShow)
{
    CArray<int, int> arCommands;
    // tools which should be visibe/unvisible
    arCommands.Add(IDC_FILL_COLOR);
    arCommands.Add(IDC_LINE_COLOR);
    arCommands.Add(IDC_TEXT_COLOR);
    arCommands.Add(IDC_LINE_WIDTH);
    arCommands.Add(IDC_LINE_STYLE);
    arCommands.Add(IDC_FONT_FAMILY);
    arCommands.Add(IDC_FONT_SIZE);
    arCommands.Add(IDC_FONT_WEIGHT);
    arCommands.Add(IDC_FONT_SLANT);

    CXTPCommandBars *pCommandBars = GetCommandBars();

    CXTPToolBar *pToolBar = NULL;
    if (pCommandBars != NULL)
        pToolBar = (CXTPToolBar*)pCommandBars->GetToolBar(IDR_EDIT);

    // Search coresponding buttons in toolbar and set visibility mode
    CXTPControl *pControl;
    if (pToolBar) {
        for (int i = 0; i < arCommands.GetSize(); ++i) {
            pControl = 
                (CXTPControl *)pToolBar->GetControls()->FindControl(arCommands[i]);
            if (pControl)
                pControl->SetVisible(bShow);
        }
    }
}

bool CMainFrameE::IsPaneVisible(UINT nPaneId)
{
	CXTPDockingPane *pPane = m_paneManager.FindPane(nPaneId);
	if (pPane && !pPane->IsClosed())
		return true;
	else
		return false;
}

void CMainFrameE::ShowPane(UINT nPaneId)
{
	CXTPDockingPane *pPane = m_paneManager.FindPane(nPaneId);
   if (pPane && pPane->IsClosed())
		m_paneManager.ShowPane(nPaneId);
}

void CMainFrameE::HidePane(UINT nPaneId)
{
	CXTPDockingPane *pPane = m_paneManager.FindPane(nPaneId);
   if (pPane && !pPane->IsClosed())
      pPane->Close();
}

bool CMainFrameE::PreviewInNormalMode(bool bForceMessage) {
    CEditorDoc *pDoc = CMainFrameE::GetEditorDocument();
    if (((pDoc && pDoc->IsPreviewActive()) || bForceMessage)
        && pDoc->project.FirstQuestionnaireIsRandomTest()
        && pDoc->project.GetFirstQuestionnaireEx()->GetQuestionTotal() > 0) {
        CString csQuestion;
        csQuestion.LoadString(IDS_STOP_JUMP_FOR_RANDOM_TEST);
        if (MessageBox(csQuestion, NULL, MB_YESNO | MB_ICONQUESTION) == IDNO) {
            pDoc->project.SetRandomTestEnabled(false);
            return true;
        }
        return false;
    }
    return true;
}

void CMainFrameE::OnJumpPos1()
{
    GET_DOC(pDoc);

    if (!PreviewInNormalMode())
        return;

    pDoc->JumpPreview(0);

    pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
}

void CMainFrameE::OnJumpEnd()
{
    GET_DOC(pDoc);

    if (!PreviewInNormalMode()) {
        return;
    }

    pDoc->PausePreview();

    pDoc->JumpPreview(pDoc->m_docLengthMs);

    pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
}

void CMainFrameE::OnJumpForward()
{
    GET_DOC(pDoc);

    if (!PreviewInNormalMode()) {
        return;
    }

    pDoc->JumpPreview(pDoc->project.GetNextPageBegin(pDoc->m_curPosMs));

    pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
}

void CMainFrameE::OnJumpBack()
{
    GET_DOC(pDoc);

    if (!PreviewInNormalMode()) {
        return;
    }

    pDoc->JumpPreview(pDoc->project.GetPrevPageBegin(pDoc->m_curPosMs));

    pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
}

void CMainFrameE::OnSetLeftMark()
{
   GET_DOC(pDoc);

   pDoc->m_markStartMs = pDoc->m_curPosMs;
   if (pDoc->m_markEndMs != -1)
   {
      // If the end marker is to the left of the
      // start marker, reset the end marker
      if (pDoc->m_markEndMs <= pDoc->m_markStartMs)
         pDoc->m_markEndMs = -1;
   }

   pDoc->UpdateAllViews(NULL, HINT_CHANGE_SELECTION);
}

void CMainFrameE::OnSetRightMark()
{
   GET_DOC(pDoc);

   pDoc->m_markEndMs = pDoc->m_curPosMs;
   if (pDoc->m_markStartMs != -1)
   {
      if (pDoc->m_markEndMs <= pDoc->m_markStartMs)
         pDoc->m_markStartMs = -1;
   }

   pDoc->UpdateAllViews(NULL, HINT_CHANGE_SELECTION);
}

void CMainFrameE::OnJumpMarkStart()
{
   GET_DOC(pDoc);

   if (!PreviewInNormalMode()) {
       return;
   }

   pDoc->JumpPreview(pDoc->m_markStartMs);

   pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
}

void CMainFrameE::OnJumpMarkEnd()
{
   GET_DOC(pDoc);

   if (!PreviewInNormalMode()) {
       return;
   }

   pDoc->JumpPreview(pDoc->m_markEndMs);

   pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
}

void CMainFrameE::OnMarkAll()
{
   GET_DOC(pDoc);

   pDoc->m_markStartMs = 0;
   pDoc->m_markEndMs   = pDoc->m_docLengthMs;

   pDoc->UpdateAllViews(NULL, HINT_CHANGE_SELECTION);
}

void CMainFrameE::OnMarkNone()
{
   GET_DOC(pDoc);

   pDoc->m_markStartMs = -1;
   pDoc->m_markEndMs   = -1;

   pDoc->UpdateAllViews(NULL, HINT_CHANGE_SELECTION);
}

void CMainFrameE::OnOpenWebpage()
{
   ::ShellExecute(
      NULL,
      _T("open"),
      _T("http://www.im-c.de"),
      NULL,
      NULL,
      SW_SHOWNORMAL);
}

void CMainFrameE::OnExportAudio()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

   if (!pDoc->project.m_isAudioFormatInitialized)
   {
      OnExportSettings();
   }

   if (!pDoc->project.m_isAudioFormatInitialized)
      return;

   CString filter;
   filter.LoadString(IDS_WAV_FILEFILTER);

   CString fileName;
   FileDialog::GetSaveFilename(AfxGetApp(), fileName, AfxGetMainWnd()->GetSafeHwnd(), 
      AfxGetInstanceHandle(), filter, _T(".wav"), _T("WAVEXPORT"), GetInitialExportDir());


   if (!fileName.IsEmpty())
   {
      // The export will tear down all render engines,
      // so they will have to be rebuilt after the export
      pDoc->SetPreviewRebuildNeeded(TRUE);

      //pDoc->project.Export(szFileName);
      //MessageBox("Export audio");
      CString file = fileName;
      CString path = fileName;
      StringManipulation::GetFilename(file);
      StringManipulation::GetPath(path);

      pDoc->project.ExportAudioAsWav(path, file);

      // Now rebuild the preview:
      pDoc->BuildPreview(GetVideoHWND());
   }
}

void CMainFrameE::OnImportAudio()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

   CString filter;
   filter.LoadString(IDS_WAV_FILEFILTER);

   CString fileName;
   FileDialog::GetOpenFilename(AfxGetApp(), fileName, AfxGetMainWnd()->GetSafeHwnd(), 
      AfxGetInstanceHandle(), filter, _T("AUDIOIMPORT"), GetInitialOpenDir());

   if (!fileName.IsEmpty())
   {
      BeginWaitCursor();
      pDoc->ImportAudio(fileName);
      EndWaitCursor();
   }
}

void CMainFrameE::OnImportVideo()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

   CString filter;
   filter.LoadString(IDS_AVI_FILEFILTER);

   CString fileName;
   FileDialog::GetOpenFilename(AfxGetApp(), fileName, AfxGetMainWnd()->GetSafeHwnd(), 
      AfxGetInstanceHandle(), filter, _T("VIDEOIMPORT"), GetInitialOpenDir());

   if (!fileName.IsEmpty())
   {
      BeginWaitCursor();
      pDoc->ImportVideo(fileName);
      // Assert that all streams are displayed after importing
      // a new video file
      m_pStreamView->UpdateStreamVisibilities(pDoc);
      EndWaitCursor();
   }
}

void CMainFrameE::OnRemoveVideo()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

   pDoc->RemoveVideo();
}

void CMainFrameE::OnClipInsert() 
{
   CString filter;
   filter.LoadString(IDS_AVI_FILEFILTER);

   CString fileName;
   FileDialog::GetOpenFilename(AfxGetApp(), fileName, AfxGetMainWnd()->GetSafeHwnd(), 
                               AfxGetInstanceHandle(), filter, _T("CLIPIMPORT"), GetInitialOpenDir());

   if (!fileName.IsEmpty())
   {
      CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
   
      pDoc->InsertClip(fileName);
      // Assert that all streams are displayed after inserting
      // a new clip
      m_pStreamView->UpdateStreamVisibilities(pDoc);
   }
}

void CMainFrameE::OnClipInsertFromLrd() {
    CString filter;
    filter.LoadString(IDS_LRD_FILEFILTER);

    CString fileName;
    FileDialog::GetOpenFilename(AfxGetApp(), fileName, AfxGetMainWnd()->GetSafeHwnd(), 
        AfxGetInstanceHandle(), filter, _T("LRDIMPORT"), GetInitialOpenDir());

    if (!fileName.IsEmpty()) {
        CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

        pDoc->InsertClipFromLrd(fileName);
        // Assert that all streams are displayed after inserting
        // a new clip
        m_pStreamView->UpdateStreamVisibilities(pDoc);
    }
}

void CMainFrameE::OnInsertStillImage() {
    CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

    if (pDoc->project.HasVideo() || pDoc->project.IsDenverDocument()) {
        CString msg;
        msg.LoadString(IDS_MSG_STILLIMAGEERROR);
        CString cap;
        cap.LoadString(IDS_INFORMATION);

        MessageBox(msg, cap, MB_OK | MB_ICONINFORMATION);
        return;
    }

    CString filter;
    filter.LoadString(IDS_IMAGE_FILTER_E);

    CString fileName;
    FileDialog::GetOpenFilename(AfxGetApp(), fileName, AfxGetMainWnd()->GetSafeHwnd(), 
        AfxGetInstanceHandle(), filter, _T("STILLIMAGE"), GetInitialOpenDir());

    if (!fileName.IsEmpty()) {
        pDoc->InsertStillImage(fileName);
    }
}

void CMainFrameE::OnClipDelete() {
    CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
    pDoc->DeleteCurrentClip();
}

void CMainFrameE::OnRemoveStillImage() {
    CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
    pDoc->RemoveStillImage();
}

void CMainFrameE::OnThumbBig() {
    CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
    if (pDoc) {
        BeginWaitCursor();
        if (pDoc->project.SetThumbWidth(CEditorProject::THUMB_BIG) && m_pStructureView) {
            m_pStructureView->UpdateThumbnailSize();
        }
        EndWaitCursor();
    }
}

void CMainFrameE::OnThumbMiddle() {
    CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

    if (pDoc) {
        BeginWaitCursor();
        if (pDoc->project.SetThumbWidth(CEditorProject::THUMB_MIDDLE) && m_pStructureView) {
            m_pStructureView->UpdateThumbnailSize();
        }
        EndWaitCursor();
    }
}

void CMainFrameE::OnThumbSmall() {
    CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

    if (pDoc) {
        BeginWaitCursor();
        if (pDoc->project.SetThumbWidth(CEditorProject::THUMB_SMALL) && m_pStructureView) {
            m_pStructureView->UpdateThumbnailSize();
        }
        EndWaitCursor();
    }
}

void CMainFrameE::OnExportSettings() {
    DisplayExportSettings();
}

void CMainFrameE::DisplayExportSettings(int nPage, const _TCHAR *tszClipFile) {
    GET_DOC(pDoc);
    int nCurPos = pDoc->m_curPosMs;
    int nSelectPage = 0;
    switch (nPage) {
    case 0:
        break;
    case 1:
        // Audio page
        if (pDoc->project.HasVideo())
            nSelectPage = 1;
        else
            nSelectPage = 0;
        break;
    case 2:
        // Metadata page
        if (pDoc->project.HasVideo())
            nSelectPage = 2;
        else
            nSelectPage = 1;
        break;
    case 3:
        if (pDoc->project.IsDenverDocument()) {
            nSelectPage = 2;
        }
        break;
    case 4:
        // Clips codecs
        if (pDoc->project.HasVideo()) {
            if (pDoc->project.HasSgClips() && !pDoc->project.IsDenverDocument())
                nSelectPage = 3;
        } else {
            if (pDoc->project.HasSgClips() && !pDoc->project.IsDenverDocument())
                nSelectPage = 2;
        }
        break;
    }
    CExportSettings propSheet(IDS_EXPORT_SETTINGS, this, nSelectPage, tszClipFile);

    propSheet.DoModal();

    if (pDoc->IsPreviewRebuildNeeded())
        pDoc->RebuildPreview();
    pDoc->JumpPreview(nCurPos);
    pDoc->UpdateAllViews(NULL);
}

void CMainFrameE::OnHelp() {

    HRESULT hr = CLanguageSupport::StartManual();
    if (hr != S_OK) {
        CString msg;
        msg.Format(IDS_MSG_OPEN_PDF_FAILED_E, _T("manual.pdf"));
        CString cap;
        cap.LoadString(IDS_ERROR_E);
        MessageBox(msg, cap, MB_OK | MB_ICONERROR);
    }
}

void CMainFrameE::OnExploreLecturnityHome() {
    Explore::ExploreLecturnityHome();
}

void CMainFrameE::OnUpdateExploreLecturnityHome(CCmdUI* pCmdUI) {
    BOOL bEnable = TRUE;

    if (CStudioApp::IsPowerTrainerMode())
        bEnable = FALSE;

    pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnMarkCurrentClip() {
    GET_DOC(pDoc);
    pDoc->MarkClipAt(pDoc->m_curPosMs);
}

void CMainFrameE::OnMarkCurrentPage() {
    GET_DOC(pDoc);
    pDoc->MarkPageAt(pDoc->m_curPosMs);
}

void CMainFrameE::OnChangeCurrentClipTitle() {
    GET_DOC(pDoc);
    pDoc->project.ChangeClipTitleAt(pDoc->m_curPosMs);
}

void CMainFrameE::OnChangeCurrentPageTitle() {
    GET_DOC(pDoc);
    pDoc->project.ChangePageTitleAt(pDoc->m_curPosMs);
}

void CMainFrameE::OnDisplayCurrentClipExportSettings() {
    GET_DOC(pDoc);
    CClipInfo *pClip = pDoc->project.FindClipAt(pDoc->m_curPosMs);
    if (pClip) {
        DisplayExportSettings(4, pClip->GetFilename());
    }
}

void CMainFrameE::OnInsertStopmark() {
    if(!m_pStreamView){
        ASSERT(FALSE);
        return;
    }
    GET_DOC(pDoc);
    bool bMarksStreamExists = true;
    if (!pDoc->project.GetMarkStream())
        bMarksStreamExists = false;

    int iCheckID = IDC_CURRENT_SLIDE_END;
    int iTimestamp = pDoc->m_curPosMs;
    m_pStreamView->InsertStopmark(iCheckID, iTimestamp);

    if (!bMarksStreamExists)
        pDoc->UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
}

void CMainFrameE::OnInsertTargetmark() {
    if(!m_pStreamView){
        ASSERT(FALSE);
        return;
    }
    GET_DOC(pDoc);
    bool bMarksStreamExists = true;
    CMarkStream *pMarkStream = pDoc->project.GetMarkStream();
    if (!pMarkStream)
        bMarksStreamExists = false;

    int iCheckID = IDC_CURRENT_SLIDE_BEGIN;

    UINT nNextTargetCount = 1;
    if (pMarkStream)
        nNextTargetCount = pMarkStream->GetJumpMarkCounter() + 1;
    CString csTargetmarkName;
    csTargetmarkName.Format(IDS_TARGET, nNextTargetCount);

    int iTimestamp = pDoc->m_curPosMs;
    m_pStreamView->InsertTargetmark(csTargetmarkName, iCheckID, iTimestamp);
    m_pStreamView->UpdateStreamVisibilities(pDoc);

    if (!bMarksStreamExists)
        pDoc->UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
}

void CMainFrameE::OnShowStructure() {
    ShowHideView(XTP_STRUCTURE_VIEW, IDC_SHOW_STRUCTURE);
}

void CMainFrameE::OnShowVideo() {
    ShowHideView(XTP_VIDEO_VIEW, IDC_SHOW_VIDEO);
}

void CMainFrameE::OnShowStreams() {
    ShowHideView(XTP_STREAM_VIEW, IDC_SHOW_STREAMS);
}

void CMainFrameE::OnShowPageobjects() 
{
	ShowHideView(XTP_PAGEOBJECTS_VIEW, IDC_SHOW_PAGEOBJECTS);
}

void CMainFrameE::OnAddButton()
{
}

LRESULT CMainFrameE::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
        CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
		CString csTitle;
		if (!pPane->IsValid() && pDoc)
		{
			switch (pPane->GetID())
			{
			case XTP_STRUCTURE_VIEW:  	
				if (!m_pStructureView)
				{
					m_pStructureView = (CStructureView *)pPane->AttachView(this, RUNTIME_CLASS(CStructureView), pDoc);
					csTitle.LoadString(IDS_TITLE_PAGES);
					pPane->SetTitle(csTitle);
				}
				break;
			case XTP_VIDEO_VIEW:
				if (!m_pVideoView)
				{
					m_pVideoView = 
						(CVideoView *)pPane->AttachView(this, RUNTIME_CLASS(CVideoView), pDoc);
					csTitle.LoadString(IDS_TITLE_VIDEO);
					pPane->SetTitle(csTitle);
				}
				break;
			case XTP_STREAM_VIEW:
				if (!m_pStreamView)
				{
					m_pStreamView = 
						(CStreamView *)pPane->AttachView(this, RUNTIME_CLASS(CStreamView), pDoc);
					csTitle.LoadString(IDS_TITLE_STREAMS);
					pPane->SetTitle(csTitle);
				}
				break;
			case XTP_PAGEOBJECTS_VIEW:
				if (!m_pInteractionStructureView)
				{
					m_pInteractionStructureView= 
                  (CInteractionStructureView *)pPane->AttachView(this, RUNTIME_CLASS(CInteractionStructureView), pDoc);
					csTitle.LoadString(IDS_TITLE_PAGEOBJECTS);
					pPane->SetTitle(csTitle);
				}

				break;
			}
		}
		return TRUE;
	}

	return FALSE;
}


//
// CCmdUI update methods
//


bool CMainFrameE::ProjectHasData()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

   return pDoc != NULL && !pDoc->project.IsEmpty();

}

bool CMainFrameE::ProjectHasDataAndNotPreviewing()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

   return pDoc != NULL && !pDoc->project.IsEmpty() && !pDoc->IsPreviewActive();
}

bool CMainFrameE::IsScreengrabbingDocument()
{
   CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();

   return pDoc != NULL && pDoc->IsScreenGrabbingDocument();
}

void CMainFrameE::OnUpdateStartPreview(CCmdUI *pCmdUI) {
    GET_DOC(pDoc);
    bool bEnable = false;
    if (pDoc == NULL || pDoc->project.IsEmpty() == true) {
        bEnable = false;
    } else {
        // BUGFIX 4835: method handles IDC_STARTSTOPPREVIEW and IDC_STARTPREVIEW
        // but enabled is valid for IDC_STARTSTOPPREVIEW only. Change of icons is done when executing pause/play/stop etc
        bEnable = ProjectHasDataAndNotPreviewing() || pDoc->IsPausedSpecial() || pCmdUI->m_nID == IDC_STARTSTOPPREVIEW;
    }
    pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdatePausePreview(CCmdUI *pCmdUI) {
    GET_DOC(pDoc);
    pCmdUI->Enable(ProjectHasData() && pDoc->IsPreviewActive());
}

void CMainFrameE::StartPausePreviewSpecialTimer() {
    if (m_nTimer == 0) {
        m_bIsDeactivated = false;
        m_nTimer = SetTimer(TIMER_ICON_ID, 500, 0);
    }
}

void CMainFrameE::StopPausePreviewSpecialTimer() {
    if (m_nTimer > 0) {
        KillTimer(m_nTimer);   
        CXTPImageManager *pImageManager = XTPImageManager();
        if (pImageManager) {
            pImageManager->SetIcons(IDR_SYMBOLBAR, IDR_SYMBOLBAR);
        }
    }
    m_nTimer = 0;
}

void CMainFrameE::OnUpdateJumpButtons(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(ProjectHasData());
}

void CMainFrameE::OnUpdateSelectionButtons(CCmdUI *pCmdUI)
{
  bool bEnable = ProjectHasDataAndNotPreviewing();

   // Selection buttons are not possible in LiveContext preview mode
  if (CStudioApp::IsLiveContextMode() && CStudioApp::IsLiveContextPreview())
      bEnable = false;

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdatePreviewSelection(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = false;
      
   // TODO what is this?
   
   if (ProjectHasDataAndNotPreviewing()) //(!pDoc->project.IsEmpty())
   {
      if (pDoc->m_markStartMs != -1 && pDoc->m_markEndMs != -1)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateCutCopy(CCmdUI *pCmdUI) {
    GET_DOC(pDoc);
    bool bEnable = false;

    if (ProjectHasDataAndNotPreviewing()) {
        if (m_pWhiteboardView != NULL &&  m_pWhiteboardView->IsActive())
            bEnable = true;
        else if (pDoc->m_markStartMs != -1 && pDoc->m_markEndMs != -1)
            bEnable = true;
    }

    pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdatePaste(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = false;
   
   if (ProjectHasDataAndNotPreviewing())
   {	
      if (m_pWhiteboardView != NULL && m_bLastCopyToWb)
      {
         if (!m_pWhiteboardView->IsClipboardEmpty())
            bEnable = true;
      }
      else if (m_pActiveView && m_pStreamView == m_pActiveView)
      {
         if (pDoc->project.GetClipBoardSize() > 0)
            bEnable = true;
      }
   }

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateJumpMark(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = false;

   if (ProjectHasData())
   {
      if (pDoc->m_markStartMs != -1 && pDoc->m_markEndMs != -1)
         bEnable = true;
   }
   
   // Jump marks are not possible in LiveContext preview mode
   if (CStudioApp::IsLiveContextMode() && CStudioApp::IsLiveContextPreview())
      bEnable = false;

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateZoomAll(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = false;
   
   if (ProjectHasData())
   {
      if (!(pDoc->m_displayStartMs <= 0 && pDoc->m_displayEndMs >= pDoc->m_docLengthMs))
         bEnable = true;
   }

   if ( pCmdUI != NULL )
      pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateZoomSelected(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = false;
   
   if (ProjectHasData())
   {
      if ((pDoc->m_markStartMs != -1 && pDoc->m_markEndMs != -1) 
          && (pDoc->m_displayEndMs - pDoc->m_displayStartMs > MIN_DISPLAY_SEC)
          && !(pDoc->m_markStartMs == pDoc->m_displayStartMs && pDoc->m_markEndMs == pDoc->m_displayEndMs) )
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateUndo(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   pCmdUI->Enable(ProjectHasDataAndNotPreviewing() && pDoc->project.IsUndoPossible());
}

void CMainFrameE::OnUpdateRedo(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   pCmdUI->Enable(ProjectHasDataAndNotPreviewing() && pDoc->project.IsRedoPossible());
}

void CMainFrameE::OnUpdateProjectCommand(CCmdUI* pCmdUI) 
{
   // TODO what is this? 
   pCmdUI->Enable(ProjectHasDataAndNotPreviewing());
}
	
void CMainFrameE::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
   BOOL bEnable = ProjectHasDataAndNotPreviewing();

   // Save is not possible in LiveContext preview mode
   if (CStudioApp::IsLiveContextMode() && CStudioApp::IsLiveContextPreview())
      bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
    BOOL bEnable = TRUE;

    // SaveAs is not possible in PowerTrainer mode
    if (CStudioApp::IsPowerTrainerMode())
        bEnable = FALSE;

    // SaveAs is not possible if there is no data and
    // during preview
    if (!ProjectHasDataAndNotPreviewing())
        bEnable = FALSE;

   // SaveAs is not possible in LiveContext mode
   if (CStudioApp::IsLiveContextMode())
      bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}
	
void CMainFrameE::OnUpdateExportSettings(CCmdUI* pCmdUI) 
{
   // ExportSettings are not possible in LiveContext preview mode
   pCmdUI->Enable(!(CStudioApp::IsLiveContextMode() && CStudioApp::IsLiveContextPreview()));
}
	
void CMainFrameE::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = !pDoc->IsPreviewActive();

   // FileOpen is not possible in LiveContext mode
   if (CStudioApp::IsLiveContextMode())
      bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateMenuAudio(CCmdUI *pCmdUI)
{
   // Menu Audio is not possible in LiveContext preview mode
   pCmdUI->Enable(!(CStudioApp::IsLiveContextMode() && CStudioApp::IsLiveContextPreview()));
}

void CMainFrameE::OnUpdateMenuVideo(CCmdUI *pCmdUI)
{
   // Menu Video is not possible in LiveContext mode
   pCmdUI->Enable(!CStudioApp::IsLiveContextMode());
}

void CMainFrameE::OnUpdateFileImport(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(!ProjectHasData());
}

void CMainFrameE::OnUpdateCreateSgStructure(CCmdUI* pCmdUI) 
{
    GET_DOC(pDoc);

    BOOL bEnable = FALSE;

    if (pDoc)
    {
        bEnable = ProjectHasDataAndNotPreviewing() &&
                  pDoc->project.IsDenverDocument() && 
                  !pDoc->project.HasStructuredSgClips();
    }

    pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateInsertPresentation(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(ProjectHasDataAndNotPreviewing() && !IsScreengrabbingDocument());
}

void CMainFrameE::OnUpdateClipInsert(CCmdUI* pCmdUI) 
{
   GET_DOC(pDoc);
   bool bEnable = false;

   if (ProjectHasDataAndNotPreviewing() && !IsScreengrabbingDocument())
   {
      if (pDoc->m_curPosMs < pDoc->m_docLengthMs) 
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateClipDelete(CCmdUI* pCmdUI) 
{
   GET_DOC(pDoc);
   bool bEnable = false;

   if (ProjectHasDataAndNotPreviewing() && !IsScreengrabbingDocument())
   {
      if (pDoc->IsClipActive())
         bEnable = true;
   }

   // ClipMark/ClipDelete is not possible in LiveContext edit mode
   if (CStudioApp::IsLiveContextMode() && !CStudioApp::IsLiveContextPreview())
      bEnable = false;

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateClipChange(CCmdUI* pCmdUI) 
{
   GET_DOC(pDoc);
   bool bEnable = false;

   if (ProjectHasDataAndNotPreviewing())
   {
      if (pDoc->IsClipActive())
         bEnable = true;
   }

   // ClipMark/ClipDelete is not possible in LiveContext edit mode
   if (CStudioApp::IsLiveContextMode() && !CStudioApp::IsLiveContextPreview())
      bEnable = false;

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateImportVideo(CCmdUI* pCmdUI) {
    GET_DOC(pDoc);

    BOOL bEnable = FALSE;
    if (ProjectHasDataAndNotPreviewing() && !IsScreengrabbingDocument())
        bEnable = TRUE;

    // ImportVideo is not possible in LiveContext edit mode
    if (CStudioApp::IsLiveContextMode() && !CStudioApp::IsLiveContextPreview())
        bEnable = FALSE;

    pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateRemoveVideo(CCmdUI* pCmdUI)
{
   GET_DOC(pDoc);
   pCmdUI->Enable(ProjectHasDataAndNotPreviewing() && pDoc->project.HasVideo());
}

void CMainFrameE::OnUpdateInsertStillImage(CCmdUI* pCmdUI)
{
   bool bEnable = ProjectHasDataAndNotPreviewing();

   // InsertStillImage is not possible in LiveContext edit mode
   if (CStudioApp::IsLiveContextMode() && !CStudioApp::IsLiveContextPreview())
      bEnable = false;

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateRemoveStillImage(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   pCmdUI->Enable(ProjectHasDataAndNotPreviewing() && pDoc->project.HasStillImage());
}
void CMainFrameE::OnUpdateRemoveClip(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = false;
   
   if (ProjectHasDataAndNotPreviewing() && !IsScreengrabbingDocument()) {
      if (pDoc->project.HasSgClips())
         bEnable = true;
      if (pDoc->IsClipActive())
         bEnable = true;
   }

   // RemoveClip is not possible in LiveContext edit mode
   if (CStudioApp::IsLiveContextMode() && !CStudioApp::IsLiveContextPreview())
      bEnable = false;

   pCmdUI->Enable(bEnable);
}
void CMainFrameE::OnUpdateThumbDimension(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = false;

   int check = 0;
   if (ProjectHasData() && pDoc->project.HasStructure())
   {
      bEnable = true;
   
      double dimension = pDoc->project.GetStructureWidth();
      switch (pCmdUI->m_nID)
      {
      case ID_VIEW_THUMB_BIG:
         if (dimension == CEditorProject::THUMB_BIG) 
            check = 1; 
         break;
      case ID_VIEW_THUMB_MIDDLE:
         if (dimension == CEditorProject::THUMB_MIDDLE) 
            check = 1; 
         break;
      case ID_VIEW_THUMB_SMALL:
         if (dimension == CEditorProject::THUMB_SMALL) 
            check = 1; 
         break;
      }
   }

   pCmdUI->SetRadio(check);
   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateHelp(CCmdUI *pCmdUI)
{
   if (!m_bHasCheckedForManual)
   {
      m_bHasManual = CLanguageSupport::ManualExists();
      m_bHasCheckedForManual = true;
   }

   pCmdUI->Enable(m_bHasManual ? TRUE : FALSE);
}

void CMainFrameE::OnUpdateMarkPage(CCmdUI *pCmdUI)
{
   GET_DOC(pDoc);
   bool bEnable = false;

   if (ProjectHasDataAndNotPreviewing())
   {
       if (!pDoc->project.IsDenverDocument() && pDoc->project.FindPageAt(pDoc->m_curPosMs) != NULL)
          bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnUpdateInsertStopmark(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(ProjectHasDataAndNotPreviewing() && !IsScreengrabbingDocument());

    //Be aware that if we are not in Screen simulation we must enter in this mode before executing command.
    pCmdUI->Enable(ProjectHasDataAndNotPreviewing());
}

void CMainFrameE::OnUpdateInsertTargetmark(CCmdUI* pCmdUI) 
{ 
	//pCmdUI->Enable(ProjectHasDataAndNotPreviewing() && !IsScreengrabbingDocument());

    //Be aware that if we are not in Screen simulation we must enter in this mode before executing command.
    pCmdUI->Enable(ProjectHasDataAndNotPreviewing());
}

void CMainFrameE::OnUpdateShowStructure(CCmdUI* pCmdUI) 
{ 
	CXTPDockingPane *pPane = m_paneManager.FindPane(XTP_STRUCTURE_VIEW);
	pCmdUI->SetCheck(pPane && !pPane->IsClosed());
}

void CMainFrameE::OnUpdateShowVideo(CCmdUI* pCmdUI) 
{ 
	CXTPDockingPane *pPane = m_paneManager.FindPane(XTP_VIDEO_VIEW);
	pCmdUI->SetCheck(pPane && !pPane->IsClosed());
}

void CMainFrameE::OnUpdateShowStreams(CCmdUI* pCmdUI) 
{ 
	CXTPDockingPane *pPane = m_paneManager.FindPane(XTP_STREAM_VIEW);
	pCmdUI->SetCheck(pPane && !pPane->IsClosed());
}

void CMainFrameE::OnUpdateShowPageobjects(CCmdUI* pCmdUI) 
{ 
    int iChecked = 0;
    CXTPDockingPane *pPane = m_paneManager.FindPane(XTP_PAGEOBJECTS_VIEW);
    if (pPane && !pPane->IsClosed())
        iChecked = 1;
    pCmdUI->SetCheck(iChecked);

}

void CMainFrameE::OnUpdateStatusPositionTotalLenght(CCmdUI* pCmdUI) {
    GET_DOC(pDoc);
    CString csWindowText;
    CString csTotalTime;

    // find pane
    CXTPStatusBarPane* pPane = NULL;
    CMainFrame *pStudioMainFrame = GetStudioMainFrame();
    if (pStudioMainFrame) {
        CXTPStatusBar *pStatusBar = pStudioMainFrame->GetStatusBar();
        if (pStatusBar) {
            pPane = pStatusBar->FindPane(pCmdUI->m_nID);
        }
    }

    if (ProjectHasData()) {
        if (!m_csSpecialStatusText.IsEmpty()) {
            // special case a special status text should be displayed?

            if (timeGetTime() < m_nTimedStatusTimeout)
                csWindowText = m_csSpecialStatusText;
            else
                m_csSpecialStatusText.Empty();
        }

        if (csWindowText.IsEmpty()) {
            // normal case: no special status text defined or time up
            Misc::FormatTickString(csTotalTime, pDoc->m_docLengthMs, pDoc->m_streamUnit, true);
            csWindowText.Format(ID_STATUSBAR_LENGHT, csTotalTime); 
            if (pPane != NULL) {
                pPane->SetEnabled(TRUE);
            }
        }
    } else {
        Misc::FormatTickString(csTotalTime, pDoc->m_docLengthMs, pDoc->m_streamUnit, true);
        csWindowText.Format(ID_STATUSBAR_LENGHT, csTotalTime);
        if (pPane != NULL) {
            pPane->SetEnabled(FALSE);
        }
    }	
    pCmdUI->SetText(csWindowText);
}

void CMainFrameE::OnUpdateStatusPositionCurrentTime(CCmdUI* pCmdUI)
{
   GET_DOC(pDoc);

   CString csWindowText;
   CString csCurrentTime;
#ifdef _EDITOR  
   CXTPStatusBarPane* pPane =m_wndStatusBar.FindPane(pCmdUI->m_nID);
   // relevant only for Editor stand-alone application 
   if(pPane==NULL)
   {
      return;
   }
#endif
   if (ProjectHasData())
   {
      if (!m_csSpecialStatusText.IsEmpty())
      {
         // special case a special status text should be displayed?

         if (timeGetTime() < m_nTimedStatusTimeout)
            csWindowText = m_csSpecialStatusText;
         else
            m_csSpecialStatusText.Empty();
      }
      
      if (csWindowText.IsEmpty())
      {
         // normal case: no special status text defined or time up
         Misc::FormatTickString(csCurrentTime, pDoc->m_curPosMs, pDoc->m_streamUnit, true);
         csWindowText.Format(ID_STATUSBAR_POSITION, csCurrentTime);
#ifdef _EDITOR  
         // relevant only for Editor stand-alone application 
         if(pPane!=NULL)
         {
            pPane->SetEnabled(TRUE);
         }
#endif
      }
   }
   else
   {
     Misc::FormatTickString(csCurrentTime, pDoc->m_curPosMs, pDoc->m_streamUnit, true);
     csWindowText.Format(ID_STATUSBAR_POSITION, csCurrentTime);
#ifdef _EDITOR  
     // relevant only for Editor stand-alone application 
     if(pPane!=NULL)
     {
        pPane->SetEnabled(FALSE);
     }
#endif
   }	
    pCmdUI->SetText(csWindowText);
}

void CMainFrameE::OnUpdateStatusPosition(CCmdUI* pCmdUI)
{
   GET_DOC(pDoc);
	
	CString csWindowText;
   CString csCurrentTime;
   CString csTotalTime;
	if (ProjectHasData())
	{
      if (!m_csSpecialStatusText.IsEmpty())
      {
         // special case a special status text should be displayed?

         if (timeGetTime() < m_nTimedStatusTimeout)
            csWindowText = m_csSpecialStatusText;
         else
            m_csSpecialStatusText.Empty();
      }
      
      if (csWindowText.IsEmpty())
      {
         // normal case: no special status text defined or time up
         Misc::FormatTickString(csCurrentTime, pDoc->m_curPosMs, pDoc->m_streamUnit, true);
         Misc::FormatTickString(csTotalTime, pDoc->m_docLengthMs, pDoc->m_streamUnit, true);
         csWindowText.Format(ID_STATUS_POSITION, csCurrentTime, csTotalTime);
      }
   }
   else
   {
     Misc::FormatTickString(csCurrentTime, pDoc->m_curPosMs, pDoc->m_streamUnit, true);
     Misc::FormatTickString(csTotalTime, pDoc->m_docLengthMs, pDoc->m_streamUnit, true);
     csWindowText.Format(ID_STATUS_POSITION, csCurrentTime, csTotalTime);
    
   }
	pCmdUI->SetText(csWindowText);
}

void CMainFrameE::OnUpdateStatusSelection(CCmdUI* pCmdUI)
{
    GET_DOC(pDoc);
    CString csWindowText;
    CString csSelectionBegin;
    Misc::FormatTickString(csSelectionBegin, pDoc->m_markStartMs, pDoc->m_streamUnit, true);

    // find pane
    CXTPStatusBarPane* pPane = NULL;
    CMainFrame *pStudioMainFrame = GetStudioMainFrame();
    if (pStudioMainFrame) {
        CXTPStatusBar *pStatusBar = pStudioMainFrame->GetStatusBar();
        if (pStatusBar) {
            pPane = pStatusBar->FindPane(pCmdUI->m_nID);
        }
    }

    CString csSelectionEnd;
    if (pDoc->m_markStartMs != -1)
    {
        Misc::FormatTickString(csSelectionBegin, pDoc->m_markStartMs, pDoc->m_streamUnit, true);
        Misc::FormatTickString(csSelectionEnd, pDoc->m_markEndMs, pDoc->m_streamUnit, true);
        csWindowText.Format(ID_STATUS_SELECTION, csSelectionBegin, csSelectionEnd);
        if(pPane!=NULL)
        {
            pPane->SetEnabled(TRUE);
        }
    }
    else
    {
        Misc::FormatTickString(csSelectionBegin, pDoc->m_markStartMs, pDoc->m_streamUnit, true);
        Misc::FormatTickString(csSelectionEnd, pDoc->m_markEndMs, pDoc->m_streamUnit, true);
        csWindowText.Format(ID_STATUS_SELECTION, csSelectionBegin, csSelectionEnd);
        if(pPane!=NULL)
        {
            pPane->SetEnabled(FALSE);
        }
    }
    pCmdUI->SetText(csWindowText);
}

void CMainFrameE::OnLrdClose()  {
    if (CStudioApp::IsLiveContextMode()) { 
        CEditorDoc *pDoc = GetEditorDocument();

        if (CStudioApp::IsLiveContextPreview()) {

            if (pDoc != NULL && pDoc->IsModified())
                pDoc->SetModifiedFlag(FALSE); // Do not ask for save upon exit of Preview
        } 
    }

    ResetCurrentIteractionId();

    CStudioApp *pApp = (CStudioApp *)AfxGetApp();

    if (pApp)
        pApp->CloseEditorMode();
}

void CMainFrameE::OnUpdateLrdClose(CCmdUI* pCmdUI) 
{
   BOOL bEnable = TRUE;

   // LrdClose is not possible in LiveContext edit mode
   if (CStudioApp::IsLiveContextMode() && !CStudioApp::IsLiveContextPreview())
      bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}

BOOL CMainFrameE::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
    // TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
    BOOL bResult = FALSE;

    if (m_pWhiteboardView != NULL) {
        // SPACE-key is pressed
        if (nCode == CN_COMMAND && nID == IDC_STARTPREVIEW) {
            if (m_pWhiteboardView && m_pWhiteboardView->IsChangingObject()) {
                m_pWhiteboardView->SendMessage(WM_KEYDOWN, VK_SPACE, 0x10);
                bResult = TRUE;
            }
        } else {
            bResult = m_pWhiteboardView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
        }
    }

    if (!bResult && m_pVideoView)
        bResult = m_pVideoView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

    if (!bResult && m_pStreamView)
        bResult = m_pStreamView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

    if (!bResult && m_pStructureView)
        bResult = m_pStructureView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

    if (bResult)
        return TRUE;

    return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

}

void CMainFrameE::OnSysColorChange()
{	  										
	CXTPMDIChildWnd::OnSysColorChange();

	RefreshColors();
	RedrawWindow(NULL, NULL, RDW_INVALIDATE);
}

void CMainFrameE::RefreshColors()
{
	s_clrLight = XTPColorManager()->LightColor(GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_WINDOW), 0xcd);		
	s_clrDark = GetXtremeColor(COLOR_3DFACE);
	
	s_clrBackgroundLight = XTPColorManager()->LightColor(GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_WINDOW), 0xcd);
	s_clrBackgroundDark = GetXtremeColor(COLOR_3DFACE);
	s_clrBorder = GetSysColor(COLOR_3DSHADOW);
	
	
#ifndef _XTP_EXCLUDE_LUNATHEME
	
   XTPCurrentSystemTheme systemTheme;
   systemTheme = XTPColorManager()->GetCurrentSystemTheme();

	s_clrStreamBg = RGB(217, 230, 250);
	s_clrStreamLine = RGB(122, 166, 250); 
			
	m_xpStyleType = WINDOWS_CLASSIC;
			
	switch (systemTheme)
	{
	case xtpSystemThemeBlue:
		{
			s_clrDark = RGB(129, 169, 226);
			s_clrLight = RGB(221, 236, 254);
			
			s_clrBackgroundLight = RGB(221, 236, 254);
			s_clrBackgroundDark = RGB(144, 153, 174);

			s_clrBorder = RGB(0, 53, 154);

			m_xpStyleType = XP_BLUE;
		} 
		break;
		
	case xtpSystemThemeOlive:
		{
			s_clrDark = RGB(183, 198, 145);
			s_clrLight = RGB(244, 247, 222);
			
			s_clrBackgroundLight = RGB(230, 234, 208);
			s_clrBackgroundDark = RGB(151, 160, 123);

			s_clrBorder = RGB(96, 119, 107);	

			m_xpStyleType = XP_OLIVE;
		}
		break;
		
	case xtpSystemThemeSilver:
		{
			s_clrDark = RGB(153, 151, 181);
			s_clrLight = RGB(243, 244, 250);
			
			s_clrBackgroundLight = RGB(227, 227, 236);
			s_clrBackgroundDark = RGB(155, 154, 179);

			s_clrBorder = RGB(118, 116, 146);	 

			m_xpStyleType = XP_SILVER;
		}
	}
#endif // _XTP_EXCLUDE_LUNATHEME	
	
}

// static
COLORREF CMainFrameE::GetColor(UINT nColorId)
{
	switch (nColorId)
	{
	case COLOR_DARK:
		return s_clrDark;
		break;
	case COLOR_LIGHT:
		return s_clrLight;
		break;
	case COLOR_BG_DARK:
		return s_clrBackgroundDark;
		break;
	case COLOR_BG_LIGHT:
		return s_clrBackgroundLight;
		break;
	case COLOR_BORDER:
		return s_clrBorder;
		break;
	case COLOR_STREAM_BG:
		return s_clrStreamBg;
		break;
	case COLOR_STREAM_LINE:
		return s_clrStreamLine;
		break;
	default:
		return RGB(255, 255, 255);
	}
}

CMainFrame *CMainFrameE::GetStudioMainFrame() {
    CMainFrame *pMainFrame = NULL;
    CWnd *pMainWnd = AfxGetMainWnd();
    if (pMainWnd->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
        pMainFrame = (CMainFrame *)pMainWnd;

    if (pMainFrame == NULL){
        if (AfxGetApp() != NULL 
            && AfxGetApp()->m_pMainWnd != NULL
            && AfxGetApp()->m_pMainWnd->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame)){
            pMainFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
        }
    }
    return pMainFrame;
}
CXTPRibbonBar* CMainFrameE::GetRibbonBar() {
    CMainFrame *pStudioMainFrame = GetStudioMainFrame();
    if (!pStudioMainFrame) {
        return NULL;
    }

    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = pStudioMainFrame->GetCommandBars();
    if (pCommandBars != NULL) {
        CXTPRibbonBar* pRibbonBar = DYNAMIC_DOWNCAST(CXTPRibbonBar, pCommandBars->GetMenuBar());
        ASSERT(pRibbonBar);
        return pRibbonBar;
    }

    return NULL;
}


void CMainFrameE::ChangeActiveView(CView *pActiveView, CView *pDeactivatedView)
{
	//For some reason this only happens after return to editor from assistant, not on first time editor is shown
   //if (m_pWhiteboardView != NULL &&
   //    pActiveView == m_pStreamView && pDeactivatedView == m_pWhiteboardView) {
   //    m_pWhiteboardView->SetInactive();
   //}
   //
   //if (pActiveView == m_pWhiteboardView &&
   //    pDeactivatedView == m_pStreamView)
   //    m_pStreamView->SetInactive();

   m_pActiveView = pActiveView;
}

void CMainFrameE::UpdateWhiteboardView()
{
   if (m_pWhiteboardView)
      m_pWhiteboardView->RedrawWindow();
}

void CMainFrameE::UpdateStreamView()
{
   if (m_pStreamView)
      m_pStreamView->RedrawWindow();
}

void CMainFrameE::UpdateVideoView()
{
   if (m_pVideoView)
      m_pVideoView->RedrawWindow();
}

void CMainFrameE::UpdateStructureView()
{
   if (m_pStructureView)
      m_pStructureView->RedrawWindow();
}

void CMainFrameE::UpdatePageObjectsView()
{
   if (m_pInteractionStructureView)
      m_pInteractionStructureView->RefreshList();
}

void CMainFrameE::AdjustToProjectMode()
{
    CDocument *pDocument = CMainFrameE::GetEditorDocument();

    if (pDocument && 
        pDocument->GetRuntimeClass() == RUNTIME_CLASS(CEditorDoc)) {

        if (!IsScreengrabbingDocument()) {
            SetViewsToStandardLayout();
            ShowHideInteractionTools(TRUE);
            ShowHideEditTools(TRUE);
        } else if (IsScreengrabbingDocument()) {
            SetViewsToScreengrabbingLayout();
            ShowHideInteractionTools(FALSE);
            ShowHideEditTools(FALSE);
        }
    }
}

void CMainFrameE::ShowTimedStatusText(CString &csMessage, UINT nTimeoutMs)
{
   m_csSpecialStatusText = csMessage;

   m_nTimedStatusTimeout = timeGetTime() + nTimeoutMs;
}

void CMainFrameE::OnTimer(UINT nIDEvent) {
    if (nIDEvent == TIMER_ICON_ID) {
        if (m_pSymbolBar) {

            CXTPImageManager *pImageManager = XTPImageManager();
            if (pImageManager != NULL) {
                pImageManager->SetMaskColor(RGB(0, 0, 0));
                if (m_bIsDeactivated) {
                    pImageManager->SetIcons(IDR_SYMBOLBAR, IDR_SYMBOLBAR);
                    ChangeToPlayHighlight();
                    m_bIsDeactivated = false;
                } else {
                    pImageManager->SetIcons(IDR_SYMBOLBAR, IDB_SYMBOLBAR);
                    ChangeToNormalPlay();
                    m_bIsDeactivated = true;
                } 
                m_pSymbolBar->RedrawWindow();
            }
        }
    } else if (nIDEvent == TIMER_REPLAY_ID) {
        if (m_bLastUpdateTooLong) {
            // skip one timer event in order to give the message queue time to do more important stuff
            m_bLastUpdateTooLong = false;
            return;
        }

        CEditorDoc *pDoc = CMainFrameE::GetEditorDocument();
        if (pDoc != NULL) {
            if (pDoc->IsPreviewActive()) {
                UINT nCurrentPreviewPos = pDoc->GetBackendPreviewPos();

                if (m_nLastCheckedPreviewPos != nCurrentPreviewPos && !pDoc->IsPausedSpecial()) {
                    DWORD dwOne = ::timeGetTime();
                    m_nLastCheckedPreviewPos = nCurrentPreviewPos;
                    int iNewPosition = nCurrentPreviewPos;

                    CQuestionnaireEx *pQuestionnaire = pDoc->project.GetFirstQuestionnaireEx();
                    if (pQuestionnaire && pQuestionnaire->IsRandomTest() && pDoc->IsPreviewActive()
                        && pQuestionnaire->GetQuestionTotal() > 0) {
                        UINT uiFirstQuestionBeginTimestamp = pDoc->project.GetFirstQuestionBeginTimeStamp();
                        if (iNewPosition > pDoc->m_curPosMs
                            && iNewPosition > uiFirstQuestionBeginTimestamp
                            && (pDoc->m_curPosMs < uiFirstQuestionBeginTimestamp 
                            || (pDoc->m_curPosMs == uiFirstQuestionBeginTimestamp && !pDoc->project.IsRandomTestStarted()))) 
                        {
                            pDoc->project.StartRandomTest(iNewPosition);
                        }
                    }

                    // check for stop marks
                    CMarkStream *pMarkStream = pDoc->project.GetMarkStream();
                    if (pMarkStream != NULL) {
                        int iCurrentPos = pDoc->m_curPosMs;
                        bool bMarkFound = pMarkStream->CheckTime((unsigned int*)&iNewPosition, iCurrentPos);

                        if (bMarkFound) {// iNewPosition is adapted
                            if (pDoc->IsPreviewActive()) {
                                BeginWaitCursor();
                                pDoc->PausePreviewSpecial();
                                EndWaitCursor();
                            }
                        }
                    }   

                    if (pDoc->InformCurrentReplayTime(iNewPosition)) {// m_curPosMs did really change?
                        pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
                    }

                    DWORD dwTwo = ::timeGetTime();
                    if (dwTwo-dwOne > TIMER_REPLAY_TIMEOUT)
                        m_bLastUpdateTooLong = true;
                }
            }

            if (m_bPositionJumped && (!pDoc->IsPreviewActive() || pDoc->IsPausedSpecial())) {
                // Another thread triggered an update:
                pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
                m_bPositionJumped = false;
            }
        }
    }

    CMDIChildWnd::OnTimer(nIDEvent);
}

BOOL CMainFrameE::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return CXTPFrameWndBase<CMDIChildWnd>::OnCommand(wParam, lParam);
}

void CMainFrameE::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CXTPMDIChildWnd::OnActivate(nState, pWndOther, bMinimized);

   // TODO: Add your message handler code here
   if (!m_pStructureView)
   {
      //CXTPDockingPane* pane = m_paneManager.FindPane(XTP_STRUCTURE_VIEW);
      m_paneManager.ClosePane(XTP_STRUCTURE_VIEW);
      m_paneManager.ShowPane(XTP_STRUCTURE_VIEW, 0);
   }
   if (!m_pVideoView)
   {
      //CXTPDockingPane* pane = m_paneManager.FindPane(XTP_STRUCTURE_VIEW);
      m_paneManager.ClosePane(XTP_VIDEO_VIEW);
      m_paneManager.ShowPane(XTP_VIDEO_VIEW, 0);
   }
   if (!m_pStreamView)
   {
      //CXTPDockingPane* pane = m_paneManager.FindPane(XTP_STRUCTURE_VIEW);
      m_paneManager.ClosePane(XTP_STREAM_VIEW);
      m_paneManager.ShowPane(XTP_STREAM_VIEW, 0);
   }
}

//ribbon messages start
void CMainFrameE::OnMenuExport()
{
}

void CMainFrameE::OnMenuAudio()
{
}

void CMainFrameE::OnMenuVideo()
{
}

void CMainFrameE::OnButtonInsertLRD()
{
}

void CMainFrameE::OnUpdateButtonInsertLRD(CCmdUI* pCmdUI)
{
   BOOL bEnable = TRUE;

   // InsertLRD is not possible in LiveContext edit mode
   if (CStudioApp::IsLiveContextMode() && !CStudioApp::IsLiveContextPreview())
      bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnButtonInsertClip()
{
}

//ribbon messages end
//ribbon implementation start
    
void CMainFrameE::CreateRibbonGalleries()
{
}

BOOL CMainFrameE::CreateRibbonBar()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();

   CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBars->Add(_T("The Ribbon"), xtpBarTop, RUNTIME_CLASS(CXTPRibbonBar));
   if (!pRibbonBar)
   {
      return FALSE; 
   }
   pRibbonBar->EnableDocking(0);

   // Hide ribbon bar in LIVECONTEXT preview mode 
   if (CStudioApp::IsLiveContextMode() && CStudioApp::IsLiveContextPreview()) { 
       pRibbonBar->SetVisible(FALSE);
   }

   //Quick Access Controls
   pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_FILE_SAVE);
   //pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, IDC_UNDO);
	//pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, IDC_REDO);
	pRibbonBar->GetQuickAccessControls()->CreateOriginalControls();
   //Frame Theme
   pRibbonBar->SetCloseable(FALSE);
   pRibbonBar->EnableFrameTheme();
   return TRUE;
}

#define ZOOM_SCALE_MAX        1000
#define ZOOM_SCALE_RANGE_MIN  0
BOOL CMainFrameE::CreateStatusBarForStudio(CXTPStatusBar *pStatusBar)
{
   m_pStatusBarStudio = pStatusBar;
   pStatusBar->RemoveAll();
   pStatusBar->SetIndicators(indicators,sizeof(indicators)/sizeof(UINT));
   CString csStatusPos;
   pStatusBar->GetPane(0)->SetBeginGroup(FALSE);
   CXTPStatusBarPane* pPane  = pStatusBar->AddIndicator(0);
   pStatusBar->EnableMarkup(TRUE);
   pPane->SetWidth(1);
   pPane->SetStyle(SBPS_STRETCH | SBPS_NOBORDERS);
   pPane->SetBeginGroup(FALSE);

   CXTPStatusBarSliderPane* pZoomPane = (CXTPStatusBarSliderPane*)pStatusBar->AddIndicator(new CXTPStatusBarSliderPane(), ID_INDICATOR_ZOOMSLIDER);
   pZoomPane->SetBeginGroup(FALSE);
   pZoomPane->SetWidth(160);
   pZoomPane->SetPos(0); // We will convert 0 -> 10%, 500 -> 100 %, 1000 -> 500 %
   pZoomPane->SetRange(ZOOM_SCALE_RANGE_MIN, ZOOM_SCALE_MAX);
   pZoomPane->SetCaption(_T("&Zoom Slider"));
   pZoomPane->SetTooltip(_T("Zoom"));
   CString csTooltip;
   csTooltip.LoadString(IDC_ZOOM_OUT);
   pZoomPane->SetTooltipPart(XTP_HTSCROLLUP, csTooltip /*_T("Zoom Out")*/);
   csTooltip.LoadString(IDC_ZOOM_IN);
   pZoomPane->SetTooltipPart(XTP_HTSCROLLDOWN, csTooltip/*_T("Zoom In")*/);


   CString zoomAll;
   zoomAll.LoadString(IDS_INDICATOR_ZOOM_ALL);
   CXTPStatusBarPane* pPaneZoomAll = pStatusBar->AddIndicator(ID_INDICATOR_ZOOM_ALL);
   pPaneZoomAll->SetButton();
   pPaneZoomAll->SetBeginGroup(FALSE);
   pPaneZoomAll->SetIconIndex(ID_INDICATOR_ZOOM_ALL);
   pPaneZoomAll->SetTooltip(zoomAll);
   pPaneZoomAll->SetCaption(zoomAll);

   CString zoomSelected;
   zoomSelected.LoadString(IDS_INDICATOR_ZOOM_SELECTED);
   CXTPStatusBarPane* pPaneZoomSelected = pStatusBar->AddIndicator(ID_INDICATOR_ZOOM_SELECTED);
   pPaneZoomSelected->SetButton();
   pPaneZoomSelected->SetBeginGroup(FALSE);
   pPaneZoomSelected->SetIconIndex(ID_INDICATOR_ZOOM_SELECTED);
   pPaneZoomSelected->SetTooltip(zoomSelected);
   pPaneZoomSelected->SetCaption(zoomSelected);

   pStatusBar->SetRibbonDividerIndex(pZoomPane->GetIndex() - 1);
   pStatusBar->SetDrawDisabledText(TRUE);
   pStatusBar->EnableCustomization();
   pStatusBar->LoadState(COMMAND_BARS_MODE_EDITOR);

   CXTPToolTipContext* pToolTipContext =pStatusBar->GetToolTipContext();
   pToolTipContext->SetStyle(xtpToolTipOffice2007);
   pToolTipContext->SetFont(pStatusBar->GetPaintManager()->GetIconFont());

   return TRUE;
}

//ribbon implementation end


#define ST_ZOOM_MAX       500   //ZOOM 500%
#define ST_ZOOM_MIDDLE    250   //ZOOM 250%
#define ST_ZOOM_MIN       100   //ZOOM 100%
#define ST_ZOOM_STEP      5     //5 - >Step zoom 10%

#define ST_POS_MAX        1000  //POS 1000 -> 500 %
#define ST_POS_MIDDLE     500   //POS 500 -> 250 %
#define ST_POS_MIN        0     //POS 0 -> 100%
#define ST_POS_STEP       1

int PosToZoom(int nPos) {
	return nPos <= ST_POS_MIDDLE ? ST_ZOOM_MIN + MulDiv(nPos, ST_ZOOM_MIDDLE - ST_ZOOM_MIN , ST_ZOOM_MAX) : 
                                  ST_ZOOM_MIDDLE + MulDiv(nPos - ST_ZOOM_MAX, ST_ZOOM_MAX - ST_ZOOM_MIDDLE, ST_ZOOM_MAX);
}
int ZoomToPos(int nZoom) {
	return nZoom < ST_ZOOM_MIDDLE ? MulDiv(nZoom - ST_ZOOM_MIN, ST_ZOOM_MAX, ST_ZOOM_MIDDLE - ST_ZOOM_MIN) : 
                                   ST_ZOOM_MAX + MulDiv(nZoom - ST_ZOOM_MIDDLE, ST_ZOOM_MAX, ST_ZOOM_MAX - ST_ZOOM_MIDDLE);
}
#ifndef EM_SETZOOM
#define EM_SETZOOM				(WM_USER + 225)
#endif

void CMainFrameE::OnUpdateZoomSliderScroll(CCmdUI *pCmdUI)
{
    GET_DOC(pDoc);
    if (pDoc->m_docLengthMs == -1)
        return;

    CMainFrame *pStudioMainFrame = GetStudioMainFrame();
    if (!pStudioMainFrame) {
        return;
    }
    CXTPStatusBar *pStatusBar = pStudioMainFrame->GetStatusBar();
    if (!pStatusBar) {
        return;
    }

    CXTPStatusBarSliderPane* pPane = (CXTPStatusBarSliderPane *)pStatusBar->FindPane(ID_INDICATOR_ZOOMSLIDER);
    if (pPane == NULL)
        return;
    if (IsPlusMinusKeyZoom() == true) {
        int length = pDoc->m_docLengthMs;
        int diff = pDoc->m_displayEndMs - pDoc->m_displayStartMs;

        double raport = (1.0*length)/(diff*1.0);
        double newPos = ST_POS_MAX - (double)ST_POS_MAX/raport + ST_ZOOM_STEP;
        int oldPos = pPane->GetPos();
        int newRealPos =(int)newPos;
        pPane->SetPos(newRealPos);
    }

    int nZoom = PosToZoom(pPane->GetPos());
    //CString zoomToolTip;
    //zoomToolTip.Format(_T("Zoom : %d%%"),nZoom);
    //pPane->SetTooltip(zoomToolTip);

    if (m_pStreamView) {
        m_pStreamView->OnUpdate(m_pStreamView, NULL, NULL);
    }
    return;
}

void CMainFrameE::OnZoomSliderScroll(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMXTPSCROLL* pNMScroll = (NMXTPSCROLL*)pNMHDR;
    CXTPStatusBarSliderPane* pPane = DYNAMIC_DOWNCAST(CXTPStatusBarSliderPane, pNMScroll->pSender);
    if (!pPane || !m_pStreamView)
        return;

    int nZoom = PosToZoom(pPane->GetPos());

    switch (pNMScroll->nSBCode) {
    case SB_TOP: 
        nZoom = ST_ZOOM_MIN; 
        break;
    case SB_BOTTOM: 
        nZoom = ST_ZOOM_MAX;
        break;

    case SB_LINEUP: 
    case SB_PAGEUP:
        nZoom = max(((nZoom / ST_ZOOM_STEP) - 1) * ST_ZOOM_STEP, ST_ZOOM_MIN); 
        SetPlusMinusKeyZoom(false); 
        break;
    case SB_LINEDOWN: 
    case SB_PAGEDOWN:
        nZoom = min(((nZoom / ST_ZOOM_STEP) + 2) * ST_ZOOM_STEP, ST_ZOOM_MAX);
        SetPlusMinusKeyZoom(false); 
        break;

    case SB_THUMBTRACK: 
        nZoom = PosToZoom(pNMScroll->nPos); 
        if (nZoom > 0 && nZoom < ST_ZOOM_MIN) 
            nZoom = ST_ZOOM_MIN; 
        break;

    /*case SB_PAGEUP: 
        nZoom = max(nZoom - ST_ZOOM_STEP, ST_ZOOM_MIN); 
        break;
    case SB_PAGEDOWN: 
        nZoom = min(nZoom + ST_ZOOM_STEP, ST_ZOOM_MAX); 
        break;*/
    }

    m_pStreamView->ZoomTo(nZoom);
    int nPos = ZoomToPos(nZoom);
    pPane->SetPos(nPos);
}

void CMainFrameE::OnZoomAll() {
    SetPlusMinusKeyZoom(true);
    if (m_pStreamView != NULL)
        m_pStreamView->ZoomAll();
}

void CMainFrameE::OnZoomSelected() {
    SetPlusMinusKeyZoom(true);
    if (m_pStreamView != NULL)
        m_pStreamView->ZoomSelected();
}

BOOL CMainFrameE::OnPinRangeClick( UINT nID ,NMHDR * pNotifyStruct, LRESULT* result)
{
   CXTPRibbonControlRecentFileListExt::GetInstance()->OnPinRangeClick(nID);
   return TRUE;
}
// PRIVATE
void CMainFrameE::ChangeToPlayHighlight()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   if (pCommandBars != NULL)
   {
      UINT uiTabStartLarge[] = {-1, -1, -1, -1, -1,-1, -1, -1, -1, -1,-1, -1, -1, -1, -1,-1,-1,IDC_STARTSTOPPREVIEW}; 
      pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE_E, uiTabStartLarge, _countof(uiTabStartLarge), CSize(32, 32));
      CXTPRibbonGroup *pGroup =  GetRibbonBar()->FindGroup(ID_GROUP_PLAYBACK_CONTROLS);
      if(pGroup)
      {
         CXTPControl* pControl = (CXTPControl*)pGroup->FindControl(IDC_STARTSTOPPREVIEW);
         pControl->SetPressed(true);
          
         CString csToolTip;
         csToolTip.LoadString(IDS_IDC_STARTSTOPPREVIEW);        
         if(csToolTip.IsEmpty()==false)
         {
            pControl->SetTooltip(csToolTip);
            pControl->SetDescription(csToolTip);
         }
         pControl->SetCaption(IDC_STARTSTOPPREVIEW); 
         pControl->RedrawParent();
      }
   }
}

HWND CMainFrameE::GetVideoHWND(bool bVideoHWND) {
    HWND hWndReturn = NULL;

    CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
    if (pDoc != NULL) {
        CVideoView *pView = pDoc->GetVideoView();
        if (pView) {
            if (bVideoHWND)
                hWndReturn = ((CVideoView *)pView)->GetSafeVideoHwnd();
            else
                hWndReturn = ((CVideoView *)pView)->GetSafeHwnd();
        }
    }

    return hWndReturn;
}

BOOL CMainFrameE::PreTranslateMessage(MSG* pMsg)
{
    BOOL bResult = FALSE;

    if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) {
        CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
        if (pDoc && !pDoc->project.IsEmpty()) {
            if (pDoc->IsPreviewActive() && m_pWhiteboardView) {
                bResult = m_pWhiteboardView->PreTranslateMessage(pMsg);
            }
        }
    }
    /*if(pMsg->message == WM_LBUTTONDOWN && m_pWhiteboardView && m_pWhiteboardView->GetSafeHwnd() != NULL)
        m_pWhiteboardView->ClickPerformed(pMsg->pt);*/

    // TODO remove: a hack (see QuestionStream.cpp)
    if (pMsg->message == WM_USER + MESSAGE_ID_DO_ACTIONS) {
        CQuestionStream *pStream = (CQuestionStream *)pMsg->wParam;

        pStream->FeedbackLoopDoActions(); // the action execution

        bResult = TRUE;
    }

    if (bResult != FALSE)
        return bResult;

    return CXTPMDIChildWnd::PreTranslateMessage(pMsg);
}

void CMainFrameE::ChangeToNormalPlay() {
    CXTPCommandBars* pCommandBars = GetCommandBars();
    if (pCommandBars != NULL && pCommandBars->GetImageManager() != NULL) {
        UINT uiTabStartLarge[] = {-1, -1, -1, -1, -1,IDC_STARTSTOPPREVIEW}; 
        pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE_E, uiTabStartLarge, _countof(uiTabStartLarge), CSize(32, 32));
        CXTPRibbonBar * pRibbonBar = GetRibbonBar();
        if (pRibbonBar != NULL) {
            CXTPRibbonGroup *pGroup =  pRibbonBar->FindGroup(ID_GROUP_PLAYBACK_CONTROLS);
            if (pGroup) {
                CXTPControl* pControl = (CXTPControl*)pGroup->FindControl(IDC_STARTSTOPPREVIEW);
                pControl->SetPressed(false);

                CString csToolTip;
                csToolTip.LoadString(IDS_IDC_STARTSTOPPREVIEW);        
                if(csToolTip.IsEmpty() == false) {
                    pControl->SetTooltip(csToolTip);
                    pControl->SetDescription(csToolTip);
                }

                pControl->SetCaption(IDC_STARTSTOPPREVIEW);  
                pControl->RedrawParent();
            }
        }
    }
}

void CMainFrameE::ChangeToPause() {
    StopPausePreviewSpecialTimer();

    CXTPCommandBars* pCommandBars = GetCommandBars();
    if (pCommandBars != NULL && pCommandBars->GetImageManager()) {
        UINT uiTabStartLarge[] = {-1, -1, -1, -1,-1, -1, -1, -1, -1,-1, -1, -1, -1, -1,-1,-1,IDC_STARTSTOPPREVIEW}; 
        pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE_E, uiTabStartLarge, _countof(uiTabStartLarge), CSize(32, 32));
        CXTPRibbonBar * pRibbonBar = GetRibbonBar();
        if (pRibbonBar != NULL) {
            CXTPRibbonGroup *pGroup =  pRibbonBar->FindGroup(ID_GROUP_PLAYBACK_CONTROLS);
            if( pGroup) {
                CXTPControl* pControl = (CXTPControl*)pGroup->FindControl(IDC_STARTSTOPPREVIEW);
                // BUGFIX 4835: pressed state is no longer used for blinking since LEC 4
                // pControl->SetPressed(true);
                pControl->SetCaption(IDC_RIBBON_PAUSEPREVIEW);
                pControl->SetPressed(FALSE);
                CString csToolTip;
                csToolTip.LoadString(IDC_RIBBON_PAUSEPREVIEW);        
                if(csToolTip.IsEmpty() == false) {
                    pControl->SetTooltip(csToolTip);
                    pControl->SetDescription(csToolTip);
                }
                pControl->RedrawParent();
            }
        }
    }
}

CStreamView* CMainFrameE::GetStreamView()
{
  return m_pStreamView;
}
bool CMainFrameE::IsPlusMinusKeyZoom()
{
  return m_bPlusMinusKeyZoom;
}
void CMainFrameE::SetPlusMinusKeyZoom(bool bPlusMinusKeyZoom)
{
  m_bPlusMinusKeyZoom = bPlusMinusKeyZoom;
}
void CMainFrameE::SetColorScheme(UINT nColorScheme)
{
    HMODULE hModule = AfxGetInstanceHandle();

    LPCTSTR lpszIniFile = _T("OFFICE2007BLACK.INI");
    switch (nColorScheme)
    {
    case 0: 
        lpszIniFile = _T("OFFICE2007BLUE.INI"); 
        break;
    case 1: 
        lpszIniFile = _T("OFFICE2007SILVER.INI"); 
        break;
    case 2: 
        lpszIniFile = _T("OFFICE2007BLACK.INI"); 
        break;
    case 3: 
        lpszIniFile = _T("OFFICE2007AQUA.INI");
        break;
    default:
        lpszIniFile = _T("OFFICE2007BLUE.INI"); 
        break;

    }


    ((CXTPOffice2007Theme*)GetCommandBars()->GetPaintManager())->SetImageHandle(hModule, lpszIniFile);

    CMainFrameE *pMainFrameE = CMainFrameE::GetCurrentInstance();
    if (pMainFrameE) 
    {
       pMainFrameE->RefreshPaneDrawing();
    }
    GetCommandBars()->GetImageManager()->RefreshAll();
    GetCommandBars()->RedrawCommandBars();
    SendMessage(WM_NCPAINT);

    RedrawWindow(0, 0, RDW_ALLCHILDREN|RDW_INVALIDATE);
}

void CMainFrameE::SwitchRecordingToSimulation() {

    CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
    if (pDoc != NULL)
        pDoc->OpenAsScreenSimulation();
    
    AdjustToProjectMode();
    
    if (pDoc != NULL)
        pDoc->RebuildPreview();
}

void CMainFrameE::SwitchSimulationToRecording() {
    CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
    if (pDoc != NULL)
        pDoc->OpenAsScreenRecording();
    
    AdjustToProjectMode();
    
    if (pDoc != NULL)
        pDoc->RebuildPreview();
}

void CMainFrameE::OnInsertInteraction() {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if(pEditorDoc != NULL && pEditorDoc->IsScreenGrabbingDocument() && !pEditorDoc->ShowAsScreenSimulation()) {
        SwitchRecordingToSimulation();
    }
    const MSG* pMsg = GetCurrentMessage();

    if (m_pWhiteboardView) {
        m_pWhiteboardView->SetInteractionType(WPARAM(pMsg->wParam));
        m_pWhiteboardView->SetStartRangeSelection();
        m_iCurrentSelectedInteractionID = (int) pMsg->wParam;
    }
}

void CMainFrameE::OnUpdateInsertInteraction(CCmdUI *pCmdUI) {
    // Always visible since Demo mode. 
    // Be aware that if we are not in Screen simulation we must enter in this mode before executing command.
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc->IsPreviewActive()) {
        pCmdUI->Enable(FALSE);
    } else {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_iCurrentSelectedInteractionID == pCmdUI->m_nID);
    }
}

void CMainFrameE::OnInsertTest() {
    CEditorDoc* pEditorDoc = (CEditorDoc *)GetActiveDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty())
        return;

    if(pEditorDoc != NULL && pEditorDoc->IsScreenGrabbingDocument() && !pEditorDoc->ShowAsScreenSimulation())
        SwitchRecordingToSimulation();

    CEditorProject *pProject = &pEditorDoc->project;

    CQuestionStream *pQuestionStream = pEditorDoc->project.GetQuestionStream(true);

    CQuestionEx *pQuestion = pQuestionStream->FindQuestionForTime(pEditorDoc->m_curPosMs);
    if (pQuestion != NULL) {
        CString msg;
        msg.Format(IDS_MSG_INSERT_SECOND_QUESTION);
        pEditorDoc->ShowErrorMessage(msg);
        return;
    }

    CArray<CQuestionnaireEx *, CQuestionnaireEx *> aQuestionnaires;
    pQuestionStream->GetQuestionnaires(&aQuestionnaires);
    if (aQuestionnaires.GetSize() == 0) {
        CQuestionnaireEx *pQuestionnaire = 
            CQuestionnaireEx::CreateDefaultQuestionnaire(pProject);
        if (pQuestionnaire) {
            HRESULT hr = pQuestionStream->AddQuestionnaire(pQuestionnaire, false);
            _ASSERT(SUCCEEDED(hr));
        }
    }

    const MSG* pMsg = GetCurrentMessage( );
    UINT nAnswerPageID = WPARAM(pMsg->wParam);

    CTestSettings dialog(IDS_TEST_SETTINGS, nAnswerPageID, this, 0);
    dialog.Init(NULL, pProject);
    UINT nReturn = dialog.DoModal();

    if (nReturn == IDOK) {
        CQuestionEx *pQuestion = new CQuestionEx();

        int iQuestionnaireIndex;
        dialog.SetUserEntries(pQuestion, iQuestionnaireIndex);

        if (iQuestionnaireIndex >= 0) { 
            HRESULT hr = S_OK;

            CArray<CQuestionnaireEx *, CQuestionnaireEx *> aQuestionnaires;
            pQuestionStream->GetQuestionnaires(&aQuestionnaires);
            CQuestionnaireEx *pQuestionnaire = aQuestionnaires[iQuestionnaireIndex];

            // Because total number of random questions can be increased by 1, before the question is actually being inserted, 
            // see if user changed this number. Can be the case when user decreases the number just by 1, and InsertSingle fails to update this correctly.
            int iCorrectNumberOfRandomQuestions = -1;
            if (pQuestionnaire->GetNumberOfRandomQuestions() == pQuestionnaire->GetQuestionTotal()
                && dialog.IsQuestionnaireSettingsShown()) {
                iCorrectNumberOfRandomQuestions = pQuestionnaire->GetNumberOfRandomQuestions();
            }

            if (pQuestionnaire->IsDoStopmarks()) {
                UINT nActivityEndMs = pQuestion->GetObjectActivityEnd();
                CMarkStream *pMarkStream = pEditorDoc->project.GetMarkStream(true);

                CStopJumpMark mark;
                mark.Init(false, NULL, nActivityEndMs);
                pMarkStream->IncludeMark(&mark, false);
            }

            hr = pQuestionStream->InsertSingle(pQuestion, pQuestionnaire, false);

            if (iCorrectNumberOfRandomQuestions != -1){
                pQuestionnaire->SetNumberOfRandomQuestions(iCorrectNumberOfRandomQuestions);
            }
            CFeedback *pNewResultFeedback = CQuestionnaireEx::CreateResultFeedback(pProject, pQuestionnaire);
            if (SUCCEEDED(hr))
                hr = pQuestionnaire->ReplaceFeedback(pNewResultFeedback, true); // also sets rollback point


            _ASSERT(SUCCEEDED(hr));

            pQuestion->UpdateChangeableTexts(pQuestion->GetAvailableSeconds());
            pEditorDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
        }

        pEditorDoc->SetModifiedFlag();
    } else {
        CQuestionnaireEx *pFirstQuestionnaire = pEditorDoc->project.GetFirstQuestionnaireEx();
        if (pFirstQuestionnaire->GetNumberOfRandomQuestions() > pFirstQuestionnaire->GetQuestionTotal())
            pFirstQuestionnaire->SetNumberOfRandomQuestions(pFirstQuestionnaire->GetQuestionTotal());
    }
}

void CMainFrameE::OnUpdateInsertTest(CCmdUI *pCmdUI) {
    // Always visible since Demo mode. 
    // Be aware that if we are not in Screen simulation we must enter in this mode before executing command.
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc->IsPreviewActive()) {
        pCmdUI->Enable(FALSE);
    } else {
        pCmdUI->Enable(TRUE);
    }
}


void CMainFrameE::OnCreateDemoDocument() {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc == NULL) {
        return;
    }

    if (pEditorDoc->IsDemoDocument()) {
        CString msg;
        msg.LoadString(IDS_DEMO_DOCUMENT_EXIST);
        CEditorDoc::ShowInfoMessage(msg);
        return;
    }

    if (!pEditorDoc->project.DocumentHasAnalysisPages() || !pEditorDoc->project.ShowClickPages()) {
        CString msg;
        msg.LoadString(IDS_CLICKPAGES_DOESNT_EXIST);
        CEditorDoc::ShowInfoMessage(msg);
        return;
    }
    if (!pEditorDoc->ShowAsScreenSimulation()) {
        SwitchRecordingToSimulation();
    }
    if (pEditorDoc->GetWhiteboardView() != NULL) { // check WhiteboardView existance after switching to simulation.
        pEditorDoc->GetWhiteboardView()->InsertDefaultDemoModeObjects();
    }
}

void CMainFrameE::OnUpdateCreateDemoDocument(CCmdUI* pCmdUI) {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc->IsPreviewActive()) {
        pCmdUI->Enable(FALSE);
    } else {
        pCmdUI->Enable(TRUE);
    }
}

void CMainFrameE::OnHideMousePointer() {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();

    // Toggle between show/hide mouse pointer
    if (pEditorDoc != NULL) {
        CPage *pPage = pEditorDoc->project.FindPageAt(pEditorDoc->m_curPosMs);
        if (pPage != NULL)
            pPage->SetHidePointers(!pPage->IsHidePointers());

        if (m_pWhiteboardView) {
            m_pWhiteboardView->ForceNextRedraw();
            m_pWhiteboardView->RedrawWindow();
        }
        pEditorDoc->SetModifiedFlag(TRUE);
    }
}

void CMainFrameE::OnUpdateHideMousePointer(CCmdUI* pCmdUI) {
    BOOL bEnable = FALSE;

    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();

    if (pEditorDoc != NULL && 
        pEditorDoc->IsScreenGrabbingDocument() && !pEditorDoc->project.ShowScreenGrabbingAsRecording()) {

        CPage *pPage = pEditorDoc->project.FindPageAt(pEditorDoc->m_curPosMs);
        if (pPage != NULL) {
            bEnable = TRUE;
            pCmdUI->SetCheck(!pPage->IsHidePointers());	
        }
    }

    if (pEditorDoc->IsPreviewActive()) {
        pCmdUI->Enable(FALSE);
    } else {
        pCmdUI->Enable(bEnable);
    }
}

void CMainFrameE::OnUpdateButtonCallout(CCmdUI* pCmdUI) {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc->IsPreviewActive()) {
        pCmdUI->Enable(FALSE);
    } else {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_iCurrentSelectedInteractionID == pCmdUI->m_nID);
    }
}

void CMainFrameE::OnButtonCallout() {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if(pEditorDoc != NULL && pEditorDoc->IsScreenGrabbingDocument() && !pEditorDoc->ShowAsScreenSimulation()) {
        SwitchRecordingToSimulation();
    }
    const MSG* pMsg = GetCurrentMessage();

    if (m_pWhiteboardView) {
        m_pWhiteboardView->SetInteractionType(WPARAM(pMsg->wParam));
        m_pWhiteboardView->SetStartRangeSelection();
        m_iCurrentSelectedInteractionID = (int) pMsg->wParam;//ID_GRAPHICALOBJECT_CALLOUT;
    }
}

void CMainFrameE::OnUpdateInsertText(CCmdUI* pCmdUI) {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc->IsPreviewActive()) {
        pCmdUI->Enable(FALSE);
    } else {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_iCurrentSelectedInteractionID == pCmdUI->m_nID);
    }
}

void CMainFrameE::OnInsertText() {

    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if(pEditorDoc != NULL && pEditorDoc->IsScreenGrabbingDocument() && !pEditorDoc->ShowAsScreenSimulation()) {
        SwitchRecordingToSimulation();
    }
    const MSG* pMsg = GetCurrentMessage();

    if (m_pWhiteboardView) {
        m_pWhiteboardView->SetInteractionType(WPARAM(pMsg->wParam));
        m_pWhiteboardView->SetStartRangeSelection();
        m_iCurrentSelectedInteractionID = (int) pMsg->wParam;//ID_GRAPHICALOBJECT_TEXT;
    }
}

void CMainFrameE::OnUpdateGalleryCallouts(CCmdUI* pCmdUI) {
    pCmdUI->Enable(TRUE);
}

void CMainFrameE::OnXTPGalleryCallouts(NMHDR* pNMHDR, LRESULT* pResult) {
    NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
    CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);

    if (pGallery) {
        CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());

        if (pItem) {
            CMainFrameE*pMainFrameE =  CMainFrameE::GetCurrentInstance();
            CXTPCommandBars* pCommandBars = pMainFrameE->GetCommandBars();

            if ( pCommandBars != NULL ) {

                UINT uiTabTools[] = {
                    pItem->GetID() == ID_CALLOUT_TOP_LEFT ? ID_GRAPHICALOBJECT_CALLOUT : -1, 
                    pItem->GetID() == ID_CALLOUT_TOP_RIGHT ? ID_GRAPHICALOBJECT_CALLOUT : -1, 
                    pItem->GetID() == ID_CALLOUT_BOTTOM_LEFT ? ID_GRAPHICALOBJECT_CALLOUT : -1, 
                    pItem->GetID() == ID_CALLOUT_BOTTOM_RIGHT ? ID_GRAPHICALOBJECT_CALLOUT : -1, 
                    pItem->GetID() == ID_CALLOUT_LEFT_BOTTOM ? ID_GRAPHICALOBJECT_CALLOUT : -1, 
                    pItem->GetID() == ID_CALLOUT_LEFT_TOP ? ID_GRAPHICALOBJECT_CALLOUT : -1, 
                    pItem->GetID() == ID_CALLOUT_RIGHT_BOTTOM ? ID_GRAPHICALOBJECT_CALLOUT : -1, 
                    pItem->GetID() == ID_CALLOUT_RIGHT_TOP ? ID_GRAPHICALOBJECT_CALLOUT : -1 
                };

                pCommandBars->GetImageManager()->SetIcons(IDB_CALLOUTS, uiTabTools, _countof(uiTabTools), CSize(32, 32));
                pCommandBars->RedrawCommandBars();


                CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
                if (pEditorDoc) {
                    pEditorDoc->project.SetActiveCalloutType(pItem->GetID());
                }


                SendMessage(WM_COMMAND, ID_GRAPHICALOBJECT_CALLOUT);      
            }
        }
        *pResult = TRUE; 
    }
}

void CMainFrameE::OnButtonDemoObject() {
    /*CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if(pEditorDoc != NULL ) {
        if(!pEditorDoc->IsDemoDocument())
            return;
    }*/
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if(pEditorDoc != NULL && pEditorDoc->IsScreenGrabbingDocument() && !pEditorDoc->ShowAsScreenSimulation()) {
        SwitchRecordingToSimulation();
    }
    const MSG* pMsg = GetCurrentMessage();

    if (m_pWhiteboardView) {
        m_pWhiteboardView->SetInteractionType(WPARAM(pMsg->wParam));
        m_pWhiteboardView->SetStartRangeSelection();
        m_iCurrentSelectedInteractionID = (int) pMsg->wParam;//ID_DEMOOBJECT;
    }
}

void CMainFrameE::OnUpdateButtonDemoObject(CCmdUI* pCmdUI) {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc->IsPreviewActive() || !pEditorDoc->IsScreenGrabbingDocument()) {// || !pEditorDoc->IsDemoDocument()) {
        pCmdUI->Enable(FALSE);
    } else {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_iCurrentSelectedInteractionID == pCmdUI->m_nID);
    }
}

void CMainFrameE::OnUpdateGalleryDemoObjects(CCmdUI* pCmdUI) {
    pCmdUI->Enable(TRUE);
}

void CMainFrameE::OnXTPGalleryDemoObjects(NMHDR* pNMHDR, LRESULT* pResult){
    NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
    CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);

    if (pGallery) {
        CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());

        if (pItem) {
            CMainFrameE*pMainFrameE =  CMainFrameE::GetCurrentInstance();
            CXTPCommandBars* pCommandBars = pMainFrameE->GetCommandBars();

            if ( pCommandBars != NULL ) {

                UINT uiTabTools[] = {
                    pItem->GetID() == ID_DEMOOBJ_TOP_LEFT ? ID_DEMOOBJECT : -1, 
                    pItem->GetID() == ID_DEMOOBJ_TOP_RIGHT ? ID_DEMOOBJECT : -1, 
                    pItem->GetID() == ID_DEMOOBJ_BOTTOM_LEFT ? ID_DEMOOBJECT : -1, 
                    pItem->GetID() == ID_DEMOOBJ_BOTTOM_RIGHT ? ID_DEMOOBJECT : -1, 
                    pItem->GetID() == ID_DEMOOBJ_LEFT_BOTTOM ? ID_DEMOOBJECT : -1, 
                    pItem->GetID() == ID_DEMOOBJ_LEFT_TOP ? ID_DEMOOBJECT : -1, 
                    pItem->GetID() == ID_DEMOOBJ_RIGHT_BOTTOM ? ID_DEMOOBJECT : -1, 
                    pItem->GetID() == ID_DEMOOBJ_RIGHT_TOP ? ID_DEMOOBJECT : -1 
                };

                pCommandBars->GetImageManager()->SetIcons(IDB_DEMOOBJECTS, uiTabTools, _countof(uiTabTools), CSize(32, 32));
                pCommandBars->RedrawCommandBars();


                CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
                if (pEditorDoc) {
                    pEditorDoc->project.SetActiveDemoObjectType(pItem->GetID());
                }


                SendMessage(WM_COMMAND, ID_DEMOOBJECT);      
            }
        }
        *pResult = TRUE; 
    }
}

void CMainFrameE::OnCreateManual() {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc == NULL) {
        return;
    }
    pEditorDoc->project.SaveRecordingAsWord();
}

void CMainFrameE::OnUpdateCreateManual(CCmdUI* pCmdUI) {
    BOOL bEnable = FALSE;
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc != NULL) {
        if (pEditorDoc->IsDemoDocument()) {
            bEnable = TRUE;
        }
    }
    pCmdUI->Enable(bEnable);
}

void CMainFrameE::OnGraphicalObjectDefaultSettings() {
    
   /* CGraphicalObjectDefaultSettingsDialog dlg;
    dlg.DoModal();*/
   // AfxMessageBox(_T("Mumu"));

    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();

    if (pEditorDoc != NULL && !pEditorDoc->project.IsEmpty()) {
        LOGFONT lf;
        //LFont::FillWithDefaultValues(&lf);
        CRect rc(0,0, 70, 50);
        CString csTitle;
        csTitle.LoadString(ID_GROUP_GRAPHICAL_OBJECTS);
        CString csText = _T("Text");
        COLORREF clrText, clrLine, clrFill;
        int iLineWidth, iLineStyle;
        pEditorDoc->project.GetGraphicalObjectDefaultSettings(clrText, clrLine, clrFill, lf, iLineWidth, iLineStyle);
        
        CEditGraphicalObjectDefaults dlg;
        dlg.Init(csTitle, csText, rc, clrFill, clrLine, clrText, &lf, iLineWidth, iLineStyle);
        if (dlg.DoModal() == IDOK)
        {
            dlg.GetUserEntries(clrText, clrLine, clrFill, lf, iLineWidth, iLineStyle);
            pEditorDoc->project.SetGraphicalObjectDefaultSettings(clrText, clrLine, clrFill, lf, iLineWidth, iLineStyle);
        }
    }
}

void CMainFrameE::OnUpdateGraphicalObjectDefaultSettings(CCmdUI* pCmdUI) {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetActiveDocument();
    if (pEditorDoc->IsPreviewActive()) {
        pCmdUI->Enable(FALSE);
    } else {
        pCmdUI->Enable(TRUE);
    }
}
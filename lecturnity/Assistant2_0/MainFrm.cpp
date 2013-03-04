// MainFrm.cpp : implementation of the CMainFrameA class
//

/* REVIEW UK
 * uQuickAccessBar -> aQuickAccessBarIds ??
 */
#include "stdafx.h"
#include "stdlib.h"


#include "lutils.h"

#include "..\Editor\editorDoc.h" // for ShowWarningMessage()
#include "..\Studio\Resource.h"		 
#include "..\Studio\Studio.h"		   
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "MainFrm.h"
#include "AssistantDoc.h"
#include "BrowseFolder\XFolderDialog.h"
#include "MissingFontsDlg.h"
#include "DlgPageSetup.h"
#include "DlgMetadata.h"
#include "TwoGBCountdown.h"

#include "LanguageSupport.h"

#include "backend\mlb_misc.h"
#include "backend\la_project.h"
#include "GridSettings.h"
#include "StartupView.h"
#include "DrawingToolSettings.h"
#include "GalleryItems.h"
#include "AssistantSettings.h"

#include "MonitorWidget.h"
#include "DocStructRecord.h"

#include "MfcUtils.h"   // lsutl32


#ifdef _STUDIO
#include "..\Studio\MainFrm.h"
#endif
#ifndef _STUDIO
#include "..\Studio\RibbonBars.h"
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrameA

#define AUDIO_TIMER 1
#define VIDEO_TIMER 2
#define DISCSPACE_TIMER 3
#define AUDIOLEVEL_TIMER 4
#define DISCSTATE_TIMER 5
#define RECORDTIME_TIMER 6
#define PAUSE_TIMER  7
#define ASSISTANT_REGISTRY _T("Software\\imc AG\\LECTURNITY\\Assistant")

#ifdef _STUDIO
IMPLEMENT_DYNCREATE(CMainFrameA, CMDIChildWnd)
#else
IMPLEMENT_DYNCREATE(CMainFrameA, CFrameWnd)
#endif

#ifdef _STUDIO
BEGIN_MESSAGE_MAP(CMainFrameA, CMDIChildWnd)
#else
BEGIN_MESSAGE_MAP(CMainFrameA, CFrameWnd)
#endif
//{{AFX_MSG_MAP(CMainFrameA)
ON_WM_CREATE()
ON_COMMAND(ID_FILE_PACK, OnPackProject)
ON_UPDATE_COMMAND_UI(ID_FILE_PACK, OnUpdatePackProject)
ON_COMMAND(ID_EXPLORE_HOME, OnExploreHome)
ON_UPDATE_COMMAND_UI(ID_EXPLORE_HOME, OnUpdateExploreHome)
ON_COMMAND(ID_SELECT_HOME, OnSelectHome)
ON_UPDATE_COMMAND_UI(ID_SELECT_HOME, OnUpdateSelectHome)
ON_COMMAND(ID_PAGE_NEW, OnNewPage)
ON_UPDATE_COMMAND_UI(ID_PAGE_NEW, OnUpdateNewPage)
ON_COMMAND(ID_PAGE_DUPLICATE, OnDuplicatePage)
ON_UPDATE_COMMAND_UI(ID_PAGE_DUPLICATE, OnUpdateDuplicatePage)
ON_COMMAND(ID_CHAPTER_NEW, OnNewChapter)
ON_UPDATE_COMMAND_UI(ID_CHAPTER_NEW, OnUpdateNewChapter)
ON_WM_CLOSE()
ON_COMMAND(ID_SHOW_IMC, CMainFrame::OnShowImc)
ON_COMMAND_RANGE(IDS_SHOW_IMC_WEBPAGE_TUTORIALS,IDS_SHOW_IMC_WEBPAGE_BUYNOW, CMainFrame::OnShowImcPages)

ON_COMMAND(ID_HELP, OnHelp)
ON_UPDATE_COMMAND_UI(ID_HELP, OnUpdateHelp)
ON_COMMAND(ID_FULL_SCREEN, OnFullScreen)
ON_COMMAND(ID_FULL_SCREEN_CS, OnFullScreenCS)
ON_COMMAND(ID_ACC_VK_ESC, OnAccVkESC)
ON_COMMAND(ID_SHOW_STRUCTURE, OnShowStructure)
ON_UPDATE_COMMAND_UI(ID_SHOW_STRUCTURE, OnUpdateShowStructure)
ON_COMMAND(ID_SHOW_RECORDINGS, OnShowRecordings)
ON_UPDATE_COMMAND_UI(ID_SHOW_RECORDINGS, OnUpdateShowRecordings)
ON_UPDATE_COMMAND_UI(ID_TOOL_SHOW_STRUCT_REC, OnUpdateToolShowStructRec)
ON_COMMAND(ID_PAGE_FOCUSED_LAYOUT, OnButtonPageFocusedLayout)
ON_UPDATE_COMMAND_UI(ID_PAGE_FOCUSED_LAYOUT, OnUpdatePageFocusedLayout)
ON_COMMAND(ID_STANDARD_LAYOUT, OnButtonStandardLayout)
ON_UPDATE_COMMAND_UI(ID_STANDARD_LAYOUT, OnUpdateStandardLayout)
ON_UPDATE_COMMAND_UI(ID_PREVIOUS_PAGE, OnUpdatePreviousPage)
ON_UPDATE_COMMAND_UI(ID_NEXT_PAGE, OnUpdateNextPage)
ON_COMMAND(ID_NEXT_PAGE, OnNextPage)
ON_COMMAND(ID_PREVIOUS_PAGE, OnPreviousPage)
ON_UPDATE_COMMAND_UI(ID_PAGE_DOWN, OnUpdateNextPage)
ON_COMMAND(ID_PAGE_UP, OnPreviousPage)
ON_COMMAND(ID_PAGE_DOWN, OnNextPage)
ON_UPDATE_COMMAND_UI(ID_PAUSE_RECORDING, OnUpdatePauseRecording)
ON_UPDATE_COMMAND_UI(ID_STOP_RECORDING, OnUpdateStopRecording)
ON_COMMAND(ID_SHOW_IMPORT_WARNINGS, OnShowImportWarnings)
ON_UPDATE_COMMAND_UI(ID_SHOW_IMPORT_WARNINGS, OnUpdateShowImportWarnings)
ON_COMMAND(ID_SHOW_AUDIO_WARNING, OnShowAudioWarning)
ON_UPDATE_COMMAND_UI(ID_SHOW_AUDIO_WARNING, OnUpdateShowAudioWarning)
ON_COMMAND(ID_SHOW_USESMARTBOARD, OnShowUseSmartBoard)
ON_UPDATE_COMMAND_UI(ID_SHOW_USESMARTBOARD, OnUpdateShowUseSmartBoard)
ON_COMMAND(IDC_SHOW_STARTPAGE, OnShowStartpage)
ON_UPDATE_COMMAND_UI(IDC_SHOW_STARTPAGE, OnUpdateShowStartpage)
ON_COMMAND(ID_SHOW_MISSING_FONTS, OnShowMissingFonts)
ON_UPDATE_COMMAND_UI(ID_SHOW_MISSING_FONTS, OnUpdateShowMissingFonts)
ON_COMMAND(ID_TOOL_SCREENGRABBING, OnToolScreengrabbing)
ON_UPDATE_COMMAND_UI(ID_TOOL_SCREENGRABBING, OnUpdateToolScreengrabbing)
ON_COMMAND(ID_SETUP_PAGE, OnSetupPage)
ON_UPDATE_COMMAND_UI(ID_SETUP_PAGE, OnUpdateSetupPage)
ON_COMMAND(ID_CHANGE_AUDIOVIDEO, OnChangeAudiovideo)
ON_UPDATE_COMMAND_UI(ID_CHANGE_AUDIOVIDEO, OnUpdateChangeAudiovideo)
ON_COMMAND(ID_CHANGE_SG_OPTIONS, OnChangeSGOptions)
ON_UPDATE_COMMAND_UI(ID_CHANGE_SG_OPTIONS, OnUpdateChangeSGOptions)
ON_COMMAND(ID_SHOW_PROPERTIES, OnShowProperties)
#ifndef _STUDIO
ON_COMMAND(ID_LSD_CLOSE, OnLsdClose)
ON_UPDATE_COMMAND_UI(ID_LSD_CLOSE, OnUpdateLsdClose)
#else
ON_COMMAND(ID_CLOSE, OnLsdClose)
ON_UPDATE_COMMAND_UI(ID_CLOSE, OnUpdateLsdClose)
#endif
ON_COMMAND(ID_LSD_NEW, OnLsdNew)
ON_UPDATE_COMMAND_UI(ID_LSD_NEW, OnUpdateLsdNew)
ON_COMMAND(ID_LSD_OPEN, OnLsdOpen)
ON_COMMAND(ID_PPT_IMPORT, OnImportPPT)
ON_UPDATE_COMMAND_UI(ID_PPT_IMPORT, OnUpdateLsdOpen)
ON_COMMAND(IDC_IMPORT_POWERPOINT_AS, OnImportPPTAs)
ON_UPDATE_COMMAND_UI(IDC_IMPORT_POWERPOINT_AS, OnUpdateImportPowerPointAs)
ON_COMMAND(IDC_RECORDPOWERPOINT, OnImportPPTAndRecord)
ON_COMMAND(ID_LSD_SAVE, OnLsdSave)
ON_UPDATE_COMMAND_UI(ID_LSD_SAVE, OnUpdateLsdSave)
ON_COMMAND(ID_LSD_SAVE_AS, OnLsdSaveAs)
ON_COMMAND(ID_FILE_OPEN, OnProjectOpen)
ON_COMMAND(ID_EDIT_CUT, OnCutObjects)
ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCutCopy)
ON_COMMAND(ID_EDIT_DELETE, OnDeleteObjects)
ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateDeleteCopy)
ON_COMMAND(ID_EDIT_COPY, OnCopyObjects)
ON_COMMAND(ID_EDIT_PASTE, OnPasteObjects)
ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePaste)
ON_WM_TIMER()
ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
ON_COMMAND(ID_FREEZE_IMAGES, OnFreezeImages)
ON_UPDATE_COMMAND_UI(ID_FREEZE_IMAGES, OnUpdateFreezeImages)
ON_COMMAND(ID_FREEZE_MASTER, OnFreezeMaster)
ON_UPDATE_COMMAND_UI(ID_FREEZE_MASTER, OnUpdateFreezeMaster)
ON_COMMAND(IDC_FREEZE_DOCUMENT, OnFreezeDocument)
ON_UPDATE_COMMAND_UI(IDC_FREEZE_DOCUMENT, OnUpdateFreezeDocument)
ON_MESSAGE(WM_HOTKEY, OnHotkeys)
ON_COMMAND(ID_USE_SMARTBOARD, OnUseSmartboard)
ON_UPDATE_COMMAND_UI(ID_USE_SMARTBOARD, OnUpdateUseSmartboard)
ON_COMMAND(ID_START_RECORDING, OnStartRecording)
ON_COMMAND(ID_START_RECORDING_STARTUP_PAGE, OnToolScreengrabbing)

ON_COMMAND(ID_PAUSE_RECORDING, OnPauseRecording)
ON_COMMAND(ID_STOP_RECORDING, OnStopRecording)
ON_UPDATE_COMMAND_UI(ID_LSD_OPEN, OnUpdateLsdOpen)
ON_UPDATE_COMMAND_UI(ID_LSD_SAVE_AS, OnUpdateLsdSave)
ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCutCopy)
ON_WM_SHOWWINDOW()
ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, OnUpdateViewGrid)

ON_COMMAND(IDC_CLOSESTARTPAGE, OnCloseStartpage)
ON_COMMAND(ID_VIEW_PRESENTATION, OnPresentationButton)
//}}AFX_MSG_MAP
#ifndef _STUDIO
ON_XTP_CREATECONTROL()
ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)
#endif

ON_MESSAGE(WM_USER_STANDARD_LAYOUT, OnStandardLayout)
ON_MESSAGE(WM_USER_PAGEFOCUSED_LAYOUT, OnPageFocusedLayout)
ON_MESSAGE(WM_USER_FULLSCREEN_LAYOUT, OnButtonFullScreenCS)

ON_MESSAGE(WM_USER_AUDIO_SETTINGS, OnChangeAudio)
ON_MESSAGE(WM_USER_VIDEO_SETTINGS, OnChangeVideo)
//ribbon messages start
ON_COMMAND(ID_MENU_NEW, OnMenuNew)
ON_UPDATE_COMMAND_UI(ID_MENU_NEW, OnUpdateMenuNew)
ON_COMMAND(ID_MENU_OPEN, OnMenuOpen)
ON_UPDATE_COMMAND_UI(ID_MENU_OPEN, OnUpdateMenuOpen)
ON_COMMAND(ID_MENU_SAVE, OnMenuSave)
ON_UPDATE_COMMAND_UI(ID_MENU_SAVE, OnUpdateMenuSave)
ON_COMMAND(ID_MENU_SAVE_AS, OnMenuSaveAs)
ON_UPDATE_COMMAND_UI(ID_MENU_SAVE_AS, OnUpdateMenuSaveAs)
ON_COMMAND(ID_MENU_PROJECT_SETTINGS, OnMenuProjectSettings)
ON_UPDATE_COMMAND_UI(ID_MENU_PROJECT_SETTINGS, OnUpdateMenuProjectSettings)
ON_COMMAND(ID_MENU_ASSISTANT_SETTINGS, OnMenuAssistantSettings)
ON_UPDATE_COMMAND_UI(ID_MENU_ASSISTANT_SETTINGS, OnUpdateMenuAssistantSettings)
ON_COMMAND(ID_RECORDER_START_STOP, OnRecordAction)

//ON_UPDATE_COMMAND_UI(ID_RECORDER_START_STOP, OnUpdateRecordAction)
ON_UPDATE_COMMAND_UI(ID_START_RECORDING, OnUpdateRecordAction)
ON_UPDATE_COMMAND_UI(ID_START_RECORDING_STARTUP_PAGE, OnUpdateRecordAction)

ON_COMMAND(ID_SCREENGRABBING_AREA, OnToolDefineArea)
ON_UPDATE_COMMAND_UI(ID_SCREENGRABBING_AREA, OnUpdateToolDefineArea)
ON_COMMAND(ID_MENU_REVERT, OnRevert)
ON_UPDATE_COMMAND_UI(ID_MENU_REVERT, OnUpdateRevert)
ON_COMMAND(ID_MENU_IMPORT, OnMenuImport)
#ifndef _STUDIO
ON_UPDATE_COMMAND_UI(XTP_ID_RIBBONCONTROLTAB, OnUpdateRibbonTab)
#endif
ON_COMMAND(ID_MONITOR_OPTION,OnMonitorOption)

ON_COMMAND_RANGE(ID_SWITCH_STANDARD_LAYOUT, ID_SWITCH_FULLSCREENLAYOUT, OnStatusBarSwitchView)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_VIEWSHORTCUTS, OnUpdateStatusBarSwitchView)
#ifndef _STUDIO
ON_XTP_EXECUTE_RANGE(XTP_ID_PIN_FILE_MRU_FIRST,XTP_ID_PIN_FILE_MRU_LAST,OnPinRangeClick)
#endif
ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateStatusBarView)
ON_COMMAND(ID_VIEW_STATUS_BAR, OnCheckBoxShowHideStatusBar)
//ribbon messages end
ON_UPDATE_COMMAND_UI(ID_MONITORWIDGET_SHOWDURATION, OnUpdateMonitorWidget)
ON_UPDATE_COMMAND_UI(ID_MONITORWIDGET_SHOWPAGES, OnUpdateMonitorWidget)
ON_UPDATE_COMMAND_UI(ID_MONITORWIDGET_SHOWDISKSPACE, OnUpdateMonitorWidget)
ON_UPDATE_COMMAND_UI(ID_MONITORWIDGET_LOCKPOSITION, OnUpdateMonitorWidget)
ON_UPDATE_COMMAND_UI(ID_MONITORWIDGET_LIMITTOWINDOW, OnUpdateMonitorWidget)

ON_XTP_EXECUTE(ID_MONITORWIDGET_SHOWDURATION, OnShowMonitorWidget)
ON_XTP_EXECUTE(ID_MONITORWIDGET_SHOWPAGES, OnShowMonitorWidget)
ON_XTP_EXECUTE(ID_MONITORWIDGET_SHOWDISKSPACE, OnShowMonitorWidget)
ON_XTP_EXECUTE(ID_MONITORWIDGET_LOCKPOSITION, OnShowMonitorWidget)
ON_XTP_EXECUTE(ID_MONITORWIDGET_LIMITTOWINDOW, OnShowMonitorWidget)

ON_UPDATE_COMMAND_UI(ID_AUDIOWIDGET_SHOWDURATION, OnUpdateAudioWidget)
ON_UPDATE_COMMAND_UI(ID_AUDIOWIDGET_SHOWDISKSPACE, OnUpdateAudioWidget)
ON_UPDATE_COMMAND_UI(ID_AUDIOWIDGET_LOCKPOSITION, OnUpdateAudioWidget)

ON_XTP_EXECUTE(ID_AUDIOWIDGET_SHOWDURATION, OnShowAudioWidget)
ON_XTP_EXECUTE(ID_AUDIOWIDGET_SHOWDISKSPACE, OnShowAudioWidget)
ON_XTP_EXECUTE(ID_AUDIOWIDGET_LOCKPOSITION, OnShowAudioWidget)

ON_MESSAGE(WM_USER_GOTO_STANDARD_LAYOUT, OnStandardLayout)

ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)

ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POSITION_TOP, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POSITION_RIGHT, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POSITION_BOTTOM, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POSITION_LEFT, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_SHOWTRANSPARENT, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_SMALLICONS, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_AUTOHIDE, OnUpdatePresentationBarAutoHide)

ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PENCILS, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_MARKERS, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_POINTER, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_START, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_STOP, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PAUSERECORDING, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_SCREENGRABBING, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PAGEFOCUSED, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_RECORDINGS, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_CUT, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_COPY, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PASTE, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_UNDO, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_ONECLICKCOPY, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_TEXT, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_LINE, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_POLYLINE, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_FREEHAND, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_ELLIPSE, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_RECTANGLE, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_POLYGON, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_INSERTPAGE, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE, OnUpdatePresentationBar)
ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_NEXTPAGE, OnUpdatePresentationBar)

ON_UPDATE_COMMAND_UI(ID_PRESENTATION_BAR_SHOW, OnUpdatePresentationBarShow)
ON_XTP_EXECUTE(ID_PRESENTATION_BAR_SHOW, OnShowPresentationBar)
ON_UPDATE_COMMAND_UI(ID_PRESENTATION_BAR_AUTOHIDE, OnUpdatePresentationBarAutoHide)
ON_XTP_EXECUTE(ID_PRESENTATION_BAR_AUTOHIDE, OnAutoHidePresentationBar)


ON_XTP_EXECUTE(ID_PRESENTATIONBAR_POSITION_TOP, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_PRESENTATIONBAR_POSITION_RIGHT, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_PRESENTATIONBAR_POSITION_BOTTOM, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_PRESENTATIONBAR_POSITION_LEFT, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_PRESENTATIONBAR_SHOWTRANSPARENT, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_PRESENTATIONBAR_SMALLICONS, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_PRESENTATIONBAR_AUTOHIDE, OnPresentationBarContextMenu)

ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PENCILS, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_MARKERS, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_POINTER, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_START, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_STOP, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PAUSERECORDING, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_SCREENGRABBING, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PAGEFOCUSED, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_RECORDINGS, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_CUT, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_COPY, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PASTE, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_UNDO, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_ONECLICKCOPY, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_TEXT, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_LINE, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_POLYLINE, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_FREEHAND, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_ELLIPSE, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_RECTANGLE, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_POLYGON, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_INSERTPAGE, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE, OnPresentationBarContextMenu)
ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_NEXTPAGE, OnPresentationBarContextMenu)

ON_UPDATE_COMMAND_UI(ID_GROUP_SHOW_HIDE, OnUpdateShowHideGroup)
ON_UPDATE_COMMAND_UI(ID_VIEW_AUDIOVIDEO_WIDGET, OnUpdateCheckboxShowHideWidget)
ON_COMMAND(ID_VIEW_AUDIOVIDEO_WIDGET, OnCheckboxShowHideWidget)
ON_UPDATE_COMMAND_UI(ID_VIEW_MONITOR_WIDGET, OnUpdateCheckboxShowHideMonitorWidget)
ON_COMMAND(ID_VIEW_MONITOR_WIDGET, OnCheckboxShowHideMonitorWidget)
ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_STARTUP_PAGE, OnUpdateCheckboxShowHideStartupPage)
ON_COMMAND(ID_VIEW_SHOW_STARTUP_PAGE, OnCheckboxShowHideStartupPage)
ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
ON_WM_GETMINMAXINFO()

ON_MESSAGE_VOID(WM_FINISH_SGSELECTION, FinishSgSelectionFromPresentationBar)

ON_WM_DESTROY()
#ifdef _STUDIO
ON_WM_MDIACTIVATE()
#endif
END_MESSAGE_MAP()

static UINT indicators[] =
{
   ID_STATUS_DISKSPACE,
      ID_STATUS_DURATION
};

static int aiFontSize [22] = { 8, 9, 10, 12, 14, 16, 18, 20, 24, 28, 32,
36, 40, 44, 48, 54, 60, 66, 72, 80, 88, 96};

// static
CDocTemplate *CMainFrameA::GetAssociatedTemplate()
{
    CDocTemplate *pAssistantDocTemplate = NULL;

    CWinApp *pApp = AfxGetApp();
    if (pApp) {
        POSITION templPos = pApp->GetFirstDocTemplatePosition();

#ifdef _STUDIO
        while (templPos != NULL) {
            CDocTemplate *pDocTemplate = pApp->GetNextDocTemplate(templPos);
            CString csDocString;
            pDocTemplate->GetDocString(csDocString, CDocTemplate::windowTitle);
            if (csDocString == _T("Assistant")) {
                pAssistantDocTemplate = pDocTemplate;
                break;
            }
        }
#else
        pAssistantDocTemplate = pApp->GetNextDocTemplate(templPos);
#endif
    }

    return pAssistantDocTemplate;
}

// static
CAssistantDoc *CMainFrameA::GetAssistantDocument()
{
    CAssistantDoc *pAssistantDoc = NULL;

    CDocTemplate *pDocTemplate = GetAssociatedTemplate();
    if (pDocTemplate != NULL) {
        POSITION docPos = pDocTemplate->GetFirstDocPosition();
        while (docPos) {
            CDocument *pDocument = pDocTemplate->GetNextDoc(docPos); 
            if (pDocument != NULL && pDocument->GetRuntimeClass() == RUNTIME_CLASS(CAssistantDoc)) {
                pAssistantDoc = (CAssistantDoc *)pDocument;
                break;
            }
        }
    }

    return pAssistantDoc;
}

// static
CAssistantView *CMainFrameA::GetAssistantView()
{ 
    CAssistantView *pAssistantView = NULL;

    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
    {
        POSITION viewPos = pAssistantDoc->GetFirstViewPosition();
        while (viewPos) {
            CView *pView = pAssistantDoc->GetNextView(viewPos);
            if (pView != NULL && pView->GetRuntimeClass() == RUNTIME_CLASS(CAssistantView)) {
                pAssistantView = (CAssistantView *)pView;
                break;
            }
        }
    }

    return pAssistantView;
}

// static
CStartupView *CMainFrameA::GetStartupView()
{ 
    CStartupView *pStartupView = NULL;

    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
    {
        POSITION viewPos = pAssistantDoc->GetFirstViewPosition();
        while (viewPos) {
            CView *pView = pAssistantDoc->GetNextView(viewPos);
            if (pView != NULL && pView->GetRuntimeClass() == RUNTIME_CLASS(CStartupView)) {
                pStartupView = (CStartupView *)pView;
                break;
            }
        }
    }

    return pStartupView;
}


CMainFrameA *CMainFrameA::s_pMainFrame = NULL;

// static
CMainFrameA* CMainFrameA::GetCurrentInstance() 
{
    return s_pMainFrame;

    /*
    CView *pView = GetAssistantView();
    
    if (pView == NULL)
        pView = GetStartupView();

    if (pView) {
        CFrameWnd *pChildFrame = pView->GetParentFrame();
        if (pChildFrame != NULL && 
            pChildFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrameA)) {
                return (CMainFrameA *)pChildFrame;
        }
    }

    return NULL;
    */
}

//Create Start tab
void CMainFrameA::CreateTabStart(CXTPCommandBars *pCommandBars, CXTPRibbonBar* pRibbon)
{
    CXTPRibbonTab* pTabStart = pRibbon->AddTab(ID_TAB_START);

    // Group Recorder
    CXTPRibbonGroup* pGroupRecorder = pTabStart->AddGroup(ID_GROUP_RECORDER);
    // Add Start/Stop Record button
    CXTPControl* pRecordAction = pGroupRecorder->Add(xtpControlButton, /*ID_RECORDER_START_STOP */ID_START_RECORDING);
    pRecordAction->SetStyle(xtpButtonIconAndCaptionBelow);
    // Add Stop Record button
    CXTPControl* pRecordStopAction = pGroupRecorder->Add(xtpControlButton,  ID_STOP_RECORDING);
    pRecordStopAction->SetStyle(xtpButtonIconAndCaptionBelow);
    // Add Pause recording button
    CXTPControl* pPauseRecording = pGroupRecorder->Add(xtpControlButton, ID_PAUSE_RECORDING);
    pPauseRecording->SetStyle(xtpButtonIconAndCaptionBelow);
    // Group Recorder end

    // Group Start Presentation
    CXTPRibbonGroup* pGroupStartPresentation = pTabStart->AddGroup(ID_GROUP_START_PRESENTATION);
    // Add Start From Current Slide button
    CXTPControl* pControlStartFromBeggining = pGroupStartPresentation->Add(xtpControlButton, ID_FULL_SCREEN);
    pControlStartFromBeggining->SetStyle(xtpButtonIconAndCaptionBelow);
    // Add Start From Beggining button
    CXTPControl* pControlStartFromCurrentSlide = pGroupStartPresentation->Add(xtpControlButton, ID_FULL_SCREEN_CS);
    pControlStartFromCurrentSlide->SetStyle(xtpButtonIconAndCaptionBelow);
    // End Group Start Presentation

    // Group Presentation-Tools
    CXTPRibbonGroup* pGroupPresentation = pTabStart->AddGroup(ID_GROUP_PRESENTATION_TOOLS);
    pGroupPresentation->SetControlsCentering();

    // Create Markers popup button with gallery
    // Create Menu for Markers button. Markers button will have a menu attached.
    CMenu menuMarkers;
    menuMarkers.LoadMenu(ID_MENU_MARKERS);

    CXTPControlPopup* pControlMarkers = (CXTPControlPopup*)pGroupPresentation->Add(xtpControlSplitButtonPopup, ID_BUTTON_MARKERS);
    pControlMarkers->SetStyle(xtpButtonIconAndCaptionBelow);

    CXTPPopupBar *pPopupBarMarkers = CXTPPopupBar::CreatePopupBar(pCommandBars);
    pPopupBarMarkers->LoadMenu(menuMarkers.GetSubMenu(0));

    pControlMarkers->SetCommandBar(pPopupBarMarkers);
    pPopupBarMarkers->InternalRelease();
    //End Create Markers popup button with gallery

    // Create Pens popup button with gallery
    // Create Menu for pens button. Pens button will have a menu attached.
    CMenu menuPens;
    menuPens.LoadMenu(ID_MENU_PENS);

    CXTPControlPopup* pControlPens = (CXTPControlPopup*)pGroupPresentation->Add(xtpControlSplitButtonPopup, ID_BUTTON_PENS);
    pControlPens->SetIconSize(CSize(32, 32));
    pControlPens->SetStyle(xtpButtonIconAndCaptionBelow);

    CXTPPopupBar* pPopupBarPens = CXTPPopupBar::CreatePopupBar(pCommandBars);
    pPopupBarPens->LoadMenu(menuPens.GetSubMenu(0));

    pControlPens->SetCommandBar(pPopupBarPens);
    pPopupBarPens->InternalRelease();
    //End Create Pens popup button with gallery


    // Add Pointer button
    CXTPControl *pPointer = pGroupPresentation->Add(xtpControlButton, ID_TOOL_TELEPOINTER);
    pPointer->SetStyle(xtpButtonIconAndCaptionBelow);
    // Add SimpleNavigation button
    CXTPControl *pSimplifiedNavigation = pGroupPresentation->Add(xtpControlButton, ID_TOOL_SIMPLENAVIGATION);
    pSimplifiedNavigation->SetStyle(xtpButtonIconAndCaptionBelow);
    //group Presentation-Tools end

    // Group Recorder
    CXTPRibbonGroup* pGroupScreenGrabbing = pTabStart->AddGroup(ID_GROUP_SCREENGRABBING);
    pGroupScreenGrabbing->ShowOptionButton();
    pGroupScreenGrabbing->GetControlGroupOption()->SetID(ID_CHANGE_SG_OPTIONS);
    pGroupScreenGrabbing->SetControlsCentering();

    // Add Screen Grabbing button
    CXTPControl* pStartScreenGrabbing = pGroupScreenGrabbing->Add(xtpControlButton, ID_TOOL_SCREENGRABBING);
    pStartScreenGrabbing->SetStyle(xtpButtonIconAndCaptionBelow);

    // Group Recording
    CXTPRibbonGroup* pGroupRecording = pTabStart->AddGroup(ID_GROUP_RECORDING);
    pGroupRecording->SetControlsCentering();
    pGroupRecording->Add(xtpControlButton, IDC_REPLAY);
    pGroupRecording->Add(xtpControlButton, IDC_EDIT);
    pGroupRecording->Add(xtpControlButton, IDC_PUBLISH);
    pGroupRecording->SetIconId(IDC_REPLAY);
    // Group Recorder end  

    // Group Insert
    CXTPRibbonGroup* pGroupInsert = pTabStart->AddGroup(ID_GROUP_INSERT);
    pGroupInsert->SetControlsCentering();
    pGroupInsert->Add(xtpControlButton, ID_PAGE_NEW);
    pGroupInsert->Add(xtpControlButton, ID_CHAPTER_NEW);

}

//Create Tools tab
void CMainFrameA::CreateTabTools(CXTPCommandBars *pCommandBars, CXTPRibbonBar* pRibbon)
{
   CXTPRibbonTab* pTabTools = pRibbon->AddTab(ID_TAB_TOOLS);

   // Group Clipboard
   CXTPRibbonGroup* pGroupClipboard = pTabTools->AddGroup(ID_GROUP_CLIPBOARD);
   pGroupClipboard->SetControlsCentering();

   CXTPControl* pControlPaste = (CXTPControl*)pGroupClipboard->Add(xtpControlButton, ID_EDIT_PASTE);

   CXTPControl* pControlCut = pGroupClipboard->Add(xtpControlButton, ID_EDIT_CUT);
   pControlCut->SetStyle(xtpButtonIconAndCaption);
   pControlCut->SetIconSize(CSize(16, 16));

   CXTPControl* pControlCopy = pGroupClipboard->Add(xtpControlButton, ID_EDIT_COPY);
   pControlCopy->SetStyle(xtpButtonIconAndCaption);
   pControlCopy->SetIconSize(CSize(16, 16));

   pControlCopy = pGroupClipboard->Add(xtpControlButton, ID_TOOL_COPY);
   pControlCopy->SetStyle(xtpButtonIconAndCaption);
   pControlCopy->SetIconSize(CSize(16, 16));

   // Group Font
   // Font family
   CXTPRibbonGroup* pGroupFont = pTabTools->AddGroup(ID_GROUP_FONT);
   pGroupFont->SetControlsGrouping();

   CXTPControlComboBox* pFontCombo = new CXTPControlComboBox();
   pFontCombo->SetDropDownListStyle();
   pFontCombo->EnableAutoComplete();
   pFontCombo->SetWidth(163);
   pGroupFont->Add(pFontCombo, ID_FONTFAMILY);

   CXTPPopupBar* pPopupBar = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
   pFontCombo->SetCommandBar(pPopupBar);

   CXTPControlGallery *pControlGallery = new CXTPControlGallery();
   pControlGallery->SetControlSize(CSize(290, 508));
   pControlGallery->SetResizable(FALSE, TRUE);
   pControlGallery->ShowLabels(TRUE);


   CXTPControlGalleryItems* pItemsFontFace = CXTPControlGalleryItems::CreateItems(pCommandBars, ID_FONT_FAMILY_LIST);
   pItemsFontFace->SetItemSize(CSize(0, 26));
   CGalleryItemFontFace::AddFontItems(pItemsFontFace);


   pControlGallery->SetItems(pItemsFontFace);
   pPopupBar->GetControls()->Add(pControlGallery, ID_FONT_FAMILY_LIST);
   pPopupBar->InternalRelease();

   // Font style
   CXTPControl* pFontStyle = pGroupFont->Add(xtpControlButton, ID_FONTWEIGHT);
   pFontStyle->SetBeginGroup(true);
   pGroupFont->Add(xtpControlButton, ID_FONTSLANT);
   pGroupFont->Add(xtpControlButton, ID_FONT_SIZE_INCREASE);
   pGroupFont->Add(xtpControlButton, ID_FONT_SIZE_DECREASE);

   // Text color
   CXTPControlPopupColor* pPopupColorTxt = new CXTPControlPopupColor();
   pPopupColorTxt->SetBeginGroup(true);
   CXTPPopupBar* pColorBarTxt = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   pColorBarTxt->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_TEXT_COLOR);
   CXTPControlColorSelector *pControlColorSelectorTxt = new CXTPControlColorSelector();
   pControlColorSelectorTxt->SetBeginGroup(TRUE);
   pColorBarTxt->GetControls()->Add(pControlColorSelectorTxt, ID_SELECTOR_TEXT_COLOR);
   CXTPControlButtonColor *pControlButtonColorTxt = new CXTPControlButtonColor();
   pControlButtonColorTxt->SetBeginGroup(TRUE);
   pColorBarTxt->GetControls()->Add(pControlButtonColorTxt, IDS_MORE_TEXT_COLORS);

   pPopupColorTxt->SetCommandBar(pColorBarTxt);

   pColorBarTxt->SetTearOffPopup(_T("Text Color"), IDR_TEXTCOLOR_POPUP, 0);
   pColorBarTxt->EnableCustomization(FALSE);
   pColorBarTxt->InternalRelease();

   pGroupFont->Add(pPopupColorTxt, ID_TEXT_COLOR);

   // Text size
   CXTPControlComboBox* pComboSize = new CXTPControlComboBox();
   pComboSize->SetBeginGroup(true);
   pComboSize->SetDropDownListStyle();
   pComboSize->SetWidth(35);
   pComboSize->SetEditText(_T("10"));
   pGroupFont->Add(pComboSize, ID_FONTSIZE);

   pPopupBar = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
   pComboSize->SetCommandBar(pPopupBar);

   pControlGallery = new CXTPControlGallery();
   pControlGallery->SetControlSize(CSize(42, 16 * 17));
   pControlGallery->SetResizable(FALSE, TRUE);

   CXTPControlGalleryItems* pItemsFontSize = CXTPControlGalleryItems::CreateItems(pCommandBars, ID_FONT_SIZE_LIST);
   pItemsFontSize->SetItemSize(CSize(0, 17));
   for(int i = 0; i < 22; i++)
   {
      CString s;
      s.Format(_T("%d"), aiFontSize[i]);
      pItemsFontSize->AddItem(s);
   }

   pControlGallery->SetItems(pItemsFontSize);

   pPopupBar->GetControls()->Add(pControlGallery, ID_FONT_SIZE_LIST);
   pPopupBar->InternalRelease();	

   // Group Styles
   // Fill color
   CXTPRibbonGroup* pGroupStyles = pTabTools->AddGroup(ID_GROUP_STYLES);
   pGroupStyles->SetControlsCentering();

   CXTPControlPopupColor* pPopupColor = new CXTPControlPopupColor();
   CXTPPopupBar* pColorBar= (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   pColorBar->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_FILL_COLOR);
   CXTPControlColorSelector *pControlColorSelector = new CXTPControlColorSelector();
   pControlColorSelector->SetBeginGroup(TRUE);
   pColorBar->GetControls()->Add(pControlColorSelector, ID_SELECTOR_FILL_COLOR);
   CXTPControlButtonColor *pControlButtonColor = new CXTPControlButtonColor();
   pControlButtonColor->SetBeginGroup(TRUE);
   pColorBar->GetControls()->Add(pControlButtonColor, IDS_MORE_FILL_COLORS);

   pPopupColor->SetCommandBar(pColorBar);

   pColorBar->SetTearOffPopup(_T("Fill Color"), IDR_FILLCOLOR_POPUP, 0);
   pColorBar->EnableCustomization(FALSE);
   pColorBar->InternalRelease();

   pGroupStyles->Add(pPopupColor, ID_FILL_COLOR);

   // Line color
   CXTPControlPopupColor* pPopupColorLine = new CXTPControlPopupColor();
   CXTPPopupBar* pColorBarLine = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   //pColorBarLine->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_LINE_COLOR);
   CXTPControlColorSelector *pControlColorSelectorLine = new CXTPControlColorSelector();
   pControlColorSelectorLine->SetBeginGroup(TRUE);
   pColorBarLine->GetControls()->Add(pControlColorSelectorLine, ID_SELECTOR_LINE_COLOR);
   CXTPControlButtonColor *pControlButtonColorLine = new CXTPControlButtonColor();
   pControlButtonColorLine->SetBeginGroup(TRUE);
   pColorBarLine->GetControls()->Add(pControlButtonColorLine, IDS_MORE_LINE_COLORS);

   pPopupColorLine->SetCommandBar(pColorBarLine);

   pColorBarLine->SetTearOffPopup(_T("Line Color"), IDR_LINECOLOR_POPUP, 0);
   pColorBarLine->EnableCustomization(FALSE);
   pColorBarLine->InternalRelease();

   pGroupStyles->Add(pPopupColorLine, ID_LINE_COLOR);

   // Line width
   CXTPControlPopup *pControlPopupLineWidth = CXTPControlPopup::CreateControlPopup(xtpControlPopup);
   CXTPPopupBar* pLineWidthBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CControlLineWidthSelector *pControlLineWidthSelector = new CControlLineWidthSelector();
   pControlLineWidthSelector->SetBeginGroup(TRUE);
   pLineWidthBar->GetControls()->Add(pControlLineWidthSelector, ID_SELECTOR_LINE_WIDTH);
   pControlPopupLineWidth->SetCommandBar(pLineWidthBar);

   pLineWidthBar->EnableCustomization(FALSE);
   pLineWidthBar->InternalRelease(); 
   pGroupStyles->Add(pControlPopupLineWidth, ID_LINE_WIDTH)->SetStyle(xtpButtonIcon);

   // Line style
   CXTPControlPopup *pControlPopupLineStyle = CXTPControlPopup::CreateControlPopup(xtpControlPopup);
   CXTPPopupBar* pLineStyleBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CControlLineStyleSelector *pControlLineStyleSelector = new CControlLineStyleSelector();
   pLineStyleBar->GetControls()->Add(pControlLineStyleSelector, ID_SELECTOR_LINE_STYLE);
   pControlPopupLineStyle->SetCommandBar(pLineStyleBar);

   pLineStyleBar->EnableCustomization(FALSE);
   pLineStyleBar->InternalRelease();
   pGroupStyles->Add(pControlPopupLineStyle, ID_LINE_STYLE)->SetStyle(xtpButtonIcon);

   // Arrow style
   CXTPControlPopup *pControlPopupArrowStyle = CXTPControlPopup::CreateControlPopup(xtpControlPopup);
   CXTPPopupBar* pArrowStyleBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CControlArrowStyleSelector *pControlArrowStyleSelector = new CControlArrowStyleSelector();
   pArrowStyleBar->GetControls()->Add(pControlArrowStyleSelector, ID_SELECTOR_ARROW_STYLE);
   pControlPopupArrowStyle->SetCommandBar(pArrowStyleBar);

   pArrowStyleBar->EnableCustomization(FALSE);
   pArrowStyleBar->InternalRelease();
   pGroupStyles->Add(pControlPopupArrowStyle, ID_ARROW_STYLE)->SetStyle(xtpButtonIcon);

   // Group Lines
   CXTPRibbonGroup* pGroupLines = pTabTools->AddGroup(ID_GROUP_LINES);
   pGroupLines->SetControlsCentering();

   CXTPControl* pControl;
   pControl = pGroupLines->Add(xtpControlButton, ID_TOOL_LINE);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetIconSize(CSize(16, 16));
   pControl = pGroupLines->Add(xtpControlButton, ID_TOOL_POLYLINE);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetIconSize(CSize(16, 16));
   pControl = pGroupLines->Add(xtpControlButton, ID_TOOL_FREEHAND);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetIconSize(CSize(16, 16));

   // Group Shapes
   CXTPRibbonGroup* pGroupShapes = pTabTools->AddGroup(ID_GROUP_SHAPES);
   pGroupShapes->SetControlsCentering();
   pControl = pGroupShapes->Add(xtpControlButton, ID_TOOL_OVAL);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetIconSize(CSize(16, 16));
   pControl = pGroupShapes->Add(xtpControlButton, ID_TOOL_RECTANGLE);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetIconSize(CSize(16, 16));
   pControl = pGroupShapes->Add(xtpControlButton, ID_TOOL_POLYGON);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetIconSize(CSize(16, 16));

   // Group Insert
   CXTPRibbonGroup* pGroupInsert = pTabTools->AddGroup(ID_GROUP_INSERT);
   pGroupInsert->SetControlsCentering();
   pGroupInsert->Add(xtpControlButton, ID_PAGE_NEW);
   pGroupInsert->Add(xtpControlButton, ID_CHAPTER_NEW);
   pGroupInsert->Add(xtpControlButton, ID_INSERT_IMAGE);
   pGroupInsert->Add(xtpControlButton, ID_TOOL_TEXT);

   // Group Explore
   CXTPRibbonGroup* pGroupExplore = pTabTools->AddGroup(ID_GROUP_EXPLORE);
   pGroupExplore->SetControlsCentering();
   // Add explore button
   CXTPControl *pExploreHome = pGroupExplore->Add(xtpControlButton, ID_EXPLORE_HOME);
   pExploreHome->SetCaption(IDS_EXPLORE_HOME);
}

//Create View tab
void CMainFrameA::CreateTabView(CXTPCommandBars *pCommandBars, CXTPRibbonBar* pRibbon)
{
    CXTPRibbonTab* pTabTools = pRibbon->AddTab(ID_TAB_VIEW);

    // Group View
    CXTPRibbonGroup* pGroupView = pTabTools->AddGroup(ID_GROUP_VIEW);

    // Add Standard Layout button
    CXTPControl* pControlStandardLayout = pGroupView->Add(xtpControlButton, ID_STANDARD_LAYOUT);
    pControlStandardLayout->SetStyle(xtpButtonIconAndCaptionBelow);
    pControlStandardLayout->SetCaption(IDS_STANDARD_LAYOUT);

    // Add Page Focused Layout button
    CXTPControl* pControlPageFocusedLayout = pGroupView->Add(xtpControlButton, ID_PAGE_FOCUSED_LAYOUT);
    pControlPageFocusedLayout->SetStyle(xtpButtonIconAndCaptionBelow);
    pControlPageFocusedLayout->SetCaption(IDS_PAGE_FOCUSED_LAYOUT);

    // Add Presentation Popup Button
    CXTPControlPopup *pControlPresentation = CXTPControlPopup::CreateControlPopup(xtpControlSplitButtonPopup/*xtpControlButtonPopup*/);
    CXTPPopupBar* pPresentationBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

    CMenu menu;
    menu.LoadMenu(IDR_SUBMENU_VIEW_PRESENTATION);

    pPresentationBar->SetCommandBars(pCommandBars);
    pPresentationBar->LoadMenu(menu.GetSubMenu(0));
    pPresentationBar->SetShowGripper(FALSE);
    pPresentationBar->SetDefaultButtonStyle(xtpButtonIconAndCaption);

    pControlPresentation->SetCommandBar(pPresentationBar);

    pPresentationBar->EnableCustomization(FALSE);
    pPresentationBar->InternalRelease(); 
    pPresentationBar->SetIconSize(CSize(16,16));

    pGroupView->Add(pControlPresentation, ID_VIEW_PRESENTATION)->SetStyle(xtpButtonIconAndCaptionBelow);
    pControlPresentation->SetCaption(ID_VIEW_PRESENTATION);
    // End Group View

    // Group Show Hide
    CXTPRibbonGroup* pGroupShowHide = pTabTools->AddGroup(ID_GROUP_SHOW_HIDE);

    // Add Show Document Structure Check Box
    CXTPControl* pControlShowDocumentStructure = pGroupShowHide->Add(xtpControlCheckBox, ID_SHOW_STRUCTURE);
    pControlShowDocumentStructure->SetFlags(xtpFlagRightAlign);
    pControlShowDocumentStructure->SetCaption(IDS_SHOW_STRUCTURE);
    //pControlDocumentStructure->SetStyle();

    // Add Show Recordings Check Box
    CXTPControl* pControlShowRecordings = pGroupShowHide->Add(xtpControlCheckBox, ID_SHOW_RECORDINGS);
    pControlShowRecordings->SetFlags(xtpFlagRightAlign);
    pControlShowRecordings->SetCaption(IDS_SHOW_RECORDINGS);

    // Add Show Status Bar Check Box
    CXTPControl* pControlShowStatusBar = pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_STATUS_BAR);
    pControlShowStatusBar->SetFlags(xtpFlagRightAlign);
    pControlShowStatusBar->SetCaption(IDS_VIEW_STATUS_BAR);
    // End Group Show Hide

    // Group Presentation
    CXTPRibbonGroup* pGroupPresentation = pTabTools->AddGroup(ID_GROUP_PRESENTATION);
    pGroupPresentation->ShowOptionButton();
    pGroupPresentation->GetControlGroupOption()->SetID(ID_MONITOR_OPTION);

    // Add Display Presentation Bar Check Box
    CXTPControl* pControlDisplayPresentationBar = pGroupPresentation->Add(xtpControlCheckBox, ID_PRESENTATION_BAR_SHOW);
    pControlDisplayPresentationBar->SetFlags(xtpFlagRightAlign);
    pControlDisplayPresentationBar->SetChecked(TRUE);
    //pControlDocumentStructure->SetStyle();

    // Add Auto Hide Presentation Bar Check Box
    CXTPControl* pControlAutoHidePresentationBar = pGroupPresentation->Add(xtpControlCheckBox, ID_PRESENTATION_BAR_AUTOHIDE);
    pControlAutoHidePresentationBar->SetFlags(xtpFlagRightAlign);

    // Add Hide Mouse Cursor Within Page View Check Box
    CXTPControl* pControlHideMouseCursor = pGroupPresentation->Add(xtpControlCheckBox, ID_HIDE_MOUSE_POINTER);
    pControlHideMouseCursor->SetFlags(xtpFlagRightAlign);
    pControlHideMouseCursor->SetChecked(true);
    // End Group Presentation


    // Group Grid
    CXTPRibbonGroup* pGroupGrid = pTabTools->AddGroup(ID_GROUP_GRID);

    // Add Display Grid Check Box
    CXTPControl* pControlDisplayGrid = pGroupGrid->Add(xtpControlCheckBox, ID_GRID_DISPLAY);
    pControlDisplayGrid->SetFlags(xtpFlagNoMovable);
    pControlDisplayGrid->SetHeight(40);

    // Add Grid Spacping label
    CXTPControl* pControlGridSpacingLabel = pGroupGrid->Add(xtpControlLabel, IDS_GRID_SPACING);
    pControlGridSpacingLabel->SetFlags(xtpFlagRightAlign | xtpFlagNoMovable);

    // Add Snap to Grid Check Box
    CXTPControl* pControlSnapToGridCheckBox = pGroupGrid->Add(xtpControlCheckBox, ID_GRID_SNAP);
    pControlSnapToGridCheckBox->SetFlags(xtpFlagNoMovable);
    pControlSnapToGridCheckBox->SetHeight(40);

    // Add Grid spacing edit control
    CXTPControlEdit* pControlEditSpacing = (CXTPControlEdit*)pGroupGrid->Add(xtpControlEdit, ID_GRID_SPACING);
    pControlEditSpacing->SetWidth(60);
    pControlEditSpacing->ShowSpinButtons();

    // End Group Grid


    // Group Lock Elements
    CXTPRibbonGroup* pGroupLockElements = pTabTools->AddGroup(ID_GROUP_LOCK_ELEMENTS);

    // Add Lock Images Check Box
    CXTPControl* pControlLockImages = pGroupLockElements->Add(xtpControlCheckBox, ID_FREEZE_IMAGES);
    pControlLockImages->SetFlags(xtpFlagRightAlign);
    pControlLockImages->SetCaption(IDS_FREEZE_IMAGES);
    //pControlDocumentStructure->SetStyle();

    // Add Lock Slide Master Elements Check Box
    CXTPControl* pControlLockSlideMasterElements = pGroupLockElements->Add(xtpControlCheckBox, ID_FREEZE_MASTER);
    pControlLockSlideMasterElements->SetFlags(xtpFlagRightAlign);
    pControlLockSlideMasterElements->SetCaption(IDS_FREEZE_MASTER);

    // Add Lock All Imported Objects Check Box
    CXTPControl* pControlLockImportedObjects = pGroupLockElements->Add(xtpControlCheckBox, IDC_FREEZE_DOCUMENT);
    pControlLockImportedObjects->SetFlags(xtpFlagRightAlign);
    pControlLockImportedObjects->SetCaption(IDS_FREEZE_DOCUMENT);
    // End Group Lock Elements
}


//Load ribbon icons
void CMainFrameA::LoadRibbonIcons(CXTPCommandBars* pCommandBars)
{
   UINT uGear = {IDB_GEAR_ASSISTANT};
   pCommandBars->GetImageManager()->SetIcons(IDB_GEAR_ASSISTANT, &uGear, 1, CSize(0, 0), xtpImageNormal);

   UINT uiSystemMenu1[] = {ID_MENU_NEW, ID_MENU_OPEN, ID_LSD_CLOSE, ID_MENU_SAVE, ID_MENU_SAVE_AS}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu1, _countof(uiSystemMenu1), CSize(32, 32));

   UINT uiSystemMenu2[] = {-1, ID_FILE_OPEN, ID_FILE_PACK, ID_FILE_SAVE, ID_FILE_SAVE_AS}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu2, _countof(uiSystemMenu2), CSize(32, 32));

   UINT uiSystemMenu3[] = {-1, ID_LSD_OPEN, ID_PPT_IMPORT, ID_LSD_SAVE, ID_LSD_SAVE_AS}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu3, _countof(uiSystemMenu3), CSize(32, 32));

   UINT uiSystemMenu4[] = {-1, -1, -1, -1, -1, -1, ID_MENU_PROJECT_SETTINGS, ID_SHOW_PROPERTIES, ID_SETUP_PAGE}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu4, _countof(uiSystemMenu4), CSize(32, 32));
  
   UINT uiSystemMenu5[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, ID_MENU_IMPORT, ID_PPT_IMPORT, IDC_IMPORT_POWERPOINT_AS,ID_MENU_REVERT,ID_FILE_NEW,ID_LSD_NEW}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu5, _countof(uiSystemMenu5), CSize(32, 32));

   UINT uiSystemMenu6[] = {-1, -1, -1, -1, -1, ID_LSD_CLOSE}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu6, _countof(uiSystemMenu6), CSize(32, 32));  
   
   UINT uiTabTools[] = {ID_FULL_SCREEN, ID_FULL_SCREEN_CS,-1,-1,-1,-1,ID_BUTTON_MARKERS,-1,-1,ID_BUTTON_PENS,-1,-1,-1,-1,-1,-1,ID_TOOL_TELEPOINTER,ID_TOOL_SIMPLENAVIGATION, ID_START_RECORDING/*ID_RECORDER_START_STOP*/,ID_PAUSE_RECORDING, -1, ID_TOOL_SCREENGRABBING,ID_STOP_RECORDING};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabTools, _countof(uiTabTools), CSize(32, 32));

   UINT uiTabView[] = {ID_STANDARD_LAYOUT, ID_PAGE_FOCUSED_LAYOUT,ID_VIEW_PRESENTATION};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_VIEW, uiTabView, _countof(uiTabView), CSize(32, 32));

   UINT uSmallIcons[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,ID_APP_ABOUT,ID_APP_EXIT,ID_MENU_ASSISTANT_SETTINGS};
   pCommandBars->GetImageManager()->SetIcons(IDB_SMALL_ICONS, uSmallIcons, _countof(uSmallIcons), CSize(16, 16));

   UINT uTabStartLarge[] = {ID_EDIT_PASTE,ID_FILL_COLOR,ID_LINE_COLOR,ID_PAGE_NEW,ID_CHAPTER_NEW,ID_INSERT_IMAGE,ID_TOOL_TEXT,IDC_REPLAY,IDC_EDIT,IDC_PUBLISH,ID_EXPLORE_HOME};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uTabStartLarge, _countof(uTabStartLarge), CSize(32, 32));

   UINT uTabStartSmall[] = {ID_FONTWEIGHT,ID_FONTSLANT,ID_FONT_SIZE_INCREASE,ID_FONT_SIZE_DECREASE,ID_TEXT_COLOR};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_SMALL, uTabStartSmall, _countof(uTabStartSmall), CSize(16, 16));  

   UINT uTabStartDraw[] = {-1, -1,-1, -1,-1,ID_TOOL_FREEHAND,ID_TOOL_LINE,ID_TOOL_RECTANGLE,ID_TOOL_OVAL,ID_TOOL_POLYLINE, ID_TOOL_POLYGON};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_SMALL, uTabStartDraw, _countof(uTabStartDraw), CSize(16, 16));

   UINT uTabStartLines[] = {-1, -1,-1, -1,-1,-1,-1,-1,-1,-1, -1, -1,-1, -1, ID_LINE_WIDTH,ID_LINE_STYLE, ID_ARROW_STYLE};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_SMALL, uTabStartLines, _countof(uTabStartLines), CSize(16, 16));

   UINT uTabStartEditElem[] = {-1, -1,-1, -1,-1,-1,-1,-1,-1,-1, -1, -1,-1, -1, -1,-1, -1,ID_EDIT_CUT,ID_EDIT_COPY,-1,ID_TOOL_COPY};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_SMALL, uTabStartEditElem, _countof(uTabStartEditElem), CSize(16, 16));


   // Add icons for Tab Start groups so when ribbon size is reduced the coresponding images will be visible
   UINT uGroupClipboard[] = {ID_GROUP_CLIPBOARD, -1, ID_GROUP_STYLES, ID_GROUP_INSERT};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupClipboard, _countof(uGroupClipboard), CSize(32, 32));

   UINT uGroupFont[] = {-1, -1, -1, -1,-1, -1, -1, -1,-1, -1, -1, ID_GROUP_FONT};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupFont, _countof(uGroupFont), CSize(32, 32));

   UINT uGroupStyles[] = {-1, -1, -1 , -1, -1, -1,-1, -1, -1,-1, -1, -1,-1,ID_GROUP_LINES, ID_GROUP_SHAPES};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupStyles, _countof(uGroupStyles), CSize(32, 32));

   UINT uGroupRecords[] = {-1, -1, -1, -1, -1, -1, -1,ID_GROUP_RECORDING};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupRecords, _countof(uGroupRecords), CSize(32, 32));

   UINT uGroupExplore[] = {-1, -1, -1,-1, -1, -1,-1, -1, -1,-1,-1,-1,-1,-1, ID_GROUP_EXPLORE};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupExplore, _countof(uGroupExplore), CSize(32, 32));

   // Add icons for Tab Tools groups so when ribbon size is reduced the coresponding images will be visible
   UINT uGroupsTools[] = {ID_GROUP_START_PRESENTATION, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,ID_GROUP_PRESENTATION_TOOLS,-1,-1,-1,-1,ID_TOOL_TELEPOINTER,-1, ID_GROUP_RECORDER,-1, -1, ID_GROUP_SCREENGRABBING};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uGroupsTools, _countof(uGroupsTools), CSize(32, 32));
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrameA construction/destruction

CMainFrameA::CMainFrameA()
{
    s_pMainFrame = this;
    m_mainRect = CRect(0,0,0,0);

   bool success = ASSISTANT::GetLecturnityHome(m_csLecHome);
   if (!success)
      m_csLecHome = "";
      
      
   m_bMinimized = false;
   
   m_iSelectedPresentationTool = 0;
   m_iPreviousLayout = 0;
   
   m_nCurrentLayout = AssistantLayouts(StandardLayout);
   m_nPreviousLayout = AssistantLayouts(StandardLayout);
   
   m_nAudioTimer = 0;
   m_nVideoTimer = 0;
   m_nCheckDiskSpace = 0;
   m_nCheckAudioLevel = 0;
   m_nUpdateDiscSpace = 0;
   m_nUpdateRecordTime = 0;
   m_nPauseButtonTimer = 0;
   m_nFlashPause = 1;

   m_iRecordingBitmapID = IDR_TOOLBAR_RECORDING;
   
   m_bRecording = false;
   m_bRecordingPaused = false;
   m_bScreenGrabing = false;
   m_bRefreshTransparentBtn = false;

   m_bRecordingIsSgOnly = false;
   
   m_bNextPageExist = m_bPreviousPageExist = true;

#ifndef _STUDIO
   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
#endif
   
   m_pDocumentStructureView = NULL;
   m_pRecordingsView = NULL;
   
   m_pCurrentView = NULL;
   m_pWhiteboardView = NULL;
   m_pStartupView  = NULL;
   
   m_nAVSettingsTab = 0;

   m_bRedrawWindow = false;
   m_bDisplayGrid = false;

#ifndef _STUDIO
   ASSISTANT::Project::InitAvGrabber();
#endif
   
	m_pSBSDK = NULL;
	m_pEvents = NULL;
   m_bSMARTBoardAttached = false;
   m_bSMARTBoardUsable = false;

	HRESULT hr;
	hr = CoCreateInstance(__uuidof(SBSDKBaseClass2), NULL, CLSCTX_INPROC_SERVER,
							    __uuidof(ISBSDKBaseClass2), (void **)(&m_pSBSDK));

	m_dwEventCookie = 0;

	if (SUCCEEDED(hr))
	{
		IConnectionPointContainer *pCPC = NULL;
		hr = m_pSBSDK->QueryInterface(__uuidof(IConnectionPointContainer), (void**)&pCPC);
		if (SUCCEEDED(hr))
		{
			IConnectionPoint *pCP = NULL;
			hr = pCPC->FindConnectionPoint(__uuidof(_ISBSDKBaseClass2Events), &pCP);
			if (SUCCEEDED(hr))
			{
				m_pEvents = new CSBSDKEventsHandler;
				m_pEvents->SetSDKIntf(this);
				LPUNKNOWN pUnk = m_pEvents->GetInterface(&IID_IUnknown);
				hr = pCP->Advise(pUnk, &m_dwEventCookie);
				pCP->Release();
			}
			pCPC->Release();
		}
	}
    
   m_bHasCheckedForManual = false;
   m_bHasManual = false;
    
    m_bDocumentStructureVisible = TRUE;

    m_bShowStartpage = true;
#ifndef _STUDIO
    m_nCurrentLocalColorScheme = 0;
#endif
    
    m_pMonitorWidget = NULL;
    m_pAudioWidget = NULL;
    m_pPresentationBar = NULL;
    m_bMonitorWidget = false;
    m_bPresentationBarAutoHide = false;
    m_bShowPresentationBar = true;
    if (AfxGetApp()) {
        m_bPresentationBarAutoHide = 
         AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("AutoHidePresentationBar"), 0) == 1 ? true : false;
        m_bShowPresentationBar = 
         AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("ShowPresentationBar"), 1) == 1 ? true : false;
    }
    

    //m_pRecentFilelist = NULL;
    m_bRecentFilelist = false;
	
    m_pWndCaptureRect = NULL;

    //m_pRecentFilelist = NULL;
    m_bRecentFilelist = false;
    m_uiPresentationTypeSelected= 0;

    m_pButtonStandardLayout = NULL;
    m_bStartSgFromHotKeys = false;
    m_bParentChanged = false;
    m_bIsShowSGPresentationBar = true;
    m_bIsInFullScreen = false;

    m_nPositionSG = AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("PositionSG"), OT_MIDDLERIGHT);
    m_nPositionFS = AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Position"), OT_BOTTOMMIDDLE);//OT_BOTTOMMIDDLE;
}

CMainFrameA::~CMainFrameA()
{
#ifndef _STUDIO
   Gdiplus::GdiplusShutdown(m_gdiplusToken);
#endif

	if (m_pSBSDK)
	{
		IConnectionPointContainer *pCPC = NULL;
		HRESULT hr = m_pSBSDK->QueryInterface(__uuidof(IConnectionPointContainer), (void**)&pCPC);
		if (SUCCEEDED(hr))
		{
			IConnectionPoint *pCP = NULL;
			hr = pCPC->FindConnectionPoint(__uuidof(_ISBSDKBaseClassEvents), &pCP);
			if (SUCCEEDED(hr))
			{
				hr = pCP->Unadvise(m_dwEventCookie);
				pCP->Release();
			}
			pCPC->Release();
		}

		m_pSBSDK->Release();
	}

	if (m_pEvents)
	{
		delete m_pEvents;
		m_pEvents = NULL;
	}

    AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("PositionSG"), m_nPositionSG);
    
#ifndef _STUDIO
   ASSISTANT::Project::DeleteAvGrabber();
#endif

}

int CMainFrameA::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#ifdef _STUDIO
   if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   ModifyStyle(WS_MINIMIZEBOX  | WS_MAXIMIZEBOX | WS_SYSMENU, 0);
#else
   if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;
#endif
   
#ifndef _STUDIO
   if (!m_wndStatusBar.Create(this) ||
      !m_wndStatusBar.SetIndicators(indicators,
      sizeof(indicators)/sizeof(UINT)))
   {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
   }

   bool bShowStatusBar = GetShowStatusBar();
   if(bShowStatusBar)
		m_wndStatusBar.ShowWindow(SW_SHOW);
   else
		m_wndStatusBar.ShowWindow(SW_HIDE);
#endif
   
   
   // Initialize the command bars
   if (!InitCommandBars())
      return -1;

#ifndef _STUDIO
   CXTPPaintManager::SetTheme(xtpThemeRibbon);//(xtpThemeOffice2003);//
#endif

   // Get a pointer to the command bars object.
   CXTPCommandBars* pCommandBars = GetCommandBars();
   if(pCommandBars == NULL)
   {
      TRACE0("Failed to create command bars object.\n");
      return -1;      // fail to create
   }
   // Add the menu bar
   //CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(
   //   _T("Menu Bar"), IDR_MAINFRAME);       
   //if(pMenuBar == NULL)
   //{
   //   TRACE0("Failed to create menu bar.\n");
   //   return -1;      // fail to create
   //}
   
#ifndef _STUDIO
   // Create ToolBar
   XTPImageManager()->SetMaskColor(RGB(0x00, 0x00, 0x00));
#endif

   // Set tooltip context
#ifndef _STUDIO
   CXTPToolTipContext* pToolTipContext = GetCommandBars()->GetToolTipContext();
   pToolTipContext->SetStyle(xtpToolTipOffice2007);
   pToolTipContext->ShowTitleAndDescription();
   pToolTipContext->ShowImage(TRUE, 0);
   pToolTipContext->SetMargin(CRect(2, 2, 2, 2));
   pToolTipContext->SetMaxTipWidth(180);
   pToolTipContext->SetFont(pCommandBars->GetPaintManager()->GetIconFont());
   pToolTipContext->SetDelayTime(TTDT_INITIAL, 900);
#endif
   

   m_paneManager.InstallDockingPanes(this);
	m_paneManager.SetTheme( xtpPaneThemeOffice2007Ext/*xtpPaneThemeWord2007*/);
	m_paneManager.SetThemedFloatingFrames(TRUE);
   
#ifndef _STUDIO
      CRibbonBars * pRibbonBars = new CRibbonBars(); 
    if(!pRibbonBars->CreateRibbonBars(pCommandBars,m_hWnd,this))
       return -1;
    CreateStatusBar();
    
    /*if(!CreateRibbonBar())
      return -1;*/
#endif

   CXTPDockingPane *pDocumentStructurePane = m_paneManager.CreatePane(IDR_PANE_DOCSTRUCT, CRect(0, 0, 250, 150), xtpPaneDockLeft/*dockLeftOf*/);
   pDocumentStructurePane->SetOptions(xtpPaneNoHideable);
   pDocumentStructurePane->SetMinTrackSize(CSize(100, 100));
   pDocumentStructurePane->SetMaxTrackSize(CSize(300, 700));
   
   CXTPDockingPane *pRecordingsPane = m_paneManager.CreatePane(IDR_PANE_RECORDINGS, CRect(0, 0, 250, 100), xtpPaneDockBottom/*dockBottomOf*/, pDocumentStructurePane);
   pRecordingsPane->SetOptions(xtpPaneNoHideable);
   
   
   CXTPDockingPaneLayout layoutNormal(&m_paneManager);
   if (layoutNormal.Load(_T("NormalLayout"))) 
   {
      // BUGFIX #4748
      // compare the docking panes ID's of the saved layout with the 
      // ID's of the docking panes created
      // If the ID's match, then the saved layout is loaded
      CXTPDockingPaneInfoList &dpl = layoutNormal.GetPaneList();
      UINT nNrOfPanes = 2;
      if(nNrOfPanes == dpl.GetCount())
      {
      
         POSITION pos = dpl.GetHeadPosition();
         bool bSamePanes = true;
         while(pos)
         {
            CXTPDockingPane* dp = dpl.GetNext(pos);
            if(dp->GetID() != IDR_PANE_DOCSTRUCT && dp->GetID() != IDR_PANE_RECORDINGS)
               bSamePanes = false;
            if(dp->GetID() == IDR_PANE_DOCSTRUCT)
            {
               MINMAXINFO mmi;
               dp->GetMinMaxInfo(&mmi);
               CPoint ptMax = mmi.ptMaxTrackSize;
               CPoint ptMin = mmi.ptMinTrackSize;
               if(ptMax != CPoint(300, 700) || ptMin != CPoint(100, 100))
                  bSamePanes = false;      
            }
         }
         
         
         if(bSamePanes)
            m_paneManager.SetLayout(&layoutNormal);   
      }
   }
       
   // NI added 
   // add some extra features to toolbars

#ifndef _STUDIO
   /*XTP_COMMANDBARS_OPTIONS*/CXTPCommandBarsOptions* pOptions = pCommandBars->GetCommandBarsOptions();
   pOptions->bAlwaysShowFullMenus = TRUE;
   pOptions->bToolBarScreenTips = TRUE;
   pOptions->bToolBarAccelTips = FALSE;//TRUE;
   pOptions->bDblClickFloat = TRUE;
   pOptions->bSyncFloatingBars = TRUE;
#endif
   
   XTP_COMMANDBARS_ICONSINFO* pIconsInfo = XTPPaintManager()->GetIconsInfo();
   pIconsInfo->bUseDisabledIcons = TRUE;
   //pIconsInfo->bUseFadedIcons = TRUE;
   //pIconsInfo->bIconsWithShadow = TRUE;
   // end NI
   
#ifndef _STUDIO
   pCommandBars->SetEnableContextMenu();
#endif

   m_pWhiteboardView = GetAssistantView();

   m_pCurrentView = m_pWhiteboardView;

   // create the transparent buttons
   m_pButtonStandardLayout = new CHoverButtonEx(WM_USER_GOTO_STANDARD_LAYOUT);
   m_pButtonStandardLayout->Create(NULL,WS_CHILD | WS_VISIBLE, CRect(0,0,65,33),
                                   m_pWhiteboardView, ID_GOTO_STANDARD_LAYOUT);
   m_pButtonStandardLayout->LoadPng();
   m_pButtonStandardLayout->SetToolTipText(ID_GOTO_STANDARD_LAYOUT);
   
   HideTransparentButton();

   StoreWindowsState(StandardLayout);

   AttachSmartBoard();
   if (GetShowUseSmartBoard() && m_bSMARTBoardUsable)
   {
      CString csMessage;
      csMessage.LoadString(IDS_USE_SMARTBOARD);
      CString csCaption;
      csCaption.LoadString(IDS_SMARTBOARD);
      int iAnswer = MessageBox(csMessage, csCaption, MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
      // TODO: use CEditorDoc::SomeStaticMethod (ie ShowQuestionMessage())
      if (iAnswer == IDNO)
         DetachSmartBoard();
   }
						  
#ifndef _STUDIO
   m_lWindowStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
#endif
   
   // Initialize variable with value from Application (saved in registry)
   m_bShowStartpage = GetShowStartupPage();

#ifndef _STUDIO
   //It's not needed anymore  LoadRibbonIcons(GetCommandBars()) -> called in RibbonBars.cpp
#endif


   m_pMonitorWidget = new CMonitorWidget();
   m_pMonitorWidget->CreateEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, AfxRegisterWndClass(CS_DBLCLKS,0,0,0), NULL, 
      WS_POPUP | WS_VISIBLE, CRect(100, 100, 400, 200), CMainFrameA::GetCurrentInstance()/*this*/, 0);
   m_pMonitorWidget->SetCommandBars(pCommandBars);
   m_pMonitorWidget->ShowWindow(SW_HIDE);
   

   m_pAudioWidget = new CAudioWidget();
   m_pAudioWidget->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, AfxRegisterWndClass(CS_DBLCLKS,0,0,0), NULL, 
      WS_POPUP | WS_VISIBLE, CRect(100, 100, 150, 150), GetDesktopWindow()/*this*/, 0);
   m_pAudioWidget->SetCommandBars(GetCommandBars());
   m_pAudioWidget->ShowWindow(SW_HIDE);

   SetAudioVideoTooltips();

   CAssistantDoc *pAssistantDocument = CMainFrameA::GetAssistantDocument();
   m_pPresentationBar = new CPresentationBar(pAssistantDocument);
   m_pPresentationBar->SetMainFrmWnd(this);
   m_pPresentationBar->CreateEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 
      AfxRegisterWndClass(NULL,0,0,0), NULL, 
      WS_POPUP | WS_VISIBLE, CRect(100, 100, 400, 200), this, 0);
   m_pPresentationBar->SetLayeredWindowAttributes(RGB(255, 255, 255), (255 * 30) / 100, LWA_ALPHA | LWA_COLORKEY);
   m_pPresentationBar->SetCommandBars(pCommandBars);
   m_pPresentationBar->ShowWindow(SW_HIDE);
   m_pPresentationBar->SetMainFrmWnd(this);

   CXTPControl *pCtrlPos = pCommandBars->GetContextMenus()->GetAt(3)->GetControl(0);
   pCtrlPos->SetStyle(xtpButtonCaption);
   CXTPControl *pCtrlAddRemove = pCommandBars->GetContextMenus()->GetAt(3)->GetControl(5);
   pCtrlAddRemove->SetStyle(xtpButtonCaption);

   return 0;
}

BOOL CMainFrameA::PreCreateWindow(CREATESTRUCT& cs)
{
#ifdef _STUDIO
   if( !CMDIChildWnd::PreCreateWindow(cs) )
      return FALSE;
#else
   if( !CFrameWnd::PreCreateWindow(cs) )
      return FALSE;
#endif
   // TODO: Modify the Window class or styles here by modifying
   //  the CREATESTRUCT cs
   
   return TRUE;
}

void CMainFrameA::OnUpdateFrameTitle(BOOL bAddToTitle)
{
#ifdef _STUDIO
    SetWindowText(_T(""));
#else
    CFrameWnd::OnUpdateFrameTitle(bAddToTitle);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrameA diagnostics

#ifdef _DEBUG
void CMainFrameA::AssertValid() const
{
#ifdef _STUDIO
   CMDIChildWnd::AssertValid();
#else
   CFrameWnd::AssertValid();
#endif
}

void CMainFrameA::Dump(CDumpContext& dc) const
{
#ifdef _STUDIO
   CMDIChildWnd::Dump(dc);
#else
   CFrameWnd::Dump(dc);
#endif
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrameA message handlers


void CMainFrameA::OnPackProject() 
{
    CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument)
   {
      CString csProjectName;
      HRESULT hr = ASSISTANT::GetSaveFilename(IDS_LAP_FILTER, csProjectName, _T(".lap"), _T("LAP"));
      
      if (SUCCEEDED(hr) && !csProjectName.IsEmpty())
         HRESULT hr = pDocument->StartPackProject(csProjectName);
   }
}

void CMainFrameA::OnUpdatePackProject(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument->HasLoadedDocuments())
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
}

void CMainFrameA::OnExploreHome() 
{
   // TODO: Add your command handler code here
   if (!m_csLecHome.IsEmpty())
   {
      HANDLE hDir = CreateFile (m_csLecHome, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
         NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
      
      if(hDir == INVALID_HANDLE_VALUE)
      {      
         CString csMessage;
         csMessage.LoadString(IDS_LEC_HOME_WARNING);

         CEditorDoc::ShowErrorMessage(csMessage);
         
         return;   
      }
      CloseHandle(hDir);
      
      ShellExecute(NULL, _T("explore"), m_csLecHome, NULL, NULL, SW_SHOWNORMAL);
   }
   else
   {
      CString csMessage;
      csMessage.LoadString(IDS_LEC_HOME_WARNING);
      CEditorDoc::ShowWarningMessage(csMessage);
   }
   
}

void CMainFrameA::OnUpdateExploreHome(CCmdUI* pCmdUI) 
{
    BOOL bEnable = TRUE;

    if (CStudioApp::IsPowerTrainerMode())
        bEnable = FALSE;

    pCmdUI->Enable(bEnable);

}

void CMainFrameA::OnSelectHome() 
{

   SelectLecturnityHome();

}

void CMainFrameA::OnUpdateSelectHome(CCmdUI* pCmdUI) 
{
    BOOL bEnable = TRUE;

    if (CStudioApp::IsPowerTrainerMode())
        bEnable = FALSE;

    pCmdUI->Enable(bEnable);
}


void CMainFrameA::SelectLecturnityHome() {
    CString csLecturnityHome;
    bool bResult = ASSISTANT::GetLecturnityHome(csLecturnityHome);

    if (!bResult) {
        csLecturnityHome = _T("");
    }

    CXFolderDialog fDlg(csLecturnityHome);
    int nOsVesrion = 0;

    fDlg.SetOsVersion((CXFolderDialog::XFILEDIALOG_OS_VERSION)nOsVesrion);

    CString csTitle;
    csTitle.LoadString(IDS_SELECT_LEC_HOME);
    fDlg.SetTitle(csTitle);

    bool bSuccess = true;
    //   CString csPath = "No Folder Selected!";
    if (fDlg.DoModal() == IDOK) {
        m_csLecHome = fDlg.GetPath();
    } else if (!csLecturnityHome.IsEmpty()) {
        m_csLecHome = csLecturnityHome;
    } else {
        bSuccess = false;
    }

    if (bSuccess) {
        SaveLecturnityHome();
    }
}

void CMainFrameA::SaveLecturnityHome(CString csLecHome) {
    bool bSuccess = true;
    if (!csLecHome.IsEmpty()) {
        m_csLecHome = csLecHome;
    }
    if (bSuccess) {
        if (_taccess(m_csLecHome,00) == -1) {
            BOOL result = CreateDirectory(m_csLecHome, NULL);
            if (result == 0) {
                CString csMessage;
                csMessage.Format(IDS_ERROR_CREATE_DIR, m_csLecHome);
                CEditorDoc::ShowErrorMessage(csMessage);
                return;
            }
        }
        CString csPath;

        csPath = m_csLecHome + _T("\\Source Documents");
        if (_taccess(csPath, 00) == -1) {
            BOOL result = CreateDirectory(csPath, NULL);
            if (result == 0) {
                CString csMessage;
                csMessage.Format(IDS_ERROR_CREATE_DIR, csPath);
                CEditorDoc::ShowErrorMessage(csMessage);
                return;
            }
        }

        csPath = m_csLecHome + _T("\\CD Projects");
        if (_taccess(csPath, 00) == -1) {
            BOOL result = CreateDirectory(csPath, NULL);
            if (result == 0) {
                CString csMessage;
                csMessage.Format(IDS_ERROR_CREATE_DIR, csPath);
                CEditorDoc::ShowErrorMessage(csMessage);
                return;
            }
        }

        csPath = m_csLecHome + _T("\\Recordings");
        if (_taccess(csPath, 00) == -1) {
            BOOL result = CreateDirectory(csPath, NULL);
            if (result == 0) {
                CString csMessage;
                csMessage.Format(IDS_ERROR_CREATE_DIR, csPath);
                CEditorDoc::ShowErrorMessage(csMessage);
                return;
            }
        }

        if (ASSISTANT::Project::active != NULL) {
            CString csRecordPath = m_csLecHome;
            csRecordPath += _T("\\Recordings");
            ASSISTANT::Project::active->SetRecordPath(csRecordPath);
        }

        ASSISTANT::SetLecturnityHome(m_csLecHome);
    }
}

#ifndef _STUDIO
void CMainFrameA::OnCustomize()
{
   // Get a pointer to the command bars object.
  
   if(m_nCurrentLayout == 3)
      {
      return;
      }
   
   CXTPCommandBars* pCommandBars = GetCommandBars();
   if(pCommandBars != NULL)
   {
      // Instanciate the customize dialog object.
      CXTPCustomizeSheet dlg(pCommandBars);
      
      // Add the options page to the customize dialog.
      CXTPCustomizeOptionsPage pageOptions(&dlg);
      dlg.AddPage(&pageOptions);
      
      // Add the commands page to the customize dialog.
      CXTPCustomizeCommandsPage* pCommands = dlg.GetCommandsPage();
      pCommands->AddCategories(IDR_MAINFRAME_A);
      
      // Use the command bar manager to initialize the 
      // customize dialog.
      pCommands->InsertAllCommandsCategory();
      pCommands->InsertBuiltInMenus(IDR_MAINFRAME_A);
      pCommands->InsertNewMenuCategory();
      
      // Dispaly the dialog.
      dlg.DoModal();
   }
}
#endif

void CMainFrameA::OnNewPage() {
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument) {
       CDocumentStructureView *pStructView = GetStructureTree();
       if (pStructView != NULL) {
           CXTPReportRow* pRow = pStructView->GetReportCtrl().GetFocusedRow();
           if (pRow != NULL) {
               CDocStructRecord *pRecord = (CDocStructRecord*)pRow->GetRecord();
               if (pRecord != NULL) {
                   UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);
                   pDocument->SelectContainer(nId);
                   HRESULT hr = pDocument->InsertNewPage(m_pDocumentStructureView);
               }
           }
       }    
   }
}

void CMainFrameA::OnUpdateNewPage(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   bool bEnable = FALSE;

   if (pDocument->HasLoadedDocuments() && 
       ASSISTANT::Project::active && ASSISTANT::Project::active->GetActiveDocument() &&
       ASSISTANT::Project::active->GetActiveDocument()->IsLoaded())
       bEnable = TRUE;

   pCmdUI->Enable(bEnable);
}

void CMainFrameA::OnDuplicatePage() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument)
      HRESULT hr = pDocument->DuplicatePage(m_pDocumentStructureView);
}

void CMainFrameA::OnUpdateDuplicatePage(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument->HasLoadedDocuments() && pDocument->HasActivePage())
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
}

void CMainFrameA::OnNewChapter() {
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument) {
       CDocumentStructureView *pStructView = GetStructureTree();
       if (pStructView != NULL) {
           CXTPReportRow* pRow = pStructView->GetReportCtrl().GetFocusedRow();
           if (pRow != NULL) {
               CDocStructRecord *pRecord = (CDocStructRecord*)pRow->GetRecord();
               if (pRecord != NULL) {
                   UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);
                   pDocument->SelectContainer(nId);
                   HRESULT hr = pDocument->InsertNewChapter(m_pDocumentStructureView);
               }
           }
       }    
   }
}

void CMainFrameA::OnUpdateNewChapter(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   bool bEnable = FALSE;

   if (pDocument->HasLoadedDocuments() && 
       ASSISTANT::Project::active && ASSISTANT::Project::active->GetActiveDocument() &&
       ASSISTANT::Project::active->GetActiveDocument()->IsLoaded())
       bEnable = TRUE;

   pCmdUI->Enable(bEnable);
}

#ifndef _STUDIO
int CMainFrameA::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
   //create System Menu start
   if (lpCreateControl->nID == ID_FILE_MRU_FILE1)
	{		
      if(m_bRecentFilelist == false)
      {
         CXTPRibbonControlRecentFileListExt::GetInstance()->SetRegPath(ASSISTANT_REGISTRY);
         lpCreateControl->pControl = CXTPRibbonControlRecentFileListExt::GetInstance();//m_pRecentFilelist;
         lpCreateControl->pControl->SetID(XTP_ID_RECENT_FILE_LIST_CONTROL);
         m_bRecentFilelist = true;
      }
		return TRUE;
	}
   if (lpCreateControl->nID == ID_APP_EXIT || lpCreateControl->nID == ID_MENU_ASSISTANT_SETTINGS )
	{		
		lpCreateControl->pControl = new CXTPRibbonControlSystemPopupBarButton();

		return TRUE;
	}
    
   if(lpCreateControl->nID == ID_MENU_NEW)
   {
      CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

#ifndef _STUDIO
		CMenu menu;
      menu.LoadMenu(ID_SUBMENU_NEW);

		CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
		pPopupBar->SetCommandBars(GetCommandBars());

		pPopupBar->LoadMenu(menu.GetSubMenu(0));

		CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_MENU_NEW_LABEL, NULL, 0);
		pControlLabel->SetWidth(296);
		pControlLabel->SetItemDefault(TRUE);
		pControlLabel->SetStyle(xtpButtonCaption);

		pPopupBar->SetShowGripper(FALSE);
		pPopupBar->SetDefaultButtonStyle(xtpButtonIconAndCaption);
		pPopupBar->SetIconSize(CSize(32, 32));
      //pPopupBar->SetDoubleGripper();

		pControl->SetCommandBar(pPopupBar);

		pPopupBar->InternalRelease();

		lpCreateControl->pControl = pControl;
#endif
		return TRUE;
   }

   if(lpCreateControl->nID == ID_MENU_OPEN)
   {
      CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

		CMenu menu;
      menu.LoadMenu(ID_SUBMENU_OPEN);

		CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
		pPopupBar->SetCommandBars(GetCommandBars());

		pPopupBar->LoadMenu(menu.GetSubMenu(0));

		CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_MENU_OPEN_LABEL, NULL, 0);
		pControlLabel->SetWidth(296);
		pControlLabel->SetItemDefault(TRUE);
		pControlLabel->SetStyle(xtpButtonCaption);

		pPopupBar->SetShowGripper(FALSE);
		pPopupBar->SetDefaultButtonStyle(xtpButtonIconAndCaption);
		pPopupBar->SetIconSize(CSize(32, 32));
      //pPopupBar->SetDoubleGripper();

		pControl->SetCommandBar(pPopupBar);

		pPopupBar->InternalRelease();

		lpCreateControl->pControl = pControl;
		return TRUE;
   }

   if(lpCreateControl->nID == ID_MENU_PROJECT_SETTINGS)
   {
      CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

		CMenu menu;
      menu.LoadMenu(ID_SUBMENU_PROJECT_SETTINGS);

		CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
		pPopupBar->SetCommandBars(GetCommandBars());

		pPopupBar->LoadMenu(menu.GetSubMenu(0));

		CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_MENU_PROJECT_SETTINGS_LABEL, NULL, 0);
		pControlLabel->SetWidth(296);
		pControlLabel->SetItemDefault(TRUE);
		pControlLabel->SetStyle(xtpButtonCaption);

		pPopupBar->SetShowGripper(FALSE);
		pPopupBar->SetDefaultButtonStyle(xtpButtonIconAndCaption);
		pPopupBar->SetIconSize(CSize(32, 32));
      //pPopupBar->SetDoubleGripper();

		pControl->SetCommandBar(pPopupBar);

		pPopupBar->InternalRelease();

		lpCreateControl->pControl = pControl;
		return TRUE;
   }

   if(lpCreateControl->nID == ID_MENU_SAVE)
   {
      CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

		CMenu menu;
      menu.LoadMenu(ID_SUBMENU_SAVE);

		CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
		pPopupBar->SetCommandBars(GetCommandBars());

		pPopupBar->LoadMenu(menu.GetSubMenu(0));

		CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_MENU_SAVE_LABEL, NULL, 0);
		pControlLabel->SetWidth(296);
		pControlLabel->SetItemDefault(TRUE);
		pControlLabel->SetStyle(xtpButtonCaption);

		pPopupBar->SetShowGripper(FALSE);
		pPopupBar->SetDefaultButtonStyle(xtpButtonIconAndCaption);
		pPopupBar->SetIconSize(CSize(32, 32));
      //pPopupBar->SetDoubleGripper();

		pControl->SetCommandBar(pPopupBar);

		pPopupBar->InternalRelease();

		lpCreateControl->pControl = pControl;
		return TRUE;
   }

   if(lpCreateControl->nID == ID_MENU_SAVE_AS)
   {
      CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

		CMenu menu;
      menu.LoadMenu(ID_SUBMENU_SAVE_AS);

		CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
		pPopupBar->SetCommandBars(GetCommandBars());

		pPopupBar->LoadMenu(menu.GetSubMenu(0));

		CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_MENU_SAVE_AS_LABEL, NULL, 0);
		pControlLabel->SetWidth(296);
		pControlLabel->SetItemDefault(TRUE);
		pControlLabel->SetStyle(xtpButtonCaption);

		pPopupBar->SetShowGripper(FALSE);
		pPopupBar->SetDefaultButtonStyle(xtpButtonIconAndCaption);
		pPopupBar->SetIconSize(CSize(32, 32));
      //pPopupBar->SetDoubleGripper();

		pControl->SetCommandBar(pPopupBar);

		pPopupBar->InternalRelease();

		lpCreateControl->pControl = pControl;
		return TRUE;
   }

   if(lpCreateControl->nID == ID_MENU_IMPORT)
   {
      CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

		CMenu menu;
      menu.LoadMenu(ID_SUBMENU_IMPORT);

		CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
		pPopupBar->SetCommandBars(GetCommandBars());

		pPopupBar->LoadMenu(menu.GetSubMenu(0));

      CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_MENU_IMPORT_LABEL, NULL, 0);
		pControlLabel->SetWidth(296);
		pControlLabel->SetItemDefault(TRUE);
		pControlLabel->SetStyle(xtpButtonCaption);

		pPopupBar->SetShowGripper(FALSE);
		pPopupBar->SetDefaultButtonStyle(xtpButtonIconAndCaption);
		pPopupBar->SetIconSize(CSize(32, 32));
      //pPopupBar->SetDoubleGripper();

		pControl->SetCommandBar(pPopupBar);

		pPopupBar->InternalRelease();

		lpCreateControl->pControl = pControl;
		return TRUE;
   }
 
	if (lpCreateControl->nID == ID_PRESENTATION_PENCILS)
	{
		CXTPControlGalleryItems *m_pItemsPens = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_PRESENTATION_PENCILS);
		
		m_pItemsPens->GetImageManager()->SetMaskColor(0x000000);
		m_pItemsPens->GetImageManager()->SetIcons(IDB_TAB_TOOLS, 0, 0, CSize(32, 32)); // Get images from same id.
		m_pItemsPens->GetImageManager()->SetMaskColor(COLORREF_NULL); // Default TopLeft Pixel.
		m_pItemsPens->ClipItems(TRUE);
		m_pItemsPens->SetItemSize(CSize(38, 38));
		// Add each item to the gallery list

		m_pItemsPens->AddItem(ID_TOOL_PENBLUE, 9);
      m_pItemsPens->AddItem(ID_TOOL_PENBLACK, 8);
      m_pItemsPens->AddItem(ID_TOOL_PENCYAN, 10);
      m_pItemsPens->AddItem(ID_TOOL_PENGREEN, 11);
      m_pItemsPens->AddItem(ID_TOOL_PENYELLOW, 12);
      m_pItemsPens->AddItem(ID_TOOL_PENRED, 13);
      m_pItemsPens->AddItem(ID_TOOL_PENMAGENTA, 14);
      m_pItemsPens->AddItem(ID_TOOL_PENWHITE, 15);


		CXTPControlGallery* pControlGallery = new CXTPControlGallery();
		pControlGallery->SetControlSize(CSize(32*4 + 6*4 /*4 columns. 32 is button size, 6 is button margins*/, 32 * 2 + 6*2 /*2 lines*/));
		pControlGallery->SetItems(m_pItemsPens);
		pControlGallery->ShowScrollBar(FALSE);
		
		lpCreateControl->pControl = pControlGallery;

		return TRUE;
	}

	if (lpCreateControl->nID == ID_PRESENTATION_MARKERS)
	{
		CXTPControlGalleryItems *m_pItemsMarkers = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_PRESENTATION_MARKERS);
		
		m_pItemsMarkers->GetImageManager()->SetMaskColor(0x000000);
		m_pItemsMarkers->GetImageManager()->SetIcons(IDB_TAB_TOOLS, 0, 0, CSize(32, 32));
		m_pItemsMarkers->GetImageManager()->SetMaskColor(COLORREF_NULL); // Default TopLeft Pixel.
		m_pItemsMarkers->ClipItems(TRUE);
		m_pItemsMarkers->SetItemSize(CSize(38, 38));
		// Add each item to the gallery list

		m_pItemsMarkers->AddItem(ID_TOOL_MARKERBLUE,6);
      m_pItemsMarkers->AddItem(ID_TOOL_MARKERORANGE,3);
      m_pItemsMarkers->AddItem(ID_TOOL_MARKERRED,4);
      m_pItemsMarkers->AddItem(ID_TOOL_MARKERGREEN,5);
      m_pItemsMarkers->AddItem(ID_TOOL_MARKERYELLOW,2);
      m_pItemsMarkers->AddItem(ID_TOOL_MARKERMAGENTA,7);

		CXTPControlGallery* pControlGallery = new CXTPControlGallery();
		pControlGallery->SetControlSize(CSize(32*3 + 6*3 /*3 columns. 32 is button size, 6 is difference between ItemSize and IconSize*/, 32 * 2 + 6*2 /*2 lines*/));
		pControlGallery->SetItems(m_pItemsMarkers);
		pControlGallery->ShowScrollBar(FALSE);
		
		lpCreateControl->pControl = pControlGallery;

		return TRUE;
	}
   if (lpCreateControl->nID == ID_VIEW_STATUS_BAR)
   {
      UpdateStatusDiskSpace();
   }
   return FALSE;
}
#endif

void CMainFrameA::OnClose() 
{
    // Close of child frames is only called in SDI applications
    CloseAllContent();

#ifdef _STUDIO
    m_pCurrentView = NULL;
   CMDIChildWnd::OnClose();
#else
    m_pCurrentView = NULL;
   CFrameWnd::OnClose();
#endif
}

void CMainFrameA::OnDestroy()
{
#ifdef _STUDIO
    // Close of child frames is only called in SDI applications
    CloseAllContent();
    CMDIChildWnd::OnDestroy();
#else
    CFrameWnd::OnDestroy();
#endif
}

void CMainFrameA::OnUpdateHelp(CCmdUI *pCmdUI)
{
   if (!m_bHasCheckedForManual)
   {
      m_bHasManual = CLanguageSupport::ManualExists();
      m_bHasCheckedForManual = true;
   }

   pCmdUI->Enable(m_bHasManual ? TRUE : FALSE);
}

void CMainFrameA::OnHelp() 
{
   HRESULT hr = CLanguageSupport::StartManual();
   if (hr != S_OK)
   {
      CString msg;
      msg.Format(IDS_MSG_OPEN_PDF_FAILED, _T("manual.pdf"));
      CEditorDoc::ShowErrorMessage(msg);
   }
}

void CMainFrameA::FullScreenModeOn()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   pDoc->SetFullScreenOn(true);

#ifndef _STUDIO
   // Store window position and dimension
   GetWindowRect(&m_mainRect);

   // Store window style
   m_lWindowStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);

   // remove the caption and the border of the mainWnd:
   SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);


   // Width and height of the Desktop
   int screenx = GetSystemMetrics(SM_CXSCREEN);
   int screeny = GetSystemMetrics(SM_CYSCREEN);

   int iBorderWidth = GetSystemMetrics(SM_CXBORDER);
   int iBorderHeight = GetSystemMetrics(SM_CYBORDER);
   // resize:
   SetWindowPos(NULL, -2*iBorderWidth, -2*iBorderHeight,
                      screenx+4*iBorderWidth, screeny+4*iBorderHeight,SWP_NOZORDER);
   RecalcLayout();

   CRect rcMainWnd, rcPresentationBar, rcStatusBar;
   GetWindowRect(rcMainWnd);

   if(m_pPresentationBar && !IsInStartupMode() && m_bShowPresentationBar)
   {
      UINT nPos = m_pPresentationBar->GetPosition();
      UpdatePresentationBarPosition(nPos);
      m_pPresentationBar->ShowWindow(SW_SHOW);
   }
   
   GetRibbonBar()->EnableFrameTheme(FALSE);
   if(m_bStatusBarVisibleStandard==TRUE)
   {
     HideStatusBar();
     m_bStatusBarVisibleStandard = FALSE;
   }
#else
   CWnd *pMainFrame = AfxGetMainWnd();
   if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
       ((CMainFrame *)pMainFrame)->FullScreenModeOn();

   if(m_pPresentationBar && !IsInStartupMode() && m_bShowPresentationBar)
   {
      UINT nPos = m_pPresentationBar->GetPosition();
      UpdatePresentationBarPosition(nPos);
      m_pPresentationBar->ShowWindow(SW_SHOW);
      m_pPresentationBar->RedrawWindow();
      m_pPresentationBar->SetAutoHide(m_bPresentationBarAutoHide);
   }
   m_bIsInFullScreen = true;

#endif
}

void CMainFrameA::FullScreenModeOff()
{
   // exit full screen 
 
#ifndef _STUDIO
   // reset window style
   ::SetWindowLong(m_hWnd, GWL_STYLE, m_lWindowStyle);

   // reset window position and dimension
   MoveWindow(&m_mainRect);

   RecalcLayout();

   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   pDoc->SetFullScreenOn(false);
   
   if(m_pPresentationBar)
      m_pPresentationBar->ShowWindow(SW_HIDE);

   UpdatePresentationIcon();
   GetRibbonBar()->EnableFrameTheme(TRUE);
   if(m_bStatusBarVisibleStandard==FALSE)
   {
     ShowStatusBar();
     m_bStatusBarVisibleStandard = TRUE;
   }
#else
   CWnd *pMainFrame = AfxGetMainWnd();
   if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
       ((CMainFrame *)pMainFrame)->FullScreenModeOff(m_uiPresentationTypeSelected);

   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   pDoc->SetFullScreenOn(false);
   if(m_pPresentationBar)
   {
      m_pPresentationBar->ShowWindow(SW_HIDE);
      m_pPresentationBar->SetAutoHide(false);
   }
   m_bIsInFullScreen = false;

#ifndef _STUDIO
      ShowMenu();
      //ShowStatusBar();
#endif

   if (m_nCurrentLayout == AssistantLayouts(FullscreenLayout))
   {
       ShowPanes();
       HideTransparentButton();

       m_nCurrentLayout = AssistantLayouts(StandardLayout);
   }
   else if(m_nCurrentLayout == AssistantLayouts(FullscreenPagefocusLayout))
   {
       m_nCurrentLayout = AssistantLayouts(StandardPagefocusLayout);
       HideTransparentButton(false);
   }
#endif
}

void CMainFrameA::OnFullScreen() 
{
   if ((m_nCurrentLayout == AssistantLayouts(StandardLayout)) || 
       (m_nCurrentLayout == AssistantLayouts(StandardPagefocusLayout)))
   {
      CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
      
      if (pDoc != NULL && pDoc->HasLoadedDocuments())
      {
         UINT nActivePageId = pDoc->GoToFirstPage();
         if (nActivePageId >= 0 && m_pDocumentStructureView != NULL)
            m_pDocumentStructureView->SetSelected(nActivePageId);
        
      }
      
      OnFullScreenCS();
   }
    m_uiPresentationTypeSelected = 0;
}

void CMainFrameA::OnFullScreenCS() 
{
   if ((m_nCurrentLayout == AssistantLayouts(StandardLayout)) || 
       (m_nCurrentLayout == AssistantLayouts(StandardPagefocusLayout)))
   {
      ChangeWindowsState((AssistantLayouts)m_nCurrentLayout);
      
      FullScreenModeOn();
      
#ifndef _STUDIO
      HideMenu();
      //HideStatusBar();
#endif
      
      HidePanes();
      
      if (m_nCurrentLayout == AssistantLayouts(StandardLayout))
      {
         m_nCurrentLayout = AssistantLayouts(FullscreenLayout);  
         HideTransparentButton();  
      }
      else if (m_nCurrentLayout == AssistantLayouts(StandardPagefocusLayout))
      {
         m_nCurrentLayout = AssistantLayouts(FullscreenPagefocusLayout);      
         HideTransparentButton(false);  
      }
    
   }
   m_uiPresentationTypeSelected = 1;
}

#ifdef _DVS2005
LRESULT CMainFrameA::OnButtonFullScreenCS(WPARAM wParam, LPARAM lParam) 
#else
void CMainFrameA::OnButtonFullScreenCS() 
#endif
{
   if ((m_nCurrentLayout == AssistantLayouts(StandardLayout)) || 
       (m_nCurrentLayout == AssistantLayouts(StandardPagefocusLayout)))
   {
      OnFullScreenCS();
   }
   else if((m_nCurrentLayout == AssistantLayouts(FullscreenLayout)) || 
       (m_nCurrentLayout == AssistantLayouts(FullscreenPagefocusLayout)))
   {
      FullScreenModeOff();
   }
   
#ifdef _DVS2005
   return 1;
#endif
}

// ESC key pressed
void CMainFrameA::OnAccVkESC() 
{
   // TODO: Add your command handler code here
   if((m_nCurrentLayout == AssistantLayouts(FullscreenLayout)) || 
      (m_nCurrentLayout == AssistantLayouts(FullscreenPagefocusLayout)))
   {
      FullScreenModeOff();  
   }
   else
   {  
      CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
      if (pDoc->HasSelectedObjects())
      {
         pDoc->UnSelectObjects();
         if (m_pWhiteboardView)
            m_pWhiteboardView->RedrawWindow();
      }
      else
      {
         if (m_pWhiteboardView)
         {
            ((CAssistantView *)m_pWhiteboardView)->ChangeTool(ID_TOOL_NOTHING);
         }
      }
   }
}

void CMainFrameA::OnShowStructure() 
{
   // TODO: Add your command handler code here
   if(m_paneManager.IsPaneClosed(IDR_PANE_DOCSTRUCT))
   {
      m_paneManager.ShowPane(IDR_PANE_DOCSTRUCT);
      m_bDocumentStructureVisibleStandard = true;
   }
   else
   {
      m_paneManager.ClosePane(IDR_PANE_DOCSTRUCT);
      m_bDocumentStructureVisibleStandard = false;
   }
   
}

void CMainFrameA::OnUpdateShowStructure(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here
   pCmdUI->SetCheck(!m_paneManager.IsPaneClosed(IDR_PANE_DOCSTRUCT));
   BOOL bEnable = TRUE;
   bool bPage = (m_nCurrentLayout == AssistantLayouts(StandardLayout) || 
                   m_nCurrentLayout == AssistantLayouts(FullscreenLayout)) ? true : false;
   /*if (m_bShowStartpage == true)  
   {
      bEnable = FALSE;
   }
   else*/ if(bPage == false)
   {
      bEnable = FALSE;
   }
   pCmdUI->Enable(bEnable);
}
void CMainFrameA::OnUpdateShowHideGroup(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here
   BOOL bEnable = TRUE;
   bool bPage = (m_nCurrentLayout == AssistantLayouts(StandardLayout) || 
                   m_nCurrentLayout == AssistantLayouts(FullscreenLayout)) ? true : false;
   if (bPage == false)
   {
      bEnable = FALSE;
   }
   pCmdUI->Enable(bEnable);
}

void CMainFrameA::OnShowRecordings() 
{
   // TODO: Add your command handler code here
   if(m_paneManager.IsPaneClosed(IDR_PANE_RECORDINGS))
   {
      m_paneManager.ShowPane(IDR_PANE_RECORDINGS);
      m_bRecordingStructureVisibleStandard = true;
   }
   else
   {
      m_paneManager.ClosePane(IDR_PANE_RECORDINGS);	
      m_bRecordingStructureVisibleStandard = false;
   }
}

void CMainFrameA::OnUpdateShowRecordings(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here
   pCmdUI->SetCheck(!m_paneManager.IsPaneClosed(IDR_PANE_RECORDINGS));	
   BOOL bEnable = TRUE;
   bool bPage = (m_nCurrentLayout == AssistantLayouts(StandardLayout) || 
                   m_nCurrentLayout == AssistantLayouts(FullscreenLayout)) ? true : false;
   /*if (m_bShowStartpage == true)  
   {
      bEnable = FALSE;
   }
   else*/ if(bPage == false)
   {
      bEnable = FALSE;
   }
   pCmdUI->Enable(bEnable);
}

void CMainFrameA::OnViewGrid() 
{
   // TODO: Add your command handler code here
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   CGridSettings grid(this);
   grid.Init(pDocument);
   int btnVal = grid.DoModal();
   pDocument->UpdateAllViews(NULL);
}



void CMainFrameA::OnUpdateViewGrid(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument->HasLoadedDocuments())
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
}

void CMainFrameA::OnCloseStartpage() 
{
    CloseStartPage();
}

void CMainFrameA::OnUpdateToolShowStructRec(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here
   pCmdUI->SetCheck(!m_paneManager.IsPaneClosed(IDR_PANE_DOCSTRUCT));	
}

void CMainFrameA::OnButtonPageFocusedLayout() 
{
#ifdef _DVS2005
	OnPageFocusedLayout(0, 0);
#else
	OnPageFocusedLayout();
#endif
}

#ifdef _DVS2005
LRESULT CMainFrameA::OnPageFocusedLayout(WPARAM wParam, LPARAM lParam) 
#else
void CMainFrameA::OnPageFocusedLayout() 
#endif
{
   if (m_nCurrentLayout == FullscreenLayout ||
       m_nCurrentLayout == StandardLayout)
   {
      ChangeWindowsState((AssistantLayouts)m_nCurrentLayout);

      if (m_nCurrentLayout == FullscreenLayout)
      {
#ifndef _STUDIO
         HideMenu();
         m_wndStatusBar.ShowWindow(SW_HIDE);
#endif
      }

      HidePanes();

      if (m_nCurrentLayout == FullscreenLayout)
      {
         m_nCurrentLayout = AssistantLayouts(FullscreenPagefocusLayout);             
      }
      else
      {
         m_nCurrentLayout = AssistantLayouts(StandardPagefocusLayout);             
      }
      HideTransparentButton(false);
   }
#ifdef _DVS2005
   return 1;
#endif
}

void CMainFrameA::OnUpdatePageFocusedLayout(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here
   pCmdUI->SetCheck(((m_nCurrentLayout == AssistantLayouts(StandardPagefocusLayout)) || (m_nCurrentLayout == AssistantLayouts(FullscreenPagefocusLayout))) ? true : false);	
   BOOL bEnable = TRUE;
   if (m_bShowStartpage == true)
   {
      bEnable = FALSE;
   }
   pCmdUI->Enable(bEnable);
}

void CMainFrameA::OnButtonStandardLayout() 
{
#ifdef _DVS2005
	OnStandardLayout(0, 0);
#else
	OnStandardLayout();
#endif
}

#ifdef _DVS2005
LRESULT CMainFrameA::OnStandardLayout(WPARAM wParam, LPARAM lParam) 
#else
void CMainFrameA::OnStandardLayout() 
#endif
{  
   if (m_nCurrentLayout == AssistantLayouts(FullscreenPagefocusLayout))
   {  
      //m_wndStatusBar.ShowWindow(SW_SHOW);
#ifndef _STUDIO
      if(m_bStatusBarVisibleStandard)
         m_wndStatusBar.ShowWindow(SW_SHOW);	
      else
         m_wndStatusBar.ShowWindow(SW_HIDE);
#endif

      HideTransparentButton();

      m_nCurrentLayout = AssistantLayouts(FullscreenLayout);
   }   
   else if (m_nCurrentLayout == AssistantLayouts(StandardPagefocusLayout))
   {         
#ifndef _STUDIO
      if (m_bStatusBarVisibleStandard)
         m_wndStatusBar.ShowWindow(SW_SHOW);
#endif

#ifndef _STUDIO
      ShowMenu();
      //ShowStatusBar();
#endif

      ShowPanes();

      HideTransparentButton();

      m_nCurrentLayout = AssistantLayouts(StandardLayout);

      StoreWindowsState(StandardLayout);
   }

#ifdef _DVS2005
   return 1;
#endif
}

void CMainFrameA::OnUpdateStandardLayout(CCmdUI* pCmdUI) 
{
    bool bEnable = true;
    if (m_bShowStartpage == true){
        bEnable = false;
    } else {
        pCmdUI->SetCheck(m_nCurrentLayout == AssistantLayouts(StandardLayout));
    }
    pCmdUI->Enable(bEnable);

}

void CMainFrameA::SwapRecordButtonImage(bool bGrayImage)
{
   if ( bGrayImage == true )
   {
      // change icon of disabled Record button to gray
      UINT uiStartRecordingDisabled[] = {-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,ID_START_RECORDING, -1, -1, -1};
      GetCommandBars()->GetImageManager()->SetIcons(IDB_PRESENTATIONBAR_ICONS_DISABLED, uiStartRecordingDisabled, _countof(uiStartRecordingDisabled), CSize(32, 32),xtpImageDisabled);
   }
   else
   {
      // change icon of disabled Record button to orange
      UINT uiStartRecordingDisabled[] = {-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,-1, ID_START_RECORDING, -1, -1};
      GetCommandBars()->GetImageManager()->SetIcons(IDB_PRESENTATIONBAR_ICONS_DISABLED, uiStartRecordingDisabled, _countof(uiStartRecordingDisabled), CSize(32, 32),xtpImageDisabled);
   }
}
void CMainFrameA::OnRecordAction() 
{
	CXTPControl* pStartRecording = NULL;

	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();

	if (pCommandBars != NULL)
	{
      CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBars->GetMenuBar();
		if ( pRibbonBar != NULL )
		{
			CXTPRibbonGroup *pGroup = (CXTPRibbonGroup*)pRibbonBar->FindGroup(ID_GROUP_RECORDER);
			if ( pGroup != NULL )
			{
				pStartRecording = (CXTPControl*)pGroup->FindControl(ID_START_RECORDING);
			}
		}
	}

	// swap function
	if ( m_bRecording == false )
	{
		CString csAction;
#ifdef UNICODE
		csAction.LoadStringW(ID_RECORDER_START_STOP);
#else
		csAction.LoadStringA(ID_RECORDER_START_STOP);
#endif // !UNICODE
		if ( pStartRecording != NULL )
			pStartRecording->SetDescription(csAction);
			
		OnStartRecording();
	}
	else
	{
		if ( pStartRecording != NULL )
			pStartRecording->SetDescription(_T("Stop"));
		OnStopRecording();
	}
}
// Ribbon->Start->Record button
void CMainFrameA::OnStartRecording() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   bool bSuccess = false;
   m_bRecordingIsSgOnly = false;
   ASSISTANT::Project::active->DoPureScreenGrabbing(false);
   if ( pDocument != NULL && !m_bRecording)
   {
      HRESULT hr = pDocument->StartRecording();
      if (SUCCEEDED(hr))
      {
         m_bRecording = true; 
         StartRecordingTimer();
         bSuccess = true;  
         ::SetFocus(m_hWnd);
         // make recording button image orange
         SwapRecordButtonImage(false);
      }
   }
   
   if (!bSuccess)
   {
      m_bRecording = false;
      StopRecordingTimer();
   }
   
}

void CMainFrameA::OnPauseRecording() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument != NULL && m_bRecording)
   {
      if (m_bRecordingPaused)
      {
         HRESULT hr = pDocument->UnPauseRecording();
         m_bRecordingPaused = false;
         if(m_nPauseButtonTimer > 0)
            KillTimer(m_nPauseButtonTimer);
         m_nPauseButtonTimer = 0;
         if (m_pPresentationBar != NULL) {
             m_pPresentationBar->ChangeFlashStatus(false);
         }
      }
      else
      {
         HRESULT hr = pDocument->PauseRecording();
         if (SUCCEEDED(hr))
		 {
            m_bRecordingPaused = true; 
            if(m_nPauseButtonTimer == 0)
               m_nPauseButtonTimer = SetTimer(PAUSE_TIMER, 500, 0);
            if (m_pPresentationBar != NULL) {
                m_pPresentationBar->ChangeFlashStatus(true);
            }
         }
         else
            m_bRecordingPaused = false;
      }
   }
   else
   {
      m_bRecordingPaused = false;
   }
}

void CMainFrameA::OnStopRecording() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   UINT nSnapSet, nSpacingValue, nDisplayGrid;

   if (m_bRecordingIsSgOnly)
   {
      OnToolScreengrabbing();
      return;
   }
   SwapRecordButtonImage(true);
   ASSISTANT::Project::active->GetGridSettings(nSnapSet, nSpacingValue, nDisplayGrid);
   m_bScreenGrabing = ASSISTANT::Project::active->isScreenGrabbingActive_;

   if(m_bDisplayGrid && m_bScreenGrabing)
   {
      ASSISTANT::Project::active->SetGridSettings(nSnapSet, nSpacingValue, 1);
      RedrawWindow();
   }
   else if(!m_bDisplayGrid && m_bScreenGrabing)
   {
      ASSISTANT::Project::active->SetGridSettings(nSnapSet, nSpacingValue, 0);
      RedrawWindow();
   }

   if (pDocument != NULL && m_bRecording)
   {
      pDocument->StopRecording();
      pDocument->RebuildRecordingTree(m_pRecordingsView);

#ifndef _STUDIO
      RestoreAssistant();
      UndoReduceAssistant();
      ResetToTop();
#else
      RestoreAssistant();
#endif
   }

   StopRecordingTimer();

   RestorePresentationBarFromSgRec();
   // needs to change parent, if previously was in screen grabbing mode.
   if ( ASSISTANT::Project::active->IsPureScreenGrabbing() == true )
   {
      m_pAudioWidget->ShowWindow(SW_HIDE);
      if ( m_bScreenGrabing == true )
         ChangeModes(m_pAudioWidget, this);
   }
   else
   {
      m_pMonitorWidget->DisableLimitToWindow(false);
      m_pMonitorWidget->ShowWindow(GetShowAVMonitor() ? SW_SHOW : SW_HIDE);
      if ( m_bScreenGrabing == true )
         ChangeModes((CAudioWidget*)m_pMonitorWidget, this);
   }

   m_bRecording = false;
   m_bRecordingPaused = false; 

    OpenLastRecording();
   //ASSISTANT::Project::active->DoPureScreenGrabbing(true);
}

void CMainFrameA::OnUpdateRecordAction(CCmdUI* pCmdUI) 
{
   if ( pCmdUI == NULL )
       return;

   if (!ASSISTANT::Project::active->IsAudioDeviceAvailable()) {
       pCmdUI->Enable(FALSE);
   } else {

       switch (pCmdUI->m_nID ) {
           // Record button from Ribbon->Start tab
       case ID_START_RECORDING:
           {
               CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
               if ( pDoc == NULL )
               {
                   pCmdUI->Enable(FALSE);
                   break;
               }
               if (pDoc->HasLoadedDocuments() == true)
               {
                   CDocumentStructureView *pStructView = GetStructureTree();
                   if(pStructView ==  NULL)
                   {
                       // Page Structure view has never been visible. Check to see if there is active document.
                       if ( ASSISTANT::Project::active->GetActiveDocument() != NULL )
                       {
                           pCmdUI->Enable(TRUE);
                           break;
                       }
                       pCmdUI->Enable(FALSE);
                       break;
                   }
                  
                   CXTPReportControl& wndReport = pStructView->GetReportCtrl();
                   bool bSelected = (wndReport.GetSelectedRows()->GetCount() > 0);
                   if (!bSelected) {
                       pCmdUI->Enable(FALSE);
                       break;
                   }
                   CXTPReportRow* pRow = wndReport.GetSelectedRows()->GetAt(0);
                   CDocStructRecord *pRecord = (CDocStructRecord*)pRow->GetRecord();
                   if ( pRecord == NULL )
                   {
                       pCmdUI->Enable(FALSE);
                       break;
                   }
                   if ((pRecord->IsDocument() && pRecord->IsLoadedDocument() == TRUE)
                       || pRecord->IsPage() 
                       || pRecord->IsChapter()
                       )
                       // document is selected and loaded. so enable Record button acordingly to the recording state
                       pCmdUI->Enable(!m_bRecording);
                   else 
                       // document selected is not loaded. Disable Record button.
                       pCmdUI->Enable(FALSE);
               }
               else
                   // Disable Record button as no document is loaded
                   pCmdUI->Enable(FALSE);
               break;
           }
       case ID_START_RECORDING_STARTUP_PAGE:
           {
               if ( pCmdUI != NULL )
                   pCmdUI->Enable(!m_bRecording);
               break;
           }
       }
   }
   if(m_bRecording)
   {
       AddTooltip(ID_GROUP_RECORDER, ID_START_RECORDING, IDS_TOOLTIP_RECORD_S2);
       //AddTooltip(ID_GROUP_RECORDER, ID_TOOL_SCREENGRABBING, IDS_TOOLTIP_SG_S2);
   }
   else
   {
       AddTooltip(ID_GROUP_RECORDER, ID_START_RECORDING, IDS_ID_START_RECORDING);
       //AddTooltip(ID_GROUP_RECORDER, ID_TOOL_SCREENGRABBING, ID_TOOL_SCREENGRABBING);
   }
}

void CMainFrameA::OnUpdatePauseRecording(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument != NULL)
   {
      if (m_bRecording)
      {
         if (m_bRecordingPaused)
         {
            pCmdUI->SetCheck(m_nFlashPause); 
            //AddTooltip(ID_GROUP_RECORDER, ID_PAUSE_RECORDING, IDS_TOOLTIP_PAUSE_S2);
         }
         else
         {
            pCmdUI->SetCheck(false); 
         //AddTooltip(ID_GROUP_RECORDER, IDS_ID_PAUSE_RECORDING, ID_PAUSE_RECORDING);
         }
         pCmdUI->Enable(true); 
      }
      else
         pCmdUI->Enable(false);
   }
   else
      pCmdUI->Enable(false);
   if(m_bRecordingPaused)
      AddTooltip(ID_GROUP_RECORDER, ID_PAUSE_RECORDING, IDS_TOOLTIP_PAUSE_S2);
   else
      AddTooltip(ID_GROUP_RECORDER, ID_PAUSE_RECORDING, IDS_ID_PAUSE_RECORDING);

   
}

void CMainFrameA::OnUpdateStopRecording(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument != NULL)
   {
      if (m_bRecording)
         pCmdUI->Enable(true); 
      else
         pCmdUI->Enable(false);
   }
   else
      pCmdUI->Enable(false);
   
}

void CMainFrameA::OnUpdateNextPage(CCmdUI* pCmdUI) 
{
   bool bEnable = false;
   
   if (ASSISTANT::Project::active != NULL)
   {
      if (ASSISTANT::Project::active->GetActivePage())
         bEnable = true;
   }
   
   pCmdUI->Enable(bEnable);	
   //   pCmdUI->Enable(true);	
}

void CMainFrameA::OnUpdatePreviousPage(CCmdUI* pCmdUI) 
{
   bool bEnable = false;
   
   if (ASSISTANT::Project::active != NULL)
   {
      if (ASSISTANT::Project::active->GetActivePage())
         bEnable = true;
   }
   
   pCmdUI->Enable(bEnable);		
   //   pCmdUI->Enable(true);		
}

void CMainFrameA::OnShowImportWarnings() 
{
   SetShowImportWarning(!(GetShowImportWarning()));
}

void CMainFrameA::OnUpdateShowImportWarnings(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(GetShowImportWarning());	
}

void CMainFrameA::OnShowAudioWarning() 
{
   SetShowAudioWarning(!(GetShowAudioWarning()));	
}

void CMainFrameA::OnUpdateShowAudioWarning(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(GetShowAudioWarning());
}

void CMainFrameA::OnShowUseSmartBoard() 
{
   SetShowUseSmartBoard(!(GetShowUseSmartBoard()));	
}

void CMainFrameA::OnUpdateShowUseSmartBoard(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(GetShowUseSmartBoard());
}

void CMainFrameA::OnShowStartpage() 
{
    SetShowStartupPage(!(GetShowStartupPage()));	

    if (!GetShowStartupPage()) 
        CloseStartPage();

    CheckAssistantMainView();
}

void CMainFrameA::OnUpdateShowStartpage(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(GetShowStartupPage());
}

void CMainFrameA::OnShowMissingFonts() 
{
   SetShowMissingFontsWarning(!(GetShowMissingFontsWarning()));
}

void CMainFrameA::OnUpdateShowMissingFonts(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(true);
   pCmdUI->SetCheck(GetShowMissingFontsWarning());
}


void CMainFrameA::DoChangeTool(short nIndexTool)
{
   if(m_iSelectedPresentationTool==nIndexTool)
      m_iSelectedPresentationTool = 0;
   else
      m_iSelectedPresentationTool = nIndexTool;
}

void CMainFrameA::OnToolDefineArea()
{
	ASSISTANT::Project::active->StartDefineArea();
}

// Ribbon->Start->Select Desktop Area
void CMainFrameA::OnToolScreengrabbing() {
    StartStopScreenRecording();
}

void CMainFrameA::StartStopScreenRecording() {
    if (!ASSISTANT::Project::active->IsAudioDeviceAvailable())
        return;

   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   m_bScreenGrabing = false;
   if (ASSISTANT::Project::active == NULL)
      return;
      
   m_bScreenGrabing = ASSISTANT::Project::active->isScreenGrabbingActive_;

   // Start Screen grabbing
   if (!m_bScreenGrabing) {
      bool bSetPureSG = m_bRecording && ASSISTANT::Project::active->IsPureScreenGrabbing() == false ? false : true;
      ASSISTANT::Project::active->DoPureScreenGrabbing(bSetPureSG);
#ifndef _STUDIO
      if (ASSISTANT::Project::active->ReduceAtStartSg())
         ReduceAssistant();
      if (ASSISTANT::Project::active->MinimizeAtStartSg())
         MinimizeAssistant();
      if (ASSISTANT::Project::active->AlwaysOnTop())
         SetToTop();
#else
      MinimizeAssistant();
#endif

      if (ASSISTANT::Project::active->GrabAtFixedPos()) {
         StartSGRecording(ASSISTANT::Project::active->GetSGRectangle());
         ShowPresentationBarInSgRec(true);
      } else if (m_bStartSgFromHotKeys) {
         CRect rcLastSelection;
         bool bIsFullScreen;
         pDoc->GetLastSelectionRectangle(rcLastSelection, bIsFullScreen);
         if (bIsFullScreen) 
            MfcUtils::GetDesktopRectangle(rcLastSelection);
         StartSGRecording(rcLastSelection);
         ShowPresentationBarInSgRec(true);
         m_bStartSgFromHotKeys = false;
      } else {
         StartNewSgSelection();
      }

      if (ASSISTANT::Project::active->IsPureScreenGrabbing()) {
         // Show audio widget
         CRect rcWidget;
         m_pAudioWidget->GetWindowRect(rcWidget);
         m_pAudioWidget->StartSavePosition();
         if ( m_pAudioWidget->UpdateWindowPlacement() == false )
            m_pAudioWidget->SetWindowPos(NULL, 100, 100, rcWidget.Width(), rcWidget.Height(), SWP_NOZORDER);
         m_pAudioWidget->ShowWindow(pDoc->GetShowAudioMonitor() ? SW_SHOW : SW_HIDE );
         if (m_nAudioTimer == 0)
            m_nAudioTimer = SetTimer(AUDIO_TIMER, 250, 0);
         // there cannot be 2 widgets. make sure
         if (m_pMonitorWidget)
             m_pMonitorWidget->ShowWindow(SW_HIDE);
      } else {
         if (m_pMonitorWidget)
             m_pMonitorWidget->DisableLimitToWindow();
         ShowMonitorWidget();
      }
   } else { // Stop screen grabbing
      UINT nSnapSet, nSpacingValue, nDisplayGrid;

      // audio widget is visible only in a pure screen recording
      if ( m_pAudioWidget->GetSafeHwnd() && m_pAudioWidget->IsWindowVisible() == TRUE )
         m_pAudioWidget->ShowWindow(SW_HIDE);

      RestorePresentationBarFromSgRec();

      ChangeModes(ASSISTANT::Project::active->IsPureScreenGrabbing() ? m_pAudioWidget : (CAudioWidget*)m_pMonitorWidget, this);
      if (ASSISTANT::Project::active->IsPureScreenGrabbing()) {
         m_pAudioWidget->ShowWindow(SW_HIDE);
      } else {
         m_pMonitorWidget->DisableLimitToWindow(false);
         ShowMonitorWidget();
      }

      if (m_pRecordingsView != NULL)
         m_pRecordingsView->RedrawWindow();
#ifndef _STUDIO
      RestoreAssistant();
      UndoReduceAssistant();
      ResetToTop();
#else
      RestoreAssistant();
#endif
      pDoc->StopScreengrabbing();
      if (pDoc->IsSgOnlyDocument()) {
         StopRecordingTimer();
         pDoc->RebuildRecordingTree(m_pRecordingsView);
         m_bRecording = false;
      }

      ASSISTANT::Project::active->GetGridSettings(nSnapSet, nSpacingValue, nDisplayGrid);
      if(m_bDisplayGrid) {
         ASSISTANT::Project::active->SetGridSettings(nSnapSet, nSpacingValue, 1);
         RedrawWindow();
      } else {
         ASSISTANT::Project::active->SetGridSettings(nSnapSet, nSpacingValue, 0);
         RedrawWindow();
      }
      ASSISTANT::Project::active->DoPureScreenGrabbing(false);
      bool bSetPureSG = m_bRecording ? false : true;
      if (bSetPureSG) {
          OpenLastRecording();
      }

      if (CStudioApp::IsLiveContextMode()) {
          CStudioApp::QuitLiveContextWithCode(EXIT_CODE_LC_SUCCESS);
      }
   }
}

void CMainFrameA::OnUpdateToolDefineArea(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (m_bRecording || pDoc->IsSgOnlyDocument())
      pCmdUI->Enable(true);  
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnUpdateToolScreengrabbing(CCmdUI* pCmdUI) 
{
   if (!ASSISTANT::Project::active->IsAudioDeviceAvailable()) {
       pCmdUI->Enable(FALSE);
       pCmdUI->SetCheck(FALSE);
   } else {
       pCmdUI->Enable(true);

       BOOL bScreenGrabbingActive = FALSE;
       if (ASSISTANT::Project::active != NULL)
           bScreenGrabbingActive = 
           ASSISTANT::Project::active->isScreenGrabbingActive_ == false ? FALSE : TRUE;


       pCmdUI->SetCheck(bScreenGrabbingActive);
       if(bScreenGrabbingActive && !m_bRedrawWindow)
       {
           if (m_pRecordingsView != NULL)
               m_pRecordingsView->RedrawWindow();
           if (m_pDocumentStructureView != NULL)
               m_pDocumentStructureView->RedrawWindow();
           m_bRedrawWindow = true;
       }
       if(!bScreenGrabbingActive)
           m_bRedrawWindow = false;
   }
   if(m_bRecording)
   {
      //AddTooltip(ID_GROUP_RECORDER, ID_START_RECORDING, IDS_TOOLTIP_RECORD_S2);
      AddTooltip(ID_GROUP_RECORDER, ID_TOOL_SCREENGRABBING, IDS_TOOLTIP_SG_S2);
   }
   else
   {
      //AddTooltip(ID_GROUP_RECORDER, ID_START_RECORDING, ID_START_RECORDING);
      if(ASSISTANT::Project::active->GrabAtFixedPos())
         AddTooltip(ID_GROUP_RECORDER, ID_TOOL_SCREENGRABBING, IDS_TOOLTIP_SG_S3);
      else
         AddTooltip(ID_GROUP_RECORDER, ID_TOOL_SCREENGRABBING, IDS_ID_TOOL_SCREENGRABBING);
   }
}

LRESULT CMainFrameA::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{  
   if (wParam == XTP_DPN_SHOWWINDOW)
   {
      CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
      CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
      
	  CString csTitle;
      if (!pPane->IsValid() && pDoc)
      {
         switch (pPane->GetID())
         {
         case IDR_PANE_DOCSTRUCT:  	
            if (!m_pDocumentStructureView)
            {
               m_pDocumentStructureView = (CDocumentStructureView *)pPane->AttachView(this, RUNTIME_CLASS(CDocumentStructureView), pDoc);
			   
            }
            else
               pPane->Attach(m_pDocumentStructureView);

			csTitle.LoadString(IDR_PANE_DOCSTRUCT);
			pPane->SetTitle(csTitle);
            break;
         case IDR_PANE_RECORDINGS:	
            if (!m_pRecordingsView)
            {
               m_pRecordingsView = (CRecordingsView *)pPane->AttachView(this, RUNTIME_CLASS(CRecordingsView), pDoc);
            }
            else
               pPane->Attach(m_pRecordingsView);
			csTitle.LoadString(IDR_PANE_RECORDINGS);
			pPane->SetTitle(csTitle);
         }
      }
      return TRUE;
   }
   return FALSE;
}

void CMainFrameA::StartSGRecording(CRect &rcSelection)
{
    CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

    if (!pDoc) 
        return;

    pDoc->StartScreengrabbing(rcSelection);
    if (pDoc->IsSgOnlyDocument())
    {
        m_bRecordingIsSgOnly = true;
        StartRecordingTimer();
        m_bRecording = true;
    }
    else
        m_bRecordingIsSgOnly = false;

    if(pDoc->IsGridVisible())
    {
        UINT nSnapSet, nSpacingValue, nDisplayGrid;
        ASSISTANT::Project::active->GetGridSettings(nSnapSet, nSpacingValue, nDisplayGrid);
        ASSISTANT::Project::active->SetGridSettings(nSnapSet, nSpacingValue, 0);
        m_bDisplayGrid = true;
        RedrawWindow();
    }
    else
        m_bDisplayGrid = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CMainFrameA::OnSetupPage() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument && pDocument->HasLoadedDocuments())
   {
      UINT nPageWidth, nPageHeight;
      COLORREF clrBackground;
      
      //pDocument->GetPageProperties(nPageWidth, nPageHeight, clrBackground);
      
      ASSISTANT::Project::active->GetPageProperties(nPageWidth, nPageHeight, clrBackground);
	  COLORREF clrBackgroundColor = ASSISTANT::GdipARGBToColorref(clrBackground);
      CDlgPageSetup dlgPageSetup(this, nPageWidth, nPageHeight, clrBackgroundColor);
      
      int btnVal = dlgPageSetup.DoModal();
      if(btnVal==IDOK) 
      {
         
         UINT nPageWidth = dlgPageSetup.GetWidth();
         UINT nPageHeight = dlgPageSetup.GetHeight();
         COLORREF clrBackground = dlgPageSetup.GetBackgroundColor();
         
         ASSISTANT::Project::active->SetPageProperties(nPageWidth, nPageHeight, clrBackground);
         
         if (m_pWhiteboardView)
            m_pWhiteboardView->RedrawWindow();
      }
   }
}

void CMainFrameA::OnUpdateSetupPage(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument && 
      pDocument->HasLoadedDocuments() && pDocument->HasActivePage())
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnChangeAudiovideo() 
{
    ShowAudioSettings();
}

void CMainFrameA::OnUpdateChangeAudiovideo(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   bool bEnabled = true;
   if (pDocument && pDocument->IsRecording())
   {
      bEnabled = false;
   }
   
   pCmdUI->Enable(bEnabled);
}

void CMainFrameA::OnChangeSGOptions() 
{
    ShowSgSettings();
}

void CMainFrameA::OnUpdateChangeSGOptions(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   bool bEnabled = true;
   if (pDocument && pDocument->IsRecording())
   {
      bEnabled = false;
   }
   
   pCmdUI->Enable(bEnabled);
}

void CMainFrameA::OnShowProperties() 
{
   // TODO: Add your command handler code here
   CDlgMetadata dlgMetadata;
   dlgMetadata.SetTitleVisibility(false);
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   CString csTitle;
   CString csAuthor;
   CString csProducer;
   CString csKeywords;
   
   if (pDocument)
      pDocument->GetMetadata(csTitle, csAuthor, csProducer, csKeywords);
   
   dlgMetadata.Init(csTitle, csAuthor, csProducer, csKeywords);
   dlgMetadata.DoModal();
   
   csTitle = dlgMetadata.GetTitle();
   csAuthor = dlgMetadata.GetAuthor();
   csProducer = dlgMetadata.GetProducer();
   csKeywords = dlgMetadata.GetKeywords();
   
   pDocument->SetMetadata(csTitle, csAuthor, csProducer, csKeywords);
   
}

void CMainFrameA::OnLsdClose() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument) {
      HRESULT hr = pDocument->CloseDocument();
   }
}

void CMainFrameA::RemoveViewsContent()
{
   if (m_pDocumentStructureView && ASSISTANT::Project::active != NULL)
      m_pDocumentStructureView->RemoveAll();

   if (m_pRecordingsView && ASSISTANT::Project::active != NULL)
      m_pRecordingsView->RemoveAll();
}

void CMainFrameA::OnUpdateLsdClose(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument && pDocument->ActiveDocumentIsLoaded())
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnLsdNew() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument)
   {
      HRESULT hr = pDocument->InsertNewDocument(m_pDocumentStructureView);
   }
}

void CMainFrameA::OnUpdateLsdNew(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument && pDocument->HasOpenProject())
   {
      if (pDocument->IsRecording())
      {
         if (ASSISTANT::Project::active && 
            ASSISTANT::Project::active->isScreenGrabbingActive_)
            pCmdUI->Enable(false);
         else
            pCmdUI->Enable(true);
      }
   }
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnLsdOpen() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument)
   {
      CString csDocumentName;
      HRESULT hr = ASSISTANT::GetOpenFilename(IDS_LSD_FILTER, csDocumentName, _T("LSD"));
      
      if (SUCCEEDED(hr) && !csDocumentName.IsEmpty())
      {
         hr = pDocument->OpenSourceDocument(csDocumentName); 
      }
   }
}

void CMainFrameA::OnUpdateLsdOpen(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument && pDocument->HasOpenProject())
   {
      if (pDocument->IsRecording())
      {
         if (ASSISTANT::Project::active && 
            ASSISTANT::Project::active->isScreenGrabbingActive_)
            pCmdUI->Enable(false);
         else
            pCmdUI->Enable(true);
      }
   }
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnImportPPT() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument)
   {
      bool bIs64BitPowerPoint = pDocument->Is64BitPowerPointInstalled();
      if (bIs64BitPowerPoint)
      {
         CString cap;
         cap.LoadString(IDS_ERROR);
         CString csError;
         csError.LoadString(IDS_64BIT_POWERPOINT);
         MessageBox(csError, cap, MB_OK | MB_ICONERROR | MB_TOPMOST);
         return;
      }

      CString csPowerPointName;
      HRESULT hr = ASSISTANT::GetOpenFilename(IDS_PPT_FILTER, csPowerPointName, _T("PPT"));
      
      if (SUCCEEDED(hr) && !csPowerPointName.IsEmpty())
      {
         //WriteHWNDToRegistry();

         hr = pDocument->ImportPowerPoint(csPowerPointName, false, false); 

         //DeleteHWNDFromRegistry();

         if (SUCCEEDED(hr))
            pDocument->RebuildStructureTree(m_pDocumentStructureView);
         
         CheckAssistantMainView();

         pDocument->UpdateAllViews(NULL);
      }
   }
}

void CMainFrameA::OnImportPPTAs() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument)
   {
      bool bIs64BitPowerPoint = pDocument->Is64BitPowerPointInstalled();
      if (bIs64BitPowerPoint)
      {
         CString cap;
         cap.LoadString(IDS_ERROR);
         CString csError;
         csError.LoadString(IDS_64BIT_POWERPOINT);
         MessageBox(csError, cap, MB_OK | MB_ICONERROR | MB_TOPMOST);
         return;
      }

      CString csPowerPointName;
      HRESULT hr = ASSISTANT::GetOpenFilename(IDS_PPT_FILTER, csPowerPointName, _T("PPT"));
      
      if (SUCCEEDED(hr) && !csPowerPointName.IsEmpty())
      {
         //WriteHWNDToRegistry();

         hr = pDocument->ImportPowerPoint(csPowerPointName, true, false); 

         //DeleteHWNDFromRegistry();
   
         if (SUCCEEDED(hr))
            pDocument->RebuildStructureTree(m_pDocumentStructureView);
         
         CheckAssistantMainView();
         
         pDocument->UpdateAllViews(NULL);
      }
   }
}

void CMainFrameA::OnUpdateImportPowerPointAs(CCmdUI* pCmdUI) 
{
    CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

    BOOL bEnable = TRUE;

    // Not usable in PowerTrainer mode
    if (CStudioApp::IsPowerTrainerMode())
        bEnable = FALSE;

    // Not usable during Screengrabbing recording
    if (pDocument && pDocument->HasOpenProject()) {
        if (pDocument->IsRecording()) {
            if (ASSISTANT::Project::active && 
                ASSISTANT::Project::active->isScreenGrabbingActive_)
            bEnable = FALSE;
        }
    }
    else
        bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}

void CMainFrameA::OnImportPPTAndRecord()
{
    CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

    if (pDocument) {
        bool bIs64BitPowerPoint = pDocument->Is64BitPowerPointInstalled();
        if (bIs64BitPowerPoint)
        {
            CString cap;
            cap.LoadString(IDS_ERROR);
            CString csError;
            csError.LoadString(IDS_64BIT_POWERPOINT);
            MessageBox(csError, cap, MB_OK | MB_ICONERROR | MB_TOPMOST);
            return;
        }

        CString csPowerPointName;
        HRESULT hr = ASSISTANT::GetOpenFilename(IDS_PPT_FILTER, csPowerPointName, _T("PPT"));

        if (SUCCEEDED(hr) && !csPowerPointName.IsEmpty()) {
            //WriteHWNDToRegistry();

            hr = pDocument->ImportPowerPoint(csPowerPointName, false, true); 

            //DeleteHWNDFromRegistry();

            if (SUCCEEDED(hr))
                pDocument->RebuildStructureTree(m_pDocumentStructureView);

            CheckAssistantMainView();

            pDocument->UpdateAllViews(NULL);

            if (SUCCEEDED(hr) && pDocument->HasLoadedDocuments())
                OnStartRecording();
        }
    }
}

void CMainFrameA::OnLsdSave() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument)
   {
      pDocument->SaveSourceDocument();
   }
}

void CMainFrameA::OnUpdateLsdSave(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument && pDocument->HasLoadedDocuments())
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnLsdSaveAs() 
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument)
   {
      pDocument->SaveSourceDocumentAs();
   }
}

void CMainFrameA::OnPreviousPage() 
{
   GoToPreviousPage();
}

void CMainFrameA::OnNextPage() 
{
   GoToNextPage();
}

BOOL CMainFrameA::OnCommand(WPARAM wParam, LPARAM lParam)
{
   WORD nCmdID = LOWORD(wParam);

#ifdef _STUDIO
   BOOL bResult =  CMDIChildWnd::OnCommand(wParam, lParam);
#else
   BOOL bResult =  CFrameWnd::OnCommand(wParam, lParam);
#endif
   
   /*if (nCmdID >= ID_FILE_MRU_FILE1 && nCmdID < ID_FILE_MRU_FILE1 + _AFX_MRU_COUNT)
   {
      if (m_pStartupView)
      {
         ((CStartupView *)m_pStartupView)->UpdateRecentFileList();
      }
   }*/

   return bResult;
}

BOOL CMainFrameA::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   BOOL bResult = FALSE;
   
   if (m_pWhiteboardView)
   {
      bResult = m_pWhiteboardView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
   }
   
   if (bResult)
      return TRUE;
   

#ifdef _STUDIO
   return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
#else
   return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
#endif
}

CDocumentStructureView *CMainFrameA::GetStructureTree()
{
   return m_pDocumentStructureView;
}

CRecordingsView *CMainFrameA::GetRecordingView()
{
   return m_pRecordingsView;
}

CAssistantView *CMainFrameA::GetWhiteboardView()
{
   return (CAssistantView *)m_pWhiteboardView;
}

// TODO: This is nasty side effect in numerous places (or the Rebuild methods are)
void CMainFrameA::UpdateStructureAndRecordingsView() {
    CAssistantDoc *pDocument = (CAssistantDoc *)GetAssistantDocument();
    if (pDocument != NULL && m_pDocumentStructureView != NULL)
        pDocument->RebuildStructureTree(m_pDocumentStructureView);
    if (pDocument != NULL && m_pRecordingsView != NULL)
        pDocument->RebuildRecordingTree(m_pRecordingsView);
}

void CMainFrameA::RefreshPaneDrawing()
{
    m_paneManager.GetPaintManager()->RefreshMetrics();
    m_paneManager.RedrawPanes();
}

void CMainFrameA::OnProjectOpen() 
{
   CString csProjectName;
   HRESULT hr = ASSISTANT::GetOpenFilename(IDS_LAP_FILTER, csProjectName, _T("LAP"));
   
   if (!csProjectName.IsEmpty())
   {
      AfxGetApp()->OpenDocumentFile(csProjectName);
   }
}

void CMainFrameA::OnCutObjects()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments())
   {
      pDoc->CutObjects();
      SetRefreshTransparentBtn();
      m_pWhiteboardView->RedrawWindow();
   }
}

void CMainFrameA::OnUpdateCutCopy(CCmdUI* pCmdUI)
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
      pDoc->HasActivePage() && pDoc->HasSelectedObjects())
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnDeleteObjects()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments())
   {
      pDoc->DeleteObjects();
      SetRefreshTransparentBtn();
      m_pWhiteboardView->RedrawWindow();
   }
}

void CMainFrameA::OnUpdateDeleteCopy(CCmdUI* pCmdUI)
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
      pDoc->HasActivePage() && pDoc->HasSelectedObjects())
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnCopyObjects()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments())
   {
      pDoc->CopyObjects();
      
      m_pWhiteboardView->RedrawWindow();
   }
}

void CMainFrameA::OnPasteObjects()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments())
   {
      pDoc->PasteObjects();
      SetRefreshTransparentBtn();
      m_pWhiteboardView->RedrawWindow();
   }
}

void CMainFrameA::OnUpdatePaste(CCmdUI* pCmdUI)
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
      pDoc->HasActivePage() && !pDoc->PasteBufferIsEmpty())
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnEditSelectAll() 
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments())
   {
      pDoc->SelectAllObjects();
      m_pWhiteboardView->RedrawWindow();
   }
}

void CMainFrameA::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments() && pDoc->HasActivePage())
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
}

void CMainFrameA::OnFreezeImages() 
{
   bool bFreezeImages = GetFreezeImages();
   bFreezeImages = !bFreezeImages;
   SetFreezeImages(bFreezeImages);
   
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   if (pDoc != NULL)
   {
      pDoc->SetFreezeImages(bFreezeImages);
      if (m_pWhiteboardView)
         m_pWhiteboardView->RedrawWindow();
   }
}

void CMainFrameA::OnUpdateFreezeImages(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   BOOL bEnable = TRUE;
   if ((m_bShowStartpage == true) || (pDoc->HasActivePage() == FALSE))
   {
      bEnable = FALSE;
   }
   
   bool bFreezeImages = GetFreezeImages();
   
   if (bFreezeImages)
      pCmdUI->SetCheck(true);
   else
      pCmdUI->SetCheck(false);
   
   pCmdUI->Enable(bEnable);
}

void CMainFrameA::OnFreezeMaster() 
{
   bool bFreezeMaster = GetFreezeMaster();
   bFreezeMaster = !bFreezeMaster;
   SetFreezeMaster(bFreezeMaster);
   
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   if (pDoc != NULL)
   {
      pDoc->SetFreezeMaster(bFreezeMaster);
      if (m_pWhiteboardView)
         m_pWhiteboardView->RedrawWindow();
   }
}

void CMainFrameA::OnUpdateFreezeMaster(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   BOOL bEnable = TRUE;
   if ((m_bShowStartpage == true) || (pDoc->HasActivePage() == FALSE))
   {
      bEnable = FALSE;
   }
   
   bool bFreezeMaster = GetFreezeMaster();
   
   if (bFreezeMaster)
      pCmdUI->SetCheck(true);
   else
      pCmdUI->SetCheck(false);
   
   pCmdUI->Enable(bEnable);
}

void CMainFrameA::OnFreezeDocument() 
{
   bool bFreezeDocument = GetFreezeDocument();
   bFreezeDocument = !bFreezeDocument;
   SetFreezeDocument(bFreezeDocument);
   
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   if (pDoc != NULL)
   {
      pDoc->SetFreezeDocument(bFreezeDocument);
      if (m_pWhiteboardView)
         m_pWhiteboardView->RedrawWindow();
   }
}

void CMainFrameA::OnUpdateFreezeDocument(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   bool bFreezeDocument = GetFreezeDocument();
   BOOL bEnable = TRUE;
   if ((m_bShowStartpage == true) || (pDoc->HasActivePage() == FALSE))
   {
      bEnable = FALSE;
   }
   if (bFreezeDocument)
      pCmdUI->SetCheck(true);
   else
      pCmdUI->SetCheck(false);
   
   pCmdUI->Enable(bEnable);
}

#ifdef _DVS2005
LRESULT CMainFrameA::OnHotkeys(WPARAM wParam, LPARAM lParam)
#else
void CMainFrameA::OnHotkeys(WPARAM wParam, LPARAM lParam)
#endif
{
   if (wParam == ID_TOOL_SCREENGRABBING)
   {
      if(m_pWndCaptureRect != NULL)
      {
         FinishedSgSelection(m_pWndCaptureRect->IsDoRecordFullScreen());
      }
      else
      {
         if(ASSISTANT::Project::active != NULL &&
            !ASSISTANT::Project::active->isScreenGrabbingActive_)
            m_bStartSgFromHotKeys = true;
         OnToolScreengrabbing(); 
      }
   }
      //OnStartRecording();
#ifndef _STUDIO
   else if (wParam == ID_REDUCE)
   {
      if (m_nCurrentLayout != AssistantLayouts(ReducedLayout))
         ReduceAssistant();
      else
         UndoReduceAssistant();
   }
   else if (wParam == ID_MINIMIZE)
   {
      if (!m_bMinimized)
         MinimizeAssistant();
      else
         RestoreAssistant();

   }
#endif
   else if (wParam == ID_RESET_SG_TOOL)
   {
      ResetSgTool();
   }
#ifdef _DVS2005
   return 1;
#endif
}

void CMainFrameA::ResetSgTool() 
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc && pDoc->IsRecording())
   {
      if (ASSISTANT::Project::active && 
         ASSISTANT::Project::active->isScreenGrabbingActive_)
      {
         if (m_pWhiteboardView)
            ((CAssistantView *)m_pWhiteboardView)->ChangeTool(ID_TOOL_NOTHING);
      }
   }   
}




void CMainFrameA::OnTimer(UINT nIDEvent) 
{
   // TODO: Add your message handler code here and/or call default
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDocument != NULL)
   {
      if(nIDEvent == m_nAudioTimer)
      {
         double dAudioValue = pDocument->GetAudioLevel();
         if(m_pMonitorWidget != NULL)
            m_pMonitorWidget->SetAudioPosition(dAudioValue); 
         if ( m_pAudioWidget != NULL && m_pAudioWidget->GetSafeHwnd() != NULL)
            m_pAudioWidget->SetAudioPosition(dAudioValue); 
      }
      if(nIDEvent == m_nVideoTimer)
      {
         // function shoul be call with parameters 
         if (ASSISTANT::Project::active)
            ASSISTANT::Project::active->UpdateVideoThumb();
         
         if(m_pMonitorWidget != NULL)
            m_pMonitorWidget->UpdateVideoThumbnail(ASSISTANT::Project::thumbInfo, ASSISTANT::Project::thumbData);
      }
      
      if(nIDEvent == m_nCheckDiskSpace)
      {
         if (ASSISTANT::Project::active)
            ASSISTANT::Project::active->CheckDiskSpace(false);
      }
      
      if(nIDEvent == m_nCheckAudioLevel)
      {
         if (ASSISTANT::Project::active)
            ASSISTANT::Project::active->CheckAudioLevel(false);
      }
      
      if(nIDEvent == m_nUpdateDiscSpace)
      {
         UpdateStatusDiskSpace();
      }

      if(nIDEvent == m_nUpdateRecordTime)
      {
         UpdateStatusDuration();
      }
   }
   if(nIDEvent == m_nPauseButtonTimer)
   {
      m_nFlashPause = 1 - m_nFlashPause;
      if(m_pPresentationBar)
         m_pPresentationBar->SetPauseButtonFlash(m_nFlashPause);
   }
   
#ifdef _STUDIO
   CMDIChildWnd::OnTimer(nIDEvent);
#else
   CFrameWnd::OnTimer(nIDEvent);
#endif
}

void CMainFrameA::OnUseSmartboard() 
{
   if (m_bSMARTBoardUsable)
   {
      if (m_bSMARTBoardAttached)
         DetachSmartBoard();
      else
         AttachSmartBoard();
   }
   
}

void CMainFrameA::OnUpdateUseSmartboard(CCmdUI* pCmdUI) 
{
   if (m_bSMARTBoardUsable)
   {
      if (m_bSMARTBoardAttached)
         pCmdUI->SetCheck(true);
      else
         pCmdUI->SetCheck(false);
      pCmdUI->Enable(true);
   }
   else
   {
      pCmdUI->Enable(false);
   }
   
}
void CMainFrameA::MinimizeAssistant()
{
   m_bMinimized = true;
#ifndef _STUDIO
   ShowWindow(SW_MINIMIZE);
#else
   CWnd *pMainFrame = AfxGetMainWnd();
   if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
       ((CMainFrame *)pMainFrame)->Minimize();
#endif
}

void CMainFrameA::RestoreAssistant()
{
    if (m_bMinimized) {
#ifndef _STUDIO
        if (IsIconic()) {
            ShowWindow(SW_RESTORE);
        }
#else
        CWnd *pMainFrame = AfxGetMainWnd();
        if (pMainFrame && (pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame)))
            ((CMainFrame *)pMainFrame)->Restore();
#endif
        m_bMinimized = false;

    }
}

#ifndef _STUDIO
void CMainFrameA::SetToTop()
{
   m_bAlwaysOnTop = true;
   SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CMainFrameA::ResetToTop()
{
   m_bAlwaysOnTop = false;
   SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}
#endif

void CMainFrameA::GoToNextPage()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments())
   {
      UINT nActivePageId = pDoc->GoToNextPage();

      if (m_pDocumentStructureView != NULL)
         m_pDocumentStructureView->SetSelected(nActivePageId);

      if (m_pWhiteboardView)
         m_pWhiteboardView->RedrawWindow();

      if(m_pButtonStandardLayout)
      {
         m_pButtonStandardLayout->RefreshBkgrnd();
      }
   }
}

void CMainFrameA::GoToPreviousPage()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

   if (pDoc != NULL && pDoc->HasLoadedDocuments())
   {
      UINT nActivePageId = pDoc->GoToPreviousPage();  

      if (m_pDocumentStructureView != NULL)
         m_pDocumentStructureView->SetSelected(nActivePageId);

      if (m_pWhiteboardView)
         m_pWhiteboardView->RedrawWindow();

      if(m_pButtonStandardLayout)
      {
         m_pButtonStandardLayout->RefreshBkgrnd();
      }
   }
}

#ifndef _STUDIO
void CMainFrameA::ReduceAssistant()
{
   if (m_nCurrentLayout != AssistantLayouts(ReducedLayout))
   {
	  
      // store which windows are visible
      StoreWindowsState(ReducedLayout);

      if (m_nCurrentLayout == FullscreenLayout || 
          m_nCurrentLayout == FullscreenPagefocusLayout)
         FullScreenModeOff();

      m_lWindowStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
      m_bStatusBarVisibleReduced = m_wndStatusBar.IsWindowVisible() == 0 ? false : true;
      m_wndStatusBar.ShowWindow(SW_HIDE);
     
      CXTPCommandBars* pCommandBars = GetCommandBars();
      if (pCommandBars != NULL)
      {           
         CXTPCommandBar* pCommandBar = (CXTPCommandBar*) pCommandBars->GetMenuBar();
         if (pCommandBar != NULL)
         {
            m_bMenuVisibleReduced = pCommandBar->IsWindowVisible() == 0 ? false : true;;
            pCommandBar->SetVisible(false); 
         }     
      }   
      
      HidePanes();
      
      m_nPreviousLayout = m_nCurrentLayout;
      m_nCurrentLayout = ReducedLayout;
      
      ReducedModeOn();      
      
      HideTransparentButton();  
   }
   
}

void CMainFrameA::UndoReduceAssistant()
{
   if (m_nCurrentLayout == AssistantLayouts(ReducedLayout))
   {      
      
      ReducedModeOff();
      
      if (m_nPreviousLayout == AssistantLayouts(FullscreenLayout) ||
          m_nPreviousLayout == AssistantLayouts(FullscreenPagefocusLayout) ||
          m_nPreviousLayout == AssistantLayouts(StandardLayout) ||
          m_nPreviousLayout == AssistantLayouts(StandardPagefocusLayout))
      {
         
         if (m_nPreviousLayout == AssistantLayouts(FullscreenLayout) ||
             m_nPreviousLayout == AssistantLayouts(FullscreenPagefocusLayout))
            FullScreenModeOn();      
         else
            FullScreenModeOff();
         
         if (m_bStatusBarVisibleReduced)
            m_wndStatusBar.ShowWindow(SW_SHOW);
         else
            m_wndStatusBar.ShowWindow(SW_HIDE);
         
         CXTPCommandBars* pCommandBars = GetCommandBars();
         if (pCommandBars != NULL)
         {
            
            CXTPCommandBar* pComandBar = (CXTPCommandBar*) pCommandBars->GetMenuBar();
            if(pComandBar != NULL && m_bMenuVisibleReduced)
               pComandBar->SetVisible(true);             
         }   
         
         if (m_bDocumentStructureVisibleReduced)
            m_paneManager.ShowPane(IDR_PANE_DOCSTRUCT);
         else
            m_paneManager.ClosePane(IDR_PANE_DOCSTRUCT);
         
         if (m_bRecordingStructureVisibleReduced)
            m_paneManager.ShowPane(IDR_PANE_RECORDINGS);
         else
            m_paneManager.ClosePane(IDR_PANE_RECORDINGS);
         
         m_nCurrentLayout = m_nPreviousLayout; 
         
         if (m_nCurrentLayout == AssistantLayouts(FullscreenPagefocusLayout))   
         {
            HideTransparentButton(false);
         }
         else if (m_nCurrentLayout == AssistantLayouts(FullscreenLayout))
         {
            HideTransparentButton();
         }
         else if (m_nCurrentLayout == AssistantLayouts(StandardPagefocusLayout))
         {
            HideTransparentButton(false);
         }
         else if (m_nCurrentLayout == AssistantLayouts(StandardLayout))
         {
            HideTransparentButton();
         }
      }
   }   
}

#endif

void CMainFrameA::RefreshTransparentButton()
{
   if(m_pButtonStandardLayout->IsWindowVisible())
   {
      CRect rcWhiteboard;
      m_pWhiteboardView->GetClientRect(&rcWhiteboard);
      MoveTransparentButton(&rcWhiteboard);
      m_pButtonStandardLayout->RefreshBkgrnd(); 
     // m_pButtonStandardLayout->ShowWindow(SW_HIDE);
     // m_pButtonStandardLayout->ShowWindow(SW_SHOW);
   }

}

void CMainFrameA::HideTransparentButton(BOOL bHide)
{
   if (bHide)
   {
      m_pButtonStandardLayout->ShowWindow(SW_HIDE);
   }
   else
   {
      if ((m_nCurrentLayout == StandardPagefocusLayout ||
         m_nCurrentLayout == FullscreenPagefocusLayout) && !m_pPresentationBar->IsWindowVisible())
      {
         m_pButtonStandardLayout->ShowWindow(SW_SHOW);
         if (m_pWhiteboardView)
            m_pWhiteboardView->RedrawWindow();
      }
   }
}

void CMainFrameA::MoveTransparentButton(LPRECT lpViewRect)
{
   if (m_pButtonStandardLayout->m_hWnd == NULL || 
       !m_pButtonStandardLayout->IsWindowVisible())
      return;

   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   if (pDoc->HasActivePage() && ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
      if (pPage != NULL && m_pWhiteboardView != NULL)
      {
         CRect rcWhiteboard;
         m_pWhiteboardView->GetClientRect(&rcWhiteboard);
         float fZoomFactor = pPage->CalculateZoomFactor(rcWhiteboard);
         UINT nXOffset, nYOffset;
         pPage->CalculateOffset(rcWhiteboard, fZoomFactor, nXOffset, nYOffset);
         int x = nXOffset + 5;
         int y = rcWhiteboard.bottom - nYOffset - 5 - 33;
         if (y < 0) y = 0;
         m_pButtonStandardLayout->MoveWindow(x, y, 65, 33, TRUE); 
      }
   }
   else
   {
      int x = 10;
      int y = lpViewRect->bottom-lpViewRect->top - 58;
      m_pButtonStandardLayout->MoveWindow(x, y, 65, 33,TRUE); 
   }
}


void CMainFrameA::OnShowWindow(BOOL bShow, UINT nStatus) 
{
   UpdateStatusDiskSpace();
   UpdateStatusDuration();
#ifdef _STUDIO
   CWnd *pMainFrame = AfxGetMainWnd();
   if (pMainFrame != NULL
       && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame)) {
       ShowControlBar(((CMainFrame*)pMainFrame)->GetStatusBar(), GetShowStatusBar() ? TRUE : FALSE, FALSE);
   }

   CMDIChildWnd::OnShowWindow(bShow, nStatus);
#else
   CFrameWnd::OnShowWindow(bShow, nStatus);
#endif
   
}

#ifdef _DVS2005
LRESULT CMainFrameA::OnChangeAudio(WPARAM wParam, LPARAM lParam)
#else
void CMainFrameA::OnChangeAudio()
#endif
{
   m_nAVSettingsTab = 0;
   OnChangeAudiovideo();
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CMainFrameA::OnChangeVideo(WPARAM wParam, LPARAM lParam)
#else
void CMainFrameA::OnChangeVideo()
#endif
{
   m_nAVSettingsTab = 1;
   OnChangeAudiovideo();
#ifdef _DVS2005
   return 1;
#endif
}


void CMainFrameA::SetShowImportWarning(bool bShowWarning)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetShowImportWarning(bShowWarning);
}

void CMainFrameA::SetShowMissingFontsWarning(bool bShowWarning)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetShowMissingFontsWarning(bShowWarning);
}

void CMainFrameA::SetShowAudioWarning(bool bShowWarning)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetShowAudioWarning(bShowWarning);
}

void CMainFrameA::SetShowUseSmartBoard(bool bShowWarning)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetShowUseSmartBoard(bShowWarning);
}

void CMainFrameA::SetShowMouseCursor(bool bShowMouseCursor)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetShowMouseCursor(bShowMouseCursor);
}
		 
void CMainFrameA::SetShowStatusBar(bool bShowStatusBar)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetShowStatusBar(bShowStatusBar);
}

void CMainFrameA::SetShowStartupPage(bool bShowStartupPage)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetShowStartupPage(bShowStartupPage);
}

void CMainFrameA::SetShowAvMonitor(bool bShowAvMonitor)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetShowAVMonitor(bShowAvMonitor);
}

void CMainFrameA::SetShowAudioMonitor(bool bShowAudioMonitor)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetShowAudioMonitor(bShowAudioMonitor);
}

void CMainFrameA::SetFreezeImages(bool bFreezeImages)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetFreezeImages(bFreezeImages);
}

void CMainFrameA::SetFreezeMaster(bool bFreezeMaster)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetFreezeMaster(bFreezeMaster);
}

void CMainFrameA::SetFreezeDocument(bool bFreezeDocument)
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        pAssistantDoc->SetFreezeDocument(bFreezeDocument);
}

bool CMainFrameA::GetShowImportWarning()
{
    bool bShowImportWarning = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bShowImportWarning = pAssistantDoc->GetShowImportWarning();

    return bShowImportWarning;
}

bool CMainFrameA::GetShowMissingFontsWarning()
{
    bool bShowMissingFontsWarning = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bShowMissingFontsWarning = pAssistantDoc->GetShowMissingFontsWarning();

    return bShowMissingFontsWarning;
}

bool CMainFrameA::GetShowAudioWarning()
{
    bool bShowAudioWarning = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bShowAudioWarning = pAssistantDoc->GetShowAudioWarning();

    return bShowAudioWarning;
}

bool CMainFrameA::GetShowUseSmartBoard()
{
    bool m_bShowUseSmartBoard = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        m_bShowUseSmartBoard = pAssistantDoc->GetShowUseSmartBoard();

    return m_bShowUseSmartBoard;
}

bool CMainFrameA::GetFreezeImages()
{
    bool bFreezeImages = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bFreezeImages = pAssistantDoc->GetFreezeImages();

    return bFreezeImages;
}

bool CMainFrameA::GetFreezeMaster()
{
    bool bFreezeMaster = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bFreezeMaster = pAssistantDoc->GetFreezeMaster();

    return bFreezeMaster;
}

bool CMainFrameA::GetFreezeDocument()
{
    bool bFreezeDocument = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bFreezeDocument = pAssistantDoc->GetFreezeDocument();

    return bFreezeDocument;
}

bool CMainFrameA::GetShowMouseCursor()
{
    bool bShowMouseCursor = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bShowMouseCursor = pAssistantDoc->GetShowMouseCursor();

    return bShowMouseCursor;
}

bool CMainFrameA::GetShowStatusBar()
{
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        return pAssistantDoc->GetShowStatusBar();
    return false;
}

bool CMainFrameA::GetShowStartupPage() 
{
    bool bShowStartupPage = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bShowStartupPage = pAssistantDoc->GetShowStartupPage();

    return bShowStartupPage;
}

bool CMainFrameA::GetSwitchAutomaticallyToEdit() 
{
    bool bSwitchAutomatically = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bSwitchAutomatically = pAssistantDoc->GetSwitchAutomaticallyToEdit();

    return bSwitchAutomatically;
}

bool CMainFrameA::GetShowAVMonitor()
{
    bool bShowAVMonitor = false;
    CAssistantDoc *pAssistantDoc = GetAssistantDocument();
    if (pAssistantDoc)
        bShowAVMonitor = pAssistantDoc->GetShowAVMonitor();

    return bShowAVMonitor;
}

CXTPCommandBars *CMainFrameA::GetCommandBars()
{
#ifndef _STUDIO
    CMainFrameA *pMainFrame = this;
    return CXTPFrameWnd::GetCommandBars();
#else
    CMainFrame *pMainFrame = NULL;
    CWnd *pMainWnd = AfxGetMainWnd();
    if (pMainWnd->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
        pMainFrame = (CMainFrame *)pMainWnd;

    if (pMainFrame == NULL)
        return NULL;

    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();

    return pCommandBars;
#endif
}

CXTPRibbonBar* CMainFrameA::GetRibbonBar()
{
#ifndef _STUDIO
    CMainFrameA *pMainFrame = this;
#else
    CMainFrame *pMainFrame = NULL;
    CWnd *pMainWnd = AfxGetMainWnd();
    if (pMainWnd->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
        pMainFrame = (CMainFrame *)pMainWnd;
#endif

    if (pMainFrame == NULL)
        return NULL;

    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
    if (pCommandBars != NULL) {
        CXTPRibbonBar* pRibbonBar = DYNAMIC_DOWNCAST(CXTPRibbonBar, pCommandBars->GetMenuBar());
        ASSERT(pRibbonBar);
        return pRibbonBar;
    }

    return NULL;
}

CString CMainFrameA::GetAudioTooltip(CAssistantDoc *pAssistantDoc)
{
   CString csToolTip = _T("");
   
   CStringArray arDevices;
   UINT nDevice;
   CUIntArray arSampleWidths;
   UINT nSampleWidth;
   CUIntArray arSampleRates;
   UINT nSampleRate;
   CStringArray arSources;
   UINT nSource;
   UINT nVolume;
   
   arDevices.RemoveAll();
   nDevice = 0;
   
   arSampleWidths.RemoveAll();
   nSampleWidth = 0;
   
   arSampleRates.RemoveAll();
   nSampleRate = 0;
   
   arSources.RemoveAll();
   nSource = 0;
   
   nVolume = 0;
   
   if (pAssistantDoc)
   {
      pAssistantDoc->GetAudioDevices(arDevices, nDevice);
      if(arDevices.GetSize() > 0 && nDevice < arDevices.GetSize())
      {
         CString csDevice;
         csDevice.Format(IDS_DEVICE, arDevices[nDevice]);
         csToolTip += csDevice;
      }
      
      pAssistantDoc->GetSampleWidths(nDevice, arSampleWidths, nSampleWidth);
      if(arSampleRates.GetSize() > 0 && nSampleRate < arSampleRates.GetSize())
      {
         CString csSampleRate;
         csSampleRate.Format(IDS_SAMPLE_RATE, arSampleRates[nSampleRate]);
         csToolTip += csSampleRate;
      }
      
      pAssistantDoc->GetSampleRates(nDevice, arSampleRates, nSampleRate);
      if(arSampleWidths.GetSize() > 0 && nSampleWidth < arSampleWidths[nSampleWidth])
      {
         CString csSampleWidth;
         csSampleWidth.Format(IDS_SAMPLE_WIDTH, arSampleWidths[nSampleWidth]);
         csToolTip += csSampleWidth;
      }
      
      pAssistantDoc->GetAudioSources(arSources, nSource);
      if(arSources.GetSize() > 0 && nSource < arSources.GetSize())
      {
         CString csAudioSource;
         csAudioSource.Format(IDS_AUDIO_SOURCE, arSources[nSource]);
         csToolTip += csAudioSource;
      }
      
      CString csVolume;
      nVolume = pAssistantDoc->GetMixerVolume();
      csVolume.Format(IDS_VOLUME, nVolume);
      csToolTip += csVolume;
      
      CString csDblClick;
      csDblClick.LoadString(IDS_DBLCLICK_AVSETTINGS);
      csToolTip += csDblClick;
      
   }
   else
   {
      csToolTip.LoadString(ID_VIEW_AUDIO);
   }
   arDevices.RemoveAll();
   arSampleWidths.RemoveAll();
   arSampleRates.RemoveAll();
   arSources.RemoveAll();
   
   return csToolTip;
}

CString CMainFrameA::GetVideoTooltip(CAssistantDoc *pAssistantDoc)
{  
   CString csToolTip = _T("");
   
   CString csDevice;
   CString csRecordVideo;
   CString csFramerate;
   CString csResolution;
   CString csCodec;
   CString csDblClick;
   CString csYes;
   CString csNo;

   
   csDevice.LoadString(IDS_DEVICE);
   if(csDevice.IsEmpty())
      csDevice = _T("Device: %s\n");
   
   csRecordVideo.LoadString(IDS_RECORD_VIDEO);
   if(csRecordVideo.IsEmpty())
      csRecordVideo = _T("Record video: %s\n");
   
   csFramerate.LoadString(IDS_FRAMERATE);
   if(csFramerate.IsEmpty())
      csFramerate = _T("Framerate: %d\n");
   
   csResolution.LoadString(IDS_RESOLUTION);
   if(csResolution.IsEmpty())
      csResolution = _T("Resolution: %dx%d\n");
   
   csCodec.LoadString(IDS_CODEC);
   if(csCodec.IsEmpty())
      csCodec = _T("Codec: %s\n");
   
   csDblClick.LoadString(IDS_DBLCLICK_AVSETTINGS);
   if(csDblClick.IsEmpty())
      csDblClick = _T("(Double click for settings)");
   
   csYes.LoadString(IDYES);
   if(csYes.IsEmpty())
      csYes = _T("Yes");

   csNo.LoadString(IDNO);
   if(csNo.IsEmpty())
      csNo = _T("No");
   
   bool bRecordVideo;
   bool bShowMonitor;
   bool bUseAlternativeSync;
   bool bSetAvOffset;
   int iAvOffset;
   UINT nXResolution;
   UINT nYResolution;
   UINT nBitPerColor;
   CArray<float, float> arFrameRates;
   UINT nFrameRateIndex;
   bool bUseVFW;
   CStringArray arVFWSources;
   UINT nVFWIndex;
   CStringArray arWDMSources;
   UINT nWDMIndex;
   CStringArray arCodecs;
   UINT nCodecIndex;
   UINT nQuality;
   bool bUseKeyframes;
   UINT nKeyframeDistance;
   bool bUseDatarateAdaption;
   UINT nDatarate;
   bool bHasAbout;
   bool bHasConfigure;
   
   bRecordVideo = false;
   bShowMonitor = false;
   bUseAlternativeSync = false;
   bSetAvOffset = false;
   iAvOffset = 0;
   nXResolution = 0;
   nYResolution = 0;
   nBitPerColor = 0;
   arFrameRates.RemoveAll();
   nFrameRateIndex = 0;
   bUseVFW = true;
   arVFWSources.RemoveAll();
   nVFWIndex = 0;
   arWDMSources.RemoveAll();
   nWDMIndex = 0;
   nCodecIndex = 0;
   arCodecs.RemoveAll();
   nQuality = 75;
   bUseKeyframes = true;
   nKeyframeDistance = 15;
   bUseDatarateAdaption = false;
   nDatarate = 500;
   
   if (pAssistantDoc)
   {
      
      pAssistantDoc->GetVideoSettings(bRecordVideo, bShowMonitor, bUseAlternativeSync,
         bSetAvOffset, iAvOffset);
      
      pAssistantDoc->GetVideoParameter(bUseVFW, arVFWSources, nVFWIndex,
         arWDMSources, nWDMIndex,
         nXResolution, nYResolution, nBitPerColor, 
         arFrameRates, nFrameRateIndex);
      
      pAssistantDoc->GetCodecSettings(arCodecs, nCodecIndex, nQuality, 
         bUseKeyframes, nKeyframeDistance,
         bUseDatarateAdaption, nDatarate,
         bHasAbout, bHasConfigure);
      
      if(bRecordVideo)
         csToolTip.Format(csRecordVideo + csDevice + csResolution + csFramerate + csCodec+csDblClick, 
         csYes,
         (bUseVFW) ? (arVFWSources[nVFWIndex]) : (arWDMSources[nWDMIndex]),
         nXResolution,
         nYResolution,
         (int)arFrameRates[nFrameRateIndex],
         arCodecs[nCodecIndex]
         );
      else
         csToolTip.Format(csRecordVideo + csDblClick, 
         csNo
         );
      
   }
   else
   {
      csToolTip.LoadString(ID_VIEW_VIDEO);
      if(csToolTip.IsEmpty())
         csToolTip = _T("Video");
   }
   arFrameRates.RemoveAll();
   arVFWSources.RemoveAll();
   arWDMSources.RemoveAll();
   arCodecs.RemoveAll();
   
   return csToolTip;
}

void CMainFrameA::SetAudioVideoTooltips()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   if (pDoc != NULL)
   { 
      if(m_pMonitorWidget != NULL)
         m_pMonitorWidget->SetAudioVideoTooltips(GetAudioTooltip(pDoc), GetVideoTooltip(pDoc));
      if ( m_pAudioWidget != NULL )
         m_pAudioWidget->SetAudioVideoTooltips(GetAudioTooltip(pDoc), GetVideoTooltip(pDoc));
   }
   
}

#ifndef _STUDIO
void CMainFrameA::ReducedModeOn()
{
   GetWindowRect(&m_mainRect);
   CRect rectToolbarPres;
   rectToolbarPres.SetRectEmpty();
   
   int nTitleH = GetSystemMetrics(SM_CYCAPTION);
   
   CXTPCommandBars* pCommandBars = GetCommandBars();

   pCommandBars->SetEnableContextMenu(false);
   ModifyStyle(WS_THICKFRAME | WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);

   SetWindowPos(NULL, m_mainRect.left, m_mainRect.bottom - nTitleH - rectToolbarPres.Height() - 7 , m_mainRect.Width() , rectToolbarPres.Height() + nTitleH + 7, SWP_SHOWWINDOW | SWP_DRAWFRAME | SWP_FRAMECHANGED);
}
#endif

#ifndef _STUDIO
void CMainFrameA::ReducedModeOff()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();

   pCommandBars->SetEnableContextMenu();
   ModifyStyle(0, WS_THICKFRAME | WS_MAXIMIZEBOX, SWP_FRAMECHANGED);

   SetWindowPos(NULL, m_mainRect.left, m_mainRect.top, m_mainRect.Width(), m_mainRect.Height(), SWP_SHOWWINDOW | SWP_DRAWFRAME);
}
#endif

void CMainFrameA::StartRecordingTimer()
{
   if (m_nCheckDiskSpace == 0)
   {  
      ASSISTANT::Project::active->CheckDiskSpace(true);
      m_nCheckDiskSpace = SetTimer(DISCSPACE_TIMER, 1000, 0);
   }

   if (m_nUpdateDiscSpace == 0)
      m_nUpdateDiscSpace = SetTimer(DISCSTATE_TIMER, 1000, 0);

   if (m_nUpdateRecordTime == 0)
      m_nUpdateRecordTime = SetTimer(RECORDTIME_TIMER, 1000, 0);

   if (m_nCheckAudioLevel == 0 && GetShowAudioWarning())
   {  
      ASSISTANT::Project::active->CheckAudioLevel(true);
      m_nCheckAudioLevel = SetTimer(AUDIOLEVEL_TIMER, 20000, 0);
   }
}

void CMainFrameA::StopRecordingTimer()
{
   if (m_nCheckDiskSpace > 0)  
   {
      KillTimer(m_nCheckDiskSpace); 
      ASSISTANT::Project::active->CheckDiskSpace(true);
   }
   m_nCheckDiskSpace = 0;

   if (m_nUpdateDiscSpace > 0)  
      KillTimer(m_nUpdateDiscSpace); 
   m_nUpdateDiscSpace = 0;

   if (m_nUpdateRecordTime > 0)  
      KillTimer(m_nUpdateRecordTime); 
   m_nUpdateRecordTime = 0;

   if (m_nCheckAudioLevel > 0)  
   {
      KillTimer(m_nCheckAudioLevel); 
      ASSISTANT::Project::active->CheckAudioLevel(true);
   }
   m_nCheckAudioLevel = 0;
}

void CMainFrameA::UpdateStatusDiskSpace()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   CString csWindowText;
   CString csStatus;
   if (pDoc && ASSISTANT::Project::active != NULL)
   {
      CString csRecordPath = ASSISTANT::Project::active->GetRecordPath();
      int iMbLeft = 0;
      if (!csRecordPath.IsEmpty())
      {
         SystemInfo::CalculateDiskSpace(csRecordPath, iMbLeft);
         
         if (iMbLeft > 1024)
         {
            csStatus.Format(_T("%dGB"), (int)(iMbLeft/1024));
         }
         else 
         {
            csStatus.Format(_T("%dMB"), iMbLeft);
         }
      }
      else
      {
         csStatus = InitDiskSpace();
      }
   }
   else
   {
      csStatus = InitDiskSpace();
   }

#ifndef _STUDIO
   m_wndStatusBar.SetPaneText(0, csStatus);//, TRUE);
#else
   CWnd *pMainFrame = AfxGetMainWnd();
   if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame)) 
   {
       // bugfix 4842: set more meaningful status bar texts
       CString csStatusText;
       csStatusText.Format(ID_STATUS_DISKSPACE, csStatus);
       ((CMainFrame *)pMainFrame)->SetStatusBarText(0, csStatusText);
   }
#endif
   if (m_pMonitorWidget != NULL)
       m_pMonitorWidget->UpdateDiskSpace(csStatus);
   if ( m_pAudioWidget != NULL )
      m_pAudioWidget->UpdateDiskSpace(csStatus);
}

void CMainFrameA::UpdateStatusDuration()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
   
   CString csWindowText;
   CString csStatus = _T("0:00:00");
   if (pDoc && pDoc->IsRecording() && ASSISTANT::Project::active != NULL)
   {
      int iRecordingTime = ASSISTANT::Project::active->GetRecordingTime();
      
      int hour = (float)iRecordingTime / 3600000;
      int msecLeft = iRecordingTime - (hour*3600000);
      
      int min = (float)msecLeft / 60000;
      msecLeft = msecLeft - (min*60000);
      
      int sec = (float)msecLeft / 1000;
      
      csStatus.Format(_T("%d:%02d:%02d"),hour, min, sec);
   }
   
#ifndef _STUDIO
   m_wndStatusBar.SetPaneText(1, csStatus);//, TRUE);
#else
   CWnd *pMainFrame = AfxGetMainWnd();
   if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
   {
       // bugfix 4842: set more meaningful status bar texts
       CString csStatusText;
       csStatusText.Format(ID_STATUS_DURATION, csStatus);
       ((CMainFrame *)pMainFrame)->SetStatusBarText(1, csStatusText);
   }
#endif

   if (m_pMonitorWidget != NULL)
       m_pMonitorWidget->UpdateDuration(csStatus);
   if ( m_pAudioWidget != NULL )
      m_pAudioWidget->UpdateDuration(csStatus);
}

void CMainFrameA::UpdateStatusPages()
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetActiveDocument();
   
   CString csWindowText;
   int nPageNr = 0;
   int nTotalPages = 0;
   if (pDoc && ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();    
      bool bFound; 
      if(pPage)
         nPageNr = ASSISTANT::Project::active->GetPageNumber(pPage->GetID(), bFound);
      nTotalPages = ASSISTANT::Project::active->GetNbrOfPages();   
   }
   if (m_pMonitorWidget != NULL)
      m_pMonitorWidget->UpdatePages(nPageNr, nTotalPages);
}

void CMainFrameA::StoreWindowsState(AssistantLayouts nCurrentLayout)
{
   if (nCurrentLayout == StandardLayout)
   {
      CXTPCommandBars* pCommandBars = GetCommandBars();
      if (pCommandBars != NULL)
      {           
         // main menu
         m_bMenuVisibleStandard = true;
         CXTPCommandBar* pCommandBar = (CXTPCommandBar*)pCommandBars->GetMenuBar();
         if(pCommandBar != NULL)
            m_bMenuVisibleStandard = pCommandBar->IsVisible() == 0 ? false : true;
         
         // panes
         m_bDocumentStructureVisibleStandard = true;
         if (m_paneManager.IsPaneClosed(IDR_PANE_DOCSTRUCT))
            m_bDocumentStructureVisibleStandard = false;
         
         m_bRecordingStructureVisibleStandard = true;
         if (m_paneManager.IsPaneClosed(IDR_PANE_RECORDINGS))
            m_bRecordingStructureVisibleStandard = false;
         
         // statuc bar
#ifndef _STUDIO
         m_bStatusBarVisibleStandard = m_wndStatusBar.IsWindowVisible() == 0 ? false : true;
#endif
      }
   }
   else if (nCurrentLayout == ReducedLayout)
   {
      CXTPCommandBars* pCommandBars = GetCommandBars();
      if (pCommandBars != NULL)
      {           
         // main menu
         m_bMenuVisibleReduced = true;
         CXTPCommandBar* pCommandBar = (CXTPCommandBar*)pCommandBars->GetMenuBar();
         if(pCommandBar != NULL)
            m_bMenuVisibleReduced = pCommandBar->IsVisible() == 0 ? false : true;
         
         // panes
         m_bDocumentStructureVisibleReduced = true;
         if (m_paneManager.IsPaneClosed(IDR_PANE_DOCSTRUCT))
            m_bDocumentStructureVisibleReduced = false;
         
         m_bRecordingStructureVisibleReduced = true;
         if (m_paneManager.IsPaneClosed(IDR_PANE_RECORDINGS))
            m_bRecordingStructureVisibleReduced = false;
         
#ifndef _STUDIO
         // statuc bar
         m_bStatusBarVisibleReduced = m_wndStatusBar.IsWindowVisible() == 0 ? false : true;
#endif
      }
   }
}

void CMainFrameA::ChangeWindowsState(AssistantLayouts nCurrentLayout)
{
   if (nCurrentLayout == ReducedLayout)
      return;

   if (nCurrentLayout == StandardLayout)
   {
      StoreWindowsState(StandardLayout);
      return;
   }
}

#ifndef _STUDIO
void CMainFrameA::HideMenu()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   if (pCommandBars != NULL)
   {         
      CXTPCommandBar* pComandBar = (CXTPCommandBar*)pCommandBars->GetMenuBar();
      if (pComandBar != NULL)
         pComandBar->SetVisible(false);  
   }
}

void CMainFrameA::ShowMenu()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   if (pCommandBars != NULL)
   {         
      CXTPCommandBar* pComandBar = (CXTPCommandBar*)pCommandBars->GetMenuBar();
      if (pComandBar != NULL)
         pComandBar->SetVisible(true);  
   }
}

void CMainFrameA::HideStatusBar()
{
#ifndef _STUDIO
   m_wndStatusBar.ShowWindow(SW_HIDE);
#endif
}

void CMainFrameA::ShowStatusBar()
{
#ifndef _STUDIO
   m_wndStatusBar.ShowWindow(SW_SHOW);
#endif
}
#endif

void CMainFrameA::HidePanes()
{
   m_paneManager.ClosePane(IDR_PANE_DOCSTRUCT);
   m_paneManager.ClosePane(IDR_PANE_RECORDINGS);
}

void CMainFrameA::ShowPanes()
{
   if (m_bDocumentStructureVisibleStandard)
      m_paneManager.ShowPane(IDR_PANE_DOCSTRUCT);
   
   if (m_bRecordingStructureVisibleStandard)
      m_paneManager.ShowPane(IDR_PANE_RECORDINGS);
}
										

void CMainFrameA::WriteHWNDToRegistry()
{
   HKEY 
      hKeyUser; 
   LONG		
      regErr;
   DWORD
      dwDummy;

   CString csSubKey = __T("Software\\imc AG\\LECTURNITY\\Assistant\\");
   csSubKey += _T("\\PPT2LEC");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_ALL_ACCESS, &hKeyUser);

   if ( regErr != ERROR_SUCCESS )
   {
      regErr = RegCreateKeyEx(HKEY_CURRENT_USER, csSubKey,
         NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyUser, &dwDummy); 
   }

   if(regErr == ERROR_SUCCESS)
   {
      TCHAR value[512];
      _stprintf(value, _T("%d"), int(m_hWnd));
      regErr = RegSetValueEx(hKeyUser, _T("HANDLE"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   }

   RegCloseKey(hKeyUser);
}

void CMainFrameA::DeleteHWNDFromRegistry()
{ 
   HKEY 
      hKeyUser; 
   LONG		
      regErr;
   DWORD
      dwDummy;

   CString csSubKey = __T("Software\\imc AG\\LECTURNITY\\Assistant\\");
   csSubKey += _T("\\PPT2LEC");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_ALL_ACCESS, &hKeyUser);

   if ( regErr != ERROR_SUCCESS )
   {
      regErr = RegCreateKeyEx(HKEY_CURRENT_USER, csSubKey,
         NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyUser, &dwDummy); 
   }

   if(regErr == ERROR_SUCCESS)
   {
      TCHAR value[512];
      _stprintf(value, _T("%d"), int(NULL));
      regErr = RegSetValueEx(hKeyUser, _T("HANDLE"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   }

   RegCloseKey(hKeyUser);

}

void CMainFrameA::SetRefreshTransparentBtn(bool bRefresh)
{
   m_bRefreshTransparentBtn = bRefresh;
}

BOOL CMainFrameA::GetRefreshTransparentBtn()
{
   return m_bRefreshTransparentBtn;
}


// SBSDK

void CMainFrameA::GetSBSDKColorAndLineWidth(int iPointerID,
                                           COLORREF &clrLine, COLORREF &clrFill,
                                           float &fTransperency, float &fFillTransperency,
                                           int &iLineWidth)
{
   if (m_pSBSDK == NULL)
      return;

   LONG lLineWidth;
   m_pSBSDK->SBSDKGetToolWidth(iPointerID, &lLineWidth);
   iLineWidth = lLineWidth;

   m_pSBSDK->SBSDKGetToolOpacity(iPointerID, &fTransperency);

   m_pSBSDK->SBSDKGetToolFillOpacity(iPointerID, &fFillTransperency);

   LONG lRed, lGreen, lBlue;
   m_pSBSDK->SBSDKGetToolColor(iPointerID, &lRed, &lGreen, &lBlue);
   clrLine = RGB(lRed, lGreen, lBlue);

   m_pSBSDK->SBSDKGetToolFillColor(iPointerID, &lRed, &lGreen, &lBlue);;
   clrFill  = RGB(lRed, lGreen, lBlue);
}

void CMainFrameA::OnPen(LONG iPointerID)
{
   if (!m_pSBSDK || !m_pWhiteboardView)
      return;

   int iLineWidth;
   float fTransperency;
   float fFillTransperency;
   COLORREF clrLine;
   COLORREF clrFill;
   GetSBSDKColorAndLineWidth(iPointerID, clrLine, clrFill, 
                             fTransperency, fFillTransperency, iLineWidth);

   ((CAssistantView *)m_pWhiteboardView)->SBSDKOnPen(iLineWidth, 
      fTransperency, fFillTransperency, 
      clrLine, clrFill);
}

void CMainFrameA::OnEraser(LONG iPointerID)
{
   if (!m_pSBSDK || !m_pWhiteboardView)
      return;

   ((CAssistantView *)m_pWhiteboardView)->SBSDKOnNoTool();
}

void CMainFrameA::OnRectangle(LONG iPointerID)
{
   if (!m_pSBSDK || !m_pWhiteboardView)
      return;

   int iLineWidth;
   float fTransperency;
   float fFillTransperency;
   COLORREF clrLine;
   COLORREF clrFill;
   GetSBSDKColorAndLineWidth(iPointerID, clrLine, clrFill, 
                             fTransperency, fFillTransperency, iLineWidth);

   ((CAssistantView *)m_pWhiteboardView)->SBSDKOnRectangle(iLineWidth, 
                    fTransperency, fFillTransperency, 
                    clrLine, clrFill);
}

void CMainFrameA::OnLine(LONG iPointerID)
{
   if (!m_pSBSDK || !m_pWhiteboardView)
      return;

   int iLineWidth;
   float fTransperency;
   float fFillTransperency;
   COLORREF clrLine;
   COLORREF clrFill;
   GetSBSDKColorAndLineWidth(iPointerID, clrLine, clrFill, 
                             fTransperency, fFillTransperency, iLineWidth);

   ((CAssistantView *)m_pWhiteboardView)->SBSDKOnLine(iLineWidth, 
               fTransperency, fFillTransperency, 
               clrLine, clrFill);
}

void CMainFrameA::OnCircle(LONG iPointerID)
{
   if (!m_pSBSDK || !m_pWhiteboardView)
      return;

   int iLineWidth;
   float fTransperency;
   float fFillTransperency;
   COLORREF clrLine;
   COLORREF clrFill;
   GetSBSDKColorAndLineWidth(iPointerID, clrLine, clrFill, 
                             fTransperency, fFillTransperency, iLineWidth);

   ((CAssistantView *)m_pWhiteboardView)->SBSDKOnCircle(iLineWidth, 
                 fTransperency, fFillTransperency, 
                 clrLine, clrFill);
}

void CMainFrameA::OnNoTool(LONG iPointerID)
{
   if (!m_pSBSDK || !m_pWhiteboardView)
      return;

   ((CAssistantView *)m_pWhiteboardView)->SBSDKOnNoTool();
}

void CMainFrameA::AttachSmartBoard()
{
   m_bSMARTBoardUsable = false;
   m_bSMARTBoardAttached = false;

   if (m_pSBSDK == NULL)
      return;

   m_pSBSDK->SBSDKAttach((LONG)GetSafeHwnd(), FALSE);
   VARIANT_BOOL bIsConnected;
   m_pSBSDK->SBSDKIsABoardConnected(&bIsConnected);
   if (bIsConnected != 0)
   {
      m_bSMARTBoardUsable = true;
      m_bSMARTBoardAttached = true;
   }

   return;
}

void CMainFrameA::DetachSmartBoard()
{
   if (m_pSBSDK == NULL)
      return;

   if (m_bSMARTBoardAttached)
      m_pSBSDK->SBSDKDetach((LONG)GetSafeHwnd());

   m_bSMARTBoardAttached = false;

}

HRESULT CMainFrameA::CheckAssistantMainView()
{
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

   if (m_pCurrentView == NULL)
      return E_FAIL;

   // Change view
   bool bViewChanged = false;
   CView* pViewAdd = NULL;
   CView* pViewRemove = NULL;
   if (pDocument->HasLoadedDocuments() || !m_bShowStartpage)
   {
      if (m_pCurrentView && m_pCurrentView->GetRuntimeClass() != RUNTIME_CLASS(CAssistantView))
      {
         pViewAdd = m_pWhiteboardView;
         pViewRemove = m_pCurrentView;
         bViewChanged = true;
      
      }
      m_bShowStartpage = false;
   }
   else
   {
      if (m_pCurrentView && m_pCurrentView->GetRuntimeClass() != RUNTIME_CLASS(CStartupView))
      {
         if (m_pStartupView == NULL)
         {
            CRect rcDefault;
            m_pCurrentView->GetClientRect(&rcDefault);
            m_pStartupView = new CStartupView;
            m_pStartupView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
               rcDefault, this, AFX_IDW_PANE_FIRST + 1, NULL);
         }
         /*else
         {
            ((CStartupView *)m_pStartupView)->UpdateRecentFileList();
         }*/

         if (m_pStartupView == NULL)
            return E_FAIL;


         pViewAdd = m_pStartupView;
         pViewRemove = m_pCurrentView;

         bViewChanged = true;
      }
   }

    // Show/Hide buttons, toolbars and panes 
#ifndef _STUDIO
   if (pDocument->HasLoadedDocuments()) {
       ShowHidePanesForSgRecording(TRUE);
   }
   else {
       ShowHidePanesForSgRecording(FALSE);
   }
#endif

   if (bViewChanged)
   {
      int nSwitchChildID = pViewAdd->GetDlgCtrlID();
      pViewAdd->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
      pViewRemove->SetDlgCtrlID(nSwitchChildID);

      // Show the newly active view and hide the inactive view.

      pViewAdd->ShowWindow(SW_SHOW);
      pViewRemove->ShowWindow(SW_HIDE);

      // Connect the newly active view to the document, and
      // disconnect the inactive view.
      pDocument->AddView(pViewAdd);
      pDocument->RemoveView(pViewRemove);

      m_pCurrentView = pViewAdd;

      SetActiveView(pViewAdd);
      RecalcLayout();
      
      SetFocus();
   }
   if (m_pCurrentView && !m_bMonitorWidget)
   {
      ShowMonitorWidget();
   }
   return S_OK;
}

HRESULT CMainFrameA::ChangeActiveStructureView(CView *pView)
{
    HRESULT hr = S_OK;

    if (m_pRecordingsView && 
        pView && pView->GetRuntimeClass() == RUNTIME_CLASS(CDocumentStructureView)) {
        m_pRecordingsView->SetAllUnselected();
        //((CDocumentStructureView *)pView)->SetSelected(-1);
    }
    else if (m_pDocumentStructureView && 
             pView && pView->GetRuntimeClass() == RUNTIME_CLASS(CRecordingsView)) {
        if (ASSISTANT::Project::active) {
            ASSISTANT::Project::active->SetNoActiveDocument();
            CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
            if (pDocument)
                pDocument->RebuildStructureTree(m_pDocumentStructureView);
        }
        m_pDocumentStructureView->SetAllUnselected();
        m_pWhiteboardView->RedrawWindow();

    }

    return hr;
}

void CMainFrameA::ShowMonitorWidget()
{
   if (  m_pMonitorWidget == NULL || ASSISTANT::Project::active == NULL )
      return;

   if( !IsInStartupMode() && !ASSISTANT::Project::active->IsPureScreenGrabbing() )
   {
      CRect rcMainWnd, rcMonitorWidget;
      GetWindowRect(rcMainWnd);
      m_pMonitorWidget->GetWindowRect(rcMonitorWidget);
      if ( m_pMonitorWidget->UpdateWindowPlacement() == false )
         m_pMonitorWidget->SetWindowPos(NULL, rcMainWnd.right-rcMonitorWidget.Width()-5, rcMainWnd.top+53,rcMonitorWidget.Width(), rcMonitorWidget.Height(), SWP_NOZORDER);

      if(GetShowAVMonitor() )
         m_pMonitorWidget->ShowWindow(/*SW_SHOW*/SW_SHOWNORMAL);
      else
         m_pMonitorWidget->ShowWindow(/*SW_SHOW*/SW_HIDE);
      m_pMonitorWidget->StartSavePosition();

      m_bMonitorWidget = GetShowAVMonitor()/*true*/;
      if (m_nVideoTimer == 0)
         m_nVideoTimer = SetTimer(VIDEO_TIMER, 500, 0);
      if (m_nAudioTimer == 0)
         m_nAudioTimer = SetTimer(AUDIO_TIMER, 250, 0);  
   } 
   else
      m_pMonitorWidget->ShowWindow(SW_HIDE);
}
#ifndef _STUDIO
void CMainFrameA::ShowHidePanesForSgRecording(BOOL bShow)
{
    if (bShow) {
        if (m_bDocumentStructureVisible)
            m_paneManager.ShowPane(IDR_PANE_DOCSTRUCT);
    }
    else {  
        if (!m_paneManager.IsPaneClosed(IDR_PANE_DOCSTRUCT)) {
            m_paneManager.ClosePane(IDR_PANE_DOCSTRUCT);
            m_bDocumentStructureVisible = TRUE;
        }
        else
            m_bDocumentStructureVisible = FALSE;
    }
}
#endif

void CMainFrameA::CloseStartPage()
{
    m_bShowStartpage = false;
    CheckAssistantMainView();
}

HRESULT CMainFrameA::StartNewSgSelection()
{
    CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
    if (pDoc == NULL)
        return E_FAIL;

    CRect rcDesktop;
    MfcUtils::GetDesktopRectangle(rcDesktop);
    if (!rcDesktop.IsRectEmpty()) {
        CRect rcLastSelection;
        bool bIsFullScreen = false;
        pDoc->GetLastSelectionRectangle(rcLastSelection, bIsFullScreen);
        if(rcLastSelection.IsRectEmpty())
           rcLastSelection.SetRect(0, 0, 640, 480);

        m_nPositionFS = m_pPresentationBar->GetPosition();
        m_pWndCaptureRect = new CNewCaptureRectChooser(ASSISTANT::Project::active->GetAvGrabber(),
                                                       rcLastSelection, bIsFullScreen, this, this);

        m_pWndCaptureRect->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                                    AfxRegisterWndClass(0,0,0,0), NULL, 
                                    WS_POPUP | WS_VISIBLE, rcDesktop, this, 0);
        
        CXTPCommandBars* pCommandBars = GetCommandBars();
        m_pPresentationBar->SetCommandBars(pCommandBars);

        UpdatePresentationBarPosition(m_nPositionSG);
       
        if (m_pPresentationBar)
            m_pPresentationBar->SetMainFrmWnd(this);

        m_pPresentationBar->RedrawWindow();  
        m_pPresentationBar->SetPreparationMode(true);
        if(m_bPresentationBarAutoHide)
           m_pPresentationBar->SetAutoHide(true);
        m_bParentChanged = true;
    }

    return S_OK;
}

void CMainFrameA::FinishSgSelectionFromPresentationBar()
{
   FinishedSgSelection(m_pWndCaptureRect->IsDoRecordFullScreen());
}

// Start screen recording
void CMainFrameA::FinishedSgSelection(bool bRecordFullScreen)
{
   if (m_pWndCaptureRect != NULL) {

      CRect rcLastCustom;
      m_pWndCaptureRect->GetLastCustomRect(rcLastCustom);
      CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
      if (pDoc != NULL)
         pDoc->SetLastSelectionRectangle(rcLastCustom, bRecordFullScreen);

      m_bIsShowSGPresentationBar = m_pWndCaptureRect->GetShowPresentationBar();

      CRect rcSelection = m_pWndCaptureRect->GetSelectionRect();
      m_nPositionSG = m_pPresentationBar->GetPosition();

      m_pWndCaptureRect->DestroyWindow();
      delete m_pWndCaptureRect;
      m_pWndCaptureRect = NULL;

      StartSGRecording(rcSelection);
      ShowPresentationBarInSgRec(false);

      ChangeModes(ASSISTANT::Project::active->IsPureScreenGrabbing() ? m_pAudioWidget : (CAudioWidget*)m_pMonitorWidget, NULL, WS_EX_TOPMOST);
      if ( ASSISTANT::Project::active->IsPureScreenGrabbing() == false )
      {
         m_pMonitorWidget->DisableLimitToWindow();
         ShowMonitorWidget();
         //m_pMonitorWidget->ShowWindow(GetShowAVMonitor() ? SW_SHOW : SW_HIDE );
      }
      else if ( pDoc != NULL )
      {
          m_pAudioWidget->ShowWindow(pDoc->GetShowAudioMonitor() ? SW_SHOW : SW_HIDE );
      }
   }
}

void CMainFrameA::CancelSgSelection()
{
    CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
    ASSISTANT::Project::active->DoPureScreenGrabbing(false);

    // make sure no audio widget is visible
    if ( m_pAudioWidget->GetSafeHwnd() && m_pAudioWidget->IsWindowVisible() )
      m_pAudioWidget->ShowWindow(SW_HIDE);
    // call it before previous parent (m_pWndCaptureRect) is deleted, to have a permanent valid reference to it's parent
    ChangeModes(ASSISTANT::Project::active->IsPureScreenGrabbing() ? m_pAudioWidget : (CAudioWidget*)m_pMonitorWidget, this);

    if (m_pWndCaptureRect != NULL) {
       CRect rcLastCustom;
       m_pWndCaptureRect->GetLastCustomRect(rcLastCustom);
       bool bIsFullScreenMode = m_pWndCaptureRect->IsDoRecordFullScreen();
       CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
       if (pDoc != NULL)
          pDoc->SetLastSelectionRectangle(rcLastCustom, bIsFullScreenMode);

        RestorePresentationBarFromSgRec();

        m_pWndCaptureRect->DestroyWindow();
        delete m_pWndCaptureRect;
        m_pWndCaptureRect = NULL;
    }

    if (m_pRecordingsView != NULL)
        m_pRecordingsView->RedrawWindow();

    if ( ASSISTANT::Project::active->IsPureScreenGrabbing() == true )
    {
       m_pAudioWidget->ShowWindow(SW_HIDE);
    }
    else 
    {
       m_pMonitorWidget->DisableLimitToWindow(false);
       ShowMonitorWidget();
    }

#ifndef _STUDIO
    RestoreAssistant();
    UndoReduceAssistant();
    ResetToTop();
#else
    RestoreAssistant();
#endif

    if (CStudioApp::IsLiveContextMode()) {
        CStudioApp::QuitLiveContextWithCode(EXIT_CODE_LC_NO_RECORDING_DONE);
    }
}


void CMainFrameA::RestorePresentationBarFromSgRec()
{
   if(m_pPresentationBar && m_bParentChanged)
   {
      m_nPositionSG = m_pPresentationBar->GetPosition();
      m_pPresentationBar->DestroyWindow();

      m_pPresentationBar->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 
         AfxRegisterWndClass(0,0,0,0), NULL, WS_POPUP | WS_VISIBLE, CRect(100, 100, 400, 200), 
         this, 0);

      m_pPresentationBar->SetLayeredWindowAttributes(RGB(255, 255, 255), (255 * 30) / 100, LWA_ALPHA | LWA_COLORKEY);  
      
      CXTPCommandBars* pCommandBars = GetCommandBars();
      m_pPresentationBar->SetCommandBars(pCommandBars);
      m_pPresentationBar->SetMainFrmWnd(this);
      m_pPresentationBar->SetPreparationMode(false);

      UpdatePresentationBarPosition(m_nPositionFS);
      
      if(m_pPresentationBar->IsWindowVisible() && !m_bIsInFullScreen)
         m_pPresentationBar->ShowWindow(SW_HIDE);

      m_pPresentationBar->RedrawWindow(); 

      m_bParentChanged = false;
   }
}

void CMainFrameA::ShowPresentationBarInSgRec(bool bFixedArea)
{
   if(m_pPresentationBar)
   {
      if(!bFixedArea)
         m_nPositionFS = m_pPresentationBar->GetPosition();

      m_pPresentationBar->DestroyWindow();

      CWnd *pParent = NULL;
      pParent = FindWindow(NULL, _T("CDrawWindow"));

      m_pPresentationBar->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 
         AfxRegisterWndClass(0,0,0,0), NULL, WS_POPUP | WS_VISIBLE, CRect(100, 100, 400, 200), 
         pParent, 0);

      m_pPresentationBar->SetLayeredWindowAttributes(RGB(255, 255, 255), (255 * 30) / 100, LWA_ALPHA | LWA_COLORKEY);  

      m_pPresentationBar->SetForegroundWindow();

      CXTPCommandBars* pCommandBars = GetCommandBars();
      m_pPresentationBar->SetCommandBars(pCommandBars);
      m_pPresentationBar->SetMainFrmWnd(this);
      m_pPresentationBar->SetPreparationMode(false);

      m_pPresentationBar->RedrawWindow(); 

      UpdatePresentationBarPosition(m_nPositionSG);

      if(m_bPresentationBarAutoHide)
         m_pPresentationBar->SetAutoHide(true);
      if(!m_bIsShowSGPresentationBar)
         m_pPresentationBar->ShowWindow(SW_HIDE);

      m_pPresentationBar->RedrawWindow();  
      m_bParentChanged = true;
   }
}

void CMainFrameA::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	int screenx = GetSystemMetrics(SM_CXSCREEN);
    int screeny = GetSystemMetrics(SM_CYSCREEN);

	//CStri
	
    int iBorderWidth = GetSystemMetrics(SM_CXBORDER);
    int iBorderHeight = GetSystemMetrics(SM_CYBORDER);

	
    
	lpMMI->ptMaxPosition = CPoint(-2*iBorderWidth, -2*iBorderHeight);
	lpMMI->ptMaxTrackSize = lpMMI->ptMaxSize = CPoint(screenx+24*iBorderWidth, screeny+24*iBorderHeight);
#ifdef _STUDIO
	CXTPMDIChildWnd::OnGetMinMaxInfo(lpMMI);
#else
   CXTPFrameWnd::OnGetMinMaxInfo(lpMMI);
#endif
}

void CMainFrameA::ShowSgSettings()
{
    CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
    if (pDocument && pDocument->IsRecording()) 
        return;

    /*CString csTitle;
    csTitle.LoadString(IDS_SG_SETTINGS_TITLE);
    CSGSettings sgSettings(csTitle);

    sgSettings.m_psh.dwFlags |= PSH_NOAPPLYNOW;
    sgSettings.m_psh.dwFlags &= ~PSH_HASHELP;

    sgSettings.Init(pDocument);

    sgSettings.DoModal();*/
    ShowAssistantSettings(ASSISTANT_SETTINGS_PAGE_SG);
}

void CMainFrameA::ShowAudioSettings()
{
    CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

    if (pDocument && pDocument->IsRecording()) 
        return;

    /*CString str;
    str.LoadString(IDS_AUDIO_VIDEO_SETTINGS);
    CAVCSettings avcSettings(str);

    avcSettings.m_psh.dwFlags |= PSH_NOAPPLYNOW;
    avcSettings.m_psh.dwFlags &= ~PSH_HASHELP;

    avcSettings.Init(pDocument, m_nAVSettingsTab);

    avcSettings.DoModal();*/
    ShowAssistantSettings(ASSISTANT_SETTINGS_PAGE_AV);
}

void CMainFrameA::GetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop)
{
    CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

    pDoc->GetSgStartStopHotKey(hotKeyStartStop);
}


void CMainFrameA::SetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop)
{
    CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

    pDoc->SetSgStartStopHotKey(hotKeyStartStop);
}

HRESULT CMainFrameA::TestHotKey(AVGRABBER::HotKey &hotKeyStartStop)
{
    CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

    HRESULT hr = pDoc->TestHotKey(hotKeyStartStop);

    return hr;
}

void CMainFrameA::GetPossibleKeys(CStringArray &arPossibleKeys)
{
    ASSISTANT::GetPossibleKeys(arPossibleKeys);
}

//Ribbon implementation Start
void CMainFrameA::OnMenuNew()
{
}
void CMainFrameA::OnMenuOpen()
{
}
void CMainFrameA::OnMenuSave()
{
}
void CMainFrameA::OnMenuSaveAs()
{
}
void CMainFrameA::OnMenuProjectSettings()
{
}
void CMainFrameA::OnMenuAssistantSettings()
{
    ShowAssistantSettings();

    /*
   CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

   //if(pDocument->IsRecording())
   //   return;

   //CXTPPropertySheet sh(_T("Mumu"));
   //CXTPRibbonCustomizeQuickAccessPageExt qap(GetCommandBars());
  // CXTPCustomizeSheet sh(GetCommandBars());
  // CXTPRibbonCustomizeQuickAccessPage qap(&sh);
  // sh.AddPage(&qap);
  // qap.AddCategories(IDR_MAINFRAME_A);
  // sh.DoModal();

   CString csTitle;
   csTitle.LoadString(ID_MENU_ASSISTANT_SETTINGS);

   CAssistantSettings assistantSettings(csTitle, GetCommandBars());
   
   assistantSettings.m_psh.dwFlags |= PSH_NOAPPLYNOW;
   assistantSettings.m_psh.dwFlags &= ~PSH_HASHELP;
   
   assistantSettings.Init(pDocument);
   
   assistantSettings.DoModal();
   */
}

void CMainFrameA::OnUpdateMenuNew(CCmdUI* pCmdUI)
{
}
void CMainFrameA::OnUpdateMenuOpen(CCmdUI* pCmdUI)
{
}
void CMainFrameA::OnUpdateMenuSave(CCmdUI* pCmdUI)
{
   if (m_pCurrentView && m_pCurrentView->GetRuntimeClass() == RUNTIME_CLASS(CStartupView))
   {
      pCmdUI->Enable(false);
   }
   else
      pCmdUI->Enable(true);
}
void CMainFrameA::OnUpdateMenuSaveAs(CCmdUI* pCmdUI)
{
   if (m_pCurrentView && m_pCurrentView->GetRuntimeClass() == RUNTIME_CLASS(CStartupView))
   {
      pCmdUI->Enable(false);
   }
   else
      pCmdUI->Enable(true);
}

void CMainFrameA::OnUpdateMenuProjectSettings(CCmdUI* pCmdUI)
{
      pCmdUI->Enable(TRUE);
}

void CMainFrameA::OnUpdateMenuAssistantSettings(CCmdUI* pCmdUI)
{
    CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
    
    pCmdUI->Enable(!pDocument->IsRecording());
}

void CMainFrameA::OnRevert()
{
   CString csMessage;
   csMessage.LoadString(IDS_REVERT_MESSAGE);
   int iAnswer = MessageBox(csMessage, NULL, MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
   // TODO: use CEditorDoc::SomeStaticMethod (ie ShowQuestionMessage())
   if(iAnswer == IDYES)
   {
      CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
      if (pDocument && pDocument->HasLoadedDocuments())
      {
         CString csLastSavedDocument;
         pDocument->GetLastSavedDocument(csLastSavedDocument);
         //pDocument->S
         pDocument->CloseDocument(false);
        
         HRESULT hr = pDocument->OpenSourceDocument(csLastSavedDocument.AllocSysString()); 
      }
   }
}
void CMainFrameA::OnUpdateRevert(CCmdUI* pCmdUI)
{
    CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

   if (pDocument && pDocument->HasLoadedDocuments())
   {
      CString csLastSavedDocument;
      csLastSavedDocument.Empty();
      pDocument->GetLastSavedDocument(csLastSavedDocument);
      if(!csLastSavedDocument.IsEmpty())
         pCmdUI->Enable(true);
      else
         pCmdUI->Enable(false);
   }
   else
      pCmdUI->Enable(false);
}
void CMainFrameA::OnMenuImport()
{
}

#ifndef _STUDIO
void CMainFrameA::OnUpdateRibbonTab(CCmdUI* pCmdUI)
{
   UINT nCurrentColorScheme = ((CAssistantApp *)AfxGetApp())->GetCurrentColorScheme();
   if(m_nCurrentLocalColorScheme != nCurrentColorScheme)
      SetColorScheme(nCurrentColorScheme);
   CXTPRibbonControlTab* pControl = DYNAMIC_DOWNCAST(CXTPRibbonControlTab, CXTPControl::FromUI(pCmdUI));
	if (!pControl)
		return;

   CXTPRibbonBar* pRibbonBar = pControl->GetRibbonBar();
   if(!pRibbonBar)
      return;

   bool bIsStartupMode = false;
   if (m_pCurrentView && m_pCurrentView->GetRuntimeClass() == RUNTIME_CLASS(CStartupView))
   {
      bIsStartupMode = true;
   }

#ifdef RIBBON_MINIMIZED
   pRibbonBar->SetRibbonMinimized(bIsStartupMode);
   
   CXTPRibbonTab* pTabTools = pRibbonBar->FindTab(ID_TAB_TOOLS);
   if(pTabTools)
      pTabTools->SetVisible(!bIsStartupMode);
   CXTPRibbonTab* pTabView = pRibbonBar->FindTab(ID_TAB_VIEW);
   if(pTabView)
      pTabView->SetVisible(!bIsStartupMode);
   CXTPRibbonTab* pTabStart = pRibbonBar->FindTab(ID_TAB_START);
   if(pTabStart)
      pTabStart->SetVisible(!bIsStartupMode);
   
   CXTPControl* pQaControlSave = pRibbonBar->GetQuickAccessControls()->FindControl(ID_FILE_SAVE);
   CXTPControl* pQaControlUndo = pRibbonBar->GetQuickAccessControls()->FindControl(ID_EDIT_UNDO);
   CXTPControl* pQaControlRedo = pRibbonBar->GetQuickAccessControls()->FindControl(ID_EDIT_REDO);
   
#endif

#ifndef RIBBON_MINIMIZED
   CXTPControl* pQaControlSave = pRibbonBar->GetQuickAccessControls()->FindControl(ID_FILE_SAVE);
   CXTPControl* pQaControlUndo = pRibbonBar->GetQuickAccessControls()->FindControl(ID_EDIT_UNDO);
   CXTPControl* pQaControlRedo = pRibbonBar->GetQuickAccessControls()->FindControl(ID_EDIT_REDO);
#endif
   if(pQaControlSave)
      pQaControlSave->SetVisible(!bIsStartupMode);
   if(pQaControlUndo)
      pQaControlUndo->SetVisible(!bIsStartupMode);
   if(pQaControlRedo)
      pQaControlRedo->SetVisible(!bIsStartupMode);

#ifdef HIDE_DISABLED_MENU_ITEMS
   CXTPControlPopup* pSysButton = pRibbonBar->GetSystemButton();
   if(pSysButton)
   {
      CXTPControls* pControls = pSysButton->GetCommandBar()->GetControls();
      for(int i = 0; i < pControls->GetCount(); i++)
      {
         CXTPControl* pControl = pControls->GetAt(i); 
         UINT nID = pControl->GetID();
         BOOL bEnable = pControl->GetEnabled();
         if(bEnable == FALSE)
         {
            pControl->SetHideFlags( xtpHideGeneric);
            pControl->SetVisible(FALSE);
         }
      }
   }
 #endif
}
#endif

//private
#ifndef _STUDIO
void CMainFrameA::SetColorScheme(UINT nColorScheme)
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

    RefreshPaneDrawing();

	GetCommandBars()->GetImageManager()->RefreshAll();
	GetCommandBars()->RedrawCommandBars();
	SendMessage(WM_NCPAINT);

	RedrawWindow(0, 0, RDW_ALLCHILDREN|RDW_INVALIDATE);


   m_nCurrentLocalColorScheme = nColorScheme;
}
#endif


//Create ribbon galleries
void CMainFrameA::CreateRibbonGalleries()
{
}

//Create ribbon
BOOL CMainFrameA::CreateRibbonBar()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();

#ifndef _STUDIO
	CMenu menu;
	menu.Attach(::GetMenu(m_hWnd));
	SetMenu(NULL);
#endif

	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBars->Add(_T("The Ribbon"), xtpBarTop, RUNTIME_CLASS(CXTPRibbonBar));
	if (!pRibbonBar)
	{
		return FALSE; 
	}
	pRibbonBar->EnableDocking(0);
   
	//pRibbonBar->GetTabPaintManager()->m_bSelectOnButtonDown = FALSE;

#ifndef _STUDIO
   //Create System menu
	CXTPControlPopup* pControlFile = (CXTPControlPopup*)pRibbonBar->AddSystemButton(IDS_SYSMENU);
	pControlFile->SetCommandBar(menu.GetSubMenu(0));
   pControlFile->SetIconId(IDB_GEAR_ASSISTANT);
	pControlFile->GetCommandBar()->SetIconSize(CSize(32, 32));
   pCommandBars->GetImageManager()->SetIcons(IDS_SYSMENU);
#endif

   CreateTabStart(GetCommandBars(), pRibbonBar);
   CreateTabTools(GetCommandBars(), pRibbonBar);
   CreateTabView(GetCommandBars(), pRibbonBar);
   CreateStatusBar();

//Create Quick Access Controls
   pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_FILE_NEW);
   pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_LSD_OPEN);
   pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_LSD_NEW);
   pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_PAGE_NEW);
   pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_FILE_SAVE)->SetVisible(FALSE);
   pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_EDIT_UNDO)->SetVisible(FALSE);
   pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_EDIT_REDO)->SetVisible(FALSE);
   pRibbonBar->GetQuickAccessControls()->CreateOriginalControls();

   UINT uQuickAccessBar[] = {ID_FILE_NEW,ID_LSD_OPEN,ID_FILE_SAVE,ID_LSD_NEW,ID_PAGE_NEW,-1,-1,-1,-1,ID_EDIT_UNDO,-1,ID_EDIT_REDO/*,-1,-1,ID_EDIT_UNDO,-1,ID_EDIT_REDO*/};
   pRibbonBar->GetQuickAccessControls()->GetCommandBars()->GetImageManager()->SetIcons(IDR_PNG_QUICKACCES, uQuickAccessBar, _countof(uQuickAccessBar), CSize(16, 16));

#ifndef _STUDIO
   //Create "About" button
   //UINT x = menu.GetMenuItemCount();
   CXTPControlPopup* pControlAbout = (CXTPControlPopup*)pRibbonBar->GetControls()->Add(xtpControlButtonPopup, ID_APP_ABOUT);
	pControlAbout->SetFlags(xtpFlagRightAlign);
   pControlAbout->SetCommandBar(menu.GetSubMenu(menu.GetMenuItemCount() - 1));
   //Frame Theme
#endif
   pRibbonBar->SetCloseable(FALSE);
   pRibbonBar->EnableFrameTheme();

   return TRUE;
}

// PRIVATE
#ifndef _STUDIO
void CMainFrameA::UpdatePresentationIcon()
{
   // Get a pointer to the command bars object. 
   CXTPCommandBars* pCommandBars = GetCommandBars();
   if (pCommandBars != NULL)
   {
      CXTPRibbonGroup *pGroup =  GetRibbonBar()->FindGroup(ID_GROUP_VIEW);
      if(pGroup)
      {
         CXTPControl* pControl = (CXTPControl*)pGroup->FindControl(ID_VIEW_PRESENTATION);

         if(m_uiPresentationTypeSelected == 1)
         {
            UINT uiTabView[] = {-1, ID_VIEW_PRESENTATION};
            pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabView, _countof(uiTabView), CSize(32, 32));
         }
         else if(m_uiPresentationTypeSelected == 0)
         {
            UINT uiTabView[] = {-1, -1,ID_VIEW_PRESENTATION};
            pCommandBars->GetImageManager()->SetIcons(IDB_TAB_VIEW, uiTabView, _countof(uiTabView), CSize(32, 32));
           
         }  
         pControl->SetCaption(ID_VIEW_PRESENTATION);  
      } 
      
      pCommandBars->RedrawCommandBars();

   }
}
#endif

// PRIVATE
void CMainFrameA::OnPresentationButton()
{
 if(m_uiPresentationTypeSelected == 1)
 {
    // from current slide
    OnFullScreenCS();
 }
 else if(m_uiPresentationTypeSelected == 0)
 {
   // from begining
   OnFullScreen();
 }
}

#ifdef _STUDIO
void CMainFrameA::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
    if (bActivate == 0) {
        KillAllTimer();
        if (m_pMonitorWidget)
        {
            m_pMonitorWidget->ShowWindow(SW_HIDE);
            m_bMonitorWidget = true;
        }
    }
    else {
       if (m_pMonitorWidget && m_bMonitorWidget == true )
       {
          ShowMonitorWidget();
          m_bMonitorWidget = false;
       }
    }

    CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
}
#endif

void CMainFrameA::ShowAssistantSettings(int iPage) {

    CAssistantDoc *pDocument = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

    CString csTitle;
    csTitle.LoadString(ID_MENU_ASSISTANT_SETTINGS);

    CAssistantSettings assistantSettings(csTitle, GetCommandBars(), NULL, iPage, false);

    assistantSettings.m_psh.dwFlags |= PSH_NOAPPLYNOW;
    assistantSettings.m_psh.dwFlags &= ~PSH_HASHELP;

    assistantSettings.Init(pDocument);

    assistantSettings.DoModal();

    if (m_pMonitorWidget != NULL)
        m_pMonitorWidget->SetAudioVideoTooltips(GetAudioTooltip(pDocument), GetVideoTooltip(pDocument));
    if (m_pAudioWidget != NULL)
       m_pAudioWidget->SetAudioVideoTooltips(GetAudioTooltip(pDocument), GetVideoTooltip(pDocument));
}

// PRIVATE
BOOL CMainFrameA::CreateStatusBar()
{
    
#ifndef _STUDIO
   CXTPStatusBarPane* pPane;

	pPane = m_wndStatusBar.AddIndicator(0);
	pPane->SetWidth(1);
	pPane->SetStyle(SBPS_STRETCH | SBPS_NOBORDERS);
	pPane->SetBeginGroup(FALSE);

	static UINT switches[] =
	{
		ID_SWITCH_STANDARD_LAYOUT,
		ID_SWITCH_STANDARDPAGEFOCUSLAYOUT,
		ID_SWITCH_FULLSCREENLAYOUT
	};

   UINT nIDSL = ID_SWITCH_STANDARD_LAYOUT;
   m_wndStatusBar.GetImageManager()->SetIcons(IDR_STANDARD_LAYOUT, &nIDSL, 1, CSize(16,16));

   UINT nIDFL = ID_SWITCH_STANDARDPAGEFOCUSLAYOUT;
   m_wndStatusBar.GetImageManager()->SetIcons(IDR_PAGE_FOCUSED, &nIDFL, 1, CSize(16,16));

   UINT nIDFS = ID_SWITCH_FULLSCREENLAYOUT;
   m_wndStatusBar.GetImageManager()->SetIcons(IDR_FULL_SCREEN, &nIDFS, 1, CSize(16,16));


	CXTPStatusBarSwitchPane* pSwitchPane = (CXTPStatusBarSwitchPane*)m_wndStatusBar.AddIndicator(new CXTPStatusBarSwitchPane(), ID_INDICATOR_VIEWSHORTCUTS);
	pSwitchPane->SetSwitches(switches, sizeof(switches)/sizeof(UINT));
	pSwitchPane->SetChecked(ID_SWITCH_STANDARD_LAYOUT);
	pSwitchPane->SetBeginGroup(TRUE);
	pSwitchPane->SetCaption(_T("&View Shortcuts"));

	m_wndStatusBar.SetDrawDisabledText(FALSE);
	m_wndStatusBar.SetRibbonDividerIndex(pSwitchPane->GetIndex() - 1);
	m_wndStatusBar.EnableCustomization();

	CXTPToolTipContext* pToolTipContext = m_wndStatusBar.GetToolTipContext();
	pToolTipContext->SetStyle(xtpToolTipOffice2007);
	pToolTipContext->SetFont(m_wndStatusBar.GetPaintManager()->GetIconFont());
#endif
   return TRUE;
}

void CMainFrameA::OnMonitorOption()
{
	PostMessage(WM_USER_AUDIO_SETTINGS, 0, 0);
}
//Ribbon implementation End

void CMainFrameA::OnUpdateStatusBarSwitchView(CCmdUI* pCmdUI)
{
#ifndef _STUDIO
   CXTPStatusBarSwitchPane* pPane = (CXTPStatusBarSwitchPane*)m_wndStatusBar.FindPane(ID_INDICATOR_VIEWSHORTCUTS);
#else
   // Status bar from Studio must be used
   CWnd *pMainFrame = AfxGetMainWnd();
   CXTPStatusBarSwitchPane* pPane = NULL;
   if (pMainFrame != NULL
       && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame)
       && ((CMainFrame *)pMainFrame)->GetStatusBar() != NULL) {
       pPane = (CXTPStatusBarSwitchPane*) ((CMainFrame *)pMainFrame)->GetStatusBar()->FindPane(ID_INDICATOR_VIEWSHORTCUTS);
   }
#endif
   if (pPane == NULL) {
       return;
   }

   CXTPStatusBarSwitchPane::SWITCH *pStandardLayoutBtn = pPane->GetSwitch(0);
   CXTPStatusBarSwitchPane::SWITCH *pStandardPageFocusedLayoutBtn = pPane->GetSwitch(1);
   CXTPStatusBarSwitchPane::SWITCH *pFullScreenBtn = pPane->GetSwitch(2);

   BOOL bEnableStandard = TRUE;
   BOOL bEnableFullscreen = TRUE;
   if (m_bShowStartpage == true) {
       bEnableStandard = FALSE;
   }
   CAssistantDoc *pAssistantDoc = GetAssistantDocument();
   if (pAssistantDoc != NULL) {
      if (pAssistantDoc->HasActivePage() == FALSE || GetStartupPageVisibility() == true) {
         bEnableFullscreen = FALSE;
      }
   }
   bool bRefresh = false;
   if (pStandardLayoutBtn->bEnabled != bEnableStandard || pFullScreenBtn->bEnabled != bEnableFullscreen) {
       bRefresh = true;
   }
   pStandardLayoutBtn->bEnabled = bEnableStandard;
   pStandardPageFocusedLayoutBtn->bEnabled = bEnableStandard;
   pFullScreenBtn->bEnabled = bEnableFullscreen;
   if (bRefresh) {
       pPane->Redraw();
   }

   switch (m_nCurrentLayout)
   {
   case AssistantLayouts(StandardLayout):
      {
         pPane->SetChecked(ID_SWITCH_STANDARD_LAYOUT);
         
         pStandardPageFocusedLayoutBtn->bHighlighted = false;
         pFullScreenBtn->bHighlighted = false;
         break;
      }
   case AssistantLayouts(StandardPagefocusLayout):
      {
         pPane->SetChecked(ID_SWITCH_STANDARDPAGEFOCUSLAYOUT);

         pStandardLayoutBtn->bHighlighted = false;
         pFullScreenBtn->bHighlighted = false;
         break;
      }
   case AssistantLayouts(FullscreenLayout):
      {
         pPane->SetChecked(ID_SWITCH_FULLSCREENLAYOUT);

         pStandardLayoutBtn->bEnabled = false;
         pStandardLayoutBtn->bHighlighted = false;
         pStandardPageFocusedLayoutBtn->bHighlighted = false;
         break;
      }
   case AssistantLayouts(FullscreenPagefocusLayout):
      {
         pPane->SetChecked(ID_SWITCH_FULLSCREENLAYOUT);

         pStandardLayoutBtn->bEnabled = false;
         pStandardLayoutBtn->bHighlighted = false;
         pStandardPageFocusedLayoutBtn->bHighlighted = true;
         break;
      }
   case AssistantLayouts(ReducedLayout):
      {
         // Status Bar is not visible in this case. If/When it will be visible please complete this case. For know we don't know the behaviour.
         ASSERT(false);
         break;
      }
   }
   return;
}

void CMainFrameA::OnStatusBarSwitchView(UINT nID)
{
   switch (nID)
   {
   case ID_SWITCH_STANDARD_LAYOUT:
      {
         OnButtonStandardLayout();
         break;
      }
   case ID_SWITCH_STANDARDPAGEFOCUSLAYOUT:
      {
         OnButtonPageFocusedLayout();
         break;
      }
   case ID_SWITCH_FULLSCREENLAYOUT:
      {
#ifdef _DVS2005
         OnButtonFullScreenCS(NULL, NULL);
#else
         OnButtonFullScreenCS();
#endif
         break;
      }
   }
}
void CMainFrameA::OnCheckboxShowHideWidget()
{
   if ( /*m_bRecording && */ASSISTANT::Project::active->IsPureScreenGrabbing())
   {
      // Show/hide audio widget
      if( m_pAudioWidget == NULL )
         return;

      bool bIsVisible = m_pAudioWidget->IsWindowVisible() == TRUE; 
      bIsVisible = !bIsVisible;// If it was visible hide it (or reverse).
      m_pAudioWidget->ShowWindow(bIsVisible ? SW_SHOWNORMAL : SW_HIDE);
      SetShowAudioMonitor(bIsVisible);
      // This settings must be saved here, as Audio Monitor is visible just on Pure Screen Grabbing
      AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("ShowAudioMonitor"), bIsVisible);
   }
   else if (m_pMonitorWidget)
   {
      // Show/hide monitor widget
      if(m_pMonitorWidget->IsWindowVisible())
      {
         m_pMonitorWidget->ShowWindow(SW_HIDE);
         m_bMonitorWidget = false;
      }
      else
      {
         m_pMonitorWidget->ShowWindow(SW_SHOWNORMAL);
         m_bMonitorWidget = true;
      }
      SetShowAvMonitor(m_bMonitorWidget);
   }
}

void CMainFrameA::OnUpdateCheckboxShowHideWidget(CCmdUI* pCmdUI)
{
   // Always enabled
   pCmdUI->Enable(TRUE);
   // see if it should be checked
   if ( ASSISTANT::Project::active->IsPureScreenGrabbing() == true )
   {
      CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();
      if ( pDoc == NULL )
         return;
      pCmdUI->SetCheck(pDoc->GetShowAudioMonitor());
   }
   else
      pCmdUI->SetCheck(GetShowAVMonitor());
}

// Enable/Disable monitor widget checkbox
void CMainFrameA::OnUpdateCheckboxShowHideMonitorWidget(CCmdUI* pCmdUI) 
{
   if (m_pMonitorWidget)
   { 
      if (m_pCurrentView && m_pCurrentView->GetRuntimeClass() == RUNTIME_CLASS(CStartupView) 
         || (m_pWndCaptureRect && ASSISTANT::Project::active->IsPureScreenGrabbing() == true))
          pCmdUI->Enable(FALSE);
      else
      {
         pCmdUI->Enable(TRUE);
         pCmdUI->SetCheck(GetShowAVMonitor()/*m_pMonitorWidget->IsWindowVisible()*/);
      }
   }
   else
      pCmdUI->Enable(FALSE);
}

// Show/hide monitor widget
void CMainFrameA::OnCheckboxShowHideMonitorWidget() 
{
   if (m_pMonitorWidget)
   {
      m_bMonitorWidget = !GetShowAVMonitor();
      m_pMonitorWidget->ShowWindow(m_bMonitorWidget? SW_SHOW : SW_HIDE);
      SetShowAvMonitor(m_bMonitorWidget);
   }
}
void CMainFrameA::OnUpdateCheckboxShowHideStartupPage(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(GetShowStartupPage());
}
void CMainFrameA::OnCheckboxShowHideStartupPage() 
{
   SetShowStartupPage(!GetShowStartupPage());
}
void CMainFrameA::OnUpdateMonitorWidget(CCmdUI* pCmdUI) 
{
   switch (pCmdUI->m_nID )
   {
   case ID_MONITORWIDGET_SHOWDURATION:
      pCmdUI->Enable();
      pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetShowDuration"), 1) == 1);
      break;
   case ID_MONITORWIDGET_SHOWPAGES:
      pCmdUI->Enable();
      pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetShowPages"), 1) == 1);
      break;
   case ID_MONITORWIDGET_SHOWDISKSPACE:
      pCmdUI->Enable();
      pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetShowDiskSpace"), 1) == 1);
      break;
   case ID_MONITORWIDGET_LOCKPOSITION:
      pCmdUI->Enable();
      pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetLockPosition"), 0) == 1);
      break;
   case ID_MONITORWIDGET_LIMITTOWINDOW:
      if ( ASSISTANT::Project::active->isScreenGrabbingActive_ || m_pWndCaptureRect != NULL )
         pCmdUI->Enable(FALSE);
      else
         pCmdUI->Enable();
      pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetLimitToWindow"), 0) == 1);
      break;
   }
}

void CMainFrameA::OnShowMonitorWidget(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_pMonitorWidget == NULL)
       return;

   CXTPCommandBars* pCommandBars = GetCommandBars();
   if (pCommandBars != NULL)
   {
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

      CXTPControl* pControl = tagNMCONTROL->pControl;
      if(pControl)
      {
         BOOL bIsChecked = pControl->GetChecked();
         if(bIsChecked)
		      pControl->SetChecked(FALSE);
	      else
	         pControl->SetChecked(TRUE);
         
         switch(pControl->GetID())
         {
         case ID_MONITORWIDGET_SHOWDURATION:
            m_pMonitorWidget->ShowDuration(!bIsChecked);
            break;
         case ID_MONITORWIDGET_SHOWPAGES:
            m_pMonitorWidget->ShowPages(!bIsChecked);
            break;
         case ID_MONITORWIDGET_SHOWDISKSPACE:
            m_pMonitorWidget->ShowDiskSpace(!bIsChecked);
            break;
         case ID_MONITORWIDGET_LOCKPOSITION:
            m_pMonitorWidget->LockPosition(!bIsChecked);
            break;
         case ID_MONITORWIDGET_LIMITTOWINDOW:
            m_pMonitorWidget->LimitToWindow(!bIsChecked);
            break;
         }
      }
   }
}

void CMainFrameA::OnUpdateAudioWidget(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();
   switch (pCmdUI->m_nID )
   {
   case ID_AUDIOWIDGET_SHOWDURATION:
      pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowDuration"), 1) == 1);
      break;
   case ID_AUDIOWIDGET_SHOWDISKSPACE:
      pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowDiskSpace"), 1) == 1);
      break;
   case ID_AUDIOWIDGET_LOCKPOSITION:
      pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioLockPosition"), 0) == 1);
      break;
   }
}
void CMainFrameA::OnShowAudioWidget(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_pAudioWidget == NULL)
       return;

   CXTPCommandBars* pCommandBars = GetCommandBars();
   if (pCommandBars != NULL)
   {
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

      CXTPControl* pControl = tagNMCONTROL->pControl;
      if(pControl)
      {
         BOOL bIsChecked = pControl->GetChecked();
         if(bIsChecked)
		      pControl->SetChecked(FALSE);
	      else
	         pControl->SetChecked(TRUE);
         
         switch(pControl->GetID())
         {
         case ID_AUDIOWIDGET_SHOWDURATION:
            m_pAudioWidget->ShowDuration(!bIsChecked);
            break;
         case ID_AUDIOWIDGET_SHOWDISKSPACE:
            m_pAudioWidget->ShowDiskSpace(!bIsChecked);
            break;
         case ID_AUDIOWIDGET_LOCKPOSITION:
            m_pAudioWidget->LockPosition(!bIsChecked);
            break;
         }
      }
   }
}
#ifndef _STUDIO
BOOL CMainFrameA::OnPinRangeClick( UINT nID ,NMHDR * pNotifyStruct, LRESULT* result)
{
   CXTPRibbonControlRecentFileListExt::GetInstance()->OnPinRangeClick(nID);
   
   return TRUE;
}
#endif

void CMainFrameA::SetPresentationBarMarker(int nMarker)
{
   if ( m_pPresentationBar != NULL )
      m_pPresentationBar->SetPresentationBarMarker(nMarker);
}

void CMainFrameA::SetPresentationBarPen(int nPen)
{
   if ( m_pPresentationBar != NULL )
      m_pPresentationBar->SetPresentationBarPen(nPen);
}
bool CMainFrameA::IsScreenGrabbing()
{
  return m_bScreenGrabing;
}

void CMainFrameA::OpenLastRecording()
{
    bool bSwitchAutomatically = GetSwitchAutomaticallyToEdit();

    if (bSwitchAutomatically) {
        ASSISTANT::PresentationInfo *lastRecording = NULL;
        if (ASSISTANT::Project::active)
            lastRecording = ASSISTANT::Project::active->GetLastRecording();

        if (lastRecording) {
            if (m_bIsInFullScreen)
                FullScreenModeOff();
            lastRecording->StartEdit();
        }
    }
}

bool CMainFrameA::IsInStartupMode()
{
    bool bIsStartupMode = false;
    if (m_pCurrentView && m_pCurrentView->GetRuntimeClass() == RUNTIME_CLASS(CStartupView))
    {
        bIsStartupMode = true;
    }

    return bIsStartupMode;
}

unsigned int CMainFrameA::GetCurrentLayout()
{
   return m_nCurrentLayout;
}

unsigned int CMainFrameA::GetFullscreenLayout()
{
   return FullscreenLayout;
}
unsigned int CMainFrameA::GetFullscreenPagefocusLayout()
{
   return FullscreenPagefocusLayout;
}
/*void CMainFrameA::SetPresentationBarTransparent()
{
   if(m_pPresentationBar && m_pPresentationBar->IsWindowVisible())
   {
      CXTPCommandBar *pCtrlPens = GetCommandBars()->GetContextMenus()->GetAt(1);
      CXTPCommandBar *pCtrlMarkers = GetCommandBars()->GetContextMenus()->GetAt(2);

      if(m_pPresentationBar->IsTransparent() && !pCtrlPens->IsVisible() && !pCtrlMarkers->IsVisible())
         m_pPresentationBar->SetPresentationBarTransparent();
   }
}

void CMainFrameA::SetPresentationBarAutoHide(CPoint point)
{
   if(m_pPresentationBar && m_bPresentationBarAutoHide)
   {
      CRect rcWnd;

      rcWnd = m_pPresentationBar->GetStandardWindowRect();

      CXTPCommandBar *pCtrlPens = GetCommandBars()->GetContextMenus()->GetAt(1);
      CXTPCommandBar *pCtrlMarkers = GetCommandBars()->GetContextMenus()->GetAt(2);
      
      if(rcWnd.PtInRect(point))
      {
         if(!m_pPresentationBar->IsWindowVisible())
            m_pPresentationBar->SetWindowAnimation(true);
      }
      else
      {
         if(m_pPresentationBar->IsWindowVisible() && !pCtrlPens->IsVisible() && !pCtrlMarkers->IsVisible())
            m_pPresentationBar->SetWindowAnimation(false);
      }
   }
}*/

void CMainFrameA::OnUpdatePresentationBar(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

void CMainFrameA::OnUpdatePresentationBarShow(CCmdUI* pCmdUI)
{
    BOOL bChecked = FALSE;

    if (m_bShowPresentationBar)
        bChecked = TRUE;

    pCmdUI->SetCheck(bChecked);
    pCmdUI->Enable(TRUE);
}

void CMainFrameA::OnUpdatePresentationBarAutoHide(CCmdUI* pCmdUI)
{
    BOOL bChecked = FALSE;

    if (m_bPresentationBarAutoHide)
        bChecked = TRUE;

    pCmdUI->SetCheck(bChecked);
    pCmdUI->Enable(TRUE);
}

void CMainFrameA::OnPresentationBarContextMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   CRect rcMainWnd, rcStatusBar;
   GetWindowRect(rcMainWnd);
#ifndef _STUDIO
   ::GetWindowRect(m_wndStatusBar, rcStatusBar);
#endif

   CXTPControl *pCtrlPos = pCommandBars->GetContextMenus()->GetAt(3)->GetControl(0);
  
   int nWidth = 0;
   int nHeight = 0;
   CXTPControl *pCtrlAutoHide = NULL;

   if (pCommandBars != NULL)
   {
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

      CXTPControl* pControl = tagNMCONTROL->pControl;
      if(pControl)
      {
         BOOL bIsChecked = pControl->GetChecked();
         if(pControl->GetParent() == pCtrlPos->GetCommandBar())
         {
            for(int i=0;i<pCtrlPos->GetCommandBar()->GetControlCount();i++)
               pCtrlPos->GetCommandBar()->GetControl(i)->SetChecked(FALSE);
         }
         if(bIsChecked)
            pControl->SetChecked(FALSE);
         else
            pControl->SetChecked(TRUE);

         switch(pControl->GetID())
         {
         case ID_PRESENTATIONBAR_POSITION_BOTTOM:
            m_pPresentationBar->SetPosition(OT_BOTTOMMIDDLE);
            break;
         case ID_PRESENTATIONBAR_POSITION_TOP:
            m_pPresentationBar->SetPosition(OT_TOPMIDDLE);  
            break;
         case ID_PRESENTATIONBAR_POSITION_LEFT:
            m_pPresentationBar->SetPosition(OT_MIDDLELEFT);
            break;
         case ID_PRESENTATIONBAR_POSITION_RIGHT:
            m_pPresentationBar->SetPosition(OT_MIDDLERIGHT);
            break;
         case ID_PRESENTATIONBAR_SHOWTRANSPARENT:
            m_pPresentationBar->ShowTransparent(pControl->GetChecked()?true:false);
            break;
         case ID_PRESENTATIONBAR_SMALLICONS:
            m_pPresentationBar->SetSmallIcons(pControl->GetChecked()?true:false);
            break;
         case ID_PRESENTATIONBAR_AUTOHIDE:
            m_bPresentationBarAutoHide = pControl->GetChecked()?true:false;
            pCtrlAutoHide = GetRibbonBar()->GetControls()->FindControl(ID_PRESENTATION_BAR_AUTOHIDE);
            if(pCtrlAutoHide)
               pCtrlAutoHide->SetChecked(m_bPresentationBarAutoHide);
            m_pPresentationBar->SetAutoHide(pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN:
            m_pPresentationBar->ShowHideExitFullScreen(pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_PENCILS:
            m_pPresentationBar->ShowHideAnnotations(ID_PRESENTATIONBAR_PENS, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_MARKERS:
            m_pPresentationBar->ShowHideAnnotations(ID_PRESENTATIONBAR_MARKERS, pControl->GetChecked()?true:false);          
            break;
         case ID_ADDORREMOVEBUTTONS_POINTER:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_TELEPOINTER, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_MOUSENAV, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_START:
            m_pPresentationBar->ShowHideRecordingTools(ID_PRESENTATIONBAR_START, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_STOP:
            m_pPresentationBar->ShowHideRecordingTools(ID_PRESENTATIONBAR_STOP, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_PAUSERECORDING:
            m_pPresentationBar->ShowHideRecordingTools(ID_PRESENTATIONBAR_PAUSE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_SCREENGRABBING:
            m_pPresentationBar->ShowHideScreenGrabbing(pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_PAGEFOCUSED:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_PAGEFOCUSED, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_DOCUMENTSTRUCT, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_RECORDINGS:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_RECORDINGS, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_CUT:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_CUT, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_COPY:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_COPY, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_PASTE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_PASTE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_UNDO:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_UNDO, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_ONECLICKCOPY:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_ONECLICKCOPY, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_TEXT:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_TEXT, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_LINE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_LINE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_POLYLINE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_POLYLINE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_FREEHAND:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_FREEHAND, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_ELLIPSE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_ELLIPSE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_RECTANGLE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_RECTANGLE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_POLYGON:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_POLYGON, pControl->GetChecked()?true:false);
            break; 
         case ID_ADDORREMOVEBUTTONS_INSERTPAGE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_INSERTPAGE, pControl->GetChecked()?true:false);
            break; 
         case ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_PREVIOUSPAGE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_NEXTPAGE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_NEXTPAGE, pControl->GetChecked()?true:false);
            break;
         }
      }
   }
}

void CMainFrameA::OnIdleUpdateCmdUI()
{
    if(m_pPresentationBar && m_pPresentationBar->IsWindowVisible())
    {
        m_pPresentationBar->UpdateDialogControls(m_pPresentationBar, FALSE);
        m_pPresentationBar->UpdateAppearance();
        if (!m_pPresentationBar->IsAutoHide())
            m_pPresentationBar->PutWindowOnTop(); 
    }

   CRect rcMainWnd;
   GetWindowRect(rcMainWnd);

   if( m_mainRect.IsRectNull() == TRUE && m_pMonitorWidget && m_pMonitorWidget->IsWindowVisible())
      m_mainRect = rcMainWnd;

   if(m_pMonitorWidget && m_pMonitorWidget->IsWindowVisible() && m_pMonitorWidget->IsLimitToWindow())
   {
      if(m_mainRect.left != rcMainWnd.left || m_mainRect.top != rcMainWnd.top ||
         m_mainRect.bottom != rcMainWnd.bottom || m_mainRect.right != rcMainWnd.right)
      {
         CRect rcWidget;
         m_pMonitorWidget->GetWindowRect(rcWidget);
         ::GetWindowRect(m_pMonitorWidget->GetSafeHwnd(), rcWidget);
         
         int cx = m_mainRect.left - rcMainWnd.left;
         int cy = m_mainRect.top - rcMainWnd.top;
         int nWidth = m_mainRect.Width() - rcMainWnd.Width();
         int nHeight = m_mainRect.Height() - rcMainWnd.Height();

         rcWidget.left -= cx;
         rcWidget.top -= cy;
         rcWidget.left -= nWidth;
         rcWidget.top -= nHeight;

        
         if(rcWidget.right > rcMainWnd.right)
            rcWidget.right = rcMainWnd.right - rcWidget.Width();
            
         if(rcWidget.top < rcMainWnd.top)
            rcWidget.top = rcMainWnd.top;

         if(rcWidget.left < rcMainWnd.left)
            rcWidget.left = rcMainWnd.left;

         if(rcWidget.bottom > rcMainWnd.bottom)
            rcWidget.top = rcMainWnd.bottom - rcWidget.Height();
          
         ::SetWindowPos(m_pMonitorWidget->GetSafeHwnd(), NULL, rcWidget.left, rcWidget.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
         
         m_mainRect = rcMainWnd;
      }
   }


#ifdef _STUDIO
   CXTPMDIChildWnd::OnIdleUpdateCmdUI();
#else
   CXTPFrameWnd::OnIdleUpdateCmdUI();
#endif
}

bool CMainFrameA::GetStartupPageVisibility()
{
   return m_bShowStartpage;
}

void CMainFrameA::OnCheckBoxShowHideStatusBar() 
{
#ifndef _STUDIO
   m_wndStatusBar.SetPaneText(0, csStatus);//, TRUE);
#else
    CWnd *pFrame = AfxGetMainWnd();
    if (pFrame && pFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame)) 
    {
        CMainFrame *pMainFrame = (CMainFrame*) pFrame;
        SetShowStatusBar(!pMainFrame->IsStatusbarVisible());
        ShowControlBar(pMainFrame->GetStatusBar(), pMainFrame->IsStatusbarVisible() == false ? TRUE : FALSE, FALSE);

        /*if (((CMainFrame *)pMainFrame)->IsStatusbarVisible()) {
            SetShowStatusBar(false);
            ShowControlBar(((CMainFrame *)pMainFrame)->GetStatusBar(), FALSE, FALSE);
        } else {
            SetShowStatusBar(true);
            ShowControlBar(((CMainFrame *)pMainFrame)->GetStatusBar(), TRUE, FALSE);
        }*/
    }
#endif
}

void CMainFrameA::OnUpdateStatusBarView(CCmdUI* pCmdUI)
{
    BOOL bEnable =TRUE;

#ifndef _STUDIO
    BOOL bStatusBarVisibility = m_wndStatusBar.IsVisible();
#else
    BOOL bStatusBarVisibility = GetShowStatusBar();
#endif

    pCmdUI->SetCheck(bStatusBarVisibility);
    pCmdUI->Enable(TRUE);

    UpdateStatusDiskSpace();
}

void CMainFrameA::CloseAllContent()
{
   KillAllTimer();

   // TODO: Add your message handler code here and/or call default
   if (m_bRecording)
   {
      CString csMessage;
      csMessage.LoadString(IDS_STOP_RECORDING);
      int iAnswer = CEditorDoc::ShowWarningMessage(csMessage, NULL, MB_YESNO);
      
      if (iAnswer == IDNO)
         return;
   }
   
   if (ASSISTANT::Project::active != NULL)
   {
      bool bClose = ASSISTANT::Project::active->CloseDocumentsAndPresentations();
      if (!bClose)
         return;
      
      delete ASSISTANT::Project::active;
      ASSISTANT::Project::active = NULL;
   }

   if (m_pPresentationBar)
      delete m_pPresentationBar;
   m_pPresentationBar = NULL;

   if (m_pMonitorWidget) {
       m_pMonitorWidget->DestroyWindow();
       delete m_pMonitorWidget;
   }
   m_pMonitorWidget = NULL;

   if (m_pAudioWidget) {
       m_pAudioWidget->DestroyWindow();
       delete m_pAudioWidget;
   }
   m_pAudioWidget = NULL;

#ifndef _STUDIO
   // Show the document structure
   ShowHidePanesForSgRecording(TRUE);
#endif

   // Set layout to standard layout
   if (m_nCurrentLayout != StandardLayout)
   {
#ifndef _STUDIO
      ShowMenu();
      //ShowStatusBar();
#endif
      ShowPanes();

   }
   //set the standard layout in order to save commandbars and panels configuration   
   SaveCommandBars(_T("CommandBars"));
   
   // Save the current state for docking panes.
   CXTPDockingPaneLayout layoutNormal(&m_paneManager);
   m_paneManager.GetLayout(&layoutNormal); 
   layoutNormal.Save(_T("NormalLayout"));

#ifndef _STUDIO
   bool bStatusVisible = m_wndStatusBar.IsWindowVisible() == 0 ? false : true;
   SetShowStatusBar(bStatusVisible);
#endif

   if (m_pButtonStandardLayout) 
       delete m_pButtonStandardLayout;
   m_pButtonStandardLayout = NULL;

}

void CMainFrameA::KillAllTimer()
{
    if (m_nAudioTimer != 0)
        KillTimer(m_nAudioTimer);
    m_nAudioTimer = 0;

    if (m_nVideoTimer != 0)
        KillTimer(m_nVideoTimer);
    m_nVideoTimer = 0;

    if (m_nCheckDiskSpace != 0)
        KillTimer(m_nCheckDiskSpace);
    m_nCheckDiskSpace = 0;

    if (m_nCheckAudioLevel != 0)
        KillTimer(m_nCheckAudioLevel);
    m_nCheckAudioLevel = 0;
    
    if (m_nUpdateDiscSpace != 0)
        KillTimer(m_nUpdateDiscSpace);
    m_nUpdateDiscSpace = 0;
    
    if (m_nUpdateRecordTime != 0)
        KillTimer(m_nUpdateRecordTime);
    m_nUpdateRecordTime = 0;
    
    if (m_nPauseButtonTimer != 0)
        KillTimer(m_nPauseButtonTimer);
    m_nPauseButtonTimer = 0;
}


void CMainFrameA::OnShowPresentationBar(NMHDR* pNMHDR, LRESULT* pResult)
{
    m_bShowPresentationBar = !m_bShowPresentationBar;

   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("ShowPresentationBar"), m_bShowPresentationBar);
}

void CMainFrameA::OnAutoHidePresentationBar(NMHDR* pNMHDR, LRESULT* pResult)
{
    m_bPresentationBarAutoHide = !m_bPresentationBarAutoHide;
  // GetPresentationBar()->SetAutoHide(m_bPresentationBarAutoHide);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("AutoHidePresentationBar"), m_bPresentationBarAutoHide);
}

CString CMainFrameA::InitDiskSpace()
{
   CString csStatus;
   CString csRecordPath;
   int iMbLeft = 0;
   ASSISTANT::GetLecturnityHome(csRecordPath);  
   if (!csRecordPath.IsEmpty())
   {
      SystemInfo::CalculateDiskSpace(csRecordPath, iMbLeft);
      if (iMbLeft > 1024)
      {
         csStatus.Format(_T("%dGB"), (int)(iMbLeft/1024));
      }
      else 
      {
         csStatus.Format(_T("%dMB"), iMbLeft);
      }

   }
   else
   {
      csStatus = _T("??MB");
   }
   return csStatus;
}

void CMainFrameA::AddTooltip(UINT nGroupId, UINT nControlID, UINT nTextID)
{
   CString csTitle, csDescription;
   CString csFromResources;
   csFromResources.LoadString(nTextID);

   int iPos = csFromResources.ReverseFind(_T('\n'));

   csTitle = csFromResources.Right(csFromResources.GetLength() - iPos - 1);
   csDescription = csFromResources.Left(iPos);

   CXTPRibbonGroup *pGroup =  GetRibbonBar()->FindGroup(nGroupId);
   if(pGroup)
   {
      CXTPControl* pControl = (CXTPControl*)pGroup->FindControl(nControlID);
      // BUGFIX 4835: pressed state is no longer used for blinking since LEC 4
      // pControl->SetPressed(true);
      if(pControl)
      {
         pControl->SetTooltip(csTitle);
         pControl->SetDescription(csDescription);
      }
   }
}

void CMainFrameA::ChangeModes(CAudioWidget *pWidgetRef, CWnd *pParentWnd, DWORD dwExtraStyle)
{
   if ( pWidgetRef == NULL )
   {
      ASSERT(FALSE);
      return;
   }
   CRect rcWidget;
   if ( pWidgetRef->GetSafeHwnd() != NULL )
      pWidgetRef->GetWindowRect(rcWidget);
   pWidgetRef->DestroyWindow();
   pWidgetRef->CreateEx(dwExtraStyle | WS_EX_LAYERED | WS_EX_TOOLWINDOW, 
      AfxRegisterWndClass(0,0,0,0), NULL, WS_POPUP | WS_VISIBLE, CRect(100, 100, 400, 200), 
      pParentWnd, 0);
   pWidgetRef->StartSavePosition();
   pWidgetRef->SetCommandBars(GetCommandBars());
   pWidgetRef->UpdateWindowPlacement();
   //   pWidgetRef->SetWindowPos(NULL, 100, 100, rcWidget.Width(), rcWidget.Height(), SWP_NOZORDER);
}

CAudioWidget* CMainFrameA::GetWidget()
{
   if ( ASSISTANT::Project::active->IsPureScreenGrabbing() == true )
      return m_pAudioWidget;
   else
      return (CAudioWidget*)m_pMonitorWidget;
}

void CMainFrameA::UpdatePresentationBarPosition(UINT nPosition) {
    CXTPControl *pControl = NULL;
    CXTPCommandBars *pCommandBars = GetCommandBars();
    if (pCommandBars != NULL) {
        CXTPControl *pCtrlPos = pCommandBars->GetContextMenus()->GetAt(3)->GetControl(0);
        switch(nPosition) {
        case OT_MIDDLERIGHT:
            pControl = pCtrlPos->GetCommandBar()->GetControls()->FindControl(ID_PRESENTATIONBAR_POSITION_RIGHT);
            break;
        case OT_BOTTOMMIDDLE:
            pControl = pCtrlPos->GetCommandBar()->GetControls()->FindControl(ID_PRESENTATIONBAR_POSITION_BOTTOM);
            break;
        case OT_TOPMIDDLE:
            pControl = pCtrlPos->GetCommandBar()->GetControls()->FindControl(ID_PRESENTATIONBAR_POSITION_TOP);
            break;
        case OT_MIDDLELEFT:
            pControl = pCtrlPos->GetCommandBar()->GetControls()->FindControl(ID_PRESENTATIONBAR_POSITION_LEFT);
            break;
        }
    }
    if (pControl != NULL && !pControl->GetChecked()) {
       NMXTPCONTROL tagNMCONTROL;
       tagNMCONTROL.pControl = pControl;
       NMHDR * pNmhdr = (NMHDR *) (LPARAM)&tagNMCONTROL;

       OnPresentationBarContextMenu(pNmhdr, 0);
    } else {
       m_pPresentationBar->SetPosition(nPosition);
    }
}
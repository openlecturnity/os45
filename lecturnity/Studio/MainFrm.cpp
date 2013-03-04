// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "RibbonBars.h"                 // Studio project root
#include "Studio.h"

#include "MainFrm.h"

#include "..\Assistant2_0\MainFrm.h"   // Assistant
#include "..\Assistant2_0\AssistantSettings.h"   // Assistant
#include "..\Assistant2_0\backend\mlb_misc.h"
#include "GalleryItems.h"               // lsutl32.dll
#include "ControlLineStyleSelector.h"   // lsutl32.dll
#include "ControlLineWidthSelector.h"   // lsutl32.dll
#include "ControlArrowStyleSelector.h"  // lsutl32.dll

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

// Global variables and defines
#define ASSISTANT_REGISTRY _T("Software\\imc AG\\LECTURNITY\\Assistant")
#define COMMAND_BARS_MODE_ASSISTANT _T("CommandBarsStudio")
#define COMMAND_BARS_MODE_EDITOR _T("CommandBarsEditor")

static UINT indicators[] =
{
    ID_STATUS_DISKSPACE,
    ID_STATUS_DURATION
};

static int aiFontSize [22] = { 8, 9, 10, 12, 14, 16, 18, 20, 24, 28, 32,
36, 40, 44, 48, 54, 60, 66, 72, 80, 88, 96};


// MFC message map 
BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_SHOWWINDOW()
    ON_COMMAND(ID_NEXT_WINDOW, &CMainFrame::OnNextWindow)
    ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)
    ON_COMMAND(XTP_ID_RIBBONCUSTOMIZE, OnCustomizeQuickAccess)
    ON_COMMAND(ID_LRD_OPEN, OnLrdOpen)
    ON_UPDATE_COMMAND_UI(ID_LRD_OPEN, OnUpdateLrdOpen)
    ON_XTP_CREATECONTROL()
    ON_WM_SYSCOMMAND()
    ON_XTP_EXECUTE_RANGE(XTP_ID_PIN_FILE_MRU_FIRST,XTP_ID_PIN_FILE_MRU_LAST,OnPinRangeClick)
    ON_UPDATE_COMMAND_UI(XTP_ID_RIBBONCONTROLTAB, OnUpdateRibbonTab)
	ON_WM_GETMINMAXINFO()
    ON_MESSAGE(WM_XTP_TOOLBARCONTEXTMENU, OnToolbarContextMenu)
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CMainFrame Constructor and destructor

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

CMainFrame::CMainFrame()
{
    m_bRecentFilelist = false;
    m_bWindowPositioningFromRegistry =false;
    m_pRibbonBars = NULL;
}

CMainFrame::~CMainFrame()
{
   SAFE_DELETE(m_pRibbonBars);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CMDIFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    // TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
    //  CREATESTRUCT cs modifizieren.

    return TRUE;
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    return CXTPMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::FullScreenModeOn()
{
    // Store window position and dimension
    CMDIChildWnd* pChild = MDIGetActive();
    if(pChild)
       pChild->ShowWindow(SW_HIDE);
    GetWindowRect(&m_rcNormalScreen);

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
     GetRibbonBar()->EnableFrameTheme(FALSE);

    HideStatusBar();
    HideMenu();
    SetWindowPos(NULL, -2*iBorderWidth, -2*iBorderHeight,
                 screenx+4*iBorderWidth, screeny+4*iBorderHeight, SWP_NOZORDER);

    if(pChild)
    {
       DWORD dwStyle  = ::GetWindowLong(pChild->m_hWnd, GWL_STYLE);
       dwStyle &= ~WS_CAPTION; 
       ::SetWindowLong(pChild->m_hWnd, GWL_STYLE, dwStyle);
       pChild->ShowWindow(SW_MAXIMIZE);
    }

    RecalcLayout();
   

   
}

void CMainFrame::FullScreenModeOff(UINT uiPresentationTypeSelected)
{
    // reset window style
   CMDIChildWnd* pChild = MDIGetActive();
   if(pChild)
      pChild->ShowWindow(SW_HIDE);
    ::SetWindowLong(m_hWnd, GWL_STYLE, m_lWindowStyle);

    // reset window position and dimension
    MoveWindow(&m_rcNormalScreen);

    RecalcLayout();

    UpdatePresentationIcon(uiPresentationTypeSelected);
    GetRibbonBar()->EnableFrameTheme(TRUE);

    ShowMenu();
    ShowStatusBar();
    if (pChild)
    {
      MDIMaximize(pChild);
      pChild->ShowWindow(SW_SHOW);
    }
}

void CMainFrame::Minimize()
{
    ShowWindow(SW_SHOWMINIMIZED);
}

void CMainFrame::Restore()
{
    if (IsIconic())
        ShowWindow(SW_RESTORE);
}

void CMainFrame::SetStatusBarText(int index, LPCTSTR lpszNewText)
{
    m_wndStatusBar.SetPaneText(index, lpszNewText);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    m_wndStatusBar.ShowWindow(SW_SHOW);

    // Initialize the command bars
    if (!InitCommandBars())
        return -1;

    CXTPPaintManager::SetTheme(xtpThemeRibbon);
    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = GetCommandBars();
    if(pCommandBars == NULL) {
        TRACE0("Failed to create command bars object.\n");
        return -1;      // fail to create
    }

    // define transparent color
    XTPImageManager()->SetMaskColor(RGB(0x00, 0x00, 0x00));

    // Set tooltip context
    CXTPToolTipContext* pToolTipContext = GetCommandBars()->GetToolTipContext();
    pToolTipContext->SetStyle(xtpToolTipOffice2007);
    pToolTipContext->ShowTitleAndDescription();
    pToolTipContext->ShowImage(TRUE, 0);
    pToolTipContext->SetMargin(CRect(2, 2, 2, 2));
    pToolTipContext->SetMaxTipWidth(180);
    pToolTipContext->SetFont(pCommandBars->GetPaintManager()->GetIconFont());
    pToolTipContext->SetDelayTime(TTDT_INITIAL, 900);

    m_lWindowStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);

    m_pRibbonBars = new CRibbonBars(); 
    if(!m_pRibbonBars->CreateRibbonBars(pCommandBars,m_hWnd,this))
       return -1;

    CreateAssistantStatusBar();

    // Read window settings (see also OnClose())
    CWinApp *pApp = AfxGetApp();
    DWORD dwStyle = pApp->GetProfileInt(_T("Window"), _T("Style"), WS_VISIBLE);
    DWORD dwIsRestored = pApp->GetProfileInt(_T("Window"), _T("Restore"), 1);
    DWORD dwWasCloseAsMaximized = pApp->GetProfileInt(_T("Window"), _T("Maximized"), 1);

    if ((dwStyle & WS_MAXIMIZE) == 0)
    {
       WINDOWPLACEMENT *pWp;
       UINT bytes;
       BOOL success = pApp->GetProfileBinary(_T("Window"), _T("Placement"), 
          (LPBYTE *) &pWp, &bytes);
       if (TRUE == success)
       {
          if (pWp->showCmd == SW_SHOWMINIMIZED)
             pWp->showCmd = SW_SHOWNORMAL;

          ASSERT(bytes == sizeof WINDOWPLACEMENT);
          SetWindowPlacement(pWp);
          delete[] pWp;
          m_bWindowPositioningFromRegistry=true;
       }
    }
    else
    {
       WINDOWPLACEMENT *pWp;
       UINT bytes;
       BOOL success = pApp->GetProfileBinary(_T("Window"), _T("PlacementMax"), 
          (LPBYTE *) &pWp, &bytes);
       if (TRUE == success)
       {
          pWp->showCmd = SW_SHOWMAXIMIZED;
          ASSERT(bytes == sizeof WINDOWPLACEMENT);
          SetWindowPlacement(pWp);
          delete[] pWp;
          m_bWindowPositioningFromRegistry=true;
       }
    }
    if(m_bWindowPositioningFromRegistry == false)
    {
       int screenx = GetSystemMetrics(SM_CXSCREEN);
       int screeny = GetSystemMetrics(SM_CYSCREEN);

       int x = screenx/2 - 480 - 8;
       int y = screeny/2 - 390 - 6; 
       int width = screenx/2 + 357 + 16 /*+ 16 */;
       int height = screeny/2 + 230 + 12 /*+ 12*/;
       WINDOWPLACEMENT wp;

       wp.flags = 1;
       wp.ptMaxPosition = CPoint(-1, -1);
       wp.ptMinPosition = CPoint(-1, -1);
       wp.showCmd = SW_SHOWNORMAL;
       wp.rcNormalPosition.left = x;
       wp.rcNormalPosition.top = y;
       wp.rcNormalPosition.right = x + width;
       wp.rcNormalPosition.bottom = y + height;

       SetWindowPlacement( &wp);
    }
    if(dwWasCloseAsMaximized == 1)
    {
      m_bCloseAsMaximized = true;
    }
    else
    {
      m_bCloseAsMaximized = false;
    }
    CString csTitle;
    csTitle.LoadString(IDS_APPLICATION_TITLE);
    SetTitle(csTitle);
    m_pRibbonBars->LoadQuickAccessControlsAssistant();
    //LoadCommandBars(_T("CommandBarsStudio"));

// Context menus are saved once SaveCommandBars is called, so we must remove
// them and then create

    pCommandBars->GetContextMenus()->RemoveAll();

    CMenu mnu;
    mnu.LoadMenu(IDR_MONITOR_WIDGET);
    pCommandBars->GetContextMenus()->Add(IDR_MONITOR_WIDGET, _T("Context Menu"), mnu.GetSubMenu(0));

    CMenu mnuPens;
    mnuPens.LoadMenu(ID_MENU_PENS);
    pCommandBars->GetContextMenus()->Add(ID_MENU_PENS, _T("Menu Pens"), mnuPens.GetSubMenu(0));

    CMenu mnuMarkers;
    mnuMarkers.LoadMenu(ID_MENU_MARKERS);
    pCommandBars->GetContextMenus()->Add(ID_MENU_MARKERS, _T("Menu Markers"), mnuMarkers.GetSubMenu(0));

    LoadPresentationBar();
    
    CMenu mnuAudioWidget;
    mnuAudioWidget.LoadMenu(IDR_AUDIO_WIDGET);
    pCommandBars->GetContextMenus()->Add(IDR_AUDIO_WIDGET, _T("Context Menu"), mnuAudioWidget.GetSubMenu(0));

    return 0;
}

//////////////////////////////////////////////////////////
/// Command and Update functions

void CMainFrame::LoadPresentationBar()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   
   CMenu mnuPresentationBar;
   mnuPresentationBar.LoadMenu(IDR_PRESENTATIONBAR);
   
   bool bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("ExitFullScreen"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Pencils"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PENCILS, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Markers"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_MARKERS, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Pointer"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_POINTER, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Navigation"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Start"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_START, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Stop"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_STOP, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Pause"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PAUSERECORDING, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("ScreenGrabbing"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_SCREENGRABBING, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("PageFocused"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PAGEFOCUSED, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("DocumentStruct"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Recordings"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_RECORDINGS, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Cut"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_CUT, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Copy"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_COPY, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Paste"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PASTE, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Undo"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_UNDO, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("OneClickCopy"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_ONECLICKCOPY, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Text"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_TEXT, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Line"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_LINE, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Polyline"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_POLYLINE, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("FreeHand"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_FREEHAND, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Ellipse"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_ELLIPSE, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Rectangle"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_RECTANGLE, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Polygon"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_POLYGON, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("InsertPage"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_INSERTPAGE, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("PreviousPage"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("NextPage"), 1) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_NEXTPAGE, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("SmallIcons"), 0) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_SMALLICONS, bShowButton?MF_CHECKED:MF_UNCHECKED);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Transparent"), 0) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_SHOWTRANSPARENT, bShowButton?MF_CHECKED:MF_UNCHECKED);

   UINT nPos = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Position"), 0) == 1)?true:false;
   mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_POSITION_BOTTOM, (nPos==OT_BOTTOMMIDDLE)?MF_CHECKED:MF_UNCHECKED);
   mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_POSITION_LEFT, (nPos==OT_MIDDLELEFT)?MF_CHECKED:MF_UNCHECKED);
   mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_POSITION_RIGHT, (nPos==OT_MIDDLERIGHT)?MF_CHECKED:MF_UNCHECKED);
   mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_POSITION_TOP, (nPos==OT_TOPMIDDLE)?MF_CHECKED:MF_UNCHECKED);

   pCommandBars->GetContextMenus()->Add(IDR_PRESENTATIONBAR, _T("Context Menu PB"), mnuPresentationBar.GetSubMenu(0));
}

void CMainFrame::OnClose()
{
   DWORD dwStyle = GetStyle();
   CWinApp *pApp = AfxGetApp();
   WINDOWPLACEMENT wp;
   BOOL success = GetWindowPlacement(&wp);
   if ((dwStyle & WS_MAXIMIZE) == 0)
   {
      if (TRUE == success)
         pApp->WriteProfileBinary(_T("Window"), _T("Placement"), 
         (LPBYTE) &wp, sizeof WINDOWPLACEMENT);
      pApp->WriteProfileInt(_T("Window"), _T("Maximized"), 0);
      pApp->WriteProfileInt(_T("Window"), _T("Restore"), 1);
   }
   else
   {
      if (TRUE == success)
         pApp->WriteProfileBinary(_T("Window"), _T("PlacementMax"), 
         (LPBYTE) &wp, sizeof WINDOWPLACEMENT);
       pApp->WriteProfileInt(_T("Window"), _T("Maximized"), 1);
       pApp->WriteProfileInt(_T("Window"), _T("Restore"), 0);
   }
   pApp->WriteProfileInt(_T("Window"), _T("Style"), dwStyle);
   bool bCloseSuccess = ((CStudioApp *)AfxGetApp())->SaveLoadedDocuments();
   
   // Save quick acces toolbars
   UINT nCurrentMode = ((CStudioApp *)AfxGetApp())->GetCurrentMode();
   if (nCurrentMode == CStudioApp::MODE_ASSISTANT) 
   {
      m_pRibbonBars->SetSaved(false);
      m_pRibbonBars->SaveQuickAccessControlsAssistant();
      SaveCommandBars(COMMAND_BARS_MODE_ASSISTANT);
   }
   else if (nCurrentMode == CStudioApp::MODE_EDITOR) 
   {
      m_pRibbonBars->SaveQuickAccessControlsEditor();
      SaveCommandBars(COMMAND_BARS_MODE_EDITOR);
   }
   if (bCloseSuccess)
      CXTPMDIFrameWnd::OnClose();
}


void CMainFrame::OnNextWindow()
{
    // Ctrl+Tab should not change the template
}


//////////////////////////////////////////////////////////
/// XTreme Toolkit functionality

void CMainFrame::OnCustomize()
{
    // Get a pointer to the command bars object.
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
        pCommands->AddCategories(IDR_MAINFRAME);

        // Use the command bar manager to initialize the 
        // customize dialog.
        pCommands->InsertAllCommandsCategory();
        pCommands->InsertBuiltInMenus(IDR_MAINFRAME);
        pCommands->InsertNewMenuCategory();

        // Dispaly the dialog.
        dlg.DoModal();
    }
}

int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{  
    BOOL bFound = FALSE;

    bFound = CreateAssistantControl(lpCreateControl);

    // bFound == TRUE if the control was found in CreateAssistantControl
    // In this case it make no sense to serach in Editor Controls
    if (!bFound)
        bFound = CreateEditorControl(lpCreateControl);
  
    return bFound;
}

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
    if (!IsWindowEnabled()) {
        ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
        return TRUE;
    }

    return CXTPMDIFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CMainFrame::OnPinRangeClick( UINT nID ,NMHDR * pNotifyStruct, LRESULT* result)
{
   CXTPRibbonControlRecentFileListExt::GetInstance()->OnPinRangeClick(nID);
   
   return TRUE;
}

void CMainFrame::OnUpdateRibbonTab(CCmdUI* pCmdUI)
{
    static UINT nLastUpdatedMode = CStudioApp::MODE_UNDEFINED;
    static UINT nLastCurrentColorScheme = 0;

    // Set correct color scheme
    UINT nCurrentColorScheme = ((CStudioApp *)AfxGetApp())->GetCurrentColorScheme();
    if  (nLastCurrentColorScheme != nCurrentColorScheme) {
        SetColorScheme(nCurrentColorScheme);
        nLastCurrentColorScheme = nCurrentColorScheme;
    }

    CXTPRibbonControlTab* pControl = DYNAMIC_DOWNCAST(CXTPRibbonControlTab, CXTPControl::FromUI(pCmdUI));
    if (!pControl)
        return;

    CXTPRibbonBar* pRibbonBar = pControl->GetRibbonBar();
    if(!pRibbonBar)
        return;
  

    // Show / Hide Menu entries
    UINT nCurrentMode = ((CStudioApp *)AfxGetApp())->GetCurrentMode();
    HRESULT hr = S_OK;
    if (nLastUpdatedMode != nCurrentMode) {
        if (nCurrentMode == CStudioApp::MODE_ASSISTANT) {
            if (SUCCEEDED(hr))
                hr = ShowEditorRibbonTabs(pRibbonBar, false);
            if (SUCCEEDED(hr))
                hr = ShowAssistantRibbonTabs(pRibbonBar, true);
            if (SUCCEEDED(hr))
                hr = ShowEditorApplicationMenu(pRibbonBar, false);
            if (SUCCEEDED(hr))
                hr = ShowAssistantApplicationMenu(pRibbonBar, true);
            CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
        }
        else if (nCurrentMode == CStudioApp::MODE_EDITOR) {
            if (SUCCEEDED(hr))
                hr = ShowAssistantRibbonTabs(pRibbonBar, false);
            if (SUCCEEDED(hr))
                hr = ShowEditorRibbonTabs(pRibbonBar, true);
            if (SUCCEEDED(hr))
                hr = ShowAssistantApplicationMenu(pRibbonBar, false);
            if (SUCCEEDED(hr))
                hr = ShowEditorApplicationMenu(pRibbonBar, true);  
       }
    }
    if (SUCCEEDED(hr))
        nLastUpdatedMode = nCurrentMode;

    // set ribbon bar depending on Assistant mode
    if (nCurrentMode == CStudioApp::MODE_ASSISTANT) 
    {
    
          CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
          if (pMainFrame)
          {
             bool bIsStartupMode = pMainFrame->IsInStartupMode();
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

#endif
          GetRibbonBars()->SetVisibleQuickAccessControl(ID_LSD_SAVE,!bIsStartupMode);
          GetRibbonBars()->SetVisibleQuickAccessControl(ID_EDIT_UNDO,!bIsStartupMode);
          GetRibbonBars()->SetVisibleQuickAccessControl(ID_EDIT_REDO,!bIsStartupMode);
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
    }
    // set ribbon bar depending on Editor mode
    else if (nCurrentMode == CStudioApp::MODE_EDITOR) 
    {
       CMainFrameE *pMainFrame = CMainFrameE::GetCurrentInstance();
   }
}

void CMainFrame::OnLrdOpen() 
{
   CString csFilter;
   csFilter.LoadString(IDS_FILEFILTER);

   CString fileName;
   FileDialog::GetOpenFilename(AfxGetApp(), fileName, GetSafeHwnd(), 
      AfxGetInstanceHandle(), csFilter, _T("LRD"), GetInitialDir());
   
   if (!fileName.IsEmpty())
   {
      AfxGetApp()->OpenDocumentFile(fileName);
   }
    
}

void CMainFrame::OnUpdateLrdOpen(CCmdUI* pCmdUI) 
{
}


bool CMainFrame::CreateAssistantStatusBar() {
    m_wndStatusBar.RemoveAll();

    if (!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))) {
        return false;
    }

    CXTPStatusBarPane* pPane;
    pPane = m_wndStatusBar.AddIndicator(0);
    if (!pPane) {
        return false;
    }
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
    m_wndStatusBar.EnableCustomization(TRUE);
    m_wndStatusBar.LoadState(COMMAND_BARS_MODE_ASSISTANT);

    CXTPToolTipContext* pToolTipContext = m_wndStatusBar.GetToolTipContext();
    pToolTipContext->SetStyle(xtpToolTipOffice2007);
    pToolTipContext->SetFont(m_wndStatusBar.GetPaintManager()->GetIconFont());

    if (pSwitchPane->GetSwitchCount() >= 3)
    {
        CXTPStatusBarSwitchPane::SWITCH *pStandardLayoutBtn = pSwitchPane->GetSwitch(0);
        CXTPStatusBarSwitchPane::SWITCH *pStandardPageFocusedLayoutBtn = pSwitchPane->GetSwitch(1);
        CXTPStatusBarSwitchPane::SWITCH *pFullScreenBtn = pSwitchPane->GetSwitch(2);
        CString csTitle, csDescription;
        if (pStandardLayoutBtn != NULL )
        {
            CRibbonBars::GetTooltipComponents(IDS_STANDARD_LAYOUT, csTitle, csDescription);
            pStandardLayoutBtn->strToolTip = csTitle;
        }
        if (pStandardPageFocusedLayoutBtn != NULL )
        {
            CRibbonBars::GetTooltipComponents(IDS_PAGE_FOCUSED_LAYOUT, csTitle, csDescription);
            pStandardPageFocusedLayoutBtn->strToolTip = csTitle;
        }
        if (pFullScreenBtn != NULL )
        {
            CRibbonBars::GetTooltipComponents(IDS_VIEW_PRESENTATION, csTitle, csDescription);
            pFullScreenBtn->strToolTip = csTitle;
        }
    }
    return true;
}
  
BOOL CMainFrame::CreateAssistantControl(LPCREATECONTROLSTRUCT lpCreateControl)
{ 
    //create System Menu start
    /*
    if (lpCreateControl->nID == ID_FILE_MRU_FILE1)
    {		
        if(m_bRecentFilelist == false) {
            CXTPRibbonControlRecentFileListExt::GetInstance()->SetRegPath(ASSISTANT_REGISTRY);
            lpCreateControl->pControl = CXTPRibbonControlRecentFileListExt::GetInstance();
            lpCreateControl->pControl->SetID(XTP_ID_RECENT_FILE_LIST_CONTROL);
            m_bRecentFilelist = true;
        }
        return TRUE;
    }
    */

    if (lpCreateControl->nID == ID_APP_EXIT || lpCreateControl->nID == ID_MENU_ASSISTANT_SETTINGS) {		
        lpCreateControl->pControl = new CXTPRibbonControlSystemPopupBarButton();

        return TRUE;
    }

    if(lpCreateControl->nID == ID_MENU_OPEN) {
        CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

        CMenu menu;
        menu.LoadMenu(ID_SUBMENU_STUDIO_OPEN);

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

    if(lpCreateControl->nID == ID_MENU_PROJECT_SETTINGS) {
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

    if(lpCreateControl->nID == ID_MENU_IMPORT) {
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

    if (lpCreateControl->nID == ID_PRESENTATION_PENCILS) {
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

    if (lpCreateControl->nID == ID_PRESENTATION_MARKERS) {
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
    if (lpCreateControl->nID == ID_CALLOUTS) {
       CXTPControlGalleryItems *m_pItemsCallouts = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_CALLOUTS);

       m_pItemsCallouts->GetImageManager()->SetMaskColor(0x000000);
       m_pItemsCallouts->GetImageManager()->SetIcons(IDB_CALLOUTS, 0, 0, CSize(32, 32));
       m_pItemsCallouts->GetImageManager()->SetMaskColor(COLORREF_NULL); // Default TopLeft Pixel.
       m_pItemsCallouts->ClipItems(TRUE);
       m_pItemsCallouts->SetItemSize(CSize(38, 38));

       // Add each item to the gallery list
       m_pItemsCallouts->AddItem(ID_CALLOUT_TOP_LEFT, 0);
       m_pItemsCallouts->AddItem(ID_CALLOUT_TOP_RIGHT, 1);
       m_pItemsCallouts->AddItem(ID_CALLOUT_BOTTOM_LEFT, 2);
       m_pItemsCallouts->AddItem(ID_CALLOUT_BOTTOM_RIGHT, 3);
       m_pItemsCallouts->AddItem(ID_CALLOUT_LEFT_BOTTOM, 4);
       m_pItemsCallouts->AddItem(ID_CALLOUT_LEFT_TOP, 5);
       m_pItemsCallouts->AddItem(ID_CALLOUT_RIGHT_BOTTOM, 6);
       m_pItemsCallouts->AddItem(ID_CALLOUT_RIGHT_TOP, 7);

       CXTPControlGallery* pControlGallery = new CXTPControlGallery();
       pControlGallery->SetControlSize(CSize(32*4 + 6*4 /*4 columns. 32 is button size, 6 is button margins*/, 32 * 2 + 6*2 /*2 lines*/));
       pControlGallery->SetItems(m_pItemsCallouts);
       pControlGallery->ShowScrollBar(FALSE);

       lpCreateControl->pControl = pControlGallery;
       return TRUE;
    }
    
    if (lpCreateControl->nID == ID_DEMOOBJECTS) {
       CXTPControlGalleryItems *m_pItemsDemoObjects = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_DEMOOBJECTS);

       m_pItemsDemoObjects->GetImageManager()->SetMaskColor(0x000000);
       m_pItemsDemoObjects->GetImageManager()->SetIcons(IDB_DEMOOBJECTS, 0, 0, CSize(32, 32));
       m_pItemsDemoObjects->GetImageManager()->SetMaskColor(COLORREF_NULL); // Default TopLeft Pixel.
       m_pItemsDemoObjects->ClipItems(TRUE);
       m_pItemsDemoObjects->SetItemSize(CSize(38, 38));

       // Add each item to the gallery list
       m_pItemsDemoObjects->AddItem(ID_DEMOOBJ_TOP_LEFT, 0);
       m_pItemsDemoObjects->AddItem(ID_DEMOOBJ_TOP_RIGHT, 1);
       m_pItemsDemoObjects->AddItem(ID_DEMOOBJ_BOTTOM_LEFT, 2);
       m_pItemsDemoObjects->AddItem(ID_DEMOOBJ_BOTTOM_RIGHT, 3);
       m_pItemsDemoObjects->AddItem(ID_DEMOOBJ_LEFT_BOTTOM, 4);
       m_pItemsDemoObjects->AddItem(ID_DEMOOBJ_LEFT_TOP, 5);
       m_pItemsDemoObjects->AddItem(ID_DEMOOBJ_RIGHT_BOTTOM, 6);
       m_pItemsDemoObjects->AddItem(ID_DEMOOBJ_RIGHT_TOP, 7);

       CXTPControlGallery* pControlGallery = new CXTPControlGallery();
       pControlGallery->SetControlSize(CSize(32*4 + 6*4 /*4 columns. 32 is button size, 6 is button margins*/, 32 * 2 + 6*2 /*2 lines*/));
       pControlGallery->SetItems(m_pItemsDemoObjects);
       pControlGallery->ShowScrollBar(FALSE);

       lpCreateControl->pControl = pControlGallery;
       return TRUE;
    }

   

    return FALSE;
}
    
BOOL CMainFrame::CreateEditorControl(LPCREATECONTROLSTRUCT lpCreateControl)
{ 
    /* // This places (placed) a submenu on the export entry; now it has directly
          the id of "Export settings" and has no submenu.
    if(lpCreateControl->nID == ID_MENU_EXPORT)
    {
        CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

        CMenu menu;
        menu.LoadMenu(ID_SUBMENU_EXPORT);

        CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
        pPopupBar->SetCommandBars(GetCommandBars());

        pPopupBar->LoadMenu(menu.GetSubMenu(0));

        CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_MENU_EXPORT_LABEL, NULL, 0);
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
    */

    if(lpCreateControl->nID == ID_MENU_AUDIO)
    {
        CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

        CMenu menu;
        menu.LoadMenu(ID_SUBMENU_AUDIO);

        CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
        pPopupBar->SetCommandBars(GetCommandBars());

        pPopupBar->LoadMenu(menu.GetSubMenu(0));

        CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_MENU_AUDIO_LABEL, NULL, 0);
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
    if(lpCreateControl->nID == ID_MENU_VIDEO)
    {
        CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

        CMenu menu;
        menu.LoadMenu(ID_SUBMENU_VIDEO);

        CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
        pPopupBar->SetCommandBars(GetCommandBars());

        pPopupBar->LoadMenu(menu.GetSubMenu(0));

        CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_MENU_VIDEO_LABEL, NULL, 0);
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
    //System menu creation end

    if (lpCreateControl->bToolBar)
    {
        CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, lpCreateControl->pCommandBar);
        if (!pToolBar)
            return FALSE;

        if (lpCreateControl->nID == IDC_FONT_FAMILY && pToolBar->GetBarID() == IDR_EDIT)
        {
            CXTPControlComboBox* pFontCombo = new CXTPControlFontComboBox();
            pFontCombo->SetDropDownListStyle();

            lpCreateControl->pControl = pFontCombo;
            return TRUE;
        }

        if (lpCreateControl->nID == IDC_FONT_SIZE /*&& pToolBar->GetBarID() == IDR_EDIT*/)
        {
            CXTPControlComboBox* pComboSize = new CXTPControlSizeComboBox();
            pComboSize->SetDropDownListStyle();

            lpCreateControl->pControl = pComboSize;
            return TRUE;
        }

        if (lpCreateControl->nID == IDC_LINE_COLOR /*&& pToolBar->GetBarID() == IDR_EDIT*/)
        {
            CXTPControlPopupColor* pPopupColor = new CXTPControlPopupColor();

            CXTPPopupBar* pColorBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

            CXTPControlColorSelector *pControlColorSelector = new CXTPControlColorSelector();
            pControlColorSelector->SetBeginGroup(TRUE);
            pColorBar->GetControls()->Add(pControlColorSelector, ID_SELECTOR_LINE_COLOR_E);
            CXTPControlButtonColor *pControlButtonColor = new CXTPControlButtonColor();
            pControlButtonColor->SetBeginGroup(TRUE);
            pColorBar->GetControls()->Add(pControlButtonColor, IDS_MORE_LINE_COLORS_E);

            pPopupColor->SetCommandBar(pColorBar);

            pColorBar->SetTearOffPopup(_T("Line Color"), IDR_LINECOLOR_POPUP, 0);
            pColorBar->EnableCustomization(FALSE);

            pColorBar->InternalRelease();

            lpCreateControl->pControl = pPopupColor;
            return TRUE; 
        }

        if (lpCreateControl->nID == IDC_FILL_COLOR /*&& pToolBar->GetBarID() == IDR_EDIT*/)
        {
            CXTPControlPopupColor* pPopupColor = new CXTPControlPopupColor();

            CXTPPopupBar* pColorBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

            CXTPControlColorSelector *pControlColorSelector = new CXTPControlColorSelector();
            pControlColorSelector->SetBeginGroup(TRUE);
            pColorBar->GetControls()->Add(pControlColorSelector, ID_SELECTOR_FILL_COLOR_E);
            CXTPControlButtonColor *pControlButtonColor = new CXTPControlButtonColor();
            pControlButtonColor->SetBeginGroup(TRUE);
            pColorBar->GetControls()->Add(pControlButtonColor, IDS_MORE_FILL_COLORS_E);

            pPopupColor->SetCommandBar(pColorBar);

            pColorBar->SetTearOffPopup(_T("Fill Color"), IDR_FILLCOLOR_POPUP, 0);
            pColorBar->EnableCustomization(FALSE);

            pColorBar->InternalRelease();

            lpCreateControl->pControl = pPopupColor;
            return TRUE;
        }

        if (lpCreateControl->nID == IDC_TEXT_COLOR /*&& pToolBar->GetBarID() == IDR_EDIT*/)
        {
            CXTPControlPopupColor* pPopupColor = new CXTPControlPopupColor();

            CXTPPopupBar* pColorBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

            CXTPControlColorSelector *pControlColorSelector = new CXTPControlColorSelector();
            pControlColorSelector->SetBeginGroup(TRUE);
            pColorBar->GetControls()->Add(pControlColorSelector, ID_SELECTOR_TEXT_COLOR_E);
            CXTPControlButtonColor *pControlButtonColor = new CXTPControlButtonColor();
            pControlButtonColor->SetBeginGroup(TRUE);
            pColorBar->GetControls()->Add(pControlButtonColor, IDS_MORE_TEXT_COLORS_E);

            pPopupColor->SetCommandBar(pColorBar);

            pColorBar->SetTearOffPopup(_T("Fill Color"), IDR_TEXTCOLOR_POPUP, 0);
            pColorBar->EnableCustomization(FALSE);

            pColorBar->InternalRelease();

            lpCreateControl->pControl = pPopupColor;
            return TRUE;
        }


        if (lpCreateControl->nID == IDC_LINE_WIDTH /*&& pToolBar->GetBarID() == IDR_EDIT*/)
        {
            CXTPControlPopup *pControlPopup = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

            CXTPPopupBar* pLineWidthBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

            CControlLineWidthSelector *pControlLineWidthSelector = new CControlLineWidthSelector();
            pControlLineWidthSelector->SetBeginGroup(TRUE);
            pLineWidthBar->GetControls()->Add(pControlLineWidthSelector, ID_SELECTOR_LINE_WIDTH_E);
            pControlPopup->SetCommandBar(pLineWidthBar);

            pLineWidthBar->EnableCustomization(FALSE);
            pLineWidthBar->InternalRelease();

            lpCreateControl->pControl = pControlPopup;

            return TRUE;
        }

        if (lpCreateControl->nID == IDC_LINE_STYLE /*&& pToolBar->GetBarID() == IDR_EDIT*/)
        {
            CXTPControlPopup *pControlPopup = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

            CXTPPopupBar* pLineStyleBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

            CControlLineStyleSelector *pControlLineStyleSelector = new CControlLineStyleSelector();
            pLineStyleBar->GetControls()->Add(pControlLineStyleSelector, ID_SELECTOR_LINE_STYLE_E);
            pControlPopup->SetCommandBar(pLineStyleBar);

            pLineStyleBar->EnableCustomization(FALSE);
            pLineStyleBar->InternalRelease();

            lpCreateControl->pControl = pControlPopup;

            return TRUE;
        }
        if (lpCreateControl->nID == IDC_ARROW_STYLE /*&& pToolBar->GetBarID() == IDR_EDIT*/)
        {
            CXTPControlPopup *pControlPopup = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

            CXTPPopupBar* pArrowStyleBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

            CControlArrowStyleSelector *pControlArrowStyleSelector = new CControlArrowStyleSelector();
            pArrowStyleBar->GetControls()->Add(pControlArrowStyleSelector, ID_SELECTOR_ARROW_STYLE_E);
            pControlPopup->SetCommandBar(pArrowStyleBar);

            pArrowStyleBar->EnableCustomization(FALSE);
            pArrowStyleBar->InternalRelease();

            lpCreateControl->pControl = pControlPopup;

            return TRUE;
        }


    }
    return FALSE;	
}

HRESULT CMainFrame::ShowAssistantRibbonTabs(CXTPRibbonBar* pRibbon, bool bShow)
{
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr)) {
        CXTPRibbonTab* pTabStart = pRibbon->FindTab(ID_TAB_START);
        if(pTabStart)
            pTabStart->SetVisible(bShow);
        else
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        CXTPRibbonTab* pTabTools = pRibbon->FindTab(ID_TAB_TOOLS);
        if(pTabTools)
            pTabTools->SetVisible(bShow);
        else
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        CXTPRibbonTab* pTabView = pRibbon->FindTab(ID_TAB_VIEW);
        if(pTabView)
            pTabView->SetVisible(bShow);
        else
            hr = E_FAIL;
    }

    return hr;
}

HRESULT CMainFrame::ShowEditorRibbonTabs(CXTPRibbonBar* pRibbon, bool bShow)
{
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr)) {
        CXTPRibbonTab* pTabStart = pRibbon->FindTab(ID_TAB_START_E);
        if(pTabStart)
            pTabStart->SetVisible(bShow);
        else
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        CXTPRibbonTab* pTabTools = pRibbon->FindTab(ID_TAB_VIEW_E);
        if(pTabTools)
            pTabTools->SetVisible(bShow);
        else
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        CXTPRibbonTab* pTabView = pRibbon->FindTab(ID_TAB_INSERT);
        if(pTabView)
            pTabView->SetVisible(bShow);
        else
            hr = E_FAIL;
    }

    return hr;
}

HRESULT CMainFrame::ShowAssistantApplicationMenu(CXTPRibbonBar* pRibbon, bool bShow)
{
    HRESULT hr = S_OK;

    UINT uiAssistantMenuEntries[] = {ID_MENU_OPEN, ID_LSD_SAVE, 
                                     ID_LSD_SAVE_AS, ID_MENU_REVERT, ID_FILE_PACK,
                                     ID_MENU_IMPORT, ID_MENU_PROJECT_SETTINGS}; 

    CXTPControlPopup* pApplicationMenu = (CXTPControlPopup*)pRibbon->GetSystemButton();
    CXTPCommandBar *pMenu = pApplicationMenu->GetCommandBar();

    CXTPControls *pControls = pMenu->GetControls();
    for (int i = 0; i < pControls->GetCount(); ++i) {
        CXTPControl *pControl = pControls->GetAt(i);
        for (int j = 0; j < _countof(uiAssistantMenuEntries); ++j) {
            if (pControl->GetID() == uiAssistantMenuEntries[j]) {
                DWORD dwHideFlags = pControl->GetHideFlags();
                if (bShow) {
                    pControl->SetHideFlags(dwHideFlags & ~xtpHideGeneric);
                    pControl->SetVisible(TRUE);
                }
                else {
                    pControl->SetHideFlags(dwHideFlags | xtpHideGeneric);
                    pControl->SetVisible(FALSE);
                }
            }
        }
    }

    return hr;
}

HRESULT CMainFrame::ShowEditorApplicationMenu(CXTPRibbonBar* pRibbon, bool bShow)
{
    HRESULT hr = S_OK;

    UINT uiEditorMenuEntries[] = {ID_FILE_OPEN, ID_FILE_SAVE, 
                                  ID_FILE_SAVE_AS, ID_FILE_IMPORT, ID_FILE_EXPORT_SETTINGS,
                                  ID_MENU_AUDIO, ID_MENU_VIDEO, ID_CLOSE}; 

    CXTPControlPopup* pApplicationMenu = (CXTPControlPopup*)pRibbon->GetSystemButton();
    CXTPCommandBar *pMenu = pApplicationMenu->GetCommandBar();

    CXTPControls *pControls = pMenu->GetControls();
    for (int i = 0; i < pControls->GetCount(); ++i) {
        CXTPControl *pControl = pControls->GetAt(i);
        for (int j = 0; j < _countof(uiEditorMenuEntries); ++j) {
            if (pControl->GetID() == uiEditorMenuEntries[j]) {
                DWORD dwHideFlags = pControl->GetHideFlags();
                if (bShow) {
                    pControl->SetHideFlags(dwHideFlags & ~xtpHideGeneric);
                    pControl->SetVisible(TRUE);
                }
                else {
                    pControl->SetHideFlags(dwHideFlags | xtpHideGeneric);
                    pControl->SetVisible(FALSE);
                }
            }
        }
    }

    return hr;
}

void CMainFrame::SetColorScheme(UINT nColorScheme)
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

    CMainFrameA *pMainFrameA = CMainFrameA::GetCurrentInstance();
    if (pMainFrameA) 
        pMainFrameA->RefreshPaneDrawing();

    GetCommandBars()->GetImageManager()->RefreshAll();
    GetCommandBars()->RedrawCommandBars();
    SendMessage(WM_NCPAINT);

    RedrawWindow(0, 0, RDW_ALLCHILDREN|RDW_INVALIDATE);  
    
    CMainFrameE *pMainFrameE = CMainFrameE::GetCurrentInstance();
    if (pMainFrameE) {   
        pMainFrameE->SetColorScheme(nColorScheme);
        pMainFrameE->RefreshPaneDrawing();
        CStreamView * pStreamView = pMainFrameE->GetStreamView();
        if (pStreamView!=NULL) {
            pStreamView->ColorSchemeChanged();
        }
    }
}

void CMainFrame::HideMenu()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   if (pCommandBars != NULL)
   {         
      CXTPCommandBar* pComandBar = (CXTPCommandBar*)pCommandBars->GetMenuBar();
      if (pComandBar != NULL)
         pComandBar->SetVisible(false);  
   }
}

void CMainFrame::ShowMenu()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   if (pCommandBars != NULL)
   {         
      CXTPCommandBar* pComandBar = (CXTPCommandBar*)pCommandBars->GetMenuBar();
      if (pComandBar != NULL)
         pComandBar->SetVisible(true);  
   }
}

void CMainFrame::HideStatusBar()
{
   //m_wndStatusBar.ShowWindow(SW_HIDE);
   ShowControlBar(&m_wndStatusBar, FALSE, FALSE);
}

void CMainFrame::ShowStatusBar(){
    CAssistantDoc *pAssistantDoc = CMainFrameA::GetAssistantDocument();
    if (pAssistantDoc != NULL && pAssistantDoc->GetShowStatusBar() == true) {
        ShowControlBar(&m_wndStatusBar, TRUE, FALSE);
    }
}

CXTPRibbonBar* CMainFrame::GetRibbonBar()
{
    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = GetCommandBars();
    if (pCommandBars != NULL) {
        CXTPRibbonBar* pRibbonBar = DYNAMIC_DOWNCAST(CXTPRibbonBar, pCommandBars->GetMenuBar());
        ASSERT(pRibbonBar);
        return pRibbonBar;
    }

    return NULL;
}

void CMainFrame::UpdateEditorStatusBarAndMenu() 
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   m_pRibbonBars->SaveQuickAccessControlsAssistant();
   GetRibbonBars()->ReInitQuickAccessControlsEditor();
   CMainFrameE *pMainFrameE = CMainFrameE::GetCurrentInstance();
   pMainFrameE->CreateStatusBarForStudio(GetStatusBar());
}

void CMainFrame::UpdatePresentationIcon(UINT uiPresentationTypeSelected)
{
    // Get a pointer to the command bars object. 
    CXTPCommandBars* pCommandBars = GetCommandBars();
    if (pCommandBars != NULL) {
        CXTPRibbonGroup *pGroup =  GetRibbonBar()->FindGroup(ID_GROUP_VIEW);
        if(pGroup) {
            CXTPControl* pControl = (CXTPControl*)pGroup->FindControl(ID_VIEW_PRESENTATION);

            if (uiPresentationTypeSelected == 1) {
                UINT uiTabView[] = {-1, ID_VIEW_PRESENTATION};
                pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabView, _countof(uiTabView), CSize(32, 32));
            }
            else if (uiPresentationTypeSelected == 0) {
                UINT uiTabView[] = {-1, -1,ID_VIEW_PRESENTATION};
                pCommandBars->GetImageManager()->SetIcons(IDB_TAB_VIEW, uiTabView, _countof(uiTabView), CSize(32, 32));

            }  
            pControl->SetCaption(ID_VIEW_PRESENTATION);  
        } 

        pCommandBars->RedrawCommandBars();

    }
}

CString CMainFrame::GetInitialDir()
{
   CString csDir;
   CSecurityUtils sec;
   bool success = sec.GetLecturnityHome(csDir);

   return csDir;
}

// CMainFrame-Diagnose

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

// CMainFrame-Meldungshandler

CString CMainFrame::InitDiskSpace()
{
   m_wndStatusBar.SetPaneText(0, CMainFrameA::InitDiskSpace());
   return CMainFrameA::InitDiskSpace();
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
   InitDiskSpace();
   CXTPMDIFrameWnd::OnShowWindow(bShow, nStatus);
 
}
bool CMainFrame::IsWindowPositioningFromRegistry()
{
  return m_bWindowPositioningFromRegistry;
}
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
   CFrameWnd* pFrameWnd = GetTopLevelFrame();
   ENSURE_VALID(pFrameWnd);
   CWinApp *pApp = AfxGetApp();
   BOOL success = FALSE; 
   WINDOWPLACEMENT wp;
   WINDOWPLACEMENT *pWp;
   UINT bytes;
   UINT nItemID = (nID & 0xFFF0);
   switch (nItemID)
   {
   case SC_SIZE:break;
   case SC_MOVE: 

      success = GetWindowPlacement(&wp);
      if (TRUE == success)
         pApp->WriteProfileBinary(_T("Window"), _T("Placement"), 
         (LPBYTE) &wp, sizeof WINDOWPLACEMENT);
      break;
   case SC_MINIMIZE:break;
   case SC_MAXIMIZE:
      success = GetWindowPlacement(&wp);
      if (TRUE == success)
         pApp->WriteProfileBinary(_T("Window"), _T("PlacementMax"), 
         (LPBYTE) &wp, sizeof WINDOWPLACEMENT);
      break;
   case SC_NEXTWINDOW:break;
   case SC_PREVWINDOW:break;
   case SC_CLOSE:break;
   case SC_RESTORE:
      if(m_bCloseAsMaximized == true)
      {
         success = pApp->GetProfileBinary(_T("Window"), _T("Placement"), 
            (LPBYTE *) &pWp, &bytes);
         if (TRUE == success)
         {
            if (pWp->showCmd == SW_SHOWMINIMIZED)
               pWp->showCmd = SW_SHOWNORMAL;

            ASSERT(bytes == sizeof WINDOWPLACEMENT);
            SetWindowPlacement(pWp);
            m_bCloseAsMaximized= false;
         }
      }
      else
      {
         success = GetWindowPlacement(&wp);
         if (TRUE == success)
            pApp->WriteProfileBinary(_T("Window"), _T("Placement"), 
            (LPBYTE) &wp, sizeof WINDOWPLACEMENT);
      }
      break;
   case SC_TASKLIST:break;
     return;
   }

   CXTPMDIFrameWnd::OnSysCommand(nID,lParam);
}
void CMainFrame::OnCustomizeQuickAccess()
{
   //ShowCustomizeDialog(2);
   CString csTitle;
   csTitle.LoadString(IDD_DIALOG_CUSTOMIZE_SETTINGS);

   CAssistantSettings assistantSettings(csTitle, GetCommandBars(), this, 1, true);
   
   assistantSettings.m_psh.dwFlags |= PSH_NOAPPLYNOW;
   assistantSettings.m_psh.dwFlags &= ~PSH_HASHELP;
   
   //assistantSettings.Init(pDocument);
   
   assistantSettings.DoModal();
}
void CMainFrame::ShowCustomizeDialog(int nSelectedPage)
{
   UINT nCurrentMode = ((CStudioApp *)AfxGetApp())->GetCurrentMode();
   CXTPCommandBars* pCommandBars = GetCommandBars();
   if(pCommandBars != NULL)
   {
      CXTPCustomizeSheet cs(pCommandBars);

      CXTPRibbonCustomizeQuickAccessPage pageQuickAccess(&cs);
      cs.AddPage(&pageQuickAccess);    
      if (nCurrentMode == CStudioApp::MODE_ASSISTANT)
      {
         pageQuickAccess.AddCategories(IDR_MAINFRAME_A);
      }
      else if(nCurrentMode == CStudioApp::MODE_EDITOR)
      {
         pageQuickAccess.AddCategories(IDR_MAINFRAME_E);
      }
      else
      {
         pageQuickAccess.AddCategories(IDR_MAINFRAME);
      }
      CXTPCustomizeMenusPage pageMenus(&cs);
      cs.AddPage(&pageMenus);

      CXTPCustomizeOptionsPage pageOptions(&cs);
      cs.AddPage(&pageOptions);

      CXTPCustomizeCommandsPage* pCommands = cs.GetCommandsPage();
      if (nCurrentMode == CStudioApp::MODE_ASSISTANT)
      {
         pCommands->AddCategories(IDR_MAINFRAME_A);
      }
      else if(nCurrentMode == CStudioApp::MODE_EDITOR)
      {
         pCommands->AddCategories(IDR_MAINFRAME_E);
      }
      else
      {
         pCommands->AddCategories(IDR_MAINFRAME);
      }
      cs.SetActivePage(nSelectedPage);
      cs.DoModal();
   }
}
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	int screenx = GetSystemMetrics(SM_CXSCREEN);
    int screeny = GetSystemMetrics(SM_CYSCREEN);

	//CStri
	
    int iBorderWidth = GetSystemMetrics(SM_CXBORDER);
    int iBorderHeight = GetSystemMetrics(SM_CYBORDER);

	
    
	lpMMI->ptMaxPosition = CPoint(-2*iBorderWidth, -2*iBorderHeight);
	lpMMI->ptMaxTrackSize = lpMMI->ptMaxSize = CPoint(screenx+24*iBorderWidth, screeny+24*iBorderHeight);

	CXTPMDIFrameWnd::OnGetMinMaxInfo(lpMMI);
}
LRESULT CMainFrame::OnToolbarContextMenu(WPARAM wParam, LPARAM lParam)
{
   CXTPToolBar* pToolBar = (CXTPToolBar*)wParam;
   CXTPPopupBar* pPopup = (CXTPPopupBar*)lParam;

   CXTPOriginalControls *pCtrl = GetRibbonBar()->GetQuickAccessControls()->GetOriginalControls();
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
   int iCount = pPopup->GetControls()->GetCount();
   UINT nID = pToolBar->GetBarID();
   
   if ((pMainFrame)&&(pCtrl!=NULL)&&(iCount>7))
   {
      bool bIsStartupMode = pMainFrame->IsInStartupMode();

      CXTPControl *pCtrlUndo = pCtrl->FindControl(ID_EDIT_UNDO);
      if(pCtrlUndo!=NULL)
      {
         int indexUndo = pCtrlUndo->GetIndex();
         CXTPControl * pControlUndo = pPopup->GetControls()->GetAt(indexUndo+2);
         if(pControlUndo!=NULL)
         {
            pControlUndo->SetEnabled(!bIsStartupMode);
         }
      }
      CXTPControl *pCtrlRedo = pCtrl->FindControl(ID_EDIT_REDO);
      if(pCtrlRedo!=NULL)
      {
         int indexRedo = pCtrlRedo->GetIndex();
         CXTPControl * pControlRedo = pPopup->GetControls()->GetAt(indexRedo+2);
         if(pControlRedo!=NULL)
         {
            pControlRedo->SetEnabled(!bIsStartupMode);
         }
      }

      CXTPControl *pCtrlLsdSave = pCtrl->FindControl(ID_LSD_SAVE);
      if(pCtrlLsdSave!=NULL)
      {
         int indexLsdSave = pCtrlLsdSave->GetIndex();
         CXTPControl * pControlLsdSave = pPopup->GetControls()->GetAt(indexLsdSave+2);
         if(pControlLsdSave!=NULL)
         {
            pControlLsdSave->SetEnabled(!bIsStartupMode);
         }
      }
   }
   return TRUE;
}

void CMainFrame::OnShowImcPages(UINT nID) 
{
   // TODO: Add your command handler code here
   CString csWebPageIMC;
   csWebPageIMC.LoadString(nID);

   csWebPageIMC.LoadString(nID);
   CMainFrame::OpenWebPage(csWebPageIMC);
   csWebPageIMC.ReleaseBuffer();
}

void CMainFrame::OnShowImc() 
{
   // TODO: Add your command handler code here
   CString csWebPageIMC;
   csWebPageIMC.LoadString(IDS_SHOW_IMC_WEBPAGE);
   OpenWebPage(csWebPageIMC);
   csWebPageIMC.ReleaseBuffer();
}

void CMainFrame::OpenWebPage(CString csWebPage) 
{
   CString csWebPageIMC = csWebPage;
   //csWebPageIMC.LoadString(IDS_SHOW_IMC_WEBPAGE);
   if(csWebPage.IsEmpty()==false)
   {
      ShellExecute(NULL, _T("open"), csWebPageIMC.GetBuffer(csWebPageIMC.GetLength()),
         NULL, NULL, SW_SHOW | SW_SHOWMAXIMIZED);
      csWebPageIMC.ReleaseBuffer();
   }
}
#include "stdafx.h"
#include "lsutl32.h"
#include "WaitSplashScreen.h"

#include "MfcUtils.h"
#include "imc_lecturnity_util_ui_SplashScreen2.h"

/* REVIEW UK
 * No "magic" numbers like 550 or 300 should be used. Sizes should be determined dynamically.
 */

IMPLEMENT_DYNAMIC(CWaitSplashScreen, CWnd)

CWaitSplashScreen::CWaitSplashScreen() {
    m_bRequireOk = false;
    m_bShouldRun = false;
    m_rcSplash = CRect(0,0,0,0);
}

CWaitSplashScreen::~CWaitSplashScreen() {
}


BEGIN_MESSAGE_MAP(CWaitSplashScreen, CWnd)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()

    ON_BN_CLICKED(IDOK, OnSplashOk)
    ON_WM_TIMER()
END_MESSAGE_MAP()

// CWaitSplashScreen-Meldungshandler

void CWaitSplashScreen::CreateSplashScreen(CString csStartingState, HWND hwndParent, bool bIsAbout) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HWND hwndDesktop = ::GetDesktopWindow();
    CRect rcDesktop;
    CWnd::FromHandle(hwndDesktop)->GetWindowRect(rcDesktop);

    CRect rcSplash;
    rcSplash.left = (rcDesktop.Width()-330)/2;
    rcSplash.top = (rcDesktop.Height()-270)/2;
    rcSplash.right = rcSplash.left + 190;
    rcSplash.bottom = rcSplash.top + 80;
#undef _DEBUG
#ifdef _DEBUG
    // Move it out of the way
    rcSplash.OffsetRect((rcDesktop.Width()-547-50)/2, -(rcDesktop.Height()-296-50)/2);
#endif

    DWORD dwStyle =/*SS_BLACKFRAME |*/ /*WS_EX_LAYERED |*/WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME /*| WS_EX_NOPARENTNOTIFY*/;
    if (bIsAbout) {
        dwStyle = 0;
    }

    CreateEx(dwStyle, AfxRegisterWndClass(0,0,0,0), NULL, WS_POPUP | WS_VISIBLE, rcSplash.left, rcSplash.top, 
        rcSplash.Width(), rcSplash.Height(), hwndParent, 0);
    // WS_CAPTION instead of WS_POPUP to get a move handle; however window size is too small then


    LOGFONT logFont;
    XTPPaintManager()->GetRegularFont()->GetLogFont(&logFont);
    _tcscpy(logFont.lfFaceName, _T("Calibri")); 
    logFont.lfHeight = 14; 
    m_font.CreateFontIndirect(&logFont);

    m_stateStatic.Create(NULL, SS_LEFT | WS_CHILD | WS_VISIBLE , CRect(5,15,185,30), this, IDC_SPLASHSCREEN_STATE);
    m_stateStatic.SetFont(&m_font);

    m_wndProgressBar.Create(WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(20,50,170,65), this, IDC_SPLASHSCREEN_PROGRESS);

    m_wndProgressBar.SetRange(0,100);
    m_wndProgressBar.SetStep(10);
    m_wndProgressBar.SetRedraw();
    m_wndProgressBar.SetUseVisualStyle(); 
    m_wndProgressBar.SetTheme(xtpControlThemeOffice2007);

    //m_okButton = new CButton();
    m_okButton.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(10,10,10,10), this, IDOK);
    m_okButton.SetFont(&m_font); 

    /*  HBITMAP hBitmap = CXTPImageManagerIcon::LoadBitmapFromResource(MAKEINTRESOURCE(IDC_SPLASHSCREEN_WAIT_IMAGE), NULL);
    m_splashImage.SetBitmap(hBitmap);
    DeleteObject(hBitmap);*/

    m_stateStatic.SetWindowText(csStartingState);

    CString csOk;
    csOk.LoadString(IDS_OK);
    m_okButton.SetWindowText(csOk);

    if (!m_bRequireOk) {
        m_okButton.EnableWindow(FALSE);
        m_okButton.ShowWindow(SW_HIDE);
    }

    GetWindowRect(m_rcSplash);
}

bool CWaitSplashScreen::Show(HWND hwndParent) {  
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_bRequireOk = true;
    CreateSplashScreen(CString(), hwndParent, true);
    ShowWindow(SW_SHOW);
    SetFocus();
    return true;
}

bool CWaitSplashScreen::Show(CString& csInitialStatusText) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CreateSplashScreen(csInitialStatusText, NULL, false);   
    return true;
}

void CWaitSplashScreen::SetRequireOk(bool bOk) {
    m_bRequireOk = bOk;

    if (m_hWnd != NULL) {
        if (m_bRequireOk) {
            m_stateStatic.ShowWindow(SW_HIDE);
            m_okButton.ShowWindow(SW_SHOW);
            m_okButton.EnableWindow(TRUE);
            m_okButton.SetFont(&m_font); 
        } else {
            m_stateStatic.ShowWindow(SW_SHOW);
            m_okButton.EnableWindow(FALSE);
            m_okButton.ShowWindow(SW_HIDE);
        }
    }
}

void CWaitSplashScreen::OnTimer(UINT nIDEvent) {
    // A timer event is thrown if auto close is
    // enabled (m_bRequireOk is not true)
    KillTimer(12345);
    ShowWindow(SW_HIDE);
}

void CWaitSplashScreen::Close() {
    //SetRequireOk(true);
    if (this->m_hWnd != NULL) {
        StepIt();
        SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        StepIt();
        SetTimer(12345, 500, NULL);
        StepIt();
    }
    StepIt();
}

void CWaitSplashScreen::OnSplashOk() {
    KillTimer(12345);
    ShowWindow(SW_HIDE);
}

BOOL CWaitSplashScreen::OnEraseBkgnd(CDC* pDC) {
    CRect rect;
    GetClientRect(&rect);
    CBrush white(RGB(212, 212, 212));
    pDC->FillRect(&rect, &white);
    SetFocus();
    return TRUE;
}

HBRUSH CWaitSplashScreen::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    //HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

    // All controls should have a white background;
    // text should be drawn in transparent mode.

    pDC->SetBkMode(TRANSPARENT);
    HBRUSH hbr = ::CreateSolidBrush(/*color*/RGB(212, 212, 212));
    SetFocus();
    return hbr;
}

CRect& CWaitSplashScreen::GetWindowBounds() {
    return m_rcSplash;
}

void CWaitSplashScreen::ChangeStatusText(CString csNewText) {
    m_stateStatic.SetWindowText(csNewText);
    UpdateWindow();
    SetFocus();
}

void CWaitSplashScreen::SetPos(UINT nPos) {
    m_wndProgressBar.SetPos(nPos);
    m_wndProgressBar.UpdateWindow();
    UpdateWindow();
}

void CWaitSplashScreen::StepIt() {
    m_wndProgressBar.StepIt();
    m_wndProgressBar.UpdateWindow();
    UpdateWindow();
}
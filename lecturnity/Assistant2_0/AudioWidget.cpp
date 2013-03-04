#include "stdafx.h"
#include "MainFrm.h"
#include "AudioWidget.h"
#include "..\Studio\Resource.h"

BEGIN_MESSAGE_MAP(CAudioWidget, CWnd)
    ON_WM_CREATE()
    ON_WM_MOUSEMOVE()
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_SETCURSOR()
    ON_WM_NCHITTEST()
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
    ON_BN_CLICKED(IDB_MONITORWIDGET_CLOSE, OnButtonClose)
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CAudioWidget, CWnd)

CAudioWidget::CAudioWidget() {
    m_hBackgroundImage = NULL;
    m_pBackgroundDc = NULL;
    m_lpBitsInfo = NULL;
    m_lpBits = NULL;
    m_pImage = NULL;
    m_pImageCloseBtn = NULL;
    m_pImageGripper = NULL;
    m_pImageNapBtn = NULL;
    m_pCommandBars = NULL;
    m_ctrlToolTipsContext = NULL;

    m_nFontSize = 10;
    m_fAudioPosition = 0.00;
    m_csDuration = _T("0:00:00");
    m_csDiskSpace = _T("??MB");
    m_csPages = _T("0/0");

    m_nWidth = 0;
    m_nHeigh = 0;

    m_bLeftButtonPressed = false;
    m_ptStartDrag = CPoint(-1,-1);
    m_csAudioToolTip = _T("");
    m_csDurationToolTip.Format(IDS_MONITOR_DURATION, m_csDuration);
    m_csDiskSpaceToolTip.Format(IDS_MONITOR_DISKSPACE, m_csDiskSpace);
    m_csPagesToolTip.Format(IDS_MONITOR_PAGES, 0, 0);

    m_nInfoOffset = INFO_OFFSET;
    m_nInfoHeight = INFO_HEIGHT;
    m_nInfoWidth = INFO_WIDTH;
    m_nAudioHeight = AUDIO_HEIGHT + AUDIO_OFFSET;
    m_nAudioVideoOffset = AUDIO_OFFSET;
    m_nMargin = MARGIN;
    m_bSavePosition = false;
    m_nIdMenu = IDR_AUDIO_WIDGET;

    m_nWidth = MARGIN*2 + AUDIO_WIDTH;
    m_nHeigh = NAP_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT;
    m_nHeigh += m_nHeigh/3;
    m_nHeigh += (INFO_HEIGHT + INFO_OFFSET)*2 + INFO_HEIGHT;

    m_bShowDuration = true;
    // Show pages option is always disabled for audio widget
    m_bShowPages = false;
    m_bShowDiskSpace = true;
    m_bEnableDrag = false;
    m_bIsOnCloseBtn = false;

    m_crBorder = GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER);
    XTPCurrentSystemTheme systemTheme = XTPColorManager()->GetCurrentSystemTheme();
    switch (systemTheme) {
    case xtpSystemThemeBlue:
    case xtpSystemThemeRoyale:
    case xtpSystemThemeAero:
        m_crBorder = RGB(0, 0, 128);
        break;

    case xtpSystemThemeOlive:
        m_crBorder = RGB(63, 93, 56);
        break;

    case xtpSystemThemeSilver:
        m_crBorder = RGB(75, 75, 11);
        break;
    }
}

CAudioWidget::~CAudioWidget() {
    SAFE_DELETE(m_pImage);
    SAFE_DELETE(m_pImageCloseBtn);
    SAFE_DELETE(m_pImageGripper);
    SAFE_DELETE(m_pImageNapBtn);

    if (m_hBackgroundImage) {
        ::DeleteObject(m_hBackgroundImage);
        m_pBackgroundDc->DeleteDC();
        delete m_pBackgroundDc;
    }

    SAFE_DELETE(m_pImage);
    SAFE_DELETE(m_pImageCloseBtn);
    SAFE_DELETE(m_pImageGripper);
    SAFE_DELETE(m_pImageNapBtn);

    CMDTARGET_RELEASE(m_ctrlToolTipsContext);
}

void CAudioWidget::OnSize(UINT nType, int cx, int cy) {
    m_nWidth = cx;
    m_nHeigh = cy; 

    int ratio = m_nWidth < m_nHeigh ? m_nWidth : m_nHeigh;
    m_nFontSize = FONT_SIZE + ratio / 120;
    m_nInfoOffset = INFO_OFFSET + ratio / 120;
    m_nInfoHeight = INFO_HEIGHT + ratio / 120;
    m_nInfoWidth = INFO_WIDTH + ratio / 60;

    if (IsWindowVisible()) {
        RecalculateControlsPosition();
    }
    SavePosition();
}

void CAudioWidget::OnMove(int x, int y) {
    CWnd::OnMove(x, y);

    if (::IsWindow(m_hWnd)) {
        ::SetWindowPos( m_hWnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
}

int CAudioWidget::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CWnd::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    // if seting saved position fails then set default position
    if (UpdateWindowPlacement() == false) {
        ::SetWindowPos(GetSafeHwnd(), NULL, 100, 100, m_nWidth, m_nHeigh, SWP_NOZORDER);
    }

    int nTop = NAP_HEIGHT + AUDIO_OFFSET;
    //Video
    nTop += m_nHeigh/3;

    //Audio
    pStaticAudio.Create(_T(""),WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, ID_VIEW_AUDIO);
    nTop += AUDIO_OFFSET;
    pStaticAudio.SetWindowPos(NULL, MARGIN, nTop, AUDIO_WIDTH, AUDIO_HEIGHT, SWP_NOZORDER);

    //Duration
    
    pStaticDuration.Create(_T(""),WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, IDC_MONITOR_DURATION);
    nTop += AUDIO_HEIGHT + INFO_OFFSET;
    pStaticDuration.SetWindowPos(NULL, MARGIN, nTop, INFO_WIDTH, INFO_HEIGHT, SWP_NOZORDER);

    //Pages
    pStaticPages.Create(_T(""),WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, IDC_MONITOR_PAGES);
    nTop += INFO_HEIGHT + INFO_OFFSET;
    pStaticPages.SetWindowPos(NULL, MARGIN, nTop, INFO_WIDTH, INFO_HEIGHT, SWP_NOZORDER);

    //Disk space
    pStaticDiskSpace.Create(_T(""),WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, IDC_MONITOR_DISKSPACE);
    nTop += INFO_HEIGHT + INFO_OFFSET;
    pStaticDiskSpace.SetWindowPos(NULL, MARGIN, nTop, INFO_WIDTH, INFO_HEIGHT, SWP_NOZORDER);

    //CLose Btn
    m_pButtonClose.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0),this,IDB_MONITORWIDGET_CLOSE);
    m_pButtonClose.SetWindowPos(NULL, m_nWidth-14, 1, 12, 11, SWP_NOZORDER);

    //m_hBitmapVideo = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MONITOR_VIDEO));

    m_pImageCloseBtn = LoadImage(IDB_MONITORWIDGET_CLOSE, _T("PNG"), ::AfxGetInstanceHandle());
    m_pImageNapBtn = LoadImage(ID_MONITORWIDGET_NAP, _T("PNG"), ::AfxGetInstanceHandle());
    m_pImageGripper = LoadImage(ID_MONITORWIDGET_GRIPPER, _T("PNG"), ::AfxGetInstanceHandle());

    CreateToolTips();

    /*if (!m_bShowDuration) {
        UpdateMonitorInfo(FALSE, TRUE, TRUE);
    }
    if (!m_bShowPages) {
        UpdateMonitorInfo(FALSE, TRUE, TRUE);
    }
    if (!m_bShowDiskSpace) {
        UpdateMonitorInfo(FALSE, TRUE, TRUE);
    }*/

    m_bShowDuration = (AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowDuration"), 1) == 1);
    // Show pages option is always disabled for audio widget so consider it default false.
    m_bShowPages = (AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowPages"), 0) == 1);
    m_bShowDiskSpace = (AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowDiskSpace"), 1) == 1 );
    m_bEnableDrag = !(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioLockPosition"), 0) == 1 );

    EnableToolTips();

    RecalculateControlsPosition();

    return 0;
}

void CAudioWidget::UpdateMonitorInfo(BOOL bShowInfo1, BOOL bShowInfo2, BOOL bShowInfo3) {
    RECT rc;
    int nHeight = 0;

    ::GetWindowRect(GetSafeHwnd(), &rc);
    ScreenToClient(&rc);

    if(bShowInfo1) {
        m_nHeigh += m_nInfoHeight;
        nHeight = (rc.bottom - rc.top) + m_nInfoHeight;
        if (bShowInfo2 || bShowInfo3) {
            m_nHeigh += m_nInfoOffset;
            nHeight += m_nInfoOffset;
        }
    } else {
        m_nHeigh -= m_nInfoHeight;
        nHeight = (rc.bottom - rc.top) - m_nInfoHeight;
        if (bShowInfo2 || bShowInfo3) {
            m_nHeigh -= m_nInfoOffset;
            nHeight -= m_nInfoOffset;
        }
    }
    SetWindowPos(
        NULL
        , rc.left
        , rc.top
        , rc.right-rc.left
        , nHeight
        , SWP_NOZORDER | SWP_NOMOVE
        );
    RecalculateControlsPosition();
}

void CAudioWidget::ShowDuration(BOOL bShowDuration) {
    m_bShowDuration = bShowDuration;
    UpdateMonitorInfo(m_bShowDuration, m_bShowPages, m_bShowDiskSpace); 
    SaveDurationSettings();
}

void CAudioWidget::ShowPages(BOOL bShowPages) {
    return;
}

void CAudioWidget::ShowDiskSpace(BOOL bShowDiskSpace) {
    m_bShowDiskSpace = bShowDiskSpace;
    UpdateMonitorInfo(m_bShowDiskSpace, m_bShowDuration, m_bShowPages);
    SaveDiskSpaceSettings();
}

void CAudioWidget::LockPosition(BOOL bLockPosition) {
    m_bEnableDrag = !bLockPosition;
    SaveLockPositionSettings(bLockPosition);
}

void CAudioWidget::SaveDurationSettings() {
    AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowDuration"), m_bShowDuration);
}

void CAudioWidget::SavePagesSettings() {
    // For know Audio widget doesn't have page settings.
    ASSERT(FALSE);
    //AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowPages"), m_bShowPages);
}

void CAudioWidget::SaveDiskSpaceSettings() {
    AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowDiskSpace"), m_bShowDiskSpace);
}

void CAudioWidget::SaveLockPositionSettings(BOOL bLockPosition) {
    AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("WidgetAudioLockPosition"), bLockPosition);
}

void CAudioWidget::RecalculateControlsPosition() {
    CRect rcDuration, rcPages, rcDiskSpace, rcVideo;

    pStaticDuration.GetWindowRect(rcDuration);
    pStaticPages.GetWindowRect(rcPages);
    pStaticDiskSpace.GetWindowRect(rcDiskSpace);

    ScreenToClient(rcDuration);
    ScreenToClient(rcPages);
    ScreenToClient(rcDiskSpace);

    //Video
    int nHeight = m_nHeigh;

    if (m_bShowDuration) {
        nHeight -= m_nInfoHeight;
    }
    if (m_bShowPages) {
        nHeight -= m_nInfoHeight;
        if (m_bShowDuration) {
            nHeight -= m_nInfoOffset;
        }
    }
    if (m_bShowDiskSpace) {
        nHeight -= m_nInfoHeight;
        if(m_bShowDuration || m_bShowPages)
            nHeight -= m_nInfoOffset;
    }

    m_nAudioVideoOffset = (nHeight*2)/(NAP_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT);

    int ratio = m_nWidth < nHeight ? m_nWidth : nHeight;

    int width = m_nWidth - 2*MARGIN;//(ratio * AUDIO_WIDTH) / (WIDGET_WIDTH);
    int left = MARGIN;//(m_nWidth-width)/2;
    left = left < 5 ? 5 : left;
    m_nMargin = left;
    int height = 0;
    int top = m_nHeigh/3; //(nHeight-height)/2;

    //Audio
    int heightAudio = (ratio*AUDIO_HEIGHT)/(NAP_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT /*- m_nHeigh/3*/);

    heightAudio = heightAudio < 5 ? 5 : heightAudio;

    m_nAudioHeight = heightAudio + m_nAudioVideoOffset;
    int topAudio =  m_nHeigh/3 + m_nAudioVideoOffset;

    pStaticAudio.SetWindowPos(NULL, left, topAudio, width, heightAudio, SWP_NOZORDER);

    //Informations   
    pStaticDuration.ShowWindow(m_bShowDuration ? SW_SHOW : SW_HIDE);
    if (m_bShowDuration) {
        int nTopDuration = m_nHeigh/3 + m_nAudioHeight;  
        pStaticDuration.SetWindowPos(NULL, m_nMargin, nTopDuration, m_nInfoWidth, m_nInfoHeight, SWP_NOZORDER);
    }

    // The way to compute offset must change. Till then leave comment below m_bShowPages
    pStaticPages.ShowWindow(m_bShowPages ? SW_SHOW : SW_HIDE);
    if (m_bShowPages) {  
        rcPages.top = m_nHeigh/3 + m_nAudioHeight;
        if (m_bShowDuration) {
            rcPages.top += m_nInfoHeight + m_nInfoOffset;
        }
        pStaticPages.SetWindowPos(NULL, m_nMargin, rcPages.top, m_nInfoWidth, m_nInfoHeight, SWP_NOZORDER);
    }

    pStaticDiskSpace.ShowWindow(m_bShowDiskSpace ? SW_SHOW : SW_HIDE);
    if (m_bShowDiskSpace) {
        rcDiskSpace.top = m_nHeigh/3 + m_nAudioHeight;
        if (m_bShowDuration) {
            rcDiskSpace.top += m_nInfoHeight + m_nInfoOffset;
        }
        // The way to compute offset must change. Till then leave comment below m_bShowPages
        //if(m_bShowPages)
        //   rcDiskSpace.top += m_nInfoHeight + m_nInfoOffset;
        pStaticDiskSpace.SetWindowPos(NULL, m_nMargin, rcDiskSpace.top, m_nInfoWidth, m_nInfoHeight, SWP_NOZORDER);
    }

    m_pButtonClose.SetWindowPos(NULL, m_nWidth-14, 1, 12, 11, SWP_NOZORDER);
}

void CAudioWidget::OnButtonClose() {
    ShowWindow(SW_HIDE);
    SavePosition();
    AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("ShowAudioMonitor"), 0);
    CMainFrameA *pMainFrameA = CMainFrameA::GetCurrentInstance();
    if (pMainFrameA != NULL) {
        pMainFrameA->SetShowAudioMonitor(false);
    }
}

void CAudioWidget::SavePosition() {
    if (m_bSavePosition == false) {
        return;
    }
    WINDOWPLACEMENT wp;
    BOOL success = GetWindowPlacement(&wp);
    if (success == TRUE) {
        AfxGetApp()->WriteProfileBinary(_T("Assistant Settings"), _T("WidgetAudioPlacement"),
        (LPBYTE) &wp, sizeof WINDOWPLACEMENT);
    }
}

bool CAudioWidget::UpdateWindowPlacement() {
    WINDOWPLACEMENT *pWp;
    UINT bytes;
    BOOL success = AfxGetApp()->GetProfileBinary(_T("Assistant Settings"), _T("WidgetAudioPlacement"), (LPBYTE *) &pWp, &bytes);
    if (success == FALSE) {
        return false;
    }
    ASSERT(bytes == sizeof WINDOWPLACEMENT);
    SetWindowPlacement(pWp);
    delete[] pWp;
    pWp = NULL;
    return true;
}

void CAudioWidget::UpdateControls() {
    if (m_hBackgroundImage) {
        ::DeleteObject(m_hBackgroundImage);
        m_pBackgroundDc->DeleteDC();
        delete m_pBackgroundDc;
    }

    CRect rc;
    GetWindowRect(&rc);

    POINT ptSrc = {0, 0};
    SIZE szWin = {m_nWidth, m_nHeigh};
    BLENDFUNCTION stBlend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    m_hBackgroundImage = NULL;
    m_pBackgroundDc    = NULL;
    CDC *pDC = GetDC();
    m_pBackgroundDc = new CDC();
    m_hBackgroundImage = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), m_nWidth, m_nHeigh);
    m_pBackgroundDc->CreateCompatibleDC(pDC);
    m_pBackgroundDc->SelectObject(m_hBackgroundImage);

    CXTPOffice2007Image* pImageBackGround;
    pImageBackGround = XTPOffice2007Images()->LoadFile(_T("PUSHBUTTONTRANSPARENT"));

    CRect rcWindow(0, 0, m_nWidth, m_nHeigh);
    //GetClientRect(rcWindow);
    //ScreenToClient(rc);

    Gdiplus::Graphics graph(m_pBackgroundDc->GetSafeHdc());
    if (m_pBackgroundDc && pImageBackGround) {
        pImageBackGround->DrawImage(m_pBackgroundDc, rcWindow, pImageBackGround->GetSource(0, 1), CRect(4, 4, 4, 4), COLORREF_NULL); 
        // Set background image corners transparent
        Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 0, 0, 0)); 
        graph.FillRectangle(&blackBrush, Gdiplus::Rect(0, 0, 2, 1));
        graph.FillRectangle(&blackBrush, Gdiplus::Rect(0, 0, 1, 2));
        graph.FillRectangle(&blackBrush, Gdiplus::Rect(m_nWidth-2, 0, m_nWidth, 1));
        graph.FillRectangle(&blackBrush, Gdiplus::Rect(m_nWidth-1, 0, m_nWidth, 2));
        graph.FillRectangle(&blackBrush, Gdiplus::Rect(0, m_nHeigh-2, 1, 2));
        graph.FillRectangle(&blackBrush, Gdiplus::Rect(0, m_nHeigh-1, 2, 1));
        graph.FillRectangle(&blackBrush, Gdiplus::Rect(m_nWidth-1, m_nHeigh-2, 1, 2));
        graph.FillRectangle(&blackBrush, Gdiplus::Rect(m_nWidth-2, m_nHeigh-1, 2, 1));
    }

    // Draw naps 
    int i = 4;
    while (i<m_nWidth-16) {
        graph.DrawImage(m_pImageNapBtn, i, 4, 3, 7);
        i += 4;
    }

    if (m_bIsOnCloseBtn) {
        CRect rcCloseBtn;
        m_pButtonClose.GetWindowRect(rcCloseBtn);
        ScreenToClient(&rcCloseBtn);
        Gdiplus::LinearGradientBrush linGrBrushBlack(
            Gdiplus::Point(rcCloseBtn.left, rcCloseBtn.top), Gdiplus::Point(rcCloseBtn.right, rcCloseBtn.bottom),
            Gdiplus::Color(255, 255, 242, 200), Gdiplus::Color(255, 255, 212, 151));
        graph.FillRectangle(&linGrBrushBlack, rcCloseBtn.left, rcCloseBtn.top, rcCloseBtn.Width(), rcCloseBtn.Height());

        Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(m_crBorder), GetGValue(m_crBorder), GetBValue(m_crBorder))); 
        Gdiplus::Pen borderPen(&brush);
        graph.DrawRectangle(&borderPen, rcCloseBtn.left, rcCloseBtn.top, rcCloseBtn.Width(), rcCloseBtn.Height());
    }
    graph.DrawImage(m_pImageCloseBtn, m_nWidth-16, 0, 16, 16);

    CRect rcSrc(0, 0, 7, 6);
    graph.DrawImage(m_pImageGripper, m_nWidth - rcSrc.Width() - 2, m_nHeigh - rcSrc.Height() - 2, 7, 6);

    DrawAudioVideo(graph);
    DrawInformations(graph);


    ::UpdateLayeredWindow(GetSafeHwnd()
        , 0
        , 0
        , &szWin
        , m_pBackgroundDc->GetSafeHdc()
        , &ptSrc
        , 0
        , &stBlend
        , ULW_ALPHA
        );


    ReleaseDC(pDC);
}

void CAudioWidget::DrawAudioVideo(Gdiplus::Graphics & graphics) {
    CRect rcAudio;
    pStaticAudio.GetWindowRect(rcAudio);
    ScreenToClient(rcAudio);

    UINT nPosition = 0;
    if (m_fAudioPosition >= 0 && m_fAudioPosition <= 1.0) {
        if (m_fAudioPosition >= 0.99) {
            m_fAudioPosition = 1.0;
        }
        nPosition = (int)(m_fAudioPosition*rcAudio.Width());   
    }

    Gdiplus::LinearGradientBrush linGrBrushBlack(
        Gdiplus::Point(rcAudio.left, 0), Gdiplus::Point(rcAudio.left+rcAudio.Width(), 0),
        Gdiplus::Color(255, 0, 0, 0), Gdiplus::Color(255, 0, 0, 0));
    graphics.FillRectangle(&linGrBrushBlack, rcAudio.left, rcAudio.top, rcAudio.Width(), rcAudio.Height());

    if (nPosition > 0) {
        int nWidth = rcAudio.Width()/2;
        int nLeft = rcAudio.left + nWidth;

        Gdiplus::LinearGradientBrush linGrBrushYG(
            Gdiplus::Point(rcAudio.left, 0),
            Gdiplus::Point(nLeft, 0),     
            Gdiplus::Color(255, 107, 233, 0),
            Gdiplus::Color(255, 219, 216, 0)); 

        int nLowerPosition = nPosition >= nWidth ? 0 : nWidth - nPosition;
        graphics.FillRectangle(&linGrBrushYG, rcAudio.left, rcAudio.top, nWidth-nLowerPosition, rcAudio.Height());

        Gdiplus::Pen greenPen(Gdiplus::Color(255, 107, 233, 0), 1);
        graphics.DrawLine(&greenPen, rcAudio.left, rcAudio.top, rcAudio.left, rcAudio.top+rcAudio.Height()-1);

        if (nPosition > nWidth) {
            Gdiplus::LinearGradientBrush linGrBrushRY(
                Gdiplus::Point(nLeft, 0),
                Gdiplus::Point(rcAudio.left+rcAudio.Width(), 0),
                Gdiplus::Color(255, 219, 216, 0),
                Gdiplus::Color(255, 153, 0, 0)); 

            graphics.FillRectangle(&linGrBrushRY, nLeft+1, rcAudio.top, nPosition-nWidth-1, rcAudio.Height());

            Gdiplus::Pen yellowPen(Gdiplus::Color(255, 219, 216, 0), 1);
            graphics.DrawLine(&yellowPen, rcAudio.left+nWidth, rcAudio.top, nLeft, rcAudio.top+rcAudio.Height()-1);
        }
    }
}

void CAudioWidget::DrawInformations(Gdiplus::Graphics & graphics) {
    CRect rcDuration, rcPages, rcDiskSpace;
    pStaticDuration.GetWindowRect(rcDuration);
    pStaticPages.GetWindowRect(rcPages);
    pStaticDiskSpace.GetWindowRect(rcDiskSpace);

    ScreenToClient(rcDuration);
    ScreenToClient(rcPages);
    ScreenToClient(rcDiskSpace);

    Gdiplus::SolidBrush textBrush(Gdiplus::Color(150, 0,0,0));
    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(150, 255, 255, 255));
    Gdiplus::Font font(_T("Arial"), m_nFontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

    if (m_bShowDuration) {
        Gdiplus::PointF textOrigin(rcDuration.left, rcDuration.top);
        Gdiplus::PointF textOriginShadow(rcDuration.left, rcDuration.top+1);

        graphics.DrawString(m_csDuration, m_csDuration.GetLength(), &font, textOriginShadow, &shadowBrush);
        graphics.DrawString(m_csDuration, m_csDuration.GetLength(), &font, textOrigin, &textBrush);
    }

    // The way to compute offset must change. Till then leave comment below m_bShowPages
    //if(m_bShowPages)
    //{
    //   Gdiplus::PointF textOrigin(rcPages.left, rcPages.top);
    //   Gdiplus::PointF textOriginShadow(rcPages.left, rcPages.top+1);

    //   graphics.DrawString(m_csPages, m_csPages.GetLength(), &font, textOriginShadow, &shadowBrush);
    //   graphics.DrawString(m_csPages, m_csPages.GetLength(), &font, textOrigin, &textBrush);
    //}

    if (m_bShowDiskSpace) {
        Gdiplus::PointF textOrigin(rcDiskSpace.left, rcDiskSpace.top);
        Gdiplus::PointF textOriginShadow(rcDiskSpace.left, rcDiskSpace.top+1);

        graphics.DrawString(m_csDiskSpace, m_csDiskSpace.GetLength(), &font, textOriginShadow, &shadowBrush);
        graphics.DrawString(m_csDiskSpace, m_csDiskSpace.GetLength(), &font, textOrigin, &textBrush);
    }
}

void CAudioWidget::UpdateVideoThumbnail(const BITMAPINFO *lpBitsInfo,const void *lpBits) {
    m_lpBitsInfo = lpBitsInfo;
    m_lpBits = (void*)lpBits;
}

void CAudioWidget::SetAudioPosition(float fPosition) {
    m_fAudioPosition = fPosition;
    if (m_hWnd == NULL) {
        return;
    }
    UpdateControls();
}

void CAudioWidget::UpdateDuration(CString csDuration) {
    m_csDuration = csDuration;
    m_csDurationToolTip.Format(IDS_MONITOR_DURATION, m_csDuration);
}

void CAudioWidget::UpdateDiskSpace(CString csDiskSpace) {
    m_csDiskSpace = csDiskSpace;
    m_csDiskSpaceToolTip.Format(IDS_MONITOR_DISKSPACE, m_csDiskSpace);
}

void CAudioWidget::UpdatePages(int nPageNo, int nTotalPages) {
    ASSERT(FALSE);
    return;
}

void CAudioWidget::OnLButtonDown(UINT nFlags, CPoint point) {
    ClientToScreen(&point);

    m_ptStartDrag = point;
    SetCapture();
    m_bLeftButtonPressed = true;

    CWnd::OnLButtonDown(nFlags, point);
}

void CAudioWidget::OnLButtonUp(UINT nFlags, CPoint point){
    ReleaseCapture();
    m_bLeftButtonPressed = false;
    m_ptStartDrag.SetPoint(-1, -1);
    CWnd::OnLButtonUp(nFlags, point);
}

void CAudioWidget::OnLButtonDblClk(UINT nFlags, CPoint point) {
    CMainFrameA *pMainFrameA = CMainFrameA::GetCurrentInstance();

    if (pMainFrameA != NULL) {
        pMainFrameA->ShowAudioSettings();
    }
}

void CAudioWidget::OnMouseMove(UINT nFlags, CPoint point) {
    CRect rc;
    GetWindowRect(rc);

    ClientToScreen(&point);

    int cx = m_ptStartDrag.x - point.x;
    int cy = m_ptStartDrag.y - point.y;

    int nLeft = rc.left - cx;
    int nTop = rc.top - cy;
    int nRight = rc.right - cx;
    int nBottom = rc.bottom - cy;

    if(nLeft != rc.left || nTop != rc.top) {
        if (m_bLeftButtonPressed && m_bEnableDrag) {
            SetWindowPos(NULL, nLeft, nTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
            m_ptStartDrag = point;
            SavePosition();
        }
    }
    CWnd::OnMouseMove(nFlags, point);
}

Gdiplus::Image * CAudioWidget::LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance) {
    Gdiplus::Image * pImage = NULL;

    if (lpszType == RT_BITMAP) {
        HBITMAP hBitmap = ::LoadBitmap( hInstance, MAKEINTRESOURCE(nID) );
        pImage = (Gdiplus::Image*)Gdiplus::Bitmap::FromHBITMAP(hBitmap, 0);
        ::DeleteObject(hBitmap);
        return pImage;
    }		

    hInstance = (hInstance == NULL) ? ::AfxGetResourceHandle() : hInstance;
    HRSRC hRsrc = ::FindResource ( hInstance, MAKEINTRESOURCE(nID), lpszType); 
    ASSERT(hRsrc != NULL);

    DWORD dwSize = ::SizeofResource( hInstance, hRsrc);
    LPBYTE lpRsrc = (LPBYTE)::LoadResource( hInstance, hRsrc);
    ASSERT(lpRsrc != NULL);

    HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
    LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
    memcpy( pMem, lpRsrc, dwSize);
    IStream * pStream = NULL;
    ::CreateStreamOnHGlobal( hMem, FALSE, &pStream);

    pImage = Gdiplus::Image::FromStream(pStream);

    ::GlobalUnlock(hMem);
    pStream->Release();
    ::FreeResource(lpRsrc);

    return pImage;
}

void CAudioWidget::OnRButtonDown(UINT nFlags, CPoint point) {
    ClientToScreen(&point);

    if (m_pCommandBars) {
        m_pCommandBars->TrackPopupMenu(m_nIdMenu, TPM_RIGHTBUTTON , point.x, point.y);
    }

    CWnd::OnRButtonDown(nFlags, point);
}

void CAudioWidget::SetCommandBars(CXTPCommandBars* pCommandBars) {
    m_pCommandBars = pCommandBars;
}


BOOL CAudioWidget::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
    if (m_bEnableDrag) {
        HCURSOR		hCursor;	
        hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE);

        CPoint ptMouse;
        BOOL bResult = ::GetCursorPos(&ptMouse);

        CRect rcWidget, rcClose;
        GetWindowRect(rcWidget);
        m_pButtonClose.GetWindowRect(rcClose);

        m_bIsOnCloseBtn = (ptMouse.x>=rcClose.left &&  ptMouse.x<=rcClose.right && ptMouse.y>=rcClose.top && ptMouse.y<=rcClose.bottom);

        bool bIsOnWidgetMargin = (ptMouse.x<(rcWidget.left+3) || ptMouse.x>(rcWidget.right-2) || ptMouse.y<(rcWidget.top+3) || ptMouse.y>(rcWidget.bottom-2)); 
        bool bIsOnBottomRightMargin = (ptMouse.x>=(rcWidget.right - GetSystemMetrics(SM_CXHSCROLL))) && (ptMouse.y>=rcWidget.bottom - GetSystemMetrics(SM_CYVSCROLL));

        if (hCursor != NULL && !m_bIsOnCloseBtn && !bIsOnWidgetMargin && !bIsOnBottomRightMargin) {
            ::SetCursor(hCursor);
            return TRUE;
        }
    } else {
        HCURSOR		hCursor;	
        hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        if (hCursor != NULL) {
            ::SetCursor(hCursor);
            return TRUE;
        }
    }
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CAudioWidget::OnNcHitTest(CPoint point) {
    UINT ht = CWnd::OnNcHitTest(point);
    if (ht == HTCLIENT) {
        CRect rc;
        GetWindowRect(rc);
        rc.left = rc.right - GetSystemMetrics(SM_CXHSCROLL);
        rc.top = rc.bottom - GetSystemMetrics(SM_CYVSCROLL);
        if (rc.PtInRect(point)) {
            ht = HTBOTTOMRIGHT;
        } else {
            ht = HTCLIENT;
        }
    }
    return ht;
}


void CAudioWidget::SetAudioVideoTooltips(CString csAudioTooltip, CString csVideoTooltip) {
    m_csAudioToolTip = csAudioTooltip;
}

BOOL CAudioWidget::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult) {
    ASSERT(pNMHDR->code == TTN_NEEDTEXT);

    TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

    if (!(pTTT->uFlags & TTF_IDISHWND)) {
        return FALSE;
    }

    UINT_PTR hWnd = pNMHDR->idFrom;
    // idFrom is actually the HWND of the tool
    UINT nID = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);

    if (ID_VIEW_AUDIO == nID) {
        pTTT->lpszText = (LPWSTR)(LPCTSTR) m_csAudioToolTip;
    } else if (IDC_MONITOR_DURATION == nID/* && m_bShowDuration*/) {
        pTTT->lpszText = (LPWSTR)(LPCTSTR) m_csDurationToolTip;
    } else if (IDC_MONITOR_PAGES == nID /*&& m_bShowPages*/) {
        pTTT->lpszText = (LPWSTR)(LPCTSTR) m_csPagesToolTip;
    } else if (IDC_MONITOR_DISKSPACE == nID /*&& m_bShowDiskSpace*/) {
        pTTT->lpszText = (LPWSTR)(LPCTSTR) m_csDiskSpaceToolTip;
    }

    pTTT->hinst = AfxGetInstanceHandle();

    *pResult = 0;
    // bring the tooltip window above other popup windows
    ::SetWindowPos(pNMHDR->hwndFrom,  HWND_TOP, 0, 0, 0, 0,
        SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
    return TRUE;    // message was handled
}

INT_PTR CAudioWidget::OnToolHitTest(CPoint point, TOOLINFO* pTI) const {
    HWND hWndChild = NULL;
    // find child window which hits the point
    // (don't use WindowFromPoint, because it ignores disabled windows)
    // see _AfxChildWindowFromPoint(m_hWnd, point);
    ::ClientToScreen(m_hWnd, &point);
    HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
    for (; hChild != NULL; hChild = ::GetWindow(hChild, GW_HWNDNEXT)) {
        if ((UINT)(WORD)::GetDlgCtrlID(hChild) != (WORD)-1 &&
            (::GetWindowLong(hChild, GWL_STYLE) & WS_VISIBLE)) {
            // see if point hits the child window
            CRect rect;
            ::GetWindowRect(hChild, rect);
            if (rect.PtInRect(point)) {
                hWndChild = hChild;
                break;
            }
        }
    }

    if (hWndChild != NULL) {
        // return positive hit if control ID isn't -1
        INT_PTR nHit = (UINT)(WORD)::GetDlgCtrlID(hWndChild);

        // hits against child windows always center the tip
        if (pTI != NULL && pTI->cbSize >= sizeof(XTP_TOOLTIP_TOOLINFO)) {
            // setup the TOOLINFO structure
            pTI->hwnd = m_hWnd;
            pTI->uId = (UINT_PTR)hWndChild;
            pTI->uFlags |= TTF_IDISHWND /*| TTF_TRANSPARENT | TTF_TRACK | TTF_SUBCLASS*/;
            pTI->lpszText = LPSTR_TEXTCALLBACK;

            // set TTF_NOTBUTTON and TTF_CENTERTIP if it isn't a button
            if (!(::SendMessage(hWndChild, WM_GETDLGCODE, 0, 0) & DLGC_BUTTON)) {
                pTI->uFlags |= TTF_NOTBUTTON|TTF_CENTERTIP;
            }
        }
        return nHit;
    }
    return -1;  // not found
}

void CAudioWidget::CreateToolTips() {
    // Set tooltip context
    if (m_ctrlToolTipsContext !=NULL) {
        ASSERT(m_ctrlToolTipsContext);
        return;
    }
    m_ctrlToolTipsContext = new CXTPWidgetToolTipContext();
    m_ctrlToolTipsContext->SetStyle(xtpToolTipOffice2007);
    m_ctrlToolTipsContext->SetMaxTipWidth(200);
    m_ctrlToolTipsContext->SetMargin(CRect(2, 2, 2, 2));  
    m_ctrlToolTipsContext->SetDelayTime(TTDT_INITIAL, 900);
}

void CAudioWidget::OnGetMinMaxInfo(MINMAXINFO FAR* pMinMaxInfo) {
    // Set the Minimum Track Size
    pMinMaxInfo->ptMinTrackSize.x = WIDGET_WIDTH;
    pMinMaxInfo->ptMinTrackSize.y = NAP_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT;

    if (m_bShowDuration) {
        pMinMaxInfo->ptMinTrackSize.y += INFO_HEIGHT;
    }
    if (m_bShowPages) {
        pMinMaxInfo->ptMinTrackSize.y += INFO_HEIGHT;
        if (m_bShowDuration) {
            pMinMaxInfo->ptMinTrackSize.y += INFO_OFFSET;
        }
    }
    if (m_bShowDiskSpace) {
        pMinMaxInfo->ptMinTrackSize.y += INFO_HEIGHT;
        if (m_bShowDuration || m_bShowPages) {
            pMinMaxInfo->ptMinTrackSize.y += INFO_OFFSET;
        }
    }
}

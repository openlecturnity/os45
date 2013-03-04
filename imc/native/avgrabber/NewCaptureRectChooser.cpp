// CaptureRectChooser.cpp: Implementierungsdatei
//

#include "global.h"
#include "NewCaptureRectChooser.h"
#include "SgSelectionPreparationBar.h"

#include "resource.h"

// DrawSdk
#include "objects.h"

#include "MfcUtils.h"   // lsutl32
#include "lutils.h"


// CCaptureRectChooser

IMPLEMENT_DYNAMIC(CNewCaptureRectChooser, CWnd)

BEGIN_MESSAGE_MAP(CNewCaptureRectChooser, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_ERASEBKGND()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()


CNewCaptureRectChooser::CNewCaptureRectChooser(AvGrabber *pAvGrabber,
                                               CRect &rcLastSelection, bool bRecordFullScreen,
                                               SGSettingsListener *pSettingsListener, 
                                               SGSelectAreaListener *pSelectAreaListener)
{
    m_pAvGrabber = pAvGrabber;

    m_nActiveMode = UNDEFINED_MODE;

    m_bRecordFullScreen = bRecordFullScreen;

    m_bLeftButtonPressed = FALSE;
    if (rcLastSelection.IsRectEmpty())
        m_rcLastCustom.SetRect(0, 0, 640, 480);
    else
        m_rcLastCustom = rcLastSelection;

    if (m_bRecordFullScreen)
        MfcUtils::GetDesktopRectangle(m_rcSelection);
    else
        m_rcSelection = m_rcLastCustom;

    m_rcCurrentWindow.SetRectEmpty();
    m_rcCurrentWindow.SetRectEmpty();

    m_hBackgroundImage = NULL;
    m_pBackgroundDc = NULL;

    m_bSelectionFinished = false;

    m_hSgSelectionCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_SG_SELECTION);

    m_pPreparationBar = NULL;
    m_bShowPresentationBar = true;

    m_pSelectionRectangle = NULL;

    m_pSettingsListener = pSettingsListener;
    m_pSelectAreaListener = pSelectAreaListener;
    m_bIsPreparationBarSticky = true;
    m_bFromCreate = false;

}

CNewCaptureRectChooser::~CNewCaptureRectChooser()
{
    if (m_hBackgroundImage)
    {
        ::DeleteObject(m_hBackgroundImage);
        m_pBackgroundDc->DeleteDC();
        delete m_pBackgroundDc;
    }

    if (m_pPreparationBar) {
        if (m_pPreparationBar->m_hWnd != NULL)
            m_pPreparationBar->DestroyWindow();
        delete m_pPreparationBar;
        m_pPreparationBar = NULL;
    }

    if (m_pSelectionRectangle) {
        if (m_pSelectionRectangle->m_hWnd != NULL)
            m_pSelectionRectangle->DestroyWindow();
        delete m_pSelectionRectangle;
        m_pSelectionRectangle = NULL;
    }

    if (m_hSgSelectionCursor)
        DestroyCursor(m_hSgSelectionCursor);

    if (!m_arWindows.IsEmpty())
        m_arWindows.RemoveAll();

}


// CCaptureRectChooser-Meldungshandler

int CNewCaptureRectChooser::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (m_pSelectionRectangle == NULL) {
        CRect rcDesktop;
        MfcUtils::GetDesktopRectangle(rcDesktop);
        m_pSelectionRectangle = new CSgSelectionRectangle((SgSelectionRectangleListener *)this);
        m_pSelectionRectangle->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 
            AfxRegisterWndClass(NULL,0,0,0), NULL, 
            WS_POPUP | WS_VISIBLE, rcDesktop, this, 0);
        m_pSelectionRectangle->SetLayeredWindowAttributes(RGB(0, 255, 0), 1, LWA_ALPHA | LWA_COLORKEY);
    }

    if (m_pPreparationBar == NULL) {
        m_pPreparationBar = 
            new CSgSelectionPreparationBar(m_pAvGrabber,
            (SGSelectionListener *)this,
            m_pSettingsListener,
            m_pSelectAreaListener);
        m_pPreparationBar->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST| WS_EX_TOOLWINDOW, 
            AfxRegisterWndClass(NULL,0,0,0), NULL, 
            WS_POPUP | WS_VISIBLE, CRect(0, 0, 210, 400), this, 0);
        m_pPreparationBar->SetLayeredWindowAttributes(RGB(255, 255, 254), (255 * 90) / 100, LWA_ALPHA | LWA_COLORKEY);
    }

    m_bFromCreate = true;
    ChangeActiveMode(PREPARATION_MODE);
    m_bFromCreate = false;

    TRACE("OnCreate");

    return 0;
}

void CNewCaptureRectChooser::OnPaint()
{
    CPaintDC dc(this); 

    CRect rcWindow;
    GetClientRect(&rcWindow);

    if (m_nActiveMode == PREPARATION_MODE) {
        CRect rcSelection;
        rcSelection = m_rcSelection;
        rcSelection.NormalizeRect();
        DrawSelectionRect(&dc, rcWindow, rcSelection);
    }
    else if (m_nActiveMode == SELECTION_MODE) {
        CRect rcSelection;
        if (m_bLeftButtonPressed)
            rcSelection = m_rcSelection;
        else 
            rcSelection = m_rcCurrentWindow;
        rcSelection.NormalizeRect();

        CXTMemDC memDC(&dc, rcWindow);
        memDC.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), m_pBackgroundDc, 0, 0, SRCCOPY);
        DrawNormal(&memDC, rcWindow, rcSelection);
        DrawCrossLines(&memDC);
        DrawCoordinates(&memDC, rcWindow, rcSelection);
    }

}

void CNewCaptureRectChooser::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_nActiveMode == SELECTION_MODE) {
        if (m_bLeftButtonPressed) {
            m_rcSelection.right = point.x;
            m_rcSelection.bottom = point.y;

            m_rcLastCustom = m_rcSelection;
        }
        else {
            CWnd *pActualWnd = GetActualWindow(point);

            CRect rcWindow;
            if (pActualWnd != NULL)
                pActualWnd->GetWindowRect(&rcWindow);
            else
                rcWindow.SetRectEmpty();

            if (rcWindow.left < 0)
                rcWindow.left = 0;
            if (rcWindow.top < 0)
                rcWindow.top = 0;

            CRect rcDesktop;
            MfcUtils::GetDesktopRectangle(rcDesktop);
            if (rcWindow.right > rcDesktop.right)
                rcWindow.right = rcDesktop.right;
            if (rcWindow.bottom > rcDesktop.bottom)
                rcWindow.bottom = rcDesktop.bottom;

            m_rcCurrentWindow = rcWindow;
        }
        RedrawWindow();
    }

    CWnd::OnMouseMove(nFlags, point);
}

void CNewCaptureRectChooser::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_nActiveMode == SELECTION_MODE) {
        m_rcSelection.left = point.x;
        m_rcSelection.top = point.y;
        m_rcSelection.right = point.x;
        m_rcSelection.bottom = point.y;
        m_bLeftButtonPressed = TRUE;
        SetCapture();
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void CNewCaptureRectChooser::OnLButtonUp(UINT nFlags, CPoint point)
{

    if (m_nActiveMode == SELECTION_MODE) {
        ReleaseCapture();
        m_rcSelection.NormalizeRect();
        if (m_rcSelection.IsRectEmpty()) {
            m_rcSelection = m_rcCurrentWindow;
            m_rcLastCustom = m_rcSelection;
        }

        m_bSelectionFinished = true;

        ChangeActiveMode(PREPARATION_MODE);
        m_bLeftButtonPressed = FALSE;
    }


    CWnd::OnLButtonUp(nFlags, point);
}

BOOL CNewCaptureRectChooser::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}


void CNewCaptureRectChooser::StartSelection()
{  
    ChangeActiveMode(SELECTION_MODE);
}

CRect &CNewCaptureRectChooser::GetSelectionRect()
{
    return m_rcSelection;
}

void CNewCaptureRectChooser::ChangeSelectionRectangle(CRect &rcSelection, bool bUpdateControls)
{
    m_rcSelection = rcSelection;
    RedrawWindow();
    if (m_pSelectionRectangle)
        m_pSelectionRectangle->RedrawWindow();

    RepositionPreparationBar();
    if (bUpdateControls) {
        m_pPreparationBar->UpdateSelectionRectangle(m_rcSelection);
        m_pPreparationBar->UpdateControls(m_rcSelection);
    }

}

void CNewCaptureRectChooser::SetShowPresentationBar(bool bShow)
{
    m_bShowPresentationBar = bShow;
}

bool CNewCaptureRectChooser::GetShowPresentationBar()
{
    return m_bShowPresentationBar;
}

void CNewCaptureRectChooser::SetStickyPreparationBar(bool bSticky)
{
    m_bIsPreparationBarSticky = bSticky;
}

void CNewCaptureRectChooser::SetRecordFullScreen(bool bRecordFullScreen)
{
    m_bRecordFullScreen = bRecordFullScreen;
}

void CNewCaptureRectChooser::GetAllWindows()
{
    if (!m_arWindows.IsEmpty())
        m_arWindows.RemoveAll();

    HWND hWnd = ::GetTopWindow(NULL);

    CArray<CWnd *, CWnd *> arWindows;
    while (hWnd != NULL)
    {
        HWND owner = ::GetWindow(hWnd, GW_OWNER);

        if (owner == NULL && hWnd != GetSafeHwnd())
        {
            CWnd *pWnd = FromHandle(hWnd);
            DWORD dwStyle = pWnd->GetStyle();
            if (pWnd->IsWindowVisible() && 
                ((dwStyle & WS_CAPTION) > 0) && 
                ((dwStyle & WS_VISIBLE) > 0)) {
                    m_arWindows.Add(hWnd);
            }
        }

        hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
    }
}

CWnd *CNewCaptureRectChooser::GetActualWindow(CPoint &ptMouse)
{
    HWND hWnd = ::GetTopWindow(NULL);

    CRect rcWindow;
    for (int i = 0; i < m_arWindows.GetSize(); ++i) {
        HWND hWnd = m_arWindows[i];
        CWnd *pWnd = FromHandle(hWnd);
        if (pWnd != NULL) {
            pWnd->GetWindowRect(&rcWindow);
            if (rcWindow.PtInRect(ptMouse)) 
                return pWnd;
        }
    }

    return GetDesktopWindow();
}

void CNewCaptureRectChooser::DrawNormal(CDC *pDC, CRect &rcWindow, CRect &rcSelection)
{  
    Gdiplus::Graphics graphics(pDC->GetSafeHdc());

    Gdiplus::SolidBrush greyBrush(Gdiplus::Color(/*64*/76, 0, 0, 0));

    Gdiplus::Rect gdipRcWindow(rcWindow.left, rcWindow.top, 
        rcWindow.Width(), rcWindow.Height());
    Gdiplus::Rect gdipRcSelection(rcSelection.left, rcSelection.top, 
        rcSelection.Width(), rcSelection.Height());
    Gdiplus::Rect gdipRcExtendedSelection(gdipRcSelection.X - 10, gdipRcSelection.Y - 10, 
        gdipRcSelection.Width + 20, gdipRcSelection.Height + 20);

    Gdiplus::Region gdipRgnDrag(gdipRcExtendedSelection);
    gdipRgnDrag.Exclude(gdipRcSelection);

    Gdiplus::Color clrHBr(255, 235, 235, 235);
    Gdiplus::Color clrHBrBack(255, 179, 179, 179);
    Gdiplus::HatchBrush dragBrush(Gdiplus::HatchStyle40Percent, clrHBr, clrHBrBack);

    graphics.FillRegion(&dragBrush, &gdipRgnDrag);

    Gdiplus::Region gdipRgnOutside(gdipRcWindow);
    //gdipRgnOutside.Exclude(gdipRcSelection);
    gdipRgnOutside.Exclude(gdipRcExtendedSelection);

    graphics.FillRegion(&greyBrush, &gdipRgnOutside);
    // graphics.FillRegion(&dragBrush, &gdipRgnDrag);


    if (!m_bLeftButtonPressed) {
        Gdiplus::Pen greenPen(Gdiplus::Color(255, 255, 179/*0, 255*/, 179), 6);
        gdipRcSelection.Inflate(-6, -6);
        //greenPen.SetAlignment(Gdiplus::PenAlignmentInset);
        greenPen.SetLineJoin(Gdiplus::LineJoinRound);
        greenPen.SetEndCap(Gdiplus::LineCapRound);
        greenPen.SetStartCap(Gdiplus::LineCapRound);
        /*
        graphics.DrawLine(&greenPen, gdipRcSelection.GetLeft(), gdipRcSelection.GetTop(), 
        gdipRcSelection.GetRight(), gdipRcSelection.GetTop());
        graphics.DrawLine(&greenPen, gdipRcSelection.GetRight(), gdipRcSelection.GetTop(), 
        gdipRcSelection.GetRight(), gdipRcSelection.GetBottom());
        graphics.DrawLine(&greenPen, gdipRcSelection.GetRight(), gdipRcSelection.GetBottom(), 
        gdipRcSelection.GetLeft(), gdipRcSelection.GetBottom());
        graphics.DrawLine(&greenPen, gdipRcSelection.GetLeft(), gdipRcSelection.GetBottom(), 
        gdipRcSelection.GetLeft(), gdipRcSelection.GetTop());
        */
        graphics.DrawRectangle(&greenPen, gdipRcSelection);
    }
    else {
        Gdiplus::Pen greenPen(Gdiplus::Color(255, 255, 179/*0, 255*/, 179), 2);
        greenPen.SetAlignment(Gdiplus::PenAlignmentInset);
        greenPen.SetDashStyle(Gdiplus::DashStyleDash);
        graphics.DrawRectangle(&greenPen, gdipRcSelection);
    }
}

void CNewCaptureRectChooser::DrawCrossLines(CDC *pDC)
{
    Gdiplus::Graphics graphics(pDC->GetSafeHdc());

    Gdiplus::Pen redPen(Gdiplus::Color(255, 255, 179, 179), 1);

    CRect rcWindow;
    GetClientRect(&rcWindow);

    CPoint ptCursor;
    GetCursorPos(&ptCursor);

    graphics.DrawLine(&redPen, 0, ptCursor.y, ptCursor.x-15, ptCursor.y);
    graphics.DrawLine(&redPen, ptCursor.x+15, ptCursor.y, rcWindow.Width(), ptCursor.y);
    graphics.DrawLine(&redPen, ptCursor.x, 0, ptCursor.x, ptCursor.y-15);
    graphics.DrawLine(&redPen, ptCursor.x, ptCursor.y+15, ptCursor.x, rcWindow.Height());
}

void CNewCaptureRectChooser::DrawCoordinates(CDC *pDC, CRect &rcWindow, CRect &rcSelection)
{
    if (m_bLeftButtonPressed) {
        Gdiplus::Graphics graphics(pDC->GetSafeHdc());

        Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 0, 0, 0));

        CRect rcWindow;
        GetClientRect(&rcWindow);

        CPoint ptCursor;
        GetCursorPos(&ptCursor);

        CString csCoordinates;
        csCoordinates.Format(_T("%d x %d"), ptCursor.x, ptCursor.y);

        LOGFONT logFont;
        XTPPaintManager()->GetRegularFont()->GetLogFont(&logFont);
        Gdiplus::Font font(pDC->GetSafeHdc(), &logFont);
        int iTextWidth = 
            DrawSdk::Text::GetTextWidth(csCoordinates, csCoordinates.GetLength(), &logFont) + 5;
        int iTextHeight = 
            DrawSdk::Text::GetTextHeight(csCoordinates, csCoordinates.GetLength(), &logFont) + 2;

        Gdiplus::PointF ptText;
        if (rcSelection.Width() > iTextWidth && rcSelection.Height() > iTextHeight) {
            ptText.X = rcSelection.right - iTextWidth;
            ptText.Y = rcSelection.bottom - iTextHeight;
            graphics.DrawString(csCoordinates, csCoordinates.GetLength(), &font, ptText, &blackBrush);
        }
    }
}

void CNewCaptureRectChooser::DrawSelectionRect(CDC *pDC, CRect &rcWindow, CRect &rcSelection)
{
    Gdiplus::Graphics graphics(pDC->GetSafeHdc());

    Gdiplus::SolidBrush greyBrush(Gdiplus::Color(255, 0, 0, 0));

    Gdiplus::Rect gdipRcWindow(rcWindow.left, rcWindow.top, 
        rcWindow.Width(), rcWindow.Height());

    // Selection rectangle
    Gdiplus::Rect gdipRcSelection(rcSelection.left, rcSelection.top, 
        rcSelection.Width(), rcSelection.Height());
    Gdiplus::Rect gdipRcExtendedSelection(gdipRcSelection.X - 10, gdipRcSelection.Y - 10, 
        gdipRcSelection.Width + 20, gdipRcSelection.Height + 20);

    // Move border
    Gdiplus::Region gdipRgnDrag(gdipRcExtendedSelection);
    gdipRgnDrag.Exclude(gdipRcSelection);

    // Fill the selection rect with transparent color
    Gdiplus::SolidBrush transparentBrush(Gdiplus::Color(255, 255, 0, 0));
    graphics.FillRectangle(&transparentBrush, gdipRcExtendedSelection);

    // Fill the region outside the extended selection rect with grey
    Gdiplus::Region gdipRgnOutside(gdipRcWindow);
    gdipRgnOutside.Exclude(gdipRcExtendedSelection);
    graphics.FillRegion(&greyBrush, &gdipRgnOutside);


    // draw red selection rectangle
    Gdiplus::Pen redPen(Gdiplus::Color(255, 255, 0, 0), 2);
    redPen.SetAlignment(Gdiplus::PenAlignmentInset);
    redPen.SetDashStyle(Gdiplus::DashStyleDash);
    graphics.DrawRectangle(&redPen, gdipRcSelection);

    // draw move border
    Gdiplus::Color clrHBr(255, 188, 188, 188);
    Gdiplus::HatchBrush dragBrush(Gdiplus::HatchStyle40Percent, clrHBr);
    graphics.FillRegion(&dragBrush, &gdipRgnDrag);

    // Draw small modify rectangles and ovals
    int iOffset = 10;

    Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
    Gdiplus::Pen blackPen(Gdiplus::Color(187, 0, 0, 0));

    Gdiplus::Rect gdipRcCircle;
    Gdiplus::Rect gdipRcSquare;
    if(gdipRcSelection.Width > 2.5 * iOffset && gdipRcSelection.Height> 2.5 * iOffset)
    {
        gdipRcCircle.X = gdipRcSelection.X - iOffset / 2 + 1;
        gdipRcCircle.Y = gdipRcSelection.Y - iOffset / 2 + 1;
        gdipRcCircle.Width = iOffset; 
        gdipRcCircle.Height = iOffset;
        graphics.FillEllipse(&whiteBrush, gdipRcCircle);
        graphics.DrawEllipse(&blackPen, gdipRcCircle);

        gdipRcCircle.X = gdipRcSelection.X + gdipRcSelection.Width - iOffset / 2 - 1;
        gdipRcCircle.Y = gdipRcSelection.Y - iOffset / 2 + 1;
        gdipRcCircle.Width = iOffset;
        gdipRcCircle.Height = iOffset;
        graphics.FillEllipse(&whiteBrush, gdipRcCircle);
        graphics.DrawEllipse(&blackPen, gdipRcCircle);

        gdipRcCircle.X = gdipRcSelection.X + gdipRcSelection.Width - iOffset / 2 - 1;
        gdipRcCircle.Y = gdipRcSelection.Y + gdipRcSelection.Height - iOffset / 2 - 1;
        gdipRcCircle.Width = iOffset;
        gdipRcCircle.Height = iOffset;
        graphics.FillEllipse(&whiteBrush, gdipRcCircle);
        graphics.DrawEllipse(&blackPen, gdipRcCircle);

        gdipRcCircle.X = gdipRcSelection.X - iOffset / 2 + 1;
        gdipRcCircle.Y = gdipRcSelection.Y + gdipRcSelection.Height - iOffset / 2 - 1;
        gdipRcCircle.Width = iOffset;
        gdipRcCircle.Height = iOffset;
        graphics.FillEllipse(&whiteBrush, gdipRcCircle);
        graphics.DrawEllipse(&blackPen, gdipRcCircle);
    }

    iOffset -=2;

    if(gdipRcSelection.Width > 4 * iOffset && gdipRcSelection.Height > 4 * iOffset)
    {
        gdipRcSquare.X = gdipRcSelection.X + (gdipRcSelection.Width - iOffset) / 2;
        gdipRcSquare.Y = gdipRcSelection.Y - iOffset / 2;
        gdipRcSquare.Width = iOffset;
        gdipRcSquare.Height = iOffset;
        graphics.FillRectangle(&whiteBrush, gdipRcSquare);
        graphics.DrawRectangle(&blackPen, gdipRcSquare);

        gdipRcSquare.X = gdipRcSelection.X + (gdipRcSelection.Width - iOffset) / 2;
        gdipRcSquare.Y = gdipRcSelection.Y + gdipRcSelection.Height - iOffset / 2;
        gdipRcSquare.Width = iOffset;
        gdipRcSquare.Height = iOffset;
        graphics.FillRectangle(&whiteBrush, gdipRcSquare);
        graphics.DrawRectangle(&blackPen, gdipRcSquare);
    }

    if(gdipRcSelection.Height > 4 * iOffset && gdipRcSelection.Width > 3 * iOffset)
    {
        gdipRcSquare.X = gdipRcSelection.X + gdipRcSelection.Width - iOffset / 2;
        gdipRcSquare.Y = gdipRcSelection.Y + (gdipRcSelection.Height - iOffset) /2;
        gdipRcSquare.Width = iOffset;
        gdipRcSquare.Height = iOffset;
        graphics.FillRectangle(&whiteBrush, gdipRcSquare);
        graphics.DrawRectangle(&blackPen, gdipRcSquare);

        gdipRcSquare.X = gdipRcSelection.X - iOffset / 2;
        gdipRcSquare.Y = gdipRcSelection.Y + (gdipRcSelection.Height - iOffset) /2;
        gdipRcSquare.Width = iOffset;
        gdipRcSquare.Height = iOffset;
        graphics.FillRectangle(&whiteBrush, gdipRcSquare);
        graphics.DrawRectangle(&blackPen, gdipRcSquare);
    }

}


void CNewCaptureRectChooser::ChangeActiveMode(UINT nNewMode)
{
    if (m_nActiveMode == nNewMode)
        return;

    
    if (nNewMode == PREPARATION_MODE) {
        m_nActiveMode = nNewMode;

        if ((GetExStyle() & WS_EX_LAYERED) == 0) {
            ModifyStyleEx(0, WS_EX_LAYERED | WS_EX_TRANSPARENT);
            SetLayeredWindowAttributes(RGB(255, 0, 0), (255 * 30) / 100, LWA_ALPHA | LWA_COLORKEY);
        }
        //static bool bFirstTime = true; 
        if (m_pPreparationBar)
        {
            if(m_bFromCreate == true)           
                m_pPreparationBar->UpdateSelectionRectangle(m_rcLastCustom);
            else
                m_pPreparationBar->UpdateSelectionRectangle(m_rcSelection);
        }
        RepositionPreparationBar();

        m_pPreparationBar->ShowWindow(SW_SHOW);
        m_pSelectionRectangle->ShowWindow(SW_SHOW);
       
        m_pSelectionRectangle->RedrawWindow();
        RedrawWindow();

        SetFocus();
    }
    else {

    
        m_pPreparationBar->ShowWindow(SW_HIDE);
        m_pSelectionRectangle->ShowWindow(SW_HIDE);

        if (LMisc::IsWindows7())
            ShowWindow(SW_HIDE);
        CopyBackgroundToBitmap();
        if (LMisc::IsWindows7())
            ShowWindow(SW_SHOW);
        GetAllWindows();

        if ((GetExStyle() & WS_EX_LAYERED) != 0)
            ModifyStyleEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, 0);

        SetFocus();

    }

    m_nActiveMode = nNewMode;

}

BOOL CNewCaptureRectChooser::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (m_nActiveMode != PREPARATION_MODE) {
        SetCursor(m_hSgSelectionCursor);  
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}


void CNewCaptureRectChooser::CopyBackgroundToBitmap()
{ 
    //Invalidate();
    //UpdateWindow();

    if (m_hBackgroundImage != NULL)
    {
        ::DeleteObject(m_hBackgroundImage);
        m_pBackgroundDc->DeleteDC();
        delete m_pBackgroundDc;
    }
    m_hBackgroundImage = NULL;
    m_pBackgroundDc = NULL;


    CRect rcWindow;
    GetClientRect(&rcWindow);

    int w = rcWindow.right - rcWindow.left;
    int h = rcWindow.bottom - rcWindow.top;

    CDC *pDC = GetDesktopWindow()->GetDC(); //GetDC();
    m_pBackgroundDc = new CDC();
    m_hBackgroundImage = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), w, h);
    m_pBackgroundDc->CreateCompatibleDC(pDC);
    m_pBackgroundDc->SelectObject(m_hBackgroundImage);
    m_pBackgroundDc->BitBlt(0, 0, w, h, 
        pDC, rcWindow.left, rcWindow.top,
        SRCCOPY);

    GetDesktopWindow()->ReleaseDC(pDC);
}

void CNewCaptureRectChooser::RepositionPreparationBar()
{
    if (m_pPreparationBar == NULL)
        return;

    CRect rcPreparationBar;
    m_pPreparationBar->GetWindowRect(&rcPreparationBar);
    CRect rcSelectionArea(&m_rcSelection);
    rcSelectionArea.NormalizeRect();

    CRect rcTestIntersect;
    rcTestIntersect.SetRect(rcSelectionArea.TopLeft(), rcSelectionArea.BottomRight());

    if (m_bIsPreparationBarSticky || 
        rcTestIntersect.IntersectRect(&rcTestIntersect, &rcPreparationBar) != FALSE )
        //rcSelectionArea.IntersectRect(&rcSelectionArea, &rcPreparationBar) != FALSE )
    {
        CRect rcNewWindowPos;
        rcNewWindowPos.left = rcSelectionArea.right + 10;
        rcNewWindowPos.top = rcSelectionArea.bottom - 400;
        rcNewWindowPos.right = rcNewWindowPos.left + 210/*200*/;
        rcNewWindowPos.bottom = rcNewWindowPos.top + 400;

        CRect rcDesktop;
        MfcUtils::GetDesktopRectangle(rcDesktop);

        if (rcNewWindowPos.right > rcDesktop.right) {
            rcNewWindowPos.left = rcSelectionArea.right - 210/*200*/;
            rcNewWindowPos.top = rcSelectionArea.bottom - 400;
            rcNewWindowPos.right = rcNewWindowPos.left + 210/*200*/;
            rcNewWindowPos.bottom = rcNewWindowPos.top + 400;
        }

        if (rcNewWindowPos.left < 0)
            rcNewWindowPos.OffsetRect(-rcNewWindowPos.left, 0);
        if (rcNewWindowPos.top < 0)
            rcNewWindowPos.OffsetRect(0, -rcNewWindowPos.top);

        m_pPreparationBar->MoveWindow(
            rcNewWindowPos.left, rcNewWindowPos.top, 
            rcNewWindowPos.Width(), rcNewWindowPos.Height());
        SetStickyPreparationBar(true);
    }
}

void CNewCaptureRectChooser::SetLastCustomRect(CRect &rcLastCustom)
{
    m_rcLastCustom.CopyRect(&rcLastCustom);
}

void CNewCaptureRectChooser::GetLastCustomRect(CRect &rcLastCustom)
{
    rcLastCustom.CopyRect(&m_rcLastCustom);
}

BOOL CNewCaptureRectChooser::PreTranslateMessage(MSG* pMsg)
{
    // Make sure that the keystrokes continue to the edit control.
    if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        m_pPreparationBar->OnCancelSelection();
        return TRUE;
    }
    return CWnd::PreTranslateMessage(pMsg);
}
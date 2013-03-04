// SgSelectionRectangle.cpp: Implementierungsdatei
//

#include "global.h"
#include "avgrabber.h"
#include "SgSelectionRectangle.h"


// CSgSelectionRectangle

BEGIN_MESSAGE_MAP(CSgSelectionRectangle, CWnd)
    ON_WM_PAINT()
    ON_WM_SETCURSOR()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


IMPLEMENT_DYNAMIC(CSgSelectionRectangle, CWnd)

CSgSelectionRectangle::CSgSelectionRectangle(SgSelectionRectangleListener *pSelectionRectangleListener)
{
    m_pSelectionRectangleListener = pSelectionRectangleListener;
    m_nModifyPos = 0;
    m_ptStartDrag.SetPoint(-1, -1);
    
    m_bLeftButtonPressed = FALSE;
}

CSgSelectionRectangle::~CSgSelectionRectangle()
{
}

// CSgSelectionRectangle-Meldungshandler
void CSgSelectionRectangle::OnPaint()
{
    CPaintDC dc(this); 
  
    if (m_pSelectionRectangleListener != NULL) {
        CRect rcSelection = m_pSelectionRectangleListener->GetSelectionRect();
        rcSelection.NormalizeRect();
        CRect rcWindow;
        GetClientRect(&rcWindow);
        DrawSelectionRect(&dc, rcWindow, rcSelection);
    }
}

void CSgSelectionRectangle::DrawSelectionRect(CDC *pDC, CRect &rcWindow, CRect &rcSelection)
{
    Gdiplus::Graphics graphics(pDC->GetSafeHdc());

    // Whole window
    Gdiplus::Rect gdipRcWindow(rcWindow.left, rcWindow.top, 
        rcWindow.Width(), rcWindow.Height());

    // Selection rectangle
    Gdiplus::Rect gdipRcSelection(rcSelection.left, rcSelection.top, 
        rcSelection.Width(), rcSelection.Height());

    // Selection rectangle including move border
    Gdiplus::Rect gdipRcExtendedSelection(gdipRcSelection.X - 10, gdipRcSelection.Y - 10, 
       gdipRcSelection.Width + 20, gdipRcSelection.Height + 20);

    // Move border
    Gdiplus::Region gdipRgnDrag(gdipRcExtendedSelection);
    gdipRgnDrag.Exclude(gdipRcSelection);

    // Clear window with transparent
    Gdiplus::SolidBrush transparentBrush(Gdiplus::Color(255, 0, 255, 0));
    graphics.FillRectangle(&transparentBrush, gdipRcWindow);

    // Fill the border arounf the selection with white
    Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
    graphics.FillRegion(&whiteBrush, &gdipRgnDrag);

}

void CSgSelectionRectangle::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_pSelectionRectangleListener != NULL) {
        CRect rcSelection = m_pSelectionRectangleListener->GetSelectionRect();

        CRect rcUpperLeftCorner(rcSelection.left-SELECT_CORNER_OFFSET, rcSelection.top-SELECT_CORNER_OFFSET, 
            rcSelection.left+SELECT_CORNER_OFFSET, rcSelection.top+SELECT_CORNER_OFFSET);
        CRect rcLowerLeftCorner(rcSelection.left-SELECT_CORNER_OFFSET, rcSelection.bottom-SELECT_CORNER_OFFSET, 
            rcSelection.left+SELECT_CORNER_OFFSET, rcSelection.bottom+SELECT_CORNER_OFFSET);
        CRect rcUpperRightCorner(rcSelection.right-SELECT_CORNER_OFFSET, rcSelection.top-SELECT_CORNER_OFFSET, 
            rcSelection.right+SELECT_CORNER_OFFSET, rcSelection.top+SELECT_CORNER_OFFSET);
        CRect rcLowerRightCorner(rcSelection.right-SELECT_CORNER_OFFSET, rcSelection.bottom-SELECT_CORNER_OFFSET, 
            rcSelection.right+SELECT_CORNER_OFFSET, rcSelection.bottom+SELECT_CORNER_OFFSET);
        CRect rcTopMiddle(rcSelection.left + (rcSelection.Width() - SELECT_CORNER_OFFSET * 2) / 2,
            rcSelection.top - SELECT_CORNER_OFFSET,
            rcSelection.left + (rcSelection.Width() + SELECT_CORNER_OFFSET * 2) / 2,
            rcSelection.top + SELECT_CORNER_OFFSET);
        CRect rcRightMiddle(rcSelection.right - SELECT_CORNER_OFFSET,
            rcSelection.top + (rcSelection.Height() - SELECT_CORNER_OFFSET * 2) /2,
            rcSelection.right + SELECT_CORNER_OFFSET,
            rcSelection.top + (rcSelection.Height() + SELECT_CORNER_OFFSET * 2) /2);
        CRect rcBottomMiddle(rcSelection.left + (rcSelection.Width() - SELECT_CORNER_OFFSET * 2) / 2,
            rcSelection.bottom - SELECT_CORNER_OFFSET,
            rcSelection.left + (rcSelection.Width() + SELECT_CORNER_OFFSET * 2) / 2,
            rcSelection.bottom + SELECT_CORNER_OFFSET);
        CRect rcLeftMiddle(rcSelection.left - SELECT_CORNER_OFFSET,
            rcSelection.top + (rcSelection.Height() - SELECT_CORNER_OFFSET * 2) /2,
            rcSelection.left + SELECT_CORNER_OFFSET,
            rcSelection.top + (rcSelection.Height() + SELECT_CORNER_OFFSET * 2) /2);
        CRect rcTop(rcSelection.left, rcSelection.top - 10, rcSelection.right, rcSelection.top);
        CRect rcRight(rcSelection.right, rcSelection.top - 10, rcSelection.right + 10, rcSelection.bottom + 10);
        CRect rcBottom(rcSelection.left, rcSelection.bottom, rcSelection.right, rcSelection.bottom + 10);
        CRect rcLeft(rcSelection.left - 10, rcSelection.top - 10, rcSelection.left, rcSelection.bottom);

        if (rcUpperLeftCorner.PtInRect(point)) {
            m_nModifyPos = HTTOPLEFT;
        }
        else if (rcLowerLeftCorner.PtInRect(point)) {
            m_nModifyPos = HTBOTTOMLEFT;
        }
        else if (rcUpperRightCorner.PtInRect(point)) {
            m_nModifyPos = HTTOPRIGHT;
        }
        else if (rcLowerRightCorner.PtInRect(point)) {
            m_nModifyPos = HTBOTTOMRIGHT;
        }
        else if (rcTopMiddle.PtInRect(point)){
            m_nModifyPos = HTTOP;
        }
        else if (rcRightMiddle.PtInRect(point)){
            m_nModifyPos = HTRIGHT;
        }
        else if (rcBottomMiddle.PtInRect(point)){
            m_nModifyPos = HTBOTTOM;
        }
        else if (rcLeftMiddle.PtInRect(point)){
            m_nModifyPos = HTLEFT;
        }
        else if(rcTop.PtInRect(point) || rcRight.PtInRect(point) || rcBottom.PtInRect(point) || rcLeft.PtInRect(point)){
            m_nModifyPos = HTBORDER;
            m_ptStartDrag = point;
        }
    }

    m_bLeftButtonPressed = TRUE;

    SetCapture();
    CWnd::OnLButtonDown(nFlags, point);
}

void CSgSelectionRectangle::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();

    if (m_pSelectionRectangleListener) {
        CRect rcSelection = m_pSelectionRectangleListener->GetSelectionRect();
        rcSelection.NormalizeRect();
        m_pSelectionRectangleListener->ChangeSelectionRectangle(rcSelection, true);
        m_pSelectionRectangleListener->SetLastCustomRect(rcSelection);
    }

    m_nModifyPos = 0;
    m_ptStartDrag.x = -1; 
    m_ptStartDrag.y = -1;

    m_bLeftButtonPressed = FALSE;

    CWnd::OnLButtonUp(nFlags, point);
}

void CSgSelectionRectangle::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_pSelectionRectangleListener != NULL && m_bLeftButtonPressed) {

        CRect rcSelection = m_pSelectionRectangleListener->GetSelectionRect();

        if (m_nModifyPos == HTTOPLEFT) {
            rcSelection.left = point.x;
            rcSelection.top = point.y;
        }
        else if (m_nModifyPos == HTBOTTOMLEFT) {
            rcSelection.left = point.x;
            rcSelection.bottom = point.y;
        }
        else if (m_nModifyPos == HTTOPRIGHT) {
            rcSelection.right = point.x;
            rcSelection.top = point.y;
        }
        else if (m_nModifyPos == HTBOTTOMRIGHT) {
            rcSelection.right = point.x;
            rcSelection.bottom = point.y;
        }
        else if (m_nModifyPos == HTTOP) {
            rcSelection.top = point.y;
        }
        else if (m_nModifyPos == HTRIGHT) {
            rcSelection.right = point.x;
        }
        else if (m_nModifyPos == HTBOTTOM) {
            rcSelection.bottom = point.y;
        }
        else if (m_nModifyPos == HTLEFT) {
            rcSelection.left = point.x;
        }
        else if (m_nModifyPos == HTBORDER) {
            int cx = m_ptStartDrag.x - point.x;
            int cy = m_ptStartDrag.y - point.y;
            CRect rcNewRect;
            rcNewRect.left = rcSelection.left - cx;
            rcNewRect.right = rcSelection.right - cx;
            rcNewRect.top = rcSelection.top - cy;
            rcNewRect.bottom = rcSelection.bottom - cy;
            bool bMove = true;
            CRect rcWindow;
            GetWindowRect(&rcWindow);
            if(rcNewRect.left < rcWindow.left || rcNewRect.top < rcWindow.top ||
                rcNewRect.right > rcWindow.right || rcNewRect.bottom > rcWindow.bottom)
                bMove = false;

            if (bMove) {
                rcSelection.left -= cx;
                rcSelection.right -= cx;
                rcSelection.top -= cy;
                rcSelection.bottom -= cy;
            }
            m_ptStartDrag = point;
        }
        CRect rcWindow;
        GetClientRect(&rcWindow);
        // The rectangle should not be outside the window
        if (rcSelection.left <= rcSelection.right) {
            if (rcSelection.left < rcWindow.left)
                rcSelection.left = rcWindow.left;
            if (rcSelection.right > rcWindow.right)
                rcSelection.right = rcWindow.right;
        }
        else {
            if (rcSelection.right < rcWindow.left)
                rcSelection.right = rcWindow.left;
            if (rcSelection.left > rcWindow.right)
                rcSelection.left = rcWindow.right;
        }
        if (rcSelection.top <= rcSelection.bottom) {
            if (rcSelection.top < rcWindow.top)
                rcSelection.top = rcWindow.top;
            if (rcSelection.bottom > rcWindow.bottom)
                rcSelection.bottom = rcWindow.bottom;
        }
        else {
            if (rcSelection.bottom < rcWindow.top)
                rcSelection.bottom = rcWindow.top;
            if (rcSelection.top > rcWindow.bottom)
                rcSelection.top = rcWindow.bottom;
        }

        m_pSelectionRectangleListener->ChangeSelectionRectangle(rcSelection, true);
        m_pSelectionRectangleListener->SetLastCustomRect(rcSelection);
    }

    CWnd::OnMouseMove(nFlags, point);
}

BOOL CSgSelectionRectangle::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (m_pSelectionRectangleListener != NULL) {
        CPoint ptCursor;    
        GetCursorPos(&ptCursor);

        CRect rcSelection = m_pSelectionRectangleListener->GetSelectionRect();

        CRect rcUpperLeftCorner(rcSelection.left-SELECT_CORNER_OFFSET, rcSelection.top-SELECT_CORNER_OFFSET, 
            rcSelection.left+SELECT_CORNER_OFFSET, rcSelection.top+SELECT_CORNER_OFFSET);
        CRect rcLowerLeftCorner(rcSelection.left-SELECT_CORNER_OFFSET, rcSelection.bottom-SELECT_CORNER_OFFSET, 
            rcSelection.left+SELECT_CORNER_OFFSET, rcSelection.bottom+SELECT_CORNER_OFFSET);
        CRect rcUpperRightCorner(rcSelection.right-SELECT_CORNER_OFFSET, rcSelection.top-SELECT_CORNER_OFFSET, 
            rcSelection.right+SELECT_CORNER_OFFSET, rcSelection.top+SELECT_CORNER_OFFSET);
        CRect rcLowerRightCorner(rcSelection.right-SELECT_CORNER_OFFSET, rcSelection.bottom-SELECT_CORNER_OFFSET, 
            rcSelection.right+SELECT_CORNER_OFFSET, rcSelection.bottom+SELECT_CORNER_OFFSET);
        CRect rcTopMiddle(rcSelection.left + (rcSelection.Width() - SELECT_CORNER_OFFSET * 2) / 2,
           rcSelection.top - SELECT_CORNER_OFFSET,
           rcSelection.left + (rcSelection.Width() + SELECT_CORNER_OFFSET * 2) / 2,
           rcSelection.top + SELECT_CORNER_OFFSET);
        CRect rcRightMiddle(rcSelection.right - SELECT_CORNER_OFFSET,
           rcSelection.top + (rcSelection.Height() - SELECT_CORNER_OFFSET * 2) /2,
           rcSelection.right + SELECT_CORNER_OFFSET,
           rcSelection.top + (rcSelection.Height() + SELECT_CORNER_OFFSET * 2) /2);
        CRect rcBottomMiddle(rcSelection.left + (rcSelection.Width() - SELECT_CORNER_OFFSET * 2) / 2,
           rcSelection.bottom - SELECT_CORNER_OFFSET,
           rcSelection.left + (rcSelection.Width() + SELECT_CORNER_OFFSET * 2) / 2,
           rcSelection.bottom + SELECT_CORNER_OFFSET);
        CRect rcLeftMiddle(rcSelection.left - SELECT_CORNER_OFFSET,
           rcSelection.top + (rcSelection.Height() - SELECT_CORNER_OFFSET * 2) /2,
           rcSelection.left + SELECT_CORNER_OFFSET,
           rcSelection.top + (rcSelection.Height() + SELECT_CORNER_OFFSET * 2) /2);
        CRect rcTop(rcSelection.left, rcSelection.top - 10, rcSelection.right, rcSelection.top);
        CRect rcRight(rcSelection.right, rcSelection.top - 10, rcSelection.right + 10, rcSelection.bottom + 10);
        CRect rcBottom(rcSelection.left, rcSelection.bottom, rcSelection.right, rcSelection.bottom + 10);
        CRect rcLeft(rcSelection.left - 10, rcSelection.top - 10, rcSelection.left, rcSelection.bottom);
        if (rcUpperLeftCorner.PtInRect(ptCursor)) 
            SetCursor(LoadCursor(NULL, IDC_SIZENWSE));   
        else if (rcLowerLeftCorner.PtInRect(ptCursor))
            SetCursor(LoadCursor(NULL, IDC_SIZENESW));   
        else if (rcUpperRightCorner.PtInRect(ptCursor))
            SetCursor(LoadCursor(NULL, IDC_SIZENESW));    
        else if (rcLowerRightCorner.PtInRect(ptCursor))
            SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
        else if (rcTopMiddle.PtInRect(ptCursor) || rcBottomMiddle.PtInRect(ptCursor))
           SetCursor(LoadCursor(NULL, IDC_SIZENS));
        else if (rcRightMiddle.PtInRect(ptCursor) || rcLeftMiddle.PtInRect(ptCursor))
           SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        else if(rcTop.PtInRect(ptCursor) || rcRight.PtInRect(ptCursor) || rcBottom.PtInRect(ptCursor) || rcLeft.PtInRect(ptCursor))
           SetCursor(LoadCursor(NULL, IDC_SIZEALL));
        else 
            SetCursor(LoadCursor(NULL, IDC_ARROW));    


        return TRUE;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CSgSelectionRectangle::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

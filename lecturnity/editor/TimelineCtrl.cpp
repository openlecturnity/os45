#include "stdafx.h"
#include "TimelineCtrl.h"
#include "MainFrm.h"
#include "misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimelineCtrl

int CTimelineCtrl::m_goodSpacingsMins[] = {18, 10, 25, 50, 100, 250, 500, 1000, 2000, 5000, 10000, 15000, 20000, 30000, 60000, 120000, 300000, 600000, 1200000}; // minutes
int CTimelineCtrl::m_goodSpacingsSecs[] = {18, 10, 25, 50, 100, 250, 500, 1000, 2000, 5000, 10000, 15000, 20000, 30000, 60000, 120000, 300000, 600000, 1200000}; // seconds
int CTimelineCtrl::m_goodSpacingsMillis[] = {21, 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 15000, 20000, 30000, 60000, 120000, 300000, 600000, 1200000}; // millis
int CTimelineCtrl::m_goodSpacingsFrames[] = {21, 1, 2, 5, 10, 15, 20, 25, 50, 100, 125, 200, 300, 400, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000}; // frames

CTimelineCtrl::CTimelineCtrl() {
    m_bLeftMousePressed = false;

    m_bmpSlider.LoadBitmap(IDB_SLIDER);
    m_penLight.CreatePen(PS_SOLID, 1, RGB(235, 176, 57));
    m_penMiddle.CreatePen(PS_SOLID, 1, RGB(204, 153, 49));
    m_penDark.CreatePen(PS_SOLID, 1, RGB(235, 176, 57));
    m_greyPen.CreatePen(PS_SOLID, 1, RGB(101, 105, 111));
    //int h[];
    //h[] = {12, 1000, 2000, 5000, 10000, 15000, 20000, 30000, 60000, 120000, 300000, 600000, 1200000};
    LoadBackgroundImage();
}   

CTimelineCtrl::~CTimelineCtrl() {
}


BEGIN_MESSAGE_MAP(CTimelineCtrl, CWnd)
    //{{AFX_MSG_MAP(CTimelineCtrl)
    //ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTimelineCtrl 

void CTimelineCtrl::OnSize(UINT nType, int cx, int cy) {
    if (cx > 0 && cy > 0) {
        if (NULL != m_doubleBuffer.m_hObject) {
            m_doubleBufferDC.SelectObject(m_pOldObject);
            m_doubleBuffer.DeleteObject();
        }
        if (NULL != m_doubleBufferDC.m_hDC) {
            m_doubleBufferDC.DeleteDC();
        }

        CDC *pDC = GetDC();
        m_doubleBuffer.CreateCompatibleBitmap(pDC, cx, cy);
        m_doubleBufferDC.CreateCompatibleDC(pDC);
        m_pOldObject = m_doubleBufferDC.SelectObject(&m_doubleBuffer);
        ReleaseDC(pDC);
    }
}

int CTimelineCtrl::FindGoodSpacing(int *spaces, int wishSpace) {
    int count = spaces[0];
    int bestSpaceIndex = 1;
    bool foundSpace = false;

    if (wishSpace < spaces[1]) {
        foundSpace = true;
    }

    for (int i=1; i<count && !foundSpace; ++i) {
        if (spaces[i] <= wishSpace && spaces[i+1] > wishSpace) {
            foundSpace = true;
        }
        bestSpaceIndex = i;
    }

    if (!foundSpace) {
        bestSpaceIndex = count;
    }

    return spaces[i];
}

void CTimelineCtrl::OnPaint() {
    CPaintDC deviceContext(this); // device context for painting

    if (NULL == m_doubleBuffer.m_hObject) {
        return;
    }

    CRect rcClient;
    GetClientRect(&rcClient);

    // Clipping
    CRect rcClip;
    deviceContext.GetClipBox(&rcClip);

    CRgn rgnClip;
    rgnClip.CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
    m_doubleBufferDC.SelectClipRgn(&rgnClip);

    //CDC *pDC = &m_doubleBufferDC;

    DrawItem(rcClient, &m_doubleBufferDC);

    // Glonk double buffer onto real component DC
    deviceContext.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_doubleBufferDC, 0, 0, SRCCOPY);
}

BOOL CTimelineCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)  {
    m_markerLeftImage.LoadBitmap(IDB_MARKER_LEFT);
    m_markerRightImage.LoadBitmap(IDB_MARKER_RIGHT);

    return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CTimelineCtrl::DrawItem(CRect &rcClient, CDC *pDC) {
    // no project
    if (m_pEditorDoc && !m_pEditorDoc->project.IsEmpty()) {
        int sizeToPaint = rcClient.Width();
        if (sizeToPaint <= 0) {
            return;
        }

        // draw background	
        if (m_pImageBackground) {
            m_pImageBackground->DrawImage(pDC, rcClient, m_pImageBackground->GetSource(), CRect(5, 5, 5, 5), COLORREF_NULL);
        }

        CPen linePen(PS_SOLID, 1, RGB(0x6A, 0x6A, 0x6A));
        CPen *pOldPen = pDC->SelectObject(&linePen);
        pDC->MoveTo(rcClient.left, rcClient.bottom-1);
        pDC->LineTo(rcClient.right, rcClient.bottom-1);

        pDC->SelectObject(&m_greyPen);
		
        // draw ruller (lines and text)
        double pixelSpace = 80.0; // minimum pixel space
		double displayPixels = sizeToPaint;
		double displayMillis = m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs;
		double millisToPixelFactor = displayPixels / displayMillis;
        double oneTickMillis = pixelSpace / millisToPixelFactor;

        int bestTickMillis = 1;
        switch (m_pEditorDoc->m_streamUnit) {
        case SU_MINUTES:
            bestTickMillis = FindGoodSpacing(m_goodSpacingsMins, (int) oneTickMillis);
            break;
        case SU_SECONDS:
            bestTickMillis = FindGoodSpacing(m_goodSpacingsSecs, (int) oneTickMillis);
            break;
        case SU_MILLIS:
            bestTickMillis = FindGoodSpacing(m_goodSpacingsMillis, (int) oneTickMillis);
            break;
        }

        //TRACE1("bestTickMillis == %d\n", bestTickMillis);
        pixelSpace = bestTickMillis * millisToPixelFactor;
        oneTickMillis = pixelSpace / millisToPixelFactor;

        if (m_pEditorDoc->m_docLengthMs > 0) {
            if (!m_tickFont.m_hObject) {
                int fontSize = 75*96 / pDC->GetDeviceCaps(LOGPIXELSY);
                m_tickFont.CreatePointFont(fontSize, _T("Arial"), pDC);
            }

            // Calculate the left offset to make the ticks look
            // nice
            int count = (int) (m_pEditorDoc->m_displayStartMs / oneTickMillis);
            double offsetMs = oneTickMillis - (((double) m_pEditorDoc->m_displayStartMs) - oneTickMillis * count);
            int offsetPixel = rcClient.left + Calculator::GetPixelFromMsec((int) offsetMs, sizeToPaint, m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs);

            int ticksToPaint = (int) (sizeToPaint / pixelSpace) + 1;
            CString tickString;
            CRect textRect;
            textRect.top = 7;
            textRect.bottom = 20;

			pDC->SetTextColor(RGB(94, 117, 152));
			CFont *pOldFont = pDC->SelectObject(&m_tickFont);
			int oldMode = pDC->SetBkMode(TRANSPARENT);

			int xPosSmall;
			for (int t=-1; t<=ticksToPaint; ++t) {
				int paintMillis = (int) (offsetMs + m_pEditorDoc->m_displayStartMs + 
					(((double) t) * oneTickMillis) + .5);
				Misc::FormatTickString(tickString, paintMillis, m_pEditorDoc->m_streamUnit);
				int xPos = offsetPixel + (int) (pixelSpace * t);
				textRect.left = xPos - (int) (pixelSpace/ 2.0);
				textRect.right = xPos + (int) (pixelSpace / 2.0);
				pDC->DrawText(tickString, &textRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				
				pDC->MoveTo(xPos, rcClient.bottom - 8);
				pDC->LineTo(xPos, rcClient.bottom - 1);
				//paint small lines
				int loops = 10;
				for (int i=1; i<loops; ++i) {
					xPosSmall = rcClient.left + xPos + (int)(pixelSpace/10 * i);
					if (rcClient.left <= xPosSmall && rcClient.right >= xPosSmall) {
						pDC->MoveTo(xPosSmall, rcClient.bottom - 5);
						pDC->LineTo(xPosSmall, rcClient.bottom - 1);
					}
				}
			}
			pDC->SetBkMode(oldMode);
			pDC->SelectObject(pOldFont);
			
			int markStart = m_pEditorDoc->m_markStartMs;
			int markEnd   = m_pEditorDoc->m_markEndMs;
			// Draw mark marks...
			if (markStart != -1 &&
				markStart >= m_pEditorDoc->m_displayStartMs &&
				markStart <= m_pEditorDoc->m_displayEndMs) {
				int markPosPx = rcClient.left - 9 + Calculator::GetPixelFromMsec(
                    markStart - m_pEditorDoc->m_displayStartMs,
                    sizeToPaint, 
                    m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs);
                CRect rcBitmap(0, 0, 10, 15);		// dimension of bitmap
                CDC bmpDC;
                bmpDC.CreateCompatibleDC(pDC);
                bmpDC.SelectObject(m_markerLeftImage.m_hObject);
                TransparentBlt(pDC->m_hDC, markPosPx, 0, 10, 15, bmpDC.m_hDC, 0, 0, 
							      rcBitmap.Width(), rcBitmap.Height(), RGB(255, 255, 255)); 
            }

            if (markEnd != -1 &&
				markEnd >= m_pEditorDoc->m_displayStartMs &&
				markEnd <= m_pEditorDoc->m_displayEndMs) {
				int markPosPx = rcClient.left + 1 + Calculator::GetPixelFromMsec(
					markEnd - m_pEditorDoc->m_displayStartMs,
					sizeToPaint, 
					m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs);
                CRect rcBitmap(0, 0, 10, 15);		// dimension of bitmap
                CDC bmpDC;
                bmpDC.CreateCompatibleDC(pDC);
                bmpDC.SelectObject(m_markerRightImage.m_hObject);
                TransparentBlt(pDC->m_hDC, markPosPx, 0, 10, 15, bmpDC.m_hDC, 0, 0,
							          rcBitmap.Width(), rcBitmap.Height(), RGB(255, 255, 255)); 
			}

            DrawCursor(pDC, rcClient.left, rcClient.right, rcClient.top, rcClient.bottom);
        }
        pDC->SelectObject(pOldPen);
    } else {
        COLORREF clrBg = CMainFrameE::GetColor(CMainFrameE::COLOR_BG_DARK);
        CBrush bgBrush(clrBg);
        pDC->FillRect(rcClient, &bgBrush);
        bgBrush.DeleteObject();
    }
}

void CTimelineCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
    m_bLeftMousePressed = true;
    SetCapture();
    m_pEditorDoc->SetUserIsScrolling(true);
}

void CTimelineCtrl::OnMouseMove(UINT nFlags, CPoint point) {
    if (m_bLeftMousePressed) {
        CRect viewRect;
        GetClientRect(&viewRect);
        if (point.x >= viewRect.right) {
            int diff = point.x - viewRect.right;
            if (diff <= 10) {
                point.x = viewRect.right + 10;
            }
        } else if (point.x <= viewRect.left) {
            int diff = viewRect.left - point.x;
            if (diff <= 10) {
                point.x = viewRect.left - 10;
            }
        }
        ((CStreamView *)GetParent())->UpdateSliderPos(point, nFlags, STATE_MOUSEMOVE);
    }
}

void CTimelineCtrl::OnLButtonUp(UINT nFlags, CPoint point)  {
    m_bLeftMousePressed = false;
    ReleaseCapture();
    m_pEditorDoc->SetUserIsScrolling(false);

    if (!CMainFrameE::GetCurrentInstance()->PreviewInNormalMode()) {
        return;
    }

    ((CStreamView *)GetParent())->UpdateSliderPos(point, nFlags, STATE_MOUSEUP);
}

BOOL CTimelineCtrl::OnEraseBkgnd(CDC* pDC) {
    return TRUE;
}
#define LINE_TOP 17
void CTimelineCtrl::DrawCursor(CDC *pDC, int left, int right, int top, int bottom) { 
    int iCursorPositionPixel = left + Calculator::GetPixelFromMsec(
        m_pEditorDoc->m_curPosMs - m_pEditorDoc->m_displayStartMs,
        right-left, 
        m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs);

    if (iCursorPositionPixel < 0 || iCursorPositionPixel > right) {
        iCursorPositionPixel = -1;
    }

    if (iCursorPositionPixel != -1) {
        CDC bmpDC;
        bmpDC.CreateCompatibleDC(pDC);
        bmpDC.SelectObject(m_bmpSlider.m_hObject);
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 0x9f;
        bf.AlphaFormat = 0;

        TransparentBlt(pDC->m_hDC, iCursorPositionPixel-7, 0, 16, LINE_TOP, 
            bmpDC.m_hDC, 0, 0, 16, LINE_TOP, RGB(0, 0, 0));

        CPen *oldPen = pDC->SelectObject(&m_penLight);
        pDC->MoveTo(iCursorPositionPixel-1, LINE_TOP);
        pDC->LineTo(iCursorPositionPixel-1, bottom);
        pDC->SelectObject(&m_penMiddle);
        pDC->MoveTo(iCursorPositionPixel, LINE_TOP);
        pDC->LineTo(iCursorPositionPixel, bottom);
        pDC->SelectObject(&m_penDark);
        pDC->MoveTo(iCursorPositionPixel+1, LINE_TOP);
        pDC->LineTo(iCursorPositionPixel+1, bottom);
        pDC->SelectObject(oldPen);
    }
}

bool CTimelineCtrl::LoadBackgroundImage() {
   //load background images
   m_pImageBackground = XTPOffice2007Images()->LoadFile(_T("BACKGROUNDSTREAM")); //BACKGROUNDTIMELINE
   return m_pImageBackground != NULL;
}

void CTimelineCtrl::ColorSchemeChanged() {
    LoadBackgroundImage();
}
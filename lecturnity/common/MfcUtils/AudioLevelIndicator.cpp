// AudioLevelIndicator.cpp : implementation file
//

#include "stdafx.h"
#include "AudioLevelIndicator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* REVIEW UK
 * Remove from Assistant (double code)
 
 * OnPaint: no magic numbers (configurable variables?? automatic/dynamic size)
 */



/////////////////////////////////////////////////////////////////////////////
// CAudioLevelIndicatorListener


/////////////////////////////////////////////////////////////////////////////
// CAudioLevelIndicator

CAudioLevelIndicator::CAudioLevelIndicator(AudioLevelIndicatorListener *pAudioLevelListener)
{
    m_pAudioLevelListener = pAudioLevelListener;

    m_ToolTip = NULL;
    m_bTracking = FALSE;
    m_bMonitorElement = FALSE;

    m_dAudioLevel = 0;
    m_nAudioTimer = 0;
}

CAudioLevelIndicator::~CAudioLevelIndicator()
{
    if (m_ToolTip != NULL)
        delete m_ToolTip;
}

BEGIN_MESSAGE_MAP(CAudioLevelIndicator, CStatic)
    //{{AFX_MSG_MAP(CAudioLevelIndicator)
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
    //}}AFX_MSG_MAP
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



int CAudioLevelIndicator::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CStatic::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_nAudioTimer = SetTimer(AUDIO_TIMER, 250, 0);  

    return 0;
}

void CAudioLevelIndicator::OnDestroy()
{
    if (m_nAudioTimer > 0)  
        KillTimer(m_nAudioTimer); 
    m_nAudioTimer = 0;

    CStatic::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CAudioLevelIndicator message handlers

BOOL CAudioLevelIndicator::OnEraseBkgnd(CDC* pDC) 
{
    return TRUE;
}

// Set the tooltip with a string resource
void CAudioLevelIndicator::SetToolTipText(UINT nId, BOOL bActivate)
{
    // load string resource
    m_csToolText.LoadString(nId);
    // If string resource is not empty
    if (m_csToolText.IsEmpty() == FALSE) SetToolTipText(m_csToolText);

}

// Set the tooltip with a CString
void CAudioLevelIndicator::SetToolTipText(CString spText, BOOL bActivate)
{
    // We cannot accept NULL pointer
    if (spText.IsEmpty()) return;

    // Initialize ToolTip
    InitToolTip();
    m_csToolText = spText;

    // If there is no tooltip defined then add it
    if (m_ToolTip->GetToolCount() == 0)
    {
        CRect rectBtn; 
        GetClientRect(rectBtn);
        m_ToolTip->AddTool(this, m_csToolText, rectBtn, 1);
    }

    // Set text for tooltip
    m_ToolTip->UpdateTipText(m_csToolText, this, 1);
    ActivateTooltip(bActivate);
}

void CAudioLevelIndicator::InitToolTip()
{
    if (m_ToolTip == NULL)
    {
        m_ToolTip = new CToolTipCtrl;
        // Create ToolTip control
        m_ToolTip->Create(this);
        m_ToolTip->SendMessage(TTM_SETMAXTIPWIDTH, 0, 300);
    }
} // End of InitToolTip

// Activate the tooltip
void CAudioLevelIndicator::ActivateTooltip(BOOL bActivate)
{
    // If there is no tooltip then do nothing
    if (m_ToolTip->GetToolCount() == 0) return;

    // Activate tooltip
    m_ToolTip->Activate(bActivate);
} // End of EnableTooltip

void CAudioLevelIndicator::DeleteToolTip()
{
    // Destroy Tooltip incase the size of the button has changed.
    if (m_ToolTip != NULL)
    {
        delete m_ToolTip;
        m_ToolTip = NULL;
    }
}

void CAudioLevelIndicator::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    // TODO: Add your message handler code here and/or call default
    if(!m_bMonitorElement)
    {
        if (m_pAudioLevelListener != NULL)
        {
            m_pAudioLevelListener->ShowAudioSettings();
        }
    }
    CStatic::OnLButtonDblClk(nFlags, point);
}

void CAudioLevelIndicator::SetMonitorGroupElement()
{
    m_bMonitorElement = TRUE;
}

BOOL CAudioLevelIndicator::PreTranslateMessage(MSG* pMsg) 
{
    // TODO: Add your specialized code here and/or call the base class  
    if (m_ToolTip != NULL)
        if (::IsWindow(m_ToolTip->m_hWnd)) // Incase m_ToolTip isn't NULL, check to see if its a valid window
            m_ToolTip->RelayEvent(pMsg);	
    return CStatic::PreTranslateMessage(pMsg);
}

LRESULT CAudioLevelIndicator::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
    // TODO: Add your message handler code here and/or call default
    /* This line corrects a problem with the tooltips not displaying when 
    the mouse passes over them, if the parent window has not been clicked yet.
    Normally this isn't an issue, but when developing multi-windowed apps, this 
    bug would appear. Setting the ActiveWindow to the parent is a solution to that.
    */
    ::SetActiveWindow(GetParent()->GetSafeHwnd());
    //	Invalidate();
    DeleteToolTip();
    // Create a new Tooltip with new Button Size and Location
    SetToolTipText(m_csToolText);
    if (m_ToolTip != NULL)
        if (::IsWindow(m_ToolTip->m_hWnd))
            //Display ToolTip
            m_ToolTip->Update();

    return 0;
}


LRESULT CAudioLevelIndicator::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
    m_bTracking = FALSE;
    //	Invalidate();
    return 0;
}

void CAudioLevelIndicator::OnMouseMove(UINT nFlags, CPoint point) 
{
    // TODO: Add your message handler code here and/or call default
    if (!m_bTracking)
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE|TME_HOVER;
        tme.dwHoverTime = 1;
        m_bTracking = _TrackMouseEvent(&tme);
    }
    CStatic::OnMouseMove(nFlags, point);
}

void CAudioLevelIndicator::OnPaint()
{
    CPaintDC dc(this); 

    CRect rcClient;
    GetClientRect(&rcClient);

    
    UINT nPosition = 0;
    if (m_dAudioLevel >= 0 && m_dAudioLevel <= 1.0)
        nPosition = (int)(m_dAudioLevel*39);

    Gdiplus::Graphics graphics(dc);

    Gdiplus::REAL dScaleX = (Gdiplus::REAL)rcClient.Width() / 13.0;
    Gdiplus::REAL dScaleY = (Gdiplus::REAL)rcClient.Height() / 40.0;

    graphics.ScaleTransform(dScaleX, dScaleY);

    // Border
    Gdiplus::Pen greyPen(Gdiplus::Color(255, 109, 109, 109), 1);
    graphics.DrawLine(&greyPen, 0, 0, 0, rcClient.Height()-1);
    greyPen.SetColor(Gdiplus::Color(255, 168, 168, 168));
    graphics.DrawLine(&greyPen, 1, 0, rcClient.Width()-1, 0);
    greyPen.SetColor(Gdiplus::Color(255, 225, 225, 225));
    graphics.DrawLine(&greyPen, rcClient.Width()-1, 1, rcClient.Width()-1, rcClient.Height());
    greyPen.SetColor(Gdiplus::Color(255, 215, 215, 215));
    graphics.DrawLine(&greyPen, 1, rcClient.Height()-1, rcClient.Width()-2, rcClient.Height()-1);

    // Background
    Gdiplus::LinearGradientBrush linGrBrushRY(
        Gdiplus::Point(0, 1),
        Gdiplus::Point(0, 16),
        Gdiplus::Color(255, 124, 47, 47),
        Gdiplus::Color(255, 174, 165, 65)); 

    Gdiplus::REAL relativeIntensities[] = {0.0f, 0.5f, 1.0f};
    Gdiplus::REAL relativePositions[]   = {0.0f, 0.25f, 1.0f};
    linGrBrushRY.SetBlend(relativeIntensities, relativePositions, 3);

    Gdiplus::LinearGradientBrush linGrBrushYG(
        Gdiplus::Point(0, 16),
        Gdiplus::Point(0, 39),
        Gdiplus::Color(255, 174, 165, 65),
        Gdiplus::Color(255, 127, 188, 77)); 

    graphics.FillRectangle(&linGrBrushRY, 1, 1, 11, 16);
    graphics.FillRectangle(&linGrBrushYG, 1, 17, 11, 22);

    // highlighting
    if (nPosition > 0) {
        Gdiplus::LinearGradientBrush linGrBrushRYBright(
            Gdiplus::Point(0, 1),
            Gdiplus::Point(0, 16),
            Gdiplus::Color(255, 153, 0, 0),
            Gdiplus::Color(255, 219, 216, 0)); 

        linGrBrushRYBright.SetBlend(relativeIntensities, relativePositions, 3);

        Gdiplus::LinearGradientBrush linGrBrushYGBright(
            Gdiplus::Point(0, 16),
            Gdiplus::Point(0, 39),
            Gdiplus::Color(255, 219, 216, 0),
            Gdiplus::Color(255, 107, 233, 0)); 

        int nLowerPosition = nPosition >= 22 ? 0 : 22 - nPosition;
        graphics.FillRectangle(&linGrBrushYGBright, 1, 17+nLowerPosition, 11, 22-nLowerPosition);


        if (nPosition > 22) {
            int nUpperPosition = nPosition >= 39 ? 0 : 38 - nPosition;
            graphics.FillRectangle(&linGrBrushRYBright, 1, 1+nUpperPosition, 11, 16-nUpperPosition);
        }

        Gdiplus::REAL relativeIntensities2[] = {0.0f, 0.1f, 0.0f};
        Gdiplus::REAL relativePositions2[]   = {0.0f, 0.5f, 1.0f};

        Gdiplus::LinearGradientBrush linGrBrushWhiteL(
            Gdiplus::Point(1, 0),
            Gdiplus::Point(6, 0),
            Gdiplus::Color(0, 255, 255, 255),
            Gdiplus::Color(148, 255, 255, 255)); 

        Gdiplus::LinearGradientBrush linGrBrushWhiteR(
            Gdiplus::Point(6, 0),
            Gdiplus::Point(13, 0),
            Gdiplus::Color(148, 255, 255, 255),
            Gdiplus::Color(0, 255, 255, 255)); 

        int nUpperPosition = nPosition >= 39 ? 0 : 38 - nPosition;
        graphics.FillRectangle(&linGrBrushWhiteL, 1, 1+nUpperPosition, 6, 38-nUpperPosition);
        graphics.FillRectangle(&linGrBrushWhiteR, 7, 1+nUpperPosition, 6, 38-nUpperPosition);

        Gdiplus::Pen whitePen(Gdiplus::Color(255, 255, 255, 255), 1);
        graphics.DrawLine(&whitePen, 1, 1+nUpperPosition, 12, 1+nUpperPosition);

    }
}

void CAudioLevelIndicator::OnTimer(UINT_PTR nIDEvent)
{
    if (m_pAudioLevelListener != NULL && GetSafeHwnd() != NULL) {
        if (nIDEvent == m_nAudioTimer) {
            int iAudioLevel = m_pAudioLevelListener->GetAudioLevel();
            m_dAudioLevel = (double)iAudioLevel / 100;
            RedrawWindow();
        }
    }

    CStatic::OnTimer(nIDEvent);
}

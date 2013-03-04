// StreamView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "StreamView.h"
//#include "editor.h"
#include "editorDoc.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStreamView

BEGIN_MESSAGE_MAP(CStreamView, CView)
    //{{AFX_MSG_MAP(CStreamView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND ()
    ON_COMMAND(ID_UNIT_MINUTES, OnUnitMinutes)
    ON_UPDATE_COMMAND_UI(ID_UNIT_MINUTES, OnUpdateUnitMinutes)
    ON_COMMAND(ID_UNIT_SECONDS, OnUnitSeconds)
    ON_UPDATE_COMMAND_UI(ID_UNIT_SECONDS, OnUpdateUnitSeconds)
    ON_COMMAND(ID_UNIT_MILLISECONDS, OnUnitMilliseconds)
    ON_UPDATE_COMMAND_UI(ID_UNIT_MILLISECONDS, OnUpdateUnitMilliseconds)
    ON_COMMAND(ID_MARKS, OnShowMarks)
    ON_UPDATE_COMMAND_UI(ID_MARKS, OnUpdateShowMarks)
    ON_COMMAND(ID_VIDEO, OnShowVideo)
    ON_UPDATE_COMMAND_UI(ID_VIDEO, OnUpdateShowVideo)
    ON_COMMAND(ID_FOLIEN, OnShowPages)
    ON_UPDATE_COMMAND_UI(ID_FOLIEN, OnUpdateShowPages)
    ON_COMMAND(ID_CLIPS, OnShowClips)
    ON_UPDATE_COMMAND_UI(ID_CLIPS, OnUpdateShowClips)
    ON_COMMAND(ID_AUDIO, OnShowAudio)
    ON_UPDATE_COMMAND_UI(ID_AUDIO, OnUpdateShowAudio)
    ON_COMMAND(IDC_ZOOM_IN, OnZoomIn)
    ON_COMMAND(IDC_ZOOM_OUT, OnZoomOut)
    ON_WM_CREATE()
    ON_WM_MOUSEWHEEL()
    ON_WM_HSCROLL()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_SETCURSOR()
    //}}AFX_MSG_MAP

    ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()


int CStreamView::SCROLLBAR_HEIGHT = ::GetSystemMetrics(SM_CYHSCROLL);

IMPLEMENT_DYNCREATE(CStreamView, CView)

CStreamView::CStreamView() {
    HINSTANCE hInstance = AfxGetApp()->m_hInstance;
    m_hPositionCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_POS);


    // if the user changed the visibility of a stream
    // use the user settings, otherwise draw streams only if they exists
    m_bUserHasChangedVideoVisibility = false;
    m_bUserHasChangedAudioVisibility = false;
    m_bUserHasChangedClipsVisibility = false;
    m_bUserHasChangedSlidesVisibility = false;
    m_bUserHasChangedMarksVisibility = false;

    m_bHavePaused = false;	
    m_lastDisplayPos = -1;
    //m_positionImage.CreateFromBitmap(IDB_TIMELINEPOSITION);
    m_bmpSlider.LoadBitmap(IDB_TIMELINEPOSITION);
    m_iCursorPositionPixel = -1;
}

CStreamView::~CStreamView()
{
}


/////////////////////////////////////////////////////////////////////////////
// Zeichnung CStreamView 

void CStreamView::OnDraw(CDC* pDC) {
    CEditorDoc* pEditorDoc = (CEditorDoc *)GetDocument();

    // everything will be painted 
    // in controls, except a small part in the lower left
    // corner... :)

    CRect rcClient;
    GetClientRect(&rcClient);	

    if (pEditorDoc && !pEditorDoc->project.IsEmpty()) {
        ShowChildWindows(true);

        CRect rcEmptyPart(rcClient.left, rcClient.bottom - SCROLLBAR_HEIGHT, 
            rcClient.left + LEFT_OFFSET, rcClient.bottom);
        COLORREF bgClr = CMainFrameE::GetColor(CMainFrameE::COLOR_BG_LIGHT);
        CBrush bgBrush(bgClr);
        pDC->FillRect(&rcEmptyPart, &bgBrush);
        bgBrush.DeleteObject();

        RedrawTimelineCtrl(pDC);
        RedrawStreamCtrl(pDC);
    } else {
        ShowChildWindows(false);
        COLORREF bgClr = CMainFrameE::GetColor(CMainFrameE::COLOR_BG_DARK);
        CBrush bgBrush(bgClr);
        pDC->FillRect(&rcClient, &bgBrush);
        bgBrush.DeleteObject();
    }
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CStreamView

#ifdef _DEBUG
void CStreamView::AssertValid() const
{
	CView::AssertValid();
}

void CStreamView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CStreamView 

int CStreamView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // toolbar with popup menus
    XTPImageManager()->SetMaskColor(RGB(254, 254, 254));

    m_wndToolBar.CreateToolBar(WS_CHILD, this);
    m_wndToolBar.LoadToolBar(IDR_TOOLBAR_STREAM);

    CXTPControl *pControlStreams = m_wndToolBar.GetControls()->FindControl(IDR_POPUP_STREAMS);
    if (pControlStreams != NULL) {
        int iIndex = pControlStreams->GetIndex();
        m_wndToolBar.GetControls()->Remove(m_wndToolBar.GetControl(iIndex));
        pControlStreams = m_wndToolBar.GetControls()->Add(xtpControlButtonPopup, IDR_POPUP_STREAMS, _T(""), iIndex);
        pControlStreams->SetFlags(xtpFlagManualUpdate);
    }

    CXTPControl *pControlUnit = m_wndToolBar.GetControls()->FindControl(IDR_POPUP_UNIT);
    if (pControlUnit != NULL) {
        int iIndex = pControlUnit->GetIndex();
        m_wndToolBar.GetControls()->Remove(m_wndToolBar.GetControl(iIndex));
        pControlUnit = m_wndToolBar.GetControls()->Add(xtpControlButtonPopup, IDR_POPUP_UNIT, _T(""), iIndex);
        pControlUnit->SetFlags(xtpFlagManualUpdate);
    }

    CXTPControl *pControlZoom = m_wndToolBar.GetControls()->FindControl(IDR_POPUP_ZOOM);
    if (pControlZoom != NULL) {
        int iIndex = pControlZoom->GetIndex();
        m_wndToolBar.GetControls()->Remove(m_wndToolBar.GetControl(iIndex));
        pControlZoom = m_wndToolBar.GetControls()->Add(xtpControlButtonPopup, IDR_POPUP_ZOOM, _T(""), iIndex);
        pControlZoom->SetFlags(xtpFlagManualUpdate);
    }

    // The timeline control:
    m_timelineCtrl.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(LEFT_OFFSET, 0, 300, 21), this, ID_TIMELINE);
    m_timelineCtrl.ModifyStyleEx(0, WS_EX_TRANSPARENT, 0);
    m_timelineCtrl.SetEditorDoc((CEditorDoc *) GetDocument());

    // This controls paints the streams:
    m_streamCtrl.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(0, 21, 300, 180), this, ID_STREAMS);
    m_streamCtrl.ModifyStyleEx(0, WS_EX_TRANSPARENT, 0);
    m_streamCtrl.SetEditorDoc((CEditorDoc *) GetDocument());

    m_displayScroll.Create(WS_CHILD|SBS_HORZ, CRect(112, 180, 300, 200), this, ID_DISPLAYSCROLL);
    //m_displayScroll.EnableWindow(FALSE);

    return 0;
}

void CStreamView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
	
	UpdateStreamVisibilities(pDoc);
	
	SCROLLINFO si;
	ZeroMemory(&si, sizeof SCROLLINFO);
	si.cbSize = sizeof SCROLLINFO;
	si.fMask  = SIF_ALL;
	m_displayScroll.SetScrollInfo(&si, FALSE);
	
	m_controlIsPressed  = 0;
	m_shiftIsPressed    = 0;
	
	RedrawWindow();
	//EnableToolTips();
}

void CStreamView::OnSize(UINT nType, int cx, int cy) {
    CView::OnSize(nType, cx, cy);
    CSize sz(0);

    // TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen

    if (m_wndToolBar.m_hWnd) {
        sz = m_wndToolBar.CalcDockingLayout(LEFT_OFFSET, LM_HORZDOCK | LM_HORZ | LM_COMMIT);
        m_wndToolBar.SetWindowPos(NULL, -2, 0, 114, sz.cy, SWP_NOZORDER);
        m_wndToolBar.Invalidate(FALSE);
    }

    //if (m_visibleStreamsButton.m_hWnd)
    //	m_visibleStreamsButton.MoveWindow( 0, 0, 16, 16);

    if (m_timelineCtrl.m_hWnd) {
        m_rcTimelineCtrl.SetRect(LEFT_OFFSET, 0, cx, sz.cy);
        m_timelineCtrl.SetWindowPos(NULL, m_rcTimelineCtrl.left, m_rcTimelineCtrl.top,
            m_rcTimelineCtrl.Width(), m_rcTimelineCtrl.Height(), SWP_NOZORDER);
    } else {
        m_rcTimelineCtrl.SetRectEmpty();
    }

    if (m_streamCtrl.m_hWnd) {
        m_rcStreamCtrl.SetRect(0, sz.cy, cx, cy-SCROLLBAR_HEIGHT);
        m_streamCtrl.SetWindowPos(NULL, m_rcStreamCtrl.left, m_rcStreamCtrl.top,
            m_rcStreamCtrl.Width(), m_rcStreamCtrl.Height(), SWP_NOZORDER);
    } else {
        m_rcStreamCtrl.SetRectEmpty();
    }

    if (m_displayScroll.m_hWnd)
        m_displayScroll.SetWindowPos(NULL, 112, cy-SCROLLBAR_HEIGHT, 
            cx-112, SCROLLBAR_HEIGHT, SWP_NOZORDER);

}

BOOL CStreamView::OnEraseBkgnd(CDC *pDC)
{
	// Do not do anything
   return TRUE;
}

BOOL CStreamView::OnToolTipNotify(UINT id, NMHDR * pNMHDR, LRESULT * pResult) {
	return(FALSE);
}

void CStreamView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) {
    if (lHint != HINT_CHANGE_POSITION && lHint != HINT_CHANGE_PAGE && lHint != HINT_CHANGE_STRUCTURE && lHint != HINT_CHANGE_SELECTION) {
        return;
    }
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
    if (!pDoc) {
        return;
    }

    // redraw stream view's controls just when position slider changes it's position.
    // if m_iCursorPositionPixel is -1 force a full redraw of this view.
    int iPos = m_iCursorPositionPixel;
    int iCursorPositionPixel = m_rcTimelineCtrl.left + Calculator::GetPixelFromMsec(
        pDoc->m_curPosMs - pDoc->m_displayStartMs,
        m_rcTimelineCtrl.right - m_rcTimelineCtrl.left, 
        pDoc->m_displayEndMs - pDoc->m_displayStartMs);

    if (pDoc->IsPreviewActive() 
        && m_iCursorPositionPixel > 0 
        && m_iCursorPositionPixel < m_rcTimelineCtrl.right - m_rcTimelineCtrl.left - 8
        && m_iCursorPositionPixel == iCursorPositionPixel) {
        return;
    }
    m_iCursorPositionPixel = iCursorPositionPixel;

    UpdateStreamVisibilities(pDoc);
    UpdateScrollBar();
    if (iPos == -1) {
        RedrawWindow();
    } else {
        RedrawWindow(NULL, NULL, RDW_INVALIDATE); //NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void CStreamView::OnUnitMinutes() {
    CEditorDoc *pDoc = (CEditorDoc *)GetDocument();
    if (pDoc) {
        pDoc->m_streamUnit = SU_MINUTES;
        OnUpdate(this, HINT_CHANGE_POSITION, NULL);
    }
}

void CStreamView::OnUpdateUnitMinutes(CCmdUI* pCmdUI) { 
    CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty()) {
        pCmdUI->SetCheck(FALSE);
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(pEditorDoc->m_streamUnit == SU_MINUTES ? TRUE : FALSE);
}

void CStreamView::OnUnitSeconds() {
    CEditorDoc *pDoc = (CEditorDoc *)GetDocument();
    if (pDoc) {
        pDoc->m_streamUnit = SU_SECONDS;
        OnUpdate(this, HINT_CHANGE_POSITION, NULL);
    }
}

void CStreamView::OnUpdateUnitSeconds(CCmdUI* pCmdUI) { 
    CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty()) {
        pCmdUI->SetCheck(FALSE);
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(pEditorDoc->m_streamUnit == SU_SECONDS ? TRUE : FALSE);
}

void CStreamView::OnUnitMilliseconds() {
    CEditorDoc *pDoc = (CEditorDoc *)GetDocument();
    if (pDoc) {
        pDoc->m_streamUnit = SU_MILLIS;
        OnUpdate(this, HINT_CHANGE_POSITION, NULL);
    }
}

void CStreamView::OnUpdateUnitMilliseconds(CCmdUI* pCmdUI) { 
    CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty()) {
        pCmdUI->SetCheck(FALSE);
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(pEditorDoc->m_streamUnit == SU_MILLIS ? TRUE : FALSE);
}

void CStreamView::OnShowMarks() {
    // Häckchen im Pulldown-Menü (Auge-Icon) wurde vom Benutzer gesetzt/entfernt

    m_streamCtrl.ShowMarksStream(!m_streamCtrl.IsMarksStreamShown());
    m_bUserHasChangedMarksVisibility = true;
    RedrawWindow(NULL, NULL, RDW_INVALIDATE);
}

void CStreamView::OnUpdateShowMarks(CCmdUI* pCmdUI) { 
    CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty()) {
        pCmdUI->SetCheck(FALSE);
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(m_streamCtrl && m_streamCtrl.IsMarksStreamShown() ? TRUE : FALSE);
}

void CStreamView::OnShowVideo() {
    m_streamCtrl.ShowVideoStream(!m_streamCtrl.IsVideoStreamShown());
    m_bUserHasChangedVideoVisibility = true;
    RedrawWindow(NULL, NULL, RDW_INVALIDATE);
}

void CStreamView::OnUpdateShowVideo(CCmdUI* pCmdUI) { 
    CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty()) {
        pCmdUI->SetCheck(FALSE);
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(m_streamCtrl && m_streamCtrl.IsVideoStreamShown() ? TRUE : FALSE);
}

void CStreamView::OnShowPages() 
{
    m_streamCtrl.ShowSlidesStream(!m_streamCtrl.IsSlideStreamShown());
	m_bUserHasChangedSlidesVisibility = true;
	RedrawWindow(NULL, NULL, RDW_INVALIDATE);
}

void CStreamView::OnUpdateShowPages(CCmdUI* pCmdUI) { 
	CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
	if (!pEditorDoc || pEditorDoc->project.IsEmpty()) {
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(m_streamCtrl && m_streamCtrl.IsSlideStreamShown() ? TRUE : FALSE);
}

void CStreamView::OnShowClips() {
	m_streamCtrl.ShowClipsStream(!m_streamCtrl.IsClipStreamShown());
	m_bUserHasChangedClipsVisibility = true;
	RedrawWindow(NULL, NULL, RDW_INVALIDATE);
}

void CStreamView::OnUpdateShowClips(CCmdUI* pCmdUI) { 
	CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
	if (!pEditorDoc || pEditorDoc->project.IsEmpty()) {
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(m_streamCtrl && m_streamCtrl.IsClipStreamShown() ? TRUE : FALSE);
}

void CStreamView::OnShowAudio() {
	m_streamCtrl.ShowAudioStream(!m_streamCtrl.IsAudioStreamShown());
	m_bUserHasChangedAudioVisibility = true;
	RedrawWindow(NULL, NULL, RDW_INVALIDATE);
}

void CStreamView::OnUpdateShowAudio(CCmdUI* pCmdUI) { 
	CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
	if (!pEditorDoc || pEditorDoc->project.IsEmpty()) {
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(m_streamCtrl && m_streamCtrl.IsAudioStreamShown() ? TRUE : FALSE);
}

void CStreamView::OnZoomIn() {
    CEditorDoc *pDoc = (CEditorDoc *)GetDocument();
    pDoc->GetMainFrame()->SetPlusMinusKeyZoom(true);
    ZoomIn();
}

void CStreamView::OnZoomOut() {
    ZoomOut();
}

BOOL CStreamView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	// WHEEL_DELTA is 1 second
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
	if (!pDoc || pDoc->project.IsEmpty())
		return FALSE;
	
	SCROLLINFO si;
	ZeroMemory(&si, sizeof SCROLLINFO);
	si.cbSize = sizeof SCROLLINFO;
	m_displayScroll.GetScrollInfo(&si, SIF_ALL);
	
	//int duration =  pDoc->m_displayEndMs - pDoc->m_displayStartMs;
	//int newPos = pDoc->m_displayStartMs;
	//int delta  = ((int) zDelta) * 1000 / WHEEL_DELTA;
	//if ((nFlags & MK_CONTROL) > 0)
	//   delta *= 10;
	//newPos += delta;
	
	int duration =  pDoc->m_displayEndMs - pDoc->m_displayStartMs;
	int newPos = pDoc->m_displayStartMs;
	double deltaRel = ((double) zDelta) / ((double) WHEEL_DELTA);
	if ((nFlags & MK_CONTROL) > 0)
		deltaRel *= 10.0;
	newPos += (int) (0.1 * deltaRel * si.nPage);
	
	if (newPos < 0)
		newPos = 0;
	if (newPos + duration > pDoc->m_docLengthMs)
		newPos = pDoc->m_docLengthMs - duration;
	
	if (pDoc->m_displayStartMs != newPos) {
		pDoc->m_displayStartMs = newPos;
		pDoc->m_displayEndMs = newPos + duration;
		
		UpdateScrollBar();
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
	
	// return CWnd::OnMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}

int CStreamView::GetStreamCtrlWidth() {
    CRect rect;
    m_streamCtrl.GetClientRect(&rect);
    int width = rect.right - rect.left;
    width -= CStreamCtrl::LEFT_BORDER;
    return width;
}

void CStreamView::UpdateStreamVisibilities(CEditorDoc *pDoc)
{
	if (!pDoc->project.IsEmpty()) {
        if (!m_bUserHasChangedMarksVisibility)
            m_streamCtrl.ShowMarksStream(pDoc->project.HasMarks());

        if (!m_bUserHasChangedVideoVisibility)
            m_streamCtrl.ShowVideoStream(pDoc->project.HasVideo());

        if (!m_bUserHasChangedClipsVisibility)
            m_streamCtrl.ShowClipsStream(pDoc->project.HasSgClips());

        if (!m_bUserHasChangedSlidesVisibility)
            m_streamCtrl.ShowSlidesStream(!pDoc->project.IsDenverDocument());

        if (!m_bUserHasChangedAudioVisibility)
            m_streamCtrl.ShowAudioStream(true);
   } else {
	   if (!m_bUserHasChangedMarksVisibility)
		   m_streamCtrl.ShowMarksStream(false);

	   if (!m_bUserHasChangedVideoVisibility)
		   m_streamCtrl.ShowVideoStream(false);
	   
	   if (!m_bUserHasChangedClipsVisibility)
		   m_streamCtrl.ShowClipsStream(false);
	   
	   if (!m_bUserHasChangedSlidesVisibility)
		   m_streamCtrl.ShowSlidesStream(false);
	   
	   if (!m_bUserHasChangedAudioVisibility)
		   m_streamCtrl.ShowAudioStream(false);
   }

    // Do not call RedrawWindow() here as will seriously decrease performace.
   //RedrawWindow();
}

void CStreamView::ShowSlideStream() {
    if (!m_streamCtrl.IsSlideStreamShown()) {
        m_streamCtrl.ShowSlidesStream(true);
        RedrawStreamCtrl(NULL);
        //m_streamCtrl.RedrawWindow(NULL, NULL, RDW_INVALIDATE);
    }
}

void CStreamView::UpdateScrollBar() {
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
    if (!pDoc) {
        return;
    }

    if (pDoc->m_displayEndMs - pDoc->m_displayStartMs >= pDoc->m_docLengthMs) {
        // No, we can see the entire timeline
        // We can set "redraw" to FALSE here, it will be
        // redrawn later anyways
        SCROLLINFO si;
        ZeroMemory(&si, sizeof SCROLLINFO);
        si.cbSize = sizeof SCROLLINFO;
        si.fMask  = SIF_ALL;
        m_displayScroll.SetScrollInfo(&si, FALSE);
        m_displayScroll.EnableWindow(FALSE);
    } else {
        m_displayScroll.EnableWindow(TRUE);
        int displayWidthMs = pDoc->m_displayEndMs - pDoc->m_displayStartMs;
        SCROLLINFO scrollInfo;
        ZeroMemory(&scrollInfo, sizeof SCROLLINFO);
        scrollInfo.cbSize = sizeof SCROLLINFO;
        scrollInfo.nMin   = 0;
        scrollInfo.nMax   = pDoc->m_docLengthMs - displayWidthMs / 2;
        scrollInfo.nPos   = pDoc->m_displayStartMs;
        scrollInfo.nPage  = displayWidthMs / 2;
        scrollInfo.fMask  = SIF_ALL; // | SIF_DISABLENOSCROLL;
        //m_displayScroll.SetScrollRange(scrollInfo.nMin, scrollInfo.nMax, FALSE);
        //m_displayScroll.SetScrollPos(scrollInfo.nPos, FALSE);
        m_displayScroll.SetScrollInfo(&scrollInfo, FALSE);
        //m_displayScroll.EnableScrollBar();
    }
}

void CStreamView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	//MessageBox("Doink");
	//   if (nSBCode == SB_THUMBPOSITION ||
	//       nSBCode == SB_THUMBTRACK)
	SCROLLINFO si;
	ZeroMemory(&si, sizeof SCROLLINFO);
	si.cbSize = sizeof SCROLLINFO;
	m_displayScroll.GetScrollInfo(&si, SIF_ALL | SIF_TRACKPOS);
	
	switch (nSBCode)
	{
	case SB_LEFT:
		break;
		
	case SB_RIGHT:
		break;
		
	case SB_LINELEFT:
		si.nPos -= si.nPage / 10;
		if (si.nPos < 0)
			si.nPos = 0;
		break;
		
	case SB_LINERIGHT:
		si.nPos += si.nPage / 10;
		if (si.nPos > si.nMax)
			si.nPos = si.nMax;
		break;
		
	case SB_PAGELEFT:
		si.nPos -= si.nPage;
		if (si.nPos < 0)
			si.nPos = 0;
		break;
		
	case SB_PAGERIGHT:
		si.nPos += si.nPage;
		if (si.nPos > si.nMax)
			si.nPos = si.nMax;
		break;
		
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
		
	case SB_ENDSCROLL:
		break;
	}
	
    m_displayScroll.SetScrollInfo(&si, TRUE);

    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();

    if (si.nPos != pDoc->m_displayStartMs) {
        UpdateDisplay(si.nPos);
    }

    //TRACE3("si.nPos == %d, nPos == %d, si.nTrackPos == %d\n", si.nPos, nPos, si.nTrackPos);

    CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CStreamView::UpdateDisplay(int newDisplayStart) {
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
    CRect rect;
    m_streamCtrl.GetClientRect(&rect);
    rect.left += CStreamCtrl::LEFT_BORDER;

    int displayWidthMs = pDoc->m_displayEndMs - pDoc->m_displayStartMs;
    //*
    pDoc->m_displayStartMs = newDisplayStart;
    pDoc->m_displayEndMs   = newDisplayStart + displayWidthMs;

    RedrawTimelineCtrl(NULL);
    RedrawStreamCtrl(NULL);
	//m_timelineCtrl.RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	//m_streamCtrl.RedrawWindow(&rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	//*/
	
	/*
	int displayWidthPixel = rect.right - rect.left;
	int deltaMs = newDisplayStart - pDoc->m_displayStartMs;
	int deltaPixel = Calculator::GetPixelFromMsec(deltaMs, displayWidthPixel, displayWidthMs);
	
	  CRect targetRect = rect;
	  CRect rest       = rect;
	  bool doSomething = false;
	  if (deltaPixel > 0)
	  {
      rect.left += deltaPixel;
      targetRect.right -= deltaPixel;
      rest.left = rest.right - deltaPixel*2;
      doSomething = true;
	  }
	  else
	  {
      rect.right += deltaPixel;
      targetRect.left -= deltaPixel;
      rest.right = rest.left - deltaPixel*2;
      doSomething = true;
	  }
	  
		if (doSomething)
		{
		pDoc->m_displayStartMs = newDisplayStart;
		pDoc->m_displayEndMs   = newDisplayStart + displayWidthMs;
		CDC *pDC = m_streamCtrl.GetDC();
		pDC->BitBlt(targetRect.left, targetRect.top, targetRect.right - targetRect.left, targetRect.bottom - targetRect.top, 
		pDC, rect.left, rect.top, SRCCOPY);
		m_streamCtrl.RedrawWindow(&rest);
		}
	//*/
}

void CStreamView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
	if (!pDoc->project.IsEmpty() &&
		(nChar == VK_RIGHT || nChar == VK_LEFT))
	{
		CRect clientRect;
		
		int msDelta = 0;
		if (nChar == VK_RIGHT || nChar == VK_LEFT)
		{
			GetClientRect(&clientRect);
			clientRect.left += CStreamCtrl::LEFT_BORDER;
			
			if (clientRect.Width() <= 0)
				return;
			
			msDelta = Calculator::GetMsecFromPixel
				(2, clientRect.Width(), pDoc->m_displayEndMs - pDoc->m_displayStartMs);
		}
		
		if (m_controlIsPressed == 1)
			msDelta *= 10;
		
		if (nChar == VK_LEFT)
			msDelta = -msDelta;
		
		if (m_shiftIsPressed == 0) // navigation
		{
			pDoc->m_curPosMs += msDelta;
			if (pDoc->m_curPosMs < 0)
				pDoc->m_curPosMs = 0;
			else if (pDoc->m_curPosMs > pDoc->m_docLengthMs)
				pDoc->m_curPosMs = pDoc->m_docLengthMs;

            if (CMainFrameE::GetCurrentInstance()->PreviewInNormalMode())
                pDoc->JumpPreview(pDoc->m_curPosMs);
			pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
			return;
		}
		else // Shift is pressed: selection
		{
			
			if (m_keyMarkStartMs == -1 ||
				m_keyMarkEndMs   == -1)
			{
				m_keyMarkStartMs = pDoc->m_curPosMs;
				m_keyMarkEndMs   = m_keyMarkStartMs + msDelta;
			}
			else
			{
				m_keyMarkEndMs  += msDelta;
			}
			
			if (m_keyMarkEndMs >= pDoc->m_docLengthMs)
				m_keyMarkEndMs = pDoc->m_docLengthMs;
			else if (m_keyMarkEndMs < 0)
				m_keyMarkEndMs = 0;
			
			if (m_keyMarkStartMs <= m_keyMarkEndMs)
			{
				pDoc->m_markStartMs = m_keyMarkStartMs;
				pDoc->m_markEndMs   = m_keyMarkEndMs;
			}
			else
			{
				pDoc->m_markStartMs = m_keyMarkEndMs;
				pDoc->m_markEndMs   = m_keyMarkStartMs;
			}
			
			RedrawWindow(NULL, NULL, RDW_INVALIDATE);
		}
	}
	else if (nChar == VK_CONTROL)
	{
		m_controlIsPressed = 1;
	}
	else if (nChar == VK_SHIFT)
	{
		m_shiftIsPressed = 1;
		m_keyMarkStartMs = pDoc->m_markStartMs;
		m_keyMarkEndMs   = pDoc->m_markEndMs;
	}
	else
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CStreamView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	switch (nChar)
	{
	case VK_CONTROL:
		m_controlIsPressed = 0;
		break;
		
	case VK_SHIFT:
		m_shiftIsPressed = 0;
		m_keyMarkStartMs = -1;
		m_keyMarkEndMs   = -1;
		break;
		
	default:
		CView::OnKeyUp(nChar, nRepCnt, nFlags);
	}
}

BOOL CStreamView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
	if (pDoc->project.IsEmpty() || nHitTest != HTCLIENT)
		return CView::OnSetCursor(pWnd, nHitTest, message);
	
	CRect windowRect;
	m_timelineCtrl.GetWindowRect(&windowRect);
	POINT ptPoint;
	BOOL bResult = ::GetCursorPos(&ptPoint);
	bool bShowTimelineCursor = (windowRect.PtInRect(ptPoint) != 0);
	
	if (bShowTimelineCursor)
		SetCursor(m_hPositionCursor);
	else
		return CView::OnSetCursor(pWnd, nHitTest, message);
	
	return TRUE;
}

void CStreamView::ResetUserVariables()
{
	m_bUserHasChangedMarksVisibility = false;
	m_bUserHasChangedVideoVisibility = false;
	m_bUserHasChangedSlidesVisibility = false;
	m_bUserHasChangedClipsVisibility = false;
	m_bUserHasChangedAudioVisibility = false;
}

HRESULT CStreamView::InsertStopmark(int iCheckID, int iTimestamp) {
    HRESULT hr = m_streamCtrl.InsertStopmark(iCheckID, iTimestamp);
    OnUpdate(this, HINT_CHANGE_STRUCTURE, NULL);
    return hr;
}

HRESULT CStreamView::InsertTargetmark(CString &csName, int iCheckID, int iTimestamp) {
	HRESULT hr = m_streamCtrl.InsertTargetmark(csName, iCheckID, iTimestamp);
    OnUpdate(this, HINT_CHANGE_STRUCTURE, NULL);
	return hr;
}

void CStreamView::ShowChildWindows(bool bShow)
{
    if (bShow)
    {
        if (!m_displayScroll.IsWindowVisible())
            m_displayScroll.ShowWindow(SW_SHOW);
        if (!m_wndToolBar.IsWindowVisible())
            m_wndToolBar.ShowWindow(SW_SHOW);
    }
    else
    {
        if (m_displayScroll.IsWindowVisible())
            m_displayScroll.ShowWindow(SW_HIDE);
        if (m_wndToolBar.IsWindowVisible())
            m_wndToolBar.ShowWindow(SW_HIDE);
    }

}

void CStreamView::UpdateSliderPos(CPoint &point, UINT nFlags, MouseStateId idMouseState)
{	
    CEditorDoc *pEditorDoc = (CEditorDoc *) GetDocument();
    if (pEditorDoc->project.IsEmpty())
        return;

    CRect rcClient;
    GetClientRect(&rcClient);

    double displayWidthMs = pEditorDoc->m_displayEndMs - pEditorDoc->m_displayStartMs;
    double displayWidthPx = rcClient.Width() - LEFT_OFFSET;

    int iCurrentMs = pEditorDoc->m_curPosMs;

    int newPos = pEditorDoc->m_displayStartMs + 
        (int) ((((double) point.x) / displayWidthPx) * displayWidthMs);
    if (newPos < 0)
        newPos = 0;
    else if (newPos > pEditorDoc->m_docLengthMs)
        newPos = pEditorDoc->m_docLengthMs;
    pEditorDoc->m_curPosMs = newPos;


    if (pEditorDoc->IsPreviewActive() && !pEditorDoc->IsPausedSpecial())
    {
        pEditorDoc->PausePreview();
        m_bHavePaused = true;
    }

    int iCurrentDisplayStart = pEditorDoc->m_displayStartMs;

    if (iCurrentMs != pEditorDoc->m_curPosMs) {
       pEditorDoc->JumpPreview(pEditorDoc->m_curPosMs);
    }

    // if zoomed and the new position is outside the zoomed range
    // there will be an automatic range update
    bool bDisplayRangeChanged = pEditorDoc->m_displayStartMs != iCurrentDisplayStart;

    if (m_bHavePaused && idMouseState == STATE_MOUSEUP)
    {
        // TODO: in the beginning nothing is to be displayed, no video exists
        // and this method returns an error code (render engine broken)
        pEditorDoc->StartPreview();
        m_bHavePaused = false;
    }

    m_iCursorPositionPixel = -1;
    pEditorDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION); 
}

void CStreamView::UpdateSliderPos(bool bErase)
{
    CEditorDoc *pEditorDoc = (CEditorDoc *) GetDocument();
    if (pEditorDoc->project.IsEmpty())
        return;

    CRect rcClient;
    GetClientRect(&rcClient);

    CDC *pDC = GetDC();
    CXTMemDC memDC(pDC, rcClient, -1);


    CRect rcStreamsAndTimeline(m_rcTimelineCtrl.left, m_rcTimelineCtrl.top, 
        m_rcStreamCtrl.right, m_rcStreamCtrl.bottom);

    memDC.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
        pDC, rcClient.left, rcClient.top, SRCCOPY); 

    memDC.SelectClipRgn(NULL);

    // Delete old slider
    if (bErase && m_lastDisplayPos >= 0)
    {
        CRgn oldSliderRgn;
        int iFirstPos = m_lastDisplayPos-20;
        if (iFirstPos < LEFT_OFFSET)
            iFirstPos = LEFT_OFFSET;
        oldSliderRgn.CreateRectRgn(iFirstPos, rcStreamsAndTimeline.top, 
            m_lastDisplayPos+20, rcStreamsAndTimeline.bottom);
        memDC.SelectClipRgn(&oldSliderRgn);

        m_timelineCtrl.DrawItem(m_rcTimelineCtrl, &memDC);
        m_streamCtrl.DrawItem(m_rcStreamCtrl, &memDC);

        memDC.SelectClipRgn(NULL);
        oldSliderRgn.DeleteObject();
        m_lastDisplayPos = -1;
    }

    // Draw new slider
    int displayPosRelMs = pEditorDoc->m_curPosMs - pEditorDoc->m_displayStartMs;
    if (displayPosRelMs >= 0 && pEditorDoc->m_curPosMs <= pEditorDoc->m_displayEndMs)
    {
        // calculate new position
        double displayWidthMs = pEditorDoc->m_displayEndMs - pEditorDoc->m_displayStartMs;
        double displayWidthPx = rcClient.Width() - LEFT_OFFSET;
        int x = (int) ((((double) displayPosRelMs) / displayWidthMs) * displayWidthPx) + LEFT_OFFSET;


        CRgn newSliderRgn;
        int iFirstPos = x - 20;
        if (iFirstPos < LEFT_OFFSET)
            iFirstPos = LEFT_OFFSET;
        newSliderRgn.CreateRectRgn(iFirstPos, rcStreamsAndTimeline.top, 
            x + 20, rcStreamsAndTimeline.bottom);
        memDC.SelectClipRgn(&newSliderRgn);

        CDC bmpDC;
        bmpDC.CreateCompatibleDC(&memDC);
        bmpDC.SelectObject(m_bmpSlider.m_hObject);
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 0x9f;
        bf.AlphaFormat = 0;

        TransparentBlt(memDC.m_hDC, x-20, rcStreamsAndTimeline.top, 40, rcStreamsAndTimeline.bottom, 
            bmpDC.m_hDC, 0, 0, 40, rcStreamsAndTimeline.bottom, RGB(0, 0, 0)); 

        memDC.SelectClipRgn(NULL);
        newSliderRgn.DeleteObject();
        m_lastDisplayPos = x;

    }

    CRgn newUpdateRgn;
    newUpdateRgn.CreateRectRgn(rcStreamsAndTimeline.left, rcStreamsAndTimeline.top, 
        rcStreamsAndTimeline.right, rcStreamsAndTimeline.bottom);
    memDC.SelectClipRgn(&newUpdateRgn);
    ReleaseDC(pDC);

}

void CStreamView::SetInactive()
{
    CEditorDoc *pEditorDoc = (CEditorDoc *) GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty())
        return;

    pEditorDoc->m_markStartMs = -1;
    pEditorDoc->m_markEndMs   = -1;

    RedrawWindow();
}

void CStreamView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
    if (bActivate) 
    {
        CMainFrameE *pMainFrame = CMainFrameE::GetCurrentInstance();
        pMainFrame->ChangeActiveView(pActivateView, pDeactiveView);
    }

    CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CStreamView::RedrawTimelineCtrl(CDC *pDC)
{
    bool bDcCreated = false;
    if (pDC == NULL)
    {
        pDC = GetDC();
        bDcCreated = true;
    }

    CRect rcTimeline = m_rcTimelineCtrl;
    rcTimeline.OffsetRect(-m_rcTimelineCtrl.left, -m_rcTimelineCtrl.top);
    CXTMemDC *pDcTimeline = new CXTMemDC(pDC, m_rcTimelineCtrl, -1);
    m_timelineCtrl.DrawItem(rcTimeline, pDcTimeline);

    delete pDcTimeline;
    if (bDcCreated)
        ReleaseDC(pDC);
}

void CStreamView::RedrawStreamCtrl(CDC *pDC)
{
    bool bDcCreated = false;
    if (pDC == NULL)
    {
        pDC = GetDC();
        bDcCreated = true;
    }

    CRect rcStream = m_rcStreamCtrl;
    rcStream.OffsetRect(-m_rcStreamCtrl.left, -m_rcStreamCtrl.top);
    CXTMemDC *pDcStream = new CXTMemDC(pDC, m_rcStreamCtrl, -1);
    m_streamCtrl.DrawItem(rcStream, pDcStream);

    delete pDcStream;
    if (bDcCreated)
        ReleaseDC(pDC);

}


void CStreamView::ZoomTo(int nZoom)
{
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();

    if (!pDoc || pDoc->m_docLengthMs == -1)
        return;

    double offSet = 1;
    if ( nZoom > 100 )
    {
        double lMultiply = ((double)400/(double)(pDoc->m_docLengthMs/1000));
        nZoom = nZoom - 100;
        offSet = (double) nZoom / lMultiply;
    } else if ( nZoom <= 100 )
        offSet = 0;

    //int startMs = 0;
    //int endMs   = pDoc->m_docLengthMs;
    int startMs = pDoc->m_displayStartMs;
    int endMs   = pDoc->m_displayEndMs;
    int ctrlWidth = GetStreamCtrlWidth();

    // calculate new display width in msec
    int diff = (int)((double)pDoc->m_docLengthMs - offSet* 1000);
    // width should not be smaller then 10 sec
    if (diff < MIN_DISPLAY_SEC)
        diff = MIN_DISPLAY_SEC;

    if (pDoc->m_curPosMs > startMs && pDoc->m_curPosMs < endMs)
    {
        // calculate curPos in pixel
        int curPosPixel  = Calculator::GetPixelFromMsec(pDoc->m_curPosMs, ctrlWidth, endMs-startMs, pDoc->m_displayStartMs);
        int newCurPosPixel  = Calculator::GetPixelFromMsec(pDoc->m_curPosMs, ctrlWidth, diff, pDoc->m_displayStartMs);

        int offset = newCurPosPixel - curPosPixel;
        int offsetMs = Calculator::GetMsecFromPixel(offset, ctrlWidth, diff);

        startMs += offsetMs;
        if ( startMs < 0 )
            startMs = 0;
        endMs = startMs + diff;
        if (endMs > pDoc->m_docLengthMs)
        {
            endMs = pDoc->m_docLengthMs;
            startMs = endMs - diff;
        }
    }
    else if (pDoc->m_curPosMs == endMs)
    {
        startMs = endMs - diff;
        if (startMs < 0)
        {
            startMs = 0;
            endMs = startMs + diff;
            if (endMs > pDoc->m_docLengthMs)
                endMs = pDoc->m_docLengthMs;
        }
    }
    else // if (pDoc->m_curPosMs == startMs)
    {
        endMs = startMs + diff;
        if (endMs > pDoc->m_docLengthMs)
        {
            endMs = pDoc->m_docLengthMs;
            startMs = endMs - diff;
            if (startMs < 0)
                startMs = 0;
        }
    }

    // calculate new curPos in pixel
    pDoc->m_displayStartMs = startMs;
    pDoc->m_displayEndMs   = endMs;

    OnUpdate(this, HINT_CHANGE_POSITION, NULL);
}

void CStreamView::ZoomIn()
{
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();

    if (!pDoc)
        return;

    int startMs = pDoc->m_displayStartMs;
    int endMs   = pDoc->m_displayEndMs;
    int ctrlWidth = GetStreamCtrlWidth();

    // calculate new display width in msec
    int diff = endMs - startMs;
    diff = (int)((double)diff / 2.0);
    // width should not be smaller then 10 sec
    if (diff < MIN_DISPLAY_SEC)
        diff = MIN_DISPLAY_SEC;

    if (pDoc->m_curPosMs > startMs && pDoc->m_curPosMs < endMs)
    {
        // calculate curPos in pixel
        int curPosPixel  = Calculator::GetPixelFromMsec(pDoc->m_curPosMs, ctrlWidth, endMs-startMs, pDoc->m_displayStartMs);
        int newCurPosPixel  = Calculator::GetPixelFromMsec(pDoc->m_curPosMs, ctrlWidth, diff, pDoc->m_displayStartMs);

        int offset = newCurPosPixel - curPosPixel;
        int offsetMs = Calculator::GetMsecFromPixel(offset, ctrlWidth, diff);

        startMs += offsetMs;
        endMs = startMs + diff;
        if (endMs > pDoc->m_docLengthMs)
        {
            endMs = pDoc->m_docLengthMs;
            startMs = endMs - diff;
        }
    }
    else if (pDoc->m_curPosMs == endMs)
    {
        startMs = endMs - diff;
        if (startMs < 0)
        {
            startMs = 0;
            endMs = startMs + diff;
            if (endMs > pDoc->m_docLengthMs)
                endMs = pDoc->m_docLengthMs;
        }
    }
    else // if (pDoc->m_curPosMs == startMs)
    {
        endMs = startMs + diff;
        if (endMs > pDoc->m_docLengthMs)
        {
            endMs = pDoc->m_docLengthMs;
            startMs = endMs - diff;
            if (startMs < 0)
                startMs = 0;
        }
    }

    // calculate new curPos in pixel
    pDoc->m_displayStartMs = startMs;
    pDoc->m_displayEndMs   = endMs;

    OnUpdate(this, HINT_CHANGE_POSITION, NULL);
}

void CStreamView::ZoomOut()
{
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();

    if (!pDoc)
        return;

    int startMs = pDoc->m_displayStartMs;
    int endMs   = pDoc->m_displayEndMs;
    int ctrlWidth = GetStreamCtrlWidth();

    int diff = endMs - startMs;
    diff *= 2;
    if (diff > pDoc->m_docLengthMs)
        diff = pDoc->m_docLengthMs;

    // calculate curPos in pixel

    if (pDoc->m_curPosMs > startMs && pDoc->m_curPosMs < endMs)
    {
        // calculate new curPos in pixel
        int curPosPixel    = Calculator::GetPixelFromMsec(pDoc->m_curPosMs, ctrlWidth, endMs-startMs, pDoc->m_displayStartMs);
        int newCurPosPixel = Calculator::GetPixelFromMsec(pDoc->m_curPosMs, ctrlWidth, diff, pDoc->m_displayStartMs);

        int offset = newCurPosPixel - curPosPixel;
        int offsetMs = Calculator::GetMsecFromPixel(offset, ctrlWidth, diff);

        startMs += offsetMs;
        if (startMs < 0)
            startMs = 0;
        endMs = startMs + diff;
        if (endMs > pDoc->m_docLengthMs)
        {
            endMs = pDoc->m_docLengthMs;
            startMs = endMs - diff;
        }
    }
    else if (pDoc->m_curPosMs == endMs)
        startMs = endMs - diff;
    else
    {
        endMs = startMs + diff;
        if (endMs > pDoc->m_docLengthMs)
        {
            endMs = pDoc->m_docLengthMs;
            startMs = endMs - diff;
        }
    }

    pDoc->m_displayStartMs = startMs;
    pDoc->m_displayEndMs   = endMs;

    OnUpdate(this, HINT_CHANGE_POSITION, NULL);
}

void CStreamView::ZoomAll()
{
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();

    if (!pDoc)
        return;

    pDoc->m_displayStartMs = 0;
    pDoc->m_displayEndMs   = pDoc->project.GetTotalLength();

    OnUpdate(this, HINT_CHANGE_POSITION, NULL);
}

void CStreamView::ZoomSelected()
{
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();

    if (!pDoc)
        return;

    int startMs = pDoc->m_markStartMs;
    int endMs   = pDoc->m_markEndMs;

    int diff = endMs - startMs;
    if (diff < MIN_DISPLAY_SEC)
    {
        int offset = (int)((double)(MIN_DISPLAY_SEC - diff) / 2.0);
        diff = MIN_DISPLAY_SEC;
        startMs -= offset;
        if (startMs < 0)
            startMs = 0;
        if (startMs + diff > pDoc->m_docLengthMs)
            startMs = pDoc->m_docLengthMs - diff;
        endMs = startMs + diff;
    }

    pDoc->m_displayStartMs = startMs;
    pDoc->m_displayEndMs   = endMs;

    OnUpdate(this, HINT_CHANGE_POSITION, NULL);
}

void CStreamView::ColorSchemeChanged() {
    m_streamCtrl.ColorSchemeChanged();
    m_timelineCtrl.ColorSchemeChanged();
}
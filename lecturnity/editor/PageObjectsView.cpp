#include "stdafx.h"
#include "PageObjectsView.h"
#include "editorDoc.h"
#include "MainFrm.h"
#include "WhiteboardView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPageObjectsView

IMPLEMENT_DYNCREATE(CPageObjectsView, CView)

CPageObjectsView::CPageObjectsView() {
    m_rgnUpdate.CreateRectRgn(0, 0, 0, 0);
    m_bFirstResize = true;
}

CPageObjectsView::~CPageObjectsView() {
}

BEGIN_MESSAGE_MAP(CPageObjectsView, CView)
    //{{AFX_MSG_MAP(CPageObjectsView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_MESSAGE( WM_IDLEUPDATECMDUI, OnIdleUpdateUIMessage )
    ON_COMMAND(IDC_BUTTON_FORWARD, OnForward)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_FORWARD, OnUpdateForward)
    ON_COMMAND(IDC_BUTTON_BACKWARD, OnBackward)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_BACKWARD, OnUpdateBackward)
    ON_COMMAND(IDC_BUTTON_FRONT, OnFront)
    ON_COMMAND(IDC_BUTTON_BACK, OnBack)
    ON_COMMAND(IDC_BUTTON_PROPERTIES, OnProperties)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_PROPERTIES, OnUpdateProperties)
    ON_COMMAND(IDC_BUTTON_RENAME, OnRename)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_RENAME, OnUpdateRename)
    ON_COMMAND(IDC_BUTTON_DELETE, OnDelete)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_DELETE, OnUpdateDelete)
    ON_WM_ERASEBKGND()
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_FRONT, OnUpdateForward)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_BACK, OnUpdateBackward)
    ON_WM_KEYDOWN()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPageObjectsView 

int CPageObjectsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CView::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    CRect rcClient;
    GetClientRect(&rcClient);

    // create list
    CRect listRect(rcClient);
    listRect.top += LIST_OFFSET;
    listRect.bottom -= BUTTONS_HEIGHT;
    listRect.left += LIST_OFFSET;
    listRect.right -= LIST_OFFSET;

    m_listPageObjects.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_EDITLABELS, 
        listRect, this, 0);  // | LVS_OWNERDRAWFIXED
    m_listPageObjects.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    CEditorDoc *pDoc = (CEditorDoc *)GetDocument();
    m_listPageObjects.SetEditorDoc(pDoc);

    m_ilStateImages.Create(IDB_ICON_EYE, 13, 1, RGB(255, 255, 255));
    m_listPageObjects.SetImageList(&m_ilStateImages, LVSIL_STATE);

    LV_COLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    lvc.iSubItem = 0;
    lvc.pszText = _T("");
    lvc.cx = 18;
    lvc.fmt = LVCFMT_LEFT;
    m_listPageObjects.InsertColumn(0,&lvc);

    int nColumnWidth = (int)((listRect.Width() - 18) / 2);

    m_csNameColumn.LoadString(IDS_NAME);

    lvc.iSubItem = 1;
    lvc.pszText = (_TCHAR *)(LPCTSTR)m_csNameColumn;
    lvc.cx = 0;
    lvc.fmt = LVCFMT_LEFT;
    m_listPageObjects.InsertColumn(1,&lvc);

    m_csTypeColumn.LoadString(IDS_TYPE);

    lvc.iSubItem = 2;
    lvc.pszText = (_TCHAR *)(LPCTSTR)m_csTypeColumn;
    lvc.cx = 0;
    lvc.fmt = LVCFMT_LEFT;
    m_listPageObjects.InsertColumn(2,&lvc);

    // create buttons

    CRect rcButton;
    CString csButtonText;
    csButtonText.LoadString(IDC_BUTTON_FORWARD);
    GetButtonRect(rcButton, IDC_BUTTON_FORWARD);
    m_btnForward.Create(csButtonText, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rcButton, this, IDC_BUTTON_FORWARD);

    csButtonText.LoadString(IDC_BUTTON_BACKWARD);
    GetButtonRect(rcButton, IDC_BUTTON_BACKWARD);
    m_btnBackward.Create(csButtonText, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rcButton, this, IDC_BUTTON_BACKWARD);

    csButtonText.LoadString(IDC_BUTTON_FRONT);
    GetButtonRect(rcButton, IDC_BUTTON_FRONT);
    m_btnFront.Create(csButtonText, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rcButton, this, IDC_BUTTON_FRONT);

    csButtonText.LoadString(IDC_BUTTON_BACK);
    GetButtonRect(rcButton, IDC_BUTTON_BACK);
    m_btnBack.Create(csButtonText, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rcButton, this, IDC_BUTTON_BACK);

    GetButtonRect(rcButton, 0);
    m_wndSeparator.Create(NULL, WS_VISIBLE | WS_CHILD | SS_SUNKEN, rcButton, this, 0);

    csButtonText.LoadString(IDC_BUTTON_PROPERTIES);
    GetButtonRect(rcButton, IDC_BUTTON_PROPERTIES);
    m_btnProperties.Create(csButtonText, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rcButton, this, IDC_BUTTON_PROPERTIES);

    csButtonText.LoadString(IDC_BUTTON_RENAME);
    GetButtonRect(rcButton, IDC_BUTTON_RENAME);
    m_btnRename.Create(csButtonText, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rcButton, this, IDC_BUTTON_RENAME);

    csButtonText.LoadString(IDC_BUTTON_DELETE);
    GetButtonRect(rcButton, IDC_BUTTON_DELETE);
    m_btnDelete.Create(csButtonText, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rcButton, this, IDC_BUTTON_DELETE);

    RefreshButtonFont();

    RefreshPageObjectList();

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CPageObjectsView 

void CPageObjectsView::OnDraw(CDC* pDC) {
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    CRect crClient;
    GetClientRect(crClient);

    bool bProjectEmpty = pDoc == NULL || pDoc->project.IsEmpty();

    COLORREF clrBackground = CMainFrameE::GetColor(CMainFrameE::COLOR_BG_LIGHT);
    if (bProjectEmpty) {
        clrBackground = CMainFrameE::GetColor(CMainFrameE::COLOR_BG_DARK);
    }

    CBrush bgBrush(clrBackground);

    if (!bProjectEmpty) {
        pDC->FillRgn(&m_rgnUpdate, &bgBrush);
        ShowChildWindows(true);
    } else {
        ShowChildWindows(false);
        pDC->FillRect(&crClient, &bgBrush);
    }

    bgBrush.DeleteObject();
}

void CPageObjectsView::ShowChildWindows(bool bShow) {
    if (bShow) {
        if (!m_listPageObjects.IsWindowVisible())
            m_listPageObjects.ShowWindow(SW_SHOW);
        if (!m_btnForward.IsWindowVisible())
            m_btnForward.ShowWindow(SW_SHOW);
        if (!m_btnBackward.IsWindowVisible())
            m_btnBackward.ShowWindow(SW_SHOW);
        if (!m_btnFront.IsWindowVisible())
            m_btnFront.ShowWindow(SW_SHOW);
        if (!m_btnBack.IsWindowVisible())
            m_btnBack.ShowWindow(SW_SHOW);
        if (!m_btnProperties.IsWindowVisible())
            m_btnProperties.ShowWindow(SW_SHOW);
        if (!m_btnRename.IsWindowVisible())
            m_btnRename.ShowWindow(SW_SHOW);
        if (!m_btnDelete.IsWindowVisible())
            m_btnDelete.ShowWindow(SW_SHOW);
        if (!m_wndSeparator.IsWindowVisible())
            m_wndSeparator.ShowWindow(SW_SHOW);
    } else {
        if (m_listPageObjects.IsWindowVisible())
            m_listPageObjects.ShowWindow(SW_HIDE);
        if (m_btnForward.IsWindowVisible())
            m_btnForward.ShowWindow(SW_HIDE);
        if (m_btnBackward.IsWindowVisible())
            m_btnBackward.ShowWindow(SW_HIDE);
        if (m_btnFront.IsWindowVisible())
            m_btnFront.ShowWindow(SW_HIDE);
        if (m_btnBack.IsWindowVisible())
            m_btnBack.ShowWindow(SW_HIDE);
        if (m_btnProperties.IsWindowVisible())
            m_btnProperties.ShowWindow(SW_HIDE);
        if (m_btnRename.IsWindowVisible())
            m_btnRename.ShowWindow(SW_HIDE);
        if (m_btnDelete.IsWindowVisible())
            m_btnDelete.ShowWindow(SW_HIDE);
        if (m_wndSeparator.IsWindowVisible())
            m_wndSeparator.ShowWindow(SW_HIDE);
    }
}

void CPageObjectsView::GetListRect(CRect &rcList) {
    GetClientRect(&rcList);

    rcList.top += LIST_OFFSET;
    rcList.bottom -= BUTTONS_HEIGHT;
    rcList.left += LIST_OFFSET;
    rcList.right -= LIST_OFFSET;
}

void CPageObjectsView::GetButtonRect(CRect &rcButton, int nID) {
    rcButton.SetRectEmpty();

    CRect rcButtons;
    GetClientRect(rcButtons);

    rcButtons.top = rcButtons.bottom - BUTTONS_HEIGHT;
    rcButtons.left += LIST_OFFSET;
    rcButtons.right -= LIST_OFFSET;

    int nButtonLeftOffset = 0;
    int nButtonWidth = 0;
    int nButtonTopOffset = 0;
    int nButtonHeight = BUTTON_HEIGHT;

    int nSmallButtonWidth = (int)((double)(rcButtons.Width() - BUTTON_SPACE) * ((double)80 / 185));
    int nMajorButtonWidth = (int)((double)(rcButtons.Width() - BUTTON_SPACE) * ((double)105 / 185));
    switch (nID) {
    case IDC_BUTTON_FORWARD:
        nButtonLeftOffset = 0;
        nButtonWidth = nSmallButtonWidth;
        nButtonTopOffset = LIST_OFFSET;
        break;
    case IDC_BUTTON_FRONT:
        nButtonLeftOffset = nSmallButtonWidth + BUTTON_SPACE;
        nButtonWidth = nMajorButtonWidth;
        nButtonTopOffset = LIST_OFFSET;
        break;
    case IDC_BUTTON_BACKWARD:
        nButtonLeftOffset = 0;
        nButtonWidth = nSmallButtonWidth;
        nButtonTopOffset = LIST_OFFSET + BUTTON_HEIGHT + BUTTON_SPACE;
        break;
    case IDC_BUTTON_BACK:
        nButtonLeftOffset = nSmallButtonWidth + BUTTON_SPACE;
        nButtonWidth = nMajorButtonWidth;
        nButtonTopOffset = LIST_OFFSET + BUTTON_HEIGHT + BUTTON_SPACE;
        break;
    case 0:	// Separator
        nButtonLeftOffset = 0;
        nButtonWidth = rcButtons.Width();
        nButtonTopOffset = LIST_OFFSET + 2 * (BUTTON_HEIGHT + BUTTON_SPACE);
        nButtonHeight = 1;
        break;
    case IDC_BUTTON_PROPERTIES:
        nButtonLeftOffset = 0;
        nButtonWidth = rcButtons.Width();
        nButtonTopOffset = LIST_OFFSET + 2 * (BUTTON_HEIGHT + BUTTON_SPACE) + BUTTON_SPACE;
        break;
    case IDC_BUTTON_RENAME:
        nButtonLeftOffset = 0;
        nButtonWidth = nSmallButtonWidth;
        nButtonTopOffset = LIST_OFFSET + 3 * (BUTTON_HEIGHT + BUTTON_SPACE) + BUTTON_SPACE;
        break;
    case IDC_BUTTON_DELETE:
        nButtonLeftOffset = nSmallButtonWidth + BUTTON_SPACE;
        nButtonWidth = nMajorButtonWidth;
        nButtonTopOffset = LIST_OFFSET + 3 * (BUTTON_HEIGHT + BUTTON_SPACE) + BUTTON_SPACE;
        break;
    }

    rcButton.SetRect(rcButtons.left + nButtonLeftOffset, rcButtons.top + nButtonTopOffset, 
        rcButtons.left + nButtonLeftOffset + nButtonWidth, rcButtons.top + nButtonTopOffset + nButtonHeight);
}

void CPageObjectsView::RenameSelectedObject() {
    int nSelectedItem = m_listPageObjects.GetSelectedItem();
    if (nSelectedItem != -1) {
        m_listPageObjects.SetFocus();
        CEdit *pEdit = m_listPageObjects.EditLabel(nSelectedItem);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CPageObjectsView

#ifdef _DEBUG
void CPageObjectsView::AssertValid() const {
    CView::AssertValid();
}

void CPageObjectsView::Dump(CDumpContext& dc) const {
    CView::Dump(dc);
}
#endif //_DEBUG

void CPageObjectsView::OnSize(UINT nType, int cx, int cy) {
    // resize columns
    int nColumnWidth[3];
    if (m_bFirstResize) {
        nColumnWidth[0] = 18;
        nColumnWidth[1] = (cx - 18) / 2;
        nColumnWidth[2] = cx - (nColumnWidth[0] + nColumnWidth[1]);
    } else {
        int nWidth = 0;
        for (int i = 0; i < 3; ++i) {
            nColumnWidth[i] = m_listPageObjects.GetColumnWidth(i);
            nWidth += nColumnWidth[i];
        }
        CRect rcClient;
        m_listPageObjects.GetClientRect(&rcClient);
        if (rcClient.Width() == 0) {
            int nNewWidth = cx - 2 * LIST_OFFSET;
            nColumnWidth[0] = 18;
            nColumnWidth[1] = (nNewWidth - 18) / 2;
            nColumnWidth[2] = nNewWidth - (nColumnWidth[0] + nColumnWidth[1]);
        } else if (nWidth == rcClient.Width()) {
            nColumnWidth[2] = 0;
        }
    }

    CView::OnSize(nType, cx, cy);

    CRect rcClient;
    GetClientRect(&rcClient);

    m_rgnUpdate.SetRectRgn(&rcClient);

    CRect rcList(rcClient);
    rcList.top += LIST_OFFSET;
    rcList.bottom -= BUTTONS_HEIGHT;
    rcList.left += LIST_OFFSET;
    rcList.right -= LIST_OFFSET;

    m_listPageObjects.SetWindowPos(NULL, rcList.left, rcList.top, rcList.Width(), rcList.Height(), SWP_NOZORDER);

    CRgn rgnRect;
    rgnRect.CreateRectRgn(rcList.left, rcList.top, rcList.right, rcList.bottom);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRect, RGN_DIFF);


    m_listPageObjects.SetColumnWidth(0, nColumnWidth[0]);
    m_listPageObjects.SetColumnWidth(1, nColumnWidth[1]);
    if (nColumnWidth[2] == 0) {
        m_listPageObjects.SetColumnWidth(2, cx - (nColumnWidth[0] + nColumnWidth[1]));
    } else { 
        m_listPageObjects.SetColumnWidth(2, nColumnWidth[2]);
    }

    // resize Buttons
    CRect rcButton;

    GetButtonRect(rcButton, IDC_BUTTON_FORWARD);
    m_btnForward.SetWindowPos(NULL, rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), SWP_NOZORDER);
    rgnRect.SetRectRgn(&rcButton);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRect, RGN_DIFF);

    GetButtonRect(rcButton, IDC_BUTTON_BACKWARD);
    m_btnBackward.SetWindowPos(NULL, rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), SWP_NOZORDER);
    rgnRect.SetRectRgn(&rcButton);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRect, RGN_DIFF);

    GetButtonRect(rcButton, IDC_BUTTON_FRONT);
    m_btnFront.SetWindowPos(NULL, rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), SWP_NOZORDER);
    rgnRect.SetRectRgn(&rcButton);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRect, RGN_DIFF);

    GetButtonRect(rcButton, IDC_BUTTON_BACK);
    m_btnBack.SetWindowPos(NULL, rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), SWP_NOZORDER);
    rgnRect.SetRectRgn(&rcButton);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRect, RGN_DIFF);

    GetButtonRect(rcButton, 0);
    m_wndSeparator.SetWindowPos(NULL, rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), SWP_NOZORDER);
    rgnRect.SetRectRgn(&rcButton);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRect, RGN_DIFF);

    GetButtonRect(rcButton, IDC_BUTTON_PROPERTIES);
    m_btnProperties.SetWindowPos(NULL, rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), SWP_NOZORDER);
    rgnRect.SetRectRgn(&rcButton);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRect, RGN_DIFF);

    GetButtonRect(rcButton, IDC_BUTTON_RENAME);
    m_btnRename.SetWindowPos(NULL, rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), SWP_NOZORDER);
    rgnRect.SetRectRgn(&rcButton);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRect, RGN_DIFF);

    GetButtonRect(rcButton, IDC_BUTTON_DELETE);
    m_btnDelete.SetWindowPos(NULL, rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), SWP_NOZORDER);
    rgnRect.SetRectRgn(&rcButton);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRect, RGN_DIFF);

    m_bFirstResize = false;
}

void CPageObjectsView::OnForward() {
    m_listPageObjects.MoveForward();
    GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
}

void CPageObjectsView::OnUpdateForward(CCmdUI *pCmdUI) {
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    UINT nSelectedItems = m_listPageObjects.GetSelectedCount();
    if (nSelectedItems == 1) {
        int nSelectedItem = m_listPageObjects.GetSelectedItem();
        if (pDoc->IsPreviewActive() || nSelectedItem == -1 || nSelectedItem == 0) { // no item selected or first item selected 
            pCmdUI->Enable(false);
        } else {
            pCmdUI->Enable(true);
        }
    } else {
        pCmdUI->Enable(false);
    }
}

void CPageObjectsView::OnBackward() {
    m_listPageObjects.MoveBackward();
    GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
}

void CPageObjectsView::OnUpdateBackward(CCmdUI *pCmdUI) {
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    UINT nSelectedItems = m_listPageObjects.GetSelectedCount();
    if (nSelectedItems == 1) {
        int nSelectedItem = m_listPageObjects.GetSelectedItem();
        int nLastItem = m_listPageObjects.GetItemCount() - 1;
        if (pDoc->IsPreviewActive() || nSelectedItem == -1 || nSelectedItem == nLastItem) { // no item selected or last item selected
            pCmdUI->Enable(false);
        } else {
            pCmdUI->Enable(true);
        }
    } else {
        pCmdUI->Enable(false);
    }
}

void CPageObjectsView::OnFront() {
    m_listPageObjects.MoveFront();
    GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
}

void CPageObjectsView::OnBack() {
    m_listPageObjects.MoveBack();
    GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
}

void CPageObjectsView::OnProperties() {
    CWhiteboardView *pWhiteboardView = CMainFrameE::GetCurrentInstance()->GetWhiteboardView();

    if (pWhiteboardView) {
        pWhiteboardView->ShowInteractionProperties();
        // also calls UpdateAllViews()
    }
}

void CPageObjectsView::OnUpdateProperties(CCmdUI *pCmdUI) {
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    UINT nSelectedItems = m_listPageObjects.GetSelectedCount();
    if (nSelectedItems == 1) {
        int nSelectedItem = m_listPageObjects.GetSelectedItem();
        if (pDoc->IsPreviewActive() || nSelectedItem == -1) { // no item selected
            pCmdUI->Enable(false);
        } else {
            pCmdUI->Enable(true);
        }
    } else {
        pCmdUI->Enable(false);
    }

    // TODO: deaktiviert "wenn kein Dialog Eigenschaften für den Objekttyp existiert"
}

void CPageObjectsView::OnRename() {
    RenameSelectedObject();
}

void CPageObjectsView::OnUpdateRename(CCmdUI *pCmdUI) {
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    UINT nSelectedItems = m_listPageObjects.GetSelectedCount();
    if (nSelectedItems == 1) {
        int nSelectedItem = m_listPageObjects.GetSelectedItem();
        if (pDoc->IsPreviewActive() || nSelectedItem == -1) {// no item selected
            pCmdUI->Enable(false);
        } else {
            pCmdUI->Enable(true);
        }
    } else {
        pCmdUI->Enable(false);
    }
}

void CPageObjectsView::OnDelete() {
    CWhiteboardView *pWhiteboardView = CMainFrameE::GetCurrentInstance()->GetWhiteboardView();

    if (pWhiteboardView) {
        pWhiteboardView->Delete();
    }
}

void CPageObjectsView::OnUpdateDelete(CCmdUI *pCmdUI) {
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    UINT nSelectedItems = m_listPageObjects.GetSelectedCount();
    if (nSelectedItems == 1) {
        int nSelectedItem = m_listPageObjects.GetSelectedItem();
        if (pDoc->IsPreviewActive() || nSelectedItem == -1) {// no item selected
            pCmdUI->Enable(false);
        } else {
            pCmdUI->Enable(true);
        }
    } else {
        pCmdUI->Enable(false);
    }
    // TODO: deaktiviert " wenn ein Seitenobjekt nicht individuell 
    // (sondern nur zusammen mit anderen zugehörigen Objekten) gelöscht werden kann "
}

LRESULT CPageObjectsView::OnIdleUpdateUIMessage(WPARAM, LPARAM)
{
    UpdateDialogControls( this, TRUE );
    return 0;
}

void CPageObjectsView::RefreshButtonFont() { 
    CFont *pButtonFont = XTPPaintManager()->GetIconFont();

    if (!pButtonFont) {
        return;
    }

    LOGFONT lf;
    pButtonFont->GetLogFont(&lf);
    m_fntButton.DeleteObject();
    m_fntButton.CreateFontIndirect(&lf);

    m_btnForward.SetFont(&m_fntButton, TRUE);
    m_btnBackward.SetFont(&m_fntButton, TRUE);
    m_btnFront.SetFont(&m_fntButton, TRUE);
    m_btnBack.SetFont(&m_fntButton, TRUE);
    m_btnProperties.SetFont(&m_fntButton, TRUE);
    m_btnRename.SetFont(&m_fntButton, TRUE);
    m_btnDelete.SetFont(&m_fntButton, TRUE);

    this->UpdateWindow();
}

void CPageObjectsView::RefreshPageObjectList() {
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
    m_listPageObjects.RefreshList(pDoc);
}

BOOL CPageObjectsView::OnEraseBkgnd(CDC* pDC) {
    // do not erase background
    return TRUE;
}

void CPageObjectsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    CEditorDoc *pDoc = (CEditorDoc *)GetDocument();

    if (!pDoc->IsPreviewActive()) {
        if (nChar == VK_F2) {
            RenameSelectedObject();
        } else if (nChar == VK_DELETE) {
            OnDelete();
        }
    }

    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPageObjectsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) {
    if (lHint != HINT_CHANGE_POSITION && lHint != HINT_CHANGE_PAGE && lHint != HINT_CHANGE_STRUCTURE) {
        return; // ignore
    }

    RefreshPageObjectList();
    RedrawWindow();	
}

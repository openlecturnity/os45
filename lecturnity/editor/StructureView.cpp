// StructureView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "StructureView.h"
#include "editorDoc.h"
#include "MainFrm.h"
#include "InteractionStream.h"
#include "QuestionStream.h"
#include "DocStructRecord.h"
//#include "DocStructReportControl.h"
//#include "InteractionRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStructureView
#define ROOT_ICON_THUMB   0
#define ROOT_ICON_PAGE    1
#define ROOT_ICON_COMPACT 2

IMPLEMENT_DYNCREATE(CStructureView, CXTPReportView)

CStructureView::CStructureView(){
    m_pDrawingBitmap = NULL;
    m_pBitmapDC = NULL;
    m_siMinimumStructure.cx = 0;
    m_siMinimumStructure.cy = 0;
    m_siCurrentImage.cx = 0;
    m_siCurrentImage.cy = 0;

    m_nLastActivePage = -1;

    m_bIsStatusBarCreated = false;
    m_nCurrentLayout = DocumentStructureLayout(Thumbnail);
    FontManipulation::CreateDefaultFont(m_defaultFont);

    m_createStructPopup.LoadMenu(IDR_CREATESTRUCTURE_CONTEXT);

    m_rgnUpdate.CreateRectRgn(0, 0, 0, 0);
    m_bIsStatusBarCreated = false;
    m_caStructInfo.RemoveAll();
    m_caChIndexes.RemoveAll();
    m_iLastActivePage = -1;
    m_bSelectFromOutside = false;
    
}

CStructureView::~CStructureView(){
    if (m_pBitmapDC){
        m_pBitmapDC->DeleteDC();
        delete m_pBitmapDC;
    }

    if (m_pDrawingBitmap){
        m_pDrawingBitmap->DeleteObject();
        delete m_pDrawingBitmap;
    }

    m_caStructInfo.RemoveAll();
    SetReportCtrl(NULL);

}


BEGIN_MESSAGE_MAP(CStructureView, CXTPReportView)
    //{{AFX_MSG_MAP(CStructureView)
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_LBUTTONUP()
    ON_WM_SETCURSOR()
    ON_COMMAND_RANGE(ID_STRUCT_INDICATOR_NORMAL_E, ID_STRUCT_INDICATOR_TITLE_E, OnStatusBarSwitchView)
    ON_UPDATE_COMMAND_UI(ID_STRUCT_INDICATOR_VIEW_E, OnUpdateStatusBarSwitchView)
    ON_COMMAND(IDC_EMBED_CLICK_PAGES, OnShowClickPages)
    ON_UPDATE_COMMAND_UI(IDC_EMBED_CLICK_PAGES, OnUpdateShowClickPages)
    ON_NOTIFY(XTP_NM_REPORT_LBUTTONDOWN/*NM_CLICK*//*XTP_NM_REPORT_SELCHANGED*/, XTP_ID_REPORT_CONTROL, OnReportItemClick)
    //}}AFX_MSG_MAP
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CStructureView 

int CStructureView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CXTPReportView::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_listElements.Create(WS_CHILD|WS_TABSTOP|WS_VISIBLE|WM_VSCROLL, CRect(0, 0, 0, 0), this, XTP_ID_REPORT_CONTROL)){
        TRACE(_T("Failed to create view window\n"));
        return -1;
    }
    m_listElements.GetToolTipContext()->SetStyle(xtpToolTipOffice2007);
    m_listElements.GetToolTipContext()->SetMaxTipWidth(200);
    m_listElements.GetToolTipContext()->SetMargin(CRect(2, 2, 2, 2));  
    m_listElements.GetToolTipContext()->SetDelayTime(TTDT_INITIAL, 900);

    SetReportCtrl(&m_listElements);
    CRect rcClient;
    GetClientRect(&rcClient);

    UpdateImageList();

    CXTPReportControl &wndReport = GetReportCtrl();

    CXTPReportColumn* pColumn = wndReport.AddColumn(new CXTPReportColumn(0, _T("Name"), 250));
    pColumn->SetTreeColumn(TRUE);
    pColumn->SetEditable(FALSE);

    CXTPDocumentStructurePaintManager* pPaintManager = new CXTPDocumentStructurePaintManager();
    wndReport.SetPaintManager(pPaintManager);

    wndReport.SetGridStyle(false, xtpReportGridNoLines);
    wndReport.GetReportHeader()->AllowColumnRemove(FALSE);
    wndReport.GetPaintManager()->SetColumnStyle(xtpReportColumnFlat);
    wndReport.AllowEdit(FALSE);
    wndReport.EditOnClick(FALSE);
    wndReport.SetMultipleSelection(FALSE);
    wndReport.SetTreeIndent(8);
    wndReport.GetReportHeader()->AllowColumnSort(FALSE);
    wndReport.ShowHeader(FALSE);
    wndReport.ShowRowFocus(FALSE);
    wndReport.GetReportHeader()->AllowColumnResize(false);
    //wndReport.EnableDragDrop(_T("MyApplicationSoare"), xtpReportAllowDrop | xtpReportAllowDrag);

    ShowScrollBar(SB_HORZ, FALSE);
    CreateStatusBar();
    

    return 0;
}
void CStructureView::OnInitialUpdate(){
    CXTPReportView::OnInitialUpdate();

    //UpdateScrollSizes();

    //CreateStatusBar();

}

void CStructureView::OnDraw(CDC* pDC){
    //CXTPReportView::OnDraw(pDC);

    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    CRect crClient;
    GetClientRect(crClient);

    bool bProjectEmpty = pDoc == NULL || pDoc->project.IsEmpty();

    COLORREF clrBackground = ((CMainFrameE *)AfxGetMainWnd())->GetColor(CMainFrameE::COLOR_BG_LIGHT);
    if (bProjectEmpty)
        clrBackground = ((CMainFrameE *)AfxGetMainWnd())->GetColor(CMainFrameE::COLOR_BG_DARK);


    CBrush bgBrush(clrBackground);
    CRect rcStatusBar;
    m_wndStatusBar.GetClientRect(&rcStatusBar);
    CRect rcReport;
    rcReport.SetRect(crClient.TopLeft(), crClient.BottomRight());
    CXTPReportControl& wndReport = GetReportCtrl();
    //wndReport.GetClientRect(&rcReport);
    rcReport.bottom -= (rcStatusBar.Height());
    wndReport.SetWindowPos(NULL, rcReport.left, rcReport.top, rcReport.Width(), rcReport.Height(), SWP_NOZORDER);

    if (!bProjectEmpty){
        pDC->FillRgn(&m_rgnUpdate, &bgBrush);
        ShowChildWindows(true);
    }
    else{
        ShowChildWindows(false);
        pDC->FillRect(&crClient, &bgBrush);
    }

    bgBrush.DeleteObject();

    //m_btnShowClickPages.RedrawButton();
    m_wndStatusBar.RedrawWindow();
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_STRUCT_INDICATOR_VIEW);
}


/////////////////////////////////////////////////////////////////////////////
// Diagnose CStructureView

#ifdef _DEBUG
void CStructureView::AssertValid() const{
    CXTPReportView::AssertValid();
}

void CStructureView::Dump(CDumpContext& dc) const{
    CXTPReportView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CStructureView 

void CStructureView::OnPaint(){
    CXTPReportView::OnPaint();

    CClientDC dc(this);
    OnDraw(&dc);
}

void CStructureView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) {
    //CXTPReportView::OnUpdate(pSender, lHint, pHint);
    if (lHint != HINT_CHANGE_POSITION && lHint != HINT_CHANGE_PAGE && lHint != HINT_CHANGE_STRUCTURE)
        return; // ignore
    if(lHint == HINT_CHANGE_STRUCTURE || lHint == HINT_CHANGE_PAGE)
        RefreshList();
    /*else if(lHint == HINT_CHANGE_PAGE)
    GetReportCtrl().RedrawControl();*/
    else{
        //RefreshList();
        CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
        int activePage =pDoc->project.GetActivePage(pDoc->m_curPosMs);
        if(m_iLastActivePage!=activePage){
            SetSelected(pDoc->m_curPosMs);
            m_iLastActivePage = activePage;
        }
    }
}

BOOL CStructureView::OnEraseBkgnd(CDC* pDC) {
    return CView::OnEraseBkgnd(pDC);
}

void CStructureView::OnSize(UINT nType, int cx, int cy) {
    CXTPReportView::OnSize(nType, cx, cy);

    //UpdateDrawingBitmap();
    //m_nLastActivePage = -1; // redraw everything

}


void CStructureView::OnLButtonUp(UINT nFlags, CPoint point) {
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();

    if (pDoc && !pDoc->project.IsEmpty() && pDoc->project.HasStructure()){
        int nHeight = (m_nCurrentLayout == DocumentStructureLayout(Thumbnail))? -1: 10;
        int iScrollPosY = GetReportCtrl().GetScrollPos(SB_VERT);// GetScrollPosition().y;
        int iTimestampMs = pDoc->project.GetTimestamp(iScrollPosY + point.y, nHeight /*+ 100*/);

        int iActivePage = pDoc->project.GetActivePage(iTimestampMs);
        if (iActivePage != m_nLastActivePage){
            if (CMainFrameE::GetCurrentInstance()->PreviewInNormalMode())
                pDoc->JumpPreview(iTimestampMs);

            pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
        }
    }

    CXTPReportView::OnLButtonDown(nFlags, point);
}

BOOL CStructureView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
    if (pDoc->project.IsEmpty())
        return CXTPReportView::OnSetCursor(pWnd, nHitTest, message);

    if (nHitTest == HTCLIENT)
        SetCursor(LoadCursor(NULL, IDC_HAND));
    else
        SetCursor(LoadCursor(NULL, IDC_ARROW));

    return TRUE;
}

HRESULT CStructureView::UpdateThumbnailSize(){
    //CEditorDoc *pDoc = (CEditorDoc *)GetDocument();
    //
    //if (!pDoc->project.IsEmpty() && pDoc->project.HasStructure()){
    //   HRESULT hrSizes = UpdateScrollSizes();
    //   if (hrSizes == S_OK){ // something was changed
    //      UpdateDrawingBitmap();
    //      
    //      m_nLastActivePage = -1; // repaint everything
    //      RedrawWindow(NULL);

    //      return S_OK;
    //   }
    //}

    return S_OK/*S_FALSE*/;
}

void CStructureView::OnContextMenu(CWnd* pWnd, CPoint point){
    CEditorDoc* pDoc = (CEditorDoc *)GetDocument();

    if (!pDoc)
        return;

    if (pDoc->IsPreviewActive())
        return;

    BOOL bShowMenu = FALSE;

    if (pDoc->project.IsDenverDocument() && !pDoc->project.HasStructuredSgClips())
        bShowMenu = TRUE;

    if (bShowMenu) {
        CPoint ptPoint = point;
        ScreenToClient(&ptPoint);

        CMenu *pPopup = m_createStructPopup.GetSubMenu(0);
        if (pPopup) 
            pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
    }

}

BOOL CStructureView::OnCommand(WPARAM wParam, LPARAM lParam){
    WORD nCmdID = LOWORD(wParam);

    // These commands are part of the CMainFrameE class
    if (nCmdID == ID_CREATESTRUCTURE){
        CMainFrameE *pMainFrame = CMainFrameE::GetCurrentInstance();
        if (pMainFrame != NULL && pMainFrame->OnCommand(wParam, lParam)){
            return TRUE; 
        }
    } 

    return CXTPReportView::OnCommand(wParam, lParam);
}

BOOL CStructureView::CreateStatusBar()
{
    if(!m_bIsStatusBarCreated){
        if (!m_wndStatusBar.Create(this)){
            TRACE0("Failed to create status bar\n");
            return FALSE;      // fail to create
        }

        m_wndStatusBar.AddIndicator(ID_SEPARATOR);

        static UINT switches[] ={
            ID_STRUCT_INDICATOR_NORMAL_E,
            ID_STRUCT_INDICATOR_TITLE_E
        };

        UINT nIDN = ID_STRUCT_INDICATOR_NORMAL_E;
        //m_wndStatusBar.GetImageManager()->SetIcons(IDB_STRUCT_INDICATOR_NORMAL, &nIDN, 1, CSize(16,16));
        m_wndStatusBar.GetImageManager()->SetIcon(ID_STRUCT_INDICATOR_NORMAL_E, ID_STRUCT_INDICATOR_NORMAL_E);

        UINT nIDT = ID_STRUCT_INDICATOR_TITLE_E;
        //m_wndStatusBar.GetImageManager()->SetIcons(IDB_STRUCT_INDICATOR_TITLE, &nIDT, 1, CSize(16,16));
        m_wndStatusBar.GetImageManager()->SetIcon(ID_STRUCT_INDICATOR_TITLE_E, ID_STRUCT_INDICATOR_TITLE_E);

        CXTPStatusBarSwitchPane* pSwitchPane = (CXTPStatusBarSwitchPane*)m_wndStatusBar.AddIndicator(new CXTPStatusBarSwitchPane(), ID_STRUCT_INDICATOR_VIEW_E);
        pSwitchPane->SetSwitches(switches, sizeof(switches)/sizeof(UINT));
        //pSwitchPane->SetChecked(ID_STRUCT_INDICATOR_NORMAL);
        pSwitchPane->SetBeginGroup(TRUE);
        pSwitchPane->SetTextFont(m_wndStatusBar.GetPaintManager()->GetIconFont());
        //pSwitchPane->SetSw
        //CRect rc1 = pSwitchPane->GetMargins();

        //pSwitchPane->SetCaption(_T("&View Shortcuts"));
        // pSwitchPane->BestFit();

        m_wndStatusBar.SetDrawDisabledText(FALSE);
        // m_wndStatusBar.SetRibbonDividerIndex(pSwitchPane->GetIndex() - 1);
        m_wndStatusBar.EnableCustomization(FALSE);

        m_wndStatusBar.GetPane(0)->SetEnabled(FALSE);
        m_bIsStatusBarCreated = true;

        // Create toggle button to show/hide "click" pages
        CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
        CRect rcButton;
        rcButton.left = 5;
        rcButton.top = 2;
        rcButton.right = rcButton.left + 18;
        rcButton.bottom = rcButton.top + 18;
        
    }
    return TRUE;
}

void CStructureView::OnStatusBarSwitchView(UINT nID){
    CXTPStatusBarSwitchPane* pPane = (CXTPStatusBarSwitchPane*)m_wndStatusBar.FindPane(ID_STRUCT_INDICATOR_VIEW_E);

    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
    if (pDoc && !pDoc->project.IsEmpty() && pDoc->project.HasStructure() && !pDoc->project.IsDenverDocument()){
        CRect rcClient;
        GetClientRect(&rcClient);
        if(m_nCurrentLayout == (nID - ID_STRUCT_INDICATOR_NORMAL_E))
            return;
        switch (nID){
        case ID_STRUCT_INDICATOR_NORMAL_E:{
            m_nCurrentLayout = DocumentStructureLayout(Thumbnail);
            //RedrawWindow(NULL, NULL, RDW_UPDATENOW);
            //pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
            //OnDraw(GetDC());
            SetRecordLayout();
            //UpdateDrawingBitmap();
            //m_nLastActivePage = -1; // repaint everything
            GetReportCtrl().SetTreeIndent(8);
            GetReportCtrl().RedrawControl();
            //InvalidateRect(&rcClient);
            //RedrawWindow(NULL);

            break;
                                          }
        case ID_STRUCT_INDICATOR_TITLE_E:{
            m_nCurrentLayout = DocumentStructureLayout(Compact);
            SetRecordLayout();
            //UpdateDrawingBitmap();
            //m_nLastActivePage = -1; // repaint everything
            GetReportCtrl().SetTreeIndent(20);
            GetReportCtrl().RedrawControl();
            //InvalidateRect(&rcClient);
            //RedrawWindow(NULL);

            //pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
            break;
                                         }
        }
        //OnUpdate(NULL, HINT_CHANGE_POSITION, NULL);
    }
    GetReportCtrl().AdjustScrollBars();
    if ( GetReportCtrl().GetSelectedRows() != NULL 
        && GetReportCtrl().GetSelectedRows()->GetCount() > 0 )
        GetReportCtrl().EnsureVisible(GetReportCtrl().GetSelectedRows()->GetAt(0));
    UpdateImageList();
}

void CStructureView::OnUpdateStatusBarSwitchView(CCmdUI* pCmdUI){

    CXTPStatusBarSwitchPane* pPane = (CXTPStatusBarSwitchPane*)m_wndStatusBar.FindPane(ID_STRUCT_INDICATOR_VIEW_E);

    CXTPStatusBarSwitchPane::SWITCH *pbtnNormalLayout = pPane->GetSwitch(0);
    CXTPStatusBarSwitchPane::SWITCH *pbtnTitleLayout = pPane->GetSwitch(1);
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
    if (pDoc && !pDoc->project.IsEmpty() && pDoc->project.HasStructure() && !pDoc->project.IsDenverDocument()){

        pbtnNormalLayout->bEnabled = TRUE;
        pbtnTitleLayout->bEnabled = TRUE;
        switch (m_nCurrentLayout){
        case DocumentStructureLayout(Thumbnail):{
            //pPane->SetChecked(ID_STRUCT_INDICATOR_NORMAL);
            pbtnNormalLayout->bChecked = TRUE;
            //pbtnNormalLayout->bHighlighted = TRUE;
            pbtnTitleLayout->bChecked = FALSE;

            break;
                                                }
        case DocumentStructureLayout(Compact):{
            //pPane->SetChecked(ID_STRUCT_INDICATOR_TITLE);
            pbtnTitleLayout->bChecked = TRUE;
            //pbtnTitleLayout->bHighlighted = TRUE;
            pbtnNormalLayout->bChecked = FALSE;
            break;
                                              }

        }
    }
    else{
        pbtnNormalLayout->bEnabled = FALSE;
        pbtnTitleLayout->bEnabled = FALSE;
    }
}

void CStructureView::OnShowClickPages() {
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
    if (pDoc != NULL) {
        pDoc->project.SetShowClickPages(!pDoc->project.ShowClickPages(), true);
    }
}

void CStructureView::OnUpdateShowClickPages(CCmdUI* pCmdUI) {
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    BOOL bEnable = FALSE;
    if (pDoc != NULL && pDoc->IsScreenGrabbingDocument()) {
        // If there are interactions there are problems if 
        // the click pages are removed or inserted (=> Bug 5658)
        bool bInteractionsAreInserted = false;
        CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream();
        if (pInteractionStream != NULL && pInteractionStream->GetInteractionsCount() > 0)
            bInteractionsAreInserted = true;
        if (!bInteractionsAreInserted) {
            CMarkStream *pMarkStream = pDoc->project.GetMarkStream();
            if (pMarkStream != NULL && pMarkStream->GetMarksCount() > 0)
                bInteractionsAreInserted = true;
        }
        if (!bInteractionsAreInserted) {
            CQuestionStream *pQuestionStream = pDoc->project.GetQuestionStream();
            if (pQuestionStream != NULL && pQuestionStream->GetQuestionCount() > 0)
                bInteractionsAreInserted = true;
        }

        if (!bInteractionsAreInserted) {
            if (pDoc->project.ShowClickPages()) {
                pCmdUI->SetCheck(TRUE);
            }
            else if (!pDoc->project.ShowClickPages()) {
                pCmdUI->SetCheck(FALSE);
            }
            if (pDoc->project.DocumentHasClickPages())
                bEnable = TRUE;
        }

    }

    pCmdUI->Enable(bEnable);
}

// Functions from DocumentStructureView
void CStructureView::UpdateImageList(){
    switch( m_nCurrentLayout){
    case DocumentStructureLayout(Thumbnail):{
        // create list
        if ( m_ilStateImages.m_hImageList != NULL )
        {
            GetReportCtrl().GetImageManager()->RemoveAll();
            VERIFY(m_ilStateImages.DeleteImageList());
        }

        VERIFY(m_ilStateImages.Create(32,32, ILC_COLOR24|ILC_MASK, 0, 1));

        CBitmap bmpFolder;
        CXTPResourceManager::AssertValid(XTPResourceManager()->LoadBitmap(&bmpFolder, IDB_FOLDER));
        m_ilStateImages.Add(&bmpFolder, RGB(255, 0, 255));

        GetReportCtrl().SetImageList(&m_ilStateImages);

        HINSTANCE hInstance = AfxGetInstanceHandle();
        GetReportCtrl().GetImageManager()->SetIconFromResource(hInstance, MAKEINTRESOURCE(IDI_CLICK_ICON), 5, CSize(16, 16), xtpImageNormal);

        ChangeAllRootsIcon(ROOT_ICON_THUMB);

        break;
                                            }
    case DocumentStructureLayout(Compact):{
        // create list
        if ( m_ilStateImages.m_hImageList != NULL ){
            GetReportCtrl().GetImageManager()->RemoveAll();
            VERIFY(m_ilStateImages.DeleteImageList());
        }

        VERIFY(m_ilStateImages.Create(16,16, ILC_COLOR24|ILC_MASK, 0, 1));

        CBitmap bmpFolder;
        CXTPResourceManager::AssertValid(XTPResourceManager()->LoadBitmap(&bmpFolder, IDB_FOLDER_SLIDE));
        m_ilStateImages.Add(&bmpFolder, RGB(0, 255, 0));

        CBitmap bmpRootItem;
        CXTPResourceManager::AssertValid(XTPResourceManager()->LoadBitmap(&bmpRootItem, IDB_POWERPOINT));
        m_ilStateImages.Add(&bmpRootItem, RGB(255, 0, 255));

        GetReportCtrl().SetImageList(&m_ilStateImages);

        HINSTANCE hInstance = AfxGetInstanceHandle();
        GetReportCtrl().GetImageManager()->SetIconFromResource(hInstance, MAKEINTRESOURCE(IDI_CLICK_ICON), 5, CSize(16, 16), xtpImageNormal);

        ChangeAllRootsIcon(ROOT_ICON_COMPACT);
        break;
                                          }
    default:{
        ASSERT(FALSE);
        break;
            }
    }
}

void CStructureView::ChangeAllRootsIcon(UINT nImageIndex){
    if ( GetReportCtrl().GetRecords() != NULL ){
        for ( int i = 0; i < GetReportCtrl().GetRecords()->GetCount(); i++){
            if ( GetReportCtrl().GetRecords()->GetAt(i) != NULL
                && GetReportCtrl().GetRecords()->GetAt(i)->GetItem(0) != NULL){
                    ((CDocStructRecordItem *)GetReportCtrl().GetRecords()->GetAt(i)->GetItem(0))->m_nImageIndex = nImageIndex;
            }
        }
    }
}

void CStructureView::SetSelected(UINT nId){
    CXTPReportControl& wndReport = GetReportCtrl();
    for(int i = 0; i < wndReport.GetRows()->GetCount(); ++i){
        CDocStructRecord* pRecord = (CDocStructRecord*)wndReport.GetRows()->GetAt(i)->GetRecord();
        if(pRecord->m_pContainer->GetID(pRecord->m_pvData) == nId){
            wndReport.SetFocusedRow(wndReport.GetRows()->GetAt(i));
            return;
        }
    } 
}

void CStructureView::SetSelected(int nTimestamp){
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
    if(pDoc == NULL)
        return;
    int iActivePage = pDoc->project.GetActivePage(nTimestamp);

    /*if(iActivePage != m_nLastActivePage)
    {*/
    CXTPReportControl& wndReport = GetReportCtrl();
    for(int i = 1; i < wndReport.GetRows()->GetCount(); ++i){
        CDocStructRecord* pRecord = (CDocStructRecord*)wndReport.GetRows()->GetAt(i)->GetRecord();
        CStructureInformation* pStructureInformation = (CStructureInformation*)pRecord->m_pvData;


        if(/*pStructureInformation->GetType() == CStructureInformation::PAGE &&*/ pStructureInformation->GetBegin() <= nTimestamp && pStructureInformation->GetEnd() > nTimestamp){
            m_bSelectFromOutside = true;
            //wndReport.SetFocusedRow(wndReport.GetRows()->GetAt(i));
            /*pRecord->*/
            wndReport.GetRows()->GetAt(i)->EnsureVisible();
            wndReport.GetRows()->GetAt(i)->SetSelected(TRUE);
            wndReport.RedrawControl();
            m_bSelectFromOutside = false;
            //return;
        }
    }
    m_nLastActivePage = iActivePage;
    /* }*/
}

void CStructureView::RemoveAll(){
    GetReportCtrl().ResetContent();
}
// Overrides from CDocumentStructure

void CStructureView::SetName(void* pData, CString csText){
    // Send assertion failure. editor items, cannot be renamed.
    ASSERT(FALSE);
}

CString CStructureView::GetName(void* pData){
    CStructureInformation *pStructureInformation = (CStructureInformation *) pData;
    ASSERT(pStructureInformation);
    return (CString)pStructureInformation->GetTitle();
}

UINT CStructureView::GetType(void* pData){
    CStructureInformation *pStructureInformation = (CStructureInformation *) pData;
    ASSERT(pStructureInformation);
    return (UINT) pStructureInformation->GetType();
}

void CStructureView::DrawThumbWhiteboard(void* pData, CRect rcThumb, CDC* pDC){
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
    if (pDoc == NULL)
        return;
    CSize siPage;
    pDoc->project.GetPageDimension(siPage);

    double dZoomFactor1 = (double)rcThumb.Width() / siPage.cx;
    double dZoomFactor2 = (double)rcThumb.Height()/ siPage.cy;

    double dZoomFactor = dZoomFactor1 < dZoomFactor2 ? dZoomFactor1 : dZoomFactor2;
    DrawSdk::ZoomManager zoom(dZoomFactor);

    CInteractionStream *pInteractions = pDoc->project.GetInteractionStream(false);
    int x = rcThumb.left;
    int y = rcThumb.top;
    int iPageX = x;
    int iPageY = y;

    double fFact = 1.0;
    double fHeight = siPage.cy * dZoomFactor; 
    double fWidth = siPage.cx * dZoomFactor; 
    CStructureInformation *pStructureInformation = (CStructureInformation *) pData;
    pStructureInformation->Draw(pDC, x,y, fWidth, fHeight, 0, 0, 0, m_defaultFont, &zoom);
    if(pInteractions){
        CPoint ptOffsets(iPageX, iPageY);
        pInteractions->Draw(pDC, pStructureInformation->GetEnd() - 1, &ptOffsets, dZoomFactor, true); 
    }
}

UINT CStructureView::GetID(void* pData){
    CStructureInformation *pStructureInformation = (CStructureInformation *) pData;
    ASSERT(pStructureInformation);
    UINT nID = pStructureInformation->GetUniqueId();
    return nID;
}

void CStructureView::GetKeywords(void *pData, CString & csKeywords){
    //ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
    //pgc->GetKeywords(csKeywords);

}

void CStructureView::SetKeywords(void *pData, CString csKeywords){
    //ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
    //pgc->GetKeywords(csKeywords);
}

void CStructureView::LoadDocument(void *pData){
    /*ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
    OnSelectionLoad((WPARAM)pgc->GetID(), 0);*/
}

bool CStructureView::ShowClickIcon(void *pData) {
    CStructureInformation *pStructureInformation = (CStructureInformation *) pData;

    if (pStructureInformation != NULL &&
        pStructureInformation->GetType() == CStructureInformation::PAGE) {
        if (((CPageInformation *)pStructureInformation)->GetPageType() == _T("click"))
            return true;
    }

    return false;
}

void CStructureView::RefreshList(){
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    if (pDoc && !pDoc->project.IsEmpty() && pDoc->project.HasStructure() && !pDoc->project.IsDenverDocument()){
        CArray<CStructureInformation *, CStructureInformation *> aElements;
        HRESULT hr = pDoc->project.GetStructureElements(&aElements);

        m_caStructInfo.RemoveAll();
        //Create m_caStructInfo array containing only the chapters and pages from 
        //CStructureInformation elements
       
        for(int i = 0; i < aElements.GetSize(); i++){
            CString s = aElements.GetAt(i)->GetTitle();
            if (aElements.GetAt(i)->GetType() != CStructureInformation::BASE) {
                m_caStructInfo.Add(aElements.GetAt(i));
            }
        }

        //m_caStructInfo.GetAt(0) will be the root of the displayed structure
        //Use an Array containing the indexes of chapters in m_caStructInfo
        //the root element is included in chapters indexes array
        m_caChIndexes.RemoveAll();
        CTreeDocStruct* pdsTree = new CTreeDocStruct();
        CArray<CTreeDocStruct*, CTreeDocStruct*>apRoots;
        if(m_caStructInfo.GetSize() > 0){
            pdsTree->m_pSi = m_caStructInfo.GetAt(0);
            apRoots.Add(pdsTree);
            m_caChIndexes.Add(0);
        }
        else
            return;
  
        CTreeDocStruct* ptdsNode;
        //incep imp
        CTreeDocStruct* ptdsRoot;
        CArray<int, int>aChIndex;
        CArray<int, int>aPagesIndex;
        // Add chapter elements to array of roots
        for(int i = 1; i < m_caStructInfo.GetSize(); i++){
            if(m_caStructInfo.GetAt(i)->GetType() == CStructureInformation::CHAPTER){
                aChIndex.Add(i);
                m_caChIndexes.Add(i);
                ptdsRoot = new CTreeDocStruct();
                ptdsRoot->m_pSi = m_caStructInfo.GetAt(i);
                apRoots.Add(ptdsRoot);
            }
            else{
                aPagesIndex.Add(i);
            }
        }

        /*
        * Add new implementation of document structure display
        * Creation if the structure tree is now based on the begin and end timestamps of 
        * each element from CStructureInformation
        */
        if(aChIndex.GetSize() > 0){
            int iCurrentRoot = 0;
            for(int i = 1; i < m_caStructInfo.GetSize(); i++) {
               
                int iParentChapterIndex = GetParentChapterIndex(m_caStructInfo.GetAt(i), iCurrentRoot);
                if(m_caStructInfo.GetAt(i)->GetType() != CStructureInformation::CHAPTER) {
                    ptdsNode = new CTreeDocStruct();
                    ptdsNode->m_pSi = m_caStructInfo.GetAt(i);
                    AddNewTreeNode(ptdsNode, apRoots.GetAt(iParentChapterIndex));
                } else {
                    iCurrentRoot++;
                    AddNewTreeNode(apRoots.GetAt(iCurrentRoot), apRoots.GetAt(iParentChapterIndex));
                }
            }

            /*
            * Comment out old implementation
            * The structure was previouslu created based on the unique ID of each element in CStructureInformation
            * This implementation worked correctly only in case no copy/cut - paste were performed
            * In case copy/cut - paste is performed the values of unique ID's are changed  
            * and the structure was not rebuild correctly
            */

            //int iStartIndex = 1;
            //for(int i = 0; i < aChIndex.GetSize(); i++){
            //    int chIdx = aChIndex.GetAt(i);
            //    for(int j = iStartIndex; j < chIdx/*aChIndex.GetAt(i)*/; j++){
            //        ptdsNode = new CTreeDocStruct();
            //        ptdsNode->m_pSi = m_caStructInfo.GetAt(j);
            //        AddNewTreeNode(ptdsNode, apRoots.GetAt(i));
            //    }
            //    int iChapterParentIndex = 0;

            //    for(int k = 0; k <= i; k ++){
            //        int id1 = m_caStructInfo.GetAt(aChIndex.GetAt(i))->GetUniqueId();
            //        int id2 = apRoots.GetAt(k)->m_pSi->GetUniqueId();
            //        if(m_caStructInfo.GetAt(aChIndex.GetAt(i))->GetUniqueId() < apRoots.GetAt(k)->m_pSi->GetUniqueId())
            //            iChapterParentIndex = k;
            //    }
            //    // TODO later: 
            //    // Bug 5077: Please Remove element (i+1) from apRoots
            //    // because now it is a child of apRoots(iChapterIndex) and should not be deleted
            //    // while deleting the apRoots array before exiting this function.
            //    AddNewTreeNode(apRoots.GetAt(i + 1), apRoots.GetAt(iChapterParentIndex));
            //    iStartIndex = aChIndex.GetAt(i) + 1;
            //    int xxxxxx = 0;
            //}
            //for(int i = aChIndex.GetAt(aChIndex.GetSize() - 1) + 1; i < m_caStructInfo.GetSize(); i++){
            //    int iChapterParentIndex = 0;
            //    for(int j = 0; j < apRoots.GetSize(); j++){
            //        if(m_caStructInfo.GetAt(i)->GetUniqueId() < apRoots.GetAt(j)->m_pSi->GetUniqueId())
            //            iChapterParentIndex = j;
            //    }
            //    ptdsNode = new CTreeDocStruct();
            //    ptdsNode->m_pSi = m_caStructInfo.GetAt(i);
            //    AddNewTreeNode(ptdsNode, apRoots.GetAt(iChapterParentIndex));
            //}
        } else { // there are no chapters in the structure, all elements belong to the root element
            for(int i = 1; i < m_caStructInfo.GetSize(); i++){
                ptdsNode = new CTreeDocStruct();
                ptdsNode->m_pSi = m_caStructInfo.GetAt(i);
                AddNewTreeNode(ptdsNode, apRoots.GetAt(0));
            }
        }
        //termin impl

        CXTPReportControl& wndReport = GetReportCtrl();
        GetReportCtrl().ResetContent();
        CDocStructRecord::s_CHAPTER = CStructureInformation::CHAPTER;
        CDocStructRecord::s_PAGE = CStructureInformation::PAGE;
        CDocStructRecord::s_DOCUMENT = CStructureInformation::BASE;
        CXTPReportRecord* pReportControl = wndReport.AddRecord(new CDocStructRecord(this, m_caStructInfo.GetAt(0), true, m_nCurrentLayout, ROOT_ICON_PAGE, false) );
        pReportControl->SetExpanded(TRUE);
        PopulateRecord(apRoots.GetAt(0), pReportControl);


        // ToDo later: Bug 5077: All elements are appended to apRoots[0] or a sub element of it
        // (AddNewTreeNode(apRoots.GetAt(i + 1), apRoots.GetAt(iChapterParentIndex));)
        // If the element will be removed from apRoots after appending it to another element
        // The delete code can be done over all apRoots elements.

        //release apRoots
        //for(int i =0; i< apRoots.GetCount(); i++)
        DeleteDoc(apRoots.GetAt(0));
        apRoots.RemoveAll();


        wndReport.Populate();
        int nActualTimestamp = pDoc->m_curPosMs;
        int nActualPage = pDoc->project.GetActivePage(nActualTimestamp);
        m_iLastActivePage = nActualPage - 1;
        wndReport.SetFocusedRow(wndReport.GetRows()->GetAt(aPagesIndex.GetAt(nActualPage - 1)));
        UpdateImageList();
        /* SetRecordLayout();
        wndReport.RedrawControl();*/
    }
    RedrawWindow(); 

}

int CStructureView::GetParentChapterIndex(CStructureInformation* pElement, int iCurrentRoot){
    for (int i = m_caChIndexes.GetSize() - 1; i >= 0; i--)
    {
        CStructureInformation* pChapterElement = m_caStructInfo.GetAt(m_caChIndexes.GetAt(i));
        if (pChapterElement->GetBegin() <= pElement->GetBegin() &&
            pChapterElement->GetEnd() >= pElement->GetEnd() && 
            pChapterElement->GetUniqueId() != pElement->GetUniqueId())
        {
            if (pElement->GetType() == CStructureInformation::CHAPTER) {
                // The parent of a chapter can be only one of the chapters with higher index 
                // in m_caChIndexes array
                if(iCurrentRoot >= i)
                    return i;
            } else {
                return i;
            }
        }
    }
    return 0;
}

void CStructureView::AddNewTreeNode(CTreeDocStruct* pChild, CTreeDocStruct* pParent){
    pParent->m_aNode.Add(pChild);
}

void CStructureView::DeleteDoc(CTreeDocStruct* pParent){
    if ( pParent == NULL )
        return;

    for(int i=0; i < pParent->m_aNode.GetCount(); i++ )
        DeleteDoc(pParent->m_aNode.GetAt(i));
    pParent->m_aNode.RemoveAll();

    SAFE_DELETE(pParent);
}


void CStructureView::PopulateRecord(CTreeDocStruct* pNode, CXTPReportRecord *pRecordRoot){
    for(int i = 0; i < pNode->m_aNode.GetSize(); i++){
        CString csNr;
        csNr.Format(_T(" - %d"), pNode->m_aNode.GetAt(i)->m_pSi->GetUniqueId());
        if(pNode->m_aNode.GetAt(i)->m_aNode.GetSize() > 0){
            CXTPReportRecord* pChild = pRecordRoot->GetChilds()->Add(new CDocStructRecord(this, pNode->m_aNode.GetAt(i)->m_pSi, true, m_nCurrentLayout, ROOT_ICON_THUMB, false));
            pChild->SetExpanded(TRUE);
            PopulateRecord(pNode->m_aNode.GetAt(i), pChild);
        }
        else{
            pRecordRoot->GetChilds()->Add(new CDocStructRecord(this, pNode->m_aNode.GetAt(i)->m_pSi, true, m_nCurrentLayout, ROOT_ICON_PAGE, false));
        }
    }
}

void CStructureView::OnReportItemClick(NMHDR * pNotifyStruct, LRESULT * /*result*/){
    XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNotifyStruct;

    if (!pItemNotify->pRow || !pItemNotify->pColumn)
        return;

    TRACE(_T("Click on row %d, col %d\n"),
        pItemNotify->pRow->GetIndex(), pItemNotify->pColumn->GetItemIndex());

    int itemIndex = pItemNotify->pRow->GetIndex();

    /*if(m_bSelectFromOutside){

    return;
    }
    if(GetReportCtrl().GetSelectedRows()->GetCount() != 1)
    return;

    CDocStructRecord* pRecord = (CDocStructRecord*)GetReportCtrl().GetSelectedRows()->GetAt(0)->GetRecord();*/

    /*if(GetReportCtrl().GetSelectedRows()->GetAt(0)->HasChildren()){
    CXTPReportRow * pRow = GetReportCtrl().GetRows()->GetNext(GetReportCtrl().GetSelectedRows()->GetAt(0), true);

    CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();
    }*/

    //if(pItemNotify->pRow->HasChildren()){
    //   BOOL bIsExpanded = pItemNotify->pRow->IsExpanded();
    //   /*if(pItemNotify->pItem == NULL)*/
    //      pItemNotify->pRow->SetExpanded(!bIsExpanded);
    //   return;
    //}
    CDocStructRecord* pRecord = (CDocStructRecord*)pItemNotify->pRow->GetRecord();
    if (pRecord == NULL)
        return;

    //CDocStructRecord* pRecord = (CDocStructRecord*)pItemNotify->pRow->GetRecord();
    CStructureInformation* pStructureInfo = (CStructureInformation*)pRecord->m_pvData;
    int iTimestamp = pStructureInfo->GetBegin();
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
    if(pDoc == NULL)
        return;

    if (CMainFrameE::GetCurrentInstance()->PreviewInNormalMode())
        pDoc->JumpPreview(iTimestamp);


    CXTPReportView::OnLButtonDown(0, pItemNotify->pt);
    //pDoc->UpdateAllViews(this, HINT_CHANGE_POSITION);


    /* if(m_iLastActivePage != itemIndex){
    int iTimestampMs = m_caStructInfo.GetAt(itemIndex)->GetBegin();
    CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
    pDoc->JumpPreview(iTimestampMs);

    pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
    }*/

    //MD - another way to fix wrong reaction on column checkbox click - comment line GetReportCtrl().Populate();
    //	instead of 	adding condition if (!bCheckBoxClicked)in function CXTPReportRecordItem::OnClick

    //*pResult = 0;
}
void CStructureView::ShowChildWindows(bool bShow){
    CXTPReportControl& wndReport = GetReportCtrl();
    if (bShow){
        if (!wndReport.IsWindowVisible()){
            wndReport.ShowWindow(SW_SHOW);
        }
    } else {
        if (wndReport.IsWindowVisible())
            wndReport.ShowWindow(SW_HIDE);
    }
    m_wndStatusBar.ShowWindow(SW_SHOW);
}


void CStructureView::SetRecordLayout() {
    int numRows = GetReportCtrl().GetRows()->GetCount();
    for (int i = 0; i < numRows; ++i) {
        CDocStructRecord* pRecord = (CDocStructRecord*) GetReportCtrl().GetRows()->GetAt(i)->GetRecord();
        pRecord->m_nCurrentLayout = m_nCurrentLayout;
        SetChildrenRecordLayout(pRecord, m_nCurrentLayout);
    }
}

void CStructureView::SetChildrenRecordLayout(CDocStructRecord* pRecord, int nLayout)
{
   if (pRecord == NULL)
      return;

   pRecord->m_nCurrentLayout = nLayout;
   if (pRecord->GetChilds() != NULL) {
      for (int i = 0; i < pRecord->GetChilds()->GetCount(); i++)
         SetChildrenRecordLayout((CDocStructRecord*)pRecord->GetChilds()->GetAt(i), nLayout);
   }
}
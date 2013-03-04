// DocumentStructure.cpp : implementation file
//

/*#include "stdafx.h"
#include "assistant.h"
#include "DocumentStructure.h"*/

#include "stdafx.h"
#include "AssistantDoc.h"
#include "DocumentStructureView.h"
#include "backend\mlb_generic.h"
#include "MainFrm.h"
#include "DocumentStructure.h"
#include "DocStructRecord.h"
#include "DocStructReportControl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDocumentStructure
#define ROOT_ICON_THUMB   0
#define ROOT_ICON_PAGE    1
#define ROOT_ICON_COMPACT 2

IMPLEMENT_DYNCREATE(CDocumentStructureView, CXTPReportView)

CDocumentStructureView::CDocumentStructureView()
{
   m_bIsStatusBarCreated = false;
   m_rgnUpdate.CreateRectRgn(0, 0, 0, 0);
   GetReportCtrl().ResetContent();
   m_nCurrentLayout = DocumentStructureLayout(Compact);
}

CDocumentStructureView::~CDocumentStructureView()
{
   RemoveAll();
}


BEGIN_MESSAGE_MAP(CDocumentStructureView, CXTPReportView)
	//{{AFX_MSG_MAP(CDocumentStructureView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(CDocStructReportControl::WM_SELECTION_LOAD, OnSelectionLoad)
	ON_MESSAGE(CDocStructReportControl::WM_SELECTION_CLOSE, OnSelectionClose)
	ON_MESSAGE(CDocStructReportControl::WM_SELECTION_REMOVE, OnSelectionRemove)
	ON_MESSAGE(CDocStructReportControl::WM_SELECTION_CHANGED, OnSelectionChanged)
	ON_MESSAGE(CDocStructReportControl::WM_SELECTION_DELETE, OnSelectionDelete)
	ON_MESSAGE(CDocStructReportControl::WM_SELECTION_SAVE, OnSelectionSave)
	ON_MESSAGE(CDocStructReportControl::WM_INSERT_PAGE, OnInsertPage)
	ON_MESSAGE(CDocStructReportControl::WM_INSERT_CHAPTER, OnInsertChapter)
	ON_MESSAGE(CDocStructReportControl::WM_MOVE_ITEM, OnMoveItem)
   ON_COMMAND_RANGE(ID_STRUCT_INDICATOR_NORMAL, ID_STRUCT_INDICATOR_TITLE, OnStatusBarSwitchView)
   ON_UPDATE_COMMAND_UI(ID_STRUCT_INDICATOR_VIEW, OnUpdateStatusBarSwitchView)
   ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocumentStructureView diagnostics

#ifdef _DEBUG
void CDocumentStructureView::AssertValid() const
{
	CView::AssertValid();
}

void CDocumentStructureView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocumentStructureView message handlers
int CDocumentStructureView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

   m_listElements = new CDocStructReportControl();
   if (!m_listElements->Create(WS_CHILD|WS_TABSTOP|WS_VISIBLE|WM_VSCROLL, CRect(0, 0, 0, 0), this, XTP_ID_REPORT_CONTROL))
   {
      TRACE(_T("Failed to create view window\n"));
      return -1;
   }
   m_listElements->GetToolTipContext()->SetStyle(xtpToolTipOffice2007);
   m_listElements->GetToolTipContext()->SetMaxTipWidth(600);
   m_listElements->GetToolTipContext()->SetMargin(CRect(2, 2, 2, 2));
   m_listElements->GetToolTipContext()->SetDelayTime(TTDT_INITIAL, 900);

   SetReportCtrl(m_listElements);

   CRect rcClient;
   GetClientRect(&rcClient);

   UpdateImageList();

   CXTPReportControl &wndReport = GetReportCtrl();

   CXTPReportColumn* pColumn = wndReport.AddColumn(new CXTPReportColumn(0, _T("Name"), 250));
   pColumn->SetTreeColumn(TRUE);
   pColumn->SetEditable(TRUE);

   CXTPDocumentStructurePaintManager* pPaintManager = new CXTPDocumentStructurePaintManager();
	wndReport.SetPaintManager(pPaintManager);

   wndReport.SetGridStyle(false, xtpReportGridNoLines);
   wndReport.GetReportHeader()->AllowColumnRemove(FALSE);
   wndReport.GetPaintManager()->SetColumnStyle(xtpReportColumnFlat);
   wndReport.AllowEdit(TRUE);
   wndReport.EditOnClick(FALSE);
   wndReport.SetMultipleSelection(FALSE);
   wndReport.SetTreeIndent(20);
   wndReport.GetReportHeader()->AllowColumnSort(FALSE);
   wndReport.ShowHeader(FALSE);
   wndReport.ShowRowFocus(FALSE);
   wndReport.GetReportHeader()->AllowColumnResize(false);
   wndReport.EnableDragDrop(_T("MyApplicationSoare"), xtpReportAllowDrop | xtpReportAllowDrag);

   ShowScrollBar(SB_HORZ, FALSE);
   CreateStatusBar();

   CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
   if (pDoc)
       pDoc->RebuildStructureTree(this);
   return 0;
}

void CDocumentStructureView::UpdateImageList()
{
   switch( m_nCurrentLayout)
   {
   case DocumentStructureLayout(Thumbnail):
      {
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

         ChangeAllRootsIcon(ROOT_ICON_THUMB);

         break;
      }
   case DocumentStructureLayout(Compact):
      {
         // create list
         if ( m_ilStateImages.m_hImageList != NULL )
         {
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

         ChangeAllRootsIcon(ROOT_ICON_COMPACT);
         break;
      }
   default:
      {
         ASSERT(FALSE);
         break;
      }
   }
}

void CDocumentStructureView::ChangeAllRootsIcon(UINT nImageIndex)
{
   if ( GetReportCtrl().GetRecords() != NULL )
   {
      for ( int i = 0; i < GetReportCtrl().GetRecords()->GetCount(); i++)
      {
         if ( GetReportCtrl().GetRecords()->GetAt(i) != NULL
            && GetReportCtrl().GetRecords()->GetAt(i)->GetItem(0) != NULL)
         {
            ((CDocStructRecordItem *)GetReportCtrl().GetRecords()->GetAt(i)->GetItem(0))->m_nImageIndex = nImageIndex;
         }
      }
   }
}

void CDocumentStructureView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
   CRect crClient;
   GetClientRect(crClient);

   CBrush bgBrush(RGB(255,255,255)); 
   CRect rcStatusBar;
   m_wndStatusBar.GetClientRect(&rcStatusBar);
   CRect rcReport;
   rcReport.SetRect(crClient.TopLeft(), crClient.BottomRight());
   CXTPReportControl& wndReport = GetReportCtrl();
   //wndReport.GetClientRect(&rcReport);
   rcReport.bottom -= (rcStatusBar.Height());
   wndReport.SetWindowPos(NULL, rcReport.left, rcReport.top, rcReport.Width(), rcReport.Height(), SWP_NOZORDER);

   pDC->FillRgn(&m_rgnUpdate, &bgBrush);
   if (!wndReport.IsWindowVisible())
   {
      wndReport.ShowWindow(SW_SHOW);
   }

   m_wndStatusBar.RedrawWindow();
   RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_STRUCT_INDICATOR_VIEW);
}

void CDocumentStructureView::InsertChapter(ASSISTANT::GenericContainer *pContainer, UINT nParentId) 
{
   int nParentIndex = -1;
   if (nParentId != 0)
      nParentIndex = FindItem(nParentId);

   CDocStructRecord::s_CHAPTER = ASSISTANT::GenericContainer::FOLDER;
   CDocStructRecord::s_PAGE = ASSISTANT::GenericContainer::PAGE;
   CDocStructRecord::s_DOCUMENT = ASSISTANT::GenericContainer::DOCUMENT;
   CXTPReportControl& wndReport = GetReportCtrl();
   UINT nIconIndex = ROOT_ICON_THUMB;
   if ( m_nCurrentLayout == DocumentStructureLayout(Compact) && nParentIndex == -1)
      nIconIndex = ROOT_ICON_COMPACT;
   
   CDocStructRecord *pRecord = new CDocStructRecord((CDocumentStructure*)this, pContainer, true, m_nCurrentLayout, nIconIndex);  
   CXTPReportRecord* pReportControl;
   if(nParentIndex == -1)
   {
      pReportControl = wndReport.AddRecord(pRecord);
   }
   else
   {
      pReportControl = wndReport.GetRows()->GetAt(nParentIndex)->GetRecord();
      pReportControl->GetChilds()->Add(pRecord); 
   }
   pReportControl->SetExpanded(TRUE);
   wndReport.Populate();
}

void CDocumentStructureView::InsertPage(ASSISTANT::GenericContainer *pContainer, UINT nParentId) 
{
   int nParentIndex = FindItem(nParentId);  
   if(nParentIndex == -1)
      nParentIndex = FindItem(nParentId, ASSISTANT::GenericContainer::DOCUMENT);

   CDocStructRecord::s_CHAPTER = ASSISTANT::GenericContainer::FOLDER;
   CDocStructRecord::s_PAGE = ASSISTANT::GenericContainer::PAGE;
   CDocStructRecord::s_DOCUMENT = ASSISTANT::GenericContainer::DOCUMENT;
   CXTPReportControl& wndReport = GetReportCtrl();
   CDocStructRecord *pRecord = new CDocStructRecord((CDocumentStructure*)this, pContainer, false, m_nCurrentLayout, ROOT_ICON_PAGE);  
   CXTPReportRecord* pReportControl = wndReport.GetRows()->GetAt(nParentIndex)->GetRecord();
   pReportControl->GetChilds()->Add(pRecord);
   pReportControl->SetExpanded(TRUE);
   wndReport.Populate();
}

void CDocumentStructureView::SetSelected(UINT nId)
{
   CXTPReportControl& wndReport = GetReportCtrl();
   for(int i = 0; i < wndReport.GetRows()->GetCount(); ++i)
   {
      CDocStructRecord* pRecord = (CDocStructRecord*)wndReport.GetRows()->GetAt(i)->GetRecord();
      if(pRecord->m_pContainer->GetID(pRecord->m_pvData) == nId)
      {
         wndReport.SetFocusedRow(wndReport.GetRows()->GetAt(i));
         return;
      }
   } 
}

void CDocumentStructureView::SetAllUnselected()
{
   CXTPReportControl& wndReport = GetReportCtrl();
   for(int i = 0; i < wndReport.GetRows()->GetCount(); ++i)
   {
      CXTPReportRow* pRow = wndReport.GetRows()->GetAt(i);
      pRow->SetSelected(FALSE);
   } 
}

void CDocumentStructureView::RemoveAll()
{
   GetReportCtrl().ResetContent();
}


#ifdef _DVS2005
LRESULT CDocumentStructureView::OnSelectionLoad(WPARAM wParam, LPARAM lParam)
#else
void CDocumentStructureView::OnSelectionDelete(WPARAM wParam, LPARAM lParam)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();

   if (pAssistantDoc)
   {
      UINT nId = (UINT)wParam;
      pAssistantDoc->LoadDocument(nId);
      pAssistantDoc->RebuildStructureTree(this);
   }
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CDocumentStructureView::OnSelectionClose(WPARAM wParam, LPARAM lParam)
#else
void CDocumentStructureView::OnSelectionClose(WPARAM wParam, LPARAM lParam)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();

   if (pAssistantDoc)
   {
      UINT nId = (UINT)wParam;
      pAssistantDoc->CloseDocument(nId);
   }
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CDocumentStructureView::OnSelectionRemove(WPARAM wParam, LPARAM lParam)
#else
void CDocumentStructureView::OnSelectionRemove(WPARAM wParam, LPARAM lParam)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();

   if (pAssistantDoc)
   {
      UINT nId = (UINT)wParam;
      pAssistantDoc->RemoveDocumentFromList(nId);
   }
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CDocumentStructureView::OnSelectionChanged(WPARAM wParam, LPARAM lParam)
#else
void CDocumentStructureView::OnSelectionChanged(WPARAM wParam, LPARAM lParam)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();

   if (pAssistantDoc)
   {
      UINT nId = (UINT)wParam;
      pAssistantDoc->SelectContainer(nId);
   }
   if(lParam)
      pAssistantDoc->RebuildStructureTree(this);
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CDocumentStructureView::OnSelectionDelete(WPARAM wParam, LPARAM lParam)
#else
void CDocumentStructureView::OnSelectionDelete(WPARAM wParam, LPARAM lParam)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();

   if (pAssistantDoc)
   {
      UINT nId = (UINT)wParam;
      pAssistantDoc->DeleteContainer(nId);
      pAssistantDoc->RebuildStructureTree(this);///???????
   }
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CDocumentStructureView::OnSelectionSave(WPARAM wParam, LPARAM lParam)
#else
void CDocumentStructureView::OnSelectionSave(WPARAM wParam, LPARAM lParam)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();

   if (pAssistantDoc)
   {
      UINT nId = (UINT)wParam;
      CString csFileName = (LPCTSTR)lParam;
      pAssistantDoc->SaveContainer(nId, csFileName);
   }
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CDocumentStructureView::OnInsertPage(WPARAM wParam, LPARAM lParam)
#else
void CDocumentStructureView::OnInsertPage(WPARAM wParam, LPARAM lParam)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();
   if (pAssistantDoc)
   {
      UINT nId = (UINT)wParam;
      pAssistantDoc->SelectContainer(nId);
      HRESULT hr = pAssistantDoc->InsertNewPage(this);
   }
#ifdef _DVS2005
   return 1;
#endif

}

#ifdef _DVS2005
LRESULT CDocumentStructureView::OnInsertChapter(WPARAM wParam, LPARAM lParam)
#else
void CDocumentStructureView::OnInsertChapter(WPARAM wParam, LPARAM lParam)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();
   if (pAssistantDoc)
   {
      UINT nId = (UINT)wParam;
      pAssistantDoc->SelectContainer(nId);
      HRESULT hr = pAssistantDoc->InsertNewChapter(this);
   }
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CDocumentStructureView::OnMoveItem(WPARAM nIdDrag, LPARAM nIdDrop)
#else
void CDocumentStructureView::OnMoveItem(UINT nIdDrag, UINT nIdDrop)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();
   if (pAssistantDoc)
   {
      HRESULT hr = pAssistantDoc->MoveContainer(this, nIdDrag, nIdDrop, m_listElements->IsDroppedInFolder(), false);
      
   }
#ifdef _DVS2005
   return 1;
#endif

}

void CDocumentStructureView::OnPaint()
{
   //CPaintDC dc(this); // device context for painting
   // Do not call CXTPReportView::OnPaint() for painting messages
   CXTPReportView::OnPaint();
   CClientDC dc(this);
   OnDraw(&dc);
}

void CDocumentStructureView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
    CMainFrameA *pMainFrameA = CMainFrameA::GetCurrentInstance();

    if (bActivate && pMainFrameA != NULL) {
        pMainFrameA->ChangeActiveStructureView(this);

    }

   CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

BOOL CDocumentStructureView::CreateStatusBar()
{
   if(!m_bIsStatusBarCreated)
   {
      if (!m_wndStatusBar.Create(this))
      {
         TRACE0("Failed to create status bar\n");
         return FALSE;      // fail to create
      }
      m_wndStatusBar.AddIndicator(ID_SEPARATOR);

      static UINT switches[] =
      {
         ID_STRUCT_INDICATOR_NORMAL,
         ID_STRUCT_INDICATOR_TITLE
      };

      UINT nIDN = ID_STRUCT_INDICATOR_NORMAL;
      m_wndStatusBar.GetImageManager()->SetIcon(ID_STRUCT_INDICATOR_NORMAL, ID_STRUCT_INDICATOR_NORMAL);

      UINT nIDT = ID_STRUCT_INDICATOR_TITLE;
      m_wndStatusBar.GetImageManager()->SetIcon(ID_STRUCT_INDICATOR_TITLE, ID_STRUCT_INDICATOR_TITLE);

      CXTPStatusBarSwitchPane* pSwitchPane = (CXTPStatusBarSwitchPane*)m_wndStatusBar.AddIndicator(new CXTPStatusBarSwitchPane(), ID_STRUCT_INDICATOR_VIEW);
      pSwitchPane->SetSwitches(switches, sizeof(switches)/sizeof(UINT));
      pSwitchPane->SetBeginGroup(TRUE);
      pSwitchPane->SetTextFont(m_wndStatusBar.GetPaintManager()->GetIconFont());

      m_wndStatusBar.SetDrawDisabledText(FALSE);
      m_wndStatusBar.EnableCustomization(FALSE);
      m_wndStatusBar.GetPane(0)->SetEnabled(FALSE);

      m_bIsStatusBarCreated = true;
   }
   return TRUE;
}

void CDocumentStructureView::OnStatusBarSwitchView(UINT nID)
{
   CXTPStatusBarSwitchPane* pPane = (CXTPStatusBarSwitchPane*)m_wndStatusBar.FindPane(ID_STRUCT_INDICATOR_VIEW);

   // CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
   //if (pDoc && !pDoc->project.IsEmpty() && pDoc->project.HasStructure() && !pDoc->project.IsDenverDocument())
   //{
   CRect rcClient;
   GetClientRect(&rcClient);
   if(m_nCurrentLayout == (nID - ID_STRUCT_INDICATOR_NORMAL))
      return;
   switch (nID)
   {
   case ID_STRUCT_INDICATOR_NORMAL:
      {
         m_nCurrentLayout = DocumentStructureLayout(Thumbnail);
         //m_bIsLayoutChanged = true;
         //RedrawWindow(NULL, NULL, RDW_UPDATENOW);
         //pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
         //OnDraw(GetDC());
         //UpdateDrawingBitmap();
         //m_nLastActivePage = -1; // repaint everything
         //InvalidateRect(&rcClient);
         SetRecordLayout();
         GetReportCtrl().SetTreeIndent(8);
         GetReportCtrl().RedrawControl();// RedrawWindow(NULL);
         RedrawWindow(NULL);

         break;
      }
   case ID_STRUCT_INDICATOR_TITLE:
      {
         m_nCurrentLayout = DocumentStructureLayout(Compact);
         // m_bIsLayoutChanged = true;
         //UpdateDrawingBitmap();
         //m_nLastActivePage = -1; // repaint everything
         //InvalidateRect(&rcClient);
         SetRecordLayout();
         GetReportCtrl().SetTreeIndent(20);
         GetReportCtrl().RedrawControl();// RedrawWindow(NULL);
         RedrawWindow(NULL);

         //pDoc->UpdateAllViews(NULL, HINT_CHANGE_POSITION);
         break;
      }
   }
   
   GetReportCtrl().AdjustScrollBars();
   if ( GetReportCtrl().GetSelectedRows() != NULL 
      && GetReportCtrl().GetSelectedRows()->GetCount() > 0 )
      GetReportCtrl().EnsureVisible(GetReportCtrl().GetSelectedRows()->GetAt(0));
   UpdateImageList();
}

void CDocumentStructureView::OnUpdateStatusBarSwitchView(CCmdUI* pCmdUI)
{
  
   CXTPStatusBarSwitchPane* pPane = (CXTPStatusBarSwitchPane*)m_wndStatusBar.FindPane(ID_STRUCT_INDICATOR_VIEW);

   CXTPStatusBarSwitchPane::SWITCH *pbtnNormalLayout = pPane->GetSwitch(0);
   CXTPStatusBarSwitchPane::SWITCH *pbtnTitleLayout = pPane->GetSwitch(1);
   pbtnNormalLayout->bEnabled = TRUE;
   pbtnTitleLayout->bEnabled = TRUE;
   switch (m_nCurrentLayout)
   {
   case DocumentStructureLayout(Thumbnail):
      {
         //pPane->SetChecked(ID_STRUCT_INDICATOR_NORMAL);
         pbtnNormalLayout->bChecked = TRUE;
         //pbtnNormalLayout->bHighlighted = TRUE;
         pbtnTitleLayout->bChecked = FALSE;

         break;
      }
   case DocumentStructureLayout(Compact):
      {
         //pPane->SetChecked(ID_STRUCT_INDICATOR_TITLE);
         pbtnTitleLayout->bChecked = TRUE;
         //pbtnTitleLayout->bHighlighted = TRUE;
         pbtnNormalLayout->bChecked = FALSE;
         break;
      }
   }
}

void CDocumentStructureView::SetRecordLayout()
{
   int numRows = GetReportCtrl().GetRows()->GetCount();
   for(int i = 0; i < numRows; ++i)
   {
      CDocStructRecord* pRecord = (CDocStructRecord*)GetReportCtrl().GetRows()->GetAt(i)->GetRecord();
      pRecord->m_nCurrentLayout = m_nCurrentLayout;
      SetChildrenRecordLayout(pRecord, m_nCurrentLayout);
   }
}

void CDocumentStructureView::SetChildrenRecordLayout(CDocStructRecord* pRecord, int nLayout)
{
   if ( pRecord == NULL )
      return;

   pRecord->m_nCurrentLayout = nLayout;
   if ( pRecord->GetChilds() != NULL )
   {
      for ( int i = 0; i < pRecord->GetChilds()->GetCount(); i++ )
         SetChildrenRecordLayout((CDocStructRecord*)pRecord->GetChilds()->GetAt(i), nLayout);
   }
}

void CDocumentStructureView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
   GetReportCtrl().RedrawControl();
}

int CDocumentStructureView::FindItem(UINT nID, int iType)
{
   int retVal = (iType == 0)? -1 : 0;
   int numRows = GetReportCtrl().GetRows()->GetCount();
   for(int i = 0; i < numRows; ++i)
   {
      CDocStructRecord* pRecord = (CDocStructRecord*)GetReportCtrl().GetRows()->GetAt(i)->GetRecord();
      if(iType == 0)
      {
         if(pRecord->m_pContainer->GetID(pRecord->m_pvData) == nID)
            return i;
      }
      else
      {
         if(pRecord->m_pContainer->GetType(pRecord->m_pvData) == ASSISTANT::GenericContainer::DOCUMENT)
            retVal = i;
      }
   }
   return retVal;
}

//int CDocumentStructureView::GetRowHeight(CDC* /*pDC*/, CXTPReportRow* pRow)
//{
//   if(m_nCurrentLayout == DocumenStructureLayout(Compact))
//      return 20;
//   CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();
//   if(pRecord->IsPage())
//      return 115;
//   else
//      return 40;
//}
void CDocumentStructureView::SetName(void* pData, CString csText)
{
   ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
   pgc->SetName(csText);
   pgc->SetChanged(true);
}

CString CDocumentStructureView::GetName(void* pData)
{
   ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
   CString csName = pgc->GetName();
   return csName;
}

UINT CDocumentStructureView::GetType(void* pData)
{
   ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
   UINT nType = pgc->GetType();
   return nType;
}


void CDocumentStructureView::DrawThumbWhiteboard(void* pData, CRect rcThumb, CDC* pDC)
{
   ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
   pgc->DrawThumbWhiteboard(rcThumb, pDC);
}

UINT CDocumentStructureView::GetID(void* pData)
{
   ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
   return pgc->GetID();
}

void CDocumentStructureView::GetKeywords(void *pData, CString & csKeywords)
{
   ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
   pgc->GetKeywords(csKeywords);
}

void CDocumentStructureView::SetKeywords(void *pData, CString csKeywords)
{
   ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
   pgc->SetKeywords(csKeywords); 
   ASSISTANT::Project::active->GetActiveDocument()->SetChanged(true);
}

void CDocumentStructureView::LoadDocument(void *pData)
{
   ASSISTANT::GenericContainer* pgc = (ASSISTANT::GenericContainer*)pData;
   //OnSelectionLoad((WPARAM)pgc->GetID(), 0);
   GetReportCtrl().SendMessage(WM_COMMAND, ID_LOAD_LSD);
}
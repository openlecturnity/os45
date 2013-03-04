// RecordingsView.cpp : implementation file
//

#include "stdafx.h"
#include "RecordingsView.h"
#include "MainFrm.h"
#include "AssistantDoc.h"
#include "DrawingToolSettings.h"
#include "backend\la_presentationInfo.h"

#include "ProfileUtils.h"   //lsutl32

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef SubclassWindow

/////////////////////////////////////////////////////////////////////////////
// CRecordingsView

IMPLEMENT_DYNCREATE(CRecordingsView, CView)

CRecordingsView::CRecordingsView()
{
   m_listRecordings = NULL;
   m_bIsRecordSelected =FALSE;
   m_bIsToolBarCreated = false;
   
}

CRecordingsView::~CRecordingsView()
{
   if ( m_listRecordings != NULL )
   {
      delete m_listRecordings;
      m_listRecordings = NULL;
   }
}


BEGIN_MESSAGE_MAP(CRecordingsView, CView)
   //{{AFX_MSG_MAP(CRecordingsView)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_MESSAGE(WM_REMOVE_ENTRY, OnRemoveEntry)
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnUpdateList)
   ON_MESSAGE(WM_DELETE_ENTRY, OnDeleteEntry) 
   ON_MESSAGE(WM_PROPERTIES, OnProperties)
   ON_LBN_DBLCLK(IDC_RECORDINGS_LIST_BOX, OnDblclkListBox)
   
   // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
   ON_WM_ERASEBKGND()
   
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CRecordingsView diagnostics

#ifdef _DEBUG
void CRecordingsView::AssertValid() const
{
   CView::AssertValid();
}

void CRecordingsView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRecordingsView message handlers

int CRecordingsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CView::OnCreate(lpCreateStruct) == -1)
      return -1;

   CreateList();
   
   CreateToolBar();

   return 0;
}

bool CRecordingsView::CreateList()
{
   CRect rcList(7,7,249,167);

   if ( m_listRecordings == NULL )
      m_listRecordings = new CRecordingsList();

   if ( m_listRecordings->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_HASSTRINGS | LBS_OWNERDRAWVARIABLE | LBS_NOTIFY | LBS_EXTENDEDSEL, rcList, this, IDC_RECORDINGS_LIST_BOX) == TRUE )
   {
      //m_listRecordings->EnableToolTips(TRUE);
      m_listRecordings->SetListStyle(xtListBoxOffice2007);
      m_listRecordings->SetItemHeight(0, 35);
      m_listRecordings->Initialize();

      CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
      if (pDoc)
         pDoc->RebuildRecordingTree(this);
      return true;
   }
   return false;
}

void CRecordingsView::OnSize(UINT nType, int cx, int cy) 
{
    CView::OnSize(nType, cx, cy);

    UINT LIST_OFFSET = 0;
    UINT LIST_OFFSET_BOTTOM = 56;

    CRect rcClient;
    GetClientRect(&rcClient);

    CRect rcList(rcClient);
    rcList.top += LIST_OFFSET;
    rcList.bottom -= LIST_OFFSET_BOTTOM;
    rcList.left += LIST_OFFSET;
    rcList.right -= LIST_OFFSET;
    if ( m_listRecordings != NULL )
        m_listRecordings->SetWindowPos(NULL, rcList.left, rcList.top, rcList.Width(), rcList.Height(), SWP_NOZORDER);

    CRect rcRibbon;
    rcRibbon.left = rcClient.left;
    rcRibbon.top = rcClient.bottom - LIST_OFFSET_BOTTOM;
    rcRibbon.right = rcClient.right;
    rcRibbon.bottom = rcClient.bottom;
    m_wndToolBar.SetWindowPos(0, rcRibbon.left, rcRibbon.top, rcRibbon.Width(), rcRibbon.Height(), SWP_NOZORDER);
}

void CRecordingsView::OnDraw(CDC* pDC)
{
   CRect crClient;
   GetClientRect(crClient);
   CBrush bgBrush(RGB(255,255,255)); 

   m_wndToolBar.RedrawWindow();
}

void CRecordingsView::InsertPresentation(ASSISTANT::PresentationInfo *pPresentation, int nIndex)
{
   if ( m_listRecordings == NULL )
   {
      ASSERT(FALSE);
      return;
   }
   CString csTextTitle = pPresentation->GetTitle();

   int iCount = m_listRecordings->GetCount();
   CString csModTitle = csTextTitle;
   csModTitle.Replace(_T("&"), _T("&&"));

   m_listRecordings->AddString(csModTitle/*csTextTitle*/);
   m_listRecordings->SetItemDataPtr(iCount, pPresentation);

   // Selecting recordings leads to side effect 
   // (selection in document structure).
   //m_listRecordings->SelectItem(0);
   //m_listRecordings->SetFocus();
}

void CRecordingsView::RemoveAll()
{
   if ( m_listRecordings != NULL )
      m_listRecordings->ResetContent();
}

void CRecordingsView::SetSelected(UINT nItem)
{
    if ( m_listRecordings != NULL )
        m_listRecordings->SelectItem(nItem);
}

void CRecordingsView::SetAllUnselected()
{
    if ( m_listRecordings != NULL )
        m_listRecordings->SetSel(-1, FALSE);
}

#ifdef _DVS2005
LRESULT CRecordingsView::OnRemoveEntry(WPARAM wParam, LPARAM lParam)
#else
void CRecordingsView::OnRemoveEntry(WPARAM wParam, LPARAM lParam)
#endif
{
    int iSelCount = m_listRecordings->GetSelCount();
    CArray<int, int> aSelItems;
    aSelItems.SetSize(iSelCount);
    
    m_listRecordings->GetSelItems(iSelCount, aSelItems.GetData());

    int iSelection = m_listRecordings->GetCount() - 1;

    CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();
    CArray<ASSISTANT::PresentationInfo *, ASSISTANT::PresentationInfo *> aItemsToRemove;
    if (pAssistantDoc) {
        for (int i = 0; i < aSelItems.GetCount(); i++) {
            if (aSelItems.GetAt(i) < iSelection) {
                iSelection = aSelItems.GetAt(i);
            }
            ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) m_listRecordings->GetItemDataPtr(aSelItems.GetAt(i));
            if (pPresentation)
                aItemsToRemove.Add(pPresentation);
        }
    }

    for (int i = 0; i < aItemsToRemove.GetCount(); i++) {
        pAssistantDoc->RemovePresentation(aItemsToRemove.GetAt(i));
    }

    if (iSelection >= 1) {
        m_listRecordings->SetSel(iSelection - 1, TRUE);
    } else {
        m_listRecordings->SetSel(0, TRUE);
    }
   /*CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();
   if (pAssistantDoc)
   {
      ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *)lParam;
      if (pPresentation)
         pAssistantDoc->RemovePresentation(pPresentation);
   }*/
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CRecordingsView::OnDeleteEntry(WPARAM wParam, LPARAM lParam)
#else
void CRecordingsView::OnDeleteEntry(WPARAM wParam, LPARAM lParam)
#endif
{
    int iSelCount = m_listRecordings->GetSelCount();
    CArray<int, int> aSelItems;
    aSelItems.SetSize(iSelCount);
    
    m_listRecordings->GetSelItems(iSelCount, aSelItems.GetData());

    int iSelection = m_listRecordings->GetCount() - 1;

    CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();
    CArray<ASSISTANT::PresentationInfo *, ASSISTANT::PresentationInfo *> aItemsToDelete;
    if (pAssistantDoc) {
        for (int i = 0; i < aSelItems.GetCount(); i++) {
            if (aSelItems.GetAt(i) < iSelection) {
                iSelection = aSelItems.GetAt(i);
            }
            ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) m_listRecordings->GetItemDataPtr(aSelItems.GetAt(i));
            if (pPresentation)
                aItemsToDelete.Add(pPresentation);
        }
    }

    for (int i = 0; i < aItemsToDelete.GetCount(); i++) {
        pAssistantDoc->DeletePresentation(aItemsToDelete.GetAt(i));
    }

    if (iSelection >= 1) {
        m_listRecordings->SetSel(iSelection - 1, TRUE);
    } else {
        m_listRecordings->SetSel(0, TRUE);
    }
   /*CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();
   if (pAssistantDoc)
   {
      ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *)lParam;
      if (pPresentation)
         pAssistantDoc->DeletePresentation(pPresentation);
   }*/
#ifdef _DVS2005
   return 1;
#endif
}

#ifdef _DVS2005
LRESULT CRecordingsView::OnProperties(WPARAM wParam, LPARAM lParam)
#else
void CRecordingsView::OnProperties(WPARAM wParam, LPARAM lParam)
#endif
{
   CAssistantDoc *pAssistantDoc = (CAssistantDoc *)GetDocument();

   if (pAssistantDoc)
   {
      ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *)lParam;
      if (pPresentation)
         pAssistantDoc->RebuildRecordingTree(this);
   }
#ifdef _DVS2005
   return 1;
#endif

}

void CRecordingsView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
    CMainFrameA *pMainFrameA = CMainFrameA::GetCurrentInstance();

    if (bActivate && pMainFrameA != NULL) {
        pMainFrameA->ChangeActiveStructureView(this);

    }

   CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

BOOL CRecordingsView::IsRecordSelected() 
{
   return m_bIsRecordSelected;	
}

void CRecordingsView::SetRecordSelected(BOOL bSelected) 
{
   m_bIsRecordSelected = bSelected;	
}

LRESULT CRecordingsView::OnUpdateList(WPARAM wParam,LPARAM lParam)
{
   if ( m_listRecordings == NULL )
   {
      ASSERT(FALSE);
      return 0L;
   }
   if(m_listRecordings->GetSelectedItem() == -1)
      SetRecordSelected(FALSE);
   else
      SetRecordSelected(TRUE);

   return 0L;
}

CRecordingsList* CRecordingsView::GetRecordList()
{
   if ( m_listRecordings == NULL )
   {
      ASSERT(FALSE);
      CreateList();
   }
   return m_listRecordings;
}

void CRecordingsView::ChangePublishButtonImage(int iProfileID, int iImageID, __int64 lCustomProfileID) {
    ProfileUtils::SetLastActivatedProfile(iProfileID, lCustomProfileID);
    m_wndToolBar.ChangeProfileImage(iImageID);
}

void CRecordingsView::FillProfileGallery(CXTPControlGallery* pGallery, 
                                         CXTPControlGalleryItems *pItems) {
    if (pGallery == NULL || pItems == NULL)
        m_wndToolBar.FillProfileGallery();
    else
        CRecordingsToolBar::FillProfileGallery(pGallery, pItems);
}

void CRecordingsView::ShowProfileSelectionDialog() {
    if (m_listRecordings)
        m_listRecordings->PublishRecordFormatSelection();
}

BOOL CRecordingsView::OnEraseBkgnd(CDC* pDC)
{
    // Erasing the background leads to flickering
    return TRUE;
}

void CRecordingsView::OnDblclkListBox()
{
   if ( m_listRecordings == NULL )
   {
      ASSERT(FALSE);
      return;
   }
	m_listRecordings->ReplayRecord();
}


BOOL CRecordingsView::CreateToolBar() {
    if(!m_bIsToolBarCreated) {
        CRect rcToolbar(0, 0, 50, 50);

        if (!m_wndToolBar.CreateToolBar(WS_VISIBLE | WS_CHILD, this)) {
            TRACE0("Failed to create tool bar\n");
            return FALSE;      // fail to create
        }
        m_bIsToolBarCreated = true;
    }
    return TRUE;
}

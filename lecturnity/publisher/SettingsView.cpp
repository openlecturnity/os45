// SettingsView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Publisher.h"
#include "PublisherDoc.h"
#include "SettingsView.h"

#include "SettingsTargetPage.h"
#include "SettingsTemplatePage.h"
#include "SettingsScormPage.h"
#include "SettingsLpdPage.h"
#include "SettingsSlidesPage.h"
#include "SettingsVideoClipsPage.h"
//#include "SettingsWmPage.h"
//#include "SettingsRmPage.h"
#include "SettingsMediaPage.h"
#include "SettingsTransferPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsView

IMPLEMENT_DYNCREATE(CSettingsView, CFormView)

CSettingsView::CSettingsView()
	: CFormView(CSettingsView::IDD)
{
	//{{AFX_DATA_INIT(CSettingsView)
	//}}AFX_DATA_INIT

   m_caPropPages.RemoveAll();
   m_nLastDisplayedTarget = -1;
   m_rcMinSize.SetRect(-1, -1, -1, -1);
}

CSettingsView::~CSettingsView()
{
}

void CSettingsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsView)
	DDX_Control(pDX, IDC_SETTINGS_CAPTION, m_titleCaption);
	DDX_Control(pDX, IDC_SETTINGS_TARGETFORMAT, m_cbbTargetFormat);
	DDX_Control(pDX, IDC_SETTINGS_TAB, m_tabCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsView, CFormView)
	//{{AFX_MSG_MAP(CSettingsView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SETTINGS_TAB, OnSettingsTabChange)
	ON_WM_HELPINFO()
	ON_CBN_SELCHANGE(IDC_SETTINGS_TARGETFORMAT, OnChangeTargetFormat)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Diagnose CSettingsView

#ifdef _DEBUG
void CSettingsView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSettingsView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSettingsView 

void CSettingsView::OnInitialUpdate() 
{
   // This will call OnUpdate() of the view, so let's keep it like that.
   // This method only checks to see if all necessary property sheets are
   // available.
	CFormView::OnInitialUpdate();

   // This method is called each time the view is attached to a new document,
   // so we'll have to check if we need some new tabs or stuff.
   //RemoveAllPropPages();
   //AddNeededPropPages();

   //GetParentFrame()->RecalcLayout();
   //ResizeParentToFit();
}

void CSettingsView::RemoveAllPropPages()
{
   m_tabCtrl.DeleteAllItems();
   int nCount = m_caPropPages.GetSize();
   for (int i=0; i<nCount; ++i)
   {
      m_caPropPages[i]->ShowWindow(SW_HIDE);
      delete m_caPropPages[i];
   }

   m_caPropPages.RemoveAll();
}

void CSettingsView::AddNeededPropPages()
{
   CPublisherDoc *pDoc = (CPublisherDoc *) GetDocument();

   switch (pDoc->nTargetFormat)
   {
   case 0: // LECTURNITY
      AddPropPage(IDD_SETTINGS_LPD);
      AddPropPage(IDD_SETTINGS_TRANSFER);
      break;
   case 1: // Streaming
      AddPropPage(IDD_SETTINGS_MEDIA);
      AddPropPage(IDD_SETTINGS_TEMPLATE);
      AddPropPage(IDD_SETTINGS_SCORM);
      AddPropPage(IDD_SETTINGS_SLIDES);
      AddPropPage(IDD_SETTINGS_VIDEOCLIPS);
      AddPropPage(IDD_SETTINGS_TRANSFER);
      break;
   case 2: // Flash
      AddPropPage(IDD_SETTINGS_TRANSFER);
      break;
   }

   CalcMinSize();
   PositionPropPages();

   m_caPropPages[0]->ShowWindow(SW_SHOW);
   m_caPropPages[0]->RedrawWindow();
}

void CSettingsView::AddPropPage(int nID)
{
   CCustomPropPage *pPage = NULL;

   switch (nID)
   {
   case IDD_SETTINGS_TARGET:
      pPage = new CSettingsTargetPage;
      break;
   case IDD_SETTINGS_TEMPLATE:
      pPage = new CSettingsTemplatePage;
      break;
   case IDD_SETTINGS_SCORM:
      pPage = new CSettingsScormPage;
      break;
   case IDD_SETTINGS_LPD:
      pPage = new CSettingsLpdPage;
      break;
   case IDD_SETTINGS_SLIDES:
      pPage = new CSettingsSlidesPage;
      break;
   case IDD_SETTINGS_VIDEOCLIPS:
      pPage = new CSettingsVideoClipsPage;
      break;
   /*
   case IDD_SETTINGS_WM:
      pPage = new CSettingsWmPage;
      break;
   case IDD_SETTINGS_RM:
      pPage = new CSettingsRmPage;
      break;
   */
   case IDD_SETTINGS_MEDIA:
      pPage = new CSettingsMediaPage;
      break;
   case IDD_SETTINGS_TRANSFER:
      pPage = new CSettingsTransferPage;
      break;
   default:
      break;
   }

   if (pPage)
   {
      m_caPropPages.Add(pPage);

      CString csTitle;
      csTitle.LoadString(nID);
      m_tabCtrl.InsertItem(m_tabCtrl.GetItemCount(), csTitle);

      pPage->Create(&m_tabCtrl);
      //pPage->SetWindowPos(&wndTop, 2, 42, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
      pPage->ShowWindow(SW_HIDE);
   }
}

void CSettingsView::PositionPropPages()
{
   CRect rcClient;
   m_tabCtrl.GetClientRect(&rcClient);
   int nCount = m_tabCtrl.GetItemCount();
   for (int i=0; i<nCount; ++i)
   {
      CRect rcItem;
      m_tabCtrl.GetItemRect(i, &rcItem);
      if (rcItem.bottom > rcClient.top)
         rcClient.top = rcItem.bottom;
   }

   rcClient.top += 1;
   rcClient.left += 2;
   rcClient.right -= 3;
   rcClient.bottom -= 3;

   for (i=0; i<nCount; ++i)
   {
      m_caPropPages[i]->SetWindowPos(&wndTop, rcClient.left, rcClient.top, 
         rcClient.Width(), rcClient.Height(), SWP_NOZORDER);
   }
}

void CSettingsView::CalcMinSize()
{
   CRect rcTabClient;
   m_tabCtrl.GetWindowRect(&rcTabClient);
   ScreenToClient(&rcTabClient);
   CRect rcLargestPage;
   m_caPropPages[0]->GetClientRect(&rcLargestPage);
   int nCount = m_tabCtrl.GetItemCount();
   for (int i=0; i<nCount; ++i)
   {
      CRect rcItem;
      m_caPropPages[i]->GetClientRect(&rcItem);
      if (rcItem.right > rcLargestPage.right)
         rcLargestPage.right = rcItem.right;
      if (rcItem.bottom > rcLargestPage.bottom)
         rcLargestPage.bottom = rcItem.bottom;
   }

   int nTabOffset = 0;
   for (i=0; i<nCount; ++i)
   {
      CRect rcItem;
      m_tabCtrl.GetItemRect(i, &rcItem);
      if (rcItem.bottom > nTabOffset)
         nTabOffset = rcItem.bottom;
   }

   m_rcMinSize.left = 0;
   m_rcMinSize.top = 0;
   m_rcMinSize.right = rcTabClient.left + rcLargestPage.Width() + 15;
   m_rcMinSize.bottom = rcTabClient.top + rcLargestPage.Height() + nTabOffset + 15;
}

void CSettingsView::OnSettingsTabChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen

   //MessageBox(_T("Ping!"));
   int nCount = m_tabCtrl.GetItemCount();
   int nIndex = m_tabCtrl.GetCurSel();
   for (int i=0; i<nCount; ++i)
   {
      if (i != nIndex)
         m_caPropPages[i]->ShowWindow(SW_HIDE);
   }
   m_caPropPages[nIndex]->ShowWindow(SW_SHOW);
	
	*pResult = 0;
}

BOOL CSettingsView::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
   MessageBox(_T("Wahnsinnstolle Hilfestellung zur allgemeinen Handhabung von Profilen und den Einstellungen"),
      _T("Hilfe"), MB_OK | MB_ICONQUESTION);

	return TRUE; //CFormView::OnHelpInfo(pHelpInfo);
}

void CSettingsView::OnChangeTargetFormat() 
{
   CPublisherDoc *pDoc = (CPublisherDoc *) GetDocument();

   //RemoveAllPropPages();
   pDoc->nTargetFormat = m_cbbTargetFormat.GetCurSel();
   pDoc->UpdateAllViews(NULL);
   //AddNeededPropPages();
   //m_cbbTargetFormat.SetFocus();
}

void CSettingsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   CPublisherDoc *pDoc = (CPublisherDoc *) GetDocument();

   if (pDoc->nTargetFormat != m_nLastDisplayedTarget)
   {
      RemoveAllPropPages();
      AddNeededPropPages();
      m_nLastDisplayedTarget = pDoc->nTargetFormat;
      if (m_cbbTargetFormat.GetCurSel() != pDoc->nTargetFormat)
         m_cbbTargetFormat.SetCurSel(pDoc->nTargetFormat);
   }
}

void CSettingsView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
   int nScrollX = cx;
   int nScrollY = cy;

   if (nScrollX < m_rcMinSize.Width())
      nScrollX = m_rcMinSize.Width();
   if (nScrollY < m_rcMinSize.Height())
      nScrollY = m_rcMinSize.Height();

   int nMargin = 0;

   if (IsWindow(m_tabCtrl.GetSafeHwnd()))
   {
      CRect rcTabClient;
      m_tabCtrl.GetWindowRect(&rcTabClient);
      ScreenToClient(&rcTabClient);
      nMargin = rcTabClient.left;
      rcTabClient.right = nScrollX - nMargin;
      rcTabClient.bottom = nScrollY - nMargin;
      m_tabCtrl.SetWindowPos(&wndTop, rcTabClient.left, rcTabClient.top,
         rcTabClient.Width(), rcTabClient.Height(), SWP_NOZORDER);

      PositionPropPages();

      CRect rcTargetCombo;
      m_cbbTargetFormat.GetWindowRect(&rcTargetCombo);
      ScreenToClient(&rcTargetCombo);
      rcTargetCombo.right = nScrollX - nMargin;
      m_cbbTargetFormat.SetWindowPos(&wndTop, 0, 0, rcTargetCombo.Width(), rcTargetCombo.Height(),
         SWP_NOZORDER | SWP_NOMOVE);

      CRect rcTitle;
      m_titleCaption.GetWindowRect(&rcTitle);
      ScreenToClient(&rcTitle);
      m_titleCaption.SetWindowPos(&wndTop, 0, 0, cx, rcTitle.Height(), SWP_NOZORDER | SWP_NOMOVE);
   }

   SetScrollSizes(MM_TEXT, CSize(nScrollX - nMargin, nScrollY - nMargin));
}

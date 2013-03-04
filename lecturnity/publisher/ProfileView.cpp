// ProfileView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "publisher.h"
#include "ProfileView.h"
#include "PublisherDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProfileView

IMPLEMENT_DYNCREATE(CProfileView, CFormView)

CProfileView::CProfileView()
	: CFormView(CProfileView::IDD)
{
	//{{AFX_DATA_INIT(CProfileView)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

CProfileView::~CProfileView()
{
}

void CProfileView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProfileView)
	DDX_Control(pDX, IDC_PROFILE_STARTPUBLISH, m_btPublish);
	DDX_Control(pDX, IDC_PROFILE_SELECT, m_cbbProfile);
	DDX_Control(pDX, IDC_PROFILE_CAPTION, m_titleCaption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProfileView, CFormView)
	//{{AFX_MSG_MAP(CProfileView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PROFILE_STARTPUBLISH, OnStartPublish)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Diagnose CProfileView

#ifdef _DEBUG
void CProfileView::AssertValid() const
{
	CFormView::AssertValid();
}

void CProfileView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CProfileView 

void CProfileView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
   if (::IsWindow(m_titleCaption.GetSafeHwnd()))
   {
      CRect rcTitle;
      m_titleCaption.GetWindowRect(&rcTitle);
      ScreenToClient(&rcTitle);
      m_titleCaption.SetWindowPos(&wndTop, 0, 0, cx, rcTitle.Height(), SWP_NOZORDER | SWP_NOMOVE);
   }

   if (::IsWindow(m_cbbProfile.GetSafeHwnd()))
   {
      int nMargin = 10; // Pixel
      CRect rcPublish;
      m_btPublish.GetWindowRect(&rcPublish);
      ScreenToClient(&rcPublish);
      CRect rcProfile;
      m_cbbProfile.GetWindowRect(&rcProfile);
      ScreenToClient(&rcProfile);
      rcProfile.right = cx - rcPublish.Width() - nMargin - 3;
      rcPublish.left = cx - rcPublish.Width() - nMargin;

      m_btPublish.SetWindowPos(&wndTop, rcPublish.left, rcPublish.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
      m_cbbProfile.SetWindowPos(&wndTop, 0, 0, rcProfile.Width(), rcProfile.Height(),
         SWP_NOZORDER | SWP_NOMOVE);
   }
}

void CProfileView::OnStartPublish() 
{
	CPublisherDoc *pDoc = (CPublisherDoc *) GetDocument();
   pDoc->StartPublish();
}

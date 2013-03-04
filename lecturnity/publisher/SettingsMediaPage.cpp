// SettingsMediaPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "publisher.h"
#include "SettingsMediaPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsMediaPage 


CSettingsMediaPage::CSettingsMediaPage(CWnd* pParent /*=NULL*/)
	: CCustomPropPage(CSettingsMediaPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsMediaPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CSettingsMediaPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsMediaPage)
	DDX_Control(pDX, IDC_MEDIA_FORMAT, m_cbbMediaFormat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsMediaPage, CCustomPropPage)
	//{{AFX_MSG_MAP(CSettingsMediaPage)
	ON_CBN_SELCHANGE(IDC_MEDIA_FORMAT, OnChangeMediaFormat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSettingsMediaPage 

BOOL CSettingsMediaPage::OnInitDialog() 
{
	CCustomPropPage::OnInitDialog();
	
   m_cbbMediaFormat.SetCurSel(0);
   CRect rcCombo;
   m_cbbMediaFormat.GetWindowRect(&rcCombo);
   m_cbbMediaFormat.ScreenToClient(&rcCombo);

   int nY = rcCombo.bottom + 12;

   m_wmPage.Create(this);
   m_wmPage.SetWindowPos(&wndTop, 0, nY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
   m_wmPage.ShowWindow(SW_SHOW);
   m_rmPage.Create(this);
   m_rmPage.SetWindowPos(&wndTop, 0, nY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
   m_rmPage.ShowWindow(SW_HIDE);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CSettingsMediaPage::OnChangeMediaFormat() 
{
	if (m_cbbMediaFormat.GetCurSel() == 0)
   {
      // Windows Media
      m_rmPage.ShowWindow(SW_HIDE);
      m_wmPage.ShowWindow(SW_SHOW);
   }
   else
   {
      // Real Media
      m_wmPage.ShowWindow(SW_HIDE);
      m_rmPage.ShowWindow(SW_SHOW);
   }
}

// SettingsScormPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Publisher.h"
#include "SettingsScormPage.h"

#include "PublisherLibs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsScormPage 


CSettingsScormPage::CSettingsScormPage(CWnd* pParent /*=NULL*/)
	: CCustomPropPage(CSettingsScormPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsScormPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CSettingsScormPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsScormPage)
	DDX_Control(pDX, IDC_SETTINGS_SCORMSTRICT, m_cbScormStrict);
	DDX_Control(pDX, IDC_SETTINGS_SCORM, m_cbScorm);
	DDX_Control(pDX, IDC_SCORM_CONFIGURE, m_btConfigure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsScormPage, CCustomPropPage)
	//{{AFX_MSG_MAP(CSettingsScormPage)
	ON_BN_CLICKED(IDC_SCORM_CONFIGURE, OnScormConfigure)
	ON_BN_CLICKED(IDC_SETTINGS_SCORM, OnChangeScorm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSettingsScormPage 

void CSettingsScormPage::OnScormConfigure() 
{
   Scorm_Configure(GetSafeHwnd());
}

void CSettingsScormPage::OnChangeScorm() 
{
   BOOL bEnable = (m_cbScorm.GetCheck() == BST_CHECKED) ? TRUE : FALSE;
   m_cbScormStrict.EnableWindow(bEnable);
   m_btConfigure.EnableWindow(bEnable);
}

BOOL CSettingsScormPage::OnInitDialog() 
{
	CCustomPropPage::OnInitDialog();

   // Defaults
   m_cbScorm.SetCheck(BST_CHECKED);
   m_cbScormStrict.SetCheck(BST_UNCHECKED);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

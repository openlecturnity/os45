// SettingsTransferPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "publisher.h"
#include "SettingsTransferPage.h"

#include "PublisherLibs.h" // LBLibs

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsTransferPage 


CSettingsTransferPage::CSettingsTransferPage(CWnd* pParent /*=NULL*/)
	: CCustomPropPage(CSettingsTransferPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsTransferPage)
	//}}AFX_DATA_INIT
}


void CSettingsTransferPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsTransferPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsTransferPage, CCustomPropPage)
	//{{AFX_MSG_MAP(CSettingsTransferPage)
	ON_BN_CLICKED(IDC_TRANSFER_CONFIGURE0, OnTransferConfigure0)
	ON_BN_CLICKED(IDC_TRANSFER_CONFIGURE1, OnTransferConfigure1)
	ON_BN_CLICKED(IDC_TRANSFER_CONFIGURE2, OnTransferConfigure2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSettingsTransferPage 

void CSettingsTransferPage::OnTransferConfigure0() 
{
   Transfer_Configure(0, GetSafeHwnd());
}

void CSettingsTransferPage::OnTransferConfigure1() 
{
   Transfer_Configure(1, GetSafeHwnd());
}

void CSettingsTransferPage::OnTransferConfigure2() 
{
   Transfer_Configure(2, GetSafeHwnd());
}

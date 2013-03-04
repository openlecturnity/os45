// TransferConfigure.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TransferConfigure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CTransferConfigure 

IMPLEMENT_DYNCREATE(CTransferConfigure, CPropertyPage)

CTransferConfigure::CTransferConfigure() : CPropertyPage(CTransferConfigure::IDD)
{
	//{{AFX_DATA_INIT(CTransferConfigure)
	//}}AFX_DATA_INIT
}

CTransferConfigure::~CTransferConfigure()
{
}

void CTransferConfigure::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransferConfigure)
	DDX_Control(pDX, IDC_TRANSFER_SCP, m_rbScp);
	DDX_Control(pDX, IDC_TRANSFER_HTTP, m_rbHttp);
	DDX_Control(pDX, IDC_TRANSFER_LBL_PORT, m_lblPort);
	DDX_Control(pDX, IDC_TRANSFER_FTP, m_rbFtp);
	DDX_Control(pDX, IDC_TRANSFER_LBL_TARGET, m_lblTarget);
	DDX_Control(pDX, IDC_TRANSFER_LOCAL, m_rbLocal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferConfigure, CPropertyPage)
	//{{AFX_MSG_MAP(CTransferConfigure)
	ON_BN_CLICKED(IDC_TRANSFER_LOCAL, OnTransferChangeTechnique)
	ON_BN_CLICKED(IDC_TRANSFER_SCP, OnTransferChangeTechnique)
	ON_BN_CLICKED(IDC_TRANSFER_HTTP, OnTransferChangeTechnique)
	ON_BN_CLICKED(IDC_TRANSFER_FTP, OnTransferChangeTechnique)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTransferConfigure 

void CTransferConfigure::OnTransferChangeTechnique() 
{
   if (m_rbLocal.GetCheck())
      m_lblTarget.SetWindowText(_T("Pfad: "));
   else
      m_lblTarget.SetWindowText(_T("Server: "));
}

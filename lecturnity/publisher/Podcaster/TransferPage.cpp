// TransferPage.cpp : implementation file
//

#include "stdafx.h"
#include "podcaster.h"
#include "TransferPage.h"
#include "TransferSettings.h"
#include "GeneralSettings.h"
#include "Registry.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransferPage dialog


CTransferPage::CTransferPage(CWnd* pParent /*=NULL*/)
	: CDialog(CTransferPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransferPage)
	m_csService = _T("??");
	m_csChannel = _T("??");
	m_csCompression = _T("??");
	m_csServer = _T("??");
	m_csIP = _T("??");
	m_csPassword = _T("??");
	m_csPort = _T("??");
	m_csSave = _T("??");
	m_Ssh = _T("??");
	m_csUsername = _T("??");
	m_csWebUrl = _T("??");
	m_csMp4Dir = _T("??");
	m_csPkf = _T("??");
	m_csXmlDir = _T("??");
	//}}AFX_DATA_INIT
}


void CTransferPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransferPage)
	DDX_Control(pDX, IDB_CHANGE, m_btnChange);
	DDX_Text(pDX, IDC_SERVICE, m_csService);
	DDX_Text(pDX, IDC_CHANNEL, m_csChannel);
	DDX_Text(pDX, IDC_COMPRESSION, m_csCompression);
	DDX_Text(pDX, IDC_HOSTNAME, m_csServer);
	DDX_Text(pDX, IDC_IP, m_csIP);
	DDX_Text(pDX, IDC_PASSWORD, m_csPassword);
	DDX_Text(pDX, IDC_PORT, m_csPort);
	DDX_Text(pDX, IDC_SAVE, m_csSave);
	DDX_Text(pDX, IDC_SSH, m_Ssh);
	DDX_Text(pDX, IDC_USERNAME, m_csUsername);
	DDX_Text(pDX, IDC_WEBURL, m_csWebUrl);
	DDX_Text(pDX, IDC_MP4DIR, m_csMp4Dir);
	DDX_Text(pDX, IDC_PKF, m_csPkf);
	DDX_Text(pDX, IDC_XMLDIR, m_csXmlDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferPage, CDialog)
	//{{AFX_MSG_MAP(CTransferPage)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDB_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransferPage message handlers

BOOL CTransferPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   color = RGB(255, 255, 255);
   brush.CreateSolidBrush(color);

   DWORD dwID[] = {
      	IDL_SERVICE,
      	IDL_CHANNEL,
	      IDL_MP4URL,
	      IDL_SERVER,
	      IDL_PORT,
	      IDL_USERNAME,
	      IDL_PASSWORD,
	      IDL_SAVEPSWD,
	      IDL_CHANNELDIR,
	      IDL_MP4DIR,
	      IDL_SSHV,
	      IDL_IPV,
	      IDL_COMPRESSION,
	      IDL_PKK,
         IDB_CHANGE,
         -1 };
   MfcUtils::Localize(this, dwID);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CTransferPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
   pDC->SetBkColor(color);
   hbr = (HBRUSH)brush;
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CTransferPage::LoadFromRegistry()
{
   // Get a pointer to Application object
   CRegistry reg;

   if(reg.Open(m_csChannelName) == ERROR_SUCCESS)
   {
      UpdateData();

      int iService = reg.GetProfileInt(_T("Service"), 0);
      switch(iService)
      {
         case 0:
            m_csService = _T("FTP");
            break;
         case 1:
            m_csService = _T("SCP");
            break;
         case 2:
            m_csService = _T("SFTP");
            break;
      }
      m_csChannel  = reg.GetProfileString(_T("Channel"), _T("??"));
      m_csServer   = reg.GetProfileString(_T("Server"), _T("??"));
      m_csPort.Format(_T("%d"), reg.GetProfileInt(_T("Port"), 21));   
      m_csUsername = reg.GetProfileString(_T("Username"), _T(""));
      m_csPassword = reg.GetProfileString(_T("Password"), _T(""));
   
      BOOL bSavePassword = reg.GetProfileInt(_T("SavePassword"), TRUE);   
      m_csSave = (bSavePassword) ? _T("Yes") : _T("No");
   
      m_csXmlDir = reg.GetProfileString(_T("ChannelDirectory"), _T(""));
      m_csMp4Dir = reg.GetProfileString(_T("MediaDirectory"), _T(""));
      m_csWebUrl = reg.GetProfileString(_T("MediaFilePath"), _T(""));

      int iSsh = reg.GetProfileInt(_T("SSHVersion"), 1);
      m_Ssh = (iSsh == 0) ? _T("1") : _T("2");

      int iIP = reg.GetProfileInt(_T("IPVersion"), 0);
      m_csIP = (iIP == 0) ? _T("4") : _T("6");
   

      BOOL bCompression = reg.GetProfileInt(_T("Compression"), 0);
      m_csCompression = (bCompression) ? _T("Yes") : _T("No");
   
      BOOL bPrivateKey  = reg.GetProfileInt(_T("PrivateKey"), 0);
      m_csPkf = (bPrivateKey) ? _T("Yes") : _T("No");

      UpdateData(FALSE);
      reg.Close();
   }
}

void CTransferPage::ClearPage()
{
   UpdateData();
	
   m_csService     = _T("??");
	m_csChannel     = _T("??");
	m_csCompression = _T("??");
	m_csServer      = _T("??");
	m_csIP          = _T("??");
	m_csPassword    = _T("??");
	m_csPort        = _T("??");
	m_csSave        = _T("??");
	m_Ssh           = _T("??");
	m_csUsername    = _T("??");
	m_csWebUrl      = _T("??");
	m_csMp4Dir      = _T("??");
	m_csPkf         = _T("??");
	m_csXmlDir      = _T("??");
   
   UpdateData(FALSE);
}

BOOL CTransferPage::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
   UpdateData();
   UpdateData(FALSE);
	
	return CDialog::DestroyWindow();
}

void CTransferPage::OnCancel() 
{
	// TODO: Add extra cleanup here
	//CDialog::OnCancel();
}


void CTransferPage::OnChange() 
{
	// TODO: Add your control notification handler code here
   CTransferSettings dlg;
   dlg.m_csChannelName = m_csChannelName;
   if(dlg.DoModal() == IDOK)
   {
      LoadFromRegistry();
   }	

   // refresh the page
   CGeneralSettings *pWnd = (CGeneralSettings *)this->GetParent();
   pWnd->Refresh();
}

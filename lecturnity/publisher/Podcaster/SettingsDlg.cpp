// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Podcaster.h"
#include "SettingsDlg.h"
#include "TransferSettings.h"
#include "MfcUtils.h"
#include "Registry.h"

#include <Wininet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog


CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CSettingsDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CSettingsDlg)
   m_iService = 0;
   m_csChannel = _T("");
   m_csServer = _T("");
   m_nPort = 21;
   m_csUsername = _T("");
   m_csPassword = _T("");
   m_csMediaFileUrl = _T("");
   m_csMediaDirectory = _T("");
   m_csChannelDirectory = _T("");
   m_bSavePassword = FALSE;
	m_iSSHVersion = 1;
	m_iIPVersion = 0;
	m_bCompression = FALSE;
	m_bPrivateKey = FALSE;
	//}}AFX_DATA_INIT
   m_hIcon = AfxGetApp()->LoadIcon(IDI_PODCASTER);
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Control(pDX, IDL_PKK, m_btnPrivateKey);
	DDX_Control(pDX, IDL_COMPRESSION, m_btnCompression);
	DDX_Control(pDX, IDC_IPV, m_boxIPVersion);
	DDX_Control(pDX, IDC_SSHV, m_boxSSHVersion);
   DDX_CBIndex(pDX, IDC_SERVICE, m_iService);
   DDX_Text(pDX, IDE_XMLURL, m_csChannel);
   DDX_Text(pDX, IDE_HOST, m_csServer);
   DDX_Text(pDX, IDE_PORT, m_nPort);
   DDX_Text(pDX, IDE_USERNAME, m_csUsername);
   DDX_Text(pDX, IDE_PASSWORD, m_csPassword);
   DDX_Text(pDX, IDE_MP4URL, m_csMediaFileUrl);
   DDX_Text(pDX, IDE_MP4DIR, m_csMediaDirectory);
   DDX_Text(pDX, IDE_CHANNELDIR, m_csChannelDirectory);
   DDX_Check(pDX, IDL_SAVEPSWD, m_bSavePassword);
	DDX_CBIndex(pDX, IDC_SSHV, m_iSSHVersion);
	DDX_CBIndex(pDX, IDC_IPV, m_iIPVersion);
	DDX_Check(pDX, IDL_COMPRESSION, m_bCompression);
	DDX_Check(pDX, IDL_PKK, m_bPrivateKey);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
   //{{AFX_MSG_MAP(CSettingsDlg)
   ON_CBN_SELCHANGE(IDC_SERVICE, OnSelchangeService)
   ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDL_PKK, OnPrivateKeyFile)
   ON_WM_PAINT()
	//ON_EN_CHANGE(IDE_XMLURL, OnChangeXmlurl)
	//ON_EN_CHANGE(IDE_MP4URL, OnChangeMp4url)
	//ON_EN_CHANGE(IDE_HOST, OnChangeHost)
	//ON_EN_CHANGE(IDE_CHANNELDIR, OnChangeChanneldir)
	//ON_EN_CHANGE(IDE_MP4DIR, OnChangeMp4dir)
	ON_EN_KILLFOCUS(IDE_XMLURL, OnChangeXmlurl)
	ON_EN_KILLFOCUS(IDE_MP4URL, OnChangeMp4url)
	ON_EN_KILLFOCUS(IDE_HOST, OnChangeHost)
	ON_EN_KILLFOCUS(IDE_CHANNELDIR, OnChangeChanneldir)
	ON_EN_KILLFOCUS(IDE_MP4DIR, OnChangeMp4dir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers

BOOL CSettingsDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   // Fill the controls with values

   OnSelchangeService();

   color = RGB(255, 255, 255);
   brush.CreateSolidBrush(color);

   //strChannel = _T("");
   //LoadFromRegistry(strChannel);

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
	      IDG_SERVER,
         -1 };
   MfcUtils::Localize(this, dwID);

   /*
   CString csCaption;
   csCaption.LoadString(IDD_SETTINGS_DIALOG);
   this->SetWindowText(csCaption);
   */

   ::SendMessage(::GetDlgItem(m_hWnd, IDE_PASSWORD), EM_SETPASSWORDCHAR, _T('*'), 0);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDlg::LoadFromRegistry(CString pszKey)
{
   // Get a pointer to Application object
   CRegistry reg;  
   if(reg.Open(pszKey) == ERROR_SUCCESS)
   {
      UpdateData();
	  CString csTemp(_T(""));
      m_iService      = reg.GetProfileInt(_T("Service"), FTP_SERVICE);   
      csTemp     = reg.GetProfileString(_T("Channel"), _T(""));
	  m_csChannel = csTemp.GetString();
      csTemp      = reg.GetProfileString(_T("Server"), _T(""));
	  m_csServer = csTemp.GetString();
      m_nPort         = reg.GetProfileInt(_T("Port"), INTERNET_DEFAULT_FTP_PORT);   
      csTemp    = reg.GetProfileString(_T("Username"), _T(""));
	  m_csUsername = csTemp.GetString();
      m_bSavePassword = reg.GetProfileInt(_T("SavePassword"), TRUE);   
      if(m_bSavePassword)
      {
         CString csAux = reg.GetProfileString(_T("Password"), _T(""));
		 CString csPassword = csAux.GetString();
         m_csPassword = m_Crypto.Decode(csPassword);
      }
      csTemp = reg.GetProfileString(_T("ChannelDirectory"), _T("/"));
	  m_csChannelDirectory = csTemp.GetString();
      csTemp   = reg.GetProfileString(_T("MediaDirectory"), _T("/"));
	  m_csMediaDirectory = csTemp.GetString();
      csTemp     = reg.GetProfileString(_T("MediaFilePath"), _T(""));
	  m_csMediaFileUrl = csTemp.GetString();

      csTemp   = reg.GetProfileString(_T("PrivateKeyFile"), _T(""));
	  m_csPrivateKeyFile = csTemp.GetString();

      m_iSSHVersion  = reg.GetProfileInt(_T("SSHVersion"), -1);
      m_iIPVersion   = reg.GetProfileInt(_T("IPVersion"), -1);
      m_bCompression = reg.GetProfileInt(_T("Compression"), 0);
      m_bPrivateKey  = reg.GetProfileInt(_T("PrivateKey"), 0);

      BOOL bActivate = (m_iService != 0);

      m_boxSSHVersion.EnableWindow(bActivate);
      m_boxIPVersion.EnableWindow(bActivate);
      m_btnCompression.EnableWindow(bActivate);
      m_btnPrivateKey.EnableWindow(bActivate);

      UpdateData(FALSE);
      reg.Close();
   }
}

void CSettingsDlg::SaveToRegistry(CString pszKey)
{
   CRegistry reg;  
   if(reg.Create(pszKey) == ERROR_SUCCESS)
   {
      // Write the information to the registry.
      UpdateData();
      reg.WriteProfileInt(_T("Service"), m_iService);   
      reg.WriteProfileString(_T("Channel"), m_csChannel);
      reg.WriteProfileString(_T("Server"), m_csServer);
      reg.WriteProfileInt(_T("Port"), m_nPort);   
      reg.WriteProfileString(_T("Username"), m_csUsername);
      reg.WriteProfileInt(_T("SavePassword"), m_bSavePassword);   
      if(m_bSavePassword)
      {
         CString csPassword = m_Crypto.Encode(m_csPassword);
         int i = csPassword.GetLength();
         reg.WriteProfileString(_T("Password"), csPassword);
      }
      else
      {
         m_csPassword.Empty();
         reg.WriteProfileString(_T("Password"), m_csPassword);
      }

      reg.WriteProfileString(_T("ChannelDirectory"), m_csChannelDirectory);
      reg.WriteProfileString(_T("MediaDirectory"), m_csMediaDirectory);
      reg.WriteProfileString(_T("MediaFilePath"), m_csMediaFileUrl);

      reg.WriteProfileInt(_T("SSHVersion"), m_iSSHVersion);   
      reg.WriteProfileInt(_T("IPVersion"), m_iIPVersion);   
      reg.WriteProfileInt(_T("Compression"), m_bCompression);   
      reg.WriteProfileInt(_T("PrivateKey"), m_bPrivateKey);   
      
      reg.WriteProfileString(_T("PrivateKeyFile"), m_csPrivateKeyFile);

      UpdateData(FALSE);
      reg.Close();
   }
}

void CSettingsDlg::OnSelchangeService() 
{
   // TODO: Add your control notification handler code here
   UpdateData();
   
   BOOL bActivate = (m_iService != 0);

   m_boxSSHVersion.EnableWindow(bActivate);
   m_boxIPVersion.EnableWindow(bActivate);
   m_btnCompression.EnableWindow(bActivate);
   m_btnPrivateKey.EnableWindow(bActivate);

   m_nPort = (bActivate) ? 22 : 21;
   UpdateData(FALSE);
}

HBRUSH CSettingsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   
   // TODO: Change any attributes of the DC here
   //DrawLogo(_T("Settings"), _T("Set the transfer parameters."));
   pDC->SetBkColor(color);
   hbr = (HBRUSH)brush;
   
   // TODO: Return a different brush if the default is not desired
   return hbr;
}


void CSettingsDlg::OnOK() 
{
	// TODO: Add extra validation here
	//CDialog::OnOK();
   UpdateData();
   UpdateData(FALSE);

   CTransferSettings *dlg = (CTransferSettings *)GetParent();
   dlg->OnOK2();
}  

void CSettingsDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
   UpdateData();
   UpdateData(FALSE);	
}

void CSettingsDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	//CDialog::OnCancel();
   CTransferSettings *dlg = (CTransferSettings *)GetParent();
   dlg->OnCancel2();
}

void CSettingsDlg::OnPrivateKeyFile() 
{
	// TODO: Add your control notification handler code here
   UpdateData();
   
   if(m_bPrivateKey)
   {
      CString csFilter = _T("PKK Files (*.pkk)|*.pkk|All Files (*.*)|*.*||");
      CFileDialog dlgOpen(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, csFilter, this);
      // allways start in current directory
      //dlg.m_ofn.lpstrInitialDir = _T(".");
      if(dlgOpen.DoModal () == IDOK)
      {
         m_csPrivateKeyFile = dlgOpen.GetPathName();
      } 
      else
      {
         m_bPrivateKey = 0;
         m_csPrivateKeyFile.Empty();
      }
   }
   else
   {
      m_csPrivateKeyFile.Empty();
   }
   
   UpdateData(FALSE);	
}


void CSettingsDlg::CheckInputTextForBlanks(CString &csInputText)
{
   CString csTrimLeft = csInputText;
   csTrimLeft.TrimLeft();
   CString csTrimLeftRight = csTrimLeft;
   csTrimLeftRight.TrimRight();

   if (csInputText.Compare(csTrimLeftRight) != 0)
   {
      CString csMessage;
      csMessage.LoadString(IDS_WARNING_WHITESPACE);
      //int nFeedback = AfxMessageBox(csMessage, MB_YESNO, MB_ICONQUESTION); 
      int nFeedback = AfxMessageBox(csMessage, MB_OK, MB_ICONINFORMATION); 
      //if (nFeedback == IDYES)
      {
         csInputText.Empty();
         csInputText.Format(csTrimLeftRight);
      }
   }
}

void CSettingsDlg::OnChangeXmlurl() 
{
   CWnd *pChannel = GetDlgItem(IDE_XMLURL);
   pChannel->GetWindowText(m_csChannel);
   CheckInputTextForBlanks(m_csChannel);
   pChannel->SetWindowText(m_csChannel);
}

void CSettingsDlg::OnChangeMp4url() 
{
   CWnd *pMediaFileUrl = GetDlgItem(IDE_MP4URL);
   pMediaFileUrl->GetWindowText(m_csMediaFileUrl);
   CheckInputTextForBlanks(m_csMediaFileUrl);
   pMediaFileUrl->SetWindowText(m_csMediaFileUrl);
}

void CSettingsDlg::OnChangeHost() 
{
   CWnd *pServer = GetDlgItem(IDE_HOST);
   pServer->GetWindowText(m_csServer);
   CheckInputTextForBlanks(m_csServer);
   pServer->SetWindowText(m_csServer);
}

void CSettingsDlg::OnChangeChanneldir() 
{
   CWnd *pChannelDirectory = GetDlgItem(IDE_CHANNELDIR);
   pChannelDirectory->GetWindowText(m_csChannelDirectory);
   CheckInputTextForBlanks(m_csChannelDirectory);
   pChannelDirectory->SetWindowText(m_csChannelDirectory);
}

void CSettingsDlg::OnChangeMp4dir() 
{
   CWnd *pMediaDirectory = GetDlgItem(IDE_MP4DIR);
   pMediaDirectory->GetWindowText(m_csMediaDirectory);
   CheckInputTextForBlanks(m_csMediaDirectory);
   pMediaDirectory->SetWindowText(m_csMediaDirectory);
}

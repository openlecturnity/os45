// TransferSettings.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "TransferSettings.h"
#include "MfcUtils.h"
#include "Registry.h"

#include <Wininet.h>


// CTransferSettings dialog

IMPLEMENT_DYNAMIC(CTransferSettings, CDialog)

CTransferSettings::CTransferSettings(CWnd* pParent /*=NULL*/)
: CDialog(CTransferSettings::IDD, pParent)
{
   m_isKeyPressed = false;
   m_bTemporaryChannel = false;
}

CTransferSettings::~CTransferSettings()
{
}

void CTransferSettings::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDE_XMLURL, m_edtChannel);
   DDX_Control(pDX, IDE_MP4URL, m_edtMediaFileUrl);
   DDX_Control(pDX, IDE_HOST, m_edtServer);
   DDX_Control(pDX, IDE_USERNAME, m_edtUsername);
   DDX_Control(pDX, IDE_PASSWORD, m_edtPassword);
   DDX_Control(pDX, IDE_CHANNELDIR, m_edtChannelDirectory);
   DDX_Control(pDX, IDE_MP4DIR, m_edtMediaDirectory);
   DDX_Control(pDX, IDC_SERVICE, m_cboService);
   DDX_Control(pDX, IDE_PORT, m_edtPort);
   DDX_Control(pDX, IDL_SAVEPSWD, m_ckbtnSavePassword);

   DDX_Control(pDX, IDC_GROUP_TRANSFER, m_gbWhiteRectangle);

}


BEGIN_MESSAGE_MAP(CTransferSettings, CDialog)
   ON_WM_CTLCOLOR()
   ON_CBN_SELCHANGE(IDC_SERVICE, OnSelchangeService)
   ON_EN_KILLFOCUS(IDE_XMLURL, OnChangeXmlurl)
   ON_EN_KILLFOCUS(IDE_MP4URL, OnChangeMp4url)
   ON_EN_KILLFOCUS(IDE_HOST, OnChangeHost)
   ON_EN_KILLFOCUS(IDE_CHANNELDIR, OnChangeChanneldir)
   ON_EN_KILLFOCUS(IDE_MP4DIR, OnChangeMp4dir)
END_MESSAGE_MAP()


// CTransferSettings message handlers

BOOL CTransferSettings::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // TODO: Add extra initialization here
   SetIcon(AfxGetApp()->LoadIcon(IDI_UPLOADER), TRUE);         // Set big icon

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
      /*		IDL_SSHV,
      IDL_IPV,
      IDL_COMPRESSION,
      IDL_PKK,*/
      IDG_SERVER,
      -1 };
      MfcUtils::Localize(this, dwID);  

      CString csCaption;
      csCaption.LoadString(IDD_SETTINGS_DIALOG);
      this->SetWindowText(csCaption);

      ::SendMessage(::GetDlgItem(m_hWnd, IDE_PASSWORD), EM_SETPASSWORDCHAR, _T('*'), 0);

      m_edtChannel.SetWindowText(_T(""));
      m_edtMediaFileUrl.SetWindowText(_T(""));
      m_edtServer.SetWindowText(_T(""));
      m_edtUsername.SetWindowText(_T(""));
      m_edtPassword.SetWindowText(_T(""));
      m_edtChannelDirectory.SetWindowText(_T(""));
      m_edtMediaDirectory.SetWindowText(_T(""));
      m_cboService.SetCurSel(0);
      m_edtPort.SetWindowText(_T("21"));
      m_ckbtnSavePassword.SetCheck(0);
      /*	m_cboSSHVersion.SetCurSel(1);
      m_cboIPVersion.SetCurSel(0);
      m_ckbtnCompression.SetCheck(0);
      m_ckbtnPrivateKey.SetCheck(0);*/

      if(m_bTemporaryChannel)
      {
         m_cboService.SetCurSel(m_tcTempChannel.iService);
         m_edtChannel.SetWindowText(m_tcTempChannel.csUrl);
         m_edtServer.SetWindowText(m_tcTempChannel.csServer);
         CString csText;
         csText.Format(_T("%d"),m_tcTempChannel.nPort);
         m_edtPort.SetWindowText(csText);
         m_edtUsername.SetWindowText(m_tcTempChannel.csUsername);
         if(m_tcTempChannel.bSavePassword)
            m_ckbtnSavePassword.SetCheck(1);
         else
            m_ckbtnSavePassword.SetCheck(0);

         if(m_ckbtnSavePassword.GetCheck())
         {
            m_edtPassword.SetWindowText(m_tcTempChannel.csPassword);
            //CString csPassword = m_Crypto.Encode(csText);
            //m_tcTempChannel.csPassword = csPassword;
         }
         m_edtChannelDirectory.SetWindowText(m_tcTempChannel.csDirectory);	
      }
      else
         LoadFromRegistry(m_csChannelName);

      return TRUE;  // return TRUE unless you set the focus to a control
      // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CTransferSettings::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) 
{   
   HBRUSH hbr = NULL;

   if (nCtlColor == CTLCOLOR_STATIC )
   {
      pDC->SetBkColor(RGB(255,255, 255));
      hbr = CreateSolidBrush(RGB(255, 255, 255));
   }
   else
      hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);  

   return hbr;
}

void CTransferSettings::LoadFromRegistry(CString pszKey)
{
   // Get a pointer to Application object
   CRegistry reg;  
   if(reg.Open(pszKey) == ERROR_SUCCESS)
   {
      UpdateData();
      //CString csTemp(_T(""));
      CString csText;

      m_cboService.SetCurSel(reg.GetProfileInt(_T("Service"), FTP_SERVICE));
      m_edtChannel.SetWindowText(reg.GetProfileString(_T("Channel"), _T("")));
      m_edtServer.SetWindowText(reg.GetProfileString(_T("Server"), _T("")));
      csText.Format(_T("%d"),INTERNET_DEFAULT_FTP_PORT);
      m_edtPort.SetWindowText(reg.GetProfileString(_T("Port"), csText)); 
      m_edtUsername.SetWindowText(reg.GetProfileString(_T("Username"), _T("")));
      m_ckbtnSavePassword.SetCheck(reg.GetProfileInt(_T("SavePassword"), TRUE));
      if(m_ckbtnSavePassword.GetCheck())
      {
         CString csAux = reg.GetProfileString(_T("Password"), _T(""));
         CString csPassword = csAux.GetString();
         m_edtPassword.SetWindowText(m_Crypto.Decode(csPassword));
      }
      m_edtChannelDirectory.SetWindowText(reg.GetProfileString(_T("ChannelDirectory"), _T("/")));
      m_edtMediaDirectory.SetWindowText(reg.GetProfileString(_T("MediaDirectory"), _T("/")));
      m_edtMediaFileUrl.SetWindowText(reg.GetProfileString(_T("MediaFilePath"), _T("")));

      /*		m_cboSSHVersion.SetCurSel(reg.GetProfileInt(_T("SSHVersion"), -1));
      m_cboIPVersion.SetCurSel(reg.GetProfileInt(_T("IPVersion"), -1));
      m_ckbtnCompression.SetCheck(reg.GetProfileInt(_T("Compression"), 0));
      m_ckbtnPrivateKey.SetCheck(reg.GetProfileInt(_T("PrivateKey"), 0));
      csText = reg.GetProfileString(_T("PrivateKeyFile"), _T(""));
      m_csPrivateKeyFile = csText.GetString();*/

      UpdateData(FALSE);
      reg.Close();
   }

   BOOL bActivate = (m_cboService.GetCurSel() != 0);

   /*	m_cboSSHVersion.EnableWindow(bActivate);
   m_cboIPVersion.EnableWindow(bActivate);
   m_ckbtnCompression.EnableWindow(bActivate);
   m_ckbtnPrivateKey.EnableWindow(bActivate);*/
}

void CTransferSettings::SaveToRegistry(CString pszKey)
{
   CRegistry reg;  
   if(reg.Create(pszKey) == ERROR_SUCCESS)
   {
      // Write the information to the registry.
      UpdateData();

      CString csText;
      reg.WriteProfileInt(_T("Service"), m_cboService.GetCurSel()); 

      m_edtChannel.GetWindowText(csText);
      reg.WriteProfileString(_T("Channel"), csText);

      m_edtServer.GetWindowText(csText);
      reg.WriteProfileString(_T("Server"), csText);

      m_edtPort.GetWindowText(csText);
      reg.WriteProfileString(_T("Port"), csText);

      m_edtUsername.GetWindowText(csText);
      reg.WriteProfileString(_T("Username"), csText);

      reg.WriteProfileInt(_T("SavePassword"), m_ckbtnSavePassword.GetCheck());   
      if(m_ckbtnSavePassword.GetCheck())
      {
         m_edtPassword.GetWindowText(csText);
         CString csPassword = m_Crypto.Encode(csText);
         reg.WriteProfileString(_T("Password"), csPassword);
      }

      m_edtChannelDirectory.GetWindowText(csText);
      reg.WriteProfileString(_T("ChannelDirectory"), csText);

      m_edtMediaDirectory.GetWindowText(csText);
      reg.WriteProfileString(_T("MediaDirectory"), csText);

      m_edtMediaFileUrl.GetWindowText(csText);
      reg.WriteProfileString(_T("MediaFilePath"), csText);

      /*		reg.WriteProfileInt(_T("SSHVersion"),m_cboSSHVersion.GetCurSel()); 
      reg.WriteProfileInt(_T("IPVersion"), m_cboIPVersion.GetCurSel());   
      reg.WriteProfileInt(_T("Compression"), m_ckbtnCompression.GetCheck()); 
      reg.WriteProfileInt(_T("PrivateKey"), m_ckbtnPrivateKey.GetCheck()); 
      reg.WriteProfileString(_T("PrivateKeyFile"), m_csPrivateKeyFile);*/

      UpdateData(FALSE);
      reg.Close();
   }
}

void CTransferSettings::OnSelchangeService() 
{
   // TODO: Add your control notification handler code here
   UpdateData();

   BOOL bActivate = (m_cboService.GetCurSel() != 0);

   /*	m_cboSSHVersion.EnableWindow(bActivate);
   m_cboIPVersion.EnableWindow(bActivate);
   m_ckbtnCompression.EnableWindow(bActivate);
   m_ckbtnPrivateKey.EnableWindow(bActivate);*/

   m_edtPort.SetWindowText((bActivate) ? _T("22") : _T("21"));

   UpdateData(FALSE);

}

void CTransferSettings::OnOK() 
{
   // TODO: Add extra validation here

   UpdateData();
   if(m_bTemporaryChannel)
   {
      m_tcTempChannel.iService = m_cboService.GetCurSel();
      m_edtChannel.GetWindowText(m_tcTempChannel.csUrl);
      m_edtServer.GetWindowText(m_tcTempChannel.csServer);
      CString csText;
      m_edtPort.GetWindowText(csText);
      m_tcTempChannel.nPort = _ttoi(csText);
      m_edtUsername.GetWindowText(m_tcTempChannel.csUsername);
      csText.Empty();
      if(m_ckbtnSavePassword.GetCheck())
      {
         m_edtPassword.GetWindowText(csText);
         m_tcTempChannel.csPassword = csText;
      }
      else
         m_tcTempChannel.csPassword = _T("");
      m_edtChannelDirectory.GetWindowText(m_tcTempChannel.csDirectory);
      if(m_ckbtnSavePassword.GetCheck())
         m_tcTempChannel.bSavePassword = true;
      else
         m_tcTempChannel.bSavePassword = false;
   }
   else
      SaveToRegistry(m_csChannelName);
   CDialog::EndDialog(IDOK);
}  

void CTransferSettings::OnPrivateKeyFile() 
{
   // TODO: Add your control notification handler code here
   UpdateData();

   /*	if(m_ckbtnPrivateKey.GetCheck())
   {
   CString csFilter = _T("PKK Files (*.pkk)|*.pkk|All Files (*.*)|*.*||");
   CFileDialog dlgOpen(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, csFilter, this);

   if(dlgOpen.DoModal () == IDOK)
   {
   m_csPrivateKeyFile = dlgOpen.GetPathName();
   } 
   else
   {
   m_ckbtnPrivateKey.SetCheck(1);
   m_csPrivateKeyFile.Empty();
   }
   }
   else
   {
   m_csPrivateKeyFile.Empty();
   }*/

   UpdateData(FALSE);
}

void CTransferSettings::CheckInputTextForBlanks(CString &csInputText)
{
   CString csText;
   int curPos= 0;
   CString csResToken;

   csText = csInputText;
   csInputText.Empty();
   csResToken = csText.Tokenize(_T(" "),curPos);
   csInputText = csResToken;

   while (csResToken != "")
   {
      csResToken = csText.Tokenize(_T(" "),curPos);
      csInputText += csResToken;
   }

   if(csText.Compare(csInputText) != 0)
   {
      CString csMessage;
      csMessage.LoadString(IDS_WARNING_WHITESPACE);
      int nFeedback = AfxMessageBox(csMessage, MB_OK, MB_ICONINFORMATION); 
      this->SetFocus();
   }
}

void CTransferSettings::OnChangeXmlurl() 
{
   CString csText;
   m_edtChannel.GetWindowText(csText);
   CheckInputTextForBlanks(csText);
   m_edtChannel.SetWindowText(csText);
}

void CTransferSettings::OnChangeMp4url() 
{
   CString csText;
   m_edtMediaFileUrl.GetWindowText(csText);
   CheckInputTextForBlanks(csText);
   m_edtMediaFileUrl.SetWindowText(csText);
}

void CTransferSettings::OnChangeHost() 
{
   CString csText;
   m_edtServer.GetWindowText(csText);
   CheckInputTextForBlanks(csText);
   m_edtServer.SetWindowText(csText);
}

void CTransferSettings::OnChangeChanneldir() 
{
   CString csText;
   m_edtChannelDirectory.GetWindowText(csText);
   CheckInputTextForBlanks(csText);
   m_edtChannelDirectory.SetWindowText(csText);
}

void CTransferSettings::OnChangeMp4dir() 
{
   CString csText;
   m_edtMediaDirectory.GetWindowText(csText);
   CheckInputTextForBlanks(csText);
   m_edtMediaDirectory.SetWindowText(csText);
}

void CTransferSettings::SetChannelName(CString csChannelName)
{
   m_csChannelName = csChannelName;
}

CString CTransferSettings::GetChannelName()
{
   return m_csChannelName;
}

CString CTransferSettings::GetChannelURL()
{
   CRegistry reg;  
   CString csChannelURL;
   if(reg.Open(m_csChannelName) == ERROR_SUCCESS)
   {
      csChannelURL = reg.GetProfileString(_T("Channel"), _T(""));
   }
   return csChannelURL;
}

void CTransferSettings::GetTempChannelSettings(TransferParam &tcTempChannel)
{
   tcTempChannel = m_tcTempChannel;
}

void CTransferSettings::SetTempChannelSettings(TransferParam tcTempChannel)
{
   m_tcTempChannel = tcTempChannel;
}


BOOL CTransferSettings::PreTranslateMessage(MSG* pMsg)
{

   if(m_isKeyPressed)
      this->RedrawWindow();

   if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
   {
      m_isKeyPressed = true;
      SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam); 
   }
   else
      if(pMsg->message == WM_SYSKEYDOWN)
      {
         m_isKeyPressed = true;
         SendMessage(WM_SYSKEYDOWN, pMsg->wParam, pMsg->lParam); 
      }
      else 
         m_isKeyPressed = false;

   return CDialog::PreTranslateMessage(pMsg);
}

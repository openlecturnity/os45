// SlideStarUploadDlg.cpp : implementation file
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <WinInet.h>
#include "SlideStarUpload.h"
#include "SlideStarUploadDlg.h"
#include "SettingsDlg.h"
#include "CmdLineInfo.h"
#include "PasswordDlg.h"
//#include "LrdDialog.h"
#include "MfcUtils.h"
//#include "AddItem.h"
#include "Registry.h"
//#include "AddItem.h"
//#include "XmlValidator.h"
#include "ZipExists.h"
#include "LanguageSupport.h"
#include <shlwapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the session thread
UINT __cdecl SessionThread(LPVOID pParam)
{
	HRESULT hr = S_OK;
   
   hr = CLanguageSupport::SetThreadLanguage();
   if (hr != S_OK)
   {
      CString csTitle;
      csTitle.LoadString(IDS_ERROR);
      CString csMessage;
      if (hr == E_LS_WRONG_REGISTRY)
         csMessage.LoadString(IDS_ERROR_INSTALLATION);
      else if (hr == E_LS_LANGUAGE_NOT_SUPPORTED)
         csMessage.LoadString(IDS_ERROR_LANGUAGE);
      else if (hr == E_LS_TREAD_LOCALE)
         csMessage.LoadString(IDS_ERROR_SET_LANGUAGE);

      MessageBox(NULL, csMessage, csTitle, MB_OK | MB_TOPMOST);
   }


   //CLanguageSupport::SetThreadLanguage();

   CSlideStarUploadDlg *dlg = (CSlideStarUploadDlg *)pParam;
   BOOL bResult = dlg->DoSession();
   ::PostMessage(dlg->m_hWnd, WM_CLOSE, 0, 0);
   return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

// Dialog Data
   //{{AFX_DATA(CAboutDlg)
   enum { IDD = IDD_ABOUTBOX };
   //}}AFX_DATA

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CAboutDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   //{{AFX_MSG(CAboutDlg)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
   //{{AFX_DATA_INIT(CAboutDlg)
   //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDlg)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
   //{{AFX_MSG_MAP(CAboutDlg)
      // No message handlers
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlideStarUploadDlg dialog

CSlideStarUploadDlg::CSlideStarUploadDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CSlideStarUploadDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CSlideStarUploadDlg)
	m_csFrom = _T("");
	m_csTo = _T("");
	//}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

void CSlideStarUploadDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSlideStarUploadDlg)
   DDX_Control(pDX, IDC_BARBOTTOM, m_stcBarBottom);
   DDX_Control(pDX, IDC_PROGRESS, m_prgProgress);
	DDX_Text(pDX, IDL_FROM, m_csFrom);
	DDX_Text(pDX, IDL_TO, m_csTo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSlideStarUploadDlg, CDialog)
   //{{AFX_MSG_MAP(CSlideStarUploadDlg)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlideStarUploadDlg message handlers

BOOL CSlideStarUploadDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   // Add "About..." menu item to system menu.

   // IDM_ABOUTBOX must be in the system command range.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL)
   {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty())
      {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

 
   // TODO: Add extra initialization here
   // find app dir
   ::CoInitialize(NULL);

   // get app
   pApp = (CSlideStarUploadApp *)AfxGetApp();
   SetIcon(pApp->LoadIcon(IDI_SLIDESTARUPLOAD), TRUE);         // Set big icon
   DrawLogo();

   ::GetModuleFileName(NULL, m_tszCurrentDir, MAX_PATH);
   PathRemoveFileSpec(m_tszCurrentDir);

   m_Thread = NULL;

   DWORD dwID[] = { IDCANCEL,
                  -1 };
   MfcUtils::Localize(this, dwID);

/*
   CString csCaption;
   csCaption.LoadString(IDD_SLIDESTARUPLOAD_DIALOG);
   this->SetWindowText(csCaption);
*/
   bDoDelete = FALSE;
   hModule = NULL;

   ::PostMessage(m_hWnd, WM_STARTSESSION, 0, 0);
   //DoSession();
   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSlideStarUploadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else
   {
      CDialog::OnSysCommand(nID, lParam);
   }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSlideStarUploadDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSlideStarUploadDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

CString CSlideStarUploadDlg::GetFileName(CString csFilePath)
{
   CString csFileName;
   csFileName = csFilePath;
   
   int iSize = csFilePath.GetLength();
   for(int i = iSize - 1;i > 0;i--)
   {
      if ((csFilePath[i] == _T('\\')) || (csFilePath[i] == _T('/')))
      {
         csFileName = csFilePath.Right(iSize - i - 1);
         break;
      }
   }
   
   return csFileName;
}

CString CSlideStarUploadDlg::GetHostName()
{
   return m_csServer;
}

BOOL CSlideStarUploadDlg::CompareFiles(CString srcFileName, CString dstFileName)
{
   BOOL bResult = FALSE;
   BYTE btSrc, btDst;
   
   CFile src, dst;

   if(src.Open(srcFileName,  CFile::modeRead | CFile::shareDenyWrite, NULL))
   {
      if(dst.Open(dstFileName,  CFile::modeRead | CFile::shareDenyWrite, NULL))
      {
         DWORD dwSrcLen = src.GetLength();
         DWORD dwDstLen = dst.GetLength();
         if(dwSrcLen == dwDstLen)
         {
            for(unsigned i=0;i < src.GetLength();i++)
            {
               src.Read((void *)&btSrc, 1);
               dst.Read((void *)&btDst, 1);
               bResult = (btSrc == btDst);
               if(!bResult)
               {
                  break;
               }               
            }
         }
         dst.Close();
      }
      src.Close();
   }

   return bResult;
}

BOOL CSlideStarUploadDlg::IsMediaUrlOk(CString &csURL)
{
   BOOL bResult;
   DWORD dwServiceType;
   CString csServer, csObject;
   INTERNET_PORT nPort;
   
   bResult = AfxParseURL(csURL, dwServiceType, csServer, csObject, nPort);
   if(bResult)
   {
      bResult = (dwServiceType == AFX_INET_SERVICE_HTTP);
      if(bResult)
      {
         bResult = !csServer.IsEmpty();
         if(bResult)
         {
            bResult = !csObject.IsEmpty();               
         }
      }
   }

   return bResult;
}

void CSlideStarUploadDlg::OnCancel() 
{
   // TODO: Add extra cleanup here
   if(m_Thread != NULL)
   {
      // stop the thread
      m_Thread->SuspendThread();
      // and kill the thred
      ::TerminateThread(m_Thread->m_hThread, -1);
      // wait for thread to terminate
      ::WaitForSingleObject(m_Thread->m_hThread, INFINITE);
      m_Thread = NULL;

      // if user press cancel delete zip file
      if(m_iService == 0)
      {
         // ftp
         if(bDoDelete)
         {
            ftp.CleanUp();
            // reopen and delete the file
            if(ftp.Open(m_csServer, m_nPort, m_csUsername, m_csPassword))
            {
               CString csZip = m_csRemoteFileName; //GetFileName(csMediaFile);
               ftp.Delete(csZip, m_csMediaDirectory);
               ftp.Close();
            }
         }
      }
      else
      {
         // kill ssh transfer
         if(hModule != NULL)
         {
            ::FreeLibrary(hModule);
            hModule = NULL;
         }
         // ssh
         if(bDoDelete)
         {
            CString csZip = m_csMediaDirectory + m_csRemoteFileName; //GetFileName(csMediaFile);
            SshDelete(csZip, FALSE);
         }
      }
   }
   CDialog::OnCancel();
}

void CSlideStarUploadDlg::DrawLogo()
{
   RECT rct;
   this->GetClientRect(&rct);
   m_stcBarBottom.MoveWindow(0, rct.bottom  - 44, rct.right, 2, TRUE);
}

void CSlideStarUploadDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default 
	CDialog::OnClose();
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// new version
/////////////////////////////////////
CString CSlideStarUploadDlg::GetActiveChannel()
{
   CRegistry reg;
   int iActive = 0;
   HKEY hKey = NULL;
   TCHAR tstrSubKey[MAX_PATH];
   DWORD dwSubKey = MAX_PATH - 1;
   FILETIME ft;

   CString csResult;
   csResult.Empty();
   
   LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
         SLIDESTARUPLOAD_KEY, 0, KEY_ENUMERATE_SUB_KEYS, &hKey);
   
   if(lResult != ERROR_SUCCESS)
   {
      return csResult;
   }

   DWORD i = 0;
   while(TRUE)
   {
      csResult.Empty();
      dwSubKey = MAX_PATH - 1;
      lResult = RegEnumKeyEx(hKey, i++, tstrSubKey,
         &dwSubKey, NULL, NULL, NULL, &ft);
      if(lResult == ERROR_NO_MORE_ITEMS)
      {
         break;
      }
      
      if(lResult != ERROR_SUCCESS)
      {
         break;
      }

      csResult = CString(tstrSubKey, dwSubKey);
      // get active value
      if(reg.Open(csResult) == ERROR_SUCCESS)
      {
         iActive = reg.GetProfileInt(_T("Active"), 0);   
         reg.Close();
      }
      if(iActive != 0)
      {
         // found
         break;
      }
   }

   RegCloseKey(hKey);
   return csResult;
}

DWORD CSlideStarUploadDlg::GetFileLength(CString csFile)
{
   DWORD dwLength = 0;

   CFile file;
   if(file.Open(csFile,  CFile::modeRead | CFile::shareDenyWrite))
   {
      dwLength = file.GetLength();
      file.Close();
   }

   return dwLength;
}

//BOOL CSlideStarUploadDlg::AddItem(LPCTSTR srcFileName, LPCTSTR dstFileName)
//{
//   BOOL bInserted = FALSE;
//   CTime data = CTime::GetCurrentTime();
//   
//   CAddItem item;
//   {
//      CString csFileName = m_csRemoteFileName; //GetFileName(csMediaFile);
//      CString csURL = m_csMediaFileUrl + csFileName + CString(_T("?")) + data.Format(_T("%Y-%m-%dT%H:%M:%S"));
//      //AfxMessageBox(csURL, MB_ICONINFORMATION); 
//      
//      bInserted = IsMediaUrlOk(csURL);
//
//      if(bInserted)
//      {
//         item.csTitle  = csTitle;
//         item.csAuthor = csAuthor,
//         item.csType = _T("archive/zip");
//         item.csLength.Format(_T("%lu"), GetFileLength(csMediaFile));
//         item.csUrl = csURL;
//         item.csDate = data.Format(_T("%a, %d %b %Y %H:%M:%S"));
//         bInserted = item.Insert(srcFileName, dstFileName);
//      }      
//      else
//      {
//         CString csFormat, csMessage;
//         csFormat.LoadString(IDS_INVPATH);
//         csMessage.Format(csFormat, m_csMediaFileUrl);
//         AfxMessageBox(csMessage, MB_ICONINFORMATION); 
//      }
//   }
//     
//   return bInserted;
//}

bool CSlideStarUploadDlg::ExistSettings()
{
	CRegistry reg;
	CString csTemp;
	if (reg.Open("") == ERROR_SUCCESS)
		csTemp = reg.GetProfileString(_T("Server"),_T(""));
	if (!csTemp.IsEmpty())
		return true;
	return false;
}
void CSlideStarUploadDlg::LoadFromRegistry(CString pszKey)
{
   // Get a pointer to Application object
   CRegistry reg;  
   if(reg.Open(pszKey) == ERROR_SUCCESS)
   {
      m_iService      = reg.GetProfileInt(_T("Service"), FTP_SERVICE);   
      //m_csChannel     = reg.GetProfileString(_T("Channel"), _T(""));
      m_csServer      = reg.GetProfileString(_T("Server"), _T(""));
      m_nPort         = reg.GetProfileInt(_T("Port"), INTERNET_DEFAULT_FTP_PORT);   
      m_csUsername    = reg.GetProfileString(_T("Username"), _T(""));
      m_bSavePassword = reg.GetProfileInt(_T("SavePassword"), TRUE);   
      if(m_bSavePassword)
      {
         CCrypto crypt;
         CString csPassword = reg.GetProfileString(_T("Password"), _T(""));
         m_csPassword = crypt.Decode(csPassword);
      }
      //m_csChannelDirectory = reg.GetProfileString(_T("ChannelDirectory"), _T("/"));
      m_csMediaDirectory   = reg.GetProfileString(_T("MediaDirectory"), _T("/"));

      if(!m_csMediaDirectory.IsEmpty())
      {
         // TODO: How can it happen that '0' can at the end of the string?
         if(m_csMediaDirectory[m_csMediaDirectory.GetLength() - 1] == 0)
            m_csMediaDirectory = m_csMediaDirectory.Left(m_csMediaDirectory.GetLength() - 1);
         if(m_csMediaDirectory[m_csMediaDirectory.GetLength() - 1] != _T('/'))
         {
            m_csMediaDirectory += _T('/');
         }
      }

      //if(!m_csChannelDirectory.IsEmpty())
      //{
      //   // TODO: How can it happen that '0' can at the end of the string?
      //   if(m_csChannelDirectory[m_csChannelDirectory.GetLength() - 1] == 0)
      //      m_csChannelDirectory = m_csChannelDirectory.Left(m_csChannelDirectory.GetLength() - 1);
      //   if(m_csChannelDirectory[m_csChannelDirectory.GetLength() - 1] != _T('/'))
      //   {
      //      m_csChannelDirectory += _T('/');
      //   }
      //}

      m_csMediaFileUrl     = reg.GetProfileString(_T("MediaFilePath"), _T("/"));
      m_csPrivateKeyFile   = reg.GetProfileString(_T("PrivateKeyFile"), _T("/"));

      m_iSSHVersion  = reg.GetProfileInt(_T("SSHVersion"), -1);
      m_iIPVersion   = reg.GetProfileInt(_T("IPVersion"), -1);
      m_bCompression = reg.GetProfileInt(_T("Compression"), 0);
      m_bPrivateKey  = reg.GetProfileInt(_T("PrivateKey"), 0);

      reg.Close();

      if(m_csMediaFileUrl.IsEmpty())
      {
         m_csMediaFileUrl += _T("/");
      }
      else
      {
         // TODO: How can it happen that '0' can at the end of the string?
         if(m_csMediaFileUrl[m_csMediaFileUrl.GetLength() - 1] == 0)
            m_csMediaFileUrl = m_csMediaFileUrl.Left(m_csMediaFileUrl.GetLength() - 1);
         if(m_csMediaFileUrl[m_csMediaFileUrl.GetLength() - 1] != _T('/'))
         {
            m_csMediaFileUrl += _T("/");
         }
      }
   }
}

BOOL CSlideStarUploadDlg::AskForPassword()
{
   BOOL bResult = FALSE;
   CPasswordDlg dlgPsw;
   dlgPsw.m_csUsername = m_csUsername;
   if(dlgPsw.DoModal() == IDOK)
   {
      m_csPassword = dlgPsw.m_csPassword;
      m_csUsername = dlgPsw.m_csUsername;
      bResult = TRUE;
   }
   return bResult;
}

BOOL CSlideStarUploadDlg::DoSession()
{
   BOOL bResult = FALSE;
   
   // 2. load params from registry
   LoadFromRegistry();

   m_csRemoteFileName = GetFileName(csMediaFile);
   
   //CZipExists zip;
   //while(TRUE)
   //{
   //   zip.m_csFileName = m_csRemoteFileName;
   //   //zip.m_csUrl = m_csMediaFileUrl;
	  //zip.m_csUrl.Format(_T("%s/%s"),  m_csServer, m_csMediaDirectory);
   //   int iResult = zip.RemoteFileExists();
   //   
   //   if(iResult == 0)
   //   {
   //      // does not exist
   //      break;
   //   }
   //   
   //   if(iResult == -1)
   //   {
   //      // network error
   //      //::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
   //      //return FALSE;
		 // break;
   //   }
   //   
   //   if(iResult == 1)
   //   {
   //      // exists
   //      zip.DoModal();
   //      
   //      if(zip.iAction == 0)
   //      {
   //         // exit
   //         ::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
   //         return FALSE;                 
   //      }
   //      
   //      if(zip.iAction == 2)
   //      {
   //         // overwrite
   //         break;
   //      }

   //      // modified
   //      m_csRemoteFileName = zip.m_csFileName;
   //      break;
   //   }
   //} 

   CString csFrom, csTo;
   csFrom.Format(IDS_CONNECTING_SERVER);
   csTo.Format(_T(""));
   SetInfoText(csFrom, csTo);

   // 4. ask for password if necessary
   if(!m_bSavePassword)
   {
      if(!AskForPassword())
      {
         ::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
         return FALSE;
      }
   }

   switch(m_iService)
   {
      case FTP_SERVICE:
         bResult = DoFtpSession();
         ::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
         break;
      default:
         bResult = DoSshSession();
         ::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
         break;
   }
     
   return bResult;
}

BOOL CSlideStarUploadDlg::DoFtpSession()
{
   BOOL bResult = FALSE;
   
   bResult = ftp.Open(m_csServer, m_nPort, m_csUsername, m_csPassword);
   if(!bResult)
   {
      return bResult;
   }
   
   CString csFrom, csTo, csFormat;
   // upload media file
   while(TRUE)
   {
      //CString csFrom, csTo, csFormat;
      csFormat.LoadString(IDL_FROM);
      csFrom.Format(csFormat, csMediaFile);
      csFormat.LoadString(IDL_TO);
	  csTo.Format(csFormat, m_csMediaDirectory + m_csRemoteFileName, GetHostName());
      SetInfoText(csFrom, csTo);

      CString csZip = m_csRemoteFileName; //GetFileName(csMediaFile);

      bDoDelete = TRUE;
      bResult = ftp.Upload(csMediaFile, csZip, m_csMediaDirectory, m_prgProgress);
      bDoDelete = FALSE;

      if(!bResult)
      {
         break;
      }

      // upload channel file
//      csFormat.LoadString(IDS_UPDATE_CHANNEL);
//      csFrom.Format(csFormat);
//      csTo.Format(_T(""));
//      SetInfoText(csFrom, csTo);
//
//      CString csXml = GetFileName(m_csChannel);
//      CString csFileUpload = pApp->csTmpDirectory + _T("\\tmp.xml");
//      bResult = ftp.Upload(csFileUpload, csXml, m_csChannelDirectory);
//      if(!bResult)
//      {
//         break;
//      }
//       
//      CString csFileDownload = pApp->csTmpDirectory + _T("\\tmp2.xml");
////      bResult = ftp.Download(csXml, csFileDownload, m_csChannelDirectory);
//      CXmlValidator valid;
//      bResult = valid.Validate(m_csChannel, csFileDownload);
//      if(!bResult)
//      {
//         break;
//      }
//
//      bResult = CompareFiles(csFileUpload, csFileDownload);
//      CString csMessage;
//      csMessage.LoadString((bResult) ? IDS_SUCCESS : IDS_NOTEQUALS);
//      AfxMessageBox(csMessage, MB_ICONINFORMATION); 
//
      break;
   }
   ftp.Close();

   CString csTemp;
   CString csMessage;
   if (!bResult)
   {
	   csTemp.LoadString(IDS_ERR_FTP_CREATE_FILE);
	   csMessage.Format(csTemp,csTo);
   }
   else
	   csMessage.LoadString(IDS_SUCCESS);
//   csMessage.LoadString((bResult) ? IDS_SUCCESS : IDS_ERR_FTP_CREATE_FILE);

   AfxMessageBox(csMessage, MB_ICONINFORMATION); 

   // to quit the application
   // return
   return bResult;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define WM_SSH_BASE    WM_USER+777
#define WM_SSH_STATUS  WM_SSH_BASE+1
#define WM_SSH_REQEXIT WM_SSH_BASE+2
#define WM_SSH_ERR     WM_SSH_BASE+3
#define WM_SSH_OK      WM_SSH_BASE+4

typedef int (__stdcall *SSHPROC)(int, char**, HWND, int); 

int CSlideStarUploadDlg::CallSsh(int argc, _TCHAR** argv, HWND handle, int iDelete = 0)
{
   int iResult = 1;
   if(hModule == NULL)
   {
      hModule = ::LoadLibrary(_T("sshdll.dll"));
   }
   if(hModule != NULL)
   {
      SSHPROC SshProc = (SSHPROC)::GetProcAddress((HINSTANCE)hModule, (LPCSTR)1);
      if(SshProc != NULL)
      {
#ifdef _UNICODE
         // argv: Convert wchar_t** to char**
         char** szargv = new char*[argc]; 
         for (int i = 0; i < argc; ++i)
         {
            //int nLen = wcslen(argv[i]);
            int nLen = ::WideCharToMultiByte(CP_ACP, 0, argv[i], -1, NULL, 0, NULL, NULL);
//            int nLen = ::WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL);
            szargv[i] = new char[nLen + 1];
            ::WideCharToMultiByte(CP_ACP, 0, argv[i], -1, szargv[i], nLen + 1, NULL, NULL);
//            ::WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, szargv[i], nLen + 1, NULL, NULL);
         }

         iResult = (SshProc)(argc, szargv, handle, iDelete);

         delete[] szargv;
         szargv = NULL;
#else
         // argv is already char**
         iResult = (SshProc)(argc, argv, handle, iDelete);
#endif //_UNICODE
      }
      ::FreeLibrary(hModule);
      hModule = NULL;
   }
   else
   {
      CString csMessage;
      csMessage.LoadString(IDM_SSHDLL);
      AfxMessageBox(csMessage, MB_ICONSTOP);
   }
   return iResult;
}

LRESULT CSlideStarUploadDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
   // TODO: Add your specialized code here and/or call the base class
   switch(message)
   {
      case WM_STARTSESSION:
         // create a separate thread
         m_Thread = AfxBeginThread(SessionThread,
         this,
         THREAD_PRIORITY_NORMAL,
         0,
         CREATE_SUSPENDED);   
         m_Thread->m_bAutoDelete = TRUE;
         m_Thread->ResumeThread();
         break;
      case WM_SSH_STATUS:
         UpdateData();
         m_prgProgress.SetRange32(0, wParam);
         m_prgProgress.SetPos(lParam);
         UpdateData(FALSE);

//         CString csFrom, csTo, csFormat;
//         csFormat.LoadString(IDL_FROM);
//         csFrom.Format(csFormat, csMediaFile);
//         csFormat.LoadString(IDL_TO);
//         csTo.Format(csFormat, m_csChannelDirectory + GetFileName(m_csChannel), GetHostName());
         break;
   }

   return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CSlideStarUploadDlg::SshUpload(CString csSrcFileName, CString csDstFileName, BOOL bQuit = FALSE)
{
   BOOL bResult = FALSE;
   iArgc = 0;
   
   //LoadFromRegistry(csActiveChannel);

   csArgc[iArgc++] = _T("SlideStarUpload.exe"); // application name
   csArgc[iArgc++] = (m_iService == 1) ? _T("-scp") : _T("-sftp");
   csArgc[iArgc++] = (m_iSSHVersion == 0) ? _T("-1") : _T("-2"); //ssh version
   csArgc[iArgc++] = (m_iIPVersion == 0) ? _T("-4") : _T("-6"); //ip version

   if(bQuit)
   {
      csArgc[iArgc++] = _T("-q"); // quit
   }

   if(m_bCompression)
   {
      csArgc[iArgc++] = _T("-C"); // compression
   }
   
   if(m_bPrivateKey)
   {
      csArgc[iArgc++] = _T("-i"); // username
      csArgc[iArgc++] = m_csPrivateKeyFile;
   }

   csArgc[iArgc++] = _T("-l"); // username
   csArgc[iArgc++] = m_csUsername; // 
   csArgc[iArgc++] = _T("-pw"); // password
   csArgc[iArgc++] = m_csPassword;  
   csArgc[iArgc++] = _T("-P"); // port
   csArgc[iArgc++].Format(_T("%d"), m_nPort);
   //source
   csArgc[iArgc++] = csSrcFileName;
   // destination
   csArgc[iArgc++].Format(_T("%s:%s"), m_csServer, csDstFileName);

   for(int i=0;i < iArgc;i++)
   {
      cArgc[i] = csArgc[i];
   }

   bResult = CallSsh(iArgc, (_TCHAR **)cArgc, m_hWnd, 0);

   return (bResult == 0); 
}

BOOL CSlideStarUploadDlg::SshDownload(CString csSrcFileName, CString csDstFileName, BOOL bQuit = FALSE)
{
   BOOL bResult = FALSE;
   iArgc = 0;
   
   //LoadFromRegistry(csActiveChannel);

   csArgc[iArgc++] = _T("SlideStarUpload.exe"); // application name
   csArgc[iArgc++] = (m_iService == 1) ? _T("-scp") : _T("-sftp");
   csArgc[iArgc++] = (m_iSSHVersion == 0) ? _T("-1") : _T("-2"); //ssh version
   csArgc[iArgc++] = (m_iIPVersion == 0) ? _T("-4") : _T("-6"); //ip version

   if(bQuit)
   {
      csArgc[iArgc++] = _T("-q"); // quit
   }

   if(m_bCompression)
   {
      csArgc[iArgc++] = _T("-C"); // compression
   }
   
   if(m_bPrivateKey)
   {
      csArgc[iArgc++] = _T("-i"); // username
      csArgc[iArgc++] = m_csPrivateKeyFile;
   }

   csArgc[iArgc++] = _T("-l"); // username
   csArgc[iArgc++] = m_csUsername; // 
   csArgc[iArgc++] = _T("-pw"); // password
   csArgc[iArgc++] = m_csPassword;  
   csArgc[iArgc++] = _T("-P"); // port
   csArgc[iArgc++].Format(_T("%d"), m_nPort);
   //source
   csArgc[iArgc++].Format(_T("%s:%s"), m_csServer, csSrcFileName);
   // destination
   csArgc[iArgc++] = csDstFileName;

   for(int i=0;i < iArgc;i++)
   {
      cArgc[i] = csArgc[i];
   }
   
   bResult = CallSsh(iArgc, (_TCHAR **)cArgc, m_hWnd, 0);

   return (bResult == 0); 
}

BOOL CSlideStarUploadDlg::SshDelete(CString csSrcFileName, BOOL bQuit)
{
   BOOL bResult = FALSE;

   for(int k=0;k < csSrcFileName.GetLength();k++)
   {
      // wildcards not allowed
      if( (csSrcFileName[k] == _T('*')) || (csSrcFileName[k] == _T('?')) )
      {
         return FALSE;
      }
   }


   iArgc = 0;
   csArgc[iArgc++] = _T("SlideStarUpload.exe"); // application name
   csArgc[iArgc++] = _T("-scp");
   csArgc[iArgc++] = (m_iSSHVersion == 0) ? _T("-1") : _T("-2"); //ssh version
   csArgc[iArgc++] = (m_iIPVersion == 0) ? _T("-4") : _T("-6"); //ip version

   if(bQuit)
   {
      csArgc[iArgc++] = _T("-q"); // quit
   }

   if(m_bCompression)
   {
      csArgc[iArgc++] = _T("-C"); // compression
   }
   
   if(m_bPrivateKey)
   {
      csArgc[iArgc++] = _T("-i"); // username
      csArgc[iArgc++] = m_csPrivateKeyFile;
   }

   csArgc[iArgc++] = _T("-l"); // username
   csArgc[iArgc++] = m_csUsername; // 
   csArgc[iArgc++] = _T("-pw"); // password
   csArgc[iArgc++] = m_csPassword;  
   csArgc[iArgc++] = _T("-P"); // port
   csArgc[iArgc++].Format(_T("%d"), m_nPort);
   //source
   csArgc[iArgc++].Format(_T("%s:%s"), m_csServer, csSrcFileName);
   // destination
   csArgc[iArgc++] = csSrcFileName;

   for(int i=0;i < iArgc;i++)
   {
      cArgc[i] = csArgc[i];
   }

   bResult = CallSsh(iArgc, (_TCHAR **)cArgc, m_hWnd, 1);
   
   return (bResult == 0); 
}

BOOL CSlideStarUploadDlg::DoSshSession()
{
   BOOL bResult = FALSE;

   CString csFrom, csTo, csFormat;
   csFormat.LoadString(IDL_FROM);
   csFrom.Format(csFormat, csMediaFile);
   csFormat.LoadString(IDL_TO);
   //csTo.Format(csFormat, m_csChannelDirectory + GetFileName(m_csChannel), GetHostName());
   csTo.Format(csFormat, m_csMediaDirectory + m_csRemoteFileName, GetHostName());
   SetInfoText(csFrom, csTo);
   
   CString csZip = m_csMediaDirectory + m_csRemoteFileName; //GetFileName(csMediaFile);
   
   bDoDelete = TRUE;
   bResult = SshUpload(csMediaFile, csZip, FALSE);
   bDoDelete = FALSE;
   
   if(bResult == FALSE)
   {
      return FALSE;
   }

   int nLower = 0;
   int nUpper = 0;
   m_prgProgress.GetRange(nLower, nUpper);
   m_prgProgress.SetPos((int)(0.85f*nUpper));

   // upload channel file
//   csFormat.LoadString(IDS_UPDATE_CHANNEL);
//   csFrom.Format(csFormat);
//   csTo.Format(_T(""));
//   SetInfoText(csFrom, csTo);
//
//   CString csXml = m_csChannelDirectory + GetFileName(m_csChannel);
//   //AfxMessageBox(csXml, MB_ICONINFORMATION); 
//
//   CString csFileUpload = pApp->csTmpDirectory + _T("\\tmp.xml");
//   bResult = SshUpload(csFileUpload, csXml, TRUE);
//   if(bResult == FALSE)
//   {
//      return FALSE;
//   }
//  
//   CString csFileDownload = pApp->csTmpDirectory + _T("\\tmp3.xml");
////   bResult = SshDownload(csXml, csFileDownload, TRUE);
//   CXmlValidator valid;
//   bResult = valid.Validate(m_csChannel, csFileDownload);
//   if(bResult == FALSE)
//   {
//      return FALSE;
//   }
//
//   bResult = CompareFiles(csFileUpload, csFileDownload);
//
   m_prgProgress.GetRange(nLower, nUpper);
   m_prgProgress.SetPos(nUpper);

   CString csMessage;
   csMessage.LoadString((bResult) ? IDS_SUCCESS : IDS_NOTEQUALS);
   AfxMessageBox(csMessage, MB_ICONINFORMATION); 

   return bResult;
}

void CSlideStarUploadDlg::SetInfoText(CString csFrom, CString csTo)
{
   CWnd *pFromText = GetDlgItem(IDL_FROM);
   CWnd *pToText = GetDlgItem(IDL_TO);
   pFromText->SetWindowText(csFrom);
   pToText->SetWindowText(csTo);
   RedrawWindow();
}

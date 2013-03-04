// Mp4Exists.cpp : implementation file
//

#include "stdafx.h"
#include <wininet.h>
#include "podcaster.h"
#include "Mp4Exists.h"
#include "NewFilename.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMp4Exists dialog


CMp4Exists::CMp4Exists(CWnd* pParent /*=NULL*/)
	: CDialog(CMp4Exists::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMp4Exists)
	m_Mp4FileName = _T("");
	//}}AFX_DATA_INIT
}


void CMp4Exists::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMp4Exists)
	DDX_Text(pDX, IDL_L2, m_Mp4FileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMp4Exists, CDialog)
	//{{AFX_MSG_MAP(CMp4Exists)
	ON_BN_CLICKED(IDOVER, OnOver)
	ON_BN_CLICKED(IDMODY, OnMody)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CMp4Exists::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_PODCASTER);
   SetIcon(m_hIcon, TRUE);

   iAction = 0;
   //m_csUrl = "http://132.230.139.59/tmp/"; //a.mp4

   DWORD dwID[] = {
         IDL_L1,
         IDL_L3,
         IDL_L4,
         IDL_L5,
         IDL_L6,
         IDL_L7,
         IDCANCEL,
         IDOVER,
         IDMODY,
         -1 };
   MfcUtils::Localize(this, dwID);

   CString csCaption;
   csCaption.LoadString(IDD_MP4EXISTS);
   this->SetWindowText(csCaption);

   UpdateData();
   m_Mp4FileName = m_csFileName;
   UpdateData(FALSE);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// CMp4Exists message handlers
void CMp4Exists::OnOK() 
{
	// TODO: Add extra validation here
}

// modify
void CMp4Exists::OnMody() 
{
	// TODO: Add your control notification handler code here
	CNewFilename mp4;  
//   mp4.m_csFileName = m_csFileName;
   if(mp4.DoModal() == IDOK)
   {  
      m_csFileName = mp4.m_csFileName + _T(".mp4");
      iAction = 1;
      CDialog::OnOK();
   }	
}

// overwrite
void CMp4Exists::OnOver() 
{
	// TODO: Add your control notification handler code here
	iAction = 2;
   CDialog::OnCancel();
}

// cancel
void CMp4Exists::OnCancel() 
{
	// TODO: Add extra cleanup here
	iAction = 0;
	CDialog::OnCancel();
}

int CMp4Exists::RemoteFileExists()
{
	int iResult = -1;   // network error
	DWORD dwFilePos = 0;
	HINTERNET hInternet = NULL, hDownload = NULL; 

   CString csUrl = m_csUrl + m_csFileName;
   //AfxMessageBox(csUrl, MB_ICONINFORMATION); 

	hInternet = InternetOpen(_T("Internet"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if(hInternet != NULL)
	{
		hDownload = InternetOpenUrl(hInternet, csUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if(hDownload != NULL)
		{
         dwFilePos = InternetSetFilePointer(hDownload, 0, NULL, FILE_BEGIN, NULL);
         iResult = (dwFilePos != (DWORD)-1) ? 1 : 0;
			InternetCloseHandle(hDownload);
		}
      else
      {
         GetInetError();
      }
		InternetCloseHandle(hInternet);
	}
   else
   {
      GetInetError();
   }
	return iResult;
}

void CMp4Exists::GetInetError()
{
   LPVOID lpMsgBuf;
   CString csErrorMessage, csFormat;
   DWORD dwErrorNo = ::GetLastError();
   
   if(dwErrorNo == 0)
   {
      return;
   }

   if(::FormatMessage( 
       FORMAT_MESSAGE_ALLOCATE_BUFFER | 
       FORMAT_MESSAGE_FROM_SYSTEM | 
       FORMAT_MESSAGE_IGNORE_INSERTS,
       NULL,
       dwErrorNo,
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
       (LPTSTR) &lpMsgBuf,
       0,
       NULL ) == 0)
   {
      // Handle the error.
      csFormat.LoadString(IDS_FTPERROR);
      csErrorMessage.Format(csFormat, dwErrorNo);
      ::MessageBox(NULL, csErrorMessage, _T("Podcaster"), MB_OK|MB_ICONSTOP);
      return;
   }

   // Display the string.
   ::MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Podcaster"), MB_OK|MB_ICONSTOP);
   // Free the buffer.
   ::LocalFree(lpMsgBuf);
}


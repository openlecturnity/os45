// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SlideStarUpload.h"
#include "PasswordDlg.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CPasswordDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPasswordDlg)
   m_csUsername = _T("");
   m_csPassword = _T("");
   //}}AFX_DATA_INIT
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPasswordDlg)
   DDX_Text(pDX, IDE_USERNAME, m_csUsername);
   DDX_Text(pDX, IDE_PASSWORD, m_csPassword);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
   //{{AFX_MSG_MAP(CPasswordDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

BOOL CPasswordDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   SetIcon(AfxGetApp()->LoadIcon(IDI_SLIDESTARUPLOAD), TRUE);         // Set big icon

   DWORD dwID[] = {IDCANCEL,
                   IDL_USERNAME,
                   IDL_PASSWORD,
                   -1 };
   MfcUtils::Localize(this, dwID);

   CString csCaption;
   csCaption.LoadString(IDD_PASSWORD_DIALOG);
   this->SetWindowText(csCaption);

   ::SendMessage(::GetDlgItem(m_hWnd, IDE_PASSWORD), 
      EM_SETPASSWORDCHAR, _T('*'), 0);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswordDlg::OnOK() 
{
   // TODO: Add extra validation here
   
   // get values
   UpdateData();
   UpdateData(FALSE);

   CDialog::OnOK();
}


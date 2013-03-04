// LrdDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Podcaster.h"
#include "LrdDialog.h"
#include "LrdDialog2.h"
#include "SettingsDlg.h"
#include "MfcUtils.h"
#include "ParseLrd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLrdDialog dialog


CLrdDialog::CLrdDialog(CWnd* pParent /*=NULL*/)
   : CDialog(CLrdDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CLrdDialog)
   m_csLRDocument = _T("");
   m_csTitle = _T("");
   m_csAuthor = _T("");
   m_iOption = -1;
   //}}AFX_DATA_INIT
   m_hIcon = AfxGetApp()->LoadIcon(IDI_PODCASTER);
}


void CLrdDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CLrdDialog)
   DDX_Control(pDX, IDE_AUTHOR, m_edtAuthor2);
   DDX_Control(pDX, IDE_TITLE, m_edtTitle2);
   DDX_Control(pDX, IDE_LRDFILE, m_edtLRDocument2);
   DDX_Control(pDX, IDB_BROWSE, m_btnBrowseButton);
   DDX_Text(pDX, IDE_LRDFILE, m_csLRDocument);
   DDX_Text(pDX, IDE_TITLE, m_csTitle);
   DDX_Text(pDX, IDE_AUTHOR, m_csAuthor);
   DDX_Radio(pDX, IDC_OPTION1, m_iOption);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLrdDialog, CDialog)
   //{{AFX_MSG_MAP(CLrdDialog)
	ON_WM_CTLCOLOR()
   ON_WM_PAINT()
   ON_BN_CLICKED(IDC_OPTION1, OnOption)
   ON_BN_CLICKED(IDC_OPTION2, OnOption)
	ON_BN_CLICKED(IDB_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLrdDialog message handlers
BOOL CLrdDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   ::CoInitialize(NULL);
   
   color = RGB(255, 255, 255);
   brush.CreateSolidBrush(color);
   
   // TODO: Add extra initialization here
   m_edtLRDocument2.SetBackColor(RGB(255, 255, 255));
   m_edtAuthor2.SetBackColor(RGB(255, 255, 255));
   m_edtTitle2.SetBackColor(RGB(255, 255, 255));

   CPodcasterApp *pApp = (CPodcasterApp *)AfxGetApp();
   CString csExtension = pApp->m_cmdInfo.m_csUnknown.Right(4 * sizeof(TCHAR));
   SetIcon(m_hIcon, TRUE);

   UpdateData();
   m_iOption = 0;
   BOOL bIsLrd = (csExtension.CompareNoCase(_T(".lrd")) == 0);
   m_csLRDocument = (bIsLrd) ? pApp->m_cmdInfo.m_csUnknown : m_csLRDocument = FindFirstLrdFile();
   ParseLrd();
   UpdateData(FALSE);

   if(bIsLrd)
   {
      this->OnOK();
   }
   
   DWORD dwID[] = {
         IDL_LRDDOC,
         IDL_METADATA,
         IDL_TITLE,
         IDL_AUTHOR,
         IDB_BROWSE,
         -1 };
   MfcUtils::Localize(this, dwID);

   //CString csCaption;
   //csCaption.LoadString(IDD_LRD_DIALOG);
   //this->SetWindowText(csCaption);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CLrdDialog::OnBrowse() 
{
   // TODO: Add your control notification handler code here
   CString csFilter = _T("LRD Files (*.lrd)|*.lrd||");
   CFileDialog dlgOpen(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, csFilter, this);
   // allways start in current directory
   //dlg.m_ofn.lpstrInitialDir = _T(".");
   if( dlgOpen.DoModal () == IDOK )
   {
      UpdateData();
      m_csLRDocument = dlgOpen.GetPathName();
      ParseLrd();
      UpdateData(FALSE);
   } 
}

void CLrdDialog::OnOK() 
{
   // TODO: Add extra validation here
   
   // Read the parameters
   UpdateData();
   UpdateData(FALSE);

   CLrdDialog2 *dlg = (CLrdDialog2 *)GetParent();
   dlg->OnOK2();  
   //CDialog::OnOK();
}

void CLrdDialog::OnCancel() 
{
   // TODO: Add extra cleanup here

   UpdateData();
   m_iOption = -1;
   m_csLRDocument.Empty();
   m_csAuthor.Empty();
   m_csTitle.Empty();
   UpdateData(FALSE);
   CLrdDialog2 *dlg = (CLrdDialog2 *)GetParent();
   dlg->OnCancel2();  
   
   //CDialog::OnCancel();
}

void CLrdDialog::OnOption() 
{
   // TODO: Add your control notification handler code here
   UpdateData();
   BOOL Enabled = (m_iOption == 0);

   m_btnBrowseButton.EnableWindow(Enabled);
   m_edtLRDocument2.EnableWindow(Enabled);

   m_edtTitle2.EnableWindow(!Enabled);
   m_edtAuthor2.EnableWindow(!Enabled);

   UpdateData(FALSE);
   
}

void CLrdDialog::ParseLrd()
{
   CParseLrd lrd;
   lrd.Parse(m_csLRDocument);
   m_csAuthor = CString(lrd.GetAuthor());
   m_csTitle  = CString(lrd.GetTitle());
}

LPTSTR CLrdDialog::FindFirstLrdFile()
{
   m_tszLrdFile[0] = 0;

   WIN32_FIND_DATA w32fd;
   ZeroMemory(&w32fd, sizeof(w32fd));
   w32fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
   
   HANDLE hFind = ::FindFirstFile(_T("*.lrd"), &w32fd);
   if (hFind != INVALID_HANDLE_VALUE)
   {
      ::FindClose(hFind);
      ::GetCurrentDirectory(MAX_PATH, m_tszCurrentDirectory);
      _stprintf(m_tszLrdFile, _T("%s\\%s"), m_tszCurrentDirectory, w32fd.cFileName);
   }
   return m_tszLrdFile;
}

HBRUSH CLrdDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
   pDC->SetBkColor(color);
   hbr = (HBRUSH)brush;

	// TODO: Return a different brush if the default is not desired
	return hbr;
}


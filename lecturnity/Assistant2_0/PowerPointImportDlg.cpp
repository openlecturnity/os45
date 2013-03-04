// PowerPointImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "assistant.h"
#include "PowerPointImportDlg.h"
#include "MfcUtils.h" // MfcUtils::Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPowerPointImportDlg dialog



CPowerPointImportDlg::CPowerPointImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPowerPointImportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPowerPointImportDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}



void CPowerPointImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPowerPointImportDlg)
	DDX_Control(pDX, IDC_RADIO_NEWPROJECT_POWERPOINT_IMPORT_DLG, m_RadioNewProject);
	DDX_Control(pDX, IDC_RADIO_EXISTINGPROJECT_POWERPOINT_IMPORT_DLG, m_RadioExistingProject);
	DDX_Control(pDX, IDC_LIST_POWERPOINT_IMPORT_DLG_EXISTING_PROJECTS, m_ExistingProjectsList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPowerPointImportDlg, CDialog)
	//{{AFX_MSG_MAP(CPowerPointImportDlg)
	ON_BN_CLICKED(IDC_RADIO_EXISTINGPROJECT_POWERPOINT_IMPORT_DLG, OnRadioPowerpointImportDlgExistingproject)
	ON_BN_CLICKED(IDC_RADIO_NEWPROJECT_POWERPOINT_IMPORT_DLG, OnRadioPowerpointImportDlgNewproject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPowerPointImportDlg message handlers
BOOL CPowerPointImportDlg::OnInitDialog() 
{
CDialog::OnInitDialog();

   CString csCap;
   csCap.LoadString(IDD_POWERPOINT_IMPORT);
   SetWindowText(csCap);

   DWORD dwIds[] = {IDC_STATIC_POWERPOINT_IMPORT_DLG,
                    IDC_RADIO_NEWPROJECT_POWERPOINT_IMPORT_DLG,
                    IDC_RADIO_EXISTINGPROJECT_POWERPOINT_IMPORT_DLG,
                    IDOK,
                    IDCANCEL,
                    -1};
   MfcUtils::Localize(this, dwIds);
	
   m_RadioNewProject.SetCheck(true);
   m_ExistingProjectsList.SetExtendedStyle(LVS_EX_GRIDLINES |LVS_EX_FULLROWSELECT);

   return true;
}

void CPowerPointImportDlg::OnRadioPowerpointImportDlgExistingproject() 
{
	// TODO: Add your control notification handler code here
m_RadioNewProject.SetCheck(false);	
m_RadioExistingProject.SetCheck(true)	;
}

void CPowerPointImportDlg::OnRadioPowerpointImportDlgNewproject() 
{
	// TODO: Add your control notification handler code here
m_RadioExistingProject.SetCheck(false)	;
m_RadioNewProject.SetCheck(true);	
}

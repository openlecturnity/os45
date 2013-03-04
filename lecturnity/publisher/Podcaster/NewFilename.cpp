// NewFilename.cpp : implementation file
//

#include "stdafx.h"
#include "podcaster.h"
#include "NewFilename.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewFilename dialog


CNewFilename::CNewFilename(CWnd* pParent /*=NULL*/)
	: CDialog(CNewFilename::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewFilename)
	m_csFileName = _T("");
	//}}AFX_DATA_INIT
}


void CNewFilename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewFilename)
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_EDIT1, m_edtFileName);
	DDX_Text(pDX, IDC_EDIT1, m_csFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewFilename, CDialog)
	//{{AFX_MSG_MAP(CNewFilename)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewFilename message handlers

BOOL CNewFilename::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_PODCASTER);
   SetIcon(m_hIcon, TRUE);

   DWORD dwID[] = {
         IDL_NF1,
         IDL_NF2,
         IDCANCEL,
         -1 };
   MfcUtils::Localize(this, dwID);

   CString csCaption;
   csCaption.LoadString(IDD_NEWMP4);
   this->SetWindowText(csCaption);

	m_edtFileName.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewFilename::OnChangeEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
   UpdateData();
   m_btnOk.EnableWindow(!m_csFileName.IsEmpty());
	
}

void CNewFilename::OnOK() 
{
	// TODO: Add extra validation here
   UpdateData();
	CDialog::OnOK();
}

// PublishProgressDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "publisher.h"
#include "PublishProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPublishProgressDlg 


CPublishProgressDlg::CPublishProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPublishProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPublishProgressDlg)
	m_lbTimeLeft = _T("");
	//}}AFX_DATA_INIT
}


void CPublishProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPublishProgressDlg)
	DDX_Control(pDX, IDC_CANCEL_PUBLISH, m_btCancel);
	DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_prgTotal);
	DDX_Control(pDX, IDC_PROGRESS_DETAIL, m_prgDetail);
	DDX_Control(pDX, IDC_LB_DETAIL, m_lbDetail);
	DDX_Text(pDX, IDC_LB_TIMELEFT, m_lbTimeLeft);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPublishProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CPublishProgressDlg)
	ON_BN_CLICKED(IDC_CANCEL_PUBLISH, OnCancelPublish)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPublishProgressDlg 

void CPublishProgressDlg::OnCancelPublish() 
{
   UINT nRet = MessageBox(_T("Sind Sie sicher, dass Sie die Verarbeitung abbrechen wollen?"), _T("Bestätigen"),
      MB_YESNO | MB_ICONQUESTION);
   if (IDYES == nRet)
      EndDialog(IDOK);
}

BOOL CPublishProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   m_prgTotal.SetRange(0, 100);
   m_prgTotal.SetPos(75);
   m_prgDetail.SetRange(0, 100);
   m_prgDetail.SetPos(33);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

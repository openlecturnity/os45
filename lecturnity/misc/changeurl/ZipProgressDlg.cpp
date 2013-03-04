// ZipProgressDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "changeurl.h"
#include "changeurlengine.h"
#include "ZipProgressDlg.h"

#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CZipProgressDlg 


CZipProgressDlg::CZipProgressDlg(CWnd* pParent, CChangeUrlEngine *pEngine)
	: m_bInitialized(false), m_pEngine(pEngine), 
     CDialog(CZipProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZipProgressDlg)
	//}}AFX_DATA_INIT

   m_bCloseRequested = false;
}


void CZipProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZipProgressDlg)
	DDX_Control(pDX, IDC_CANCEL, m_btCancel);
	DDX_Control(pDX, IDC_FILELABEL, m_lbStatus);
	DDX_Control(pDX, IDC_ZIPPROGRESS, m_progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZipProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CZipProgressDlg)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CZipProgressDlg 

BOOL CZipProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CString csCap;
   csCap.LoadString(IDS_ZIP_CAPTION);
   SetWindowText(csCap);

   DWORD dwIds[] = {IDC_STATUS_LABEL,
                    IDCANCEL,
                    -1};

   MfcUtils::Localize(this, dwIds);

   m_bInitialized = true;

   SetTimer(12345, 500, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CZipProgressDlg::SetMax(int nMax)
{
   m_progress.SetRange(0, nMax);
   m_progress.SetPos(0);
}

void CZipProgressDlg::SetProgress(const _TCHAR *tszString, int nProgress)
{
   m_progress.SetPos(nProgress);
   m_lbStatus.SetWindowText(tszString);
}

void CZipProgressDlg::OnCancel() 
{
   m_btCancel.EnableWindow(FALSE);
   m_pEngine->RequestZipCancel();
}

void CZipProgressDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen

   if (m_bCloseRequested)
   {
      KillTimer(12345);
      EndDialog(0);
   }
	
	CDialog::OnTimer(nIDEvent);
}

void CZipProgressDlg::CloseDialog()
{
   m_bCloseRequested = true;
}

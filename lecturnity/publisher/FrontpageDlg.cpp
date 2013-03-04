// FrontpageDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "publisher.h"
#include "FrontpageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFrontpageDlg 


CFrontpageDlg::CFrontpageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFrontpageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrontpageDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_white.CreateSolidBrush(RGB(255, 255, 255));
}


void CFrontpageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrontpageDlg)
	DDX_Control(pDX, IDC_FRONTPAGE_PROFILE, m_cbbProfile);
	DDX_Control(pDX, IDC_FRONTPAGE_FILENAME, m_edFileName);
	DDX_Control(pDX, IDC_FRONTPAGE_DONTSHOWAGAIN, m_cbDontShowAgain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrontpageDlg, CDialog)
	//{{AFX_MSG_MAP(CFrontpageDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_FRONTPAGE_EXPERTMODE, OnExpertMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFrontpageDlg 

HBRUSH CFrontpageDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   // return m_white;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CFrontpageDlg::OnExpertMode() 
{
   EndDialog(IDOK);	
}

BOOL CFrontpageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

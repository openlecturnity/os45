// IntlFilenameWarningDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filesdk.h"
#include "IntlFilenameWarningDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CIntlFilenameWarningDlg 


CIntlFilenameWarningDlg::CIntlFilenameWarningDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIntlFilenameWarningDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIntlFilenameWarningDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CIntlFilenameWarningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntlFilenameWarningDlg)
	DDX_Control(pDX, IDC_INTL_DONT_WARN, m_cbDontWarn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIntlFilenameWarningDlg, CDialog)
	//{{AFX_MSG_MAP(CIntlFilenameWarningDlg)
	ON_BN_CLICKED(ID_YES, OnYes)
	ON_BN_CLICKED(ID_NO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CIntlFilenameWarningDlg 

BOOL CIntlFilenameWarningDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   CString csCaption;
   csCaption.LoadString(IDD);
   SetWindowText(csCaption);

   DWORD dwIds[] = {IDC_INTL_WARNING,
                    IDC_INTL_DONT_WARN,
                    ID_YES,
                    ID_NO,
                    -1};
   Localize(this, dwIds);

   m_cbDontWarn.SetCheck(BST_UNCHECKED);
   m_bDontShowDialogAgain = false;
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CIntlFilenameWarningDlg::OnYes() 
{
   m_bDontShowDialogAgain = (m_cbDontWarn.GetCheck() == BST_CHECKED);
   EndDialog(IDYES);	
}

void CIntlFilenameWarningDlg::OnNo() 
{
   m_bDontShowDialogAgain = (m_cbDontWarn.GetCheck() == BST_CHECKED);
   EndDialog(IDNO);	
}

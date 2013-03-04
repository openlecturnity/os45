// InsertStopmarkDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "MfcUtils.h"
#include "InsertStopmarkDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInsertStopmarkDialog 


CInsertStopmarkDialog::CInsertStopmarkDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInsertStopmarkDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertStopmarkDialog)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

CInsertStopmarkDialog::CInsertStopmarkDialog(CWnd* pParent, int &nCheckID) : CDialog(CInsertStopmarkDialog::IDD, pParent)
{
	m_nCheckID = &nCheckID;
}

void CInsertStopmarkDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertStopmarkDialog)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertStopmarkDialog, CDialog)
	//{{AFX_MSG_MAP(CInsertStopmarkDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CInsertStopmarkDialog 

BOOL CInsertStopmarkDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   CString csTitle;
   csTitle.LoadString(CInsertStopmarkDialog::IDD);
   SetWindowText(csTitle);
   
   DWORD dwIds[] = {IDS_INSERT,
                    IDC_CURRENT_SLIDE_END,
                    IDC_CURRENT_TIMESTAMP,
                    IDC_ALL_SLIDES,
                    IDOK,
                    IDCANCEL,
                    -1};
   MfcUtils::Localize(this, dwIds);

	CheckRadioButton(IDC_CURRENT_SLIDE_END, IDC_ALL_SLIDES, *m_nCheckID);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CInsertStopmarkDialog::OnCancel() 
{
	*m_nCheckID = IDCANCEL;

	CDialog::OnCancel();
}

void CInsertStopmarkDialog::OnOK() 
{
	*m_nCheckID = GetCheckedRadioButton(IDC_CURRENT_SLIDE_END, IDC_ALL_SLIDES);

	CDialog::OnOK();
}

// static functions

void CInsertStopmarkDialog::ShowDialog(CWnd* pParent, int &nCheckID)
{
	CInsertStopmarkDialog dlgInsertStopmark(pParent, nCheckID);
	dlgInsertStopmark.DoModal();
}

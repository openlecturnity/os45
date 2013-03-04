// InsertTargetmarkDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "InsertTargetmarkDialog.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInsertTargetmarkDialog 


CInsertTargetmarkDialog::CInsertTargetmarkDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInsertTargetmarkDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertTargetmarkDialog)
	//}}AFX_DATA_INIT
}

CInsertTargetmarkDialog::CInsertTargetmarkDialog(CWnd* pParent, int &nCheckID, CString &csTargetmarkName, bool bDisableRadioButtons)
	: CDialog(CInsertTargetmarkDialog::IDD, pParent)
{
	m_nCheckID = &nCheckID;
	m_csTargetmarkName = &csTargetmarkName;
	m_bDisableRadioButtons = bDisableRadioButtons;
}

void CInsertTargetmarkDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertTargetmarkDialog)
	DDX_Control(pDX, IDC_EDIT_TARGETNAME, m_editTargetmarkName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertTargetmarkDialog, CDialog)
	//{{AFX_MSG_MAP(CInsertTargetmarkDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CInsertTargetmarkDialog 

BOOL CInsertTargetmarkDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CString csTitle;
   csTitle.LoadString(CInsertTargetmarkDialog::IDD);
   SetWindowText(csTitle);
   
   DWORD dwIds[] = {IDS_INSERT,
                    IDC_CURRENT_SLIDE_BEGIN,
                    IDC_CURRENT_TIMESTAMP,
                    IDOK,
                    IDCANCEL,
                    -1};
   MfcUtils::Localize(this, dwIds);

	// fill text field with targetmark name
	m_editTargetmarkName.SetWindowText((*m_csTargetmarkName));

	// check current radio box
	CheckRadioButton(IDC_CURRENT_SLIDE_BEGIN, IDC_CURRENT_TIMESTAMP, *m_nCheckID);

	if (m_bDisableRadioButtons)
	{
		for (int nID = IDC_CURRENT_SLIDE_BEGIN; nID <= IDC_CURRENT_TIMESTAMP; ++nID)
		{
			CWnd *pButton = GetDlgItem(nID);
			if (pButton)
				pButton->EnableWindow(FALSE);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CInsertTargetmarkDialog::OnCancel() 
{
	*m_nCheckID = IDCANCEL;
	CDialog::OnCancel();
}

void CInsertTargetmarkDialog::OnOK() 
{
	// TODO: Zusätzliche Prüfung hier einfügen
	
	*m_nCheckID = GetCheckedRadioButton(IDC_CURRENT_SLIDE_BEGIN, IDC_CURRENT_TIMESTAMP);
	CString csTargetmarkname;
	m_editTargetmarkName.GetWindowText(csTargetmarkname);
	(*m_csTargetmarkName) = csTargetmarkname;

	CDialog::OnOK();
}

// static functions

void CInsertTargetmarkDialog::ShowDialog(CWnd* pParent, int &nCheckID, CString &csTargetmarkName, bool bDisableRadioButtons)
{
	CInsertTargetmarkDialog dlgInsertTargetmark(pParent, nCheckID, csTargetmarkName, bDisableRadioButtons);
	dlgInsertTargetmark.DoModal();
}
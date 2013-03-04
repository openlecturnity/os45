// ScormProgressDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "ScormProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CScormProgressDlg 


CScormProgressDlg::CScormProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScormProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScormProgressDlg)
	//}}AFX_DATA_INIT
}


void CScormProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScormProgressDlg)
	DDX_Control(pDX, IDC_SCORM_PRGLABEL, m_lbProgress);
	DDX_Control(pDX, IDC_SCORM_TOTAL_PROGRESS, m_pcTotalProgress);
	DDX_Control(pDX, IDC_SCORM_FILE_PROGRESS, m_pcFileProgress);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScormProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CScormProgressDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CScormProgressDlg 

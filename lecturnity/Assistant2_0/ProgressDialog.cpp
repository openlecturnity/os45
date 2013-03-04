// ProgressDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include <process.h> 
#include "ProgressDialog.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Klasse CProgress zur Steuerung von CProgressdialog

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProgressDialogA 


CProgressDialogA::CProgressDialogA(CWnd* pParent, HANDLE hDialogInitEvent)
	: CDialog(CProgressDialogA::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialogA)
	//}}AFX_DATA_INIT

   m_hDialogInitEvent = hDialogInitEvent;
}

void CProgressDialogA::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDialogA)
	DDX_Control(pDX, IDC_PROGRESS_LABEL, m_progressLabel);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDialogA, CDialog)
	//{{AFX_MSG_MAP(CProgressDialogA)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CProgressDialogA 

void CProgressDialogA::SetLabel(LPCTSTR lpText)
{
   if (m_progressLabel.m_hWnd)
      m_progressLabel.SetWindowText(lpText);
}

void CProgressDialogA::SetLabel(UINT nResourceID)
{
   //CString string;
   m_labelString.LoadString(nResourceID);
   SetLabel(m_labelString);
}


void CProgressDialogA::SetRange(int lower, int upper)
{
   if (IsWindow(m_progress.m_hWnd))
      m_progress.SetRange(lower, upper);
}

void CProgressDialogA::SetStep(int step)
{
   if (IsWindow(m_progress.m_hWnd))
      m_progress.SetStep(step);
}

void CProgressDialogA::SetPos(int pos)
{
   if (IsWindow(m_progress.m_hWnd))
      m_progress.SetPos(pos);
}

void CProgressDialogA::OffsetPos(int offset)
{
   if (IsWindow(m_progress.m_hWnd))
      m_progress.OffsetPos(offset);
}

LRESULT CProgressDialogA::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
   if (message == WM_USER && wParam == END_DIALOG)
   {
      EndWaitCursor();
      EndDialog(0);
   }

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CProgressDialogA::OnInitDialog() 
{
	CDialog::OnInitDialog();

   DWORD dwIds[] = {IDC_PROGRESS_LABEL, 
                   -1};
   MfcUtils::Localize(this, dwIds);

	if (m_hDialogInitEvent)
      SetEvent(m_hDialogInitEvent);

   BeginWaitCursor();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

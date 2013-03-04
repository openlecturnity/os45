// ProgressDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include <process.h> 
#include "SingleProgressDialog.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 * Standard progress dialog with label and progress bar
 */

void CSingleProgressDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSingleProgressDialog)
    DDX_Control(pDX, IDC_PROGRESS_LABEL, m_progressLabel);
    DDX_Control(pDX, IDC_PROGRESS_BAR, m_progress);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSingleProgressDialog, CDialog)
    //{{AFX_MSG_MAP(CSingleProgressDialog)
    ON_MESSAGE(WM_USER, OnCheckEndDialog)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CSingleProgressDialog::CSingleProgressDialog(CWnd* pParent, HANDLE hDialogInitEvent)
: CDialog(CSingleProgressDialog::IDD, pParent) {
    //{{AFX_DATA_INIT(CSingleProgressDialog)
    //}}AFX_DATA_INIT

    m_hDialogInitEvent = hDialogInitEvent;
}

/*
 * Public functions to manipulate the appearance of the progress dialog
 */

void CSingleProgressDialog::SetRange(int lower, int upper) {
    if (IsWindow(m_progress.m_hWnd))
        m_progress.SetRange(lower, upper);
}

void CSingleProgressDialog::SetLabel(LPCTSTR lpText) {
    if (m_progressLabel.m_hWnd)
        m_progressLabel.SetWindowText(lpText);
}

void CSingleProgressDialog::SetLabel(UINT nResourceID) {
    m_labelString.LoadString(nResourceID);
    SetLabel(m_labelString);
}

void CSingleProgressDialog::SetStep(int step) {
    if (IsWindow(m_progress.m_hWnd))
        m_progress.SetStep(step);
}

void CSingleProgressDialog::SetPos(int pos) {
    if (IsWindow(m_progress.m_hWnd))
        m_progress.SetPos(pos);
}

void CSingleProgressDialog::OffsetPos(int offset) {
    if (IsWindow(m_progress.m_hWnd))
        m_progress.OffsetPos(offset);
}

/*
 * Private functions to create and handle the progress dialog
 */

BOOL CSingleProgressDialog::OnInitDialog()  {
    CDialog::OnInitDialog();
    
    SetLabel(_T(""));
    m_progress.SetRange(0, 100);
    m_progress.SetPos(0);

    if (m_hDialogInitEvent)
        SetEvent(m_hDialogInitEvent);

    BeginWaitCursor();
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

LRESULT CSingleProgressDialog::OnCheckEndDialog(WPARAM wParam, LPARAM lParam) {
    if (wParam == END_DIALOG) {
        EndWaitCursor();
        EndDialog(0);
    }

    return 0;
}

// WaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "WaitDlg.h"
#include "MfcUtils.h"


// CWaitDlg dialog

IMPLEMENT_DYNAMIC(CWaitDlg, CDialog)

CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWaitDlg::IDD, pParent) {
}

CWaitDlg::~CWaitDlg() {
}

void CWaitDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_WAIT_MESSAGE, m_Message);
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
    ON_WM_SHOWWINDOW()
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CWaitDlg message handlers

BOOL CWaitDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    CString csMessage;
    csMessage.LoadString(IDC_WAIT_MESSAGE);
    m_Message.SetWindowText(csMessage);
    csMessage.LoadString(IDS_PODCASTER);
    SetWindowText(csMessage);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaitDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
    CDialog::OnShowWindow(bShow, nStatus);

    if (bShow) {
        UpdateWindow();
    }
    // TODO: Add your message handler code here
}

void CWaitDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {
    CDialog::OnActivate(nState, pWndOther, bMinimized);

    // TODO: Add your message handler code here

    UpdateWindow();
}
// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "MfcUtils.h"
#include "PasswordDlg.h"


// CPasswordDlg dialog

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialog)

CPasswordDlg::CPasswordDlg(CString csUser /*= NULL*/, CString csServer /*= NULL*/,CWnd* pParent /*=NULL*/)
: CDialog(CPasswordDlg::IDD, pParent) {
#ifndef _WIN32_WCE
    EnableActiveAccessibility();
#endif
    m_csUserName = csUser;
    m_csPassword = _T("");
    m_csServer = csServer;
}

CPasswordDlg::~CPasswordDlg() {
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_USERNAME, m_edtUserName);
    DDX_Control(pDX, IDE_PASSWORD, m_edtPassword);
    DDX_Control(pDX, IDOK, m_btnOK);
    DDX_Control(pDX, IDCANCEL, m_btnCancel);
    DDX_Control(pDX, IDL_PASSWORD, m_lblPassword);
    DDX_Control(pDX, IDL_USERNAME, m_lblUsername);
    DDX_Control(pDX, IDL_PWD_SERVER, m_lblPwdServer);
    DDX_Control(pDX, IDC_STATIC_SERVER, m_lblServer);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CPasswordDlg::OnBnClickedOk)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPasswordDlg message handlers

BOOL CPasswordDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    SetIcon(AfxGetApp()->LoadIcon(IDI_UPLOADER), true);

    DWORD dwID[] = {
        IDL_USERNAME,
        IDL_PASSWORD,
        IDOK,
        IDCANCEL,
        -1
    };

    MfcUtils::Localize(this, dwID);

    CString csCaption;
    csCaption.LoadString(IDD_PASSWORD_DIALOG);
    SetWindowText(csCaption);
    m_edtUserName.SetWindowText(m_csUserName);

    if (!m_csServer.IsEmpty()) {
        SetDllDialog();
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswordDlg::OnBnClickedOk() {
    // TODO: Add your control notification handler code here
    m_edtUserName.GetWindowText(m_csUserName);
    m_edtPassword.GetWindowText(m_csPassword);
    OnOK();
}

CString CPasswordDlg::GetUsername(void) {
    return m_csUserName;
}

CString CPasswordDlg::GetPassword(void) {
    return m_csPassword;
}

void CPasswordDlg::SetUsername(CString csUserName) {
    m_csUserName = csUserName;
}

void CPasswordDlg::SetDllDialog() {
    // m_lblUsername.ShowWindow(SW_HIDE);
    // m_edtUserName.ShowWindow(SW_HIDE);

    CRect rcClient;
    GetWindowRect(rcClient);
    CRect rcBtnOK;
    m_btnOK.GetWindowRect(&rcBtnOK);
    CRect rcBtnCancel;
    m_btnOK.GetWindowRect(&rcBtnCancel);
    CRect rcLbl;
    m_lblPassword.GetWindowRect(&rcLbl);
    CRect rcEdt;
    m_edtPassword.GetWindowRect(&rcEdt);
    int cx = 5 + 50 + rcEdt.Width() + 20 + rcBtnOK.Width() + 5;
    int cy = rcClient.Height() + 12;

    SetWindowPos(NULL, 0,0 , cx, cy, SWP_NOZORDER | SWP_SHOWWINDOW);
    GetWindowRect(&rcClient);
    GetClientRect(&rcClient);

    //m_btnOK.SetWindowPos(NULL,rcClient.right - 5 - rcBtn.Width(), rcClient.top + 10, rcBtn.Width(), rcBtn.Height(), SWP_SHOWWINDOW);
    //m_btnCancel.SetWindowPos(NULL, rcClient.right - 5 - rcBtn.Width(), rcClient.top + 10 + 5 + rcBtn.Height(), rcBtn.Width(), rcBtn.Height(), SWP_SHOWWINDOW);
    m_btnOK.SetWindowPos(NULL,rcClient.left + 5 + 22 + 86 , rcClient.bottom - rcBtnOK.Height() - 5, rcBtnOK.Width(), rcBtnOK.Height(), SWP_SHOWWINDOW);
    m_btnCancel.SetWindowPos(NULL, rcClient.left + 5 + 10 + 5 + 22 + 2 * rcBtnOK.Width(), rcClient.bottom - rcBtnCancel.Height() - 5, rcBtnCancel.Width(), rcBtnCancel.Height(), SWP_SHOWWINDOW);
    m_lblUsername.SetWindowPos(NULL, rcClient.left + 5, rcClient.bottom - rcBtnOK.Height() - 17 - 5 - rcLbl.Height() - rcEdt.Height(), 72, rcLbl.Height(), SWP_SHOWWINDOW); 
    m_lblPassword.SetWindowPos(NULL, rcClient.left + 5, rcClient.bottom - rcBtnOK.Height() - 17 - rcLbl.Height(), 72, rcLbl.Height(), SWP_SHOWWINDOW); 

    m_edtUserName.SetWindowPos(NULL, rcClient.left + 10 + 72, rcClient.bottom - rcBtnOK.Height() - 11 - 5 - 2 * rcEdt.Height(), rcEdt.Width(), rcEdt.Height(), SWP_SHOWWINDOW);
    m_edtPassword.SetWindowPos(NULL, rcClient.left + 10 + 72, rcClient.bottom - rcBtnOK.Height() - 11 - rcEdt.Height(), rcEdt.Width(), rcEdt.Height(), SWP_SHOWWINDOW);

    CString csPwdServer;
    csPwdServer.LoadString(IDS_PWD_SERVER);
    /*CString csLblPwdText;
    csLblPwdText.Format(csPwdServer, m_csUserName, m_csServer);*/
    m_lblPwdServer.ShowWindow(SW_SHOW);
    m_lblPwdServer.SetWindowText(/*csLblPwdText*/csPwdServer);


    CFont* pLblFont = m_lblServer.GetFont();
    LOGFONT lf;
    pLblFont->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    pLblFont->CreateFontIndirect(&lf);
    m_lblServer.SetFont(pLblFont,TRUE);  
    CString csServer;
    csServer.Format(_T("\"%s\""), m_csServer);
    m_lblServer.ShowWindow(SW_SHOW);
    m_lblServer.SetWindowText(csServer);

    CString csCaption, csText;
    csText.LoadString(IDS_PWD_DIALOG_DLL);
    csCaption.Format(csText, m_csServer);
    SetWindowText(csCaption);
    CenterWindow();
}

HBRUSH CPasswordDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here
    COLORREF textColor = RGB(0, 0, 255);

    // Set text color
    if (pWnd->GetDlgCtrlID() == IDC_STATIC_SERVER) {
        pDC->SetTextColor(textColor);
    }

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}

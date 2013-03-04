#pragma once

#include "Uploader.h"
#include "afxwin.h"

// CPasswordDlg dialog

class CPasswordDlg : public CDialog {
    DECLARE_DYNAMIC(CPasswordDlg)

public:
    CPasswordDlg(CString csUser = NULL, CString csServer = NULL, CWnd* pParent = NULL);   // standard constructor
    virtual ~CPasswordDlg();

    // Dialog Data
    enum { IDD = IDD_PASSWORD_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    CString GetUsername(void);
    CString GetPassword(void);
    void SetUsername(CString csUserName);
private:
    CEdit m_edtUserName;
    CEdit m_edtPassword;
    CString m_csUserName;
    CString m_csPassword;
    CString m_csServer;
    CButton m_btnOK;
    CButton m_btnCancel;
    CStatic m_lblPassword;
    CStatic m_lblUsername;
    void SetDllDialog();
    CStatic m_lblPwdServer;
    CStatic m_lblServer;
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

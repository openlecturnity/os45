#pragma once


// CWaitDlg dialog

class CWaitDlg : public CDialog {
    DECLARE_DYNAMIC(CWaitDlg)

public:
    CWaitDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CWaitDlg();

    // Dialog Data
    enum { IDD = IDD_WAIT_DIALOG };
private:
    CStatic m_Message;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

    DECLARE_MESSAGE_MAP()
};

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTransferProgressDlg dialog

class CTransferProgressDlg : public CDialog {
    DECLARE_DYNAMIC(CTransferProgressDlg)

    CString m_csFrom;
    CString m_csTo;
    double* m_pdProgress;

public:
    CTransferProgressDlg(double* pdProgress, CString csFrom = _T(""), CString csTo = _T(""), CWnd* pParent = NULL);   // standard constructor
    virtual ~CTransferProgressDlg();

    void SetFromText(CString csFrom);
    void SetToText(CString csTo);

    // Dialog Data
    enum { IDD = IDD_TRANSFER_PROGRESS_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
private:
    CProgressCtrl m_PcProgress;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
    CStatic m_lblFrom;
    CStatic m_lblTo;
};

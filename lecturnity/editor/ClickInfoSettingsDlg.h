#pragma once
#include "afxwin.h"


// CClickInfoSettingsDlg dialog
class CLcElementInfo;

class CClickInfoSettingsDlg : public CDialog {
    DECLARE_DYNAMIC(CClickInfoSettingsDlg)

public:
    CClickInfoSettingsDlg(CLcElementInfo *pLcElementInfo, CWnd* pParent = NULL);   // standard constructor
    virtual ~CClickInfoSettingsDlg();

    // Dialog Data
    enum { IDD = IDD_DIALOG_CLICK_INFO_SETTINGS };

    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
private:
    void SetControlType(CString csControlType);

private:
    CEdit m_edtAppName;
    CEdit m_edtCtrlName;
    CComboBox m_cmbCtrlType;

    CLcElementInfo *m_pLcElementInfo;
    // Copy of original element info. Dialog works with this one. 
    CLcElementInfo *m_pLcElementInfoCopy;

    CArray<CString, CString> m_acsControlTypes;

    CEdit m_edtX;
    CEdit m_edtY;
    CEdit m_edtWidth;
    CEdit m_edtHeight;
};

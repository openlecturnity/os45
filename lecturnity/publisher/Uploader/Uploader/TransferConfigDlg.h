#pragma once
#include "afxwin.h"
#include "xpgroupbox.h"
#include "CustomAutoComplete.h"


#define T_TRANSFER 0
#define T_CHANNEL 1
#define T_MEDIA 2
#define RECENT_KEY _T("Software\\imc AG\\LECTURNITY\\Publisher\\Recent\\")

// CTranferConfigDlg dialog

class CTransferConfigDlg : public CDialog {
    DECLARE_DYNAMIC(CTransferConfigDlg)

    TransferParam m_tp;
    int m_iType;
    bool m_bIsSlidestar;

    void CheckInputTextForBlanks(CString &csInputText);
    void CheckDirectoryFormat(CString &csDir);
    void CheckUrlDirectoryFormat(CString &csDir);
    void LoadTransferParam();
    void SaveTransferParam();
    void InitTransferParam();
    void IsNewChannel(bool bIsNewChannel);

public:
    CTransferConfigDlg(int Type, CWnd* pParent = NULL);   //constructor
    CTransferConfigDlg(int Type, TransferParam tp, CWnd* pParent = NULL, bool bIsSlidestar = false);   //constructor
    CTransferConfigDlg(int Type, bool bFromReg, CWnd* pParent = NULL);   //constructor
    virtual ~CTransferConfigDlg();

    TransferParam GetTransferParam();
    void SetTransferParam(TransferParam tpTransfer);

    void SetChannelName(CString csChannelName);
    CString GetChannelURL();
    void SetTempChannel(bool bTempChannel);

    // Dialog Data
    enum { IDD = IDD_TRANSFER_CONFIG_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL DestroyWindow();

    DECLARE_MESSAGE_MAP()
protected:
    afx_msg void OnCbnSelchangeService();
    afx_msg void OnEnKillfocusHost();
    afx_msg void OnEnKillfocusDirectory();
    afx_msg void OnBnClickedOk();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnSavePassword();

private:
    CXPGroupBox m_CbBackground;
    CEdit m_edtHost;
    CEdit m_edtDirectory;
    CEdit m_edtUsername;
    CEdit m_edtPassword;
    CButton m_CSavePassword;
    CEdit m_edtPort;
    CComboBox m_CbService;
    CEdit m_edtUrl;
    CStatic m_lblUrl;
    CStatic m_lblDirectory;
    CStatic m_GbServer;
    CStatic m_lblHost;
    CStatic m_lblUserName;
    CStatic m_lblPassword;
    CStatic m_lblService;
    CStatic m_lblPort;
    CStatic m_IBar;
    CButton m_btnOk;
    CButton m_btnCancel;

private:
    CString m_csChannelName;
    bool m_bTemporaryChannel;
    bool m_isKeyPressed;
    bool m_isFocused;
    bool m_bIsNewChannel;

    //Auto complete elements
    CCustomAutoComplete* m_acUrl;
    CCustomAutoComplete* m_acHost;
    CCustomAutoComplete* m_acUser;
    CCustomAutoComplete* m_acDir;
    CCustomAutoComplete* m_acPort;   

private:
    CString GetFileName(CString csFilePath);
    BOOL CheckTransferParam();
};

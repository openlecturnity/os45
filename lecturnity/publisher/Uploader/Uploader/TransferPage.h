#pragma once
#include "afxwin.h"

// CTransferPage dialog

class CTransferPage: public CDialog {
    DECLARE_DYNAMIC(CTransferPage)

public:
    CTransferPage(CWnd* pParent = NULL);   // standard constructor
    virtual ~CTransferPage();

    // Dialog Data
    enum { IDD = IDD_TRANSFER_PAGE };
private:
    CStatic m_stcChannelService;
    CStatic m_stcChannelUrl;
    CStatic m_stcChannelServer;
    CStatic m_stcChannelPort;
    CStatic m_stcChannelUsername;
    CStatic m_stcChannelPassword;
    CStatic m_stcChannelSave;
    CStatic m_stcChannelDir;
    CButton m_btnChannelChange;

    CStatic m_stcMediaService;
    CStatic m_stcMediaUrl;
    CStatic m_stcMediaServer;
    CStatic m_stcMediaPort;
    CStatic m_stcMediaUsername;
    CStatic m_stcMediaPassword;
    CStatic m_stcMediaSave;
    CStatic m_stcMediaDir;
    CButton m_btnMediaChange;

    //Message map functions
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnChangeChannel();
    afx_msg void OnChangeMedia();

    DECLARE_MESSAGE_MAP()

    //Memebers

public:
    void SetChannelName(CString csChannelName);
    CString GetChannelName();
    CString GetChannelURL();
    void LoadFromRegistry();
    void ClearPage();
    void EnableChangeButton(bool bEnable);
    void SetTemporaryChannel(TransferParam tpTempChannel);
    void SetTemporaryMedia(TransferParam tpTempMedia);
    TransferParam GetTemporaryChannel();
    void SetTempChannel(bool bTempChannel);

    //Attributes
private:
    CString m_csChannelName;
    bool m_bTemporaryChannel;
    TransferParam m_tcTempChannel;
    TransferParam m_tcTempMedia;
};

#pragma once
#include "XPGroupBox.h"
#include "Uploader.h"


// CChannelMetadata dialog

class CChannelMetadata : public CDialog {
    DECLARE_DYNAMIC(CChannelMetadata)

public:
    CChannelMetadata(CWnd* pParent = NULL);   // standard constructor
    virtual ~CChannelMetadata();

    // Dialog Data
    enum { IDD = IDD_CHANNELMETADATA };
private:
    CEdit m_edtTitle;
    CEdit m_edtSubtitle;
    CEdit m_edtAuthor;
    CEdit m_edtDescription;
    CEdit m_edtSummary;
    CEdit m_edtLanguage;
    CEdit m_edtCopyright;
    CEdit m_edtOwner;
    CEdit m_edtEmail;
    CEdit m_edtLink;
    CEdit m_edtWebmaster;
    CEdit m_edtExplicit;
    CEdit m_edtKeywords;
    CXPGroupBox m_gbWhiteRectangle;

    //Message map functions
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

    DECLARE_MESSAGE_MAP()

    //Methods
public:
    BOOL CreateNewChannel(LPCTSTR tstrNewChnnel);
    void InitMetadata(TransferMetadata tmMetadata);
    void SetMetadata(TransferMetadata tmMetadata);
    TransferMetadata GetMetadata();
    BOOL CreateTempChannel(LPCTSTR tstrTempChannel, TransferMetadata tmTempMetadata); 
    void SetTempChannel(bool bTempChannel);
    TransferMetadata GetTempMetadata();

    //Attributes
private:
    bool m_isKeyPressed;
    bool m_isFocused;
    bool m_bTemporaryChannel;	
    TransferMetadata m_tmTempMetadata;
    TransferMetadata m_tmMetadata;
    CUploaderApp *pApp;
};

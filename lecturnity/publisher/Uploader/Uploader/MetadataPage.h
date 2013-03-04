#pragma once
#include "afxwin.h"
#include "Uploader.h"


// CMetadataPage dialog

class CMetadataPage : public CDialog {
    DECLARE_DYNAMIC(CMetadataPage)

public:
    CMetadataPage(CWnd* pParent = NULL);   // standard constructor
    virtual ~CMetadataPage();

    // Dialog Data
    enum { IDD = IDD_METADATA_PAGE };
private:
    CStatic m_stcTitle;
    CStatic m_stcSubtitle;
    CStatic m_stcAuthor;
    CStatic m_stcDescription;
    CStatic m_stcSummary;
    CStatic m_stcLanguage;
    CStatic m_stcCopyright;
    CStatic m_stcOwner;
    CStatic m_stcEmail;
    CStatic m_stcLink;
    CStatic m_stcWebmaster;
    CStatic m_stcExplicit;
    CStatic m_stcKeywords;
    CButton m_btnChange;

    //Message map functions
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnChange();

    DECLARE_MESSAGE_MAP()

    //Members
public:
    BOOL GetMetadata(CString csUrl, BOOL bDisplayError = TRUE);
    CString GetTitle();
    void EnableChangeButton(bool bEnable);
    void SetTempChannel(bool bTempChannel);
    void SetTempMetadata(TransferMetadata tmTempMetadata);

    //Attributes
private:
    CUploaderApp *pApp;	
    bool m_bTemporaryChannel;
    TransferMetadata m_tmTempMetadata;
};

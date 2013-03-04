#pragma once
#include "xpgroupbox.h"
#include "afxwin.h"


// CMainDlg dialog

class CMainDlg : public CDialog {
    DECLARE_DYNAMIC(CMainDlg)

    CString m_csDocFile;
    CString m_csLrdFile;

    BOOL CheckUrlFileName(CString csUrl, int iServiceType = 0);
    void SearchForLrd(CString csMediaFile);
    void ParseLrd(bool bIsContextMenu);
    CString FindFirstLrdFile();

public:
    CMainDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CMainDlg();

    // Dialog Data
    enum { IDD = IDD_MAIN_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
private:
    CXPGroupBox m_GbBackground;
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedSettings();
    afx_msg void OnBnClickedPodcast();
    afx_msg void OnCbnEditchangePodcastDoc();
    afx_msg void OnCbnSelchangePodcastDoc();
    afx_msg void OnBnClickedBrowseDoc();
    afx_msg void OnBnClickedLrdMetadata();
    afx_msg void OnBnClickedBrowseLrd();
    afx_msg void OnBnClickedExplicitMetadata();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
private:
    CComboBox m_CbDocFiles;
    CButton m_btnBrowseLrd;
    CEdit m_edtLrdFile;
    CEdit m_edtTitle;
    CString m_csTitle;
    CEdit m_edtAuthor;
    CString m_csAuthor;
    bool m_bIsContextMenu;
public:
    CButton m_RLrdMetadata;
    CButton m_btnPodcast;
    void ShowPodcastSettings();
    void Podcast(CString csMp4File, bool bIsContextMenu = false);
    void ShowYtMainDlg(void);
};

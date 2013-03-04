#pragma once
#include "afxwin.h"


// CFileExists dialog

class CFileExists : public CDialog {
    DECLARE_DYNAMIC(CFileExists)

public:
    CFileExists(bool bNoMody = false, CWnd* pParent = NULL);   // standard constructor
    virtual ~CFileExists();

    // Dialog Data
    enum { IDD = IDD_MP4EXISTS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
public:
    afx_msg void OnBnClickedOk();
public:
    afx_msg void OnBnClickedMody();
public:
    afx_msg void OnBnClickedOver();
public:
    afx_msg void OnBnClickedCancel();
public:
    int RemoteFileExists(void);
    void SetUrl(CString csUrl);
    void SetFilename(CString csFilename);
    CString GetNewFilename();
    int GetAction();
private:
    void GetInetError();
    CString m_csFileName;
    CString m_csUrl;
    int m_iAction;
    CStatic m_cstFileName;
    CStatic m_lblMody;
    CButton m_btnMody;
    bool m_bNoMody;
};

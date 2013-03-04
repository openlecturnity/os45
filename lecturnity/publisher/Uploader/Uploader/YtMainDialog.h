#pragma once
#include "afxwin.h"


// CYtMainDialog dialog

class CYtMainDialog : public CDialog {
	DECLARE_DYNAMIC(CYtMainDialog)

public:
	CYtMainDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CYtMainDialog();

// Dialog Data
	enum { IDD = IDD_YT_MAIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonYtAuth();
public:
    CEdit m_edtDummy;
private:
    CString m_csAuthToken;
    CArray<CString, CString>m_acsYtCategories;
    bool GetAuthToken(CString csResponse, CString &csToken, CString &csError);
    void InitCategories();
    //CString CreateHeader
    CButton m_btnYtUpload;
    YtTransferParam m_ytTp;
    afx_msg void OnBnClickedYtUploadButton();
    CEdit m_edtMp4;
    afx_msg void OnBnClickedYtBrowseButton();
    CEdit m_edtDmy2;
    afx_msg void OnBnClickedButton1();
};

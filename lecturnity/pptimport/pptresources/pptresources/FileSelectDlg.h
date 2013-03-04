#pragma once
#include "afxwin.h"


// CFileSelectDlg dialog

class CFileSelectDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CFileSelectDlg)

public:
	CFileSelectDlg();
	virtual ~CFileSelectDlg();

// Dialog Data
	enum { IDD = IDD_FILE_SELECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	virtual BOOL OnSetActive();
	void Init(CString pPPTName, WCHAR *pLSDName);
private:
	CString m_csPPTName;
	WCHAR *m_pLSDName;
	CEdit m_edtFileName;
	CButton m_btnBrowse;
	afx_msg void OnBnClickedButtonBrowse();
	virtual BOOL OnWizardFinish();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

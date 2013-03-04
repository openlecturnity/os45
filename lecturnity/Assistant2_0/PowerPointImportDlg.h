#if !defined(AFX_POWERPOINTIMPORTDLG_H__F6DBA234_0AE5_43E9_9919_807B5C28491E__INCLUDED_)
#define AFX_POWERPOINTIMPORTDLG_H__F6DBA234_0AE5_43E9_9919_807B5C28491E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PowerPointImportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPowerPointImportDlg dialog

class CPowerPointImportDlg : public CDialog
{
// Construction
public:
	CPowerPointImportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPowerPointImportDlg)
	enum { IDD = IDD_POWERPOINT_IMPORT };
	CButton	m_RadioNewProject;
	CButton	m_RadioExistingProject;
	CListCtrl	m_ExistingProjectsList;
	//}}AFX_DATA
   

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPowerPointImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPowerPointImportDlg)
   virtual BOOL OnInitDialog();
	afx_msg void OnRadioPowerpointImportDlgExistingproject();
	afx_msg void OnRadioPowerpointImportDlgNewproject();
	//}}AFX_MSG

   
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POWERPOINTIMPORTDLG_H__F6DBA234_0AE5_43E9_9919_807B5C28491E__INCLUDED_)

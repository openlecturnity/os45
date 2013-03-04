#if !defined(AFX_MP4EXISTS_H__3AB6767C_400C_458C_9DE1_A53DF3297985__INCLUDED_)
#define AFX_MP4EXISTS_H__3AB6767C_400C_458C_9DE1_A53DF3297985__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mp4Exists.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMp4Exists dialog

class CMp4Exists : public CDialog
{
   void GetInetError();
// Construction
public:
	CMp4Exists(CWnd* pParent = NULL);   // standard constructor
   int RemoteFileExists();

   CString m_csFileName;
   CString m_csUrl;
   int iAction;

// Dialog Data
	//{{AFX_DATA(CMp4Exists)
	enum { IDD = IDD_MP4EXISTS };
	CString	m_Mp4FileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMp4Exists)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMp4Exists)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnOver();
	virtual void OnCancel();
	afx_msg void OnMody();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MP4EXISTS_H__3AB6767C_400C_458C_9DE1_A53DF3297985__INCLUDED_)

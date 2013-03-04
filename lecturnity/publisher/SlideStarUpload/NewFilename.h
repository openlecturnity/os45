#if !defined(AFX_NEWFILENAME_H__3977B0F1_1C1F_4009_AD2B_0D6E6B176A08__INCLUDED_)
#define AFX_NEWFILENAME_H__3977B0F1_1C1F_4009_AD2B_0D6E6B176A08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewFilename.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewFilename dialog

class CNewFilename : public CDialog
{
// Construction
public:
	CNewFilename(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewFilename)
	enum { IDD = IDD_NEWZIP };
	CButton	m_btnOk;
	CEdit	m_edtFileName;
	CString	m_csFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewFilename)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewFilename)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdit();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWFILENAME_H__3977B0F1_1C1F_4009_AD2B_0D6E6B176A08__INCLUDED_)

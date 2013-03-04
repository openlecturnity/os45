#if !defined(AFX_CONFIRMDELETE_H__B7EE646D_ABB5_4CE2_8CD9_BB0ABFA5C9C5__INCLUDED_)
#define AFX_CONFIRMDELETE_H__B7EE646D_ABB5_4CE2_8CD9_BB0ABFA5C9C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfirmDelete.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfirmDelete dialog

class CConfirmDelete : public CDialog
{
// Construction
public:
	CConfirmDelete(CWnd* pParent = NULL);   // standard constructor
private:
   void OnYes();
   void OnNo();
// Dialog Data
	//{{AFX_DATA(CConfirmDelete)
	enum { IDD = IDD_CONFIRM_DELETE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmDelete)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmDelete)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIRMDELETE_H__B7EE646D_ABB5_4CE2_8CD9_BB0ABFA5C9C5__INCLUDED_)

#if !defined(AFX_TARGETLISTDD_H__A0FACDA6_3927_4D5B_8210_3AE2D901A108__INCLUDED_)
#define AFX_TARGETLISTDD_H__A0FACDA6_3927_4D5B_8210_3AE2D901A108__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TargetListDD.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CTargetListDD window

class CTargetListDD : public CListCtrl
{
// Construction
public:
	CTargetListDD();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTargetListDD)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTargetListDD();
		
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CTargetListDD)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegintrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);


	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
private:
	CString csOldText;


public:
	int GetSelectedItem();



};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TARGETLISTDD_H__A0FACDA6_3927_4D5B_8210_3AE2D901A108__INCLUDED_)

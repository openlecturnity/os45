#if !defined(AFX_MYTABCTRL_H__BAF69081_1BBC_476D_A303_7660CCB9A9B7__INCLUDED_)
#define AFX_MYTABCTRL_H__BAF69081_1BBC_476D_A303_7660CCB9A9B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MyTabCtrl window

class MyTabCtrl : public CTabCtrl
{
// Construction
public:
	MyTabCtrl();

// Attributes
public:
	int m_DialogID[2];
	CDialog *m_Dialog[2];

	int m_nPageCount;

	void ActivateTabDialogs();
	void InitDialogs();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyTabCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~MyTabCtrl();
   void GetTransferSettings(CString csChannelName);

	// Generated message map functions
protected:
	//{{AFX_MSG(MyTabCtrl)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTABCTRL_H__BAF69081_1BBC_476D_A303_7660CCB9A9B7__INCLUDED_)

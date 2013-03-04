#if !defined(AFX_LRDDIALOG2_H__457BB9B2_8513_426C_9371_DAB52015A25A__INCLUDED_)
#define AFX_LRDDIALOG2_H__457BB9B2_8513_426C_9371_DAB52015A25A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LrdDialog2.h : header file
//

#include "LrdDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CLrdDialog2 dialog

class CLrdDialog2 : public CDialog
{
   CLrdDialog dlg;
// Construction
   CString csActiveChannel;
   void SearchForLrd(CString csMediaFile);
public:
   CString csIniMp4File;
   HWND m_HWaitDlg;
	CLrdDialog2(CWnd* pParent = NULL);   // standard constructor
   void OnOK2();
   void OnCancel2();

// Dialog Data
	//{{AFX_DATA(CLrdDialog2)
	enum { IDD = IDD_LRD_DIALOG2 };
	CComboBox	m_Mp4Files;
	CStatic	m_stcBarBottom;
	CString	m_csMp4Files;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLrdDialog2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DrawLogo();
	// Generated message map functions
	//{{AFX_MSG(CLrdDialog2)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnBrowse();
	afx_msg void OnPodcast();
	afx_msg void OnSetings();
	afx_msg void OnEditchangePodcastdoc();
	afx_msg void OnSelchangePodcastdoc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   BOOL CheckUrlFileName(CString csUrl);
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
public:
//	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LRDDIALOG2_H__457BB9B2_8513_426C_9371_DAB52015A25A__INCLUDED_)

#pragma once
#include "afxwin.h"


// CWaitDlg dialog

class CWaitDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaitDlg)

public:
	CWaitDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaitDlg();

// Dialog Data
	enum { IDD = IDD_WAIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Message;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
public:
//	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};

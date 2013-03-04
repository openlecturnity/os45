#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#define WM_SELECT_CALL		WM_USER+11
#define WM_CHANGE_PROGRESS  WM_USER+12
#define WM_END_PROGRESS     WM_USER+13
#define WM_CHANGE_PROG_TEXT	WM_USER+14

// CMyProgressDlg dialog

class CMyProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CMyProgressDlg)

public:
	CMyProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMyProgressDlg();

// Dialog Data
	enum { IDD = IDD_PROGRESS_DLG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	LRESULT OnSelectCall(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeProgress(WPARAM wParam, LPARAM lParam);
	LRESULT OnEndProgress(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeProgText(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void Init(HWND *pHwnd, CEvent * pEvent = NULL, bool bIsWizard = false);
	enum
	{
		TITLE_MASTER,
		MASTER,
		TEMPLATE,
		SLIDE
	};
private:
	HWND	*m_pHwnd;
	CEvent	*m_pEvent;
	bool	m_bIsWizard;
	CProgressCtrl m_pctProgress;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButton m_btnBack;
	CButton m_btnNext;
	CButton m_btnCancel;
	int aiProgressVal[4];
	int iSelectedCall;
	INT_PTR m_iTimer;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int m_iDurationVal;
	HICON m_hIcon;
};

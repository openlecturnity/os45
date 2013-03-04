#if !defined(AFX_TRANSFERPAGE_H__34632D9A_7BA3_4ED8_9E0E_07154FAA5E7F__INCLUDED_)
#define AFX_TRANSFERPAGE_H__34632D9A_7BA3_4ED8_9E0E_07154FAA5E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransferPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransferPage dialog

class CTransferPage : public CDialog
{
   COLORREF color;
   CBrush brush;
// Construction
public:
   CString m_csChannelName;
	CTransferPage(CWnd* pParent = NULL);   // standard constructor
   void LoadFromRegistry();
   void ClearPage();

// Dialog Data
	//{{AFX_DATA(CTransferPage)
	enum { IDD = IDD_TRANSFER_PAGE };
	CButton	m_btnChange;
	CString	m_csService;
	CString	m_csChannel;
	CString	m_csCompression;
	CString	m_csServer;
	CString	m_csIP;
	CString	m_csPassword;
	CString	m_csPort;
	CString	m_csSave;
	CString	m_Ssh;
	CString	m_csUsername;
	CString	m_csWebUrl;
	CString	m_csMp4Dir;
	CString	m_csPkf;
	CString	m_csXmlDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransferPage)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransferPage)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnCancel();
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFERPAGE_H__34632D9A_7BA3_4ED8_9E0E_07154FAA5E7F__INCLUDED_)

#if !defined(AFX_CHANNELMETADATA2_H__DE4DC7B3_DBAD_4BA3_B924_12B659E82905__INCLUDED_)
#define AFX_CHANNELMETADATA2_H__DE4DC7B3_DBAD_4BA3_B924_12B659E82905__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChannelMetadata2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChannelMetadata2 dialog

class CChannelMetadata2 : public CDialog
{
   COLORREF color;
   CBrush brush;
// Construction
public:
	CChannelMetadata2(CWnd* pParent = NULL);   // standard constructor
   void Refresh();
   void Save();
   BOOL CreateNewChannel(LPCTSTR tstrNewChnnel);
// Dialog Data
	//{{AFX_DATA(CChannelMetadata2)
	enum { IDD = IDD_CHANNELMETADATA2 };
	CEdit	m_editTitle;
	CString	m_csAutor;
	CString	m_csCopyright;
	CString	m_csDescription;
	CString	m_csLanguage;
	CString	m_csEMail;
	CString	m_csOwner;
	CString	m_csSubtitle;
	CString	m_csSummary;
	CString	m_csTitle;
	CString	m_csExplicit;
	CString	m_csKeyword;
	CString	m_csLink;
	CString	m_csWebmaster;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChannelMetadata2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChannelMetadata2)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANNELMETADATA2_H__DE4DC7B3_DBAD_4BA3_B924_12B659E82905__INCLUDED_)

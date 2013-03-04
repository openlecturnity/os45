#if !defined(AFX_METADATAPAGE_H__F16E7E4E_543D_41A2_94D4_673190BDC4AB__INCLUDED_)
#define AFX_METADATAPAGE_H__F16E7E4E_543D_41A2_94D4_673190BDC4AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MetadataPage.h : header file
//
#include "resource.h"
#include "Podcaster.h"

/////////////////////////////////////////////////////////////////////////////
// CMetadataPage dialog

class CMetadataPage : public CDialog
{
   COLORREF color;
   CBrush brush;
   CPodcasterApp *pApp;
// Construction
public:
	BOOL GetMetadata(CString csUrl, BOOL bDisplayError = TRUE);
	CMetadataPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMetadataPage)
	enum { IDD = IDD_METADATA_PAGE };
	CButton	m_btnChange;
	CString	m_csAuthor;
	CString	m_csCopyright;
	CString	m_csEmail;
	CString	m_csDescription;
	CString	m_csExplicit;
	CString	m_csKeywords;
	CString	m_csLanguage;
	CString	m_csLink;
	CString	m_csOwner;
	CString	m_csSubtitle;
	CString	m_csSummary;
	CString	m_csTitle;
	CString	m_csWebmaster;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMetadataPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMetadataPage)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METADATAPAGE_H__F16E7E4E_543D_41A2_94D4_673190BDC4AB__INCLUDED_)

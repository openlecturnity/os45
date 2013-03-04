#if !defined(AFX_CHANNELMETADATA_H__DAE64165_60B7_4A93_B287_18FB0F76E952__INCLUDED_)
#define AFX_CHANNELMETADATA_H__DAE64165_60B7_4A93_B287_18FB0F76E952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChannelMetadata.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChannelMetadata dialog
#include "ChannelMetadata2.h"

class CChannelMetadata : public CDialog
{
   void DrawLogo();
// Construction
public:
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

   CChannelMetadata2 dlg;
	CChannelMetadata(CWnd* pParent = NULL);   // standard constructor
   void OnOK2();
   void OnCancel2();

// Dialog Data
	//{{AFX_DATA(CChannelMetadata)
	enum { IDD = IDD_CHANNELMETADATA };
	CStatic	m_stcBarBottom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChannelMetadata)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChannelMetadata)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANNELMETADATA_H__DAE64165_60B7_4A93_B287_18FB0F76E952__INCLUDED_)

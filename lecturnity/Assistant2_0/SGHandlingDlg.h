#if !defined(AFX_SGHANDLINGDLG_H__2A1A0262_CBEB_4AFC_BC4B_64AE230FE661__INCLUDED_)
#define AFX_SGHANDLINGDLG_H__2A1A0262_CBEB_4AFC_BC4B_64AE230FE661__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SGHandlingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSGHandlingDlg dialog
#include "AssistantDoc.h"
#include "..\Studio\Resource.h"

class CSGHandlingDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSGHandlingDlg)

// Construction
public:
	CSGHandlingDlg();
	~CSGHandlingDlg();

   void Init(CAssistantDoc *pAssistantDoc);

// Dialog Data
	//{{AFX_DATA(CSGHandlingDlg)
	enum { IDD = IDD_DIALOG_SG_HANDLING };
	CButton	m_chkSuspendVideo;
	CButton	m_chkReduceAssistant;
	CButton	m_chkQuickCapture;
	CButton	m_chkMinimizeAssistant;
	CButton	m_chkDroppedFrames;
	CButton	m_chkDisplayAcceleration;
	CButton	m_chkClearAnnotation;
	CButton	m_chkBorderPanning;
	CButton	m_chkAlvaysOnTop;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSGHandlingDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSGHandlingDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
   CAssistantDoc *m_pAssistantDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SGHANDLINGDLG_H__2A1A0262_CBEB_4AFC_BC4B_64AE230FE661__INCLUDED_)

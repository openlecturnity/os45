#if !defined(AFX_SGHOTKEYSDLG_H__AF74D2E5_A789_4E6B_8F1F_9755E10773C8__INCLUDED_)
#define AFX_SGHOTKEYSDLG_H__AF74D2E5_A789_4E6B_8F1F_9755E10773C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SGHotkeysDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSGHotkeysDlg dialog
#include "AssistantDoc.h"
#include "..\Studio\Resource.h"

class CSGHotkeysDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSGHotkeysDlg)

// Construction
public:
	CSGHotkeysDlg();
	~CSGHotkeysDlg();

   void Init(CAssistantDoc *pAssistantDoc);

// Dialog Data
	//{{AFX_DATA(CSGHotkeysDlg)
	enum { IDD = IDD_DIALOG_SG_HOTKEYS };
	CComboBox	m_cmbStartStop;
	CComboBox	m_cmbReduceRestore;
	CComboBox	m_cmbMinimizeRestore;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSGHotkeysDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSGHotkeysDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
   CAssistantDoc *m_pAssistantDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SGHOTKEYSDLG_H__AF74D2E5_A789_4E6B_8F1F_9755E10773C8__INCLUDED_)

#if !defined(AFX_TRANSFERSETTINGS_H__BE075B8E_5528_44B8_9C38_C3925E72FFE9__INCLUDED_)
#define AFX_TRANSFERSETTINGS_H__BE075B8E_5528_44B8_9C38_C3925E72FFE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransferSettings.h : header file
//
#include "SettingsDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CTransferSettings dialog

class CTransferSettings : public CDialog
{
   CSettingsDlg dlg;
// Construction
public:
	CTransferSettings(CWnd* pParent = NULL);   // standard constructor
   CString m_csChannelName;
   CString m_csChannel;
   void OnOK2();
   void OnCancel2();

// Dialog Data
	//{{AFX_DATA(CTransferSettings)
	enum { IDD = IDD_SETTINGS_DIALOG2 };
	CStatic	m_stcBarBottom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransferSettings)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void DrawLogo();

  // Generated message map functions
	//{{AFX_MSG(CTransferSettings)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFERSETTINGS_H__BE075B8E_5528_44B8_9C38_C3925E72FFE9__INCLUDED_)

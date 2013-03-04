#if !defined(AFX_SGCODECDLG_H__8EAFDFDE_1E4B_40F0_89E7_22B035FF5C65__INCLUDED_)
#define AFX_SGCODECDLG_H__8EAFDFDE_1E4B_40F0_89E7_22B035FF5C65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SGCodecDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSGCodecDlg dialog

#include "AssistantDoc.h"
class CSGCodecDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSGCodecDlg)

// Construction
public:
	CSGCodecDlg();
	~CSGCodecDlg();

   void Init(CAssistantDoc *pAssistantDoc);

// Dialog Data
	//{{AFX_DATA(CSGCodecDlg)
	enum { IDD = IDD_DIALOG_SG_CODEC };
	CStatic	m_stFrames;
	CStatic	m_stDistance;
	CStatic	m_stDatarateKb;
	CStatic	m_stDatarate;
	CSpinButtonCtrl	m_spnQuality;
	CSliderCtrl	m_sldQuality;
	CEdit	m_edtQuality;
	CEdit	m_edtKeyframe;
	CEdit	m_edtDatarate;
	CComboBox	m_cmbSgCodec;
	CButton	m_chkKeyframe;
	CButton	m_chkDatarate;
	CButton	m_btnConfigure;
	CButton	m_btnAbout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSGCodecDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSGCodecDlg)
	virtual BOOL OnInitDialog();
   afx_msg void OnKickIdle();
	afx_msg void OnDeltaposSpinSgQuality(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderSgQuality(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckSgKeyframe();
	afx_msg void OnCheckSgDatarate();
	afx_msg void OnButtonSgCodecAbout();
	afx_msg void OnButtonSgConfgure();
	afx_msg void OnUpdateButtonCodecConfigure(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonCodecAbout(CCmdUI *pCmdUI);
	afx_msg void OnSelchangeComboSgCodec();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
   CAssistantDoc *m_pAssistantDoc;

   bool m_bHasAbout;
   bool m_bHasConfigure;

   bool m_bSupportsQuality;
   bool m_bSupportsKeyframes;
   bool m_bSupportsDatarate;

   
   CStringArray m_arCodecList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SGCODECDLG_H__8EAFDFDE_1E4B_40F0_89E7_22B035FF5C65__INCLUDED_)

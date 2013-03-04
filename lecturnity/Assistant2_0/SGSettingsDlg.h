#if !defined(AFX_SGSETTINGSDLG_H__C80FF5EC_41D5_4211_8C66_2C5ED10F2EE1__INCLUDED_)
#define AFX_SGSETTINGSDLG_H__C80FF5EC_41D5_4211_8C66_2C5ED10F2EE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SGSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSGSettingsDlg dialog
#include "AssistantDoc.h"
#include "..\Studio\Resource.h"

class CSGSettingsDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSGSettingsDlg)

// Construction
public:
	CSGSettingsDlg();
	~CSGSettingsDlg();

   void Init(CAssistantDoc *pAssistantDoc);

// Dialog Data
	//{{AFX_DATA(CSGSettingsDlg)
	enum { IDD = IDD_DIALOG_SG_SETTINGS };
	CStatic	m_stFixedWidth;
	CStatic	m_stAdaptiveWidth;
	CStatic	m_stAdaptiveHeight;
	CStatic	m_stFixedY;
	CStatic	m_stFixedX;
	CStatic	m_stFixedHeight;
	CSpinButtonCtrl	m_spnFrStatic;
	CSpinButtonCtrl	m_spnFrAdaptive;
	CEdit	m_edtFrStatic;
	CEdit	m_edtFrAdaptiveWidth;
	CEdit	m_edtFrAdaptiveHeight;
	CEdit	m_edtFrAdaptive;
	CEdit	m_edtFixedY;
	CEdit	m_edtFixedX;
	CEdit	m_edtFixedWidth;
	CEdit	m_edtFixedHeight;
	CButton	m_chkMouseVisual;
	CButton	m_chkMouseAcoustic;
	CButton	m_chkFixedRegion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSGSettingsDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSGSettingsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioFramerateStatic();
	afx_msg void OnRadioFramerateAdaptive();
	afx_msg void OnCheckFixedRegion();
	afx_msg void OnUpdateEditFrAdaptive();
	afx_msg void OnUpdateEditFrStatic();
   afx_msg void OnUpdateCheckFixedRegion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
   CAssistantDoc *m_pAssistantDoc;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SGSETTINGSDLG_H__C80FF5EC_41D5_4211_8C66_2C5ED10F2EE1__INCLUDED_)

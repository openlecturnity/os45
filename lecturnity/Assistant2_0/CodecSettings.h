#if !defined(AFX_CODECSETTINGS_H__D9705341_7385_4FC3_85A3_3496C57245C6__INCLUDED_)
#define AFX_CODECSETTINGS_H__D9705341_7385_4FC3_85A3_3496C57245C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CodecSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCodecSettings dialog

#include "..\Studio\Resource.h"

class CCodecSettings : public CPropertyPage
{
	DECLARE_DYNCREATE(CCodecSettings)

// Construction
public:
	CCodecSettings();
	~CCodecSettings();

   void Init(CAssistantDoc *pAssistantDoc);
   void UpdatePage();

// Dialog Data
	//{{AFX_DATA(CCodecSettings)
	enum { IDD = IDD_DIALOG_CODEC };
	CComboBox	m_cmbCodecs;
	CStatic	m_stKeyframeDistance;
	CStatic	m_stKeyframeFrames;
	CStatic	m_stDatarateTarget;
	CStatic	m_stDatarateKb;
	CEdit	m_edtDatarate;
	CEdit	m_edtKeyrame;
	CButton	m_chkKeyframe;
	CButton	m_chkDatarate;
	CSpinButtonCtrl	m_spnQuality;
	CSliderCtrl	m_sldQuality;
	CEdit	m_edtQuality;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCodecSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCodecSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinQuality(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderQuality(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckCodecKeyframe();
	afx_msg void OnCheckCodecDatarate();
   afx_msg void OnKickIdle();
   afx_msg void OnUpdateLblDistance(CCmdUI *pCmdUI);
   afx_msg void OnUpdateEdtDistance(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkFrames(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLblFrames(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkTargetDatarate(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLblTargetDatarate(CCmdUI *pCmdUI);
   afx_msg void OnUpdateEdtTargetDatarate(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLblKBS(CCmdUI *pCmdUI);
	afx_msg void OnChangeEditQuality();
	afx_msg void OnChangeEditKeyframe();
	afx_msg void OnChangeEditDatarate();
	afx_msg void OnSelchangeComboCodecs();
	afx_msg void OnButtonCodecAbout();
	afx_msg void OnUpdateButtonCodecAbout(CCmdUI *pCmdUI);
	afx_msg void OnButtonCodecConfigure();
	afx_msg void OnUpdateButtonCodecConfigure(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CAssistantDoc *m_pAssistantDoc;

   CStringArray m_arCodecs;
   UINT m_nCodecIndex;
   UINT m_nQuality;
   bool m_bUseKeyframes;
   UINT m_nKeyframeDistance;
   bool m_bUseDatarateAdaption;
   UINT m_nDatarate;
   bool m_bHasAbout;
   bool m_bHasConfigure;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODECSETTINGS_H__D9705341_7385_4FC3_85A3_3496C57245C6__INCLUDED_)

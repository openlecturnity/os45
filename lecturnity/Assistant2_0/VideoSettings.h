#if !defined(AFX_VIDEOSETTINGS_H__1CD6FDE5_380F_4BC2_8165_E105AAFB53E9__INCLUDED_)
#define AFX_VIDEOSETTINGS_H__1CD6FDE5_380F_4BC2_8165_E105AAFB53E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoSettings dialog

#include "AssistantDoc.h"
#include "..\Studio\Resource.h"

class CVideoSettings : public CPropertyPage
{
	DECLARE_DYNCREATE(CVideoSettings)

// Construction
public:
	CVideoSettings();
	~CVideoSettings();

   void Init(CAssistantDoc *pAssistantDoc);

// Dialog Data
	//{{AFX_DATA(CVideoSettings)
	enum { IDD = IDD_DIALOG_VIDEO };
	CComboBox	m_cmbFramerate;
	CComboBox	m_cmbDriverModel;
	CStatic	m_stResolution;
	CStatic	m_stColordepth;
	CEdit	m_edtAvOffset;
	CStatic	m_stAvMs;
	CButton	m_chkAvOffset;
	CButton	m_chkAltAVSync;
	CButton	m_chkVideoMonitor;
	CButton	m_chkRecordVideo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVideoSettings)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CVideoSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckRecordVideo();
	afx_msg void OnCheckVideoMonitor();
	afx_msg void OnCheckAvOffset();
	afx_msg void OnChangeEditAvOffset();
	afx_msg void OnCheckAlternativeAvSync();
	afx_msg void OnUpdateCheckAlternativeAvSync(CCmdUI *pCmdUI);
   afx_msg void OnKickIdle();
   afx_msg void OnUpdateChkRecordVideo(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkVideoMonitor(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkAlternativeAVSync(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkAVOffset(CCmdUI *pCmdUI);
   afx_msg void OnUpdateEdtAVOffset(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLblAVms(CCmdUI *pCmdUI);
   afx_msg void OnUpdateCmbFramerate(CCmdUI *pCmdUI);
   afx_msg void OnUpdateBtnWindowsDriver(CCmdUI *pCmdUI);
	afx_msg void OnSelchangeComboVideoDriver();
	afx_msg void OnButtonWdmDv();
   afx_msg void OnUpdateButtonWdmDv(CCmdUI *pCmdUI);
	afx_msg void OnButtonWdmFormat();
   afx_msg void OnUpdateButtonWdmFormat(CCmdUI *pCmdUI);
	afx_msg void OnButtonWdmSettings();
   afx_msg void OnUpdateButtonWdmSettings(CCmdUI *pCmdUI);
	afx_msg void OnButtonWdmSource();
   afx_msg void OnUpdateButtonWdmSource(CCmdUI *pCmdUI);
	afx_msg void OnAvOffset();
   afx_msg void OnUpdateResolutionSource(CCmdUI *pCmdUI);
   afx_msg void OnUpdateColorDepthSource(CCmdUI *pCmdUI);
	afx_msg void OnSelchangeComboVideoFramerate();
	afx_msg void OnEditchangeComboVideoFramerate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   void UpdatePage();
//   void EnableCodecPage(bool bEnable);
   void UpdateCodecPage();


private:
   CAssistantDoc *m_pAssistantDoc;

   bool m_bRecordVideo;
   bool m_bShowMonitor;
   bool m_bUseAlternativeSync;
   bool m_bSetAvOffset;
   int m_iAvOffset;
   UINT m_nXResolution;
   UINT m_nYResolution;
   UINT m_nBitPerColor;
   CArray<float, float> m_arFrameRates;
   UINT m_nFrameRateIndex;
   bool m_bUseVFW;
   CStringArray m_arVFWSources;
   UINT m_nVFWIndex;
   CStringArray m_arWDMSources;
   UINT m_nWDMIndex;

   HCURSOR m_cursorWait;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOSETTINGS_H__1CD6FDE5_380F_4BC2_8165_E105AAFB53E9__INCLUDED_)

#if !defined(AFX_AUDIOSETTINGS_H__1BF87552_4DE5_4D09_98F5_DD6BC35BA09C__INCLUDED_)
#define AFX_AUDIOSETTINGS_H__1BF87552_4DE5_4D09_98F5_DD6BC35BA09C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioSettings.h : header file
//

#include "..\Studio\Resource.h"

class CAssistantDoc;

/////////////////////////////////////////////////////////////////////////////
// CAudioSettings dialog

class CAudioSettings : public CPropertyPage
{
	DECLARE_DYNCREATE(CAudioSettings)

// Construction
public:
	void SetVolumeRange(int nMin=0, int nMax=100);
	int GetVolume();
	void SetVolume(int nValue);
	CAudioSettings();
	~CAudioSettings();

   void Init(CAssistantDoc *pAssistantDoc);
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAudioSettings)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAudioSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAudioDevice();
	afx_msg void OnSelchangeAudioSampleRate();
	afx_msg void OnSelchangeAudioSampleWidth();
	afx_msg void OnSelchangeAudioSource();
   afx_msg void OnUpdateAudioSource(CCmdUI *pCmdUI);
   afx_msg void OnUpdateAudioSlider(CCmdUI *pCmdUI);
   afx_msg void OnKickIdle();
	afx_msg void OnReleasedcaptureAudioSlider(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   
// Dialog Data
	//{{AFX_DATA(CAudioSettings)
	enum { IDD = IDD_DIALOG_AUDIO };
	CSliderCtrl	m_sldVolume;
	CComboBox	m_cmbAudioSource;
	CComboBox	m_cmbAudioDevice;
	CComboBox	m_cmbAudioSampleWidth;
	CComboBox	m_cmbAudioSampleRate;
	//}}AFX_DATA

   CAssistantDoc *m_pAssistantDoc;

   CStringArray m_arDevices;
   UINT m_nDevice;
   CUIntArray m_arSampleWidths;
   UINT m_nSampleWidth;
   CUIntArray m_arSampleRates;
   UINT m_nSampleRate;
   CStringArray m_arSources;
   UINT m_nSource;
   int m_nMinRange;
   int m_nMaxRange;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOSETTINGS_H__1BF87552_4DE5_4D09_98F5_DD6BC35BA09C__INCLUDED_)

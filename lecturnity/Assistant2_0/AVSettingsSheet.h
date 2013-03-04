#pragma once

#include "AssistantDoc.h"
// CAVSettingsSheet dialog

class CAVSettingsSheet : public CXTPPropertyPage/*CPropertyPage*/
{
	DECLARE_DYNAMIC(CAVSettingsSheet)

public:
	CAVSettingsSheet();
	virtual ~CAVSettingsSheet();

// Dialog Data
	enum { IDD = IDD_DIALOG_AV_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   afx_msg void OnKickIdle();
   //Audio messages begin
   afx_msg void OnSelchangeAudioDevice();
   afx_msg void OnSelchangeAudioSampleRate();
	afx_msg void OnSelchangeAudioSampleWidth();
	afx_msg void OnSelchangeAudioSource();
   afx_msg void OnUpdateAudioSource(CCmdUI *pCmdUI);
   afx_msg void OnUpdateAudioSlider(CCmdUI *pCmdUI);
	afx_msg void OnReleasedcaptureAudioSlider(NMHDR* pNMHDR, LRESULT* pResult);
   //Audio messages end
   //Video messages begin
   afx_msg void OnCheckRecordVideo();
	afx_msg void OnCheckVideoMonitor();
	afx_msg void OnCheckAvOffset();
	afx_msg void OnChangeEditAvOffset();
	afx_msg void OnCheckAlternativeAvSync();
	afx_msg void OnUpdateCheckAlternativeAvSync(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkRecordVideo(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkVideoMonitor(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkAlternativeAVSync(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkAVOffset(CCmdUI *pCmdUI);
   afx_msg void OnUpdateEdtAVOffset(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLblAVms(CCmdUI *pCmdUI);
   afx_msg void OnUpdateVideoLabels(CCmdUI *pCmdUI);
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
   //Video messages end
   //Codec messages begin
   afx_msg void OnDeltaposSpinQuality(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderQuality(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckCodecKeyframe();
	afx_msg void OnCheckCodecDatarate();
	afx_msg void OnChangeEditQuality();
	afx_msg void OnChangeEditKeyframe();
	afx_msg void OnChangeEditDatarate();
	afx_msg void OnSelchangeComboCodecs();
	afx_msg void OnButtonCodecAbout();
	afx_msg void OnButtonCodecConfigure();
   afx_msg void OnUpdateButtonCodecAbout(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonCodecConfigure(CCmdUI *pCmdUI);
   afx_msg void OnUpdateGroupSettings(CCmdUI *pCmdUI);
   afx_msg void OnUpdateStaticCodec(CCmdUI *pCmdUI);
   afx_msg void OnUpdateComboCodec(CCmdUI *pCmdUI);
   afx_msg void OnUpdateGroupQuality(CCmdUI *pCmdUI);
   afx_msg void OnUpdateSliderQuality(CCmdUI *pCmdUI);
   afx_msg void OnUpdateEdtQuality(CCmdUI *pCmdUI);
   afx_msg void OnUpdateSpinQuality(CCmdUI *pCmdUI);
   //
   afx_msg void OnUpdateLblDistance(CCmdUI *pCmdUI);
   afx_msg void OnUpdateEdtDistance(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkFrames(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLblFrames(CCmdUI *pCmdUI);
   afx_msg void OnUpdateChkTargetDatarate(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLblTargetDatarate(CCmdUI *pCmdUI);
   afx_msg void OnUpdateEdtTargetDatarate(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLblKBS(CCmdUI *pCmdUI);
   //
   //Codec messages end
	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
public:
   virtual void OnOK();
   virtual void OnCancel();
public:
   void Init(CAssistantDoc* pDocument);
   //Audio methods begin
   void SetVolumeRange(int nMin=0, int nMax=100);
	int GetVolume();
	void SetVolume(int nValue);
   //Audio methods end
protected:
   void LocalizeAudio();
   void LocalizeVideo();
   void LocalizeCodec();
   void InitAudio();
   void InitVideo();
   void InitCodec();
   void SaveSettingsAudio();
   void SaveSettingsVideo();
   void SaveSettingsCodec();
   void UpdateRecordVideoHint();
   CAssistantDoc *m_pAssistantDoc;
   //Audio variables begin
   CSliderCtrl	m_sldVolume;
	CComboBox	m_cmbAudioSource;
	CComboBox	m_cmbAudioDevice;
	CComboBox	m_cmbAudioSampleWidth;
	CComboBox	m_cmbAudioSampleRate;
   CStringArray m_arDevices;
   // Settings at initialization. See below why these were required:
   //   Normally the project settings should be save just on OK. But some values from dialog 
   //   are available just after device or source is set into the project (e.g. audio volume after the source is set).
   //   If Cancel is chosen these values will be restored.
   UINT m_nInitialDevice;
   UINT m_nInitialSource;
   // Settings changed withing dialog. If Cancel is chosen these values will be overriten.
   UINT m_nDevice;
   UINT m_nSource;

   CUIntArray m_arSampleWidths;
   UINT m_nSampleWidth;
   CUIntArray m_arSampleRates;
   UINT m_nSampleRate;
   CStringArray m_arSources;
   int m_nMinRange;
   int m_nMaxRange;
   //Audio variables end
   //Video variables begin
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
   void UpdateVideoPage();
   void UpdateCodecPage();
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
   //Video variables end
   //Codec variables begin
   void UpdatePageCodec();
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
    CStringArray m_arCodecs;
   UINT m_nCodecIndex;
   UINT m_nQuality;
   bool m_bUseKeyframes;
   UINT m_nKeyframeDistance;
   bool m_bUseDatarateAdaption;
   UINT m_nDatarate;
   bool m_bHasAbout;
   bool m_bHasConfigure;
   //Codec variables end
   CXTPPropertyPageStaticCaption m_caption[4];
};

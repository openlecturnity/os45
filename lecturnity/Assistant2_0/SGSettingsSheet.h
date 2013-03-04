#pragma once

//#include "SGSettings.h"
//#include "SGHandlingDlg.h"
#include "AssistantDoc.h"
// CSGSettingsSheet dialog

class CSGSettingsSheet : public CXTPPropertyPage
{
	DECLARE_DYNAMIC(CSGSettingsSheet)

public:
	CSGSettingsSheet();
	virtual ~CSGSettingsSheet();

// Dialog Data
	enum { IDD = IDD_DIALOG_SGB_SETTINGS };
   //General Variables begin
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
   //General Variables end
   //Handling Variables begin
   CButton	m_chkSuspendVideo;
	CButton	m_chkQuickCapture;
	CButton	m_chkDroppedFrames;
	CButton	m_chkDisplayAcceleration;
	CButton	m_chkClearAnnotation;
	CButton	m_chkBorderPanning;
   //Handling Variables end
   //Hotkeys Variables begin
   CComboBox	m_cmbStartStop;
   //Hotkeys Variables end
   //Codec Variables begin
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
   //Codec Variables end

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //CSGSettings m_sheetSG;
   //General Messages begin
   afx_msg void OnKickIdle();
   afx_msg void OnRadioFramerateStatic();
	afx_msg void OnRadioFramerateAdaptive();
	afx_msg void OnCheckFixedRegion();
	afx_msg void OnUpdateEditFrAdaptive();
	afx_msg void OnUpdateEditFrStatic();
   afx_msg void OnUpdateCheckFixedRegion();
   //General Messages end
   //Codec Messages begin
   afx_msg void OnDeltaposSpinSgQuality(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderSgQuality(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckSgKeyframe();
	afx_msg void OnCheckSgDatarate();
	afx_msg void OnButtonSgCodecAbout();
	afx_msg void OnButtonSgConfgure();
	afx_msg void OnUpdateButtonCodecConfigure(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonCodecAbout(CCmdUI *pCmdUI);
	afx_msg void OnSelchangeComboSgCodec();
   //Codec Messages End
	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnSetActive();
   virtual BOOL OnInitDialog();
public:
   virtual void OnOK();
   virtual void OnCancel();
public:
   void Init(CAssistantDoc* pDocument);
protected:
   CAssistantDoc *m_pAssistantDoc;
   void LocalizeGeneral();
   void LocalizeHandling();
   void LocalizeHotkeys();
   void LocalizeCodec();
   void InitGeneral();
   void InitHandling();
   void InitHotkeys();
   void InitCodec();
   void SaveSettingsGeneral();
   void SaveSettingsClickInformation();
   void SaveSettingsHandling();
   void SaveSettingsHotkeys();
   void SaveSettingsCodec();

   CXTPPropertyPageStaticCaption m_caption[5];
   //codec members begin
   bool m_bHasAbout;
   bool m_bHasConfigure;

   bool m_bSupportsQuality;
   bool m_bSupportsKeyframes;
   bool m_bSupportsDatarate;

   CStringArray m_arCodecList;
   // Save initial codec index. This value will be restored if cancel is pressed.
   //   The ASSISTANT::Project values are updated after codec combo box value is changed 
   //   and we need to access project values: hasSgAboutDialog_ and hasSgConfigureDialog_
   int m_nInitialCodexIndex;
   //codec members end
public:
    afx_msg void OnBnClickedStaticGbSgHotkeysStart2();
};

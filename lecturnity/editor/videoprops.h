#if !defined(AFX_VIDEOPROPS_H__98B52AEF_8821_4002_ACD1_71C9CF912ACF__INCLUDED_)
#define AFX_VIDEOPROPS_H__98B52AEF_8821_4002_ACD1_71C9CF912ACF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "avinfo.h"
#include "..\Studio\resource.h"
class CEditorDoc;
class CAviSettings;

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CVideoProps 

class CVideoProps : public CPropertyPage
{
	DECLARE_DYNCREATE(CVideoProps)

// Konstruktion
public:
	CVideoProps(DWORD dwId=IDD_VIDEO, CAviSettings *pAviSettings=NULL, const _TCHAR *tszClipSource=NULL, int nClipNr=0);
	~CVideoProps();

// Dialogfelddaten
	//{{AFX_DATA(CVideoProps)
	enum { IDD = IDD_VIDEO };
	CButton	m_btResetVideoSize;
	CStatic	m_offsetLabel;
	CStatic	m_msLabel;
	CEdit	m_offsetEdit;
	CButton	m_smartRecompressCheck;
	CButton	m_setDefaultButton;
	CButton	m_aboutButton;
	CButton	m_configureButton;
	CEdit	m_keyframesEdit;
	CButton	m_useKeyframesRadio;
	CStatic	m_qualityLabel;
	CSliderCtrl	m_qualitySlider;
	CEdit	m_frameRateEdit;
	CEdit	m_sizeYEdit;
	CEdit	m_sizeXEdit;
	CButton	m_keepAspectRadio;
	CComboBox	m_codecCombo;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CVideoProps)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CVideoProps)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeepAspectClick();
	afx_msg void OnSizeXLostFocus();
	afx_msg void OnSizeYLostFocus();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCodecChange();
	afx_msg void OnSetCodecDefault();
	afx_msg void OnConfigureCodec();
	afx_msg void OnAboutCodec();
	afx_msg void OnEditChange();
	afx_msg void OnUseKeyframesClick();
	afx_msg void OnFrameRateLostFocus();
	afx_msg void OnSmartRecompressClick();
	afx_msg void OnChangeOffset();
	afx_msg void OnOffsetLostFocus();
   afx_msg void OnResetVideoSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   CEditorDoc *m_pDoc;
   CAviSettings *m_pAviSettings;
   CString     m_csClipSource;
   CString     m_csCaption;
   bool        m_bHasClipSource;
   bool        m_isEnabled;

   LONGLONG    m_avgTimePerFrame100ns;
   // x / y
   double      m_initialAspectRatio;

   CSize       m_lastVideoSize;
   CODECINFO **m_plCodecList;
   int         m_codecListSize;
   DWORD       m_lastCodecSelected;
   bool        m_isFirstTime;

   AVInfo      m_avInfo;

protected:
   void ReadDefaultSettings();
   void PopulateCodecSettings();
   // Returns the index of the codec in the codec
   // list with the given FOURCC code if found,
   // otherwise -1.
   int  FindCodec(DWORD fcc);

   void UpdateCodecControls();
   int GetKeyframeRate();

   virtual void MySetModified(BOOL bChanged=TRUE);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_VIDEOPROPS_H__98B52AEF_8821_4002_ACD1_71C9CF912ACF__INCLUDED_

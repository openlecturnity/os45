#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "FloatSpinCtrl.h"
#include "..\Studio\resource.h"
#include "EditorDoc.h"
#include "InteractionButtonPreview.h"
#include "ButtonAppearanceSettings.h"
#include "InteractionAreaEx.h"

// CCalloutConfigDialog dialog

class CInteractiveCalloutConfigDialog : public CDialog {
    DECLARE_DYNAMIC(CInteractiveCalloutConfigDialog)

public:
    CInteractiveCalloutConfigDialog(CWnd* pParent = NULL);   // standard constructor
    virtual ~CInteractiveCalloutConfigDialog();

    // Dialog Data
    enum { IDD = IDD_INTERACTIVECALLOUT_CONFIG_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
    CSliderCtrl m_sldTime;
    CSliderCtrl m_sldPeriod;
    CEdit m_edtTime;
    CEdit m_edtPeriod;
    CFloatSpinCtrl m_spnTime;
    CFloatSpinCtrl m_spnPeriod;

	CEdit	m_wndButtonText;
    CComboBox	m_wndChoosebuttonTypes;
	CInteractionButtonPreview m_wndPreview[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
    
public:
    virtual BOOL OnInitDialog();

    //void Init(int iDisplayTime, int iDisplayPeriod, int iCurrentMs, int iTotalLen);
    void Init(int iDisplayTime, int iDisplayPeriod, CEditorDoc *pEditorDoc, CInteractionAreaEx *pInteraction);
    void GetDisplayParameters(int &iDisplayTime, int &iPeriod);
	void SetUserEntries(CRect &rcArea, CRect &rcAreaButton, CInteractionAreaEx *pInteraction);
private:
    int m_iDisplayTime;
    int m_iDisplayPeriod;
    int m_iCurrentMs;
    int m_iTotalLen;
    CEditorDoc *m_pEditorDoc;
    int m_iActivePage;
    void SetCurrentPageInfo(int iDisplayTime);
    void SetPeriod(int iDisplayTime);
	void CalculateButtonArea(CRect &rcArea, CRect &rcAreaButton);
    bool m_bInitialPeriod;
    afx_msg void OnDeltaposCalloutSpinTime(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposCalloutSpinPeriod(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawCalloutSliderPeriod(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedCalloutButtonCurrentTime();
protected:
    CStatic m_lblPageName;
    CStatic m_lblPageStart;
    CStatic m_lblPageEnd;
    CStatic m_lblMaxPeriod;
    CStatic m_lblPageLength;

	CString m_csButtonText;
    CRect m_rcArea;
	CButtonAppearanceSettings *m_pButtonVisualSettings;
	CInteractionAreaEx *m_pInteraction;

	afx_msg void OnChangeButtonText();
	afx_msg void OnUpdateEdit(CCmdUI* pCmdUI);
    afx_msg void OnSelchangeButtonTypes();
	afx_msg void OnEdit();
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnEnUpdateCalloutEditTime();
    afx_msg void OnEnUpdateCalloutEditPeriod();
    afx_msg void OnNMReleasedcaptureCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnBnClickedIbuttonResetAll();
};

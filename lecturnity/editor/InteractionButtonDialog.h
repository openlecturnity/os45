#if !defined(AFX_INTERACTIONBUTTONDIALOG_H__61B8E789_1B38_45FA_B033_6E95168768A0__INCLUDED_)
#define AFX_INTERACTIONBUTTONDIALOG_H__61B8E789_1B38_45FA_B033_6E95168768A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageButtonWithStyle.h"
#include "InteractionAreaEx.h"
#include "InteractionButtonPreview.h"
#include "FloatSpinCtrl.h"
#include "ScrollHelper.h"
#include "..\Studio\resource.h"
#include "ButtonAppearanceSettings.h"
#include "afxwin.h"
#include "afxcmn.h"



/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInteractionButtonDialog 

class CInteractionButtonDialog : public CDialog {
public:

// Konstruktion
    CInteractionButtonDialog(CWnd* pParent = NULL);   // default constructor
    ~CInteractionButtonDialog();   // default deconstructor

    void Init(CInteractionAreaEx *pInteractionButton, CRect &rcArea, CEditorProject *pProject);

// Dialogfelddaten
    //{{AFX_DATA(CInteractionButtonDialog)
    enum { IDD = IDD_INTERACTION_BUTTON };
    CComboBox	m_wndMouseClickAction;
    CEdit	m_wndButtonText;
    CComboBox	m_wndChoosebuttonTypes;
    CImageButtonWithStyle	m_btnMouseClickIntern;
    CInteractionButtonPreview m_wndPreview[CButtonAppearanceSettings::ACTION_EVENT_COUNT]; // same value as CButtonAppearanceSettings::ACTION_EVENT_COUNT
    BOOL	m_bSetStopmark;
    BOOL	m_bShowMouseCursor;
    //}}AFX_DATA


// Überschreibungen
    // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CInteractionButtonDialog)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

public:  
    BOOL ContinueModal();

private:
    void CreateSlidePopup(int nButtonId, int nEditId);
    void CreateTargetmarkPopup(int nButtonId, int nEditId);
    void ExtractActionParameters();
    void SetActionText(CMouseAction *pMouseAction, int nEditId);
    void CalculateButtonArea(CRect &rcArea);

public:
    void SetUserEntries(CRect &rcArea, CInteractionAreaEx *pInteraction);
    static void Clean();
    virtual void OnOK();
    void OnBtnClickOK();

// Implementierung
protected:

    // Generierte Nachrichtenzuordnungsfunktionen
    //{{AFX_MSG(CInteractionButtonDialog)
    virtual BOOL OnInitDialog();
    afx_msg void OnIdleUpdateUIMessage(); // needed for ON_UPDATE_COMMAND_UI
    afx_msg void OnChangeButtonText();
    afx_msg void OnPopupIntern();
    afx_msg void OnPopupExtern();
    afx_msg void OnUpdatePopupExtern(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePopupIntern(CCmdUI* pCmdUI);
    afx_msg void OnUpdateActivityButtons(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSetStopMark(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUI);
    afx_msg void OnSelchangeButtonTypes();
    afx_msg void OnSelchangeMousedownAction();
    
    afx_msg void OnEdit();
    afx_msg void OnUpdateCustomTimeSettings(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCustomTimeSettings2(CCmdUI* pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:

    CInteractionAreaEx *m_pInteractionButton;
    CEditorProject *m_pEditorProject;

    HBITMAP	m_hBitmap;
    CMenu m_wndMenu;

    // Parameters for interaction
    AreaActionTypeId m_nActionTypeId;
    AreaActionTypeId m_nNextActionTypeId;
    UINT m_nMarkOrPageId;
    CString m_csActionPath;
    AreaPeriodId m_visualityPeriodId;
    AreaPeriodId m_activityPeriodId;

    CString m_csButtonText;
    CRect m_rcArea;

    // Once this is changed all new buttons created will have these new values.
    static CButtonAppearanceSettings *s_pButtonVisualSettings;

    void SetPeriod(int iDisplayTime);
    bool m_bInitialPeriod;
    int m_iCustomStartTime;
    int m_iCustomPeriod;
    bool m_bIsInitialized;
    CSize m_szInitial;
    CScrollHelper *m_pScrollHelper;

    void SetPeriodA(int iDisplayTime);
    bool m_bInitialPeriodA;
    int m_iCustomStartTimeA;
    int m_iCustomPeriodA;

    int m_iButtonVOffset;
    int m_iBarVOffset;
    int m_iBackVOffset;
    void RepositionControls();

private:
    CButton m_cbStartReplay;
    CButton m_cbStopReplay;
    afx_msg void OnBnClickedActionStopReplay();
    afx_msg void OnBnClickedActionStartReplay();
protected:
    CSliderCtrl m_sldTime;
    CSliderCtrl m_sldPeriod;
    CEdit m_edtTime;
    CEdit m_edtPeriod;
    CFloatSpinCtrl m_spnTime;
    CFloatSpinCtrl m_spnPeriod;
    CStatic m_lblMaxPeriod;
public:
    afx_msg void OnNMCustomdrawCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposCalloutSpinTime(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnUpdateCalloutEditTime();
    afx_msg void OnNMCustomdrawCalloutSliderPeriod(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposCalloutSpinPeriod(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnUpdateCalloutEditPeriod();
    afx_msg void OnNMReleasedcaptureCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
public:
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
public:
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
    CStatic m_stBottom;
public:
    CButton m_btnOK;
public:
    CButton m_btnCancel;
protected:
    CSliderCtrl m_sldTimeA;
    CSliderCtrl m_sldPeriodA;
    CEdit m_edtTimeA;
    CEdit m_edtPeriodA;
    CFloatSpinCtrl m_spnTimeA;
    CFloatSpinCtrl m_spnPeriodA;
    CStatic m_lblMaxPeriodA;
public:
    afx_msg void OnNMCustomdrawCalloutSliderTime2(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnDeltaposCalloutSpinTime2(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnEnUpdateCalloutEditTime2();
public:
    afx_msg void OnNMCustomdrawCalloutSliderPeriod2(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnDeltaposCalloutSpinPeriod2(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnEnUpdateCalloutEditPeriod2();
public:
    afx_msg void OnNMReleasedcaptureCalloutSliderTime2(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnBnClickedIbuttonResetAll();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_INTERACTIONBUTTONDIALOG_H__61B8E789_1B38_45FA_B033_6E95168768A0__INCLUDED_

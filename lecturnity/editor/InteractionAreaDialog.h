#if !defined(AFX_INTERACTIONAREADIALOG_H__57C260E9_3001_4D64_BD7A_FBBBE7260324__INCLUDED_)
#define AFX_INTERACTIONAREADIALOG_H__57C260E9_3001_4D64_BD7A_FBBBE7260324__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ImageButtonWithStyle.h"
#include "InteractionAreaEx.h"
#include "FloatSpinCtrl.h"
#include "ScrollHelper.h"
#include "..\Studio\resource.h"
#include "afxwin.h"
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInteractionAreaDialog 

class CInteractionAreaDialog : public CDialog
{
// Konstruktion
public:
    CInteractionAreaDialog(CWnd* pParent = NULL);   // default contructor
    ~CInteractionAreaDialog();                      // default destructor

    void Init(CInteractionAreaEx *pInteractionArea, CEditorDoc *pEditorDoc);
    virtual void OnOK();
    void OnBtnClickOK();

// Dialogfelddaten
	//{{AFX_DATA(CInteractionAreaDialog)
    enum { IDD = IDD_INTERACTION_AREA };
    CImageButtonWithStyle m_btnActivationIntern;
    CImageButtonWithStyle m_btnMouseDownIntern;
    CImageButtonWithStyle m_btnMouseUpIntern;
    CImageButtonWithStyle m_btnMouseEnterIntern;
    CImageButtonWithStyle m_btnMouseLeaveIntern;
    CComboBox	m_wndClickAction;
    CComboBox	m_wndDownAction;
    CComboBox	m_wndUpAction;
    CComboBox	m_wndEnterAction;
    CComboBox	m_wndLeaveAction;
    BOOL	m_bSetStopmark;
    BOOL	m_bShowMouseCursor;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CInteractionAreaDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

   /* TODO NI GG */
 
// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CInteractionAreaDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnIdleUpdateUIMessage(); // needed for ON_UPDATE_COMMAND_UI
	afx_msg void OnPopupActivationExtern();
	afx_msg void OnPopupIntern();
    afx_msg void OnPopupExtern();
    afx_msg void OnUpdatePopupExtern(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePopupIntern(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUI);
    afx_msg void OnSelchangeAreaActivationAction();
    afx_msg void OnSelchangeAreaMousedownAction();
    afx_msg void OnSelchangeAreaMouseupAction();
    afx_msg void OnSelchangeAreaMouseinAction();
    afx_msg void OnSelchangeAreaMouseoutAction();
    afx_msg void OnBnClickedActivStartReplay();
    afx_msg void OnBnClickedActivStopReplay();
    afx_msg void OnBnClickedMouseupStartReplay();
    afx_msg void OnBnClickedMouseupStopReplay();
    afx_msg void OnBnClickedMousedownStopReplay();
    afx_msg void OnBnClickedMousedownStartReplay();
    afx_msg void OnBnClickedMouseinStartReplay();
    afx_msg void OnBnClickedMouseinStopReplay();
    afx_msg void OnBnClickedMouseoutStartReplay();
    afx_msg void OnBnClickedMouseoutStopReplay();
    afx_msg void OnUpdateCustomTimeSettings(CCmdUI* pCmdUI);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    void CreateSlidePopup(int nAction, int nButtonId, int nEditId);
    void CreateTargetmarkPopup(int nAction, int nButtonId, int nEditId);
    void ExtractActionParameters(int nAction);
    void SetActionText(CMouseAction *pMouseAction, int nEditId);
    void UpdateMouseAction(int actionId, UINT nControlID, CMouseAction *pMouseAction, CComboBox *pComboBoxWnd, CButton *pStartCheckBox, CButton *pStopCheckBox);
    void UpdateMouseAction(CComboBox *pComboBoxWnd, CButton *pStartCheckBox, CButton *pStopCheckBox);
    void UpdateControlActions(int nAction, UINT nControlID, CButton *pStartReplayCheckBox, CButton *pStopReplayCheckBox);

public:
    void SetUserEntries(CRect &rcArea, CInteractionAreaEx *pInteraction);

public:  
    BOOL ContinueModal();

private:
    enum
    {
        ACTION_ACTIVATION = 0,
        ACTION_MOUSEDOWN = 1,
        ACTION_MOUSEUP = 2,
        ACTION_MOUSEIN = 3,
        ACTION_MOUSEOUT = 4,
        ACTION_COUNT = 5
    };

    CInteractionAreaEx *m_pInteractionArea;
    CEditorDoc *m_pEditorDoc;

    HBITMAP	m_hBitmap;
    CMenu m_wndMenu;

    // Parameters for interaction
    AreaActionTypeId m_nActionTypeId[ACTION_COUNT];
    AreaActionTypeId m_nNextActionTypeId[ACTION_COUNT];
    UINT m_nMarkOrPageId[ACTION_COUNT];
    CString m_csActionPath[ACTION_COUNT];
    AreaPeriodId m_periodId;

    void SetPeriod(int iDisplayTime);
    bool m_bInitialPeriod;
    int m_iCustomStartTime;
    int m_iCustomPeriod;
    bool m_bIsInitialized;
    CSize m_szInitial;
    CScrollHelper *m_pScrollHelper;
    void RepositionControls();

    int m_iButtonVOffset;
    int m_iBarVOffset;
    int m_iBackVOffset;

    CButton m_cbActivStartReplay;
    CButton m_cbActivStopReplay;
    CButton m_cbMouseupStopReplay;
    CButton m_cbMouseupStartReplay;
    CButton m_cbMousedownStopReplay;
    CButton m_cbMousedownStartReplay;
    CButton m_cbMouseinStartReplay;
    CButton m_cbMouseinStopReplay;
    CButton m_cbMouseoutStartReplay;
    CButton m_cbMouseoutStopReplay;
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
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
public:
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
    CStatic m_stBottom;
public:
    CButton m_btnOK;
public:
    CButton m_btnCancel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_INTERACTIONAREADIALOG_H__57C260E9_3001_4D64_BD7A_FBBBE7260324__INCLUDED_

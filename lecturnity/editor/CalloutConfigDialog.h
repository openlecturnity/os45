#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "FloatSpinCtrl.h"
#include "..\Studio\resource.h"
#include "EditorDoc.h"

// CCalloutConfigDialog dialog

class CCalloutConfigDialog : public CDialog {
    DECLARE_DYNAMIC(CCalloutConfigDialog)

public:
    CCalloutConfigDialog(CWnd* pParent = NULL);   // standard constructor
    virtual ~CCalloutConfigDialog();

    // Dialog Data
    enum { IDD = IDD_CALLOUT_CONFIG_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
    CSliderCtrl m_sldTime;
    CSliderCtrl m_sldPeriod;
    CEdit m_edtTime;
    CEdit m_edtPeriod;
    CFloatSpinCtrl m_spnTime;
    CFloatSpinCtrl m_spnPeriod;
public:
    virtual BOOL OnInitDialog();

    void Init(int iDisplayTime, int iDisplayPeriod, int iCurrentMs, int iTotalLen);
    void Init(int iDisplayTime, int iDisplayPeriod, CEditorDoc *pEditorDoc);
    void GetDisplayParameters(int &iDisplayTime, int &iPeriod);
private:
    int m_iDisplayTime;
    int m_iDisplayPeriod;
    int m_iCurrentMs;
    int m_iTotalLen;
    CEditorDoc *m_pEditorDoc;
    int m_iActivePage;
    void SetCurrentPageInfo(int iDisplayTime);
    void SetPeriod(int iDisplayTime);
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
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnEnUpdateCalloutEditTime();
    afx_msg void OnEnUpdateCalloutEditPeriod();
    afx_msg void OnNMReleasedcaptureCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult);
};

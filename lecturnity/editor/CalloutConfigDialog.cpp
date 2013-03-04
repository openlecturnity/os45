// CalloutConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CalloutConfigDialog.h"
#include "PageStream.h"
#include "MfcUtils.h"

// CCalloutConfigDialog dialog

IMPLEMENT_DYNAMIC(CCalloutConfigDialog, CDialog)

CCalloutConfigDialog::CCalloutConfigDialog(CWnd* pParent) : CDialog(CCalloutConfigDialog::IDD, pParent) {
    m_iDisplayTime = -1;
    m_iDisplayPeriod = -1;
    m_iCurrentMs = -1;
    m_iTotalLen = -1;
    m_iActivePage = -1;
    m_bInitialPeriod = false;
}

CCalloutConfigDialog::~CCalloutConfigDialog() {
}

void CCalloutConfigDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CALLOUT_SLIDER_TIME, m_sldTime);
    DDX_Control(pDX, IDC_CALLOUT_SLIDER_PERIOD, m_sldPeriod);
    DDX_Control(pDX, IDC_CALLOUT_EDIT_TIME, m_edtTime);
    DDX_Control(pDX, IDC_CALLOUT_EDIT_PERIOD, m_edtPeriod);
    DDX_Control(pDX, IDC_CALLOUT_SPIN_TIME, m_spnTime);
    DDX_Control(pDX, IDC_CALLOUT_SPIN_PERIOD, m_spnPeriod);
    DDX_Control(pDX, IDC_CALLOUT_LBL_PNVALUE, m_lblPageName);
    DDX_Control(pDX, IDC_CALLOUT_LBL_PSVALUE, m_lblPageStart);
    DDX_Control(pDX, IDC_CALLOUT_LBL_PEVALUE, m_lblPageEnd);
    DDX_Control(pDX, IDC_CALLOUT_LBL_MAX_PERIOD, m_lblMaxPeriod);
    DDX_Control(pDX, IDC_CALLOUT_LBL_PLVALUE, m_lblPageLength);
}

BEGIN_MESSAGE_MAP(CCalloutConfigDialog, CDialog)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_TIME, &CCalloutConfigDialog::OnDeltaposCalloutSpinTime)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_TIME, &CCalloutConfigDialog::OnNMCustomdrawCalloutSliderTime)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_PERIOD, &CCalloutConfigDialog::OnDeltaposCalloutSpinPeriod)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_PERIOD, &CCalloutConfigDialog::OnNMCustomdrawCalloutSliderPeriod)
    ON_BN_CLICKED(IDC_CALLOUT_BUTTON_CURRENT_TIME, &CCalloutConfigDialog::OnBnClickedCalloutButtonCurrentTime)
    ON_BN_CLICKED(IDOK, &CCalloutConfigDialog::OnBnClickedOk)
    //ON_EN_CHANGE(IDC_CALLOUT_EDIT_TIME, &CCalloutConfigDialog::OnEnChangeCalloutEditTime)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_TIME, &CCalloutConfigDialog::OnEnUpdateCalloutEditTime)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_PERIOD, &CCalloutConfigDialog::OnEnUpdateCalloutEditPeriod)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_CALLOUT_SLIDER_TIME, &CCalloutConfigDialog::OnNMReleasedcaptureCalloutSliderTime)
END_MESSAGE_MAP()

// CCalloutConfigDialog message handlers

BOOL CCalloutConfigDialog::OnInitDialog() {
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    /*CString csText;
    csText.Format(_T("%.3f"), (float)m_iCurrentMs/1000);
    m_edtTime.SetWindowText(csText);

    csText.Format(_T("%.3f"), (float)m_iDisplayPeriod/1000);
    m_edtPeriod.SetWindowText(csText);*/
    DWORD dwIds[] = {IDC_CALLOUT_LBL_PAGE_NAME,
        IDC_CALLOUT_LBL_PAGE_START,
        IDC_CALLOUT_LBL_PAGE_END,
        IDC_CALLOUT_LBL_DTIME,
        IDC_CALLOUT_LBL_DPERIOD,
        IDC_CALLOUT_TIME_SEC,          
        IDC_CALLOUT_PERIOD_SEC,         
        IDC_CALLOUT_BUTTON_CURRENT_TIME,
        IDC_CALLOUT_GROUP,
        IDC_CALLOUT_LBL_PAGE_LENGHT,
        IDOK,
        IDCANCEL,
        -1};
    MfcUtils::Localize(this, dwIds);

    /*CString csCallout;
    csCallout.LoadString(ID_GRAPHICALOBJECT_CALLOUT);
    CString csTextObject;
    csTextObject.LoadString(ID_GRAPHICALOBJECT_TEXT);
    CString csDisplay;
    csDisplay.LoadString(IDC_CALLOUT_LBL_DTIME);*/
    CString csTitle;// = csCallout + _T("\\")+ csTextObject + _T(" ") + csDisplay;
    csTitle.LoadString(IDS_GO_CONFIG_DIALOG_TITLE);
    SetWindowText(csTitle);
    m_spnTime.SetRangeAndDelta(0, m_iTotalLen / 1000.0, 0.001);
    m_spnTime.SetBuddy(&m_edtTime);
    m_spnTime.SetPos(m_iDisplayTime / 1000.0);
    m_spnTime.SetDecimalPlaces(3);

    m_sldTime.SetRange(0, m_iTotalLen);
    m_sldTime.SetTicFreq(1);
    m_sldTime.SetPos(m_iDisplayTime);

    m_spnPeriod.SetBuddy(&m_edtPeriod);
    m_spnPeriod.SetRangeAndDelta(0.01,  m_iTotalLen / 1000.0, 0.001);
    m_sldPeriod.SetRange(10, m_iTotalLen);
    m_sldPeriod.SetTicFreq(1);
    m_bInitialPeriod = true;
    SetPeriod(m_iDisplayTime);
    m_bInitialPeriod = false;

    SetCurrentPageInfo(m_iCurrentMs);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CCalloutConfigDialog::Init(int iDisplayTime, int iDisplayPeriod, int iCurrentMs, int iTotalLen) {
    m_iDisplayTime = iDisplayTime;
    m_iDisplayPeriod = iDisplayPeriod - 1;
    m_iCurrentMs = iCurrentMs;
    m_iTotalLen = iTotalLen;
}

void CCalloutConfigDialog::Init(int iDisplayTime, int iDisplayPeriod, CEditorDoc *pEditorDoc) {
    m_iDisplayTime = iDisplayTime;
    m_iDisplayPeriod = iDisplayPeriod - 1;
    m_iCurrentMs = pEditorDoc->m_curPosMs;
    m_iTotalLen = pEditorDoc->m_docLengthMs;
    m_pEditorDoc = pEditorDoc;
}

void CCalloutConfigDialog::SetCurrentPageInfo(int iDisplayTime) {
    int iActivePage = m_pEditorDoc->project.GetActivePage(iDisplayTime);
    if(iActivePage != m_iActivePage) {
        m_iActivePage = iActivePage;
    } else {
        return;
    }
    UINT nStartMs, nEndMs;
    CString csSeconds;
    csSeconds.LoadString(IDC_CALLOUT_TIME_SEC);
    m_pEditorDoc->project.GetPagePeriod(iDisplayTime, nStartMs, nEndMs);
    CString csLblText;
    csLblText.Format(_T("%.3f %s"), nStartMs / 1000.0, csSeconds);
    m_lblPageStart.SetWindowText(csLblText);
    csLblText.Format(_T("%.3f %s"), nEndMs / 1000.0, csSeconds);
    m_lblPageEnd.SetWindowText(csLblText);
    csLblText.Format(_T("%.3f %s"), (nEndMs - nStartMs) / 1000.0, csSeconds);
    m_lblPageLength.SetWindowText(csLblText);

    CArray<CPage*, CPage*> caPages;
    m_pEditorDoc->project.GetPages(caPages, iDisplayTime , iDisplayTime + 1);
    if(caPages.GetCount() > 0) {
        CString csTitle = caPages.GetAt(0)->GetTitle();
        m_lblPageName.SetWindowText(csTitle);
    }
}

void CCalloutConfigDialog::SetPeriod(int iDisplayTime) {
    int iRemainingPeriod = m_iTotalLen - iDisplayTime + 1;
    UINT nStartMs, nEndMs;
    m_pEditorDoc->project.GetPagePeriod(iDisplayTime, nStartMs, nEndMs);

    //m_spnPeriod.SetRangeAndDelta(0,  iRemainingPeriod / 1000.0, 0.001);
    int iCurrentPageLength = nEndMs - nStartMs;
    if(!m_bInitialPeriod) {
        m_iDisplayPeriod = iCurrentPageLength;
    }
    if(iRemainingPeriod < m_iDisplayPeriod) {
        m_iDisplayPeriod = iRemainingPeriod;
    }
    //m_iDisplayPeriod++;
    m_spnPeriod.SetPos(m_iDisplayPeriod / 1000.0);
    m_spnPeriod.SetDecimalPlaces(3);

    // m_sldPeriod.SetRange(0, iRemainingPeriod);
    // m_sldPeriod.SetTicFreq(1);
    m_sldPeriod.SetPos(m_iDisplayPeriod);
    CString csMaxPeriod;
    csMaxPeriod.Format(_T("%.3f"), m_iTotalLen / 1000.0);
    m_lblMaxPeriod.SetWindowText(csMaxPeriod);
}

void CCalloutConfigDialog::OnDeltaposCalloutSpinTime(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
    float fPos = m_spnTime.GetPos();
    int iSldPos = fPos * 1000;
    m_sldTime.SetPos(iSldPos);
    SetCurrentPageInfo(iSldPos);
    SetPeriod(iSldPos);
    *pResult = 0;
}

void CCalloutConfigDialog::OnNMCustomdrawCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    int iPos = m_sldTime.GetPos();
    m_spnTime.SetPos(iPos / 1000.0);
    SetCurrentPageInfo(iPos);
    /*SetPeriod(iPos);*/
    *pResult = 0;
}

void CCalloutConfigDialog::OnDeltaposCalloutSpinPeriod(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    float fPos = m_spnPeriod.GetPos();
    int iSldPos = fPos * 1000;
    m_sldPeriod.SetPos(iSldPos);
    m_iDisplayPeriod = iSldPos;
    *pResult = 0;
}

void CCalloutConfigDialog::OnNMCustomdrawCalloutSliderPeriod(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    int iPos = m_sldPeriod.GetPos();
    m_spnPeriod.SetPos(iPos / 1000.0);
    m_iDisplayPeriod = iPos;
    *pResult = 0;
}

void CCalloutConfigDialog::OnBnClickedCalloutButtonCurrentTime() {
    m_spnTime.SetPos(m_iCurrentMs / 1000.0);
    m_sldTime.SetPos(m_iCurrentMs);
}

void CCalloutConfigDialog::GetDisplayParameters(int &iDisplayTime, int &iPeriod) {
    iDisplayTime = m_iDisplayTime;
    iPeriod = m_iDisplayPeriod;
}

void CCalloutConfigDialog::OnBnClickedOk() {
    m_iDisplayTime = m_spnTime.GetPos() * 1000;//m_sldTime.GetPos();
    m_iDisplayPeriod = m_spnPeriod.GetPos() * 1000;//m_sldPeriod.GetPos();
    if (m_iDisplayPeriod > m_iTotalLen - m_iDisplayTime) {
        m_iDisplayPeriod = m_iTotalLen - m_iDisplayTime;
    }
    OnOK();
}

void CCalloutConfigDialog::OnEnUpdateCalloutEditTime() {
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function to send the EM_SETEVENTMASK message to the control
    // with the ENM_UPDATE flag ORed into the lParam mask.

    int iInitialPos = m_sldTime.GetPos();
    int iCursorPos = m_edtTime.GetSel();
    double fInitValue = m_spnTime.GetPos();

    int iChangedPos = fInitValue*1000;
    m_sldTime.SetPos(iChangedPos);


    if(iInitialPos != iChangedPos) {
        m_edtTime.SetSel(iCursorPos + 1, iCursorPos + 1);
    }
}

void CCalloutConfigDialog::OnEnUpdateCalloutEditPeriod() {
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function to send the EM_SETEVENTMASK message to the control
    // with the ENM_UPDATE flag ORed into the lParam mask.

    int iInitialPos = m_sldPeriod.GetPos();
    int iCursorPos = m_edtPeriod .GetSel();
    double fInitValue = m_spnPeriod.GetPos();

    int iChangedPos = fInitValue*1000;
    m_sldPeriod.SetPos(iChangedPos);


    if(iInitialPos != iChangedPos) {
        m_edtPeriod.SetSel(iCursorPos + 1, iCursorPos + 1);
    }
}

void CCalloutConfigDialog::OnNMReleasedcaptureCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult) {
    int iPos = m_sldTime.GetPos();
    /*m_spnTime.SetPos(iPos / 1000.0);
    SetCurrentPageInfo(iPos);*/
    SetPeriod(iPos);
    *pResult = 0;
}

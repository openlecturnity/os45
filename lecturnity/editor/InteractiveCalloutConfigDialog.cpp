// CalloutConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "InteractiveCalloutConfigDialog.h"
#include "PageStream.h"
#include "MfcUtils.h"
#include "EditInteractionDialog.h"
#include "WhiteboardView.h"

// CCalloutConfigDialog dialog

IMPLEMENT_DYNAMIC(CInteractiveCalloutConfigDialog, CDialog)

CInteractiveCalloutConfigDialog::CInteractiveCalloutConfigDialog(CWnd* pParent) : CDialog(CInteractiveCalloutConfigDialog::IDD, pParent) {
    m_iDisplayTime = -1;
    m_iDisplayPeriod = -1;
    m_iCurrentMs = -1;
    m_iTotalLen = -1;
    m_iActivePage = -1;
    m_bInitialPeriod = false;
	m_pButtonVisualSettings = new CButtonAppearanceSettings(true);
}

CInteractiveCalloutConfigDialog::~CInteractiveCalloutConfigDialog() {
	SAFE_DELETE(m_pButtonVisualSettings);
	SAFE_DELETE(m_pInteraction);
}

void CInteractiveCalloutConfigDialog::DoDataExchange(CDataExchange* pDX) {
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

	DDX_Control(pDX, IDC_BUTTON_TEXT, m_wndButtonText);
	DDX_Control(pDX, IDC_CHOOSE_BUTTON_TYPES, m_wndChoosebuttonTypes);
	DDX_Control(pDX, IDC_PREVIEW_MOUSEOVER, m_wndPreview[CButtonAppearanceSettings::ACTION_OVER]);
    DDX_Control(pDX, IDC_PREVIEW_NORMAL, m_wndPreview[CButtonAppearanceSettings::ACTION_NORMAL]);
    DDX_Control(pDX, IDC_PREVIEW_PRESSED, m_wndPreview[CButtonAppearanceSettings::ACTION_PRESSED]);
    DDX_Control(pDX, IDC_PREVIEW_PASSIV, m_wndPreview[CButtonAppearanceSettings::ACTION_PASSIV]);
}

BEGIN_MESSAGE_MAP(CInteractiveCalloutConfigDialog, CDialog)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_TIME, &CInteractiveCalloutConfigDialog::OnDeltaposCalloutSpinTime)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_TIME, &CInteractiveCalloutConfigDialog::OnNMCustomdrawCalloutSliderTime)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_PERIOD, &CInteractiveCalloutConfigDialog::OnDeltaposCalloutSpinPeriod)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_PERIOD, &CInteractiveCalloutConfigDialog::OnNMCustomdrawCalloutSliderPeriod)
    ON_BN_CLICKED(IDC_CALLOUT_BUTTON_CURRENT_TIME, &CInteractiveCalloutConfigDialog::OnBnClickedCalloutButtonCurrentTime)
    ON_BN_CLICKED(IDOK, &CInteractiveCalloutConfigDialog::OnBnClickedOk)
    //ON_EN_CHANGE(IDC_CALLOUT_EDIT_TIME, &CCalloutConfigDialog::OnEnChangeCalloutEditTime)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_TIME, &CInteractiveCalloutConfigDialog::OnEnUpdateCalloutEditTime)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_PERIOD, &CInteractiveCalloutConfigDialog::OnEnUpdateCalloutEditPeriod)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_CALLOUT_SLIDER_TIME, &CInteractiveCalloutConfigDialog::OnNMReleasedcaptureCalloutSliderTime)

	ON_EN_CHANGE(IDC_BUTTON_TEXT, OnChangeButtonText)
	ON_CBN_SELCHANGE(IDC_CHOOSE_BUTTON_TYPES, OnSelchangeButtonTypes)
	ON_BN_CLICKED(IDC_EDIT_PASSIV, OnEdit)
    ON_BN_CLICKED(IDC_EDIT_NORMAL, OnEdit)
    ON_BN_CLICKED(IDC_EDIT_PRESSED, OnEdit)
    ON_BN_CLICKED(IDC_EDIT_MOUSEOVER, OnEdit)
    ON_BN_CLICKED(IDC_IBUTTON_RESET_ALL, &CInteractiveCalloutConfigDialog::OnBnClickedIbuttonResetAll)
END_MESSAGE_MAP()

// CCalloutConfigDialog message handlers

BOOL CInteractiveCalloutConfigDialog::OnInitDialog() {
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

		IDC_INTERACTION_TYPE,
        IDC_TYPE,
        IDC_BUTTON_TEXT_LABEL,
        IDC_LABEL_PASSIV,
        IDC_LABEL_NORMAL,
        IDC_LABEL_MOUSEOVER,
        IDC_LABEL_MOUSEDOWN,
        IDC_EDIT_PASSIV,
        IDC_EDIT_NORMAL,
        IDC_EDIT_MOUSEOVER,
        IDC_EDIT_PRESSED,

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

    CString csResetText;
    csResetText.LoadString(IDC_GODEF_BUTTON_RESET);
    GetDlgItem(IDC_IBUTTON_RESET_ALL)->SetWindowText(csResetText);

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

	CString csType;
    csType.LoadString(IDS_TEXT_BUTTON);
    m_wndChoosebuttonTypes.AddString(csType);
    csType.LoadString(IDS_IMAGE_BUTTON);
    m_wndChoosebuttonTypes.AddString(csType);
    m_wndChoosebuttonTypes.SetCurSel(CButtonAppearanceSettings::TEXT_BUTTON);

	CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
        paObjects[CButtonAppearanceSettings::ACTION_PASSIV] = m_pInteraction->GetInactiveObjects();
        paObjects[CButtonAppearanceSettings::ACTION_NORMAL] = m_pInteraction->GetNormalObjects();
        paObjects[CButtonAppearanceSettings::ACTION_OVER] = m_pInteraction->GetOverObjects();
        paObjects[CButtonAppearanceSettings::ACTION_PRESSED] = m_pInteraction->GetPressedObjects();

        m_pButtonVisualSettings->m_bIsImageButton = true;
        for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
            for (int j = 0; j < paObjects[i]->GetSize(); ++j) {
                DrawSdk::DrawObject *pObject = paObjects[i]->GetAt(j);
                if (pObject->GetType() == DrawSdk::RECTANGLE) {
                    if (((DrawSdk::ColorObject *)pObject)->GetIsFilled()) {
                        m_pButtonVisualSettings->m_clrFill[i] = ((DrawSdk::Rectangle *)pObject)->GetFillColorRef();
                    } else {
                        m_pButtonVisualSettings->m_clrLine[i] = ((DrawSdk::Rectangle *)pObject)->GetOutlineColorRef();
                    }
                    if (!((DrawSdk::Rectangle *)pObject)->GetIsFilled()) {
                        m_pButtonVisualSettings->m_nLineWidth[i] = ((DrawSdk::Rectangle *)pObject)->GetLineWidth();
                        m_pButtonVisualSettings->m_nLineStyle[i] = ((DrawSdk::Rectangle *)pObject)->GetLineStyle();
                    }
                    m_pButtonVisualSettings->m_bIsImageButton = false;
                }
                if (pObject->GetType() == DrawSdk::IMAGE) {
                    m_pButtonVisualSettings->m_csFileNames[i] = ((DrawSdk::Image *)pObject)->GetImageName();
                }
                if (pObject->GetType() == DrawSdk::TEXT) {
                    m_pButtonVisualSettings->m_clrText[i] = ((DrawSdk::Text *)pObject)->GetOutlineColorRef();
                    m_csButtonText = ((DrawSdk::Text *)pObject)->GetString();
                    ((DrawSdk::Text *)pObject)->GetLogFont(&m_pButtonVisualSettings->m_lf[i]);
                    m_wndButtonText.SetWindowText(m_csButtonText);
                }
            }
        }
		if (m_pButtonVisualSettings->m_bIsImageButton) {
        m_wndChoosebuttonTypes.SetCurSel(CButtonAppearanceSettings::IMAGE_BUTTON);
    } else {
        m_wndChoosebuttonTypes.SetCurSel(CButtonAppearanceSettings::TEXT_BUTTON);
    }

    for (i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        m_wndPreview[i].IsImageButton(m_pButtonVisualSettings->m_bIsImageButton);
        if (m_pButtonVisualSettings->m_bIsImageButton) {
            m_wndPreview[i].SetImage(m_pButtonVisualSettings->m_csFileNames[i]);
            m_wndPreview[i].GetImageDimension(m_pButtonVisualSettings->m_siImageDimension[i]);
        } else {
            m_wndPreview[i].SetFillColor(m_pButtonVisualSettings->m_clrFill[i]);
            m_wndPreview[i].SetLineColor(m_pButtonVisualSettings->m_clrLine[i]);
        }

        m_wndPreview[i].SetTextColor(m_pButtonVisualSettings->m_clrText[i]);
        m_wndPreview[i].SetButtonFont(&m_pButtonVisualSettings->m_lf[i]);
        m_wndPreview[i].SetText(m_csButtonText);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

//void CInteractiveCalloutConfigDialog::Init(int iDisplayTime, int iDisplayPeriod, int iCurrentMs, int iTotalLen) {
//    m_iDisplayTime = iDisplayTime;
//    m_iDisplayPeriod = iDisplayPeriod - 1;
//    m_iCurrentMs = iCurrentMs;
//    m_iTotalLen = iTotalLen;
//}

void CInteractiveCalloutConfigDialog::Init(int iDisplayTime, int iDisplayPeriod, CEditorDoc *pEditorDoc, CInteractionAreaEx *pInteraction) {
    m_iDisplayTime = iDisplayTime;
    m_iDisplayPeriod = iDisplayPeriod - 1;
    m_iCurrentMs = pEditorDoc->m_curPosMs;
    m_iTotalLen = pEditorDoc->m_docLengthMs;
    m_pEditorDoc = pEditorDoc;
	m_pInteraction = pInteraction;
	m_rcArea = m_pInteraction->GetArea();
}

void CInteractiveCalloutConfigDialog::OnEdit() {
    const MSG* pMsg = GetCurrentMessage( );

    int iAction = -1;
    CString csTitle;
    switch (WPARAM(pMsg->wParam)) {
    case IDC_EDIT_PASSIV:
        csTitle.LoadString(IDS_PREVIEW_PASSIV);
        iAction = CButtonAppearanceSettings::ACTION_PASSIV;
        break;
    case IDC_EDIT_NORMAL:
        csTitle.LoadString(IDS_PREVIEW_NORMAL);
        iAction = CButtonAppearanceSettings::ACTION_NORMAL;
        break;
    case IDC_EDIT_PRESSED:
        csTitle.LoadString(IDS_PREVIEW_PRESSED);
        iAction = CButtonAppearanceSettings::ACTION_PRESSED;
        break;
    case IDC_EDIT_MOUSEOVER:
        csTitle.LoadString(IDS_PREVIEW_MOUSEOVER);
        iAction = CButtonAppearanceSettings::ACTION_OVER;
        break;
    }

    // TODO Error message
    if (iAction < 0)
        return;

    if (m_wndChoosebuttonTypes.GetCurSel() == 0) {
        CEditInteractionTextDialog dlgEdit( this);


        dlgEdit.Init(csTitle, m_csButtonText, m_rcArea, m_pButtonVisualSettings->m_clrFill[iAction], m_pButtonVisualSettings->m_clrLine[iAction],
            m_pButtonVisualSettings->m_clrText[iAction], &m_pButtonVisualSettings->m_lf[iAction]);

        INT_PTR nRet = IDCANCEL;
        nRet = dlgEdit.DoModal();

        if (nRet == IDOK) {

            dlgEdit.SetUserEntries(m_rcArea, m_pButtonVisualSettings->m_clrFill[iAction], m_pButtonVisualSettings->m_clrLine[iAction],
                m_pButtonVisualSettings->m_clrText[iAction], &m_pButtonVisualSettings->m_lf[iAction]);

            m_wndPreview[iAction].SetColors(m_pButtonVisualSettings->m_clrFill[iAction], m_pButtonVisualSettings->m_clrLine[iAction],
                m_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&m_pButtonVisualSettings->m_lf[iAction]);
        }
    } else {
        CEditInteractionButtonDialog dlgEdit( this);

        dlgEdit.Init(csTitle, m_csButtonText, m_rcArea, m_pButtonVisualSettings->m_csFileNames[iAction],
            m_pButtonVisualSettings->m_clrText[iAction], &m_pButtonVisualSettings->m_lf[iAction]);

        INT_PTR nRet = IDCANCEL;
        nRet = dlgEdit.DoModal();

        if (nRet == IDOK) {
            dlgEdit.SetUserEntries(m_rcArea, m_pButtonVisualSettings->m_csFileNames[iAction],
                m_pButtonVisualSettings->m_clrText[iAction], &m_pButtonVisualSettings->m_lf[iAction]);

            m_wndPreview[iAction].SetTextColor(m_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&m_pButtonVisualSettings->m_lf[iAction]);
            m_wndPreview[iAction].SetImage(m_pButtonVisualSettings->m_csFileNames[iAction]);
            m_wndPreview[iAction].GetImageDimension(m_pButtonVisualSettings->m_siImageDimension[iAction]);

            if (WPARAM(pMsg->wParam) == IDC_EDIT_PASSIV) {
                CString csImagePath = m_pButtonVisualSettings->m_csFileNames[iAction];
                int iSlashPos = csImagePath.ReverseFind('\\');
                csImagePath = csImagePath.Left(iSlashPos);

                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_NORMAL] = csImagePath;
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_NORMAL] += "\\Normal.png";
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_OVER] = csImagePath;
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_OVER] += "\\MouseOver.png";
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_PRESSED] = csImagePath;
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_PRESSED] += "\\MouseDown.png";

                for (int i = 1; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
                    m_wndPreview[i].SetImage(m_pButtonVisualSettings->m_csFileNames[i]);
                    m_wndPreview[i].GetImageDimension(m_pButtonVisualSettings->m_siImageDimension[i]);
                }
            }
        }
    }
    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        m_wndPreview[i].RedrawWindow();
    }
}

void CInteractiveCalloutConfigDialog::OnSelchangeButtonTypes() {
    UINT nButtonType = m_wndChoosebuttonTypes.GetCurSel();

    bool bIsImageButton = false;
    if (nButtonType == CButtonAppearanceSettings::IMAGE_BUTTON)
        bIsImageButton = true;

    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        m_wndPreview[i].IsImageButton(bIsImageButton);
        if (bIsImageButton) {
            m_wndPreview[i].SetImage(m_pButtonVisualSettings->m_csFileNames[i]);
            m_wndPreview[i].GetImageDimension(m_pButtonVisualSettings->m_siImageDimension[i]);
        } else {
            m_wndPreview[i].SetFillColor(m_pButtonVisualSettings->m_clrFill[i]);
            m_wndPreview[i].SetLineColor(m_pButtonVisualSettings->m_clrLine[i]);
        }
    }
}

void CInteractiveCalloutConfigDialog::OnChangeButtonText() {
    m_wndButtonText.GetWindowText(m_csButtonText);

    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        m_wndPreview[i].SetText(m_csButtonText);
        m_wndPreview[i].RedrawWindow();
    }
}

void CInteractiveCalloutConfigDialog::SetCurrentPageInfo(int iDisplayTime) {
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

void CInteractiveCalloutConfigDialog::SetPeriod(int iDisplayTime) {
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

void CInteractiveCalloutConfigDialog::OnDeltaposCalloutSpinTime(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
    float fPos = m_spnTime.GetPos();
    int iSldPos = fPos * 1000;
    m_sldTime.SetPos(iSldPos);
    SetCurrentPageInfo(iSldPos);
    SetPeriod(iSldPos);
    *pResult = 0;
}

void CInteractiveCalloutConfigDialog::OnNMCustomdrawCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    int iPos = m_sldTime.GetPos();
    m_spnTime.SetPos(iPos / 1000.0);
    SetCurrentPageInfo(iPos);
    /*SetPeriod(iPos);*/
    *pResult = 0;
}

void CInteractiveCalloutConfigDialog::OnDeltaposCalloutSpinPeriod(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    float fPos = m_spnPeriod.GetPos();
    int iSldPos = fPos * 1000;
    m_sldPeriod.SetPos(iSldPos);
    m_iDisplayPeriod = iSldPos;
    *pResult = 0;
}

void CInteractiveCalloutConfigDialog::OnNMCustomdrawCalloutSliderPeriod(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    int iPos = m_sldPeriod.GetPos();
    m_spnPeriod.SetPos(iPos / 1000.0);
    m_iDisplayPeriod = iPos;
    *pResult = 0;
}

void CInteractiveCalloutConfigDialog::OnBnClickedCalloutButtonCurrentTime() {
    m_spnTime.SetPos(m_iCurrentMs / 1000.0);
    m_sldTime.SetPos(m_iCurrentMs);
}

void CInteractiveCalloutConfigDialog::GetDisplayParameters(int &iDisplayTime, int &iPeriod) {
    iDisplayTime = m_iDisplayTime;
    iPeriod = m_iDisplayPeriod;
}

void CInteractiveCalloutConfigDialog::OnBnClickedOk() {
    m_iDisplayTime = m_spnTime.GetPos() * 1000;//m_sldTime.GetPos();
    m_iDisplayPeriod = m_spnPeriod.GetPos() * 1000;//m_sldPeriod.GetPos();
    if (m_iDisplayPeriod > m_iTotalLen - m_iDisplayTime) {
        m_iDisplayPeriod = m_iTotalLen - m_iDisplayTime;
    }

	m_pButtonVisualSettings->m_bIsImageButton = (m_wndChoosebuttonTypes.GetCurSel() == CButtonAppearanceSettings::IMAGE_BUTTON);

    OnOK();
}

void CInteractiveCalloutConfigDialog::SetUserEntries(CRect &rcArea, CRect &rcAreaButton, CInteractionAreaEx *pInteraction) {

    //CalculateButtonArea(rcArea, rcAreaButton);

    CTimePeriod visibility;
	visibility.Init(m_pInteraction->GetVisibilityPeriod(), m_pInteraction->GetVisibilityStart(), m_pInteraction->GetVisibilityStart() + m_pInteraction->GetVisibilityLength() - 1);
    CTimePeriod activity;
	activity.Init(m_pInteraction->GetActivityPeriod(), m_pInteraction->GetActivityStart(), m_pInteraction->GetActivityStart() + m_pInteraction->GetActivityLength() - 1);

	pInteraction->Init(&rcAreaButton, m_pInteraction->IsHandCursor(),
		m_pInteraction->GetClickAction(),m_pInteraction->GetDownAction(),m_pInteraction->GetUpAction(),m_pInteraction->GetEnterAction(),m_pInteraction->GetLeaveAction(),
		&visibility, &activity, &m_pEditorDoc->project);

    if (m_pInteraction){
        pInteraction->SetDemoDocumentObject(m_pInteraction->IsDemoDocumentObject());
    }

	CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
	for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
		CPoint ptText;

		if (m_pButtonVisualSettings->m_bIsImageButton) {
			CInteractionAreaEx::CreateButtonObjects(&aObjects[i], rcAreaButton, m_pButtonVisualSettings->m_siImageDimension[i], m_pButtonVisualSettings->m_csFileNames[i],
				m_csButtonText, m_pButtonVisualSettings->m_clrText[i], &m_pButtonVisualSettings->m_lf[i]);
		} else {
			CInteractionAreaEx::CreateButtonObjects(&aObjects[i], rcAreaButton, m_pButtonVisualSettings->m_clrFill[i], m_pButtonVisualSettings->m_clrLine[i], 
				m_pButtonVisualSettings->m_nLineWidth[i], m_pButtonVisualSettings->m_nLineStyle[i], 
				m_csButtonText, m_pButtonVisualSettings->m_clrText[i], &m_pButtonVisualSettings->m_lf[i]);
		}

	}

	pInteraction->InitButton(BUTTON_TYPE_OTHER, &(aObjects[CButtonAppearanceSettings::ACTION_NORMAL]), 
		&(aObjects[CButtonAppearanceSettings::ACTION_OVER]), &(aObjects[CButtonAppearanceSettings::ACTION_PRESSED]), &(aObjects[CButtonAppearanceSettings::ACTION_PASSIV]), m_pButtonVisualSettings->m_bIsImageButton);

	for (i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
		for (int j = 0; j < aObjects[i].GetSize(); ++j) {
			if (aObjects[i][j])
			delete aObjects[i][j];
		}
		aObjects[i].RemoveAll();
	}
}

void CInteractiveCalloutConfigDialog::CalculateButtonArea(CRect &rcArea, CRect &rcAreaButton) {
	CRect rcOldButton(rcAreaButton.left,rcAreaButton.top,rcAreaButton.right,rcAreaButton.bottom);
	rcAreaButton.SetRectEmpty();
	for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
		CSize siButton(0, 0);
		if (m_pButtonVisualSettings->m_bIsImageButton && !m_pButtonVisualSettings->m_csFileNames[i].IsEmpty()) {
			CInteractionAreaEx::CalculateButtonSize(siButton, m_pButtonVisualSettings->m_csFileNames[i]);
			rcAreaButton.left = rcOldButton.left + (rcOldButton.Width() - siButton.cx)/2.0;
			rcAreaButton.right = rcOldButton.right - (rcOldButton.Width() - siButton.cx)/2.0;
			rcAreaButton.top = rcOldButton.bottom - siButton.cy;
			rcAreaButton.bottom = rcOldButton.bottom;
		} else {
			CInteractionAreaEx::CalculateButtonSize(siButton, m_csButtonText, &m_pButtonVisualSettings->m_lf[i]);

			if (siButton.cx > rcAreaButton.Width())
			{
				rcAreaButton.left = rcOldButton.left + (rcOldButton.Width() - siButton.cx)/2.0;
				rcAreaButton.right = rcOldButton.right - (rcOldButton.Width() - siButton.cx)/2.0;
			}
			if (siButton.cy > rcAreaButton.Height())
			{
				rcAreaButton.top = rcOldButton.bottom - siButton.cy;
				rcAreaButton.bottom = rcOldButton.bottom;
			}
			if (rcAreaButton.Width() > rcArea.Width())
			{
				rcAreaButton.left = rcArea.left;
				rcAreaButton.right = rcArea.right;
			}
			if (rcAreaButton.Height() > rcArea.Height())
			{
				rcAreaButton.top = rcArea.top;
				rcAreaButton.bottom = rcArea.bottom;
			}
		}
	}
}

void CInteractiveCalloutConfigDialog::OnEnUpdateCalloutEditTime() {
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

void CInteractiveCalloutConfigDialog::OnEnUpdateCalloutEditPeriod() {
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

void CInteractiveCalloutConfigDialog::OnNMReleasedcaptureCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult) {
    int iPos = m_sldTime.GetPos();
    /*m_spnTime.SetPos(iPos / 1000.0);
    SetCurrentPageInfo(iPos);*/
    SetPeriod(iPos);
    *pResult = 0;
}

void CInteractiveCalloutConfigDialog::OnBnClickedIbuttonResetAll()
{
    // TODO: Add your control notification handler code here
    m_pButtonVisualSettings->ReInitialize();
    for(int iAction = 0; iAction < 4; iAction++) {
        if (m_wndChoosebuttonTypes.GetCurSel() == 0 ) {
            m_wndPreview[iAction].SetColors(m_pButtonVisualSettings->m_clrFill[iAction], m_pButtonVisualSettings->m_clrLine[iAction],
                m_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&m_pButtonVisualSettings->m_lf[iAction]);
        } else {
            m_wndPreview[iAction].SetTextColor(m_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&m_pButtonVisualSettings->m_lf[iAction]);
            m_wndPreview[iAction].SetImage(m_pButtonVisualSettings->m_csFileNames[iAction]);
            m_wndPreview[iAction].GetImageDimension(m_pButtonVisualSettings->m_siImageDimension[iAction]);
            m_wndPreview[iAction].RedrawWindow();
        }
    }
}

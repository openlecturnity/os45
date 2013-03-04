// InteractionButtonDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "InteractionButtonDialog.h"
#include "editorDoc.h"
#include "EditInteractionDialog.h"
#include "lutils.h"
#include "MfcUtils.h"
#include "ButtonAppearanceSettings.h"

#include "WhiteboardView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInteractionButtonDialog 

CButtonAppearanceSettings *CInteractionButtonDialog::s_pButtonVisualSettings = NULL;
void CInteractionButtonDialog::Clean() {
    SAFE_DELETE(s_pButtonVisualSettings);
}

CInteractionButtonDialog::CInteractionButtonDialog(CWnd* pParent /*=NULL*/)
: CDialog(CInteractionButtonDialog::IDD, pParent) {
    if (s_pButtonVisualSettings == NULL) {
        s_pButtonVisualSettings = new CButtonAppearanceSettings();
    }
    //{{AFX_DATA_INIT(CInteractionButtonDialog)
    m_bSetStopmark = FALSE;
    m_bShowMouseCursor = FALSE;
    //}}AFX_DATA_INIT

    m_pInteractionButton = NULL;

    m_nMarkOrPageId = 0;

    m_hBitmap = static_cast<HBITMAP>(LoadImage (AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BUTTON_DOWN),
        IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));

    m_csButtonText.LoadString(IDS_CONTINUE);
    m_bInitialPeriod = false;
    m_iCustomStartTime = -1;
    m_iCustomPeriod = -1;
    m_bIsInitialized = false;
    m_iCustomStartTimeA = -1;
    m_iCustomPeriodA = -1;
    m_bInitialPeriodA = false;
    m_pScrollHelper = new CScrollHelper();


}

CInteractionButtonDialog::~CInteractionButtonDialog() {
    DeleteObject(m_hBitmap);
    if(m_pScrollHelper)
        delete m_pScrollHelper;
}


void CInteractionButtonDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CInteractionButtonDialog)
    DDX_Control(pDX, IDC_MOUSEDOWN_ACTION, m_wndMouseClickAction);
    DDX_Control(pDX, IDC_BUTTON_TEXT, m_wndButtonText);
    DDX_Control(pDX, IDC_CHOOSE_BUTTON_TYPES, m_wndChoosebuttonTypes);
    DDX_Control(pDX, IDC_POPUP_MOUSEDOWN_INTERN, m_btnMouseClickIntern);
    DDX_Control(pDX, IDC_PREVIEW_MOUSEOVER, m_wndPreview[CButtonAppearanceSettings::ACTION_OVER]);
    DDX_Control(pDX, IDC_PREVIEW_NORMAL, m_wndPreview[CButtonAppearanceSettings::ACTION_NORMAL]);
    DDX_Control(pDX, IDC_PREVIEW_PRESSED, m_wndPreview[CButtonAppearanceSettings::ACTION_PRESSED]);
    DDX_Control(pDX, IDC_PREVIEW_PASSIV, m_wndPreview[CButtonAppearanceSettings::ACTION_PASSIV]);

    DDX_Check(pDX, IDC_SET_STOPMARK, m_bSetStopmark);
    DDX_Check(pDX, IDC_SHOW_MOUSECURSOR, m_bShowMouseCursor);
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_ACTION_START_REPLAY, m_cbStartReplay);
    DDX_Control(pDX, IDC_ACTION_STOP_REPLAY, m_cbStopReplay);
    DDX_Control(pDX, IDC_CALLOUT_SLIDER_TIME, m_sldTime);
    DDX_Control(pDX, IDC_CALLOUT_SLIDER_PERIOD, m_sldPeriod);
    DDX_Control(pDX, IDC_CALLOUT_EDIT_TIME, m_edtTime);
    DDX_Control(pDX, IDC_CALLOUT_EDIT_PERIOD, m_edtPeriod);
    DDX_Control(pDX, IDC_CALLOUT_SPIN_TIME, m_spnTime);
    DDX_Control(pDX, IDC_CALLOUT_SPIN_PERIOD, m_spnPeriod);
    DDX_Control(pDX, IDC_CALLOUT_LBL_MAX_PERIOD, m_lblMaxPeriod);
    DDX_Control(pDX, IDC_STATIC_BACKGROUND, m_stBottom);
    DDX_Control(pDX, IDOK, m_btnOK);
    DDX_Control(pDX, IDCANCEL, m_btnCancel);
    DDX_Control(pDX, IDC_CALLOUT_SLIDER_TIME2, m_sldTimeA);
    DDX_Control(pDX, IDC_CALLOUT_SLIDER_PERIOD2, m_sldPeriodA);
    DDX_Control(pDX, IDC_CALLOUT_EDIT_TIME2, m_edtTimeA);
    DDX_Control(pDX, IDC_CALLOUT_EDIT_PERIOD2, m_edtPeriodA);
    DDX_Control(pDX, IDC_CALLOUT_SPIN_TIME2, m_spnTimeA);
    DDX_Control(pDX, IDC_CALLOUT_SPIN_PERIOD2, m_spnPeriodA);
    DDX_Control(pDX, IDC_CALLOUT_LBL_MAX_PERIOD2, m_lblMaxPeriodA);
}


BEGIN_MESSAGE_MAP(CInteractionButtonDialog, CDialog)
    //{{AFX_MSG_MAP(CInteractionButtonDialog)
    ON_MESSAGE_VOID( WM_IDLEUPDATECMDUI, OnIdleUpdateUIMessage )
    ON_EN_CHANGE(IDC_BUTTON_TEXT, OnChangeButtonText)
    ON_BN_CLICKED(IDC_POPUP_MOUSEDOWN_INTERN, OnPopupIntern)
    ON_BN_CLICKED(IDC_POPUP_MOUSEDOWN_EXTERN, OnPopupExtern)
    ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEDOWN_EXTERN, OnUpdatePopupExtern)
    ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEDOWN_INTERN, OnUpdatePopupIntern)
    ON_UPDATE_COMMAND_UI(IDC_AC_ALL_PAGES, OnUpdateActivityButtons)
    ON_UPDATE_COMMAND_UI(IDC_MOUSEDOWN_DESTINATION, OnUpdateEdit)
    ON_CBN_SELCHANGE(IDC_CHOOSE_BUTTON_TYPES, OnSelchangeButtonTypes)
    ON_CBN_SELCHANGE(IDC_MOUSEDOWN_ACTION, OnSelchangeMousedownAction)
    ON_UPDATE_COMMAND_UI(IDC_AC_CURRENT_PAGE, OnUpdateActivityButtons)
    ON_UPDATE_COMMAND_UI(IDC_AC_CURRENT_TIMESTAMP, OnUpdateActivityButtons)
    ON_UPDATE_COMMAND_UI(IDC_AC_END_OF_PAGE, OnUpdateActivityButtons)
    ON_UPDATE_COMMAND_UI(IDC_AC_CHOOSE, OnUpdateActivityButtons)
    ON_UPDATE_COMMAND_UI(IDC_SET_STOPMARK, OnUpdateSetStopMark)
    ON_UPDATE_COMMAND_UI_RANGE(IDC_CALLOUT_SLIDER_TIME, IDC_CALLOUT_LBL_PLVALUE,OnUpdateCustomTimeSettings)
    ON_UPDATE_COMMAND_UI_RANGE(IDC_CALLOUT_SLIDER_TIME2, IDC_CALLOUT_SPIN_PERIOD2,OnUpdateCustomTimeSettings2)
    ON_BN_CLICKED(IDC_EDIT_PASSIV, OnEdit)
    ON_BN_CLICKED(IDC_EDIT_NORMAL, OnEdit)
    ON_BN_CLICKED(IDC_EDIT_PRESSED, OnEdit)
    ON_BN_CLICKED(IDC_EDIT_MOUSEOVER, OnEdit)
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_ACTION_STOP_REPLAY, &CInteractionButtonDialog::OnBnClickedActionStopReplay)
    ON_BN_CLICKED(IDC_ACTION_START_REPLAY, &CInteractionButtonDialog::OnBnClickedActionStartReplay)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_TIME, &CInteractionButtonDialog::OnNMCustomdrawCalloutSliderTime)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_TIME, &CInteractionButtonDialog::OnDeltaposCalloutSpinTime)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_TIME, &CInteractionButtonDialog::OnEnUpdateCalloutEditTime)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_PERIOD, &CInteractionButtonDialog::OnNMCustomdrawCalloutSliderPeriod)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_PERIOD, &CInteractionButtonDialog::OnDeltaposCalloutSpinPeriod)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_PERIOD, &CInteractionButtonDialog::OnEnUpdateCalloutEditPeriod)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_CALLOUT_SLIDER_TIME, &CInteractionButtonDialog::OnNMReleasedcaptureCalloutSliderTime)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_WM_MOUSEWHEEL()
    ON_WM_VSCROLL()
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_TIME2, &CInteractionButtonDialog::OnNMCustomdrawCalloutSliderTime2)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_TIME2, &CInteractionButtonDialog::OnDeltaposCalloutSpinTime2)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_TIME2, &CInteractionButtonDialog::OnEnUpdateCalloutEditTime2)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_PERIOD2, &CInteractionButtonDialog::OnNMCustomdrawCalloutSliderPeriod2)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_PERIOD2, &CInteractionButtonDialog::OnDeltaposCalloutSpinPeriod2)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_PERIOD2, &CInteractionButtonDialog::OnEnUpdateCalloutEditPeriod2)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_CALLOUT_SLIDER_TIME2, &CInteractionButtonDialog::OnNMReleasedcaptureCalloutSliderTime2)
    ON_BN_CLICKED(IDC_IBUTTON_RESET_ALL, &CInteractionButtonDialog::OnBnClickedIbuttonResetAll)
END_MESSAGE_MAP()


void CInteractionButtonDialog::Init(CInteractionAreaEx *pInteractionButton, CRect &rcArea, 
                                    CEditorProject *pProject) {
    m_pInteractionButton = pInteractionButton;
    m_rcArea = rcArea;
    m_pEditorProject = pProject;
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CInteractionButtonDialog 

BOOL CInteractionButtonDialog::OnInitDialog() {
    CDialog::OnInitDialog();

    CString csTitle;
    csTitle.LoadString(CInteractionButtonDialog::IDD);
    SetWindowText(csTitle);

    DWORD dwIds[] = {IDC_INTERACTION_TYPE,
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
        IDC_IF_MOUSEDOWN,
        IDC_ACTION_MD,
        IDC_SHOW_BUTTON,
        IDC_AC_ALL_PAGES,
        IDC_AC_CURRENT_PAGE,
        IDC_AC_CURRENT_TIMESTAMP,
        IDC_AC_END_OF_PAGE,
        IDC_AC_ANALOG_VB,
        IDC_VB_END_OF_PAGE,
        IDC_VB_CURRENT_PAGE,
        IDC_VB_CURRENT_TIMESTAMP,
        IDC_VB_ALL_PAGES,
        IDC_BUTTON_ACTIVE,
        IDC_STOP_REPLAY,
        IDC_SET_STOPMARK,
        IDC_SHOW_MOUSECURSOR,
        IDC_ACTION_START_REPLAY,
        IDC_ACTION_STOP_REPLAY,
        IDC_CALLOUT_LBL_DTIME,
        IDC_CALLOUT_LBL_DPERIOD,
        IDC_CALLOUT_TIME_SEC,          
        IDC_CALLOUT_PERIOD_SEC,
        IDC_CALLOUT_LBL_DTIME2,
        IDC_CALLOUT_LBL_DPERIOD2,
        IDC_CALLOUT_TIME_SEC2,          
        IDC_CALLOUT_PERIOD_SEC2,
        IDOK,
        IDCANCEL,
        -1};
    MfcUtils::Localize(this, dwIds);

    CString csCustom;
    csCustom.LoadString(IDS_PAGE_SIZE_CUSTOM);
    GetDlgItem(IDC_VB_NOT_ON_PAGES)->SetWindowText(csCustom);
    GetDlgItem(IDC_AC_CHOOSE)->SetWindowText(csCustom);

    CString csResetText;
    csResetText.LoadString(IDC_GODEF_BUTTON_RESET);
    GetDlgItem(IDC_IBUTTON_RESET_ALL)->SetWindowText(csResetText);


    CString csType;
    csType.LoadString(IDS_TEXT_BUTTON);
    m_wndChoosebuttonTypes.AddString(csType);
    csType.LoadString(IDS_IMAGE_BUTTON);
    m_wndChoosebuttonTypes.AddString(csType);
    m_wndChoosebuttonTypes.SetCurSel(CButtonAppearanceSettings::TEXT_BUTTON);

    m_btnMouseClickIntern.SetBitmap(m_hBitmap);

    CStringArray aActionStrings;
    CMouseAction::GetActionStrings(aActionStrings, m_pEditorProject->FirstQuestionnaireIsRandomTest());
    for (int i = 0; i < aActionStrings.GetSize(); ++i) {
        m_wndMouseClickAction.AddString(aActionStrings[i]);
        if (!m_pEditorProject->FirstQuestionnaireIsRandomTest() && i >= INTERACTION_JUMP_RANDOM_QUESTION) {
            m_wndMouseClickAction.SetItemData(i, i + 1);
        } else {
            m_wndMouseClickAction.SetItemData(i, i);
        }
    }

    int iStart = 0;
    int iStartA = 0;
    if (m_pInteractionButton) {
        if (m_pInteractionButton->GetClickAction()) {
            m_nActionTypeId = m_pInteractionButton->GetClickAction()->GetActionId();
            if (m_pInteractionButton->GetClickAction()->GetNextAction() != NULL) {
                m_nNextActionTypeId = m_pInteractionButton->GetClickAction()->GetNextAction()->GetActionId();
            } else {
                m_nNextActionTypeId = INTERACTION_NO_ACTION;
            }
            for (int i =0; i< m_wndMouseClickAction.GetCount(); i++) {
                if (m_wndMouseClickAction.GetItemData(i) == m_nActionTypeId) {
                    m_wndMouseClickAction.SetCurSel(i);
                    break;
                }
            }
            

            if (m_nNextActionTypeId == INTERACTION_START_REPLAY || m_nActionTypeId == INTERACTION_JUMP_RANDOM_QUESTION) {
                m_cbStartReplay.SetCheck(1);
                m_cbStopReplay.SetCheck(0);
            } else if (m_nNextActionTypeId == INTERACTION_STOP_REPLAY) {
                m_cbStartReplay.SetCheck(0);
                m_cbStopReplay.SetCheck(1);
            } else {
                m_cbStartReplay.SetCheck(0);
                m_cbStopReplay.SetCheck(0);
            }
            if (m_nActionTypeId == INTERACTION_NO_ACTION || m_nActionTypeId == INTERACTION_START_REPLAY || m_nActionTypeId == INTERACTION_STOP_REPLAY || m_nActionTypeId == INTERACTION_EXIT_PROCESS || m_nActionTypeId == INTERACTION_JUMP_RANDOM_QUESTION) {
                m_cbStartReplay.EnableWindow(0);
                m_cbStopReplay.EnableWindow(0);
            } else {
                m_cbStartReplay.EnableWindow(1);
                m_cbStopReplay.EnableWindow(1);
            }

            SetActionText(m_pInteractionButton->GetClickAction(), IDC_MOUSEDOWN_DESTINATION);
            m_nMarkOrPageId = m_pInteractionButton->GetClickAction()->GetPageMarkId();
        } else {
            m_wndMouseClickAction.SetCurSel(INTERACTION_NO_ACTION);
            m_cbStartReplay.SetCheck(0);
            m_cbStopReplay.SetCheck(0);
            m_cbStartReplay.EnableWindow(0);
            m_cbStopReplay.EnableWindow(0);
        }

        CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
        paObjects[CButtonAppearanceSettings::ACTION_PASSIV] = m_pInteractionButton->GetInactiveObjects();
        paObjects[CButtonAppearanceSettings::ACTION_NORMAL] = m_pInteractionButton->GetNormalObjects();
        paObjects[CButtonAppearanceSettings::ACTION_OVER] = m_pInteractionButton->GetOverObjects();
        paObjects[CButtonAppearanceSettings::ACTION_PRESSED] = m_pInteractionButton->GetPressedObjects();

        s_pButtonVisualSettings->m_bIsImageButton = true;
        for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
            for (int j = 0; j < paObjects[i]->GetSize(); ++j) {
                DrawSdk::DrawObject *pObject = paObjects[i]->GetAt(j);
                if (pObject->GetType() == DrawSdk::RECTANGLE) {
                    if (((DrawSdk::ColorObject *)pObject)->GetIsFilled()) {
                        s_pButtonVisualSettings->m_clrFill[i] = ((DrawSdk::Rectangle *)pObject)->GetFillColorRef();
                    } else {
                        s_pButtonVisualSettings->m_clrLine[i] = ((DrawSdk::Rectangle *)pObject)->GetOutlineColorRef();
                    }
                    if (!((DrawSdk::Rectangle *)pObject)->GetIsFilled()) {
                        s_pButtonVisualSettings->m_nLineWidth[i] = ((DrawSdk::Rectangle *)pObject)->GetLineWidth();
                        s_pButtonVisualSettings->m_nLineStyle[i] = ((DrawSdk::Rectangle *)pObject)->GetLineStyle();
                    }
                    s_pButtonVisualSettings->m_bIsImageButton = false;
                }
                if (pObject->GetType() == DrawSdk::IMAGE) {
                    s_pButtonVisualSettings->m_csFileNames[i] = ((DrawSdk::Image *)pObject)->GetImageName();
                }
                if (pObject->GetType() == DrawSdk::TEXT) {
                    s_pButtonVisualSettings->m_clrText[i] = ((DrawSdk::Text *)pObject)->GetOutlineColorRef();
                    m_csButtonText = ((DrawSdk::Text *)pObject)->GetString();
                    ((DrawSdk::Text *)pObject)->GetLogFont(&s_pButtonVisualSettings->m_lf[i]);
                    m_wndButtonText.SetWindowText(m_csButtonText);
                }
            }
        }


        AreaPeriodId activityPeriodId = m_pInteractionButton->GetActivityPeriod();
        iStartA = m_pInteractionButton->GetActivityStart();
        int iLengthA = m_pInteractionButton->GetActivityLength();
        int iRadioButtonId = IDC_AC_CURRENT_PAGE;
        switch (activityPeriodId) {
        case INTERACTION_PERIOD_PAGE:
            iRadioButtonId = IDC_AC_CURRENT_PAGE;
            break;
        case INTERACTION_PERIOD_END_PAGE:
            iRadioButtonId = IDC_AC_END_OF_PAGE;
            break;
        case INTERACTION_PERIOD_TIME:
            if (iLengthA == 1)
                iRadioButtonId = IDC_AC_CURRENT_TIMESTAMP;
            else 
                iRadioButtonId = IDC_AC_CHOOSE;
            break;
        case INTERACTION_PERIOD_ALL_PAGES:
            iRadioButtonId = IDC_AC_ALL_PAGES;
            break;
        }
        CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, iRadioButtonId);
        if (iRadioButtonId == IDC_AC_CHOOSE)
            CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, iRadioButtonId);


        AreaPeriodId visualityPeriodId = m_pInteractionButton->GetVisibilityPeriod();
        iStart = m_pInteractionButton->GetVisibilityStart();
        int iLength = m_pInteractionButton->GetVisibilityLength();
        iRadioButtonId = -1; //IDC_VB_CURRENT_PAGE;
        switch (visualityPeriodId) {
        case INTERACTION_PERIOD_PAGE:
            iRadioButtonId = IDC_VB_CURRENT_PAGE;
            break;
        case INTERACTION_PERIOD_END_PAGE:
            iRadioButtonId = IDC_VB_END_OF_PAGE;
            break;
        case INTERACTION_PERIOD_TIME:
            if (iLength == 1)
                iRadioButtonId = IDC_VB_CURRENT_TIMESTAMP;
            else 
                iRadioButtonId = IDC_VB_NOT_ON_PAGES;
            break;
        case INTERACTION_PERIOD_ALL_PAGES:
            iRadioButtonId = IDC_VB_ALL_PAGES;
            break;
        }
        CheckRadioButton(IDC_VB_ALL_PAGES, IDC_VB_NOT_ON_PAGES, iRadioButtonId);

        

        if (activityPeriodId == visualityPeriodId) {
            CheckDlgButton(IDC_AC_ANALOG_VB, true);
        } else {
            CheckDlgButton(IDC_AC_ANALOG_VB, false);
        }

        if(m_pInteractionButton->IsDemoDocumentObject()){
            CheckDlgButton(IDC_AC_ANALOG_VB, false);
            GetDlgItem(IDC_AC_ANALOG_VB)->EnableWindow(0);
        }


        m_bShowMouseCursor = m_pInteractionButton->IsHandCursor() == true ? TRUE : FALSE;

    } else {
        m_wndMouseClickAction.SetCurSel(INTERACTION_JUMP_NEXT_PAGE);
        m_wndButtonText.SetWindowText(m_csButtonText);

        CheckRadioButton(IDC_VB_ALL_PAGES, IDC_VB_NOT_ON_PAGES, IDC_VB_NOT_ON_PAGES);//IDC_VB_CURRENT_PAGE);
        CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, -1);//IDC_AC_CURRENT_PAGE);
        CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, IDC_AC_CHOOSE);
        CheckDlgButton(IDC_AC_ANALOG_VB, true);
        m_bShowMouseCursor = FALSE;
        iStart = iStartA = m_pEditorProject->m_pDoc->GetCurrentPreviewPos();
    }

    m_spnTime.SetRangeAndDelta(0, m_pEditorProject->GetTotalLength() / 1000.0, 0.001);
    m_spnTime.SetBuddy(&m_edtTime);
    m_spnTime.SetPos(iStart / 1000.0);
    m_spnTime.SetDecimalPlaces(3);

    m_sldTime.SetRange(0, m_pEditorProject->GetTotalLength());
    m_sldTime.SetTicFreq(1);
    m_sldTime.SetPos(iStart);

    m_spnPeriod.SetBuddy(&m_edtPeriod);
    m_spnPeriod.SetRangeAndDelta(0.01,  m_pEditorProject->GetTotalLength() / 1000.0, 0.001);
    m_sldPeriod.SetRange(10, m_pEditorProject->GetTotalLength());
    m_sldPeriod.SetTicFreq(1);
    m_bInitialPeriod = true;
    SetPeriod(iStart);
    m_bInitialPeriod = false;

    m_spnTimeA.SetRangeAndDelta(0, m_pEditorProject->GetTotalLength() / 1000.0, 0.001);
    m_spnTimeA.SetBuddy(&m_edtTimeA);
    m_spnTimeA.SetPos(iStart / 1000.0);
    m_spnTimeA.SetDecimalPlaces(3);

    m_sldTimeA.SetRange(0, m_pEditorProject->GetTotalLength());
    m_sldTimeA.SetTicFreq(1);
    m_sldTimeA.SetPos(iStart);

    m_spnPeriodA.SetBuddy(&m_edtPeriodA);
    m_spnPeriodA.SetRangeAndDelta(0.01,  m_pEditorProject->GetTotalLength() / 1000.0, 0.001);
    m_sldPeriodA.SetRange(10, m_pEditorProject->GetTotalLength());
    m_sldPeriodA.SetTicFreq(1);
    m_bInitialPeriodA = true;
    SetPeriodA(iStart);
    m_bInitialPeriodA = false;

    CButton *pCheckButton = (CButton *)GetDlgItem(IDC_SHOW_MOUSECURSOR);
    pCheckButton->SetCheck(m_bShowMouseCursor);

    if (s_pButtonVisualSettings->m_bIsImageButton) {
        m_wndChoosebuttonTypes.SetCurSel(CButtonAppearanceSettings::IMAGE_BUTTON);
    } else {
        m_wndChoosebuttonTypes.SetCurSel(CButtonAppearanceSettings::TEXT_BUTTON);
    }

    for (i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        m_wndPreview[i].IsImageButton(s_pButtonVisualSettings->m_bIsImageButton);
        if (s_pButtonVisualSettings->m_bIsImageButton) {
            m_wndPreview[i].SetImage(s_pButtonVisualSettings->m_csFileNames[i]);
            m_wndPreview[i].GetImageDimension(s_pButtonVisualSettings->m_siImageDimension[i]);
        } else {
            m_wndPreview[i].SetFillColor(s_pButtonVisualSettings->m_clrFill[i]);
            m_wndPreview[i].SetLineColor(s_pButtonVisualSettings->m_clrLine[i]);
        }

        m_wndPreview[i].SetTextColor(s_pButtonVisualSettings->m_clrText[i]);
        m_wndPreview[i].SetButtonFont(&s_pButtonVisualSettings->m_lf[i]);
        m_wndPreview[i].SetText(m_csButtonText);
    }

   
    /*CRect rcOK;
    m_btnOK.GetWindowRect(&rcOK);
    CRect rcBack;
    m_stBottom.GetWindowRect(&rcBack);
   

    int screenY = GetSystemMetrics(SM_CYMAXIMIZED);*/

    CRect rcWin;
    GetWindowRect(&rcWin);
   /* m_iButtonVOffset = rcWin.bottom - rcOK.bottom;
    m_iBackVOffset = rcWin.bottom - rcBack.bottom;*/
    

    m_szInitial = rcWin.Size();

    /*if( screenY < rcWin.Height() + 40) {
        rcWin.top = 20;
        rcWin.bottom = screenY - 20;
    }*/
       
    m_pScrollHelper->AttachWnd(this);
    m_pScrollHelper->SetDisplaySize(m_szInitial.cx - 20, m_szInitial.cy );
    

    /*MoveWindow(&rcWin, TRUE);
    CenterWindow();*/

    m_bIsInitialized = true;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CInteractionButtonDialog::SetPeriod(int iDisplayTime) {
    int iRemainingPeriod = m_pEditorProject->GetTotalLength() - iDisplayTime + 1;
    UINT nStartMs, nEndMs;
    m_pEditorProject->GetPagePeriod(iDisplayTime, nStartMs, nEndMs);

    int m_iDisplayPeriod = 1;
    if (m_pInteractionButton)
        m_iDisplayPeriod = m_pInteractionButton->GetVisibilityLength() - 1;
    else
        m_iDisplayPeriod = nEndMs - iDisplayTime;
    
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
    csMaxPeriod.Format(_T("%.3f"), m_pEditorProject->GetTotalLength() / 1000.0);
    m_lblMaxPeriod.SetWindowText(csMaxPeriod);
}

void CInteractionButtonDialog::SetPeriodA(int iDisplayTime) {
    int iRemainingPeriod = m_pEditorProject->GetTotalLength() - iDisplayTime + 1;
    UINT nStartMs, nEndMs;
    m_pEditorProject->GetPagePeriod(iDisplayTime, nStartMs, nEndMs);

    int m_iDisplayPeriod = 1;
    if (m_pInteractionButton)
        m_iDisplayPeriod = m_pInteractionButton->GetVisibilityLength() - 1;
    else
        m_iDisplayPeriod = nEndMs - iDisplayTime;
    //m_spnPeriod.SetRangeAndDelta(0,  iRemainingPeriod / 1000.0, 0.001);
    int iCurrentPageLength = nEndMs - nStartMs;
    if(!m_bInitialPeriod) {
        m_iDisplayPeriod = iCurrentPageLength;
    }
    if(iRemainingPeriod < m_iDisplayPeriod) {
        m_iDisplayPeriod = iRemainingPeriod;
    }
    //m_iDisplayPeriod++;
    m_spnPeriodA.SetPos(m_iDisplayPeriod / 1000.0);
    m_spnPeriodA.SetDecimalPlaces(3);

    // m_sldPeriod.SetRange(0, iRemainingPeriod);
    // m_sldPeriod.SetTicFreq(1);
    m_sldPeriodA.SetPos(m_iDisplayPeriod);
    CString csMaxPeriod;
    csMaxPeriod.Format(_T("%.3f"), m_pEditorProject->GetTotalLength() / 1000.0);
    m_lblMaxPeriodA.SetWindowText(csMaxPeriod);
}

void CInteractionButtonDialog::OnIdleUpdateUIMessage() {
    UpdateDialogControls( this, TRUE );
}

BOOL CInteractionButtonDialog::ContinueModal() {
    SendMessage( WM_IDLEUPDATECMDUI, WPARAM(TRUE), 0);
    return CDialog::ContinueModal();
}

void CInteractionButtonDialog::OnChangeButtonText() {
    m_wndButtonText.GetWindowText(m_csButtonText);

    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        m_wndPreview[i].SetText(m_csButtonText);
        m_wndPreview[i].RedrawWindow();
    }
}

void CInteractionButtonDialog::OnPopupIntern() {
    AreaActionTypeId nActionTypeId = (AreaActionTypeId)m_wndMouseClickAction.GetItemData(m_wndMouseClickAction.GetCurSel());
	
	if (nActionTypeId == INTERACTION_JUMP_SPECIFIC_PAGE /*||
        nActionStringId == SPECIFIC_PAGE_STARTREPLAY ||
        nActionStringId == SPECIFIC_PAGE_STOPREPLAY*/)
		CreateSlidePopup(IDC_POPUP_MOUSEDOWN_INTERN, IDC_MOUSEDOWN_DESTINATION);
	else if (nActionTypeId == INTERACTION_JUMP_TARGET_MARK /*||
            nActionStringId == TARGET_MARK_STARTREPLAY ||
            nActionStringId == TARGET_MARK_STOPREPLAY*/)
		CreateTargetmarkPopup(IDC_POPUP_MOUSEDOWN_INTERN, IDC_MOUSEDOWN_DESTINATION);
}

void CInteractionButtonDialog::OnPopupExtern() {
    AreaActionTypeId nActionTypeId = (AreaActionTypeId)m_wndMouseClickAction.GetItemData(m_wndMouseClickAction.GetCurSel());
    CString csFilter;
    if (nActionTypeId == INTERACTION_OPEN_URL)
        csFilter.LoadString(IDS_HTML_FILTER);
    else 
        csFilter.LoadString(IDS_ALL_FILES);

    CFileDialog *pFileDialog = new CFileDialog(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, csFilter, this);
    pFileDialog->DoModal();

    CString csFileName = pFileDialog->GetPathName();

    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MOUSEDOWN_DESTINATION);
    if (pEdit)
        pEdit->SetWindowText(csFileName);

    delete pFileDialog;
}

void CInteractionButtonDialog::OnUpdatePopupIntern(CCmdUI *pCmdUI) {
    bool bEnable = false;

    AreaActionTypeId nAction = (AreaActionTypeId)m_wndMouseClickAction.GetItemData(m_wndMouseClickAction.GetCurSel());

    if (nAction == INTERACTION_JUMP_SPECIFIC_PAGE ||
        /*nAction == SPECIFIC_PAGE_STARTREPLAY ||
        nAction == SPECIFIC_PAGE_STOPREPLAY ||*/ 
        nAction == INTERACTION_JUMP_TARGET_MARK/*||
        nAction == TARGET_MARK_STARTREPLAY ||
        nAction == TARGET_MARK_STOPREPLAY*/) {
        bEnable = true;
    }

    pCmdUI->Enable(bEnable);
}

void CInteractionButtonDialog::OnUpdatePopupExtern(CCmdUI *pCmdUI) {
    bool bEnable = false;

    AreaActionTypeId nAction = (AreaActionTypeId)m_wndMouseClickAction.GetItemData(m_wndMouseClickAction.GetCurSel());

    if (nAction == INTERACTION_OPEN_URL || nAction == INTERACTION_OPEN_FILE)
        bEnable = true;

    pCmdUI->Enable(bEnable);
}

void CInteractionButtonDialog::OnUpdateSetStopMark(CCmdUI* pCmdUI){
    if(m_pInteractionButton && m_pInteractionButton->IsDemoDocumentObject()){
        pCmdUI->Enable(false);
        return;
    }
}

void CInteractionButtonDialog::OnUpdateActivityButtons(CCmdUI *pCmdUI) {
    bool bEnable = true;

    UINT nAnalogChecked = IsDlgButtonChecked(IDC_AC_ANALOG_VB);

    if (nAnalogChecked != 0) {
        bEnable = false;
        int iVisualityButton = GetCheckedRadioButton(IDC_VB_ALL_PAGES, IDC_VB_NOT_ON_PAGES);
        switch (iVisualityButton) {
        case IDC_VB_ALL_PAGES:
            CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, IDC_AC_ALL_PAGES);
            CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, -1);
            break;
        case IDC_VB_CURRENT_TIMESTAMP:
            CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, IDC_AC_CURRENT_TIMESTAMP);
            CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, -1);
            break;
        case IDC_VB_CURRENT_PAGE:
            CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, IDC_AC_CURRENT_PAGE);
            CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, -1);
            break;
        case IDC_VB_END_OF_PAGE:
            CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, IDC_AC_END_OF_PAGE);
            CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, -1);
            break;
        case IDC_VB_NOT_ON_PAGES:
            CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, IDC_AC_CHOOSE);
            CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, -1);
            break;
        }
    } else {
        int iVisualityButton = GetCheckedRadioButton(IDC_VB_ALL_PAGES, IDC_VB_NOT_ON_PAGES);
        int iActivityButton = GetCheckedRadioButton(IDC_AC_ALL_PAGES, IDC_AC_CHOOSE);
        switch (iVisualityButton) {
        case IDC_VB_ALL_PAGES:
            bEnable = true;
            break;
        case IDC_VB_CURRENT_TIMESTAMP:
            CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, IDC_AC_CURRENT_TIMESTAMP);
            CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, -1);
            if (pCmdUI->m_nID != IDC_AC_CURRENT_TIMESTAMP)
                bEnable = false;
            break;
        case IDC_VB_CURRENT_PAGE:
            // select another radio button
            if (iActivityButton == IDC_AC_ALL_PAGES){
                CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, IDC_AC_CURRENT_PAGE);
                CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, -1);
            }

            // diasable all pages
            if (pCmdUI->m_nID == IDC_AC_ALL_PAGES)
                bEnable = false;
            break;
        case IDC_VB_END_OF_PAGE:
            CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, IDC_AC_END_OF_PAGE);
            CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, -1);
            if (pCmdUI->m_nID != IDC_AC_END_OF_PAGE)
                bEnable = false;
            break;
        case IDC_VB_NOT_ON_PAGES:
            CheckRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE, IDC_AC_CHOOSE);
            CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE, -1);
            if (pCmdUI->m_nID != IDC_AC_CHOOSE)
                bEnable = false;
            break;
        }
    }

    pCmdUI->Enable(bEnable);
}

void CInteractionButtonDialog::OnUpdateEdit(CCmdUI *pCmdUI)
{
    bool bEnable = false;

    AreaActionTypeId nAction = (AreaActionTypeId)m_wndMouseClickAction.GetItemData(m_wndMouseClickAction.GetCurSel());

    if (nAction == INTERACTION_JUMP_SPECIFIC_PAGE || 
        nAction == INTERACTION_JUMP_TARGET_MARK ||
        nAction == INTERACTION_OPEN_URL || 
        nAction == INTERACTION_OPEN_FILE)
        bEnable = true;

    pCmdUI->Enable(bEnable);
}

void CInteractionButtonDialog::OnSelchangeButtonTypes() {
    UINT nButtonType = m_wndChoosebuttonTypes.GetCurSel();

    bool bIsImageButton = false;
    if (nButtonType == CButtonAppearanceSettings::IMAGE_BUTTON)
        bIsImageButton = true;

    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        m_wndPreview[i].IsImageButton(bIsImageButton);
        if (bIsImageButton) {
            m_wndPreview[i].SetImage(s_pButtonVisualSettings->m_csFileNames[i]);
            m_wndPreview[i].GetImageDimension(s_pButtonVisualSettings->m_siImageDimension[i]);
        } else {
            m_wndPreview[i].SetFillColor(s_pButtonVisualSettings->m_clrFill[i]);
            m_wndPreview[i].SetLineColor(s_pButtonVisualSettings->m_clrLine[i]);
        }
    }
}

void CInteractionButtonDialog::OnSelchangeMousedownAction() {
    static UINT nLastAction = 0;

    UINT nAction = m_wndMouseClickAction.GetItemData(m_wndMouseClickAction.GetCurSel());
    bool bActionChanged = true;
    if (nAction == nLastAction)
        bActionChanged = false;
    nLastAction = nAction;

    if (bActionChanged) {
        CEdit *pEditWnd = (CEdit *)GetDlgItem(IDC_MOUSEDOWN_DESTINATION);
        pEditWnd->SetWindowText(_T(""));

        m_cbStartReplay.SetCheck(nAction == INTERACTION_JUMP_RANDOM_QUESTION ? 1 : 0);
        m_cbStopReplay.SetCheck(0);

        bool bDisableStartStopCheckBox = (nAction == INTERACTION_NO_ACTION || nAction == INTERACTION_START_REPLAY || nAction == INTERACTION_STOP_REPLAY || nAction == INTERACTION_EXIT_PROCESS || nAction == INTERACTION_JUMP_RANDOM_QUESTION);
        m_cbStartReplay.EnableWindow(bDisableStartStopCheckBox ? 0 : 1);
        m_cbStopReplay.EnableWindow(bDisableStartStopCheckBox ? 0 : 1);
    }
}

void CInteractionButtonDialog::CreateSlidePopup(int nButtonId, int nEditId) {
    CRect rcButton;
    CWnd *pWndButton = GetDlgItem(nButtonId);

    pWndButton->GetClientRect(rcButton);
    pWndButton->ClientToScreen(&rcButton);

    m_wndMenu.CreatePopupMenu();

    CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

    CArray<CPage *, CPage *> caPages;
    m_pEditorProject->GetPages(caPages, 0, pDoc->m_docLengthMs);
    for (int i = 0; i < caPages.GetSize(); ++i) {
        CString csPageTitle;
        csPageTitle = caPages[i]->GetTitle();
        m_wndMenu.AppendMenu(MF_STRING, i+1, csPageTitle);
    }

    BOOL bRet = m_wndMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, rcButton.left, rcButton.bottom, this);

    CWnd *pEdit = GetDlgItem(nEditId);
    // if bRet is zero the menu was cancelled 
    // or an error occured
    if (bRet != 0) {
        CString csItem;
        m_wndMenu.GetMenuString(bRet, csItem, MF_BYCOMMAND);
        pEdit->SetWindowText(csItem);
        m_nMarkOrPageId = caPages[bRet-1]->GetJumpId();
    }

    caPages.RemoveAll();
    m_wndMenu.DestroyMenu();
}

void CInteractionButtonDialog::CreateTargetmarkPopup(int nButtonId, int nEditId) {
    CRect rcButton;
    CWnd *pWndButton = GetDlgItem(nButtonId);

    pWndButton->GetClientRect(rcButton);
    pWndButton->ClientToScreen(&rcButton);

    CMarkStream *pMarkStream = m_pEditorProject->GetMarkStream();
    if (pMarkStream) {
        m_wndMenu.CreatePopupMenu();

        CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

        CArray<CStopJumpMark *, CStopJumpMark *> caJumpMarks;
        pMarkStream->GetJumpMarksInRegion(caJumpMarks, 0, pDoc->m_docLengthMs);
        for (int i = 0; i < caJumpMarks.GetSize(); ++i) {
            CString csLabel;
            csLabel = caJumpMarks[i]->GetLabel();
            m_wndMenu.AppendMenu(MF_STRING, i+1, csLabel);
        }

        BOOL bRet = m_wndMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, rcButton.left, rcButton.bottom, this);

        CWnd *pEdit = GetDlgItem(nEditId);
        // if bRet is zero the menu was cancelled 
        // or an error occured
        if (bRet != 0) {
            CString csItem;
            m_wndMenu.GetMenuString(bRet, csItem, MF_BYCOMMAND);
            pEdit->SetWindowText(csItem);
            m_nMarkOrPageId = caJumpMarks[bRet-1]->GetId();
        }

        caJumpMarks.RemoveAll();

        m_wndMenu.DestroyMenu();
    }
}

void CInteractionButtonDialog::ExtractActionParameters() {	
    m_nActionTypeId = (AreaActionTypeId)m_wndMouseClickAction.GetItemData(m_wndMouseClickAction.GetCurSel());

    if(m_cbStartReplay.GetCheck())
        m_nNextActionTypeId = INTERACTION_START_REPLAY;
    else if (m_cbStopReplay.GetCheck())
        m_nNextActionTypeId = INTERACTION_STOP_REPLAY;
    else
        m_nNextActionTypeId = INTERACTION_NO_ACTION;

    if (m_nActionTypeId == INTERACTION_OPEN_URL || m_nActionTypeId == INTERACTION_OPEN_FILE) {	
        CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MOUSEDOWN_DESTINATION);
        if (pEdit)
            pEdit->GetWindowText(m_csActionPath);
    }
}

void CInteractionButtonDialog::SetActionText(CMouseAction *pMouseAction, int nEditId) {	
    CString csText;
    csText.Empty();

    UINT nPageOrMarkId = pMouseAction->GetPageMarkId();
    AreaActionTypeId nActionTypeId = pMouseAction->GetActionId();

    if (nActionTypeId == INTERACTION_JUMP_SPECIFIC_PAGE) {
        csText = m_pEditorProject->GetPageTitleForId(nPageOrMarkId);
    }
    else if (nActionTypeId == INTERACTION_JUMP_TARGET_MARK) {
        CStopJumpMark *pMark = m_pEditorProject->GetMarkWithId(nPageOrMarkId);
        if (pMark != NULL)
            csText = pMark->GetLabel();
        // TODO else this id is undefined
    } else if (nActionTypeId == INTERACTION_OPEN_URL || nActionTypeId == INTERACTION_OPEN_FILE) {
        csText = pMouseAction->GetPath();
    }

    CEdit *pEdit = (CEdit *)GetDlgItem(nEditId);
    pEdit->SetWindowText(csText);
}

void CInteractionButtonDialog::CalculateButtonArea(CRect &rcArea) {
    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        CSize siButton(0, 0);
        if (s_pButtonVisualSettings->m_bIsImageButton && !s_pButtonVisualSettings->m_csFileNames[i].IsEmpty()) {
            CInteractionAreaEx::CalculateButtonSize(siButton, s_pButtonVisualSettings->m_csFileNames[i]);
            rcArea.right = rcArea.left + siButton.cx;
            rcArea.bottom = rcArea.top + siButton.cy;
        } else {
            CInteractionAreaEx::CalculateButtonSize(siButton, m_csButtonText, &s_pButtonVisualSettings->m_lf[i]);

            CEditorDoc *pDoc = m_pEditorProject->m_pDoc;
            CRect rcWhiteboard = pDoc->GetWhiteboardView()->GetPageDimension();
            double dZoom = pDoc->GetWhiteboardView()->GetZoomFactor();

            if (siButton.cx > rcArea.Width()) {
                rcArea.right = rcArea.left + siButton.cx;

                // if the button is outside whiteboard area, recalculate its position
                int iRight = (int)(rcWhiteboard.Width() * 1.0/dZoom + 0.5);
                if (rcArea.right > iRight) {
                    rcArea.left = iRight - siButton.cx;
                    rcArea.right = iRight;
                }
            }

            if (siButton.cy > rcArea.Height()) {
                rcArea.bottom = rcArea.top + siButton.cy;

                // if the button is outside whiteboard area, recalculate its position
                int iBottom = (int)(rcWhiteboard.Height() * 1.0/dZoom + 0.5);
                if (rcArea.bottom > iBottom) {
                    rcArea.top = iBottom - siButton.cy;
                    rcArea.bottom = iBottom;
                }
            }
        }
    }
}

void CInteractionButtonDialog::SetUserEntries(CRect &rcArea, CInteractionAreaEx *pInteraction) {
    //AreaActionTypeId nNextAction;
    //AreaActionTypeId nActionTypeId = CMouseAction::GetActionId(m_nActionStringId, nNextAction);

    CMouseAction mouseAction;
    mouseAction.Init(m_pEditorProject, m_nActionTypeId, m_nMarkOrPageId, m_csActionPath, m_nNextActionTypeId);

    UINT nVisualityStartMs = 0;
    UINT nVisualityEndMs = 0;

    CEditorDoc *pDoc = m_pEditorProject->m_pDoc;
    UINT nCurrPos = pDoc->m_curPosMs;
    UINT nEndPos = pDoc->m_displayEndMs;

    switch (m_visualityPeriodId) {
    case INTERACTION_PERIOD_PAGE:
        m_pEditorProject->GetPagePeriod(pDoc->m_curPosMs,
            nVisualityStartMs, nVisualityEndMs);
        break;
    case INTERACTION_PERIOD_END_PAGE:
        m_pEditorProject->GetPagePeriod(pDoc->m_curPosMs,
            nVisualityStartMs, nVisualityEndMs);

        // Note: also in this case the time needs to be the whole page.
        break;
    case INTERACTION_PERIOD_TIME:
        nVisualityStartMs = (m_iCustomStartTime == -1)? (nCurrPos - 5 > 0)? nCurrPos - 5: 0 : m_iCustomStartTime;
        nVisualityEndMs = (m_iCustomPeriod == -1)? (nCurrPos + 5 < nEndPos)? nCurrPos + 5: nEndPos : m_iCustomStartTime + m_iCustomPeriod;
        break;
    case INTERACTION_PERIOD_ALL_PAGES:
        nVisualityStartMs = 0;
        nVisualityEndMs = pDoc->m_displayEndMs;
        break;
    }


    UINT nActivityStartMs = 0;
    UINT nActivityEndMs = 0;

    switch (m_activityPeriodId) {
    case INTERACTION_PERIOD_PAGE:
        m_pEditorProject->GetPagePeriod(pDoc->m_curPosMs, 
            nActivityStartMs, nActivityEndMs);
        break;
    case INTERACTION_PERIOD_END_PAGE:
        m_pEditorProject->GetPagePeriod(pDoc->m_curPosMs, 
            nActivityStartMs, nActivityEndMs);

        // Note: also in this case the time needs to be the whole page.
        break;
    case INTERACTION_PERIOD_TIME:
        nActivityStartMs = (m_iCustomStartTimeA == -1) ? (nCurrPos - 5 > 0)? nCurrPos - 5: 0 : m_iCustomStartTimeA;
        nActivityEndMs = (m_iCustomPeriodA == -1) ? (nCurrPos + 5 < nEndPos)? nCurrPos + 5: nEndPos : m_iCustomStartTimeA + m_iCustomPeriodA;
        break;
    case INTERACTION_PERIOD_ALL_PAGES:
        nActivityStartMs = 0;
        nActivityEndMs = pDoc->m_displayEndMs;
        break;
    }

    if (m_bSetStopmark && !(m_pInteractionButton != NULL && m_pInteractionButton->IsDemoDocumentObject())) {	
        HRESULT hr = S_OK;

        CMarkStream* pMarkStream = NULL;
        if (SUCCEEDED(hr) && nActivityEndMs >= 0) {
            pMarkStream = m_pEditorProject->GetMarkStream(true);
            if (!pMarkStream)	// TODO: Fehlermeldung
                hr = E_FAIL;
        }

        if (SUCCEEDED(hr)) {
            CStopJumpMark mark;
            mark.Init(false, NULL, nActivityEndMs);
            pMarkStream->IncludeMark(&mark);
        }
    }

    CalculateButtonArea(rcArea);

    CTimePeriod visibility;
    visibility.Init(m_visualityPeriodId, nVisualityStartMs, nVisualityEndMs);
    CTimePeriod activity;
    activity.Init(m_activityPeriodId, nActivityStartMs, nActivityEndMs);

    pInteraction->Init(&rcArea, m_bShowMouseCursor == 0 ? false : true,
        &mouseAction, NULL, 
        NULL, NULL, NULL, 
        &visibility, &activity, m_pEditorProject);
    if (m_pInteractionButton){
        pInteraction->SetDemoDocumentObject(m_pInteractionButton->IsDemoDocumentObject());
    }

    CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        CPoint ptText;

        if (s_pButtonVisualSettings->m_bIsImageButton) {
            CInteractionAreaEx::CreateButtonObjects(&aObjects[i], rcArea, s_pButtonVisualSettings->m_siImageDimension[i], s_pButtonVisualSettings->m_csFileNames[i],
                m_csButtonText, s_pButtonVisualSettings->m_clrText[i], &s_pButtonVisualSettings->m_lf[i]);
        } else {
            CInteractionAreaEx::CreateButtonObjects(&aObjects[i], rcArea, s_pButtonVisualSettings->m_clrFill[i], s_pButtonVisualSettings->m_clrLine[i], 
                s_pButtonVisualSettings->m_nLineWidth[i], s_pButtonVisualSettings->m_nLineStyle[i], 
                m_csButtonText, s_pButtonVisualSettings->m_clrText[i], &s_pButtonVisualSettings->m_lf[i]);
        }

    }

    pInteraction->InitButton(BUTTON_TYPE_OTHER, &(aObjects[CButtonAppearanceSettings::ACTION_NORMAL]), 
        &(aObjects[CButtonAppearanceSettings::ACTION_OVER]), &(aObjects[CButtonAppearanceSettings::ACTION_PRESSED]), &(aObjects[CButtonAppearanceSettings::ACTION_PASSIV]), s_pButtonVisualSettings->m_bIsImageButton);

    for (i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        for (int j = 0; j < aObjects[i].GetSize(); ++j) {
            DrawSdk::DrawObject *pObject = aObjects[i][j];
            if (pObject)
                delete pObject;
        }
    }
}


void CInteractionButtonDialog::OnBtnClickOK()//OnOK() 
{
    ExtractActionParameters();

    s_pButtonVisualSettings->m_bIsImageButton = (m_wndChoosebuttonTypes.GetCurSel() == CButtonAppearanceSettings::IMAGE_BUTTON);

    int iRadioButtonId = GetCheckedRadioButton(IDC_VB_ALL_PAGES, IDC_VB_NOT_ON_PAGES);
    m_visualityPeriodId = INTERACTION_PERIOD_ALL_PAGES;
    switch (iRadioButtonId) {
    case IDC_VB_CURRENT_PAGE:
        m_visualityPeriodId = INTERACTION_PERIOD_PAGE;
        break;
    case IDC_VB_END_OF_PAGE:
        m_visualityPeriodId = INTERACTION_PERIOD_END_PAGE;
        break;
    case IDC_VB_CURRENT_TIMESTAMP:
        m_visualityPeriodId = INTERACTION_PERIOD_TIME;
        break;
    case IDC_VB_ALL_PAGES:
        m_visualityPeriodId = INTERACTION_PERIOD_ALL_PAGES;
        break;
    case IDC_VB_NOT_ON_PAGES:
        m_visualityPeriodId = INTERACTION_PERIOD_TIME;
        m_iCustomStartTime = m_sldTime.GetPos();
        m_iCustomPeriod = m_sldPeriod.GetPos();
        break;
    }


    UINT nAnalogIsChecked = IsDlgButtonChecked(IDC_AC_ANALOG_VB);
    if (nAnalogIsChecked != 0) {
        m_activityPeriodId = m_visualityPeriodId;
        if (m_iCustomStartTime != -1 || m_iCustomPeriod != -1){
            m_iCustomStartTimeA = m_sldTimeA.GetPos();
            m_iCustomPeriodA = m_sldPeriodA.GetPos();
        }
    } else {
        iRadioButtonId = GetCheckedRadioButton(IDC_AC_ALL_PAGES, IDC_AC_END_OF_PAGE);
        m_activityPeriodId = INTERACTION_PERIOD_ALL_PAGES;
        switch (iRadioButtonId)
        {
        case IDC_AC_CURRENT_PAGE:
            m_activityPeriodId = INTERACTION_PERIOD_PAGE;
            break;
        case IDC_AC_END_OF_PAGE:
            m_activityPeriodId = INTERACTION_PERIOD_END_PAGE;
            break;
        case IDC_AC_CURRENT_TIMESTAMP:
            m_activityPeriodId = INTERACTION_PERIOD_TIME;
            break;
        case IDC_AC_ALL_PAGES:
            m_activityPeriodId = INTERACTION_PERIOD_ALL_PAGES;
            break;
        }
        if (IsDlgButtonChecked(IDC_AC_CHOOSE))
        {
            m_activityPeriodId = INTERACTION_PERIOD_TIME;
            m_iCustomStartTimeA = m_sldTimeA.GetPos();
            m_iCustomPeriodA = m_sldPeriodA.GetPos();
        }
    }

    CDialog::OnOK();
}

void CInteractionButtonDialog::OnOK()  {
    return;
}
void CInteractionButtonDialog::OnEdit() {
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


        dlgEdit.Init(csTitle, m_csButtonText, m_rcArea, s_pButtonVisualSettings->m_clrFill[iAction], s_pButtonVisualSettings->m_clrLine[iAction],
            s_pButtonVisualSettings->m_clrText[iAction], &s_pButtonVisualSettings->m_lf[iAction]);

        INT_PTR nRet = IDCANCEL;
        nRet = dlgEdit.DoModal();

        if (nRet == IDOK) {

            dlgEdit.SetUserEntries(m_rcArea, s_pButtonVisualSettings->m_clrFill[iAction], s_pButtonVisualSettings->m_clrLine[iAction],
                s_pButtonVisualSettings->m_clrText[iAction], &s_pButtonVisualSettings->m_lf[iAction]);

            m_wndPreview[iAction].SetColors(s_pButtonVisualSettings->m_clrFill[iAction], s_pButtonVisualSettings->m_clrLine[iAction],
                s_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&s_pButtonVisualSettings->m_lf[iAction]);
        }
    } else {
        CEditInteractionButtonDialog dlgEdit( this);

        dlgEdit.Init(csTitle, m_csButtonText, m_rcArea, s_pButtonVisualSettings->m_csFileNames[iAction],
            s_pButtonVisualSettings->m_clrText[iAction], &s_pButtonVisualSettings->m_lf[iAction]);

        INT_PTR nRet = IDCANCEL;
        nRet = dlgEdit.DoModal();

        if (nRet == IDOK) {
            dlgEdit.SetUserEntries(m_rcArea, s_pButtonVisualSettings->m_csFileNames[iAction],
                s_pButtonVisualSettings->m_clrText[iAction], &s_pButtonVisualSettings->m_lf[iAction]);

            m_wndPreview[iAction].SetTextColor(s_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&s_pButtonVisualSettings->m_lf[iAction]);
            m_wndPreview[iAction].SetImage(s_pButtonVisualSettings->m_csFileNames[iAction]);
            m_wndPreview[iAction].GetImageDimension(s_pButtonVisualSettings->m_siImageDimension[iAction]);

            if (WPARAM(pMsg->wParam) == IDC_EDIT_PASSIV) {
                CString csImagePath = s_pButtonVisualSettings->m_csFileNames[iAction];
                int iSlashPos = csImagePath.ReverseFind('\\');
                csImagePath = csImagePath.Left(iSlashPos);

                s_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_NORMAL] = csImagePath;
                s_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_NORMAL] += "\\Normal.png";
                s_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_OVER] = csImagePath;
                s_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_OVER] += "\\MouseOver.png";
                s_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_PRESSED] = csImagePath;
                s_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_PRESSED] += "\\MouseDown.png";

                for (int i = 1; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
                    m_wndPreview[i].SetImage(s_pButtonVisualSettings->m_csFileNames[i]);
                    m_wndPreview[i].GetImageDimension(s_pButtonVisualSettings->m_siImageDimension[i]);
                }
            }
        }
    }
    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        m_wndPreview[i].RedrawWindow();
    }
}

void CInteractionButtonDialog::OnBnClickedActionStopReplay() {
    if (m_cbStopReplay.GetCheck())
        m_cbStartReplay.SetCheck(0);
}

void CInteractionButtonDialog::OnBnClickedActionStartReplay() {
    if (m_cbStartReplay.GetCheck())
        m_cbStopReplay.SetCheck(0);
}

void CInteractionButtonDialog::OnNMCustomdrawCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here
    int iPos = m_sldTime.GetPos();
    m_spnTime.SetPos(iPos / 1000.0);
    *pResult = 0;
}

void CInteractionButtonDialog::OnDeltaposCalloutSpinTime(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
     float fPos = m_spnTime.GetPos();
    int iSldPos = fPos * 1000;
    m_sldTime.SetPos(iSldPos);
    SetPeriod(iSldPos);
    *pResult = 0;
}

void CInteractionButtonDialog::OnEnUpdateCalloutEditTime()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function to send the EM_SETEVENTMASK message to the control
    // with the ENM_UPDATE flag ORed into the lParam mask.

    // TODO:  Add your control notification handler code here
    int iInitialPos = m_sldTime.GetPos();
    int iCursorPos = m_edtTime.GetSel();
    double fInitValue = m_spnTime.GetPos();

    int iChangedPos = fInitValue*1000;
    m_sldTime.SetPos(iChangedPos);


    if(iInitialPos != iChangedPos) {
        m_edtTime.SetSel(iCursorPos + 1, iCursorPos + 1);
    }
}

void CInteractionButtonDialog::OnNMCustomdrawCalloutSliderPeriod(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here
    int iPos = m_sldPeriod.GetPos();
    m_spnPeriod.SetPos(iPos / 1000.0);
    *pResult = 0;
}

void CInteractionButtonDialog::OnDeltaposCalloutSpinPeriod(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
    float fPos = m_spnPeriod.GetPos();
    int iSldPos = fPos * 1000;
    m_sldPeriod.SetPos(iSldPos);
    *pResult = 0;
}

void CInteractionButtonDialog::OnEnUpdateCalloutEditPeriod()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function to send the EM_SETEVENTMASK message to the control
    // with the ENM_UPDATE flag ORed into the lParam mask.

    // TODO:  Add your control notification handler code here
    int iInitialPos = m_sldPeriod.GetPos();
    int iCursorPos = m_edtPeriod .GetSel();
    double fInitValue = m_spnPeriod.GetPos();

    int iChangedPos = fInitValue*1000;
    m_sldPeriod.SetPos(iChangedPos);


    if(iInitialPos != iChangedPos) {
        m_edtPeriod.SetSel(iCursorPos + 1, iCursorPos + 1);
    }
}

void CInteractionButtonDialog::OnNMReleasedcaptureCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    int iPos = m_sldTime.GetPos();

    SetPeriod(iPos);
    *pResult = 0;
}

void CInteractionButtonDialog::OnUpdateCustomTimeSettings(CCmdUI* pCmdUI) {
    int radioId = GetCheckedRadioButton(IDC_VB_ALL_PAGES, IDC_VB_NOT_ON_PAGES);
    if (radioId == IDC_VB_NOT_ON_PAGES)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CInteractionButtonDialog::OnUpdateCustomTimeSettings2(CCmdUI* pCmdUI) {
    int radioId = IsDlgButtonChecked(IDC_AC_CHOOSE);//GetCheckedRadioButton(IDC_AC_CHOOSE, IDC_AC_CHOOSE);
    UINT iAnalogVB = IsDlgButtonChecked(IDC_AC_ANALOG_VB);
    if (radioId !=0 && iAnalogVB == 0) {
        pCmdUI->Enable(TRUE);
    } else {
        pCmdUI->Enable(FALSE);
       m_sldPeriodA.SetPos(m_sldPeriod.GetPos());
        /*m_spnPeriodA.SetPos(m_spnPeriod.GetPos());*/
    }
}

void CInteractionButtonDialog::RepositionControls() {
    if (m_bIsInitialized) {
        CRect rcWin;
        GetWindowRect(&rcWin);

        CRect rcOK;
        m_btnOK.GetWindowRect(&rcOK);
        CRect rcCancel;
        m_btnCancel.GetWindowRect(&rcCancel);
        CRect rcBack;
        m_stBottom.GetWindowRect(&rcBack);
        

        CRect rcControl;
        rcControl = rcOK;
        rcControl.top = rcWin.bottom - m_iButtonVOffset - rcOK.Height();
        rcControl.bottom = rcWin.bottom - m_iButtonVOffset;
        
        rcControl.left = rcOK.left;
        rcControl.right = rcOK.right;
        ScreenToClient(&rcControl);
        //m_btnOK.MoveWindow(&rcControl, TRUE);

        CRect rcControl1;
        rcControl1 = rcCancel;
        rcControl1.top = rcWin.bottom - m_iButtonVOffset - rcCancel.Height();
        rcControl1.bottom = rcWin.bottom - m_iButtonVOffset;
        
        rcControl1.left = rcCancel.left;
        rcControl1.right = rcCancel.right;
        ScreenToClient(&rcControl1);
        //m_btnCancel.MoveWindow(&rcControl, TRUE);

        CRect rcControl2;
        rcControl2 = rcBack;
        rcControl2.top = rcWin.bottom - m_iBackVOffset - rcBack.Height();
        rcControl2.bottom = rcWin.bottom - m_iBackVOffset;
        
        rcControl2.left = rcBack.left;
        rcControl2.right = rcBack.right;
        ScreenToClient(&rcControl2);
        //m_stBottom.MoveWindow(&rcControl, TRUE);
         

        HDWP hDefer = BeginDeferWindowPos(3);
        UINT uFlags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER;
        if(hDefer)
            DeferWindowPos(hDefer, m_btnOK.m_hWnd, NULL, rcControl.left, rcControl.top, rcControl.Width(), rcControl.Height(), uFlags);
        if(hDefer)
            DeferWindowPos(hDefer, m_btnCancel.m_hWnd, NULL, rcControl1.left, rcControl1.top, rcControl1.Width(), rcControl1.Height(), uFlags);
        if(hDefer)
            DeferWindowPos(hDefer, m_stBottom.m_hWnd, NULL, rcControl2.left, rcControl2.top, rcControl2.Width(), rcControl2.Height(), uFlags);
        

        EndDeferWindowPos(hDefer);
        InvalidateRect(&rcControl2);
    }
}

void CInteractionButtonDialog::OnSize(UINT nType, int cx, int cy)
{
    
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    m_pScrollHelper->OnSize(nType, cx, cy);

    //RepositionControls();
    
}

void CInteractionButtonDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Add your message handler code here and/or call default
    if (m_bIsInitialized) {
        lpMMI->ptMinTrackSize.x = m_szInitial.cx;
        lpMMI->ptMinTrackSize.y = 300;
        lpMMI->ptMaxTrackSize.x = m_szInitial.cx;
        lpMMI->ptMaxTrackSize.y = m_szInitial.cy;
    }

    CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CInteractionButtonDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Add your message handler code here and/or call default

    /*return CDialog::OnMouseWheel(nFlags, zDelta, pt);*/
    BOOL wasScrolled = m_pScrollHelper->OnMouseWheel(nFlags, zDelta, pt);
    /*if (wasScrolled)
        RepositionControls();*/
    return wasScrolled;
}

void CInteractionButtonDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default

    /*CDialog::OnVScroll(nSBCode, nPos, pScrollBar);*/
    m_pScrollHelper->OnVScroll(nSBCode, nPos, pScrollBar);
    //RepositionControls();
}

void CInteractionButtonDialog::OnNMCustomdrawCalloutSliderTime2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here
    int iPos = m_sldTimeA.GetPos();
    m_spnTimeA.SetPos(iPos / 1000.0);
    *pResult = 0;
}

void CInteractionButtonDialog::OnDeltaposCalloutSpinTime2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
    float fPos = m_spnTimeA.GetPos();
    int iSldPos = fPos * 1000;
    m_sldTimeA.SetPos(iSldPos);
    SetPeriodA(iSldPos);
    *pResult = 0;
}

void CInteractionButtonDialog::OnEnUpdateCalloutEditTime2()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function to send the EM_SETEVENTMASK message to the control
    // with the ENM_UPDATE flag ORed into the lParam mask.

    // TODO:  Add your control notification handler code here
    int iInitialPos = m_sldTimeA.GetPos();
    int iCursorPos = m_edtTimeA.GetSel();
    double fInitValue = m_spnTimeA.GetPos();

    int iChangedPos = fInitValue*1000;
    m_sldTimeA.SetPos(iChangedPos);


    if(iInitialPos != iChangedPos) {
        m_edtTimeA.SetSel(iCursorPos + 1, iCursorPos + 1);
    }
}

void CInteractionButtonDialog::OnNMCustomdrawCalloutSliderPeriod2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here
    int iPos = m_sldPeriodA.GetPos();
    m_spnPeriodA.SetPos(iPos / 1000.0);
    *pResult = 0;
}

void CInteractionButtonDialog::OnDeltaposCalloutSpinPeriod2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
    float fPos = m_spnPeriodA.GetPos();
    int iSldPos = fPos * 1000;
    m_sldPeriodA.SetPos(iSldPos);
    *pResult = 0;
}

void CInteractionButtonDialog::OnEnUpdateCalloutEditPeriod2()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function to send the EM_SETEVENTMASK message to the control
    // with the ENM_UPDATE flag ORed into the lParam mask.

    // TODO:  Add your control notification handler code here
    int iInitialPos = m_sldPeriodA.GetPos();
    int iCursorPos = m_edtPeriodA.GetSel();
    double fInitValue = m_spnPeriodA.GetPos();

    int iChangedPos = fInitValue*1000;
    m_sldPeriodA.SetPos(iChangedPos);


    if(iInitialPos != iChangedPos) {
        m_edtPeriodA.SetSel(iCursorPos + 1, iCursorPos + 1);
    }
}

void CInteractionButtonDialog::OnNMReleasedcaptureCalloutSliderTime2(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    int iPos = m_sldTimeA.GetPos();

    SetPeriodA(iPos);
    *pResult = 0;
}

void CInteractionButtonDialog::OnBnClickedIbuttonResetAll()
{
    // TODO: Add your control notification handler code here

    s_pButtonVisualSettings->ReInitialize();
    for(int iAction = 0; iAction < 4; iAction++) {
        if (m_wndChoosebuttonTypes.GetCurSel() == 0 ) {
            m_wndPreview[iAction].SetColors(s_pButtonVisualSettings->m_clrFill[iAction], s_pButtonVisualSettings->m_clrLine[iAction],
                s_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&s_pButtonVisualSettings->m_lf[iAction]);
        } else {
            m_wndPreview[iAction].SetTextColor(s_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&s_pButtonVisualSettings->m_lf[iAction]);
            m_wndPreview[iAction].SetImage(s_pButtonVisualSettings->m_csFileNames[iAction]);
            m_wndPreview[iAction].GetImageDimension(s_pButtonVisualSettings->m_siImageDimension[iAction]);
            m_wndPreview[iAction].RedrawWindow();
        }
    }
    
}

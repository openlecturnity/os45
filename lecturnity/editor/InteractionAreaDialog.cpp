// InteractionAreaDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "InteractionAreaDialog.h"
#include "editorDoc.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInteractionAreaDialog 


CInteractionAreaDialog::CInteractionAreaDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInteractionAreaDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInteractionAreaDialog)
	m_bSetStopmark = FALSE;
	m_bShowMouseCursor = FALSE;
	//}}AFX_DATA_INIT
	
	m_pInteractionArea = NULL;

	for (int i = 0; i < ACTION_COUNT; ++i)
		m_nMarkOrPageId[i] = 0;

	m_hBitmap = static_cast<HBITMAP>(LoadImage (AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BUTTON_DOWN),
									IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));

    m_bInitialPeriod = false;
    m_iCustomStartTime = -1;
    m_iCustomPeriod = -1;
    m_bIsInitialized = false;
    m_pScrollHelper = new CScrollHelper();
}

CInteractionAreaDialog::~CInteractionAreaDialog()
{
   DeleteObject(m_hBitmap);
   if(m_pScrollHelper)
        delete m_pScrollHelper;
}

void CInteractionAreaDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CInteractionAreaDialog)
    DDX_Control(pDX, IDC_POPUP_ACTIVATION_INTERN, m_btnActivationIntern);
    DDX_Control(pDX, IDC_POPUP_MOUSEDOWN_INTERN, m_btnMouseDownIntern);
    DDX_Control(pDX, IDC_POPUP_MOUSEUP_INTERN, m_btnMouseUpIntern);
    DDX_Control(pDX, IDC_POPUP_MOUSEIN_INTERN, m_btnMouseEnterIntern);
    DDX_Control(pDX, IDC_POPUP_MOUSEOUT_INTERN, m_btnMouseLeaveIntern);
    DDX_Control(pDX, IDC_AREA_ACTIVATION_ACTION, m_wndClickAction);
    DDX_Control(pDX, IDC_AREA_MOUSEDOWN_ACTION, m_wndDownAction);
    DDX_Control(pDX, IDC_AREA_MOUSEUP_ACTION, m_wndUpAction);
    DDX_Control(pDX, IDC_AREA_MOUSEIN_ACTION, m_wndEnterAction);
    DDX_Control(pDX, IDC_AREA_MOUSEOUT_ACTION, m_wndLeaveAction);
    DDX_Check(pDX, IDC_SET_STOPMARK, m_bSetStopmark);
    DDX_Check(pDX, IDC_SHOW_MOUSECURSOR, m_bShowMouseCursor);
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_ACTIV_START_REPLAY, m_cbActivStartReplay);
    DDX_Control(pDX, IDC_ACTIV_STOP_REPLAY, m_cbActivStopReplay);
    DDX_Control(pDX, IDC_MOUSEUP_STOP_REPLAY, m_cbMouseupStopReplay);
    DDX_Control(pDX, IDC_MOUSEUP_START_REPLAY, m_cbMouseupStartReplay);
    DDX_Control(pDX, IDC_MOUSEDOWN_STOP_REPLAY, m_cbMousedownStopReplay);
    DDX_Control(pDX, IDC_MOUSEDOWN_START_REPLAY, m_cbMousedownStartReplay);
    DDX_Control(pDX, IDC_MOUSEIN_START_REPLAY, m_cbMouseinStartReplay);
    DDX_Control(pDX, IDC_MOUSEIN_STOP_REPLAY, m_cbMouseinStopReplay);
    DDX_Control(pDX, IDC_MOUSEOUT_START_REPLAY, m_cbMouseoutStartReplay);
    DDX_Control(pDX, IDC_MOUSEOUT_STOP_REPLAY, m_cbMouseoutStopReplay);
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
}


BEGIN_MESSAGE_MAP(CInteractionAreaDialog, CDialog)
	//{{AFX_MSG_MAP(CInteractionAreaDialog)
	ON_MESSAGE_VOID( WM_IDLEUPDATECMDUI, OnIdleUpdateUIMessage )

	ON_BN_CLICKED(IDC_POPUP_ACTIVATION_INTERN, OnPopupIntern)
	ON_BN_CLICKED(IDC_POPUP_MOUSEDOWN_INTERN, OnPopupIntern)
	ON_BN_CLICKED(IDC_POPUP_MOUSEUP_INTERN, OnPopupIntern)
	ON_BN_CLICKED(IDC_POPUP_MOUSEIN_INTERN, OnPopupIntern)
	ON_BN_CLICKED(IDC_POPUP_MOUSEOUT_INTERN, OnPopupIntern)

	ON_BN_CLICKED(IDC_POPUP_ACTIVATION_EXTERN, OnPopupExtern)
	ON_BN_CLICKED(IDC_POPUP_MOUSEDOWN_EXTERN, OnPopupExtern)
	ON_BN_CLICKED(IDC_POPUP_MOUSEUP_EXTERN, OnPopupExtern)
	ON_BN_CLICKED(IDC_POPUP_MOUSEIN_EXTERN, OnPopupExtern)
	ON_BN_CLICKED(IDC_POPUP_MOUSEOUT_EXTERN, OnPopupExtern)

	ON_UPDATE_COMMAND_UI(IDC_POPUP_ACTIVATION_INTERN, OnUpdatePopupIntern)
	ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEDOWN_INTERN, OnUpdatePopupIntern)
	ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEUP_INTERN, OnUpdatePopupIntern)
	ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEIN_INTERN, OnUpdatePopupIntern)
	ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEOUT_INTERN, OnUpdatePopupIntern)

	ON_UPDATE_COMMAND_UI(IDC_POPUP_ACTIVATION_EXTERN, OnUpdatePopupExtern)
	ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEDOWN_EXTERN, OnUpdatePopupExtern)
	ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEUP_EXTERN, OnUpdatePopupExtern)
	ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEIN_EXTERN, OnUpdatePopupExtern)
	ON_UPDATE_COMMAND_UI(IDC_POPUP_MOUSEOUT_EXTERN, OnUpdatePopupExtern)

	ON_UPDATE_COMMAND_UI(IDC_ACTIVATION_DESTINATION, OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(IDC_MOUSEDOWN_DESTINATION, OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(IDC_MOUSEUP_DESTINATION, OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(IDC_MOUSEIN_DESTINATION, OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(IDC_MOUSEOUT_DESTINATION, OnUpdateEdit)

	ON_CBN_SELCHANGE(IDC_AREA_ACTIVATION_ACTION, OnSelchangeAreaActivationAction)
	ON_CBN_SELCHANGE(IDC_AREA_MOUSEDOWN_ACTION, OnSelchangeAreaMousedownAction)
	ON_CBN_SELCHANGE(IDC_AREA_MOUSEUP_ACTION, OnSelchangeAreaMouseupAction)
	ON_CBN_SELCHANGE(IDC_AREA_MOUSEIN_ACTION, OnSelchangeAreaMouseinAction)
	ON_CBN_SELCHANGE(IDC_AREA_MOUSEOUT_ACTION, OnSelchangeAreaMouseoutAction)

    ON_UPDATE_COMMAND_UI_RANGE(IDC_CALLOUT_SLIDER_TIME, IDC_CALLOUT_LBL_PLVALUE,OnUpdateCustomTimeSettings)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ACTIV_START_REPLAY, &CInteractionAreaDialog::OnBnClickedActivStartReplay)
	ON_BN_CLICKED(IDC_ACTIV_STOP_REPLAY, &CInteractionAreaDialog::OnBnClickedActivStopReplay)
	ON_BN_CLICKED(IDC_MOUSEUP_START_REPLAY, &CInteractionAreaDialog::OnBnClickedMouseupStartReplay)
	ON_BN_CLICKED(IDC_MOUSEUP_STOP_REPLAY, &CInteractionAreaDialog::OnBnClickedMouseupStopReplay)
	ON_BN_CLICKED(IDC_MOUSEDOWN_STOP_REPLAY, &CInteractionAreaDialog::OnBnClickedMousedownStopReplay)
	ON_BN_CLICKED(IDC_MOUSEDOWN_START_REPLAY, &CInteractionAreaDialog::OnBnClickedMousedownStartReplay)
	ON_BN_CLICKED(IDC_MOUSEIN_START_REPLAY, &CInteractionAreaDialog::OnBnClickedMouseinStartReplay)
	ON_BN_CLICKED(IDC_MOUSEIN_STOP_REPLAY, &CInteractionAreaDialog::OnBnClickedMouseinStopReplay)
	ON_BN_CLICKED(IDC_MOUSEOUT_START_REPLAY, &CInteractionAreaDialog::OnBnClickedMouseoutStartReplay)
	ON_BN_CLICKED(IDC_MOUSEOUT_STOP_REPLAY, &CInteractionAreaDialog::OnBnClickedMouseoutStopReplay)

    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_TIME, &CInteractionAreaDialog::OnNMCustomdrawCalloutSliderTime)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_TIME, &CInteractionAreaDialog::OnDeltaposCalloutSpinTime)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_TIME, &CInteractionAreaDialog::OnEnUpdateCalloutEditTime)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLOUT_SLIDER_PERIOD, &CInteractionAreaDialog::OnNMCustomdrawCalloutSliderPeriod)
    ON_NOTIFY(UDN_DELTAPOS, IDC_CALLOUT_SPIN_PERIOD, &CInteractionAreaDialog::OnDeltaposCalloutSpinPeriod)
    ON_EN_UPDATE(IDC_CALLOUT_EDIT_PERIOD, &CInteractionAreaDialog::OnEnUpdateCalloutEditPeriod)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_CALLOUT_SLIDER_TIME, &CInteractionAreaDialog::OnNMReleasedcaptureCalloutSliderTime)
    ON_WM_GETMINMAXINFO()
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


void CInteractionAreaDialog::Init(CInteractionAreaEx *pInteractionArea, CEditorDoc *pEditorDoc)
{
	m_pInteractionArea = pInteractionArea;
	m_pEditorDoc = pEditorDoc;
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CInteractionAreaDialog 

BOOL CInteractionAreaDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();

    CString csTitle;
    csTitle.LoadString(CInteractionAreaDialog::IDD);
    SetWindowText(csTitle);

    DWORD dwIds[] = {IDC_ACTION_AC,
        IDC_ACTION_MU,
        IDC_ACTION_MD,
        IDC_ACTION_MI,
        IDC_ACTION_MO,
        IDC_IF_ACTIVATED,
        IDC_IF_MOUSEUP,
        IDC_IF_MOUSEDOWN,
        IDC_IF_MOUSEIN,
        IDC_IF_MOUSEOUT,
        IDC_INTERACTIONAREA_ACTIVE,
        IDC_AC_ALL_PAGES,
        IDC_AC_CURRENT_PAGE,
        IDC_AC_CURRENT_TIMESTAMP,
        IDC_AC_END_OF_PAGE,
        IDC_STOP_REPLAY,
        IDC_SET_STOPMARK,
        IDC_SHOW_MOUSECURSOR,
        IDC_ACTIV_START_REPLAY,
        IDC_ACTIV_STOP_REPLAY,
        IDC_MOUSEIN_START_REPLAY,
        IDC_MOUSEIN_STOP_REPLAY,
        IDC_MOUSEOUT_START_REPLAY,
        IDC_MOUSEOUT_STOP_REPLAY,
        IDC_MOUSEUP_START_REPLAY,
        IDC_MOUSEUP_STOP_REPLAY,
        IDC_MOUSEDOWN_START_REPLAY,
        IDC_MOUSEDOWN_STOP_REPLAY,
        IDC_CALLOUT_LBL_DTIME,
        IDC_CALLOUT_LBL_DPERIOD,
        IDC_CALLOUT_TIME_SEC,          
        IDC_CALLOUT_PERIOD_SEC,
        IDOK,
        IDCANCEL,
        -1};
    MfcUtils::Localize(this, dwIds);

    CString csCustom;
    csCustom.LoadString(IDS_PAGE_SIZE_CUSTOM);
    GetDlgItem(IDC_AC_CHOOSE)->SetWindowText(csCustom);

    m_btnActivationIntern.SetBitmap(m_hBitmap);
    m_btnMouseDownIntern.SetBitmap(m_hBitmap);
    m_btnMouseUpIntern.SetBitmap(m_hBitmap);
    m_btnMouseEnterIntern.SetBitmap(m_hBitmap);
    m_btnMouseLeaveIntern.SetBitmap(m_hBitmap);

    CStringArray aActionStrings;
    CMouseAction::GetActionStrings(aActionStrings, m_pEditorDoc && m_pEditorDoc->project.FirstQuestionnaireIsRandomTest());
    for (int i = 0; i < aActionStrings.GetSize(); ++i)
    {
        m_wndClickAction.AddString(aActionStrings[i]);
        m_wndUpAction.AddString(aActionStrings[i]);
        m_wndDownAction.AddString(aActionStrings[i]);
        m_wndEnterAction.AddString(aActionStrings[i]);
        m_wndLeaveAction.AddString(aActionStrings[i]);

        int iItemData = i;
        if (!(m_pEditorDoc && m_pEditorDoc->project.FirstQuestionnaireIsRandomTest()) && i >= INTERACTION_JUMP_RANDOM_QUESTION) {
            iItemData = i + 1;
        }

        m_wndClickAction.SetItemData(i, iItemData);
        m_wndUpAction.SetItemData(i, iItemData);
        m_wndDownAction.SetItemData(i, iItemData);
        m_wndEnterAction.SetItemData(i, iItemData);
        m_wndLeaveAction.SetItemData(i, iItemData);
    }

    int iStart = 0;
    if (m_pInteractionArea) {
        if (m_pInteractionArea->GetClickAction()) {
            UpdateMouseAction(ACTION_ACTIVATION, IDC_ACTIVATION_DESTINATION, m_pInteractionArea->GetClickAction(), &m_wndClickAction, &m_cbActivStartReplay, &m_cbActivStopReplay);
        } else {
            UpdateMouseAction(&m_wndClickAction, &m_cbActivStartReplay, &m_cbActivStopReplay);
        }

        if (m_pInteractionArea->GetUpAction()) {
            UpdateMouseAction(ACTION_MOUSEUP, IDC_MOUSEUP_DESTINATION, m_pInteractionArea->GetUpAction(), &m_wndUpAction, &m_cbMouseupStartReplay, &m_cbMouseupStopReplay);
        } else {
            UpdateMouseAction(&m_wndUpAction, &m_cbMouseupStartReplay, &m_cbMouseupStopReplay);
        }

        if (m_pInteractionArea->GetDownAction()) {
            UpdateMouseAction(ACTION_MOUSEDOWN, IDC_MOUSEDOWN_DESTINATION, m_pInteractionArea->GetDownAction(), &m_wndDownAction, &m_cbMousedownStartReplay, &m_cbMousedownStopReplay);
        } else {
            UpdateMouseAction(&m_wndDownAction, &m_cbMousedownStartReplay, &m_cbMousedownStopReplay);
        }

        if (m_pInteractionArea->GetEnterAction()) {
            UpdateMouseAction(ACTION_MOUSEIN, IDC_MOUSEIN_DESTINATION, m_pInteractionArea->GetEnterAction(), &m_wndEnterAction, &m_cbMouseinStartReplay, &m_cbMouseinStopReplay);
        } else {
            UpdateMouseAction(&m_wndEnterAction, &m_cbMouseinStartReplay, &m_cbMouseinStopReplay);
        }

        if (m_pInteractionArea->GetLeaveAction()) {
            UpdateMouseAction(ACTION_MOUSEOUT, IDC_MOUSEOUT_DESTINATION, m_pInteractionArea->GetLeaveAction(), &m_wndLeaveAction, &m_cbMouseoutStartReplay, &m_cbMouseoutStopReplay);
        } else {
            UpdateMouseAction(&m_wndLeaveAction, &m_cbMouseoutStartReplay, &m_cbMouseoutStopReplay);
        }


        AreaPeriodId periodId = m_pInteractionArea->GetActivityPeriod();
        AreaPeriodId periodVisible = m_pInteractionArea->GetVisibilityPeriod();
        iStart = m_pInteractionArea->GetVisibilityStart();
        int iLength = m_pInteractionArea->GetVisibilityLength();
        int iRadioButtonId = IDC_AC_CURRENT_PAGE;
        switch (periodId)
        {
        case INTERACTION_PERIOD_PAGE:
            iRadioButtonId = IDC_AC_CURRENT_PAGE;
            break;
        case INTERACTION_PERIOD_END_PAGE:
            iRadioButtonId = IDC_AC_END_OF_PAGE;
            break;
        case INTERACTION_PERIOD_TIME:
            iRadioButtonId = IDC_AC_CURRENT_TIMESTAMP;
            break;
        case INTERACTION_PERIOD_ALL_PAGES:
            iRadioButtonId = IDC_AC_ALL_PAGES;
            break;
        }
        if (periodVisible == INTERACTION_PERIOD_TIME && iLength != 1)
            iRadioButtonId = IDC_AC_CHOOSE;

        CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_CHOOSE, iRadioButtonId);

        m_bShowMouseCursor = m_pInteractionArea->IsHandCursor() == true ? TRUE : FALSE;
    }
    else
    {
        m_wndClickAction.SetCurSel(INTERACTION_JUMP_NEXT_PAGE);
        m_wndUpAction.SetCurSel(INTERACTION_NO_ACTION);
        m_wndDownAction.SetCurSel(INTERACTION_NO_ACTION);
        m_wndEnterAction.SetCurSel(INTERACTION_NO_ACTION);
        m_wndLeaveAction.SetCurSel(INTERACTION_NO_ACTION);
        CheckRadioButton(IDC_AC_ALL_PAGES, IDC_AC_CHOOSE, IDC_AC_CHOOSE);
        iStart = m_pEditorDoc->GetCurrentPreviewPos();

        m_bShowMouseCursor = FALSE;
    }

    m_spnTime.SetRangeAndDelta(0, m_pEditorDoc->m_displayEndMs / 1000.0, 0.001);
    m_spnTime.SetBuddy(&m_edtTime);
    m_spnTime.SetPos(iStart / 1000.0);
    m_spnTime.SetDecimalPlaces(3);

    m_sldTime.SetRange(0, m_pEditorDoc->m_displayEndMs);
    m_sldTime.SetTicFreq(1);
    m_sldTime.SetPos(iStart);

    m_spnPeriod.SetBuddy(&m_edtPeriod);
    m_spnPeriod.SetRangeAndDelta(0.01,  m_pEditorDoc->m_displayEndMs / 1000.0, 0.001);
    m_sldPeriod.SetRange(10, m_pEditorDoc->m_displayEndMs);
    m_sldPeriod.SetTicFreq(1);
    m_bInitialPeriod = true;
    SetPeriod(iStart);
    m_bInitialPeriod = false;

    CButton *pCheckButton = (CButton *)GetDlgItem(IDC_SHOW_MOUSECURSOR);
    pCheckButton->SetCheck(m_bShowMouseCursor);

   /* CRect rcOK;
    m_btnOK.GetWindowRect(&rcOK);
    CRect rcBack;
    m_stBottom.GetWindowRect(&rcBack);*/
   

    int screenY = GetSystemMetrics(SM_CYMAXIMIZED);

    CRect rcWin;
    GetWindowRect(&rcWin);
    /*m_iButtonVOffset = rcWin.bottom - rcOK.bottom;
    m_iBackVOffset = rcWin.bottom - rcBack.bottom;*/

    m_szInitial = rcWin.Size();
    
   /* if( screenY < rcWin.Height() + 40) {
        rcWin.top = 20;
        rcWin.bottom = screenY - 20;
    }*/
    
    m_pScrollHelper->AttachWnd(this);
    m_pScrollHelper->SetDisplaySize(m_szInitial.cx - 20, m_szInitial.cy );
    m_bIsInitialized = true;

   /* MoveWindow(&rcWin, TRUE);
    CenterWindow();*/

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CInteractionAreaDialog::SetPeriod(int iDisplayTime) {
    int iRemainingPeriod = m_pEditorDoc->m_displayEndMs - iDisplayTime + 1;
    UINT nStartMs, nEndMs;
    m_pEditorDoc->project.GetPagePeriod(iDisplayTime, nStartMs, nEndMs);

    int m_iDisplayPeriod = 1;
    if (m_pInteractionArea)
        m_iDisplayPeriod = m_pInteractionArea->GetVisibilityLength() - 1;
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
    csMaxPeriod.Format(_T("%.3f"), m_pEditorDoc->m_displayEndMs / 1000.0);
    m_lblMaxPeriod.SetWindowText(csMaxPeriod);
}

void CInteractionAreaDialog::UpdateMouseAction(int actionId, UINT nControlID, CMouseAction *pMouseAction, CComboBox *pComboBoxWnd, CButton *pStartCheckBox, CButton *pStopCheckBox) {
    if (!(pComboBoxWnd && pStartCheckBox && pStopCheckBox)) {
        ASSERT(FALSE);
        return;
    }
    m_nActionTypeId[actionId] = pMouseAction->GetActionId();
    if (pMouseAction->GetNextAction())
        m_nNextActionTypeId[actionId] = pMouseAction->GetNextAction()->GetActionId();
    else
        m_nNextActionTypeId[actionId] = INTERACTION_NO_ACTION;

    for (int i =0; i< pComboBoxWnd->GetCount(); i++) {
        if (pComboBoxWnd->GetItemData(i) == m_nActionTypeId[actionId]) {
            pComboBoxWnd->SetCurSel(i);
            break;
        }
    }

    if (m_nNextActionTypeId[actionId] == INTERACTION_START_REPLAY || m_nActionTypeId[actionId] == INTERACTION_JUMP_RANDOM_QUESTION) {
        pStartCheckBox->SetCheck(1);
        pStopCheckBox->SetCheck(0);
    } else if (m_nNextActionTypeId[actionId] == INTERACTION_STOP_REPLAY) {
        pStartCheckBox->SetCheck(0);
        pStopCheckBox->SetCheck(1);
    } else {
        pStartCheckBox->SetCheck(0);
        pStopCheckBox->SetCheck(0);
    }

    bool bDisableCheckBox = (m_nActionTypeId[actionId] == INTERACTION_NO_ACTION || m_nActionTypeId[actionId] == INTERACTION_START_REPLAY || m_nActionTypeId[actionId] == INTERACTION_STOP_REPLAY || m_nActionTypeId[actionId] == INTERACTION_EXIT_PROCESS || m_nActionTypeId[actionId] == INTERACTION_JUMP_RANDOM_QUESTION);
    pStartCheckBox->EnableWindow(bDisableCheckBox ? 0 : 1);
    pStopCheckBox->EnableWindow(bDisableCheckBox ? 0 : 1);

    SetActionText(pMouseAction, nControlID);
    m_nMarkOrPageId[actionId] = pMouseAction->GetPageMarkId();
}

void CInteractionAreaDialog::UpdateMouseAction(CComboBox *pComboBoxWnd, CButton *pStartCheckBox, CButton *pStopCheckBox) {
    pComboBoxWnd->SetCurSel(INTERACTION_NO_ACTION);
    pStartCheckBox->SetCheck(0);
    pStopCheckBox->SetCheck(0);
    pStartCheckBox->EnableWindow(0);
    pStopCheckBox->EnableWindow(0);
}

void CInteractionAreaDialog::OnIdleUpdateUIMessage()
{
	UpdateDialogControls( this, TRUE );
}

BOOL CInteractionAreaDialog::ContinueModal()
{
    SendMessage( WM_IDLEUPDATECMDUI, WPARAM(TRUE), 0);

    return CDialog::ContinueModal();
}

void CInteractionAreaDialog::OnPopupIntern() 
{
	const MSG* pMsg = GetCurrentMessage( );

	UINT nAction = 0;
	AreaActionTypeId nActionTypeId = INTERACTION_NO_ACTION;
	UINT nButtonId = 0;
	UINT nEditId = 0;
	switch (WPARAM(pMsg->wParam))
	{
	case IDC_POPUP_ACTIVATION_INTERN:
		nAction = ACTION_ACTIVATION;
		nActionTypeId = (AreaActionTypeId)m_wndClickAction.GetItemData(m_wndClickAction.GetCurSel());
		nButtonId = IDC_POPUP_ACTIVATION_INTERN;
		nEditId = IDC_ACTIVATION_DESTINATION;
		break;
	case IDC_POPUP_MOUSEDOWN_INTERN:
		nAction = ACTION_MOUSEDOWN;
		nActionTypeId = (AreaActionTypeId)m_wndDownAction.GetItemData(m_wndDownAction.GetCurSel());
		nButtonId = IDC_POPUP_MOUSEDOWN_INTERN;
		nEditId = IDC_MOUSEDOWN_DESTINATION;
		break;
	case IDC_POPUP_MOUSEUP_INTERN:
		nAction = ACTION_MOUSEUP;
		nActionTypeId = (AreaActionTypeId)m_wndUpAction.GetItemData(m_wndUpAction.GetCurSel());
		nButtonId = IDC_POPUP_MOUSEUP_INTERN;
		nEditId = IDC_MOUSEUP_DESTINATION;
		break;
	case IDC_POPUP_MOUSEIN_INTERN:
		nAction = ACTION_MOUSEIN;
		nActionTypeId = (AreaActionTypeId)m_wndEnterAction.GetItemData(m_wndEnterAction.GetCurSel());
		nButtonId = IDC_POPUP_MOUSEIN_INTERN;
		nEditId = IDC_MOUSEIN_DESTINATION;
		break;
	case IDC_POPUP_MOUSEOUT_INTERN:
		nAction = ACTION_MOUSEOUT;
		nActionTypeId = (AreaActionTypeId)m_wndLeaveAction.GetItemData(m_wndLeaveAction.GetCurSel());
		nButtonId = IDC_POPUP_MOUSEOUT_INTERN;
		nEditId = IDC_MOUSEOUT_DESTINATION;
		break;
	default:
		return;
	}
	
	if (nActionTypeId == INTERACTION_JUMP_SPECIFIC_PAGE/* ||
       nActionStringId == SPECIFIC_PAGE_STARTREPLAY ||
       nActionStringId == SPECIFIC_PAGE_STOPREPLAY*/)
		CreateSlidePopup(nAction, nButtonId, nEditId);
	else if (nActionTypeId == INTERACTION_JUMP_TARGET_MARK/* ||
            nActionStringId == TARGET_MARK_STARTREPLAY ||
            nActionStringId == TARGET_MARK_STOPREPLAY*/)
		CreateTargetmarkPopup(nAction, nButtonId, nEditId);
}

void CInteractionAreaDialog::OnPopupExtern() 
{
	const MSG* pMsg = GetCurrentMessage( );

	AreaActionTypeId nActionTypeId = INTERACTION_NO_ACTION;
	UINT nEditId = 0;
	switch (WPARAM(pMsg->wParam))
	{
	case IDC_POPUP_ACTIVATION_EXTERN:
		nActionTypeId = (AreaActionTypeId)m_wndClickAction.GetItemData(m_wndClickAction.GetCurSel());
		nEditId = IDC_ACTIVATION_DESTINATION;
		break;
	case IDC_POPUP_MOUSEDOWN_EXTERN:
		nActionTypeId = (AreaActionTypeId)m_wndDownAction.GetItemData(m_wndDownAction.GetCurSel());
		nEditId = IDC_MOUSEDOWN_DESTINATION;
		break;
	case IDC_POPUP_MOUSEUP_EXTERN:
		nActionTypeId = (AreaActionTypeId)m_wndUpAction.GetItemData(m_wndUpAction.GetCurSel());
		nEditId = IDC_MOUSEUP_DESTINATION;
		break;
	case IDC_POPUP_MOUSEIN_EXTERN:
		nActionTypeId = (AreaActionTypeId)m_wndEnterAction.GetItemData(m_wndEnterAction.GetCurSel());
		nEditId = IDC_MOUSEIN_DESTINATION;
		break;
	case IDC_POPUP_MOUSEOUT_EXTERN:
		nActionTypeId = (AreaActionTypeId)m_wndLeaveAction.GetItemData(m_wndLeaveAction.GetCurSel());
		nEditId = IDC_MOUSEOUT_DESTINATION;
		break;
	default:
		return;
	}

	CString csFilter;
	if (nActionTypeId == INTERACTION_OPEN_URL)
		csFilter.LoadString(IDS_HTML_FILTER);
	else 
		csFilter.LoadString(IDS_ALL_FILES);

	CFileDialog *pFileDialog = new CFileDialog(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, csFilter, this);
	pFileDialog->DoModal();

	CString csFileName = pFileDialog->GetPathName();
	
	CEdit *pEdit = (CEdit *)GetDlgItem(nEditId);
	if (pEdit)
		pEdit->SetWindowText(csFileName);

	delete pFileDialog;

}

void CInteractionAreaDialog::OnUpdatePopupIntern(CCmdUI *pCmdUI)
{
	bool bEnable = false;

	AreaActionTypeId nActionTypeId = INTERACTION_NO_ACTION;
	if (pCmdUI->m_nID == IDC_POPUP_ACTIVATION_INTERN)
		nActionTypeId = (AreaActionTypeId)m_wndClickAction.GetItemData(m_wndClickAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_POPUP_MOUSEDOWN_INTERN)
		nActionTypeId = (AreaActionTypeId)m_wndDownAction.GetItemData(m_wndDownAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_POPUP_MOUSEUP_INTERN)
		nActionTypeId = (AreaActionTypeId)m_wndUpAction.GetItemData(m_wndUpAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_POPUP_MOUSEIN_INTERN)
		nActionTypeId = (AreaActionTypeId)m_wndEnterAction.GetItemData(m_wndEnterAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_POPUP_MOUSEOUT_INTERN)
		nActionTypeId = (AreaActionTypeId)m_wndLeaveAction.GetItemData(m_wndLeaveAction.GetCurSel());

	if (nActionTypeId == INTERACTION_JUMP_SPECIFIC_PAGE || 
       /*nActionStringId == SPECIFIC_PAGE_STARTREPLAY || 
       nActionStringId == SPECIFIC_PAGE_STOPREPLAY || */
		 nActionTypeId == INTERACTION_JUMP_TARGET_MARK /*||
       nActionStringId == TARGET_MARK_STARTREPLAY || 
       nActionStringId == TARGET_MARK_STOPREPLAY*/)
		bEnable = true;

   pCmdUI->Enable(bEnable);
}

void CInteractionAreaDialog::OnUpdatePopupExtern(CCmdUI *pCmdUI)
{
	bool bEnable = false;

	AreaActionTypeId nActionTypeId = INTERACTION_NO_ACTION;
	if (pCmdUI->m_nID == IDC_POPUP_ACTIVATION_EXTERN)
		nActionTypeId = (AreaActionTypeId)m_wndClickAction.GetItemData(m_wndClickAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_POPUP_MOUSEDOWN_EXTERN)
		nActionTypeId = (AreaActionTypeId)m_wndDownAction.GetItemData(m_wndDownAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_POPUP_MOUSEUP_EXTERN)
		nActionTypeId = (AreaActionTypeId)m_wndUpAction.GetItemData(m_wndUpAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_POPUP_MOUSEIN_EXTERN)
		nActionTypeId = (AreaActionTypeId)m_wndEnterAction.GetItemData(m_wndEnterAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_POPUP_MOUSEOUT_EXTERN)
		nActionTypeId = (AreaActionTypeId)m_wndLeaveAction.GetItemData(m_wndLeaveAction.GetCurSel());

	if (nActionTypeId == INTERACTION_OPEN_URL || nActionTypeId == INTERACTION_OPEN_FILE)
		bEnable = true;
	
   pCmdUI->Enable(bEnable);
}

void CInteractionAreaDialog::OnUpdateEdit(CCmdUI *pCmdUI)
{
	bool bEnable = false;

	AreaActionTypeId nActionTypeId = INTERACTION_NO_ACTION;
	if (pCmdUI->m_nID == IDC_ACTIVATION_DESTINATION)
		nActionTypeId = (AreaActionTypeId)m_wndClickAction.GetItemData(m_wndClickAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_MOUSEUP_DESTINATION)
		nActionTypeId = (AreaActionTypeId)m_wndUpAction.GetItemData(m_wndUpAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_MOUSEDOWN_DESTINATION)
		nActionTypeId = (AreaActionTypeId)m_wndDownAction.GetItemData(m_wndDownAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_MOUSEIN_DESTINATION)
		nActionTypeId = (AreaActionTypeId)m_wndEnterAction.GetItemData(m_wndEnterAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_MOUSEOUT_DESTINATION)
		nActionTypeId = (AreaActionTypeId)m_wndLeaveAction.GetItemData(m_wndLeaveAction.GetCurSel());

	if (nActionTypeId == INTERACTION_JUMP_SPECIFIC_PAGE || 
		nActionTypeId == INTERACTION_JUMP_TARGET_MARK ||
		nActionTypeId == INTERACTION_OPEN_URL || 
		nActionTypeId == INTERACTION_OPEN_FILE)
		bEnable = true;

   pCmdUI->Enable(bEnable);
}

void CInteractionAreaDialog::UpdateControlActions(int nAction, UINT nControlID, CButton *pStartReplayCheckBox, CButton *pStopReplayCheckBox) {
    CEdit *pEditWnd = (CEdit *)GetDlgItem(nControlID);
    if (pEditWnd)
        pEditWnd->SetWindowText(_T(""));
    pStartReplayCheckBox->SetCheck(nAction == INTERACTION_JUMP_RANDOM_QUESTION ? 1 : 0);
    pStopReplayCheckBox->SetCheck(0);

    bool bDisableCheckboxes = (nAction == INTERACTION_NO_ACTION || nAction == INTERACTION_START_REPLAY || nAction == INTERACTION_STOP_REPLAY || nAction == INTERACTION_EXIT_PROCESS || nAction == INTERACTION_JUMP_RANDOM_QUESTION);
    pStartReplayCheckBox->EnableWindow(bDisableCheckboxes ? 0 : 1);
    pStopReplayCheckBox->EnableWindow(bDisableCheckboxes ? 0 : 1);
}

void CInteractionAreaDialog::OnSelchangeAreaActivationAction() {
	static int nLastAction = -1;

	AreaActionTypeId nAction = (AreaActionTypeId)m_wndClickAction.GetItemData(m_wndClickAction.GetCurSel());
    if (nAction != nLastAction) {
        UpdateControlActions(nAction, IDC_ACTIVATION_DESTINATION, &m_cbActivStartReplay, &m_cbActivStopReplay);
    }	
	nLastAction = nAction;
}

void CInteractionAreaDialog::OnSelchangeAreaMousedownAction() {
	static int nLastAction = -1;

	AreaActionTypeId nAction = (AreaActionTypeId)m_wndDownAction.GetItemData(m_wndDownAction.GetCurSel());
    if (nAction != nLastAction) {
		UpdateControlActions(nAction, IDC_MOUSEDOWN_DESTINATION, &m_cbMousedownStartReplay, &m_cbMousedownStopReplay);
    }
	nLastAction = nAction;
}

void CInteractionAreaDialog::OnSelchangeAreaMouseupAction() {
	static int nLastAction = -1;

	AreaActionTypeId nAction = (AreaActionTypeId)m_wndUpAction.GetItemData(m_wndUpAction.GetCurSel());
    if (nAction != nLastAction) {
        UpdateControlActions(nAction, IDC_MOUSEUP_DESTINATION, &m_cbMouseupStartReplay, &m_cbMouseupStopReplay);
    }
	nLastAction = nAction;
}

void CInteractionAreaDialog::OnSelchangeAreaMouseinAction() {
	static int nLastAction = -1;

	AreaActionTypeId nAction = (AreaActionTypeId)m_wndEnterAction.GetItemData(m_wndEnterAction.GetCurSel());
    if (nAction != nLastAction) {
        UpdateControlActions(nAction, IDC_MOUSEIN_DESTINATION, &m_cbMouseinStartReplay, &m_cbMouseinStopReplay);
    }
	nLastAction = nAction;
}

void CInteractionAreaDialog::OnSelchangeAreaMouseoutAction() {
	static int nLastAction = -1;

	AreaActionTypeId nAction = (AreaActionTypeId)m_wndLeaveAction.GetItemData(m_wndLeaveAction.GetCurSel());
    if (nAction != nLastAction) {
        UpdateControlActions(nAction, IDC_MOUSEOUT_DESTINATION, &m_cbMouseoutStartReplay, &m_cbMouseoutStopReplay);
    }
	nLastAction = nAction;
}

void CInteractionAreaDialog::OnBtnClickOK()//OnOK() 
{
   // read the user settings and save it

   for (int i = 0; i < ACTION_COUNT; ++i)
	   ExtractActionParameters(i);

   int iRadioButtonId = GetCheckedRadioButton(IDC_AC_ALL_PAGES, IDC_AC_CHOOSE);
	m_periodId = INTERACTION_PERIOD_END_PAGE;
   CPage *pCurrentPage = NULL;
	switch (iRadioButtonId)
	{
	case IDC_AC_CURRENT_PAGE:
		m_periodId = INTERACTION_PERIOD_PAGE;
		break;
	case IDC_AC_END_OF_PAGE:
		m_periodId = INTERACTION_PERIOD_END_PAGE;
		break;
	case IDC_AC_CURRENT_TIMESTAMP:
		m_periodId = INTERACTION_PERIOD_TIME;
		break;
	case IDC_AC_ALL_PAGES:
		m_periodId = INTERACTION_PERIOD_ALL_PAGES;
		break;
    case IDC_AC_CHOOSE:
        m_periodId = INTERACTION_PERIOD_TIME;
        m_iCustomStartTime = m_sldTime.GetPos();
        m_iCustomPeriod = m_sldPeriod.GetPos();
        break;
	}

	CDialog::OnOK();
}

void CInteractionAreaDialog::OnOK()
{
    return;
}

// private functions

void CInteractionAreaDialog::CreateSlidePopup(int nAction, int nButtonId, int nEditId)
{
	CRect rcButton;
	CWnd *pWndButton = GetDlgItem(nButtonId);

	pWndButton->GetClientRect(rcButton);
	pWndButton->ClientToScreen(&rcButton);

	m_wndMenu.CreatePopupMenu();

	CArray<CPage *, CPage *> caPages;
	m_pEditorDoc->project.GetPages(caPages, 0, m_pEditorDoc->m_docLengthMs);
	for (int i = 0; i < caPages.GetSize(); ++i)
	{
		CString csPageTitle;
		csPageTitle = caPages[i]->GetTitle();
		m_wndMenu.AppendMenu(MF_STRING, i+1, csPageTitle);
	}
   
	BOOL bRet = m_wndMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, rcButton.left, rcButton.bottom, this);

	CWnd *pEdit = GetDlgItem(nEditId);
	// if bRet is zero the menu was cancelled 
	// or an error occured
	if (bRet != 0)
	{
		CString csItem;
		m_wndMenu.GetMenuString(bRet, csItem, MF_BYCOMMAND);
		pEdit->SetWindowText(csItem);
		m_nMarkOrPageId[nAction] = caPages[bRet-1]->GetJumpId();
	}

	caPages.RemoveAll();
	m_wndMenu.DestroyMenu();
}

void CInteractionAreaDialog::CreateTargetmarkPopup(int nAction, int nButtonId, int nEditId)
{
	CRect rcButton;
   CWnd *pWndButton = GetDlgItem(nButtonId);
   
   pWndButton->GetClientRect(rcButton);
   pWndButton->ClientToScreen(&rcButton);
   
   CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
   if (pMarkStream)
   {
      m_wndMenu.CreatePopupMenu();
      
      CArray<CStopJumpMark *, CStopJumpMark *> caJumpMarks;
      pMarkStream->GetJumpMarksInRegion(caJumpMarks, 0, m_pEditorDoc->m_docLengthMs);
      for (int i = 0; i < caJumpMarks.GetSize(); ++i)
      {
         CString csLabel;
         csLabel = caJumpMarks[i]->GetLabel();
         m_wndMenu.AppendMenu(MF_STRING, i+1, csLabel);
      }
      
      BOOL bRet = m_wndMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, rcButton.left, rcButton.bottom, this);
      
      CWnd *pEdit = GetDlgItem(nEditId);
      // if bRet is zero the menu was cancelled 
      // or an error occured
      if (bRet != 0)
      {
         CString csItem;
         m_wndMenu.GetMenuString(bRet, csItem, MF_BYCOMMAND);
         pEdit->SetWindowText(csItem);
         m_nMarkOrPageId[nAction] = caJumpMarks[bRet-1]->GetId();
      }
      
      caJumpMarks.RemoveAll();
      
      m_wndMenu.DestroyMenu();
   }
}

void CInteractionAreaDialog::ExtractActionParameters(int nAction)
{	
   int nEditId = 0;
   AreaActionTypeId nActionTypeId, nNextActionTypeId;
   switch (nAction)
   {
   case ACTION_ACTIVATION:
      nEditId = IDC_ACTIVATION_DESTINATION;
      nActionTypeId = (AreaActionTypeId)m_wndClickAction.GetItemData(m_wndClickAction.GetCurSel());
	  if(m_cbActivStartReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_START_REPLAY;
	   else if (m_cbActivStopReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_STOP_REPLAY;
	   else
		   nNextActionTypeId = INTERACTION_NO_ACTION;
      break;
   case ACTION_MOUSEDOWN:
      nEditId = IDC_MOUSEDOWN_DESTINATION;
      nActionTypeId = (AreaActionTypeId)m_wndDownAction.GetItemData(m_wndDownAction.GetCurSel());
	   if(m_cbMousedownStartReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_START_REPLAY;
	   else if (m_cbMousedownStopReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_STOP_REPLAY;
	   else
		   nNextActionTypeId = INTERACTION_NO_ACTION;
      break;
   case ACTION_MOUSEUP:
      nEditId = IDC_MOUSEUP_DESTINATION;
      nActionTypeId = (AreaActionTypeId)m_wndUpAction.GetItemData(m_wndUpAction.GetCurSel());
	  if(m_cbMouseupStartReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_START_REPLAY;
	   else if (m_cbMouseupStopReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_STOP_REPLAY;
	   else
		   nNextActionTypeId = INTERACTION_NO_ACTION;
      break;
   case ACTION_MOUSEIN:
      nEditId = IDC_MOUSEIN_DESTINATION;
      nActionTypeId = (AreaActionTypeId)m_wndEnterAction.GetItemData(m_wndEnterAction.GetCurSel());
	  if(m_cbMouseinStartReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_START_REPLAY;
	   else if (m_cbMouseinStopReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_STOP_REPLAY;
	   else
		   nNextActionTypeId = INTERACTION_NO_ACTION;
      break;
   case ACTION_MOUSEOUT:
      nEditId = IDC_MOUSEOUT_DESTINATION;
      nActionTypeId = (AreaActionTypeId)m_wndLeaveAction.GetItemData(m_wndLeaveAction.GetCurSel());
	  if(m_cbMouseoutStartReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_START_REPLAY;
	   else if (m_cbMouseoutStopReplay.GetCheck())
		   nNextActionTypeId = INTERACTION_STOP_REPLAY;
	   else
		   nNextActionTypeId = INTERACTION_NO_ACTION;
      break;
   }
   
   m_nActionTypeId[nAction] = nActionTypeId;
   m_nNextActionTypeId[nAction] = nNextActionTypeId;

   if (nActionTypeId == INTERACTION_OPEN_URL || nActionTypeId == INTERACTION_OPEN_FILE)
	{	
		CEdit *pEdit = (CEdit *)GetDlgItem(nEditId);
		if (pEdit)
			pEdit->GetWindowText(m_csActionPath[nAction]);
	}
}

void CInteractionAreaDialog::SetActionText(CMouseAction *pMouseAction, int nEditId)
{	
	CString csText;
	csText.Empty();

	UINT nPageOrMarkId = pMouseAction->GetPageMarkId();
	AreaActionTypeId nActionTypeId = pMouseAction->GetActionId();

	if (nActionTypeId == INTERACTION_JUMP_SPECIFIC_PAGE)
	{
      csText = m_pEditorDoc->project.GetPageTitleForId(nPageOrMarkId);
   }
	else if (nActionTypeId == INTERACTION_JUMP_TARGET_MARK)
	{
		CArray<CStopJumpMark *, CStopJumpMark *> caJumpMarks;
		CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
		pMarkStream->GetJumpMarksInRegion(caJumpMarks, 0, m_pEditorDoc->m_displayEndMs);
		for (int i = 0; i < caJumpMarks.GetSize(); ++i)
		{
			if (caJumpMarks[i]->GetId() == nPageOrMarkId)
			{
				csText = caJumpMarks[i]->GetLabel();
				break;
			}
		}
      caJumpMarks.RemoveAll();
	}
	else if (nActionTypeId == INTERACTION_OPEN_URL || nActionTypeId == INTERACTION_OPEN_FILE)
	{
		csText = pMouseAction->GetPath();
	}

	CEdit *pEdit = (CEdit *)GetDlgItem(nEditId);
	pEdit->SetWindowText(csText);
}


void CInteractionAreaDialog::SetUserEntries(CRect &rcArea, CInteractionAreaEx *pInteraction)
{
   CMouseAction *pMouseAction[ACTION_COUNT];
   for (int i = 0; i < 5; ++i)
   {
      if (m_nActionTypeId[i] != INTERACTION_NO_ACTION)
      {
         //AreaActionTypeId nNextAction = INTERACTION_NO_ACTION;
         //AreaActionTypeId nActionTypeId = CMouseAction::GetActionId(m_nActionStringId[i], nNextAction);
         pMouseAction[i] = new CMouseAction();
         pMouseAction[i]->Init(&m_pEditorDoc->project, m_nActionTypeId[i], m_nMarkOrPageId[i], m_csActionPath[i], m_nNextActionTypeId[i]);
      }
      else
         pMouseAction[i] = NULL;
   }
   
   UINT nActivityStartMs = 0;
   UINT nActivityEndMs = 0;

   UINT nCurrPos = m_pEditorDoc->m_curPosMs;
   UINT nEndPos = m_pEditorDoc->m_displayEndMs;

   switch (m_periodId)
   {
   case INTERACTION_PERIOD_PAGE:
      m_pEditorDoc->project.GetPagePeriod(m_pEditorDoc->m_curPosMs, 
                                          nActivityStartMs, nActivityEndMs);
      break;
   case INTERACTION_PERIOD_END_PAGE:
      m_pEditorDoc->project.GetPagePeriod(m_pEditorDoc->m_curPosMs, 
                                          nActivityStartMs, nActivityEndMs);
      // Note: also in this case the activity time needs to be the whole page.
      // Otherwise the page length is not known for stream operations.
      break;
   case INTERACTION_PERIOD_TIME:
       nActivityStartMs = (m_iCustomStartTime == -1) ? (nCurrPos - 5 > 0)? nCurrPos - 5: 0 : m_iCustomStartTime;
       nActivityEndMs = (m_iCustomPeriod == -1) ? (nCurrPos + 5 < nEndPos)? nCurrPos + 5: nEndPos : m_iCustomStartTime + m_iCustomPeriod;
      break;
   case INTERACTION_PERIOD_ALL_PAGES:
      nActivityStartMs = 0;
      nActivityEndMs = m_pEditorDoc->m_displayEndMs;
      break;
   }
   
	if (m_bSetStopmark)
	{	
		HRESULT hr = S_OK;
		CMarkStream* pMarkStream = NULL;
		if (SUCCEEDED(hr) && nActivityEndMs >= 0)
		{
			pMarkStream = m_pEditorDoc->project.GetMarkStream(true);
			if (!pMarkStream)	// TODO: Error message
				hr = E_FAIL;
		}
		
		if (SUCCEEDED(hr))
		{
			CStopJumpMark mark;
			mark.Init(false, NULL, nActivityEndMs);
			pMarkStream->IncludeMark(&mark);
		}
	}

   CTimePeriod visibility;
   visibility.Init(m_periodId, nActivityStartMs, nActivityEndMs);
   CTimePeriod activity;
   activity.Init(m_periodId, nActivityStartMs, nActivityEndMs);

   pInteraction->Init(&rcArea, m_bShowMouseCursor == 0 ? false : true, 
                     pMouseAction[ACTION_ACTIVATION], pMouseAction[ACTION_MOUSEDOWN], 
                     pMouseAction[ACTION_MOUSEUP], pMouseAction[ACTION_MOUSEIN], 
                     pMouseAction[ACTION_MOUSEOUT], 
                     &visibility, &activity, &m_pEditorDoc->project);
   
   for (i = 0; i < 5; ++i)
   {
      if (pMouseAction[i])
         delete pMouseAction[i];
   }

}

void CInteractionAreaDialog::OnBnClickedActivStartReplay() {
	if (m_cbActivStartReplay.GetCheck())
		m_cbActivStopReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnBnClickedActivStopReplay() {
	if (m_cbActivStopReplay.GetCheck())
		m_cbActivStartReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnBnClickedMouseupStartReplay() {
	if (m_cbMouseupStartReplay.GetCheck())
		m_cbMouseupStopReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnBnClickedMouseupStopReplay() {
	if (m_cbMouseupStopReplay.GetCheck())
		m_cbMouseupStartReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnBnClickedMousedownStopReplay() {
	if (m_cbMousedownStopReplay.GetCheck())
		m_cbMousedownStartReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnBnClickedMousedownStartReplay() {
	if (m_cbMousedownStartReplay.GetCheck())
		m_cbMousedownStopReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnBnClickedMouseinStartReplay() {
	if (m_cbMouseinStartReplay.GetCheck())
		m_cbMouseinStopReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnBnClickedMouseinStopReplay() {
	if (m_cbMouseinStopReplay.GetCheck())
		m_cbMouseinStartReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnBnClickedMouseoutStartReplay() {
	if (m_cbMouseoutStartReplay.GetCheck())
		m_cbMouseoutStopReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnBnClickedMouseoutStopReplay() {
	if (m_cbMouseoutStopReplay.GetCheck())
		m_cbMouseoutStartReplay.SetCheck(0);
}

void CInteractionAreaDialog::OnNMCustomdrawCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here
    int iPos = m_sldTime.GetPos();
    m_spnTime.SetPos(iPos / 1000.0);
    *pResult = 0;
}

void CInteractionAreaDialog::OnDeltaposCalloutSpinTime(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
     float fPos = m_spnTime.GetPos();
    int iSldPos = fPos * 1000;
    m_sldTime.SetPos(iSldPos);
    SetPeriod(iSldPos);
    *pResult = 0;
}

void CInteractionAreaDialog::OnEnUpdateCalloutEditTime()
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

void CInteractionAreaDialog::OnNMCustomdrawCalloutSliderPeriod(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here
    int iPos = m_sldPeriod.GetPos();
    m_spnPeriod.SetPos(iPos / 1000.0);
    *pResult = 0;
}

void CInteractionAreaDialog::OnDeltaposCalloutSpinPeriod(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
    float fPos = m_spnPeriod.GetPos();
    int iSldPos = fPos * 1000;
    m_sldPeriod.SetPos(iSldPos);
    *pResult = 0;
}

void CInteractionAreaDialog::OnEnUpdateCalloutEditPeriod()
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

void CInteractionAreaDialog::OnNMReleasedcaptureCalloutSliderTime(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    int iPos = m_sldTime.GetPos();

    SetPeriod(iPos);
    *pResult = 0;
}

void CInteractionAreaDialog::OnUpdateCustomTimeSettings(CCmdUI* pCmdUI) {
    int radioId = GetCheckedRadioButton(IDC_AC_ALL_PAGES, IDC_AC_CHOOSE);
    if (radioId == IDC_AC_CHOOSE)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
}
void CInteractionAreaDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
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

void CInteractionAreaDialog::RepositionControls() {
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

void CInteractionAreaDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    m_pScrollHelper->OnSize(nType, cx, cy);

    //RepositionControls();
    
}

void CInteractionAreaDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default

    /*CDialog::OnVScroll(nSBCode, nPos, pScrollBar);*/
    m_pScrollHelper->OnVScroll(nSBCode, nPos, pScrollBar);
    //RepositionControls();
}

BOOL CInteractionAreaDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Add your message handler code here and/or call default

    /*return CDialog::OnMouseWheel(nFlags, zDelta, pt);*/
    BOOL wasScrolled = m_pScrollHelper->OnMouseWheel(nFlags, zDelta, pt);
    /*if(wasScrolled)
        RepositionControls();*/
    return wasScrolled;
}

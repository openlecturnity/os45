// TestQuestionPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "TestQuestionPage.h"
#include "MfcUtils.h"
#include "TestSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "QuestionnaireSettings.h"

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CTestQuestionPage 

IMPLEMENT_DYNCREATE(CTestQuestionPage, CPropertyPage)

CTestQuestionPage::CTestQuestionPage() : CPropertyPage(CTestQuestionPage::IDD) {
    //{{AFX_DATA_INIT(CTestQuestionPage)
    //}}AFX_DATA_INIT

    m_csCaption.LoadString(CTestQuestionPage::IDD);
    m_psp.pszTitle = m_csCaption;
    m_psp.dwFlags |= PSP_USETITLE;

    m_pEditorProject = NULL;

    m_bButtonsChanged = false;
    m_bEditNewQuestion = false;
}

CTestQuestionPage::~CTestQuestionPage() {
}

void CTestQuestionPage::DoDataExchange(CDataExchange* pDX) {
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTestQuestionPage)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestQuestionPage, CPropertyPage)
    //{{AFX_MSG_MAP(CTestQuestionPage)
    ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
    ON_UPDATE_COMMAND_UI(IDC_AC_CURRENT_PAGE, OnUpdateActivityButtons)
    ON_BN_CLICKED(IDC_SHOW_QUESTIONAIRE_SETTINGS, OnShowQuestionaireSettings)
    ON_UPDATE_COMMAND_UI(IDC_SHOW_QUESTIONAIRE_SETTINGS, OnUpdateShowQuestionnaireSettings)
    ON_BN_CLICKED(IDC_SHOW_QUESTIONAIRE_BUTTONS, OnShowQuestionaireSettings)
    ON_CBN_SELCHANGE(IDC_TEST_CORRECT_ACTION, OnSelchangeTestCorrectAction)
    ON_UPDATE_COMMAND_UI(IDC_AC_CURRENT_TIMESTAMP, OnUpdateActivityButtons)
    ON_UPDATE_COMMAND_UI(IDC_AC_END_OF_PAGE, OnUpdateActivityButtons)
    ON_UPDATE_COMMAND_UI(IDC_SHOW_QUESTIONAIRE_BUTTONS, OnUpdateShowQuestionnaireSettings)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTestQuestionPage 

BOOL CTestQuestionPage::OnInitDialog() {
    CPropertyPage::OnInitDialog();

    DWORD dwIds[] = {IDC_GROUP_QUESTION,
                    IDC_LABEL_TITLE,
                    IDC_LABEL_QUESTION,
                    IDC_GROUP_BUTTON,
                    IDC_CHECK_SHOW_DELETE,
                    IDC_SHOW_QUESTIONAIRE_BUTTONS,
                    IDC_GROUP_VISIBILITY,
                    IDC_VB_CURRENT_PAGE,
                    IDC_VB_CURRENT_TIMESTAMP,
                    IDC_VB_END_OF_PAGE,
                    IDC_GROUP_ACTIVITY,
                    IDC_AC_ANALOG_VB,
                    IDC_AC_CURRENT_PAGE,
                    IDC_AC_CURRENT_TIMESTAMP,
                    IDC_AC_END_OF_PAGE,
                    -1};
    MfcUtils::Localize(this, dwIds);

    if (m_pEditorProject == NULL) {
        return TRUE;
    }

    /*/
    // Fill Questionnaire combo box
    if (m_aQuestionnaires.GetSize() > 0)
    {
    for (int i = 0; i < m_aQuestionnaires.GetSize(); ++i)
    {
    CQuestionnaireEx *pQuestionnaire = m_aQuestionnaires[i];
    m_comboQuestionnaires.AddString(pQuestionnaire->GetTitle());
    }     
    }
    /*/

    CEdit *pEdit = NULL;

    // Initialize title field
    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TITLE);
    if (pEdit) {
        pEdit->SetWindowText(m_csTitle);
    }

    // Initialize question field
    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_QUESTION);
    if (pEdit) {
        pEdit->SetWindowText(m_csQuestion);
    }

    /*/
    if (m_iQuestionnaireIndex >= 0 && m_iQuestionnaireIndex < m_comboQuestionnaires.GetCount())
    m_comboQuestionnaires.SetCurSel(m_iQuestionnaireIndex);
    /*/

    CButton *pCheckBox = (CButton *)GetDlgItem(IDC_CHECK_SHOW_DELETE);
    if (pCheckBox) {
        pCheckBox->SetCheck(m_bShowDelete);
    }
    pCheckBox = (CButton *)GetDlgItem(IDC_CHECK_SHOW_BACK);
    if (pCheckBox) {
        pCheckBox->SetCheck(true);
    }
    pCheckBox = (CButton *)GetDlgItem(IDC_CHECK_SHOW_NEXT);
    if (pCheckBox) {
        pCheckBox->SetCheck(true);
    }

    UINT nVisibiltyButtonId = IDC_VB_CURRENT_PAGE;
    switch (m_visualityPeriodId) {
    case INTERACTION_PERIOD_PAGE:
        nVisibiltyButtonId = IDC_VB_CURRENT_PAGE;
        break;
    case INTERACTION_PERIOD_END_PAGE:
        nVisibiltyButtonId = IDC_VB_END_OF_PAGE;
        break;
    case INTERACTION_PERIOD_TIME:
        nVisibiltyButtonId = IDC_VB_CURRENT_TIMESTAMP;
        break;
    }
    CheckRadioButton(IDC_VB_CURRENT_PAGE, IDC_VB_END_OF_PAGE, nVisibiltyButtonId);

    pCheckBox = (CButton*) GetDlgItem(IDC_AC_ANALOG_VB);
    if (pCheckBox) {
        pCheckBox->SetCheck(m_activityPeriodId == m_visualityPeriodId);
    }

    UINT nActivityButtonId = IDC_AC_CURRENT_PAGE;
    switch (m_activityPeriodId) {
    case INTERACTION_PERIOD_PAGE:
        nActivityButtonId = IDC_AC_CURRENT_PAGE;
        break;
    case INTERACTION_PERIOD_END_PAGE:
        nActivityButtonId = IDC_AC_END_OF_PAGE;
        break;
    case INTERACTION_PERIOD_TIME:
        nActivityButtonId = IDC_AC_CURRENT_TIMESTAMP;
        break;
    }
    CheckRadioButton(IDC_AC_CURRENT_PAGE, IDC_AC_END_OF_PAGE, nActivityButtonId);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTestQuestionPage::OnKickIdle() {
    UpdateDialogControls(this, FALSE);
}

void CTestQuestionPage::OnUpdateActivityButtons(CCmdUI *pCmdUI) {
    bool bEnable = true;

    UINT nAnalogChecked = IsDlgButtonChecked(IDC_AC_ANALOG_VB);

    if (nAnalogChecked != 0) {
        bEnable = false;
        int iVisualityButton = GetCheckedRadioButton(IDC_VB_CURRENT_PAGE, IDC_VB_END_OF_PAGE);
        switch (iVisualityButton) {
        case IDC_VB_CURRENT_TIMESTAMP:
            CheckRadioButton(IDC_AC_CURRENT_PAGE, IDC_AC_END_OF_PAGE, IDC_AC_CURRENT_TIMESTAMP);
            break;
        case IDC_VB_CURRENT_PAGE:
            CheckRadioButton(IDC_AC_CURRENT_PAGE, IDC_AC_END_OF_PAGE, IDC_AC_CURRENT_PAGE);
            break;
        case IDC_VB_END_OF_PAGE:
            CheckRadioButton(IDC_AC_CURRENT_PAGE, IDC_AC_END_OF_PAGE, IDC_AC_END_OF_PAGE);
            break;
        }
    } else {
        int iVisualityButton = GetCheckedRadioButton(IDC_VB_CURRENT_PAGE, IDC_VB_END_OF_PAGE);
        switch (iVisualityButton) {
        case IDC_VB_CURRENT_TIMESTAMP:
            CheckRadioButton(IDC_AC_CURRENT_PAGE, IDC_AC_END_OF_PAGE, IDC_AC_CURRENT_TIMESTAMP);
            if (pCmdUI->m_nID != IDC_AC_CURRENT_TIMESTAMP)
                bEnable = false;
            break;
        case IDC_VB_CURRENT_PAGE:
            bEnable = true;
            break;
        case IDC_VB_END_OF_PAGE:
            CheckRadioButton(IDC_AC_CURRENT_PAGE, IDC_AC_END_OF_PAGE, IDC_AC_END_OF_PAGE);
            if (pCmdUI->m_nID != IDC_AC_END_OF_PAGE)
                bEnable = false;
            break;
        }
    }

    pCmdUI->Enable(bEnable);
}


void CTestQuestionPage::OnOK() {
    // TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen

    CPage *pCurrentPage = NULL;

    int iRadioButtonId = GetCheckedRadioButton(IDC_VB_CURRENT_PAGE, IDC_VB_END_OF_PAGE);
    m_visualityPeriodId = INTERACTION_PERIOD_PAGE;
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
    }


    UINT nAnalogIsChecked = IsDlgButtonChecked(IDC_AC_ANALOG_VB);
    if (nAnalogIsChecked != 0) {
        m_activityPeriodId = m_visualityPeriodId;
    } else {
        iRadioButtonId = GetCheckedRadioButton(IDC_AC_CURRENT_PAGE, IDC_AC_END_OF_PAGE);
        m_activityPeriodId = INTERACTION_PERIOD_PAGE;
        switch (iRadioButtonId) {
        case IDC_AC_CURRENT_PAGE:
            m_activityPeriodId = INTERACTION_PERIOD_PAGE;
            break;
        case IDC_AC_END_OF_PAGE:
            m_activityPeriodId = INTERACTION_PERIOD_END_PAGE;
            break;
        case IDC_AC_CURRENT_TIMESTAMP:
            m_activityPeriodId = INTERACTION_PERIOD_TIME;
            break;
        }
    }

    /*/
    if (m_comboQuestionnaires.GetCount() > 0)
    m_iQuestionnaireIndex = m_comboQuestionnaires.GetCurSel();
    else
    /*/
    m_iQuestionnaireIndex = 0;

    CEdit *pEdit = NULL;
    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TITLE);
    if (pEdit) {
        pEdit->GetWindowText(m_csTitle);
    }

    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_QUESTION);
    if (pEdit) {
        pEdit->GetWindowText(m_csQuestion);
    }

    UINT nDeleteIsChecked = IsDlgButtonChecked(IDC_CHECK_SHOW_DELETE);
    m_bShowDelete = nDeleteIsChecked == 0 ? false: true;

    CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////////////////
///// public functions

void CTestQuestionPage::Init(CQuestionEx *pQuestion, CEditorProject *pProject) {
    m_pEditorProject = pProject;

    CQuestionStream *pQuestionStream = m_pEditorProject->GetQuestionStream(true);
    if (pQuestionStream) {
        pQuestionStream->GetQuestionnaires(&m_aQuestionnaires);
    }

    m_iQuestionnaireIndex = 0;

    m_visualityPeriodId = INTERACTION_PERIOD_PAGE;
    m_activityPeriodId = INTERACTION_PERIOD_PAGE;

    if (pQuestion) {
        m_bShowDelete = pQuestion->HasDeleteButtonEnabled();
        m_csTitle = pQuestion->GetTitle();
        m_csQuestion = pQuestion->GetQuestion();

        /*/
        CQuestionnaireEx *pQuestionnaire = pQuestion->GetBelongsTo();
        for (int i = 0; i < m_aQuestionnaires.GetSize(); ++i)
        {
        if (pQuestionnaire == m_aQuestionnaires[i])
        {
        m_iQuestionnaireIndex = i;
        break;
        }
        }
        /*/
        CTimePeriod *objectVisibility = pQuestion->GetObjectVisibility();
        m_visualityPeriodId = objectVisibility->GetPeriodId();

        CTimePeriod *objectActivity = pQuestion->GetObjectActivity();
        m_activityPeriodId = objectActivity->GetPeriodId();

    } else {  
        m_bEditNewQuestion = true;
        CQuestionStream *pQuestionStream = m_pEditorProject->GetQuestionStream(false);
        UINT nCount = 1;
        if (pQuestionStream) {
            nCount = pQuestionStream->GetQuestionCount() + 1;
        }
        m_csTitle.Format(IDS_DEFAULT_QUESTION_TITLE, nCount);
        m_csQuestion.LoadString(IDS_DEFAULT_QUESTION);
    }
}

void CTestQuestionPage::GetUserEntries(CString &csTitle, CString &csQuestion, int &iQuestionnaireIndex, 
                                       UINT &nPageStartMs, UINT &nPageEndMs, 
                                       CTimePeriod &visualityPeriod, CTimePeriod &activityPeriod, 
                                       bool &bShowDelete, bool &bButtonsChanged) {
    CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

    m_pEditorProject->GetPagePeriod(pDoc->m_curPosMs, nPageStartMs, nPageEndMs);

    bShowDelete = m_bShowDelete;
    UINT nVisualityStartMs = 0;
    UINT nVisualityEndMs = 0;

    switch (m_visualityPeriodId) {
    case INTERACTION_PERIOD_PAGE:
        m_pEditorProject->GetPagePeriod(pDoc->m_curPosMs, nVisualityStartMs, nVisualityEndMs);
        break;
    case INTERACTION_PERIOD_END_PAGE:
        m_pEditorProject->GetPagePeriod(pDoc->m_curPosMs, nVisualityStartMs, nVisualityEndMs);

        // Note: also in this case the time needs to be the whole page.
        break;
    case INTERACTION_PERIOD_TIME:
        nVisualityStartMs = pDoc->m_curPosMs;
        nVisualityEndMs = pDoc->m_curPosMs;
        break;
    case INTERACTION_PERIOD_ALL_PAGES:
        nVisualityStartMs = 0;
        nVisualityEndMs = pDoc->m_displayEndMs;
        break;
    }
    visualityPeriod.Init(m_visualityPeriodId, nVisualityStartMs, nVisualityEndMs);

    UINT nActivityStartMs = 0;
    UINT nActivityEndMs = 0;

    switch (m_activityPeriodId) {
    case INTERACTION_PERIOD_PAGE:
        m_pEditorProject->GetPagePeriod(pDoc->m_curPosMs, nActivityStartMs, nActivityEndMs);
        break;
    case INTERACTION_PERIOD_END_PAGE:
        m_pEditorProject->GetPagePeriod(pDoc->m_curPosMs, nActivityStartMs, nActivityEndMs);

        // Note: also in this case the activity needs to be the whole page.
        break;
    case INTERACTION_PERIOD_TIME:
        nActivityStartMs = pDoc->m_curPosMs;
        nActivityEndMs = pDoc->m_curPosMs;
        break;
    case INTERACTION_PERIOD_ALL_PAGES:
        nActivityStartMs = 0;
        nActivityEndMs = pDoc->m_displayEndMs;
        break;
    }
    activityPeriod.Init(m_activityPeriodId, nActivityStartMs, nActivityEndMs);

    csTitle = m_csTitle;
    csQuestion = m_csQuestion;

    iQuestionnaireIndex = m_iQuestionnaireIndex;

    bButtonsChanged = m_bButtonsChanged;
}

void CTestQuestionPage::OnShowQuestionaireSettings() {
    
    if (ShowQuestionnaireSettings(m_bButtonsChanged, m_bEditNewQuestion) == IDOK ) {
        CWnd *pParent = GetParent();
        if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CTestSettings))) {
            ((CTestSettings*) pParent)->SetQuestionnaireSettingsShown();
            ((CTestSettings*) pParent)->RefreshOptionpageActionsControls();
        }
    }
}

void CTestQuestionPage::OnUpdateShowQuestionnaireSettings(CCmdUI *pCmdUI) {
    BOOL bEnable = TRUE;
    pCmdUI->Enable(bEnable);
}

void CTestQuestionPage::OnSelchangeTestCorrectAction() {
    m_iQuestionnaireIndex = 0; //m_comboQuestionnaires.GetCurSel();
}

UINT CTestQuestionPage::ShowQuestionnaireSettings(bool &bButtonsChanged, bool bEditNewQuestion) 
{
    if (m_iQuestionnaireIndex < 0 || m_iQuestionnaireIndex >= m_aQuestionnaires.GetSize()) {
        return IDCANCEL;
    }

    CQuestionnaireEx *pQuestionnaireOld = m_aQuestionnaires[m_iQuestionnaireIndex];
    if (pQuestionnaireOld == NULL) {
        return IDCANCEL;
    }

    const MSG* pMsg = GetCurrentMessage();

    CQuestionnaireSettings dialog(IDS_QUESTIONNAIRE_SETTINGS, this, 0, bEditNewQuestion);
    if (WPARAM(pMsg->wParam) == IDC_SHOW_QUESTIONAIRE_SETTINGS) {
        dialog.SetActivePage(0);
    } else {
        dialog.SetActivePage(2);
    }

    dialog.Init(m_pEditorProject, pQuestionnaireOld);
    UINT nReturn = dialog.DoModal();

    if (nReturn == IDOK) {
        CQuestionnaireEx *pQuestionnaireNew = new CQuestionnaireEx();
        dialog.SetUserEntries(m_pEditorProject, pQuestionnaireNew);

        CQuestionStream *pQuestionStream = m_pEditorProject->GetQuestionStream(true);

        HRESULT hr = S_OK;

        bButtonsChanged = false;
        if (SUCCEEDED(hr)) {
            bButtonsChanged = pQuestionnaireOld->IsButtonConfigurationDifferent(pQuestionnaireNew);
            hr = pQuestionStream->ChangeQuestionnaire(pQuestionnaireOld, pQuestionnaireNew, false);
        }

        if (m_pEditorProject->FirstQuestionnaireIsRandomTest()) {
            m_pEditorProject->SetJumpRandomQuestionActions();
        } else {
            m_pEditorProject->ClearJumpRandomQuestionActions();
        }

        // replace result feedback
        CFeedback *pNewResultFeedback = CQuestionnaireEx::CreateResultFeedback(m_pEditorProject, pQuestionnaireNew);

        if (SUCCEEDED(hr)) {
            hr = pQuestionnaireNew->ReplaceFeedback(pNewResultFeedback, true);
        }

        _ASSERT(SUCCEEDED(hr));

        // update dialog labels
        m_aQuestionnaires.RemoveAll();
        pQuestionStream->GetQuestionnaires(&m_aQuestionnaires);

        /*/
        m_comboQuestionnaires.ResetContent();
        if (m_aQuestionnaires.GetSize() > 0)
        {
            for (int i = 0; i < m_aQuestionnaires.GetSize(); ++i)
            {
                CQuestionnaireEx *pQuestionnaire = m_aQuestionnaires[i];
                m_comboQuestionnaires.AddString(pQuestionnaire->GetTitle());
            }  

            if (m_iQuestionnaireIndex < 0)
                m_iQuestionnaireIndex = 0;
            if (m_iQuestionnaireIndex >= 0 && 
                m_iQuestionnaireIndex < m_aQuestionnaires.GetSize())
                m_comboQuestionnaires.SetCurSel(m_iQuestionnaireIndex);
        }
        else
        {
            m_iQuestionnaireIndex = -1;
        }
        /*/
        m_iQuestionnaireIndex = 0;
    }
    return nReturn;
}
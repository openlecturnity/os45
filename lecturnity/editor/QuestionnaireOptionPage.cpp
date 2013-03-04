// QuestionnaireOptionPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "QuestionnaireOptionPage.h"
#include "MfcUtils.h"
#include "QuestionnaireEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CQuestionnaireOptionPage 

IMPLEMENT_DYNCREATE(CQuestionnaireOptionPage, CPropertyPage)

CQuestionnaireOptionPage::CQuestionnaireOptionPage() : CPropertyPage(CQuestionnaireOptionPage::IDD) {
    //{{AFX_DATA_INIT(CQuestionnaireOptionPage)
        // HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
    //}}AFX_DATA_INIT

    m_csCaption.LoadString(CQuestionnaireOptionPage::IDD);
    m_psp.pszTitle = m_csCaption;
    m_psp.dwFlags |= PSP_USETITLE;

    m_bSetStopmark = TRUE;
    m_pEditorProject = NULL;
    m_bIsRandomTest = false;
    m_iNumberOfRandomQuestions = 1;

    m_hBitmap = static_cast<HBITMAP>(LoadImage (AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BUTTON_DOWN),
        IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
}

CQuestionnaireOptionPage::~CQuestionnaireOptionPage() {
    DeleteObject(m_hBitmap);
}

void CQuestionnaireOptionPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CQuestionnaireOptionPage)
    DDX_Control(pDX, IDC_TEST_WRONG_DESTINATION, m_editFailureAction);
    DDX_Control(pDX, IDC_TEST_CORRECT_DESTINATION, m_editCorrectAction);
    DDX_Control(pDX, IDC_TEST_CORRECT_ACTION, m_comboCorrectAction);
    DDX_Control(pDX, IDC_TEST_WRONG_ACTION, m_comboWrongAction);
    DDX_Control(pDX, IDC_EDIT_RANDOM_QUESTION_NO, m_editNumberOfRandomQuestions);
    DDX_Control(pDX, IDC_SPIN_RANDOM_QUESTION_NO, m_spnMaxNumberOfRandomQuestions);
    DDX_Control(pDX, IDC_TEST_CORRECT_INTERN, m_btnCorrectIntern);
    DDX_Control(pDX, IDC_TEST_WRONG_INTERN, m_btnWrongIntern);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuestionnaireOptionPage, CPropertyPage)
	//{{AFX_MSG_MAP(CQuestionnaireOptionPage)
	ON_BN_CLICKED(IDC_OPTIONS_SETTINGS, OnOptionsSettings)
    ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
	ON_BN_CLICKED(IDC_TEST_CORRECT_INTERN, OnPopupIntern)
	ON_UPDATE_COMMAND_UI(IDC_TEST_CORRECT_INTERN, OnUpdatePopupIntern)
	ON_BN_CLICKED(IDC_TEST_CORRECT_EXTERN, OnPopupExtern)
	ON_UPDATE_COMMAND_UI(IDC_TEST_CORRECT_EXTERN, OnUpdatePopupExtern)
	ON_BN_CLICKED(IDC_TEST_WRONG_INTERN, OnPopupIntern)
	ON_UPDATE_COMMAND_UI(IDC_TEST_WRONG_INTERN, OnUpdatePopupIntern)
	ON_BN_CLICKED(IDC_TEST_WRONG_EXTERN, OnPopupExtern)
	ON_UPDATE_COMMAND_UI(IDC_TEST_WRONG_EXTERN, OnUpdatePopupExtern)
	ON_UPDATE_COMMAND_UI(IDC_TEST_CORRECT_DESTINATION, OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(IDC_TEST_WRONG_DESTINATION, OnUpdateEdit)
	ON_CBN_SELCHANGE(IDC_TEST_CORRECT_ACTION, OnSelchangeAreaCorrectAction)
	ON_CBN_SELCHANGE(IDC_TEST_WRONG_ACTION, OnSelchangeAreaFailureAction)
    ON_BN_CLICKED(IDC_RANDOMIZED_QUESTIONS, &OnBnClickedRandomized)
    ON_EN_CHANGE(IDC_EDIT_RANDOM_QUESTION_NO, &OnEnChangeEditRandomQuestionNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CQuestionnaireOptionPage 


BOOL CQuestionnaireOptionPage::OnInitDialog() {
    CPropertyPage::OnInitDialog();

    DWORD dwIds[] = {IDC_OPTIONS_SETTINGS,
        IDC_RATING_AT_END,
        IDC_SHOW_PROGRESS,
        IDC_STOP_REPLAY,
        IDC_SET_STOPMARK,
        IDC_SUCCESS,
        IDC_LABEL_SUCCESS,
        IDC_LABEL_FAIL,
        IDC_STATIC_RAND_QUESTION_NO,
        IDC_RANDOMIZED_QUESTIONS,
        -1};
    MfcUtils::Localize(this, dwIds);

    LoadComboBoxActions();

    CButton *pCheckBox = NULL;

    pCheckBox = (CButton *)GetDlgItem(IDC_RATING_AT_END);
    if (pCheckBox)
        pCheckBox->SetCheck(m_bEvaluateAtEnd);

    pCheckBox = (CButton *)GetDlgItem(IDC_SHOW_PROCESS);
    if (pCheckBox)
        pCheckBox->SetCheck(m_bShowProcess);

    pCheckBox = (CButton *)GetDlgItem(IDC_SET_STOPMARK);
    if (pCheckBox)
        pCheckBox->SetCheck(m_bSetStopmark);

    pCheckBox = (CButton *)GetDlgItem(IDC_RANDOMIZED_QUESTIONS);
    if (pCheckBox)
        pCheckBox->SetCheck(m_bIsRandomTest);

    UpdateRandomizedQuestionsControls();

    m_btnCorrectIntern.SetBitmap(m_hBitmap);
    m_btnWrongIntern.SetBitmap(m_hBitmap);

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CQuestionnaireOptionPage::LoadComboBoxActions() {
    m_comboCorrectAction.ResetContent();
    m_comboWrongAction.ResetContent();

    CStringArray aActionStrings;
    CMouseAction::GetActionStrings(aActionStrings, false); // we don't take in consideration "Jump to random question" because it is not accessible in this dialog.
    for (int i = 0; i < aActionStrings.GetSize(); ++i) {
        m_comboCorrectAction.AddString(aActionStrings[i]);
        m_comboWrongAction.AddString(aActionStrings[i]);

        int iItemData = i;
        if (/*!m_bIsRandomTest && */i >= INTERACTION_JUMP_RANDOM_QUESTION)
            iItemData = i + 1;
        m_comboCorrectAction.SetItemData(i, iItemData);
        m_comboWrongAction.SetItemData(i, iItemData);
    }

    for (int i =0; i< m_comboCorrectAction.GetCount(); i++) {
        if (m_comboCorrectAction.GetItemData(i) == m_correctAction) {
            if (m_bIsRandomTest == false && m_correctAction == INTERACTION_JUMP_RANDOM_QUESTION) {
                m_comboCorrectAction.SetCurSel(0); // INTERACTION_NO_ACTION
            } else {
                m_comboCorrectAction.SetCurSel(i);
            }
            break;
        }
    }
    m_editCorrectAction.SetWindowText(m_csCorrectActionPath);

    for (int i =0; i< m_comboWrongAction.GetCount(); i++) {
        if (m_comboWrongAction.GetItemData(i) == m_failureAction) {
            if (m_bIsRandomTest == false && m_failureAction == INTERACTION_JUMP_RANDOM_QUESTION) {
                m_comboWrongAction.SetCurSel(0); // INTERACTION_NO_ACTION
            } else {
                m_comboWrongAction.SetCurSel(i);
            }
            break;
        }
    }
    m_editFailureAction.SetWindowText(m_csFailureActionPath);
}

void CQuestionnaireOptionPage::OnOptionsSettings() {
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
}

void CQuestionnaireOptionPage::OnKickIdle() {
    UpdateDialogControls(this, FALSE);
}

void CQuestionnaireOptionPage::OnPopupIntern() {
    const MSG* pMsg = GetCurrentMessage( );

    UINT nAction = 0;
    AreaActionTypeId nActionTypeId = INTERACTION_NO_ACTION;
    UINT nButtonId = 0;
    UINT nEditId = 0;
    switch (WPARAM(pMsg->wParam)) {
    case IDC_TEST_CORRECT_INTERN:
        nAction = ACTION_CORRECT;
        nActionTypeId = (AreaActionTypeId)m_comboCorrectAction.GetItemData(m_comboCorrectAction.GetCurSel());
        nButtonId = IDC_TEST_CORRECT_INTERN;
        nEditId = IDC_TEST_CORRECT_DESTINATION;
        break;
    case IDC_TEST_WRONG_INTERN:
        nAction = ACTION_WRONG;
        nActionTypeId = (AreaActionTypeId)m_comboWrongAction.GetItemData(m_comboWrongAction.GetCurSel());
        nButtonId = IDC_TEST_WRONG_INTERN;
        nEditId = IDC_TEST_WRONG_DESTINATION;
        break;
    default:
        return;
    }

    if (nActionTypeId == INTERACTION_JUMP_SPECIFIC_PAGE)
        CreateSlidePopup(nAction, nButtonId, nEditId);
    else if (nActionTypeId == INTERACTION_JUMP_TARGET_MARK)
        CreateTargetmarkPopup(nAction, nButtonId, nEditId);
}

void CQuestionnaireOptionPage::OnPopupExtern() {
    const MSG* pMsg = GetCurrentMessage( );

    UINT nAction = 0;
    UINT nEditId = 0;
    switch (WPARAM(pMsg->wParam)) {
    case IDC_TEST_CORRECT_EXTERN:
        nAction = m_comboCorrectAction.GetItemData(m_comboCorrectAction.GetCurSel());
        nEditId = IDC_TEST_CORRECT_DESTINATION;
        break;
    case IDC_TEST_WRONG_EXTERN:
        nAction = m_comboWrongAction.GetItemData(m_comboWrongAction.GetCurSel());
        nEditId = IDC_TEST_WRONG_DESTINATION;
        break;
    default:
        return;
    }

    CString csFilter;
    if (nAction == INTERACTION_OPEN_URL)
        csFilter.LoadString(IDS_HTML_FILTER);
    else 
        csFilter.LoadString(IDS_ALL_FILES);

    CFileDialog *pFileDialog = new CFileDialog(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, csFilter, this);
    pFileDialog->DoModal();

    CString csFileName = pFileDialog->GetFileName();

    CEdit *pEdit = (CEdit *)GetDlgItem(nEditId);
    if (pEdit)
        pEdit->SetWindowText(csFileName);

    delete pFileDialog;

}

void CQuestionnaireOptionPage::OnUpdatePopupIntern(CCmdUI *pCmdUI) {
    bool bEnable = false;

    int nAction = -1;
    if (pCmdUI->m_nID == IDC_TEST_CORRECT_INTERN)
        nAction = m_comboCorrectAction.GetItemData(m_comboCorrectAction.GetCurSel());
    else if (pCmdUI->m_nID == IDC_TEST_WRONG_INTERN)
        nAction = m_comboWrongAction.GetItemData(m_comboWrongAction.GetCurSel());

    if (nAction == INTERACTION_JUMP_SPECIFIC_PAGE || nAction == INTERACTION_JUMP_TARGET_MARK)
        bEnable = true;

    pCmdUI->Enable(bEnable);
}

void CQuestionnaireOptionPage::OnUpdatePopupExtern(CCmdUI *pCmdUI) {
    bool bEnable = false;

    int nAction = -1;
    if (pCmdUI->m_nID == IDC_TEST_CORRECT_EXTERN)
        nAction = m_comboCorrectAction.GetItemData(m_comboCorrectAction.GetCurSel());
    else if (pCmdUI->m_nID == IDC_TEST_WRONG_EXTERN)
        nAction = m_comboWrongAction.GetItemData(m_comboWrongAction.GetCurSel());

    if (nAction == INTERACTION_OPEN_URL || nAction == INTERACTION_OPEN_FILE)
        bEnable = true;

    pCmdUI->Enable(bEnable);
}

void CQuestionnaireOptionPage::OnUpdateEdit(CCmdUI *pCmdUI) {
    bool bEnable = false;

    int nAction = -1;
    if (pCmdUI->m_nID == IDC_TEST_CORRECT_DESTINATION)
        nAction = m_comboCorrectAction.GetItemData(m_comboCorrectAction.GetCurSel());
    else if (pCmdUI->m_nID == IDC_TEST_WRONG_DESTINATION)
        nAction = m_comboWrongAction.GetItemData(m_comboWrongAction.GetCurSel());

    if (nAction == INTERACTION_JUMP_SPECIFIC_PAGE || 
        nAction == INTERACTION_JUMP_TARGET_MARK ||
        nAction == INTERACTION_OPEN_URL || 
        nAction == INTERACTION_OPEN_FILE)
        bEnable = true;

    pCmdUI->Enable(bEnable);
}

void CQuestionnaireOptionPage::OnSelchangeAreaCorrectAction() {
    static UINT nLastAction = 0;

    UINT nAction = m_comboCorrectAction.GetItemData(m_comboCorrectAction.GetCurSel());
    bool bActionChanged = true;
    if (nAction == nLastAction)
        bActionChanged = false;
    nLastAction = nAction;

    if (bActionChanged) {
        CEdit *pEditWnd = (CEdit *)GetDlgItem(IDC_TEST_CORRECT_DESTINATION);
        pEditWnd->SetWindowText(_T(""));
    }
}

void CQuestionnaireOptionPage::OnSelchangeAreaFailureAction() {
    static UINT nLastAction = 0;

    UINT nAction = m_comboWrongAction.GetItemData(m_comboWrongAction.GetCurSel());
    bool bActionChanged = true;
    if (nAction == nLastAction)
        bActionChanged = false;
    nLastAction = nAction;

    if (bActionChanged) {
        CEdit *pEditWnd = (CEdit *)GetDlgItem(IDC_TEST_WRONG_DESTINATION);
        pEditWnd->SetWindowText(_T(""));
    }
}

void CQuestionnaireOptionPage::OnOK() {
    CButton *pCheckButton = NULL;

    pCheckButton = (CButton *)GetDlgItem(IDC_RATING_AT_END);
    if (pCheckButton)
        m_bEvaluateAtEnd = pCheckButton->GetCheck() == 0? false : true;

    pCheckButton = (CButton *)GetDlgItem(IDC_SHOW_PROCESS);
    if (pCheckButton)
        m_bShowProcess = pCheckButton->GetCheck() == 0? false : true;

    pCheckButton = (CButton *)GetDlgItem(IDC_SET_STOPMARK);
    if (pCheckButton)
        m_bSetStopmark = pCheckButton->GetCheck() == 0? false : true;

    pCheckButton = (CButton *)GetDlgItem(IDC_RANDOMIZED_QUESTIONS);
    if (pCheckButton)
        m_bIsRandomTest = pCheckButton->GetCheck() == 0? false : true;

    if (m_bIsRandomTest) {
        CString csNumber;
        m_editNumberOfRandomQuestions.GetWindowText(csNumber);
        m_iNumberOfRandomQuestions = _ttoi(csNumber);
    }

    m_correctAction = (AreaActionTypeId)m_comboCorrectAction.GetItemData(m_comboCorrectAction.GetCurSel());
    m_editCorrectAction.GetWindowText(m_csCorrectActionPath);

    m_failureAction = (AreaActionTypeId)m_comboWrongAction.GetItemData(m_comboWrongAction.GetCurSel());
    m_editFailureAction.GetWindowText(m_csFailureActionPath);

    CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// public functions

void CQuestionnaireOptionPage::Init(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire, bool bEditNewQuestion) {
    m_pEditorProject = pProject;
    if (pQuestionnaire) {
        m_bEvaluateAtEnd = pQuestionnaire->IsShowEvaluation();
        m_bShowProcess = pQuestionnaire->IsShowProgress();
        m_bSetStopmark = pQuestionnaire->IsDoStopmarks();
        m_bIsRandomTest = pQuestionnaire->IsRandomTest();
        m_iNumberOfRandomQuestions = pQuestionnaire->GetNumberOfRandomQuestions();
        if (m_iNumberOfRandomQuestions == 0 || m_iNumberOfRandomQuestions == pQuestionnaire->GetQuestionTotal()) {
            m_iNumberOfRandomQuestions += (bEditNewQuestion ? 1 : 0);
        }
        m_iQuestionsNumber = pQuestionnaire->GetQuestionTotal() + (bEditNewQuestion ? 1 : 0);
        
        if (m_iNumberOfRandomQuestions == 0)
            m_iNumberOfRandomQuestions = m_iQuestionsNumber;

        CMouseAction *pCorrectAction = pQuestionnaire->GetCorrectAction();
        if (pCorrectAction != NULL) {
            m_correctAction = pCorrectAction->GetActionId();
            m_csCorrectActionPath = pCorrectAction->GetPath();
            m_nCorrectMarkOrPageId = pCorrectAction->GetPageMarkId();
        } else {
            m_correctAction = INTERACTION_NO_ACTION;
        }

        CMouseAction *pFailureAction = pQuestionnaire->GetFailureAction();
        if (pFailureAction != NULL) {
            m_failureAction = pFailureAction->GetActionId();
            m_csFailureActionPath = pFailureAction->GetPath();
            m_nFailureMarkOrPageId = pFailureAction->GetPageMarkId();
        } else {
            m_failureAction = INTERACTION_NO_ACTION;
        }
    } else {
        m_bEvaluateAtEnd = true;
        m_bShowProcess = true;
        m_bSetStopmark = true;
        m_bIsRandomTest = false;
        m_iNumberOfRandomQuestions = 1 + (bEditNewQuestion ? 1 : 0);

        m_correctAction = INTERACTION_JUMP_NEXT_PAGE;
        m_csCorrectActionPath = _T("");
        m_nCorrectMarkOrPageId = 0;

        m_failureAction = INTERACTION_NO_ACTION;
        m_csFailureActionPath = _T("");
        m_nFailureMarkOrPageId = 0;
    }
}

void CQuestionnaireOptionPage::GetUserEntries(CMouseAction *pCorrectMouseAction, 
                                              CMouseAction *pFailureMouseAction, 
                                              bool &bShowEvaluation, bool &bShowProgress, bool &bDoStopmark, bool &bIsRandomTest, UINT &iNumberOfRandomQuestions){
    bShowEvaluation = m_bEvaluateAtEnd;
    bShowProgress = m_bShowProcess;
    bDoStopmark = m_bSetStopmark;
    bIsRandomTest = m_bIsRandomTest;
    iNumberOfRandomQuestions = m_iNumberOfRandomQuestions;

    pCorrectMouseAction->Init(m_pEditorProject, m_correctAction, m_nCorrectMarkOrPageId, m_csCorrectActionPath); 
    pFailureMouseAction->Init(m_pEditorProject, m_failureAction, m_nFailureMarkOrPageId, m_csFailureActionPath); 
}


/////////////////////////////////////////////////////////////////////////////
// private functions

void CQuestionnaireOptionPage::CreateSlidePopup(int nAction, int nButtonId, int nEditId) {
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
        if (nAction == ACTION_CORRECT) {
            m_nCorrectMarkOrPageId = caPages[bRet-1]->GetJumpId();
        } else {
            m_nFailureMarkOrPageId = caPages[bRet-1]->GetJumpId();
        }
    }

    caPages.RemoveAll();
    m_wndMenu.DestroyMenu();
}

void CQuestionnaireOptionPage::CreateTargetmarkPopup(int nAction, int nButtonId, int nEditId) {
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
            if (nAction == ACTION_CORRECT) {
                m_nCorrectMarkOrPageId = caJumpMarks[bRet-1]->GetId();
            } else {
                m_nFailureMarkOrPageId = caJumpMarks[bRet-1]->GetId();
            }
        }
        caJumpMarks.RemoveAll();
        m_wndMenu.DestroyMenu();
    }
}

UINT CQuestionnaireOptionPage::GetIndex(AreaActionTypeId actionId) {
    UINT nRet = 0;

    switch (actionId) {
    case INTERACTION_NO_ACTION:
        nRet = 0;
        break;
    case INTERACTION_JUMP_NEXT_PAGE:
        nRet = 1;
        break;
    case INTERACTION_JUMP_PREVIOUS_PAGE:
        nRet = 2;
        break;
    case INTERACTION_JUMP_SPECIFIC_PAGE:
        nRet = 3;
        break;
    case INTERACTION_JUMP_TARGET_MARK:
        nRet = 4;
        break;
    case INTERACTION_JUMP_RANDOM_QUESTION:
        nRet = 5;
        break;
    case INTERACTION_OPEN_URL:
        nRet = 6;
        break;
    case INTERACTION_OPEN_FILE:
        nRet = 7;
        break;
    case INTERACTION_START_REPLAY:
        nRet = 8;
        break;
    case INTERACTION_STOP_REPLAY:
        nRet = 9;
        break;
    case INTERACTION_AUDIO_ON:
        nRet = 10;
        break;
    case INTERACTION_AUDIO_OFF:
        nRet = 11;
        break;
    case INTERACTION_EXIT_PROCESS:
        nRet = 12;
        break;
    }

    return nRet;
}

void CQuestionnaireOptionPage::OnBnClickedRandomized()
{
    CButton *pCheckBox = (CButton *)GetDlgItem(IDC_RANDOMIZED_QUESTIONS);
    if (pCheckBox)
        m_bIsRandomTest = pCheckBox->GetCheck() == 0 ? false : true;

    if (!m_bIsRandomTest) {
        CString csCaption;
        csCaption.LoadString(IDS_WARNING);
        CString csMessage;
        csMessage.LoadString(IDS_CHANGE_RANDOM_CONTROLS);
        if (MessageBox(csMessage, csCaption, MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDNO) {
            m_bIsRandomTest = true;
            pCheckBox->SetCheck(TRUE);
            return;
        }
    }

    UpdateRandomizedQuestionsControls();
    LoadComboBoxActions();
}

void CQuestionnaireOptionPage::UpdateRandomizedQuestionsControls() {
    m_editNumberOfRandomQuestions.EnableWindow(m_bIsRandomTest ? TRUE : FALSE);
    m_spnMaxNumberOfRandomQuestions.EnableWindow(m_bIsRandomTest ? TRUE : FALSE);

    if (m_bIsRandomTest) {
        CString csNumber;
        csNumber.Format(_T("%d"), m_iNumberOfRandomQuestions);
        m_editNumberOfRandomQuestions.SetWindowText(csNumber);

        m_spnMaxNumberOfRandomQuestions.SetRange(1, m_iQuestionsNumber);
        m_spnMaxNumberOfRandomQuestions.SetPos(m_iNumberOfRandomQuestions);
        m_spnMaxNumberOfRandomQuestions.SetBuddy(&m_editNumberOfRandomQuestions);
    }
}

void CQuestionnaireOptionPage::OnEnChangeEditRandomQuestionNo() {
    if (m_iQuestionsNumber == 0) // this shouldn't happend, but make sure there won't be infinite loop.
        return;
    CString csNumber;
    m_editNumberOfRandomQuestions.GetWindowText(csNumber);

    // Make sure the range is between 1 and questions number.
    int iNumber = _ttoi(csNumber);
    if (iNumber > m_iQuestionsNumber) {
        csNumber.Format(_T("%d"), m_iQuestionsNumber);
        m_editNumberOfRandomQuestions.SetWindowText(csNumber);
    } else if (iNumber <= 0) {
        m_editNumberOfRandomQuestions.SetWindowText(_T("1"));
    } else {
        m_iNumberOfRandomQuestions = iNumber;
    }
}

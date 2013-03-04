// QuestionnaireSettings.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "QuestionnaireSettings.h"
#include "MfcUtils.h"
#include "TestQuestionPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuestionnaireSettings

IMPLEMENT_DYNAMIC(CQuestionnaireSettings, CPropertySheet)

CQuestionnaireSettings::CQuestionnaireSettings(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, bool bEditNewQuestion)
    :CPropertySheet(nIDCaption, pParentWnd, iSelectPage) {
    m_pQuestionnaireQuestionPage = new CQuestionnaireQuestionPage();
    AddPage(m_pQuestionnaireQuestionPage);

    m_pQuestionnaireOptionPage = new CQuestionnaireOptionPage();
    AddPage(m_pQuestionnaireOptionPage);

    m_pQuestionnaireButtonPage = new CQuestionnaireButtonPage();
    AddPage(m_pQuestionnaireButtonPage);

    m_bEditNewQuestion = bEditNewQuestion;
}

CQuestionnaireSettings::~CQuestionnaireSettings() {
    if (m_pQuestionnaireQuestionPage)
        delete m_pQuestionnaireQuestionPage;

    if (m_pQuestionnaireOptionPage)
        delete m_pQuestionnaireOptionPage;

    if (m_pQuestionnaireButtonPage)
        delete m_pQuestionnaireButtonPage;
}


BEGIN_MESSAGE_MAP(CQuestionnaireSettings, CPropertySheet)
	//{{AFX_MSG_MAP(CQuestionnaireSettings)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CQuestionnaireSettings 

BOOL CQuestionnaireSettings::OnInitDialog() {
    BOOL bResult = CPropertySheet::OnInitDialog();

    DWORD dwIds[] = {IDOK,
        IDCANCEL,
        -1};

    MfcUtils::Localize(this, dwIds);

    CWnd *pApply = GetDlgItem(ID_APPLY_NOW);
    if (pApply != NULL)
        pApply->DestroyWindow();

    CWnd *pHelp = GetDlgItem(IDHELP);
    if (pHelp != NULL)
        pHelp->DestroyWindow();

    CRect rectBtn;
    int nSpacing = 6;        // space between two buttons...

    int ids [] = {IDOK, IDCANCEL};
    for (int i = 0; i < sizeof(ids)/sizeof(int); i++) {
        GetDlgItem (ids [i])->GetWindowRect (rectBtn);

        ScreenToClient (&rectBtn);
        int btnWidth = rectBtn.Width();
        rectBtn.left = rectBtn.left + (btnWidth + nSpacing)* 2;
        rectBtn.right = rectBtn.right + (btnWidth + nSpacing)* 2;

        GetDlgItem (ids [i])->MoveWindow(rectBtn);
    }

    CPropertyPage *pActivePage = GetActivePage();
    SetActivePage(m_pQuestionnaireQuestionPage);
    SetActivePage(m_pQuestionnaireOptionPage);
    SetActivePage(m_pQuestionnaireButtonPage);

    SetActivePage(pActivePage);

    return bResult;
}

BOOL CQuestionnaireSettings::ContinueModal() {
    CPropertyPage* pActivePage = GetActivePage();
    if (NULL != pActivePage)
        pActivePage->SendMessage(WM_KICKIDLE, 0, 0); 

    return CPropertySheet::ContinueModal();
}

/////////////////////////////////////////////////////////////////////////////
// public functions

void CQuestionnaireSettings::Init(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire) {
    m_pQuestionnaireQuestionPage->Init(pProject, pQuestionnaire);
    m_pQuestionnaireOptionPage->Init(pProject, pQuestionnaire, m_bEditNewQuestion);
    m_pQuestionnaireButtonPage->Init(pQuestionnaire);
}

void CQuestionnaireSettings::SetUserEntries(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire) {
    // first get the button attributes, 
    // okButton is used by the feedbacks of question page
    COLORREF clrFill[4];
    COLORREF clrLine[4];
    COLORREF clrText[4];
    LOGFONT  logFont[4];
    CString csFilename[4];
    bool bIsImageButton;
    CString csOk;
    CString csSubmit;
    CString csDelete;
    m_pQuestionnaireButtonPage->GetUserEntries(clrFill, clrLine, clrText, logFont, 
        bIsImageButton, csFilename,
        csOk, csSubmit, csDelete);
    pQuestionnaire->SetButtonColor(clrFill, clrLine, clrText);
    pQuestionnaire->SetButtonFont(logFont);
    pQuestionnaire->SetButtonFilenames(csFilename);
    pQuestionnaire->SetButtonsAreImageButtons(bIsImageButton);

    pQuestionnaire->SetOkText(csOk);

    CString csName;
    bool bPassTypeRelative;
    UINT nNeededPoints;
    CArray<CFeedback *, CFeedback *> aGeneralFeedback;
    m_pQuestionnaireQuestionPage->GetUserEntries(pProject, pQuestionnaire, csName, 
        bPassTypeRelative, nNeededPoints, &aGeneralFeedback);

    CMouseAction correctMouseAction;
    CMouseAction failureMouseAction;
    bool bShowEvaluation = false;
    bool bShowProgress = false;
    bool bDoStopmark = false;
    bool bIsRandomTest = false;
    UINT iNumberOfRandomQuestions = 0;
    m_pQuestionnaireOptionPage->GetUserEntries(&correctMouseAction, &failureMouseAction, 
        bShowEvaluation, bShowProgress, bDoStopmark, bIsRandomTest, iNumberOfRandomQuestions);

    pQuestionnaire->Init(csName, bShowEvaluation, bShowProgress, bDoStopmark, bIsRandomTest, iNumberOfRandomQuestions,
        &correctMouseAction, &failureMouseAction, 
        bPassTypeRelative, nNeededPoints, 
        pProject, csOk, csSubmit, csDelete, &aGeneralFeedback);
}
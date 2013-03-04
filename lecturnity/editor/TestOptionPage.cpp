// TestOptionPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "TestOptionPage.h"
#include "MfcUtils.h"
#include "QuestionEx.h"
#include "QuestionStream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "lutils.h"

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CTestOptionPage 

IMPLEMENT_DYNCREATE(CTestOptionPage, CPropertyPage)

CTestOptionPage::CTestOptionPage() : CPropertyPage(CTestOptionPage::IDD)
{
	//{{AFX_DATA_INIT(CTestOptionPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   
	m_hBitmap = static_cast<HBITMAP>(LoadImage (AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BUTTON_DOWN),
									IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));

   m_csCaption.LoadString(CTestOptionPage::IDD);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   m_pEditorProject = NULL;
   
   m_feedBackTypes[0] = FEEDBACK_TYPE_Q_CORRECT;
   m_feedBackTypes[1] = FEEDBACK_TYPE_Q_WRONG;
   m_feedBackTypes[2] = FEEDBACK_TYPE_Q_REPEAT;
   m_feedBackTypes[3] = FEEDBACK_TYPE_Q_TIME;

   for (int i = 0; i < 4; ++i)
      m_pFeedbacks[i] = NULL;
}

CTestOptionPage::~CTestOptionPage()
{
   DeleteObject(m_hBitmap);
}

void CTestOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestOptionPage)
	DDX_Control(pDX, IDC_TEST_WRONG_INTERN, m_btnTestWrongIntern);
	DDX_Control(pDX, IDC_TEST_CORRECT_INTERN, m_btnTestCorrectIntern);
	DDX_Control(pDX, IDC_TEST_WRONG_DESTINATION, m_editFailureDestination);
	DDX_Control(pDX, IDC_TEST_CORRECT_DESTINATION, m_editCorrectDestination);
	DDX_Control(pDX, IDC_TEST_CORRECT_ACTION, m_comboSuccessAction);
	DDX_Control(pDX, IDC_TEST_WRONG_ACTION, m_comboFailureAction);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CORRECT_START_REPLAY, m_cbCorrectStartReplay);
	DDX_Control(pDX, IDC_CORRECT_STOP_REPLAY, m_cbCorrectStopReplay);
	DDX_Control(pDX, IDC_FAILURE_START_REPLAY, m_cbFailureStartReplay);
	DDX_Control(pDX, IDC_FAILURE_STOP_REPLAY, m_cbFailureStopReplay);
}


BEGIN_MESSAGE_MAP(CTestOptionPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTestOptionPage)
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
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CORRECT_START_REPLAY, &CTestOptionPage::OnBnClickedCorrectStartReplay)
	ON_BN_CLICKED(IDC_CORRECT_STOP_REPLAY, &CTestOptionPage::OnBnClickedCorrectStopReplay)
	ON_BN_CLICKED(IDC_FAILURE_START_REPLAY, &CTestOptionPage::OnBnClickedFailureStartReplay)
	ON_BN_CLICKED(IDC_FAILURE_STOP_REPLAY, &CTestOptionPage::OnBnClickedFailureStopReplay)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTestOptionPage 

BOOL CTestOptionPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
   DWORD dwIds[] = {IDC_GROUP_CORRECT,
                    IDC_ACTION_CORRECT,
                    IDC_GROUP_FAILURE,
                    IDC_ACTION_FAILURE,
                    IDC_GROUP_INDIVIDUAL_FEEDBACK,
                    IDC_CHECK_ANSWER_CORRECT,
                    IDC_CHECK_ANSWER_ERROR,
                    IDC_CHECK_ANSWER_TIMEOUT,
                    IDC_CHECK_ANSWER_RECAPITULATE,
					IDC_CORRECT_START_REPLAY,
					IDC_CORRECT_STOP_REPLAY,
					IDC_FAILURE_START_REPLAY,
					IDC_FAILURE_STOP_REPLAY,
                    -1};
   MfcUtils::Localize(this, dwIds);

   if (m_pEditorProject == NULL)
      return TRUE;

    RefreshActionsControls();

   CEdit *pEdit = NULL;
   CButton *pCheckBox = NULL;
   
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_CORRECT);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[0]);
   pCheckBox = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_CORRECT);
   if (pCheckBox)
      pCheckBox->SetCheck(m_bShowFeedback[0]);
   
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_ERROR);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[1]);
   pCheckBox = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_ERROR);
   if (pCheckBox)
      pCheckBox->SetCheck(m_bShowFeedback[1]);
   
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_RECAPITULATE);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[2]);
   pCheckBox = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_RECAPITULATE);
   if (pCheckBox)
      pCheckBox->SetCheck(m_bShowFeedback[2]);
   
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_TIMEOUT);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[3]);
   pCheckBox = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_TIMEOUT);
   if (pCheckBox)
      pCheckBox->SetCheck(m_bShowFeedback[3]);
   

   m_btnTestCorrectIntern.SetBitmap(m_hBitmap);
   m_btnTestWrongIntern.SetBitmap(m_hBitmap);
   
   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTestOptionPage::RefreshActionsControls() {
    // Reload combobox content so that all posible actions are included (e.g. INTERACTION_JUMP_RANDOM_QUESTION)
    m_comboSuccessAction.ResetContent();
    m_comboFailureAction.ResetContent();
    CStringArray aActionStrings;
    CMouseAction::GetActionStrings(aActionStrings, IsRandomTest());
    for (int i = 0; i < aActionStrings.GetSize(); ++i) {
        m_comboSuccessAction.AddString(aActionStrings[i]);
        m_comboFailureAction.AddString(aActionStrings[i]);

        int iItemData = i;
        if (!IsRandomTest() && i >= INTERACTION_JUMP_RANDOM_QUESTION)
            iItemData = i + 1;
        m_comboSuccessAction.SetItemData(i, iItemData);
        m_comboFailureAction.SetItemData(i, iItemData);
    }


    if (!IsRandomTest()) {
        if (m_nCorrectAction == INTERACTION_JUMP_RANDOM_QUESTION) {
            m_comboSuccessAction.SetCurSel(0); // INTERACTION_NO_ACTION
            m_nCorrectAction = INTERACTION_NO_ACTION;
        } else {
            for (int i =0; i< m_comboSuccessAction.GetCount(); i++) {
                if (m_comboSuccessAction.GetItemData(i) == m_nCorrectAction) {
                    m_comboSuccessAction.SetCurSel(i);
                    break;
                }
            }
        }
        m_editCorrectDestination.SetWindowText(m_csCorrectActionPath);

        if (m_nFailureAction == INTERACTION_JUMP_RANDOM_QUESTION) {
            m_comboFailureAction.SetCurSel(0); // INTERACTION_NO_ACTION
            m_nFailureAction = INTERACTION_NO_ACTION;
        } else {
            for (int i =0; i< m_comboFailureAction.GetCount(); i++) {
                if (m_comboFailureAction.GetItemData(i) == m_nFailureAction) {
                    m_comboFailureAction.SetCurSel(i);
                    break;
                }
            }
        }
        m_editFailureDestination.SetWindowText(m_csFailureActionPath);
        UINT ids[] = {IDC_TEST_CORRECT_ACTION,
            IDC_TEST_CORRECT_DESTINATION,
            IDC_TEST_CORRECT_INTERN,
            IDC_TEST_WRONG_ACTION,
            IDC_TEST_WRONG_DESTINATION,
            IDC_TEST_WRONG_INTERN};
        for (int i = 0; i < _countof(ids); i++) {
            CWnd *pWnd = GetDlgItem(ids[i]);
            if (pWnd != NULL) 
                pWnd->EnableWindow(TRUE);
        }
    } else {
        UINT ids[] = {IDC_TEST_CORRECT_ACTION,
            IDC_TEST_CORRECT_DESTINATION,
            IDC_TEST_CORRECT_INTERN,
            IDC_TEST_WRONG_ACTION,
            IDC_TEST_WRONG_DESTINATION,
            IDC_TEST_WRONG_INTERN};
        for (int i = 0; i < _countof(ids); i++) {
            CWnd *pWnd = GetDlgItem(ids[i]);
            if (pWnd != NULL) 
                pWnd->EnableWindow(FALSE);
        }

        // Clear content of Edit box controls
        CEdit *pEditWnd = (CEdit *)GetDlgItem(IDC_TEST_WRONG_DESTINATION);
        pEditWnd->SetWindowText(_T(""));
        pEditWnd = (CEdit *)GetDlgItem(IDC_TEST_CORRECT_DESTINATION);
        pEditWnd->SetWindowText(_T(""));

        // Set INTERACTION_JUMP_RANDOM_QUESTION action
        for (int i =0; i< m_comboSuccessAction.GetCount(); i++) {
            if (m_comboSuccessAction.GetItemData(i) == INTERACTION_JUMP_RANDOM_QUESTION) {
                m_comboSuccessAction.SetCurSel(i);
                m_nCorrectAction = INTERACTION_JUMP_RANDOM_QUESTION;
                break;
            }
        }
        for (int i =0; i< m_comboFailureAction.GetCount(); i++) {
            if (m_comboFailureAction.GetItemData(i) == INTERACTION_JUMP_RANDOM_QUESTION) {
                m_comboFailureAction.SetCurSel(i);
                m_nFailureAction = INTERACTION_JUMP_RANDOM_QUESTION;
                break;
            }
        }
    }

    if (m_nCorrectNextAction == INTERACTION_START_REPLAY || m_nCorrectAction == INTERACTION_JUMP_RANDOM_QUESTION) {
        m_cbCorrectStartReplay.SetCheck(1);
        m_cbCorrectStopReplay.SetCheck(0);
    } else if (m_nCorrectNextAction == INTERACTION_STOP_REPLAY) {
        m_cbCorrectStartReplay.SetCheck(0);
        m_cbCorrectStopReplay.SetCheck(1);
    } else {
        m_cbCorrectStartReplay.SetCheck(0);
        m_cbCorrectStopReplay.SetCheck(0);
    }

    if (m_nFailureNextAction == INTERACTION_START_REPLAY || m_nFailureAction == INTERACTION_JUMP_RANDOM_QUESTION) {
        m_cbFailureStartReplay.SetCheck(1);
        m_cbFailureStopReplay.SetCheck(0);
    } else if (m_nFailureNextAction == INTERACTION_STOP_REPLAY) {
        m_cbFailureStartReplay.SetCheck(0);
        m_cbFailureStopReplay.SetCheck(1);
    } else {
        m_cbFailureStartReplay.SetCheck(0);
        m_cbFailureStopReplay.SetCheck(0);
    }

    bool bDisableCorrectStartStopReplay = m_nCorrectAction == INTERACTION_NO_ACTION || m_nCorrectAction == INTERACTION_START_REPLAY || m_nCorrectAction == INTERACTION_STOP_REPLAY || m_nCorrectAction == INTERACTION_EXIT_PROCESS || m_nCorrectAction == INTERACTION_JUMP_RANDOM_QUESTION;
    m_cbCorrectStartReplay.EnableWindow(bDisableCorrectStartStopReplay ? 0 : 1);
    m_cbCorrectStopReplay.EnableWindow(bDisableCorrectStartStopReplay ? 0 : 1);

    bool bDisableFailureStartStopReplay = m_nFailureAction == INTERACTION_NO_ACTION || m_nFailureAction == INTERACTION_START_REPLAY || m_nFailureAction == INTERACTION_STOP_REPLAY || m_nFailureAction == INTERACTION_EXIT_PROCESS || m_nFailureAction == INTERACTION_JUMP_RANDOM_QUESTION;
    m_cbFailureStartReplay.EnableWindow(bDisableFailureStartStopReplay ? 0 : 1);
    m_cbFailureStopReplay.EnableWindow(bDisableFailureStartStopReplay ? 0 : 1);
}

void CTestOptionPage::OnKickIdle()
{
   UpdateDialogControls(this, FALSE);
}

void CTestOptionPage::OnOK() 
{
    m_nCorrectAction = (AreaActionTypeId)m_comboSuccessAction.GetItemData(m_comboSuccessAction.GetCurSel());

    if (m_nCorrectAction == INTERACTION_OPEN_URL || m_nCorrectAction == INTERACTION_OPEN_FILE)
    {	
        CEdit *pEdit = (CEdit *)GetDlgItem(IDC_TEST_CORRECT_DESTINATION);
        if (pEdit)
            pEdit->GetWindowText(m_csCorrectActionPath);
    }
    if(m_cbCorrectStartReplay.GetCheck())
        m_nCorrectNextAction = INTERACTION_START_REPLAY;
    else if (m_cbCorrectStopReplay.GetCheck())
        m_nCorrectNextAction = INTERACTION_STOP_REPLAY;
    else
        m_nCorrectNextAction = INTERACTION_NO_ACTION;
    m_nFailureAction = (AreaActionTypeId)m_comboFailureAction.GetItemData(m_comboFailureAction.GetCurSel());

    if (m_nFailureAction == INTERACTION_OPEN_URL || m_nFailureAction == INTERACTION_OPEN_FILE)
    {	
        CEdit *pEdit = (CEdit *)GetDlgItem(IDC_TEST_WRONG_DESTINATION);
        if (pEdit)
            pEdit->GetWindowText(m_csFailureActionPath);
    }
   if(m_cbFailureStartReplay.GetCheck())
	   m_nFailureNextAction = INTERACTION_START_REPLAY;
   else if (m_cbFailureStopReplay.GetCheck())
	   m_nFailureNextAction = INTERACTION_STOP_REPLAY;
   else
	   m_nFailureNextAction = INTERACTION_NO_ACTION;

   
   CButton *pButton = NULL;
   CEdit *pEdit = NULL;

   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_CORRECT);
   if (pButton)
      m_bShowFeedback[0] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_CORRECT);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[0]);

   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_ERROR);
   if (pButton)
      m_bShowFeedback[1] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_ERROR);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[1]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_RECAPITULATE);
   if (pButton)
      m_bShowFeedback[2] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_RECAPITULATE);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[2]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_TIMEOUT);
   if (pButton)
      m_bShowFeedback[3] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_TIMEOUT);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[3]);

	CPropertyPage::OnOK();
}

void CTestOptionPage::OnPopupIntern() 
{
	const MSG* pMsg = GetCurrentMessage( );

	UINT nAction = 0;
	AreaActionTypeId nActionTypeId = INTERACTION_NO_ACTION;
	UINT nButtonId = 0;
	UINT nEditId = 0;
	switch (WPARAM(pMsg->wParam))
	{
	case IDC_TEST_CORRECT_INTERN:
		nAction = ACTION_CORRECT;
		nActionTypeId = (AreaActionTypeId)m_comboSuccessAction.GetItemData(m_comboSuccessAction.GetCurSel());
		nButtonId = IDC_TEST_CORRECT_INTERN;
		nEditId = IDC_TEST_CORRECT_DESTINATION;
		break;
	case IDC_TEST_WRONG_INTERN:
		nAction = ACTION_WRONG;
		nActionTypeId = (AreaActionTypeId)m_comboFailureAction.GetItemData(m_comboFailureAction.GetCurSel());
		nButtonId = IDC_TEST_WRONG_INTERN;
		nEditId = IDC_TEST_WRONG_DESTINATION;
		break;
	default:
		return;
	}
	
	if (nActionTypeId == SPECIFIC_PAGE /*||
       nActionStringId == SPECIFIC_PAGE_STARTREPLAY ||
       nActionStringId == SPECIFIC_PAGE_STOPREPLAY*/)
		CreateSlidePopup(nAction, nButtonId, nEditId);
	else if (nActionTypeId == TARGET_MARK/*||
            nActionStringId == TARGET_MARK_STARTREPLAY ||
            nActionStringId == TARGET_MARK_STOPREPLAY*/)
		CreateTargetmarkPopup(nAction, nButtonId, nEditId);
}

void CTestOptionPage::OnPopupExtern() 
{
	const MSG* pMsg = GetCurrentMessage( );

	AreaActionTypeId nActionTypeId = INTERACTION_NO_ACTION;
	UINT nEditId = 0;
	switch (WPARAM(pMsg->wParam))
	{
	case IDC_TEST_CORRECT_EXTERN:
		nActionTypeId = (AreaActionTypeId)m_comboSuccessAction.GetItemData(m_comboSuccessAction.GetCurSel());
		nEditId = IDC_TEST_CORRECT_DESTINATION;
		break;
	case IDC_TEST_WRONG_EXTERN:
		nActionTypeId = (AreaActionTypeId)m_comboFailureAction.GetItemData(m_comboFailureAction.GetCurSel());
		nEditId = IDC_TEST_WRONG_DESTINATION;
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

	CString csFileName = pFileDialog->GetFileName();
	
	CEdit *pEdit = (CEdit *)GetDlgItem(nEditId);
	if (pEdit)
		pEdit->SetWindowText(csFileName);

	delete pFileDialog;

}

void CTestOptionPage::OnUpdatePopupIntern(CCmdUI *pCmdUI)
{
    if (IsRandomTest()) {
        pCmdUI->Enable(FALSE);
        return; // do not enable these controls for random tests
    }
	bool bEnable = false;

	int nAction = -1;
	if (pCmdUI->m_nID == IDC_TEST_CORRECT_INTERN)
		nAction = m_comboSuccessAction.GetItemData(m_comboSuccessAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_TEST_WRONG_INTERN)
		nAction = m_comboFailureAction.GetItemData(m_comboFailureAction.GetCurSel());

	if (nAction == INTERACTION_JUMP_SPECIFIC_PAGE || 
       /*nAction == SPECIFIC_PAGE_STARTREPLAY || 
       nAction == SPECIFIC_PAGE_STOPREPLAY || */
		 nAction == INTERACTION_JUMP_TARGET_MARK /*|| 
       nAction == TARGET_MARK_STARTREPLAY || 
       nAction == TARGET_MARK_STOPREPLAY*/)
		bEnable = true;

   pCmdUI->Enable(bEnable);
}

void CTestOptionPage::OnUpdatePopupExtern(CCmdUI *pCmdUI)
{
    if (IsRandomTest()) {
        pCmdUI->Enable(FALSE);
        return; // do not enable these controls for random tests
    }

	bool bEnable = false;

	int nAction = -1;
	if (pCmdUI->m_nID == IDC_TEST_CORRECT_EXTERN)
		nAction = m_comboSuccessAction.GetItemData(m_comboSuccessAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_TEST_WRONG_EXTERN)
		nAction = m_comboFailureAction.GetItemData(m_comboFailureAction.GetCurSel());

	if (nAction == INTERACTION_OPEN_URL || nAction == INTERACTION_OPEN_FILE)
        bEnable = true;
	
   pCmdUI->Enable(bEnable);
}

void CTestOptionPage::OnUpdateEdit(CCmdUI *pCmdUI)
{
    if (IsRandomTest()) {
        return; // do not enable these controls for random tests
    }
	bool bEnable = false;

	int nAction = -1;
	if (pCmdUI->m_nID == IDC_TEST_CORRECT_DESTINATION)
		nAction = m_comboSuccessAction.GetItemData(m_comboSuccessAction.GetCurSel());
	else if (pCmdUI->m_nID == IDC_TEST_WRONG_DESTINATION)
		nAction = m_comboFailureAction.GetItemData(m_comboFailureAction.GetCurSel());

	if (nAction == INTERACTION_JUMP_SPECIFIC_PAGE ||
		nAction == INTERACTION_JUMP_TARGET_MARK ||
		nAction == INTERACTION_OPEN_URL || 
		nAction == INTERACTION_OPEN_FILE)
		bEnable = true;

   pCmdUI->Enable(bEnable);
}

void CTestOptionPage::OnSelchangeAreaCorrectAction() 
{
	static AreaActionTypeId nLastAction = INTERACTION_NO_ACTION;

	AreaActionTypeId nAction = (AreaActionTypeId)m_comboSuccessAction.GetItemData(m_comboSuccessAction.GetCurSel());
	bool bActionChanged = true;
	if (nAction == nLastAction)
		bActionChanged = false;
	nLastAction = nAction;
	
	if (bActionChanged)
	{
		CEdit *pEditWnd = (CEdit *)GetDlgItem(IDC_TEST_CORRECT_DESTINATION);
		pEditWnd->SetWindowText(_T(""));
		m_cbCorrectStartReplay.SetCheck(0);
		m_cbCorrectStopReplay.SetCheck(0);
		if (nAction == INTERACTION_NO_ACTION || nAction == INTERACTION_START_REPLAY || nAction == INTERACTION_STOP_REPLAY || nAction == INTERACTION_EXIT_PROCESS)
		{
			m_cbCorrectStartReplay.EnableWindow(0);
			m_cbCorrectStopReplay.EnableWindow(0);
		}
		else
		{
			m_cbCorrectStartReplay.EnableWindow(1);
			m_cbCorrectStopReplay.EnableWindow(1);
		}
	}
}

void CTestOptionPage::OnSelchangeAreaFailureAction() 
{
   static AreaActionTypeId nLastAction = INTERACTION_NO_ACTION;

	AreaActionTypeId nAction = (AreaActionTypeId)m_comboFailureAction.GetItemData(m_comboFailureAction.GetCurSel());
	bool bActionChanged = true;
	if (nAction == nLastAction)
		bActionChanged = false;
	nLastAction = nAction;
	
	if (bActionChanged)
	{
		CEdit *pEditWnd = (CEdit *)GetDlgItem(IDC_TEST_WRONG_DESTINATION);
		pEditWnd->SetWindowText(_T(""));
		m_cbCorrectStartReplay.SetCheck(0);
		m_cbCorrectStopReplay.SetCheck(0);
		if (nAction == INTERACTION_NO_ACTION || nAction == INTERACTION_START_REPLAY || nAction == INTERACTION_STOP_REPLAY || nAction == INTERACTION_EXIT_PROCESS)
		{
			m_cbFailureStartReplay.EnableWindow(0);
			m_cbFailureStopReplay.EnableWindow(0);
		}
		else
		{
			m_cbFailureStartReplay.EnableWindow(1);
			m_cbFailureStopReplay.EnableWindow(1);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// private functions

void CTestOptionPage::CreateSlidePopup(int nAction, int nButtonId, int nEditId)
{
	CRect rcButton;
	CWnd *pWndButton = GetDlgItem(nButtonId);

	pWndButton->GetClientRect(rcButton);
	pWndButton->ClientToScreen(&rcButton);

	m_wndMenu.CreatePopupMenu();

    CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

    CArray<CPage *, CPage *> caPages;
	m_pEditorProject->GetPages(caPages, 0, pDoc->m_docLengthMs);
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
      if (nAction == ACTION_CORRECT)
		   m_nCorrectMarkOrPageId = caPages[bRet-1]->GetJumpId();
      else
		   m_nFailureMarkOrPageId = caPages[bRet-1]->GetJumpId();
	}

	caPages.RemoveAll();
	m_wndMenu.DestroyMenu();
}

void CTestOptionPage::CreateTargetmarkPopup(int nAction, int nButtonId, int nEditId)
{
   CRect rcButton;
   CWnd *pWndButton = GetDlgItem(nButtonId);
   
   pWndButton->GetClientRect(rcButton);
   pWndButton->ClientToScreen(&rcButton);
   
   CMarkStream *pMarkStream = m_pEditorProject->GetMarkStream();
   if (pMarkStream) {
      CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

      m_wndMenu.CreatePopupMenu();

      CArray<CStopJumpMark *, CStopJumpMark *> caJumpMarks;
      pMarkStream->GetJumpMarksInRegion(caJumpMarks, 0, pDoc->m_docLengthMs);
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
         if (nAction == ACTION_CORRECT)
            m_nCorrectMarkOrPageId = caJumpMarks[bRet-1]->GetId();
         else
            m_nFailureMarkOrPageId = caJumpMarks[bRet-1]->GetId();
      }
      
      caJumpMarks.RemoveAll();
      
      m_wndMenu.DestroyMenu();
   }
}

/////////////////////////////////////////////////////////////////////////////
// public functions

void CTestOptionPage::Init(CQuestionEx *pQuestion, CEditorProject *pProject)
{
   m_pEditorProject = pProject;
   
   m_nCorrectAction = pQuestion != NULL ? INTERACTION_NO_ACTION : INTERACTION_JUMP_NEXT_PAGE;
   m_nCorrectNextAction = INTERACTION_NO_ACTION;
   m_csCorrectActionPath.Empty();
   m_nCorrectMarkOrPageId = 0;
   
   m_nFailureAction = INTERACTION_NO_ACTION;
   m_nFailureNextAction = INTERACTION_NO_ACTION;
   m_csFailureActionPath.Empty();
   m_nFailureMarkOrPageId = 0;

   for (int i = 0; i < 4; ++i)
      m_bShowFeedback[i] = true;

   m_csFeedback[0].LoadString(IDS_ANSWER_CORRECT);
   m_csFeedback[1].LoadString(IDS_ANSWER_WRONG);
   m_csFeedback[2].LoadString(IDS_ANSWER_RECAPITULATE);
   m_csFeedback[3].LoadString(IDS_ANSWER_TIMEOUT);

   if (pProject != NULL) {
       CQuestionnaireEx *pFirstQuestionnaire = pProject->GetFirstQuestionnaireEx();
       if (pFirstQuestionnaire) {
           CFeedback *pFeedback = NULL;
           for (int i = 0; i < 4; ++i)
           {
               pFeedback = pFirstQuestionnaire->GetFeedback(m_feedBackTypes[i]);
               if (pFeedback)
               {
                   m_csFeedback[i] = pFeedback->GetText();
                   m_bShowFeedback[i] = !pFeedback->IsDisabled();
               }
           }
       }
   }

   if (pQuestion)
   {
      CMouseAction *pCorrectAction = pQuestion->GetCorrectAction();
      if (pCorrectAction != NULL)
      {
		  m_nCorrectAction = pCorrectAction->GetActionId();
		 if (m_nCorrectAction == INTERACTION_NO_ACTION || m_nCorrectAction == INTERACTION_START_REPLAY || m_nCorrectAction == INTERACTION_STOP_REPLAY || m_nCorrectAction == INTERACTION_EXIT_PROCESS)
		{
			m_nCorrectNextAction = INTERACTION_NO_ACTION;
		}
		else
		{
			if (pCorrectAction->GetNextAction() != NULL)
			    m_nCorrectNextAction = pCorrectAction->GetNextAction()->GetActionId();
			else
				m_nCorrectNextAction = INTERACTION_NO_ACTION;
		}
         m_csCorrectActionPath = pCorrectAction->GetPath();
         m_nCorrectMarkOrPageId = pCorrectAction->GetPageMarkId();
      }
      if (m_nCorrectAction == INTERACTION_JUMP_SPECIFIC_PAGE /*||
          m_correctAction == SPECIFIC_PAGE_STARTREPLAY ||
          m_correctAction == SPECIFIC_PAGE_STOPREPLAY*/)
      {
         m_csCorrectActionPath = m_pEditorProject->GetPageTitleForId(m_nCorrectMarkOrPageId);
      }
      else if (m_nCorrectAction == INTERACTION_JUMP_TARGET_MARK /*||
               m_correctAction == TARGET_MARK_STARTREPLAY ||
               m_correctAction == TARGET_MARK_STOPREPLAY*/)
      {
         CStopJumpMark *pMark = m_pEditorProject->GetMarkWithId(m_nCorrectMarkOrPageId);
         if (pMark != NULL)
            m_csCorrectActionPath = pMark->GetLabel();
         // TODO else this id is undefined
      }


      CMouseAction *pFailureAction = pQuestion->GetFailureAction();
      if (pFailureAction != NULL)
      {
         m_nFailureAction = pFailureAction->GetActionId();
		 if (m_nFailureAction == INTERACTION_NO_ACTION || m_nFailureAction == INTERACTION_START_REPLAY || m_nFailureAction == INTERACTION_STOP_REPLAY || m_nFailureAction == INTERACTION_EXIT_PROCESS)
		{
			m_nFailureNextAction = INTERACTION_NO_ACTION;
		}
		else
		{
		    if (pFailureAction->GetNextAction() != NULL)
			    m_nFailureNextAction = pFailureAction->GetNextAction()->GetActionId();
			else
				m_nFailureNextAction = INTERACTION_NO_ACTION;
		}
         m_csFailureActionPath = pFailureAction->GetPath();
         m_nFailureMarkOrPageId = pFailureAction->GetPageMarkId();
      }
      if (m_nFailureAction == INTERACTION_JUMP_SPECIFIC_PAGE /*||
          m_failureAction == SPECIFIC_PAGE_STARTREPLAY ||
          m_failureAction == SPECIFIC_PAGE_STOPREPLAY*/)
      {
         m_csFailureActionPath = m_pEditorProject->GetPageTitleForId(m_nFailureMarkOrPageId);
      }
      else if (m_nFailureAction == INTERACTION_JUMP_TARGET_MARK /*||
               m_failureAction == TARGET_MARK_STARTREPLAY ||
               m_failureAction == TARGET_MARK_STOPREPLAY*/)
      {
         CStopJumpMark *pMark = m_pEditorProject->GetMarkWithId(m_nFailureMarkOrPageId);
         if (pMark != NULL)
            m_csFailureActionPath = pMark->GetLabel();
         // TODO else this id is undefined
      }


      CFeedback *pFeedback = NULL;

      for (int i = 0; i < 4; ++i)
      {
         pFeedback = pQuestion->GetFeedback(m_feedBackTypes[i]);
         if (pFeedback)
         {
            m_csFeedback[i] = pFeedback->GetText();
            m_bShowFeedback[i] = !pFeedback->IsDisabled();
            m_pFeedbacks[i] = pFeedback;
         }
         else
         {
            m_pFeedbacks[i] = NULL;
         }
      }
   }
}

void CTestOptionPage::GetUserEntries(CQuestionnaireEx *pQuestionnaire, CTimePeriod  &visualityPeriod, CTimePeriod &activityPeriod, 
                                     CMouseAction *pCorrectAction, CMouseAction *pFailureAction, 
                                     CArray<CFeedback *, CFeedback *> *paSpecificFeedback)
{
   //AreaActionTypeId nNextAction = INTERACTION_NO_ACTION;

   //AreaActionTypeId nCorrectAction = CMouseAction::GetActionId(m_correctAction, nNextAction);
   //nNextAction = m_nCorrectNextAction;
   pCorrectAction->Init(m_pEditorProject, m_nCorrectAction, m_nCorrectMarkOrPageId, 
                        m_csCorrectActionPath, m_nCorrectNextAction);
 
   
   //AreaActionTypeId nFailureAction = CMouseAction::GetActionId(m_failureAction, nNextAction);
   //nNextAction = m_nFailureNextAction;
   pFailureAction->Init(m_pEditorProject, m_nFailureAction, m_nFailureMarkOrPageId, 
                        m_csFailureActionPath, m_nFailureNextAction);


   CFeedback *pFeedback = NULL;
   
   CSize siPage;
   m_pEditorProject->GetPageDimension(siPage);
   
   for (int i = 0; i < 4; ++i)
   {
      CFeedback *pFeedback = 
         CQuestionnaireEx::CreateFeedback(m_pEditorProject, pQuestionnaire, m_feedBackTypes[i], m_csFeedback[i], 
                                   !m_bShowFeedback[i], visualityPeriod);

      if (m_pFeedbacks[i] != NULL)
         pFeedback->SetHashKey(m_pFeedbacks[i]->GetHashKey());
 
      paSpecificFeedback->Add(pFeedback);
   }
  
}

void CTestOptionPage::OnBnClickedCorrectStartReplay()
{
	// TODO: Add your control notification handler code here
	if (m_cbCorrectStartReplay.GetCheck())
		m_cbCorrectStopReplay.SetCheck(0);
}

void CTestOptionPage::OnBnClickedCorrectStopReplay()
{
	// TODO: Add your control notification handler code here
	if (m_cbCorrectStopReplay.GetCheck())
		m_cbCorrectStartReplay.SetCheck(0);
}

void CTestOptionPage::OnBnClickedFailureStartReplay()
{
	// TODO: Add your control notification handler code here
	if (m_cbFailureStartReplay.GetCheck())
		m_cbFailureStopReplay.SetCheck(0);
}

void CTestOptionPage::OnBnClickedFailureStopReplay()
{
	// TODO: Add your control notification handler code here
	if (m_cbFailureStopReplay.GetCheck())
		m_cbFailureStartReplay.SetCheck(0);
}

// QuestionnaireQuestionPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "QuestionnaireQuestionPage.h"
#include "editorDoc.h"
#include "MfcUtils.h"
#include "QuestionStream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "lutils.h"

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CQuestionnaireQuestionPage 

IMPLEMENT_DYNCREATE(CQuestionnaireQuestionPage, CPropertyPage)

CQuestionnaireQuestionPage::CQuestionnaireQuestionPage() : CPropertyPage(CQuestionnaireQuestionPage::IDD)
{
	//{{AFX_DATA_INIT(CQuestionnaireQuestionPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   
   m_csCaption.LoadString(CQuestionnaireQuestionPage::IDD);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   for (int i = 0; i < 6; ++i)
      m_pFeedback[i] = NULL;
}

CQuestionnaireQuestionPage::~CQuestionnaireQuestionPage()
{
}

void CQuestionnaireQuestionPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuestionnaireQuestionPage)
	DDX_Control(pDX, IDC_SCROLL_SUCCESS_POINTS, m_scrollPoints);
	DDX_Control(pDX, IDC_SCROLL_SUCCESS_PERCENTAGE, m_scrollPercentage);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuestionnaireQuestionPage, CPropertyPage)
	//{{AFX_MSG_MAP(CQuestionnaireQuestionPage)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
   ON_UPDATE_COMMAND_UI(IDC_EDIT_SUCCESS_PERCENTAGE, OnUpdateEditPercentage)
   ON_UPDATE_COMMAND_UI(IDC_EDIT_SUCCESS_POINTS, OnUpdateEditPoints)
	ON_EN_CHANGE(IDC_EDIT_SUCCESS_PERCENTAGE, OnChangeEditSuccessPercentage)
	ON_EN_CHANGE(IDC_EDIT_SUCCESS_POINTS, OnChangeEditSuccessPoints)
   ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CQuestionnaireQuestionPage 

BOOL CQuestionnaireQuestionPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
   DWORD dwIds[] = {IDC_QUESTION_COMMON,
                    IDC_LABEL_NAME,
                    IDC_LABEL_FEEDBACK,
                    IDC_CHECK_ANSWER_CORRECT,
                    IDC_CHECK_ANSWER_ERROR,
                    IDC_CHECK_ANSWER_TIMEOUT,
                    IDC_CHECK_ANSWER_RECAPITULATE,
                    IDC_CHECK_ANSWER_SUCCESS,
                    IDC_CHECK_ANSWER_FAILED,
                    IDC_SUCCESS_PERCENTAGE,
                    IDC_LABEL_PERCENTAGE,
                    IDC_LABEL_POINTS,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_scrollPercentage.SetScrollRange(1, 100);
   m_scrollPoints.SetScrollRange(1, 100);

   m_editName.SetWindowText(m_csName);
   CButton *pButton = NULL;
   CEdit *pEdit = NULL;

   // Feedback 
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_CORRECT);
   if (pButton)
      pButton->SetCheck(m_bFeedbackChecked[0]);
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_CORRECT);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[0]);

   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_ERROR);
   if (pButton)
      pButton->SetCheck(m_bFeedbackChecked[1]);
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_ERROR);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[1]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_RECAPITULATE);
   if (pButton)
      pButton->SetCheck(m_bFeedbackChecked[2]);
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_RECAPITULATE);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[2]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_TIMEOUT);
   if (pButton)
      pButton->SetCheck(m_bFeedbackChecked[3]);
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_TIMEOUT);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[3]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_FAILED);
   if (pButton)
      pButton->SetCheck(m_bFeedbackChecked[4]);
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_FAILED);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[4]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_SUCCESS);
   if (pButton)
      pButton->SetCheck(m_bFeedbackChecked[5]);
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_SUCCESS);
   if (pEdit)
      pEdit->SetWindowText(m_csFeedback[5]);

   
   // Points
   if (m_bPassTypeRelativ)
   {
      CheckRadioButton(IDC_RADIO_SUCCESS_PERCENTAGE, IDC_RADIO_SUCCESS_POINTS, IDC_RADIO_SUCCESS_PERCENTAGE);
      m_scrollPercentage.SetScrollPos(m_nNeededPoints);
      m_scrollPoints.SetScrollPos(0);
   }
   else
   {
      CheckRadioButton(IDC_RADIO_SUCCESS_PERCENTAGE, IDC_RADIO_SUCCESS_POINTS, IDC_RADIO_SUCCESS_POINTS);
      m_scrollPercentage.SetScrollPos(0);
      m_scrollPoints.SetScrollPos(m_nNeededPoints);
   }

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CQuestionnaireQuestionPage::OnKickIdle()
{
   UpdateDialogControls(this, FALSE);
}

void CQuestionnaireQuestionPage::OnUpdateEditPercentage(CCmdUI *pCmdUI)
{
   int iNumber = m_scrollPercentage.GetScrollPos();
   CString csNumber;
   csNumber.Format(_T("%d"), iNumber);
   pCmdUI->SetText(csNumber);
   
   int iCheckedRadioBox = GetCheckedRadioButton(IDC_RADIO_SUCCESS_PERCENTAGE, IDC_RADIO_SUCCESS_POINTS);
   bool bEnable = FALSE;

   if (iCheckedRadioBox == IDC_RADIO_SUCCESS_PERCENTAGE)
      bEnable = TRUE;
   
   m_scrollPercentage.EnableWindow(bEnable);
   pCmdUI->Enable(bEnable);
}

void CQuestionnaireQuestionPage::OnUpdateEditPoints(CCmdUI *pCmdUI)
{
   
   int iNumber = m_scrollPoints.GetScrollPos();
   CString csNumber;
   csNumber.Format(_T("%d"), iNumber);
   pCmdUI->SetText(csNumber);
   
   int iCheckedRadioBox = GetCheckedRadioButton(IDC_RADIO_SUCCESS_PERCENTAGE, IDC_RADIO_SUCCESS_POINTS);
   bool bEnable = FALSE;

   if (iCheckedRadioBox == IDC_RADIO_SUCCESS_POINTS)
      bEnable = TRUE;
   
   m_scrollPoints.EnableWindow(bEnable);
   pCmdUI->Enable(bEnable);
}

void CQuestionnaireQuestionPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int iCurrentPos = pScrollBar->GetScrollPos();
   if (nSBCode == SB_LINEDOWN)
   {
      --iCurrentPos;
      if (iCurrentPos < 1)
         iCurrentPos = 1;
   }
   else if (nSBCode == SB_LINEUP)
   {
      ++iCurrentPos;
      if (iCurrentPos > 100)
         iCurrentPos = 100;
   }

	pScrollBar->SetScrollPos(iCurrentPos);
	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CQuestionnaireQuestionPage::OnChangeEditSuccessPercentage() 
{
   CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SUCCESS_PERCENTAGE);

   if (pEdit)
   {
      CString csWindowText;
      pEdit->GetWindowText(csWindowText);
      int iNumber = _ttoi(csWindowText);
      if (iNumber >= 1 && iNumber <= 100)
         m_scrollPercentage.SetScrollPos(iNumber);
   }
}

void CQuestionnaireQuestionPage::OnChangeEditSuccessPoints() 
{
   CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SUCCESS_POINTS);

   if (pEdit)
   {
      CString csWindowText;
      pEdit->GetWindowText(csWindowText);
      int iNumber = _ttoi(csWindowText);
      if (iNumber >= 1 && iNumber <= 100)
         m_scrollPoints.SetScrollPos(iNumber);
   }
}

void CQuestionnaireQuestionPage::OnOK() 
{
   CEdit *pEdit = NULL;

   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_NAME);
   if (pEdit)
       pEdit->GetWindowText(m_csName);

   CButton *pButton = NULL;

   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_CORRECT);
   if (pButton)
      m_bFeedbackChecked[0] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_CORRECT);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[0]);

   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_ERROR);
   if (pButton)
      m_bFeedbackChecked[1] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_ERROR);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[1]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_RECAPITULATE);
   if (pButton)
      m_bFeedbackChecked[2] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_RECAPITULATE);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[2]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_TIMEOUT);
   if (pButton)
      m_bFeedbackChecked[3] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_TIMEOUT);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[3]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_FAILED);
   if (pButton)
      m_bFeedbackChecked[4] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_FAILED);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[4]);
   
   pButton = (CButton *)GetDlgItem(IDC_CHECK_ANSWER_SUCCESS);
   if (pButton)
      m_bFeedbackChecked[5] = pButton->GetCheck() == 1 ? true : false;
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ANSWER_SUCCESS);
   if (pEdit)
      pEdit->GetWindowText(m_csFeedback[5]);
   
   int iCheckedRadioButton = GetCheckedRadioButton(IDC_RADIO_SUCCESS_PERCENTAGE, 
                                                   IDC_RADIO_SUCCESS_POINTS);

   m_bPassTypeRelativ = iCheckedRadioButton == IDC_RADIO_SUCCESS_PERCENTAGE ? true : false;
   if (m_bPassTypeRelativ)
      m_nNeededPoints = m_scrollPercentage.GetScrollPos();
   else
      m_nNeededPoints = m_scrollPoints.GetScrollPos();
   
	CPropertyPage::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// public functions

void CQuestionnaireQuestionPage::Init(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire)
{
   if (pQuestionnaire)
   {
      m_csName = pQuestionnaire->GetTitle();
      
      CFeedback *pFeedback = NULL;

      pFeedback = pQuestionnaire->GetFeedback(FEEDBACK_TYPE_Q_CORRECT);
      if (pFeedback)
      {
         m_csFeedback[0] = pFeedback->GetText();
         m_bFeedbackChecked[0] = !pFeedback->IsDisabled();
         m_pFeedback[0] = pFeedback;
      }
      else
      {
         m_csFeedback[0].LoadString(IDS_ANSWER_CORRECT);
         m_bFeedbackChecked[0] = true;
         m_pFeedback[0] = NULL;
      }

      pFeedback = pQuestionnaire->GetFeedback(FEEDBACK_TYPE_Q_WRONG);
      if (pFeedback)
      {
         m_csFeedback[1] = pFeedback->GetText();
         m_bFeedbackChecked[1] = !pFeedback->IsDisabled();
         m_pFeedback[1] = pFeedback;
      }
      else
      {
         m_csFeedback[1].LoadString(IDS_ANSWER_WRONG);
         m_bFeedbackChecked[1] = true;
         m_pFeedback[1] = NULL;
      }

      pFeedback = pQuestionnaire->GetFeedback(FEEDBACK_TYPE_Q_REPEAT);
      if (pFeedback)
      {
         m_csFeedback[2] = pFeedback->GetText();
         m_bFeedbackChecked[2] = !pFeedback->IsDisabled();
         m_pFeedback[2] = pFeedback;
      }
      else
      {
         m_csFeedback[2].LoadString(IDS_ANSWER_RECAPITULATE);
         m_bFeedbackChecked[2] = true;
         m_pFeedback[2] = NULL;
      }

      pFeedback = pQuestionnaire->GetFeedback(FEEDBACK_TYPE_Q_TIME);
      if (pFeedback)
      {
         m_csFeedback[3] = pFeedback->GetText();
         m_bFeedbackChecked[3] = !pFeedback->IsDisabled();
         m_pFeedback[3] = pFeedback;
      }
      else
      {
         m_csFeedback[3].LoadString(IDS_ANSWER_TIMEOUT);
         m_bFeedbackChecked[3] = true;
         m_pFeedback[3] = NULL;
      }

      pFeedback = pQuestionnaire->GetFeedback(FEEDBACK_TYPE_QQ_FAILED);
      if (pFeedback)
      {
         m_csFeedback[4] = pFeedback->GetText();
         m_bFeedbackChecked[4] = !pFeedback->IsDisabled();
         m_pFeedback[4] = pFeedback;
      }
      else
      {
         m_csFeedback[4].LoadString(IDS_ANSWER_FAILED);
         m_bFeedbackChecked[4] = false;
         m_pFeedback[4] = NULL;
      }

      pFeedback = pQuestionnaire->GetFeedback(FEEDBACK_TYPE_QQ_PASSED);
      if (pFeedback)
      {
         m_csFeedback[5] = pFeedback->GetText();
         m_bFeedbackChecked[5] = !pFeedback->IsDisabled();
         m_pFeedback[5] = pFeedback;
      }
      else
      {
         m_csFeedback[5].LoadString(IDS_ANSWER_SUCCESS);
         m_bFeedbackChecked[5] = false;
         m_pFeedback[5] = NULL;
      }

      m_nNeededPoints = pQuestionnaire->GetNeededPoints();
      m_bPassTypeRelativ = pQuestionnaire->IsPassTypeRelative();
   }
   else
   {
      CQuestionStream *pQuestionStream = pProject->GetQuestionStream(false);
      UINT nCount = 1;
      if (pQuestionStream != NULL)
         nCount = pQuestionStream->GetQuestionnaireCount() + 1;
      m_csName.Format(IDS_QUESTIONNAIRE_TITLE, nCount);

      m_csFeedback[0].LoadString(IDS_ANSWER_CORRECT);
      m_bFeedbackChecked[0] = true;
      m_pFeedback[0] = NULL;
      m_csFeedback[1].LoadString(IDS_ANSWER_WRONG);
      m_bFeedbackChecked[1] = true;
      m_pFeedback[1] = NULL;
      m_csFeedback[2].LoadString(IDS_ANSWER_RECAPITULATE);
      m_bFeedbackChecked[2] = true;
      m_pFeedback[2] = NULL;
      m_csFeedback[3].LoadString(IDS_ANSWER_TIMEOUT);
      m_bFeedbackChecked[3] = true;
      m_pFeedback[3] = NULL;
      m_csFeedback[4].LoadString(IDS_ANSWER_FAILED);
      m_bFeedbackChecked[4] = true;
      m_pFeedback[4] = NULL;
      m_csFeedback[5].LoadString(IDS_ANSWER_SUCCESS);
      m_bFeedbackChecked[5] = true;
      m_pFeedback[5] = NULL;
      
      m_nNeededPoints = 100;
      m_bPassTypeRelativ = true;
   }


}

void CQuestionnaireQuestionPage::GetUserEntries(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire, 
                                                CString &csName, bool &bPassTypeRelative, 
                                                UINT &nNeededPoints,
                                                CArray<CFeedback *, CFeedback *> *paGeneralFeedback)
{
   csName = m_csName;
   bPassTypeRelative = m_bPassTypeRelativ;
   nNeededPoints = m_nNeededPoints;
   
   CFeedback *pFeedback = NULL;
   
   pFeedback = new CFeedback();
   pFeedback->Init(FEEDBACK_TYPE_Q_CORRECT, !m_bFeedbackChecked[0], false, m_csFeedback[0], NULL, NULL); 
   if (m_pFeedback[0] != NULL)
      pFeedback->SetHashKey(m_pFeedback[0]->GetHashKey());
   paGeneralFeedback->Add(pFeedback);

   pFeedback = new CFeedback();
   pFeedback->Init(FEEDBACK_TYPE_Q_WRONG, !m_bFeedbackChecked[1], false, m_csFeedback[1], NULL, NULL);
   if (m_pFeedback[1] != NULL)
      pFeedback->SetHashKey(m_pFeedback[1]->GetHashKey()); 
   paGeneralFeedback->Add(pFeedback);
   
   pFeedback = new CFeedback();
   pFeedback->Init(FEEDBACK_TYPE_Q_REPEAT, !m_bFeedbackChecked[2], false, m_csFeedback[2], NULL, NULL); 
   if (m_pFeedback[2] != NULL)  
      pFeedback->SetHashKey(m_pFeedback[2]->GetHashKey());
   paGeneralFeedback->Add(pFeedback);
   
   pFeedback = new CFeedback();
   pFeedback->Init(FEEDBACK_TYPE_Q_TIME, !m_bFeedbackChecked[3], false, m_csFeedback[3], NULL, NULL); 
   if (m_pFeedback[3] != NULL)
      pFeedback->SetHashKey(m_pFeedback[3]->GetHashKey());
   paGeneralFeedback->Add(pFeedback);
   
   CEditorDoc *pDoc = pProject->m_pDoc;

   CTimePeriod visualityPeriod;
   visualityPeriod.Init(INTERACTION_PERIOD_ALL_PAGES, 0, pDoc->m_displayEndMs);

   pFeedback = CQuestionnaireEx::CreateFeedback(pProject, pQuestionnaire, FEEDBACK_TYPE_QQ_FAILED, 
                                               m_csFeedback[4], !m_bFeedbackChecked[4], visualityPeriod);  
   if (m_pFeedback[4] != NULL)
      pFeedback->SetHashKey(m_pFeedback[4]->GetHashKey());
   paGeneralFeedback->Add(pFeedback);

   pFeedback = CQuestionnaireEx::CreateFeedback(pProject, pQuestionnaire, FEEDBACK_TYPE_QQ_PASSED, 
                                               m_csFeedback[5], !m_bFeedbackChecked[5], visualityPeriod);  
   if (m_pFeedback[5] != NULL)
      pFeedback->SetHashKey(m_pFeedback[5]->GetHashKey());
   paGeneralFeedback->Add(pFeedback);
   

   pFeedback= CQuestionnaireEx::CreateResultFeedback(pProject, pQuestionnaire);

   paGeneralFeedback->Add(pFeedback);

}
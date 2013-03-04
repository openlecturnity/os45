// TestAnswerPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "TestAnswerPage.h"
#include "QuestionStream.h"
#include "EditorDoc.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CTestAnswerPage 

CTestAnswerPage::CTestAnswerPage(int iPageID) : CPropertyPage(iPageID)
{
	//{{AFX_DATA_INIT(CTestAnswerPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   
   m_pEditorProject = NULL;
}

CTestAnswerPage::~CTestAnswerPage()
{
}

void CTestAnswerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestAnswerPage)
	DDX_Control(pDX, IDC_EDIT_TIME_LIMIT, m_editTimeLimit);
	DDX_Control(pDX, IDC_SCROLL_TRIES, m_scrollTries);
	DDX_Control(pDX, IDC_TRIES_COUNT, m_editTries);
	DDX_Control(pDX, IDC_EDIT_SUCCESS_POINTS, m_editSuccess);
	DDX_Control(pDX, IDC_SCROLL_POINTS, m_scrollPoints);
   DDX_Control(pDX, IDC_EDIT_QUESTION_ID, m_editQuestionID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestAnswerPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTestAnswerPage)
   ON_UPDATE_COMMAND_UI(IDC_TRIES_COUNT, OnUpdateTries)
   ON_UPDATE_COMMAND_UI(IDC_EDIT_SUCCESS_POINTS, OnUpdatePoints)
   ON_UPDATE_COMMAND_UI(IDC_EDIT_TIME_LIMIT, OnUpdateTimeLimit)
	ON_EN_CHANGE(IDC_EDIT_SUCCESS_POINTS, OnChangeEditSuccessPoints)
	ON_EN_CHANGE(IDC_TRIES_COUNT, OnChangeEditTries)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTestAnswerPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   DWORD dwIds[] = {IDC_GROUP_TRIES,
                    IDC_RADIO_TRIES,
                    IDC_RADIO_NO_TRY_LIMIT,
                    IDC_RADIO_CORRECT_POINTS,
                    IDC_GROUP_VALIDATION,
                    IDC_POINTS,
                    IDC_RADIO_NO_RATING,
                    IDC_GROUP_TIMELIMIT,
                    IDC_CHECK_LIMIT_TIME,
                    IDC_GROUP_DATA_TRANSMISSION,
                    IDC_QUESTION_IDENTIFIER,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_scrollTries.SetScrollRange(0, 100);
   m_scrollPoints.SetScrollRange(0, 100);
   
   CButton *pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_LIMIT_TIME);
   if (pCheckButton)
      pCheckButton->SetCheck(m_bTimeLimit);

   UINT nAvailableTime = m_nAvailableTimeSec == 0 ? 5 * 60 : m_nAvailableTimeSec;
   UINT nMin = nAvailableTime / 60;
   UINT nSec = nAvailableTime - (nMin * 60);
   m_editTimeLimit.SetTime(nMin, nSec);
   
   m_scrollTries.SetScrollPos(m_nMaximumTries > 0 ? m_nMaximumTries : 3);
   if (m_bTryLimit)
      CheckRadioButton(IDC_RADIO_TRIES, IDC_RADIO_NO_TRY_LIMIT, IDC_RADIO_TRIES);
   else
      CheckRadioButton(IDC_RADIO_TRIES, IDC_RADIO_NO_TRY_LIMIT, IDC_RADIO_NO_TRY_LIMIT);

   m_scrollPoints.SetScrollPos(m_nAchievablePoints);
   if (m_bNoRating)
      CheckRadioButton(IDC_RADIO_CORRECT_POINTS, IDC_RADIO_NO_RATING, IDC_RADIO_NO_RATING);
   else
      CheckRadioButton(IDC_RADIO_CORRECT_POINTS, IDC_RADIO_NO_RATING, IDC_RADIO_CORRECT_POINTS);
   
   m_editQuestionID.SetWindowText(m_csQuestionId);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTestAnswerPage 

void CTestAnswerPage::OnOK() 
{
   CEdit *pEdit = NULL;

   int iCheckedRadioBox = GetCheckedRadioButton(IDC_RADIO_CORRECT_POINTS, IDC_RADIO_NO_RATING);
   if (iCheckedRadioBox == IDC_RADIO_NO_RATING)
      m_nAchievablePoints = 0;
   else
      m_nAchievablePoints = m_scrollPoints.GetScrollPos();

   iCheckedRadioBox = GetCheckedRadioButton(IDC_RADIO_TRIES, IDC_RADIO_NO_TRY_LIMIT);
   if (iCheckedRadioBox == IDC_RADIO_NO_TRY_LIMIT)
      m_nMaximumTries = 0;
   else
      m_nMaximumTries = m_scrollTries.GetScrollPos();

   CButton *pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_LIMIT_TIME);
   int iChecked = pCheckButton->GetCheck();
   if (iChecked == 0)
      m_nAvailableTimeSec = 0;
   else
   {
      m_editTimeLimit.ComputeDateTime();
      int mm = m_editTimeLimit.GetMins();
      int ss = m_editTimeLimit.GetSecs();
      m_nAvailableTimeSec = mm * 60 + ss;
   }

   m_editQuestionID.GetWindowText(m_csQuestionId);

	CPropertyPage::OnOK();
}

void CTestAnswerPage::OnUpdateTries(CCmdUI *pCmdUI)
{
   int iNumber = m_scrollTries.GetScrollPos();
   CString csNumber;
   csNumber.Format(_T("%d"), iNumber);
   pCmdUI->SetText(csNumber);

   int iCheckedRadioBox = GetCheckedRadioButton(IDC_RADIO_TRIES, IDC_RADIO_NO_TRY_LIMIT);
   bool bEnable = FALSE;

   if (iCheckedRadioBox == IDC_RADIO_TRIES)
      bEnable = TRUE;
   
   m_scrollTries.EnableWindow(bEnable);
   pCmdUI->Enable(bEnable);
}


void CTestAnswerPage::OnUpdatePoints(CCmdUI *pCmdUI)
{
   
   int iNumber = m_scrollPoints.GetScrollPos();
   CString csNumber;
   csNumber.Format(_T("%d"), iNumber);
   pCmdUI->SetText(csNumber);
   
   int iCheckedRadioBox = GetCheckedRadioButton(IDC_RADIO_CORRECT_POINTS, IDC_RADIO_NO_RATING);
   bool bEnable = FALSE;

   if (iCheckedRadioBox == IDC_RADIO_CORRECT_POINTS)
      bEnable = TRUE;
   
   m_scrollPoints.EnableWindow(bEnable);
   pCmdUI->Enable(bEnable);
}

void CTestAnswerPage::OnUpdateTimeLimit(CCmdUI *pCmdUI)
{  
   bool bEnable = FALSE;

   CButton *pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_LIMIT_TIME);
   int iChecked = pCheckButton->GetCheck();

   if (iChecked == 1)
      bEnable = TRUE;
   
   pCmdUI->Enable(bEnable);
}

void CTestAnswerPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CTestAnswerPage::OnChangeEditSuccessPoints() 
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

void CTestAnswerPage::OnChangeEditTries() 
{
   CString csWindowText;
   m_editTries.GetWindowText(csWindowText);
   int iNumber = _ttoi(csWindowText);
   if (iNumber >= 1 && iNumber <= 100)
      m_scrollTries.SetScrollPos(iNumber);
   
}

/////////////////////////////////////////////////////////////////////////////
// public functions

void CTestAnswerPage::Init(CQuestionEx *pQuestion, CEditorProject *pProject)
{
   m_pEditorProject = pProject;
   
   m_nAvailableTimeSec = 5*60; // 5 minutes
   m_bTimeLimit = false;
   
   m_nMaximumTries = 3;
   m_bTryLimit = false;
   
   m_nAchievablePoints = 1;
   m_bNoRating = false;

   CQuestionStream *pQuestionStream = pProject->GetQuestionStream();
   if (pQuestionStream)
      m_csQuestionId = pQuestionStream->CreateScormId();

   if (pQuestion)
   {
      m_nAvailableTimeSec = pQuestion->GetAvailableSeconds();
      if (m_nAvailableTimeSec == 0)
         m_bTimeLimit = false;
      else
         m_bTimeLimit = true;
   
      m_nMaximumTries = pQuestion->GetMaximumTries();
      if (m_nMaximumTries == 0)
         m_bTryLimit = false;
      else
         m_bTryLimit = true;
   
      m_nAchievablePoints = pQuestion->GetAchievablePoints();
      if (m_nAchievablePoints == 0)
         m_bNoRating = true;
      else
         m_bNoRating = false;

      m_csQuestionId = pQuestion->GetScormId();
   }
}

void CTestAnswerPage::GetUserEntries(UINT &nAchievablePoints, UINT &nMaximumTries, 
                                     UINT &nAvailableTimeMs, CString &csScormId)
{
   nAchievablePoints = m_nAchievablePoints;
   nMaximumTries = m_nMaximumTries;
   nAvailableTimeMs = m_nAvailableTimeSec * 1000;

   csScormId = m_csQuestionId;
}

// TestSettings.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "TestSettings.h"
#include "ChangeableObject.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "lutils.h"

/////////////////////////////////////////////////////////////////////////////
// CTestSettings

IMPLEMENT_DYNAMIC(CTestSettings, CPropertySheet)

CTestSettings::CTestSettings(UINT nIDCaption, UINT nAnswerPageID, CWnd* pParentWnd, UINT iSelectPage)
:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   m_bIsQuestionnaireSettingsShown = false;
   m_pTestQuestionPage = new CTestQuestionPage();
   AddPage(m_pTestQuestionPage);
   
   m_questionType = QUESTION_TYPE_NOTHING;
   switch (nAnswerPageID)
   {
   case ID_ADD_QUESTION_DD:
      m_pTestAnswerPage = new CTestDragAndDropPage();
      m_questionType = QUESTION_TYPE_DRAGNDROP;
      break;
   case ID_ADD_QUESTION_FB:
      m_pTestAnswerPage = new CTestFillInBlankPage();
      m_questionType = QUESTION_TYPE_TEXTGAP;
      break;
   case ID_ADD_QUESTION_MC:
      m_pTestAnswerPage = new CTestMultipleChoicePage();
      m_questionType = QUESTION_TYPE_CHOICE;
      break;
   }
   AddPage(m_pTestAnswerPage);
   
   m_pTestOptionPage = new CTestOptionPage();
   AddPage(m_pTestOptionPage);
   
   m_pEditorProject = NULL;

   
   m_pTitleText = NULL;
   m_bUseTitleHashKey = false;
   m_nTitleHashKey = 0;

   m_pQuestionText = NULL;
   m_bUseQuestionHashKey = false;
   m_nQuestionHashKey = 0;

   m_pTimerText = NULL;
   m_bUseTimerTextHashKey = false;
   m_nTimerTextHashKey = 0;

   m_pTimerObject = NULL;
   m_bUseTimerObjectHashKey = false;
   m_nTimerObjectHashKey = 0;

   m_pTriesText = NULL;
   m_bUseTriesTextHashKey = false;
   m_nTriesTextHashKey = 0;

   m_pTriesObject = NULL;
   m_bUseTriesObjectHashKey = false;
   m_nTriesObjectHashKey = 0;

   m_pProgressText = NULL;
   m_bUseProgressTextHashKey = false;
   m_nProgressTextHashKey = 0;

   m_pProgressObject = NULL;
   m_bUseProgressObjectHashKey = false;
   m_nProgressObjectHashKey = 0;

   
   m_pSubmitButton = NULL;
   m_pDeleteButton = NULL;

   m_dUpperBound = CQuestionnaireEx::TOP_BORDER;
   
   m_bButtonsChanged = false;
}

void CTestSettings::Init(CQuestionEx *pQuestion, CEditorProject *pProject)
{
   m_pTestQuestionPage->Init(pQuestion, pProject);
   m_pTestAnswerPage->Init(pQuestion, pProject);
   m_pTestOptionPage->Init(pQuestion, pProject);

   double dScaleX, dScaleY;
   CQuestionEx::GetQuestionScale(pProject, dScaleX, dScaleY);

   m_dUpperBound = (double)CQuestionnaireEx::TOP_BORDER * dScaleY;

   if (pQuestion)
   {
      
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
      pQuestion->GetObjectPointers(&aObjects);
      
      for (int i = 0; i < aObjects.GetSize(); ++i)
      {   
         CInteractionAreaEx *pInteraction = aObjects[i];
         
         if (pInteraction != NULL)
         {
            if (pInteraction->GetClassId() == INTERACTION_CLASS_MOVE)
            {
               CMoveableObjects *pMoveableObject = (CMoveableObjects *)pInteraction;
               CArray<DrawSdk::DrawObject *,DrawSdk::DrawObject *> *paObjects = pMoveableObject->GetObjects();
               for (int j = 0; j < paObjects->GetSize(); ++j)
               {
                  DrawSdk::DrawObject *pObject = paObjects->GetAt(j);
                  
                  if (pObject != NULL && pObject->GetType() == DrawSdk::TEXT)
                  {
                     DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
                     if (pText)
                     {  
                        switch (pText->GetTextType())
                        {
                        case DrawSdk::TEXT_TYPE_QUESTION_TITLE:
                           m_pTitleText = pText;
                           m_bUseTitleHashKey = true;
                           m_nTitleHashKey = pMoveableObject->GetHashKey();
                           break;
                        case DrawSdk::TEXT_TYPE_QUESTION_TEXT:
                           if (m_pQuestionText == NULL)
                              m_pQuestionText = pText;
                           m_bUseQuestionHashKey = true;
                           m_nQuestionHashKey = pMoveableObject->GetHashKey();
                           break;
                        case DrawSdk::TEXT_TYPE_TIME_SUPPORT:
                           m_pTimerText = pText;
                           m_bUseTimerTextHashKey = true;
                           m_nTimerTextHashKey = pMoveableObject->GetHashKey();
                           break;
                        case DrawSdk::TEXT_TYPE_TRIES_SUPPORT:
                           m_pTriesText = pText;
                           m_bUseTriesTextHashKey = true;
                           m_nTriesTextHashKey = pMoveableObject->GetHashKey();
                           break;
                        case DrawSdk::TEXT_TYPE_NUMBER_SUPPORT:
                           m_pProgressText = pText;
                           m_bUseProgressTextHashKey = true;
                           m_nProgressTextHashKey = pMoveableObject->GetHashKey();
                           break;
                        }
                     }
                  }
               }
            }
            if (pInteraction->GetClassId() == INTERACTION_CLASS_CHANGE)
            {
               CChangeableObject *pChangeableObject = (CChangeableObject *)pInteraction;
               DrawSdk::DrawObject *pObject = pChangeableObject->GetObject();
               
               if (pObject != NULL && pObject->GetType() == DrawSdk::TEXT)
               {
                  DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
                  if (pText)
                  {  
                     switch (pText->GetTextType())
                     {
                     case DrawSdk::TEXT_TYPE_CHANGE_TIME:
                        m_pTimerObject = pText;
                        m_bUseTimerObjectHashKey = true;
                        m_nTimerObjectHashKey = pChangeableObject->GetHashKey();
                        break;
                     case DrawSdk::TEXT_TYPE_CHANGE_TRIES:
                        m_pTriesObject = pText;
                        m_bUseTriesObjectHashKey = true;
                        m_nTriesObjectHashKey = pChangeableObject->GetHashKey();
                        break;
                     case DrawSdk::TEXT_TYPE_CHANGE_NUMBER:
                        m_pProgressObject = pText;
                        m_bUseProgressObjectHashKey = true;
                        m_nProgressObjectHashKey = pChangeableObject->GetHashKey();
                        break;
                     }
                  }
               }
            }
            if (pInteraction->GetClassId() == INTERACTION_CLASS_BUTTON)
            {
               if (pInteraction->GetButtonType() == BUTTON_TYPE_SUBMIT)
                  m_pSubmitButton = pInteraction;
               else if (pInteraction->GetButtonType() == BUTTON_TYPE_DELETE)
                  m_pDeleteButton = pInteraction;

            }
         }
      }
   }

   m_pEditorProject = pProject;
}

CTestSettings::~CTestSettings()
{
   if (m_pTestQuestionPage)
      delete m_pTestQuestionPage;
   
   if (m_pTestAnswerPage)
      delete m_pTestAnswerPage;
   
   if (m_pTestOptionPage)
      delete m_pTestOptionPage;
}


BEGIN_MESSAGE_MAP(CTestSettings, CPropertySheet)
//{{AFX_MSG_MAP(CTestSettings)
	ON_COMMAND(IDC_SHOW_QUESTIONAIRE_SETTINGS, OnShowQuestionaireSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTestSettings 

BOOL CTestSettings::OnInitDialog() 
{
   BOOL bResult = CPropertySheet::OnInitDialog();
   
   DWORD dwIds[] = {IDOK,
                    IDCANCEL,
                    -1};

   MfcUtils::Localize(this, dwIds);

   // delete apply and help button
   CWnd *pApply = GetDlgItem(ID_APPLY_NOW);
   if(pApply != NULL)
      pApply->DestroyWindow();
   
   // replace OK an Cancel button
   CRect rectBtn;
   int nSpacing = 6;        // space between two buttons...
   
   // use help to show test properties
   
   CWnd *pHelp = GetDlgItem(IDHELP);
   if(pHelp != NULL)
   {
      CString csTest;
      csTest.LoadString(IDC_TEST);
      pHelp->SetDlgCtrlID(IDC_SHOW_QUESTIONAIRE_SETTINGS);
      pHelp->SetWindowText(csTest);
      pHelp->GetWindowRect(rectBtn);
      ScreenToClient (&rectBtn);
      int btnWidth = rectBtn.Width();
      rectBtn.left = nSpacing;
      rectBtn.right = nSpacing + btnWidth;
      pHelp->MoveWindow(rectBtn);
      CWnd *pWnd = GetDlgItem(IDC_SHOW_QUESTIONAIRE_SETTINGS);
   }


   int ids [] = {IDOK, IDCANCEL};
   for( int i =0; i < sizeof(ids)/sizeof(int); i++)
   {
      GetDlgItem (ids [i])->GetWindowRect (rectBtn);
      
      ScreenToClient (&rectBtn);
      int btnWidth = rectBtn.Width();
      rectBtn.left = rectBtn.left + (btnWidth + nSpacing)* 2;
      rectBtn.right = rectBtn.right + (btnWidth + nSpacing)* 2;
      
      GetDlgItem (ids [i])->MoveWindow(rectBtn);
   }
   
   CPropertyPage *pActivePage = GetActivePage();
   SetActivePage(m_pTestQuestionPage);
   SetActivePage(m_pTestAnswerPage);
   SetActivePage(m_pTestOptionPage);
   
   SetActivePage(pActivePage);
   return bResult;
}

BOOL CTestSettings::ContinueModal() 
{
   CPropertyPage* pActivePage = GetActivePage();
   if (NULL != pActivePage)
      pActivePage->SendMessage(WM_KICKIDLE, 0, 0); 

	return CPropertySheet::ContinueModal();
}


void CTestSettings::SetUserEntries(CQuestionEx *pQuestion, int &iQuestionnaireIndex)
{
   // Question page
   CString csTitle;
   CString csQuestion;
   CString csScormId;
   UINT nPageStartMs; 
   UINT nPageEndMs;
   CTimePeriod visibilityPeriod;
   CTimePeriod activityPeriod;
   bool bShowDelete;
   bool bButtonsChanged;
   m_pTestQuestionPage->GetUserEntries(csTitle, csQuestion, iQuestionnaireIndex, 
                                       nPageStartMs, nPageEndMs, 
                                       visibilityPeriod, activityPeriod, bShowDelete, bButtonsChanged);
   if (bButtonsChanged)
      m_bButtonsChanged = true;
   
   CQuestionnaireEx *pQuestionnaire = NULL;
   if (iQuestionnaireIndex >= 0)
   { 
      HRESULT hr = S_OK;
      
      CArray<CQuestionnaireEx *, CQuestionnaireEx *> aQuestionnaires;
      CQuestionStream *pQuestionStream = m_pEditorProject->GetQuestionStream(false);
      if (pQuestionStream)
      {
         pQuestionStream->GetQuestionnaires(&aQuestionnaires);
         pQuestionnaire = aQuestionnaires[iQuestionnaireIndex];
      }
   }

   // Option page
   CArray<CFeedback *, CFeedback *> aSpecificFeedback;
   CMouseAction correctAction;
   CMouseAction failureAction;
   m_pTestOptionPage->GetUserEntries(pQuestionnaire, visibilityPeriod, activityPeriod, 
                                     &correctAction, &failureAction, &aSpecificFeedback);

   // Answer page
   UINT nAchievablePoints = 0;
   UINT nMaximumTries = 0;
   UINT nAvailableTimeMs = 0;
   m_pTestAnswerPage->GetUserEntries(nAchievablePoints, nMaximumTries, nAvailableTimeMs, csScormId);

   // Initialize Question
   CInteractionStream *pInteractionStream = m_pEditorProject->GetInteractionStream(true);

   CTimePeriod periodPage;
   periodPage.Init(INTERACTION_PERIOD_PAGE, nPageStartMs, nPageEndMs);
   HRESULT hr = pQuestion->Init(m_questionType, csTitle, csQuestion, csScormId,
      &periodPage, &visibilityPeriod, &activityPeriod, &correctAction, &failureAction, 
      nAchievablePoints, nMaximumTries, nAvailableTimeMs, m_pEditorProject, &aSpecificFeedback);
  

   if (m_questionType == QUESTION_TYPE_DRAGNDROP)
   {
      DndAlignmentId dndAlignment = 
         ((CTestDragAndDropPage *)m_pTestAnswerPage)->GetAlignment();
      pQuestion->SetDndAlignment(dndAlignment);
   }
   
   CRect rcSubmitButton(0, 0, 0, 0);
   CRect rcDeleteButton(0, 0, 0, 0);

   pQuestionnaire->CalculateDefaultButtonPosition(m_pEditorProject, rcSubmitButton, rcDeleteButton); 

   if (SUCCEEDED(hr))
      CreateSubmitButton(pQuestionnaire, pQuestion, visibilityPeriod, activityPeriod, rcSubmitButton);

   if (bShowDelete && SUCCEEDED(hr))
      CreateDeleteButton(pQuestionnaire, pQuestion, visibilityPeriod, activityPeriod, rcDeleteButton);
   

   if (SUCCEEDED(hr))
      CreateTitleText(pQuestion, csTitle, csQuestion, visibilityPeriod);
   if (SUCCEEDED(hr))
      CreateQuestionStateText(pQuestionnaire, pQuestion, visibilityPeriod);

   // insert question objects (like d&d images) last: they should be on top 
   if (SUCCEEDED(hr))
      m_pTestAnswerPage->FillWithObjects(pQuestion, visibilityPeriod, activityPeriod, m_dUpperBound);
 

   //if (SUCCEEDED(hr))
   //   pQuestion->MoveFeedbackToFront();

   _ASSERT(SUCCEEDED(hr));
}  

void CTestSettings::CreateDeleteButton(CQuestionnaireEx *pQuestionnaire, CQuestionEx *pQuestion, 
                                       CTimePeriod &visibilityPeriod, CTimePeriod &activityPeriod,
                                       CRect &rcDeleteButton)
{

   CString csDelete;
   if (pQuestionnaire)
      csDelete = pQuestionnaire->GetDeleteText();
   else
      csDelete.LoadString(IDC_BUTTON_DELETE);

   CRect rcDelete = rcDeleteButton;

   if (!m_bButtonsChanged && m_pDeleteButton != NULL)
   {
      rcDelete = m_pDeleteButton->GetArea();
   }

   CMouseAction mouseAction;
   mouseAction.Init(m_pEditorProject, INTERACTION_RESET_QUESTION);
   
   CInteractionAreaEx *pDeleteButton = NULL;
   //if (m_pDeleteButton)
   //   pDeleteButton = m_pDeleteButton->Copy(true);
   //else
      pDeleteButton = CQuestionnaireEx::CreateQuestionnaireButton(m_pEditorProject, pQuestionnaire, BUTTON_TYPE_DELETE,
                                                  &visibilityPeriod, &activityPeriod, mouseAction,
                                                  rcDelete, csDelete);

   if (m_pDeleteButton)
      pDeleteButton->SetHashKey(m_pDeleteButton->GetHashKey());

   pQuestion->AddObject(pDeleteButton);
}

void CTestSettings::CreateSubmitButton(CQuestionnaireEx *pQuestionnaire, CQuestionEx *pQuestion, 
                                       CTimePeriod &visibilityPeriod, CTimePeriod &activityPeriod,
                                       CRect &rcSubmitButton)
{
   // Submit button

   CString csSubmit;
   if (pQuestionnaire)
      csSubmit = pQuestionnaire->GetSubmitText();
   else
      csSubmit.LoadString(IDS_BUTTON_SUBMIT);

   CRect rcSubmit = rcSubmitButton;
  
   if (!m_bButtonsChanged && m_pSubmitButton != NULL)
   { 
      rcSubmit = m_pSubmitButton->GetArea();
   }
   
   CMouseAction mouseAction;
   mouseAction.Init(m_pEditorProject, INTERACTION_CHECK_QUESTION);
   
   CInteractionAreaEx *pSubmitButton = NULL;
   //if (m_pSubmitButton)
   //   pSubmitButton = m_pSubmitButton->Copy(true);
   //else
      pSubmitButton = CQuestionnaireEx::CreateQuestionnaireButton(m_pEditorProject, pQuestionnaire, BUTTON_TYPE_SUBMIT,
                                                  &visibilityPeriod, &activityPeriod, mouseAction, 
                                                  rcSubmit, csSubmit);

   if (m_pSubmitButton)
      pSubmitButton->SetHashKey(m_pSubmitButton->GetHashKey());

   rcSubmitButton = rcSubmit;
   pQuestion->AddObject(pSubmitButton);
}

void CTestSettings::CreateTitleText(CQuestionEx *pQuestion, CString &csTitle, CString &csQuestion, CTimePeriod &visibilityPeriod)
{
   // Add Title
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;

   LOGFONT lf;
   LFont::FillWithDefaultValues(&lf);
   lf.lfHeight = -19;
   lf.lfWeight = FW_BOLD;

   double dTextWidth = DrawSdk::Text::GetTextWidth(csTitle, csTitle.GetLength(), &lf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csTitle, csTitle.GetLength(), &lf);
   double dTextAscent = DrawSdk::Text::GetTextAscent(&lf);

   double dScaleX, dScaleY;
   CQuestionEx::GetQuestionScale(m_pEditorProject, dScaleX, dScaleY);

   double dXPos = (double)CQuestionnaireEx::LEFT_BORDER * dScaleX;
   double dYPos = (double)CQuestionnaireEx::TOP_BORDER * dScaleY;
   
   dYPos += dTextAscent;

   COLORREF clrText = RGB(21, 21, 21);
   if (m_pTitleText)
   {
      dXPos = m_pTitleText->GetX();
      dYPos = m_pTitleText->GetY();
      clrText = ((DrawSdk::Text *)m_pTitleText)->GetOutlineColorRef();
      ((DrawSdk::Text *)m_pTitleText)->GetLogFont(&lf);
   }
   DrawSdk::Text *pTitleText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, 
                                                 clrText, csTitle, csTitle.GetLength(), &lf, DrawSdk::TEXT_TYPE_QUESTION_TITLE);
   

   m_dUpperBound = dYPos + dTextHeight;

   aObjects.Add(pTitleText);

   CMoveableObjects *pTitleObject = new CMoveableObjects();
   pTitleObject->Init(NULL, &visibilityPeriod, m_pEditorProject, &aObjects, MOVEABLE_TYPE_OTHER);
   if (m_bUseTitleHashKey)
      pTitleObject->SetHashKey(m_nTitleHashKey);
   pQuestion->AddObject(pTitleObject);

   if (pTitleText)
      delete pTitleText;
   aObjects.RemoveAll();


   // Add question
   lf.lfHeight = -16;
   lf.lfWeight = FW_NORMAL;
   dTextAscent = DrawSdk::Text::GetTextAscent(&lf);

   dXPos = (double)CQuestionnaireEx::LEFT_BORDER * dScaleX;;
   dYPos += (20.0 * dScaleY) + dTextAscent;
   clrText = RGB(0, 0, 0);
   if (m_pQuestionText)
   {
      dXPos = m_pQuestionText->GetX();
      dYPos = m_pQuestionText->GetY();
      clrText = ((DrawSdk::Text *)m_pQuestionText)->GetOutlineColorRef();
      ((DrawSdk::Text *)m_pQuestionText)->GetLogFont(&lf);
   }
   
   CString csLine;
   bool bInsertNewLine = false;
   CArray<DrawSdk::Text *, DrawSdk::Text *> aTexts;
   for (int i = 0; i < csQuestion.GetLength(); ++i)
   {
      if (bInsertNewLine)
      {
         dTextWidth = DrawSdk::Text::GetTextWidth(csLine, csLine.GetLength(), &lf);
         dTextHeight = DrawSdk::Text::GetTextHeight(csLine, csLine.GetLength(), &lf);
         DrawSdk::Text *pQuestionText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, clrText, 
            csLine, csLine.GetLength(), &lf, DrawSdk::TEXT_TYPE_QUESTION_TEXT);

         aTexts.Add(pQuestionText);
         
         m_dUpperBound = dYPos + dTextHeight;

         aObjects.Add(pQuestionText);

         dYPos += dTextHeight;
         csLine.Empty();
         bInsertNewLine = false;
      }

      if (csQuestion[i] == 13)
      {
         bInsertNewLine = true;
         ++i;
      }
      else
      {
         csLine += csQuestion[i];
      }
   }
   
   if (!csLine.IsEmpty())
   {
      dTextWidth = DrawSdk::Text::GetTextWidth(csLine, csLine.GetLength(), &lf);
      dTextHeight = DrawSdk::Text::GetTextHeight(csLine, csLine.GetLength(), &lf);
      DrawSdk::Text *pQuestionText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight, clrText, 
         csLine, csLine.GetLength(), &lf, DrawSdk::TEXT_TYPE_QUESTION_TEXT);
      
      aTexts.Add(pQuestionText);
      aObjects.Add(pQuestionText);
   }


   CMoveableObjects *pQuestionObject = new CMoveableObjects();
   pQuestionObject->Init(NULL, &visibilityPeriod, m_pEditorProject, &aObjects, MOVEABLE_TYPE_OTHER);   
   if (m_bUseQuestionHashKey)
      pQuestionObject->SetHashKey(m_nQuestionHashKey);
   pQuestion->AddObject(pQuestionObject);

   for (i = 0; i < aTexts.GetSize(); ++i)
   {
      if (aTexts[i] != NULL)
         delete aTexts[i];
   }
   aTexts.RemoveAll();

   aObjects.RemoveAll();
}

void CTestSettings::CreateQuestionStateText(CQuestionnaireEx *pQuestionnaire, CQuestionEx *pQuestion, CTimePeriod &visibilityPeriod)
{
   // add timelimit, progress and try text

   bool bUseOldPositions = true;
   if ((pQuestion->GetAvailableSeconds() > 0 && !m_pTimerText) ||
       (pQuestion->GetAvailableSeconds() <= 0 && m_pTimerText))
       bUseOldPositions = false;

   if ((pQuestion->GetMaximumTries() > 0 && !m_pTriesText) ||
       (pQuestion->GetMaximumTries() <= 0 && m_pTriesText))
       bUseOldPositions = false;
   
   if (((pQuestionnaire == NULL || pQuestionnaire->IsShowProgress()) && !m_pProgressText) ||
       ((pQuestionnaire != NULL && !pQuestionnaire->IsShowProgress()) && m_pProgressText))
       bUseOldPositions = false;
      
   
   LOGFONT lf;
   COLORREF clrText;
   double dYPos;
   CQuestionEx::CalculateQuestionStateBottomPosition(m_pEditorProject, dYPos);
   double dXPos = 0;

   ////// time counter
   
   if (pQuestion->GetAvailableSeconds() > 0)
   {
      // create text
      CQuestionEx::CalculateQuestionStateLeftPosition(m_pEditorProject, dXPos);
      
      CString csTimer;
      csTimer.LoadString(IDS_TIME_TEXT);
      
      CQuestionEx::GetQuestionStateDefaultColor(clrText);
      CQuestionEx::GetQuestionStateDefaultFont(lf);

      if (m_pTimerText)
      {
         csTimer = m_pTimerText->GetString();
         clrText = ((DrawSdk::Text *)m_pTimerText)->GetOutlineColorRef();
         ((DrawSdk::Text *)m_pTimerText)->GetLogFont(&lf);
         if (bUseOldPositions)
         {
            dXPos = m_pTimerText->GetX();
            dYPos = m_pTimerText->GetY();
            double dTextHeight = DrawSdk::Text::GetTextHeight(csTimer, csTimer.GetLength(), &lf);
            dYPos += 1.1 * dTextHeight;
         }
      }

      pQuestion->AddTimerText(m_pEditorProject, visibilityPeriod,
                              dXPos, dYPos, 
                              csTimer, clrText, lf, 
                              m_bUseTimerTextHashKey, m_nTimerTextHashKey);
     
      
      // create timer
      CQuestionEx::GetQuestionStateDefaultColor(clrText);
      CQuestionEx::GetQuestionStateDefaultFont(lf);
      if (m_pTimerObject)
      {
         if (bUseOldPositions)
         {
            dXPos = m_pTimerObject->GetX();
            dYPos = m_pTimerObject->GetY();
         }
         clrText = ((DrawSdk::Text *)m_pTimerObject)->GetOutlineColorRef();
         ((DrawSdk::Text *)m_pTimerObject)->GetLogFont(&lf);
      }
      pQuestion->AddTimerObject(m_pEditorProject, visibilityPeriod,
                                dXPos, dYPos, 
                                clrText, lf, 
                                m_bUseTimerObjectHashKey, m_nTimerObjectHashKey);
   }
   
      ////// try text
   if (pQuestion->GetMaximumTries() > 0)
   {
      // create text
      CQuestionEx::CalculateQuestionStateLeftPosition(m_pEditorProject, dXPos);

      CString csTries;
      csTries.LoadString(IDS_TRY_TEXT);

      CQuestionEx::GetQuestionStateDefaultColor(clrText);
      CQuestionEx::GetQuestionStateDefaultFont(lf);

      if (m_pTriesText)
      {
         csTries = m_pTriesText->GetString();
         clrText = ((DrawSdk::Text *)m_pTriesText)->GetOutlineColorRef();
         ((DrawSdk::Text *)m_pTriesText)->GetLogFont(&lf);
         if (bUseOldPositions)
         {
            dXPos = m_pTriesText->GetX();
            dYPos = m_pTriesText->GetY();
            double dTextHeight = DrawSdk::Text::GetTextHeight(csTries, csTries.GetLength(), &lf);
            dYPos += 1.1 * dTextHeight;
         }
      }

      pQuestion->AddTriesText(m_pEditorProject, visibilityPeriod, 
                              dXPos, dYPos, 
                              csTries, clrText, lf, 
                              m_bUseTriesTextHashKey, m_nTriesTextHashKey);

      
      // create try counter
      CQuestionEx::GetQuestionStateDefaultColor(clrText);
      CQuestionEx::GetQuestionStateDefaultFont(lf);

      if (m_pTriesObject)
      {
         if (bUseOldPositions)
         {
            dXPos = m_pTriesObject->GetX();
            dYPos = m_pTriesObject->GetY();
         }
         clrText = ((DrawSdk::Text *)m_pTriesObject)->GetOutlineColorRef();
         ((DrawSdk::Text *)m_pTriesObject)->GetLogFont(&lf);
      }

      pQuestion->AddTriesObject(m_pEditorProject, visibilityPeriod, dXPos, dYPos,
                                clrText, lf, m_bUseTriesObjectHashKey, m_nTriesObjectHashKey);
     
   }
   
   ////// progress text
   if (pQuestionnaire == NULL || pQuestionnaire->IsShowProgress())
   {
      CQuestionEx::CalculateQuestionStateLeftPosition(m_pEditorProject, dXPos);

      CString csProgress;
      csProgress.LoadString(IDS_PROGRESS_TEXT);
      
      CQuestionEx::GetQuestionStateDefaultColor(clrText);
      CQuestionEx::GetQuestionStateDefaultFont(lf);

      if (m_pProgressText)
      {
         csProgress = m_pProgressText->GetString();
         clrText = ((DrawSdk::Text *)m_pProgressText)->GetOutlineColorRef();
         ((DrawSdk::Text *)m_pProgressText)->GetLogFont(&lf);
         if (bUseOldPositions)
         {
            dXPos = m_pProgressText->GetX();
            dYPos = m_pProgressText->GetY();
            double dTextHeight = DrawSdk::Text::GetTextHeight(csProgress, csProgress.GetLength(), &lf);
            dYPos += 1.1 * dTextHeight;
         }
      }
      // create text
      
      pQuestion->AddProgressText(m_pEditorProject, visibilityPeriod,
                                 dXPos, dYPos, 
                                 csProgress, clrText, lf, 
                                 false, m_bUseProgressTextHashKey, m_nProgressTextHashKey);

     

      // create changeable object for question counter
      CQuestionEx::GetQuestionStateDefaultColor(clrText);
      CQuestionEx::GetQuestionStateDefaultFont(lf);

      if (m_pProgressObject)
      {
         if (bUseOldPositions)
         {
            dXPos = m_pProgressObject->GetX();
            dYPos = m_pProgressObject->GetY();
         }
         clrText = ((DrawSdk::Text *)m_pProgressText)->GetOutlineColorRef();
         ((DrawSdk::Text *)m_pProgressObject)->GetLogFont(&lf);
      }

      pQuestion->AddProgressObject(m_pEditorProject, visibilityPeriod, 
                                   dXPos, dYPos, 
                                   clrText, lf,
                                   false, m_bUseProgressObjectHashKey, m_nProgressObjectHashKey);
      
   }


}

CString CTestSettings::GetButtonText(CInteractionAreaEx *pButton)
{
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paNormalObjects;

   paNormalObjects = pButton->GetNormalObjects();


   for (int i = 0; i < paNormalObjects->GetSize(); ++i)
   {
      DrawSdk::DrawObject *pObject = paNormalObjects->GetAt(i);
      if (pObject->GetType() == DrawSdk::TEXT)
      {
         return ((DrawSdk::Text *)pObject)->GetString();
      }
   }
   return _T("");
}

void CTestSettings::OnShowQuestionaireSettings() 
{
    if (m_pTestQuestionPage->ShowQuestionnaireSettings(m_bButtonsChanged, m_pTestQuestionPage->GetEditNewQuestion()) == IDOK ) {
        SetQuestionnaireSettingsShown();
        m_pTestOptionPage->RefreshActionsControls();
    }
}

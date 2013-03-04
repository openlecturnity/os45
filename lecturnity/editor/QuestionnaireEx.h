#if !defined(AFX_QUESTIONNAIREEX_H__B426B413_1A7B_4009_9CA0_7DC43E63418E__INCLUDED_)
#define AFX_QUESTIONNAIREEX_H__B426B413_1A7B_4009_9CA0_7DC43E63418E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "objects.h"
#include "Questionnaire.h"
#include "Feedback.h"
#include "FileOutput.h"
#include "InteractionAreaEx.h"
#include "WhiteboardView.h"
class CEditorDoc;
class CInteractionStream;
class CQuestionEx;


class CQuestionnaireEx : public CObject
{
   DECLARE_DYNCREATE(CQuestionnaireEx)

public:
	CQuestionnaireEx();
	virtual ~CQuestionnaireEx();

public:
   
   enum {
      LEFT_BORDER = 40,
      TOP_BORDER = 100,
      BUTTON_WIDTH = 90,
      BUTTON_HEIGHT = 20,
      BUTTON_OFFSET = 20,
      BOTTOM_BORDER = 30
   };

public:
   // Handling of questions should be done elsewhere so
   // setting bIgnoreQuestions to false will return an error.
   // You need nInsertionMs for resolving/adjusting jump times.
   HRESULT Init(CQuestionnaire *pQuestionnaire, 
      bool bIgnoreQuestions, CEditorProject *pProject, UINT nInsertionMs);
   HRESULT Init(CString& csTitle, bool bShowEvaluation, bool bShowProgress, bool bDoStopmarks, bool bIsRandomTest, UINT iNumberOfRandomQuestions,
      CMouseAction *pCorrectAction, CMouseAction *pFailureAction,
      bool bPassTypeRelative, UINT nNeededPoints, CEditorProject *pProject,
      CString& csOkText, CString& csSubmitText, CString& csDeleteText,
      CArray<CFeedback *, CFeedback *> *paGeneralFeedback);
   
   HRESULT ResolveJumpTimes(CEditorProject *pProject, UINT nMoveMs);

   //HRESULT MoveFeedbackToFront();

   HRESULT CommitFeedback();
   HRESULT RemoveFeedback();

   CQuestionnaireEx* Copy();

   HRESULT UpdateButtonAppearance();

   CString& GetTitle() { return m_csTitle; }
   bool IsPassTypeRelative() { return m_bPassTypeRelative; }
   UINT GetNeededPoints() { return m_nNeededPoints; }
   CFeedback *GetFeedback(FeedbackTypeId idType);
   CMouseAction *GetCorrectAction() { return m_pCorrectAction; }
   CMouseAction *GetFailureAction() { return m_pFailureAction; }
   bool IsShowEvaluation() { return m_bShowEvaluation; }
   bool IsShowProgress() { return m_bShowProgress; }
   bool IsEvaluationDefined();
   bool IsDoStopmarks() { return m_bDoStopmarks; }
   int GetQuestionNumber(CQuestionEx *pQuestion);
   UINT GetQuestionTotal() { return m_aQuestionPointers.GetSize(); }

   HRESULT RegisterQuestion(CQuestionEx *pQuestion);
   HRESULT UnregisterQuestion(CQuestionEx *pQuestion, CQuestionEx *pQuestionNew = NULL);
   /** This preserves the question ordering. */
   HRESULT ExchangeQuestion(CQuestionEx *pQuestionOld, CQuestionEx *pQuestionNew);
   bool HasQuestions();
   HRESULT TransferQuestionPointers(CQuestionnaireEx *pTarget); // used by CQuestionStream::ChangeQuestionnaire()
   HRESULT ClaimQuestionPointers();
   //bool HasValidQuestionPointers() { return m_bQuestionPointersValid; }
   //HRESULT SetValidQuestionPointers(bool bValid) { m_bQuestionPointersValid = bValid; }
   
   HRESULT ReplaceFeedback(CFeedback *pNewFeedback, bool bSetRollback = false);

   HRESULT Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
      LPCTSTR tszExportPrefix, LPCTSTR tszLepFilename, CEditorProject *pProject);

   bool AnyQuestionsLeft();
   bool CheckPassed(UINT *pnMax = NULL, UINT *pnGet = NULL, UINT *pnPass = NULL);
   HRESULT ShowEvaluation(CWhiteboardView *pView);
   HRESULT Execute(CEditorDoc *pDoc, bool bQuestionnairePassed);
   HRESULT ResetAfterPreview();

   void SetButtonColor(COLORREF *pClrFill, COLORREF *pClrLine, COLORREF *pClrText);
   void SetButtonFilenames(CString *csFilename);
   void SetButtonFont(LOGFONT *pLogFont);
   void GetNormalButtonValues(COLORREF &clrFill, COLORREF &clrLine, COLORREF &clrText, LOGFONT &lf, CString &csFilename);
   void GetMouseoverButtonValues(COLORREF &clrFill, COLORREF &clrLine, COLORREF &clrText, LOGFONT &lf, CString &csFilename);
   void GetPressedButtonValues(COLORREF &clrFill, COLORREF &clrLine, COLORREF &clrText, LOGFONT &lf, CString &csFilename);
   void GetInactiveButtonValues(COLORREF &clrFill, COLORREF &clrLine, COLORREF &clrText, LOGFONT &lf, CString &csFilename);

   CString& GetOkText()     { return m_csOkText; }
   void SetOkText(CString &csOkText) {m_csOkText = csOkText;}
   CString& GetSubmitText() { return m_csSubmitText; }
   CString& GetDeleteText() { return m_csDeleteText; }
   bool ButtonsAreImageButtons() {return m_bHasImageButtons;}
   void SetButtonsAreImageButtons(bool bHasImageButtons) {m_bHasImageButtons = bHasImageButtons;}
   bool IsButtonConfigurationDifferent(CQuestionnaireEx *pOther);
 
   HRESULT ChangeQuestionsShowProgress(bool bShowProgress);

   // create Questionnaire with default values
   static CQuestionnaireEx *CreateDefaultQuestionnaire(CEditorProject *pProject);
   static CFeedback *CreateResultFeedback(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire); //CArray<CQuestionEx *, CQuestionEx *> *paQuestionPointers);

   // calculate default values for "Submit" and "Delete" button
   void CalculateDefaultButtonPosition(CEditorProject *pProject, CRect &rcSubmit, CRect &rcDelete);
   HRESULT CalculateButtonSize(ButtonTypeId idButtonType, CSize &siButton, TCHAR *szButton=NULL);


   // Members for randomized questionnnaire
   bool IsRandomTest();
   // Returns the random question number
   UINT GetNumberOfRandomQuestions() { return m_iNumberOfRandomQuestions; }
   // Sets the random question number
   void SetNumberOfRandomQuestions(UINT nNumberOfRandomQuestions);
   // Returns the index within the random array of the specified question. 
   // This is relevant during play, as the number of the question shown.
   int GetRandomQuestionIndex(CQuestionEx *pQuestion);
   // Randomize questions
   bool RandomizeQuestionnaire();
   // Changes the mouse action of this questionaire for randomize play.
   void ConvertAction(CMouseAction *pAction);
   // Unregisters unpeeked questions during randomization from questionnaire, so that feedback will not consider these questions.
   // If there is a question on the first page, will insert a stopmark at begining of document. When this stopmark is reached a jump is made on the first random questions.
   void PrepareDocumentForRandomizePlay();
   // Reregisters questions, and removes the stopmark created in PrepareDocumentForRandomizePlay.
   void RestoreDocumentFromRandomizePlay();
   // Returns the question from the iIndex position within m_aRandomizedQuestionPointers array.
   CQuestionEx *GetRandomQuestionFromPosition(int iIndex);

   int GetRandomQuestionReplayIndex();
   void SetRandomQuestionReplayIndex(int iRandomQuestionReplayIndex);

private:
   HRESULT CloneTo(CQuestionnaireEx *pTarget);

   HRESULT SetDefaultFontAndColor();
   HRESULT GetAllFeedback(CArray<CFeedback *, CFeedback *> *paFeedback);
   HRESULT WriteDefaultButtonConfig(CFileOutput *pFileOutput, LPCTSTR tszLepFilename);

   CString m_csTitle;
   CMouseAction *m_pCorrectAction;
   CMouseAction *m_pFailureAction;
   bool m_bShowEvaluation;
   bool m_bShowProgress;

   bool m_bDoStopmarks;
   bool m_bPassTypeRelative;
   // can be a percentage value (bPassTypeRelative == true) or an absolute value
   UINT m_nNeededPoints;
   CMap<FeedbackTypeId, FeedbackTypeId, CFeedback *, CFeedback *> m_mapFeedbackNotVisible;
   CArray<CQuestionEx *, CQuestionEx *> m_aQuestionPointers;
   bool m_bQuestionPointersValid;
   CEditorProject *m_pEditorProject;
   CArray<CFeedback *, CFeedback *> m_aFeedbackToCommit;

   
   CString m_csFilename[4];
   COLORREF m_clrFill[4];
   COLORREF m_clrLine[4];
   COLORREF m_clrText[4];
   LOGFONT  m_lf[4];
   
   CString m_csOkText;
   CString m_csSubmitText;
   CString m_csDeleteText;
   bool m_bHasImageButtons;
   
   bool m_bJumpsResolved;

   /* Variables that support randomized Questionnaire */
   struct QuestionMouseActions {
      QuestionMouseActions (CQuestionEx *pBelongsTo, CMouseAction *pCorrectAction, CMouseAction *pFailureAction);
      ~QuestionMouseActions();

      CQuestionEx *m_pBelongsTo;
      CMouseAction *m_pCorrectAction;
      CMouseAction *m_pFailureAction;
   };
   // Holds a backup of correct/failure mouse actions for each question. This is required as on "random test preview" these values are changed
   CArray<QuestionMouseActions*, QuestionMouseActions*> m_aQuestionMouseActions;
   // Holds a backup of correct mouse action of this questionnaire
   CMouseAction *m_pCorrectActionBackup;
   // Holds a backup of failure mouse action of this questionnaire
   CMouseAction *m_pFailureActionBackup;

   // Specifies if this test is a random test.
   bool m_bIsRandomTest;
   // The number of random questions. Must be less than current questions number (value returned by GetQuestionTotal function).
   UINT m_iNumberOfRandomQuestions;
   // This array contains pointers of the questions resulted from randomization. These are selected from m_aQuestionPointers.
   CArray<CQuestionEx *, CQuestionEx *> m_aRandomizedQuestionPointers;
   // This array contains pointers of the questions that were not peeeked during randomization. These are selected from m_aQuestionPointers but not in m_aRandomizedQuestionPointers.
   CArray<CQuestionEx *, CQuestionEx *> m_aUnPeekedRandomizedQuestions;
   // Holds the index of the current random question played.
   int m_iRandomQuestionReplayIndex;
public:
   static enum 
   {
      PASSIV_BUTTON_BG = RGB(171, 199, 238),
      PASSIV_BUTTON_LINE = RGB(163, 193, 245),
      PASSIV_BUTTON_TEXT = RGB(159, 174, 204),
      NORMAL_BUTTON_BG = RGB(171, 199, 238),
      NORMAL_BUTTON_LINE = RGB(163, 193, 245),
      NORMAL_BUTTON_TEXT = RGB(0, 0, 0),
      MOUSEO_BUTTON_BG = RGB(255, 228, 176),
      MOUSEO_BUTTON_LINE = RGB(0, 0, 128),
      MOUSEO_BUTTON_TEXT = RGB(0, 0, 0),
      PRESSED_BUTTON_BG = RGB(255, 175, 107),
      PRESSED_BUTTON_LINE = RGB(0, 0, 128),
      PRESSED_BUTTON_TEXT = RGB(0, 0, 0)
   };

   static CFeedback *CreateFeedback(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire, 
                                    FeedbackTypeId feedbackType, 
                                    CString csFeedback, bool bDisabled, CTimePeriod &visualityPeriod);

   static CInteractionAreaEx *CreateQuestionnaireButton(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire, 
                                                        ButtonTypeId idButtonType,
                                                        CTimePeriod *pVisualityPeriod, CTimePeriod *pActivityPeriod, 
                                                        CMouseAction &clickAction,
                                                        CRect &rcButton, CString &csButton);

};

#endif // !defined(AFX_QUESTIONNAIREEX_H__B426B413_1A7B_4009_9CA0_7DC43E63418E__INCLUDED_)

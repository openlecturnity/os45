#if !defined(AFX_QUESTIONEX_H__C4990414_4196_4457_AC81_73509F451ED0__INCLUDED_)
#define AFX_QUESTIONEX_H__C4990414_4196_4457_AC81_73509F451ED0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feedback.h"
#include "InteractionAreaEx.h"
class CInteractionStream;
class CWhiteboardView;


enum QuestionTypeId
{
   QUESTION_TYPE_NOTHING,
      QUESTION_TYPE_CHOICE,
      QUESTION_TYPE_TEXTGAP,
      QUESTION_TYPE_DRAGNDROP
};

enum DndAlignmentId
{
   QUESTION_DND_ALIGNMENT_UNDEFINED,
      QUESTION_DND_ALIGNMENT_HORIZONTAL,
      QUESTION_DND_ALIGNMENT_VERTICAL,
};

/*
 * Questions class. Can be any of Single/Multiple choice, Drag & Drop or Fill in the blank.
 * Each Question instance belongs to a CQuestionnaireEx.
*/
class CQuestionEx : public CObject
{
   DECLARE_DYNCREATE(CQuestionEx)

public:
	CQuestionEx();
	virtual ~CQuestionEx();

   /**
    * A question belongs to a page. These page times have to be specified here.
    * The question objects themself can have different (subset) times.
    */
   HRESULT Init(QuestionTypeId idType, CString& csTitle, CString& csQuestion, CString& csScormId,
      CTimePeriod *pPageVisibility, CTimePeriod *pObjectVisibility, CTimePeriod *pObjectActivity,
      CMouseAction *pCorrectAction, CMouseAction *pFailureAction,
      UINT nAchievablePoints, UINT nMaximumTries, UINT nAvailableTimeMs,
      CEditorProject *pProject, //CArray<CInteractionAreaEx *, CInteractionAreaEx *> *pObjects,
      CArray<CFeedback *, CFeedback *> *paSpecificFeedback);
   HRESULT Init(CQuestion *pQuestion, UINT nStreamToMs, CQuestionnaireEx *pBelongsTo, CEditorProject *pProject);

   CQuestionEx* Copy(bool bIncludeJoinId = true);

   HRESULT ResolveJumpTimes(CEditorProject *pProject, UINT nMoveMs);

   HRESULT UpdateButtonAppearance(CQuestionnaireEx *pQuestionnaire, CEditorProject *pProject);
   HRESULT ChangeQuestionsShowProgress(CEditorProject *pProject, bool bShowProgress);

   QuestionTypeId GetType() { return m_idType; }
   CString& GetTitle() { return m_csTitle; }
   CString& GetQuestion() { return m_csQuestion; }
   CString& GetScormId() { return m_csScormId; }

   UINT GetJoinId() { return m_nJoinId; }
   void SetJoinId(UINT nJoinId) { m_nJoinId = nJoinId; }

   /** Only call for changing during id conflict resolving (in CQuestionStream). */
   void SetScormId(CString& csId) { m_csScormId = csId; }
   UINT GetAchievablePoints() { return m_nAchievablePoints; }
   UINT GetMaximumTries() { return m_nMaximumTries; }
   UINT GetAvailableSeconds() { return m_nAvailableSeconds; }

   HRESULT IncrementTimes(UINT nDiffMs);
   HRESULT DecrementTimes(UINT nDiffMs);
   HRESULT TrimToRange(UINT nFromMs, UINT nLengthMs);
   HRESULT RemovePart(UINT nFromMs, UINT nLengthMs);
   bool MeetsWith(CQuestionEx *pOther, UINT nMoveMs);
   HRESULT Enlarge(CQuestionEx *pOther, UINT nMoveMs);
   HRESULT Enlarge(UINT nOtherStartMs, UINT nOtherLengthMs);
   CQuestionEx *Split(UINT nSplitMs);
   
   CTimePeriod *GetObjectVisibility() { return &m_ObjectVisibility; }
   CTimePeriod *GetObjectActivity() { return &m_ObjectActivity; }
   CTimePeriod *GetPageVisibility() { return &m_PageVisibility; }
   UINT GetPageBegin() { return m_PageVisibility.GetBegin(); }
   UINT GetPageLength() { return m_PageVisibility.GetLength(); }
   UINT GetObjectActivityEnd() { return m_ObjectActivity.GetEnd(); }
   bool PageContains(UINT nPositionMs);
   bool Intersects(CQuestionEx *pOther);
   bool BeginsAtOrAfter(UINT nPositionMs);

   /** Populates given array with objects page objects pointers except questionnaire feedback */
   HRESULT GetObjectPointers(CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paObjects);
   
   HRESULT SetBelongsTo(CQuestionnaireEx *pBelongsTo);
   CQuestionnaireEx *GetBelongsTo() { return m_pBelongsTo; }
   CFeedback *GetFeedback(FeedbackTypeId idType, bool bGetSuper = false);
   CMouseAction *GetCorrectAction() { return m_pCorrectAction; }
   void SetCorrectAction(CMouseAction *pCorrectAction) {m_pCorrectAction = pCorrectAction; }
   CMouseAction *GetFailureAction() { return m_pFailureAction; }
   void SetFailureAction(CMouseAction *pFailureAction) { m_pFailureAction = pFailureAction; }

   DndAlignmentId GetDndAlignment() { return m_idDndAlignment; }
   void SetDndAlignment(DndAlignmentId idAlign) { m_idDndAlignment = idAlign; }
   /*
    * If question is of type QUESTION_TYPE_CHOICE, return true if is multiple choice or false if it is single choice.
    * If question is not of type QUESTION_TYPE_CHOICE, return false.
   */
   bool IsMultipleChoice();

   HRESULT AddObject(CInteractionAreaEx *pObject);
   HRESULT CommitObjects();
   HRESULT RemoveObjects();
     
   //HRESULT MoveFeedbackToFront();

   HRESULT Write(CFileOutput *pOutput, UINT nIdentLength, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, LPCTSTR tszExportPrefix, CEditorProject *pProject);


   bool AreAttemptsLeft();
   bool AnsweredOnce() { return m_nTakenAttempts > 0; }
   bool IsEnabled() { return !m_bDisabled; }
   bool IsAnsweredCorrect() { return m_bAnswerCorrect; }
   HRESULT UpdateAnswered(bool bQuestionSuccess);
   HRESULT ShowFeedback(FeedbackTypeId idFeedback, CWhiteboardView *pView, bool bWaitForFinish = true);
   bool IsFeedbackActive(FeedbackTypeId idFeedback);
   HRESULT Disable();
   HRESULT Execute(CEditorDoc *pDoc, bool bQuestionSuccess);
   HRESULT ResetAfterPreview();
   bool IsTimeUp();
   bool IsTimeable() { return m_nAvailableSeconds > 0; }
   bool IsTimerMessageShown() { return m_bTimerMessageShown; }
   /** Returns the remaining time in seconds or 0 when time's up or not defined. */
   int UpdateTimer();
   /** Returns the remaining time in seconds or 0 when time's up or not defined. */
   int StopTimer();
   /** Returns true if the Question's Delete button is visible on page. Flase otherwise. */
   bool HasDeleteButtonEnabled();
   HRESULT UpdateChangeableTexts(int iRemainingSeconds);

   HRESULT AddTimerText(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                        double &dXPos, double &dYPos, 
                        CString &csTimer, COLORREF clrText, LOGFONT &lf, 
                        bool bUseHashKey=false, UINT nHashKey=0);
   HRESULT AddTimerObject(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                          double &dXPos, double &dYPos, 
                          COLORREF clrText, LOGFONT &lf, 
                          bool bUseHashKey=false, UINT nHashKey=0);
   HRESULT AddTriesText(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                        double &dXPos, double &dYPos, 
                        CString &csTries, COLORREF clrText, LOGFONT &lf, 
                        bool bUseHashKey=false, UINT nHashKey=0);
   HRESULT AddTriesObject(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                          double &dXPos, double &dYPos, 
                          COLORREF clrText, LOGFONT &lf, 
                          bool bUseHashKey=false, UINT nHashKey=0);
   HRESULT AddProgressText(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                        double &dXPos, double &dYPos, 
                        CString &csProgress, COLORREF clrText, LOGFONT &lf, 
                        bool bInsertInStream, bool bUseHashKey=false, UINT nHashKey=0);
   HRESULT AddProgressObject(CEditorProject *pProject, CTimePeriod &visibilityPeriod, 
                          double &dXPos, double &dYPos, 
                          COLORREF clrText, LOGFONT &lf, 
                          bool bInsertInStream, bool bUseHashKey=false, UINT nHashKey=0);
   
   static HRESULT GetQuestionScale(CEditorProject *pProject, double &dScaleX, double &dScaleY);
   static HRESULT GetStateTextHeight(CEditorProject *pProject, double &dTextHeight);
   static HRESULT CalculateQuestionStateBottomPosition(CEditorProject *pProject, double &dYPos);
   static HRESULT CalculateQuestionStateLeftPosition(CEditorProject *pProject, double &dXPos);
   static HRESULT GetQuestionStateDefaultFont(LOGFONT &lf);
   static HRESULT GetQuestionStateDefaultColor(COLORREF &clrText) {clrText = RGB(63, 63, 63); return S_OK;}

private:
   HRESULT CloneTo(CQuestionEx *pOther, bool bIncludeJoinId);

   HRESULT GetAllFeedback(CArray<CFeedback *, CFeedback *> *paFeedback);
   
   HRESULT GetQuestionStateInteractions(CInteractionAreaEx *&pTimerText, CInteractionAreaEx *&pTimerValue,
                                        CInteractionAreaEx *&pPossibleTriesText, CInteractionAreaEx *&pPossibleTriesValue,
                                        CInteractionAreaEx *&pProgressText, CInteractionAreaEx *&pProgressValue);

   QuestionTypeId m_idType;
   CString m_csTitle;
   CString m_csQuestion;
   CString m_csScormId;
   CTimePeriod m_ObjectVisibility;
   CTimePeriod m_ObjectActivity;
   CTimePeriod m_PageVisibility;
   CMouseAction *m_pCorrectAction;
   CMouseAction *m_pFailureAction;
   bool m_bJumpsResolved;
   UINT m_nAchievablePoints; // 0 means undefined
   UINT m_nMaximumTries;     // 0 means undefined
   UINT m_nAvailableSeconds; // 0 means undefined

   /*
    * Pointer to project's interaction stream
    */
   CInteractionStream *m_pInteractionStream;
   /*
   * Graphical objects that will be inserted into current question. (Strings, buttons (Delete, Submit), Images)
   */
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> m_aObjectsToCommit;
   /*
    * Pointer to the Questionnaire that belongs to
   */
   CQuestionnaireEx *m_pBelongsTo;
   UINT m_nJoinId;

   DndAlignmentId m_idDndAlignment;

   UINT m_nTakenAttempts;
   bool m_bAnswerCorrect;
   bool m_bDisabled;
   bool m_bTimerMessageShown;
   UINT m_nUsedSeconds;
   UINT m_nStartSeconds;
};

#endif // !defined(AFX_QUESTIONEX_H__C4990414_4196_4457_AC81_73509F451ED0__INCLUDED_)

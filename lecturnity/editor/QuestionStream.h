#if !defined(AFX_QUESTIONSTREAM_H__50FBC8EF_3012_4A30_AA04_0DE3762C2A69__INCLUDED_)
#define AFX_QUESTIONSTREAM_H__50FBC8EF_3012_4A30_AA04_0DE3762C2A69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseStream.h"
#include "QuestionEx.h"
#include "QuestionnaireEx.h"

#define E_QS_CONTAINS_QUESTIONS _HRESULT_TYPEDEF_(0x80d50001L)

// for separating CQuestionEx from CQuestionnaireEx during undo
#define IUNDO_OBJECT_QUESTION 1
#define IUNDO_OBJECT_QUESTIONNAIRE 2

#define IUNDO_ACTION_TIMES_CHANGE 1

#define ID_VALID_CHECK 0x7399adcc

#define MESSAGE_ID_DO_ACTIONS 29384

enum QuestionOperationId
{
   QUESTION_OPERATION_CHECK,
      QUESTION_OPERATION_RESET
};

class CQuestionStream : public CBaseStream  
{
public:
	CQuestionStream();
	virtual ~CQuestionStream();

   HRESULT Insert(UINT nToMs, UINT nLength, CWhiteboardStream *pWhiteboard);
   HRESULT InsertDenver(UINT nToMs, UINT nLengthMs);

   HRESULT ResolveJumpTimes(UINT nMoveMs);

   HRESULT InsertSingle(CQuestionEx *pQuestion, CQuestionnaireEx *pBelongsTo, 
      bool bUserInteraction = true, bool bDoCollisionCheck = true);
   HRESULT DeleteSingle(CQuestionEx *pQuestion, bool bRemoveObjects, bool bDoRollback);
   HRESULT ChangeQuestion(CQuestionEx *pOld, CQuestionEx *pNew, 
      bool bUserInteraction = true, bool bDoCollisionCheck = true);
   CQuestionEx* FindQuestionForTime(UINT nPositionMs);
   UINT GetQuestionCount() { return m_aQuestions.GetSize(); }
   CString& CreateScormId(CString& csExisting = CString(_T("")));

   HRESULT StartTimerThread();
   HRESULT ResetAfterPreview();

   /**
    * Before you can add a question you must first create a questionnaire
    * containing it; so this method returns an error if the given
    * questionnaire already handles/contains questions
    */
   HRESULT AddQuestionnaire(CQuestionnaireEx *pQuestionnaire, bool bUserInteraction = true);
   HRESULT GetQuestionnaires(CArray<CQuestionnaireEx *, CQuestionnaireEx *> *paTarget);
   UINT GetQuestionnaireCount() {return m_aQuestionnaires.GetSize();}
   HRESULT ChangeQuestionnaire(CQuestionnaireEx *pOld, CQuestionnaireEx *pNew, bool bDoRollback=true);
   HRESULT RemoveQuestionnaire(CQuestionnaireEx *pQuestionnaire, bool bUserInteraction = true);

   virtual HRESULT UndoAction(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs = 0, UINT nLengthMs = 0);

   virtual HRESULT ReleaseData(BYTE *pData, UINT nDataLength, bool bDeleteCompletely,
      UndoActionId idActionCode, UINT nPositionMs, UINT nLengthMs);

   HRESULT FillCopyMouseActions(CArray<CMouseAction *, CMouseAction *> *paActions);

   HRESULT WriteLepEntry(CFileOutput *pOutput, LPCTSTR tszLepFilename);
   HRESULT WriteFileEntry(CFileOutput *pOutput, bool bDoIdent, 
      bool bUseFullImagePath, LPCTSTR tszExportPrefix, LPCTSTR tszLepFilename);
   HRESULT ReadLepEntry(SgmlElement *pSgml);

   HRESULT OperateQuestion(UINT nPreviewMs, QuestionOperationId idOperation);

   void PrintState(CArray<CQuestionEx *, CQuestionEx *> *paQuestions = NULL);

   // TODO this is quite a hack: needed to be called from ui message loop
   // otherwise the audio is silent for some seconds if StartPreview() was 
   // called by the wrong thread (FeedbackThreadLoop())
   void FeedbackLoopDoActions();

protected:

   virtual HRESULT SpecificUndo(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs);

   virtual HRESULT CutCopyOrDelete(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard, bool bDoCopy, bool bDoDelete);
   virtual HRESULT InsertRange(UINT nToMs, UINT nLengthMs, BYTE *pData, UINT nByteCount, bool bUndoWanted);

private:
   HRESULT Insert(UINT nToMs, UINT nLength, CQuestionnaire **paQuestionnaires, UINT nCount);

   HRESULT EnlargeQuestion(UINT nExistingIndex, CQuestionEx *pToInclude, UINT nMoveMs);
   HRESULT ChangeTimes(CQuestionEx *pQuestion, int iChangeMs);

   bool IsQuestionnaireInUse(CQuestionnaireEx *pQuestionnaire);

   HRESULT QuestionAnswered(CQuestionEx *pQuestion, bool bQuestionSuccess);

   static UINT TimerThreadLauncher(LPVOID pParam);
   UINT TimerThreadLoop();

   static UINT FeedbackThreadLauncher(LPVOID pParam);
   UINT FeedbackThreadLoop();
   
   bool m_bDoLoopTimer;
   CQuestionEx *m_pLastTimerQuestion;
   CWinThread *m_pThread;

   bool m_bQuestionSuccess;
   CQuestionEx *m_pAnsweredQuestion;
   int m_iFeedbackStartPosMs;

   // sorted by begin time
   CArray<CQuestionEx *, CQuestionEx *> m_aQuestions;

   CMap<CString, LPCTSTR, bool, bool> m_mapScormIds;

   // simple list, they contain references to their questions
   // which is (only) important during writing
   CArray<CQuestionnaireEx *, CQuestionnaireEx *> m_aQuestionnaires;

   CQuestionEx **m_ppLastClipboardData;
   UINT m_nLastClipboardCount;

   int m_nProjectValid; // needed for proper check in thread loop
};

#endif // !defined(AFX_QUESTIONSTREAM_H__50FBC8EF_3012_4A30_AA04_0DE3762C2A69__INCLUDED_)

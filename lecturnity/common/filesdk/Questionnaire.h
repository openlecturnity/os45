#if !defined(AFX_QUESTIONNAIRE_H__8196D085_70FD_4BE1_AB16_A7C9E205F329__INCLUDED_)
#define AFX_QUESTIONNAIRE_H__8196D085_70FD_4BE1_AB16_A7C9E205F329__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define E_QUE_INVALID_DATA  _HRESULT_TYPEDEF_(0x80b70002L)


#include "..\drawsdk\objects.h"
#include "InteractionArea.h"
#include "SgmlParser.h"


class FILESDK_EXPORT CFeedbackDefinition
{
public:
   CFeedbackDefinition();
   virtual ~CFeedbackDefinition();

   HRESULT Parse(SgmlElement *pElement, CString& csRecordPath);

   CString& GetType() { return m_csType; }
   bool IsDisabled() { return m_bIsDisabled; }
   bool IsInherited() { return m_bIsInherited; }
   CString& GetText() { return m_csText; }
   CInteractionArea* GetOkButton() { return m_pOkButton; }

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>& GetObjects() { return m_aObjects; }

   HRESULT ExtractObjects(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paTarget);
   CInteractionArea* ExtractOkButton();

private:
   bool m_bIsDisabled;
   bool m_bIsInherited;
   CString m_csType;
   CString m_csText;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aObjects;
   CInteractionArea *m_pOkButton;
   // TODO "no" button
};


class FILESDK_EXPORT CQuestion
{
public:
	CQuestion();
	virtual ~CQuestion();

   HRESULT Parse(SgmlElement *pElement, CString& csRecordPath);

   CString  GetType()             { return m_csType; }
   CString& GetTitle()            { return m_csTitle; }
   CString& GetQuestion()         { return m_csQuestion; }
   CString& GetScormId()          { return m_csScormId; }
   UINT     GetAchievablePoints() { return m_nAchievablePoints; }
   UINT     GetMaximumTime()      { return m_nMaximumTime; }
   UINT     GetMaximumAttempts()  { return m_nMaximumAttempts; }
   CString& GetSuccessAction()    { return m_csSuccessAction; }
   CString& GetFailureAction()    { return m_csFailureAction; }
   CString& GetPageVisibility()   { return m_csPageVisibility; }
   CString& GetObjectVisibility() { return m_csObjectVisibility; }
   CString& GetObjectActivity()   { return m_csObjectActivity; }
   CString& GetDndAlignment()     { return m_csDndAlignment; }

   CArray<CFeedbackDefinition *, CFeedbackDefinition *>& GetFeedback() { return m_aFeedback; }

   HRESULT ExtractFeedback(CArray<CFeedbackDefinition *, CFeedbackDefinition *> *paFeedback);
   // Activity times are stored in a CPoints as a pair of beginning/end timestamps:
   HRESULT GetActivityTimes(CPoint *pActivitiyTimePeriod);

private:
   CString m_csType;
   CString m_csTitle;
   CString m_csQuestion;
   CString m_csScormId;
   UINT    m_nAchievablePoints;
   UINT    m_nMaximumTime;
   UINT    m_nMaximumAttempts;
   CString m_csSuccessAction;
   CString m_csFailureAction;
   CString m_csPageVisibility;
   CString m_csObjectVisibility;
   CString m_csObjectActivity;
   CString m_csDndAlignment;

   CArray<CFeedbackDefinition *, CFeedbackDefinition *> m_aFeedback;
};


class FILESDK_EXPORT CQuestionnaire  
{
public:
	CQuestionnaire();
	virtual ~CQuestionnaire();

   HRESULT Parse(SgmlElement *pElement, CString& csRecordPath);

   CString& GetTitle()         { return m_csTitle; }
   UINT     GetMaximumPoints() { return m_nMaximumPoints; }
   UINT     GetPassPoints()    { return m_nPassPoints; }
   // if true (relative) then m_nPassPoints is a percentage value
   bool     IsRelative()       { return m_bTypeRelative; }
   bool     IsShowEvaluation() { return m_bShowEvaluation; }
   bool     IsShowProgress()   { return m_bShowProgress; }
   // Specifies that the questionnaire will run using randomly chosen questions.
   bool     IsRandomTest(){ return m_bIsRandomTest; }
   // The number of random questions that will be chosen from total number of questions. Between 1 and total number of questions.
   UINT     GetNumberOfRandomQuestions(){ return m_iNumberOfRandomQuestions; }
   bool     IsDoStopmarks()    { return m_bDoStopmarks; }
   bool     IsImageButtons()   { return m_bHasImageButtons; }
   CString& GetSuccessAction() { return m_csSuccessAction; }
   CString& GetFailureAction() { return m_csFailureAction; }
   UINT     GetQuestionCount() { return m_aQuestions.GetSize(); }
   CString& GetOkText()        { return m_csOkText; }
   CString& GetSubmitText()    { return m_csSubmitText; }
   CString& GetDeleteText()    { return m_csDeleteText; }

   bool ContainsButtonConfig() { return m_bContainsButtonConfig; }
   void GetNormalButtonValues(COLORREF &clrFill, COLORREF &clrLine, COLORREF &clrText, LOGFONT &lf, CString &csFilename);
   void GetMouseoverButtonValues(COLORREF &clrFill, COLORREF &clrLine, COLORREF &clrText, LOGFONT &lf, CString &csFilename);
   void GetPressedButtonValues(COLORREF &clrFill, COLORREF &clrLine, COLORREF &clrText, LOGFONT &lf, CString &csFilename);
   void GetInactiveButtonValues(COLORREF &clrFill, COLORREF &clrLine, COLORREF &clrText, LOGFONT &lf, CString &csFilename);

   CArray<CQuestion *, CQuestion *>&                     GetQuestions() { return m_aQuestions; }
   CArray<CFeedbackDefinition *, CFeedbackDefinition *>& GetFeedback()  { return m_aFeedback; }

   HRESULT ExtractQuestions(CArray<CQuestion *, CQuestion *> *paQuestions);
   HRESULT ExtractFeedback(CArray<CFeedbackDefinition *, CFeedbackDefinition *> *paFeedback);
   // Activity times are stored in a CPoint as a pair of beginning/end timestamps:
   HRESULT GetActivityTimes(CPoint *pActivitiyTimePeriod);

private:
   HRESULT ParseDefaultButtonConfig(SgmlElement *pConfigElement);

   CString m_csTitle;
   UINT    m_nMaximumPoints;
   UINT    m_nPassPoints;
   bool    m_bTypeRelative;
   bool    m_bShowEvaluation;
   bool    m_bShowProgress;
   bool    m_bDoStopmarks;
   bool    m_bHasImageButtons;
   // Specifies that the questionnaire will run using randomly chosen questions.
   bool    m_bIsRandomTest;
   // The number of random questions that will be chosen from total number of questions. Between 1 and total number of questions.
   UINT    m_iNumberOfRandomQuestions;

   CString m_csSuccessAction;
   CString m_csFailureAction;
   CString m_csOkText;
   CString m_csSubmitText;
   CString m_csDeleteText;

   bool     m_bContainsButtonConfig;
   // sligthly double code'ish to CQuestionnaireEx
   CString  m_acsButtonFilename[4];
   COLORREF m_aclrButtonFill[4];
   COLORREF m_aclrButtonLine[4];
   COLORREF m_aclrButtonText[4];
   LOGFONT  m_alfButton[4];

   CArray<CQuestion *, CQuestion *> m_aQuestions;
   CArray<CFeedbackDefinition *, CFeedbackDefinition *> m_aFeedback;
};

#endif // !defined(AFX_QUESTIONNAIRE_H__8196D085_70FD_4BE1_AB16_A7C9E205F329__INCLUDED_)

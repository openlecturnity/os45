#if !defined(AFX_TESTSETTINGS_H__75C21796_982E_4FD9_8163_39F60EDD511B__INCLUDED_)
#define AFX_TESTSETTINGS_H__75C21796_982E_4FD9_8163_39F60EDD511B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestSettings.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// CTestSettings

#include "TestQuestionPage.h"
#include "TestOptionPage.h"
#include "TestDragAndDropPage.h"
#include "TestMultipleChoicePage.h"
#include "TestFillInBlankPage.h"

class CTestSettings : public CPropertySheet
{
	DECLARE_DYNAMIC(CTestSettings)

// Konstruktion
public:
	CTestSettings(UINT nIDCaption, UINT nAnswerPageID, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

public:

// Operationen
public:
	virtual ~CTestSettings();

   void Init(CQuestionEx *pQuestion, CEditorProject *pProject);
   void SetUserEntries(CQuestionEx *pQuestion, int &iQuestionnaireIndex);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTestSettings)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL ContinueModal();
    void SetQuestionnaireSettingsShown() {m_bIsQuestionnaireSettingsShown = true;};
    bool IsQuestionnaireSettingsShown() { return m_bIsQuestionnaireSettingsShown;};
    void RefreshOptionpageActionsControls() {m_pTestOptionPage->RefreshActionsControls();};
	//}}AFX_VIRTUAL

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CTestSettings)
	afx_msg void OnShowQuestionaireSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   void CreateDeleteButton(CQuestionnaireEx *pQuestionnaire, CQuestionEx *pQuestion, 
                           CTimePeriod &visibilityPeriod, CTimePeriod &activityPeriod,
                           CRect &rcDeleteButton);
   void CreateSubmitButton(CQuestionnaireEx *pQuestionnaire, CQuestionEx *pQuestion, 
                           CTimePeriod &visibilityPeriod, CTimePeriod &activityPeriod,
                           CRect &rcSubmitButton);
   void CreateTitleText(CQuestionEx *pQuestion, CString &csTitle, CString &csQuestion, CTimePeriod &visibilityPeriod);
   void CreateQuestionStateText(CQuestionnaireEx *pQuestionnaire, CQuestionEx *pQuestion, CTimePeriod &visibilityPeriod);
   CString GetButtonText(CInteractionAreaEx *pButton);

// Attribute
private:

   CTestQuestionPage *m_pTestQuestionPage;
   CTestOptionPage *m_pTestOptionPage;
   CTestAnswerPage *m_pTestAnswerPage;

   CEditorProject *m_pEditorProject;

   QuestionTypeId m_questionType;

   
   DrawSdk::Text *m_pTitleText ;
   bool m_bUseTitleHashKey;
   UINT m_nTitleHashKey;

   DrawSdk::Text *m_pQuestionText;
   bool m_bUseQuestionHashKey;
   UINT m_nQuestionHashKey;

   DrawSdk::Text *m_pTimerText;
   bool m_bUseTimerTextHashKey;
   UINT m_nTimerTextHashKey;

   DrawSdk::Text *m_pTimerObject;
   bool m_bUseTimerObjectHashKey;
   UINT m_nTimerObjectHashKey;

   DrawSdk::Text *m_pTriesText;
   bool m_bUseTriesTextHashKey;
   UINT m_nTriesTextHashKey;

   DrawSdk::Text *m_pTriesObject;
   bool m_bUseTriesObjectHashKey;
   UINT m_nTriesObjectHashKey;

   DrawSdk::Text *m_pProgressText;
   bool m_bUseProgressTextHashKey;
   UINT m_nProgressTextHashKey;

   DrawSdk::Text *m_pProgressObject;
   bool m_bUseProgressObjectHashKey;
   UINT m_nProgressObjectHashKey;

   double m_dUpperBound;
   CInteractionAreaEx *m_pSubmitButton;
   CInteractionAreaEx *m_pDeleteButton;

   bool m_bButtonsChanged;
   bool m_bIsQuestionnaireSettingsShown;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TESTSETTINGS_H__75C21796_982E_4FD9_8163_39F60EDD511B__INCLUDED_

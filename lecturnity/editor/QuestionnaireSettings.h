#if !defined(AFX_QUESTIONNAIRESETTINGS_H__C0CF7F60_E0A5_49FF_9467_0D2D4571AE08__INCLUDED_)
#define AFX_QUESTIONNAIRESETTINGS_H__C0CF7F60_E0A5_49FF_9467_0D2D4571AE08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuestionnaireSettings.h : Header-Datei
//

#include "QuestionnaireQuestionPage.h"
#include "QuestionnaireOptionPage.h"
#include "QuestionnaireButtonPage.h"

/////////////////////////////////////////////////////////////////////////////
// CQuestionnaireSettings

class CQuestionnaireSettings : public CPropertySheet {
    DECLARE_DYNAMIC(CQuestionnaireSettings)

// Konstruktion
public:
    CQuestionnaireSettings(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, bool bEditNewQuestion = false);

// Operationen
public:
    void Init(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire);
    void SetUserEntries(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CQuestionnaireSettings)
public:
    virtual BOOL OnInitDialog();
    virtual BOOL ContinueModal();
	//}}AFX_VIRTUAL

// Implementierung
public:
    virtual ~CQuestionnaireSettings();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CQuestionnaireSettings)
		// HINWEIS - Der Klassen-Assistent fügt hier Member-Funktionen ein und entfernt diese.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attribute
private:
    CQuestionnaireQuestionPage *m_pQuestionnaireQuestionPage;
    CQuestionnaireOptionPage *m_pQuestionnaireOptionPage;
    CQuestionnaireButtonPage *m_pQuestionnaireButtonPage;
    bool m_bEditNewQuestion;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_QUESTIONNAIRESETTINGS_H__C0CF7F60_E0A5_49FF_9467_0D2D4571AE08__INCLUDED_

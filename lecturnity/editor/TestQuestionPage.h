#if !defined(AFX_TESTQUESTIONPAGE_H__67F89C00_CDA1_4CCB_B2A0_5987B6403696__INCLUDED_)
#define AFX_TESTQUESTIONPAGE_H__67F89C00_CDA1_4CCB_B2A0_5987B6403696__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "editorDoc.h"
#include "QuestionStream.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTestQuestionPage 

class CTestQuestionPage : public CPropertyPage {
    DECLARE_DYNCREATE(CTestQuestionPage)

    // Konstruktion
public:
    CTestQuestionPage();
    ~CTestQuestionPage();

    // Dialogfelddaten
    //{{AFX_DATA(CTestQuestionPage)
    enum { IDD = IDD_TEST_QUESTION_PAGE };
    CComboBox	m_comboQuestionnaires;
    //}}AFX_DATA


public:
    void Init(CQuestionEx *pQuestion, CEditorProject *pProject);
    void GetUserEntries(CString &csTitle, CString &csQuestion, int &iQuestionnaireIndex, 
        UINT &nPageStartMs, UINT &nPageLengthMs, 
        CTimePeriod  &visualityPeriod, CTimePeriod &activityPeriod, 
        bool &bShowDelete, bool &bButtonsChanged);

    UINT ShowQuestionnaireSettings(bool &bButtonsChanged, bool bEditNewQuestion = false);

    bool GetEditNewQuestion() {return m_bEditNewQuestion;}

// Überschreibungen
    // Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CTestQuestionPage)
public:
    virtual void OnOK();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

// Implementierung
protected:
    // Generierte Nachrichtenzuordnungsfunktionen
    //{{AFX_MSG(CTestQuestionPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnKickIdle();
    afx_msg void OnUpdateActivityButtons(CCmdUI* pCmdUI);
    afx_msg void OnShowQuestionaireSettings();
    afx_msg void OnUpdateShowQuestionnaireSettings(CCmdUI* pCmdUI);
    afx_msg void OnSelchangeTestCorrectAction(); 
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// Attributes
private:
    CEditorProject *m_pEditorProject;
    CString m_csTitle;
    CString m_csQuestion;
    AreaPeriodId m_visualityPeriodId;
    AreaPeriodId m_activityPeriodId;
    CArray<CQuestionnaireEx *, CQuestionnaireEx *> m_aQuestionnaires;
    int m_iQuestionnaireIndex;
    bool m_bShowDelete;
    bool m_bButtonsChanged;
    // True if a new question is edited. False if a new question is to be created.
    bool m_bEditNewQuestion;

protected:
    CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TESTQUESTIONPAGE_H__67F89C00_CDA1_4CCB_B2A0_5987B6403696__INCLUDED_

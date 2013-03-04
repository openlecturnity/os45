#if !defined(AFX_QUESTIONNAIREOPTIONPAGE_H__E1867A16_5416_42C7_A37E_9CCBBB618D0A__INCLUDED_)
#define AFX_QUESTIONNAIREOPTIONPAGE_H__E1867A16_5416_42C7_A37E_9CCBBB618D0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "editorDoc.h"
#include "QuestionnaireEx.h"
#include "ImageButtonWithStyle.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CQuestionnaireOptionPage 

class CQuestionnaireOptionPage : public CPropertyPage {
    DECLARE_DYNCREATE(CQuestionnaireOptionPage)

// Konstruktion
public:
    CQuestionnaireOptionPage();
    ~CQuestionnaireOptionPage();

// Dialogfelddaten
	//{{AFX_DATA(CQuestionnaireOptionPage)
    enum { IDD = IDD_QUESTIONNAIRE_OPTION_PAGE };
    CEdit	m_editFailureAction;
    CEdit	m_editCorrectAction;
    CComboBox	m_comboCorrectAction;
    CComboBox	m_comboWrongAction;
    CEdit m_editNumberOfRandomQuestions;
    CSpinButtonCtrl	m_spnMaxNumberOfRandomQuestions;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CQuestionnaireOptionPage)
public:
    virtual void OnOK();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Operationen
public:
    void Init(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire, bool bEditNewQuestion = false);
    void GetUserEntries(CMouseAction *pCorrectMouseAction, 
        CMouseAction *pFailureMouseAction, 
        bool &bShowEvaluation, bool &bShowProgress, bool &bDoStopmark, bool &bIsRandomTest, UINT &iNumberOfRandomQuestions);
    void UpdateRandomizedQuestionsControls();


// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CQuestionnaireOptionPage)
	virtual BOOL OnInitDialog();
    afx_msg void OnKickIdle();
    afx_msg void OnOptionsSettings();
    afx_msg void OnPopupIntern();
    afx_msg void OnPopupExtern();
    afx_msg void OnUpdatePopupExtern(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePopupIntern(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUI);
    afx_msg void OnSelchangeAreaCorrectAction();
    afx_msg void OnSelchangeAreaFailureAction();
    afx_msg void OnBnClickedRandomized();
    afx_msg void OnEnChangeEditRandomQuestionNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    void CreateSlidePopup(int nAction, int nButtonId, int nEditId);
    void CreateTargetmarkPopup(int nAction, int nButtonId, int nEditId);
    UINT GetIndex(AreaActionTypeId actionId);
    void LoadComboBoxActions();

private:
    enum {
        ACTION_CORRECT,
        ACTION_WRONG
    };

    CEditorProject *m_pEditorProject;

    CMenu m_wndMenu;

    CString m_csCorrectActionPath;
    AreaActionTypeId m_correctAction; 
    UINT m_nCorrectMarkOrPageId;

    CString m_csFailureActionPath;
    AreaActionTypeId m_failureAction; 
    UINT m_nFailureMarkOrPageId;

    bool m_bEvaluateAtEnd;
    bool m_bShowProcess;
    bool m_bSetStopmark;

    // Specifies if questions are randomly peeked within a test.
    bool m_bIsRandomTest;
    // The number of randomly peeked questions
    UINT m_iNumberOfRandomQuestions;
    // The total number of questions from questionnaire. (This was created to avoid usage of pQuestionnaire->GetQuestionTotal() in OnUpdate... functions)
    UINT m_iQuestionsNumber;

    // Image of the popup intern button.
    HBITMAP	m_hBitmap;
    CImageButtonWithStyle m_btnCorrectIntern;
    CImageButtonWithStyle m_btnWrongIntern;
protected:
    CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_QUESTIONNAIREOPTIONPAGE_H__E1867A16_5416_42C7_A37E_9CCBBB618D0A__INCLUDED_

#if !defined(AFX_TESTOPTIONPAGE_H__09C95758_8B21_4F7B_9A0D_7B204E9BC177__INCLUDED_)
#define AFX_TESTOPTIONPAGE_H__09C95758_8B21_4F7B_9A0D_7B204E9BC177__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "editorDoc.h"
#include "QuestionnaireEx.h"
#include "ImageButtonWithStyle.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTestOptionPage 

class CTestOptionPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CTestOptionPage)

// Konstruktion
public:
	CTestOptionPage();
	~CTestOptionPage();
   
// Dialogfelddaten
	//{{AFX_DATA(CTestOptionPage)
	enum { IDD = IDD_TEST_OPTION_PAGE };
	CImageButtonWithStyle	m_btnTestWrongIntern;
	CImageButtonWithStyle	m_btnTestCorrectIntern;
	CEdit	m_editFailureDestination;
	CEdit	m_editCorrectDestination;
	CComboBox	m_comboSuccessAction;
	CComboBox	m_comboFailureAction;
	//}}AFX_DATA

public:
    void Init(CQuestionEx *pQuestion, CEditorProject *pProject);
    void GetUserEntries(CQuestionnaireEx *pQuestionnaire, CTimePeriod  &visualityPeriod, CTimePeriod &activityPeriod, 
        CMouseAction *pCorrectAction, CMouseAction *pFailureAction, 
        CArray<CFeedback *, CFeedback *> *paSpecificFeedback);
    void RefreshActionsControls();

// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CTestOptionPage)
public:
    virtual void OnOK();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTestOptionPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnKickIdle();
    afx_msg void OnPopupIntern();
    afx_msg void OnPopupExtern();
    afx_msg void OnUpdatePopupExtern(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePopupIntern(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUI);
    afx_msg void OnSelchangeAreaCorrectAction();
    afx_msg void OnSelchangeAreaFailureAction();
    afx_msg void OnBnClickedCorrectStartReplay();
    afx_msg void OnBnClickedCorrectStopReplay();
    afx_msg void OnBnClickedFailureStartReplay();
    afx_msg void OnBnClickedFailureStopReplay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    void CreateSlidePopup(int nAction, int nButtonId, int nEditId);
    void CreateTargetmarkPopup(int nAction, int nButtonId, int nEditId);
    bool IsRandomTest() { return m_pEditorProject && m_pEditorProject->FirstQuestionnaireIsRandomTest();};
// Attributes
private:
    enum {
        ACTION_CORRECT,
        ACTION_WRONG,
        FEEDBACK_BORDER = 40,
        FEEDBACK_HEIGHT = 150,
        OK_WIDTH = 80, 
        OK_HEIGHT = 20,
        OK_Y_OFFSET = 5
    };

    CMenu m_wndMenu;

    CEditorProject *m_pEditorProject;

    CString m_csCorrectActionPath;
    AreaActionTypeId m_nCorrectAction;
    AreaActionTypeId m_nCorrectNextAction;
    UINT m_nCorrectMarkOrPageId;

    CString m_csFailureActionPath;
    AreaActionTypeId m_nFailureAction; 
    AreaActionTypeId m_nFailureNextAction;
    UINT m_nFailureMarkOrPageId;

    FeedbackTypeId m_feedBackTypes[4];
    bool m_bShowFeedback[4];
    CString m_csFeedback[4];
    CFeedback *m_pFeedbacks[4];
    HBITMAP	m_hBitmap;

    CString m_csCaption;

    CButton m_cbCorrectStartReplay;
    CButton m_cbCorrectStopReplay;
    CButton m_cbFailureStartReplay;
    CButton m_cbFailureStopReplay;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TESTOPTIONPAGE_H__09C95758_8B21_4F7B_9A0D_7B204E9BC177__INCLUDED_

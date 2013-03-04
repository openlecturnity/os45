#if !defined(AFX_TESTFILLINBLANKPAGE_H__AD909418_592B_4984_8D21_315FA86184B8__INCLUDED_)
#define AFX_TESTFILLINBLANKPAGE_H__AD909418_592B_4984_8D21_315FA86184B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnswerEditFB.h"
#include "ListCtrlEx.h" 
#include "..\Studio\resource.h"
#include "TestAnswerPage.h"	 

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTestFillInBlankPage 
                                                      
class CTestFillInBlankPage : public CTestAnswerPage
{
	DECLARE_DYNCREATE(CTestFillInBlankPage)

// Konstruktion
public:
	CTestFillInBlankPage();
	~CTestFillInBlankPage();

public:
   virtual void Init(CQuestionEx *pQuestion, CEditorProject *pProject);
   virtual void FillWithObjects(CQuestionEx *pQuestion, CTimePeriod &visibiltyPeriod, 
                                CTimePeriod &activityPeriod, double dUpperBound);


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTestFillInBlankPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTestFillInBlankPage)
	afx_msg void OnKickIdle();
	afx_msg void OnInsertSpace();
	afx_msg void OnDeleteSpace();
	afx_msg void OnAddAnswer();
	afx_msg void OnDeleteAnswer();
	afx_msg void OnUpdateInsertSpace(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDeleteSpace(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAddAnswer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDeleteAnswer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateListAnswer(CCmdUI *pCmdUI);
	virtual BOOL OnInitDialog();
	afx_msg void OnClickAnswerList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnSelectionChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   void UpdateBlankAnswers();

public:
// Dialogfelddaten
	//{{AFX_DATA(CTestFillInBlankPage)
	enum { IDD = IDD_TEST_FB_PAGE };
	CButton	m_btnDeleteSpace;
	CListCtrlEx	m_lstAnswers;
	CAnswerEditFB	m_answerFB;
	//}}AFX_DATA

private:
   enum {
      Y_OFFSET = 180,
      X_OFFSET = 90
   };
   CStringArray m_aTextAndBlanks;
   CArray<CStringArray *, CStringArray *> m_aCorrectAnswer;
   CEmptyField *m_pCurrentBlank;

protected:
   CString m_csCaption;
   LOGFONT m_lf;
   COLORREF m_clrText;

   bool m_bPositionSet;
   CRect m_rcPosition;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TESTFILLINBLANKPAGE_H__AD909418_592B_4984_8D21_315FA86184B8__INCLUDED_

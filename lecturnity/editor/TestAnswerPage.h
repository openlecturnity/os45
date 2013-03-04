#if !defined(AFX_TESTANSWERPAGE_H__6F32D2B9_32AF_4353_B51E_5DF9729FF2E0__INCLUDED_)
#define AFX_TESTANSWERPAGE_H__6F32D2B9_32AF_4353_B51E_5DF9729FF2E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestionEx.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTestAnswerPage 

class CTestAnswerPage : public CPropertyPage
{
// Konstruktion
public:
	CTestAnswerPage(int iPageID);
	~CTestAnswerPage();

// Dialogfelddaten
	//{{AFX_DATA(CTestAnswerPage)
   CXTTimeEditEx	m_editTimeLimit;
	CEdit	m_editTries;
	CScrollBar m_scrollTries;
	CScrollBar m_scrollPoints;
	CEdit	m_editSuccess;
   CEdit m_editQuestionID;
	//}}AFX_DATA

public:
   virtual void Init(CQuestionEx *pQuestion, CEditorProject *pProject);
   void GetUserEntries(UINT &nAchievablePoints, UINT &nMaximumTries, 
                       UINT &nAvailableTimeMs, CString &csScormId);
   virtual void FillWithObjects(CQuestionEx *pQuestion, CTimePeriod &visibiltyPeriod, 
                                CTimePeriod &activityPeriod, double dUpperBound) = 0;

// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTestAnswerPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTestAnswerPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateTries(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateTimeLimit(CCmdUI *pCmdUI); 
	afx_msg void OnUpdatePoints(CCmdUI *pCmdUI); 
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEditSuccessPoints();
	afx_msg void OnChangeEditTries();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attributes
protected:
   CEditorProject *m_pEditorProject;

   UINT m_nAchievablePoints;
   bool m_bNoRating;
   UINT m_nMaximumTries;
   bool m_bTryLimit;
   UINT m_nAvailableTimeSec;
   bool m_bTimeLimit;

   CString m_csCaption;

   CString m_csQuestionId;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TESTANSWERPAGE_H__6F32D2B9_32AF_4353_B51E_5DF9729FF2E0__INCLUDED_

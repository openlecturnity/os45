#if !defined(AFX_QUESTIONNAIREQUESTIONPAGE_H__10DC937E_B36C_4208_B57A_8C639E4A6CFC__INCLUDED_)
#define AFX_QUESTIONNAIREQUESTIONPAGE_H__10DC937E_B36C_4208_B57A_8C639E4A6CFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "editorDoc.h"
#include "Feedback.h"
#include "..\Studio\resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CQuestionnaireQuestionPage 

class CQuestionnaireQuestionPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CQuestionnaireQuestionPage)

// Konstruktion
public:
	CQuestionnaireQuestionPage();
	~CQuestionnaireQuestionPage();

// Dialogfelddaten
	//{{AFX_DATA(CQuestionnaireQuestionPage)
	enum { IDD = IDD_QUESTIONNAIRE_QUESTION_PAGE };
	CScrollBar	m_scrollPoints;
	CScrollBar	m_scrollPercentage;
	CEdit	m_editName;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CQuestionnaireQuestionPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Operationen
public:
   void Init(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire);
   void GetUserEntries(CEditorProject *pProject, CQuestionnaireEx *pQuestionnaire, 
                       CString &csName, bool &bPassTypeRelative, UINT &nNeededPoints,
                       CArray<CFeedback *, CFeedback *> *paGeneralFeedback);


// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CQuestionnaireQuestionPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnKickIdle();
	afx_msg void OnUpdateEditPercentage(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateEditPoints(CCmdUI *pCmdUI); 
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEditSuccessPercentage();
	afx_msg void OnChangeEditSuccessPoints();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CString m_csName;

   CString m_csFeedback[6];
   bool m_bFeedbackChecked[6];
   CFeedback *m_pFeedback[6];

   UINT m_nMaximumPoints;
   UINT m_nNeededPoints;
   bool m_bPassTypeRelativ;

protected:
   CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_QUESTIONNAIREQUESTIONPAGE_H__10DC937E_B36C_4208_B57A_8C639E4A6CFC__INCLUDED_

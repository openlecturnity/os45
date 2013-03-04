#if !defined(AFX_TESTMULTIPLECHOICEPAGE_H__E1702392_E9E7_4848_9257_2A735F37A338__INCLUDED_)
#define AFX_TESTMULTIPLECHOICEPAGE_H__E1702392_E9E7_4848_9257_2A735F37A338__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestMultipleChoicePage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTestMultipleChoicePage 

#include "ListCtrlEx.h"
#include "..\Studio\resource.h"
#include "TestAnswerPage.h"
#include "DynamicObject.h"

class CAnswerContainer
{
public:
   CAnswerContainer();
   ~CAnswerContainer() {}

public:
   CString m_csAnswer;
   bool m_bIsCorrect;
   DrawSdk::DrawObject *m_pText;
   bool m_bUseHashKey;
   UINT m_nHashKey;
   CDynamicObject *m_pButton;
};

class CTestMultipleChoicePage : public CTestAnswerPage
{
   DECLARE_DYNCREATE(CTestMultipleChoicePage)
      
      // Konstruktion
public:
   CTestMultipleChoicePage();
   ~CTestMultipleChoicePage();
   
   // Dialogfelddaten
   //{{AFX_DATA(CTestMultipleChoicePage)
	enum { IDD = IDD_TEST_MC_PAGE };
   CListCtrlEx	m_listAnswer;
   CComboBox m_comboChoice;
	//}}AFX_DATA
   
   
   // Überschreibungen
   // Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CTestMultipleChoicePage)
	public:
	virtual void OnOK();
	protected:
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
   
   // Implementierung
protected:
   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CTestMultipleChoicePage)
   afx_msg void OnKickIdle();
   virtual BOOL OnInitDialog();
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnAdd();
   afx_msg void OnDelete();
   afx_msg void OnRename();
   afx_msg void OnMoveup();
   afx_msg void OnMovedown();
   afx_msg void OnStyleSelection();
   afx_msg void OnUpdateDelete(CCmdUI *pCmdUI);
   afx_msg void OnUpdateRename(CCmdUI *pCmdUI);
   afx_msg void OnUpdateMoveup(CCmdUI *pCmdUI);
   afx_msg void OnUpdateMovedown(CCmdUI *pCmdUI);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
public:
   virtual void Init(CQuestionEx *pQuestion, CEditorProject *pProject);
   virtual void FillWithObjects(CQuestionEx *pQuestion, CTimePeriod &visibiltyPeriod, 
                                CTimePeriod &activityPeriod, double dUpperBound);
   
private:
   enum {
      TEXT_BUTTON_DIST = 20,
      LINE_DIST = 10,
      Y_OFFSET = 180,
      X_OFFSET = 90
   };
	CImageList m_ilCheckButtons;
   CBitmap m_bmpCheckButtons;
	CImageList m_ilRadioButtons;
   CBitmap m_bmpRadioButtons;

   bool m_bSingleSelection;

   CArray<CAnswerContainer *, CAnswerContainer *> m_aAnswers;
   bool m_bItemWasMoved;
   
protected:
   CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TESTMULTIPLECHOICEPAGE_H__E1702392_E9E7_4848_9257_2A735F37A338__INCLUDED_

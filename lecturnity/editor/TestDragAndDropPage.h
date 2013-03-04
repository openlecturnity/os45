#if !defined(AFX_TESTDRAGANDDROPPAGE_H__DBB6723E_6504_45D4_8BDF_01AFBA5D6DC4__INCLUDED_)
#define AFX_TESTDRAGANDDROPPAGE_H__DBB6723E_6504_45D4_8BDF_01AFBA5D6DC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTestDragAndDropPage 

#include "AnswerListDD.h"
#include "ListCtrlEx.h"
#include "..\Studio\resource.h"
#include "TestAnswerPage.h"

class CSourceContainer
{
public:
   CSourceContainer();
   ~CSourceContainer() {}

   CString m_csImageName;
   CString m_csTargetName;
   DrawSdk::DrawObject *m_pImage;
   DrawSdk::DrawObject *m_pTargetArea;
   int m_iBelongsTo;
   bool m_bUseHashKey;
   UINT m_nHashKey;

};

class CTargetContainer
{
public:
   CTargetContainer();
   ~CTargetContainer() {}

   CString m_csTargetName;
   DrawSdk::DrawObject *m_pTargetArea;
   DrawSdk::DrawObject *m_pText;
   int m_iBelongsTo;
   bool m_bUseTargetHashKey;
   UINT m_nTargetHashKey;
   bool m_bUseTextHashKey;
   UINT m_nTextHashKey;

};

class CTestDragAndDropPage : public CTestAnswerPage
{
	DECLARE_DYNCREATE(CTestDragAndDropPage)

// Konstruktion
public:
	CTestDragAndDropPage();
	~CTestDragAndDropPage();

// Dialogfelddaten
	//{{AFX_DATA(CTestDragAndDropPage)
	enum { IDD = IDD_TEST_DD_PAGE };
	CAnswerListDD	m_answerListDD;
	CListCtrlEx	m_targetList;
	CComboBox	m_AnswerComboBox;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTestDragAndDropPage)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTestDragAndDropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnKickIdle();
	afx_msg void OnTargetListAdd();
	afx_msg void OnTargetListRename();
	afx_msg void OnTargetListDelete();
	afx_msg void OnTargetListUp();
	afx_msg void OnTargetListDown();
	afx_msg void OnAnswerListAdd();
	afx_msg void OnAnswerListRename();
	afx_msg void OnAnswerListDelete();
	afx_msg void OnAnswerListUp();
	afx_msg void OnAnswerListDown();
	afx_msg void OnUpdateTargetListDelete(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateTargetListUp(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateTargetListDown(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateTargetListRename(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAnswerListDelete(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateAnswerListUp(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateAnswerListDown(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateAnswerListRename(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateAnswerComboBox(CCmdUI *pCmdUI);
	afx_msg void OnDblclkListAnswerDd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeAnswerCombobox();
	afx_msg void OnSelchangeAlignmentCombobox();
	afx_msg void OnItemchangedListAnswerDd(NMHDR* pNMHDR, LRESULT* pResult);   
	afx_msg void OnOdstatechangedListAnswerDd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
			  afx_msg void OnFinishAnswerRename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   virtual void Init(CQuestionEx *pQuestion, CEditorProject *pProject);
   virtual void FillWithObjects(CQuestionEx *pQuestion, CTimePeriod &visibiltyPeriod,
                                CTimePeriod &activityPeriod, double dUpperBound);
   DndAlignmentId GetAlignment();

private:
	void PopulateAnswerComboBox();
	void AnswerListEditItem();
	void AnswerListAddItem();

// Attributes
protected:	
	CImageList m_imageList;
   
private:
   enum 
   {
      ALIGNMENT_HORIZONTAL = 0,
      ALIGNMENT_VERTICAL = 1,
      TEXT_Y_OFFSET = 5,
      OBJECT_BORDER_Y_TOP_OFFSET = 180,
      OBJECT_BORDER_Y_BOTTOM_OFFSET = 126,
      OBJECT_BORDER_X_OFFSET = 40,
      SPACING = 20
   };

	CRect m_ThumbnailRect;
	int m_AnswerListSelectedItemIndex;

   UINT m_iAlignment;
   bool m_bAlignmentChanged;

   CArray<CSourceContainer *, CSourceContainer *> m_aImageData;
   CArray<CTargetContainer *, CTargetContainer *> m_aTargetData;
   CUIntArray m_aRenameSources;
   UINT m_nRenameTarget;

   static CString m_csDefaultDirectory;

   bool m_bItemWasMoved;
   

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TESTDRAGANDDROPPAGE_H__DBB6723E_6504_45D4_8BDF_01AFBA5D6DC4__INCLUDED_

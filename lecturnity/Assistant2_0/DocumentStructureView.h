#if !defined(AFX_DOCUMENTSTRUCTUREVIEW_H__E0C1DF0A_0A4F_4474_8AF6_567139936FD6__INCLUDED_)
#define AFX_DOCUMENTSTRUCTUREVIEW_H__E0C1DF0A_0A4F_4474_8AF6_567139936FD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocumentStructureView.h : header file
//

//#include "DocStructRecord.h"
//#include "DocStructReportControl.h"
#include "Resource.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "..\Studio\Resource.h"
#include "DocumentStructure.h"

namespace ASSISTANT{
   class GenericContainer;
}
class CDocStructReportControl;
class CDocStructRecord;

/////////////////////////////////////////////////////////////////////////////
// CDocumentStructureView view

class CDocumentStructureView : public CXTPReportView, public CDocumentStructure
{
protected:
	CDocumentStructureView();           // protected constructor used by dynamic creation
   virtual ~CDocumentStructureView();
	DECLARE_DYNCREATE(CDocumentStructureView)

// Attributes
protected:
   CXTPStatusBar m_wndStatusBar;
   bool m_bIsStatusBarCreated;
   CDocStructReportControl *m_listElements;
   CRgn m_rgnUpdate;
   CImageList m_ilStateImages;
   int m_nCurrentLayout;

// Operations
   int FindItem(UINT nID, int iType = 0);
   void SetRecordLayout();
   BOOL CreateStatusBar();
   void UpdateImageList();
   void ChangeAllRootsIcon(UINT nImageIndex);
   void SetChildrenRecordLayout(CDocStructRecord* pRecord, int nLayout);

public:

   virtual void SetName(void* pData, CString csText);
   virtual CString GetName(void* pData);
   virtual UINT GetType(void *pData);
   virtual void DrawThumbWhiteboard(void* pData, CRect rcThumb, CDC* pDC);
   virtual UINT GetID(void* pData);
   virtual void GetKeywords(void *pData, CString & csKeywords);
   virtual void SetKeywords(void *pData, CString csKeywords);
   virtual void LoadDocument(void *pData);


   //int GetRowHeight(CDC* /*pDC*/, CXTPReportRow* pRow);
   void InsertChapter(ASSISTANT::GenericContainer *pContainer, UINT nParentId);
   void InsertPage(ASSISTANT::GenericContainer *pContainer, UINT nParentId);
   void SetSelected(UINT nId);
   void SetAllUnselected();
   void RemoveAll();
#ifdef _DVS2005
   LRESULT OnMoveItem(WPARAM nIdDrag, LPARAM nIdDrop);
#else
   void OnMoveItem(UINT nIdDrag, UINT nIdDrop);
#endif
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocumentStructureView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
   virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
#ifdef _DVS2005
   LRESULT OnSelectionLoad(WPARAM wParam, LPARAM lParam);
   LRESULT OnSelectionChanged(WPARAM wParam, LPARAM lParam);
   LRESULT OnSelectionClose(WPARAM wParam, LPARAM lParam);
   LRESULT OnSelectionRemove(WPARAM wParam, LPARAM lParam);
   LRESULT OnSelectionDelete(WPARAM wParam, LPARAM lParam);
   LRESULT OnSelectionSave(WPARAM wParam, LPARAM lParam);
   LRESULT OnInsertPage(WPARAM wParam, LPARAM lParam);
   LRESULT OnInsertChapter(WPARAM wParam, LPARAM lParam);
#else
   void OnSelectionChanged(WPARAM wParam, LPARAM lParam);
   void OnSelectionLoad(WPARAM wParam, LPARAM lParam);
   void OnSelectionClose(WPARAM wParam, LPARAM lParam);
   void OnSelectionRemove(WPARAM wParam, LPARAM lParam);
   void OnSelectionDelete(WPARAM wParam, LPARAM lParam);
   void OnSelectionSave(WPARAM wParam, LPARAM lParam);
   void OnInsertPage(WPARAM wParam, LPARAM lParam);
   void OnInsertChapter(WPARAM wParam, LPARAM lParam);
#endif
	//}}AFX_VIRTUAL

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocumentStructureView)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnPaint();
   virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
   afx_msg void OnStatusBarSwitchView(UINT nID);
   afx_msg void OnUpdateStatusBarSwitchView(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCUMENTSTRUCTUREVIEW_H__E0C1DF0A_0A4F_4474_8AF6_567139936FD6__INCLUDED_)

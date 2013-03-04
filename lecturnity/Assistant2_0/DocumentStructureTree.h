#if !defined(AFX_DOCUMENTSTRUCTURETREE_H__63152F9B_961E_4558_ACB4_9D0E4F0B8A1E__INCLUDED_)
#define AFX_DOCUMENTSTRUCTURETREE_H__63152F9B_961E_4558_ACB4_9D0E4F0B8A1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;
// DocumentStructureTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DocumentStructureTree window

class CDocumentStructureTree : public CTreeCtrl
{
// Construction
public:
	CDocumentStructureTree();

	struct FolderState
	{
		vector<int> anId;
		vector<int> anExpanded;
	}m_folderState; 

// Attributes

private:
   HTREEITEM m_hItemDrag;
   HTREEITEM m_hItemDrop;
   HTREEITEM m_hItemEdit;
   bool m_bAfterTree;
   bool m_bBeforeTree;
   bool m_bFileNameExists;
   bool m_bEndDrag;
   CString m_csItemText;
   HTREEITEM m_hItemId;
   UINT m_nLastId;
   
private:
   void FinishDragging(BOOL bDraggingImageList);
   void OnEndDrag(UINT nFlags, CPoint point);
   void OnRenameItem();
   void OnDeleteItem();
   void OnInsertPage();
   void OnInsertChapter();
   void OnSavePageChapter();
   void OnInsertKeywords();
   void FindName(CTreeCtrl& tree, CString name, HTREEITEM hItem);
   bool FindItemId(UINT nId, HTREEITEM hItem);
   void SendMoveMessage(HTREEITEM hItemDrag, HTREEITEM hItemDrop);
	
public:
   void SetLastId(UINT nPageID);
   UINT GetLastId();
   void SetSelection(UINT nID);
   UINT GetSelectionId();
   HTREEITEM FindItem(UINT nId, HTREEITEM hParentItem = NULL);
   bool bInsertInChapter;
   bool bInsertFirst;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocumentStructureTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocumentStructureTree();

	// Generated message map functions

	//{{AFX_MSG(CDocumentStructureTree)

   public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void DoDataExchange(CDataExchange* pDX); 
   afx_msg void OnSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
   enum {
      WM_SELECTION_CHANGED = WM_USER + 1,
      WM_SELECTION_DELETE = WM_USER + 2,
      WM_SELECTION_SAVE = WM_USER + 3,
      WM_INSERT_PAGE = WM_USER + 4,
      WM_INSERT_CHAPTER = WM_USER + 5,
      WM_MOVE_ITEM = WM_USER + 6

   };
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCUMENTSTRUCTURETREE_H__63152F9B_961E_4558_ACB4_9D0E4F0B8A1E__INCLUDED_)

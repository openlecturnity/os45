#if !defined(AFX_LISTCTRLEX_H__2316AED6_1997_41CB_AD75_167B5E999CC2__INCLUDED_)
#define AFX_LISTCTRLEX_H__2316AED6_1997_41CB_AD75_167B5E999CC2__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlEx.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CListCtrlEx 

#define OFFSET_FIRST    2
#define OFFSET_OTHER    6

class CListCtrlEx : public CListCtrl
{
   // Konstruktion
public:
   CListCtrlEx();
   
   // Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CListCtrlEx)
public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //}}AFX_VIRTUAL
   
   // Implementierung
public:
   virtual ~CListCtrlEx();

   void Init(int iColumnCount) {m_iColumnCount = iColumnCount;}
   void ChangeSelectionType(int iType);
   CEdit *GetEdit() {return m_pEdit;}
   int GetCurrentItem() {return m_iItem;}
   void SetCurrentItem(int iItem) {m_iItem = iItem;}
   bool IsItemSelected();

   HRESULT InsertNewItem(int iItem, void *pItemData = NULL);
	HRESULT EditItem(int iItem);
	HRESULT EndEditItem();
   HRESULT MoveSelectedUp();
   HRESULT MoveSelectedDown();
   
   // Generierte Nachrichtenzuordnungsfunktionen
protected:
   //{{AFX_MSG(CListCtrlEx)
   afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnSetFocus(CWnd* pOldWnd);
   afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
   //}}AFX_MSG
   
   DECLARE_MESSAGE_MAP()
      
      virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
   
private:
   bool IsChecked(int iIndex);
   
public:	
   void CheckItem(int nNewCheckedItem);
   void SelectItem(int nItemID);
   int GetSelectedItem();	

public:
   enum {
      SINGLE_SELECTION = 0,
      MULTIPLE_SELECTION = 1,
      WM_EDIT_FINISHED = WM_USER + 2
   };

private:
   CEdit *m_pEdit;

   int m_iSingleCheckedItem;
   int m_iCheckOption;
   int m_iMove;
   int m_iItem;
   bool m_bHasFocus;
   int m_iColumnCount;
   
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_LISTCTRLEX_H__2316AED6_1997_41CB_AD75_167B5E999CC2__INCLUDED_

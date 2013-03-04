#if !defined(AFX_PAGEOBJECTSLIST_H__8DA29339_7804_48E7_9E96_277ABFE50912__INCLUDED_)
#define AFX_PAGEOBJECTSLIST_H__8DA29339_7804_48E7_9E96_277ABFE50912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageObjectsList.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CPageObjectsList 

#include "ExtEdit.h"
#include "InteractionAreaEx.h"

class CEditorDoc;

class CPageObjectsList : public CListCtrl
{
// Konstruktion
public:
	CPageObjectsList();

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPageObjectsList)
	protected:
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CPageObjectsList();

   void SetEditorDoc(CEditorDoc *pEditorDoc) {m_pEditorDoc = pEditorDoc;}

	// Generierte Nachrichtenzuordnungsfunktionen


protected:
	//{{AFX_MSG(CPageObjectsList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	void CheckItem(int nItemID);
		
public:
   void DeSelectItems();
	void SelectItem(int nItemID);
	int GetSelectedItem();
   void RefreshList(CEditorDoc *pDoc);
   void MoveForward();
   void MoveBackward();
   void MoveFront();
   void MoveBack();


private:
	CXTHeaderCtrl	m_wndHeaderCtrl;
	CExtEdit m_wndEdit;

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> m_caInteractions;

   CEditorDoc *m_pEditorDoc;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PAGEOBJECTSLIST_H__8DA29339_7804_48E7_9E96_277ABFE50912__INCLUDED_

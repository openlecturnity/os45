#if !defined(AFX_TEXTEDIT_H__E53B823D_1A18_43DC_A9BC_300209182901__INCLUDED_)
#define AFX_TEXTEDIT_H__E53B823D_1A18_43DC_A9BC_300209182901__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextEdit.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CTextEdit 

#include "backend/mlb_objects.h"

class CTextEdit : public CRichEditCtrl
{
// Konstruktion
public:
	CTextEdit();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTextEdit)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CTextEdit();
   void SetGroup(ASSISTANT::Group *pGroup) {m_pGroup = pGroup;}

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CTextEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

   ASSISTANT::Group *m_pGroup;
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEXTEDIT_H__E53B823D_1A18_43DC_A9BC_300209182901__INCLUDED_

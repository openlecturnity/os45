#if !defined(AFX_EXTEDIT_H__1EBE4EC7_3BAE_4EF7_957D_72DD07D1FE73__INCLUDED_)
#define AFX_EXTEDIT_H__1EBE4EC7_3BAE_4EF7_957D_72DD07D1FE73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtEdit.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CExtEdit 

/** 
 * Overwrites PreTranslateMessage() and thus makes sure that all key events
 * are handled here. Application-wide accelerators (like "a", "e" or " ") 
 * do not interfere the text input.
 * Also handles paint colors (through message reflection): WM_CTLCOLOR_REFLECT
 */
class CExtEdit : public CEdit
{
// Konstruktion
public:
	CExtEdit();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CExtEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CExtEdit();

   CFont* GetExtFont() { return m_pLastFont; }
   void SetExtFont(CFont *pFont);
   void SetTextColor(COLORREF clrText) { m_clrText = clrText; }
   void SetBackColor(COLORREF clrBack);

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CExtEdit)
   afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
   CFont *m_pLastFont;
   //CBrush m_brshBkInactive;
   CBrush m_brshBkActive;
   COLORREF m_clrText;
   COLORREF m_clrBack;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EXTEDIT_H__1EBE4EC7_3BAE_4EF7_957D_72DD07D1FE73__INCLUDED_

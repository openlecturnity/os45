#if !defined(AFX_DLGCOLORBUTTON_H__ABD65BFC_8752_4C66_9C29_DCEAC5BC991D__INCLUDED_)
#define AFX_DLGCOLORBUTTON_H__ABD65BFC_8752_4C66_9C29_DCEAC5BC991D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorButton.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CDlgColorButton 

class CDlgColorButton : public CXTColorPicker
{
// Konstruktion
public:
	CDlgColorButton();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgColorButton)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CDlgColorButton();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CDlgColorButton)
		// HINWEIS - Der Klassen-Assistent fügt hier Member-Funktionen ein und entfernt diese.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
		
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGCOLORBUTTON_H__ABD65BFC_8752_4C66_9C29_DCEAC5BC991D__INCLUDED_

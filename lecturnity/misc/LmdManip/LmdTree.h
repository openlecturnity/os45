#if !defined(AFX_LMDTREE_H__44783717_D684_45B7_A20C_2835FEC8098B__INCLUDED_)
#define AFX_LMDTREE_H__44783717_D684_45B7_A20C_2835FEC8098B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LmdTree.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CLmdTree 

class CLmdTree : public CTreeCtrl
{
// Konstruktion
public:
	CLmdTree();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CLmdTree)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CLmdTree();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CLmdTree)
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_LMDTREE_H__44783717_D684_45B7_A20C_2835FEC8098B__INCLUDED_

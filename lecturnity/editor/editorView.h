// editorView.h : Schnittstelle der Klasse CEditorView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITORVIEW_H__6BDBD555_D8ED_40C0_840B_C906C1108B53__INCLUDED_)
#define AFX_EDITORVIEW_H__6BDBD555_D8ED_40C0_840B_C906C1108B53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CEditorView : public CView
{
protected: // Nur aus Serialisierung erzeugen
	CEditorView();
	DECLARE_DYNCREATE(CEditorView)

// Attribute
public:
	CEditorDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CEditorView)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Testversion in editorView.cpp
inline CEditorDoc* CEditorView::GetDocument()
   { return (CEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_EDITORVIEW_H__6BDBD555_D8ED_40C0_840B_C906C1108B53__INCLUDED_)

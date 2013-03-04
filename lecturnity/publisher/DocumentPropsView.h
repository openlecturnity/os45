// DocumentPropsView.h : Schnittstelle der Klasse CDocumentPropsView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCUMENTPROPSVIEW_H__009825C6_7480_49FD_A51F_17769D7A5C8B__INCLUDED_)
#define AFX_DOCUMENTPROPSVIEW_H__009825C6_7480_49FD_A51F_17769D7A5C8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TitleStatic.h"

class CDocumentPropsView : public CFormView
{
protected: // Nur aus Serialisierung erzeugen
	CDocumentPropsView();
	DECLARE_DYNCREATE(CDocumentPropsView)

public:
	//{{AFX_DATA(CDocumentPropsView)
	enum { IDD = IDD_LRDINFO_FORM };
	CTitleStatic	m_titleCaption;
	CEdit	m_edAudio;
	CEdit	m_edVideo;
	CEdit	m_edTitle;
	CEdit	m_edRecordDate;
	CEdit	m_edProducer;
	CEdit	m_edKeywords;
	CEdit	m_edFilename;
	CListBox	m_lbClips;
	CEdit	m_edAuthor;
	//}}AFX_DATA

// Attribute
public:
	CPublisherDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CDocumentPropsView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual void OnInitialUpdate(); // das erste mal nach der Konstruktion aufgerufen
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CDocumentPropsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CDocumentPropsView)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Testversion in DocumentPropsView.cpp
inline CPublisherDoc* CDocumentPropsView::GetDocument()
   { return (CPublisherDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_DOCUMENTPROPSVIEW_H__009825C6_7480_49FD_A51F_17769D7A5C8B__INCLUDED_)

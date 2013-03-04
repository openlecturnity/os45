// PublisherDoc.h : Schnittstelle der Klasse CPublisherDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLISHERDOC_H__C4B140D2_ABC3_4940_9E46_252044AD8F00__INCLUDED_)
#define AFX_PUBLISHERDOC_H__C4B140D2_ABC3_4940_9E46_252044AD8F00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPublisherDoc : public CDocument
{
protected: // Nur aus Serialisierung erzeugen
	CPublisherDoc();
	DECLARE_DYNCREATE(CPublisherDoc)

// Attribute
public:

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPublisherDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

   virtual void StartPublish();

// Implementierung
public:
	virtual ~CPublisherDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CPublisherDoc)
	afx_msg void OnTemplateConfigure();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
   // 0: LECTURNITY, 1: Streaming, 2: Flash
   int   nTargetFormat;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PUBLISHERDOC_H__C4B140D2_ABC3_4940_9E46_252044AD8F00__INCLUDED_)

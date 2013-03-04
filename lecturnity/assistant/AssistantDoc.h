// AssistantDoc.h : Schnittstelle der Klasse CAssistantDoc
//


#pragma once

class CAssistantDoc : public CDocument
{
protected: // Nur aus Serialisierung erstellen
	CAssistantDoc();
	DECLARE_DYNCREATE(CAssistantDoc)

// Attribute
public:

// Operationen
public:

// Überschreibungen
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementierung
public:
	virtual ~CAssistantDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};



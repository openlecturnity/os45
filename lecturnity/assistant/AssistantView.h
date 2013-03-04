// AssistantView.h : Schnittstelle der Klasse CAssistantView
//


#pragma once


class CAssistantView : public CView
{
protected: // Nur aus Serialisierung erstellen
	CAssistantView();
	DECLARE_DYNCREATE(CAssistantView)

// Attribute
public:
	CAssistantDoc* GetDocument() const;

// Operationen
public:

// Überschreibungen
	public:
	virtual void OnDraw(CDC* pDC);  // Überladen, um diese Ansicht darzustellen
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);



// Implementierung
public:
	virtual ~CAssistantView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Debugversion in AssistantView.cpp
inline CAssistantDoc* CAssistantView::GetDocument() const
   { return reinterpret_cast<CAssistantDoc*>(m_pDocument); }
#endif


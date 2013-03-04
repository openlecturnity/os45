// AssistantView.cpp : Implementierung der Klasse CAssistantView
//

#include "stdafx.h"
#include "Assistant.h"

#include "AssistantDoc.h"
#include "AssistantView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAssistantView

IMPLEMENT_DYNCREATE(CAssistantView, CView)

BEGIN_MESSAGE_MAP(CAssistantView, CView)
	// Standarddruckbefehle
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)



END_MESSAGE_MAP()

// CAssistantView Erstellung/Zerstörung

CAssistantView::CAssistantView()
{
	// TODO: Hier Code zum Erstellen einfügen
}

CAssistantView::~CAssistantView()
{
}

BOOL CAssistantView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CView::PreCreateWindow(cs);
}

// CAssistantView-Zeichnung

void CAssistantView::OnDraw(CDC* /*pDC*/)
{
	CAssistantDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: Code zum Zeichnen der systemeigenen Daten hinzufügen
}


// CAssistantView drucken

BOOL CAssistantView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CAssistantView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}

void CAssistantView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Bereinigung nach dem Drucken einfügen
}

// CAssistantView Diagnose

#ifdef _DEBUG
void CAssistantView::AssertValid() const
{
	CView::AssertValid();
}

void CAssistantView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAssistantDoc* CAssistantView::GetDocument() const // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAssistantDoc)));
	return (CAssistantDoc*)m_pDocument;
}
#endif //_DEBUG


// CAssistantView Meldungshandler

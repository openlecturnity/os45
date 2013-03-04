// AssistantDoc.cpp : Implementierung der Klasse CAssistantDoc
//

#include "stdafx.h"
#include "Assistant.h"

#include "AssistantDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAssistantDoc

IMPLEMENT_DYNCREATE(CAssistantDoc, CDocument)

BEGIN_MESSAGE_MAP(CAssistantDoc, CDocument)
END_MESSAGE_MAP()


// CAssistantDoc Erstellung/Zerstörung

CAssistantDoc::CAssistantDoc()
{
	// TODO: Hier Code für One-Time-Konstruktion einfügen

}

CAssistantDoc::~CAssistantDoc()
{
}

BOOL CAssistantDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}




// CAssistantDoc Serialisierung

void CAssistantDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: Hier Code zum Speichern einfügen
	}
	else
	{
		// TODO: Hier Code zum Laden einfügen
	}
}


// CAssistantDoc Diagnose

#ifdef _DEBUG
void CAssistantDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAssistantDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAssistantDoc-Befehle

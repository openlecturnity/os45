// PublisherDoc.cpp : Implementierung der Klasse CPublisherDoc
//

#include "stdafx.h"
#include "Publisher.h"

#include "PublisherDoc.h"
#include "PublishEngine.h"

#include "PublisherLibs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPublisherDoc

IMPLEMENT_DYNCREATE(CPublisherDoc, CDocument)

BEGIN_MESSAGE_MAP(CPublisherDoc, CDocument)
	//{{AFX_MSG_MAP(CPublisherDoc)
	ON_BN_CLICKED(IDC_TEMPLATE_CONFIGURE, OnTemplateConfigure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPublisherDoc Konstruktion/Destruktion

CPublisherDoc::CPublisherDoc()
{
	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen

}

CPublisherDoc::~CPublisherDoc()
{
}

BOOL CPublisherDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)
   nTargetFormat = 0;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPublisherDoc Serialisierung

void CPublisherDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// ZU ERLEDIGEN: Hier Code zum Speichern einfügen
	}
	else
	{
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPublisherDoc Diagnose

#ifdef _DEBUG
void CPublisherDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPublisherDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPublisherDoc Befehle

void CPublisherDoc::OnTemplateConfigure() 
{
	Template_Configure(AfxGetMainWnd()->GetSafeHwnd());
}

void CPublisherDoc::StartPublish()
{
   CPublishEngine engine(this);
   engine.Publish();
}

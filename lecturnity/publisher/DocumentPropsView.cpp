// DocumentPropsView.cpp : Implementierung der Klasse CDocumentPropsView
//

#include "stdafx.h"
#include "Publisher.h"

#include "PublisherDoc.h"
#include "DocumentPropsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocumentPropsView

IMPLEMENT_DYNCREATE(CDocumentPropsView, CFormView)

BEGIN_MESSAGE_MAP(CDocumentPropsView, CFormView)
	//{{AFX_MSG_MAP(CDocumentPropsView)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocumentPropsView Konstruktion/Destruktion

CDocumentPropsView::CDocumentPropsView()
	: CFormView(CDocumentPropsView::IDD)
{
	//{{AFX_DATA_INIT(CDocumentPropsView)
		// HINWEIS: Der Klassenassistent fügt hier Member-Initialisierung ein
	//}}AFX_DATA_INIT
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen,

}

CDocumentPropsView::~CDocumentPropsView()
{
}

void CDocumentPropsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocumentPropsView)
	DDX_Control(pDX, IDC_LRDINFO_CAPTION, m_titleCaption);
	DDX_Control(pDX, IDC_LRDINFO_AUDIO, m_edAudio);
	DDX_Control(pDX, IDC_LRDINFO_VIDEO, m_edVideo);
	DDX_Control(pDX, IDC_LRDINFO_TITLE, m_edTitle);
	DDX_Control(pDX, IDC_LRDINFO_RECORDDATE, m_edRecordDate);
	DDX_Control(pDX, IDC_LRDINFO_PRODUCER, m_edProducer);
	DDX_Control(pDX, IDC_LRDINFO_KEYWORDS, m_edKeywords);
	DDX_Control(pDX, IDC_LRDINFO_FILENAME, m_edFilename);
	DDX_Control(pDX, IDC_LRDINFO_CLIPSLIST, m_lbClips);
	DDX_Control(pDX, IDC_LRDINFO_AUTHOR, m_edAuthor);
	//}}AFX_DATA_MAP
}

BOOL CDocumentPropsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CFormView::PreCreateWindow(cs);
}

void CDocumentPropsView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

   m_edFilename.SetWindowText(_T("C:\\Dokumente und Einstellungen\\danielss\\Eigene Dateien\\LECTURNITY\\Aufzeichnungen\\unbenannt_0\\unbenannt.lrd"));
   m_edRecordDate.SetWindowText(_T("06.10.2004, 08:24"));
   m_edTitle.SetWindowText(_T("Der neue Publisher, eine GUI-Studie"));
   m_edAuthor.SetWindowText(_T("Martin Danielsson"));
   m_edProducer.SetWindowText(_T("Dr. Rainer Mueller"));
   m_edKeywords.SetWindowText(_T("LECTURNITY; Publisher; MFC; Java; Neue Implementation; C++; Microsoft; Aufzeichnung; Document/View-Konzept"));
   m_edVideo.SetWindowText(_T("iv50, 25.000 fps, 320x240"));
   int aTabStops[] = {10, 40, 80, 110};
   m_lbClips.SetTabStops(4, aTabStops);
   m_lbClips.AddString(_T("1\tlsgc\t5.000fps\t769x532 Pixel"));
   m_lbClips.AddString(_T("2\tlsgc\t5.000fps\t442x397 Pixel"));

	//GetParentFrame()->RecalcLayout();
	//ResizeParentToFit();
}

/////////////////////////////////////////////////////////////////////////////
// CDocumentPropsView Diagnose

#ifdef _DEBUG
void CDocumentPropsView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDocumentPropsView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPublisherDoc* CDocumentPropsView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPublisherDoc)));
	return (CPublisherDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocumentPropsView Nachrichten-Handler

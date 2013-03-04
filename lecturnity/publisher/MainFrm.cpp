// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "Publisher.h"

#include "MainFrm.h"
#include "PublisherDoc.h"

#include "ProfileView.h"
#include "DocumentPropsView.h"
#include "SettingsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Zerstörung

CMainFrame::CMainFrame()
{
	// ZU ERLEDIGEN: Hier Code zur Member-Initialisierung einfügen
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
   /*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}
   */

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Statusleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

	// ZU ERLEDIGEN: Löschen Sie diese drei Zeilen, wenn Sie nicht wollen, dass die Symbolleiste
	//  andockbar ist.
   /*
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
   */
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnose

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Nachrichten-Handler


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
   m_vertSplitter.CreateStatic(this, 2, 1);
   m_horzSplitter.CreateStatic(&m_vertSplitter, 1, 2, 
      WS_CHILD | WS_VISIBLE, m_vertSplitter.IdFromRowCol(1, 0));
   
   /*
   long dlgBaseUnits = ::GetDialogBaseUnits();
   short baseunitX = LOWORD(dlgBaseUnits);
   short baseunitY = HIWORD(dlgBaseUnits);
   int pixelX = (273 * baseunitX) / 4; 
   int pixelY = (268 * baseunitY) / 8;
   */

   BOOL res = m_vertSplitter.CreateView(0, 0, RUNTIME_CLASS(CProfileView), CSize(790, 60), pContext);
   int pixelX = 410;
   int pixelY = 450;
   res = m_horzSplitter.CreateView(0, 0, RUNTIME_CLASS(CDocumentPropsView),
      CSize(pixelX, pixelY), pContext);
   res = m_horzSplitter.CreateView(0, 1, RUNTIME_CLASS(CSettingsView), CSize(1, pixelY),
      pContext);

	return TRUE; //CFrameWnd::OnCreateClient(lpcs, pContext);
}

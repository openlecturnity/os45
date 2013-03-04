#include "stdafx.h"
#include "JavaStarter.h"
#include "JavaStarterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJavaStarterDlg Dialogfeld

CJavaStarterDlg::CJavaStarterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJavaStarterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJavaStarterDlg)
		// HINWEIS: Der Klassenassistent fügt hier Member-Initialisierung ein
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJavaStarterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJavaStarterDlg)
		// HINWEIS: Der Klassenassistent fügt an dieser Stelle DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CJavaStarterDlg, CDialog)
	//{{AFX_MSG_MAP(CJavaStarterDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJavaStarterDlg Nachrichten-Handler

BOOL CJavaStarterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
	// ZU ERLEDIGEN: Hier zusätzliche Initialisierung einfügen
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CJavaStarterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CJavaStarterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

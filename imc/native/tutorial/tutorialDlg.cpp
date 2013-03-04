// tutorialDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "tutorial.h"
#include "tutorialDlg.h"
#include "KeyGenerator.h" // KerberokHandler, szLECTURNITY_REGISTRY

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTutorialDlg Dialogfeld

CTutorialDlg::CTutorialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTutorialDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTutorialDlg)
		// HINWEIS: Der Klassenassistent fügt hier Member-Initialisierung ein
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME_SMALL);
}

void CTutorialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTutorialDlg)
	DDX_Control(pDX, IDC_FILELABEL, fileLabel_);
	DDX_Control(pDX, IDOK, okButton_);
	DDX_Control(pDX, IDCANCEL, cancelButton_);
	DDX_Control(pDX, IDC_HELPTEXT, helpLabel_);
	DDX_Control(pDX, IDC_FILENAME, filenameField_);
	DDX_Control(pDX, IDBROWSE, browseButton_);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTutorialDlg, CDialog)
	//{{AFX_MSG_MAP(CTutorialDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDBROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTutorialDlg Nachrichten-Handler

BOOL CTutorialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
	// ZU ERLEDIGEN: Hier zusätzliche Initialisierung einfügen

   char temp[512];
   HINSTANCE instance = AfxGetInstanceHandle();

   //SetThreadLocale(MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT));
   //SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
   //SetThreadLocale(MAKELCID(MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH), SORT_DEFAULT));
   SetThreadLocale(CTutorialApp.g_languageID);

   LoadString(instance, IDS_HELPLABEL, temp, 512);
   helpLabel_.SetWindowText(temp);
   LoadString(instance, IDS_OKBUTTON, temp, 512);
   okButton_.SetWindowText(temp);
   LoadString(instance, IDS_CANCELBUTTON, temp, 512);
   cancelButton_.SetWindowText(temp);
   LoadString(instance, IDS_BROWSE, temp, 512);
   browseButton_.SetWindowText(temp);
   LoadString(instance, IDS_FILELABEL, temp, 512);
   fileLabel_.SetWindowText(temp);

   filenameField_.SetWindowText(CTutorialApp::g_szTutorialPath);

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CTutorialDlg::OnPaint() 
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

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CTutorialDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTutorialDlg::OnBrowse() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen

   CFileDialog openDialog(TRUE, ".html", "tutorial.html", 
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST, 
      "LECTURNITY Tutorial (tutorial*.html)|tutorial*.html||");
   int result = openDialog.DoModal();

   if (result == IDOK)
   {
      filenameField_.SetWindowText(openDialog.GetPathName());
      okButton_.EnableWindow();
   }
   else
   {
      okButton_.EnableWindow(FALSE);
   }
}

void CTutorialDlg::OnOK() 
{
   // Zunächst die alte Methode aufrufen, so dass das Fenster verschwindet:
	CDialog::OnOK();

   CString fileName;
   filenameField_.GetWindowText(fileName);

   JavaLauncher javaLauncher(fileName);
   if (javaLauncher.doLaunch())
   {
      // note the html file name in the registry
      CRegKey registry;

      LONG result = registry.Create(HKEY_CURRENT_USER, szLECTURNITY_REGISTRY);
      if (result != ERROR_SUCCESS)
      {
         HINSTANCE instance = AfxGetInstanceHandle();
         char error[128];
         LoadString(instance, IDS_ERROR, error, 128);
         char errorMsg[512];
         LoadString(instance, IDS_ERR_REGISTRY, errorMsg, 512);

         MessageBox(errorMsg, error, MB_OK|MB_ICONEXCLAMATION);
         return;
      }

      result = registry.SetValue(fileName, "TutorialPath");
      registry.Close();
   }
}

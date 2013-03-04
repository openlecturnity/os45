// tutorial.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "tutorial.h"
#include "tutorialDlg.h"
#include "launchjava.h"
#include "KeyGenerator.h" // KerberokHandler

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTutorialApp

BEGIN_MESSAGE_MAP(CTutorialApp, CWinApp)
	//{{AFX_MSG_MAP(CTutorialApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTutorialApp Konstruktion

CTutorialApp::CTutorialApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CTutorialApp-Objekt

CTutorialApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTutorialApp Initialisierung

BOOL CTutorialApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

   // Read out the Lecturnity Suite language
   //ASSISTANT::RegistryEntry registryEntry;
   //ASSISTANT::regEntry entry;

   //registryEntry.ReadSecurityEntry(&entry);
   // MessageBox(NULL, entry.language, "Lang", MB_OK|MB_ICONEXCLAMATION);

   char szLanguage[8];
   KerberokHandler::GetLanguageCode(szLanguage);

   /*
   if (entry.language[0] == 'e' && entry.language[1] == 'n')
      g_languageID = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
   else
      g_languageID = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT);
   */

   if (szLanguage[0] == 'e' && szLanguage[1] == 'n')
      g_languageID = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
   else
      g_languageID = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT);
   SetThreadLocale(g_languageID);

   // read the standard path
   readTutorialPath();

   if (!isPathValid())
   {
      // Create the application:
      CTutorialDlg dlg;
	   m_pMainWnd = &dlg;
	   int nResponse = dlg.DoModal();
   }
   else
   {
      // A correct g_szTutorialPath was found in the registry; take that.
      JavaLauncher javaLauncher(g_szTutorialPath);
      javaLauncher.doLaunch();
   }

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}

void CTutorialApp::readTutorialPath()
{
   CRegKey registry;

   DWORD dwCount = 512;
   LONG result = registry.Open(HKEY_CURRENT_USER, szLECTURNITY_REGISTRY);
   if (result == ERROR_SUCCESS)
   {
      result      = registry.QueryValue
         (g_szTutorialPath,
         "TutorialPath",
          &dwCount);
      registry.Close();

      if (result == ERROR_SUCCESS)
         return; // we're done.
   }

   // not successful; let's try HKEY_LOCAL_MACHINE

   result = registry.Open(HKEY_LOCAL_MACHINE, szLECTURNITY_REGISTRY);
   if (result != ERROR_SUCCESS)
   {
      // no TutorialPath nowhere
      strcpy(g_szTutorialPath, "\000");
      return;
   }

   result = registry.QueryValue
      (g_szTutorialPath,
       "TutorialPath",
       &dwCount);
   registry.Close();

   if (result == ERROR_SUCCESS)
      return; // ok, we're done

   // default tuturial path is empty
   strcpy(g_szTutorialPath, "\000");
   return;
}

bool CTutorialApp::isPathValid()
{
   try
   {
      CFileStatus status;
      if (CFile::GetStatus(g_szTutorialPath, status) == TRUE)
         return true;
   }
   catch (exception)
   {
      MessageBox(NULL, "Exception", "Exception", MB_OK);
      return false;
   }

   return false;
}

LCID CTutorialApp::g_languageID = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT);
char CTutorialApp::g_szTutorialPath[512] = "\000";

// editor.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "editor.h"

#include "MainFrm.h"
#include "editorDoc.h"
#include "editorView.h"
#include "getdxver.h"
#include "WhiteboardView.h"

#include "Warning16Bit.h"

#include "lutils.h" // LAutoUpdate

#include "LanguageSupport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditorApp

BEGIN_MESSAGE_MAP(CEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorApp Konstruktion

CEditorApp::CEditorApp()
{

}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CEditorApp-Objekt

CEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEditorApp Initialisierung

BOOL CEditorApp::InitInstance()
{
   //_CrtSetBreakAlloc(8528);
	
   HRESULT hr;

   //CEditorProject p;
   //p.OpenAndExportAutomatic(_T("D:\\LECTURNITY\\Recordings\\Seiten in Kapiteln\\unbenannt.lep"), true);
   
   hr = CLanguageSupport::SetThreadLanguage();
   if (hr != S_OK)
   {
      CString csTitle;
      csTitle.LoadString(IDS_ERROR_E);
      CString csMessage;
      if (hr == E_LS_WRONG_REGISTRY)
         csMessage.LoadString(IDS_ERROR_INSTALLATION_E);
      else if (hr == E_LS_LANGUAGE_NOT_SUPPORTED)
         csMessage.LoadString(IDS_ERROR_LANGUAGE_E);
      else if (hr == E_LS_TREAD_LOCALE)
         csMessage.LoadString(IDS_ERROR_SET_LANGUAGE_E);

      MessageBox(NULL, csMessage, csTitle, MB_OK | MB_TOPMOST);
   }

	InitCommonControls();
	CWinApp::InitInstance();


   // Automatic Update checking
   LAutoUpdate lAutoUpdate;
   if (FALSE != lAutoUpdate.CheckForUpdates())
      return FALSE;

//   AfxSetResourceHandle(m_hInstResDll);

	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

   // In order to get "Lecturnity Editor" as the application
   // title (e.g. in the Registry), we have to set the title
   // manually. The reason is because AFX_IDS_APP_TITLE cannot
   // be loaded at the initialization of our CWinApp object;
   // the resources are linked dynamically to the application
   // and are not available to that time.
   CString csAppName;
   csAppName.LoadString(AFX_IDS_APP_TITLE);
   //First free the string allocated by MFC at CWinApp startup.
   //The string is allocated before InitInstance is called.
   free((void*)m_pszAppName);
   //Change the name of the application file.
   //The CWinApp destructor will free the memory.
   m_pszAppName = _tcsdup(csAppName);

	// Ändern des Registrierungsschlüssels, unter dem unsere Einstellungen gespeichert sind.
   if (m_pszRegistryKey)
      free((void*)m_pszRegistryKey);
   m_pszRegistryKey = _tcsdup(_T("imc AG\\LECTURNITY\\Editor"));
   if (m_pszProfileName)
      free((void*)m_pszProfileName);
   m_pszProfileName = _tcsdup(_T(""));
	//SetRegistryKey(_T("imc AG"));

   AfxEnableControlContainer();

   // Let's do a DirectX check here
   TCHAR tszVersion[32];
   hr = DirectXDiag::GetDXVersion(&m_dwDirectXVersion, tszVersion, 32);
   if (FAILED(hr))
   {
      m_dwDirectXVersion = 0;
      _stprintf(tszVersion, _T("?.?"));
   }

   if (m_dwDirectXVersion < 0x00080000) // DirectX 8.0
   {
      CString msg;
      msg.Format(IDS_MSG_DIRECTX_E, tszVersion);
      CString cap;
      cap.LoadString(IDS_ERROR_E);
      MessageBox(NULL, msg, cap, MB_ICONERROR | MB_OK);
      return FALSE;
   }

   // 16 Bit display?
   if (1 != GetProfileInt(_T("Settings"), _T("DoNotDisplay16BitWarning"), 0))
   {
      DEVMODE devMode;
      ZeroMemory(&devMode, sizeof DEVMODE);
      devMode.dmSize = sizeof DEVMODE;
      if (0 != ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode))
      {
         if (devMode.dmBitsPerPel <= 16)
         {
            CWarning16Bit warn16Bit(NULL);
            if (IDYES == warn16Bit.DoModal())
            {
               ::ShellExecute(NULL, _T("open"), _T("rundll32.exe"), 
                  _T("shell32.dll,Control_RunDLL desk.cpl,,3"), NULL, SW_SHOW);

               return FALSE;
            }
         }
      }
   }

   bool shouldRun = false;
   bool hasErrorOccurred = false;
   int restEvalDays = -1;
   bool isLimitedVersion = false;
   bool isEvalVersion    = false;
   bool hasExpired       = false;
   CString csExpirationDate = _T("");
   CString csOwnVersion;
   csOwnVersion.LoadString(IDS_VERSION_E);
   m_nVersionType = m_securityUtils.GetVersionType(csOwnVersion);

   if (m_nVersionType == VERSION_ERROR)
   {
      hasErrorOccurred = true;
   }
   else if ((m_nVersionType & EVALUATION_VERSION) > 0)
   {
      isEvalVersion = true;
      // Evaluation version
      CString csInstallDate;
      bool success = m_securityUtils.GetInstallationDate(csInstallDate);
      if (!success)
         csInstallDate = _T("000000");
      if (csInstallDate == _T("000000"))
      {
         hasErrorOccurred = true;
      }
      else
      {
         CString csYear = _T("00");
         csYear.SetAt(0, csInstallDate.GetAt(0));
         csYear.SetAt(1, csInstallDate.GetAt(1));
         int year = 2000 + _ttoi(csYear);
         CString csMonth = _T("00");
         csMonth.SetAt(0, csInstallDate.GetAt(2));
         csMonth.SetAt(1, csInstallDate.GetAt(3));
         int month = _ttoi(csMonth);
         CString csDay = _T("00");
         csDay.SetAt(0, csInstallDate.GetAt(4));
         csDay.SetAt(1, csInstallDate.GetAt(5));
         int day = _ttoi(csDay);
         CTime installDate(year, month, day, 0, 0, 0);

         int evaluationPeriod = m_securityUtils.GetEvaluationPeriod();
         CTimeSpan evalSpan(evaluationPeriod + 1, 0, 0, 0);
         CTime lastValidDate = installDate + evalSpan;

         CTime todaysDate = CTime::GetCurrentTime();
         CTimeSpan evalRest = lastValidDate - todaysDate;

         int restMins = evalRest.GetTotalMinutes();
         restEvalDays = evalRest.GetTotalHours() / 24;

         if (restMins > 0)
            shouldRun = true;
      }
   }
   else
   {
      // Campus or enterprise version
      CString csExpDate;
      bool success = m_securityUtils.GetExpirationDate(csExpDate);
      if (success)
      {
         if (csExpDate != "")
         {
            isLimitedVersion = true;
            CString csYear = _T("0000");
            csYear.SetAt(0, csExpDate.GetAt(0));
            csYear.SetAt(1, csExpDate.GetAt(1));
            csYear.SetAt(2, csExpDate.GetAt(2));
            csYear.SetAt(3, csExpDate.GetAt(3));
            CString csMonth = _T("00");
            csMonth.SetAt(0, csExpDate.GetAt(4));
            csMonth.SetAt(1, csExpDate.GetAt(5));
            CString csDay = _T("00");
            csDay.SetAt(0, csExpDate.GetAt(6));
            csDay.SetAt(1, csExpDate.GetAt(7));

            CString csLang;
            csLang.LoadString(IDS_LANGUAGE_E);
            if (csLang == _T("de"))
               csExpirationDate = csDay + _T(".") + csMonth + _T(".") + csYear;
            else
               csExpirationDate = csMonth + _T("/") + csDay + _T("/") + csYear;
            int year = _ttoi(csYear);
            int month = _ttoi(csMonth);
            int day = _ttoi(csDay);

            CTime expDate(year, month, day, 0, 0, 0);
            CTime todaysDate = CTime::GetCurrentTime();
            CTimeSpan restTime = expDate - todaysDate;
            if (restTime.GetTotalHours() >= -24)
               shouldRun = true;
            else
               hasExpired = true;
         }
         else // unlimited version
            shouldRun = true;
      }
      else
      {
         hasErrorOccurred = true;
      }
   }

   if (hasErrorOccurred)
   {
      CString msg;
      msg.LoadString(IDS_MSG_LICENSE_ERROR_E);
      CString cap;
      cap.LoadString(IDS_ERROR_E);

      ::MessageBox(NULL, msg, cap, MB_ICONERROR | MB_OK);
      return FALSE;
   }

   // Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

   m_splashScreen.SetVersionType(m_nVersionType);
   m_splashScreen.SetIsEvalDialog(isEvalVersion, restEvalDays);
   m_splashScreen.SetIsExpirationDialog(isLimitedVersion, hasExpired, csExpirationDate);
   m_splashScreen.SetRequireOk(isEvalVersion || !shouldRun);

   if (shouldRun)
   {
      // possible solution for prohibiting the splash screen when desired:     
      //short sShiftState = ::GetKeyState(VK_SHIFT);
      //short sCtrlState = ::GetKeyState(VK_CONTROL);
      //boolean bSupressSplash = sShiftState & 0x8000 && sCtrlState & 0x8000; // shift and ctrl are pressed

      m_splashScreen.Create(IDD_SPLASH_E);
      //if (!bSupressSplash)
      m_splashScreen.ShowWindow(SW_SHOW);
	  m_splashScreen.UpdateWindow();
   }
   else
   {
      m_splashScreen.DoModal();
      return FALSE;
   }


	LoadStdProfileSettings(8);  // Standard INI-Dateioptionen laden (einschließlich MRU)

	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME_E,
		RUNTIME_CLASS(CEditorDoc),
		RUNTIME_CLASS(CMainFrameE),       // Haupt-SDI-Rahmenfenster
		RUNTIME_CLASS(CWhiteboardView));
	AddDocTemplate(pDocTemplate);

	// Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Distribution of command line arguments; also creates the m_pMainWnd.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

   // Save toolkit resource's language to temp dir and load it
   LCID languageID;
   if ( SUCCEEDED(CLanguageSupport::GetLanguageId(languageID)) == FALSE )
   {
      // Will load default (English)
      ASSERT(false);
   }
   CString csLanguageCode = "en";

   switch (languageID)
   {
   case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
      csLanguageCode = _T("de");
      break;
   case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
      csLanguageCode = _T("en");
      break;
   case MAKELCID(MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT), SORT_DEFAULT):
      csLanguageCode = _T("vi");
      break;
   }

   ::CoInitialize(NULL); // Initializes the COM library otherwise CXTPResourceManager crashed on app close

   _TCHAR pFileName[MAX_PATH];
   if ( CLanguageSupport::SaveToolkitResourceLanguageFile(csLanguageCode, csAppName, pFileName) == false )
   {
#ifdef _DEBUG
      ::MessageBox(NULL, _T("An error occured while loading XTP language resources!"), _T("Error"), MB_OK | MB_ICONERROR);
#endif
   }
   if ( XTPResourceManager()->SetResourceManager(new CXTPResourceManager()) == FALSE )
   {
#ifdef _DEBUG
      ::MessageBox(NULL, _T("An error occured while setting XTP application defined Resource manager!"), _T("Error"), MB_OK | MB_ICONERROR);
#endif
   }
   XTPResourceManager()->SetResourceFile(pFileName);

   // Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
   m_pMainWnd->ShowWindow(SW_SHOW);
   m_pMainWnd->UpdateWindow();
   m_splashScreen.SetRequireOk(true);
   m_splashScreen.StartAutoCloseTimer();

   return TRUE;
}

int CEditorApp::GetVersionType()
{
   return m_nVersionType;
}

/////////////////////////////////////////////////////////////////////////////
// CEditorApp-Nachrichtenbehandlungsroutinen

int CEditorApp::ExitInstance() 
{
	/*
   if (m_hInstResDll)
      ::FreeLibrary(m_hInstResDll);
   m_hInstResDll = NULL;
   */
   XTPResourceManager()->Close();
   CoUninitialize();

	return CWinApp::ExitInstance();
}

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CEditorApp::OnAppAbout()
{
   m_splashScreen.ShowWindow(SW_SHOW);

   // TODO if you want it modal:
   // however this is no solution to the start splash screen
   // when the main window is not yet there or if the main window should't
   // appear at all.
   //CSplashScreen splash(m_pMainWnd);
   //splash.DoModal();
}
/* We override the CEditorApp version to make it use CXTPRecentFileListExt
 * instead of CRecentFileList.
 */
void CEditorApp::LoadStdProfileSettings(UINT nMaxMRU)
{
  const TCHAR csPreviewSection[] = _T("Settings");
  const TCHAR csPreviewEntry[] = _T("PreviewPages");
 ASSERT_VALID(this);
 ASSERT(m_pRecentFileList == NULL);


 if (nMaxMRU != 0)
 {
  // create file MRU since nMaxMRU not zero
   LPCTSTR szFileSection = _T("Recent File List");
   LPCTSTR szFileEntry = _T("File%d");
   // Here's the important part--overriding CRecentFileList
   m_pRecentFileList = new CXTPRecentFileListExt(0, szFileSection, szFileEntry, nMaxMRU,30);
   m_pRecentFileList->ReadList();
 }
 // 0 by default means not set
  m_nNumPreviewPages = GetProfileInt(csPreviewSection, csPreviewEntry, 0);
}  // LoadStdProfileSettings()
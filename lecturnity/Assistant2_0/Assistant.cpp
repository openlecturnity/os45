// Assistant.cpp : Defines the class behaviors for the application.
//

#include <stdafx.h>
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

#include "MainFrm.h"
#include "Assistant.h"
#include "AssistantDoc.h"
#include "AssistantView.h"
#include "getdxver.h"
#include "Warning16Bit.h"
#include "mlb_misc.h"
#include "LanguageSupport.h"

#include "KeyGenerator.h"
#include "lutils.h" // LAutoUpdate

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAssistantApp

BEGIN_MESSAGE_MAP(CAssistantApp, CWinApp)
//{{AFX_MSG_MAP(CAssistantApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
//}}AFX_MSG_MAP
// Standard print setup command
ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
ON_COMMAND(ID_FILE_NEW, &CAssistantApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, &CAssistantApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAssistantApp construction

CAssistantApp::CAssistantApp()
{
   m_bShowImportWarning = true;
   m_bShowAudioWarning = true;
   m_bShowUseSmartBoard = true;
   m_bShowMissingFontsWarning = true;
   m_bFreezeImages = false;
   m_bFreezeMaster = false;
   m_bFreezeDocument = false;
   m_bShowMouseCursor = true;
   m_bShowStartupPage = true;
   m_nCurrentColorScheme = 0;
}
CAssistantApp::~CAssistantApp() {
   m_splashScreen2.DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CAssistantApp object

CAssistantApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAssistantApp initialization

// private
bool CAssistantApp::CheckVersionAndShowSplash() {

  /* bool shouldRun = false;
   bool hasErrorOccurred = false;
   int restEvalDays = -1;
   bool isLimitedVersion = false;
   bool isEvalVersion    = false;
   bool hasExpired       = false;


   CString csExpirationDate = _T("");
   CString csOwnVersion;
   csOwnVersion.LoadString(IDS_VERSION);
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
         if (csExpDate != _T(""))
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
            csLang.LoadString(IDS_LANGUAGE);
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
      msg.LoadString(IDS_MSG_LICENSE_ERROR);
      CString cap;
      cap.LoadString(IDS_ERROR);

      ::MessageBox(NULL, msg, cap, MB_ICONERROR | MB_OK | MB_TOPMOST);
      
      return false;
   }

   m_splashScreen.SetVersionType(m_nVersionType);
   m_splashScreen.SetIsEvalDialog(isEvalVersion, restEvalDays);
   m_splashScreen.SetIsExpirationDialog(isLimitedVersion, hasExpired, csExpirationDate);
   m_splashScreen.SetRequireOk(isEvalVersion || !shouldRun);
   
   if (shouldRun)
   {
      m_splashScreen.Create(IDD_SPLASH);
      m_splashScreen.ShowWindow(SW_SHOW);
      CString csStartingState;
      csStartingState.LoadString(IDS_STARTING_STATE);
      m_splashScreen.SetStateText(csStartingState);

      m_splashScreen.UpdateWindow();   
   }
   else
   {
      m_splashScreen.DoModal();
    
      return false;
   }
*/
   return true;
}

// private
void CAssistantApp::CheckLecturnityHome() {
   CString csLecturnityHome;
   bool bLecturnityHome = ASSISTANT::GetLecturnityHome(csLecturnityHome);
   if (!ASSISTANT::LecturnityHomeEntryExist())
   {
      CString csMessage;
      csMessage.LoadString(IDS_LEC_HOME_WARNING);
      CString csCaption;
      csCaption.LoadString(IDS_WARNING);
      int iAnswer = ::MessageBox(NULL, csMessage, csCaption, MB_OK | MB_TOPMOST);
      
      ((CMainFrameA *)m_pMainWnd)->SelectLecturnityHome();
   }
   else if (_taccess(csLecturnityHome, 00) == -1)
   {
      CString csMessage;
      csMessage.LoadString(IDS_NO_VALID_LECTURNITY_HOME);
      CString csCaption;
      csCaption.LoadString(IDS_WARNING);
      int iAnswer = MessageBox(NULL, csMessage, csCaption, MB_OK | MB_TOPMOST);
      
      ((CMainFrameA *)m_pMainWnd)->SelectLecturnityHome();
   }
   else
   {
      _TCHAR tszLanguageCode[128];
      bool ret = KerberokHandler::GetLanguageCode(tszLanguageCode);
      CString csLanguage = tszLanguageCode;
      if (csLanguage == "de")
      {
         // if there is a lecturnity home director with german sub directories,
         // without english sub directories, display a warning
         CString csGermanRecording = csLecturnityHome + _T("\\Aufzeichnungen");
         CString csRecording = csLecturnityHome + _T("\\Recordings");
         CString csGermanSource = csLecturnityHome + _T("\\Quelldokumente");
         CString csSource = csLecturnityHome + _T("\\Source Documents");
         if ((_taccess(csSource, 00) == -1 && _taccess(csGermanSource, 00) != -1) &&
             (_taccess(csRecording, 00) == -1 && _taccess(csGermanRecording, 00) != -1))
         {
            CString csMessage;
            csMessage.LoadString(IDS_DIRECTORYNAME_CHANGED);
            CString csCaption;
            csCaption.LoadString(IDS_WARNING);
            MessageBox(NULL, csMessage, csCaption, MB_OK | MB_TOPMOST);
            
            if (_taccess(csSource, 00) == -1)
            {
               BOOL result = CreateDirectory(csSource, NULL);
               if (result == 0)
               {
                  csMessage.Format(IDS_ERROR_CREATE_DIR, csSource);
                  MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
               }
            }
         
            CString csCdProjects = csLecturnityHome + _T("\\CD Projects");
            if (_taccess(csCdProjects, 00) == -1)
            {
               BOOL result = CreateDirectory(csCdProjects, NULL);
               if (result == 0)
               {
                  csMessage.Format(IDS_ERROR_CREATE_DIR, csCdProjects);
                  MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
               }
            }
         
            if (_taccess(csRecording, 00) == -1)
            {
               BOOL result = CreateDirectory(csRecording, NULL);
               if (result == 0)
               {
                  csMessage.Format(IDS_ERROR_CREATE_DIR, csRecording);
                  MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
               }
            }
         }
      }
   }
}

BOOL CAssistantApp::InitInstance()
{
   HRESULT hr;
   
   hr = CLanguageSupport::SetThreadLanguage();
   if (hr != S_OK)
   {
      CString csTitle;
      csTitle.LoadString(IDS_ERROR);
      CString csMessage;
      if (hr == E_LS_WRONG_REGISTRY)
         csMessage.LoadString(IDS_ERROR_INSTALLATION);
      else if (hr == E_LS_LANGUAGE_NOT_SUPPORTED)
         csMessage.LoadString(IDS_ERROR_LANGUAGE);
      else if (hr == E_LS_TREAD_LOCALE)
         csMessage.LoadString(IDS_ERROR_SET_LANGUAGE);

      MessageBox(NULL, csMessage, csTitle, MB_OK | MB_TOPMOST);
   }

	AfxEnableControlContainer();


	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
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
   m_pszRegistryKey = _tcsdup(_T("imc AG\\LECTURNITY\\Assistant"));
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
      msg.Format(IDS_MSG_DIRECTX, tszVersion);
      CString cap;
      cap.LoadString(IDS_ERROR);
      MessageBox(NULL, msg, cap, MB_ICONERROR | MB_OK | MB_TOPMOST);
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
            CWarning16BitA warn16Bit(NULL);
            if (IDYES == warn16Bit.DoModal())
            {
               ::ShellExecute(NULL, _T("open"), _T("rundll32.exe"), 
                  _T("shell32.dll,Control_RunDLL desk.cpl,,3"), NULL, SW_SHOW);

               return FALSE;
            }
         }
      }
   }

    
   CString csStartingState;
   csStartingState.LoadString(IDS_STARTING_STATE);
   bool bVersionSuccess = m_splashScreen2.Show(csStartingState);

    //bool bVersionSuccess = CheckVersionAndShowSplash();
    if (!bVersionSuccess)
        return FALSE;


	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

   // load ShowMissingFontsWarnings, ShowImportWarnings, AudioWarnings

   bool bDisplayImportWarning = 
      GetProfileInt(_T("Settings"), _T("DoNotDisplayImportWarning"), 0) == 0 ? true : false;
   SetShowImportWarning(bDisplayImportWarning);

   bool bDisplayAudioWarning = 
      GetProfileInt(_T("Settings"), _T("DoNotDisplayAudioWarning"), 0) == 0 ? true : false;
   SetShowAudioWarning(bDisplayAudioWarning);

   bool bDisplayUseSmartBoard = 
      GetProfileInt(_T("Settings"), _T("DoNotShowUseSmartBoard"), 0) == 0 ? true : false;
   SetShowUseSmartBoard(bDisplayUseSmartBoard);

   bool bDisplayMissingFontsWarning = 
      GetProfileInt(_T("Settings"), _T("DoNotDisplayMissingFontsWarning"), 0) == 0 ? true : false;
   SetShowMissingFontsWarning(bDisplayMissingFontsWarning);

   bool bFreezeImages = 
      GetProfileInt(_T("Settings"), _T("FreezeImages"), 0) == 0 ? false : true;
   SetFreezeImages(bFreezeImages);
   
   bool bFreezeMaster = 
      GetProfileInt(_T("Settings"), _T("FreezeMaster"), 1) == 0 ? false : true;
   SetFreezeMaster(bFreezeMaster);
   
   bool bFreezeDocument = 
      GetProfileInt(_T("Settings"), _T("FreezeDocument"), 1) == 0 ? false : true;
   SetFreezeDocument(bFreezeDocument);
   
   bool bShowMouseCursor = 
      GetProfileInt(_T("Settings"), _T("ShowCursor"), 1) == 0 ? false : true;
   SetShowMouseCursor(bShowMouseCursor);
   			
   bool bShowStatusBar = 
      GetProfileInt(_T("Settings"), _T("ShowStatusBar"), 1) == 0 ? false : true;
   SetShowStatusBar(bShowStatusBar);
   
   bool bShowStartupPage = 
      GetProfileInt(_T("Settings"), _T("ShowStartupPage"), 1) == 0 ? false : true;
   SetShowStartupPage(bShowStartupPage);

   UINT nCurrentColorScheme = GetProfileInt(_T("Settings"), _T("ColorScheme"), 0);
   SetCurrentColorScheme(nCurrentColorScheme);


   
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
   
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME_A,
		RUNTIME_CLASS(CAssistantDoc),
		RUNTIME_CLASS(CMainFrameA),       // main SDI frame window
		RUNTIME_CLASS(CAssistantView));
	AddDocTemplate(pDocTemplate);

    // Save toolkit resource's language to temp dir and load it
    _TCHAR tszLanguageCode[128];
    KerberokHandler::GetLanguageCode(tszLanguageCode);
    _TCHAR pFileName[MAX_PATH];
    CLanguageSupport::SaveToolkitResourceLanguageFile(tszLanguageCode, csAppName, pFileName);
    XTPResourceManager()->SetResourceFile(pFileName);

	// Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
    if (!ProcessShellCommand(cmdInfo)) // Calls OnFileNew() or DocTemplate::OpenDocumentFile()
        return FALSE;

	// The one and only window has been initialized, so show and update it.


    int screenx=GetSystemMetrics(SM_CXSCREEN);
    int screeny=GetSystemMetrics(SM_CYSCREEN);
    CString csATitle;
    csATitle.LoadString(IDS_ASSISTANT_TITLE);
    //csATitle = "noname - " + csATitle;
    //m_pMainWnd->SetWindowText(csATitle);
    CMainFrameA::GetCurrentInstance()->SetAudioVideoTooltips();
    m_pMainWnd->SetWindowPos(NULL,screenx/2-580,screeny/2-390,screenx/2+357 + 115/*160*/,screeny/2+230 + 60/*290*//*200*/,SWP_NOZORDER);
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    //m_splashScreen.SetRequireOk(true);
    //m_splashScreen.StartAutoCloseTimer();

    
    m_splashScreen2.Close();

    CheckLecturnityHome();

#ifndef _STUDIO
    ((CMainFrameA *)m_pMainWnd)->CheckAssistantMainView();
#endif

    ::SetFocus(m_pMainWnd->m_hWnd);
    return TRUE;
}

int CAssistantApp::ExitInstance() 
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

int CAssistantApp::GetVersionType()
{
   return m_nVersionType;
}

void CAssistantApp::OnAppAbout()
{
   m_splashScreen2.Show();
   //m_splashScreen.SetRequireOk(true);
   //m_splashScreen.ShowWindow(SW_SHOW);
   // TODO if you want it modal:
   // however this is no solution to the start splash screen
   // when the main window is not yet there or if the main window should't
   // appear at all.
   //CSplashScreen splash(m_pMainWnd);
   //splash.DoModal();
}

void CAssistantApp::SetShowImportWarning(bool bShowWarning)
{
   m_bShowImportWarning = bShowWarning;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DoNotDisplayImportWarning"), !bShowWarning);
}

void CAssistantApp::SetShowMissingFontsWarning(bool bShowWarning)
{
   m_bShowMissingFontsWarning = bShowWarning;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DoNotDisplayMissingFontsWarning"), !bShowWarning);
}

void CAssistantApp::SetShowAudioWarning(bool bShowWarning)
{
   m_bShowAudioWarning = bShowWarning;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DoNotDisplayAudioWarning"), !bShowWarning);
}

void CAssistantApp::SetShowUseSmartBoard(bool bShowWarning)
{
   m_bShowUseSmartBoard = bShowWarning;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DoNotShowUseSmartBoard"), !bShowWarning);
}

void CAssistantApp::SetShowMouseCursor(bool bShowMouseCursor)
{
   m_bShowMouseCursor = bShowMouseCursor;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("ShowCursor"), bShowMouseCursor);
}
		 
void CAssistantApp::SetShowStatusBar(bool bShowStatusBar)
{
   m_bShowStatusBar = bShowStatusBar;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("ShowStatusBar"), bShowStatusBar);
}

void CAssistantApp::SetShowStartupPage(bool bShowStartupPage)
{
   m_bShowStartupPage = bShowStartupPage;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("ShowStartupPage"), bShowStartupPage);
}

void CAssistantApp::SetFreezeImages(bool bFreezeImages)
{
   m_bFreezeImages = bFreezeImages;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("FreezeImages"), bFreezeImages);
}

void CAssistantApp::SetFreezeMaster(bool bFreezeMaster)
{
   m_bFreezeMaster = bFreezeMaster;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("FreezeMaster"), bFreezeMaster);
}

void CAssistantApp::SetFreezeDocument(bool bFreezeDocument)
{
   m_bFreezeDocument = bFreezeDocument;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("FreezeDocument"), bFreezeDocument);
}

void CAssistantApp::SetCurrentColorScheme(UINT nColorScheme)
{
   m_nCurrentColorScheme = nColorScheme;
   AfxGetApp()->WriteProfileInt(_T("Settings"), _T("ColorScheme"), nColorScheme);
}

void CAssistantApp::OnFileNew()
{
   CWinApp::OnFileNew();
}

void CAssistantApp::OnFileOpen()
{
   CWinApp::OnFileOpen();
}

/* We override the CEditorApp version to make it use CXTPRecentFileListExt
 * instead of CRecentFileList.
 */
void CAssistantApp::LoadStdProfileSettings(UINT nMaxMRU)
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


/* REVIEW UK
 * CLanguageSupport::SetThreadLanguage(): Move error handling to this method (currently done after it).
 *   Maybe specify a parent frame (can be null) as a parameter to this method.
 *   -> This should also be done for any other occurrance of SetThreadLanguage().
 * "Message.h" is too generally named; also does not need to be class (namespace would be sufficient).
 *   -> Why not use MessageBoxHelper.h (that's what it does)?
 *   -> Furthermore: It shouldn't use always NULL as parent (only when there is no parent).
 */

#include "stdafx.h"
#include "Studio.h"
#include "MainFrm.h"

#include "Message.h"
#include "StudioCommandLineInfo.h"
#include "LiveContextRecording.h"

#include "..\Assistant2_0\Warning16Bit.h"
#include "..\Assistant2_0\MainFrm.h"
#include "..\Assistant2_0\AssistantDoc.h"
#include "..\Assistant2_0\AssistantView.h"
#include "..\Assistant2_0\backend\mlb_misc.h"
#include "..\Assistant2_0\backend\la_registry.h" // ASSISTANT::SuiteEntry
#include "..\Assistant2_0\BrowseFolder\XFolderDialog.h"

#include "..\Editor\MainFrm.h"
#include "..\Editor\EditorDoc.h"
#include "..\Editor\WhiteboardView.h"

// lsutl32
#include "getdxver.h"
#include "LanguageSupport.h"
#include "MfcUtils.h"
#include "ProfileUtils.h"
#include "EnterLicenseDialog.h"

#include "KeyGenerator.h"

#include "lutils.h"
#include "RibbonBars.h"
#include "DebugLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define COMMAND_BARS_MODE_ASSISTANT _T("CommandBarsStudio")
#define COMMAND_BARS_MODE_EDITOR _T("CommandBarsEditor")


// Debug line: -lc PUBLISH D:\LECTURNITY\Recordings\EmptyDir\some.lrd -profile x -target y

// CStudioApp

BEGIN_MESSAGE_MAP(CStudioApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &CStudioApp::OnAppAbout)
    ON_UPDATE_COMMAND_UI(ID_APP_ABOUT, &CStudioApp::OnUpdateAppAbout)
    // Dateibasierte Standarddokumentbefehle
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()



int CStudioApp::s_iExitCodeToBe = 0;
bool CStudioApp::s_bIsPowerTrainerMode = false;
CString CStudioApp::s_csPowerTrainerProjectName;

CLiveContext CStudioApp::s_cLiveContext;


const CString CStudioApp::s_sCurrentLepVersion = CString("1.2");
const CString CStudioApp::s_sCompatibleLepVersion = CString("1.2");


// static
void CStudioApp::QuitLiveContextWithCode(int iExitCode) {
    s_iExitCodeToBe = iExitCode;

    CWnd *pWnd = AfxGetMainWnd();
    if (pWnd != NULL && ::IsWindow(pWnd->m_hWnd))
        pWnd->PostMessage(WM_CLOSE);

    // Other variations like ::PostQuitMessage(1);
    // will short circuit the normal tear down mechanism (which results in null pointers etc.).
}

// static
bool CStudioApp::CreateManual(CString &csLrdPath, CString &csWordFilename, CString &csDocumentTitle) {
    
    CEditorProject project;

    LFile fileLrd(csLrdPath);
    if (!fileLrd.Exists(true)) { // exists with a size > 0
        return false;
    }

    CString csLepPath = CEditorDoc::GetCorrespondingLep(csLrdPath);

    bool bUseLrd = csLepPath.IsEmpty();

    bool bSuccess = true;
    if (!bUseLrd) {
        bSuccess = project.Open(csLepPath);
    } else {
        project.Import(csLrdPath, true);
    }

    if (!bSuccess)
        s_iExitCodeToBe = EXIT_CODE_LC_ERROR_CREATE_MANUAL;

    if (bSuccess) {
        if (project.ShowScreenGrabbingAsRecording())
            project.ConvertToDemoDocument();
        HRESULT hr = project.SaveRecordingAsWord(csWordFilename, csDocumentTitle);
        if (hr == E_ABORT) {
            s_iExitCodeToBe = EXIT_CODE_LC_TASK_CANCEL;
        }
        else if (hr != S_OK) {
            s_iExitCodeToBe = EXIT_CODE_LC_ERROR_CREATE_MANUAL;
            bSuccess = false;
        }
    }
    
    return bSuccess;
}

// static
bool CStudioApp::CreateDemoDocument(CString &csLrdPath, CString &csDocFilename) {
    
    CEditorProject project;

    LFile fileLrd(csLrdPath);
    if (!fileLrd.Exists(true)) { // exists with a size > 0
        return false;
    }

    CString csLepPath = CEditorDoc::GetCorrespondingLep(csLrdPath);

    bool bUseLrd = csLepPath.IsEmpty();

    bool bSuccess = true;
    if (!bUseLrd) {
        bSuccess = project.Open(csLepPath);
    } else {
        project.Import(csLrdPath, true);
    }

    if (bSuccess) {
        if (project.ShowScreenGrabbingAsRecording()) {
            project.SetShowScreenGrabbingAsRecording(false);
            project.ConvertToDemoDocument();
        }
        bSuccess = CMainFrameE::DoFileExport(::AfxGetMainWnd(), &project, NULL, csDocFilename);
        project.Close();
    }

    if (!bSuccess && s_iExitCodeToBe == EXIT_CODE_LC_SUCCESS)
        s_iExitCodeToBe = EXIT_CODE_LC_ERROR_CREATE_DEMODOC;

    return bSuccess;
}

// static
void CStudioApp::SetLiveContextExitCode(int iExitCode) {
    s_iExitCodeToBe = iExitCode;
}

// static
bool CStudioApp::Finalize(CString &csLrdPath, IProgressListener *pListener, 
                          HANDLE hExportFinish, bool bWithOpenProgressDialog) {

    CString csLepPath = CEditorDoc::GetCorrespondingLep(csLrdPath);

    bool bSuccess = !csLepPath.IsEmpty();

    if (bSuccess) {
        LFile fileLep(csLepPath);
        bSuccess = fileLep.Exists(true); // this file must exist
    }

    CEditorProject p;
    if (bSuccess) {
        if (bWithOpenProgressDialog)
            bSuccess = p.StartOpen(csLepPath); // Is blocking (calls Dlg->DoModal())
        else
            bSuccess = p.Open(csLepPath, pListener); // Is without progress dialog; also blocking
    }

    if (bSuccess)
        bSuccess = p.ExportAutomatic(pListener, true, hExportFinish);
    // Exporting also saves the export path information if mode is MODE_ASSISTANT

    return bSuccess;
}

// static
bool CStudioApp::Publish(CString &csLrdPath, 
                         bool bDirectMode, bool bSelectFormat, int iSelectedFormat,
                         CString& csTargetPath) {

    LFile fileLrd(csLrdPath);
    if (!fileLrd.Exists(true)) { // exists with a size > 0
        return false;
    }

     LIo::MessagePipe pipePublisher;

     HRESULT hrPipe = S_OK;
    if (IsLiveContextMode())
        hrPipe = pipePublisher.Init(_T("publisher_exe_pipe"), true);
      
    
    CString csConverterPath;

    ASSISTANT::SuiteEntry suiteEntry;
    if (!suiteEntry.ReadProgramPath(csConverterPath))
        csConverterPath.Empty();
    // TODO use lsutl32/lutils methods for this

    if (!csConverterPath.IsEmpty()) {
        if (csConverterPath[csConverterPath.GetLength() - 1] != _T('\\'))
            csConverterPath += _T("\\");
        csConverterPath += _T("Publisher");

        CString csConverterExe = csConverterPath;
        csConverterExe += _T("\\publisher_debug.exe");

        if (_taccess(csConverterExe, 04) != 0) {
            // Normal case: publisher_debug.exe does not exist

            csConverterExe = csConverterPath;
            csConverterExe += _T("\\publisher.exe");
        }

        CString csParameter = _T("");
        if (bDirectMode) {
            csParameter += _T("-direct ");
            if (bSelectFormat && iSelectedFormat != PublishingFormat::TYPE_CUSTOM) {
                CString csFormat;
                csFormat.Format(_T("%d "), iSelectedFormat);
                csParameter += csFormat;
            }
        }

        csParameter += _T(" \"");
        csParameter += csLrdPath;
        csParameter += _T("\"");

        if (csTargetPath.GetLength() > 0) {
            //csParameter += _T(" -Dprofile.output=\"");
            // Also possible but has a problem with "unicode" paths (java.exe does not support...)

            csParameter += _T(" -output \"");
            csParameter += csTargetPath;
            csParameter += _T("\"");
        }

        HINSTANCE hInstance = ShellExecute(NULL, _T("open"), csConverterExe, csParameter, csConverterPath, SW_SHOWNORMAL);
       
        if ((int)hInstance <= 32) {
            s_iExitCodeToBe = EXIT_CODE_LC_PUBLISHER_START;

            return false;
            // TODO maybe something like DebugPrint() in such cases?
        }

    } else {
        // IDS_ERROR_FIND_CONVERTER
        s_iExitCodeToBe = EXIT_CODE_LC_PUBLISHER_START;

        return false;
    }

    if (IsLiveContextMode() && SUCCEEDED(hrPipe)) {
        _TCHAR tszMessage[256];

        DWORD dwExitCode = 0;

         bool bSomethingReceived = LMisc::WaitForPublisherJavaFinish(
             &pipePublisher, tszMessage, 256, &dwExitCode); // This is blocking

        s_iExitCodeToBe = dwExitCode;
    }


    return true;
}


CStudioApp::CStudioApp()
{
	s_csPowerTrainerProjectName.Empty();

    m_dwDirectXVersion = 0;
    m_nVersionType = FULL_VERSION;

    m_pDocTemplateAssistant = NULL;
    m_pDocTemplateEditor = NULL;

    m_nCurrentMode = MODE_ASSISTANT;
    m_nCurrentColorScheme = 0;

    m_pSplashScreen2 = NULL;
    m_pLiveContextStartScreen = NULL;
    //m_pWaitSplashScreen = NULL;
    m_bReturnFromEditMode = false;
    
    m_bShowFormatDialog = false;   
	m_csLrdFileName.Empty();

    m_gdiplusToken = 0;

    m_bXtResourcesOpened = false;

    m_pDptCommunicator = NULL;
}

CStudioApp::~CStudioApp() {
    // Do not delete m_pDocTemplateAssistant and m_pDocTemplateEditor;
    // is done by CDocManager (MFC),

    SAFE_DELETE(m_pSplashScreen2);
    //if (m_pWaitSplashScreen != NULL)
    //    delete m_pWaitSplashScreen;
    SAFE_DELETE(m_pDptCommunicator);
    SAFE_DELETE(m_pLiveContextStartScreen);
}

// Das einzige CStudioApp-Objekt

CStudioApp theApp;


void CStudioApp::OnFileNew()
{
    OpenDocumentFile(NULL);
}

// CStudioApp-Initialisierung

BOOL CStudioApp::InitInstance()
{
    HRESULT hr = S_OK;

    //::_CrtSetBreakAlloc(24767);

    // InitCommonControlsEx() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
    // die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
    // von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    // Befehlszeile auf Standardumgebungsbefehle überprüfen, DDE, Datei öffnen
    CStudioCommandLineInfo cmdInfo(&GetLiveContext());
    ParseCommandLine(cmdInfo);

    m_bShowFormatDialog = cmdInfo.IsShowFormatDialog();

    // Get information from command line
    if (m_bShowFormatDialog) {
        m_csLrdFileName = cmdInfo.GetFormatLrdFileName();
        HWND hWndDesktop = GetDesktopWindow();
        MfcUtils::ShowPublisherFormatSelection(NULL, m_csLrdFileName);
        return TRUE;
    }

    if (cmdInfo.IsDoFinalizeRecording())
        m_pDptCommunicator = new CDptCommunicator();
 
    // Get information from command line
    s_bIsPowerTrainerMode = cmdInfo.IsPowerTrainerMode();
    if (s_bIsPowerTrainerMode)
        s_csPowerTrainerProjectName = cmdInfo.GetPowerTrainerProjectName();

    if (GetLiveContext().IsLiveContextMode()) {
        if (!GetLiveContext().CheckIntegrity()) {
            s_iExitCodeToBe = EXIT_CODE_LC_WRONG_PARAMETERS;
            return FALSE;
        }
    } else {
        
        // If  the installation is done by LIVECONTEXT but it is not started
        // in LIVECONTEXT mode it should fail. An error message is displayed
        // with a possibility to enter a key.

        bool bIsLiveContextInstalled = KerberokHandler::CheckLiveContextInstallation();
        if (bIsLiveContextInstalled) {

            CString csDirectStartError;
            csDirectStartError.LoadString(IDS_MSG_ERROR_LIVECONTEXT_DIRECT);
            //if (KerberokHandler::FailWithLiveContextInstallation(csDirectStartError))
            //    return FALSE;

            int iChoice = MessageBox(
                NULL, csDirectStartError, _T("LECTURNITY"), MB_YESNO | MB_ICONERROR | MB_DEFBUTTON2);

            if (iChoice == IDYES) {
                while (bIsLiveContextInstalled) {
                    bool bKeyWritten = CEnterLicenseDialog::ShowItModal();

                    if (bKeyWritten)
                        bIsLiveContextInstalled = KerberokHandler::CheckLiveContextInstallation();
                    else
                        break;
                }

            }
            
            if (bIsLiveContextInstalled) // still?
                return FALSE; // end program
        }
    }

    // Set language for GUI components and error messages
    hr = CLanguageSupport::SetThreadLanguage();

    if (!cmdInfo.IsDoFinalizeRecording() && hr != S_OK) {
        if (hr == E_LS_WRONG_REGISTRY)
            Message::ShowError(IDS_ERROR_INSTALLATION);
        else if (hr == E_LS_LANGUAGE_NOT_SUPPORTED)
            Message::ShowError(IDS_ERROR_LANGUAGE);
        else if (hr == E_LS_THREAD_LOCALE)
            Message::ShowError(IDS_ERROR_SET_LANGUAGE);
    }
    hr = S_OK;


    hr = CheckDirectXVersion(m_pDptCommunicator);
    if (FAILED(hr)) { 
        return FALSE;
    }

    // Do not use CoIniializeEx if AfxOleInit is used.
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    // Initialize Gdiplus
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    if (cmdInfo.IsDoFinalizeRecording()) {
        CString csLrdPath = cmdInfo.GetLrdToFinalizeName();

        // 3: two progresses are here and the third in the Publisher while Publishing
        m_pDptCommunicator->ActivateProgressScaling(3); 

        if (Finalize(csLrdPath, m_pDptCommunicator, NULL, false))
            return TRUE;
        else
            return FALSE;

    }

    if (GetLiveContext().IsLiveContextMode() && GetLiveContext().GetModeType() == LcPublish) {

        bool bSuccess = true;
        CString csLrdPath = GetLiveContextRecordFile();

        bool bDocNeedsFinalize = LMisc::CheckNeedsFinalization(csLrdPath);
        if (bDocNeedsFinalize)
            bSuccess = Finalize(csLrdPath, NULL, NULL, true);

        if (_taccess(GetLiveContext().GetProfile(), 04) != 0) {
            // Profile file does not exist or is not readable
            s_iExitCodeToBe = EXIT_CODE_LC_MISSING_PROFILE;
            return FALSE;
        }

        if (bSuccess) {

            ProfileUtils::WriteActiveProfile(GetLiveContext().GetProfile());

            bSuccess = Publish(csLrdPath, true, false,
                PublishingFormat::TYPE_CUSTOM, GetLiveContext().GetOutput());
        }

        if (bSuccess)
            return TRUE;
        else {
            s_iExitCodeToBe = EXIT_CODE_LC_ERROR_PUBLISH_START;
            return FALSE;
        }
    }
    
    if (GetLiveContext().IsLiveContextMode() && GetLiveContext().GetModeType() == LcCreateManual) {

        bool bSuccess = true;
        CString csLrdPath = GetLiveContext().GetInput();

        bSuccess = CreateManual(csLrdPath, GetLiveContext().GetOutput(), GetLiveContext().GetTitle());

        if (bSuccess)
            return TRUE;
        else {
            return FALSE;
        }
    }
    
    if (GetLiveContext().IsLiveContextMode() && GetLiveContext().GetModeType() == LcCreateDemoDoc) {

        bool bSuccess = true;
        CString csLrdPath = GetLiveContext().GetInput();

        bSuccess = CreateDemoDocument(csLrdPath, GetLiveContext().GetOutput());

        if (bSuccess)
            return TRUE;
        else {
            s_iExitCodeToBe = EXIT_CODE_LC_ERROR_CREATE_DEMODOC;
            return FALSE;
        }
    }

    if (GetLiveContext().IsLiveContextMode() && GetLiveContext().GetModeType() == LcRecord) {
        CString csStartingState;
        csStartingState.LoadString(IDS_STARTING_STATE);
        m_pLiveContextStartScreen = new CLiveContextStartScreen();
        m_pLiveContextStartScreen->Show(csStartingState, true, false);
        m_pLiveContextStartScreen->UpdateProgress(1,5);

        CLiveContextCommunicator::GetInstance()->Init(CStudioApp::GetLiveContext().GetPipeName());
        m_pLiveContextStartScreen->UpdateProgress(2,5);

        CString csApplication;
        ChangeApplicationName(csApplication);
        // SetXtLanguageResource must be called because otherwise the defaut buttons(ok,cancel) from
        // settings dialog would have EN texts in DE version 
        SetXtLanguageResource(csApplication);
        m_pLiveContextStartScreen->UpdateProgress(3,5);
        
        CLiveContextRecording liveContextRecording;
        bool bSuccess = liveContextRecording.StartScreenRecording(m_pLiveContextStartScreen);
        
        if (bSuccess) {
            // If there was a problem or cancel, the Livecontext exit code will be set
            // in CLiveContextStartScreen.
            return TRUE;
        } else {
            // StartScreenRecording only return false if there was a problem 
            // initializing audio
            s_iExitCodeToBe = EXIT_CODE_LC_ERROR_AUDIO_INIT;
            return FALSE;
        }
    }


    // Automatic Update checking
    /*LAutoUpdate lAutoUpdate;
    if (FALSE != lAutoUpdate.CheckForUpdates())
        return FALSE;*/

    // Initializing OLE with AfxOleInit leads to problems with PowerPoint import
    // -> "Server is busy"

    // OLE-Bibliotheken initialisieren
    //if (!AfxOleInit())
    //{
    //    AfxMessageBox(IDP_OLE_INIT_FAILED);
    //    return FALSE;
    //}
    

    CString csApplication;
    ChangeApplicationName(csApplication);
    ChangeRegistryKey();
    ChangeProfileDirectory();

    AfxEnableControlContainer();

    hr = CheckFor16BitDisplay();
    if (FAILED(hr))
        return FALSE;

    CString csStartingState;
    csStartingState.LoadString(IDS_STARTING_STATE);

    if(!IsLiveContextMode()) {
        m_pSplashScreen2 = new CSplashScreen2;
#ifndef _DEBUG
        bool bVersionSuccess = m_pSplashScreen2->Show(csStartingState);
        if (!bVersionSuccess)
            return FALSE;
#endif
    } else {
        m_pLiveContextStartScreen = new CLiveContextStartScreen();
        bool bIsEditMode = (GetLiveContext().GetModeType() == LcEdit);
        m_pLiveContextStartScreen->Show(csStartingState, false, bIsEditMode);
    }

    UINT nCurrentColorScheme = GetProfileInt(_T("Settings"), _T("ColorScheme"), 0);
    SetCurrentColorScheme(nCurrentColorScheme);

    // Might have been changed during SplashScreen
    hr = GetVersion();
    if (FAILED(hr))
        return FALSE;


    m_pDocTemplateAssistant = new CStudioDocTemplate(IDR_MAINFRAME_A,
        RUNTIME_CLASS(CAssistantDoc),
        RUNTIME_CLASS(CMainFrameA), // Benutzerspezifischer MDI-Child-Rahmen
        RUNTIME_CLASS(CAssistantView));
    if (!m_pDocTemplateAssistant)
        return FALSE;
    AddDocTemplate(m_pDocTemplateAssistant);
    if(IsLiveContextMode())
        m_pLiveContextStartScreen->UpdateProgress(1, 6);

    m_pDocTemplateEditor = new CStudioDocTemplate(IDR_MAINFRAME_E,
        RUNTIME_CLASS(CEditorDoc),
        RUNTIME_CLASS(CMainFrameE), // Benutzerspezifischer MDI-Child-Rahmen
        RUNTIME_CLASS(CWhiteboardView));
    if (!m_pDocTemplateEditor)
        return FALSE;
    AddDocTemplate(m_pDocTemplateEditor);
    if(IsLiveContextMode())
        m_pLiveContextStartScreen->UpdateProgress(2, 6);

    // Haupt-MDI-Rahmenfenster erstellen
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME)) {
        delete pMainFrame;
        return FALSE;
    }
    m_pMainWnd = pMainFrame;
    
    // Rufen Sie DragAcceptFiles nur auf, wenn eine Suffix vorhanden ist.
    //  In einer MDI-Anwendung ist dies unmittelbar nach dem Festlegen von m_pMainWnd erforderlich
    m_pMainWnd->DragAcceptFiles(TRUE);
    if(IsLiveContextMode())
        m_pLiveContextStartScreen->UpdateProgress(3, 6);

    SetXtLanguageResource(csApplication);
    if(IsLiveContextMode())
        m_pLiveContextStartScreen->UpdateProgress(4, 6);

    
    if (GetLiveContext().IsLiveContextMode()) {
        LcModeType iLcModeType = GetLiveContext().GetModeType();

        // Setting proper parameters for "ProcessShellCommand()" (which opens documents)
        if (iLcModeType == LcPreview || iLcModeType == LcEdit) {
            cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
            cmdInfo.m_strFileName = GetLiveContextRecordFile();
        }
    }
    if(IsLiveContextMode())
        m_pLiveContextStartScreen->UpdateProgress(5, 6);

    // Verteilung der in der Befehlszeile angegebenen Befehle. Gibt FALSE zurück, wenn
    // die Anwendung mit /RegServer, /Register, /Unregserver oder /Unregister gestartet wurde.
    // TODO: Error message with correct parameter info
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;
    if(IsLiveContextMode())
        m_pLiveContextStartScreen->UpdateProgress(6, 6);

    // Update Splash screen
    //m_splashScreen.SetRequireOk(true);
    //m_splashScreen.StartAutoCloseTimer();
    if(!IsLiveContextMode())
        m_pSplashScreen2->Close();
    else
        m_pLiveContextStartScreen->Close();

    if (GetLiveContext().IsLiveContextMode()) {
        if (GetLiveContext().GetModeType() == LcPreview) {
            CEditorDoc *pDoc = CMainFrameE::GetEditorDocument();
            if (pDoc != NULL)
                pDoc->StartPreview();
        }
        // LcEdit needs no more special handling during start
        // LcPublish is considered further above
    }

    if (!GetLiveContext().IsLiveContextMode())
        CheckLecturnityHome();

    ::SetFocus(m_pMainWnd->m_hWnd);

    // If the startup page should be visible, switch
    CMainFrameA *pChildFrameA = CMainFrameA::GetCurrentInstance();
    if (pChildFrameA)
        pChildFrameA->CheckAssistantMainView();

    return TRUE;
}

int CStudioApp::ExitInstance()
{
    Gdiplus::GdiplusShutdown(m_gdiplusToken);

    if (m_bXtResourcesOpened)
        XTPResourceManager()->Close();

    CoUninitialize();

    int iSystemExitCode = CWinApp::ExitInstance();

    int iThisExitCode = s_iExitCodeToBe;
    if (iThisExitCode == 0 && iSystemExitCode != 0)
        iThisExitCode = iSystemExitCode;

    if (GetLiveContext().IsLiveContextMode()) {
        if (CLiveContextCommunicator::GetInstance()->Connected()) {
            CLiveContextCommunicator::GetInstance()->SendProgramExit(iThisExitCode, 
                &CLiveContextCommunicator::GetInstance()->GetWarnings(), 
                &CLiveContextCommunicator::GetInstance()->GetErrors());
        }
        CLiveContextCommunicator *lcCommunicator = CLiveContextCommunicator::GetInstance();
        SAFE_DELETE(lcCommunicator);
    }

    return iThisExitCode;
}

void CStudioApp::LoadStdProfileSettings(UINT nMaxMRU)
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
}

CDocument* CStudioApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
    // TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.

    CString csFileSuffix = lpszFileName;
    StringManipulation::GetFileSuffix(csFileSuffix);


    bool bDoOpen = false;
    bool bCloseSuccess = true;
    if (lpszFileName == NULL) { // File new
        bCloseSuccess = m_pDocTemplateAssistant->CloseDocumentFile();
        if (bCloseSuccess)
            bCloseSuccess = m_pDocTemplateEditor->CloseDocumentFile();
    }
    else {
        UINT nNewDocumentMode = MODE_UNDEFINED;
        if (csFileSuffix.CompareNoCase(_T("lsd")) == 0)
            nNewDocumentMode = MODE_ASSISTANT;
        if (csFileSuffix.CompareNoCase(_T("lrd")) == 0)
            nNewDocumentMode = MODE_EDITOR;

        if (nNewDocumentMode == MODE_UNDEFINED) {
            // TODO ERROR MESSGAE
            return NULL;
        }
        if (nNewDocumentMode != m_nCurrentMode) {
            if (m_nCurrentMode == MODE_ASSISTANT)
                bCloseSuccess = m_pDocTemplateAssistant->CloseDocumentFile();
            if (m_nCurrentMode == MODE_EDITOR)
                bCloseSuccess = m_pDocTemplateEditor->CloseDocumentFile();
        }
        if (bCloseSuccess)
            m_nCurrentMode = nNewDocumentMode;
    }
    if (bCloseSuccess)
        bDoOpen = true;


    CDocument *pDocument = NULL;
    if (bDoOpen) {
        if (m_nCurrentMode == MODE_ASSISTANT) {
           if(m_bReturnFromEditMode)
              m_pDocTemplateAssistant->m_bRetFromEditMode = true;
            pDocument = m_pDocTemplateAssistant->OpenDocumentFile(lpszFileName);
        }
        else {
            pDocument = m_pDocTemplateEditor->OpenDocumentFile(lpszFileName);
        }

        if (m_nCurrentMode == MODE_ASSISTANT) {
            CMainFrameA *pChildFrameA = CMainFrameA::GetCurrentInstance();
            if (pChildFrameA) {
                pChildFrameA->CheckAssistantMainView();
                if(pChildFrameA->GetStartupView()!= NULL && pChildFrameA->IsInStartupMode())
                   pChildFrameA->GetStartupView()->SetReturnFromEditMode(false);
                CXTPRibbonTab* pTabStart = pChildFrameA->GetRibbonBar()->FindTab(ID_TAB_START);
                if (pTabStart)
                    pTabStart->Select();
            }
            ASSISTANT::Project::active->ActivateStartStopHotKey(true);
        } else if (m_nCurrentMode == MODE_EDITOR) {
            ASSISTANT::Project::active->ActivateStartStopHotKey(false);
            CMainFrameE *pChildFrameE = CMainFrameE::GetCurrentInstance();
            if (pChildFrameE) {
                CXTPRibbonTab* pTabStart = pChildFrameE->GetRibbonBar()->FindTab(ID_TAB_START_E);
                if (pTabStart)
                    pTabStart->Select();
            }
            CEditorDoc *pDoc = CMainFrameE::GetEditorDocument();
            if (pDoc != NULL && pDoc->IsDemoModeRequired()) {
                pDoc->OpenAsScreenSimulation();
                pChildFrameE->AdjustToProjectMode();
            }
            CWnd *pMainFrame = AfxGetMainWnd();
            if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame)) {
                ((CMainFrame *)pMainFrame)->GetRibbonBars()->SetSaved(false);
                ((CMainFrame *)pMainFrame)->GetRibbonBars()->SetLoaded(false);
                ((CMainFrame *)pMainFrame)->GetStatusBar()->SaveState(COMMAND_BARS_MODE_ASSISTANT);
                ((CMainFrame *)pMainFrame)->UpdateEditorStatusBarAndMenu();
            }
        }
    }

    return pDocument;

}

void CStudioApp::WinHelpInternal(DWORD_PTR dwData, UINT nCmd) {

    bool bHasManual = CLanguageSupport::ManualExists();
    if (bHasManual && CMainFrameA::GetCurrentInstance() != NULL) {
        CMainFrameA::GetCurrentInstance()->SendMessage(WM_COMMAND, ID_HELP);
    }
}

// CStudioApp-Meldungshandler

void CStudioApp::OnAppAbout()
{
    //m_splashScreen.SetRequireOk(true);
    //m_splashScreen.ShowWindow(SW_SHOW);

    if (m_pSplashScreen2 != NULL)
        delete m_pSplashScreen2;
    m_pSplashScreen2 = NULL;

    // Create it new with proper window as parent
    if(!IsLiveContextMode()) {
        m_pSplashScreen2 = new CSplashScreen2;
        m_pSplashScreen2->Show(m_pMainWnd->m_hWnd);
    }
}

void CStudioApp::OnUpdateAppAbout(CCmdUI* pCmdUI)
{
   BOOL bEnable = TRUE;

   // About is not possible in LiveContext mode
   if (IsLiveContextMode())
      bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}

void CStudioApp::CloseEditorMode() {

    m_bReturnFromEditMode = true;
    UINT nCurrentMode = m_nCurrentMode;
    m_nCurrentMode = MODE_ASSISTANT;
    CWaitCursor wait;
    CString csStartingState;
    csStartingState.LoadString(IDS_EDITOR_CLOSE_WAIT_STATE);
    CDocument *pDocument = OpenDocumentFile(NULL); 

    //m_pWaitSplashScreen = new CWaitSplashScreen;
    //bool bVersionSuccess = m_pWaitSplashScreen->Show(csStartingState);
    //m_pWaitSplashScreen->StepIt();

    if (pDocument) {
        CMainFrame *pMainFrameStudio = (CMainFrame*)AfxGetMainWnd();
        if (pMainFrameStudio == NULL || pMainFrameStudio->GetRuntimeClass() != RUNTIME_CLASS(CMainFrame))
            return ;
        pMainFrameStudio->GetStatusBar()->SaveState(COMMAND_BARS_MODE_EDITOR);
        CMainFrameA *pMainFrameA = CMainFrameA::GetCurrentInstance();
        if(pMainFrameA != NULL) {
            pMainFrameStudio->CreateAssistantStatusBar();
            pMainFrameA->UpdateStatusDiskSpace();
            pMainFrameA->UpdateStatusDuration();
        }

        CXTPCommandBars* pCommandBars = pMainFrameStudio->GetCommandBars();
        pMainFrameStudio->GetRibbonBars()->SaveQuickAccessControlsEditor();
        pMainFrameStudio->GetRibbonBars()->ReInitQuickAccessControlsAssistant();
    } else {
        if (CMainFrameE::GetCurrentInstance() && CMainFrameE::GetCurrentInstance()->GetStructureView()) {
            CMainFrameE::GetCurrentInstance()->GetStructureView()->RefreshList();
        }
        m_nCurrentMode = nCurrentMode;
    }


    if (IsLiveContextMode()) {
        int iLiveContextExitCode = 0;
        if (!IsLiveContextPreview()) {
            // Check for Edit mode but no change done; set return value accordingly
            // The .lep against which is checked here will be written by "OpenDocumentFile()" above.

            bool bDocModified = LMisc::CheckNeedsFinalization(GetLiveContextRecordFile());
            if (!bDocModified)
                iLiveContextExitCode = EXIT_CODE_LC_NO_EDIT_DONE;
        }

        QuitLiveContextWithCode(iLiveContextExitCode);
    }
}

void CStudioApp::SetCurrentColorScheme(UINT nColorScheme)
{
    m_nCurrentColorScheme = nColorScheme;
    AfxGetApp()->WriteProfileInt(_T("Settings"), _T("ColorScheme"), nColorScheme);
}

bool CStudioApp::SaveLoadedDocuments()
{
    bool bCloseSuccess = true;

    if (m_nCurrentMode == MODE_ASSISTANT)
        bCloseSuccess = m_pDocTemplateAssistant->Save();
    if (m_nCurrentMode == MODE_EDITOR)
        bCloseSuccess = m_pDocTemplateEditor->Save();

    return bCloseSuccess;
}

// private functions 
void CStudioApp::ChangeApplicationName(CString &csApplication)
{
    // Load Application name
    csApplication.LoadString(AFX_IDS_APP_TITLE);
    //First free the string allocated by MFC at CWinApp startup.
    //The string is allocated before InitInstance is called.
    free((void*)m_pszAppName);
    //Change the name of the application file.
    //The CWinApp destructor will free the memory.
    m_pszAppName = _tcsdup(csApplication);
}

void CStudioApp::ChangeRegistryKey()
{
    // Change registry key, where application settings are saved.
    CString csRegistryKey;
    csRegistryKey.LoadString(IDS_REGISTRY_KEY);
    if (m_pszRegistryKey)
        free((void*)m_pszRegistryKey);
    m_pszRegistryKey = _tcsdup(csRegistryKey);
}

void CStudioApp::ChangeProfileDirectory()
{
    // Change directory where the application profile should be saved
    if (m_pszProfileName)
        free((void*)m_pszProfileName);
    m_pszProfileName = _tcsdup(_T(""));
}

HRESULT CStudioApp::CheckDirectXVersion(CDptCommunicator *pDptCommunicator)
{
    HRESULT hr = S_OK;

    // Let's do a DirectX check here
    TCHAR tszVersion[32];
    hr = DirectXDiag::GetDXVersion(&m_dwDirectXVersion, tszVersion, 32);
    if (FAILED(hr)) {
        m_dwDirectXVersion = 0;
        _stprintf(tszVersion, _T("?.?"));
    }

    if (m_dwDirectXVersion < 0x00080000) { // DirectX 8.0
        CString csDxMessage;
        csDxMessage.Format(IDS_MSG_DIRECTX, tszVersion);
        if (pDptCommunicator)
            pDptCommunicator->ShowMessage(IProgressListener::PROGRESS_MSG_ERROR, csDxMessage);
        else
            Message::ShowError(csDxMessage);
        hr = E_FAIL;
    }

    return hr;
}


HRESULT CStudioApp::CheckFor16BitDisplay()
{
    HRESULT hr = S_OK;

    // 16 Bit display?
    if (1 != GetProfileInt(_T("Settings"), _T("DoNotDisplay16BitWarning"), 0)) {
        DEVMODE devMode;
        ZeroMemory(&devMode, sizeof DEVMODE);
        devMode.dmSize = sizeof DEVMODE;
        if (0 != ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode)) {
            if (devMode.dmBitsPerPel <= 16) {
                CWarning16BitA warn16Bit(NULL);
                if (IDYES == warn16Bit.DoModal()) {
                    ::ShellExecute(NULL, _T("open"), _T("rundll32.exe"), 
                        _T("shell32.dll,Control_RunDLL desk.cpl,,3"), NULL, SW_SHOW);

                    hr = E_FAIL;
                }
            }
        }
    }

    return hr;
}

HRESULT CStudioApp::GetVersion()
{
    HRESULT hr = S_OK;

    CString csOwnVersion;
    csOwnVersion.LoadString(IDS_VERSION);
    m_nVersionType = m_securityUtils.GetVersionType(csOwnVersion);

    if (m_nVersionType == VERSION_ERROR)
        hr = E_FAIL;

    return hr;
}

void CStudioApp::SetXtLanguageResource(CString &csApplication)
{
    // Save toolkit resource's language to temp dir and load it
    _TCHAR tszLanguageCode[128];
    KerberokHandler::GetLanguageCode(tszLanguageCode);
    _TCHAR pFileName[MAX_PATH];
    CLanguageSupport::SaveToolkitResourceLanguageFile(tszLanguageCode, csApplication, pFileName);
    XTPResourceManager()->SetResourceFile(pFileName);

    m_bXtResourcesOpened = true;
}

void CStudioApp::CheckLecturnityHome()
{
    CString csLecturnityHome;
    bool bLecturnityHome = ASSISTANT::GetLecturnityHome(csLecturnityHome);
    if (!ASSISTANT::LecturnityHomeEntryExist()) {
        Message::ShowWarning(IDS_LEC_HOME_WARNING);
        SelectLecturnityHome();
    }
    else if (_taccess(csLecturnityHome, 00) == -1) {
        Message::ShowWarning(IDS_NO_VALID_LECTURNITY_HOME);
        SelectLecturnityHome();
    }
    else {
        _TCHAR tszLanguageCode[128];
        bool ret = KerberokHandler::GetLanguageCode(tszLanguageCode);
        CString csLanguage = tszLanguageCode;
        if (csLanguage == "de") {
            // if there is a lecturnity home director with german sub directories,
            // without english sub directories, display a warning
            CString csGermanRecording = csLecturnityHome + _T("\\Aufzeichnungen");
            CString csRecording = csLecturnityHome + _T("\\Recordings");
            CString csGermanSource = csLecturnityHome + _T("\\Quelldokumente");
            CString csSource = csLecturnityHome + _T("\\Source Documents");
            if ((_taccess(csSource, 00) == -1 && _taccess(csGermanSource, 00) != -1) &&
                (_taccess(csRecording, 00) == -1 && _taccess(csGermanRecording, 00) != -1)) {

                Message::ShowWarning(IDS_DIRECTORYNAME_CHANGED);

                if (_taccess(csSource, 00) == -1) {
                    BOOL result = CreateDirectory(csSource, NULL);
                    if (result == 0) {
                        CString csMessage;
                        csMessage.Format(IDS_ERROR_CREATE_DIR, csSource);
                        Message::Show(csMessage);
                    }
                }

                CString csCdProjects = csLecturnityHome + _T("\\CD Projects");
                if (_taccess(csCdProjects, 00) == -1) {
                    BOOL result = CreateDirectory(csCdProjects, NULL);
                    if (result == 0) {
                        CString csMessage;
                        csMessage.Format(IDS_ERROR_CREATE_DIR, csSource);
                        Message::Show(csMessage);
                    }
                }

                if (_taccess(csRecording, 00) == -1) {
                    BOOL result = CreateDirectory(csRecording, NULL);
                    if (result == 0) {
                        CString csMessage;
                        csMessage.Format(IDS_ERROR_CREATE_DIR, csSource);
                        Message::Show(csMessage);
                    }
                }
            }
        }
    }
}

void CStudioApp::SelectLecturnityHome()
{
    CString csLecturnityHome;
    bool bResult = ASSISTANT::GetLecturnityHome(csLecturnityHome);

    if (!bResult)
        csLecturnityHome = _T("");

    CXFolderDialog fDlg(csLecturnityHome);
    int nOsVesrion = 0;

    fDlg.SetOsVersion((CXFolderDialog::XFILEDIALOG_OS_VERSION)nOsVesrion);

    CString csTitle;
    csTitle.LoadString(IDS_SELECT_LEC_HOME);
    fDlg.SetTitle(csTitle);

    bool bSuccess = true;
    //   CString csPath = "No Folder Selected!";
    if(fDlg.DoModal() == IDOK)
        m_csLecturnityHome = fDlg.GetPath();
    else if (!csLecturnityHome.IsEmpty())
        m_csLecturnityHome = csLecturnityHome;
    else
        bSuccess = false;

    if (bSuccess) {
        if (_taccess(m_csLecturnityHome, 00) == -1) {
            BOOL result = CreateDirectory(m_csLecturnityHome, NULL);
            if (result == 0) {
                CString csMessage;
                csMessage.Format(IDS_ERROR_CREATE_DIR, m_csLecturnityHome);
                Message::Show(csMessage);
                return;
            }
        }
        CString csPath;

        csPath = m_csLecturnityHome + _T("\\Source Documents");
        if (_taccess(csPath, 00) == -1) {
            BOOL result = CreateDirectory(csPath, NULL);
            if (result == 0) {
                CString csMessage;
                csMessage.Format(IDS_ERROR_CREATE_DIR, csPath);
                Message::Show(csMessage);
                return;
            }
        }

        csPath = m_csLecturnityHome + _T("\\CD Projects");
        if (_taccess(csPath, 00) == -1) {
            BOOL result = CreateDirectory(csPath, NULL);
            if (result == 0)
            {
                CString csMessage;
                csMessage.Format(IDS_ERROR_CREATE_DIR, csPath);
                Message::Show(csMessage);
                return;
            }
        }

        csPath = m_csLecturnityHome + _T("\\Recordings");
        if (_taccess(csPath, 00) == -1) {
            BOOL result = CreateDirectory(csPath, NULL);
            if (result == 0) {
                CString csMessage;
                csMessage.Format(IDS_ERROR_CREATE_DIR, csPath);
                Message::Show(csMessage);
                return;
            }
        }

        if (ASSISTANT::Project::active != NULL)
        {
            CString csRecordPath = m_csLecturnityHome;
            csRecordPath += _T("\\Recordings");
            ASSISTANT::Project::active->SetRecordPath(csRecordPath);
        }

        ASSISTANT::SetLecturnityHome(m_csLecturnityHome);
    }

}

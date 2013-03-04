// Studio.h : Hauptheaderdatei für die Studio-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
#error "\"stdafx.h\" vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"       // Hauptsymbole
#include "StudioDocTemplate.h"
#include "SplashScreen.h"
#include "DptCommunicator.h"

//#include "LiveContextDirectDlg.h"
class CLiveContextDirectDlg; // avoid unnecessary warnings/fuss
#include "securityutils.h"  // lsutl32.dll
#include "SplashScreen2.h"
//#include "WaitSplashScreen.h"
#include "LiveContextStartScreen.h"
#include "LiveContext.h"

#define EXIT_CODE_LC_TASK_CANCEL            3 // not used here: Publisher uses it
#define EXIT_CODE_LC_NO_EDIT_DONE           2
#define EXIT_CODE_LC_NO_RECORDING_DONE      1
#define EXIT_CODE_LC_SUCCESS                0
#define EXIT_CODE_LC_GENERIC_ERROR          -1 // shouldn't be used here (Java Publisher does it)
#define EXIT_CODE_LC_PUBLISHER_START        -2
#define EXIT_CODE_LC_WRONG_PARAMETERS       -3
#define EXIT_CODE_LC_MISSING_PROFILE        -4
#define EXIT_CODE_LC_ERROR_PUBLISH_START    -5
#define EXIT_CODE_LC_ERROR_AUDIO_INIT       -6
#define EXIT_CODE_LC_ERROR_CREATE_MANUAL    -7
#define EXIT_CODE_LC_ERROR_CREATE_DEMODOC   -8


class CStudioApp : public CWinApp
{
public:
    static bool Finalize(CString &csLrdPath, IProgressListener *pListener, 
        HANDLE hExportFinish, bool bWithOpenProgressDialog);
    static bool Publish(CString &csLrdPath, 
        bool bDirectMode, bool bSelectFormat, int iSelectedFormat,
        CString& csTargetPath);

    static bool IsPowerTrainerMode() { return s_bIsPowerTrainerMode; }
    static CString &GetPowerTrainerProjectName() { return s_csPowerTrainerProjectName; }

    static CLiveContext &GetLiveContext() {return s_cLiveContext;}
    static bool IsLiveContextMode(){return s_cLiveContext.IsLiveContextMode();}
    static CString GetLiveContextRecordFile(){return s_cLiveContext.GetInput();}
    static CString GetLiveContextPipeName(){return s_cLiveContext.GetPipeName();}
    static bool IsLiveContextPreview(){return s_cLiveContext.IsPreview();}

    static void SetLiveContextExitCode(int iExitCode);
    static void QuitLiveContextWithCode(int iExitCode);

    static bool CreateManual(CString &csLrdPath, CString &csWordFilename, CString &csDocumentTitle);
    static bool CreateDemoDocument(CString &csLrdPath, CString &csDocFilename);

    static UINT PublishProgressThread(LPVOID pParam);


    CStudioApp();
    ~CStudioApp();


    // Überschreibungen
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual void LoadStdProfileSettings(UINT nMaxMRU);
    virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual void WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);

	afx_msg void OnAppAbout();
	afx_msg void OnUpdateAppAbout(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()

public:
    UINT GetCurrentMode() {return m_nCurrentMode;}
    void CloseEditorMode();
    UINT GetCurrentColorScheme(){return m_nCurrentColorScheme;}
    void SetCurrentColorScheme(UINT nColorScheme);
    bool SaveLoadedDocuments();



private:

    void OnFileNew();

    void ChangeApplicationName(CString &csApplication);
    void ChangeRegistryKey();
    void ChangeProfileDirectory();
    HRESULT CheckDirectXVersion(CDptCommunicator *pDptCommunicator);
    HRESULT CheckFor16BitDisplay();
    HRESULT GetVersion();
    //HRESULT ShowSplashScreen();
   // void PositionMainWindow();
    void SetXtLanguageResource(CString &csApplication);
    void CheckLecturnityHome();
    void SelectLecturnityHome();

    //Variables
public:
    enum {
        MODE_UNDEFINED = 0,
        MODE_ASSISTANT = 1,
        MODE_EDITOR = 2
    };

    static const CString s_sCurrentLepVersion;

    // Even if the current format is newer it is still compatible with this old version:
    static const CString s_sCompatibleLepVersion; 

protected:

private:
    static int s_iExitCodeToBe;

    static bool s_bIsPowerTrainerMode;
    static CString s_csPowerTrainerProjectName;

    /* This instance will be populated by CStudioCommandLineInfo call. 
     * Will also be passed to avgrabber project, for further usage.
     */
    static CLiveContext s_cLiveContext;

    ULONG_PTR m_gdiplusToken;
    DWORD m_dwDirectXVersion;
    int m_nVersionType;
    CSecurityUtils m_securityUtils;

    CSplashScreen2 *m_pSplashScreen2;
    CLiveContextStartScreen *m_pLiveContextStartScreen;
    //CWaitSplashScreen *m_pWaitSplashScreen;

    CString m_csLecturnityHome;

    CStudioDocTemplate* m_pDocTemplateAssistant;
    CStudioDocTemplate* m_pDocTemplateEditor;

    UINT m_nCurrentMode;
    UINT m_nCurrentColorScheme;
    bool m_bReturnFromEditMode;

    bool m_bShowFormatDialog;
    CString m_csLrdFileName;

    bool m_bXtResourcesOpened;

    CDptCommunicator *m_pDptCommunicator;
};

extern CStudioApp theApp;
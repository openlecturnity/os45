// lsutl32.cpp : Legt die Initialisierungsroutinen für die DLL fest.
//

#include "stdafx.h"
#include "lsutl32.h"
#include "KeyGenerator.h"
#include "lutils.h" // for pipes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Entry point for invocation with rundll32.exe
void CALLBACK UpdateLicenseKey(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    CString csKey = lpszCmdLine;

    // -1: use the current language
    bool bKerberokSuccess = KerberokHandler::WriteKerberokAll(csKey, -1, true);

    LIo::MessagePipe pipe;
    HRESULT hrPipe = pipe.Init(_T("key-success-return"), false);
    if (SUCCEEDED(hrPipe)) {
        if (bKerberokSuccess)
            hrPipe = pipe.WriteMessage(_T("ok"));
        else
            hrPipe = pipe.WriteMessage(_T("failure"));
    }
}



/////////////////////////////////////////////////////////////////////////////
// CLsutl32App

BEGIN_MESSAGE_MAP(CLsutl32App, CWinApp)
	//{{AFX_MSG_MAP(CLsutl32App)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



CLsutl32App::CLsutl32App() {
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

CLsutl32App theApp;

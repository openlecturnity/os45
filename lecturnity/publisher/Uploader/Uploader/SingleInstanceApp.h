/////////////////////////////////////////////////////////////////////////////
// SingleInstanceApp.h
// Abstract:
//		SingleInstanceApp class implements functionality to make
//		single instance app.	
//
// (L) Copyleft Maxim Egorushkin. e-maxim@yandex.ru
// Thanks by e-mail would be enough to use it :)

#pragma once
#include <windows.h>

/////////////////////////////////////////////////////////////////////////////
// SingleInstanceApp
// Abstract:
//		Win32 API version.

class SingleInstanceApp {
public:
    // parameters:
    //		[in] pszUniqueString - string to give the file mapping 
    //		system unique name. GUID would fit the best.
    //		[out] uMessage - variable to register message to.
    //		This message will be posted to first app instance thread.
    //		So, intrested app can process this message to get the command line
    //		the second instance was tried to launch with.
    SingleInstanceApp(LPCTSTR pszUniqueString, UINT& uMessage);
    ~SingleInstanceApp();

    // returns:
    //		true if running the first instance
    bool AmITheFirst() const;

    // returns:
    //		The command line the second instance was tried to launch with.
    LPCTSTR GetSecondInstanceCmdLine() const;

private:
    bool _bAmITheFirst;
    HANDLE _hMapping;
    struct SharedData {
        DWORD dwThreadID;
        TCHAR pszCmdLine[0x800];
    }* _pSharedData;
};

/////////////////////////////////////////////////////////////////////////////

#ifdef __AFXWIN_H__

/////////////////////////////////////////////////////////////////////////////
// CSingleInstanceApp
// Abstract:
//		MFC friendly wrapper of SingleInstanceApp. Derive your application
//		class from it instead of CWinApp.

class CSingleInstanceApp : public CWinApp, public SingleInstanceApp {
public:
    CSingleInstanceApp(LPCTSTR pszUniqueString);

    // Override this to get notified when second instance was tried to launch.
    virtual void OnSecondInstance(UINT, LPARAM);

private:
    DECLARE_MESSAGE_MAP()
    static UINT sm_uMessage;
};

/////////////////////////////////////////////////////////////////////////////

#endif // __AFXWIN_H__
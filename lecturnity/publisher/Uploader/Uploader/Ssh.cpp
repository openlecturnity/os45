#include "stdafx.h"
#include "Ssh.h"

// is defined in puttylib.lib (PSCP.C)
extern "C" int pscp_main(int argc, char *argv[], HWND hWnd, int iDelete, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, int *piError, int iCheck, int* piExist);

UINT __cdecl MySShThread(LPVOID pParam) {
    SSHStruct *pSSH = (SSHStruct*)pParam;
    int err = pscp_main(pSSH->argc, pSSH->argv, pSSH->handle, pSSH->iDelete, pSSH->dProgress, pSSH->pbIsCancel, pSSH->pbIsThreadActive, pSSH->piError, pSSH->iCheck, pSSH->piExists);
    pSSH->pbIsThreadActive = false;
    return *pSSH->piError;
}

CSsh::CSsh(void) {
    m_iErrCode = 0;
    iArgc = 0;
}

CSsh::~CSsh(void) {
    for (int i = 0; i < 32; i++) {
        m_csArgc[i].Empty();
    }
}

BOOL CSsh::Upload(CString csSrcFileName, CString csDstFileName, TransferParam tp, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive /*= NULL*/, BOOL bQuit) {
    BOOL bResult = FALSE;
    iArgc = 0;
    *pbIsThreadActive = true;
    m_csArgc[iArgc++] = _T("Uploader.exe"); // application name
    m_csArgc[iArgc++] = (tp.iService == UPLOAD_SCP) ? _T("-scp") : _T("-sftp");
    m_csArgc[iArgc++] = _T("-2"); //ssh version
    m_csArgc[iArgc++] = _T("-4"); //ip version

    if (bQuit) {
        m_csArgc[iArgc++] = _T("-q"); // quit
    }

    m_csArgc[iArgc++] = _T("-l"); // username
    m_csArgc[iArgc++] = tp.csUsername; // 
    m_csArgc[iArgc++] = _T("-pw"); // password
    m_csArgc[iArgc++] = tp.csPassword;  
    m_csArgc[iArgc++] = _T("-P"); // port
    m_csArgc[iArgc++].Format(_T("%d"), tp.nPort);

    m_csArgc[iArgc++] = _T("-r"); 

    //source
    m_csArgc[iArgc++].Format(_T("%s"), csSrcFileName);
    // destination
    m_csArgc[iArgc++].Format(_T("%s:%s"), tp.csServer, csDstFileName);

    _TCHAR **atszParams = new _TCHAR*[32];
    for (int i=0;i < iArgc;i++) {
        LPCTSTR tszOneParam = m_csArgc[i];
        int iOneParamLen = (int)_tcslen(tszOneParam);
        atszParams[i] = new _TCHAR[iOneParamLen + 1];
        wcscpy_s(atszParams[i], iOneParamLen + 1, tszOneParam);
    }
    int iCheck;
    bResult = m_iErrCode = CallSsh(iArgc, atszParams, m_hWnd, dProgress, pbIsCancel, pbIsThreadActive, &iCheck, 0, 0);
    for (int i = 0 ; i < iArgc; i++) {   
        delete[] atszParams[i];
    }
    delete[] atszParams;
    *pbIsThreadActive = false;
    return (bResult == 0); 
}

BOOL CSsh::Download(CString csSrcFileName, CString csDstFileName, TransferParam tp, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, BOOL bQuit) {
    BOOL bResult = FALSE;
    iArgc = 0;
    *pbIsThreadActive = true;
    m_csArgc[iArgc++] = _T("Uploader.exe"); // application name
    m_csArgc[iArgc++] = (tp.iService == UPLOAD_SCP) ? _T("-scp") : _T("-sftp");
    m_csArgc[iArgc++] = _T("-2"); //ssh version
    m_csArgc[iArgc++] = _T("-4"); //ip version

    if (bQuit) {
        m_csArgc[iArgc++] = _T("-q"); // quit
    }

    m_csArgc[iArgc++] = _T("-l"); // username
    m_csArgc[iArgc++] = tp.csUsername; // 
    m_csArgc[iArgc++] = _T("-pw"); // password
    m_csArgc[iArgc++] = tp.csPassword;  
    m_csArgc[iArgc++] = _T("-P"); // port
    m_csArgc[iArgc++].Format(_T("%d"), tp.nPort);
    //source
    m_csArgc[iArgc++].Format(_T("%s:%s"), tp.csServer, csSrcFileName);
    // destination
    m_csArgc[iArgc++].Format(_T("%s"), csDstFileName);

    _TCHAR **atszParams = new _TCHAR*[32];
    for (int i=0;i < iArgc;i++) {
        LPCTSTR tszOneParam = m_csArgc[i];
        int iOneParamLen = (int)_tcslen(tszOneParam);
        atszParams[i] = new _TCHAR[iOneParamLen + 1];
        wcscpy_s(atszParams[i], iOneParamLen + 1, tszOneParam);
    }

    int iCheck;
    bResult = m_iErrCode = CallSsh(iArgc, atszParams, m_hWnd, dProgress, pbIsCancel, pbIsThreadActive, &iCheck, 0,0);
    for (int i = 0 ; i < iArgc; i++) {
        delete[] atszParams[i];
    }
    delete[] atszParams;
    *pbIsThreadActive = false;
    return (bResult == 0); 
}

BOOL CSsh::Delete(CString csSrcFileName, TransferParam tp, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, BOOL bQuit) {
    BOOL bResult = FALSE;
    *pbIsThreadActive = true;
    for (int k=0;k < csSrcFileName.GetLength();k++) {
        // wildcards not allowed
        if ((csSrcFileName[k] == _T('*')) || (csSrcFileName[k] == _T('?'))) {
            return FALSE;
        }
    }

    iArgc = 0;
    m_csArgc[iArgc++] = _T("Uploader.exe"); // application name
    m_csArgc[iArgc++] = (tp.iService == UPLOAD_SCP) ? _T("-scp") : _T("-sftp");
    m_csArgc[iArgc++] = _T("-2"); //ssh version
    m_csArgc[iArgc++] = _T("-4"); //ip version

    if (bQuit) {
        m_csArgc[iArgc++] = _T("-q"); // quit
    }

    m_csArgc[iArgc++] = _T("-l"); // username
    m_csArgc[iArgc++] = tp.csUsername; // 
    m_csArgc[iArgc++] = _T("-pw"); // password
    m_csArgc[iArgc++] = tp.csPassword;  
    m_csArgc[iArgc++] = _T("-P"); // port
    m_csArgc[iArgc++].Format(_T("%d"), tp.nPort);
    //source
    m_csArgc[iArgc++].Format(_T("%s:%s"), tp.csServer, csSrcFileName);
    // destination
    m_csArgc[iArgc++].Format(_T("%s"), csSrcFileName);

    _TCHAR **atszParams = new _TCHAR*[32];
    for (int i=0;i < iArgc;i++) {
        LPCTSTR tszOneParam = m_csArgc[i];
        int iOneParamLen = (int)_tcslen(tszOneParam);
        atszParams[i] = new _TCHAR[iOneParamLen + 1];
        wcscpy_s(atszParams[i], iOneParamLen + 1, tszOneParam);
    }

    int iCheck;
    bResult = m_iErrCode = CallSsh(iArgc, atszParams, m_hWnd, dProgress, pbIsCancel, pbIsThreadActive, &iCheck, 1,0);
    for (int i = 0 ; i < iArgc; i++) {
        delete[] atszParams[i];
    }
    delete[] atszParams;

    *pbIsThreadActive = false;
    return (bResult == 0); 
}

int CSsh::CallSsh(int argc, WCHAR** argv, HWND handle, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, int *piExists, int iDelete = 0, int iCheck = 0) {
    *pbIsThreadActive = true;
    // argv: Convert wchar_t** to char**
    char** szargv = new char*[argc]; 
    for (int i = 0; i < argc; ++i) {
        int nLen = ::WideCharToMultiByte(CP_ACP, 0, argv[i], -1, NULL, 0, NULL, NULL);
        szargv[i] = new char[nLen + 1];
        ::WideCharToMultiByte(CP_ACP, 0, argv[i], -1, szargv[i], nLen + 1, NULL, NULL);
        szargv[i][nLen] = 0;
    }

    int iResult = CallSsh(argc, szargv, handle, dProgress, pbIsCancel, pbIsThreadActive, piExists, iDelete, iCheck);

    for (int i = 0 ; i < iArgc; i++) {
        delete szargv[i];
    }
    delete[] szargv;
    szargv = NULL;

    *pbIsThreadActive = false;
    return iResult;
}

int CSsh::CallSsh(int argc, char** argv, HWND handle, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, int* piExists, int iDelete = 0, int iCheck = 0) {

    //    Now used directly from a lib
    int iResult = 1;
    *pbIsThreadActive = true;
    SSHStruct str;
    str.argc = argc;
    str.argv = argv;
    str.handle = handle;
    str.dProgress = dProgress;
    str.piError = &m_iErrCode;
    str.piExists = piExists;
    str.iDelete = iDelete;
    str.iCheck = iCheck;
    str.pbIsCancel = pbIsCancel;
    str.pbIsThreadActive = pbIsThreadActive;

    HANDLE hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MySShThread, &str, 0, NULL);
    ::WaitForSingleObject(hThread, INFINITE);
    //while (*pbIsThreadActive)
    // Sleep(10);
    hThread = NULL;
    iResult = m_iErrCode;

    return iResult;
}

CString CSsh::GetErrStr() {
    CString csErr;
    CString csText;

    if (m_iErrCode == 8 || m_iErrCode == WSAEACCES) {
        csErr.LoadString(IDS_WRONG_PASSWORD);
    } else {
        csText.LoadString(IDS_FTPERROR);
        csErr.Format(csText, m_iErrCode);
    }
#ifdef _UPLOADER_EXE
    MessageBox(NULL, csErr, _T("Uploader"), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND | MB_TOPMOST | MB_TASKMODAL);
#endif

    return csErr;
}

BOOL CSsh::CheckTarget(CString csServer, int iProtocol, int iPort, CString csUser, CString csPassword, CString csTargetDirOrFile, bool &bExists, bool* pbIsDir, bool* pbIsEmptyDir) {
    BOOL bResult = FALSE;
    iArgc = 0;
    bool bIsThreadActive = true;
    m_csArgc[iArgc++] = _T("Uploader.exe"); // application name
    m_csArgc[iArgc++] = (iProtocol == 1) ? _T("-scp") : _T("-sftp");
    m_csArgc[iArgc++] = _T("-2"); //ssh version
    m_csArgc[iArgc++] = _T("-4"); //ip version

    m_csArgc[iArgc++] = _T("-l"); // username
    m_csArgc[iArgc++] = csUser; // 
    m_csArgc[iArgc++] = _T("-pw"); // password
    m_csArgc[iArgc++] = csPassword;  
    m_csArgc[iArgc++] = _T("-P"); // port
    m_csArgc[iArgc++].Format(_T("%d"), iPort);
    //source
    m_csArgc[iArgc++].Format(_T("%s:%s"), csServer, csTargetDirOrFile);
    // destination
    m_csArgc[iArgc++].Format(_T("%s"), csTargetDirOrFile);   

    _TCHAR **atszParams = new _TCHAR*[32];
    for (int i=0;i < iArgc;i++) {
        LPCTSTR tszOneParam = m_csArgc[i];
        int iOneParamLen = (int)_tcslen(tszOneParam);
        atszParams[i] = new _TCHAR[iOneParamLen + 1];
        wcscpy_s(atszParams[i], iOneParamLen + 1, tszOneParam);
    }
    int iExists = 0;
    double dProgress;
    bool pbIsCancel = false;
    bResult = m_iErrCode = CallSsh(iArgc, atszParams, m_hWnd, &dProgress, &pbIsCancel, &bIsThreadActive, &iExists, 0,1);
    for (int i = 0 ; i < iArgc; i++) {   
        delete[] atszParams[i];
    }
    delete[] atszParams;

    if (iExists == 0) {
        bExists = false;
        *pbIsDir = false;
        *pbIsEmptyDir = false;
    } else {
        bExists = true;
        if (iExists == 1) {
            *pbIsDir = false;
        } else {
            *pbIsDir = true;
            if (iExists == 2) {
                *pbIsEmptyDir = true;
            } else {
                *pbIsEmptyDir = false;
            }
        }
    }
    bIsThreadActive = false;
    return (bResult == 0); 
}
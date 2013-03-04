#pragma once
#include "Uploader.h"
struct SSHStruct {
    int argc;
    char** argv;
    HWND handle;
    double *dProgress;
    bool *pbIsCancel;
    bool *pbIsThreadActive;
    int* piExists;
    int* piError;
    int iDelete;
    int iCheck;
};

class CSsh {
private:
    int iArgc;
    CString m_csArgc[32];
    HWND m_hWnd;
    int m_iErrCode;
    typedef int (__stdcall *SSHPROC)(int, char**, HWND, int, double*,int,int*); 
    int CallSsh(int argc, char** argv, HWND handle, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, int* piExists, int iDelete, int iCheck);
    int CallSsh(int argc, WCHAR** argv, HWND handle, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, int* piExists, int iDelete, int iCheck);
public:
    CSsh(void);
    ~CSsh(void);
    BOOL Upload(CString csSrcFileName, CString csDstFileName, TransferParam tp, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, BOOL bQuit = FALSE);
    BOOL Download(CString csSrcFileName, CString csDstFileName, TransferParam tp, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, BOOL bQuit = FALSE);
    BOOL Delete(CString csSrcFileName, TransferParam tp, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, BOOL bQuit = FALSE);
    CString GetErrStr();
    int GetErrCode(){return m_iErrCode;};
    BOOL CheckTarget(CString csServer, int iProtocol, int iPort, CString csUser, CString csPassword, CString csTargetDirOrFile, bool &bExists, bool* pbIsDir, bool* pbIsEmptyDir);
};

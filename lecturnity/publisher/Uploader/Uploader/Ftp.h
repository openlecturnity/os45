// Ftp.h: interface for the CFtp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTP_H__3F9C2A64_214B_4BCB_919C_F7335F28894B__INCLUDED_)
#define AFX_FTP_H__3F9C2A64_214B_4BCB_919C_F7335F28894B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <WinInet.h>
#include "afxinet.h"

class CFtp {
private:
    CInternetSession m_Session;
    CFtpConnection *m_FtpConnection;
    CInternetFile* m_FtpFile;

    CString GetInetError(DWORD dwErrorNo);
    CString m_errStr;
    int m_iErrCode;

    CFile File;

public:
    CFtp();
    virtual ~CFtp();
    BOOL Open(CString csServer, int nPort, CString csUsername, CString csPassword);
    void Close();
    BOOL Download(CString csSrcFileName, CString csDstFileName, CString csDirectory);
    BOOL Upload(CString csSrcFileName, CString csDstFileName, CString csDirectory);
    BOOL Upload(CString csSrcFileName,  CString csDstFileName, CString csDirectory, CProgressCtrl &ctrlProgress);
    BOOL Upload(CString csSrcFileName,  CString csDstFileName, CString csDirectory, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive);
    BOOL Delete(CString csDstFileName, CString csDirectory, bool bIsSubDir = false);

    CString ErrorMessage(UINT uId);
    CString ErrorMessage(UINT uId, CString csParam1);
    CString ErrorMessage(UINT uId, CString csParam1, CString csParam2);
    void CleanUp();
    CString GetErrString(){return m_errStr;};
    int GetErrCode(){return m_iErrCode;};
    BOOL CheckTarget(CString csTargetDirOrFile, bool &bExists, bool* pbIsDir, bool* pbIsEmptyDir);
    BOOL CreateSubDirectory(CString csDirectory, CString csSubDirName);
};

#endif // !defined(AFX_FTP_H__3F9C2A64_214B_4BCB_919C_F7335F28894B__INCLUDED_)

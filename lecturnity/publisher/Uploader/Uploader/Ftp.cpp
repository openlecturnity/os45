// Ftp.cpp: implementation of the CFtp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ftp.h"
#include "Uploader.h"
#include "LanguageSupport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtp::CFtp() {
    m_iErrCode = 0;
    m_FtpConnection = NULL;
    m_FtpFile = NULL;
}

CFtp::~CFtp() {
    Close();

    // close file
    if (m_FtpFile != NULL) {
        m_FtpFile->Close();
        delete m_FtpFile;
        m_FtpFile = NULL;
    }
    if (m_FtpConnection != NULL) {
        delete m_FtpConnection;
    }
}

BOOL CFtp::Open(CString csServer, int nPort, CString csUsername, CString csPassword) {
    BOOL bResult = FALSE;
    CString csCurrentDirectory;
    try {
        m_FtpConnection = m_Session.GetFtpConnection(csServer, csUsername, csPassword, nPort);     
        if (m_FtpConnection != NULL) {
            bResult = m_FtpConnection->GetCurrentDirectory(csCurrentDirectory);
        }
    } catch (CInternetException *exc) {
        m_errStr = GetInetError(exc->m_dwError);
        exc->Delete();
    }   
    return bResult;
}

void CFtp::Close() {
    if (m_FtpFile != NULL) {
        m_FtpFile->Close();
        delete m_FtpFile;
        m_FtpFile = NULL;
    }
}

CString CFtp::GetInetError(DWORD dwErrorNo) {
    m_iErrCode = dwErrorNo;
    HRESULT hr = S_OK;
    LCID langId = 0;
    hr = CLanguageSupport::GetLanguageId(langId);
    if (hr != S_OK) {
        CString csTitle;
        csTitle.LoadString(IDS_ERROR);
        CString csMessage;
        if (hr == E_LS_WRONG_REGISTRY) {
            csMessage.LoadString(IDS_ERROR_INSTALLATION);
        } else if (hr == E_LS_LANGUAGE_NOT_SUPPORTED) {
            csMessage.LoadString(IDS_ERROR_LANGUAGE);
        } else if (hr == E_LS_TREAD_LOCALE) {
            csMessage.LoadString(IDS_ERROR_SET_LANGUAGE);
        }
#ifdef _UPLOADER_EXE
        MessageBox(NULL, csMessage, csTitle, MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_TOPMOST | MB_TASKMODAL);
#endif
    }

    CString csErrorMessage, csFormat;
    LPTSTR tszMessage;

    HANDLE hModule = ::GetModuleHandle(_T("WinINet.dll"));

    if (hModule != NULL) { 
        /* got module */
        if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_HMODULE,
            (LPCVOID)hModule,
            dwErrorNo,
            langId,//0, // default language
            (LPTSTR)&tszMessage,
            0,
            NULL) != 0) {
         /* format succeeded */
            csErrorMessage.Format(_T("%s"), tszMessage);
            csErrorMessage.Replace(_T("\r\n"), _T(""));
            ::LocalFree(tszMessage);
#ifdef _UPLOADER_EXE
            AfxMessageBox(csErrorMessage, MB_OK + MB_ICONSTOP + MB_SETFOREGROUND + MB_TOPMOST + MB_TASKMODAL);
#endif
            return csErrorMessage;
        } /* format succeeded */
    } /* got module */

    csFormat.LoadString(IDS_FTPERROR);
    csErrorMessage.Format(csFormat, dwErrorNo);
#ifdef _UPLOADER_EXE
    AfxMessageBox(csErrorMessage, MB_OK + MB_ICONSTOP + MB_SETFOREGROUND + MB_TOPMOST + MB_TASKMODAL);
#endif
    return csErrorMessage;
} // getInetError

BOOL CFtp::Upload(CString csSrcFileName, CString csDstFileName, CString csDirectory) {
    BOOL bResult = FALSE;
    try {
        bResult = m_FtpConnection->SetCurrentDirectory(csDirectory);
        if (bResult) {
            bResult = m_FtpConnection->PutFile(csSrcFileName, csDstFileName);
            if (!bResult) {
                m_errStr = ErrorMessage(IDS_ERRUPLOADXML, csSrcFileName, csDstFileName);
            }
        } else {
            m_errStr = ErrorMessage(IDS_ERRDIR1, csDirectory);
        }
    } catch (CInternetException *exc) {
        m_errStr = GetInetError(exc->m_dwError);
        exc->Delete();
    }   
    return bResult;
}
BOOL CFtp::CreateSubDirectory(CString csDirectory, CString csSubDirName) {
    BOOL bResult = FALSE;
    try {
        bResult = m_FtpConnection->SetCurrentDirectory(csDirectory);
        if (bResult) {
            bResult = m_FtpConnection->CreateDirectory(csSubDirName);
            if (!bResult) {
                m_errStr = ErrorMessage(IDS_ERR_FTP_CREATE_FILE, csSubDirName);
            }
        } else {
            m_iErrCode = 1;
            m_errStr = ErrorMessage(IDS_ERRDIR2, csDirectory);
        }
    } catch (CInternetException *exc) {
        m_errStr = GetInetError(exc->m_dwError);
        exc->Delete();
    }   
    return bResult;
}

#define BLOCKSIZE 4096
BOOL CFtp::Upload(CString csSrcFileName, CString csDstFileName, CString csDirectory, CProgressCtrl &m_prgProgress)  {
    // TODO: Add your control notification handler code here
    BOOL bResult = FALSE;
    BYTE buffer[BLOCKSIZE];
    DWORD dwRead;

    DWORD m_dwMediaFileLength = 0;
    bResult = File.Open(csSrcFileName,  CFile::modeRead, NULL);
    if (bResult) {
        // get file length
        m_dwMediaFileLength = (DWORD)File.GetLength();
        // Read in 4096-byte blocks,
        m_prgProgress.SetPos(0);
        m_prgProgress.SetRange32(0, (int)File.GetLength()/BLOCKSIZE);
        m_prgProgress.SetStep(BLOCKSIZE);

        try {
            bResult = m_FtpConnection->SetCurrentDirectory(csDirectory);
            if (bResult) {
                try {
                    m_FtpFile = m_FtpConnection->OpenFile(csDstFileName, GENERIC_WRITE);
                } catch (CException *ex) {
                    m_iErrCode = 98765;
                    m_errStr = ErrorMessage(IDS_ERR_FTP_CREATE_FILE, csSrcFileName);
                    bResult = FALSE;
                }
                if (m_FtpFile != NULL) {
                    // do the upload using BLOCSIZE bloks
                    do {
                        dwRead = File.Read(buffer, BLOCKSIZE);
                        if (dwRead > 0) {
                            // no error return ?
                            m_FtpFile->Write(buffer, dwRead);
                            m_prgProgress.SetPos(m_prgProgress.GetPos() + 1);
                        }
                    } while (dwRead > 0);

                    m_FtpFile->Close();
                    delete m_FtpFile;
                    m_FtpFile = NULL;

                    bResult = TRUE;
                }
                else {
                    m_errStr = ErrorMessage(IDS_ERR_FTP_CREATE_FILE, csSrcFileName);
                }
            } else {
                m_iErrCode = 1;
                m_errStr = ErrorMessage(IDS_ERRDIR2, csDirectory);
            }
        } catch (CInternetException *exc) {
            m_errStr = GetInetError(exc->m_dwError);
            exc->Delete();
            bResult = FALSE;
        }   
        File.Close();
    }
    else {
        m_errStr = ErrorMessage(IDS_ERROPENFILE, csSrcFileName);
    }
    return bResult;
}

BOOL CFtp::Upload(CString csSrcFileName, CString csDstFileName, CString csDirectory, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive) {
    // TODO: Add your control notification handler code here
    *pbIsThreadActive = true;
    BOOL bResult = FALSE;
    BYTE buffer[BLOCKSIZE];
    DWORD dwRead;

    DWORD m_dwMediaFileLength = 0;
    bResult = File.Open(csSrcFileName,  CFile::modeRead, NULL);
    if (bResult) {
        // get file length
        m_dwMediaFileLength = (DWORD)File.GetLength();
        // Read in 4096-byte blocks,
        try {
            bResult = m_FtpConnection->SetCurrentDirectory(csDirectory);
            if (bResult){
                try {
                    m_FtpFile = m_FtpConnection->OpenFile(csDstFileName, GENERIC_WRITE);
                } catch (CException *ex) {
                    m_iErrCode = 98765;
                    m_errStr = ErrorMessage(IDS_ERR_FTP_CREATE_FILE, csSrcFileName);
                    bResult = FALSE;
                } 
                if (m_FtpFile != NULL) {
                    // do the upload using BLOCSIZE bloks
                    do {
                        dwRead = File.Read(buffer, BLOCKSIZE);
                        if (dwRead > 0) {
                            // no error return ?
                            m_FtpFile->Write(buffer, dwRead);
                            *dProgress = *dProgress + (dwRead / (m_dwMediaFileLength * 1.0)) * 0.95;
                        }
                    } while ((dwRead > 0) && (*pbIsCancel == false));

                    m_FtpFile->Close();
                    delete m_FtpFile;
                    m_FtpFile = NULL;

                    bResult = TRUE;
                } else {
                    m_errStr = ErrorMessage(IDS_ERR_FTP_CREATE_FILE, csSrcFileName);
                }
            } else {
                m_iErrCode = 1;
                m_errStr = ErrorMessage(IDS_ERRDIR2, csDirectory);
            }
        } catch (CInternetException *exc) {
            m_errStr = GetInetError(exc->m_dwError);
            exc->Delete();
            bResult = FALSE;
        }   

        File.Close();
    } else {
        m_errStr = ErrorMessage(IDS_ERROPENFILE, csSrcFileName);
    }
    *pbIsThreadActive = false;
    return bResult;
}

BOOL CFtp::Download(CString csSrcFileName, CString csDstFileName, CString csDirectory) {
    BOOL bResult = FALSE;
    try {
        bResult = m_FtpConnection->SetCurrentDirectory(csDirectory);
        if (bResult) {
            bResult = m_FtpConnection->GetFile(csSrcFileName, csDstFileName, FALSE);
            if (!bResult){
                m_errStr = ErrorMessage(IDS_ERRUPLOADXML, csSrcFileName, csDstFileName);
            }
        } else {
            m_errStr = ErrorMessage(IDS_ERRDIR1, csDirectory);
        }
    } catch (CInternetException *exc) {
        m_errStr = GetInetError(exc->m_dwError);
        exc->Delete();
    }   
    return bResult;
}

BOOL CFtp::Delete(CString csDstFileName, CString csDirectory, bool bIsSubDir) {
    BOOL bResult = FALSE;
    try {
        bResult = m_FtpConnection->SetCurrentDirectory(csDirectory);
        if (bResult) {
            if (!bIsSubDir) {
                bResult = m_FtpConnection->Remove(csDstFileName);
            } else {
                CString csSub = csDirectory + csDstFileName;    
                bResult = m_FtpConnection->RemoveDirectory(csSub/*csDirectory*/);
            }
        } else {
            m_errStr = ErrorMessage(IDS_ERRDIR1, csDirectory);
        }
    } catch (CInternetException *Exception) {
        m_errStr = GetInetError(Exception->m_dwError);
        Exception->Delete();
    }   
    return bResult;
}

void CFtp::CleanUp() {
    File.Close();
    Close();
}

CString CFtp::ErrorMessage(UINT uId) {
    CString csMessage;
    csMessage.LoadString(uId);
#ifdef _UPLOADER_EXE
    MessageBox(NULL, csMessage, _T("Uploader"), MB_OK + MB_ICONSTOP + MB_SETFOREGROUND + MB_TOPMOST + MB_TASKMODAL);
#endif
    return csMessage;
}

CString CFtp::ErrorMessage(UINT uId, CString csParam1) {
    CString csMessage, csFormat;
    csFormat.LoadString(uId);
    csMessage.Format(csFormat, csParam1);
#ifdef _UPLOADER_EXE
    MessageBox(NULL, csMessage, _T("Uploader"), MB_OK + MB_ICONSTOP + MB_SETFOREGROUND + MB_TOPMOST + MB_TASKMODAL);
#endif
    return csMessage;
}

CString CFtp::ErrorMessage(UINT uId, CString csParam1, CString csParam2) {
    CString csMessage, csFormat;
    csFormat.LoadString(uId);
    csMessage.Format(csFormat, csParam1, csParam2);
#ifdef _UPLOADER_EXE
    MessageBox(NULL, csMessage, _T("Uploader"), MB_OK + MB_ICONSTOP + MB_SETFOREGROUND + MB_TOPMOST + MB_TASKMODAL);
#endif
    return csMessage;
}

BOOL CFtp::CheckTarget(CString csTargetDirOrFile, bool &bExists, bool* pbIsDir, bool* pbIsEmptyDir) {
    bExists = false;
    if (csTargetDirOrFile.IsEmpty()) {
        //should be loaded from resources
        m_errStr = _T("Target is empty");
        return FALSE;
    }
    int iResult, iPosFwd, iPosBack, iPos;
    CString csBaseDir, csTarget;
    CFtpFileFind ftpFind(m_FtpConnection);
    iPosFwd = csTargetDirOrFile.ReverseFind(_T('/'));
    iPosBack = csTargetDirOrFile.ReverseFind(_T('\\'));
    if (iPosFwd > iPosBack) {
        iPos = iPosFwd;
    } else {
        iPos = iPosBack;
    }
    csBaseDir = csTargetDirOrFile.Left(iPos);
    csTarget = csTargetDirOrFile.Right(csTargetDirOrFile.GetLength() - iPos - 1);
    if (!csBaseDir.IsEmpty()) {
        if (!m_FtpConnection->SetCurrentDirectory(csBaseDir)) {
            m_errStr.LoadString(IDS_ERRDIR2);
            return FALSE;
        }
    }
    if (csTarget.IsEmpty()) {
        *pbIsDir = true;
        iResult = ftpFind.FindFile();
        if (!iResult) {
            if (GetLastError() == ERROR_NO_MORE_FILES) {
                *pbIsEmptyDir = true;
                bExists = true;
                return TRUE;
            }
            //should be loaded from resources
            m_errStr = _T("can not check directory");
            return FALSE;
        } else {
            *pbIsEmptyDir = false;
            bExists = true;
            return TRUE;
        }
    } else {
        iResult = ftpFind.FindFile(csTarget);
        if (iResult) {
            *pbIsDir = false;
            bExists = true;
            return TRUE;
        } else {
            iResult = m_FtpConnection->SetCurrentDirectory(csTarget);
            if (iResult) {
                *pbIsDir = true;
                iResult = ftpFind.FindFile();
                if (!iResult) {
                    if (GetLastError() == ERROR_NO_MORE_FILES) {
                        *pbIsEmptyDir = true;
                        bExists = true;
                        return TRUE;
                    }
                    //should be loaded from resources
                    m_errStr = _T("can not check directory");
                    return FALSE;
                } else {
                    *pbIsEmptyDir = false;
                    bExists = true;
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}
// Uploader.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Uploader.h"
#include "PasswordDlg.h"
#include "TransferConfigDlg.h"
#include "MainDlg.h"
#include <wininet.h>
#include "LanguageSupport.h"
#include "TransferingContent.h"
#include "WaitDlg.h"
#include "GeneralSettings.h"
#include "SingleInstanceApp.h"
#include "CmdLineInfo.h"
#include "Crypto.h"
#include "UploaderUtils.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


// CUploaderApp

BEGIN_MESSAGE_MAP(CUploaderApp, CWinApp)
END_MESSAGE_MAP()


// CUploaderApp construction

CUploaderApp::CUploaderApp() : CSingleInstanceApp(TEXT("{49E4CD26-9489-4474-91B0-2A30F6CA3097}")) {
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}


// The one and only CUploaderApp object

CUploaderApp theApp;


// CUploaderApp initialization

BOOL CUploaderApp::InitInstance() {
    AfxOleInit();
    CWinApp::InitInstance();
    ::CoInitialize(NULL);

    m_csTmpDirectory.Empty();
    int n = ::GetTempPath(MAX_PATH, m_tstrTmpDirectory);
    if (n != 0) {
        m_csTmpDirectory = CString(m_tstrTmpDirectory, n);
    }

#ifdef _UPLOADER_EXE
    if (!AmITheFirst()) { 
        return FALSE; 
    }

    CCmdLineInfo cmdInfo;
    CString cline = ::GetCommandLine();
    ParseCommandLine(cmdInfo);

    CMainDlg main;
    if (cmdInfo.m_dwFlags & CMD_SETTINGS) {
        main.ShowPodcastSettings();
    } else if(cmdInfo.m_dwFlags & CMD_TRANSFER) {
        main.Podcast(cmdInfo.m_csMediaFileName, true);
    } else if(cmdInfo.m_dwFlags & CMD_YTTRANSFER) {
        main.ShowYtMainDlg();
    } else {
        main.DoModal();
    }

#endif

    HRESULT hr = S_OK;
    hr = CLanguageSupport::SetThreadLanguage();
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

        MessageBox(NULL, csMessage, csTitle, MB_OK | MB_TOPMOST);
    }

    return TRUE;
}

CString CUploaderApp::GetTempDirectory() {
    return m_csTmpDirectory;
}

CUploader::CUploader(void) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    m_csPwd1 = _T("");
    m_csPwd2 = _T("");
    m_csServer1 = _T("");
    m_csServer2 = _T("");
    m_csUser1 = _T("");
    m_csUser2 = _T("");
    m_csSlsServer = _T("");
    m_nCount = 0;
    m_bIsNewPwd = false; 
    m_dTempProgress = NULL;
    m_hParentHwnd = NULL;
    m_csNewTargetDir = _T("");
    m_pbIsCancel = new bool();
    *m_pbIsCancel = false;

    m_pbIsThreadActive = new bool();
    *m_pbIsThreadActive = false;
    m_piCurrFile = NULL;

    m_bIsSlsSession = false;
    m_csSlsUser = _T("");
    m_csSlsPass = _T("");
    m_csTempSlsUser = _T("");
    m_csTempSlsPass = _T("");
    m_csTempSlsId = _T("");
    m_bLocalCall = false;
    m_csYtAuthToken = _T("");
    m_csYtAuthUser = _T("");
    m_bYtIsNewUserOrPassword = false;
    m_csYtNewUser = _T("");
    m_csYtNewPassword = _T("");
    m_bSlsServerWasEmpty = false;
    m_csNewSlsServer = _T("");
    InitYtCategories();
}

CUploader::~CUploader(void) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (m_dTempProgress != NULL) {
        delete m_dTempProgress;
    }
}

void CUploader::InitYtCategories() {
    m_acsYtCategories.Add(_T("Education"));
    m_acsYtCategories.Add(_T("Film"));
    m_acsYtCategories.Add(_T("Autos"));
    m_acsYtCategories.Add(_T("Music"));
    m_acsYtCategories.Add(_T("Animals"));
    m_acsYtCategories.Add(_T("Sports"));
    m_acsYtCategories.Add(_T("Travel"));
    m_acsYtCategories.Add(_T("Shortmov"));
    m_acsYtCategories.Add(_T("Videoblog"));
    m_acsYtCategories.Add(_T("Games"));
    m_acsYtCategories.Add(_T("Comedy"));
    m_acsYtCategories.Add(_T("People"));
    m_acsYtCategories.Add(_T("News"));
    m_acsYtCategories.Add(_T("Entertainment"));
    m_acsYtCategories.Add(_T("Howto"));
    m_acsYtCategories.Add(_T("Nonprofit"));
    m_acsYtCategories.Add(_T("Tech"));
}

void CUploader::SetErrorString(int nId, CString csStr /*= NULL*/) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString csErr;
    csErr.LoadString(nId);

    m_csError.Format(csErr, csStr);

}

CString CUploader::SetTargDir(CString csDirName) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString csRet = csDirName;
    int iPosF = csDirName.ReverseFind(_T('\\'));
    int iPosB = csDirName.ReverseFind(_T('/'));
    int iPos = (iPosF > iPosB)? iPosF : iPosB;

    if ((iPos != -1) && (iPos != csDirName.GetLength() - 1)) {
        csRet += _T('/');  
    }

    return csRet;
}

HRESULT CUploader::CreateSubDirOnServer(CString csRoot, TransferParam tp) {
    HRESULT hr = S_OK;
    CArray<CString, CString>aSubDirs;
    int iPos = 0;
    CString csNewDir = tp.csDirectory;
    CString csSubdir = csNewDir.Right(csNewDir.GetLength() - csRoot.GetLength());
    CString csTok;
    csTok = csSubdir.Tokenize(_T("/"), iPos);
    while (csTok != _T("")) {
        aSubDirs.Add(csTok);
        csTok = csSubdir.Tokenize(_T("/"), iPos);
    }
    CString csCurrDir = csRoot;
    for (int i = 0; i < aSubDirs.GetSize(); i++) {
        bool bIsDir = false;
        bool bIsDirEmpty = false;
        CString csNewSubDir = csCurrDir + aSubDirs.GetAt(i) + _T("/");
        CString pass;
        CCrypto crypt;
        pass = crypt.Encode(tp.csPassword);
        CheckTargetExists(tp.csServer, tp.iService, tp.nPort, tp.csUsername, pass, csNewSubDir, &bIsDir, &bIsDirEmpty);
        if (!bIsDir) {
            CTransferingContent& rTContent = CTransferingContent::GetInstance();
            tp.csDirectory = csCurrDir;
            if (!rTContent.CreateSubDirectory(tp, aSubDirs.GetAt(i))) {
                hr = ERR_CANNOT_CREATE_FILE_DIR;
                m_csError = rTContent.GetErrStr();
                break;
            }
        }
        csCurrDir = csNewSubDir;
    }
    tp.csDirectory = csNewDir;
    aSubDirs.RemoveAll();
    return hr;
}

UINT CUploader::SendSlidestarRequest(UINT iReqType) {
    UINT res = ERR_NO_ERROR;
    CString csHeaders = _T("Content-Type: application/x-www-form-urlencoded");
    //CString csQueryParam = _T("login=soarerm13@yahoo.com&pass=1234");
    CString csQueryParam = _T("login=") + m_csSlsUser + _T("&pass=") + m_csSlsPass;
    CString csReq = _T("");
    if (iReqType == SLIDESTAR_REQ_ACQUIRE) {
        csReq = _T("lecturnity/acquire_login?");
    } else {
        csReq = _T("lecturnity/complete?");
        csQueryParam += (_T("&id=") + m_csTempSlsId);
    }

    DWORD flags;
    if (!InternetGetConnectedState(&flags, NULL)) {
        return ERR_NETWORK_ERROR;
    }

    CInternetSession session;
    try {
        CHttpConnection* pConnection = session.GetHttpConnection(m_csSlsServer/*_T("slidestar.de")*/);
        if (pConnection != NULL) {
            CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, csReq + csQueryParam);
            if (pFile != NULL) {
                BOOL result = pFile->SendRequest(csHeaders);//strHeaders/*, (LPVOID)(LPCTSTR)strFormData, strFormData.GetLength()*/);
                char aa;
                CString csResponse = _T("");
                while (pFile->Read(&aa,1) == 1) {
                    csResponse += aa;
                }
                pFile->Close();

                CString csErrParam = _T("\"msg\": ");
                CString csErr = GetResponseParamValue(csResponse, csErrParam);
                if (csErr.CompareNoCase(_T("success")) == 0) {
                    if (iReqType == SLIDESTAR_REQ_ACQUIRE) {
                        CString csIdParam = _T("\"id\": ");
                        CString csLoginParam = _T("\"login\": ");
                        CString csPassParam = _T("\"pass\": ");
                        m_csTempSlsId = GetResponseParamValue(csResponse, csIdParam);
                        m_csTempSlsUser = GetResponseParamValue(csResponse, csLoginParam);
                        m_csTempSlsPass = GetResponseParamValue(csResponse, csPassParam);
                    }
                } else {
                    res = ERR_AUTHENTICATION_FAILED;
                }
            } else {
                res = ERR_CANNOT_CREATE_FILE_DIR;
            }
        } else {
            res = ERR_SERVER_NOT_REACHABLE;
        }
    } catch(CInternetException* pEx) {
        DWORD iErrCode = pEx->m_dwError;
        pEx->Delete();
        if (iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || iErrCode == ERROR_INTERNET_LOGIN_FAILURE) {
            return ERR_AUTHENTICATION_FAILED;
        } else if (iErrCode == ERROR_INTERNET_INVALID_URL || iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED) {
            return ERR_SERVER_DOES_NOT_EXIST;
        } else if (iErrCode == ERROR_INTERNET_CANNOT_CONNECT) {
            return ERR_SERVER_NOT_REACHABLE;
        } else if (iErrCode == 105 || iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || iErrCode == ERROR_INTERNET_CONNECTION_RESET || iErrCode == ERROR_INTERNET_TIMEOUT) {
            return ERR_NETWORK_ERROR;
        } else if (iErrCode == 98765) {
            return ERR_CANNOT_OVERWRITE_FILE;
        } else {
            return ERR_GENERIC_ERROR;/*bResult*/;
        }
    }
    return res;
}

CString CUploader::GetResponseParamValue(CString csResponse, CString csParam) {
    CString csRet = _T("");

    int pos = csResponse.Find(csParam);
    if (pos != -1) {
        if (csParam == _T("\"id\": ")) {
            int iEndPos = csResponse.Find(_T(","), pos);
            if (iEndPos == -1) {
                return csRet;
            }
            int iStartPos = pos + csParam.GetLength();
            csRet = csResponse.Mid(iStartPos, iEndPos - iStartPos);
        } else {
            int iStartComma = csResponse.Find(_T("\""), pos + csParam.GetLength());
            if (iStartComma == -1) {
                return csRet;
            }
            int iEndComma = csResponse.Find(_T("\""), iStartComma+1);
            if (iEndComma == -1) {
                return csRet;
            }
            csRet = csResponse.Mid(iStartComma + 1, iEndComma - iStartComma -1);
        }
    }
    return csRet;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::CheckYtUpload(CString csUser, CString csPassword, CString csRecLength) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _tprintf(_T("!!!Upl in CUploader::CheckYtUpload method user = %s - length = %s\n"), csUser, csRecLength);
    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    CString csPass = _T("");
    if (!csPassword.IsEmpty()) {
        CCrypto crypt;
        csPass = crypt.Decode(csPassword);
    }
    if (csPass.IsEmpty() || csUser.IsEmpty()) {
        CWnd *pParent = CWnd::FromHandle(m_hParentHwnd);
        CPasswordDlg dlg(csUser, _T("YouTube"), pParent);
        if (dlg.DoModal() == IDOK) {
            csPass = dlg.GetPassword();
            csUser = dlg.GetUsername();
            m_bYtIsNewUserOrPassword = true;
            m_csYtNewUser = csUser;
            m_csYtNewPassword = csPass;
        } else {
            return S_FALSE;
        }
    }
    UINT iRecLen = GetRecordLengthSeconds(csRecLength);
    if (iRecLen > YT_MAX_MOVIE_LEN) {
        CString csErr;
        csErr.LoadString(IDS_YT_ERR_TOO_LONG);
        m_csError = csErr;
        return ERR_GENERIC_ERROR;
    }
    CString csAuthToken = _T("");
    CString csAuthUser = _T("");
    _tprintf(_T("!!!Upl in CUploader::CheckYtUpload before Authentify user = %s \n"), csUser);
    HRESULT res = rTContent.YtAuthentify(csUser, csPass, csAuthToken, csAuthUser);
    if (res == S_OK) {
        m_csYtAuthToken = csAuthToken;
        m_csYtAuthUser = csAuthUser;
    } else {
        m_csError = rTContent.GetErrStr();
    }
    return res;
}

UINT CUploader::GetRecordLengthSeconds(CString csRecLength) {
    UINT iSeconds = 0;
    int iPos = 0;
    CArray<CString, CString>acsLen;
    CString csTok = csRecLength.Tokenize(_T(":"), iPos);
    while(csTok != _T("")) {
        acsLen.Add(csTok);
        csTok = csRecLength.Tokenize(_T(":"), iPos);
    }
    int iSize = acsLen.GetSize();
    for (int i = 0; i < iSize; i++) {
        int val = _wtoi(acsLen.GetAt(i));

        if ((iSize - 1 - i) > 0) {
            val *= 60 * (iSize - 1 - i);
        }

        iSeconds += val ;
    }
    return iSeconds;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::CheckUpload(CString csServer, int iProtocol, int iPort, 
                                                                  CString csUser, CString csPassword, CString csTargetDir) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    static int iNumOfReq = 0;
    TransferParam tp;
    tp.csServer = csServer;
    tp.iService = iProtocol;
    tp.nPort = iPort;
    tp.csUsername = csUser;
    tp.csPassword = _T("");
    if (!csPassword.IsEmpty()) {
        CCrypto crypt;
        tp.csPassword = crypt.Decode(csPassword);
    }
    tp.csDirectory = SetTargDir(csTargetDir);

    if (m_nCount == 0) {
        m_csServer1 = tp.csServer;
        m_csUser1 = tp.csUsername;
    } else {
        m_csServer2 = tp.csServer;
        m_csUser2 = tp.csUsername;
    }

    //BUGFIX bug5541
    if (m_bIsSlsSession && csServer.IsEmpty()) {
        CWnd *pParent = CWnd::FromHandle(m_hParentHwnd);
        CUploaderUtils uu;
        int iRes = uu.ShowSlidestarSettingsDialog(pParent, tp.csServer, tp.csUsername, tp.csPassword);
        if (iRes == IDOK) {
            uu.GetSlidestarTransferSettings(tp.csServer, tp.csUsername, tp.csPassword);
            if (!tp.csServer.IsEmpty()) {
                m_bSlsServerWasEmpty = true;
                m_csNewSlsServer = tp.csServer;
            }

            if (!tp.csPassword.IsEmpty()) {
                m_bIsNewPwd = true;
            }
        } else {
            return ERR_SERVER_NOT_REACHABLE;
        }


    }

    if (csPassword == _T("") && tp.csPassword.IsEmpty()) {
        if (m_nCount == 1 && m_csServer1 == m_csServer2 && m_csUser1 == m_csUser2) {
            m_csPwd2 = m_csPwd1;
            tp.csPassword = m_csPwd2;  
        } else {
            CWnd *pParent = CWnd::FromHandle(m_hParentHwnd);
            CPasswordDlg dlg(csUser, csServer, pParent);
            if (dlg.DoModal() == IDOK) {
                tp.csPassword = dlg.GetPassword();
                tp.csUsername = dlg.GetUsername();
                m_bIsNewPwd = true;
            } else if(!m_bIsSlsSession) {
                return S_FALSE;
            }
        }
    }

    if (m_nCount == 0) {
        m_csPwd1 = tp.csPassword;
        m_csUser1 = tp.csUsername;
    } else {
        m_csPwd2 = tp.csPassword;
        m_csUser2 = tp.csUsername;
    }

    m_nCount++;

    m_csNewTargetDir = tp.csDirectory;

    //if((tp.csServer.CompareNoCase(_T("slidestar.de")) == 0) && (!m_bLocalCall) && (iNumOfReq == 0))
    if (m_bIsSlsSession) {
        iNumOfReq++;
        m_csSlsUser = tp.csUsername;
        m_csSlsPass = tp.csPassword;
        m_csSlsServer = tp.csServer;
        UINT res = SendSlidestarRequest(SLIDESTAR_REQ_ACQUIRE);

        if (res == ERR_NO_ERROR) {
            CSocket socket;
            socket.Create();
            if (!socket.Connect(tp.csServer, tp.nPort)) {
                socket.Close();
                return ERR_SERVER_NOT_REACHABLE;
            }
            socket.Close();
        }

        if (res != ERR_NO_ERROR) {
            return res;
        }
        //m_bIsSlsSession = true;
    }

    if(m_bIsSlsSession) {
        tp.csUsername = m_csTempSlsUser;
        tp.csPassword = m_csTempSlsPass;
        tp.iService = UPLOAD_SFTP;
        //      tp.csDirectory = _T("/home/") + m_csTempSlsUser + tp.csDirectory;
        tp.csDirectory = _T("./");
    }

    CFile dummy;
    char ch = 'a';

    TCHAR aPath[MAX_PATH];
    ::GetTempPath(MAX_PATH, aPath); 

    CTime t = CTime::GetCurrentTime();

    CString csTime;
    csTime.Format(_T("dummy_%d_%d_%d_%d_%d_%d.txt"), t.GetDay(), t.GetMonth(), t.GetYear(), t.GetHour(), t.GetMinute(), t.GetSecond());
    CString csDummyFileName(aPath);
    csDummyFileName += _T("\\") + csTime;//_T("\\my_dummy.txt");
    if (dummy.Open(csDummyFileName, CFile::modeCreate | CFile::modeWrite)) {
        dummy.Write(&ch, 1);
        dummy.Close();
    } else {
        SetErrorString(IDS_ERROPENFILE, csDummyFileName);     
        return E_FAIL;
    }

    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    m_dTempProgress = new double();
    bool bCancel = false;
    bool bIsThreadActive = false;

    HRESULT res = rTContent.Upload(tp, csDummyFileName, csTime, m_dTempProgress, &bCancel, &bIsThreadActive); 
    if (res == ERR_NO_ERROR) {
        rTContent.Upload(tp, NULL, csTime, m_dTempProgress, &bCancel, &bIsThreadActive, true);
    } else {
        int iErrCode = rTContent.GetErrCode();
        if (iErrCode == 1 || ((iErrCode == 53) && (tp.iService == UPLOAD_SFTP))) {
            bool bIsDir = false;
            bool bIsEmptyDir = false;
            CString pass;
            CCrypto crypt;
            pass = crypt.Encode(tp.csPassword);
            if (CheckTargetExists(tp.csServer, tp.iService, tp.nPort, tp.csUsername, pass, tp.csDirectory, &bIsDir, &bIsEmptyDir) != S_OK) {
                res = CreateSubDirOnServer(_T("/"), tp);
            } else {
                m_csError = rTContent.GetErrStr();
            }
        } else {
            m_csError = rTContent.GetErrStr();//_T("an error occured"); //to be modified
        }
    }
    try {
        CFile::Remove(csDummyFileName);//Rename( csFileName, csNewName );
    } catch(CFileException* pEx ) {
#ifdef _DEBUG
        afxDump << "File " << csDummyFileName << " not found, cause = "
            << pEx->m_cause << "\n";
#endif
        pEx->Delete();
    }

    if (res != S_OK) {
        m_nCount--;
    }

    return res;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::StartYtUpload(CString csUser, CString csPassword, CString csTitle, CString csDescription, CString csKeywords, CString csCategory, int iPrivacy, CArray<CString , CString > &aFilenames) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CCrypto crypt;
    if (!m_bIsSession) {
        return E_FAIL;
    }
    YtTransferParam ytp;
    if (m_bYtIsNewUserOrPassword == true) {
        ytp.csUsername = m_csYtNewUser;
        ytp.csPassword = m_csYtNewPassword;
        m_bYtIsNewUserOrPassword = false;
        m_csYtNewUser.Empty();
        m_csYtNewPassword.Empty();
    } else {
        ytp.csUsername = csUser;
        ytp.csPassword = _T("");
        if (!csPassword.IsEmpty()) {
            ytp.csPassword = crypt.Decode(csPassword);
        }
    }
    ytp.csTitle = csTitle;
    ytp.csDescription = csDescription;
    ytp.csKeywords = csKeywords;
    ytp.csCategory = csCategory;
    ytp.iPrivacy = iPrivacy;

    //CYTTransferDlg dlg;
    CUploaderUtils uu;
    CWnd *pParent = CWnd::FromHandle(m_hParentHwnd);
    int iResId = uu.ShowYouTubeSettingsDialog(pParent, ytp.csUsername, ytp.csPassword, ytp.csTitle, ytp.csDescription,
        ytp.csKeywords, ytp.csCategory, ytp.iPrivacy, true);
    if (iResId == IDOK) {
        uu.GetYouTubeTransferSettings(ytp.csUsername, ytp.csPassword, ytp.csTitle, 
            ytp.csDescription, ytp.csKeywords, ytp.csCategory, ytp.iPrivacy);
    } else if (iResId == IDCANCEL) {
        SetErrorString(IDS_YTU_WRONG_PARAMS);
        return ERR_GENERIC_ERROR;
    }
    /*bool bCheckReqiredParams = dlg.CheckYtRequiredParameters(ytp.csTitle, ytp.csDescription, ytp.csKeywords);
    if(!bCheckReqiredParams)
    {
    CString csYtCheckError = dlg.GetYtCheckRequiredErrorMessage();

    MessageBox(NULL, csYtCheckError, _T("Uploader"), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND | MB_TOPMOST | MB_TASKMODAL);
    if(dlg.DoModal() == IDOK)
    {
    ytp = dlg.GetYtTransferParams();
    }
    else
    {
    SetErrorString(IDS_YTU_WRONG_PARAMS);
    return ERR_GENERIC_ERROR;
    }

    }*/
    _tprintf(_T("!!!UPL category = %s\n"), ytp.csCategory);
    if (*m_pbIsCancel == true) {
        return S_OK;
    }

    if (aFilenames.GetSize() <= 0) {
        SetErrorString(IDS_ERR_NO_FILES);
        return E_FAIL;
    }

    m_dProgress = new double();
    *m_dProgress = 0.0;

    ULONGLONG lTotalSize = 0;
    CArray<ULONGLONG, ULONGLONG> aFileLen;
    for (int i = 0; i < aFilenames.GetSize(); i++) {
        CFile file;
        if (file.Open(aFilenames.GetAt(i), CFile::modeRead)) {
            lTotalSize += file.GetLength();
            aFileLen.Add(file.GetLength());
            file.Close();
        } else {
            SetErrorString(IDS_ERROPENFILE, aFilenames.GetAt(i));
            return E_FAIL;
        }
    }

    if (lTotalSize > YT_MAX_MOVIE_SIZE) {
        CString csErr;
        csErr.LoadString(IDS_YT_ERR_TOO_BIG);
        m_csError = csErr;
        return ERR_GENERIC_ERROR;
    }

    m_adProcStart.RemoveAll();
    for (int i = 0; i < aFileLen.GetSize(); i++) {
        double dProc = (100.0 * (double)aFileLen.GetAt(i)) / (double) lTotalSize;
        m_adProcStart.Add(dProc);
    }

    if (m_piCurrFile != NULL) {
        delete m_piCurrFile;
        m_piCurrFile = NULL;
    }
    m_piCurrFile = new int();
    m_dCurrProg = 0;

    if (*m_pbIsCancel == true) {
        return S_OK;
    }

    if (!m_csYtAuthToken.IsEmpty() && !m_csYtAuthUser.IsEmpty() && m_csYtAuthUser.CompareNoCase(ytp.csUsername) != 0) {
        ytp.csUsername = m_csYtAuthUser;
    }

    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    rTContent.SetParentHwnd(m_hParentHwnd);
    rTContent.MyStartYtUpload(ytp, aFilenames.GetAt(0), m_csYtAuthToken, m_dProgress, m_piCurrFile, m_pbIsCancel, m_pbIsThreadActive);
    m_csYtAuthUser.Empty();
    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::StartUpload(CString csServer, int iProtocol, int iPort, 
                                                                  CString csUser, CString csPassword, CString csTargetDir, 
                                                                  CArray<CString , CString > &aFilenames) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CCrypto crypt;
    if (!m_bIsSession) {
        return E_FAIL;
    }

    TransferParam tp;
    tp.csServer = csServer;
    tp.iService = iProtocol;
    tp.nPort = iPort;
    tp.csUsername = csUser;
    if (m_bIsNewPwd) {
        tp.csPassword = m_csPwd1;
    } else {
        tp.csPassword = _T("");
        if (!csPassword.IsEmpty()) {
            tp.csPassword = crypt.Decode(csPassword);
        }
    }
    tp.csDirectory = SetTargDir(csTargetDir);//csTargetDir;

    if (*m_pbIsCancel == true) {
        return S_OK;
    }

    if (aFilenames.GetSize() <= 0) {
        SetErrorString(IDS_ERR_NO_FILES);
        return E_FAIL;
    }

    if (tp.iService != UPLOAD_FTP) {
        CMainDlg mainDlg;
        for (int i = 0 ; i < aFilenames.GetSize(); i++) {
            CString csFilename = aFilenames.GetAt(i);
            if (tp.iService == UPLOAD_SFTP) {
                int iPos = csFilename.ReverseFind(_T('\\'));
                if (iPos != -1) {
                    csFilename = csFilename.Right(csFilename.GetLength() - iPos - 1);
                }
                csFilename = tp.csDirectory + csFilename;
            }

            if (!mainDlg.CheckUrlFileName(csFilename/*aFilenames.GetAt(i)*/, tp.iService)) {
                SetErrorString(IDS_ERR_MP4URL);
                return E_FAIL;
            }
        }
    }

    if (m_csNewTargetDir.IsEmpty() || (tp.csDirectory.Find(m_csNewTargetDir) == -1)) {
        m_bLocalCall = true;
        CheckUpload(csServer, iProtocol, iPort, csUser, csPassword, csTargetDir);
        m_bLocalCall = false;
    } else {
        int iRes = m_csNewTargetDir.CompareNoCase(tp.csDirectory);
        if (iRes < 0) {
            HRESULT hr = CreateSubDirOnServer(m_csNewTargetDir, tp);
            if (hr != S_OK) {
                return hr;
            }
        }
    }
    m_dProgress = new double();
    *m_dProgress = 0.0;

    ULONGLONG lTotalSize = 0;
    CArray<ULONGLONG, ULONGLONG> aFileLen;
    for (int i = 0; i < aFilenames.GetSize(); i++) {
        CFile file;
        if (file.Open(aFilenames.GetAt(i), CFile::modeRead)) {
            lTotalSize += file.GetLength();
            aFileLen.Add(file.GetLength());
            file.Close();
        } else {
            SetErrorString(IDS_ERROPENFILE, aFilenames.GetAt(i));
            return E_FAIL;
        }
    }

    m_adProcStart.RemoveAll();
    for (int i = 0; i < aFileLen.GetSize(); i++) {
        double dProc = (100.0 * (double)aFileLen.GetAt(i)) / (double) lTotalSize;
        m_adProcStart.Add(dProc);
    }

    if (m_piCurrFile != NULL) {
        delete m_piCurrFile;
        m_piCurrFile = NULL;
    }
    m_piCurrFile = new int();
    m_dCurrProg = 0;

    if (*m_pbIsCancel == true) {
        return S_OK;
    }
    
    if (m_bIsSlsSession) {
        if (m_bSlsServerWasEmpty) {
            tp.csServer = m_csNewSlsServer;
        }
        tp.csUsername = m_csTempSlsUser;
        tp.csPassword = m_csTempSlsPass;
        tp.iService = UPLOAD_SFTP;
        //      tp.csDirectory = _T("/home/") + m_csTempSlsUser + tp.csDirectory;
        tp.csDirectory = _T("./");
    }

    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    rTContent.SetParentHwnd(m_hParentHwnd);
    rTContent.MyStartUpload(tp,aFilenames,m_dProgress, m_piCurrFile, m_pbIsCancel, m_pbIsThreadActive);
    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::StartPodcast(
    CString csChannelServer, int iChannelProtocol, int iChannelPort, 
    CString csChannelUser, CString csChannelPassword, CString csChannelTargetDir,
    CString csMediaServer, int iMediaProtocol, int iMediaPort, 
    CString csMediaUser, CString csMediaPassword, CString csMediaTargetDir,
    CString csMediaDirectoryUrl, CString csChannelFileUrl,
    CArray<CString , CString > &aFilenames, CString csTitle, CString csAuthor) {

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CCrypto crypt;
    TransferParam MediaTP;
    MediaTP.csServer = csMediaServer;
    MediaTP.iService = iMediaProtocol;
    MediaTP.nPort = iMediaPort;
    MediaTP.csUsername = csMediaUser;
    MediaTP.csDirectory = csMediaTargetDir;
    MediaTP.csUrl = csMediaDirectoryUrl;
    TransferParam ChannelTP;
    ChannelTP.csServer = csChannelServer;
    ChannelTP.iService = iChannelProtocol;
    ChannelTP.nPort = iChannelPort;
    ChannelTP.csUsername = csChannelUser;
    ChannelTP.csDirectory = csChannelTargetDir;
    ChannelTP.csUrl = csChannelFileUrl;

    if (m_bIsNewPwd) {
        ChannelTP.csPassword = m_csPwd1;
        ChannelTP.csUsername = m_csUser1;
        MediaTP.csPassword = m_csPwd2;
        MediaTP.csUsername = m_csUser2;
    } else {
        ChannelTP.csPassword = _T("");
        if (!csChannelPassword.IsEmpty()) {
            ChannelTP.csPassword = crypt.Decode(csChannelPassword);
        }
        MediaTP.csPassword = _T("");
        if (!csMediaPassword.IsEmpty()) {
            MediaTP.csPassword = crypt.Decode(csMediaPassword);
        }
    }
    if (*m_pbIsCancel == true) {
        return S_OK;
    }
    m_dProgress = new double();
    *m_dProgress = 0.0;

    if (aFilenames.GetSize() <= 0) {
        SetErrorString(IDS_ERR_NO_FILES);
        return E_FAIL;
    }

    ULONGLONG lTotalSize = 0;
    CArray<ULONGLONG, ULONGLONG> aFileLen;
    for (int i = 0; i < aFilenames.GetSize(); i++) {
        CFile file;
        if (file.Open(aFilenames.GetAt(i), CFile::modeRead)) {
            lTotalSize += file.GetLength();
            aFileLen.Add(file.GetLength());
            file.Close();
        } else {
            SetErrorString(IDS_ERROPENFILE, aFilenames.GetAt(i));
            return E_FAIL;
        }
    }

    CArray<double, double> aProc;
    m_adProcStart.RemoveAll();
    for (int i = 0; i < aFileLen.GetSize(); i++) {
        double dProc = (100.0 * (double)aFileLen.GetAt(i)) / (double) lTotalSize;
        m_adProcStart.Add(dProc);
    }

    if (m_piCurrFile != NULL) {
        delete m_piCurrFile;
        m_piCurrFile = NULL;
    }
    m_piCurrFile = new int();
    m_dCurrProg = 0;
    if (*m_pbIsCancel == true) {
        return S_OK;
    }
    CTransferingContent& rTContent = CTransferingContent::GetInstance();

    rTContent.MyStartPodcast(MediaTP, ChannelTP, aFilenames, csTitle, csAuthor, m_dProgress, m_piCurrFile, m_pbIsCancel, m_pbIsThreadActive);
    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::GetProgress(double *pdProgress) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    m_csError = rTContent.GetErrStr();
    if (!m_csError.IsEmpty()) {
        int iErrCode = rTContent.GetErrCode();
        //ftp error codes
        if (iErrCode == 87 || iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || iErrCode == ERROR_INTERNET_LOGIN_FAILURE)
            return ERR_AUTHENTICATION_FAILED;
        else if (iErrCode == ERROR_INTERNET_INVALID_URL || iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
            return ERR_SERVER_DOES_NOT_EXIST;
        else if (iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
            return ERR_SERVER_NOT_REACHABLE;
        else if (iErrCode == 105 || iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || iErrCode == ERROR_INTERNET_CONNECTION_RESET || iErrCode == ERROR_FTP_DROPPED || iErrCode == ERROR_INTERNET_TIMEOUT)
            return ERR_NETWORK_ERROR;
        else if (iErrCode == 98765)
            return ERR_CANNOT_OVERWRITE_FILE;
        else
            return ERR_GENERIC_ERROR;/*bResult*/;

        //scp error codes
        if (iErrCode == 105 || iErrCode == 6 || iErrCode == 38 || iErrCode == 48 || iErrCode == 64 || iErrCode == 71)
            return ERR_NETWORK_ERROR;
        else if (iErrCode == WSAEADDRNOTAVAIL || iErrCode == WSAENETDOWN || iErrCode == WSAENETUNREACH || iErrCode == WSAENETRESET)
            return ERR_NETWORK_ERROR;
        else if (iErrCode == 22)
            return ERR_SERVER_DOES_NOT_EXIST;
        else if (iErrCode == WSAECONNREFUSED || iErrCode == WSAEHOSTDOWN || iErrCode == WSAEHOSTUNREACH)
            return ERR_SERVER_NOT_REACHABLE;
        else if (iErrCode == WSAEACCES || iErrCode == 8)
            return ERR_AUTHENTICATION_FAILED;
        else if (iErrCode == 2 || iErrCode == 3)
            return ERR_CANNOT_CREATE_FILE_DIR;
        else
            return ERR_GENERIC_ERROR;
    }

    double dCurrVal = 0.0;
    for (int i = 0 ; i < *m_piCurrFile; i++) {
        dCurrVal += m_adProcStart.GetAt(i);//(*m_piCurrFile);
    }
    *pdProgress =dCurrVal + *m_dProgress * m_adProcStart.GetAt(*m_piCurrFile) ;//a + b;//m_adProcStart.GetAt(*m_piCurrFile) +  *m_dProgress * m_adProcStart.GetAt(*m_piCurrFile+1) ; 
    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::ShowTransferConfigDialog(CString csServer, int iProtocol, int iPort, 
                                                                               CString csUser, CString csPassword, CString csTargetDir) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CCrypto crypt;
    m_tp.csServer = csServer;
    m_tp.iService = iProtocol;
    m_tp.nPort = iPort;
    m_tp.csUsername = csUser;
    m_tp.csPassword = _T("");
    if (!csPassword.IsEmpty()) {
        m_tp.csPassword = crypt.Decode(csPassword);
    }
    m_tp.csDirectory = csTargetDir;
    if (m_tp.csPassword.IsEmpty()) {
        m_tp.bSavePassword = false;
    } else {
        m_tp.bSavePassword = true;
    }

    CWnd *pParent = CWnd::FromHandle(m_hParentHwnd);
    if (m_bIsSlsSession) {
        CUploaderUtils uu;
        int iRes = uu.ShowSlidestarSettingsDialog(pParent, m_tp.csServer, m_tp.csUsername, m_tp.csPassword);
        if (iRes == IDOK) {
            uu.GetSlidestarTransferSettings(m_tp.csServer, m_tp.csUsername, m_tp.csPassword);
            m_tp.iService = UPLOAD_SFTP;
            m_tp.nPort = 22;
            m_tp.csDirectory = _T("/");
            return S_OK;
        }
    } else {
        CTransferConfigDlg tcDlg(0, m_tp, pParent, m_bIsSlsSession);
        if (tcDlg.DoModal() == IDOK) {
            m_tp = tcDlg.GetTransferParam();
            return S_OK;
        }
    }

    return S_FALSE; // Cancel selected
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::ShowYtTransferConfigDialog(CString csUser, CString csPassword, CString csTitle, CString csDescription, CString csKeywords, CString csCategory, int iPrivacy) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CCrypto crypt;
    m_ytTp.csUsername = csUser;
    m_ytTp.csPassword = _T("");
    if (!csPassword.IsEmpty()) {
        m_ytTp.csPassword = crypt.Decode(csPassword);
    }
    m_ytTp.csTitle = csTitle;
    m_ytTp.csDescription = csDescription;
    m_ytTp.csKeywords = csKeywords;
    /*int iCat = 0;
    if(iCategory >=0 && iCategory < m_acsYtCategories.GetSize())
    iCat = iCategory;
    m_ytTp.iCategory = iCat;*/
    CString csYtCat = csCategory;
    if (csYtCat.IsEmpty()) {
        csYtCat = _T("Education");
    }
    m_ytTp.csCategory = csYtCat;
    m_ytTp.iPrivacy = iPrivacy;

    HRESULT res = S_FALSE;
    CWnd *pParent = CWnd::FromHandle(m_hParentHwnd);
    //CYTTransferDlg* pYtTDld = new CYTTransferDlg(m_ytTp, m_acsYtCategories, pParent);

    CUploaderUtils uu;  
    int iResId = uu.ShowYouTubeSettingsDialog(pParent, m_ytTp.csUsername, m_ytTp.csPassword, m_ytTp.csTitle,
        m_ytTp.csDescription, m_ytTp.csKeywords, m_ytTp.csCategory, m_ytTp.iPrivacy);
    /*CYouTubeSettings* pdlg = new CYouTubeSettings(); 
    pdlg->DoModal();*/
    _tprintf(_T("!!!UPL dupa de CYouTubeSettings\n"));
    if(iResId == IDOK) {
        uu.GetYouTubeTransferSettings(m_ytTp.csUsername, m_ytTp.csPassword, m_ytTp.csTitle, 
            m_ytTp.csDescription, m_ytTp.csKeywords, m_ytTp.csCategory, m_ytTp.iPrivacy);
        res = S_OK;
    }

    /*if(pYtTDld->DoModal() == IDOK)
    {
    m_ytTp = pYtTDld->GetYtTransferParams();
    res = S_OK;
    }
    if(pYtTDld != NULL)
    delete (pYtTDld);*/
    return res;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::GetTransferData(CString &csServer, int *piProtocol, int *piPort, 
                                                                      CString &csUser, CString &csPassword, CString &csTargetDir) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CCrypto crypt;
    csServer = m_tp.csServer;
    *piProtocol = m_tp.iService;
    *piPort = m_tp.nPort;
    csUser = m_tp.csUsername;
    csPassword = crypt.Encode(m_tp.csPassword);
    csTargetDir = m_tp.csDirectory;

    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::GetYtTransferData(CString &csUser, CString &csPassword, CString &csTitle, CString &csDescription, CString &csKeywords, CString &csCategory, int *iPrivacy) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CCrypto crypt;
    csUser = m_ytTp.csUsername;
    csPassword = crypt.Encode(m_ytTp.csPassword);
    csTitle = m_ytTp.csTitle;
    csDescription = m_ytTp.csDescription;
    csKeywords = m_ytTp.csKeywords;
    csCategory = m_ytTp.csCategory;
    *iPrivacy = m_ytTp.iPrivacy;
    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::ShowPodcastChannelConfigDialog(
    CString csPodcastChannelName, CString csPodcastChannelFileUrl, CString csPodcastMediaDirUrl, 
    CString csChannelServer, int iChannelProtocol, int iChannelPort, CString csChannelUser, 
    CString csChannelPassword, CString csChannelTargetDir, 
    CString csMediaServer, int iMediaProtocol, int iMediaPort, CString csMediaUser, 
    CString csMediaPassword, CString csMediaTargetDir) {

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CCrypto crypt;
    TransferParam ctp;
    ctp.csServer = csChannelServer;
    ctp.iService = iChannelProtocol;
    ctp.nPort = iChannelPort;
    ctp.csUsername = csChannelUser;
    ctp.csPassword = _T("");
    if (!csChannelPassword.IsEmpty()) {
        ctp.csPassword = crypt.Decode(csChannelPassword);
    }
    if (ctp.csPassword.IsEmpty()) {
        ctp.bSavePassword = false;
    } else {
        ctp.bSavePassword = true;
    }
    ctp.csDirectory = csChannelTargetDir;
    ctp.csUrl = csPodcastChannelFileUrl;
    TransferParam mtp;
    mtp.csServer = csMediaServer;
    mtp.iService = iMediaProtocol;
    mtp.nPort = iMediaPort;
    mtp.csUsername = csMediaUser;
    mtp.csPassword = _T("");
    if (!csMediaPassword.IsEmpty()) {
        mtp.csPassword = crypt.Decode(csMediaPassword);
    }
    if (mtp.csPassword.IsEmpty()) {
        mtp.bSavePassword = false;
    } else {
        mtp.bSavePassword = true;
    }
    mtp.csDirectory = csMediaTargetDir;
    mtp.csUrl = csPodcastMediaDirUrl;


    HRESULT res = S_FALSE;
    TransferMetadata tm;

    m_csPodcastChannelName = csPodcastChannelName;

    CWnd *pParent = CWnd::FromHandle(m_hParentHwnd);
    CGeneralSettings *dlg;
    if (csPodcastChannelFileUrl == _T("")) {
        dlg = new CGeneralSettings(pParent);
    } else {
        dlg = new CGeneralSettings(csPodcastChannelName, ctp, mtp, tm, pParent);
    }

    if (dlg->DoModal() == IDOK) {
        if (dlg->IsTempChannelSelected()) {
            m_tcChannel = dlg->GetTemporaryChannel();
            m_tcMedia = dlg->GetTemporaryMedia();

        } else {
            m_tcChannel = dlg->GetActiveChannelData();
            m_tcMedia = dlg->GetActiveMediaData();
        }

        if (m_tcChannel.csServer == m_tcMedia.csServer && m_tcChannel.csUsername == m_tcMedia.csUsername) {
            if (!m_tcChannel.bSavePassword && m_tcMedia.bSavePassword) {
                m_tcChannel.csPassword = m_tcMedia.csPassword;
            } else {
                if (m_tcChannel.bSavePassword && !m_tcMedia.bSavePassword) {
                    m_tcMedia.csPassword = m_tcChannel.csPassword;
                }
            }
        }      
        res = S_OK;
    }
    if (dlg != NULL) {
        delete(dlg);
    }
    return res;//S_FALSE; // Cancel selected
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::GetPodcastChannelData(
    CString &csPodcastChannelName, CString &csPodcastChannelFileUrl, CString &csPodcastMediaDirURL, 
    CString &csChannelServer, int *piChannelProtocol, int *piChannelPort, CString &csChannelUser, 
    CString &csChannelPassword, CString &csChannelTargetDir, 
    CString &csMediaServer, int *piMediaProtocol, int *piMediaPort, CString &csMediaUser, 
    CString &csMediaPassword, CString &csMediaTargetDir) {

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CCrypto crypt;
    csPodcastChannelName = m_csPodcastChannelName;
    csPodcastChannelFileUrl = m_tcChannel.csUrl;
    csChannelServer = m_tcChannel.csServer;
    *piChannelProtocol = m_tcChannel.iService;
    *piChannelPort = m_tcChannel.nPort;
    csChannelUser = m_tcChannel.csUsername;
    csChannelPassword = crypt.Encode(m_tcChannel.csPassword);
    csChannelTargetDir = m_tcChannel.csDirectory;

    csPodcastMediaDirURL = m_tcMedia.csUrl;
    csMediaServer = m_tcMedia.csServer;
    *piMediaProtocol = m_tcMedia.iService;
    *piMediaPort = m_tcMedia.nPort;
    csMediaUser = m_tcMedia.csUsername;
    csMediaPassword = crypt.Encode(m_tcMedia.csPassword);
    csMediaTargetDir = m_tcMedia.csDirectory;
    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::GetErrorString(CString &csLastError) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    csLastError = m_csError;
    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::CheckTargetExists(CString csServer, int iProtocol, int iPort, CString csUser, CString csPassword, CString csTargetDirOrFile, bool* pbIsDir, bool* pbIsEmptyDir) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    if (csPassword.IsEmpty()) {
        csPassword = m_csPwd1;//m_csPwd;
    } else {
        CCrypto crypt;
        csPassword = crypt.Decode(csPassword);
    }
    return rTContent.CheckTarget(csServer, iProtocol, iPort, csUser, csPassword, csTargetDirOrFile, pbIsDir, pbIsEmptyDir);
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::FinishSession() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    rTContent.Cleanup(false);
    m_csPwd1 = _T("");
    m_csPwd2 = _T("");
    m_csServer1 = _T("");
    m_csServer2 = _T("");
    m_csUser1 = _T("");
    m_csUser2 = _T("");
    m_bIsNewPwd = false;
    m_csNewTargetDir = _T("");
    m_bSlsServerWasEmpty = false;
    m_csNewSlsServer = _T("");
    if (m_pbIsCancel != NULL) {
        delete m_pbIsCancel;
        m_pbIsCancel = NULL;
    }
    if (m_pbIsThreadActive != NULL) {
        delete m_pbIsThreadActive;
        m_pbIsThreadActive = NULL;
    }
    m_bIsSession = false;
    if (m_bIsSlsSession) {
        UINT res = SendSlidestarRequest(SLIDESTAR_REQ_COMPLETE);
    }
    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::StartSession() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (m_pbIsCancel == NULL) {
        m_pbIsCancel = new bool();
    }
    *m_pbIsCancel = false;

    if (m_pbIsThreadActive == NULL) {
        m_pbIsThreadActive = new bool();
    }
    *m_pbIsThreadActive = false;
    m_bIsSession = true;
    return S_OK;
}

HRESULT /*__declspec(dllexport) CALLBACK*/ CUploader::CancelSession() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *m_pbIsCancel = true;

    _tprintf(_T("Session to be Cancelled\n"));
    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    while (*m_pbIsThreadActive) {
        Sleep(400);
    }
    rTContent.Cleanup();
    _tprintf(_T("Session Cancelled\n"));
    m_bIsSession = false;
    return S_OK;
}
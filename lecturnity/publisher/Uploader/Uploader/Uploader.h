// Uploader.h : main header file for the Uploader DLL
//

#pragma once



#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif


#include "resource.h"		// main symbols
#include "SingleInstanceApp.h"

#ifdef _UPLOADER_DLL
    #define UPLOADER_EXPORT __declspec(dllexport)
#else
    #ifdef _UPLOADER_EXE
        #define UPLOADER_EXPORT 
    #else
        #define UPLOADER_EXPORT __declspec(dllimport)
    #endif
#endif

// CUploaderApp
// See Uploader.cpp for the implementation of this class
//

#define UPLOAD_FTP  0
#define UPLOAD_SCP  1
#define UPLOAD_SFTP 2
const UINT SLIDESTAR_REQ_ACQUIRE = 0;
const UINT SLIDESTAR_REQ_COMPLETE = 1;
const UINT ERR_NO_ERROR		= 0;
const UINT ERR_FTP_OPEN		= 1;
const UINT ERR_FTP_UPLOAD	= 2;
const UINT ERR_SCP_UPLOAD	= 3;
const UINT ERR_HTTP_REQ 	= 4;
const UINT ERR_CHANNEL_CHANGE = 4;
const HRESULT ERR_CANNOT_OVERWRITE_FILE = -7;
const HRESULT ERR_CANNOT_CREATE_FILE_DIR = -6;
const HRESULT ERR_AUTHENTICATION_FAILED = -5;
const HRESULT ERR_SERVER_NOT_REACHABLE = -4;
const HRESULT ERR_SERVER_DOES_NOT_EXIST = -3;
const HRESULT ERR_NETWORK_ERROR = -2;
const HRESULT ERR_GENERIC_ERROR = -1;
const DWORD YT_MAX_MOVIE_SIZE = 1024 * 1024 * 1024; //1 GB
const DWORD YT_MAX_MOVIE_LEN = 9 * 60 + 59; // 9:59 min


struct TransferParam {
    int       iService;
    CString   csServer;
    CString   csDirectory;
    UINT      nPort;
    CString   csUsername;
    CString   csPassword;
    CString   csUrl;
    BOOL      bSavePassword;
};

struct TransferMetadata {
    CString	csTitle;
    CString	csSubtitle;
    CString csAuthor;
    CString csDescription;
    CString csSummary;
    CString csLanguage;
    CString csCopyright;
    CString	csOwner;
    CString	csEmail;
    CString	csLink;
    CString	csWebmaster;
    CString csExplicit;
    CString csKeywords;
};

struct YtTransferParam {
    CString   csUsername;
    CString   csPassword;
    CString   csTitle;
    CString   csDescription;
    CString   csKeywords;
    CString   csCategory;
    int       iPrivacy;
};

class CUploaderApp : public /*CWinApp*/CSingleInstanceApp {
public:
    CUploaderApp();

    // Overrides
public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()

private:
    CString m_csTmpDirectory;
    TCHAR m_tstrTmpDirectory[MAX_PATH + 1];
public:
    CString GetTempDirectory();
};

class /*__declspec(dllexport)*/UPLOADER_EXPORT CUploader{
public:
    CUploader(void);
public:
    ~CUploader(void);
    HRESULT /*__declspec(dllexport) CALLBACK*/ ShowYtTransferConfigDialog(CString csUser, CString csPassword, CString csTitle, CString csDescription, CString csKeywords, CString csCategory, int iPrivacy);
    HRESULT /*__declspec(dllexport) CALLBACK*/ GetYtTransferData(CString &csUser, CString &csPassword, CString &csTitle, CString &csDescription, CString &csKeywords, CString &csCategory, int *iPrivacy);
    HRESULT /*__declspec(dllexport) CALLBACK*/ CheckYtUpload(CString csUser, CString csPassword, CString csRecLength);
    HRESULT /*__declspec(dllexport) CALLBACK*/ CheckUpload(CString csServer, int iProtocol, int iPort, CString csUser, CString csPassword, CString csTargetDir);
    HRESULT /*__declspec(dllexport) CALLBACK*/ StartYtUpload(CString csUser, CString csPassword, CString csTitle, CString csDescription, CString csKeywords, CString csCategory, int iPrivacy, CArray<CString , CString > &aFilenames);
    HRESULT /*__declspec(dllexport) CALLBACK*/ StartUpload(CString csServer, int iProtocol, int iPort, CString csUser, CString csPassword, CString csTargetDir, CArray<CString , CString > &aFilenames);
    HRESULT /*__declspec(dllexport) CALLBACK*/ StartPodcast(CString csChannelServer, int iChannelProtocol, int iChannelPort, CString csChannelUser, CString csChannelPassword, CString csChannelTargetDir, CString csMediaServer, int iMediaProtocol, int iMediaPort, CString csMediaUser, CString csMediaPassword, CString csMediaTargetDir, CString csMediaDirectoryUrl, CString csChannelFileUrl, CArray<CString , CString > &aFilenames, CString csTitle, CString csAuthor);
    HRESULT /*__declspec(dllexport) CALLBACK*/ GetProgress(double *pdProgress);
    HRESULT /*__declspec(dllexport) CALLBACK*/ ShowTransferConfigDialog(CString csServer, int iProtocol, int iPort, CString csUser, CString csPassword, CString csTargetDir);
    HRESULT /*__declspec(dllexport) CALLBACK*/ GetTransferData(CString &csServer, int *piProtocol, int *piPort, CString &csUser, CString &csPassword, CString &csTargetDir);
    HRESULT /*__declspec(dllexport) CALLBACK*/ ShowPodcastChannelConfigDialog(CString csPodcastChannelName, CString csPodcastChannelFileUrl, CString csPodcastMediaDirURL, CString csChannelServer, int iChannelProtocol, int iChannelPort, CString csChannelUser, CString csChannelPassword, CString csChannelTargetDir, CString csMediaServer, int iMediaProtocol, int iMediaPort, CString csMediaUser, CString csMediaPassword, CString csMediaTargetDir);
    HRESULT /*__declspec(dllexport) CALLBACK*/ GetPodcastChannelData(CString &csPodcastChannelName, CString &csPodcastChannelFileUrl, CString &csPodcastMediaDirURL, CString &csChannelServer, int *piChannelProtocol, int *piChannelPort, CString &csChannelUser, CString &csChannelPassword, CString &csChannelTargetDir, CString &csMediaServer, int *piMediaProtocol, int *piMediaPort, CString &csMediaUser, CString &csMediaPassword, CString &csMediaTargetDir);
    HRESULT /*__declspec(dllexport) CALLBACK*/ GetErrorString(CString &csLastError);
    HRESULT /*__declspec(dllexport) CALLBACK*/ CheckTargetExists(CString csServer, int iProtocol, int iPort, CString csUser, CString csPassword, CString csTargetDirOrFile, bool* pbIsDir, bool* pbIsEmptyDir);
    HRESULT /*__declspec(dllexport) CALLBACK*/ SetParentHwnd(HWND hParentHwnd){m_hParentHwnd = hParentHwnd; return S_OK;};
    HRESULT /*__declspec(dllexport) CALLBACK*/ FinishSession();
    HRESULT /*__declspec(dllexport) CALLBACK*/ StartSession();
    HRESULT /*__declspec(dllexport) CALLBACK*/ CancelSession();
    HRESULT IsSlidestar(bool bIsSls){m_bIsSlsSession = bIsSls;return S_OK;};
private:
    UINT GetRecordLengthSeconds(CString csRecLength);
    void SetErrorString(int nId, CString csStr = NULL);
    HRESULT CreateSubDirOnServer(CString csRoot, TransferParam tp);
    CString SetTargDir(CString csDirName);
    UINT SendSlidestarRequest(UINT iReqType);
    CString GetResponseParamValue(CString csResponse, CString csParam);
    void InitYtCategories();
    YtTransferParam m_ytTp;
    TransferParam m_tp;
    TransferParam m_tcChannel;
    TransferParam m_tcMedia;
    CString m_csPodcastChannelName;
    CString m_csError;
    double *m_dProgress;
    double *m_dTempProgress;
    int *m_piCurrFile;
    CArray<double, double> m_adProcStart;
    double m_dCurrProg;
    CString m_csPwd1;
    CString m_csPwd2;
    int m_nCount;
    bool m_bIsNewPwd;
    CString m_csServer1;
    CString m_csServer2;
    CString m_csSlsServer;
    CString m_csUser1;
    CString m_csUser2;
    HWND m_hParentHwnd;
    CString m_csNewTargetDir;
    bool *m_pbIsCancel;
    bool *m_pbIsThreadActive;
    bool m_bIsSession;
    bool m_bIsSlsSession;
    CString m_csSlsUser;
    CString m_csSlsPass;
    CString m_csTempSlsUser;
    CString m_csTempSlsPass;
    CString m_csTempSlsId;
    bool m_bLocalCall;
    CArray<CString, CString>m_acsYtCategories;
    CString m_csYtAuthToken;
    CString m_csYtAuthUser;
    bool m_bYtIsNewUserOrPassword;
    CString m_csYtNewUser;
    CString m_csYtNewPassword;
    bool m_bSlsServerWasEmpty;
    CString m_csNewSlsServer;
};
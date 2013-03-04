#pragma once
#include "afxwin.h"
#include "afxinet.h"
class CYouTube {
public:
    CYouTube(void);
public:
    virtual ~CYouTube(void);
    BOOL Authentify(CString csUser, CString csPassword);
    UINT Upload(CString csFile, YtTransferParam tp, double *pdProgress, bool *pbIsCancel);
    void GetAuthToken(CString &csToken){csToken = m_csAuthToken;};
    void SetAuthToken(CString csToken){m_csAuthToken = csToken;};
    void GetAuthUser(CString &csAuthUser){csAuthUser = m_csAuthUser;};
    CString GetErrString(){return m_errStr;};
    int GetErrCode(){return m_iErrCode;};
    
private:
    CString CreatePreFileData(YtTransferParam tp);
    void AddUploadRequestHeaders(CString csFileName, DWORD dwTotalReqLength, CHttpFile *pHttpFile);
    bool ProcessAuthResponse(CString csResponse, CString &csToken, CString &csAuthUser, CString &csError);
    //void InitCategories();
    CString GetHttpResponse(CHttpFile* pHttpFile, int iType);
    UINT ProcessUploadResponse(CString csResponse);
    CString GetInetError(DWORD dwErrorNo);
    void ProcessAuthError(CString csErrResponse, CString &csErrorStr, int &iErrorCode);
    CString GetUploadErrorString(CString csResponse);
    
    CArray<CString, CString>m_acsCategories; 
    CString m_errStr;
    int m_iErrCode;
    CString m_csAuthToken;
    CString m_csBoundryStr;
    CString m_csBoundry;
    CString m_csLineTerm;
    CString m_csClientCode;
    CString m_csDevCode;
    CString m_csVideoId;
    CString m_csAuthUser;
    
};

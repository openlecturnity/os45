#include "StdAfx.h"
#include "Uploader.h"
#include "YouTube.h"
#include "LanguageSupport.h"

CYouTube::CYouTube(void) {
    m_csAuthToken = _T("");
    m_csAuthUser = _T("");
    m_csBoundryStr = _T("soare131415");
    m_csBoundry = _T("--") + m_csBoundryStr;
    m_csLineTerm = _T("\r\n");
    m_csClientCode = _T("client id");
    m_csDevCode = _T("key=AI39si5j1jdpE7RGjtLTcHyd6-GfBoGoLvFrpcI92Y2Ia6U2d7mFeimX2SLiwlaDBaI9pLFs9ItQ8-nhD_niL51u4XhdJhF0ng");
    m_csVideoId = _T("");
    // InitCategories();
}

CYouTube::~CYouTube(void) {
}

BOOL CYouTube::Authentify(CString csUser, CString csPassword) {
    BOOL bRes = FALSE;
    CInternetSession session;
    _tprintf(_T("!!!Upl in CYouTube::Authentify user = %s, pass=%s \n"), csUser, csPassword);
    CString csPath = _T("/youtube/accounts/ClientLogin");
    CString csQuerryFormat =  _T("?Email=%s&Passwd=%s&service=youtube&source=Lec.YouTube.Library");
    CString csQuerry;
    csQuerry.Format(csQuerryFormat, csUser, csPassword);
    csPath += csQuerry;
    try {
        _tprintf(_T("!!!Upl in CYouTube::Authentify before GetHttpConnection\n"));
        CHttpConnection* pConnection = session.GetHttpConnection(_T("www.google.com"), INTERNET_DEFAULT_HTTPS_PORT, _T(""), _T(""));
        if (pConnection != NULL) {
            _tprintf(_T("!!!Upl in CYouTube::Authentify after GetHttpConnection\n"));
            _tprintf(_T("!!!Upl in CYouTube::Authentify before OpenRequest\n"));
            CHttpFile* pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, csPath,
                NULL, 1, NULL, HTTP_VERSION, 
                INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID |INTERNET_FLAG_IGNORE_CERT_DATE_INVALID);
            _tprintf(_T("!!!Upl in CYouTube::Authentify after OpenRequest\n"));
            if (pHttpFile != NULL) {
                _tprintf(_T("!!!Upl in CYouTube::Authentify before SendRequest\n"));
                CString csHeaders = _T("Content-Type: application/x-www-form-urlencoded\r\n");
                BOOL result = pHttpFile->SendRequest(csHeaders);
                _tprintf(_T("!!!Upl in CYouTube::Authentify after SendRequest\n"));
                if (result != FALSE) {
                    _tprintf(_T("!!!Upl in CYouTube::Authentify before GetHttpResponse\n"));
                    CString csResponse = GetHttpResponse(pHttpFile, 1);
                    _tprintf(_T("!!!Upl in CYouTube::Authentify after GetHttpResponse resp=%s\n"), csResponse);
                    CString csAuthToken, csAuthUser, csErrorString;
                    if (ProcessAuthResponse(csResponse, csAuthToken, csAuthUser,csErrorString)) {
                        m_csAuthToken = csAuthToken;
                        m_csAuthUser = csAuthUser;
                        bRes = TRUE;
                        //for test begin
                        //m_errStr = csResponse;
                        //for test end
                    } else {
                        bRes = FALSE;// ERR_AUTHENTICATION_FAILED;
                        ProcessAuthError(csErrorString, m_errStr, m_iErrCode);
                    }
                } else {
                    return ERR_HTTP_REQ;
                }
                pHttpFile->Close();
            } else {
                //return error http file
            }
            pConnection->Close();
        } else {
            //return http connection
        }
    } catch(CInternetException *pEx) {
        m_errStr = GetInetError(pEx->m_dwError);
    }
    session.Close();

    return bRes;
}

UINT CYouTube::Upload(CString csFile, YtTransferParam tp, double *pdProgress, bool *pbIsCancel) {
    UINT res = S_FALSE;
    CFile file;
    BOOL bFile = file.Open(csFile, CFile::modeRead);
    if (!bFile) {
        //set error message IDS_ERROPENFILE
        return S_FALSE;
    }

    int iFileSize = file.GetLength();
    CString csPreFileData = CreatePreFileData(tp);
    CString csPostFileData = m_csLineTerm + m_csBoundry + _T("--");
    int nUtfSize = ::WideCharToMultiByte(CP_UTF8, 0, csPreFileData, csPreFileData.GetLength(), NULL, 0, NULL, NULL);
    char *szTemp = new char[nUtfSize];// + 1];
    memset(szTemp, 0, nUtfSize /*+ 1*/);
    ::WideCharToMultiByte(CP_UTF8, 0, csPreFileData, csPreFileData.GetLength(), szTemp, nUtfSize /*+ 1*/, NULL, NULL);

    DWORD dwTotalLength =  /*csPreFileData.GetLength()*/nUtfSize /*+ 1*/ + iFileSize + csPostFileData.GetLength();

    CString csResponse = _T("");
    CInternetSession session;
    CInternetException* pCancelEx = new CInternetException(1);
    try {
        CString strServer;
        CString strObject;
        DWORD    dwType = 0;
        INTERNET_PORT wPort;

        CString csPath;
        csPath.Format(_T("/feeds/api/users/%s/uploads"), tp.csUsername);
        CString szUri = _T("http://uploads.gdata.youtube.com") + csPath;
        AfxParseURL(szUri, dwType, strServer, strObject, wPort);

        DWORD dwTimeout = 1000 * 60 * 5;
        session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, dwTimeout);
        session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, dwTimeout);
        CHttpConnection* pConnection = session.GetHttpConnection(strServer, wPort, _T(""), _T(""));//(_T("uploads.gdata.youtube.com"), INTERNET_DEFAULT_HTTP_PORT,_T(""), _T(""));
        if (pConnection != NULL) {
            CHttpFile* pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject/*csPath*/, NULL, 1, NULL, _T("HTTP/1.1"), INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT);
            if (pHttpFile != NULL) {
                AddUploadRequestHeaders(csFile, dwTotalLength, pHttpFile);
                BOOL bres = pHttpFile->SendRequestEx(dwTotalLength, HSR_ASYNC/*, HSR_SYNC | HSR_INITIATE*/);

                bool bCancel = false;

                DWORD dwErr = GetLastError();
                USES_CONVERSION;

#ifdef _UNICODE
                pHttpFile->Write(szTemp, nUtfSize /*+ 1*/ );//(W2A(csPreFileData), csPreFileData.GetLength());
#else
                pHttpFile->Write((LPSTR)(LPCSTR)csPreFileData, csPreFileData.GetLength());
#endif
                //pFile->Write(pFileBuff, iFileSize);
                DWORD dwReadLength = -1;
                DWORD dwChunkLength = iFileSize / 100;//2 * 1024;
                BYTE* pBuffer = new BYTE[dwChunkLength];
                //pBuffer  = malloc(dwChunkLength);
                while ( 0 != dwReadLength) {
                    if (pbIsCancel != NULL && *pbIsCancel == true) {
                        bCancel = true;
                        break;
                    }
                    dwReadLength = file.Read(pBuffer, dwChunkLength);
                    if ( 0 != dwReadLength ) {
                        pHttpFile->Write(pBuffer, dwReadLength);
                        if (pdProgress != NULL) {
                            *pdProgress += (dwReadLength / (iFileSize * 1.0)) * 0.95;
                        }
                    }
                }
                delete ( pBuffer );
#ifdef _UNICODE
                pHttpFile->Write(W2A(csPostFileData), csPostFileData.GetLength());
#else 
                pHttpFile->Write((LPSTR)(LPCSTR)csPostFileData, csPostFileData.GetLength());
#endif

                if (!bCancel) {
                    pHttpFile->EndRequest(/*HSR_SYNC*/);

                    csResponse = GetHttpResponse(pHttpFile, 0);
                } else {
                    //pHttpFile->EndRequest(HSR_ASYNC);
                    return S_OK;
                }


                pHttpFile->Close();

                res = S_OK;

                //for test begin
                //m_errStr = csResponse;
                //for test end

            } else {
                //return error http file
            }
            pConnection->Close();
        } else {
            //return http connection
        }

    } catch(CInternetException *pEx) {
        // GetInetError
        _tprintf(_T("Intra pe ex in upload!!!!\n"));
        m_errStr = GetInetError(pEx->m_dwError);
        pEx->Delete();
        res = S_FALSE;
        //return 
    }
    session.Close();
    file.Close();

    if (res == S_OK) {
        res = ProcessUploadResponse(csResponse);
    }

    return res;
}

bool CYouTube::ProcessAuthResponse(CString csResponse, CString &csToken, CString &csAuthUser, CString &csError) {
    bool bRes = false;
    if (csResponse.IsEmpty()) {
        return false;
    }
    int iPos = 0;
    CString csSeparator = _T("\n");
    CString csTok = csResponse.Tokenize(csSeparator, iPos);
    while (csTok != "") {
        if (csTok.Find(_T("Auth=")) != -1){
            csToken = csTok.Right(csTok.GetLength() - 5);
            csError = _T("");
            bRes = true;
            int iUserPos = csResponse.Find(_T("YouTubeUser="));
            if( iUserPos!= -1) {            
                csAuthUser = csResponse.Mid(iUserPos + 12, csResponse.GetLength() - 1 - iUserPos - 12);               
            }
            break;
        }

        if (csTok.Find(_T("Error=")) != -1) {
            csError = csTok.Right(csTok.GetLength() - 6);
            csToken = _T("");
            csAuthUser = _T("");
            bRes = false;
            break;
        }
        csTok = csResponse.Tokenize(csSeparator, iPos);
    }
    return bRes;
}

CString CYouTube::CreatePreFileData(YtTransferParam tp) {
    CString csPreFileFormat=_T("");
    csPreFileFormat += m_csLineTerm +m_csBoundry + m_csLineTerm + _T("Content-Type: application/atom+xml; charset=UTF-8") + m_csLineTerm + m_csLineTerm;
    csPreFileFormat += _T("<?xml version=\"1.0\"?><entry xmlns=\"http://www.w3.org/2005/Atom\" ");
    csPreFileFormat += _T("xmlns:media=\"http://search.yahoo.com/mrss/\" xmlns:yt=\"http://gdata.youtube.com/schemas/2007\">");
    csPreFileFormat += _T("<media:group><media:title type=\"plain\">%s</media:title>");
    csPreFileFormat += _T("<media:description type=\"plain\">%s</media:description>");
    csPreFileFormat += _T("<media:category scheme=\"http://gdata.youtube.com/schemas/2007/categories.cat\">%s</media:category>");
    csPreFileFormat += _T("<media:keywords>%s</media:keywords>");
    csPreFileFormat += _T("%s</media:group></entry>") + m_csLineTerm;
    csPreFileFormat += m_csBoundry + m_csLineTerm + _T("Content-Type: video/mp4") + m_csLineTerm + _T("Content-Transfer-Encoding: binary") + m_csLineTerm + m_csLineTerm;

    CString csPrivacy = _T("");
    if (tp.iPrivacy != 0) {
        csPrivacy = _T("<yt:private/>");
    }

    CString csPreFile;
    csPreFile.Format(csPreFileFormat,tp.csTitle, tp.csDescription, tp.csCategory, tp.csKeywords, csPrivacy);
    return csPreFile;
}

void CYouTube::AddUploadRequestHeaders(CString csFileName, DWORD dwTotalReqLength, CHttpFile *pHttpFile) {
    CString csMp4Name = _T("");
    int iPos = csFileName.ReverseFind(_T('\\'));
    if (iPos != -1) {
        csMp4Name = csFileName.Right(csFileName.GetLength() - iPos -1);
    } else {
        csMp4Name = csFileName;
    }

    CString csHeader = _T("");
    CString csHeaderFormat = _T("");

    csHeaderFormat.Format(_T("Content-Type: multipart/related; boundary=%s;\r\n"), m_csBoundryStr);
    csHeader += csHeaderFormat;
    pHttpFile->AddRequestHeaders(csHeader);
    csHeader.Empty();
    csHeaderFormat.Empty();

    csHeaderFormat.Format(_T("Authorization: GoogleLogin auth=%s\r\n"), m_csAuthToken);
    csHeader += csHeaderFormat;
    pHttpFile->AddRequestHeaders(csHeader);
    csHeader.Empty();
    csHeaderFormat.Empty();

    csHeaderFormat.Format(_T("X-GData-Client: %s\r\n"), m_csClientCode);
    csHeader += csHeaderFormat;
    pHttpFile->AddRequestHeaders(csHeader);
    csHeader.Empty();
    csHeaderFormat.Empty();

    csHeaderFormat.Format(_T("X-GData-Key: %s\r\n"), m_csDevCode);
    csHeader += csHeaderFormat;
    pHttpFile->AddRequestHeaders(csHeader);
    csHeader.Empty();
    csHeaderFormat.Empty();

    csHeaderFormat.Format(_T("Slug: %s\r\n"), csMp4Name);
    csHeader += csHeaderFormat;
    pHttpFile->AddRequestHeaders(csHeader);
    csHeader.Empty();

    csHeader += _T("Host: uploads.gdata.youtube.com\r\n");
    pHttpFile->AddRequestHeaders(csHeader);
    csHeader.Empty();
    csHeaderFormat.Empty();

    csHeaderFormat.Format(_T("Content-Length: %d\r\n"), dwTotalReqLength);
    csHeader += csHeaderFormat;
    pHttpFile->AddRequestHeaders(csHeader);
    csHeader.Empty();
    csHeaderFormat.Empty();

    csHeader += _T("Expect: 100-continue\r\n");
    pHttpFile->AddRequestHeaders(csHeader);
    csHeader.Empty();

    csHeader += _T("Connection: Close\r\n");
    pHttpFile->AddRequestHeaders(csHeader);
}

//void CYouTube::InitCategories()
//{
//    m_acsCategories.RemoveAll();
//
//    m_acsCategories.Add(_T("Education"));
//    m_acsCategories.Add(_T("Film"));
//    m_acsCategories.Add(_T("Autos"));
//    m_acsCategories.Add(_T("Music"));
//    m_acsCategories.Add(_T("Animals"));
//    m_acsCategories.Add(_T("Sports"));
//    m_acsCategories.Add(_T("Travel"));
//    m_acsCategories.Add(_T("Shortmov"));
//    m_acsCategories.Add(_T("Videoblog"));
//    m_acsCategories.Add(_T("Games"));
//    m_acsCategories.Add(_T("Comedy"));
//    m_acsCategories.Add(_T("People"));
//    m_acsCategories.Add(_T("News"));
//    m_acsCategories.Add(_T("Entertainment"));
//    m_acsCategories.Add(_T("Howto"));
//    m_acsCategories.Add(_T("Nonprofit"));
//    m_acsCategories.Add(_T("Tech"));
//}

CString CYouTube::GetHttpResponse(CHttpFile* pHttpFile, int iType) {
    /*CString csTestFile = _T("c:\\Soare\\Work\\LEC40\\youtube\\resp1");
    if(iType == 0)
    {
    csTestFile += _T("_upl.html");
    }
    else
    {
    csTestFile += _T("_auth.txt");
    }
    CFile tFile;
    tFile.Open(csTestFile, CFile::modeCreate | CFile::modeWrite);*/

    char aa;
    CString csResponse = _T("");
    while (pHttpFile->Read(&aa,1) == 1) {
        csResponse += aa;
        //tFile.Write(&aa, 1);
    }
    //tFile.Close();
    return csResponse;
}

UINT CYouTube::ProcessUploadResponse(CString csResponse) {   
    int iIdStartPos = csResponse.Find(_T("<id>"));
    if (iIdStartPos != -1) {
        int iIdEndPos = csResponse.Find(_T("</id>"), iIdStartPos);
        if (iIdEndPos > iIdStartPos) {
            return S_OK; 
        }
    }

    m_errStr = GetUploadErrorString(csResponse);

    return S_FALSE;
}

CString CYouTube::GetUploadErrorString(CString csResponse) {
    CString retVal = _T("");
    CString csUploadError;
    if (csResponse.Find(_T("<errors>")) != -1) {
        if (csResponse.Find(_T("yt:validation")) != -1) {
            if (csResponse.Find(_T("deprecated")) != -1) {
                csUploadError.LoadString(IDS_YTU_DEPRECATED);
                retVal += csUploadError;
            }
            if (csResponse.Find(_T("invalid_character")) != -1) {
                csUploadError.LoadString(IDS_YTU_INVALID_CHAR);
                retVal += csUploadError;
            }
        }
        if (csResponse.Find(_T("yt:quota")) != -1) {
            if (csResponse.Find(_T("too_many_recent_calls")) != -1) {
                csUploadError.LoadString(IDS_YTU_TOO_MANY_CALLS);
                retVal += csUploadError;
            }
            if (csResponse.Find(_T("too_many_entries")) != -1) {
                csUploadError.LoadString(IDS_YTU_TOO_MANY_ENTRIES);
                retVal += csUploadError;
            }
        }
        if (csResponse.Find(_T("yt:authentication")) != -1) {
            if (csResponse.Find(_T("InvalidToken")) != -1) {
                csUploadError.LoadString(IDS_YTU_TOKEN_INVALID);
                retVal += csUploadError;
            }
            if (csResponse.Find(_T("TokenExpired")) != -1) {
                csUploadError.LoadString(IDS_YTU_TOKEN_EXPIRED);
                retVal += csUploadError;
            }
        }
        if (csResponse.Find(_T("yt:service")) != -1) {
            csUploadError.LoadString(IDS_YTU_SERVICE);
            retVal += csUploadError;
        }
        if (retVal.IsEmpty()) {
            csUploadError.LoadString(IDS_YT_ERR_UNKNOWN);
            retVal += csUploadError;
        }
    } else {
        csUploadError.LoadString(IDS_YT_ERR_UNKNOWN);
        retVal += csUploadError;
    }
    return retVal;
}

CString CYouTube::GetInetError(DWORD dwErrorNo) {
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

void CYouTube::ProcessAuthError(CString csErrResponse, CString &csErrorStr, int &iErrorCode) {
    CString csErr;
    if (csErrResponse.CompareNoCase(_T("BadAuthentication")) == 0) {
        csErr.LoadString(IDS_YT_ERR_AUTH_BAD_AUTH);
        csErrorStr = csErr;
        iErrorCode = 87;
    } else if (csErrResponse.CompareNoCase(_T("NotVerified")) == 0) {
        csErr.LoadString(IDS_YT_ERR_AUTH_NOT_VERIF);
        csErrorStr = csErr;
        iErrorCode = 1;
    } else if (csErrResponse.CompareNoCase(_T("TermsNotAgreed")) == 0) {
        csErr.LoadString(IDS_YT_ERR_AUTH_TERMS_NAGREED);
        csErrorStr = csErr;
        iErrorCode = 1;
    } else if (csErrResponse.CompareNoCase(_T("CaptchaRequired")) == 0) {
        csErr.LoadString(IDS_YT_ERR_AUTH_CAPCHA_REQ);
        csErrorStr = csErr;
        iErrorCode = 1;
    } else if (csErrResponse.CompareNoCase(_T("AccountDeleted")) == 0) {
        csErr.LoadString(IDS_YT_ERR_AUTH_ACCOUNT_DEL);
        csErrorStr = csErr;
        iErrorCode = 1;
    } else if (csErrResponse.CompareNoCase(_T("AccountDisabled")) == 0) {
        csErr.LoadString(IDS_YT_ERR_AUTH_ACCOUNT_DIS);
        csErrorStr = csErr;
        iErrorCode = 1;
    } else if (csErrResponse.CompareNoCase(_T("ServiceDisabled")) == 0) {
        csErr.LoadString(IDS_YT_ERR_AUTH_SERVICE_DIS);
        csErrorStr = csErr;
        iErrorCode = 1;
    } else if (csErrResponse.CompareNoCase(_T("ServiceUnavailable")) == 0) {
        csErr.LoadString(IDS_YT_ERR_AUTH_SERVICE_UNAV);
        csErrorStr = csErr;
        iErrorCode = 1;
    } else {
        csErr.LoadString(IDS_YT_ERR_UNKNOWN);
        csErrorStr = csErr;
        iErrorCode = 1;
    }
}
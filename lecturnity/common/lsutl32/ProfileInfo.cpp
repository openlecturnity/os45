#include "stdafx.h"
#include "Resource.h"
#include "ProfileInfo.h"
#include "ProfileManager.h"
#include "ProfileUtils.h"
#include "MfcUtils.h"
#include "JNISupport.h"
#include "LanguageSupport.h"
#include "MiscFunctions.h"

#include "lutils.h" // lutils

// Some functions of the manage dialog are called from publisher.
// For this it is necessary to make the profile manager global
CProfileManager *s_pProfileManager = NULL;
int s_iLastError = S_OK;

ProfileInfo::ProfileInfo() {
    InitializeVariables();
}

ProfileInfo::ProfileInfo(CString &csFilename) {
    InitializeVariables();

    m_csFilename = csFilename;
}

ProfileInfo::ProfileInfo(ProfileInfo *pProfileInfo) {
    InitializeVariables();

    m_iProfileID = pProfileInfo->GetID();
    m_iProfileVersion = pProfileInfo->GetVersion();
    m_iProfileType = pProfileInfo->GetType();

    m_csFilename = pProfileInfo->GetFilename();
    m_csTitle = pProfileInfo->GetTitle();
    m_csTargetFormat = pProfileInfo->GetTargetFormat();
    m_csStorageDistribution = pProfileInfo->GetStorageDistribution();

    m_bActivated = pProfileInfo->IsActivated();

    m_bCreatedByAdmin = pProfileInfo->IsCreatedByAdmin();

    pProfileInfo->GetKeysAndValues(m_aKeys, m_aValues);
}

ProfileInfo::ProfileInfo(CStringArray &aKeys, CStringArray &aValues)
{
    InitializeVariables();

    m_aKeys.Append(aKeys);
    m_aValues.Append(aValues);

    ExtractTargetFormat();
    ExtractStorageDistribution();
}

ProfileInfo::~ProfileInfo(void) {
    m_aKeys.RemoveAll();
    m_aValues.RemoveAll();
}

void ProfileInfo::GetKeysAndValues(CStringArray &aKeys, CStringArray &aValues) {
    for (int i = 0; i < m_aKeys.GetCount(); ++i) {
        aKeys.Add(m_aKeys[i]);
        aValues.Add(m_aValues[i]);
    }
}

void ProfileInfo::SetTitle(CString &csTitle) {
    m_csTitle = csTitle;

    for (int i = 0; i < m_aKeys.GetSize(); ++i) {
        if (m_aKeys[i] == _T("strTitle")) {
            m_aValues[i] = m_csTitle;
        }
    }
}

HRESULT ProfileInfo::Write(LPCTSTR szFilename) {
    HRESULT hr = S_OK;

    if (szFilename == NULL && m_csFilename.IsEmpty()) {
        HKEY hKey = HKEY_LOCAL_MACHINE;

        bool bIsUserAdmin = LMisc::IsUserAdmin();
        if (!bIsUserAdmin)
            hKey = HKEY_CURRENT_USER;
        // TODO this does not reflect "elevation" on Windows Vista/7 (not admin but can become one)

        CString csProfileDirectory;
        hr = ProfileUtils::GetProfileDirectory(hKey, csProfileDirectory, true);

        ProfileInfo *pInfo = ProfileUtils::GetProfileWithID(m_iProfileID);
        if (pInfo) {
            CString csFileToDelete = pInfo->GetFilename();
            CString csFilePath = csFileToDelete;
            StringManipulation::GetPath(csFilePath);
            if (csFilePath == csProfileDirectory)
                DeleteFile(csFileToDelete);
            else 
                ProfileUtils::GetNewProfileID(m_iProfileID);
            delete pInfo;
        } 

        if (SUCCEEDED(hr))
            m_csFilename.Format(_T("%s\\Profile_%d_%I64u.lpp"), csProfileDirectory, m_iProfileType, m_iProfileID);
    }

    CString csFilename = m_csFilename;
    if (SUCCEEDED(hr) && szFilename != NULL)
        csFilename = szFilename;

    LURESULT lr = S_OK;
    LFile *pProfileFile = NULL;
    if (SUCCEEDED(hr)) {
        pProfileFile = new LFile(csFilename);
        if (pProfileFile == NULL)
            hr = E_PM_FILE_CREATE;
    }

    if (SUCCEEDED(hr)) {
        lr = pProfileFile->Create(LFILE_TYPE_TEXT_UNICODE, TRUE);
        if (lr != S_OK)
            hr = E_PM_FILE_CREATE;
        if (!pProfileFile->Exists())
            hr = E_PM_FILE_NOTEXIST;
    }

    CString csLine;
    if (SUCCEEDED(hr)) {
        csLine.Format(_T("lpp_version=%d\n"), m_iProfileVersion);
        lr = pProfileFile->WriteText(csLine, csLine.GetLength());
        if (lr != S_OK)
            hr = E_PM_FILE_WRITE;
    }

    if (SUCCEEDED(hr)) {
        csLine.Format(_T("lpp_id=%I64d\n"), m_iProfileID);
        lr = pProfileFile->WriteText(csLine, csLine.GetLength());
        if (lr != S_OK)
            hr = E_PM_FILE_WRITE;
    }

    if (SUCCEEDED(hr)) {
        csLine.Format(_T("lpp_title=%s\n"), m_csTitle);
        lr = pProfileFile->WriteText(csLine, csLine.GetLength());
        if (lr != S_OK)
            hr = E_PM_FILE_WRITE;
    }

    if (SUCCEEDED(hr)) {
        csLine.Format(_T("lpp_type=%d\n"), m_iProfileType);
        lr = pProfileFile->WriteText(csLine, csLine.GetLength());
        if (lr != S_OK)
            hr = E_PM_FILE_WRITE;
    }

    if (SUCCEEDED(hr)) {
        for (int i = 0; i < m_aValues.GetSize() && SUCCEEDED(hr); ++i) {
            csLine.Format(_T("%s=%s\n"), m_aKeys[i], m_aValues[i]);
            lr = pProfileFile->WriteText(csLine, csLine.GetLength());
            if (lr != S_OK)
                hr = E_PM_FILE_WRITE;
        }
    }

    if (pProfileFile) {
        pProfileFile->Close();
        delete pProfileFile;
    }

    return hr;
}

HRESULT ProfileInfo::Read() {
    HRESULT hr = S_OK;

    LFile profileFile(m_csFilename);

    if (!profileFile.Exists())
        hr = E_PM_FILE_NOTEXIST;

    if (SUCCEEDED(hr)) {
        LURESULT lr = profileFile.Open();
        if (lr == LFILE_ERR_OPEN || lr == LFILE_ERR_ALREADY_OPEN)
            hr = E_PM_FILE_OPEN;
    }

    if (SUCCEEDED(hr)) {
        LBuffer fileBuffer(256);

        DWORD dwBytesRead;
        CStringW csLine;
        UINT uiLinesRead = 0;

        // Read BOM (first 2 bytes)
        LURESULT lr = profileFile.ReadRaw(&fileBuffer, 0, 2, &dwBytesRead);
        if (lr != S_OK)
            hr = E_PM_FILE_READ;


        bool bFirstBlock = true;
        if (SUCCEEDED(hr)) {
            do {
                lr = profileFile.ReadRaw(&fileBuffer, 0, fileBuffer.GetSize(), &dwBytesRead);
                if (lr != S_OK)
                    hr = E_PM_FILE_READ;

                if (SUCCEEDED(hr) && bFirstBlock) {
                    WCHAR *pIdent = (WCHAR *)fileBuffer.GetBuffer();
                    if (wcsncmp(pIdent, L"lpp_", 4) != 0)
                        hr = E_PM_WRONG_FORMAT;
                    bFirstBlock = false;
                }

                if (SUCCEEDED(hr)) {
                    WCHAR *pBuffer = (WCHAR *)fileBuffer.GetBuffer();
                    UINT dwCharsRead = dwBytesRead / sizeof(WCHAR);
                    for (int i = 0; i <  dwCharsRead; ++i) {
                        if (pBuffer[i] == L'\n') {
                            CString csKey;
                            CString csValue;
                            //Ignore lines which begins with % or #
                            if (csLine[0] != L'%' && csLine[0] != L'#') {
                                int iBlankPos = csLine.Find(L'=');
                                if (iBlankPos >= 0) {
#ifdef _UNICODE
                                    csKey = csLine.Left(iBlankPos);
                                    csValue = csLine.Right(csLine.GetLength() - (iBlankPos+1));
#else
                                    CStringW csKeyW = csLine.Left(iBlankPos);
                                    int nLen = csKeyW.GetLength();
                                    char *szRet = new char[nLen + 1];
                                    WideCharToMultiByte(CP_ACP, 0, csKeyW, -1, 
                                        szRet, nLen + 1, NULL, NULL);
                                    csKey = szRet;
                                    delete szRet;

                                    CStringW csValueW = csLine.Right(csLine.GetLength() - (iBlankPos+1)); 
                                    nLen = csValueW.GetLength();
                                    szRet = new char[nLen + 1];
                                    WideCharToMultiByte(CP_ACP, 0, csValueW, -1, 
                                        szRet, nLen + 1, NULL, NULL);
                                    csValue = szRet;
                                    delete szRet;
#endif
                                    if (csKey == _T("lpp_id")) {
                                        m_iProfileID = _ttoi64((LPCTSTR)csValue);
                                    } else if (csKey == _T("lpp_version")) {
                                        m_iProfileVersion = _ttoi((LPCTSTR)csValue);
                                    } else if (csKey == _T("lpp_title")) {
                                        m_csTitle = csValue;
                                    } else if (csKey == _T("lpp_type")) {
                                        m_iProfileType = _ttoi((LPCTSTR)csValue);
                                    } else {
                                        m_aKeys.Add(csKey);
                                        m_aValues.Add(csValue);
                                    }
                                }
                            }
                            csLine.Empty();
                        } else {
                            csLine += pBuffer[i];
                        }
                    }
                }

            } while (dwBytesRead == fileBuffer.GetSize());
        }

        profileFile.Close();
    }

    ExtractTargetFormat();
    ExtractStorageDistribution();

    return hr;
}

HRESULT ProfileInfo::DeleteProfile() {
    HRESULT hr = S_OK;

    if (m_csFilename.IsEmpty())
        hr = E_FAIL;

    BOOL bRet = FALSE;
    if (SUCCEEDED(hr)) {
        bRet = ::DeleteFile(m_csFilename);
        if (bRet == 0)
            hr = E_FAIL;
    }

    return hr;
}

HRESULT ProfileInfo::Edit(CWnd *pCallingWindow, CString csLrdName) {

    HRESULT hr = S_OK;

    CString csConverterPath = JNISupport::GetInstallDir();

    if (!csConverterPath.IsEmpty()) {
        ProfileUtils::HandlePublisherFinish(pCallingWindow);

        if (csConverterPath[csConverterPath.GetLength() - 1] != _T('\\'))
            csConverterPath += _T("\\");
        csConverterPath += _T("Publisher");

        CString csConverterExe = csConverterPath;
        csConverterExe += _T("\\publisher.exe");

        CString csParameter;
        csParameter.Format(_T("-profile %I64d \"%s\""), m_iProfileID, csLrdName);

        HINSTANCE hInstance = ShellExecute(NULL, _T("open"), csConverterExe, csParameter, csConverterPath, SW_SHOWNORMAL);
        if ((int)hInstance <= 32)
            hr = E_FAIL;

    }
    else 
        hr = E_FAIL;

    return hr;
}

void ProfileInfo::InitializeVariables() {
    m_iProfileID = 0;
    m_iProfileVersion = 0;
    m_iProfileType = -1;

    m_csFilename = "";
    m_csTitle = "";
    m_csTargetFormat = "";
    m_csStorageDistribution = "";

    m_bActivated = false;

    m_aKeys.RemoveAll();
    m_aValues.RemoveAll();

    m_bCreatedByAdmin = false;
}

void ProfileInfo::ExtractTargetFormat()
{
    int iExportType = -1;
    for (int i = 0; i < m_aKeys.GetSize(); ++i) {
        if (m_aKeys[i] == _T("iExportType"))
            iExportType = _ttoi(m_aValues[i]);
    }

    m_csTargetFormat.Empty();

    switch (iExportType) {
        case EXPORT_TYPE_LECTURNITY:
            m_csTargetFormat.LoadString(IDS_EXPORT_LPD);
            break;
        case EXPORT_TYPE_REAL:
            m_csTargetFormat.LoadString(IDS_EXPORT_REAL);
            break;
        case EXPORT_TYPE_WMT:
            m_csTargetFormat.LoadString(IDS_EXPORT_WM);
            break;
        case EXPORT_TYPE_FLASH:
            m_csTargetFormat.LoadString(IDS_EXPORT_FLASH);
            break;
        case EXPORT_TYPE_VIDEO:
            m_csTargetFormat.LoadString(IDS_EXPORT_IPOD);
            break;
    }
}

void ProfileInfo::ExtractStorageDistribution()
{
    int iReplayType = -1;
    for (int i = 0; i < m_aKeys.GetSize(); ++i) {
        if (m_aKeys[i] == _T("iReplayType"))
            iReplayType = _ttoi(m_aValues[i]);
    }

    int iServerType = -1;
    for (int i = 0; i < m_aKeys.GetSize(); ++i) {
        if (m_aKeys[i] == _T("iServerType"))
            iServerType = _ttoi(m_aValues[i]);
    }

    int iTransferType = -1;
    for (int i = 0; i < m_aKeys.GetSize(); ++i) {
        if (m_aKeys[i] == _T("iTransferType"))
            iTransferType = _ttoi(m_aValues[i]);
    }

    switch (iReplayType) {
        case REPLAY_LOCAL:
            m_csStorageDistribution.LoadString(IDS_SD_LOCAL);
            break;
        case REPLAY_SERVER:
            switch (iServerType) {
                case SRV_FILE_SERVER:
                    m_csStorageDistribution.LoadString(IDS_SD_FILE_SERVER);
                    break;
                case SRV_WEB_SERVER:
                    if (iTransferType == TRANSFER_NETWORK_DRIVE)
                        m_csStorageDistribution.LoadString(IDS_SD_WEBSERVER);
                    else
                        m_csStorageDistribution.LoadString(IDS_SD_WEBSERVER_FT);
                    break;
                case SRV_STREAMING_SERVER:
                    if (iTransferType == TRANSFER_NETWORK_DRIVE)
                        m_csStorageDistribution.LoadString(IDS_SD_WEB_STREAMING);
                    else
                        m_csStorageDistribution.LoadString(IDS_SD_WEB_STREAMING_FT);
                    break;
                case SRV_PODCAST_SERVER:
                    m_csStorageDistribution.LoadString(IDS_SD_PODCAST);
                    break;
                case SRV_SLIDESTAR:
                    m_csStorageDistribution.LoadString(IDS_SD_SLIDESTAR);
                    break;
            }
            break;
    }
}

#include "stdafx.h"

#include <locale.h>

#include "Resource.h"
#include "ProfileUtils.h"
#include "YouTubeSettings.h"
#include "SlidestarSettings.h"
#include "MfcUtils.h"

#include "lutils.h"         // lutils
#include "KeyGenerator.h"   // keygenlib


static ProfileNameIDAndType g_aPossibleProfiles[] = {
    ProfileNameIDAndType(_T("Flash"), ID_PROFILE_FLASH, PublishingFormat::TYPE_FLASH, 0),
    ProfileNameIDAndType(_T("WindowsMedia"), ID_PROFILE_WM, PublishingFormat::TYPE_WINDOWSMEDIA, 1),
    ProfileNameIDAndType(_T("RealMedia"), ID_PROFILE_RM, PublishingFormat::TYPE_REALMEDIA, 2),
    ProfileNameIDAndType(_T("IPOD"), ID_PROFILE_IPOD, PublishingFormat::TYPE_IPOD, 3),
    ProfileNameIDAndType(_T("LPD"), ID_PROFILE_LPD, PublishingFormat::TYPE_LECTURNITY, 4),
    ProfileNameIDAndType(_T("YouTube"), ID_PROFILE_YOUTUBE, PublishingFormat::TYPE_YOUTUBE, 5),
    ProfileNameIDAndType(_T("Slidestar"), ID_PROFILE_SLIDESTAR, PublishingFormat::TYPE_SLIDESTAR, 6),
    ProfileNameIDAndType(_T("CLIX"), ID_PROFILE_CLIX, PublishingFormat::TYPE_CLIX, 7),
    ProfileNameIDAndType(_T("Custom"), ID_PROFILE_CUSTOM, PublishingFormat::TYPE_CUSTOM, 8)};
    
static int g_iPossibleProfilesCount = 9;
    
ProfileUtils::ProfileUtils(void) {
}

ProfileUtils::~ProfileUtils(void) {
}

/// public functions

HRESULT ProfileUtils::GetProfileDirectory(HKEY hKey, CString &csProfileDirectory, bool bCreate) {
    HRESULT hr = S_OK;

    CString csApplicationData;

    hr = GetApplicationDataDirectory(hKey, csApplicationData);

    if (SUCCEEDED(hr))
        hr = AddProfileSubDirectory(csApplicationData, csProfileDirectory, bCreate);

    return hr;
}

HRESULT ProfileUtils::GetDirectPublishProfile(int &iPublishingFormat) {
    HRESULT hr = S_OK;

    iPublishingFormat = PublishingFormat::TYPE_NOTHING;

    DWORD dwPublishingFormat;
    bool bResult = LRegistry::ReadDwordRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher"), 
        _T("DirectPublishFormat"), &dwPublishingFormat);

    if (!bResult)
        hr = E_FAIL;
    else
        iPublishingFormat = dwPublishingFormat;

    return hr;
}

HRESULT ProfileUtils::SetDirectPublishProfile(int &iPublishingFormat) {
    HRESULT hr = S_OK;

    DWORD dwPublishingFormat = (DWORD)iPublishingFormat;
    bool bResult = LRegistry::WriteDwordRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Publisher"), 
        _T("DirectPublishFormat"), dwPublishingFormat);

    if (!bResult)
        hr = E_FAIL;

    return hr;
}

HRESULT ProfileUtils::ResetActiveProfile() {
    HRESULT hr = S_OK;

    bool bResult = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles"),
        _T("Active"), _T(""));

    if (!bResult)
        hr = E_FAIL;

    return hr;
}

HRESULT ProfileUtils::ReadActiveProfile(CString &csFilename)
{
    HRESULT hr = S_OK;

    // Get active profile from registry
    _TCHAR tszActiveProfile[MAX_PATH];
    unsigned long lLength = MAX_PATH;
    bool bRet = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles"), 
        _T("Active"), tszActiveProfile, &lLength);

    if (bRet == false)
        tszActiveProfile[0] = _T('\0');

    if (SUCCEEDED(hr)) {
        csFilename = tszActiveProfile;
        if (csFilename.IsEmpty())
            hr = E_FAIL;
    }

    return hr;
}
HRESULT ProfileUtils::GetActiveProfileID(__int64 &lProfileID) {
    HRESULT hr = S_OK;

    CString csFilename;
    hr = ReadActiveProfile(csFilename);

    ProfileInfo *pInfo = NULL;
    if (SUCCEEDED(hr))
        pInfo = new ProfileInfo(csFilename);
    
    if (pInfo == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr))
        hr = pInfo->Read();
    
    lProfileID = 0;
    if (SUCCEEDED(hr))
        lProfileID = pInfo->GetID();

    if (pInfo != NULL)
        delete pInfo;

    return hr;
}

HRESULT ProfileUtils::ReadCustomProfiles(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation) {
    HRESULT hr = S_OK;

    CArray<ProfileInfo *, ProfileInfo *> aAllProfileInformation;
    hr = ReadProfiles(aAllProfileInformation);

    if (SUCCEEDED(hr)) {
        for (int i = 0; i < aAllProfileInformation.GetSize(); ++i) {
            // default profiles have IDs betweeen 0 and 3000
            if (aAllProfileInformation[i]->GetID() > 0 && 
                aAllProfileInformation[i]->GetID() < DEFAULT_PROFILE_MAX)
                delete aAllProfileInformation[i];
            else 
                aProfileInformation.Add(aAllProfileInformation[i]);
        }
        aAllProfileInformation.RemoveAll();
    }

    return hr;
}

HRESULT ProfileUtils::WriteActiveProfile(CString &csFilename) {
    HRESULT hr = S_OK;

    bool bResult = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles"),
        _T("Active"), csFilename);

    if (!bResult)
        hr = E_FAIL;

    return hr;
}

HRESULT ProfileUtils::ReadProfiles(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation) {
    HRESULT hr = S_OK;

    // Search for profiles in All Users application directory
    CString csAllUserApplicationData;
    hr = GetProfileDirectory(HKEY_LOCAL_MACHINE, csAllUserApplicationData, false);

    if (SUCCEEDED(hr)) {
        CString csAllUserApplicationDataFiles = csAllUserApplicationData + _T("\\*.lpp");

        WIN32_FIND_DATA ffd;
        HANDLE hFile = FindFirstFile((LPCTSTR)csAllUserApplicationDataFiles, &ffd);

        if (hFile != INVALID_HANDLE_VALUE) {             
            do {
                CString csFile = csAllUserApplicationData + _T("\\") + ffd.cFileName;
                ProfileInfo *pNewProfileInfo = new ProfileInfo(csFile);
                pNewProfileInfo->Read();
                pNewProfileInfo->SetCreatedByAdmin(true);
                aProfileInformation.Add(pNewProfileInfo);
            } while (FindNextFile(hFile, &ffd));
        } 
        FindClose(hFile);
    }

    // Search for profiles in user application directory
    CString csUserApplicationData;
    if (SUCCEEDED(hr)) {
        hr = GetProfileDirectory(HKEY_CURRENT_USER, csUserApplicationData, false);
    }

    if (SUCCEEDED(hr)) {
        CString csUserApplicationDataFiles = csUserApplicationData + _T("\\*.lpp");

        WIN32_FIND_DATA ffd;
        HANDLE hFile = FindFirstFile((LPCTSTR)csUserApplicationDataFiles, &ffd);

        if (hFile != INVALID_HANDLE_VALUE) {  
            do {
                CString csFile = csUserApplicationData + _T("\\") + ffd.cFileName;
                ProfileInfo *pNewProfileInfo = new ProfileInfo(csFile);
                pNewProfileInfo->Read();
                pNewProfileInfo->SetCreatedByAdmin(false);
                aProfileInformation.Add(pNewProfileInfo);
            } while (FindNextFile(hFile, &ffd));
        } 

        FindClose(hFile);
    }

    return hr;
}

HRESULT ProfileUtils::GetRecentProfiles(CArray<RegistryProfileInfo *, RegistryProfileInfo *> &arRecentProfiles) {
    HRESULT hr = S_OK;

    arRecentProfiles.RemoveAll();

    _TCHAR tszActiveProfile[MAX_PATH];
    unsigned long lLength = MAX_PATH;
    
    DWORD dwEntriesCount = 0;
    bool bSuccess = LRegistry::EnumRegistryValues(HKEY_CURRENT_USER,
                                  _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Recent\\List"), 
                                  NULL, &dwEntriesCount, false);

    if (!bSuccess)
        hr = E_FAIL;

    LRegistry::ENTRY **paEntries = NULL;
    if (SUCCEEDED(hr)) {
        paEntries = new LRegistry::ENTRY*[dwEntriesCount+1];
        for (int i = 0; i < dwEntriesCount; ++i)
            paEntries[i] = new LRegistry::ENTRY();
    }

    if (SUCCEEDED(hr)) {
        bSuccess = LRegistry::EnumRegistryValues(HKEY_CURRENT_USER,
            _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Recent\\List"), 
            paEntries, &dwEntriesCount, false);

        if (!bSuccess)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        for (int i = 0; i < dwEntriesCount; ++i) {
            CString csName = paEntries[i]->lsName;

            bool bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
                _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Recent\\List"), 
                csName, tszActiveProfile, &lLength);
            if (bSuccess && lLength > 0) {
                RegistryProfileInfo *pNewProfileInfo = new RegistryProfileInfo(csName, tszActiveProfile);
                arRecentProfiles.Add(pNewProfileInfo);
                int k = 0;
            }
        }
    }

    if (paEntries != NULL) {
        for (int i = 0; i < dwEntriesCount; ++i)
            delete paEntries[i];
        delete[] paEntries;
    }

    if (SUCCEEDED(hr)) {
        // check recent profiles (existance)
        CArray<ProfileInfo *, ProfileInfo *> aProfileInformation;
        hr = ReadCustomProfiles(aProfileInformation);
        CArray<int, int> arPositionsToDelete;

        // Remove all custom profiles which are no longer existing
        arPositionsToDelete.RemoveAll();
        for (int i = 0; i < arRecentProfiles.GetCount(); ++i) {
            if (arRecentProfiles[i]->GetProfileType() == PublishingFormat::TYPE_CUSTOM) {
                bool bFound = false;
                for (int j = 0; j < aProfileInformation.GetCount() && !bFound; ++j) {
                    if (arRecentProfiles[i]->GetCustomProfileID() == aProfileInformation[j]->GetID())
                        bFound = true;
                }
                if (!bFound)
                    arPositionsToDelete.Add(i);
            }
        }

        for (int i = arPositionsToDelete.GetCount()-1; i >= 0; --i) {
            if (arRecentProfiles[arPositionsToDelete[i]] != NULL)
                delete arRecentProfiles[arPositionsToDelete[i]];
            arRecentProfiles.RemoveAt(arPositionsToDelete[i]);
        }


        // Remove old custom profile if there are more than five
        bool bFinished = false;
        while (!bFinished) {
            int iCustomProfileCount = 0;
            CTime *pOldest = NULL;
            int iOldestPos = -1;
            for (int i = 0; i < arRecentProfiles.GetCount(); ++i) {
                if (arRecentProfiles[i]->GetProfileType() == PublishingFormat::TYPE_CUSTOM) {
                    CTime *pElementTime = arRecentProfiles[i]->GetLastActivatedTime();
                    if (pOldest == NULL || 
                        (pElementTime != NULL && *pElementTime < *pOldest)) {
                            pOldest = pElementTime;
                            iOldestPos = i;
                    }
                    ++iCustomProfileCount;
                }
            }

            if (iCustomProfileCount <= 5 || iOldestPos < 0)
                bFinished = true;
            else {
                if (arRecentProfiles[iOldestPos] != NULL)
                    delete arRecentProfiles[iOldestPos];
                arRecentProfiles.RemoveAt(iOldestPos);
            }
        }

        for (int i = 0; i < aProfileInformation.GetCount(); ++i)
            if (aProfileInformation[i] != NULL)
                delete aProfileInformation[i];
        aProfileInformation.RemoveAll();
    }

    return S_OK;
}

HRESULT ProfileUtils::SetRecentProfiles(CArray<RegistryProfileInfo *, RegistryProfileInfo *> &arRecentProfiles) {
    HRESULT hr = S_OK; 
    
    int iCustomCount = 0;
    
    LRegistry::DeleteRegistryKey(HKEY_CURRENT_USER, _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Recent\\List"));

    for (int i = 0; i < arRecentProfiles.GetSize(); ++i) {
        if (arRecentProfiles[i] != NULL) {
            CString csRegistryEntry;
            arRecentProfiles[i]->CreateRegistryEntry(csRegistryEntry);
            CString csName = arRecentProfiles[i]->GetName();
            if (arRecentProfiles[i]->GetProfileType() == PublishingFormat::TYPE_CUSTOM) {
                CString csCount;
                csCount.Format(_T(" %d"), iCustomCount);
                csName += csCount;
                ++iCustomCount;
            }
            bool bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
                                _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Recent\\List"), 
                                csName, csRegistryEntry);
        }
    }

    return hr;
}
    
HRESULT ProfileUtils::GetLastActivatedProfile(CString &csLastActivated) { 
    HRESULT hr = S_OK;

    int iProfileType;
    hr = GetDirectPublishProfile(iProfileType);

    if (SUCCEEDED(hr))
        hr = GetProfileNameFromID(iProfileType, csLastActivated, false);

    return hr;
}

HRESULT ProfileUtils::GetLastActivatedProfile(CString &csLastActivated, __int64 &lCustomProfileID) {
    HRESULT hr = S_OK;

    hr = GetLastActivatedProfile(csLastActivated);

    lCustomProfileID = 0;
    if (csLastActivated == _T("Custom"))
        hr = ProfileUtils::GetActiveProfileID(lCustomProfileID);

    return hr;
}

HRESULT ProfileUtils::SetLastActivatedProfile(int iDefaultProfileID, __int64 lCustomProfileID, bool bIsCommandID) {
    HRESULT hr = S_OK;

    CString csLastActivated;
    hr = ProfileUtils::GetProfileNameFromID(iDefaultProfileID, csLastActivated, bIsCommandID);
    if (SUCCEEDED(hr) && !csLastActivated.IsEmpty())
        hr = SetLastActivatedProfile(csLastActivated, lCustomProfileID);

    return hr;
}

HRESULT ProfileUtils::SetLastActivatedProfile(CString csLastActivated, __int64 lCustomProfileID) {
    HRESULT hr = S_OK; 

    int iProfileType = GetProfileTypeFromName(csLastActivated);
    hr = SetDirectPublishProfile(iProfileType);

    if (iProfileType == PublishingFormat::TYPE_CUSTOM) {
        ProfileInfo *pInfo = ProfileUtils::GetProfileWithID(lCustomProfileID);

        if (pInfo) {
            hr = ProfileUtils::WriteActiveProfile(pInfo->GetFilename());
            delete pInfo;
        } else
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        AddProfileToRecentList(iProfileType);
    }

    return hr;
}

HRESULT ProfileUtils::SetProfilePublished(int iDefaultProfileID, CString csDocTitle) {
    HRESULT hr = S_OK;

    CString csLastActivated;
    hr = ProfileUtils::GetProfileNameFromID(iDefaultProfileID, csLastActivated, false);

    CArray<RegistryProfileInfo *, RegistryProfileInfo *> arRecentProfile;
    if (SUCCEEDED(hr))
        hr = GetRecentProfiles(arRecentProfile);

    if (SUCCEEDED(hr)) {
        bool bFound = false;
        for (int i = 0; i < arRecentProfile.GetSize(); ++i) {
            if (arRecentProfile[i]->GetName() == csLastActivated) {
                arRecentProfile[i]->ResetNotUsedCount();
                arRecentProfile[i]->UpdatePublishingTime();
                arRecentProfile[i]->SetLastPublishedDoc(csDocTitle);
                bFound = true;
            } else {
                arRecentProfile[i]->IncrementNotUsedCount();
            }
        }

        if (!bFound) {
            __int64 lCustomProfileID = 0;
            if (csLastActivated != _T("Custom"))
                ProfileUtils::GetActiveProfileID(lCustomProfileID);
            RegistryProfileInfo *pNewProfile = new RegistryProfileInfo(csLastActivated, lCustomProfileID);
            pNewProfile->UpdatePublishingTime();
            pNewProfile->SetLastPublishedDoc(csDocTitle);
            arRecentProfile.Add(pNewProfile);
        }
    }

    if (SUCCEEDED(hr))
        hr = SetRecentProfiles(arRecentProfile);

    for (int i = 0; i < arRecentProfile.GetSize(); ++i)
        if (arRecentProfile[i] != NULL)
            delete arRecentProfile[i];
    arRecentProfile.RemoveAll();

    return hr;
}

HRESULT ProfileUtils::AddProfileToRecentList(int iDefaultProfileID) {
    HRESULT hr = S_OK;

    CString csLastActivated;
    hr = ProfileUtils::GetProfileNameFromID(iDefaultProfileID, csLastActivated, false);
    __int64 lCustomProfileID = 0;

    if (SUCCEEDED(hr)) {
        if (csLastActivated == _T("Custom"))
            hr = ProfileUtils::GetActiveProfileID(lCustomProfileID);
    }

    CArray<RegistryProfileInfo *, RegistryProfileInfo *> arRecentProfile;
    if (SUCCEEDED(hr))
        hr = GetRecentProfiles(arRecentProfile);

    if (SUCCEEDED(hr)) {
        bool bFound = false;
        for (int i = 0; i < arRecentProfile.GetSize(); ++i) {
            if (arRecentProfile[i] != NULL &&
                arRecentProfile[i]->GetName() == csLastActivated &&
                arRecentProfile[i]->GetCustomProfileID() == lCustomProfileID) {
                arRecentProfile[i]->UpdateLastActivatedTime();
                bFound = true;
            }
        }

        if (!bFound) {
            RegistryProfileInfo *pNewProfile = new RegistryProfileInfo(csLastActivated, lCustomProfileID);
            pNewProfile->UpdateLastActivatedTime();
            arRecentProfile.Add(pNewProfile);
        }
    }

    if (SUCCEEDED(hr))
        hr = SetRecentProfiles(arRecentProfile);

    for (int i = 0; i < arRecentProfile.GetSize(); ++i)
        if (arRecentProfile[i] != NULL)
            delete arRecentProfile[i];
    arRecentProfile.RemoveAll();

    return hr;
}

int ProfileUtils::GetDefaultProfileIDFromName(CString csProfileName) {
    for (int i = 0; i < g_iPossibleProfilesCount; ++i) {
        if (g_aPossibleProfiles[i].GetName() == csProfileName)
            return g_aPossibleProfiles[i].GetDefaultProfileID();
    }

    return -1;
}

int ProfileUtils::GetProfileTypeFromName(CString csProfileName) {
    for (int i = 0; i < g_iPossibleProfilesCount; ++i) {
        if (g_aPossibleProfiles[i].GetName() == csProfileName)
            return g_aPossibleProfiles[i].GetProfileType();
    }

    return -1;
}

int ProfileUtils::GetImageIDFromName(CString csProfileName) {
    for (int i = 0; i < g_iPossibleProfilesCount; ++i) {
        if (g_aPossibleProfiles[i].GetName() == csProfileName)
            return g_aPossibleProfiles[i].GetImageID();
    }

    return -1;
}

HRESULT ProfileUtils::GetProfileNameFromID(int iDefaultProfileID, CString &csProfileName, bool bIsCommandID) {
    HRESULT hr = S_OK;

    csProfileName.Empty();
    if (bIsCommandID) {
        bool bFound = false;
        for (int i = 0; i < g_iPossibleProfilesCount && !bFound; ++i) {
            if (g_aPossibleProfiles[i].GetDefaultProfileID() == iDefaultProfileID) {
                csProfileName = g_aPossibleProfiles[i].GetName();
                bFound = true;
            }
        }
        if (!bFound)
            hr = E_FAIL;
    } else {
        bool bFound = false;
        for (int i = 0; i < g_iPossibleProfilesCount && !bFound; ++i) {
            if (g_aPossibleProfiles[i].GetProfileType() == iDefaultProfileID) {
                csProfileName = g_aPossibleProfiles[i].GetName();
                bFound = true;
            }
        }
        if (!bFound)
            hr = E_FAIL;
    }

    return hr;
}

void ProfileUtils::GetAllPossibleProfiles(CStringArray &aPossibleProfiles) {
    aPossibleProfiles.RemoveAll();
    
    for (int i = 0; i < g_iPossibleProfilesCount; ++i) 
        aPossibleProfiles.Add(g_aPossibleProfiles[i].GetName());
}

HRESULT ProfileUtils::ParseProfileMessage(CString csMessage, bool &bCancel, __int64 &iProfileID) {
    HRESULT hr = S_OK;

    bCancel = true;
    iProfileID = 0;

    CString csPart1, csPart2, csPart3;
    int iFirstBlank = csMessage.Find(_T(" "));
    if (iFirstBlank >= 0)
        csPart1 = csMessage.Mid(0, iFirstBlank);
    else
        hr = E_FAIL;

    int iSecondBlank = 0;
    if (SUCCEEDED(hr)) {
        iSecondBlank = csMessage.Find(_T(" "), iFirstBlank+1);
        if (iSecondBlank >= 0)
            csPart2 = csMessage.Mid(iFirstBlank+1, (iSecondBlank-(iFirstBlank+1)));
        else
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr) && iSecondBlank+1 < csMessage.GetLength())
        csPart3 = csMessage.Mid(iSecondBlank+1, (csMessage.GetLength()-(iSecondBlank+1)));
    else
        hr = E_FAIL;

    if (SUCCEEDED(hr) && csPart1 == _T("Profile")) {
        iProfileID = _ttoi64(csPart2);
        if (csPart3 == _T("SUCCESS")) 
            bCancel = false;
        else if (csPart3 == _T("CANCEL"))
            bCancel = true;
    }

    return hr;
}

RegistryProfileInfo *ProfileUtils::GetInfoFromActiveProfile() {
    CString csLastActivated;
    __int64 lCustomProfileID = 0;
    ProfileUtils::GetLastActivatedProfile(csLastActivated, lCustomProfileID);

    CArray<RegistryProfileInfo *, RegistryProfileInfo *> arRecentProfiles;
    ProfileUtils::GetRecentProfiles(arRecentProfiles);
    RegistryProfileInfo *pActivatedProfile = NULL;
    for (int i = 0; i < arRecentProfiles.GetCount() && !pActivatedProfile; ++i) {
        if (arRecentProfiles[i]->GetName() == csLastActivated && 
            arRecentProfiles[i]->GetCustomProfileID() == lCustomProfileID)
            pActivatedProfile = arRecentProfiles[i]->Copy();
    }

    if (pActivatedProfile == NULL) {
        pActivatedProfile = new RegistryProfileInfo(csLastActivated, _T(""));
    }
    for (int i = 0; i < arRecentProfiles.GetCount(); ++i) {
        if (arRecentProfiles[i])
            delete arRecentProfiles[i];
    }

    return pActivatedProfile;
}

ProfileInfo *ProfileUtils::GetProfileWithID(__int64 iProfileID) {
    HRESULT hr = S_OK;

    ProfileInfo *pProfileInfo = NULL;

    CArray<ProfileInfo *, ProfileInfo *> arProfile;
    hr = ReadCustomProfiles(arProfile);

    if (SUCCEEDED(hr)) {
        for (int i = 0; i < arProfile.GetSize() && pProfileInfo == NULL; ++i) {
            if (arProfile[i]->GetID() == iProfileID) {
                pProfileInfo = new ProfileInfo(arProfile[i]);
                break;
            }
        }
    }

    for (int i = 0; i < arProfile.GetSize(); ++i)
        if (arProfile[i] != NULL)
            delete arProfile[i];
    arProfile.RemoveAll();

    return pProfileInfo;
}

HRESULT ProfileUtils::GetNewProfileID(__int64 &iNewProfileID) {
    HRESULT hr = S_OK;

    // REVIEW UK:
    // Is it really necessary to read and delete all profiles for any method call?
    // "Magic number" 3000; should be a constant/documented
    // arProfile should not be an array but a map; using the id as key
    //  -> then the bIDAlreadyUsed loop would not be necessary
    // Variable name bCorrectID: better bUnusedIdFound
    // Variable name iNewProfileID: it is really a long: lNewProfileId

    CArray<ProfileInfo *, ProfileInfo *> arProfile;
    hr = ReadCustomProfiles(arProfile);

    bool bCorrectID = false;
    while (SUCCEEDED(hr) && !bCorrectID) {
        iNewProfileID = 0;
        if (SUCCEEDED(hr)) {
            while (iNewProfileID >= 0 && iNewProfileID < 3000 && SUCCEEDED(hr)) {
                iNewProfileID = LMisc::GetRandomValue();
                if (iNewProfileID == 0)
                    hr = E_FAIL;
            }
        }

        bool bIDAlreadyUsed = false;
        if (SUCCEEDED(hr)) {
            for (int i = 0; i < arProfile.GetSize() && !bIDAlreadyUsed; ++i) {
                if (arProfile[i]->GetID() == iNewProfileID)
                    bIDAlreadyUsed = true;
            }
        }

        if (!bIDAlreadyUsed)
            bCorrectID = true;
    }

    for (int i = 0; i < arProfile.GetSize(); ++i)
        if (arProfile[i] != NULL)
            delete arProfile[i];
    arProfile.RemoveAll();

    return hr;
}

HRESULT ProfileUtils::HandlePublisherFinish(CWnd *pCallingWindow) {
    
    CWinThread *pThread = AfxBeginThread(ProfileUtils::HandlePublisherFinishThread, pCallingWindow);
 
    return S_OK;
}

UINT ProfileUtils::HandlePublisherFinishThread(LPVOID pParam) {
    
    _TCHAR tszMessage[256];

    LIo::MessagePipe pipeExe;
    HRESULT hrPipe = pipeExe.Init(_T("publisher_exe_pipe"), true);
  
    bool bSomethingReceived = false;
    if (SUCCEEDED(hrPipe))
        bSomethingReceived = LMisc::WaitForPublisherJavaFinish(&pipeExe, tszMessage, 256, NULL);

    // Send this anyway: will also handle failed program start.
    // If there is a thread waiting for a profile pipe message, send cancel
    // If there id no thread waiting for this message nothing happens
    _stprintf(tszMessage, _T("Profile 0 CANCEL"));

    LIo::MessagePipe pipeMsg;
    hrPipe = pipeMsg.Init(_T("publisher_msg_pipe"), false);
    if (SUCCEEDED(hrPipe))
        pipeMsg.WriteMessage(tszMessage);

    return 0;
}

HRESULT ProfileUtils::WaitForProfile(IProfileFinishListener *pListener) {
    CWinThread *pThread = AfxBeginThread(ProfileUtils::WaitForProfileThread, pListener);
   
    return S_OK;
}

UINT ProfileUtils::WaitForProfileThread(LPVOID pParam) {

    LIo::MessagePipe pipe;
    HRESULT hrPipe = pipe.Init(_T("publisher_msg_pipe"), true);

    _TCHAR tszMessage[256];
    tszMessage[0] = 0;
    if (SUCCEEDED(hrPipe))
        hrPipe = pipe.ReadMessage(tszMessage, 256);

    CString csMessage =  _T("Profile 0 CANCEL");
    if (SUCCEEDED(hrPipe))
        csMessage = tszMessage;

    IProfileFinishListener *pListener = (IProfileFinishListener *)pParam;
    if (pListener != NULL)
        pListener->PublisherFinished(csMessage);

    return 0;
}

int ProfileUtils::ShowYouTubeSettings() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

    CYouTubeSettings dlgYouTubeSettings;
    int returnID = dlgYouTubeSettings.DoModal();

    return returnID;
}

int ProfileUtils::ShowSlidestarSettings() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

    CSlidestarSettings dlgSlidestarSettings;
    int returnID = dlgSlidestarSettings.DoModal();

    return returnID;
}

void ProfileUtils::ShowErrorMessage(UINT iActionMessageID, UINT iErrorMessageID, UINT iCaptionID) {
    CString csCaption;
    if (iCaptionID > 0)
        csCaption.LoadString(iCaptionID);
    else
        csCaption.LoadString(IDS_ERROR_CAPTION);

    CString csError;
    if (iErrorMessageID > 0)
        csError.LoadString(iErrorMessageID);

    CString csMessage;
    csMessage.LoadString(iActionMessageID);
    if (iErrorMessageID > 0)
        csMessage.Format(_T("%s\n\n%s"), csMessage, csError);
    MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
}

/// private functions

HRESULT ProfileUtils::GetApplicationDataDirectory(HKEY hMainKey, CString &csApplicationData) {
    HRESULT hr = S_OK;

    _TCHAR szFolderPath[MAX_PATH];

    BOOL bRet = TRUE;
    if (hMainKey == HKEY_LOCAL_MACHINE)
        bRet = SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_COMMON_APPDATA , TRUE);
    else
        bRet = SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_APPDATA, TRUE);

    csApplicationData = szFolderPath;

    if (bRet == FALSE)
        hr = E_PM_GET_APPLDIR;

    return hr;
}

HRESULT ProfileUtils::AddProfileSubDirectory(CString &csApplicationDataDirectory, CString &csProfileDirectory, bool bCreate) {
    HRESULT hr = S_OK;
    bool bCreateSuccess = false;

    if (SUCCEEDED(hr)) {
        csProfileDirectory = csApplicationDataDirectory;
        if (bCreate && !DirectoryExists(csProfileDirectory, _T("Lecturnity"))) {
            bCreateSuccess = LIo::CreateDirs(csProfileDirectory, _T("Lecturnity"));
            if (!bCreateSuccess)
                hr = E_PM_CREATE_SUBDIR;
        }
    }

    if (SUCCEEDED(hr)) {
        csProfileDirectory += _T("\\Lecturnity");
        if (bCreate && !DirectoryExists(csProfileDirectory, _T("Publisher"))) {
            bCreateSuccess = LIo::CreateDirs(csProfileDirectory, _T("Publisher"));
            if (!bCreateSuccess)
                hr = E_PM_CREATE_SUBDIR;
        }
    }

    if (SUCCEEDED(hr)) {
        csProfileDirectory += _T("\\Publisher");
        if (bCreate && !DirectoryExists(csProfileDirectory, _T("Profiles"))) {
            bCreateSuccess = LIo::CreateDirs(csProfileDirectory, _T("Profiles"));
            if (!bCreateSuccess)
                hr = E_PM_CREATE_SUBDIR;
        }
    }

    if (SUCCEEDED(hr)) {
        csProfileDirectory += _T("\\Profiles");
    }

    return hr;
}

bool ProfileUtils::DirectoryExists(const _TCHAR *tszBaseDirectory, const _TCHAR *tszSubDirectory) {
    bool bRet = false;

    CString csDirectory = tszBaseDirectory;
    csDirectory += "\\";
    csDirectory += tszSubDirectory;

#ifdef _UNICODE
    if (_waccess((LPCTSTR)csDirectory, 06) == 0)
#else
    if (_access((LPCSTR)csDirectory, 06) == 0)
#endif
        bRet = true;

    return bRet;
}

/// RegistryProfileInfo class

RegistryProfileInfo::RegistryProfileInfo(void) {
    m_iNotUsedCount = 0;
    m_pLastPublished = NULL;
    m_pLastActivated = NULL;
    m_csProfileName.Empty();
    m_csProfileDescription.Empty();
    m_csLastPublishedDoc.Empty();
    m_lCustomProfileID = 0;
}

RegistryProfileInfo::RegistryProfileInfo(CString csProfileName, __int64 lCustomProfileID) {
    m_iNotUsedCount = 0;
    m_pLastPublished = NULL;
    m_pLastActivated = NULL;

    m_csProfileName = csProfileName;
    int iBlankPos = m_csProfileName.Find(_T(" "));
    // If the profile is a custom profile there is an additional 
    // counter appended to the profile name
    if (iBlankPos > 0)
        m_csProfileName = m_csProfileName.Mid(0, iBlankPos);

    m_csProfileDescription.Empty();
    m_csLastPublishedDoc.Empty();
    m_lCustomProfileID = lCustomProfileID;
}

RegistryProfileInfo::RegistryProfileInfo(CString csProfileName, CString csProfileInformation) {
    m_iNotUsedCount = 0;
    m_pLastPublished = NULL;
    m_pLastActivated = NULL;
    m_csProfileDescription.Empty();
    m_csLastPublishedDoc.Empty();
    m_lCustomProfileID = 0;

    m_csProfileName = csProfileName;
    int iBlankPos = m_csProfileName.Find(_T(" "));
    // If the profile is a custom profile there is an additional 
    // counter appended to the profile name
    if (iBlankPos > 0)
        m_csProfileName = m_csProfileName.Mid(0, iBlankPos);

    if (!csProfileInformation.IsEmpty()) {
        CString csLastPublishedDate;
        ExtractLastPublishedTime(csProfileInformation, csLastPublishedDate);
        if (!csLastPublishedDate.IsEmpty())
            ExtractTimeFromString(csLastPublishedDate, &m_pLastPublished);

        CString csNotUsedCount;
        ExtractNotUsedCountTime(csProfileInformation, csNotUsedCount);
        if (!csNotUsedCount.IsEmpty()) 
            m_iNotUsedCount = _ttoi(csNotUsedCount);

        
        CString csLastUsedDocument;
        ExtractLastUsedDocument(csProfileInformation, csLastUsedDocument);
        if (!csLastUsedDocument.IsEmpty())
            m_csLastPublishedDoc = csLastUsedDocument;

        CString csCustomProfileID;
        ExtractCustomProfileID(csProfileInformation, csCustomProfileID);
        if (!csCustomProfileID.IsEmpty()) {
            _int64 lProfileID = _ttoi64(csCustomProfileID);
            m_lCustomProfileID = lProfileID;
        }
        
        CString csLastActivatedTime;
        ExtractLastActivatedTime(csProfileInformation, csLastActivatedTime);
        if (!csLastActivatedTime.IsEmpty()) 
            ExtractTimeFromString(csLastActivatedTime, &m_pLastActivated);
    }
}

RegistryProfileInfo::~RegistryProfileInfo(void) {
    if (m_pLastPublished)
        delete m_pLastPublished;
    m_pLastPublished = NULL;

    if (m_pLastActivated)
        delete m_pLastActivated;
    m_pLastActivated = NULL;
}

RegistryProfileInfo *RegistryProfileInfo::Copy() {
    RegistryProfileInfo *pCopiedProfile = new RegistryProfileInfo();

    pCopiedProfile->SetName(m_csProfileName);
    pCopiedProfile->SetDescription(m_csProfileDescription);
    pCopiedProfile->SetLastPublishedDoc(m_csLastPublishedDoc);
    pCopiedProfile->SetNotUsedCount(m_iNotUsedCount);
    pCopiedProfile->SetLastPublishedTime(m_pLastPublished);
    pCopiedProfile->SetLastActivatedTime(m_pLastActivated);
    pCopiedProfile->SetCustomProfileID(m_lCustomProfileID);

    return pCopiedProfile;
}

void RegistryProfileInfo::CreateRegistryEntry(CString &csRegistryEntry) {
    CString csLastPublished = _T("0_0_0_0_0_0");
    if (m_pLastPublished != NULL) 
        csLastPublished.Format(_T("%d_%d_%d_%d_%d_%d"), 
                               m_pLastPublished->GetDay(), m_pLastPublished->GetMonth(), 
                               m_pLastPublished->GetYear(), m_pLastPublished->GetHour(),
                               m_pLastPublished->GetMinute(), m_pLastPublished->GetSecond());
    
    CString csLastActivated = _T("0_0_0_0_0_0");
    if (m_pLastActivated != NULL) 
        csLastActivated.Format(_T("%d_%d_%d_%d_%d_%d"), 
                               m_pLastActivated->GetDay(), m_pLastActivated->GetMonth(), 
                               m_pLastActivated->GetYear(), m_pLastActivated->GetHour(),
                               m_pLastActivated->GetMinute(), m_pLastActivated->GetSecond());


    if (m_csProfileName != _T("Custom"))
        csRegistryEntry.Format(_T("%s:%d:%s"), csLastPublished, m_iNotUsedCount,
                                               m_csLastPublishedDoc);
    else
        csRegistryEntry.Format(_T("%s:%d:%s:%I64d:%s"), csLastPublished, 
                                                        m_iNotUsedCount, m_csLastPublishedDoc, 
                                                        m_lCustomProfileID, csLastActivated);
}

void RegistryProfileInfo::SetLastPublishedTime(CTime *pLastPublished) {
    if (pLastPublished == NULL)
        return;

    if (m_pLastPublished)
        delete m_pLastPublished;

    m_pLastPublished = new CTime(pLastPublished->GetYear(), pLastPublished->GetMonth(), 
                                 pLastPublished->GetDay(),  pLastPublished->GetHour(), 
                                 pLastPublished->GetMinute(), pLastPublished->GetSecond());
}

void RegistryProfileInfo::SetLastActivatedTime(CTime *pLastActivated) {
    if (pLastActivated == NULL)
        return;

    if (m_pLastActivated)
        delete m_pLastActivated;

    m_pLastActivated = new CTime(pLastActivated->GetYear(), pLastActivated->GetMonth(), 
                                 pLastActivated->GetDay(),  pLastActivated->GetHour(), 
                                 pLastActivated->GetMinute(), pLastActivated->GetSecond());
}

void RegistryProfileInfo::UpdatePublishingTime() {

    if (m_pLastPublished)
        delete m_pLastPublished;
    m_pLastPublished = NULL;

    m_pLastPublished = new CTime(CTime::GetCurrentTime());

    // if the document is published both times are updated.
    UpdateLastActivatedTime();
}

void RegistryProfileInfo::UpdateLastActivatedTime() {
    if (m_pLastActivated)
        delete m_pLastActivated;
    m_pLastActivated = NULL;

    m_pLastActivated = new CTime(CTime::GetCurrentTime());
}

void RegistryProfileInfo::GetLocalizedName(CString &csName) {

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    int iDefaultProfileID = ProfileUtils::GetDefaultProfileIDFromName(m_csProfileName);
    if (iDefaultProfileID != -1)
        csName.LoadString(iDefaultProfileID);
}

void RegistryProfileInfo::GetDescription(CString &csDescription) {

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    csDescription.Empty();

    int iDefaultProfileID = ProfileUtils::GetDefaultProfileIDFromName(m_csProfileName);
    switch(iDefaultProfileID) {
        case ID_PROFILE_FLASH:
            csDescription.LoadString(IDS_FORMAT_FLASH);
            break;
        case ID_PROFILE_WM:
            csDescription.LoadString(IDS_FORMAT_WM);
            break;
        case ID_PROFILE_RM:
            csDescription.LoadString(IDS_FORMAT_RM);
            break;
        case ID_PROFILE_IPOD:
            csDescription.LoadString(IDS_FORMAT_IPOD);
            break;
        case ID_PROFILE_LPD:
            csDescription.LoadString(IDS_FORMAT_LPD);
            break;
        case ID_PROFILE_YOUTUBE:
            csDescription.LoadString(IDS_FORMAT_YOUTUBE);
            break;
        case ID_PROFILE_SLIDESTAR:
            csDescription.LoadString(IDS_FORMAT_SLIDESTAR);
            break;
        case ID_PROFILE_CUSTOM:
            csDescription.LoadString(IDS_FORMAT_CUSTOM);
            break;
    }
}

void RegistryProfileInfo::GetDate(CString &csDate) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    csDate.Empty();
    if (m_pLastPublished) {
        _TCHAR tszLanguageCode[128];
        bool ret = KerberokHandler::GetLanguageCode(tszLanguageCode);
        char *szOldLocale = NULL;
        if (_tcscmp(tszLanguageCode, _T("de")) == 0) {
            szOldLocale = setlocale(LC_TIME, "German");
            csDate = m_pLastPublished->Format( "%A, %d. %B %Y, %H:%M" );
        }
        else {
            szOldLocale = setlocale(LC_TIME, "English");
            csDate = m_pLastPublished->Format( "%A, %B %d, %Y, %H:%M" );
        }

        if (strlen(szOldLocale) > 0)
            setlocale(LC_TIME, szOldLocale);

    }
}

int RegistryProfileInfo::GetDefaultProfileID() {
    return ProfileUtils::GetDefaultProfileIDFromName(m_csProfileName);
}

int RegistryProfileInfo::GetProfileType() {
    return ProfileUtils::GetProfileTypeFromName(m_csProfileName);
}

int RegistryProfileInfo::GetImageID() {
    return ProfileUtils::GetImageIDFromName(m_csProfileName);
}

void RegistryProfileInfo::ExtractLastPublishedTime(CString csProfileInformation, CString &csLastPublishedDate) {

    int iFirstColonPos = csProfileInformation.Find(_T(":"));
    if (iFirstColonPos >= 0) {
        csLastPublishedDate = csProfileInformation.Mid(0, iFirstColonPos);
    }
}

void RegistryProfileInfo::ExtractNotUsedCountTime(CString csProfileInformation, CString &csNotUsedCount) {

    csNotUsedCount.Empty();

    int iFirstColonPos = csProfileInformation.Find(_T(":"));
    if (iFirstColonPos < 0)
        return;

    int iSecondColonPos = csProfileInformation.Find(_T(":"), iFirstColonPos+1);
    if (iSecondColonPos < 0)
        iSecondColonPos = csProfileInformation.GetLength();

    if (iSecondColonPos >= 0)
        csNotUsedCount = csProfileInformation.Mid(iFirstColonPos+1, iSecondColonPos - (iFirstColonPos+1));
}

void RegistryProfileInfo::ExtractLastUsedDocument(CString csProfileInformation, CString &csLastUsedDocument) {
    csLastUsedDocument.Empty();

    int iFirstColonPos = csProfileInformation.Find(_T(":"));
    if (iFirstColonPos < 0)
        return;

    int iSecondColonPos = csProfileInformation.Find(_T(":"), iFirstColonPos+1);
    if (iSecondColonPos < 0)
        return;
    
    int iThirdColonPos = csProfileInformation.Find(_T(":"), iSecondColonPos+1);
    if (iThirdColonPos < 0)
        iThirdColonPos = csProfileInformation.GetLength();

    if (iThirdColonPos >= 0)
        csLastUsedDocument = csProfileInformation.Mid(iSecondColonPos+1, 
                                                      iThirdColonPos - (iSecondColonPos+1));
}

void RegistryProfileInfo::ExtractCustomProfileID(CString csProfileInformation, CString &csCustomProfileID) {
    csCustomProfileID.Empty();

    int iFirstColonPos = csProfileInformation.Find(_T(":"));
    if (iFirstColonPos < 0)
        return;

    int iSecondColonPos = csProfileInformation.Find(_T(":"), iFirstColonPos+1);
    if (iSecondColonPos < 0)
        return;
    
    int iThirdColonPos = csProfileInformation.Find(_T(":"), iSecondColonPos+1);
    if (iThirdColonPos < 0)
        return;
    
    int iFourthColonPos = csProfileInformation.Find(_T(":"), iThirdColonPos+1);
    if (iFourthColonPos < 0)
        iFourthColonPos = csProfileInformation.GetLength();

    if (iFourthColonPos >= 0)
        csCustomProfileID = csProfileInformation.Mid(iThirdColonPos+1, 
                                                      iFourthColonPos - (iThirdColonPos+1));
}

void RegistryProfileInfo::ExtractLastActivatedTime(CString csProfileInformation, CString &csLastActivatedTime) {
    csLastActivatedTime.Empty();

    int iFirstColonPos = csProfileInformation.Find(_T(":"));
    if (iFirstColonPos < 0)
        return;

    int iSecondColonPos = csProfileInformation.Find(_T(":"), iFirstColonPos+1);
    if (iSecondColonPos < 0)
        return;
    
    int iThirdColonPos = csProfileInformation.Find(_T(":"), iSecondColonPos+1);
    if (iThirdColonPos < 0)
        return;
    
    int iFourthColonPos = csProfileInformation.Find(_T(":"), iThirdColonPos+1);
    if (iFourthColonPos < 0)
        return;
    
    int iFifthColonPos = csProfileInformation.Find(_T(":"), iFourthColonPos+1);
    if (iFifthColonPos < 0)
        iFifthColonPos = csProfileInformation.GetLength();

    if (iFifthColonPos >= 0)
        csLastActivatedTime = csProfileInformation.Mid(iFourthColonPos+1, 
                                                       iFifthColonPos - (iFourthColonPos+1));
}

void RegistryProfileInfo::ExtractTimeFromString(CString csTime, CTime **pTime) {

    HRESULT hr = S_OK;

    int iFirstUnderscorePos = csTime.Find(_T("_"));
    if (iFirstUnderscorePos < 0)
        hr = E_FAIL;
    
    int iDay = 0;
    if (SUCCEEDED(hr)) {
        CString csDay = csTime.Mid(0, iFirstUnderscorePos);
        iDay = _ttoi(csDay);
    }

    int iSecondUnderscorePos = 0;
    if (SUCCEEDED(hr)) {
        iSecondUnderscorePos = csTime.Find(_T("_"), iFirstUnderscorePos+1);
        if (iSecondUnderscorePos < 0)
            hr = E_FAIL;
    }

    int iMonth = 0;
    if (SUCCEEDED(hr)) {
        CString csMonth = csTime.Mid(iFirstUnderscorePos+1, 
                                     iSecondUnderscorePos - (iFirstUnderscorePos+1));
        iMonth = _ttoi(csMonth);
    }

    int iThirdUnderscorePos = 0;
    if (SUCCEEDED(hr)) {
        iThirdUnderscorePos = csTime.Find(_T("_"), iSecondUnderscorePos+1);
        if (iThirdUnderscorePos < 0)
            hr = E_FAIL;
    }

    int iYear = 0;
    if (SUCCEEDED(hr)) {
        CString csYear = csTime.Mid(iSecondUnderscorePos+1, 
                                    iThirdUnderscorePos - (iSecondUnderscorePos+1));
        iYear = _ttoi(csYear);
    }

    int iFourthUnderscorePos = 0;
    if (SUCCEEDED(hr)) {
        iFourthUnderscorePos = csTime.Find(_T("_"), iThirdUnderscorePos+1);
        if (iFourthUnderscorePos < 0)
            hr = E_FAIL;
    }

    int iHour = 0;
    if (SUCCEEDED(hr)) {
        CString csHour = csTime.Mid(iThirdUnderscorePos+1, 
            iFourthUnderscorePos - (iThirdUnderscorePos+1));
        iHour = _ttoi(csHour);
    }

    int iFifthUnderscorePos = 0;
    if (SUCCEEDED(hr)) {
        iFifthUnderscorePos = csTime.Find(_T("_"), iFourthUnderscorePos+1);
        if (iFifthUnderscorePos < 0)
            iFifthUnderscorePos = csTime.GetLength();
    }

    int iMinute = 0;
    if (SUCCEEDED(hr)) {
        CString csMinute = csTime.Mid(iFourthUnderscorePos+1, 
                                      iFifthUnderscorePos - (iFourthUnderscorePos+1));
        iMinute = _ttoi(csMinute);
    }

    int iSecond = 0;
    if (SUCCEEDED(hr) && iFifthUnderscorePos != csTime.GetLength()) {
        CString csSecond = csTime.Mid(iFifthUnderscorePos+1, 
                                      csTime.GetLength() - (iFifthUnderscorePos+1));
        iSecond = _ttoi(csSecond);
    }

    if (*pTime)
        delete *pTime;

    if (iYear != 0 && iMonth != 0 && iDay != 0)
        *pTime = new CTime(iYear, iMonth, iDay, iHour, iMinute, iSecond);
}

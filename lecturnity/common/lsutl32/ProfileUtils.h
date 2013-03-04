#pragma once

#include "ProfileInfo.h"

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif

#define DEFAULT_PROFILE_MAX 3000

class ProfileNameIDAndType
{
public:
    ProfileNameIDAndType(CString csName, int iDefaultProfileID, int iProfileType, int iImageID) {
        m_csName = csName;
        m_iDefaultProfileID = iDefaultProfileID;
        m_iProfileType = iProfileType;
        m_iImageID = iImageID;

    }
    ~ProfileNameIDAndType(void) {}

    CString &GetName() {return m_csName;}
    int GetDefaultProfileID() {return m_iDefaultProfileID;}
    int GetProfileType() {return m_iProfileType;}
    int GetImageID() {return m_iImageID;}

private:
    CString m_csName;
    int m_iDefaultProfileID;
    int m_iProfileType; 
    int m_iImageID;

};

class LSUTL32_EXPORT RegistryProfileInfo
{
public:
    RegistryProfileInfo(void);
    RegistryProfileInfo(CString csProfileName, __int64 lCustomProfileID);
    RegistryProfileInfo(CString csProfileName, CString csProfileInformation);
    ~RegistryProfileInfo(void);

    RegistryProfileInfo *Copy();
    int GetDefaultProfileID();
    int GetProfileType();
    int GetImageID();
    void SetName(CString csProfileName) {m_csProfileName = csProfileName;}
    CString &GetName() {return m_csProfileName;}
    void GetLocalizedName(CString &csName);
    void SetDescription(CString csDescription) {m_csProfileDescription = csDescription;}
    void GetDescription(CString &csDescription);
    void GetDate(CString &csDate);
    CString &GetLastPublishedDoc() {return m_csLastPublishedDoc;}
    void SetLastPublishedDoc(CString csLastPublishedDoc) {m_csLastPublishedDoc = csLastPublishedDoc;}
    void CreateRegistryEntry(CString &csRegistryEntry);
    void SetNotUsedCount(int iNotUsedCount) {m_iNotUsedCount = iNotUsedCount;}
    int GetNotUsedCount() {return m_iNotUsedCount;}
    void ResetNotUsedCount() {m_iNotUsedCount = 0;}
    void IncrementNotUsedCount() {++m_iNotUsedCount;}
    void UpdatePublishingTime();
    void UpdateLastActivatedTime();
    void SetLastPublishedTime(CTime *pLastUsed);
    void SetLastActivatedTime(CTime *pLastActivated);
    CTime *GetLastActivatedTime() {return m_pLastActivated;}
    void SetCustomProfileID(__int64 lCustomProfileID) {m_lCustomProfileID = lCustomProfileID;}
    __int64 GetCustomProfileID() {return m_lCustomProfileID;}

private:
    void ExtractLastPublishedTime(CString csProfileInformation, CString &csLastPublishedDate);
    void ExtractNotUsedCountTime(CString csProfileInformation, CString &csNotUsedCount);
    void ExtractLastUsedDocument(CString csProfileInformation, CString &csLastUsedDocument);
    void ExtractCustomProfileID(CString csProfileInformation, CString &csCustomProfileID);
    void ExtractLastActivatedTime(CString csProfileInformation, CString &csLastActivatedTime);
    void ExtractTimeFromString(CString csTime, CTime **pTime);

private:
    CString m_csProfileName;
    CString m_csProfileDescription;
    CString m_csLastPublishedDoc;
    __int64 m_lCustomProfileID;
    int m_iNotUsedCount;
    CTime *m_pLastPublished;
    CTime *m_pLastActivated;
};


class IProfileFinishListener
{
public:
    IProfileFinishListener() {}
    ~IProfileFinishListener() {}

public:
    virtual void PublisherFinished(CString csPublisherMessage) = 0;
};


class LSUTL32_EXPORT ProfileUtils
{
public:
    ProfileUtils(void);
    ~ProfileUtils(void);

public:

    static HRESULT GetProfileDirectory(HKEY hKey, CString &csProfileDirectory, bool bCreate);
    static HRESULT GetDirectPublishProfile(int &iPublishingFormat);
    static HRESULT SetDirectPublishProfile(int &iPublishingFormat);
    static HRESULT ResetActiveProfile();
    static HRESULT ReadActiveProfile(CString &csFilename);
    static HRESULT GetActiveProfileID(__int64 &lProfileID);
    static HRESULT ReadCustomProfiles(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation);
    static HRESULT WriteActiveProfile(CString &csFilename);
    static HRESULT ReadProfiles(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation);
    static HRESULT GetRecentProfiles(CArray<RegistryProfileInfo *, RegistryProfileInfo *> &arRecentProfiles);
    static HRESULT SetRecentProfiles(CArray<RegistryProfileInfo *, RegistryProfileInfo *> &arRecentProfiles);
    static HRESULT GetLastActivatedProfile(CString &csLastActivated);
    static HRESULT GetLastActivatedProfile(CString &csLastActivated, __int64 &lCustomProfileID);
    static HRESULT SetLastActivatedProfile(int iDefaultProfileID, __int64 lCustomProfileID, bool bIsCommandID=true);
    static HRESULT SetLastActivatedProfile(CString csLastActivated, __int64 lCustomProfileID);
    static int GetDefaultProfileIDFromName(CString csProfileName);
    static int GetProfileTypeFromName(CString csProfileName);
    static int GetImageIDFromName(CString csProfileName);
    static HRESULT GetProfileNameFromID(int iDefaultProfileID, CString &csProfileName, bool bIsCommandID=true);
    static HRESULT SetProfilePublished(int iProfileFormatID, CString csDocTitle);
    static HRESULT AddProfileToRecentList(int iDefaultProfileID);
    static void GetAllPossibleProfiles(CStringArray &aPossibleProfiles);
    static HRESULT ParseProfileMessage(CString csMessage, bool &bCancel, __int64 &iProfileID);
    static RegistryProfileInfo *GetInfoFromActiveProfile();
    static ProfileInfo *GetProfileWithID(__int64 iProfileID);
    static HRESULT GetNewProfileID(__int64 &iNewProfileID);

    static HRESULT HandlePublisherFinish(CWnd *pCallingWindow);
    static UINT HandlePublisherFinishThread(LPVOID pParam);

    static HRESULT WaitForProfile(IProfileFinishListener *pListener);
    static UINT WaitForProfileThread(LPVOID pParam);
   
    static int ShowYouTubeSettings();
    static int ShowSlidestarSettings();

    //// Error messages
    static void ShowErrorMessage(UINT iActionMessageID, UINT iErrorMessageID=0, UINT iCaptionID=0);


private:
    static HRESULT GetApplicationDataDirectory(HKEY hMainKey, CString &csApplicationData);
    static HRESULT AddProfileSubDirectory(CString &csApplicationDataDirectory, CString &csProfileDirectory, bool bCreate);
    static bool DirectoryExists(const _TCHAR *tszBaseDirectory, const _TCHAR *tszSubDirectory);
};

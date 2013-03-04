#ifndef __PUBLISHERPROFILE_H__19EF2849_6A4B_4e0f_A343_51EF8F27C2EA
#define __PUBLISHERPROFILE_H__19EF2849_6A4B_4e0f_A343_51EF8F27C2EA

#include "JNISupport.h"

#define WM_NEWPROFILE      WM_USER+1
#define E_PM_FILE_OPEN     _HRESULT_TYPEDEF_(0x80ea0001L)
#define E_PM_FILE_CREATE   _HRESULT_TYPEDEF_(0x80ea0002L)
#define E_PM_FILE_NOTEXIST _HRESULT_TYPEDEF_(0x80ea0003L)
#define E_PM_FILE_READ     _HRESULT_TYPEDEF_(0x80ea0004L)
#define E_PM_FILE_WRITE    _HRESULT_TYPEDEF_(0x80ea0005L)
#define E_PM_JNI_FAILED    _HRESULT_TYPEDEF_(0x80ea0006L)
#define E_PM_OPEN_KEY      _HRESULT_TYPEDEF_(0x80ea0007L)
#define E_PM_QUERY_VALUE   _HRESULT_TYPEDEF_(0x80ea0008L)
#define E_PM_WRONG_FORMAT  _HRESULT_TYPEDEF_(0x80ea0009L)
#define E_PM_CREATE_SUBDIR _HRESULT_TYPEDEF_(0x80ea000AL)
#define E_PM_GET_APPLDIR   _HRESULT_TYPEDEF_(0x80ea000BL)
#define E_PM_NOTINLIST     _HRESULT_TYPEDEF_(0x80ea000CL)

#define EXPORT_TYPE_LECTURNITY      0
#define EXPORT_TYPE_REAL            1
#define EXPORT_TYPE_WMT             2
#define EXPORT_TYPE_FLASH           3
#define EXPORT_TYPE_VIDEO           4

#define TEMPLATE_FLASH_NORMAL       0
#define TEMPLATE_FLASH_SLIDESTAR    1
#define TEMPLATE_FLASH_PSP          2

#define REPLAY_LOCAL                0
#define REPLAY_HTTP                 1
#define REPLAY_SERVER               2

#define SRV_FILE_SERVER             0
#define SRV_WEB_SERVER              1
#define SRV_STREAMING_SERVER        2
#define SRV_PODCAST_SERVER          3
#define SRV_SLIDESTAR               4
#define SRV_YOUTUBE                 5

#define TRANSFER_NETWORK_DRIVE      0
#define TRANSFER_UPLOAD             1

class CProfileManager;

class ProfileInfo
{
public:
   ProfileInfo();
   ProfileInfo(CString &csFilename);
   ProfileInfo(ProfileInfo *pProfileInfo);
   ProfileInfo(CStringArray &aKeys, CStringArray &aValues);
   ~ProfileInfo(void);
   
public:
   long GetID() {return m_iProfileID;}
   int GetVersion() {return m_iProfileVersion;}
   int GetType() {return m_iProfileType;}
   CString &GetFilename() {return m_csFilename;}
   CString &GetTitle() {return m_csTitle;}
   CString &GetTargetFormat() {return m_csTargetFormat;}
   CString &GetStorageDistribution() {return m_csStorageDistribution;}
   bool IsActivated() {return m_bActivated;}
   void GetKeysAndValues(CStringArray &aKeys, CStringArray &aValues);
   bool IsCreatedByAdmin() {return m_bCreatedByAdmin;}

   void SetID(long id) {m_iProfileID = id;}
   void SetVersion(int iProfileVersion) {m_iProfileVersion = iProfileVersion;}
   void SetType(int iProfileType) {m_iProfileType = iProfileType;}
   void SetFilename(CString &csFilename) {m_csFilename = csFilename;}
   void EmptyFilename() {m_csFilename.Empty();}
   void SetTitle(CString &csTitle);
   void SetActivated(bool bActivated) {m_bActivated = bActivated;}
   void SetCreatedByAdmin(bool bCreatedByAdmin) {m_bCreatedByAdmin = bCreatedByAdmin;}

   HRESULT Write(LPCTSTR szFilename = NULL);
   HRESULT Read();
   HRESULT DeleteProfile();

private:
   void ExtractTargetFormat();
   void ExtractStorageDistribution();

private:
   CString m_csFilename;

   long m_iProfileID;
   int m_iProfileVersion;
   int m_iProfileType;

   CString m_csTitle;
   CString m_csTargetFormat;
   CString m_csStorageDistribution;
   bool m_bActivated;
   
   CStringArray m_aKeys;
   CStringArray m_aValues;

   bool m_bCreatedByAdmin;
};

class PublisherProfile
{
public:
   PublisherProfile(void);
public:
   ~PublisherProfile(void);

public:
   UINT WriteProfileSettings(JNIEnv *env, jobject jProfileInfo, jobjectArray jaEntries);
   jobjectArray ReadProfileSettings(JNIEnv *env, jobject jProfileInfo, bool bActivate = false);
   jobject ManageProfiles(JNIEnv *env, jobject joPanel, jstring jsLrdFile);
   jobject GetActiveProfile(JNIEnv *env);
   void CancelAppendOrEdit();
   int GetLastError() {return m_iLastError;}

private:
   HRESULT GetStringsFromArray(JNIEnv *env, jobjectArray jEntryArray, CStringArray &aKeys, CStringArray &aValues);
   jobjectArray SetStringsToArray(JNIEnv *env, CStringArray &aKeys, CStringArray &aValues);
   
   HRESULT ReadProfiles(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation);
   HRESULT FillJavaProfileInfo(JNIEnv *env, jobject jProfileInfo, ProfileInfo *pProfileInfo);
   HRESULT FillCProfileInfo(JNIEnv *env, jobject jProfileInfo, ProfileInfo *pProfileInfo);

   void ShowErrorMessage(UINT iActionMessageID, UINT iErrorMessageID=0);


public:
   static bool UserIsAdministrator();
   static HRESULT GetProfileDirectory(HKEY hKey, CString &csProfileDirectory, bool bCreate);

private:
   static bool DirectoryExists(const _TCHAR *tszBaseDirectory, const _TCHAR *tszSubDirectory);
   static HRESULT GetApplicationDataDirectory(HKEY hMainKey, CString &csApplicationData);
   static HRESULT AddProfileSubDirectory(CString &csApplicationDataDirectory, CString &csProfileDirectory, bool bCreate);

private:
   CProfileManager *m_pManageDialog;

   int m_iLastError;

};

#endif //__PUBLISHERPROFILE_H__19EF2849_6A4B_4e0f_A343_51EF8F27C2EA

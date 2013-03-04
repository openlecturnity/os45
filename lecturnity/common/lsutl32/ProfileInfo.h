#pragma once

class CProfileManager;

#define E_PM_FILE_OPEN     _HRESULT_TYPEDEF_(0x80ea0001L)
#define E_PM_FILE_CREATE   _HRESULT_TYPEDEF_(0x80ea0002L)
#define E_PM_FILE_NOTEXIST _HRESULT_TYPEDEF_(0x80ea0003L)
#define E_PM_FILE_READ     _HRESULT_TYPEDEF_(0x80ea0004L)
#define E_PM_FILE_WRITE    _HRESULT_TYPEDEF_(0x80ea0005L)
#define E_PM_JNI_FAILED    _HRESULT_TYPEDEF_(0x80ea0006L)
#define E_PM_WRONG_FORMAT  _HRESULT_TYPEDEF_(0x80ea0009L)
#define E_PM_CREATE_SUBDIR _HRESULT_TYPEDEF_(0x80ea000AL)
#define E_PM_GET_APPLDIR   _HRESULT_TYPEDEF_(0x80ea000BL)
#define E_PM_NOTINLIST     _HRESULT_TYPEDEF_(0x80ea000CL)

#define EXPORT_TYPE_LECTURNITY      0
#define EXPORT_TYPE_REAL            1
#define EXPORT_TYPE_WMT             2
#define EXPORT_TYPE_FLASH           3
#define EXPORT_TYPE_VIDEO           4
#define EXPORT_TYPE_YOUTUBE         5
#define EXPORT_TYPE_SLIDESTAR       6

#define REPLAY_LOCAL                0
#define REPLAY_HTTP                 1
#define REPLAY_SERVER               2

#define SRV_FILE_SERVER             0
#define SRV_WEB_SERVER              1
#define SRV_STREAMING_SERVER        2
#define SRV_PODCAST_SERVER          3
#define SRV_SLIDESTAR               4

#define TRANSFER_NETWORK_DRIVE      0
#define TRANSFER_UPLOAD             1

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

class LSUTL32_EXPORT ProfileInfo
{
public:
    ProfileInfo();
    ProfileInfo(CString &csFilename);
    ProfileInfo(ProfileInfo *pProfileInfo);
    ProfileInfo(CStringArray &aKeys, CStringArray &aValues);
    ~ProfileInfo(void);

public:
    __int64 GetID() {return m_iProfileID;}
    int GetVersion() {return m_iProfileVersion;}
    int GetType() {return m_iProfileType;}
    CString &GetFilename() {return m_csFilename;}
    CString &GetTitle() {return m_csTitle;}
    CString &GetTargetFormat() {return m_csTargetFormat;}
    CString &GetStorageDistribution() {return m_csStorageDistribution;}
    bool IsActivated() {return m_bActivated;}
    void GetKeysAndValues(CStringArray &aKeys, CStringArray &aValues);
    bool IsCreatedByAdmin() {return m_bCreatedByAdmin;}
    void SetID(__int64 id) {m_iProfileID = id;}
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
    HRESULT Edit(CWnd *pCallingWindow, CString csLrdName);

private:
    void InitializeVariables();
    void ExtractTargetFormat();
    void ExtractStorageDistribution();

private:
    CString m_csFilename;

    __int64 m_iProfileID;
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

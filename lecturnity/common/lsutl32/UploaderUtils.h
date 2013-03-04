#pragma once

#include "YouTubeSettings.h"
#include "SlidestarSettings.h"
#ifdef _LSUTL32_DLL
    #define LSUTL32_EXPORT __declspec(dllexport)
#else 
    #ifdef _LSUTL32_STATIC
        #define LSUTL32_EXPORT 
    #else
        #define LSUTL32_EXPORT __declspec(dllimport)
    #endif
#endif

class LSUTL32_EXPORT CUploaderUtils {
public:
    CUploaderUtils(void);
public:
    ~CUploaderUtils(void);
public:
    int ShowYouTubeSettingsDialog(CWnd* pParent, CString csUser, CString csPassword,CString csTitle, 
        CString csDescription, CString csKeywords, CString csCategory, int iPrivacy, bool bCheck = false);
    void GetYouTubeTransferSettings(CString &csUser, CString &csPassword,CString &csTitle, 
        CString &csDescription, CString &csKeywords, CString &csCategory, int &iPrivacy);
    int ShowSlidestarSettingsDialog(CWnd* pParent, CString csServer, CString csUser, CString csPassword);
    void GetSlidestarTransferSettings(CString &csServer, CString &csUser, CString &csPassword);
private:
    YouTubeTransferSettings m_ytts;
    SlidestarTransferSettings m_sts;
};

#ifndef __MFCUTILS_H__DECD7213_DA4C_477c_92EE_C4F07393C7C9
#define __MFCUTILS_H__DECD7213_DA4C_477c_92EE_C4F07393C7C9


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


#include "ColorStatic.h"
#include "ImageStatic.h"
#include "MyTabCtrl.h"
#include "MySheet.h"
#include "CustomPropPage.h"
#include "TitleStatic.h"


#define RETRIEVE_CSTRING(function, cstring) \
   { \
      _TCHAR *tszDummy = NULL; \
      DWORD   dwDummy  = 0; \
      ##function(NULL, &dwDummy); \
      dwDummy++; \
      tszDummy = new _TCHAR[dwDummy]; \
      ##function(tszDummy, &dwDummy); \
      ##cstring = tszDummy; \
   }

namespace MfcUtils
{
   LSUTL32_EXPORT void Localize(CWnd *pWnd, DWORD *adwIds);
   LSUTL32_EXPORT HRESULT FitRectInRect(CRect& rcAvailable, CRect& rcToFit, bool bScaleLarger = false);
   LSUTL32_EXPORT void GetPrivateBuild(CString& csBuild);
   LSUTL32_EXPORT int ShowPublisherFormatSelection(HWND parent, CString csLrdName);
   LSUTL32_EXPORT int GetPublisherFormatSelection();
   LSUTL32_EXPORT void GetDesktopRectangle(CRect &rcDesktop);
   LSUTL32_EXPORT void GetCurrentMonitorRectangle(CRect &rcDesktop, CPoint ptMonitor);
}

namespace PublishingFormat
{
    enum PublishFormatId {
        TYPE_NOTHING = -1,
        TYPE_LECTURNITY = 0,
        TYPE_REALMEDIA = 1,
        TYPE_WINDOWSMEDIA = 2,
        TYPE_FLASH = 3,
        TYPE_IPOD = 4,
        TYPE_YOUTUBE = 5,
        TYPE_SLIDESTAR = 6,
        TYPE_CLIX = 7,
        TYPE_CUSTOM = 8
    };
}

#endif // __MFCUTILS_H__DECD7213_DA4C_477c_92EE_C4F07393C7C9

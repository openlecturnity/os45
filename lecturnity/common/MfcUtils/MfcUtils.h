#ifndef __MFCUTILS_H__DECD7213_DA4C_477c_92EE_C4F07393C7C9
#define __MFCUTILS_H__DECD7213_DA4C_477c_92EE_C4F07393C7C9

#pragma message("Do not use anymore: These files are now in lsutl32.")
#pragma message("Incomplete compile break statement...")

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
   void Localize(CWnd *pWnd, DWORD *adwIds);
   HRESULT FitRectInRect(CRect& rcAvailable, CRect& rcToFit, bool bScaleLarger = false);
   void GetPrivateBuild(CString& csBuild);
}

#endif // __MFCUTILS_H__DECD7213_DA4C_477c_92EE_C4F07393C7C9

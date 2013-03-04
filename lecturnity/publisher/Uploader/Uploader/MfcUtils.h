#ifndef __MFCUTILS_H__DECD7213_DA4C_477c_92EE_C4F07393C7C9
#define __MFCUTILS_H__DECD7213_DA4C_477c_92EE_C4F07393C7C9

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

namespace MfcUtils {
    void Localize(CWnd *pWnd, DWORD *adwIds);
}

#endif // __MFCUTILS_H__DECD7213_DA4C_477c_92EE_C4F07393C7C9

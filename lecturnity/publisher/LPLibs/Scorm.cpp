#include "StdAfx.h"
#include "ScormEngine.h"

// The single global CScormEngine instance.
// May be referenced by using
// extern CScormEngine g_scormEngine;
// in the other source files.
CScormEngine g_scormEngine;

// It's not as bad as it looks.
#define SCORM_IMPL(x, y, z) \
   UINT Scorm_##x##y \
   { \
      AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
      return g_scormEngine.##x##z; \
   }


SCORM_IMPL(Init, (), ())
SCORM_IMPL(ReadSettings, (), ())
SCORM_IMPL(WriteSettings, (), ())
SCORM_IMPL(GetSettings, (_TCHAR *tszSettings, DWORD *pdwSize), (tszSettings, pdwSize))
SCORM_IMPL(UseSettings, (const _TCHAR *tszSettings), (tszSettings))
SCORM_IMPL(CleanUp, (), ())
SCORM_IMPL(Configure, (HWND hWndParent, bool bDoNotSave), (hWndParent, bDoNotSave))
SCORM_IMPL(SetTargetFileName, (const _TCHAR *tszTargetFile), (tszTargetFile))
SCORM_IMPL(GetTargetFilePath, (_TCHAR *tszTargetPath, DWORD *pdwSize), (tszTargetPath, pdwSize))
SCORM_IMPL(SetIndexFileName, (const _TCHAR *tszRealFileName, const _TCHAR *tszFileName), (tszRealFileName, tszFileName))
SCORM_IMPL(AddResourceFile, (const _TCHAR *tszRealFileName, const _TCHAR *tszFileName), (tszRealFileName, tszFileName))
SCORM_IMPL(AddMetadataKeyword, (const _TCHAR *tszKeyword), (tszKeyword))
SCORM_IMPL(AddGeneralKeyword, (const _TCHAR *tszKey, const _TCHAR *tszValue), (tszKey, tszValue))
SCORM_IMPL(AddFullTextString, (const _TCHAR *tszFulltext), (tszFulltext))
SCORM_IMPL(CreatePackage, (HWND hWndParent), (hWndParent))
SCORM_IMPL(GetLanguage, (_TCHAR *tszLanguage, DWORD *pdwSize), (tszLanguage, pdwSize))
SCORM_IMPL(GetVersion, (_TCHAR *tszVersion, DWORD *pdwSize), (tszVersion, pdwSize))
SCORM_IMPL(GetStatus, (_TCHAR *tszStatus, DWORD *pdwSize), (tszStatus, pdwSize))
SCORM_IMPL(GetFullTextAsKeywords, (BOOL *pbFullText), (pbFullText))
SCORM_IMPL(GetProgress, (DWORD *pdwProgress), (pdwProgress))
SCORM_IMPL(Cancel, (), ())

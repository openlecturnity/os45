#include "StdAfx.h"
#include "resource.h"
#include "TemplateEngine.h"

// The single TemplateEngine instance.
CTemplateEngine g_templateEngine;

// This makes it easier to write and change the wrapper
#define TEMPL_IMPL(x, y, z) \
   UINT Template_##x##y \
   { \
      AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
      return g_templateEngine.##x##z; \
   }

TEMPL_IMPL(Init, (), ());
TEMPL_IMPL(CleanUp, (), ());
TEMPL_IMPL(ReadSettings, (), ());
TEMPL_IMPL(ReadSettingsFromFile, (const _TCHAR *tszPath, bool bOverride), (tszPath, bOverride));
TEMPL_IMPL(WriteSettings, (bool bForAllUsers, bool bSavePath), (bForAllUsers, bSavePath));
TEMPL_IMPL(GetSettings, (_TCHAR *tszSettings,  DWORD *pdwSize), (tszSettings, pdwSize));
TEMPL_IMPL(SetSettings, (const _TCHAR *tszSettings), (tszSettings));
TEMPL_IMPL(GetCurrentTemplatePath, (_TCHAR *tszPath,  DWORD *pdwSize), (tszPath, pdwSize));
TEMPL_IMPL(Configure, (HWND hWndParent, bool bDoNotSave), (hWndParent, bDoNotSave));
TEMPL_IMPL(AddKeyword, (const _TCHAR *tszKeyword, const _TCHAR *tszReplace), (tszKeyword, tszReplace));
TEMPL_IMPL(GetTitle, (_TCHAR *tszTitle, DWORD *pdwSize), (tszTitle, pdwSize));
TEMPL_IMPL(GetDescription, (_TCHAR *tszName, DWORD *pdwSize), (tszName, pdwSize));
TEMPL_IMPL(GetAuthor, (_TCHAR *tszAuthor, DWORD *pdwSize), (tszAuthor, pdwSize));
TEMPL_IMPL(GetCopyright, (_TCHAR *tszCopyright, DWORD *pdwSize), (tszCopyright, pdwSize));
TEMPL_IMPL(IsScormCompliant, (BOOL *pbCompliant), (pbCompliant));
TEMPL_IMPL(IsUtfTemplate, (BOOL *pbUtf), (pbUtf));
TEMPL_IMPL(DoParse, (HWND hWndParent), (hWndParent));
TEMPL_IMPL(SetTargetDirectory, (const _TCHAR *tszDirectory), (tszDirectory));
TEMPL_IMPL(GetTargetDirectory, (_TCHAR *tszDirectory, DWORD *pdwSize), (tszDirectory, pdwSize));
TEMPL_IMPL(GetFileCount, (DWORD *pdwCount), (pdwCount));
TEMPL_IMPL(GetFile, (UINT nIndex, _TCHAR *tszFileName, DWORD *pdwSize), (nIndex, tszFileName, pdwSize));
TEMPL_IMPL(GetIndexFile, (_TCHAR *tszFileName, DWORD *pdwSize), (tszFileName, pdwSize));
TEMPL_IMPL(SetScormEnabled, (bool bScormEnabled), (bScormEnabled));
TEMPL_IMPL(SetDocumentType, (const _TCHAR *tszDocType), (tszDocType));
TEMPL_IMPL(SetTargetFormat, (const _TCHAR *tszTargetFormat), (tszTargetFormat));
TEMPL_IMPL(GetMaxSlidesWidth, (int nScreenWidth, int nVideoWidth, int nStillImageWidth, int *pMaxSlidesWidth), (nScreenWidth, nVideoWidth, nStillImageWidth, pMaxSlidesWidth));
TEMPL_IMPL(GetMaxSlidesHeight, (int nScreenHeight, int nVideoHeight, int nStillImageHeight, int *pMaxSlidesHeight), (nScreenHeight, nVideoHeight, nStillImageHeight, pMaxSlidesHeight));
TEMPL_IMPL(GetGlobalSetting, (const _TCHAR *tszSettingsName, _TCHAR *tszSetting, DWORD *pdwSize), (tszSettingsName, tszSetting, pdwSize));
TEMPL_IMPL(GetStandardNavigationStates, (int *aiStandardNavigationStates), (aiStandardNavigationStates));
TEMPL_IMPL(PutStandardNavigationStatesToLrd, (const _TCHAR *tszLrdFile, int *aiStandardNavigationStates), (tszLrdFile, aiStandardNavigationStates));

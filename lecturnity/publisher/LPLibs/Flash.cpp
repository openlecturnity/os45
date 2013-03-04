#include "StdAfx.h"
#include "FlashEngine.h"

// May be referenced by using
// extern CFlashEngine g_FlashEngine;
// in the other source files.
CFlashEngine g_FlashEngine;

// It's not as bad as it looks.
#define FLASH_IMPL(x, y, z) \
   UINT Flash_##x##y \
   { \
      AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
      return g_FlashEngine.##x##z; \
   }

FLASH_IMPL(SetInputFiles, (const _TCHAR *tszLrdFile, const _TCHAR *tszEvqFile, const _TCHAR *tszObjFile, const _TCHAR *tszLadFile), (tszLrdFile, tszEvqFile, tszObjFile, tszLadFile))
FLASH_IMPL(SetInputAudioIsFromFlv, (bool bIsAudioFlv, const _TCHAR *tszAudioFlvFile), (bIsAudioFlv, tszAudioFlvFile))
//FLASH_IMPL(SetInputAudioFile, (const _TCHAR *tszAudioFile, const _TCHAR *tszAudioFlvFile), (tszAudioFile, tszAudioFlvFile))
FLASH_IMPL(SetInputVideoFile, (const _TCHAR *tszVideoFile, const _TCHAR *tszVideoFlvFile, int nVideoWidth, int nVideoHeight, bool bDoPadding), (tszVideoFile, tszVideoFlvFile, nVideoWidth, nVideoHeight, bDoPadding))
FLASH_IMPL(SetNumberOfClips, (int iNumOfClips), (iNumOfClips))
FLASH_IMPL(SetInputClipFile, (int iIndex, const _TCHAR *tszClipFile, const _TCHAR *tszClipFlvFile, int nClipWidth, int nClipHeight, bool bDoPadding, long lClipBeginMs, long lClipEndMs), (iIndex, tszClipFile, tszClipFlvFile, nClipWidth, nClipHeight, bDoPadding, lClipBeginMs, lClipEndMs))
FLASH_IMPL(SetInputStillImageFile, (const _TCHAR *tszStillImageFile, int nWidth, int nHeight), (tszStillImageFile, nWidth, nHeight))
FLASH_IMPL(SetOutputFile, (const _TCHAR *tszSwfFile), (tszSwfFile))
FLASH_IMPL(SetRtmpPath, (const _TCHAR *tszRtmpPath), (tszRtmpPath))
FLASH_IMPL(SetWbInputDimension, (int nWidth, int nHeight), (nWidth, nHeight))
FLASH_IMPL(SetWbOutputDimension, (int nWidth, int nHeight), (nWidth, nHeight))
FLASH_IMPL(SetMovieDimension, (int nWidth, int nHeight), (nWidth, nHeight))
FLASH_IMPL(SetScormEnabled, (bool bScormEnabled), (bScormEnabled))
FLASH_IMPL(SetFlashVersion, (int nFlashVersion), (nFlashVersion))
FLASH_IMPL(SetAutostartMode, (bool bAutostartMode), (bAutostartMode))
FLASH_IMPL(SetSgStandAloneMode, (bool bSgStandAloneMode), (bSgStandAloneMode))
FLASH_IMPL(SetDrawEvalNote, (bool bDrawEvalNote), (bDrawEvalNote))
FLASH_IMPL(SetPspDesign, (bool bPspDesign), (bPspDesign))
FLASH_IMPL(SetShowClipsInVideoArea, (bool bShowClipsInVideoArea), (bShowClipsInVideoArea))
FLASH_IMPL(HideLecturnityIdentity, (bool bHideLecIdentity), (bHideLecIdentity))
FLASH_IMPL(EnableOutputDir, (const _TCHAR *tszOutputDir), (tszOutputDir))
FLASH_IMPL(ResetAllDimensions, (), ())
FLASH_IMPL(SetIncreaseKeyframes, (bool bIncreaseKeyframes), (bIncreaseKeyframes))
FLASH_IMPL(GetMovieWidth, (), ())
FLASH_IMPL(GetMovieHeight, (), ())
FLASH_IMPL(GetContentDimensions, (const SIZE *sizeMovie, const SIZE *sizeVideoIn, const SIZE *sizePagesIn, const SIZE *sizeLogo, bool bDisplayHeaderText, bool bIsControlbarVisible, bool bShowVideoOnPages, SIZE *sizeVideoOut, SIZE *sizePagesOut), (sizeMovie, sizeVideoIn, sizePagesIn, sizeLogo, bDisplayHeaderText, bIsControlbarVisible, bShowVideoOnPages, sizeVideoOut, sizePagesOut))
FLASH_IMPL(ObtainInsets, (RECT *rectInsets, const SIZE *sizeWbInput, bool bDisplayLogo, const SIZE *sizeLogo, const SIZE *sizeVideo, bool bIsControlbarVisible), (rectInsets, sizeWbInput, bDisplayLogo, sizeLogo, sizeVideo, bIsControlbarVisible))
FLASH_IMPL(ConvertImages, (), ())
FLASH_IMPL(ConvertFonts, (), ())
FLASH_IMPL(ConvertAudioToMp3Ds, (_TCHAR *tszMp3File, int iAudioBitrate), (tszMp3File, iAudioBitrate))
FLASH_IMPL(SetStandardNavigationStates, (int nNavigationControlBar, int nNavigationStandardButtons, int nNavigationTimeLine, int nNavigationTimeDisplay, int nNavigationDocumentStructure, int nNavigationPluginContextMenu), (nNavigationControlBar, nNavigationStandardButtons, nNavigationTimeLine, nNavigationTimeDisplay, nNavigationDocumentStructure, nNavigationPluginContextMenu))
FLASH_IMPL(BuildMovie, (CMap<CString, LPCTSTR, CString, LPCTSTR> *pMapMetadata, Gdiplus::ARGB refBasicColor, Gdiplus::ARGB refBackColor, Gdiplus::ARGB refTextColor, _TCHAR *tszLogoFile, _TCHAR *tszLogoUrl, int iFlashFormatType, bool bShowVideoOnPages), (pMapMetadata, refBasicColor, refBackColor, refTextColor, tszLogoFile, tszLogoUrl, iFlashFormatType, bShowVideoOnPages))
FLASH_IMPL(PrepareScorm, (), ())
FLASH_IMPL(CreateSlidestar, (CString csThumbFiles), (csThumbFiles))
FLASH_IMPL(CreateFlexFiles, (const _TCHAR *tszBaseUrl, const _TCHAR *tszStreamUrl, bool bShowVideoOnPages, bool bShowClipsInVideoArea, bool bStripFlvSuffix, bool bDoAutostart, bool bScormEnabled, bool bUseOriginalSlideSize), (tszBaseUrl, tszStreamUrl, bShowVideoOnPages, bShowClipsInVideoArea, bStripFlvSuffix, bDoAutostart, bScormEnabled, bUseOriginalSlideSize))
FLASH_IMPL(GetListOfInteractiveOpenedFiles, (CString* acsFileList), (acsFileList))
FLASH_IMPL(PrepareCopiesOfInteractiveOpenedFiles, (), ())
FLASH_IMPL(Cancel, (), ())
FLASH_IMPL(Cleanup, (), ())
FLASH_IMPL(LastErrorDetail, (_TCHAR *tszError, DWORD *dwSize), (tszError, dwSize))
FLASH_IMPL(GetFontListFontNotFound, (_TCHAR *tszFontList, DWORD *dwSize), (tszFontList, dwSize))
FLASH_IMPL(GetFontListGlyphNotFound, (_TCHAR *tszFontList, DWORD *dwSize), (tszFontList, dwSize))
FLASH_IMPL(GetFontListStylesNotSupported, (_TCHAR *tszFontList, DWORD *dwSize), (tszFontList, dwSize))
FLASH_IMPL(GetPreloaderFilename, (_TCHAR *tszPreloaderFilename, DWORD *dwSize), (tszPreloaderFilename, dwSize))
FLASH_IMPL(GetTooShortPagesTimes, (_TCHAR *tszTooShortPagesTimes, DWORD *dwSize), (tszTooShortPagesTimes, dwSize))
FLASH_IMPL(EnableOutputDir, (_TCHAR *tszEnableDir), (tszEnableDir))

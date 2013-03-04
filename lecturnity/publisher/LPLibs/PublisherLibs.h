#ifndef __PUBLISHERLIBS_H__19EF2849_6A4B_4e0f_A343_51EF8F27C2EA
#define __PUBLISHERLIBS_H__19EF2849_6A4B_4e0f_A343_51EF8F27C2EA

// General
extern UINT LPLibs_Init();
extern UINT LPLibs_CleanUp();

// SCORM
extern UINT Scorm_ReadSettings();
extern UINT Scorm_WriteSettings();
extern UINT Scorm_GetSettings(_TCHAR *tszSettings, DWORD *pdwSize);
extern UINT Scorm_UseSettings(const _TCHAR *tszSettings);
extern UINT Scorm_Configure(HWND hWndParent, bool bDoNotSave);
extern UINT Scorm_Init();
extern UINT Scorm_CleanUp();
extern UINT Scorm_SetTargetFileName(const _TCHAR *tszTargetFile);
extern UINT Scorm_GetTargetFilePath(_TCHAR *tszTargetPath, DWORD *pdwSize);
extern UINT Scorm_SetIndexFileName(const _TCHAR *tszRealFileName, const _TCHAR *tszFileName);
extern UINT Scorm_AddResourceFile(const _TCHAR *tszRealFileName, const _TCHAR *tszFileName);
extern UINT Scorm_AddMetadataKeyword(const _TCHAR *tszKeyword);
extern UINT Scorm_AddGeneralKeyword(const _TCHAR *tszKey, const _TCHAR *tszValue);
extern UINT Scorm_AddFullTextString(const _TCHAR *tszFulltext);
extern UINT Scorm_CreatePackage(HWND hWndParent);
extern UINT Scorm_GetProgress(DWORD *pdwProgress);
extern UINT Scorm_Cancel();
extern UINT Scorm_GetLanguage(_TCHAR *tszLanguage, DWORD *pdwSize);
extern UINT Scorm_GetVersion(_TCHAR *tszVersion, DWORD *pdwSize);
extern UINT Scorm_GetStatus(_TCHAR *tszStatus, DWORD *pdwSize);
extern UINT Scorm_GetFullTextAsKeywords(BOOL *pbFullText);

// TemplateParser
extern UINT Template_Init();
extern UINT Template_CleanUp();
extern UINT Template_ReadSettings();
extern UINT Template_ReadSettingsFromFile(const _TCHAR *tszPath, bool bOverride);
extern UINT Template_WriteSettings(bool bForAllUsers, bool bSavePath);
extern UINT Template_GetSettings(_TCHAR *tszSettings, DWORD *pdwSize);
extern UINT Template_SetSettings(const _TCHAR *tszSettings);
extern UINT Template_GetCurrentTemplatePath(_TCHAR *tszSettings, DWORD *pdwSize);
extern UINT Template_Configure(HWND hWndParent, bool bDoNotSave);
extern UINT Template_AddKeyword(const _TCHAR *tszKeyword, const _TCHAR *tszReplace);
extern UINT Template_GetTitle(_TCHAR *tszTitle, DWORD *pdwSize);
extern UINT Template_GetDescription(_TCHAR *tszDescription, DWORD *pdwSize);
extern UINT Template_GetAuthor(_TCHAR *tszAuthor, DWORD *pdwSize);
extern UINT Template_GetCopyright(_TCHAR *tszCopyright, DWORD *pdwSize);
extern UINT Template_IsScormCompliant(BOOL *pbCompliant);
extern UINT Template_IsUtfTemplate(BOOL *pbUtf);
extern UINT Template_DoParse(HWND hWndParent);
extern UINT Template_SetTargetDirectory(const _TCHAR *tszDirectory);
extern UINT Template_GetTargetDirectory(_TCHAR *tszDirectory, DWORD *pdwSize);
extern UINT Template_GetFileCount(DWORD *pdwCount);
extern UINT Template_GetFile(UINT nIndex, _TCHAR *tszFileName, DWORD *pdwSize);
extern UINT Template_GetIndexFile(_TCHAR *tszFileName, DWORD *pdwSize);
extern UINT Template_SetDocumentType(const _TCHAR *tszDocType);
extern UINT Template_SetScormEnabled(bool bScormEnabled);
extern UINT Template_SetTargetFormat(const _TCHAR *tszTargetFormat);
extern UINT Template_GetMaxSlidesWidth(int nScreenWidth, int nVideoWidth, int nStillImageWidth, int *pMaxSlidesWidth);
extern UINT Template_GetMaxSlidesHeight(int nScreenHeight, int nVideoHeight, int nStillImageHeight, int *pMaxSlidesHeight);
extern UINT Template_GetGlobalSetting(const _TCHAR *tszSettingsName, _TCHAR *tszSetting, DWORD *pdwSize);
extern UINT Template_GetStandardNavigationStates(int *aiStandardNavigationStates);
extern UINT Template_PutStandardNavigationStatesToLrd(const _TCHAR *tszLrdFile, int *aiStandardNavigationStates);

// Transfer
extern UINT Transfer_Configure(DWORD dwIndex, HWND hWndParent);

// Flash Export
extern UINT Flash_SetInputFiles(const _TCHAR *tszLrdFile, const _TCHAR *tszEvqFile, 
                                const _TCHAR *tszObjFile, const _TCHAR *tszLadFile);
extern UINT Flash_SetInputAudioIsFromFlv(bool bIsAudioFlv, const _TCHAR *tszAudioFlvFile);
//extern UINT Flash_SetInputAudioFile(const _TCHAR *tszAudioFile, const _TCHAR *tszAudioFlvFile);
extern UINT Flash_SetInputVideoFile(const _TCHAR *tszVideoFile, const _TCHAR *tszVideoFlvFile, 
                                    int nVideoWidth, int nVideoHeight, bool bDoPadding);
extern UINT Flash_SetNumberOfClips(int iNumOfClips);
extern UINT Flash_SetInputClipFile(int iIndex, const _TCHAR *tszClipFile, const _TCHAR *tszClipFlvFile, 
                                   int nClipWidth, int nClipHeight, bool bDoPadding, 
                                   long lClipBeginMs, long lClipEndMs);
extern UINT Flash_SetInputStillImageFile(const _TCHAR *tszStillImageFile, int nWidth, int nHeight);
extern UINT Flash_SetOutputFile(const _TCHAR *tszSwfFile);
extern UINT Flash_SetRtmpPath(const _TCHAR *tszRtmpPath);
extern UINT Flash_SetWbInputDimension(int iWidth, int iHeight);
extern UINT Flash_SetWbOutputDimension(int iWidth, int iHeight);
extern UINT Flash_SetMovieDimension(int iWidth, int iHeight);
extern UINT Flash_SetFlashVersion(int iFlashVersion);
extern UINT Flash_SetScormEnabled(bool bScormEnabled);
extern UINT Flash_SetAutostartMode(bool bAutostartMode);
extern UINT Flash_SetSgStandAloneMode(bool bSgStandAloneMode);
extern UINT Flash_SetDrawEvalNote(bool bDrawEvalNote);
extern UINT Flash_SetPspDesign(bool bPspDesign);
extern UINT Flash_SetShowClipsInVideoArea(bool bShowClipsInVideoArea);
extern UINT Flash_HideLecturnityIdentity(bool bHideLecIdentity);
extern UINT Flash_EnableOutputDir(const _TCHAR *tszOutputDir);
extern UINT Flash_ResetAllDimensions();
extern UINT Flash_SetIncreaseKeyframes(bool bIncreaseKeyframes);
extern UINT Flash_GetMovieWidth();
extern UINT Flash_GetMovieHeight();
extern UINT Flash_GetContentDimensions(const SIZE *sizeMovie, const SIZE *sizeVideoIn, const SIZE *sizePagesIn, const SIZE *sizeLogo, bool bDisplayHeaderText, bool bIsControlbarVisible, bool bShowVideoOnPages, SIZE *sizeVideoOut, SIZE *sizePagesOut);
extern UINT Flash_ObtainInsets(RECT *rectInsets, const SIZE *sizeWbInput, bool bDisplayHeaderText, const SIZE *sizeLogo, 
                               const SIZE *sizeVideo, bool bIsControlbarVisible);
extern UINT Flash_ConvertImages();
extern UINT Flash_ConvertFonts();
extern UINT Flash_ConvertAudioToMp3Ds(_TCHAR *tszMp3File, int iAudioBitrate);
extern UINT Flash_SetStandardNavigationStates(int nNavigationControlBar, int nNavigationStandardButtons, 
                                              int nNavigationTimeLine, int nNavigationTimeDisplay, 
                                              int nNavigationDocumentStructure, int nNavigationPluginContextMenu);
extern UINT Flash_BuildMovie(CMap<CString, LPCTSTR, CString, LPCTSTR> *pMapMetadata,
                             Gdiplus::ARGB refBasicColor, Gdiplus::ARGB refBackColor, Gdiplus::ARGB refTextColor, 
                             _TCHAR *tszLogoFile, _TCHAR *tszLogoUrl, 
                             int iFlashFormatType, bool bShowVideoOnPages);
extern UINT Flash_CreateSlidestar(CString csThumbFiles);
extern UINT Flash_CreateFlexFiles(const _TCHAR *tszBaseUrl, const _TCHAR *tszStreamUrl, bool bShowVideoOnPages, bool bShowClipsInVideoArea, 
                                  bool bStripFlvSuffix, bool bDoAutostart, bool bScormEnabled, bool bUseOriginalSlideSize);
extern UINT Flash_PrepareScorm();
extern UINT Flash_GetListOfInteractiveOpenedFiles(CString* acsFileList);
extern UINT Flash_PrepareCopiesOfInteractiveOpenedFiles();
extern UINT Flash_Cancel();
extern UINT Flash_Cleanup();
extern UINT Flash_LastErrorDetail(_TCHAR *tszError, DWORD *dwSize);
extern UINT Flash_GetFontListFontNotFound(_TCHAR *tszFontList, DWORD *dwSize);
extern UINT Flash_GetFontListGlyphNotFound(_TCHAR *tszFontList, DWORD *dwSize);
extern UINT Flash_GetFontListStylesNotSupported(_TCHAR *tszFontList, DWORD *dwSize);
extern UINT Flash_GetPreloaderFilename(_TCHAR *tszPreloaderFilename, DWORD *dwSize);
extern UINT Flash_GetTooShortPagesTimes(_TCHAR *tszTooShortPagesTimes, DWORD *dwSize);
extern UINT Flash_EnableOutputDir(_TCHAR *tszEnableDir);

#endif // __PUBLISHERLIBS_H__19EF2849_6A4B_4e0f_A343_51EF8F27C2EA

#if !defined(AFX_SCORMENGINE_H__4F5D76F1_061E_473A_AF1E_8940C360C202__INCLUDED_)
#define AFX_SCORMENGINE_H__4F5D76F1_061E_473A_AF1E_8940C360C202__INCLUDED_

#include <windef.h>

#include "mingpp.h"
#include "ColorUtils.h"
#include "WhiteboardStream.h"
#include "InteractionArea.h"
#include "Questionnaire.h"

//#include "./src/blocks/font.h"
// Copied from "font.h":
#define SWF_FONT_HASLAYOUT    (1<<7)
#define SWF_FONT_SHIFTJIS     (1<<6)
#define SWF_FONT_UNICODE      (1<<5)
#define SWF_FONT_ANSI         (1<<4)
#define SWF_FONT_WIDEOFFSETS  (1<<3)
#define SWF_FONT_WIDECODES    (1<<2)
#define SWF_FONT_ISITALIC     (1<<1)
#define SWF_FONT_ISBOLD       (1<<0)

static int   FLASH_OBJECT_BOUNDARY     = 15400;
static int   FLASH_INTERACTIONS_BOUNDARY = 15895;
static float DEFAULT_SLIDES_WIDTH      = 720.0f;
static float DEFAULT_SLIDES_HEIGHT     = 540.0f;
static float DEFAULT_CONTROLBAR_HEIGHT = 35.0f;
static float MINIMUM_CONTROLBAR_WIDTH  = 620.0f;
static float MINIMUM_METADATA_WIDTH    = 240.0f;
// The size of the border around the slides
static long  FLASH_BORDERSIZE = 3L;
// The padding between the objects (slides, logo image, metadata, ...)
static long  FLASH_PADDING = 10L;
static long  FLASH_SMALL_PADDING = 3L;
// The height of the metadata text (equal to the font size used for the text)
static long  FLASH_METADATAHEIGHT = 16L;
// The maximum size of text buffer for ActionScript files
static long  MAX_ACTIONSCRIPT_BUFFER = 65536L;
// PSP Design constants:
static float PSP_MOVIE_WIDTH           = 480.0f;
static float PSP_MOVIE_HEIGHT          = 272.0f;
static float PSP_CONTENT_WIDTH         = 363.0f;
static float PSP_CONTENT_HEIGHT        = 272.0f;
static float PSP_OFFSET_X              =  32.0f;
// Preloader file name (also used in the Java code of the Publisher in "FlashSnippetCreator.java" and "FlashWriterWizardPanel.java")
static CString STR_PRELOADER_FILENAME  = _T("lecturnityPreloader.swf");
static CString STR_PRELOADER_SUFFIX  = _T("_preloader");
static CString STR_CONTENT = _T("content");
// Use either Preloader file name or Preloader suffix 
static bool B_USE_PRELOADER_FILENAME = true;

// Structure for stop marks
struct FLASH_STOPMARK
{
   int nMsTime;
   int nFrame;
};

struct FLASH_FULLTEXT
{
   int nMsTime;
   DrawSdk::Text *pTextObject;
};

// Originally defined in "QuestionnaireEx.h"
// TODO: Only one enumeration should be used
enum FeedbackTypeId
{
   FEEDBACK_TYPE_NOTHING,
   FEEDBACK_TYPE_Q_CORRECT,
   FEEDBACK_TYPE_Q_WRONG,
   FEEDBACK_TYPE_Q_REPEAT,
   FEEDBACK_TYPE_Q_TIME,
   FEEDBACK_TYPE_Q_TRIES,
   FEEDBACK_TYPE_QQ_PASSED,
   FEEDBACK_TYPE_QQ_FAILED,
   FEEDBACK_TYPE_QQ_EVAL
};

// Flash has a 16000 objects (and frame) border,  
// so there are some restrictions according to 
// the number of objects used and their depth 
// (depth = layer index). 
//
// Values used in FlashEngine and FlashEngineHelper:
//
// Maximum number of objects created from the Object Queue:
// Object Boundary:     15400
// Depths:
// - Interaction:       maxIndexSoFar + 100
// - Dummy DragImage:   15895
// - Telepointer:       15898
// - Video:             15899
// - Clips:             15900 - 15964 (65 Clips should be enough)
// - Dummy Background:  15966 - 15969
// - Control Bar:       15970
// - Slides Border:     15971
// - Logo Image:        15972
// - MetadataText:      15973
// - PreloaderL:        15975
// - PreloaderM:        15976
// - Protection layer:  15977
// - TestingMessages:   15978 - 15979
// - EvalNote:          15980
// - Tooltip:           15981
// - General message:   15982

class CFlashEngine
{
public:
	CString GetScormEnabled();
	CFlashEngine();
	virtual ~CFlashEngine();
   
   UINT SetInputFiles(const _TCHAR *tszLrdFile, const _TCHAR *tszEvqFile, 
      const _TCHAR *tszObjFile, const _TCHAR *tszLadFile);
   UINT SetInputAudioIsFromFlv(bool bIsAudioFlv, const _TCHAR *tszAudioFlvFile);
//   UINT SetInputAudioFile(const _TCHAR *tszAudioFile, const _TCHAR *tszAudioFlvFile);
   UINT SetInputVideoFile(const _TCHAR *tszVideoFile, const _TCHAR *tszVideoFlvFile, 
      int nVideoWidth, int nVideoHeight, bool bDoPadding);
   UINT SetNumberOfClips(int iNumOfClips);
   UINT SetInputClipFile(int iIndex, const _TCHAR *tszClipFile, const _TCHAR *tszClipFlvFile, 
      int nClipWidth, int nClipHeight, bool bDoPadding, long lClipBeginMs, long lClipEndMs);
   UINT SetInputStillImageFile(const _TCHAR *tszStillImageFile, int nWidth, int nHeight);
   UINT SetOutputFile(const _TCHAR *tszSwfFile);
   UINT SetRtmpPath(const _TCHAR *tszRtmpPath);
   UINT SetWbInputDimension(int iWidth, int iHeight);
   UINT SetWbOutputDimension(int iWidth, int iHeight);
   UINT SetMovieDimension(int iWidth, int iHeight);
   UINT SetFlashVersion(int iFlashVersion);
   UINT SetAutostartMode(bool bAutostartMode);
   UINT SetSgStandAloneMode(bool bSgStandAloneMode);
   UINT SetDrawEvalNote(bool bDrawEvalNote);
   UINT SetPspDesign(bool bIsPspDesign);
   UINT SetShowClipsInVideoArea(bool bShowClipsInVideoArea);
   UINT HideLecturnityIdentity(bool bHideLecIdentity);
   UINT ResetAllDimensions();
   UINT SetIncreaseKeyframes(bool bIncreaseKeyframes);
   UINT SetScormEnabled(bool bScormEnabled);
   UINT GetMovieWidth();
   UINT GetMovieHeight();
   UINT GetContentDimensions(const SIZE *sizeMovie, const SIZE *sizeVideoIn, const SIZE *sizePagesIn, const SIZE *sizeLogo, 
      bool bDisplayHeaderText, bool bIsControlbarVisible, bool bShowVideoOnPages, SIZE *sizeVideoOut, SIZE *sizePagesOut);
   UINT ObtainInsets(RECT *rectInsets, const SIZE *sizeWbInput, bool bDisplayHeaderText, const SIZE *sizeLogo, 
      const SIZE *sizeVideo, bool bIsControlbarVisible);
   UINT SetBasicColors(int r, int g, int b);
   UINT RescaleImages(float scaleX, float scaleY);
   UINT WriteRescaledImage(DrawSdk::Image *pImageObject, CString csImageIn, CString &csImageOut, 
      float scaleX, float scaleY);
   // If an Image is converted without resizing, use 'csImageIn' as 'csImageTmp'
   UINT ConvertImage(CString csImageIn, CString csImageTmp, CString &csImageOut);
   UINT ConvertImages();
   UINT ConvertFonts();
   UINT ConvertAudioToMp3Ds(_TCHAR *tszOutputFile, int iAudioKBit); 
   UINT ConvertAudioToMp3Ds(_TCHAR *tszInputFile, _TCHAR *tszOutputFile, int iAudioKBit); 
   UINT BuildFont(CString csFont);
   UINT SetStandardNavigationStates(int nNavigationControlBar, int nNavigationStandardButtons, 
                                    int nNavigationTimeLine, int nNavigationTimeDisplay, 
                                    int nNavigationDocumentStructure, int nNavigationPluginContextMenu);
   UINT BuildMovie(CMap<CString, LPCTSTR, CString, LPCTSTR> *pMapMetadata,
      Gdiplus::ARGB refBasicColor, Gdiplus::ARGB refBackColor, Gdiplus::ARGB refTextColor, 
      _TCHAR *tszLogoFile, _TCHAR *tszLogoUrl, int iFlashFormatType, bool bShowVideoOnPages);
   UINT PrepareScorm();
   UINT CreateSlidestar(CString csThumbFiles);
   UINT CreateFlexFiles(const _TCHAR *tszBaseUrl, const _TCHAR *tszStreamUrl, 
                        bool bShowVideoOnPages, bool bShowClipsInVideoArea, 
                        bool bStripFlvSuffix, bool bDoAutostart, 
                        bool bScormEnabled, 
                        bool bUseOriginalSlideSize);
   UINT ConvertLmdFile(const _TCHAR *tszPathFileLmdIn, const _TCHAR *tszPathFileLmdOut);
   UINT WriteFullTextList(CFileOutput *pDoc);
   CString GetNavigationStateFromIndex(int idx);
   int GetListOfInteractiveOpenedFiles(CString* acsFileList);
   UINT PrepareCopiesOfInteractiveOpenedFiles();
   UINT GetAudioInfos(CString csAudioFile, int *iSampleRate, int *iAudioLengthMs);
   UINT Cancel();
   UINT Cleanup();
   UINT LastErrorDetail(_TCHAR *tszError, DWORD *dwSize);
   UINT GetFontListFontNotFound(_TCHAR *tszFontList, DWORD *dwSize);
   UINT GetFontListGlyphNotFound(_TCHAR *tszFontList, DWORD *dwSize);
   UINT GetFontListFontOrGlyphNotFound(_TCHAR *tszFontList, DWORD *dwSize, bool bIsFont=true);
   UINT GetFontListStylesNotSupported(_TCHAR *tszFontList, DWORD *dwSize);
   UINT GetPreloaderFilename(_TCHAR *tszPreloaderFilename, DWORD *dwSize);
   UINT GetTooShortPagesTimes(_TCHAR *tszTooShortPagesTimes, DWORD *dwSize);
   UINT EnableOutputDir(const _TCHAR *tszEnableDir);

private:
   void ClearWhiteboardArraysAndMaps();
   UINT AddInteractionToMovie(SWFMovie *movie, SWFSprite *sprPages);
   UINT ReadAndProcessWhiteboardStream();
   UINT GetObjectListFromWhiteboardStream();
   UINT GetStopJumpMarksFromWhiteboardStream();
   UINT GetInteractionObjectsFromWhiteboardStream();
   UINT GetQuestionnaireFromWhiteboardStream();
   void FillHashTablesForFeedbacks(CArray<CFeedbackDefinition *, CFeedbackDefinition *> *paFeedbacks);
   void FillHashTablesForDrawObject(DrawSdk::DrawObject *pObject);
   UINT RescaleImageObject(DrawSdk::DrawObject *pObject, float scaleX, float scaleY);
   UINT CalculateScaleFactor();
   UINT SearchForFonts();
   UINT LookForMetadata(bool &bShowMetadata, CMap<CString, LPCTSTR, CString, LPCTSTR> *pMapMetadata);
   UINT CalculateMovieDimension(bool bShowMetadata, bool bIsControlbarVisible, bool bHasPages);
   UINT GetSpritesDimension(bool bShowMetadata, bool bIsControlbarVisible, bool bHasPages, CRect &rcSprites);
   UINT CalculateInsets(RECT *rectInsets, float& fControlbarHeight, 
                        bool bDisplayHeaderText, bool bIsControlbarVisible,
                        const SIZE *pSizePages, const SIZE *pSizeVideo, const SIZE *pSizeLogo);
   UINT CalculateSpritePositions(int iVideoWidth, int iVideoHeight,
                                 int iWbWidth, int iWbHeight,
                                 int iLogoWidth, int iLogoHeight,
                                 int iControlBarWidth, int iControlBarHeight,
                                 int iMetadataWidth, int iMetadataHeight);
   UINT CalculateSpritePositionsNew(int iMovieWidth, int iMovieHeight, 
                                    int iVideoWidth, int iVideoHeight,
                                    int iWbWidth, int iWbHeight,
                                    int iLogoWidth, int iLogoHeight,
                                    int iControlBarWidth, int iControlBarHeight,
                                    int iMetadataWidth, int iMetadataHeight, 
                                    int &iNewWbWidth, int &iNewWbHeight, 
                                    bool bShowVideoOnPages);
   UINT CalculateSpritePositionsSlidestarNew(int iMovieWidth, int iMovieHeight, 
                                    int iVideoWidth, int iVideoHeight,
                                    int iWbWidth, int iWbHeight,
                                    /*int iLogoWidth, int iLogoHeight,*/
                                    int iControlBarWidth, int iControlBarHeight,
                                    /*int iMetadataWidth, int iMetadataHeight, */
                                    int &iNewWbWidth, int &iNewWbHeight, 
                                    bool bShowVideoOnPages);
   UINT CalculateWhiteboardScalingFromMovie(UINT nMovieWith, UINT nMovieSize);
   UINT CalculateSpritePositionsPsp(int iVideoWidth, int iVideoHeight,
                                    int iWbWidth, int iWbHeight);
   UINT CalculateSpritePositionsPspNew(int iMovieWidth, int iMovieHeight, 
                                       int iVideoWidth, int iVideoHeight,
                                       int iWbWidth, int iWbHeight, 
                                       int &iNewWbWidth, int &iNewWbHeight, 
                                       bool bShowVideoOnPages);
   UINT CalculateSpritePositionsSlidestar(int iVideoWidth, int iVideoHeight,
                                               int iWbWidth, int iWbHeight);
   UINT CalculateFlashPlayerDimension();
   UINT GetMovieSize(UINT &iMovieWidth, UINT &iMovieHeight);
   UINT GetPageTimestamps(int nLength);
  
   UINT EmbedObjects(SWFSprite *movieClip, bool bDryRun, bool bConserveObjects, 
      int nMsecPerFrame, DWORD *dwAddedObjects);
   UINT MapFlashImageFromImageObject(DrawSdk::Image *pImageObject, SWFShape **pShape, SWFBlock **pBlock, 
      CMap<CString, LPCTSTR, SWFShape*, SWFShape*> &mapImages, 
      CMap<SWFBlock*, SWFBlock*, float, float> &mapImageScalingX,
      CMap<SWFBlock*, SWFBlock*, float, float> &mapImageScalingY);
   UINT MapFlashFontFromTextObject(DrawSdk::Text *pTextObject);
   SWFFont* GetFlashFontFromTextObject(DrawSdk::Text *pTextObject);
   CQuestionnaire* GetQuestionnaireFromStartMs(int nStartMs);
   UINT ShowLostStopmarksWarningMessage();
   DWORD CommandCall(_TCHAR* pszCommandLine);
   CString GetInstallDir();
   CString GetFlashBinPath();
   CString GetFlashFdbPath();
   CString GetOutputPath();
   UINT InitClipFilenames();
   CString GetTimeStringFromMs(int timeMs);
   void GetNewDragableImageName(DrawSdk::Image *pImageObject, CString &csNewDragableImageName);
   void ReCheckStopmarkList();
   int GetPagingTimeInTimeInterval(int nFromMs, int nToMs);
   UINT CreatePreloaderSwf(Gdiplus::ARGB refBackColor);

private: // Methods in FlashEngineHelper
   // Common drawing stuff
   void DrawLine(SWFShape *pShape, float x, float y, float width, float height, 
      float fLineWidth, bool bDashedLine, bool bDottedDash, bool bDrawHairline=false);
   void DrawLineTo(SWFShape *pShape, float x1, float y1, float x2, float y2,
      float fLineWidth, bool bDashedLine, bool bDottedDash, bool bDrawHairline=false);
   void DrawBroadenLine(SWFShape *pShape, float x, float y, float width, float height, 
      float fLineWidth);
   void DrawRectangle(SWFShape *pShape, float x, float y, float width, float height,
      float fLineWidth=0.0f, bool bDashedLine=false, bool bDottedDash=false, bool bDrawHairline=false);
   void DrawPolygon(SWFShape *pShape, DrawSdk::DPoint* pPoints, int iPointCount,
      float fLineWidth=0.0f, bool bDashedLine=false, bool bDottedDash=false, bool bDrawHairline=false);
   void DrawOval(SWFShape *pShape, float x, float y, float width, float height,
      float fLineWidth=0.0f, bool bDashedLine=false, bool bDottedDash=false, bool bDrawHairline=false);
   void DrawPptLine(SWFSprite *pSprParent, float x, float y, float width, float height, 
      float fLineWidth, bool bDashedLine, bool bDottedDash);
   void DrawPptLineTo(SWFSprite *pSprParent, float x1, float y1, float x2, float y2, 
      float fLineWidth, bool bDashedLine, bool bDottedDash);
   void DrawPptRectangle(SWFSprite *pSprParent, SWFShape *pShape, 
      float x, float y, float width, float height,
      float fLineWidth=0.0f, bool bDashedLine=false, bool bDottedDash=false);
   void DrawPptOval(SWFSprite *pSprParent, SWFShape *pShape, 
      float x, float y, float width, float height,
      float fLineWidth=0.0f, bool bDashedLine=false, bool bDottedDash=false);
   void DrawLineAndArrowHeads(SWFSprite *pSprParent, int nArrowStyle, int nArrowConfig, 
      float x, float y, float dx, float dy, 
      float fLineWidth, bool bDashedLine, bool bDottedDash, Gdiplus::ARGB fcref);
   void DrawArrowHead(SWFShape *pShape, int nArrowType, int nArrowSize, 
      float x, float y, double dXn, double dYn, float fLineWidth, Gdiplus::ARGB fcref);
   void DrawPointer(SWFShape *pShape, float x, float y, float size, int nType);
   void DrawTelePointer(SWFShape *pShape, float x, float y, float size);
   void DrawMouseCursor(SWFShape *pShape, float x, float y, float size);
   void DrawText(SWFText *pSwfText, float x, float y, SWFFont *pSwfFont, CString csText, 
      float fFontSize, Gdiplus::ARGB crFontColor);
   void DrawUnderline(SWFSprite *pSprParent, float x, float y, float fLength, 
      LOGFONT lf, Gdiplus::ARGB crColor);
   void DrawImage(SWFShape *pShape, SWFBitmap *pBitmap, bool bConsiderMetadata=false);
   bool CheckForArrowHeadConsistency(int nArrowStyle, int &nBeginType, int &nBeginSize, 
      int &nEndType, int &nEndSize);

   // Control Bar stuff
   // -----------------
   // 3D geometric shapes for the control bar
   void DrawPlayShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb);
   void DrawPauseShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb);
   void DrawGotoFirstFrameShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb);
   void DrawGotoLastFrameShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb);
   void DrawGotoPrevPageShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb);
   void DrawGotoNextPageShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb);
   void DrawVolumeShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb);
   void InitSymbolShape(SWFShape *pShape, float &x, float &y, float &size, 
      MYRGB rgb100, MYRGB &rgb067, MYRGB &rgb095, MYRGB &rgb125); 
   void Draw3DRoundedRectangle(SWFShape *pShape, float x, float y, float width, float height, 
      float radius, int penSize, int penPaletteFlag, int shapeFlag, 
      int borderFlag, int fillFlag, int embossFlag, int gradientFlag, 
      SWFGradient *gradient=NULL, MYRGB *rgbBorder=NULL, MYRGB *rgbBackground=NULL);

   // Buttons for the control bar
   void CreateButtonShape(SWFShape *pShape, int r=-1, int g=-1, int b=-1, int a=0xFF);
   void DrawRectButtonShape(SWFShape *pShape, float width, float height, int r=-1, int g=-1, int b=-1, int a=0xFF);
   void DrawOvalButtonShape(SWFShape *pShape, float width, float height, int r=-1, int g=-1, int b=-1, int a=0xFF);

   // Elements for control bar
   // A common method to create a 3D style button
   void DrawCommon3dButton(SWFSprite *pParentSprite, 
      float x, float y, float width, float height,
      int buttonShapeFlag, CString csButtonName, CString csButtonTitle, 
      SWFShape *shpBtnUp, SWFShape *shpBtnOver, SWFShape *shpBtnDown, SWFShape *shpBtnDisabled, 
      _TCHAR *scriptPressed, _TCHAR *scriptReleased, _TCHAR *scriptEntered);
   void DrawDisabledButton(SWFSprite *pSprite, float x, float y, float width, float height, 
      int buttonShapeFlag, CString csButtonName, SWFShape *shpBtnDisabled, _TCHAR *scriptEntered);
   // Button elements for the control bar
   UINT CreatePlayButton(SWFSprite *pSprite, float x, float y, float width, float height);
   UINT CreatePauseButton(SWFSprite *pSprite, float x, float y, float width, float height);
   UINT CreateGotoFirstFrameButton(SWFSprite *pSprite, float x, float y, float width, float height);
   UINT CreateGotoLastFrameButton(SWFSprite *pSprite, float x, float y, float width, float height);
   UINT CreateGotoPrevPageButton(SWFSprite *pSprite, float x, float y, float width, float height);
   UINT CreateGotoNextPageButton(SWFSprite *pSprite, float x, float y, float width, float height);
   UINT CreateSoundOnButton(SWFSprite *pSprite, float x, float y, float width, float height);
   UINT CreateSoundOffButton(SWFSprite *pSprite, float x, float y, float width, float height);
   UINT CreateTimeSliderButton(SWFSprite *pSprite, float x, float y, float length, 
      float width, float height, bool bIsVertical=false);
   UINT CreateVolumeSliderButton(SWFSprite *pSprite, float x, float y, float length, 
      float width, float height, bool bIsVertical=false);
   UINT CreateSliderLine(SWFSprite *pSprite, float x, float y, float length, float size,
      CString csName, bool bIsVertical=false);
   UINT CreateTextFieldBox(SWFSprite *pSprite, float x, float y, float length, float size, 
      CString csName);
   UINT CreateTimeDisplay(SWFSprite *pSprite, float x, float y, float fontHeight, float scalePsp=1.0f);
   UINT CreateSlideDisplay(SWFSprite *pSprite, float x, float y, float fontHeight, float scalePsp=1.0f);
   UINT CreateActiveTimeLine(SWFSprite *pSprite, float x, float y, float length, float size,
      float btnWidth, bool bIsVertical=false);
   UINT CreateActiveVolumeLine(SWFSprite *pSprite, float x, float y, float length, float size, 
      float btnWidth, bool bIsVertical=false);

   //Elements for Slidestar control bar

   UINT CheckForSlidestarControlBarFlash();
   void DrawSlidestarControlbarBackground(SWFSprite *sprControl, float width, float height);   
   void CreateSlidestarCommonButtons(SWFSprite *pParentSprite, 
      float x, float y, CString csButtonName, CString csButtonTitle, char* swfImageNormal, char* swfImageOver, 
      _TCHAR *scriptPressed, _TCHAR *scriptReleased, _TCHAR *scriptEntered);

   UINT CreateSlidestarControlBar(SWFMovie *movie);
   UINT CreateSlidestarPlayButton(SWFSprite *pSprite, float x, float y);
   UINT CreateSlidestarPauseButton(SWFSprite *pSprite, float x, float y);
   UINT CreateSlidestarGotoFirstFrameButton(SWFSprite *pSprite, float x, float y);
   UINT CreateSlidestarGotoPrevPageButton(SWFSprite *pSprite, float x, float y);
   UINT CreateSlidestarGotoNextPageButton(SWFSprite *pSprite, float x, float y);
   UINT CreateSlidestarGotoLastFrameButton(SWFSprite *pSprite, float x, float y);
   UINT CreateSlidestarActiveTimeLine(SWFSprite *pSprite, float x, float y, float length, float size);
   UINT CreateSlidestarTimeSliderButton(SWFSprite *pSprite, float x, float y, float length,
      float width, float height);
   UINT CreateSlidestarSliderLine(SWFSprite *pSprite, float x, float y, CString csName, char *cSwfName);
   UINT CreateSlidestarPreloader(SWFSprite *sprControl, float x, float y, float controlbarHeight);
   UINT CreateSlidestarActiveVolumeLine(SWFSprite *pSprite, float x, float y, float length, float size, float btnWidth);
   UINT CreateSlidestarVolumeSliderButton(SWFSprite *pSprite, float x, float y, float length,
      float width, float height);
   UINT CreateSlidestarSoundOffButton(SWFSprite *pSprite, float x, float y);
   UINT CreateSlidestarSoundOnButton(SWFSprite *pSprite, float x, float y);
   

   // Interaction elements
   // --------------------
   // A common method to create an interaction area/button object
   void CreateInteractionArea(SWFSprite *sprPages, float x, float y, float width, float height,
      int idx, CInteractionArea *pInteractionArea, 
      _TCHAR *scriptClicked, _TCHAR *scriptMouseUp, _TCHAR *scriptMouseDown, 
      _TCHAR *scriptMouseOver, _TCHAR *scriptMouseOut);
   void DrawInteractionArea(SWFSprite *sprCommon, float x, float y, float width, float height,
      int idx, CInteractionArea *pInteractionArea, 
      _TCHAR *scriptClicked, _TCHAR *scriptMouseUp, _TCHAR *scriptMouseDown, 
      _TCHAR *scriptMouseOver, _TCHAR *scriptMouseOut);
   void DrawDisabledInteractionButton(SWFSprite *sprPages, float x, float y, float width, float height,
      int idx, CString csButtonName, CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *paInactiveDrawObjects);
   void DrawInteractionButtonShape(SWFButton *pButton, CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *paDrawObjects, 
      byte iFlag);
   // Create interaction areas & buttons, RadioButtons/CheckBoxes, dragable images and TextFields
   UINT CreateInteractionAreas(SWFSprite *sprPages);
   UINT CreateDragableImages(SWFSprite *sprPages);
   UINT CreateChangeableTextObjects(SWFSprite *sprPages);
   UINT CreateQuestionTextObjects(SWFSprite *sprPages);
   UINT CreateSpriteFromButtonTextObject(SWFSprite *pSprite, DrawSdk::DrawObject *pObject, float x, float y);
   UINT CreateShapeFromButtonGraphicalObject(SWFShape *pShape, DrawSdk::DrawObject *pObject);
   UINT CreateCheckBox(SWFSprite *sprPages, float x, float y, float size, int idx);
   UINT CreateRadioButton(SWFSprite *sprPages, float x, float y, float size, int idx, int idxStart, int idxEnd);
   UINT CreateDragableImage(SWFSprite *sprPages, SWFShape *pShape, float x, float y, float width, float height, int idx); 
   UINT CreateTargetArea(SWFSprite *sprPages, int idx, DrawSdk::TargetArea* pDrawObjectTarget);
   UINT CreateTextField(SWFSprite *sprPages, float x, float y, float width, float height, int idx);
   UINT CreateInteractiveText(SWFSprite *sprPages, DrawSdk::Text *pTextObject, CString csType, 
      float x, float y, float width, float height, int idx, int nStartMs);
   UINT CreateInteractionMaskingLayer(SWFSprite *sprPages);
   int GetFrameFromTimestamp(int nTimestamp);
   void GetIndexRangeForRadioButton(int idx, int &idxStart, int &idxEnd);
   // Event-Handling for interaction areas/buttons
   CString ReinterpretActionStringForFlash(CString csAction, int nPage=-1);
   CString Get2ndCommandFromActionArgument(CString csArgument, bool bAddSeparator);
   CString GetSlashSubstitutedString(CString csArgument);
   int GetIntegerFromActionArgument(CString csArgument);
   int GetPageFromInteractionArea(CInteractionArea *pInteractionArea);
   int GetPageFromActivityFrames(int iFrameBegin, int iFrameEnd);
   bool AreInteractionsInTimeInterval(int nBeginMs, int nEndMs); 

   // Draw Eval Note
   UINT DrawEvalNote(SWFMovie *movie);


   // ActionScripts for Movie, Control Bar, Buttons, Slider, etc.
   UINT AddActionScriptVariables(SWFMovie *movie);
   UINT AddActionScriptConfigurableVariables(SWFMovie *movie);
   UINT AddActionScriptInitFunctions(SWFMovie *movie, int nFlashVersion, 
                                     bool bHasPages, bool bHasVideo, bool bHasClips);
   UINT AddActionScriptCommonFunctions(SWFMovie *movie);
   UINT AddActionScriptForLoading(SWFMovie *movie);
   UINT AddActionScriptForSlidestar(SWFMovie *movie, bool bHasPages, bool bHasVideo, bool bHasClips);
   UINT AddActionScriptForSlidestar(SWFMovie *movie);
   UINT AddActionScriptForNavigation(SWFMovie *movie);
   UINT AddActionScriptForEventHandling(SWFMovie *movie);
   UINT AddActionScriptForSynchronization(SWFMovie *movie);
   UINT AddActionScriptForDisplayUpdates(SWFMovie *movie);
   UINT AddActionScriptForTooltips(SWFMovie *movie);
   UINT AddActionScriptForPagingToMovie(SWFMovie *movie);
   UINT AddActionScriptForTestsToMovie(SWFMovie *movie);
   UINT AddActionScriptForStopmarksToMovie(SWFMovie *movie, int numOfStopmarks, int* anStopmarkFrames);
   UINT AddActionScriptForTargetmarksToMovie(SWFMovie *movie, int numOfTargetmarks, int* anTargetmarkFrames);
   UINT AddActionScriptForInteractionAreasToMovie(SWFMovie *movie);
   UINT AddActionScriptForDragableImagesToMovie(SWFMovie *movie);
   UINT AddActionScriptForChangeableTextsToMovie(SWFMovie *movie);
   UINT AddActionScriptForQuestionTextsToMovie(SWFMovie *movie);
   UINT AddActionScriptForTestingToMovie(SWFMovie *movie);
   UINT AddActionScriptForFeedbackToMovie(SWFMovie *movie);
   UINT AddActionScriptForMessageBoxToMovie(SWFMovie *movie);
   UINT AddActionScriptForSaveLoadSuspendData(SWFMovie *movie);
   UINT AddActionScriptForLoadTestingData(SWFMovie *movie);
   UINT AddActionScriptForNetConnectionToMovie(SWFMovie *movie, CString csNetConnection);
   UINT AddActionScriptForFlvStreamsToMovie(SWFMovie *movie);
   UINT AddActionScriptForVideoStreamUrlToMovie(SWFMovie *movie, CString csVideoStreamUrl);
   UINT AddActionScriptForVideoStreamingToMovie(SWFMovie *movie);
   UINT AddActionScriptForClipStreamUrlsToMovie(SWFMovie *movie, CString *acsClipStreamUrls);
   UINT AddActionScriptForClipStreamingToMovie(SWFMovie *movie);
   UINT AddActionScriptToControlBar(SWFSprite *sprControl, float fScale);
   UINT AddActionScriptForPageSprite(SWFSprite *sprPages);
   void FillFeedbackEntry(SWFMovie *movie, CFeedbackDefinition *pFeedback, int qIdx, CString csAction, 
      bool bEnabled, bool bIsQuestionnaire);
   int GetIndexFromFeedbackType(CString csFeedbackType);
   int GetPageFromInteractionObject(DrawSdk::DrawObject *pDrawObject);
   int FillFeedbackSummaryStrings(CString &csResult, CString &csQuestion, CString &csAnswer, CString &csPoints, 
      CString &csMaximumPoints, CString &csRequiredPoints, CString &csRequiredPercentage, 
      CString &csAchievedPoints, CString &csAchievedPercentage);
   void PrintActionScript(CString csObject, _TCHAR *script, bool bForceRelease=false);

   // WebPlayer templates stuff
   void DrawBorderLines(SWFShape *shpBorder, float x, float y, float width, float height, int fillFlag);
   void DrawControlbarBackground(SWFSprite *sprControl, float width, float height);
   void DrawSlidesBorder(SWFMovie *movie, bool bShowMetadata);
   UINT DrawWebplayerLogo(SWFSprite *sprControl, SWFFont *fntLogo, 
      float x, float y, float scale);
   void DrawSeparator(SWFSprite *sprControl, float x, float height, CString csName);
   // Tooltip for buttons
   UINT CreateTooltipField(SWFMovie *movie, TCHAR *tszLogoUrl);
   UINT CreateTooltip(SWFButton *pButton, CString csTooltip, CString csPosition);
   // A simple "Preloader"
   UINT CreatePreloader(SWFSprite *sprControl, float x, float y, SWFFont *fntLogo, float controlbarHeight);
   // The control bar itself
   UINT CreateControlBar(SWFMovie *movie, SWFFont *fntLogo);
   UINT CreateControlBarPsp(SWFMovie *movie, SWFFont *fntLogo);

   // Additional Logo and Metadata
   UINT LoadLogoImage(SWFBitmap **ppBitmap, CString csImageFile, 
     int &logoWidth, int &logoHeight);
   UINT DrawLogoImage(SWFMovie *movie, SWFBitmap * bmpLogo, float x, float y, 
                      _TCHAR *tszLogoUrl);
   UINT DrawMetadataText(SWFMovie *movie, Gdiplus::ARGB refTextColor, SWFFont *fnt);
   UINT DrawMetadataTextAboveVideo(SWFMovie *movie, Gdiplus::ARGB refTextColor, SWFFont *fnt);
   UINT DrawDummyBackground(SWFMovie *movie, float x, float y, float width, float height, 
      Gdiplus::ARGB bgColor);
   UINT DrawPlayerBackground(SWFMovie *movie);

   UINT AddAudioToMovie(SWFMovie *movie);
   UINT AddVideoToMovie(SWFMovie *movie);
   UINT AddClipsToMovie(SWFMovie *movie);

   UINT AddWbObjectsToMovie(SWFMovie *movie, int nMscecPerFrame);
   UINT AddOtherPageRelatedContent(SWFMovie *movie, SWFSprite *sprPages);

   UINT AddNetConnectionToMovie(SWFMovie *movie);

   // A (general) message dialog for error/warning messages
   UINT DrawMessageBox(SWFMovie *movie);
   // Testing Messages
   UINT DrawTestingFeedback(SWFSprite *sprPages);

   // Read ActionScript from file
   UINT ReadActionScriptFromFile(CString csFilename, _TCHAR* tszScript);

   // Convert CString to char*
   char* GetCharFromCString(CString csString);
   // Convert CString to UTF-8 char*
   char* GetUtf8CharFromCString(CString csString);

   // new functions to structure the source code
   HRESULT InsertLogoImage(_TCHAR *tszLogoFile, _TCHAR *tszLogoUrl, 
                           bool &bShowLogoImage, SWFBitmap **ppBmpLogo);
   HRESULT CalculateFrameRate(int &nLength, int &nMsecPerFrame);

private: // Members
   
   enum DocumentTypeId
   {
      DOCUMENT_TYPE_UNKNOWN,
      DOCUMENT_TYPE_AUDIO_PAGES,
      DOCUMENT_TYPE_AUDIO_CLIPS_PAGES,
      DOCUMENT_TYPE_AUDIO_VIDEO_PAGES,
      DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES,
      DOCUMENT_TYPE_DENVER
   };

   CWhiteboardStream *m_pWhiteboardStream;
   CPageStream *m_pPageStream;

   // Objects list
   CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> m_aObjects;
   // Interaction Objects list
   CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> m_aInteractionObjects;
   // Stopmarks list
   CArray<FLASH_STOPMARK, FLASH_STOPMARK> m_aFlashStopmarks;
   // Targetmarks list (for Bugfix 5302)
   CArray<FLASH_STOPMARK, FLASH_STOPMARK> m_aFlashTargetmarks;
   // Fulltext list
   CArray<FLASH_FULLTEXT, FLASH_FULLTEXT> m_aFlashFulltexts;
   // Frame timestamps list
   CArray<int, int> m_aiFrameTimestampsMs;
   // Paging frames list
   CArray<int, int> m_aiPagingFrames;
   // Interaction areas list
   CArray<CInteractionArea*, CInteractionArea*> m_aInteractionAreas;
   // Questionnaires list
   CArray<CQuestionnaire*, CQuestionnaire*> m_aQuestionnaires;
   // Hash tables for Images and Fonts 
   CMap<CString, LPCTSTR, SWFBitmap*, SWFBitmap*> m_mapBitmaps;
   CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapImageNames;
   CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapFontNames;
   // Hash tables for warning messages belonging to font names
   CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapFontNamesFontNotFound;
   CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapFontNamesGlyphNotFound;
   CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapFontNamesStylesNotSupported;
   // Hash table for interactive opened files (which have to be copied)
   CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapInteractiveOpenedFileNames;

   // Hash table for temporarily created files
   CMap<CString, LPCTSTR, FILE*, FILE*> m_mapTempFiles;
   // Hash table for all fonts used here
   CMap<CString, LPCTSTR, SWFFont*, SWFFont*> m_mapSwfFonts;

   // 'OK' button for testing feedback
   CInteractionArea* m_pOkButton;

   DocumentTypeId m_docType;

   CString m_csErrorDetail;
   CString m_csTooShortPagesTimes;

   CString m_csInputPath;
   CString m_csSlidestarAddFiles;
   CString m_csPreloaderFilename;

   _TCHAR *m_tszSwfFile;
   _TCHAR *m_tszPreloaderSwfFile;

   _TCHAR *m_tszLadFile;
   _TCHAR *m_tszMp3File;

   _TCHAR *m_tszVideoFile;
   _TCHAR *m_tszVideoFlvFile;

   _TCHAR **m_atszClipFiles;
   _TCHAR **m_atszClipFlvFiles;

   _TCHAR *m_tszStillImageFile;

   _TCHAR *m_tszRtmpPath;

   CString m_csAuthor;
   CString m_csTitle;

   int m_iFlashVersion;
   
   int m_iAudioLengthMs;
   int m_iVideoLengthMs;
   int m_iWbLengthMs;
   int m_iMovieLengthMs;

   // Input and output dimension of whiteboard pages
   int m_iWbInputWidth;
   int m_iWbInputHeight;
   int m_iWbOutputWidth;
   int m_iWbOutputHeight;

   int m_iLogoWidth;
   int m_iLogoHeight;

   int m_iNavigationControlBar;
   int m_iNavigationStandardButtons;
   int m_iNavigationTimeLine;
   int m_iNavigationTimeDisplay;
   int m_iNavigationDocumentStructure;
   int m_iNavigationPluginContextMenu;

   int m_iMaxIndexSoFar;
   int m_iMaxInteractionIndex;
   int m_iAccVideoWidth;
   int m_iAccVideoHeight;
   int m_iStillImageWidth;
   int m_iStillImageHeight;
   int m_iNumOfClips;
   int m_iAbsoluteFramesCount;
   SIZE *m_aSizeClips;
   long *m_alClipsBeginMs;
   long *m_alClipsEndMs;

   // Dimension of complete flash movie (metadata + video + pages + control bar)
   CRect m_rcMovie;
   CRect m_rcFlashPlayer;

   CRect m_rcLogo;
   CRect m_rcMetadata;
   float m_fScaleMetadata;
   CRect m_rcVideo;
   float m_fScaleVideo;
   CRect m_rcPages;
   float m_fScalePages;
   CRect m_rcControlBar;
   float m_fScaleControlBar;
   CRect m_rcPspContent;
   float m_fScalePspContent;

   float m_fMovieOffsetX;
   float m_fMovieOffsetY;

   float m_fFrameRate;

   double m_dGlobalScale;
   double m_dContentScale;
   double m_dFeedbackScale;
   double m_dWhiteboardScale;

   bool m_bRectanglesCorrected;

   bool m_bIsNewLecPlayer;

   bool m_bAutostartMode;
   bool m_bCancelRequested;
   bool m_bCleanupCalled;
   bool m_bScormEnabled;
   bool m_bDrawEvalNote;
   bool m_bIsSlidestar;
   bool m_bIsPspDesign;
   bool m_bShowClipsInVideoArea;
   bool m_bLecIdentityHidden;
   bool m_bIncreaseKeyframes;
   bool m_bIsRtmpStreaming;
   
   bool m_bHasVideo;
   bool m_bHasClips;
   bool m_bHasMP3Audio;
   bool m_bHasFlvAudio;
   bool m_bHasPages;
   bool m_bHasControlBar;
   bool m_bHasMetadata;
   bool m_bIsSimulationDocument;
   float m_fClipSynchronisationDelta;

   ULONG_PTR m_gdiplusToken;
   HRESULT m_hrPossibleWarning;

   // Colors for background, buttons, control bar, etc.
   // (Note: the 3 digit number defines the lightness in percent)
   MYRGB m_rgb100; // background color
   MYRGB m_rgb095, m_rgb105;
   MYRGB m_rgb080, m_rgb125;
   MYRGB m_rgb067, m_rgb150;
   MYRGB m_rgb050, m_rgb200;
   MYRGB m_rgb033, m_rgb115;
   MYRGB m_rgb005, m_rgb090;
   MYRGB m_rgb045, m_rgb055;
   // Colors for button symbols
   MYRGB m_rgbUp, m_rgbOver, m_rgbDown;
   // Color for disabled button symbols
   MYRGB m_rgbDisabled;
   // Standard (outline) color for common objects
   Gdiplus::ARGB m_argbStd;
   // Extra shapes for arrow heads
   SWFShape *m_pShpArrowBegin;
   SWFShape *m_pShpArrowEnd;

   // standard font
   SWFFont  *m_pFntStd;
   SWFFont  *m_pFntLogo;

   // Some gradients for 3D effects 
   SWFGradient *m_swfLinearGradient;
   SWFGradient *m_swfRadialGradient;
   SWFGradient *m_swfRadialGradientSlider;
   SWFGradient *m_swfLinearGradientBackground;
   SWFGradient *m_swfLinearGradientMouseUp;
   SWFGradient *m_swfLinearGradientMouseDown;
   SWFGradient *m_swfLinearGradientDarker;
   SWFGradient *m_swfRadialGradientDarker;
   SWFGradient *m_swfLinearGradientLighter;
   SWFGradient *m_swfRadialGradientLighter;
   SWFGradient *m_swfRadialGradientDisabled;
   SWFGradient *m_swfRadialGradientDisabledLighter;
   SWFGradient *m_swfRadialGradientDisabledDarker;

   CString m_csFeedbackMsg;
};

#endif // !defined(AFX_SCORMENGINE_H__4F5D76F1_061E_473A_AF1E_8940C360C202__INCLUDED_)

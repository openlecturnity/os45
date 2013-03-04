#if !defined(AFX_SCORMENGINE_H__4F5D76F1_061E_473A_AF1E_8940C360C202__INCLUDED_)
#define AFX_SCORMENGINE_H__4F5D76F1_061E_473A_AF1E_8940C360C202__INCLUDED_

#include <windef.h>

#include "mingpp.h"
#include "ColorUtils.h"
#include "PageObject.h"

static float DEFAULT_SLIDES_WIDTH     = 720.0f;
static float DEFAULT_SLIDES_HEIGHT    = 540.0f;
static float DEFAULT_CONTROLBAR_HEIGHT = 35.0f;
static float MINIMUM_CONTROLBAR_WIDTH = 620.0f;
// The size of the border around the slides
static long  FLASH_BORDERSIZE = 3;
// The padding between the objects (slides, logo image, metadata, ...)
static long  FLASH_PADDING = 10;
// The height of the metadata text (equal to the font size used for the text)
static long  FLASH_METADATAHEIGHT = 16;
// The maximum size of text buffer for ActionScript files
static long  MAX_ACTIONSCRIPT_BUFFER = 65536;

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
// - Dummy DragImage:   15400
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
// - TestingMessages:   15977 - 15979
// - Tooltip:           15980

// Structure to save image information
class ImageInformation
{
public:
   ImageInformation(ASSISTANT::Image *pImage)
   {
      m_csFilename = pImage->GetImageName();
      m_csFilepath = pImage->GetImagePath();
      m_csFilepath += m_csFilename;
      m_dWidth = pImage->GetWidth();
      m_dHeight = pImage->GetHeight();

      m_pImage = pImage;

      m_pBitmapShape = NULL;
   }

   ~ImageInformation() {}

   bool IsEqual(ASSISTANT::Image *pImage)
   {
      bool bRet = true;

      if (pImage->GetImageName() != m_csFilename)
         bRet = false;
      if (abs(pImage->GetWidth() - m_dWidth) > 1)
         bRet = false;
      if (abs(pImage->GetHeight() - m_dHeight) > 1)
         bRet = false;

      return bRet;
   }

   bool IsRescaled() 
   {
      return m_csOutputFilename.IsEmpty() ? false : true;
   }

   ASSISTANT::Image *GetImage() {return m_pImage;}

   void SetInputName(CString &csInputName) {m_csFilename = csInputName;}
   void SetOutputName(CString &csOutputName) {m_csOutputFilename = csOutputName;}

   CString &GetInputName() {return m_csFilepath;}
   CString &GetOutputName() {return m_csOutputFilename;}

   void SetBitmapShape(SWFShape *pBitmapShape) {m_pBitmapShape = pBitmapShape;}
   SWFShape *GetBitmapShape() {return m_pBitmapShape;}

private:
   CString m_csFilename;
   CString m_csFilepath;
   double m_dWidth;
   double m_dHeight;
   ASSISTANT::Image *m_pImage;

   CString m_csOutputFilename;
   SWFShape *m_pBitmapShape;
};

class CFlashEngine
{
public:
	CString GetScormEnabled();
	CFlashEngine();
	virtual ~CFlashEngine();
   
   UINT SetInputFiles(const _TCHAR *tszEvqFile, 
      const _TCHAR *tszObjFile, const _TCHAR *tszAudioFile);
   UINT SetOutputFile(const _TCHAR *tszSwfFile);
   UINT SetInputDimensions(int iWidth, int iHeight);
   UINT SetOutputDimensions(int iWidth, int iHeight);
   UINT SetAutostartMode(bool bAutostartMode);
   UINT SetScormEnabled(bool bScormEnabled);
   UINT GetMovieWidth();
   UINT GetMovieHeight();
   UINT SetBasicColors(int r, int g, int b);
   UINT RescaleImages(float scaleX, float scaleY);
   UINT WriteRescaledImage(ASSISTANT::Image *pImageObject, CString &csImageOut, 
      float scaleX, float scaleY);
   // If an Image is converted without resizing, use 'csImageIn' as 'csImageTmp'
   UINT ConvertImage(ImageInformation *pImageInformation, CString csImageIn, 
                     CString csImageTmp, CString &csImageOut, LPCTSTR tszProgramPath);
   UINT ConvertImages(LPCTSTR tszProgramPath);
   UINT ConvertFonts(LPCTSTR tszProgramPath);
   UINT ConvertAudio(int iAudioBitrate);
   UINT BuildFont(CString csFont);
   UINT SetStandardNavigationStates(int nNavigationControlBar, int nNavigationStandardButtons, 
                                    int nNavigationTimeLine, int nNavigationTimeDisplay, 
                                    int nNavigationDocumentStructure, int nNavigationPluginContextMenu);
   UINT BuildMovie(CArray<ASSISTANT::Page *, ASSISTANT::Page *> &arPages,
                              Gdiplus::ARGB refBackColor, LPCTSTR tszProgramPath);
   UINT BuildMovie(ASSISTANT::Page * pPage,
                   Gdiplus::ARGB refBackColor, LPCTSTR tszProgramPath);
   UINT PrepareScorm();
   UINT PrepareCopiesOfInteractiveOpenedFiles();
   UINT Cancel();
   UINT Cleanup();
   UINT LastErrorDetail(_TCHAR *tszError, DWORD *dwSize);
   UINT ReadAndProcessPages(ASSISTANT::Page *pPage);

private:
   void FillHashTablesForDrawObject(ASSISTANT::DrawObject *pObject);
   UINT RescaleImageObject(ImageInformation *pImageInformation, float scaleX, float scaleY);
   UINT EmbedObjects(SWFMovie *movie, ASSISTANT::Page *pPage, DWORD *dwAddedObjects);
   UINT MapFlashImageFromImageObject(ASSISTANT::Image *pImageObject, SWFShape **pShape, SWFBlock **pBlock, 
      CMap<SWFBlock*, SWFBlock*, float, float> &mapImageScalingX,
      CMap<SWFBlock*, SWFBlock*, float, float> &mapImageScalingY);
   UINT MapFlashFontFromTextObject(ASSISTANT::Text *pTextObject, HRESULT &hrPossibleWarning);
   SWFFont* GetFlashFontFromTextObject(ASSISTANT::Text *pTextObject);
   DWORD CommandCall(_TCHAR* pszCommandLine);
   CString GetInstallDir();
   CString GetFlashBinPath();
   CString GetOutputPath();
   CString GetTimeStringFromMs(int timeMs);
   
   // Convert CString to char*
   char* GetCharFromCString(CString csString);
   // Convert CString to UTF-8 char*
   char* GetUtf8CharFromCString(CString csString);

private: // Methods in FlashEngineHelper
   // Common drawing stuff
   void DrawLine(SWFShape *pShape, float x, float y, float width, float height, 
      float fLineWidth, bool bDashedLine, bool bDottedDash);
   void DrawLineTo(SWFShape *pShape, float x1, float y1, float x2, float y2,
      float fLineWidth, bool bDashedLine, bool bDottedDash);
   void DrawRectangle(SWFShape *pShape, float x, float y, float width, float height,
      float fLineWidth=0.0f, bool bDashedLine=false, bool bDottedDash=false);
   void DrawOval(SWFShape *pShape, float x, float y, float width, float height,
      float fLineWidth=0.0f, bool bDashedLine=false, bool bDottedDash=false);
   void DrawLineAndArrowHeads(SWFShape *pShape, int nArrowStyle, int nArrowConfig, 
      float x, float y, float dx, float dy, 
      float fLineWidth, bool bDashedLine, bool bDottedDash, Gdiplus::ARGB fcref);
   void DrawArrowHead(SWFShape *pShape, int nArrowType, int nArrowSize, 
      float x, float y, double dXn, double dYn, float fLineWidth, Gdiplus::ARGB fcref);
   void DrawTelepointer(SWFShape *pShape, float x, float y, float size);
   void DrawText(SWFText *pSwfText, float x, float y, SWFFont *pSwfFont, CString csText, 
      float fFontSize, Gdiplus::ARGB crFontColor);
   void DrawImage(SWFShape *pShape, SWFBitmap *pBitmap);
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
   

   // Create interaction areas & buttons, RadioButtons/CheckBoxes, dragable images and TextFields
   int GetFrameFromTimestamp(int nTimestamp);
   void GetIndexRangeForRadioButton(int idx, int &idxStart, int &idxEnd);
   // Event-Handling for interaction areas/buttons
   LPTSTR GetTtfName(LOGFONT &lf);

private: // Members
   // Hash tables for Images and Fonts 
   CArray<ImageInformation *, ImageInformation *> m_aImageInformation;
   CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapFontNames;

   // Hash table for temporarily created files
   CMap<CString, LPCTSTR, FILE*, FILE*> m_mapTempFiles;
   // Hash table for all fonts used here
   CMap<CString, LPCTSTR, SWFFont*, SWFFont*> m_mapSwfFonts;

   CString m_csErrorDetail;

   CString m_csInputPath;
   _TCHAR *m_tszSwfFile;
   int m_iFlashVersion;
   int m_iMovieLengthMs;
   int m_iInputWidth;
   int m_iInputHeight;
   int m_iOutputWidth;
   int m_iOutputHeight;
   int m_iNavigationControlBar;
   int m_iNavigationStandardButtons;
   int m_iNavigationTimeLine;
   int m_iNavigationTimeDisplay;
   int m_iNavigationDocumentStructure;
   int m_iNavigationPluginContextMenu;
   int m_iMaxIndexSoFar;
   float m_fMovieWidth;
   float m_fMovieHeight;
   float m_fSlideOffsetX;
   float m_fSlideOffsetY;
   float m_fControlbarHeight;
   float m_fScale;
   float m_fFrameRate;
   double m_dGlobalScale;
   double m_dFeedbackScale;
   bool m_bAutostartMode;
   bool m_bCancelRequested;
   bool m_bCleanupCalled;
   bool    m_bScormEnabled;

   ULONG_PTR m_gdiplusToken;

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

   // standard font
   SWFFont  *m_fntStd;

};

#endif // !defined(AFX_SCORMENGINE_H__4F5D76F1_061E_473A_AF1E_8940C360C202__INCLUDED_)

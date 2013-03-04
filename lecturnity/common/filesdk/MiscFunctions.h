#ifndef __MISCFUNCTIONS__
#define __MISCFUNCTIONS__

#include "StdAfx.h"
#include "export.h"

class FILESDK_EXPORT StringManipulation
{
public:
   StringManipulation() {}
   ~StringManipulation() {}

public:
   // extracts path from complete filename
   static void GetPath(CString &fileName);
   // extracts filename from complete path
   static void GetFilename(CString &fileName);
   // extracts file prefix from completet filename
   static void GetFilePrefix(CString &prefix);
   // extracts path & file prefix from completet filename
   static void GetPathFilePrefix(CString &prefix);
   // extracts file suffix from completet filename
   static void GetFileSuffix(CString &suffix);
   // translate (rgb-)color information from an object-file into a COLORREF value
   static COLORREF GetColorref(CString &color, bool &transparent);
   static Gdiplus::ARGB GetARGB(CString &color);
   // methods for command comparison and parameter extraction
   static bool StartsWith(CString& csLonger, CString& csShorter);
   static bool GetParameter(CString& csActionString);
   static bool GetNextCommand(CString& csActionString);
   // backslash special characters (<, >, \, ") for html 
   static void TransformForSgml(CString &sourceString, CString &targetString);
   static void TransformForSgml(LPCTSTR sourceString, CString &targetString);
   static void TransformFromSgml(CString &sourceString, CString &targetString);
   static void TransformFromSgml(LPCTSTR sourceString, CString &targetString);

   static CString MakeRelative(LPCTSTR szLepFileName, LPCTSTR szAbsTarget);
   static CString MakeRelativeAndSgml(LPCTSTR szLepFileName, LPCTSTR szAbsTarget);
   static CString MakeAbsolute(LPCTSTR szLepFileName, LPCTSTR szRelTarget);
   // remove filename reserved characters from csFilename string
   static void RemoveReservedFilenameChars(CString &csFilename);
   static void ReplaceHtmlSpecialCharacters(CString &csStr);

   static void EncodeXmlSpecialCharacters(CString &csStr);
   static void DecodeXmlSpecialCharacters(CString &csStr);

   //static bool VerifyInternationalFilename(HWND hWndParent, LPTSTR tszFileName, bool bSave);

   // extract time periods of activity/visibility from a string to CPoints as pairs of beginning/end timestamps
   // (returns S_OK or E_FAIL)
   static HRESULT ExtractAllTimePeriodsFromString(CString csAllTimePeriods, CArray<CPoint, CPoint> *paTimePeriods);
   static HRESULT ExtractOneTimePeriodFromString(CString csOneTimePeriod, CArray<CPoint, CPoint> *paTimePeriods);
};

class FILESDK_EXPORT FontManipulation
{
public:
   FontManipulation() {}
   ~FontManipulation () {}

public:
   // fills the logfont structure according given parameters
   static void Create(CString &fontFamily, int size, CString &weight, CString &slant, bool bUnderline, LOGFONT &logFont);
   // fills the logfont structure with default parameters
   static void CreateDefaultFont(LOGFONT &logFont, bool isBold = false, bool isItalic = false);
};

class FILESDK_EXPORT CStringBuffer
{
public:
   CStringBuffer(int nInitialCharacters=256);
   ~CStringBuffer();

   LPCTSTR GetStringBuffer();
// It seems that inline declared methods can not be linked statically
#ifndef _FILESDK_STATIC
   void inline Empty();
   bool inline IsEmpty();
   void inline Add(_TCHAR c);
#else
   void Empty();
   bool IsEmpty();
   void Add(_TCHAR c);
#endif
   void Add(LPCTSTR lpString);
private:
   _TCHAR *m_pBuffer;
   int   m_nBufferSize;
   int   m_nBufferPos;
};

class AofColor
{

public:
   AofColor() {}
   ~AofColor() {}


   static int colArray[63][3];

   static int Get(int red, int green, int blue);
   static Gdiplus::ARGB GetARGB(int index);
   static COLORREF Get(int index);
};

class FILESDK_EXPORT Calculator
{
public: 
   Calculator() {}
   ~Calculator() {}

   static int GetPixelFromMsec(int msec, int widthInPixel, int widthInMsec, int msecOffset = 0);
   static int GetMsecFromPixel(int pixel, int widthInPixel, int widthInMsec);
   static void FitRectInRect(int containerWidth, int containerHeight,
                             int contentWidth, int contentHeight,
                             int *newWidth, int *newHeight,
                             bool stretch=false);
};

class FILESDK_EXPORT FileInformation
{
public: 
   FileInformation() {}
   ~FileInformation() {}

   //static bool FileBeginsWith(LPCTSTR filename, LPCTSTR compareString, DWORD count);
   static bool DirectoryContainsLrd(LPCTSTR directory, LPCTSTR excludingLrd);
};

class FILESDK_EXPORT VideoInformation
{
public:
   VideoInformation() {}
   ~VideoInformation() {}

   // PZI: write poster frame at specified time and scaled to specified dimensions as image file according to file extention
   static bool WritePosterFrame(LPCTSTR tszImageFileName, LPCTSTR tszVideoFileName, int streamTimeMsec=0, int width=0, int height=0 );
   // PZI: additional argument "streamTimeMsec" to get poster frame at the specified time (default is 0)
   static bool GetPosterFrame(LPCTSTR tszFileName, int &width, int &height, HBITMAP *phBitmap, int streamTimeMsec = 0);
   static bool GetVideoDuration(LPCTSTR tszFileName, int &durationMs);
   static bool GetVideoFramerate(LPCTSTR tszFileName, double &framerate);
   static bool GetVideoSize(LPCTSTR tszFileName, SIZE &sizeVideo);
   static bool GetVideoInformation(LPCTSTR tszFileName, VIDEOINFOHEADER *pVideoInfoHeader, 
      double &durationSec, double &framerate);
   static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};

class FILESDK_EXPORT FileDialog
{
public:
   FileDialog() {}
   ~FileDialog() {}

   static bool GetOpenFilename(CWinApp *pApp, CString &csOpenFilename, HWND cWnd, HINSTANCE instance, 
                               LPCTSTR tszFilter, LPCTSTR tszId, LPCTSTR tszInitialDir=NULL);
   static bool GetSaveFilename(CWinApp *pApp, CString &csSaveFilename, HWND cWnd, HINSTANCE instance, 
                               LPCTSTR tszFilter, LPCTSTR tszFileExtension, LPCTSTR tszId, LPCTSTR tszInitialDir=NULL, LPCTSTR tszInitialFileName=NULL,
                               bool bOverwritePrompt=true);
};

class FILESDK_EXPORT Explore
{
public:
   static bool ExploreLecturnityHome();
};

class FILESDK_EXPORT SystemInfo
{
public:
   //static bool Check16BitOrHigher();
   static void CalculateDiskSpace(LPCTSTR pszPath, int &mbLeft);
};

class FILESDK_EXPORT ImageProcessor
{
public:
   //static bool Check16BitOrHigher();
   static Gdiplus::Image * LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL);
};

#endif //__MISCFUNCTIONS__
/*********************************************************************

 program : mlb_misc.h
 authors : Gabriela Maass
 date    : 29.11.2000
 desc    : Miscelaneous functions 

 **********************************************************************/

#ifndef __ASSISTANT_MISC__
#define __ASSISTANT_MISC__

#pragma warning (disable: 4786)

//#include <windows.h>

#include "la_cliplist.h"
#include "mlb_types.h"
#include "sgml.h"
#include <gdiplus.h>

namespace ASSISTANT 
{

   enum filetype 
   {
      PROJECT,
      DOCUMENT,
      UNDEFINED
   };

   void RemoveClosingSlash(CString &path);
   void GetPath(CString &path);
   void GetName(CString &name);
   void GetDrive(CString &drive);
   void GetPrefix(CString &prefix);
   void GetSuffix(CString &suffix);

   void GetUserHome(CString &ssUserHome);
   bool LecturnityHomeEntryExist();
   bool GetLecturnityHome(CString &lecturnityHome);
   void SetLecturnityHome(CString &lecturnityHome);
   void GetAppData(CString &appData);
   void GetSourceDirectory(CString &sourceDirectory);
   void GetRecordingDirectory(CString &recordingDirectory);

   int SwapArrowConfig(int arrowConfig);
   int GetBeginArrowConfig(int arrowConfig);
   int GetEndArrowConfig(int arrowConfig);
   int GetIntArrowStyle(LPCTSTR arrowStyle);
   void GetArrowConfig(int iArrowStyle, int &iArrowConfig, CString &csArrowStyle);

   int CalculateZoomedLineWidth(int lineWidth, float zoomFaktor);
   void CalculateNextDataPath(CString &csDateTime, const _TCHAR *recordPath, const _TCHAR *prefix, CString &dataPath);
   void CalculateNextPrefix(CString &csPrefix, CStringArray &arExistingPrefix);
   bool FileExist(const _TCHAR *filename);
   bool DirectoryExist(const _TCHAR *szDirectoryName);
   bool TryNextDataPath(const _TCHAR *recordPath, const _TCHAR *prefix);
   bool DeleteDirectory(const _TCHAR *directory);

   void GetTimeString(CString &timeString);
   void TranslateMsecToTimeString(int msec, CString &timeString);
   void GetDateString(CString &dateString, int language);
   /*String must be in format dd.mm.yyyy*/
   bool GetDateFromString(SYSTEMTIME &sTime, CString &csDate);
   /*String must be in format: hh:mm*/
   bool GetTimeFromString(SYSTEMTIME &sTime, CString &csTime);

   void CreateListFromString(CString &srcString, CStringArray &destList);
   void CreateStringFromList(CString &destString, CStringArray &srcList);

   void FillLogFont(LOGFONT &logFont, LPCTSTR fontFamily, int fontSize, 
                    LPCTSTR fontWeight, LPCTSTR fontSlant, BOOL bIsUnderlined = FALSE);

   // Convert Gdiplus color to "#aarrggbb" format and vice versa (used for LSD)
   void GdipARGBToString(Gdiplus::ARGB argb, CString &csColor);
   Gdiplus::ARGB StringToGdipARGB(LPCTSTR color);

   // Convert COLORREF RGB data to Gdiplus ARGB and vice versa
   COLORREF GdipARGBToColorref(Gdiplus::ARGB argb);
   Gdiplus::ARGB ColorrefToGdipARGB(COLORREF clr, int iTransperency=255);

   // Convert Gdiplus dash style to " ", "." or "_" format and vice versa (used for LSD)
   void GdipLineStyleToString(Gdiplus::DashStyle gdipLineStyle, CString &csStyle);
   Gdiplus::DashStyle StringToGdipLineStyle(LPCTSTR tszStyle);

   // Convert Gdiplus color to "0xaarrggbb" format (used for OBJ)
   void GdipARGBToObjString(Gdiplus::ARGB argb, CString &csColor);
   int GdipLineStyleToObjStyle(Gdiplus::DashStyle gdipLineStyle);

   Gdiplus::ARGB MakeARGB(int iRed, int iGreen, int iBlue, int iTransperency);

#ifndef _STUDIO
   bool IsEditorInstalled();
   void GetProgramPath(CString &csProgramPath);
#endif

   bool GetShellFolders(const _TCHAR *subFolder, CString &returnValue);
   CString GetInitialDir(LPCTSTR szFileType, bool bOpen);
   void SetInitialDir(CString &csDir, LPCTSTR szFileType, bool bOpen);
   HRESULT GetOpenFilename(UINT idFilter, CString &csFilename, LPCTSTR szFileType);
   HRESULT GetSaveFilename(UINT idFilter, CString &csFilename, LPCTSTR szExtension, LPCTSTR szFileType);
   
   
   double ComputeDistance(Gdiplus::PointF &ptLineBegin, Gdiplus::PointF &ptLineEnd, 
      Gdiplus::PointF &ptPoint);

   WCHAR *ConvertTCharToWChar(LPCTSTR tszString);

   void GetPossibleKeys(CStringArray &arPossibleKeys);
}

#endif

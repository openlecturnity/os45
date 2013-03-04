#include "StdAfx.h"

#include "MiscFunctions.h"
#include "FilenameChangeDlg.h"
#include "IntlFilenameWarningDlg.h"
#include "lutils.h"
#include "dvdmedia.h"

// PZI: required for CImage in WritePosterFrameAsPng()
#include <atlimage.h> // TODO: use GDI+ instead

#include <gdiplus.h>
using namespace Gdiplus;


/*********************************************
 *********************************************/

/*
Extract the path information from an complete filename, without terminating \
*/
void StringManipulation::GetPath(CString &fileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int pos = fileName.ReverseFind('\\');
   if (pos == -1)
      pos = fileName.ReverseFind(_T('/'));
   if (pos != -1)
      fileName.Delete(pos, fileName.GetLength()-pos);
}

void StringManipulation::GetFilename(CString &fileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int pos = fileName.ReverseFind(_T('\\'));
   if (pos == -1)
      pos = fileName.ReverseFind(_T('/'));
   if (pos != -1)
      fileName.Delete(0, pos+1);
}

void StringManipulation::GetFilePrefix(CString &prefix)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int pos = prefix.ReverseFind(_T('\\'));
   if (pos == -1)
      pos = prefix.ReverseFind(_T('/'));
   if (pos != -1)
      prefix.Delete(0, pos+1);
   
   pos = prefix.ReverseFind(_T('.'));
   if (pos != -1)
      prefix.Delete(pos, prefix.GetLength()-pos);
}

void StringManipulation::GetPathFilePrefix(CString &prefix)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int pos = prefix.ReverseFind(_T('.'));
   if (pos != -1)
      prefix.Delete(pos, prefix.GetLength()-pos);
}

void StringManipulation::GetFileSuffix(CString &suffix)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int pos = suffix.ReverseFind(_T('.'));
   if (pos != -1)
      suffix.Delete(0, pos+1);

}


CString StringManipulation::MakeRelative(LPCTSTR szLepFileName, LPCTSTR szAbsTarget)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   _TCHAR szRelPath[_MAX_PATH];
   if (!LIo::MakeRelativePath(szRelPath, szLepFileName, szAbsTarget))
      _tcscpy(szRelPath, szAbsTarget);
   CString csTempPath = szRelPath;

   return csTempPath;
}

CString StringManipulation::MakeRelativeAndSgml(LPCTSTR szLepFileName, LPCTSTR szAbsTarget)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   _TCHAR szRelPath[_MAX_PATH];
   if (!LIo::MakeRelativePath(szRelPath, szLepFileName, szAbsTarget))
      _tcscpy(szRelPath, szAbsTarget);
   CString csTempPath;
   StringManipulation::TransformForSgml(szRelPath, csTempPath);

   return csTempPath;
}

CString StringManipulation::MakeAbsolute(LPCTSTR szLepFileName, LPCTSTR szRelTarget)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   _TCHAR szAbsPath[_MAX_PATH];
   
   if (!LIo::MakeAbsolutePath(szAbsPath, szLepFileName, szRelTarget))
   {
      _tcscpy(szAbsPath, szRelTarget);
   }

   return CString(szAbsPath);
}

/*
bool StringManipulation::VerifyInternationalFilename(HWND hWndParent, LPTSTR tszFileName, bool bSave)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CString csSlash(_T('\\'));
   CString csFilePrefix = tszFileName;
   CString csPath       = tszFileName;
   CString csSuffix     = tszFileName;
   if (csPath.ReverseFind(_T('/')) != -1)
      csSlash = _T('/');
   GetFilePrefix(csFilePrefix);
   GetPath(csPath);
   GetFileSuffix(csSuffix);

   bool bHasHighChars = false;
   int nLen = csFilePrefix.GetLength();
   for (int i=0; i<nLen && !bHasHighChars; ++i)
   {
      _TCHAR c = csFilePrefix[i];
      if (c > 127 || c < 32 || c == _T('?'))
         bHasHighChars = true;
   }

   bool bReturn = false;

   if (bHasHighChars)
   {
      if (bSave)
      {
         bool bDoNotVerifyFilenames = false;
         LRegistry::ReadSettingsBool(_T("FileSDK"), _T("DoNotVerifyIntlFilenames"), &bDoNotVerifyFilenames, false);

         if (!bDoNotVerifyFilenames)
         {
            CWnd wndParent;
            wndParent.Attach(hWndParent);

            CIntlFilenameWarningDlg ifwd(&wndParent);
            int nRes = ifwd.DoModal();

            if (ifwd.GetDontShowDialogAgain() == true)
               LRegistry::WriteSettingsBool(_T("FileSDK"), _T("DoNotVerifyIntlFilenames"), true);

            if (IDYES == nRes)
            {
               CFilenameChangeDlg fdc(csFilePrefix, &wndParent);
               int nRes = fdc.DoModal();

               if (nRes == IDOK)
               {
                  _stprintf(tszFileName, _T("%s%s%s.%s"), csPath, csSlash, fdc.GetFileName(), csSuffix);
                  bReturn = true;
               }
            }

            wndParent.Detach();
         }
      }
      else
      {
         // Load case
      }

      return false;
   }
   else
   {
      bReturn = true;
   }

   return bReturn;
}
*/

/*
 * extract time periods of activity/visibility from a string to CPoints as pairs of beginning/end timestamps
 */
HRESULT StringManipulation::ExtractAllTimePeriodsFromString(CString csAllTimePeriods, CArray<CPoint, CPoint> *paTimePeriods)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  HRESULT hr = S_OK;

#ifdef _DEBUG   
   printf("ExtractAllTimePeriodsFromString(%s)\n", csAllTimePeriods);
#endif

   // Separator for different time periods is ';'.
   // [Separator for text - time periods is ' ' (blank).]

   // Divide String into substrings of time periods
   // and get time periods from the substrings
   CString csOneTimePeriod = _T("");
   int idx = 0;
   while ((SUCCEEDED(hr)) && (idx > -1) && (idx < csAllTimePeriods.GetLength()))
   {
      int idxNew = csAllTimePeriods.Find(_T(';'), idx);

      if (idxNew > -1)
      {
         csOneTimePeriod = csAllTimePeriods.Mid(idx, idxNew-idx);
         idx = idxNew +1;
      }
      else
      {
         csOneTimePeriod = csAllTimePeriods.Mid(idx);
         idx = idxNew;
      }

      if (csOneTimePeriod.GetLength() > 0)
         hr = ExtractOneTimePeriodFromString(csOneTimePeriod, paTimePeriods);
   }

   return hr;
}

HRESULT StringManipulation::ExtractOneTimePeriodFromString(CString csOneTimePeriod, CArray<CPoint, CPoint> *paTimePeriods)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   // Separator in time periods between 2 timestamps is '-',
   // but one single timestamp (without separator) is also possible.
   // The String can also contain text (e.g. "all-pages", ...)

   CString csBegin = "";
   CString csEnd = "";
   bool bSeparatorFound = false;
   int i = 0;

   while (i < csOneTimePeriod.GetLength())
   {
      if ((csOneTimePeriod.GetAt(i) > 47) && (csOneTimePeriod.GetAt(i) < 58))
      {
         if (bSeparatorFound)
            csEnd += csOneTimePeriod.GetAt(i);
         else
            csBegin += csOneTimePeriod.GetAt(i);
      }
      else if (csOneTimePeriod.GetAt(i) == '-')
      {
         // Separator found, 
         // but there must be a valid beginning timestamp 
         // otherwise it must be ignored (e.g. for text "page-end", "all-pages").
         if (csBegin.GetLength() > 0)
            bSeparatorFound = true;
      }

      i++;
   }

   // At least one timestamp should be found (the beginning timestamp)
   // if not --> return error code
   if (csBegin.GetLength() == 0)
      return E_FAIL;

   int nMsBegin = _ttoi(csBegin);

   // if no end timestamp is found --> end timestamp = begin timestamp
   int nMsEnd = (csEnd.GetLength() > 0) ? _ttoi(csEnd) : nMsBegin;

   // Add the time period to the array of time periods
   CPoint point(nMsBegin, nMsEnd);
   paTimePeriods->Add(point);

#ifdef _DEBUG   
   printf("ExtractOneTimePeriodFromString(%s): %d, %d  (%d)\n", csOneTimePeriod, point.x, point.y, paTimePeriods->GetSize());
#endif

   return hr;
}

/*********************************************
 *********************************************/

int AofColor::colArray[63][3] = { 
    {  0,  0,  0},{255,  0,  0},{  0,255,  0}, /* standard */
    {255,255,  0},{  0,  0,255},{255,  0,255},
    {  0,255,255},{255,255,255},
    {213,213,213},{190,190,190},{124,124,124}, /*grey */
    {103,103,103},{ 85, 85, 85},{ 51, 51, 51},
    { 28, 28, 28},
    {255,204,204},{255,154,154},{255,103,103}, /*red*/
    {255, 51, 51},{204,  0,  0}, {154,  0,  0},
    {103,  0,  0},{ 51,  0,  0},
    {204,255,204},{154,255,154},{103,255,103}, /*green*/
    { 51,255, 51},{  0,204,  0},{  0,154,  0},
    {  0,103,  0},{  0, 51,  0},
    {255,255,204},{255,255,154},{255,255,103}, /*yellow*/
    {255,255, 51},{204,204,  0},{154,154,  0},
    {103,103,  0},{ 51, 51,  0},
    {204,204,255},{154,154,255},{103,103,255}, /*blue*/
    { 51, 51,255},{  0,  0,204},{  0,  0,154},
    {  0,  0,103},{  0,  0, 51},
    {255,204,255},{255,154,255},{255,103,255}, /*pink*/
    {255, 51,255},{204,  0,204},{154,  0,154},
    {103,  0,103},{ 51,  0, 51},
    {204,255,255},{154,255,255},{103,255,255}, /*turquoise*/
    { 51,255,255},{  0,204,204},{  0,154,154},
    {  0,103,103},{  0, 51, 51}};

int AofColor::Get(int red, int green, int blue)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int ret = 0;

   int iRaw1 = (colArray[0][0] - red) * (colArray[0][0] - red) + 
      (colArray[0][1] - green) * (colArray[0][1] - green) + 
      (colArray[0][2] - blue) * (colArray[0][2] - blue);
   int min_dist = (int)sqrt((double)iRaw1);

   for (int i = 1; i < 63; i++) 
   {
      int iRaw2 = (colArray[i][0] - red) * (colArray[i][0] - red) +
         (colArray[i][1] - green) * (colArray[i][1] - green) +
         (colArray[i][2] - blue) * (colArray[i][2] - blue);
      
      int zw_dist = (int)sqrt((double)iRaw2);
     if (zw_dist < min_dist) 
     {
       min_dist = zw_dist;
       ret = i;
     }
   }

   return ret;
}

COLORREF AofColor::Get(int index)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (index >= 63)
      return RGB(255, 255, 255);
   else
      return RGB(colArray[index][0], colArray[index][1], colArray[index][2]); 
}

Gdiplus::ARGB AofColor::GetARGB(int index)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (index >= 63)
      return Gdiplus::Color::MakeARGB(0, 255, 255, 255);
   else
      return Gdiplus::Color::MakeARGB(0, colArray[index][0], colArray[index][1], colArray[index][2]);
}


COLORREF StringManipulation::GetColorref(CString &color, bool &transparent)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   
   COLORREF clrRet = RGB(255, 255, 255);
   
   transparent = false;

   if (color.GetLength() == 8)
   {
      _TCHAR colorBuf[9];
      _tcsncpy(colorBuf, color, color.GetLength());
      colorBuf[8] = '\0';
      clrRet = _tcstoul(colorBuf, NULL, 16);
      BYTE red = (BYTE) (((COLORREF)(clrRet >> 16) & 0x000000ff));
      BYTE green = (BYTE) (((COLORREF)(clrRet >> 8) & 0x000000ff));
      BYTE blue = (BYTE) (((COLORREF)(clrRet) & 0x000000ff));
      clrRet = RGB(red, green, blue);
   }
   else if (color.GetLength() == 10)
   {
      transparent = true;
      _TCHAR colorBuf[11];
      _tcsncpy(colorBuf, color, color.GetLength());
      colorBuf[10] = '\0';
      clrRet = _tcstoul(colorBuf, NULL, 16);
      BYTE red = (BYTE) (((COLORREF)(clrRet >> 16) & 0x000000ff));
      BYTE green = (BYTE) (((COLORREF)(clrRet >> 8) & 0x000000ff));
      BYTE blue = (BYTE) (((COLORREF)(clrRet) & 0x000000ff));
      clrRet = RGB(red, green, blue);
   }

   return clrRet;
}

Gdiplus::ARGB StringManipulation::GetARGB(CString &color)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   
   COLORREF clrRet = Gdiplus::Color::MakeARGB(0, 255, 255, 255);
   
   if (color.GetLength() == 8)
   {
      _TCHAR colorBuf[9];
      _tcsncpy(colorBuf, color, color.GetLength());
      colorBuf[8] = '\0';
      clrRet = _tcstoul(colorBuf, NULL, 16);
      clrRet += 0xff000000;
   }
   else if (color.GetLength() == 10)
   {
      _TCHAR colorBuf[11];
      _tcsncpy(colorBuf, color, color.GetLength());
      colorBuf[10] = '\0';
      clrRet = _tcstoul(colorBuf, NULL, 16);
   }

   return clrRet;
}

bool StringManipulation::StartsWith(CString& csLonger, CString& csShorter)
{
   if (csShorter.GetLength() > csLonger.GetLength())
      return false;
   else
   {
      for (int i=0; i<csShorter.GetLength(); ++i)
      {
         if (csShorter.GetAt(i) != csLonger.GetAt(i))
            return false;
      }

      return true;
   }
}

bool StringManipulation::GetParameter(CString& csActionString)
{
   int idxOne = csActionString.Find(_T(" "));
   int idxTwo = csActionString.Find(_T(";"));
   if (idxTwo < 0)
      idxTwo = csActionString.GetLength();

   if (idxOne > -1)
      csActionString = csActionString.Mid(idxOne + 1, idxTwo - (idxOne + 1));
   else
      csActionString.Empty();

   return idxOne > -1;
}

bool StringManipulation::GetNextCommand(CString& csActionString)
{
   int idx = csActionString.Find(_T(";"));
   if (idx > -1)
      csActionString = csActionString.Right(csActionString.GetLength() - (idx + 1));
   else
      csActionString.Empty();

   return idx > -1;
}
   

void StringManipulation::TransformForSgml(CString &sourceString, CString &targetString)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   targetString.Empty();

   for (int i = 0; i < sourceString.GetLength(); ++i)
   {
      if (sourceString[i] == '<' ||
          sourceString[i] == '>' || 
          sourceString[i] == '\"' || 
          sourceString[i] == '\\' ||
          sourceString[i] == '\'')
          targetString += '\\';
      targetString += sourceString[i];
   }
   
}

void StringManipulation::TransformForSgml(LPCTSTR sourceString, CString &targetString)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   targetString.Empty();

   CString csTemp = sourceString;
   TransformForSgml(csTemp, targetString);

   // This was double code:
   /*
   char *p = (char *)sourceString;
   for (unsigned int i = 0; i < strlen(sourceString); ++i)
   {
      if (*p == '<' ||
          *p == '>' || 
          *p == '\"' || 
          *p == '\\')
          targetString += '\\';
      targetString += *p;
      ++p;
   }
   */
}

void StringManipulation::TransformFromSgml(CString &sourceString, CString &targetString)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   targetString.Empty();

   for (int i = 0; i < sourceString.GetLength(); ++i) {
       if (i+1 < sourceString.GetLength() && 
           sourceString[i] == _T('\\') &&
           (sourceString[i+1] == '<' ||
            sourceString[i+1] == '>' || 
            sourceString[i+1] == '\"' || 
            sourceString[i+1] == '\\' ||
            sourceString[i+1] == '\'')) 
            ;   // ignore
       else
           targetString += sourceString[i];
   }
   
}

void StringManipulation::TransformFromSgml(LPCTSTR sourceString, CString &targetString)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   targetString.Empty();

   CString csTemp = sourceString;
   TransformFromSgml(csTemp, targetString);
   
}

void StringManipulation::RemoveReservedFilenameChars(CString &csFilename) {
   csFilename.Replace(_T("<"), _T("_")); //(less than)
   csFilename.Replace(_T(">"), _T("_")); //(greater than)
   csFilename.Replace(_T(":"), _T("_")); //(colon)
   csFilename.Replace(_T("\""), _T("_"));//(double quote)
   csFilename.Replace(_T("/"), _T("_")); //(forward slash)
   csFilename.Replace(_T("\\"), _T("_"));//(backslash)
   csFilename.Replace(_T("|"), _T("_")); //(vertical bar or pipe)
   csFilename.Replace(_T("?"), _T("_")); //(question mark)
   csFilename.Replace(_T("*"), _T("_")); //(asterisk)
}

void StringManipulation::ReplaceHtmlSpecialCharacters(CString &csStr) {
    csStr.Replace(_T("&"), _T("&amp;"));
    csStr.Replace(_T("<"), _T("&lt;"));
    csStr.Replace(_T(">"), _T("&gt;"));
}

void StringManipulation::EncodeXmlSpecialCharacters(CString &csStr) {
    csStr.Replace(_T("&"), _T("&amp;"));
    csStr.Replace(_T("<"), _T("&lt;"));
    csStr.Replace(_T(">"), _T("&gt;"));
    csStr.Replace(_T("\""), _T("&quot;"));
    csStr.Replace(_T("'"), _T("&apos;"));
}

void StringManipulation::DecodeXmlSpecialCharacters(CString &csStr) {
    //csStr.Replace(_T("&amp;"), _T("&"));
    csStr.Replace(_T("&lt;"), _T("<"));
    csStr.Replace(_T("&gt;"), _T(">"));
    csStr.Replace(_T("&quot;"), _T("\""));
    csStr.Replace(_T("&apos;"), _T("'"));
	csStr.Replace(_T("&amp;"), _T("&"));
}

/*********************************************
 *********************************************/

void FontManipulation::Create(CString &fontFamily, int size, CString &weight, CString &slant, bool bUnderline, LOGFONT &logFont)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   ZeroMemory(&(logFont), sizeof(LOGFONT));  
												
   logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE; 

   logFont.lfHeight = (-1) * size;

   logFont.lfWidth       = 0;
   logFont.lfEscapement  = 0 ;
   logFont.lfOrientation = 0 ;

   if (weight == _T("bold"))
      logFont.lfWeight = FW_BOLD;
   else
      logFont.lfWeight = FW_NORMAL;

   if (slant == _T("italic"))
      logFont.lfItalic = true;
   else
      logFont.lfItalic = false;

   logFont.lfUnderline     = bUnderline ;
   logFont.lfStrikeOut     = FALSE ;
   logFont.lfCharSet       = DEFAULT_CHARSET ;
   logFont.lfOutPrecision  = OUT_TT_PRECIS; //OUT_TT_ONLY_PRECIS ;   
   logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS ;   
   logFont.lfQuality       = PROOF_QUALITY; 
   
   if (fontFamily == _T("Helvetica") || fontFamily == _T("Helvetica-Bold") ||
       fontFamily == _T("Helvetica-Oblique") || fontFamily == _T("Helvetica-BoldOblique"))
   {
      _tcscpy(logFont.lfFaceName, _T("Arial"));  
   }
   else if (fontFamily == _T("Times-Roman") || fontFamily == _T("Times-Bold") ||
            fontFamily == _T("Times-Italic") || fontFamily == _T("Times-BoldItalic"))
   {
      _tcscpy(logFont.lfFaceName, _T("Times New Roman"));  
   }
   else if (fontFamily == _T("LucidaSans-Roman") || fontFamily == _T("LucidaSans-Bold") || 
            fontFamily == _T("LucidaSans-Italic") || fontFamily == _T("LucidaSans-BoldItalic"))
   {
      _tcscpy(logFont.lfFaceName, _T("Lucida Console"));  
   }
   else
      _tcscpy(logFont.lfFaceName, fontFamily);  
}

void FontManipulation::CreateDefaultFont(LOGFONT &logFont, bool isBold, bool isItalic)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   ZeroMemory(&(logFont), sizeof(LOGFONT));  
												
   logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE; 

   logFont.lfHeight = (-1) * 11;

   logFont.lfWidth       = 0;
   logFont.lfEscapement  = 0 ;
   logFont.lfOrientation = 0 ;

   if (isBold)
      logFont.lfWeight = FW_BOLD;
   else
      logFont.lfWeight = FW_NORMAL;

   if (isItalic)
      logFont.lfItalic = true;
   else
      logFont.lfItalic = false;

   logFont.lfUnderline     = FALSE ;
   logFont.lfStrikeOut     = FALSE ;
   logFont.lfCharSet       = DEFAULT_CHARSET ;
   logFont.lfOutPrecision  = OUT_TT_PRECIS; //OUT_TT_ONLY_PRECIS ;   
   logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS ;   
   logFont.lfQuality       = PROOF_QUALITY; 
   
   _tcscpy(logFont.lfFaceName, _T("Arial"));  
}

/*********************************************
 *********************************************/

CStringBuffer::CStringBuffer(int nInitialCharacters)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_pBuffer = NULL;
   if (nInitialCharacters > 0)
   {
       m_pBuffer = new _TCHAR[nInitialCharacters + 1];
       ZeroMemory(m_pBuffer, (nInitialCharacters + 1) * sizeof _TCHAR);
   }

   m_nBufferSize = (nInitialCharacters > 0) ? nInitialCharacters : 0;
   m_nBufferPos  = 0;
}

CStringBuffer::~CStringBuffer()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_pBuffer)
      delete[] m_pBuffer;
   m_pBuffer = NULL;
   m_nBufferSize = 0;
}

#ifndef _FILESDK_STATIC
void inline CStringBuffer::Empty()
#else
void CStringBuffer::Empty()
#endif
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_pBuffer)
      m_pBuffer[0] = '\000';
   m_nBufferPos = 0;
}

#ifndef _FILESDK_STATIC
bool inline CStringBuffer::IsEmpty()
#else
bool CStringBuffer::IsEmpty()
#endif
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return (m_nBufferPos == 0);
}

#ifndef _FILESDK_STATIC
void inline CStringBuffer::Add(_TCHAR c)
#else
void CStringBuffer::Add(_TCHAR c)
#endif
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_nBufferPos >= m_nBufferSize - 1)
   {
      if (m_nBufferSize == 0)
         m_nBufferSize = 128;

      _TCHAR *tmp = new _TCHAR[m_nBufferSize * 2 + 1];
      ZeroMemory(tmp, (m_nBufferSize * 2 + 1) * sizeof _TCHAR);

      if (m_pBuffer)
      {
         _tcsncpy(tmp, m_pBuffer, m_nBufferSize);
         delete[] m_pBuffer;
      }

      m_pBuffer = tmp;
      m_nBufferSize *= 2;
   }

   m_pBuffer[m_nBufferPos++] = c;
}

void CStringBuffer::Add(LPCTSTR lpString)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int len = _tcslen(lpString);
   for (int i=0; i<len; ++i)
      Add(lpString[i]);
}

LPCTSTR CStringBuffer::GetStringBuffer()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_pBuffer[m_nBufferPos] = '\000';
   return (LPCTSTR)m_pBuffer;
}

/*********************************************
 *********************************************/

int Calculator::GetPixelFromMsec(int msec, int widthInPixel, int widthInMsec, int msecOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int x = (int)(((double)(msec - msecOffset) / widthInMsec) * widthInPixel);

   return x;
}

int Calculator::GetMsecFromPixel(int pixel, int widthInPixel, int widthInMsec)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int msec = (int)(((double)pixel / widthInPixel) * widthInMsec);

   return msec;
}

void Calculator::FitRectInRect(int containerWidth, int containerHeight,
                               int contentWidth, int contentHeight,
                               int *newWidth, int *newHeight,
                               bool stretch)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (newWidth == NULL ||
       newHeight == NULL)
      return;

   if (containerWidth >= contentWidth &&
       containerHeight >= contentHeight &&
       !stretch)
   {
      *newWidth = contentWidth;
      *newHeight = contentHeight;
      return;
   }

   double xf = ((double) containerWidth) / ((double) contentWidth);
   double yf = ((double) containerHeight) / ((double) contentHeight);

   double f = xf < yf ? xf : yf;

   *newWidth = (int) (f * ((double) contentWidth) + .5);
   if (*newWidth > containerWidth)
      *newWidth = containerWidth;
   *newHeight = (int) (f * ((double) contentHeight) + .5);
   if (*newHeight > containerHeight)
      *newHeight = containerHeight;
}

/*********************************************
 *********************************************/

/* What kind of a comparisson should this be? bytes? characters?
bool FileInformation::FileBeginsWith(LPCTSTR filename, LPCTSTR compareString, DWORD count)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   
   HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

   if (file ==  INVALID_HANDLE_VALUE)
      return false;

   char *charBuf; 
   charBuf = (char *)malloc(count+1);
   DWORD read;
   if (ReadFile(file, charBuf, count, &read, NULL))
   {
      if (read == count)
      {
         if (strncmp(charBuf, compareString, count) == 0)
         {
            delete charBuf;
            return true;
         }
      }
   }

   delete charBuf;
   return false;
}
*/

bool FileInformation::DirectoryContainsLrd(LPCTSTR directory, LPCTSTR excludingLrd)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HANDLE
      hFile;
   WIN32_FIND_DATA 
      ffd;
   CString 
      dirName;
  

   dirName = directory;
   if (dirName.IsEmpty())
      return false;

   dirName += "\\*.*";

   hFile = FindFirstFile((LPCTSTR)dirName, &ffd);

   if (hFile != INVALID_HANDLE_VALUE)
   {             
      while (FindNextFile(hFile, &ffd))
      {
         if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
         {
            CString fileSuffix = ffd.cFileName;
            StringManipulation::GetFileSuffix(fileSuffix);
            fileSuffix.MakeUpper();
            if (fileSuffix == "LRD")
            {
               CString fileName = excludingLrd;
               StringManipulation::GetFilename(fileName);
               if (fileName != ffd.cFileName)
               {
                  FindClose(hFile);
                  return true;
               }
            }
         }
      }
   } 
           
   FindClose(hFile);

   return false;
}

/*********************************************
 *********************************************/

// PZI: additional argument "streamTimeMsec" to get poster frame at the specified time (default is 0)
bool VideoInformation::GetPosterFrame(LPCTSTR szFileName, int &nWidth, int &nHeight, HBITMAP *phBitmap, int streamTimeMsec)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (phBitmap == NULL)
      return false;

   HRESULT hr = ::CoInitialize(NULL);
   if (FAILED(hr))
      return false;

   CComPtr <IMediaDet> pMediaDet = NULL;
   hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_ALL, IID_IMediaDet, (void **) &pMediaDet);

   if (SUCCEEDED(hr) && pMediaDet)
      hr = pMediaDet->put_Filename(_bstr_t(szFileName));
   else
      hr = E_FAIL;

   long streamCount = 0;
   int videoWidth = 0;
   int videoHeight = 0;
   if (SUCCEEDED(hr))
      hr = pMediaDet->get_OutputStreams(&streamCount);
   if (SUCCEEDED(hr))
   {
      CMediaType mediaType;
      bool videoFound = false;
      for (int i=0; i<streamCount && !videoFound && SUCCEEDED(hr); ++i)
      {
         mediaType.InitMediaType();
         hr = pMediaDet->put_CurrentStream(i);
         if (SUCCEEDED(hr))
            hr = pMediaDet->get_StreamMediaType(&mediaType);
         if (SUCCEEDED(hr))
         {
            if ((*(mediaType.Type()) == MEDIATYPE_Video) &&
                (*(mediaType.FormatType()) == FORMAT_VideoInfo))
            {
               VIDEOINFOHEADER *pHeader = (VIDEOINFOHEADER *) mediaType.Format();
               videoWidth = pHeader->bmiHeader.biWidth;
               videoHeight = pHeader->bmiHeader.biHeight;
               videoFound = true;
            } else if ((*(mediaType.Type()) == MEDIATYPE_Video) &&
                (*(mediaType.FormatType()) == FORMAT_VideoInfo2)) {
               VIDEOINFOHEADER2 *pHeader = (VIDEOINFOHEADER2 *) mediaType.Format();
               videoWidth = pHeader->bmiHeader.biWidth;
               videoHeight = pHeader->bmiHeader.biHeight;
               videoFound = true;
            }
         }
      }
   }

   // PZI: no scaling if width and height is not specified
   if((nWidth == 0) && (nHeight == 0)) {
      nWidth = videoWidth;
      nHeight = videoHeight;
   }

   long bufferSize = 0;
   // nWidth and nHeight give the maximum size
   int width = nWidth;
   int height = nHeight;

   // How big a frame should we grab?
   Calculator::FitRectInRect(nWidth, nHeight, videoWidth, videoHeight, &width, &height);

   // PZI: add 1 msec to fix bug in IMediaDet::GetBitmapBits()
   // otherwise GetBitmapBits() may deliver the frame before the frame that corespond to the given streamtime
   // note: error occurs at framerate of 7 fps but not at 5 fps
   // TODO: check other framerates and confirm bugfix
   // note: do not fix 0 msec to ensure backward compatibility with default setting of 0 msec 
   if(streamTimeMsec > 0)
      streamTimeMsec += 1;

   // translate msec to seconds
   double streamTime = ((double)streamTimeMsec)/1000.0;
   char *pBuffer = NULL;
   if (SUCCEEDED(hr))
      hr = pMediaDet->GetBitmapBits(streamTime, &bufferSize, NULL, width, height);
   if (SUCCEEDED(hr))
   {
      pBuffer = new char[bufferSize];
      // get bitmap at specified time
      hr = pMediaDet->GetBitmapBits(streamTime, &bufferSize, pBuffer, width, height);
   }

   if (SUCCEEDED(hr))
   {
      BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER*)pBuffer;
      HDC hdcDest = GetDC(0);

      // Find the address of the start of the image data.
      void *pData = pBuffer + sizeof(BITMAPINFOHEADER);

      // Note: In general a BITMAPINFOHEADER can include extra color
      // information at the end, so calculating the offset to the image
      // data is not generally correct. However, the IMediaDet interface
      // always returns an RGB-24 image with no extra color information.

      BITMAPINFO bmi;
      ZeroMemory(&bmi, sizeof(BITMAPINFO));
      CopyMemory(&(bmi.bmiHeader), bmih, sizeof(BITMAPINFOHEADER));
      *phBitmap = CreateDIBitmap(hdcDest, bmih, CBM_INIT, 
         pData, &bmi, DIB_RGB_COLORS);

      ::DeleteDC(hdcDest);
   }
   if (pBuffer)
      delete[] pBuffer;
   pBuffer = NULL;

   ::CoUninitialize();

   if (SUCCEEDED(hr))
   {
      nWidth = width;
      nHeight = height;
   }

   return SUCCEEDED(hr) ? true : false;
}

// PZI: write poster frame at specified time and scaled to specified dimensions as image file according to file extention
bool VideoInformation::WritePosterFrame(LPCTSTR tszImageFileName, LPCTSTR tszVideoFileName, int streamTimeMsec, int width, int height)
{
   // Configure LSGC to write frames without mouse pointer
   SetEnvironmentVariable(_T("LSGC_NO_POINTER"),_T("TRUE"));

   bool result = false;
   HBITMAP bitmap;
   if(GetPosterFrame(tszVideoFileName, width, height, &bitmap, streamTimeMsec)){
      // get device dependent bitmap (DDB)
      CImage imageDDB;
      imageDDB.Attach(bitmap);
      imageDDB.Save(tszImageFileName);

      result = true;
   }
   // unset LSGC configuration
   SetEnvironmentVariable(_T("LSGC_NO_POINTER"), NULL);
   return result;
}

bool VideoInformation::GetVideoDuration(LPCTSTR szFileName, int &durationMs)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = ::CoInitialize(NULL);
   if (FAILED(hr))
      return false;

   CComPtr<IMediaDet> pMediaDet = NULL;
   hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_ALL, IID_IMediaDet, (void **) &pMediaDet);
   if (SUCCEEDED(hr))
      hr = pMediaDet->put_Filename(_bstr_t(szFileName));

   double streamLength = 0.0;
   if (SUCCEEDED(hr))
      hr = pMediaDet->get_StreamLength(&streamLength);

   if (SUCCEEDED(hr))
      durationMs = (int) (1000.0 * streamLength);

   ::CoUninitialize();

   return SUCCEEDED(hr) ? true : false;
}
   
bool VideoInformation::GetVideoFramerate(LPCTSTR tszFileName, double &framerate)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = ::CoInitialize(NULL);
   if (FAILED(hr))
      return false;

   CComPtr<IMediaDet> pMediaDet = NULL;
   hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_ALL, IID_IMediaDet, (void **) &pMediaDet);
   if (SUCCEEDED(hr))
      hr = pMediaDet->put_Filename(_bstr_t(tszFileName));

   if (SUCCEEDED(hr))
      hr = pMediaDet->get_FrameRate(&framerate);


   ::CoUninitialize();

   return SUCCEEDED(hr) ? true : false;
}
   
bool VideoInformation::GetVideoSize(LPCTSTR tszFileName, SIZE &videoSize)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = ::CoInitialize(NULL);
   if (FAILED(hr))
      return false;

   CComPtr <IMediaDet> pMediaDet = NULL;
   hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_ALL, IID_IMediaDet, (void **) &pMediaDet);

   if (SUCCEEDED(hr) && pMediaDet)
      hr = pMediaDet->put_Filename(_bstr_t(tszFileName));
   else
      hr = E_FAIL;

   long streamCount = 0;
   int videoWidth = 0;
   int videoHeight = 0;
   if (SUCCEEDED(hr))
      hr = pMediaDet->get_OutputStreams(&streamCount);
   if (SUCCEEDED(hr))
   {
      CMediaType mediaType;
      bool videoFound = false;
      for (int i=0; i<streamCount && !videoFound && SUCCEEDED(hr); ++i)
      {
         mediaType.InitMediaType();
         hr = pMediaDet->put_CurrentStream(i);
         if (SUCCEEDED(hr))
            hr = pMediaDet->get_StreamMediaType(&mediaType);
         if (SUCCEEDED(hr))
         {
            if ((*(mediaType.Type()) == MEDIATYPE_Video) &&
                (*(mediaType.FormatType()) == FORMAT_VideoInfo))
            {
               VIDEOINFOHEADER *pHeader = (VIDEOINFOHEADER *) mediaType.Format();
               videoWidth = pHeader->bmiHeader.biWidth;
               videoHeight = pHeader->bmiHeader.biHeight;
               videoFound = true;
            }
         }
      }
   }

   videoSize.cx = videoWidth;
   videoSize.cy = videoHeight;

   ::CoUninitialize();

   return SUCCEEDED(hr) ? true : false;
}
   
bool VideoInformation::GetVideoInformation(LPCTSTR tszFileName, VIDEOINFOHEADER *pVideoInfoHeader, 
                                           double &durationSec, double &framerate)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = ::CoInitialize(NULL);
   if (FAILED(hr))
      return false;

   VIDEOINFOHEADER *pHeader = NULL;

   CComPtr<IMediaDet> pMediaDet = NULL;
   hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_ALL, IID_IMediaDet, (void **) &pMediaDet);
   if (SUCCEEDED(hr))
      hr = pMediaDet->put_Filename(_bstr_t(tszFileName));
   else
      hr = E_FAIL;

   if (SUCCEEDED(hr))
      hr = pMediaDet->get_StreamLength(&durationSec);

   if (SUCCEEDED(hr))
      hr = pMediaDet->get_FrameRate(&framerate);

   long streamCount = 0;
   if (SUCCEEDED(hr))
      hr = pMediaDet->get_OutputStreams(&streamCount);
   if (SUCCEEDED(hr))
   {
      CMediaType mediaType;
      bool videoFound = false;
      for (int i=0; i<streamCount && !videoFound && SUCCEEDED(hr); ++i)
      {
         mediaType.InitMediaType();
         hr = pMediaDet->put_CurrentStream(i);
         if (SUCCEEDED(hr))
            hr = pMediaDet->get_StreamMediaType(&mediaType);
         if (SUCCEEDED(hr))
         {
            if ((*(mediaType.Type()) == MEDIATYPE_Video) &&
                (*(mediaType.FormatType()) == FORMAT_VideoInfo))
            {
               pHeader = (VIDEOINFOHEADER *) mediaType.Format();
               videoFound = true;
            }
         }
      }
   }

   if (SUCCEEDED(hr) && pHeader!=NULL)
   {
      // Copy the VIDEOINFOHEADER structure 
      *pVideoInfoHeader = *pHeader;
   }

   ::CoUninitialize();

   return (SUCCEEDED(hr) && pHeader!=NULL) ? true : false;
}

int VideoInformation::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

/*********************************************
 *********************************************/

bool FileDialog::GetOpenFilename(CWinApp *pApp, CString &csOpenFilename, HWND cWnd, 
                                 HINSTANCE instance, LPCTSTR filter, LPCTSTR tszId, LPCTSTR tszInitialDir)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   _TCHAR tszFilter[256];
   _tcsncpy(tszFilter, filter, 256);
   for (int i=0; i<256; ++i)
      if (tszFilter[i] == _T('|')) tszFilter[i] = _T('\000');

   _TCHAR tszFileName[2*MAX_PATH];
   tszFileName[0] = '\000';

   CString csLastDir = "";
   _TCHAR tszLastDir[2*MAX_PATH];
   
   bool bHasLastDir = false;
   if (tszId)
   {
      csLastDir = pApp->GetProfileString(_T("Dirs"), tszId, tszInitialDir);
      _tcsncpy(tszLastDir, csLastDir, MAX_PATH);
      bHasLastDir = true;
   }
   else if (tszInitialDir)
   {
      _tcsncpy(tszLastDir, tszInitialDir, MAX_PATH);
      bHasLastDir = true;
   }


   OPENFILENAME ofn;
   memset(&ofn, 0, sizeof(OPENFILENAME));
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = cWnd;
   ofn.hInstance = instance;
   ofn.lpstrFilter = tszFilter;
   ofn.lpstrCustomFilter = NULL;
   ofn.nMaxCustFilter = 0;
   ofn.nFilterIndex = 0;
   ofn.lpstrFile = tszFileName;
   ofn.nMaxFile = MAX_PATH - 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = bHasLastDir ? tszLastDir : NULL;
   ofn.lpstrTitle = NULL;
   ofn.Flags = OFN_HIDEREADONLY|OFN_ENABLESIZING;

   BOOL res = ::GetOpenFileName(&ofn);

   

   if (res != 0)
   {
      csOpenFilename = tszFileName;
      CFile file;
      if (file.Open(csOpenFilename, CFile::modeRead)) {
          file.Close();
      } else {
          CString csMsg;
          CString csMessage;
          csMessage.LoadString(IDS_MSG_FILE_NOTFOUND);
          csMsg.Format(csMessage, csOpenFilename);
          csOpenFilename.Empty();
          CString csCap;
          csCap.LoadString(IDS_WARNING);
          MessageBox(NULL, csMsg, csCap, MB_OK | MB_ICONWARNING);
          return true;
      }
      if (tszId)
      {
         CString csTempName = tszFileName;
         StringManipulation::GetPath(csTempName);
         pApp->WriteProfileString(_T("Dirs"), tszId, csTempName);
      }
   }
   else
      csOpenFilename.Empty();

   return true;
}

bool FileDialog::GetSaveFilename(CWinApp *pApp, CString &csSaveFilename, HWND cWnd, HINSTANCE instance, 
                                 LPCTSTR filter, LPCTSTR tszFileExtension, LPCTSTR tszId, LPCTSTR tszInitialDir, LPCTSTR tszInitialFileName,
                                 bool bOverwritePrompt /*=true*/)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   _TCHAR tszFilter[256];
   _tcsncpy(tszFilter, filter, 256);
   for (int i=0; i<256; ++i)
      if (tszFilter[i] == _T('|')) tszFilter[i] = _T('\000');

   _TCHAR tszFileName[512];
   tszFileName[0] = '\000';
   if (tszInitialFileName != NULL)
      _tcscpy(tszFileName, tszInitialFileName);

   CString csLastDir = "";
   _TCHAR tszLastDir[MAX_PATH];
   bool bHasLastDir = false;
   if (tszId)
   {
      csLastDir = pApp->GetProfileString(_T("Dirs"), tszId, tszInitialDir);
      _tcsncpy(tszLastDir, csLastDir, MAX_PATH);
      bHasLastDir = true;
   }
   else if (tszInitialDir)
   {
      _tcsncpy(tszLastDir, tszInitialDir, MAX_PATH);
      bHasLastDir = true;
   }

   OPENFILENAME ofn;
   memset(&ofn, 0, sizeof(OPENFILENAME));
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = cWnd;
   ofn.hInstance = instance;
   ofn.lpstrFilter = tszFilter;
   ofn.lpstrCustomFilter = NULL;
   ofn.nMaxCustFilter = 0;
   ofn.nFilterIndex = 0;
   ofn.lpstrFile = tszFileName;
   ofn.nMaxFile = MAX_PATH - 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = bHasLastDir ? tszLastDir : NULL;
   ofn.lpstrTitle = NULL;
   ofn.lpstrDefExt = tszFileExtension;
   ofn.Flags = OFN_HIDEREADONLY|OFN_ENABLESIZING;
   if (bOverwritePrompt)
      ofn.Flags |= OFN_OVERWRITEPROMPT;

   BOOL res = ::GetSaveFileName(&ofn);
   
   if (res != 0)
   {
      // As of 2.0.p2 Unicode is supported and this not necessary anymore
      //StringManipulation::VerifyInternationalFilename(cWnd, tszFileName, true);

      csSaveFilename = tszFileName;
      if (tszId)
      {
         CString csTempName = tszFileName;
         StringManipulation::GetPath(csTempName);
         pApp->WriteProfileString(_T("Dirs"), tszId, csTempName);
      }
   }
   else
      csSaveFilename.Empty();

   return true;
}

/* static */ bool Explore::ExploreLecturnityHome()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   _TCHAR tszLecturnityHome[MAX_PATH];
   bool success = LMisc::GetLecturnityHome(tszLecturnityHome);

   if (success)
   {
      CString csLecturnityHome = tszLecturnityHome;
      if (csLecturnityHome[csLecturnityHome.GetLength() - 1] == _T('\\'))
         csLecturnityHome = csLecturnityHome.Left(csLecturnityHome.GetLength() - 1);

      success = (32 >= (DWORD) ::ShellExecute(NULL, _T("explore"), 
         csLecturnityHome, NULL, NULL, SW_SHOWNORMAL));
   }

   return success;
}

/* static */ /*bool SystemInfo::Check16BitOrHigher()
{
   DEVMODE devMode;
   ZeroMemory(&devMode, sizeof DEVMODE);
   devMode.dmSize = sizeof DEVMODE;
   if (0 != ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode))
   {
      if (devMode.dmBitsPerPel < 16)
      {
         CString csMsg;
         csMsg.LoadString(IDS_WARN_LESS_16BIT);
         CString csCap;
         csCap.LoadString(IDS_ERROR);
         if (IDYES == ::MessageBox(::GetForegroundWindow(), csMsg, csCap, MB_YESNO | MB_ICONERROR))
         {
            ::ShellExecute(NULL, _T("open"), _T("rundll32.exe"), 
               _T("shell32.dll,Control_RunDLL desk.cpl,,3"), NULL, SW_SHOW);
         }

         return false;
      }
   }

   return true;
}
*/

/* static */ void SystemInfo::CalculateDiskSpace(LPCTSTR pszPath, int &mbLeft) {
    ULARGE_INTEGER
        freeBytesAvailableToCaller,
        totalNumberOfBytes,
        totalNumberOfFreeBytes;

    if (!GetDiskFreeSpaceEx(pszPath, &freeBytesAvailableToCaller, &totalNumberOfBytes,  &totalNumberOfFreeBytes)) {
        mbLeft = 0;
        return;
    }
 
    mbLeft = freeBytesAvailableToCaller.QuadPart/(1024*1024);

    return;
}
/* static */Gdiplus::Image * ImageProcessor::LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance) {
     AFX_MANAGE_STATE(AfxGetStaticModuleState());
   Gdiplus::Image * pImage = NULL;

   if( lpszType == RT_BITMAP )
   {
      HBITMAP hBitmap = ::LoadBitmap( hInstance, MAKEINTRESOURCE(nID) );
      pImage = (Gdiplus::Image*)Gdiplus::Bitmap::FromHBITMAP(hBitmap, 0);
      ::DeleteObject(hBitmap);
      return pImage;
   }		

   hInstance = (hInstance == NULL) ? ::AfxGetResourceHandle() : hInstance;
   HRSRC hRsrc = ::FindResource ( hInstance, MAKEINTRESOURCE(nID), lpszType); 
   ASSERT(hRsrc != NULL);

   DWORD dwSize = ::SizeofResource( hInstance, hRsrc);
   LPBYTE lpRsrc = (LPBYTE)::LoadResource( hInstance, hRsrc);
   ASSERT(lpRsrc != NULL);

   HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
   if ( hMem )
   {
      LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
      if ( pMem )
      {
         memcpy( pMem, lpRsrc, dwSize);
         IStream * pStream = NULL;
         ::CreateStreamOnHGlobal( hMem, FALSE, &pStream);

         if ( pStream != NULL )
         {
            pImage = Gdiplus::Image::FromStream(pStream);
            pStream->Release();
         }
         ::GlobalUnlock(hMem);
      }
      ::GlobalFree(hMem);
   }
   ::FreeResource(lpRsrc);

   return pImage;
}
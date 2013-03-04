/*********************************************************************

 program : mlb_misc.cpp
 authors : Gabriela Maass
 date    : 29.11.2000
 desc    : Miscelaneous functions 

 **********************************************************************/

#include "StdAfx.h"

#include <io.h>
#include <stdio.h>

#include <direct.h>


#ifndef _STUDIO
#include "..\Assistant.h"
#endif
#include "mlb_misc.h"
#include "la_project.h" 
#include "lutils.h"
#include "la_registry.h"
#include "MiscFunctions.h"

#include "la_presentationinfo.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

void ASSISTANT::RemoveClosingSlash(CString &csPath)
{
   int iLastChar = csPath.GetLength() - 1;
   if (csPath[iLastChar] == _T('\\'))
      csPath.Delete(iLastChar, 1);
}

void ASSISTANT::GetPath(CString &csPath)
{
   int iSlashPos = csPath.ReverseFind(_T('\\'));
   if (iSlashPos != -1)
      csPath = csPath.Left(iSlashPos);
   else
      csPath.Empty();
  
}

void ASSISTANT::GetName(CString &csName)
{
   int iSlashPos = csName.ReverseFind(_T('\\'));
   if (iSlashPos != -1)
      csName = csName.Right(csName.GetLength() - (iSlashPos+1));
  
}

void ASSISTANT::GetDrive(CString &csDrive)
{
   int iSlashPos = csDrive.ReverseFind(_T(':'));
   if (iSlashPos != -1)
      csDrive = csDrive.Left(iSlashPos);
}

void ASSISTANT::GetPrefix(CString &csPrefix)
{
   int iSlashPos = csPrefix.ReverseFind(_T('.'));
   if (iSlashPos != -1)
      csPrefix = csPrefix.Left(iSlashPos);

   return;
}

void ASSISTANT::GetSuffix(CString &csPrefix)
{
   int iSlashPos = csPrefix.ReverseFind(_T('.'));
   if (iSlashPos != -1)
      csPrefix.Delete(0, iSlashPos);
   
   return;
}

void ASSISTANT::GetUserHome(CString &ssUserHome)
{
   WindowsEntry *regEntry = new WindowsEntry();
   bool result = regEntry->GetShellFolders(_T("Personal"), ssUserHome);
   delete regEntry;
   if (!result)
   {
      
      LPCTSTR 
         variableName = _T("USERPROFILE");
      LPTSTR
         variableValue;
      int 
         nSize;
      nSize = GetEnvironmentVariable(variableName, NULL, 0);
      variableValue = (LPTSTR)malloc((nSize+1)*sizeof(TCHAR));
      
      int result = GetEnvironmentVariable(variableName, variableValue, nSize);
      if (result == 0) 
      {
         free(variableValue);
         nSize = GetCurrentDirectory(0, NULL);
         variableValue = (LPTSTR)malloc((nSize+1)*sizeof(TCHAR));
         result = GetCurrentDirectory(nSize, variableValue);
         if (result == 0)
         {
            free(variableValue);
            variableValue = (LPTSTR)malloc(10*sizeof(TCHAR));
            _tcscpy(variableValue, _T("."));
         }
         
         ssUserHome = variableValue;
      }
      else
      {
         CString ownFile = variableValue;
         
         LANGID languageId = GetSystemDefaultLangID();
         if ((languageId == 0x0407) || (languageId == 0x0807) || (languageId == 0x0c07) || 
            (languageId == 0x1007) || (languageId == 0x1407))
         {
            ownFile += _T("\\Eigene Dateien");
         }
         else
         {
            ownFile += _T("\\My Documents");
         }
         if (_taccess(ownFile, 00) != -1)
         {
            ssUserHome = ownFile;
         }
         else
         {
            ssUserHome = variableValue;
         }
      }
      free(variableValue);
   }
}

bool ASSISTANT::GetLecturnityHome(CString &lecturnityHome)
{
   lecturnityHome.Empty();

   _TCHAR tszLecturnityHome[MAX_PATH];
   bool bResult = false;
   bResult = LMisc::GetLecturnityHome(tszLecturnityHome);
   if (bResult)
      lecturnityHome = tszLecturnityHome;
   else
      lecturnityHome.Empty();

   return bResult;
}

bool ASSISTANT::LecturnityHomeEntryExist()
{

   DWORD dwSize = 0;
   bool bResult = LRegistry::ReadSettingsString(_T(""), _T("LecturnityHome"), NULL, &dwSize, false);

   return bResult;
}


void ASSISTANT::SetLecturnityHome(CString &lecturnityHome)
{
   ASSISTANT::RemoveClosingSlash(lecturnityHome);
   
   LRegistry::WriteSettingsString(_T(""), _T("LecturnityHome"), (LPCTSTR)lecturnityHome, false);
}


void ASSISTANT::GetAppData(CString &appData)
{
   TCHAR tszApplicationDataDir[MAX_PATH];
   bool bResult = LMisc::GetApplicationDataDir(tszApplicationDataDir);

   if (bResult)
      appData = tszApplicationDataDir;
   else
      appData.Empty();

}

void ASSISTANT::GetSourceDirectory(CString &sourceDirectory)
{
   CString lecturnityHome;
   bool bLecturnityHome = ASSISTANT::GetLecturnityHome(lecturnityHome);

   if (!bLecturnityHome || _taccess(lecturnityHome, 00) == -1)
   {
      sourceDirectory = _T("");
      return;
   }

   lecturnityHome += _T("\\Source Documents");
   sourceDirectory = lecturnityHome;
}

void ASSISTANT::GetRecordingDirectory(CString &recordingDirectory)
{
   CString lecturnityHome;
   bool bLecturnityHome = ASSISTANT::GetLecturnityHome(lecturnityHome);

   // This case should no longer occur
   if (!bLecturnityHome || (_taccess(lecturnityHome, 00) == -1))  // lecturnity home doesn`t exists
   {	  
      GetUserHome(lecturnityHome);
   }

   recordingDirectory = lecturnityHome;
   recordingDirectory += _T("\\Recordings");
   
   if (_taccess(recordingDirectory,00) == -1)
   {
      CreateDirectory(recordingDirectory, NULL);
   }
}

int ASSISTANT::SwapArrowConfig(int arrowConfig)
{
   if (arrowConfig == 0) 
      return 0;

   int tmpArrowConfig = ((arrowConfig / 1000))        * 10   +    // begin arrow style -> end arrow style
                        ((arrowConfig % 1000) / 100)  * 1    +    // begin arrow dimension -> end arrow dimension
                        ((arrowConfig % 100) / 10)    * 1000 +    // end arrow style -> begin arrow style
                        ((arrowConfig % 10))          * 100;      // end arrow dimension -> begin arrow dimension    
  
   return tmpArrowConfig;
}

int ASSISTANT::GetBeginArrowConfig(int arrowConfig)
{
   if (arrowConfig == 0) 
      return 0;

   int tmpArrowConfig = ((arrowConfig / 1000))        * 1000   +    // begin arrow style 
                        ((arrowConfig % 1000) / 100)  * 100;        // begin arrow dimension 
  
   return tmpArrowConfig;
}

int ASSISTANT::GetEndArrowConfig(int arrowConfig)
{
   if (arrowConfig == 0) 
      return 0;

   int tmpArrowConfig = ((arrowConfig % 100) / 10)    * 10 +    // end arrow style
                        ((arrowConfig % 10))          * 1;      // end arrow dimension    
  
   return tmpArrowConfig;
}

int ASSISTANT::GetIntArrowStyle(LPCTSTR arrowStyle)
{
   int iArrowStyle = 0;
   if (_tcscmp(arrowStyle, _T("first")) == 0)
      iArrowStyle = 1;
   else if (_tcscmp(arrowStyle, _T("last")) == 0)
      iArrowStyle = 2;
   else if (_tcscmp(arrowStyle, _T("both")) == 0)
      iArrowStyle = 3;

   return iArrowStyle;
}

void ASSISTANT::GetArrowConfig(int iArrowStyle, int &iArrowConfig, CString &csArrowStyle)
{
   switch (iArrowStyle)
   {
   case 0:
      iArrowConfig = 0;
      csArrowStyle = _T("none");
      break;
   case 1:
      iArrowConfig = 29;
      csArrowStyle = _T("last");
      break;
   case 2:
      iArrowConfig = 2900;
      csArrowStyle = _T("first");
      break;
   case 3:
      iArrowConfig = 2929;
      csArrowStyle = _T("both");
      break;
   case 4:
      iArrowConfig = 19;
      csArrowStyle = _T("last");
      break;
   case 5:
      iArrowConfig = 1900;
      csArrowStyle = _T("first");
      break;
   case 6:
      iArrowConfig = 1919;
      csArrowStyle = _T("both");
      break;
   case 7:
      iArrowConfig = 4919;
      csArrowStyle = _T("both");
      break;
   case 8:
      iArrowConfig = 5919;
      csArrowStyle = _T("both");
      break;
   case 9:
      iArrowConfig = 4949;
      csArrowStyle = _T("both");
      break;
   case 10:
      iArrowConfig = 5959;
      csArrowStyle = _T("both");
      break;
   }
}

int ASSISTANT::CalculateZoomedLineWidth(int lineWidth, float zoomFactor)
{
   int 
      zoomedLineWidth;
   float
      diff;

   zoomedLineWidth = lineWidth * zoomFactor;

   diff = (lineWidth * zoomFactor) - zoomedLineWidth;
   if ((diff > 0.4) || (zoomedLineWidth == 0)) 
      ++zoomedLineWidth;

   return zoomedLineWidth;
}

void ASSISTANT::CalculateNextDataPath(CString &csDateTime, const _TCHAR *recordPath, const _TCHAR *prefix, CString &dataPath)
{
   CString
      zwDir;
   bool
      recordDirCreated;
   char
      buf[10];
   int
      i;

   dataPath.Empty();

   zwDir = recordPath;
   zwDir += _T("\\");
   zwDir += prefix;
   zwDir += _T("_");
   zwDir += csDateTime;

   i = 0;
   dataPath = zwDir;

   recordDirCreated = true;
   while (recordDirCreated && (_tmkdir(dataPath) == -1)) 
   {
      if (errno == EEXIST) 
      {
         i++;
         dataPath = zwDir;
         dataPath += _T("_");
         dataPath += _itoa(i,buf,10);
      }
      else 
      {
         recordDirCreated = false;
         dataPath.Empty();
      }
   }
}

void ASSISTANT::CalculateNextPrefix(CString &csPrefix, CStringArray &arExistingPrefix) {   
    int iActualNumber = 1;
    CString csNewPrefix;
    bool bSearchFinished = false;
    while (!bSearchFinished && iActualNumber < 9999) {
        csNewPrefix.Format(_T("%s %d"), csPrefix, iActualNumber);

        bool bDocumentAlreadyExists = false;
        for (int i = 0; i < arExistingPrefix.GetSize(); ++i) {
            if (arExistingPrefix[i] == csNewPrefix) {
                bDocumentAlreadyExists = true;
                break;
            }
        }

        if (!bDocumentAlreadyExists)
            bSearchFinished = true;

        if (!bSearchFinished)
            ++iActualNumber;
    }

    csPrefix = csNewPrefix;
}

bool ASSISTANT::FileExist(const _TCHAR *filename)
{
   if (_taccess(filename, 00) == 0) 
      return true;
   else
      return false;
}

bool ASSISTANT::DirectoryExist(const _TCHAR *szDirectoryName)
{
   if (_taccess(szDirectoryName, 00) == 0) 
      return true;
   else
      return false;
}

bool ASSISTANT::TryNextDataPath(const _TCHAR *recordPath, const _TCHAR *prefix)
{
   CString
      zwDir,
      dataPath;
   char
      buf[10];
   int
      i;

   dataPath.Empty();

   zwDir = recordPath;
   zwDir += _T("\\");
   zwDir += prefix;
   zwDir += _T("_");

   i = 0;
   dataPath = zwDir;
   dataPath += _itoa(i,buf,10);
   while (_tmkdir(dataPath) == -1)
   {
      if (errno == EEXIST) 
      {
         i++;
         dataPath = zwDir;
         dataPath += _itoa(i,buf,10);
      }
      else 
      {
         return false;
      }
   }

   _trmdir(dataPath);
   return true;

}

bool ASSISTANT::DeleteDirectory(const _TCHAR *directory)
{
   HANDLE
      hFile;
   WIN32_FIND_DATA 
      ffd;
   CString
      dirName;
  
   if (directory == NULL)
      return false;

   dirName = directory;
   dirName += _T("\\*.*");

   hFile = FindFirstFile((LPCTSTR)dirName, &ffd);

   if (hFile != INVALID_HANDLE_VALUE)
   {             
      while (FindNextFile(hFile, &ffd))
      {
         if (ffd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
         {
            /*
            if ((strcmp(ffd.cFileName, _T("..")) == 0))
               continue;
            dirName = directory;
            dirName += _T("\\");
            dirName += ffd.cFileName;
            ASSISTANT::DeleteDirectory(dirName);
            */
         }
         else 
         {
            dirName = directory;
            dirName += _T("\\");
            dirName += ffd.cFileName;
            DeleteFile(dirName);
         }
      }
   } 

            
   FindClose(hFile);
   RemoveDirectory(directory);

   return true;
}

void ASSISTANT::GetTimeString(CString &timeString)
{
   _TCHAR
	   timebuf[128];
   time_t
      ldate;
   struct tm 
      *today;

  time(&ldate);    
  today = localtime( &ldate );

  _stprintf(timebuf, _T("%d:%02d"), today->tm_hour, today->tm_min);

  timeString = timebuf;

}

void ASSISTANT::TranslateMsecToTimeString(int msec, CString &timeString)
{
   _TCHAR
      timebuf[128];

   int totSec = msec / 1000;
   int min  = totSec / 60;
   int sec  = totSec % 60;

   _stprintf(timebuf, _T("%02d:%02d"), min, sec);  
 
   timeString = timebuf;
}

void ASSISTANT::GetDateString(CString &dateString, int language)
{
   CTime time = CTime::GetCurrentTime();

   dateString.Format(_T("%02d.%02d.%d"), time.GetDay(), time.GetMonth(), time.GetYear());
}

bool ASSISTANT::GetDateFromString(SYSTEMTIME &sTime, CString &csDate)
{
   int curPos = 0;
   if ( csDate.GetLength() < 8 )
      return false;

   if ( csDate.Find(_T(".")) != -1)
   {//Current format
      CString resToken = csDate.Tokenize(_T("."), curPos);
      if ( resToken.GetLength() < 1 ) return false;
      sTime.wDay = _ttoi(resToken);
      resToken = csDate.Tokenize(_T("."), curPos);
      if ( resToken.GetLength() < 1 ) return false;
      sTime.wMonth  = _ttoi(resToken);
      resToken = csDate.Tokenize(_T("."), curPos);
      if ( resToken.GetLength() < 1 ) return false;
      sTime.wYear = _ttoi(resToken);
   }
   else if ( csDate.Find(_T("/")) != -1 )
   {// old format (american)
      CString resToken = csDate.Tokenize(_T("/"), curPos);
      if ( resToken.GetLength() < 1 ) return false;
      sTime.wMonth  = _ttoi(resToken);
      resToken = csDate.Tokenize(_T("/"), curPos);
      if ( resToken.GetLength() < 1 ) return false;
      sTime.wDay = _ttoi(resToken);
      resToken = csDate.Tokenize(_T("/"), curPos);
      if ( resToken.GetLength() < 1 ) return false;
      sTime.wYear = _ttoi(resToken);
   } 
   else
   { // At least unknown format or bad input.
      return false;
   }
   // _ttoi function failed for some function from above.
   if ( sTime.wYear == 0 || sTime.wMonth == 0 || sTime.wDay == 0 )
      return false;
   return true;
}

bool ASSISTANT::GetTimeFromString(SYSTEMTIME &sTime, CString &csTime)
{
   int curPos = 0;
   if ( csTime.GetLength() < 3 || csTime.Find(_T(":")) <= 0 )
      return false;

   CString resToken = csTime.Tokenize(_T(":"), curPos);
   if ( resToken.GetLength() < 1 ) return false;
   sTime.wHour = _ttoi(resToken);
   resToken = csTime.Tokenize(_T(":"), curPos);
   if ( resToken.GetLength() < 1 ) return false;
   sTime.wMinute = _ttoi(resToken);

   return true;
}

// Triple Code:
// See mlb_page.cpp, la_thumbs.cpp
void ASSISTANT::CreateListFromString(CString &srcString, CStringArray &destList)
{
   CString 
      listElement;

   listElement.Empty();
   for (int i = 0; i < srcString.GetLength(); ++i) 
   {
      if (srcString[i] == _T(';'))
      {
         destList.Add(listElement);
         listElement.Empty();
      }
      else 
      {
         if (!((srcString[i] == _T(' ')) && (listElement.GetLength() == 0)))
            listElement += srcString[i];
      }
   }
   
   if (listElement.GetLength() > 0)
   {
      destList.Add(listElement);
      listElement.Empty();
   }

}

void ASSISTANT::CreateStringFromList(CString &destString, CStringArray &srcList)
{
   destString.Empty();
   for (int i = 0; i < srcList.GetSize(); ++i)
   {
      if ( i > 0)
         destString += _T("; ");
      destString += srcList[i];
   }
}

void ASSISTANT::FillLogFont(LOGFONT &logFont, 
                            LPCTSTR fontFamily, int fontSize, 
                            LPCTSTR fontWeight, LPCTSTR fontSlant, BOOL bIsUnderlined)
{
   ZeroMemory(&(logFont), sizeof(LOGFONT));  
												
   logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE; 

   logFont.lfHeight = (-1) * abs(fontSize);

   logFont.lfWidth       = 0;
   logFont.lfEscapement  = 0 ;
   logFont.lfOrientation = 0 ;

   if (_tcscmp(fontWeight, _T("bold")) == 0)
      logFont.lfWeight = FW_BOLD;
   else
      logFont.lfWeight = FW_NORMAL;

   if (_tcscmp(fontSlant, _T("italic")) == 0)
      logFont.lfItalic = true;
   else
      logFont.lfItalic = false;

   logFont.lfUnderline     = FALSE ;
   logFont.lfStrikeOut     = FALSE ;
   logFont.lfCharSet       = DEFAULT_CHARSET ;
   logFont.lfOutPrecision  = OUT_TT_PRECIS; //OUT_TT_ONLY_PRECIS ;   
   logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS ;   
   logFont.lfQuality       = PROOF_QUALITY; 
   logFont.lfUnderline     = bIsUnderlined;
   
   _tcscpy(logFont.lfFaceName, fontFamily);  
}

void ASSISTANT::GdipARGBToString(Gdiplus::ARGB argb, CString &csColor)
{
   if (argb == 0)
   {
      csColor = _T("none");
      return;
   }

   Gdiplus::Color gdipColor(argb);

   if (gdipColor.GetAlpha() == 255)
      csColor.Format(_T("#%02x%02x%02x"), gdipColor.GetRed(), gdipColor.GetGreen(), gdipColor.GetBlue());
   else
      csColor.Format(_T("#%02x%02x%02x%02x"), gdipColor.GetAlpha(), gdipColor.GetRed(), gdipColor.GetGreen(), gdipColor.GetBlue());
}

Gdiplus::ARGB ASSISTANT::StringToGdipARGB(LPCTSTR color)
{
   if (color == NULL || _tcscmp(color, _T("none")) == 0)
      return 0;

   CString sColor = color;
   if (sColor[0] == _T('#')) // color value from LSD
   {
      sColor.Replace(_T("#"), _T("0x"));
   }
   else
   {
      sColor = sColor.Right(sColor.GetLength() - 1);
   }
   Gdiplus::ARGB argbColor = _tcstoul(sColor, NULL, 16);
   BYTE red = (BYTE) (((DWORD)(argbColor >> 16) & 0x000000ff));
   BYTE green = (BYTE) (((DWORD)(argbColor >> 8) & 0x000000ff));
   BYTE blue = (BYTE) (((DWORD)(argbColor) & 0x000000ff));

   BYTE alpha = (((DWORD)(argbColor >> 24) & 0x000000ff));
   if (alpha == 0) // no alpha value given, set to solid
      argbColor += 0xff000000;

   return argbColor;

}

COLORREF ASSISTANT::GdipARGBToColorref(Gdiplus::ARGB argb)
{
   Gdiplus::Color color(argb);
   
   return (RGB(color.GetRed(), color.GetGreen(), color.GetBlue()));
}

Gdiplus::ARGB ASSISTANT::ColorrefToGdipARGB(COLORREF clr, int iTransperency)
{
   Gdiplus::ARGB argb = Gdiplus::Color::MakeARGB(iTransperency, GetRValue(clr), GetGValue(clr), GetBValue(clr));

   return argb;
}

void ASSISTANT::GdipLineStyleToString(Gdiplus::DashStyle gdipLineStyle, CString &csLineStyle)
{
   if (gdipLineStyle == Gdiplus::DashStyleDot)
      csLineStyle = _T(".");
   else if (gdipLineStyle == Gdiplus::DashStyleDash)
      csLineStyle = _T("_");
   else
      csLineStyle = _T(" ");
}

Gdiplus::DashStyle ASSISTANT::StringToGdipLineStyle(LPCTSTR tszStyle)
{
   if (_tcscmp(tszStyle, _T(".")) == 0 || 
       _tcscmp(tszStyle, _T(",")) == 0)
      return  Gdiplus::DashStyleDot;
   else if (_tcscmp(tszStyle, _T("_")) == 0 || 
            _tcscmp(tszStyle, _T("-")) == 0 || 
            _tcscmp(tszStyle, _T(".-")) == 0 || 
            _tcscmp(tszStyle, _T("..-")) == 0)
      return Gdiplus::DashStyleDash;
   else
      return Gdiplus::DashStyleSolid;
}

void ASSISTANT::GdipARGBToObjString(Gdiplus::ARGB argb, CString &csColor)
{
   Gdiplus::Color gdipColor(argb);

   if (gdipColor.GetAlpha() == 255)
      csColor.Format(_T("%02x%02x%02x"), gdipColor.GetRed(), gdipColor.GetGreen(), gdipColor.GetBlue());
   else
      csColor.Format(_T("%02x%02x%02x%02x"), gdipColor.GetAlpha(), gdipColor.GetRed(), gdipColor.GetGreen(), gdipColor.GetBlue());
}

int ASSISTANT::GdipLineStyleToObjStyle(Gdiplus::DashStyle gdipLineStyle)
{
   if (gdipLineStyle == Gdiplus::DashStyleDot)
      return 1;
   else if (gdipLineStyle == Gdiplus::DashStyleDash)
      return 2;
   else
      return 0;
}

Gdiplus::ARGB ASSISTANT::MakeARGB(int iRed, int iGreen, int iBlue, int iTransperency)
{
   Gdiplus::ARGB argbColor; 
   if (iTransperency != 255)
   {
      argbColor = Gdiplus::Color::MakeARGB(iTransperency, iRed, iGreen, iBlue);
   }
   else
   {
      argbColor = RGB(iRed, iGreen, iBlue);
   }

   return argbColor;

}

#ifndef _STUDIO
bool ASSISTANT::IsEditorInstalled()
{
   ASSISTANT::SuiteEntry suiteEntry;

   CString csProgramPath;
   if (!suiteEntry.ReadProgramPath(csProgramPath))
      csProgramPath = _T("");

   if (csProgramPath.GetLength() > 0) 
   {
      CString csEditorPath = csProgramPath;
      csEditorPath += _T("\\Editor\\editor.exe");

      if (_taccess(csEditorPath, 00) == 0)
         return true;
      else
         return false;
   }

   return false;
}

void ASSISTANT::GetProgramPath(CString &csProgramPath)
{
   ASSISTANT::SuiteEntry
      suiteEntry;

   CString programPath;
   if (!suiteEntry.ReadProgramPath(programPath))
      programPath = _T("");

   if (programPath.GetLength() > 0) 
   {
      csProgramPath = programPath;
      if (csProgramPath[csProgramPath.GetLength() - 1] != _T('\\'))
         csProgramPath += _T("\\");
      csProgramPath += _T("Assistant");
   }

}
#endif

bool ASSISTANT::GetShellFolders(const _TCHAR *subFolder, CString &returnValue)
{
	DWORD		
      dwType,
      dwBufSize = 0;
   char 
      *binData;
   HKEY 
      hKeyUser; 

   LONG regErr = RegOpenKeyEx (HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), NULL,KEY_READ,&hKeyUser); 
	if ( regErr != ERROR_SUCCESS )
   {
      return false;
	}

	regErr = RegQueryValueEx(hKeyUser, subFolder, NULL, &dwType, NULL, &dwBufSize );

	if ( regErr != ERROR_SUCCESS )
   {
      RegCloseKey(hKeyUser);
      return false;
	}
	
	if ( dwType != REG_SZ )
	{
      RegCloseKey(hKeyUser);
      return false; 
	}

	binData = (char *)malloc( dwBufSize + 8 );
	if ( binData == NULL )
	{
      RegCloseKey(hKeyUser);
      return false;
	}

	regErr = RegQueryValueEx(hKeyUser, subFolder, NULL, &dwType, (unsigned char *)binData, &dwBufSize );

	if ( regErr != ERROR_SUCCESS )
	{
		free( binData );
      RegCloseKey(hKeyUser);
      return false;
	}

   returnValue = binData;

   free(binData);

   RegCloseKey(hKeyUser);

   return true;
}


CString ASSISTANT::GetInitialDir(LPCTSTR szFileType, bool bOpen)
{
   CString csDir;
   _TCHAR szDefaultDirectory[MAX_PATH];
   unsigned long lLength = MAX_PATH;
   bool bRet = false;

   if (_tcscmp(szFileType, _T("LSD")) == 0 ||
       _tcscmp(szFileType, _T("LAP")) == 0 ||
       _tcscmp(szFileType, _T("PPT")) == 0)
   {
      if (bOpen)
         bRet = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
                                                   _T("Software\\imc AG\\LECTURNITY\\Assistant\\Dirs\\Open"), szFileType, 
                                                    szDefaultDirectory, &lLength);
      else
         bRet = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
                                                   _T("Software\\imc AG\\LECTURNITY\\Assistant\\Dirs\\Save"), szFileType, 
                                                    szDefaultDirectory, &lLength);
      
      if (bRet)
         csDir = szDefaultDirectory;
      else
      {
         ASSISTANT::GetLecturnityHome(csDir);
         csDir += _T("\\Source Documents"); 
      }
   }
   else if (_tcscmp(szFileType, _T("IMAGE")) == 0)
   {
      if (bOpen)
         bRet = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
                                                   _T("Software\\imc AG\\LECTURNITY\\Assistant\\Dirs\\Open"), szFileType, 
                                                    szDefaultDirectory, &lLength);
      else
         bRet = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
                                                   _T("Software\\imc AG\\LECTURNITY\\Assistant\\Dirs\\Save"), szFileType, 
                                                    szDefaultDirectory, &lLength);
      
      if (bRet)
         csDir = szDefaultDirectory;
      else
      {
         GetShellFolders(_T("My Pictures"), csDir);
      }
   }



   return csDir;
}


void ASSISTANT::SetInitialDir(CString &csDir, LPCTSTR szFileType, bool bOpen)
{
   if (bOpen)
      LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
                        _T("Software\\imc AG\\LECTURNITY\\Assistant\\Dirs\\Open"), szFileType, csDir);
   else
      LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
                        _T("Software\\imc AG\\LECTURNITY\\Assistant\\Dirs\\Save"), szFileType, csDir);
}

HRESULT ASSISTANT::GetOpenFilename(UINT idFilter, CString &csFilename, LPCTSTR szFileType)
{
	CString csFilter;
	csFilter.LoadString(idFilter);

   FileDialog::GetOpenFilename(AfxGetApp(), csFilename, AfxGetMainWnd()->GetSafeHwnd(), 
                               AfxGetInstanceHandle(), csFilter, NULL, GetInitialDir(szFileType, true));

   if (!csFilename.IsEmpty())
   {
      CString csDir = csFilename;
      ASSISTANT::GetPath(csDir);
      SetInitialDir(csDir, szFileType, true);
   }

   return S_OK;
}

HRESULT ASSISTANT::GetSaveFilename(UINT idFilter, CString &csFilename, LPCTSTR szExtension, LPCTSTR szFileType)
{
	CString csFilter;
	csFilter.LoadString(idFilter);

    StringManipulation::RemoveReservedFilenameChars(csFilename);

   FileDialog::GetSaveFilename(AfxGetApp(), csFilename, AfxGetMainWnd()->GetSafeHwnd(), 
                               AfxGetInstanceHandle(), csFilter, szExtension, NULL, 
                               GetInitialDir(szFileType, false), csFilename);

   if (!csFilename.IsEmpty())
   {
      CString csDir = csFilename;
      ASSISTANT::GetPath(csDir);
      SetInitialDir(csDir, szFileType, false);
   }

   return S_OK;
}

double ASSISTANT::ComputeDistance(Gdiplus::PointF &ptLineBegin, Gdiplus::PointF &ptLineEnd, 
                       Gdiplus::PointF &ptPoint)
{
   double x, y;
   
   /*
   * Compute the point on the line that is closest to the
   * point.  This must be done separately for vertical edges,
   * horizontal edges, and other edges.
   */
   
   if (ptLineBegin.X == ptLineEnd.X)      // vertical
   {
      x = ptLineBegin.X;
      if (ptLineBegin.Y >= ptLineEnd.Y) 
      {
         y = MIN(ptLineBegin.Y, ptPoint.Y);
         y = MAX(y, ptLineEnd.Y);
      } 
      else 
      {
         y = MIN(ptLineEnd.Y, ptPoint.Y);
         y = MAX(y, ptLineBegin.Y);
      }
   } 
   else if (ptLineBegin.Y == ptLineEnd.Y)    // horizontal
   {
      y = ptLineBegin.Y;
      if (ptLineBegin.X >= ptLineEnd.X) 
      {
         x = MIN(ptLineBegin.X, ptPoint.X);
         x = MAX(x, ptLineEnd.X);
      } 
      else 
      {
         x = MIN(ptLineEnd.X, ptPoint.X);
         x = MAX(x, ptLineBegin.X);
      }
   } 
   else 
   {
      /*
      * The edge is neither horizontal nor vertical.  Convert the
      * edge to a line equation of the form y = m1*x + b1.  Then
      * compute a line perpendicular to this edge but passing
      * through the point, also in the form y = m2*x + b2.
      */
      
      double m1 = (ptLineEnd.Y - ptLineBegin.Y)/(ptLineEnd.X - ptLineBegin.X);
      double b1 = ptLineBegin.Y - m1*ptLineBegin.X;
      double m2 = -1.0/m1;
      double b2 = ptPoint.Y - m2*ptPoint.X;
      x = (b2 - b1)/(m1 - m2);
      y = m1*x + b1;
      if (ptLineBegin.X > ptLineEnd.X) 
      {
         if (x > ptLineBegin.X) 
         {
            x = ptLineBegin.X;
            y = ptLineBegin.Y;
         } 
         else if (x < ptLineEnd.X) 
         {
            x = ptLineEnd.X;
            y = ptLineEnd.Y;
         }
      } 
      else 
      {
         if (x > ptLineEnd.X) 
         {
            x = ptLineEnd.X;
            y = ptLineEnd.Y;
         } 
         else if (x < ptLineBegin.X) 
         {
            x = ptLineBegin.X;
            y = ptLineBegin.Y;
         }
      }
   }
   
   /*
   * Compute the distance to the closest point.
   */
   
   return hypot(ptPoint.X - x, ptPoint.Y - y);
}


WCHAR *ASSISTANT::ConvertTCharToWChar(LPCTSTR tszString)
{
   WCHAR *wszReturn = NULL;
   
   int iStringLength = _tcslen(tszString);

   wszReturn = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR));

#ifndef _UNICODE
   MultiByteToWideChar(CP_ACP, 0, tszString, -1, wszReturn, iStringLength + 1); 
#else
   wcscpy(wszReturn, tszString);
#endif

   return wszReturn;
}
   

void ASSISTANT::GetPossibleKeys(CStringArray &arPossibleKeys)
{
   CString csNone;
   csNone.LoadString(IDS_SG_HOTKEYS_CMB_NONE);

   arPossibleKeys.Add(csNone);
   arPossibleKeys.Add(_T("F9"));
   arPossibleKeys.Add(_T("F10"));
   arPossibleKeys.Add(_T("F11"));
   arPossibleKeys.Add(_T("F12"));
   arPossibleKeys.Add(_T("A"));
   arPossibleKeys.Add(_T("B"));
   arPossibleKeys.Add(_T("C"));
   arPossibleKeys.Add(_T("D"));
   arPossibleKeys.Add(_T("E"));
   arPossibleKeys.Add(_T("F"));
   arPossibleKeys.Add(_T("G"));
   arPossibleKeys.Add(_T("H"));
   arPossibleKeys.Add(_T("I"));
   arPossibleKeys.Add(_T("J"));
   arPossibleKeys.Add(_T("K"));
   arPossibleKeys.Add(_T("L"));
   arPossibleKeys.Add(_T("M"));
   arPossibleKeys.Add(_T("N"));
   arPossibleKeys.Add(_T("O"));
   arPossibleKeys.Add(_T("P"));
   arPossibleKeys.Add(_T("Q"));
   arPossibleKeys.Add(_T("R"));
   arPossibleKeys.Add(_T("S"));
   arPossibleKeys.Add(_T("T"));
   arPossibleKeys.Add(_T("U"));
   arPossibleKeys.Add(_T("V"));
   arPossibleKeys.Add(_T("W"));
   arPossibleKeys.Add(_T("X"));
   arPossibleKeys.Add(_T("Y"));
   arPossibleKeys.Add(_T("Z"));
}

#include "StdAfx.h"
//#include <windows.h>

#include "la_font.h"
#include "..\..\Studio\Resource.h"
#include "lutils.h"

static int CALLBACK ShowFonts(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam);


ASSISTANT::FontInfo::FontInfo()
{
   fontFamily_.Empty();
   for (int i = 0; i < 4; ++i)
   {
      fontFace_[i].Empty();
      fontFilename_[i].Empty();
   }
   hasTTF_ = false;
   isAskedForTTF_ = false;
}

ASSISTANT::FontInfo::~FontInfo()
{
   fontFamily_.Empty();
   for (int i = 0; i < 4; ++i)
   {
      fontFace_[i].Empty();
      fontFilename_[i].Empty();
   }

   DeleteErrorPages();
}

void ASSISTANT::FontInfo::SetFontFamily(const _TCHAR *fontFamily)
{
   fontFamily_ = fontFamily;
}

void ASSISTANT::FontInfo::SetFontFilename(const _TCHAR *fontFace, const _TCHAR *fontFilename)
{
   for (int i = 0; i < 4; ++i)
   {
      if (fontFace_[i] == fontFace && fontFilename_[i] == _T(""))
      {
         fontFilename_[i] = fontFilename;
         hasTTF_ = true;
      }

      if (fontFace_[i] == _T("Wingdings") && _tcscmp(fontFace, _T("WingDings")) == 0 && 
          fontFilename_[i] == _T(""))
      {
         fontFilename_[i] = fontFilename;
         hasTTF_ = true;
      }
   }
}

void ASSISTANT::FontInfo::AddFontFace(const _TCHAR *fontFace, int index)
{
   if (index >= 0 && index < 4)
      fontFace_[index] = fontFace;
}

CString &ASSISTANT::FontInfo::GetFontFilename(bool isBold, bool isItalic)
{
//#ifdef _DEBUG
//   {
//      _TCHAR tszMsg[1024];
//      _stprintf(tszMsg, _T("isBold: %d, isItalic: %d\n\n"
//                           "fontFilename_[0] == \"%s\"\n"
//                           "fontFilename_[1] == \"%s\"\n"
//                           "fontFilename_[2] == \"%s\"\n"
//                           "fontFilename_[3] == \"%s\"\n"),
//                           isBold, isItalic,
//                           fontFilename_[0],
//                           fontFilename_[1],
//                           fontFilename_[2],
//                           fontFilename_[3]);
//      MessageBox(NULL, tszMsg, _T("Information"), MB_OK);
//   }
//#endif
   if (isItalic)
   {
      if (isBold)
         return fontFilename_[3];
      else
         return fontFilename_[2];
   }
   else if (isBold)
      return fontFilename_[1];
   else
      return fontFilename_[0];
}

bool ASSISTANT::FontInfo::IsSystemFamilyEqual(const _TCHAR *systemFamily)
{
   CString tmpFontName = systemFamily;

   //CString::size_type pos = tmpFontName.find_last_of("(");
   //tmpFontName = tmpFontName.substr(0, pos-1);

   for (int i = 0; i < 4; ++i)
   {
//#ifdef _DEBUG
//      _TCHAR tszTmp[256];
//      _stprintf(tszTmp, _T("'%s' == '%s'?"), tmpFontName, fontFace_[i]);
//      MessageBox(NULL, tszTmp, _T("IsSystemFamilyEqual"), MB_OK);
//#endif
      if (tmpFontName == fontFace_[i])
         return true;
   }

   if (fontFamily_ == _T("Wingdings"))
   {
      if (tmpFontName == _T("WingDings"))
         return true;
   }

   if (fontFamily_ == _T("Arial Narrow"))
   {
      if (tmpFontName == _T("Arial Narrow Standard"))
         return true;
   }

   return false;
}

bool ASSISTANT::FontInfo::IsFontFamilyEqual(const _TCHAR *fontFamily)
{
//#ifdef _DEBUG
//   _TCHAR tszFont[256];
//   _stprintf(tszFont, _T("\"%s\" == \"%s\""), fontFamily_, fontFamily);
//   MessageBox(NULL, tszFont, _T("Font family"), MB_OK);
//#endif
   if (fontFamily_ == fontFamily)
      return true;

   return false;
}

bool ASSISTANT::FontInfo::HasTTFFilename()
{
      isAskedForTTF_ = true;
      if (hasTTF_)
         return true;
      else
         return false;
   }

void ASSISTANT::FontInfo::AddErrorPage(LPCTSTR szPageNumber, int iPage)
{
   bool bNumberIsAlreadyInserted = false;
   for (int i = 0; i != m_aErrorPages.GetSize(); ++i)
   {
      PAGE_NUMBER *pPageNumber = m_aErrorPages[i];
      if (pPageNumber->iPageNumber == iPage)
      {
         bNumberIsAlreadyInserted = true;
         break;
      }
   }

   if (!bNumberIsAlreadyInserted)
   {
      PAGE_NUMBER *pPageNumber = new PAGE_NUMBER;
      pPageNumber->csPageNumber = szPageNumber;
      pPageNumber->iPageNumber = iPage;
      m_aErrorPages.Add(pPageNumber);
   }
}

bool ASSISTANT::FontInfo::HasNewErrors()
{
   if (m_aErrorPages.GetSize() == 0)
      return false;

   return true;
}

void ASSISTANT::FontInfo::DeleteErrorPages()
{
   for (int i = 0; i != m_aErrorPages.GetSize(); ++i)
      delete (m_aErrorPages[i]);

   m_aErrorPages.RemoveAll();
}

void ASSISTANT::FontInfo::FillErrorPageList(CString & csErrorPageList)
{
   csErrorPageList.Empty();
   bool bFirst = true;
   for (int i = 0; i != m_aErrorPages.GetSize(); ++i)
   {
      if (!bFirst)
         csErrorPageList += _T(", ");
      else
         bFirst = false;

      PAGE_NUMBER *pPageNumber = m_aErrorPages[i];
      csErrorPageList += pPageNumber->csPageNumber;
   }
}

/********************************************/
/********************************************/
/********************************************/

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

ASSISTANT::FontList::FontList()
{
   InitVariables();
}

ASSISTANT::FontList::~FontList()
{
   ReleaseVariables();
}

void ASSISTANT::FontList::InitVariables()
{
   InitializeFontfamilies();
   
}

void ASSISTANT::FontList::ReleaseVariables()
{
   for (int i = 0; i < fontInfoList_.GetSize(); ++i)
   {
      delete fontInfoList_[i];
   }
   fontInfoList_.RemoveAll();
   
   for (i = 0; i < notSupportedFonts_.GetSize(); ++i)
   {
      delete notSupportedFonts_[i];
   }
   notSupportedFonts_.RemoveAll();
   
   for (i = 0; i < m_lstNoTrueTypeFonts_.GetSize(); ++i)
   {
      delete m_lstNoTrueTypeFonts_[i];
   }
   m_lstNoTrueTypeFonts_.RemoveAll();  
}


void ASSISTANT::FontList::InitializeFontfamilies()
{
   _TCHAR faceName[32];
   faceName[0] = _T('\0');

   HDC hDc = GetWindowDC(NULL);

   LOGFONT logFont;
   logFont.lfCharSet = DEFAULT_CHARSET ;
   _tcscpy(logFont.lfFaceName, faceName);
   logFont.lfPitchAndFamily = 0;
   logFont.lfOutPrecision  = OUT_DEFAULT_PRECIS;

   int result = EnumFontFamiliesEx(hDc, &logFont, (FONTENUMPROC )ShowFonts, (long)this, 0);
   
   ReleaseDC(NULL, hDc);

   AddFilenames();
   return;
}

bool ASSISTANT::FontList::FindFontInfo(const _TCHAR *entryName, CArray<FontInfo *, FontInfo *> *pList)
{
   for (int i = 0; i < fontInfoList_.GetSize(); ++i)
   {
      if (fontInfoList_[i]->IsSystemFamilyEqual(entryName))
      {
         pList->Add(fontInfoList_[i]);
         //return (*i);
      }
   }

   return (pList->GetSize() > 0);
}

void ASSISTANT::FontList::AddFilenames()
{
   HKEY hKeyMachine; 
   LONG regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
                     _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), NULL,KEY_READ,&hKeyMachine); 
	if ( regErr != ERROR_SUCCESS )
   {
      LONG regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
                        _T("Software\\Microsoft\\Windows\\CurrentVersion\\Fonts"), NULL,KEY_READ,&hKeyMachine); 
      if ( regErr != ERROR_SUCCESS )
      {
         return;
      }
	}

   DWORD subValues;
   regErr = RegQueryInfoKey(hKeyMachine, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
   if ( regErr != ERROR_SUCCESS )
   {
      return;
   }

   HRESULT hr = ::CoInitialize(NULL);
   ULONG_PTR token;
   Gdiplus::GdiplusStartupInput gdipsi;
   ZeroMemory(&gdipsi, sizeof Gdiplus::GdiplusStartupInput);
   gdipsi.GdiplusVersion = 1;
   Gdiplus::GdiplusStartup(&token, &gdipsi, NULL);
   HDC hdc = ::CreateCompatibleDC(NULL);

   // Find the Windows Fonts directory
   CString lsWindowsFontsDir;
   {
      _TCHAR tszWinDir[_MAX_PATH];
      ::GetWindowsDirectory(tszWinDir, _MAX_PATH);
      lsWindowsFontsDir = tszWinDir;
      if (lsWindowsFontsDir[lsWindowsFontsDir.GetLength() - 1] != _T('\\'))
         lsWindowsFontsDir += _T("\\");
      lsWindowsFontsDir += _T("Fonts\\");
   }
   
   DWORD dwType;
   _TCHAR entryName[512];
   unsigned long entryNameLength = 512;
   TCHAR entryValue[512];
   unsigned long entryValueLength = 512;
   for (int i = 0; i < (int)subValues; i++) 
   {          
      entryNameLength = 512;
      entryValueLength = 512;
      if (RegEnumValue(hKeyMachine, i, entryName, &entryNameLength, NULL, &dwType, (LPBYTE)entryValue, &entryValueLength) != ERROR_SUCCESS)
      {
         continue;
      }
      CString faceName = entryName;
      int iBracePos = faceName.ReverseFind('(');
      faceName = faceName.Left(iBracePos-1);

      CArray<FontInfo *, FontInfo *> fontInfoList;

      bool bFoundFontInfo = false;
      if (FindFontInfo(faceName, &fontInfoList))
      {
         bFoundFontInfo = true;
      }
      else
      {
         // BUGFIX 1935:
         // In some cases, the font face name in the registry does not
         // match the font face name returned in the OUTLINETEXTMETRIC
         // object used in ShowFonts. In that case, we'll try to find
         // the localized name of the font by instanciating it using
         // a PrivateFontCollection (GDI+) object and retrieving the
         // face name over that font's OUTLINETEXTMETRIC. This is a quite
         // nasty workaround, but we could not find other possibilites.

         //_TCHAR tszMsg[256];
         //_stprintf(tszMsg, _T("Checking font '%s'"), entryName);
         //MessageBox(NULL, tszMsg, _T("Information"), MB_OK);
         bool success = true;

         Gdiplus::PrivateFontCollection pfc;

         CString csFontsFileName;
         csFontsFileName = entryValue;
         if (csFontsFileName.Find(_T("\\")) == -1)
         {
            CString csTtfName;
            csTtfName = entryValue;
            csFontsFileName = lsWindowsFontsDir;
            csFontsFileName += csTtfName;
         }

         WCHAR wszFontFileName[_MAX_PATH];

#ifdef _UNICODE
         wcscpy(wszFontFileName, csFontsFileName);
#else
         ::MultiByteToWideChar(CP_ACP, 0, csFontsFileName, -1, wszFontFileName, _MAX_PATH);
#endif

         Gdiplus::Status stat = pfc.AddFontFile(wszFontFileName);
         success = (stat == Gdiplus::Ok);
         int nFams = 0;
         if (success)
            nFams = pfc.GetFamilyCount();
         if (nFams > 0)
         {
            Gdiplus::FontFamily *aFams = new Gdiplus::FontFamily[nFams];
            int nFound = 0;
            stat = pfc.GetFamilies(nFams, aFams, &nFound);
            success = (stat == Gdiplus::Ok);
            WCHAR wszName[LF_FACESIZE];
            if (success)
            {
               stat = aFams[0].GetFamilyName(wszName);
               success = (stat == Gdiplus::Ok);
            }

            INT fontStyle = -1;
            if (success)
            {
               if (FALSE != aFams[0].IsStyleAvailable(Gdiplus::FontStyleRegular))
                  fontStyle = Gdiplus::FontStyleRegular;
               else if (FALSE != aFams[0].IsStyleAvailable(Gdiplus::FontStyleBold))
                  fontStyle = Gdiplus::FontStyleBold;
               else if (FALSE != aFams[0].IsStyleAvailable(Gdiplus::FontStyleItalic))
                  fontStyle = Gdiplus::FontStyleItalic;
               else if (FALSE != aFams[0].IsStyleAvailable(Gdiplus::FontStyleBoldItalic))
                  fontStyle = Gdiplus::FontStyleBoldItalic;
            }

            if (fontStyle != -1)
            {
               Gdiplus::Graphics graphics(hdc);
               Gdiplus::Font font(&aFams[0], 10.0, fontStyle);
#ifdef _UNICODE
               LOGFONTW lf;
               stat = font.GetLogFontW(&graphics, &lf);
               success = (stat == Gdiplus::Ok);
               HFONT hfont = NULL;
               if (success)
                  hfont = ::CreateFontIndirectW(&lf);
#else
               LOGFONTA lf;
               stat = font.GetLogFontA(&graphics, &lf);
               success = (stat == Gdiplus::Ok);
               HFONT hfont = NULL;
               if (success)
                  hfont = ::CreateFontIndirectA(&lf);
#endif
               success = (hfont != NULL);
               if (success)
               {
                  HGDIOBJ hOld = ::SelectObject(hdc, hfont);

                  UINT nSize = ::GetOutlineTextMetrics(hdc, 0, NULL);
                  if (nSize > 0)
                  {
                     LPOUTLINETEXTMETRIC lpOtm = (LPOUTLINETEXTMETRIC) new char[nSize];

                     nSize = ::GetOutlineTextMetrics(hdc, nSize, lpOtm);

                     if (nSize != 0)
                     {
                        faceName = (_TCHAR *)((DWORD) lpOtm + lpOtm->otmpFaceName);

                        if (FindFontInfo(faceName, &fontInfoList))
                        {
                           bFoundFontInfo = true;
                        }
                     }

                     delete[] lpOtm;
                  }

                  ::SelectObject(hdc, hOld);
                  ::DeleteObject(hfont);
               }
            }

            delete[] aFams;
         }

      }
      // =======================

      //if (FindFontInfo(faceName, &fontInfoList))
      if (bFoundFontInfo)
      {
         for (int i = 0; i < fontInfoList.GetSize(); ++i)
         {
            CString csValue = entryValue;
            fontInfoList[i]->SetFontFilename(faceName, csValue);
         }
      }
//#ifdef _DEBUG
//      else
//      {
//         _TCHAR tszMsg[512];
//         _stprintf(tszMsg, _T("Font info not found:\n'%s'\n'%s'"), faceName, entryValue);
//         MessageBox(NULL, tszMsg, _T("Information"), MB_OK);
//      }
//#endif
   } 

   ::DeleteDC(hdc);
   Gdiplus::GdiplusShutdown(token);
   ::CoUninitialize();

   RegCloseKey(hKeyMachine);

   return;
}

/*
bool ASSISTANT::FontList::GetFontProperties(const char *fileName, const char *fontPath, LPFONT_PROPERTIES lpFontProps)
{
	FILE *f;
	bool bRetVal = false;
   
   CString filePath = fontPath;
   filePath += "\\";
   filePath += fileName;

	if ((f = fopen(filePath, "rb")) != NULL)
   {
		TT_OFFSET_TABLE ttOffsetTable;
		int bytesRead = fread(&ttOffsetTable, sizeof(TT_OFFSET_TABLE), 1, f);
		ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
		ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
		ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

		//check is this is a true type font and the version is 1.0
		if(ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
      {
         fclose(f);
			return bRetVal;
      }
		
		TT_TABLE_DIRECTORY tblDir;
		BOOL bFound = FALSE;
      CString csTemp;
      

      for(int i=0; i< ttOffsetTable.uNumOfTables; i++)
      {
         fread(&tblDir, 1, sizeof(TT_TABLE_DIRECTORY), f);
         csTemp = tblDir.szTag;
         csTemp = csTemp.substr(0, 4);
         if(csTemp == "name")
         {
            bFound = TRUE;
            tblDir.uLength = SWAPLONG(tblDir.uLength);
            tblDir.uOffset = SWAPLONG(tblDir.uOffset);
            break;
         }
         else if(csTemp.empty())
         {
            break;
         }
      }
		
		if(bFound)
      {
			fseek(f, tblDir.uOffset, SEEK_SET);
			TT_NAME_TABLE_HEADER ttNTHeader;
			fread(&ttNTHeader, 1, sizeof(TT_NAME_TABLE_HEADER), f);
			ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
			ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);
			TT_NAME_RECORD ttRecord;
			bFound = FALSE;
			
			for(int i=0; 
				i<ttNTHeader.uNRCount && 
					(lpFontProps->csCopyright.empty() || lpFontProps->csName.empty() || lpFontProps->csTrademark.empty() || lpFontProps->csFamily.empty()); 
				i++)
            {
				fread(&ttRecord, 1, sizeof(TT_NAME_RECORD), f);
				ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
				ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
				ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);

				if(ttRecord.uNameID == 1 || ttRecord.uNameID == 0 || ttRecord.uNameID == 7){
					int nPos = ftell(f);
					fseek(f, tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset, SEEK_SET);
					csTemp.erase();
               char *buffer;
               buffer = (char *)malloc(ttRecord.uStringLength+1);
					fread(buffer, 1, ttRecord.uStringLength, f);
               buffer[ttRecord.uStringLength] = '\0';
               csTemp = buffer;
               free(buffer);
					if(csTemp.length() > 0){
						switch(ttRecord.uNameID){
						case 1:
							lpFontProps->csFamily.empty() ? lpFontProps->csFamily = csTemp : void(0);
							bRetVal = TRUE;
							break;
						case 0:
							lpFontProps->csCopyright.empty() ? lpFontProps->csCopyright = csTemp : void(0);
							break;
						case 7:
							lpFontProps->csTrademark.empty() ? lpFontProps->csTrademark = csTemp : void(0);
							break;
						case 4:
							lpFontProps->csName.empty() ? lpFontProps->csName = csTemp : void(0);
							break;
						default:
							break;
						}
					}
					fseek(f, nPos, SEEK_SET);
				}
			}			
		}
		fclose(f);
		if(lpFontProps->csName.empty())
			lpFontProps->csName = lpFontProps->csFamily;

      lpFontProps->ttfName = fileName;
	}

	return bRetVal;
	
}
*/

/*
void ASSISTANT::FontList::AddTTFFilenames()
{
   int bufferSize = GetWindowsDirectory(NULL, 0);

   LPTSTR lpBuffer = (LPTSTR)malloc(bufferSize+1);
   bufferSize = GetWindowsDirectory(lpBuffer, bufferSize);

   CString fontPath = lpBuffer;
   free(lpBuffer);

   fontPath += "\\Fonts";
		
   WIN32_FIND_DATA 
      ffd;

   CString fontMask = fontPath;
   fontMask += "\\*.ttf";
   HANDLE hFile = FindFirstFile((LPCTSTR)fontMask, &ffd);

   if (hFile != INVALID_HANDLE_VALUE)
   {             
      while (FindNextFile(hFile, &ffd))
      {
			LPFONT_PROPERTIES lpFontProps = new FONT_PROPERTIES;
         GetFontProperties(ffd.cFileName, fontPath, lpFontProps);
       
         FontInfo *fontInfo = FindFontInfo(lpFontProps->csFamily);
         if (fontInfo)
            fontInfo->SetFontFilename(lpFontProps->ttfName);
      }
   } 
         
   FindClose(hFile);
   // SERACH FOR FILES

}
*/
void ASSISTANT::FontList::GetFontFamilies(CString &fontFamilies)
{
   fontFamilies.Empty();
   for (int i = 0; i < fontInfoList_.GetSize(); ++i)
   {
      fontFamilies += _T("{");
      fontFamilies += fontInfoList_[i]->GetFontFamily();
      fontFamilies += _T("} ");
   }

   return;
}

void ASSISTANT::FontList::GetFontFilename(const _TCHAR *fontFamily, CString &fileName, bool isBold, bool isItalic)
{
   for (int i = 0; i < fontInfoList_.GetSize(); ++i)
   {
      if (fontInfoList_[i]->IsFontFamilyEqual(fontFamily))
      {
         fileName = fontInfoList_[i]->GetFontFilename(isBold, isItalic);
         break;
      }
   }

   return;
}

ASSISTANT::FontInfo *ASSISTANT::FontList::AddNewFamily(const _TCHAR *family, bool bIsTTFont)
{
   FontInfo *newFontInfo = new FontInfo();

   newFontInfo->SetFontFamily(family);

   if (bIsTTFont)
   {
      for (int i = 0; i < fontInfoList_.GetSize(); ++i)
      {
         if (_tcscmp(fontInfoList_[i]->GetFontFamily(), family) > 0)
         {
            fontInfoList_.InsertAt(i, newFontInfo);
            return newFontInfo;
         }
      }
      fontInfoList_.Add(newFontInfo);
   }
   else
   {
      m_lstNoTrueTypeFonts_.Add(newFontInfo);
   }


   return newFontInfo;
}

bool ASSISTANT::FontList::HasTTFFilename(const _TCHAR *fontFamily)
{
   for (int i = 0; i < fontInfoList_.GetSize(); ++i)
   {
      if (fontInfoList_[i]->IsFontFamilyEqual(fontFamily))
      {
         return fontInfoList_[i]->HasTTFFilename();
      }
   }
   
   for (i = 0; i < m_lstNoTrueTypeFonts_.GetSize(); ++i)
   {
      if (m_lstNoTrueTypeFonts_[i]->IsFontFamilyEqual(fontFamily))
      {
         return m_lstNoTrueTypeFonts_[i]->HasTTFFilename();
      }
   }
   
   for (i = 0; i < notSupportedFonts_.GetSize(); ++i)
   {
      if (notSupportedFonts_[i]->IsFontFamilyEqual(fontFamily))
      {
         return notSupportedFonts_[i]->HasTTFFilename();
      }
   }
   
   FontInfo *notSupported = new FontInfo();
   notSupported->SetFontFamily(fontFamily);
   notSupportedFonts_.Add(notSupported);
   return notSupported->HasTTFFilename();
   

   return false;
}

void ASSISTANT::FontList::AddErrorPage(const _TCHAR *fontFamily, LPCTSTR szPageNumber, int iPageNumber)
{
   for (int i = 0; i < m_lstNoTrueTypeFonts_.GetSize(); ++i)
   {
      FontInfo *pInfo = m_lstNoTrueTypeFonts_[i];
      if (m_lstNoTrueTypeFonts_[i]->IsFontFamilyEqual(fontFamily))
      {
         m_lstNoTrueTypeFonts_[i]->AddErrorPage(szPageNumber, iPageNumber);
         return;
      }
   }

   for (i = 0; i < notSupportedFonts_.GetSize(); ++i)
   {
      if (notSupportedFonts_[i]->IsFontFamilyEqual(fontFamily))
      {
         notSupportedFonts_[i]->AddErrorPage(szPageNumber, iPageNumber);
         return;
      }
   }
}


bool ASSISTANT::FontList::IncludeUnsupportedFonts()
{
   
   for (int i = 0; i < m_lstNoTrueTypeFonts_.GetSize(); ++i)
   {
      if (m_lstNoTrueTypeFonts_[i]->HasNewErrors())
         return true;
   }

   for (i = 0; i < notSupportedFonts_.GetSize(); ++i)
   {
      if (notSupportedFonts_[i]->HasNewErrors())
         return true;
   }

   return false;
}

void ASSISTANT::FontList::DeleteErrorPages()
{
   for (int i = 0; i < m_lstNoTrueTypeFonts_.GetSize(); ++i)
     m_lstNoTrueTypeFonts_[i]->DeleteErrorPages();

   for (i = 0; i < notSupportedFonts_.GetSize(); ++i)
     notSupportedFonts_[i]->DeleteErrorPages();
}
      
void ASSISTANT::FontList::FillErrorList(CStringArray &arFontNames, CStringArray &arErrorPages, 
                                        CStringArray &arErrorTypes)
{
   CString csErrorType;
   csErrorType.LoadString(IDS_NO_TRUE_TYPE);
   for (int i = 0; i < m_lstNoTrueTypeFonts_.GetSize(); ++i)
   {
      if (m_lstNoTrueTypeFonts_[i]->HasNewErrors())
      {
         arFontNames.Add(m_lstNoTrueTypeFonts_[i]->GetFontFamily());
         CString ssErrorPages;
         m_lstNoTrueTypeFonts_[i]->FillErrorPageList(ssErrorPages);
         arErrorPages.Add(ssErrorPages);
         arErrorTypes.Add(csErrorType);
      }

   }

   csErrorType.LoadString(IDS_NOT_INSTALLED);
   for (i = 0; i < notSupportedFonts_.GetSize(); ++i)
   {
      if (notSupportedFonts_[i]->HasNewErrors())
      {
         arFontNames.Add(notSupportedFonts_[i]->GetFontFamily());
         CString ssErrorPages;
         notSupportedFonts_[i]->FillErrorPageList(ssErrorPages);
         arErrorPages.Add(ssErrorPages);
         arErrorTypes.Add(csErrorType);
      }

   }
}

static int CALLBACK ShowFonts(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
   static CString lastFamilyName;
   ASSISTANT::FontList *fontClass = ( ASSISTANT::FontList *)lParam;

   if (FontType == TRUETYPE_FONTTYPE)
   {
      CString familyName = (_TCHAR *)lpelfe->elfLogFont.lfFaceName; //lpelfe->elfLogFont.lfFaceName;
//#ifdef _DEBUG
//      MessageBox(NULL, (char *) lpelfe->elfStyle, _T("elfStyle"), MB_OK);
//#endif
      
      if (familyName != lastFamilyName)
      { 
         if (familyName[0] != _T('@'))
         {    
            ASSISTANT::FontInfo *fontInfo = fontClass->AddNewFamily(familyName, true);

            if (fontInfo)
            {
               LOGFONT lf;
               ZeroMemory(&lf, sizeof(lf));  
										         
               lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; 
               lf.lfHeight = 0; //-18;
               lf.lfWidth  = 0;
               lf.lfEscapement = 0 ;
               lf.lfOrientation = 0 ;
               lf.lfUnderline = FALSE ;
               lf.lfStrikeOut = FALSE ;
               lf.lfCharSet = DEFAULT_CHARSET ;
               lf.lfOutPrecision = OUT_DEFAULT_PRECIS; 
               lf.lfClipPrecision = CLIP_DEFAULT_PRECIS ;   
               lf.lfQuality = DEFAULT_QUALITY; 
                 
               _tcscpy(lf.lfFaceName, familyName); 
      
               HDC hdc = CreateCompatibleDC(NULL);  
               if ( hdc != NULL )
               {
                  for (int j = 0; j < 4; ++j)
                  { 

                     if (j == 0)
                     {
                        lf.lfWeight = FW_NORMAL;
                        lf.lfItalic = false;
                     }
                     else if (j == 1)
                     {
                        lf.lfWeight = FW_BOLD;
                        lf.lfItalic = false;
                     }
                     else if (j == 2)
                     {
                        lf.lfWeight = FW_NORMAL;
                        lf.lfItalic = true;
                     }
                     else if (j == 3)
                     {
                        lf.lfWeight = FW_BOLD;
                        lf.lfItalic = true;
                     }

                     HFONT font = CreateFontIndirect(&lf);
                     UINT cbData;       
                     LPOUTLINETEXTMETRIC lpoltm;     
                     SelectObject(hdc, font);
                     cbData = GetOutlineTextMetrics (hdc, 0, NULL);
                     DeleteObject(font);
                     CString faceName;
                     if (cbData == 0) 
                     {
                        faceName = familyName;
                     }
                     else
                     {
                        lpoltm = (LPOUTLINETEXTMETRIC)LocalAlloc (LPTR, cbData);
                        if ( lpoltm )
                        {
                           GetOutlineTextMetrics (hdc, cbData, lpoltm);  
                           faceName = (TCHAR *)((DWORD) lpoltm + lpoltm->otmpFaceName);
                           //#ifdef _DEBUG
                           //                     if (strstr(faceName, "Arial") != NULL)
                           //                        MessageBox(NULL, faceName, _T("Face name"), MB_OK);
                           //#endif
                           LocalFree(lpoltm);
                        }

                     }
                     fontInfo->AddFontFace(faceName, j);
                  }
                  DeleteDC(hdc);
               }
            }
         }
      }
      lastFamilyName = familyName;
   }
   else
   {
      CString familyName = lpelfe->elfLogFont.lfFaceName;
      
      ASSISTANT::FontInfo *fontInfo = fontClass->AddNewFamily(familyName, false);
   }
   return 1;
}

#include "StdAfx.h"
#include "TemplateKeywords.h"
#include "MiscFunctions.h" // StringManipulation (File SDK)
#include "ImageStatic.h" // for Logo Image
#include "lutils.h" // for registry information about Logo Image

/* ************************************************* */

CTemplateKeyword::CTemplateKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName)
{
   m_csName = tszName;
   if (tszFriendlyName)
      m_csFriendlyName = tszFriendlyName;
   else
      m_csFriendlyName = tszName;
   m_pConfig = NULL;
   m_dwMinStructSize = sizeof TE_VAR;
}

void CTemplateKeyword::Restore(LPVOID pData, DWORD dwSize)
{
   if (m_pConfig)
      delete[] m_pConfig;
   if (dwSize >= m_dwMinStructSize)
      m_pConfig = (TE_VAR *) new char[dwSize];
   else
      m_pConfig = (TE_VAR *) new char[m_dwMinStructSize];
   memcpy(m_pConfig, pData, dwSize);
}

bool CTemplateKeyword::Serialize(LPVOID pData, DWORD *pdwSize)
{
   if (m_pConfig == NULL)
      return false;
   if (pData == NULL)
   {
      if (pdwSize == NULL)
         return false;

      *pdwSize = m_pConfig->dwSize;
      return true;
   }

   if (pdwSize == NULL)
      return false;
   if (*pdwSize < m_pConfig->dwSize)
      return false;
   memcpy(pData, (const char *) m_pConfig, m_pConfig->dwSize);

   return true;
}

/* ************************************************* */

CTemplateImageKeyword::CTemplateImageKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName,
                                             int nMinWidth, int nMinHeight, int nMaxWidth, int nMaxHeight)
   : CTemplateKeyword(tszName, tszFriendlyName),
     m_nMinWidth(nMinWidth), m_nMinHeight(nMinHeight), 
     m_nMaxWidth(nMaxWidth), m_nMaxHeight(nMaxHeight)
{
   m_dwMinStructSize = sizeof TE_VAR_IMAGE;
   m_pConfig = (TE_VAR *) new char[sizeof TE_VAR_IMAGE];
   memset(m_pConfig, 0, sizeof TE_VAR_IMAGE);
   m_pConfig->dwSize = sizeof TE_VAR_IMAGE;
   m_pConfig->dwType = TE_TYPE_IMAGE;
#ifdef _UNICODE
   wcscpy(m_pConfig->wszVarName, tszName);
#else
   ::MultiByteToWideChar(CP_ACP, 0, tszName, -1, m_pConfig->wszVarName, 128);
#endif
}

void CTemplateImageKeyword::SetFileName(const _TCHAR *tszFileName)
{
   if (tszFileName == NULL)
      return;

   //_tcscpy(((TE_VAR_IMAGE *) m_pConfig)->tszFileName, tszFileName);
#ifdef _UNICODE
   wcscpy(((TE_VAR_IMAGE *) m_pConfig)->wszFileName, tszFileName);
#else
   ::MultiByteToWideChar(CP_ACP, 0, tszFileName, -1, ((TE_VAR_IMAGE *) m_pConfig)->wszFileName, MAX_PATH);
#endif
}

UINT CTemplateImageKeyword::GetFileName(_TCHAR *tszFileName, DWORD *pdwSize)
{
   RETURN_WSTRING( ((TE_VAR_IMAGE *) m_pConfig)->wszFileName, tszFileName, pdwSize);
}

void CTemplateImageKeyword::SetImageSizeHandling(DWORD dwImageSizeHandling)
{
   ((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling = dwImageSizeHandling;
}

DWORD CTemplateImageKeyword::GetImageSizeHandling()
{
   if ( (((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling) 
      && (((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling >= TE_IMAGE_FITS_ALREADY) 
      && (((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling <= TE_IMAGE_DO_NOT_ADAPT) )
   {
      _tprintf(_T("+ Image size handling: %d\n"), ((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling);
      return ((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling;
   }
   else
   {
      _tprintf(_T("- Image size handling: %d\n"), ((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling);
      // For 1.7.0 - 1.7.0.p3: Read Image Size and then decide 
      // to return TE_IMAGE_DO_NOT_ADAPT (default) or TE_IMAGE_FITS_ALREADY
      CString csPathFileName(((TE_VAR_IMAGE *) m_pConfig)->wszFileName);
      int nOrigWidth  = 0;
      int nOrigHeight = 0;

      GetImageSizeFromFile(csPathFileName, nOrigWidth, nOrigHeight);

      int nNewWidth   = nOrigWidth;
      int nNewHeight  = nOrigHeight; 

      bool bHasToBeAdapted = CalculateAdaptedImageSize(nOrigWidth, nOrigHeight, nNewWidth, nNewHeight);

      if (bHasToBeAdapted)
         return TE_IMAGE_DO_NOT_ADAPT;
      else
         return TE_IMAGE_FITS_ALREADY;
   }
}

CString CTemplateImageKeyword::GetImageName()
{
   CString csImageName(((TE_VAR_IMAGE *) m_pConfig)->wszFileName);
   StringManipulation::GetFilename(csImageName);
   return csImageName;
}

CString CTemplateImageKeyword::GetReplaceString()
{
   CString csPathFileName(((TE_VAR_IMAGE *) m_pConfig)->wszFileName);
   CString csFileName(csPathFileName);
   StringManipulation::GetFilename(csFileName);
   CString csReplace(_T(""));
   
   DWORD dwImageSizeHandling = ((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling;
   int nOrigWidth  = 0;
   int nOrigHeight = 0;

   if (csFileName != _T(""))
   {
      if (dwImageSizeHandling == TE_IMAGE_ADAPT_SIZE)
      {
         GetImageSizeFromFile(csPathFileName, nOrigWidth, nOrigHeight);
      }

      if ((dwImageSizeHandling == TE_IMAGE_ADAPT_SIZE) && (nOrigWidth > 0) && (nOrigHeight > 0))
      {
         // Resize image in browser, if necessary
         int nNewWidth  = nOrigWidth;
         int nNewHeight = nOrigHeight;

         CalculateAdaptedImageSize(nOrigWidth, nOrigHeight, nNewWidth, nNewHeight);

         _tprintf(_T("New Logo image size:      %d x %d\n"), nNewWidth, nNewHeight);

         csReplace.Format(_T("<img src=\"%s\" width=\"%d\" height=\"%d\" border=\"0\" alt=\"Image\">"), csFileName, nNewWidth, nNewHeight);
      }
      else
      {
         csReplace.Format(_T("<img src=\"%s\" border=\"0\" alt=\"Image\">"), csFileName);
      }
   }
   return csReplace;
}

CString CTemplateImageKeyword::GetStringRepresentation()
{
   DWORD dwImageSizeHandling = ((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling;
   WCHAR *wszFileName = ((TE_VAR_IMAGE *) m_pConfig)->wszFileName;

   CString csReturn;
#ifdef _UNICODE
   csReturn.Format(_T("%s;%d"), wszFileName, dwImageSizeHandling);
#else
   this will not compile; _UNICODE required;
#endif
   return csReturn;
}

bool CTemplateImageKeyword::ParseFromString(CString& csRepresentation)
{
   int iIdx = csRepresentation.Find(_T(";"));
   CString csFileName = csRepresentation.Left(iIdx);
   CString csSizeHandling = csRepresentation.Right(csRepresentation.GetLength() - iIdx - 1);

   if (csSizeHandling.GetLength() <= 0) // at least this must be specified, file name can be empty
      return false;

#ifdef _UNICODE
   WCHAR *wszFileName = ((TE_VAR_IMAGE *) m_pConfig)->wszFileName;
   wcsncpy(wszFileName, (LPCTSTR)csFileName, csFileName.GetLength());
   wszFileName[csFileName.GetLength()] = 0;
#else
   this will not compile; _UNICODE required;
#endif

   ((TE_VAR_IMAGE *) m_pConfig)->dwImageSizeHandling = _ttoi(csSizeHandling);

   return true;
}

void CTemplateImageKeyword::GetImageSizeFromFile(CString csFileName, int &nOrigWidth, int &nOrigHeight)
{
   if (csFileName.IsEmpty())
   {
      nOrigWidth  = 0;
      nOrigHeight = 0;
   }
   else
   {
      CImageStatic* pImgStatic = new CImageStatic();
      pImgStatic->ReadImage(csFileName);

      nOrigWidth  = pImgStatic->GetImageWidth();
      nOrigHeight = pImgStatic->GetImageHeight();
      _tprintf(_T("Original Logo image size: %d x %d\n"), nOrigWidth, nOrigHeight);

      if (pImgStatic != NULL)
         delete pImgStatic;
   }
}

bool CTemplateImageKeyword::CalculateAdaptedImageSize(int &nOrigWidth, int &nOrigHeight, int &nNewWidth, int &nNewHeight)
{
   // This method returns true, if the image has to be adapted - and false, if not.
   bool bHasToBeAdapted = false;

   if ((nOrigWidth < m_nMinWidth) || (nOrigHeight < m_nMinHeight))
   {
      bHasToBeAdapted = true;
      
      double fH = ((double) m_nMinWidth) / ((double) nOrigWidth);
      double fV = ((double) m_nMinHeight) / ((double) nOrigHeight);
      double f  = max(fV, fH);

      nNewWidth  = (int) (0.5f + f * ((double) nOrigWidth));
      nNewHeight = (int) (0.5f + f * ((double) nOrigHeight));
   }

   if ((nOrigWidth > m_nMaxWidth) || (nOrigHeight > m_nMaxHeight))
   {
      bHasToBeAdapted = true;
      
      double fH = ((double) m_nMaxWidth) / ((double) nOrigWidth);
      double fV = ((double) m_nMaxHeight) / ((double) nOrigHeight);
      double f  = min(fV, fH);

      nNewWidth  = (int) (0.5f + f * ((double) nOrigWidth));
      nNewHeight = (int) (0.5f + f * ((double) nOrigHeight));
   }

   return bHasToBeAdapted;
}

/* ************************************************* */

CTemplateIntegerKeyword::CTemplateIntegerKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName,
                                                 int nMinValue, int nMaxValue) 
   : CTemplateKeyword(tszName, tszFriendlyName),
     m_nMinValue(nMinValue), m_nMaxValue(nMaxValue)
{
   m_dwMinStructSize = sizeof TE_VAR_INTEGER;
   m_pConfig = (TE_VAR *) new char[sizeof TE_VAR_INTEGER];
   memset(m_pConfig, 0, sizeof TE_VAR_INTEGER);
   m_pConfig->dwSize = sizeof TE_VAR_INTEGER;
   m_pConfig->dwType = TE_TYPE_INTEGER;
#ifdef _UNICODE
   wcscpy(m_pConfig->wszVarName, tszName);
#else
   ::MultiByteToWideChar(CP_ACP, 0, tszName, -1, m_pConfig->wszVarName, 128);
#endif
}

void CTemplateIntegerKeyword::SetValue(int nValue)
{
   ((TE_VAR_INTEGER *) m_pConfig)->nValue = nValue;
}

int CTemplateIntegerKeyword::GetValue()
{
   return ((TE_VAR_INTEGER *) m_pConfig)->nValue;
}

CString CTemplateIntegerKeyword::GetReplaceString()
{
   CString csReplace;
   csReplace.Format(_T("%d"), GetValue());
   return csReplace;
}

CString CTemplateIntegerKeyword::GetStringRepresentation()
{
   return GetReplaceString();
}

bool CTemplateIntegerKeyword::ParseFromString(CString& csRepresentation)
{
   if (csRepresentation.GetLength() <= 0)
      return false;

   SetValue(_ttoi(csRepresentation));

   return true;
}

/* ************************************************* */

CTemplateColorKeyword::CTemplateColorKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName)
   : CTemplateKeyword(tszName, tszFriendlyName)
{
   m_dwMinStructSize = sizeof TE_VAR_COLOR;
   m_pConfig = (TE_VAR *) new char[sizeof TE_VAR_COLOR];
   memset(m_pConfig, 0, sizeof TE_VAR_COLOR);
   m_pConfig->dwSize = sizeof TE_VAR_COLOR;
   m_pConfig->dwType = TE_TYPE_COLOR;
#ifdef _UNICODE
   wcscpy(m_pConfig->wszVarName, tszName);
#else
   ::MultiByteToWideChar(CP_ACP, 0, tszName, -1, m_pConfig->wszVarName, 128);
#endif
}

void CTemplateColorKeyword::SetColor(COLORREF color)
{
   ((TE_VAR_COLOR *) m_pConfig)->rgbColor = color;
}

COLORREF CTemplateColorKeyword::GetColor()
{
   return ((TE_VAR_COLOR *) m_pConfig)->rgbColor;
}

CString CTemplateColorKeyword::GetReplaceString()
{
   CString csReplace;
   COLORREF color = GetColor();
   // COLORREF is BGR, we need RGB
   DWORD rgbColor = ((color & 0x00ff0000) >> 16) | (color & 0x0000ff00) | ((color & 0x000000ff) << 16);
   csReplace.Format(_T("#%06x"), rgbColor);
   return csReplace;
}

CString CTemplateColorKeyword::GetStringRepresentation()
{
   return GetReplaceString();
}

bool CTemplateColorKeyword::ParseFromString(CString& csRepresentation)
{
   if (csRepresentation.GetLength() <= 6)
      return false;

   _TCHAR *tszDummy = 0;
   int r = (int)_tcstol(csRepresentation.Mid(1, 2), &tszDummy, 16);
   int g = (int)_tcstol(csRepresentation.Mid(3, 2), &tszDummy, 16);
   int b = (int)_tcstol(csRepresentation.Mid(5, 2), &tszDummy, 16);

   int bgr = b << 16 | g << 8 | r;

   SetColor((COLORREF)bgr);

   return true;
}

/* ************************************************* */

CTemplateGeneralKeyword::CTemplateGeneralKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName)
   : CTemplateKeyword(tszName, tszFriendlyName)
{
   m_dwMinStructSize = sizeof TE_VAR_GENERAL;
   m_pConfig = (TE_VAR *) new char[sizeof TE_VAR_GENERAL];
   memset(m_pConfig, 0, sizeof TE_VAR_GENERAL);
   m_pConfig->dwSize = sizeof TE_VAR_GENERAL;
   m_pConfig->dwType = TE_TYPE_GENERAL;
#ifdef _UNICODE
   wcscpy(m_pConfig->wszVarName, tszName);
#else
   ::MultiByteToWideChar(CP_ACP, 0, tszName, -1, m_pConfig->wszVarName, 128);
#endif
}

void CTemplateGeneralKeyword::SetGeneral(const _TCHAR *tszGeneral)
{
   if (tszGeneral == NULL)
      return;

   int nLen = _tcslen(tszGeneral);
   int nNewSize = (sizeof TE_VAR_GENERAL) + 2*nLen;
   TE_VAR_GENERAL *pNewConfig = (TE_VAR_GENERAL *) new char[nNewSize]; // 0 char is already in TE_VAR_GENERAL!
   memcpy(pNewConfig, m_pConfig, sizeof TE_VAR_GENERAL);
#ifdef _UNICODE
   wcscpy(pNewConfig->wszGeneral, tszGeneral);
#else
   MultiByteToWideChar(CP_ACP, 0, tszGeneral, -1, pNewConfig->wszGeneral, nLen + 1);
#endif
   pNewConfig->dwSize = nNewSize;
   delete[] m_pConfig;
   m_pConfig = pNewConfig;
}

UINT CTemplateGeneralKeyword::GetGeneral(_TCHAR *tszGeneral, DWORD *pdwSize)
{
   RETURN_WSTRING(((TE_VAR_GENERAL *) m_pConfig)->wszGeneral, tszGeneral, pdwSize);
}

CString CTemplateGeneralKeyword::GetReplaceString()
{
   CString csReplace(((TE_VAR_GENERAL *) m_pConfig)->wszGeneral);
   return csReplace;
}

CString CTemplateGeneralKeyword::GetStringRepresentation()
{
   return GetReplaceString();
}

bool CTemplateGeneralKeyword::ParseFromString(CString& csRepresentation)
{
   SetGeneral(csRepresentation);

   return true;
}

/* ************************************************* */

CTemplateTextKeyword::CTemplateTextKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName,
                                           int nMaxLength) 
   : CTemplateKeyword(tszName, tszFriendlyName),
     m_nMaxLength(nMaxLength) 
{
   m_dwMinStructSize = sizeof TE_VAR_TEXT;
   m_pConfig = (TE_VAR *) new char[sizeof TE_VAR_TEXT];
   memset(m_pConfig, 0, sizeof TE_VAR_TEXT);
   m_pConfig->dwSize = sizeof TE_VAR_TEXT;
   m_pConfig->dwType = TE_TYPE_TEXT;
#ifdef _UNICODE
   wcscpy(m_pConfig->wszVarName, tszName);
#else
   ::MultiByteToWideChar(CP_ACP, 0, tszName, -1, m_pConfig->wszVarName, 128);
#endif
}

void CTemplateTextKeyword::SetText(const _TCHAR *tszText)
{
   if (tszText == NULL)
      return;

   int nLen = _tcslen(tszText);
   int nNewSize = (sizeof TE_VAR_TEXT) + 2*nLen;
   TE_VAR_TEXT *pNewConfig = (TE_VAR_TEXT *) new char[nNewSize]; // 0 char is already in TE_VAR_TEXT!
   memcpy(pNewConfig, m_pConfig, sizeof TE_VAR_TEXT);
#ifdef _UNICODE
   wcscpy(pNewConfig->wszText, tszText);
#else
   ::MultiByteToWideChar(CP_ACP, 0, tszText, -1, pNewConfig->wszText, nLen + 1);
#endif
   pNewConfig->dwSize = nNewSize;
   delete[] m_pConfig;
   m_pConfig = pNewConfig;
}

UINT CTemplateTextKeyword::GetText(_TCHAR *tszText, DWORD *pdwText)
{
   RETURN_WSTRING(((TE_VAR_TEXT *) m_pConfig)->wszText, tszText, pdwText);
}

CString CTemplateTextKeyword::GetReplaceString()
{
   CString csReplace(((TE_VAR_TEXT *) m_pConfig)->wszText);
   return csReplace;
}


CString CTemplateTextKeyword::GetStringRepresentation()
{
   return GetReplaceString();
}

bool CTemplateTextKeyword::ParseFromString(CString& csRepresentation)
{
   SetText(csRepresentation);

   return true;
}

int AddOptionsFromString(CArray<CString, CString> &caOptions, const _TCHAR *tszString)
{
   int nCount = 0;
   _TCHAR *tszTemp = _tcsdup(tszString);
   _TCHAR *tszPointer = tszTemp;
   _TCHAR *tszNext = _tcschr(tszPointer, _T('|'));
   while (tszNext)
   {
      ++nCount;
      *tszNext = 0; //_T(0);
      caOptions.Add(CString(tszPointer));
      *tszNext = _T('|');

      tszPointer = tszNext + 1;
      tszNext = _tcschr(tszPointer, _T('|'));
   }

   ++nCount;
   caOptions.Add(CString(tszPointer));

#ifdef _DEBUG
   for (int i=0; i<nCount; ++i)
      _tprintf(_T("Option: %s\n"), caOptions[i]);
#endif

   delete[] tszTemp;
   return nCount;
}

CTemplateSelectKeyword::CTemplateSelectKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName, 
                                               const _TCHAR *tszVisibleOptions,
                                               const _TCHAR *tszOptions)
                                               : CTemplateTextKeyword(tszName, tszFriendlyName, 0)
{
   // m_pConfig inherited from CTemplateTextKeyword
   m_pConfig->dwType = TE_TYPE_SELECT;

   // Parse the options
   int nVisibleOptionCount = AddOptionsFromString(m_caVisibleOptions, tszVisibleOptions);
   int nOptionCount = AddOptionsFromString(m_caOptions, tszOptions);

   m_nOptionCount = min(nVisibleOptionCount, nOptionCount);
}
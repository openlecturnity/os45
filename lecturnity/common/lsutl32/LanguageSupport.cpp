#include "stdafx.h"
#include "LanguageSupport.h"

#include "KeyGenerator.h"
#include "lutils.h"
#include "resource.h"

/* REVIEW UK
 * SaveToolkitResourceLanguageFile() should have a documentation showing what it is doing
 */

CLanguageSupport::CLanguageSupport(void)
{
}

CLanguageSupport::~CLanguageSupport(void)
{
}

HRESULT CLanguageSupport::GetLanguageId(LCID& languageID)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   HRESULT hResult = S_OK;

   _TCHAR tszLanguageCode[128];
   bool ret = KerberokHandler::GetLanguageCode(tszLanguageCode);

   if (!ret)
      hResult = E_LS_WRONG_REGISTRY;

   LCID threadLocaleId;
   if (SUCCEEDED(hResult))
   {
      CString csLanguageCode = tszLanguageCode;
  
      if (csLanguageCode == _T("de"))
         threadLocaleId = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT);
      else if (csLanguageCode == _T("en"))
         threadLocaleId = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
      else if (csLanguageCode == _T("vi"))
         threadLocaleId = MAKELCID(MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT), SORT_DEFAULT);
      else
      {
         // have a default in any case
         threadLocaleId = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
         hResult = E_LS_LANGUAGE_NOT_SUPPORTED;
      }
	  languageID = threadLocaleId;
   }
	return hResult;
}

HRESULT CLanguageSupport::SetThreadLanguage(void)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   HRESULT hResult = S_OK;

   _TCHAR tszLanguageCode[128];
   bool ret = KerberokHandler::GetLanguageCode(tszLanguageCode);

   if (!ret)
      hResult = E_LS_WRONG_REGISTRY;

   LCID threadLocaleId;
   if (SUCCEEDED(hResult))
   {
      CString csLanguageCode = tszLanguageCode;
  
      if (csLanguageCode == _T("de"))
         threadLocaleId = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT);
      else if (csLanguageCode == _T("en"))
         threadLocaleId = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
      else if (csLanguageCode == _T("vi"))
         threadLocaleId = MAKELCID(MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT), SORT_DEFAULT);
      else
      {
         // have a default in any case
         threadLocaleId = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
         hResult = E_LS_LANGUAGE_NOT_SUPPORTED;
      }
   }

   if (SUCCEEDED(hResult))
   {
       if(LMisc::IsWindowsVistaOrHigher())
	   {
         typedef LANGID (WINAPI *FP_SetThreadUILanguage)(LANGID LangId);
		   HMODULE hKernel32 = GetModuleHandle(_T("Kernel32.dll"));
		   FARPROC pFn = GetProcAddress(hKernel32, "SetThreadUILanguage");
		   FP_SetThreadUILanguage pSetThreadUILanguage = (FP_SetThreadUILanguage)pFn;
		   LANGID langid = pSetThreadUILanguage(threadLocaleId);
		   if(threadLocaleId != langid)
			   hResult = E_LS_THREAD_LOCALE;	
		   
	   }
	   else
	   { 
         BOOL bRet = ::SetThreadLocale(threadLocaleId);
		   if (!bRet)
			   hResult = E_LS_THREAD_LOCALE;
	   }
   }

   return hResult;
}

#ifndef IMC_MAX_PATH
#define IMC_MAX_PATH       4096
#endif

HRESULT CLanguageSupport::StartManual()
{
   HRESULT hr = S_OK;

   unsigned long maxLength = IMC_MAX_PATH;
   TCHAR szString[IMC_MAX_PATH];

   bool success = LRegistry::ReadSettingsString(_T(""), _T("InstallDir"), szString, &maxLength, NULL);
   if (!success)
      hr = E_FAIL;
   
   _TCHAR tszLanguageCode[128];
   CString pdfName;
   if (SUCCEEDED(hr))
   {
      pdfName = szString;
      if (pdfName[pdfName.GetLength()-1] != _T('\\'))
         pdfName += _T("\\");

      bool ret = KerberokHandler::GetLanguageCode(tszLanguageCode);

      if (!ret)
         hr = E_FAIL;
   }

   HANDLE hFileObj = NULL;
   if (SUCCEEDED(hr))
   {
      CString csLanguageCode = tszLanguageCode;

      if (csLanguageCode == _T("de"))
         pdfName += _T("manual_de.pdf");
      else if (csLanguageCode == _T("en"))
         pdfName += _T("manual_en.pdf");
      else
         pdfName += _T("manual_en.pdf");

      CString csMessage = _T("");
      
      hFileObj = CreateFile(pdfName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);  
      if (hFileObj == INVALID_HANDLE_VALUE) 
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
   {
      CloseHandle(hFileObj);
      HINSTANCE hResult = ::ShellExecute(NULL, _T("open"), pdfName, NULL, NULL, SW_SHOWNORMAL);
      if ((int) hResult <= 32)
         hr = E_FAIL;
   }    

   return hr;
}

bool CLanguageSupport::ManualExists()
{
   bool bExists = false;

   DWORD dwLen = IMC_MAX_PATH;
   _TCHAR tszInstallDir[IMC_MAX_PATH];
   bool success = LRegistry::ReadSettingsString(_T(""), _T("InstallDir"), tszInstallDir, &dwLen, NULL);
   if (success)
   {
      CString csManualName = tszInstallDir;
      if (csManualName[csManualName.GetLength() - 1] != _T('\\'))
         csManualName += _T('\\');

      _TCHAR tszLanguageCode[128];
      bool ret = KerberokHandler::GetLanguageCode(tszLanguageCode);

      CString csLanguageCode = tszLanguageCode;
      if (ret)
      {
         if (csLanguageCode == _T("de"))
            csManualName += _T("manual_de.pdf");
         else if (csLanguageCode == _T("en"))
            csManualName += _T("manual_en.pdf");
         else
            csManualName += _T("manual_en.pdf");


         if (_taccess(csManualName, 04) == 0)
            bExists = true;
      }
   }


   return bExists;
}

bool CLanguageSupport::SaveToolkitResourceLanguageFile(LPCTSTR csLanguage, LPCTSTR sAppName, _TCHAR *tszFileName)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
   if (::GetTempPath(MAX_PATH, tszFileName) == 0) 
   {
      ASSERT(false);
      return false;
   }

   // Set Language file
   _stprintf(tszFileName,_T("%sToolkitProResource%s.xml"), tszFileName, sAppName );

   DeleteFile(tszFileName);
   HANDLE hFile = CreateFile(tszFileName, GENERIC_WRITE, FILE_SHARE_READ,
         NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile == INVALID_HANDLE_VALUE)
   {
#ifdef _DEBUG
	   LPVOID lpMsgBuf = 0;

	   ::FormatMessage(
		   FORMAT_MESSAGE_ALLOCATE_BUFFER |
		   FORMAT_MESSAGE_FROM_SYSTEM |
		   FORMAT_MESSAGE_IGNORE_INSERTS,
		   NULL,
         GetLastError(),
		   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		   (LPTSTR) &lpMsgBuf,
		   0,
		   NULL
		   );

	   CString csMessageError((LPCTSTR)lpMsgBuf);

	   // Free the buffer.
	   ::LocalFree( lpMsgBuf );

      CString strStaticMsg;
      strStaticMsg.LoadString(IDS_ERR_TOOLKIT_LANG_FILE_MISSING);
      AfxMessageBox(strStaticMsg + csMessageError, MB_ICONERROR);
#endif
      return false;
   }

   HINSTANCE hInst = AfxGetInstanceHandle();
   if (hInst == NULL)
   {
      ASSERT(false);
	   return false;
   }
   HRSRC hResource = ::FindResource(hInst, MAKEINTRESOURCE(IDR_LANGUAGE_EN), _T("LANG"));
   if (hResource == NULL)
   {
      ASSERT(false);
	   return false;
   }

   HGLOBAL hGlobal = LoadResource(hInst, hResource);
   if (hGlobal == NULL)
   {
      ASSERT(false);
	   return false;
   }

   if ( _tcscmp(csLanguage, _T("de")) == 0 )
   {
      hResource = ::FindResource(hInst, MAKEINTRESOURCE(IDR_LANGUAGE_DE), _T("LANG"));
   }else if (_tcscmp(csLanguage, _T("vi")) == 0 )
   {
      hResource = ::FindResource(hInst, MAKEINTRESOURCE(IDR_LANGUAGE_VI), _T("LANG"));
   }
   if (!hResource)
   {
      ASSERT(false);
      return false;
   }

   DWORD dwFileSize = ::SizeofResource(hInst, hResource);
   if (dwFileSize <= 0)
   {
      ASSERT(false);
      return false;
   }
   const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
   if (pResourceData == NULL)
   {
      ASSERT(false);
      return false;
   }
   CString csFileContent;
   HGLOBAL m_hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
   if (m_hBuffer == NULL)
   {
      ASSERT(false);
      return false;
   }
   void* pBuffer = ::GlobalLock(m_hBuffer);
   if (pBuffer == NULL )
   {
      ::GlobalFree(m_hBuffer);
      m_hBuffer = NULL;
      ASSERT(false);
      return false;
   }

   CopyMemory(pBuffer, pResourceData, dwFileSize);

   CFile file(hFile);
   file.Write(pBuffer, dwFileSize);
   file.Close();

   ::GlobalUnlock(m_hBuffer);
   ::GlobalFree(m_hBuffer);
   m_hBuffer = NULL;
   
   return true;
}
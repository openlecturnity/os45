#ifndef __LANGUAGESUPPORT_H__
#define __LANGUAGESUPPORT_H__

// Instead of including "StdAfx.h" (like in the other header files):
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif

#define E_LS_WRONG_REGISTRY  _HRESULT_TYPEDEF_(0x80560001L)
#define E_LS_LANGUAGE_NOT_SUPPORTED  _HRESULT_TYPEDEF_(0x80560002L)
#define E_LS_THREAD_LOCALE  _HRESULT_TYPEDEF_(0x80560003L)

class LSUTL32_EXPORT CLanguageSupport
{
public:
   CLanguageSupport(void);

public:
   ~CLanguageSupport(void);

   static HRESULT SetThreadLanguage(void);
   static HRESULT GetLanguageId(LCID& languageID);
   static HRESULT StartManual();
   static bool ManualExists();
   static bool SaveToolkitResourceLanguageFile(LPCTSTR sLanguage, LPCTSTR sAppName, _TCHAR *pFileName);
   
public:
};

#endif
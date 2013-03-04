#ifndef __LANGUAGESUPPORT_H__
#define __LANGUAGESUPPORT_H__

// Instead of including "StdAfx.h" (like in the other header files):
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else
   #define LSUTL32_EXPORT __declspec(dllimport)
#endif

#define E_LS_WRONG_REGISTRY  _HRESULT_TYPEDEF_(0x80560001L)
#define E_LS_LANGUAGE_NOT_SUPPORTED  _HRESULT_TYPEDEF_(0x80560002L)
#define E_LS_TREAD_LOCALE  _HRESULT_TYPEDEF_(0x80560003L)

class LSUTL32_EXPORT CLanguageSupport
{
public:
   CLanguageSupport(void);

public:
   ~CLanguageSupport(void);

   static HRESULT SetThreadLanguage(void);
   static HRESULT GetLanguageId(LCID& languageID);
   
public:
};

#endif
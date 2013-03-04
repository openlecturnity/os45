// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__9BA5264D_52FE_4464_8A15_8BD7846FC7FA__INCLUDED_)
#define AFX_STDAFX_H__9BA5264D_52FE_4464_8A15_8BD7846FC7FA__INCLUDED_

#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0500
#endif

#pragma warning(disable: 4018 4244 4996)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE-Klassen
#include <afxodlgs.h>       // MFC OLE-Dialogfeldklassen
#include <afxdisp.h>        // MFC Automatisierungsklassen
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC-Datenbankklassen
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO-Datenbankklassen
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <gdiplus.h> // GDI+

#include <afxtempl.h> // CArray
#include <set> // set

#include <vfw.h> // AVIFile*
#include <crtdbg.h> // _RPT // debug output

#include <atlbase.h> // all 3 for DirectShow
#include <comutil.h> // _bstr_
#include <qedit.h>
#include <streams.h>
#include <dvdmedia.h> // VIDEOINFOHEADER2

/* ************************************************* */

enum
{
   TEMPLATE_RESET_NORESET = 0,
   TEMPLATE_RESET_SYSTEMDEFAULT,
   TEMPLATE_RESET_TEMPLATEDEFAULT
};

/* ************************************************* */

#define RETURN_STRING(source, target, size) \
   if (##size == NULL) \
      return E_INVALIDARG; \
   if (##target == NULL) \
   {\
      *##size = _tcslen(##source) + 1;\
      return S_OK;\
   }\
   if (*##size <= _tcslen(##source))\
      return E_INVALIDARG;\
   _tcscpy(##target, ##source);\
   return S_OK;

#ifdef _UNICODE

   #define RETURN_WSTRING(source, target, size) \
      if (##size == NULL) \
         return E_INVALIDARG; \
      if (##target == NULL) \
      { \
         *##size = wcslen(##source) + 1; \
         return S_OK; \
      } \
      if (*##size <= wcslen(##source)) \
         return E_INVALIDARG; \
      wcscpy(##target, ##source); \
      return S_OK;

#else

   #define RETURN_WSTRING(source, target, size) \
      if (##size == NULL) \
         return E_INVALIDARG; \
      if (##target == NULL) \
      { \
         *##size = wcslen(##source) + 1; \
         return S_OK; \
      } \
      if (*##size <= wcslen(##source)) \
         return E_INVALIDARG; \
      ::WideCharToMultiByte(CP_ACP, 0, ##source, -1, ##target, *##size, NULL, NULL); \
      return S_OK;

#endif


#define HEXTOINT(x) (BYTE) ((((_TCHAR) x) < 65 ? ((_TCHAR) x) - 48 : ((_TCHAR) x) - 55))
#define RGB2BGR(x) (COLORREF) (((x & 0x00ff0000) >> 16) | (x & 0x0000ff00) | ((x & 0x000000ff) << 16))
#define BGR2RGB(x) (DWORD) RGB2BGR(x)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_STDAFX_H__9BA5264D_52FE_4464_8A15_8BD7846FC7FA__INCLUDED_)

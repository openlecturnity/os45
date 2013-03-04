// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__CDC72F2B_574D_4046_86B7_E57F4910FE44__INCLUDED_)
#define AFX_STDAFX_H__CDC72F2B_574D_4046_86B7_E57F4910FE44__INCLUDED_

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

#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif

#define INITGUID
//#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include <gdiplus.h>
#include "XTToolkitPro.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

// {A65B8071-3BFE-4213-9A5B-491DA4461CA7}
static const GUID CLSID_DxDiagProvider = 
{ 0xA65B8071, 0x3BFE, 0x4213, { 0x9A, 0x5B, 0x49, 0x1D, 0xA4, 0x46, 0x1C, 0xA7 } };

// {9C6B4CB0-23F8-49CC-A3ED-45A55000A6D2}
static const GUID IID_IDxDiagProvider =
{ 0x9C6B4CB0, 0x23F8, 0x49CC, { 0xA3, 0xED, 0x45, 0xA5, 0x50, 0x00, 0xA6, 0xD2 } };


#endif // !defined(AFX_STDAFX_H__CDC72F2B_574D_4046_86B7_E57F4910FE44__INCLUDED_)

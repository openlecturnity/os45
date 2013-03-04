// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#pragma message "Fail on Build... This project is dead. Use 'Uploader'...

#if !defined(AFX_STDAFX_H__74B36C89_44AA_47EA_86FD_54595965B021__INCLUDED_)
#define AFX_STDAFX_H__74B36C89_44AA_47EA_86FD_54595965B021__INCLUDED_

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
#include <afxdisp.h>        // MFC Automatisierungsklassen
#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxctl.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#include <io.h>
#include <gdiplus.h>
#include <qedit.h>
#include <atlbase.h> // CComQIPtr, ...
#include <afxtempl.h> // CList, CArray
#include <comdef.h> // _bstr_t

#include <XTToolkitPro.h> 

#endif // !defined(AFX_STDAFX_H__74B36C89_44AA_47EA_86FD_54595965B021__INCLUDED_)

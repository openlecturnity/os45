// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden.
#endif

// Ändern Sie folgende Definitionen für Plattformen, die älter als die unten angegebenen sind.
// In MSDN finden Sie die neuesten Informationen über die entsprechenden Werte für die unterschiedlichen Plattformen.
#ifndef WINVER				// Lassen Sie die Verwendung spezifischer Features von Windows XP oder später zu.
#define WINVER 0x0501		// Ändern Sie dies in den geeigneten Wert für andere Versionen von Windows.
#endif

#ifndef _WIN32_WINNT		// Lassen Sie die Verwendung spezifischer Features von Windows XP oder später zu.                   
#define _WIN32_WINNT 0x0501	// Ändern Sie dies in den geeigneten Wert für andere Versionen von Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Lassen Sie die Verwendung spezifischer Features von Windows 98 oder später zu.
#define _WIN32_WINDOWS 0x0410 // Ändern Sie dies in den geeigneten Wert für Windows Me oder höher.
#endif

#ifndef _WIN32_IE			// Lassen Sie die Verwendung spezifischer Features von IE 6.0 oder später zu.
#define _WIN32_IE 0x0600	// Ändern Sie dies in den geeigneten Wert für andere Versionen von IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// Einige CString-Konstruktoren sind explizit.

#include <afxwin.h>         // MFC-Kern- und Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE-Klassen
#include <afxodlgs.h>       // MFC OLE-Dialogfeldklassen
#include <afxdisp.h>        // MFC-Automatisierungsklassen
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC-ODBC-Datenbankklassen
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC-DAO-Datenbankklassen
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für allgemeine Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxpriv.h> 

#include "tchar.h"
#include <windows.h>
#include <afxdlgs.h>
#include <afxmt.h>
#include <afxtempl.h>



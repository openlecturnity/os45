#pragma once

#pragma warning(disable: 4018 4244 4267 4311 4312 4996)

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header ausschließen
#endif

// Ändern Sie folgende Definitionen für Plattformen, die älter als die unten angegebenen sind.
// Unter MSDN finden Sie die neuesten Informationen über die entsprechenden Werte für die unterschiedlichen Plattformen.
#ifndef WINVER				// Lassen Sie die Verwendung spezifischer Features von Windows XP oder später zu.
#define WINVER 0x0501		// Ändern Sie den entsprechenden Wert, um auf andere Versionen von Windows abzuzielen.
#endif

#ifndef _WIN32_WINNT		// Lassen Sie die Verwendung spezifischer Features von Windows XP oder später zu.                   
#define _WIN32_WINNT 0x0501	// Ändern Sie den entsprechenden Wert, um auf andere Versionen von Windows abzuzielen.
#endif						

#ifndef _WIN32_WINDOWS		// Lassen Sie die Verwendung spezifischer Features von Windows 98 oder später zu.
#define _WIN32_WINDOWS 0x0410 // Ändern Sie den entsprechenden Wert, um auf mindestens Windows Me abzuzielen.
#endif

#ifndef _WIN32_IE			// Lassen Sie die Verwendung spezifischer Features von IE 6.0 oder später zu.
#define _WIN32_IE 0x0600	// Ändern Sie den entsprechenden Wert, um auf andere Versionen von IE abzuzielen.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// einige CString-Konstruktoren sind explizit

// Deaktiviert das Ausblenden einiger häufiger und oft ignorierter Warnungen durch MFC
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxdisp.h>        // MFC-Automatisierungsklassen

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für allgemeine Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

// Assistant
#if _MSC_VER > 1200 //MFC 7.0
#include <..\src\mfc\afximpl.h>	// MFC Global data
#else
#include <..\src\afximpl.h>	    // MFC Global data
#endif

#include <gdiplus.h>
#include <XTToolkitPro.h>

// Additionally for Editor
#include <io.h>
#include <qedit.h>
#include <atlbase.h> // CComQIPtr, ...
#include <afxtempl.h> // CList, CArray
#include <comdef.h> // _bstr_t
#include <afxctl.h>


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

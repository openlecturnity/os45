#if !defined(AFX_STDAFX_H__D28BA3C5_326D_4D01_93E0_4549BA19704A__INCLUDED_)
#define AFX_STDAFX_H__D28BA3C5_326D_4D01_93E0_4549BA19704A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxdisp.h>        // MFC Automatisierungsklassen
#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <io.h> // _taccess()
#include <mmsystem.h> // timeGetTime()
#include <tchar.h>
#include <winsock2.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_STDAFX_H__D28BA3C5_326D_4D01_93E0_4549BA19704A__INCLUDED_)

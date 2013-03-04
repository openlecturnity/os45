// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once

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

#define WIN32_LEAN_AND_MEAN		// Selten verwendete Teile der Windows-Header nicht einbinden.
// Windows-Headerdateien:
#include <afx.h>
#include <afxwin.h>
#include <afxext.h>         // MFC-Erweiterungen
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für allgemeine Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>


// TODO: Hier auf zusätzliche Header, die das Programm erfordert, verweisen.

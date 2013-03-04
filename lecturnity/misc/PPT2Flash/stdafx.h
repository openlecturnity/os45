// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once

#pragma warning(disable: 4018 4244 4996 4267)

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif	


#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
#endif
					

#define WIN32_LEAN_AND_MEAN		// Selten verwendete Teile der Windows-Header nicht einbinden.

#include <afxwin.h>         // MFC core and standard components
#include <stdio.h>
#include <tchar.h>

#include <string.h>

#include <cstringt.h>
#include <atlstr.h>

#include <gdiplus.h>


// TODO: Hier auf zusätzliche Header, die das Programm erfordert, verweisen.

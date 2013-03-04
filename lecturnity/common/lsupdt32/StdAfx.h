#ifndef __LSUPDT32_STAFX_H__
#define __LSUPDT32_STAFX_H__

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

// MFC Support
#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxcmn.h>  // z.B. CToolTipCtrl
#include <afxtempl.h> // CArray

//#include <windows.h>
#include <winuser.h>
#include <tchar.h>

// Internet support
#include <wininet.h>

#include <comdef.h>
#include <gdiplus.h>

// LECTURNITY Utilities
#include "lutils.h"

#endif

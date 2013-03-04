#ifndef __STDAFX_H__8ECC66B8_83D8_4eac_8883_77B29DF756EB
#define __STDAFX_H__8ECC66B8_83D8_4eac_8883_77B29DF756EB

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

#include <afxwin.h>
#include <afxext.h>         // MFC-Erweiterungen

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE-Klassen
#include <afxodlgs.h>       // MFC OLE-Dialogfeldklassen
#include <afxdisp.h>        // MFC Automatisierungsklassen
#endif // _AFX_NO_OLE_SUPPORT

#include <afxcmn.h>

#include <gdiplus.h>
#endif // __STDAFX_H__8ECC66B8_83D8_4eac_8883_77B29DF756EB
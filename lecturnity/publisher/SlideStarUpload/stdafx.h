// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D5F58827_03F0_435D_ACB4_5AEDC138A097__INCLUDED_)
#define AFX_STDAFX_H__D5F58827_03F0_435D_ACB4_5AEDC138A097__INCLUDED_

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

#define VC_EXTRALEAN      // Exclude rarely-used stuff from Windows headers
#define Y_BOTTOM 44
#define BORDER 16
#define SLIDESTARUPLOAD_KEY _T("Software\\imc AG\\LECTURNITY\\Publisher\\SlideStarUpload\\")

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>      // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D5F58827_03F0_435D_ACB4_5AEDC138A097__INCLUDED_)

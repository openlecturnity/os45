#if !defined(AFX_STDAFX_H__8DA4421F_4D2A_40D7_AFF7_5459BA442D1C__INCLUDED_)
#define AFX_STDAFX_H__8DA4421F_4D2A_40D7_AFF7_5459BA442D1C__INCLUDED_

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


// Fügen Sie hier Ihre Header-Dateien ein
#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden


#include <afxwin.h>
#include <afxtempl.h> // needed for templated classes??
#include <tchar.h>
#include <crtdbg.h>
#include <olectl.h>
#include <streams.h>
#include <gdiplus.h>
#include <atlbase.h> // CComPtr
#include <qedit.h> // IMediaDet
#include <vfw.h> // AviFile functions


#endif // !defined(AFX_STDAFX_H__8DA4421F_4D2A_40D7_AFF7_5459BA442D1C__INCLUDED_)

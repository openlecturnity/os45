// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__CFD8D08E_B640_4575_9D6A_3D35216C389D__INCLUDED_)
#define AFX_STDAFX_H__CFD8D08E_B640_4575_9D6A_3D35216C389D__INCLUDED_

#pragma warning(disable: 4018 4244 4996)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Fügen Sie hier Ihre Header-Dateien ein
#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

#include <windows.h>

//#include <afxwin.h> // kann nicht verwendet werden, da es DllMain umbiegt

#include <tchar.h> // needs to be included before dshow.h (since then?)
#include <dshow.h> // AM_MEDIA_TYPE
#include <mtype.h> // Create- und DeleteMediaType (aus DXSDK/BaseClasses)
#include <crtdbg.h> // _ASSERT
#include <io.h> // _access
#include <atlbase.h> // CComPtr
#include <qedit.h> // IMediaDet
#include <comdef.h> // BSTR, _bstr_t
#include <iostream> // std::cin
#include <process.h> // _beginthread
#include "ladfilter.h" // IID_ILadParser


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt zusätzliche Deklarationen unmittelbar vor der vorherigen Zeile ein.

#endif // !defined(AFX_STDAFX_H__CFD8D08E_B640_4575_9D6A_3D35216C389D__INCLUDED_)

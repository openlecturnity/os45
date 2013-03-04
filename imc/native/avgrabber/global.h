#ifndef __GLOBAL_H
#define __GLOBAL_H

#pragma warning(disable: 4786 4018 4244 4996)

#ifdef AVGRABBER_DLL
   #define AVGRABBER_EXPORT   __declspec(dllexport)
#else
   #define AVGRABBER_EXPORT   __declspec(dllimport)
#endif

#ifdef AVGRABBER_TEST
   #undef AVGRABBER_EXPORT
   #define AVGRABBER_EXPORT
#endif


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif	


#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500	// Change this to the appropriate value to target IE 5.0 or later.
#endif

// Fügen Sie hier Ihre Header-Dateien ein
#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

#include <afxwin.h>

extern HMODULE g_hDllInstance;

//#include <windows.h>
#include <wingdi.h>
#include <vfw.h>
#include <Sys/timeb.h>
#include <math.h>
#include <process.h>
#include <signal.h>
#include <io.h>
//#include <qedit.h>
#include <time.h>

#include <comdef.h>
#include <tchar.h>
#include "objects.h" // DrawSdk, this include should be placed before std::.. includes

#include <iostream>
#include <list>
#include <vector>
//#include <string>
#include <map>

#include "localizer.h"
#include "exceptions.h"
#include "AviStructs.h"
#include "RiffFile.h"
#include "VideoFile.h"
#include "MyAudio.h"
#include "VideoCapturer.h"
#include "audiomixer.h"
#include "cpumeter.h"
#include "iohook.h" // NMouseHook/iohook DLL
#include "NewDrawWindow.h"
#include "rectkeeper.h"
#include "ScreenCapturer.h"
#include "wdmcapturer.h"
#include "hwaccelmanager.h"
#include "CaptureRectChooser.h"
#include "samplecallback.h"
#include "avgrabber.h"


#include <XTToolkitPro.h>

#endif

// changeurl.h : Haupt-Header-Datei für die Anwendung CHANGEURL
//

#if !defined(AFX_CHANGEURL_H__21EDD357_F0D8_4B16_841B_9E7AAA2E5A31__INCLUDED_)
#define AFX_CHANGEURL_H__21EDD357_F0D8_4B16_841B_9E7AAA2E5A31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CChangeurlApp:
// Siehe changeurl.cpp für die Implementierung dieser Klasse
//

class CChangeurlApp : public CWinApp
{
public:
	CChangeurlApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CChangeurlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CChangeurlApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_CHANGEURL_H__21EDD357_F0D8_4B16_841B_9E7AAA2E5A31__INCLUDED_)

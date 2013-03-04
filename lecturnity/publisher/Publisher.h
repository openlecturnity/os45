// Publisher.h : Haupt-Header-Datei für die Anwendung PUBLISHER
//

#if !defined(AFX_PUBLISHER_H__0370FE8F_9DF0_4A4A_9C0F_63624F7EB51A__INCLUDED_)
#define AFX_PUBLISHER_H__0370FE8F_9DF0_4A4A_9C0F_63624F7EB51A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CPublisherApp:
// Siehe Publisher.cpp für die Implementierung dieser Klasse
//

class CPublisherApp : public CWinApp
{
public:
	CPublisherApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPublisherApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung
	//{{AFX_MSG(CPublisherApp)
	afx_msg void OnAppAbout();
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PUBLISHER_H__0370FE8F_9DF0_4A4A_9C0F_63624F7EB51A__INCLUDED_)

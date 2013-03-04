// LmdManip.h : Haupt-Header-Datei für die Anwendung LMDMANIP
//

#if !defined(AFX_LMDMANIP_H__673264EF_F797_4C31_96EF_135013B78DD9__INCLUDED_)
#define AFX_LMDMANIP_H__673264EF_F797_4C31_96EF_135013B78DD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CLmdManipApp:
// Siehe LmdManip.cpp für die Implementierung dieser Klasse
//

class CLmdManipApp : public CWinApp
{
public:
	CLmdManipApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CLmdManipApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CLmdManipApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_LMDMANIP_H__673264EF_F797_4C31_96EF_135013B78DD9__INCLUDED_)

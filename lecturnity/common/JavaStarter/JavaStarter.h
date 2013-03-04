#if !defined(AFX_JAVASTARTER_H__B061EDF8_DA34_4017_B2AB_AB5BE87E1A1D__INCLUDED_)
#define AFX_JAVASTARTER_H__B061EDF8_DA34_4017_B2AB_AB5BE87E1A1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CJavaStarterApp:
// Siehe JavaStarter.cpp für die Implementierung dieser Klasse
//

class CJavaStarterApp : public CWinApp
{
public:
	CJavaStarterApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CJavaStarterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CJavaStarterApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_JAVASTARTER_H__B061EDF8_DA34_4017_B2AB_AB5BE87E1A1D__INCLUDED_)

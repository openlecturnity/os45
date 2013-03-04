// LPLibs.h : Haupt-Header-Datei für die DLL LPLIBS
//

#if !defined(AFX_LPLIBS_H__17387BE0_B031_4FB0_8BE9_B02414582272__INCLUDED_)
#define AFX_LPLIBS_H__17387BE0_B031_4FB0_8BE9_B02414582272__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole
#include "TemplateEngine.h"
#include "ScormEngine.h"

// Declared in Template.cpp
extern CTemplateEngine g_templateEngine;

// Declared in Scorm.cpp
extern CScormEngine    g_scormEngine;

/////////////////////////////////////////////////////////////////////////////
// CLPLibsApp
// Siehe LPLibs.cpp für die Implementierung dieser Klasse
//

class CLPLibsApp : public CWinApp
{
public:
	CLPLibsApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CLPLibsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CLPLibsApp)
	afx_msg void OnSelectVariable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   HINSTANCE m_hMingDll;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_LPLIBS_H__17387BE0_B031_4FB0_8BE9_B02414582272__INCLUDED_)

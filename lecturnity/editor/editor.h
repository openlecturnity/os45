// editor.h : Haupt-Header-Datei für die Anwendung EDITOR
//

#if !defined(AFX_EDITOR_H__62F83C73_D859_4675_902B_D994D09EAA7C__INCLUDED_)
#define AFX_EDITOR_H__62F83C73_D859_4675_902B_D994D09EAA7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\Studio\Resource.h"
#include "securityutils.h"
#include "splashscreen.h"


/////////////////////////////////////////////////////////////////////////////
// CEditorApp:
// Siehe editor.cpp für die Implementierung dieser Klasse
//

class CEditorApp : public CWinApp
{
public:
	CEditorApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
    virtual void LoadStdProfileSettings(UINT nMaxMRU);
	//}}AFX_VIRTUAL

// Implementierung
	//{{AFX_MSG(CEditorApp)
	afx_msg void OnAppAbout();
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   // Returns one of the following:
   //  - FULL_VERSION (== 0L)
   //  - UNIVERSITY_VERSION (== 2L)
   //  - EVALUATION_VERSION (== 1L)
   //  - UNIVERSITY_EVALUATION_VERSION (==3L)
   // In order to check for an evaluation version, 
   // AND with EVALUATION_VERSION and check for > 0.
   int GetVersionType();
   CSecurityUtils m_securityUtils;
   DWORD GetDXVersion() { return m_dwDirectXVersion; }

private:
   HINSTANCE      m_hInstResDll;
   int            m_nVersionType;
   DWORD          m_dwDirectXVersion;
   CSplashScreenE m_splashScreen;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_EDITOR_H__62F83C73_D859_4675_902B_D994D09EAA7C__INCLUDED_)

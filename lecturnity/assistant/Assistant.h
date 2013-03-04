// Assistant.h : Hauptheaderdatei für die Assistant-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"       // Hauptsymbole


// CAssistantApp:
// Siehe Assistant.cpp für die Implementierung dieser Klasse
//

class CAssistantApp : public CWinApp
{
public:
	CAssistantApp();


// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAssistantApp theApp;

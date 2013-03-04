#if !defined(AFX_SETTINGSVIDEOCLIPSPAGE_H__B79A7DDC_B19A_4AEE_8C26_CC119AF7754D__INCLUDED_)
#define AFX_SETTINGSVIDEOCLIPSPAGE_H__B79A7DDC_B19A_4AEE_8C26_CC119AF7754D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsVideoClipsPage.h : Header-Datei
//

#include "CustomPropPage.h" // MfcUtils

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsVideoClipsPage 

class CSettingsVideoClipsPage : public CCustomPropPage
{
// Konstruktion
public:
	CSettingsVideoClipsPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSettingsVideoClipsPage)
	enum { IDD = IDD_SETTINGS_VIDEOCLIPS };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSettingsVideoClipsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSettingsVideoClipsPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SETTINGSVIDEOCLIPSPAGE_H__B79A7DDC_B19A_4AEE_8C26_CC119AF7754D__INCLUDED_

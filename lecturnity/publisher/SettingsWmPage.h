#if !defined(AFX_SETTINGSWMPAGE_H__C6EA92E0_1D4C_4E16_ACE4_AD612254932F__INCLUDED_)
#define AFX_SETTINGSWMPAGE_H__C6EA92E0_1D4C_4E16_ACE4_AD612254932F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsWmPage.h : Header-Datei
//

#include "CustomPropPage.h" // MfcUtils

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsWmPage 

class CSettingsWmPage : public CCustomPropPage
{
// Konstruktion
public:
	CSettingsWmPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSettingsWmPage)
	enum { IDD = IDD_SETTINGS_WM };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSettingsWmPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSettingsWmPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SETTINGSWMPAGE_H__C6EA92E0_1D4C_4E16_ACE4_AD612254932F__INCLUDED_

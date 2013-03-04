#if !defined(AFX_SETTINGSTARGETPAGE_H__E9E0E884_CFD0_4291_B052_A53C4D1C0ED1__INCLUDED_)
#define AFX_SETTINGSTARGETPAGE_H__E9E0E884_CFD0_4291_B052_A53C4D1C0ED1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsTargetPage.h : Header-Datei
//

#include "CustomPropPage.h" // MfcUtils

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsTargetPage 

class CSettingsTargetPage : public CCustomPropPage
{
// Konstruktion
public:
	CSettingsTargetPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSettingsTargetPage)
	enum { IDD = IDD_SETTINGS_TARGET };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSettingsTargetPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSettingsTargetPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SETTINGSTARGETPAGE_H__E9E0E884_CFD0_4291_B052_A53C4D1C0ED1__INCLUDED_

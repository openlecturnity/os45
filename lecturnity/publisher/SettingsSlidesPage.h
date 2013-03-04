#if !defined(AFX_SETTINGSSLIDESPAGE_H__348C5258_EB16_4DBC_B4FB_72B3D3D45766__INCLUDED_)
#define AFX_SETTINGSSLIDESPAGE_H__348C5258_EB16_4DBC_B4FB_72B3D3D45766__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsSlidesPage.h : Header-Datei
//

#include "CustomPropPage.h" // MfcUtils

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsSlidesPage 

class CSettingsSlidesPage : public CCustomPropPage
{
// Konstruktion
public:
	CSettingsSlidesPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSettingsSlidesPage)
	enum { IDD = IDD_SETTINGS_SLIDES };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSettingsSlidesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSettingsSlidesPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SETTINGSSLIDESPAGE_H__348C5258_EB16_4DBC_B4FB_72B3D3D45766__INCLUDED_

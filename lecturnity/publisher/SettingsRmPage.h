#if !defined(AFX_SETTINGSRMPAGE_H__D323965E_1D1D_4A25_9E8C_98390E573B2F__INCLUDED_)
#define AFX_SETTINGSRMPAGE_H__D323965E_1D1D_4A25_9E8C_98390E573B2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsRmPage.h : Header-Datei
//

#include "CustomPropPage.h" // MfcUtils

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsRmPage 

class CSettingsRmPage : public CCustomPropPage
{
// Konstruktion
public:
	CSettingsRmPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSettingsRmPage)
	enum { IDD = IDD_SETTINGS_RM };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSettingsRmPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSettingsRmPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SETTINGSRMPAGE_H__D323965E_1D1D_4A25_9E8C_98390E573B2F__INCLUDED_

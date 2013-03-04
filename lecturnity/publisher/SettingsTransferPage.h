#if !defined(AFX_SETTINGSTRANSFERPAGE_H__C1906B64_7796_464F_8458_0A00BB25FB14__INCLUDED_)
#define AFX_SETTINGSTRANSFERPAGE_H__C1906B64_7796_464F_8458_0A00BB25FB14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsTransferPage.h : Header-Datei
//

#include "CustomPropPage.h" // MfcUtils

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsTransferPage 

class CSettingsTransferPage : public CCustomPropPage
{
// Konstruktion
public:
	CSettingsTransferPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSettingsTransferPage)
	enum { IDD = IDD_SETTINGS_TRANSFER };
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSettingsTransferPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSettingsTransferPage)
	afx_msg void OnTransferConfigure0();
	afx_msg void OnTransferConfigure1();
	afx_msg void OnTransferConfigure2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SETTINGSTRANSFERPAGE_H__C1906B64_7796_464F_8458_0A00BB25FB14__INCLUDED_

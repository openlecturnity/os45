#if !defined(AFX_SETTINGSSCORMPAGE_H__EEF5D4CF_4D28_4CCA_B014_4241C871F287__INCLUDED_)
#define AFX_SETTINGSSCORMPAGE_H__EEF5D4CF_4D28_4CCA_B014_4241C871F287__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsScormPage.h : Header-Datei
//

#include "CustomPropPage.h" // MfcUtils

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsScormPage 

class CSettingsScormPage : public CCustomPropPage
{
// Konstruktion
public:
	CSettingsScormPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSettingsScormPage)
	enum { IDD = IDD_SETTINGS_SCORM };
	CButton	m_cbScormStrict;
	CButton	m_cbScorm;
	CButton	m_btConfigure;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSettingsScormPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSettingsScormPage)
	afx_msg void OnScormConfigure();
	afx_msg void OnChangeScorm();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SETTINGSSCORMPAGE_H__EEF5D4CF_4D28_4CCA_B014_4241C871F287__INCLUDED_

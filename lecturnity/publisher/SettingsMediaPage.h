#if !defined(AFX_SETTINGSMEDIAPAGE_H__E8D909FD_8A33_4A37_AC85_906808A9572B__INCLUDED_)
#define AFX_SETTINGSMEDIAPAGE_H__E8D909FD_8A33_4A37_AC85_906808A9572B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsMediaPage.h : Header-Datei
//

#include "CustomPropPage.h" // MfcUtils
#include "SettingsWmPage.h"
#include "SettingsRmPage.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsMediaPage 

class CSettingsMediaPage : public CCustomPropPage
{
// Konstruktion
public:
	CSettingsMediaPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSettingsMediaPage)
	enum { IDD = IDD_SETTINGS_MEDIA };
	CComboBox	m_cbbMediaFormat;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSettingsMediaPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSettingsMediaPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeMediaFormat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CSettingsWmPage m_wmPage;
   CSettingsRmPage m_rmPage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SETTINGSMEDIAPAGE_H__E8D909FD_8A33_4A37_AC85_906808A9572B__INCLUDED_

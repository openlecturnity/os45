#if !defined(AFX_NOTADMINDIALOG_H__A7DB4EA6_7020_41DE_B26C_79B1D0AAEC5A__INCLUDED_)
#define AFX_NOTADMINDIALOG_H__A7DB4EA6_7020_41DE_B26C_79B1D0AAEC5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NotAdminDialog.h : Header-Datei
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CNotAdminDialog 

class CNotAdminDialog : public CDialog
{
// Konstruktion
public:
	CNotAdminDialog(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CNotAdminDialog)
	enum { IDD = IDD_NOTADMIN };
	CButton	m_cbDisableAutoUpdate;
	CButton	m_cbCheckInDays;
	//}}AFX_DATA

public:
   bool IsDisableAutoUpdateChecked();
   bool IsCheckInDaysChecked();

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CNotAdminDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CNotAdminDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCheckbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   // This may seem redundant, but after the dialog
   // has been closed (after a DoModal()), all dialog
   // elements are destroyed, and thus can not be accessed
   // anymore.
   bool m_bIsDisableAutoUpdateChecked;
   bool m_bIsCheckInDaysChecked;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_NOTADMINDIALOG_H__A7DB4EA6_7020_41DE_B26C_79B1D0AAEC5A__INCLUDED_

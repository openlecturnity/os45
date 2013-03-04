#if !defined(AFX_PATCHAVAILABLEDIALOG_H__E1BDE204_4F1C_48CB_AC23_970410531C85__INCLUDED_)
#define AFX_PATCHAVAILABLEDIALOG_H__E1BDE204_4F1C_48CB_AC23_970410531C85__INCLUDED_

#include "resource.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatchAvailableDialog.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPatchAvailableDialog 

class CPatchAvailableDialog : public CDialog
{
// Konstruktion
public:
	CPatchAvailableDialog(CString csReleaseNotesUrl, int nPatchSize, CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPatchAvailableDialog)
	enum { IDD = IDD_PATCHAVAILABLE };
	CStatic	m_lbDownload;
	CButton	m_cbDisableAutoUpdate;
	//}}AFX_DATA

public:
   bool IsDisableAutoUpdateChecked() { return m_bIsDisableAutoUpdateChecked; }
   bool HasDisplayedReleaseNotes() { return m_bHasDisplayedReleaseNotes; }

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPatchAvailableDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPatchAvailableDialog)
	afx_msg void OnReleaseNotes();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCheckbox();
	afx_msg void OnNo();
   afx_msg void OnYes();
   afx_msg void OnLater();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CString m_csReleaseNotesUrl;
   int     m_nPatchSize;
   bool    m_bIsDisableAutoUpdateChecked;
   bool    m_bHasDisplayedReleaseNotes;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PATCHAVAILABLEDIALOG_H__E1BDE204_4F1C_48CB_AC23_970410531C85__INCLUDED_

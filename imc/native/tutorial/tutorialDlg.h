// tutorialDlg.h : Header-Datei
//

#if !defined(AFX_TUTORIALDLG_H__5FE6B0FA_E121_429F_B997_FBC954083B25__INCLUDED_)
#define AFX_TUTORIALDLG_H__5FE6B0FA_E121_429F_B997_FBC954083B25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTutorialDlg Dialogfeld

#include "launchjava.h"

class CTutorialDlg : public CDialog
{
// Konstruktion
public:
	CTutorialDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTutorialDlg)
	enum { IDD = IDD_TUTORIAL_DIALOG };
	CStatic	fileLabel_;
	CButton	okButton_;
	CButton	cancelButton_;
	CStatic	helpLabel_;
	CEdit	filenameField_;
	CButton	browseButton_;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CTutorialDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CTutorialDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_TUTORIALDLG_H__5FE6B0FA_E121_429F_B997_FBC954083B25__INCLUDED_)

#if !defined(AFX_FILENAMECHANGEDLG_H__763D46D1_DE45_4A25_BE86_290191A9EC5D__INCLUDED_)
#define AFX_FILENAMECHANGEDLG_H__763D46D1_DE45_4A25_BE86_290191A9EC5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilenameChangeDlg.h : Header-Datei
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFilenameChangeDlg 

class CFilenameChangeDlg : public CDialog
{
// Konstruktion
public:
	CFilenameChangeDlg(const _TCHAR *tszFilename, CWnd* pParent = NULL);   // Standardkonstruktor

   CString GetFileName() { return m_csFileName; }
// Dialogfelddaten
	//{{AFX_DATA(CFilenameChangeDlg)
	enum { IDD = IDD_FILENAME };
	CEdit	m_edFileName;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFilenameChangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CFilenameChangeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString m_csFileName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FILENAMECHANGEDLG_H__763D46D1_DE45_4A25_BE86_290191A9EC5D__INCLUDED_

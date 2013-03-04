#if !defined(AFX_SCORMPROGRESSDLG_H__FC1EB39F_A959_4849_9D3E_4C6A31844596__INCLUDED_)
#define AFX_SCORMPROGRESSDLG_H__FC1EB39F_A959_4849_9D3E_4C6A31844596__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScormProgressDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CScormProgressDlg 

class CScormProgressDlg : public CDialog
{
// Konstruktion
public:
	CScormProgressDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CScormProgressDlg)
	enum { IDD = IDD_SCORM_PROGRESS };
	CStatic	m_lbProgress;
	CProgressCtrl	m_pcTotalProgress;
	CProgressCtrl	m_pcFileProgress;
	CButton	m_btCancel;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CScormProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CScormProgressDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SCORMPROGRESSDLG_H__FC1EB39F_A959_4849_9D3E_4C6A31844596__INCLUDED_

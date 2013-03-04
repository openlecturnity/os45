#if !defined(AFX_INSERTTARGETMARKDIALOG_H__607E3EBA_ACA2_430B_964F_36605D930872__INCLUDED_)
#define AFX_INSERTTARGETMARKDIALOG_H__607E3EBA_ACA2_430B_964F_36605D930872__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InsertTargetmarkDialog.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////

class CInsertTargetmarkDialog : public CDialog
{
// Konstruktion
public:
	CInsertTargetmarkDialog(CWnd* pParent = NULL);   // Standardkonstruktor
	CInsertTargetmarkDialog(CWnd* pParent, int &nCheckID, CString &csTargetmarkName, bool bDisableRadioButtons);

// Dialogfelddaten
	//{{AFX_DATA(CInsertTargetmarkDialog)
	enum { IDD = IDD_INSERT_TARGETMARK };
	CEdit	m_editTargetmarkName;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CInsertTargetmarkDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CInsertTargetmarkDialog)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int *m_nCheckID;
	CString *m_csTargetmarkName;
	bool m_bDisableRadioButtons;

public:
	static void ShowDialog(CWnd* pParent, int &nCheckID, CString &csTargetmarkName, bool bDisableRadioButtons=false);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_INSERTTARGETMARKDIALOG_H__607E3EBA_ACA2_430B_964F_36605D930872__INCLUDED_

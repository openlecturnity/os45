#if !defined(AFX_FRONTPAGEDLG_H__73EC8AE2_2C88_4D09_B5B5_1F7B4DFDFBA4__INCLUDED_)
#define AFX_FRONTPAGEDLG_H__73EC8AE2_2C88_4D09_B5B5_1F7B4DFDFBA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrontpageDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFrontpageDlg 

class CFrontpageDlg : public CDialog
{
// Konstruktion
public:
	CFrontpageDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CFrontpageDlg)
	enum { IDD = IDD_FRONTPAGE };
	CComboBox	m_cbbProfile;
	CEdit	m_edFileName;
	CButton	m_cbDontShowAgain;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFrontpageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CFrontpageDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnExpertMode();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CBrush m_white;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FRONTPAGEDLG_H__73EC8AE2_2C88_4D09_B5B5_1F7B4DFDFBA4__INCLUDED_

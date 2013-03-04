#if !defined(AFX_JAVASTARTERDLG_H__C93D1C29_C076_4552_94A3_B00A2D55BBBF__INCLUDED_)
#define AFX_JAVASTARTERDLG_H__C93D1C29_C076_4552_94A3_B00A2D55BBBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CJavaStarterDlg Dialogfeld

class CJavaStarterDlg : public CDialog
{
// Konstruktion
public:
	CJavaStarterDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CJavaStarterDlg)
	enum { IDD = IDD_JAVASTARTER_DIALOG };
		// HINWEIS: der Klassenassistent fügt an dieser Stelle Datenelemente (Members) ein
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CJavaStarterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CJavaStarterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_JAVASTARTERDLG_H__C93D1C29_C076_4552_94A3_B00A2D55BBBF__INCLUDED_)

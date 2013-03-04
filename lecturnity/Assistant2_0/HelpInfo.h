#if !defined(AFX_HELPINFO_H__B5141DD2_DF25_4A1E_BAD9_CE4E6723D2AB__INCLUDED_)
#define AFX_HELPINFO_H__B5141DD2_DF25_4A1E_BAD9_CE4E6723D2AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpInfo.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CHelpInfo 

#include "..\Studio\Resource.h"

class CHelpInfo : public CDialog
{
// Konstruktion
public:
	CHelpInfo(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CHelpInfo)
	enum { IDD = IDD_QUICK_INFO };
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CHelpInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CHelpInfo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CBrush m_brushYellow;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_HELPINFO_H__B5141DD2_DF25_4A1E_BAD9_CE4E6723D2AB__INCLUDED_

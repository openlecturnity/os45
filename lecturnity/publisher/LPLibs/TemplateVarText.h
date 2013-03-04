#if !defined(AFX_TEMPLATEVARTEXT_H__B3F62BFF_48A2_4FB1_8391_7230099F503D__INCLUDED_)
#define AFX_TEMPLATEVARTEXT_H__B3F62BFF_48A2_4FB1_8391_7230099F503D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateVarText.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarText 

#include "TemplateEngine.h"

class CTemplateVarText : public CDialog
{
// Konstruktion
public:
	CTemplateVarText(CTemplateKeyword *pKeyword);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTemplateVarText)
	enum { IDD = IDD_TEMPLATE_VAR_TEXT };
	CEdit	m_edText;
	CEdit	m_edVarName;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTemplateVarText)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTemplateVarText)
	virtual BOOL OnInitDialog();
	afx_msg void OnLeaveTextField();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CTemplateTextKeyword *m_pKeyword;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEMPLATEVARTEXT_H__B3F62BFF_48A2_4FB1_8391_7230099F503D__INCLUDED_

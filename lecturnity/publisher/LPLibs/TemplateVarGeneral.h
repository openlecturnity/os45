#if !defined(AFX_TEMPLATEVARGENERAL_H__76F6B9A8_8B3B_488A_8D2B_EE7505B6A5E6__INCLUDED_)
#define AFX_TEMPLATEVARGENERAL_H__76F6B9A8_8B3B_488A_8D2B_EE7505B6A5E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateVarGeneral.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarGeneral 

#include "TemplateEngine.h"

class CTemplateVarGeneral : public CDialog
{
// Konstruktion
public:
	CTemplateVarGeneral(CTemplateKeyword *pKeyword);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTemplateVarGeneral)
	enum { IDD = IDD_TEMPLATE_VAR_GENERAL };
	CEdit	m_edGeneral;
	CEdit	m_edVarName;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTemplateVarGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTemplateVarGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnLeaveTextField();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CTemplateGeneralKeyword *m_pKeyword;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEMPLATEVARGENERAL_H__76F6B9A8_8B3B_488A_8D2B_EE7505B6A5E6__INCLUDED_

#if !defined(AFX_TEMPLATEVARSELECT_H__E360FC32_9561_434F_A3A9_21725A1B9B47__INCLUDED_)
#define AFX_TEMPLATEVARSELECT_H__E360FC32_9561_434F_A3A9_21725A1B9B47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateVarSelect.h : Header-Datei
//

#include "TemplateEngine.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarSelect 

class CTemplateVarSelect : public CDialog
{
// Konstruktion
public:
	CTemplateVarSelect(CTemplateKeyword *pKeyword);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTemplateVarSelect)
	enum { IDD = IDD_TEMPLATE_VAR_SELECT };
	CComboBox	m_cbSelect;
	CEdit	m_edVarName;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTemplateVarSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTemplateVarSelect)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSelection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CTemplateSelectKeyword *m_pKeyword;

private:
   void CheckForStandardNavigation(int nSel);
   void CheckForRealPlayerWarning();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEMPLATEVARSELECT_H__E360FC32_9561_434F_A3A9_21725A1B9B47__INCLUDED_

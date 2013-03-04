#if !defined(AFX_TEMPLATECONFIGUREPAGE_H__4706A036_844B_4D48_AC4B_08E0B598CF7C__INCLUDED_)
#define AFX_TEMPLATECONFIGUREPAGE_H__4706A036_844B_4D48_AC4B_08E0B598CF7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateConfigurePage.h : Header-Datei
//


// Note: To design a own ListBox, a class "MyListBox" has to 
// derived from "CListBox" and the "DrawItem()" method has to be 
// overwritten. - Also necessary:
// --> Resource IDD_TEMPLATE_VARIABLES has to be changed to 
// "Owner Draw" --> "Fixed" and "Has String" --> "true"
#include "MyListBox.h"

class CTemplateKeyword;
class CTemplateSelectPage;

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateConfigurePage 

class CTemplateConfigurePage : public CPropertyPage
{
// Konstruktion
public:
	CTemplateConfigurePage(CTemplateSelectPage *pSelectPage);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTemplateConfigurePage)
	enum { IDD = IDD_TEMPLATE_VARIABLES };
	CStatic	m_lbNoVarSelected;
///	CListBox	m_lbVariables;
	MyListBox	m_lbVariables;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTemplateConfigurePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTemplateConfigurePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeVariable();
	afx_msg void OnResetAllVariables();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CTemplateSelectPage *m_pSelectPage;
   CString m_csCaption;
   CArray <CDialog *, CDialog *> m_caConfigWnds;
   CArray <CTemplateKeyword *, CTemplateKeyword *> m_caKeywords;

public:
   void SetKeywords(CArray<CTemplateKeyword *, CTemplateKeyword *> &caKeywords);
   CArray <CTemplateKeyword *, CTemplateKeyword *>* GetKeywords();
   void UpdateSubWindows();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEMPLATECONFIGUREPAGE_H__4706A036_844B_4D48_AC4B_08E0B598CF7C__INCLUDED_

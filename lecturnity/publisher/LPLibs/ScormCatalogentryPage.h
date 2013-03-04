#if !defined(AFX_SCORMCATALOGENTRYPAGE_H__668A4378_DFBB_4151_A552_1C940C254319__INCLUDED_)
#define AFX_SCORMCATALOGENTRYPAGE_H__668A4378_DFBB_4151_A552_1C940C254319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScormCatalogentryPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CScormCatalogentryPage 

class CScormCatalogentryPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CScormCatalogentryPage)

// Konstruktion
public:
	CScormCatalogentryPage();
	~CScormCatalogentryPage();

// Dialogfelddaten
	//{{AFX_DATA(CScormCatalogentryPage)
	enum { IDD = IDD_SCORM_CATALOGENTRY };
	CEdit	m_edCeEntry;
	CEdit	m_edCeCatalog;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CScormCatalogentryPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CScormCatalogentryPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAnything();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SCORMCATALOGENTRYPAGE_H__668A4378_DFBB_4151_A552_1C940C254319__INCLUDED_

#if !defined(AFX_SCORMGENERALPAGE_H__E3B4B0ED_462E_4504_B538_3640DD427C09__INCLUDED_)
#define AFX_SCORMGENERALPAGE_H__E3B4B0ED_462E_4504_B538_3640DD427C09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScormGeneralPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CScormGeneralPage 

class CScormGeneralPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CScormGeneralPage)

// Konstruktion
public:
	CScormGeneralPage();
	~CScormGeneralPage();

// Dialogfelddaten
	//{{AFX_DATA(CScormGeneralPage)
	enum { IDD = IDD_SCORM_GENERAL };
	CEdit	m_edVersion;
	CComboBox	m_cbbStatus;
	CComboBox	m_cbbLanguage;
	CButton	m_cbFulltextAsKeyword;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CScormGeneralPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CScormGeneralPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAnything();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SCORMGENERALPAGE_H__E3B4B0ED_462E_4504_B538_3640DD427C09__INCLUDED_

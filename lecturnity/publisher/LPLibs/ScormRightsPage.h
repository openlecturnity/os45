#if !defined(AFX_SCORMRIGHTSPAGE_H__CB4428C5_217B_490B_9553_D24008448F1D__INCLUDED_)
#define AFX_SCORMRIGHTSPAGE_H__CB4428C5_217B_490B_9553_D24008448F1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScormRightsPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CScormRightsPage 

class CScormRightsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CScormRightsPage)

// Konstruktion
public:
	CScormRightsPage();
	~CScormRightsPage();

// Dialogfelddaten
	//{{AFX_DATA(CScormRightsPage)
	enum { IDD = IDD_SCORM_RIGHTS };
	CButton	m_cbCost;
	CButton	m_cbCopyright;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CScormRightsPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CScormRightsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangedAnything();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SCORMRIGHTSPAGE_H__CB4428C5_217B_490B_9553_D24008448F1D__INCLUDED_

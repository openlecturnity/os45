#if !defined(AFX_METADATAPROPS_H__50458CA8_FC46_4DC0_9372_A2CD1755D4FE__INCLUDED_)
#define AFX_METADATAPROPS_H__50458CA8_FC46_4DC0_9372_A2CD1755D4FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "editorDoc.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CMetadataProps 

class CMetadataProps : public CPropertyPage
{
	DECLARE_DYNCREATE(CMetadataProps)

// Konstruktion
public:
	CMetadataProps();
	~CMetadataProps();

// Dialogfelddaten
	//{{AFX_DATA(CMetadataProps)
	enum { IDD = IDD_METADATA_E };
	CEdit	m_edCodepage;
	CEdit	m_titleEdit;
	CEdit	m_producerEdit;
	CEdit	m_keywordsEdit;
	CEdit	m_authorEdit;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CMetadataProps)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CMetadataProps)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CEditorDoc *m_pDoc;
   CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_METADATAPROPS_H__50458CA8_FC46_4DC0_9372_A2CD1755D4FE__INCLUDED_

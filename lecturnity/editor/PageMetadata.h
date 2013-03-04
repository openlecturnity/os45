#if !defined(AFX_PAGEMETADATA_H__9C4743FD_4E6C_49BB_983E_BC36FA39DC8F__INCLUDED_)
#define AFX_PAGEMETADATA_H__9C4743FD_4E6C_49BB_983E_BC36FA39DC8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LmdFile.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPageMetadata 

class CPageMetadata : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMetadata)

// Konstruktion
public:
	CPageMetadata();
	~CPageMetadata();

// Dialogfelddaten
	//{{AFX_DATA(CPageMetadata)
	enum { IDD = IDD_PAGE_PROPERTIES };
	CEdit	m_edTitle;
	CEdit	m_edKeywords;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPageMetadata)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPageMetadata)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   void SetCurrentPage(CPageInformation *pPage);
   void SetCurrentClip(CClipInfo *pClip);

private:
   CPageInformation *m_pPage;
   CClipInfo *m_pClip; 

protected:
   CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PAGEMETADATA_H__9C4743FD_4E6C_49BB_983E_BC36FA39DC8F__INCLUDED_

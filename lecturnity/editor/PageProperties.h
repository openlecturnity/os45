#if !defined(AFX_PAGEPROPERTIES2_H__3BD21667_F6A1_4F0D_A79A_FEAF94F635C5__INCLUDED_)
#define AFX_PAGEPROPERTIES2_H__3BD21667_F6A1_4F0D_A79A_FEAF94F635C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageProperties2.h : Header-Datei
//

#include "PageMetadata.h"

/////////////////////////////////////////////////////////////////////////////
// CPageProperties

class CPageProperties : public CPropertySheet
{
	DECLARE_DYNAMIC(CPageProperties)

// Konstruktion
public:
	CPageProperties(CPageInformation *pPage, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPageProperties(CPageInformation *pPage, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPageProperties(CClipInfo *pClip, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPageProperties(CClipInfo *pClip, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPageProperties)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CPageProperties();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CPageProperties)
		// HINWEIS - Der Klassen-Assistent fügt hier Member-Funktionen ein und entfernt diese.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   void Init(CPageInformation *pPage);
   void Init(CClipInfo *pClip);

   CPageMetadata *m_pMetadata;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PAGEPROPERTIES2_H__3BD21667_F6A1_4F0D_A79A_FEAF94F635C5__INCLUDED_

#if !defined(AFX_SELECTTEMPLATEPAGE_H__52CA8E29_ECB6_4DD8_BE96_600D2E912B21__INCLUDED_)
#define AFX_SELECTTEMPLATEPAGE_H__52CA8E29_ECB6_4DD8_BE96_600D2E912B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// selecttemplatepage.h : Header-Datei
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSelectTemplatePage 

class CTemplateInfo
{
public:
   CTemplateInfo() {}
   CTemplateInfo(const _TCHAR *szPath, const _TCHAR *szTitle, const _TCHAR *szDescription, const _TCHAR *szAuthor, const _TCHAR *szCopyright) :
      m_csPath(szPath), m_csTitle(szTitle), m_csDescription(szDescription), m_csAuthor(szAuthor), m_csCopyright(szCopyright)
   {}

   CString m_csTitle;
   CString m_csDescription;
   CString m_csPath;
   CString m_csAuthor;
   CString m_csCopyright;
};

class CSelectTemplatePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSelectTemplatePage)

// Konstruktion
public:
	CSelectTemplatePage();
	~CSelectTemplatePage();

// Dialogfelddaten
	//{{AFX_DATA(CSelectTemplatePage)
	enum { IDD = IDD_SELECT_TEMPLATE };
	CStatic	m_lbPath;
	CStatic	m_lbDescription;
	CStatic	m_lbCopyright;
	CStatic	m_lbAuthor;
	CListBox	m_lsTemplates;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSelectTemplatePage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSelectTemplatePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeTemplate();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   bool AddTemplateToList(const _TCHAR *szPath);
   CString GetSplittedPathString(CString csPath);

   CArray<CTemplateInfo, CTemplateInfo> m_caTemplates;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SELECTTEMPLATEPAGE_H__52CA8E29_ECB6_4DD8_BE96_600D2E912B21__INCLUDED_

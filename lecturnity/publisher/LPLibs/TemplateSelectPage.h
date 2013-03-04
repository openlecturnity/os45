#if !defined(AFX_TEMPLATESELECTPAGE_H__DB8ADB83_D8FA_430A_ACB1_1FFE4F38095A__INCLUDED_)
#define AFX_TEMPLATESELECTPAGE_H__DB8ADB83_D8FA_430A_ACB1_1FFE4F38095A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateSelectPage.h : Header-Datei
//

#include "TemplateConfigurePage.h"
#include "TemplatePreviewDlg.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateSelectPage 

class CTemplateKeyword; // Forward declaration: TemplateEngine.h


class CTemplateSelectPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTemplateSelectPage)

// Konstruktion
public:
	CTemplateSelectPage();
	~CTemplateSelectPage();

// Dialogfelddaten
	//{{AFX_DATA(CTemplateSelectPage)
	enum { IDD = IDD_TEMPLATE_SELECT };
	CStatic	m_lbScormCompatible;
	CButton	m_cbPreview;
	CListBox	m_listTemplates;
	CButton	m_btRemove;
	CStatic	m_lbPath;
	CStatic	m_lbDescription;
	CStatic	m_lbCopyright;
	CStatic	m_lbAuthor;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTemplateSelectPage)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL


public:
   // If bTotalReset is true, the default configuration of the
   // template is read, otherwise the default setting of the
   // computer (in the HKLM branch of the registry).
   void ResetTemplateConfiguration(bool bTotalReset);

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTemplateSelectPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeTemplate();
	afx_msg void OnAddTemplate();
	afx_msg void OnRemoveTemplate();
	afx_msg void OnDestroy();
	afx_msg void OnTogglePreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString m_csCaption;
   CTemplateConfigurePage *m_pConfigurePage;
   CTemplatePreviewDlg     m_previewDlg;
   CArray<CString, CString> m_caTemplates;
   CArray<CTemplateKeyword *, CTemplateKeyword *> m_caKeywords;
   int m_nOriginalTemplate;

   bool AddTemplateToList(const _TCHAR *tszTemplateDir);
   // nReset:
   // - TEMPLATE_RESET_NORESET: Template configuration is not
   //   reset, all configuration is read from the registry
   // - TEMPLATE_RESET_SYSTEMDEFAULT: Template configuration is
   //   reset to what is stored under the HKLM branch of the
   //   registry for this template. Or to the template default
   //   if such information is not available.
   // - TEMPLATE_RESET_TEMPLATEDEFAULT: Template configuration
   //   is reset to the default values for the template itself.
   void SelectTemplate(int nIndex, int nReset=TEMPLATE_RESET_NORESET);

   void SaveTemplateConfiguration(bool bSavePath);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEMPLATESELECTPAGE_H__DB8ADB83_D8FA_430A_ACB1_1FFE4F38095A__INCLUDED_

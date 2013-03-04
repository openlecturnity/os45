// TemplateDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplateDlg.h"
#include "TemplateEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg

IMPLEMENT_DYNAMIC(CTemplateDlg, CMySheet)

CTemplateDlg::CTemplateDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMySheet(nIDCaption, pParentWnd, iSelectPage)
{
   m_pTemplateSelect = new CTemplateSelectPage;
   AddPage(m_pTemplateSelect);
   m_pTemplateConfigure = NULL;
}

CTemplateDlg::CTemplateDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CTemplateDlg::~CTemplateDlg()
{
   delete m_pTemplateSelect;
}


BEGIN_MESSAGE_MAP(CTemplateDlg, CMySheet)
	//{{AFX_MSG_MAP(CTemplateDlg)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplateDlg 

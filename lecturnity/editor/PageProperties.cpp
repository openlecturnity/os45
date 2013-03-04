// PageProperties2.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "PageProperties.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageProperties

IMPLEMENT_DYNAMIC(CPageProperties, CPropertySheet)

CPageProperties::CPageProperties(CPageInformation *pPage, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   m_pMetadata = NULL;
   Init(pPage);
}

CPageProperties::CPageProperties(CPageInformation *pPage, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   m_pMetadata = NULL;
   Init(pPage);
}

CPageProperties::CPageProperties(CClipInfo *pClip, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   m_pMetadata = NULL;
   Init(pClip);
}

CPageProperties::CPageProperties(CClipInfo *pClip, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   m_pMetadata = NULL;
   Init(pClip);
}

CPageProperties::~CPageProperties()
{
   if (m_pMetadata)
      delete m_pMetadata;
   m_pMetadata = NULL;
}


BEGIN_MESSAGE_MAP(CPageProperties, CPropertySheet)
	//{{AFX_MSG_MAP(CPageProperties)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPageProperties 

void CPageProperties::Init(CPageInformation *pPage) 
{	
   m_pMetadata = new CPageMetadata();
   m_pMetadata->SetCurrentPage(pPage);
   
   AddPage(m_pMetadata);

	return;
}

void CPageProperties::Init(CClipInfo *pClip) 
{	
   m_pMetadata = new CPageMetadata();
   m_pMetadata->SetCurrentClip(pClip);
   
   AddPage(m_pMetadata);

	return;
}

BOOL CPageProperties::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
   
   DWORD dwIds[] = {IDOK,
                    IDCANCEL,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CWnd *pApply = GetDlgItem(ID_APPLY_NOW);
   if(pApply != NULL)
      pApply->DestroyWindow();
   
   CWnd *pHelp = GetDlgItem(IDHELP);
   if(pHelp != NULL)
      pHelp->DestroyWindow();
   
   CRect rectBtn;
   int nSpacing = 6;        // space between two buttons...

   int ids [] = {IDOK, IDCANCEL};
   for( int i =0; i < sizeof(ids)/sizeof(int); i++)
   {
      GetDlgItem (ids [i])->GetWindowRect (rectBtn);
      
      ScreenToClient (&rectBtn);
      int btnWidth = rectBtn.Width();
      rectBtn.left = rectBtn.left + (btnWidth + nSpacing)* 2;
      rectBtn.right = rectBtn.right + (btnWidth + nSpacing)* 2;
      
      GetDlgItem (ids [i])->MoveWindow(rectBtn);
   }


	return bResult;
}

// ScormRightsPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "LPLibs.h"
#include "ScormRightsPage.h"
#include "ScormEngine.h"

#include "MfcUtils.h"

extern CScormEngine g_scormEngine;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CScormRightsPage 

IMPLEMENT_DYNCREATE(CScormRightsPage, CPropertyPage)

CScormRightsPage::CScormRightsPage() : CPropertyPage(CScormRightsPage::IDD)
{
	//{{AFX_DATA_INIT(CScormRightsPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   if (FALSE != m_csCaption.LoadString(IDD))
   {
      m_psp.pszTitle = m_csCaption;
      m_psp.dwFlags |= PSP_USETITLE;
   }
}

CScormRightsPage::~CScormRightsPage()
{
}

void CScormRightsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScormRightsPage)
	DDX_Control(pDX, IDC_SCORM_COST, m_cbCost);
	DDX_Control(pDX, IDC_SCORM_COPYRIGHT, m_cbCopyright);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScormRightsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CScormRightsPage)
	ON_BN_CLICKED(IDC_SCORM_COST, OnChangedAnything)
	ON_BN_CLICKED(IDC_SCORM_COPYRIGHT, OnChangedAnything)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CScormRightsPage 

BOOL CScormRightsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   m_cbCost.SetCheck(g_scormEngine.m_scormSettings.bCost ? BST_CHECKED : BST_UNCHECKED);
   m_cbCopyright.SetCheck(g_scormEngine.m_scormSettings.bCopyright ? BST_CHECKED : BST_UNCHECKED);

   DWORD dwIds[] = {IDC_SCORM_COPYRIGHT_GROUP,
                    IDC_SCORM_COST,
                    IDC_SCORM_COPYRIGHT,
                    IDC_SCORM_DESC_SETTINGS,
                    -1};
   MfcUtils::Localize(this, dwIds);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CScormRightsPage::OnChangedAnything() 
{
   CPropertyPage::SetModified();
}

BOOL CScormRightsPage::OnApply() 
{
   g_scormEngine.m_scormSettings.bCopyright = (m_cbCopyright.GetCheck() == BST_CHECKED) ? true : false;
   g_scormEngine.m_scormSettings.bCost = (m_cbCost.GetCheck() == BST_CHECKED) ? true : false;

   g_scormEngine.WriteSettings();

   return CPropertyPage::OnApply();
}

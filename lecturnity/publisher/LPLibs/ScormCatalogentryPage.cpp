// ScormCatalogentryPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "LPLibs.h"
#include "ScormCatalogentryPage.h"
#include "ScormEngine.h"
#include "MfcUtils.h"

extern CScormEngine g_scormEngine;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CScormCatalogentryPage 

IMPLEMENT_DYNCREATE(CScormCatalogentryPage, CPropertyPage)

CScormCatalogentryPage::CScormCatalogentryPage() : CPropertyPage(CScormCatalogentryPage::IDD)
{
	//{{AFX_DATA_INIT(CScormCatalogentryPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   if (FALSE != m_csCaption.LoadString(IDD))
   {
      m_psp.pszTitle = m_csCaption;
      m_psp.dwFlags |= PSP_USETITLE;
   }
}

CScormCatalogentryPage::~CScormCatalogentryPage()
{
}


void CScormCatalogentryPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScormCatalogentryPage)
	DDX_Control(pDX, IDC_ENTRY_NAME, m_edCeEntry);
	DDX_Control(pDX, IDC_CATALOG_NAME, m_edCeCatalog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScormCatalogentryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CScormCatalogentryPage)
	ON_EN_CHANGE(IDC_CATALOG_NAME, OnChangeAnything)
	ON_EN_CHANGE(IDC_ENTRY_NAME, OnChangeAnything)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CScormCatalogentryPage 

BOOL CScormCatalogentryPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   CString csGnrlCeCatalog(g_scormEngine.m_scormSettings.m_tszGnrlCeCatalog);
   m_edCeCatalog.SetWindowText(csGnrlCeCatalog);
   CString csGnrlCeEntry(g_scormEngine.m_scormSettings.m_tszGnrlCeEntry);
   m_edCeEntry.SetWindowText(csGnrlCeEntry);

   DWORD dwIds[] = {IDC_SCORM_CATALOGENTRY_GROUP,
                    IDC_SCORM_DESC_SETTINGS,
                    IDC_SCORM_LB_CATALOG,
                    IDC_SCORM_LB_ENTRY,
                    -1};
   MfcUtils::Localize(this, dwIds);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL CScormCatalogentryPage::OnApply() 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
   memset(g_scormEngine.m_scormSettings.m_tszGnrlCeCatalog, 0, SCORM_GNRL_CE_CATALOG_SIZE * sizeof _TCHAR);
   memset(g_scormEngine.m_scormSettings.m_tszGnrlCeEntry, 0, SCORM_GNRL_CE_ENTRY_SIZE * sizeof _TCHAR);

   CString csGnrlCeCatalog;
   m_edCeCatalog.GetWindowText(csGnrlCeCatalog);
   CString csGnrlCeEntry;
   m_edCeEntry.GetWindowText(csGnrlCeEntry);
   _tcsncpy(g_scormEngine.m_scormSettings.m_tszGnrlCeCatalog, csGnrlCeCatalog, SCORM_GNRL_CE_CATALOG_SIZE-1);
   _tcsncpy(g_scormEngine.m_scormSettings.m_tszGnrlCeEntry, csGnrlCeEntry, SCORM_GNRL_CE_ENTRY_SIZE-1);

   g_scormEngine.WriteSettings();

	return CPropertyPage::OnApply();
}

void CScormCatalogentryPage::OnChangeAnything() 
{
   CPropertyPage::SetModified();
}

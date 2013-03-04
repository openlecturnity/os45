// ScormGeneralPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "LPLibs.h"
#include "ScormGeneralPage.h"
#include "ScormEngine.h"
#include "MfcUtils.h"

extern CScormEngine g_scormEngine;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CScormGeneralPage 

IMPLEMENT_DYNCREATE(CScormGeneralPage, CPropertyPage)

CScormGeneralPage::CScormGeneralPage() : CPropertyPage(CScormGeneralPage::IDD)
{
	//{{AFX_DATA_INIT(CScormGeneralPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT


   if (FALSE != m_csCaption.LoadString(IDD))
   {
      m_psp.pszTitle = m_csCaption;
      m_psp.dwFlags |= PSP_USETITLE;
   }
}

CScormGeneralPage::~CScormGeneralPage()
{
}

void CScormGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScormGeneralPage)
	DDX_Control(pDX, IDC_SCORM_VERSION, m_edVersion);
	DDX_Control(pDX, IDC_SCORM_STATUS, m_cbbStatus);
	DDX_Control(pDX, IDC_SCORM_LANGUAGE, m_cbbLanguage);
	DDX_Control(pDX, IDC_SCORM_FULLTEXTKEYWORDS, m_cbFulltextAsKeyword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScormGeneralPage, CPropertyPage)
	//{{AFX_MSG_MAP(CScormGeneralPage)
	ON_BN_CLICKED(IDC_SCORM_FULLTEXTKEYWORDS, OnChangeAnything)
   ON_CBN_SELCHANGE(IDC_SCORM_LANGUAGE, OnChangeAnything)
   ON_CBN_SELCHANGE(IDC_SCORM_STATUS, OnChangeAnything)
   ON_EN_CHANGE(IDC_SCORM_VERSION, OnChangeAnything)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CScormGeneralPage 

BOOL CScormGeneralPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
   CString csVersion(g_scormEngine.m_scormSettings.szVersion);
   m_edVersion.SetWindowText(csVersion);
   // Bug 3026: Disable "version" field, until SCORM version is changeable
   m_edVersion.SetReadOnly(TRUE);
   CString csStatus(g_scormEngine.m_scormSettings.szStatus);
   int nCount = m_cbbStatus.GetCount();
   int i = 0;
   for (i = 0; i < nCount; ++i)
   {
      CString csItem;
      m_cbbStatus.GetLBText(i, csItem);
      if (csItem == csStatus)
         m_cbbStatus.SetCurSel(i);
   }

   nCount = m_cbbLanguage.GetCount();
   CString csLanguage(g_scormEngine.m_scormSettings.szLanguage);
   if (csLanguage == _T(""))
      csLanguage.LoadString(IDS_LANGUAGE);
   for (i = 0; i < nCount; ++i)
   {
      CString csItem;
      m_cbbLanguage.GetLBText(i, csItem);
      if (csItem.Mid(csItem.GetLength() - 3, 2) == csLanguage)
         m_cbbLanguage.SetCurSel(i);
   }

   m_cbFulltextAsKeyword.SetCheck(g_scormEngine.m_scormSettings.bFulltextAsKeyword ? BST_CHECKED : BST_UNCHECKED);

   DWORD dwIds[] = {IDC_SCORM_DOCINFO,
                    IDC_SCORM_LB_LANGUAGE,
                    IDC_SCORM_SETTINGS_GROUP,
                    IDC_SCORM_DESC_SETTINGS,
                    IDC_SCORM_LB_VERSION,
                    IDC_SCORM_LB_STATUS,
                    IDC_SCORM_FULLTEXTKEYWORDS,
                    -1};

   MfcUtils::Localize(this, dwIds);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL CScormGeneralPage::OnApply() 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
   memset(g_scormEngine.m_scormSettings.szLanguage, 0, SCORM_LANGUAGE_SIZE);
   memset(g_scormEngine.m_scormSettings.szVersion, 0, SCORM_VERSION_SIZE);
   memset(g_scormEngine.m_scormSettings.szStatus, 0, SCORM_STATUS_SIZE);

   CString csLanguageString;
   m_cbbLanguage.GetLBText(m_cbbLanguage.GetCurSel(), csLanguageString);
   CString csLanguage = csLanguageString.Mid(csLanguageString.GetLength() - 3, 2);
   CString csVersion;
   m_edVersion.GetWindowText(csVersion);
   CString csStatus;
   m_cbbStatus.GetLBText(m_cbbStatus.GetCurSel(), csStatus);
#ifdef _UNICODE
   ::WideCharToMultiByte(CP_ACP, 0, csLanguage, -1, 
      g_scormEngine.m_scormSettings.szLanguage, SCORM_LANGUAGE_SIZE-1, NULL, NULL);
   ::WideCharToMultiByte(CP_ACP, 0, csVersion, -1,
      g_scormEngine.m_scormSettings.szVersion, SCORM_VERSION_SIZE-1, NULL, NULL);
   ::WideCharToMultiByte(CP_ACP, 0, csStatus, -1,
      g_scormEngine.m_scormSettings.szStatus, SCORM_STATUS_SIZE-1, NULL, NULL);
#else
   strcpy(g_scormEngine.m_scormSettings.szLanguage, csLanguage);
   strncpy(g_scormEngine.m_scormSettings.szVersion, csVersion, SCORM_VERSION_SIZE-1);
   strcpy(g_scormEngine.m_scormSettings.szStatus, csStatus);
#endif

   g_scormEngine.m_scormSettings.bFulltextAsKeyword = (m_cbFulltextAsKeyword.GetCheck() == BST_CHECKED);
	
   g_scormEngine.WriteSettings();

	return CPropertyPage::OnApply();
}

void CScormGeneralPage::OnChangeAnything() 
{
   CPropertyPage::SetModified();
}

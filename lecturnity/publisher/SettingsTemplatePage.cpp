// SettingsTemplatePage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Publisher.h"
#include "SettingsTemplatePage.h"

#include "PublisherLibs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsTemplatePage 


CSettingsTemplatePage::CSettingsTemplatePage(CWnd* pParent /*=NULL*/)
	: CCustomPropPage(CSettingsTemplatePage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsTemplatePage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CSettingsTemplatePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsTemplatePage)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsTemplatePage, CCustomPropPage)
	//{{AFX_MSG_MAP(CSettingsTemplatePage)
	ON_BN_CLICKED(IDC_TEMPLATE_CONFIGURE, OnTemplateConfigure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSettingsTemplatePage 

void CSettingsTemplatePage::OnTemplateConfigure() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	Template_Configure(GetSafeHwnd());
}

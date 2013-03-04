// SettingsSlidesPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Publisher.h"
#include "SettingsSlidesPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsSlidesPage 


CSettingsSlidesPage::CSettingsSlidesPage(CWnd* pParent /*=NULL*/)
	: CCustomPropPage(CSettingsSlidesPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsSlidesPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CSettingsSlidesPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsSlidesPage)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsSlidesPage, CCustomPropPage)
	//{{AFX_MSG_MAP(CSettingsSlidesPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSettingsSlidesPage 

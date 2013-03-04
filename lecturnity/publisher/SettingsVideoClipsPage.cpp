// SettingsVideoClipsPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Publisher.h"
#include "SettingsVideoClipsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSettingsVideoClipsPage 


CSettingsVideoClipsPage::CSettingsVideoClipsPage(CWnd* pParent /*=NULL*/)
	: CCustomPropPage(CSettingsVideoClipsPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsVideoClipsPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CSettingsVideoClipsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsVideoClipsPage)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsVideoClipsPage, CCustomPropPage)
	//{{AFX_MSG_MAP(CSettingsVideoClipsPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSettingsVideoClipsPage 

// NotAdminDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lsupdt32.h"
#include "NotAdminDialog.h"

#include "MfcUtils.h"     // MftUtils

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CNotAdminDialog 


CNotAdminDialog::CNotAdminDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNotAdminDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNotAdminDialog)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CNotAdminDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotAdminDialog)
	DDX_Control(pDX, IDC_DISABLEAUTOUPDATE, m_cbDisableAutoUpdate);
	DDX_Control(pDX, IDC_CHECKINDAYS, m_cbCheckInDays);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNotAdminDialog, CDialog)
	//{{AFX_MSG_MAP(CNotAdminDialog)
	ON_BN_CLICKED(IDC_CHECKINDAYS, OnChangeCheckbox)
	ON_BN_CLICKED(IDC_DISABLEAUTOUPDATE, OnChangeCheckbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CNotAdminDialog 

BOOL CNotAdminDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   DWORD dwIds[] = {IDC_LABEL_NA_INFO,
                    IDC_CHECKINDAYS,
                    IDC_DISABLEAUTOUPDATE,
                    IDOK,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CString csTitle;
   csTitle.LoadString(IDD_NOTADMIN);
   SetWindowText(csTitle);

	// TODO: Zusätzliche Initialisierung hier einfügen

   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();

   CString csCheckInDays;
   csCheckInDays.Format(IDS_MSG_NA_CHECKINDAYS, pDll->GetCheckEvery());
   m_cbCheckInDays.SetWindowText(csCheckInDays);

   m_cbCheckInDays.SetCheck(BST_CHECKED);
   m_bIsCheckInDaysChecked = true;
   m_cbDisableAutoUpdate.SetCheck(BST_UNCHECKED);
   m_bIsDisableAutoUpdateChecked = false;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

bool CNotAdminDialog::IsDisableAutoUpdateChecked()
{
   return m_bIsDisableAutoUpdateChecked;
}

bool CNotAdminDialog::IsCheckInDaysChecked()
{
   return m_bIsCheckInDaysChecked;
}

void CNotAdminDialog::OnChangeCheckbox() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
   m_bIsDisableAutoUpdateChecked = (m_cbDisableAutoUpdate.GetCheck() == BST_CHECKED);
   m_bIsCheckInDaysChecked = (m_cbCheckInDays.GetCheck() == BST_CHECKED);
}

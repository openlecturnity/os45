// updateconfigure.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lsupdt32.h"
#include "resource.h"
#include "updateconfigure.h"
#include "AutoUpdateDialog.h"
#include "MfcUtils.h"     // MftUtils

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CUpdateConfigure 

IMPLEMENT_DYNCREATE(CUpdateConfigure, CPropertyPage)

CUpdateConfigure::CUpdateConfigure() : CPropertyPage(CUpdateConfigure::IDD)
{
	//{{AFX_DATA_INIT(CUpdateConfigure)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   
   m_csCaption.LoadString(CUpdateConfigure::IDD);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   m_hIcon = AfxGetApp()->LoadIcon(IDI_AUTOUPDATE);
   m_nReturnValue = FALSE;
}

CUpdateConfigure::~CUpdateConfigure()
{
}

void CUpdateConfigure::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateConfigure)
	DDX_Control(pDX, IDC_DAYS, m_cbbDays);
	DDX_Control(pDX, IDC_NEWSCHECK, m_cbAnnouncements);
	DDX_Control(pDX, IDC_AUTOUPDATE, m_cbAutoUpdate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateConfigure, CPropertyPage)
	//{{AFX_MSG_MAP(CUpdateConfigure)
	ON_COMMAND(IDC_SEARCHUPDATES, OnSearchUpdates)
	ON_COMMAND(IDC_BUTTON_INSTALL_PATCH, OnInstallPatch)
	ON_BN_CLICKED(IDC_AUTOUPDATE, OnChange)
	ON_BN_CLICKED(IDC_NEWSCHECK, OnChange)
	ON_CBN_SELCHANGE(IDC_DAYS, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CUpdateConfigure 

void CUpdateConfigure::OnSearchUpdates() 
{
   CAutoUpdateDialog aud(this);
   aud.SetForceCheck(true);
   m_nReturnValue = aud.DoCheck();

   // Re-read the settings, which may have changed
   ReadSettings();

   // If an update has been issued, m_nReturnValue has
   // the value TRUE; in that case, this dialog
   // should be closed.
   if (m_nReturnValue == TRUE)
      EndDialog(m_nReturnValue);
}

void CUpdateConfigure::OnInstallPatch()
{
    // This is a bit double code to CAutoUpdateDialog::CheckUpdates()


    if (!LMisc::IsUserAdmin() && !(LMisc::IsWindowsVistaOrHigher() && LMisc::CanUserElevate()))
    {
        if (!LMisc::IsWindowsVistaOrHigher())
        {
            CString csError;
            csError.LoadString(IDS_ERR_NOTADMIN);

            MessageBox(csError, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONERROR);
        }
        // else this is handled (more or less) by LaunchProgram() -> ShellExecute(..., _T("runas"))
    }

    CString csFilters;
    csFilters.LoadString(IDS_TXT_FILTERS);

    CFileDialog dlgOpenPatch(TRUE, _T("msp"), NULL, 0, csFilters, this);
    int iReturnFile = dlgOpenPatch.DoModal();

    if (iReturnFile == IDOK)
    {
        CString csEndStart;
        csEndStart.LoadString(IDS_TXT_ENDSTART);

        int iReturnContinue = MessageBox(csEndStart, _T("LECTURNITY Auto-Update"), MB_OKCANCEL);

        if (iReturnContinue == IDOK)
        {
            CString csSelected = dlgOpenPatch.GetPathName();

            CString csParameter;
            csParameter.Format(_T("/p \"%s\""), csSelected);

            CString csMsiExec = _T("msiexec.exe");

            bool bSuccess = LMisc::LaunchProgram(csMsiExec, csParameter, true);

            if (bSuccess)
                EndDialog(0);
            else
            {
                CString csError;
                csError.LoadString(IDS_ERR_MSIEXEC);
                MessageBox(csError, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONERROR);
            }
        }
    }
}

BOOL CUpdateConfigure::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   DWORD dwIds[] = {IDC_GROUP_SETTINGS,
                    IDC_LABEL_UAN_SETTINGS,
                    IDC_AUTOUPDATE,
                    IDC_NEWSCHECK,
                    IDC_LABEL_UAN_SEARCH,
                    IDC_LABEL_UAN_ALL,
                    IDC_LABEL_UAN_DAYS,
                    IDC_LABEL_UAN_GO,
                    IDC_SEARCHUPDATES,
                    IDC_LABEL_INSTALL_PATCH,
                    IDC_BUTTON_INSTALL_PATCH,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CString csTitle;
   csTitle.LoadString(IDD_AUTOUPDATE);
   SetWindowText(csTitle);

   SetIcon(m_hIcon, TRUE);
   SetIcon(m_hIcon, FALSE);

   ReadSettings();

   // TODO: Zusätzliche Initialisierung hier einfügen
   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CUpdateConfigure::ReadSettings()
{
   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();

   // Update the dialog to match the current settings
   // from the CLsUpdt32Dll object
   m_cbAutoUpdate.SetCheck(pDll->IsAutoUpdateEnabled() ? BST_CHECKED : BST_UNCHECKED);
   m_cbAnnouncements.SetCheck(pDll->IsAnnouncementsEnabled() ? BST_CHECKED : BST_UNCHECKED);

   DWORD dwCheck = pDll->GetCheckEvery();
   CString csCheck;
   csCheck.Format(_T("%d"), dwCheck);
   m_cbbDays.SelectString(-1, csCheck);
}

BOOL CUpdateConfigure::OnApply() 
{
   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();
   pDll->EnableAutoUpdate(m_cbAutoUpdate.GetCheck() == BST_CHECKED ? true : false);
   pDll->EnableAnnouncements(m_cbAnnouncements.GetCheck() == BST_CHECKED ? true : false);
   CString csCheck;
   m_cbbDays.GetLBText(m_cbbDays.GetCurSel(), csCheck);
   DWORD dwCheck = _ttoi(csCheck);
   pDll->SetCheckEvery(dwCheck);
	
	return CPropertyPage::OnApply();
}

void CUpdateConfigure::OnChange() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   SetModified();
}

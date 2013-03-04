// PatchAvailableDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lsupdt32.h"
#include "PatchAvailableDialog.h"

#include "MfcUtils.h"     // MftUtils

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPatchAvailableDialog 


CPatchAvailableDialog::CPatchAvailableDialog(CString csReleaseNotesUrl, int nPatchSize, CWnd* pParent /*=NULL*/)
	: CDialog(CPatchAvailableDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPatchAvailableDialog)
	//}}AFX_DATA_INIT

   m_csReleaseNotesUrl = csReleaseNotesUrl;
   m_nPatchSize        = nPatchSize;
}


void CPatchAvailableDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchAvailableDialog)
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD, m_lbDownload);
	DDX_Control(pDX, IDC_DISABLEAUTOUPDATE, m_cbDisableAutoUpdate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatchAvailableDialog, CDialog)
	//{{AFX_MSG_MAP(CPatchAvailableDialog)
	ON_BN_CLICKED(IDC_RELEASENOTES, OnReleaseNotes)
	ON_BN_CLICKED(IDC_DISABLEAUTOUPDATE, OnChangeCheckbox)
	ON_BN_CLICKED(IDNO, OnNo)
   ON_BN_CLICKED(IDYES, OnYes)
   ON_BN_CLICKED(IDLATER, OnLater)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPatchAvailableDialog 

BOOL CPatchAvailableDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   DWORD dwIds[] = {IDC_LABEL_PA_INFO,
                    IDC_RELEASENOTES,
                    IDC_STATIC_DOWNLOAD,
                    IDC_DISABLEAUTOUPDATE,
                    IDYES,
                    IDNO,
                    IDLATER,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CString csTitle;
   csTitle.LoadString(IDD_PATCHAVAILABLE);
   SetWindowText(csTitle);

	// TODO: Zusätzliche Initialisierung hier einfügen
   double dDownloadSizeMb = ((double) m_nPatchSize) / 1024.0 / 1024.0;
   // It looks stupid if the file size of the patch is less than
   // 100kb: The message would say "... (0.0MB)".
   if (dDownloadSizeMb < 0.1)
      dDownloadSizeMb = 0.1;
   CString csDownloadLabel;
   csDownloadLabel.Format(IDS_MSG_PA_DOWNLOAD, dDownloadSizeMb);
   m_lbDownload.SetWindowText(csDownloadLabel); 

   m_bIsDisableAutoUpdateChecked = false;
   m_bHasDisplayedReleaseNotes = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CPatchAvailableDialog::OnReleaseNotes() 
{
   DWORD dwReturn = (DWORD) ::ShellExecute(GetSafeHwnd(), _T("open"), 
      m_csReleaseNotesUrl, NULL, NULL, SW_SHOW);

   if (dwReturn <= 32)
   {
      CString csError;
      csError.LoadString(IDS_ERR_RELEASENOTES);
      MessageBox(csError, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONWARNING);
   }
   else
   {
      m_bHasDisplayedReleaseNotes = true;
   }
}

void CPatchAvailableDialog::OnChangeCheckbox() 
{
   m_bIsDisableAutoUpdateChecked = (m_cbDisableAutoUpdate.GetCheck() == BST_CHECKED);
}

void CPatchAvailableDialog::OnNo() 
{
	EndDialog(IDNO);
}

void CPatchAvailableDialog::OnYes()
{
   CDialog::OnOK();
}

void CPatchAvailableDialog::OnLater()
{
   CDialog::OnCancel();
}

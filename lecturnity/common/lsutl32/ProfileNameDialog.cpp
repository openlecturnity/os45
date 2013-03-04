// ProfileNameDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ProfileNameDialog.h"
#include "MfcUtils.h"


// CProfileNameDialog-Dialogfeld

IMPLEMENT_DYNAMIC(CProfileNameDialog, CDialog)

CProfileNameDialog::CProfileNameDialog(CString csProfileName, CWnd* pParent /*=NULL*/)
	: CDialog(CProfileNameDialog::IDD, pParent)
{
   m_csProfileName = csProfileName;
}

CProfileNameDialog::~CProfileNameDialog()
{
}

void CProfileNameDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_EDIT_PROFILENAME, m_editName);
}


BEGIN_MESSAGE_MAP(CProfileNameDialog, CDialog)
END_MESSAGE_MAP()


// CProfileNameDialog-Meldungshandler

BOOL CProfileNameDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   CString csCaption;
   csCaption.LoadString(IDS_PROFILE_NAME);
   SetWindowText(csCaption);

   DWORD dwIds[] = {IDC_LABEL_PROFILENAME,
                    IDOK,
                    IDCANCEL,
                    -1};

   MfcUtils::Localize(this, dwIds);

   SetFocus();
   m_editName.SetWindowText(m_csProfileName);
   m_editName.SetSel(0, -1);
   m_editName.SetFocus();
   

   return FALSE;  // return TRUE unless you set the focus to a control
   // AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}

void CProfileNameDialog::OnOK()
{
   CWnd *pNameEdit = GetDlgItem(IDC_EDIT_PROFILENAME);

   pNameEdit->GetWindowText(m_csProfileName);

   CDialog::OnOK();
}

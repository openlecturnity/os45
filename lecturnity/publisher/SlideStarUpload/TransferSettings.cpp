// TransferSettings.cpp : implementation file
//

#include "stdafx.h"
#include "SlideStarUpload.h"
#include "TransferSettings.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransferSettings dialog


CTransferSettings::CTransferSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CTransferSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransferSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTransferSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransferSettings)
	DDX_Control(pDX, IDC_BARBOTTOM, m_stcBarBottom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferSettings, CDialog)
	//{{AFX_MSG_MAP(CTransferSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransferSettings message handlers
void CTransferSettings::DrawLogo()
{
   RECT rct;
   this->GetClientRect(&rct);
   m_stcBarBottom.MoveWindow(0, rct.bottom  - Y_BOTTOM, rct.right, 2, TRUE);
}

BOOL CTransferSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   SetIcon(AfxGetApp()->LoadIcon(IDI_SLIDESTARUPLOAD), TRUE);         // Set big icon

   DrawLogo();	

   RECT rt;
   
   dlg.Create(IDD_SETTINGS_DIALOG, this);
   dlg.ShowWindow(SW_SHOW);
   //dlg.LoadFromRegistry(m_csChannelName);
   dlg.LoadFromRegistry();
   GetClientRect(&rt);
   ::MoveWindow(dlg.m_hWnd, BORDER, BORDER, rt.right - 2*BORDER, rt.bottom - 2*BORDER - 44, TRUE);

   DWORD dwID[] = {IDCANCEL,
                   -1 };
   MfcUtils::Localize(this, dwID);

   CString csCaption;
   csCaption.LoadString(IDD_SETTINGS_DIALOG2);
   this->SetWindowText(csCaption);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransferSettings::OnOK2() 
{
   this->OnOK();
}

void CTransferSettings::OnOK() 
{
	// TODO: Add extra validation here
   //dlg.SaveToRegistry(m_csChannelName);
	dlg.SaveToRegistry();
	CDialog::OnOK();
}

BOOL CTransferSettings::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
   m_csChannel = dlg.m_csChannel;
	
	return CDialog::DestroyWindow();
}

void CTransferSettings::OnCancel() 
{
	// TODO: Add extra cleanup here	
	CDialog::OnCancel();
}

void CTransferSettings::OnCancel2() 
{
   OnCancel();
}


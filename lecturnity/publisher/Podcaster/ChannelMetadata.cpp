// ChannelMetadata.cpp : implementation file
//

#include "stdafx.h"
#include "podcaster.h"
#include "ChannelMetadata.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChannelMetadata dialog


CChannelMetadata::CChannelMetadata(CWnd* pParent /*=NULL*/)
	: CDialog(CChannelMetadata::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChannelMetadata)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChannelMetadata::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelMetadata)
	DDX_Control(pDX, IDC_BARBOTTOM, m_stcBarBottom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChannelMetadata, CDialog)
	//{{AFX_MSG_MAP(CChannelMetadata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannelMetadata message handlers

BOOL CChannelMetadata::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   SetIcon(AfxGetApp()->LoadIcon(IDI_PODCASTER), TRUE);         // Set big icon

   RECT rt;
   dlg.Create(IDD_CHANNELMETADATA2, this);
   dlg.ShowWindow(SW_SHOW);
   GetClientRect(&rt);
   ::MoveWindow(dlg.m_hWnd, BORDER, BORDER, rt.right - 2*BORDER, rt.bottom - 2*BORDER - 44, TRUE);
   
   DrawLogo();
   
   UpdateData();
	dlg.m_csAutor = m_csAutor;
	dlg.m_csCopyright = m_csCopyright;
	dlg.m_csDescription = m_csDescription;
	dlg.m_csLanguage = m_csLanguage;
	dlg.m_csEMail = m_csEMail;
	dlg.m_csOwner = m_csOwner;
	dlg.m_csSubtitle = m_csSubtitle;
	dlg.m_csSummary = m_csSummary;
	dlg.m_csTitle = m_csTitle;
	dlg.m_csExplicit = m_csExplicit;
	dlg.m_csKeyword = m_csKeyword;
	dlg.m_csLink = m_csLink;
	dlg.m_csWebmaster = m_csWebmaster;
   UpdateData(FALSE);

   dlg.Refresh();

   DWORD dwID[] = {IDCANCEL,
                   -1 };
   MfcUtils::Localize(this, dwID);

   CString csCaption;
   csCaption.LoadString(IDD_CHANNELMETADATA);
   this->SetWindowText(csCaption);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChannelMetadata::DrawLogo()
{
   RECT rct;
   this->GetClientRect(&rct);
   m_stcBarBottom.MoveWindow(0, rct.bottom  - 44, rct.right, 2, TRUE);
}


BOOL CChannelMetadata::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DestroyWindow();
}

void CChannelMetadata::OnOK() 
{
	// TODO: Add extra validation here
   dlg.Save();
   UpdateData();
	m_csAutor = dlg.m_csAutor;
	m_csCopyright = dlg.m_csCopyright;
	m_csDescription = dlg.m_csDescription;
	m_csLanguage = dlg.m_csLanguage;
	m_csEMail = dlg.m_csEMail;
	m_csOwner = dlg.m_csOwner;
	m_csSubtitle = dlg.m_csSubtitle;
	m_csSummary = dlg.m_csSummary;
	m_csTitle = dlg.m_csTitle;
	m_csExplicit = dlg.m_csExplicit;
	m_csKeyword = dlg.m_csKeyword;
	m_csLink = dlg.m_csLink;
	m_csWebmaster = dlg.m_csWebmaster;
   UpdateData(FALSE);

   CDialog::OnOK();
}

void CChannelMetadata::OnOK2() 
{
   OnOK();
}

void CChannelMetadata::OnCancel() 
{
	// TODO: Add extra cleanup here	
	CDialog::OnCancel();
}

void CChannelMetadata::OnCancel2() 
{
   OnCancel();
}

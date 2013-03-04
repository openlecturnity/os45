// ChannelMetadata2.cpp : implementation file
//

#include "stdafx.h"
#include "podcaster.h"
#include "ChannelMetadata2.h"
#include "ChannelMetadata.h"
#include "XmlValidator.h"
#include "NewChannel.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChannelMetadata2 dialog


CChannelMetadata2::CChannelMetadata2(CWnd* pParent /*=NULL*/)
	: CDialog(CChannelMetadata2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChannelMetadata2)
	m_csAutor = _T("");
	m_csCopyright = _T("");
	m_csDescription = _T("");
	m_csLanguage = _T("");
	m_csEMail = _T("");
	m_csOwner = _T("");
	m_csSubtitle = _T("");
	m_csSummary = _T("");
	m_csTitle = _T("");
	m_csExplicit = _T("");
	m_csKeyword = _T("");
	m_csLink = _T("");
	m_csWebmaster = _T("");
	//}}AFX_DATA_INIT
}


void CChannelMetadata2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelMetadata2)
	DDX_Control(pDX, IDE_TITLE, m_editTitle);
	DDX_Text(pDX, IDE_AUTHOR, m_csAutor);
	DDX_Text(pDX, IDE_COPYRIGHT, m_csCopyright);
	DDX_Text(pDX, IDE_DESCRIPTION, m_csDescription);
	DDX_Text(pDX, IDE_LANGUAGE, m_csLanguage);
	DDX_Text(pDX, IDE_MAIL, m_csEMail);
	DDX_Text(pDX, IDE_OWNER, m_csOwner);
	DDX_Text(pDX, IDE_SUBTITLE, m_csSubtitle);
	DDX_Text(pDX, IDE_SUMARRY, m_csSummary);
	DDX_Text(pDX, IDE_TITLE, m_csTitle);
	DDX_Text(pDX, IDE_EXPLICIT, m_csExplicit);
	DDX_Text(pDX, IDE_KEYWORDS, m_csKeyword);
	DDX_Text(pDX, IDE_LINK, m_csLink);
	DDX_Text(pDX, IDE_WEBMASTER, m_csWebmaster);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChannelMetadata2, CDialog)
	//{{AFX_MSG_MAP(CChannelMetadata2)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannelMetadata2 message handlers

HBRUSH CChannelMetadata2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
   pDC->SetBkColor(color);
   hbr = (HBRUSH)brush;

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CChannelMetadata2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   ::CoInitialize(NULL);
   color = RGB(255, 255, 255);
   brush.CreateSolidBrush(color);
   
      DWORD dwID[] = {
	      IDL_TITLE,
	      IDL_SUBTITLE,
	      IDL_AUTHOR,
	      IDL_DESCRIPTION,
	      IDL_SUMMARY,
	      IDL_LANGUAGE,
	      IDL_COPYRIGHT,
	      IDL_OWNER,
	      IDL_MAIL,
	      IDL_LINK,
	      IDL_WEBMASTER,
	      IDL_EXPLICIT,
	      IDL_KEYWORDS,
                   -1 };
   MfcUtils::Localize(this, dwID);

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChannelMetadata2::OnOK() 
{
	// TODO: Add extra validation here
   UpdateData();
   UpdateData(FALSE);

   CChannelMetadata *dlg = (CChannelMetadata*) GetParent();
   dlg->OnOK2();
	//CDialog::OnOK();
}

void CChannelMetadata2::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
   UpdateData();
   UpdateData(FALSE);
	
}

void CChannelMetadata2::Refresh()
{
   UpdateData(FALSE);
}

void CChannelMetadata2::Save()
{
   UpdateData();
   UpdateData(FALSE);
}

void CChannelMetadata2::OnCancel() 
{
	// TODO: Add extra cleanup here
	//CDialog::OnCancel();
   CChannelMetadata *dlg = (CChannelMetadata*) GetParent();
   dlg->OnCancel2();
}

BOOL CChannelMetadata2::CreateNewChannel(LPCTSTR tstrNewChannel) 
{
   BOOL bResult = FALSE;
   
   CNewChannel chn;
   
   chn.m_csAutor       = m_csAutor;
	chn.m_csCopyright   = m_csCopyright;
	chn.m_csDescription = m_csDescription;
	chn.m_csLanguage    = m_csLanguage;
	chn.m_csEMail       = m_csEMail;
	chn.m_csOwner       = m_csOwner;
	chn.m_csSubtitle    = m_csSubtitle;
	chn.m_csSummary     = m_csSummary;
	chn.m_csTitle       = m_csTitle;
	chn.m_csExplicit    = m_csExplicit;
	chn.m_csKeyword     = m_csKeyword;
	chn.m_csLink        = m_csLink;
	chn.m_csWebmaster   = m_csWebmaster;

   bResult = chn.Create(tstrNewChannel);

   return bResult;

}

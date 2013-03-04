// MetadataPage.cpp : implementation file
//

#include "stdafx.h"
#include "podcaster.h"
#include "MetadataPage.h"
#include "ChannelMetadata.h"
#include "GetMetadata.h"
#include "SetMetadata.h"
#include "MyTabCtrl.h"
#include "TransferPage.h"
#include "GeneralSettings.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMetadataPage dialog


CMetadataPage::CMetadataPage(CWnd* pParent /*=NULL*/)
	: CDialog(CMetadataPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMetadataPage)
	m_csAuthor = _T("??");
	m_csCopyright = _T("??");
	m_csEmail = _T("??");
	m_csDescription = _T("??");
	m_csExplicit = _T("??");
	m_csKeywords = _T("??");
	m_csLanguage = _T("??");
	m_csLink = _T("??");
	m_csOwner = _T("??");
	m_csSubtitle = _T("??");
	m_csSummary = _T("??");
	m_csTitle = _T("??");
	m_csWebmaster = _T("??");
	//}}AFX_DATA_INIT
}


void CMetadataPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMetadataPage)
	DDX_Control(pDX, IDB_CHANGE, m_btnChange);
	DDX_Text(pDX, IDC_AUTHOR, m_csAuthor);
	DDX_Text(pDX, IDC_COPYRIGHT, m_csCopyright);
	DDX_Text(pDX, IDC_EMAIL, m_csEmail);
	DDX_Text(pDX, IDC_DESCRIPTION, m_csDescription);
	DDX_Text(pDX, IDC_EXPLICIT, m_csExplicit);
	DDX_Text(pDX, IDC_KEYWORDS, m_csKeywords);
	DDX_Text(pDX, IDC_LANGUAGE, m_csLanguage);
	DDX_Text(pDX, IDC_LINK, m_csLink);
	DDX_Text(pDX, IDC_OWNER, m_csOwner);
	DDX_Text(pDX, IDC_SUBTITLE, m_csSubtitle);
	DDX_Text(pDX, IDC_SUMMARY, m_csSummary);
	DDX_Text(pDX, IDC_TITLE, m_csTitle);
	DDX_Text(pDX, IDC_WEBMASTER, m_csWebmaster);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMetadataPage, CDialog)
	//{{AFX_MSG_MAP(CMetadataPage)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDB_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMetadataPage message handlers

HBRUSH CMetadataPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
   pDC->SetBkColor(color);
   hbr = (HBRUSH)brush;
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CMetadataPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   ::CoInitialize(NULL);
   pApp = (CPodcasterApp *)AfxGetApp();

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
	      IDB_CHANGE,
                   -1 };
   MfcUtils::Localize(this, dwID);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMetadataPage::OnChange() 
{
	// TODO: Add your control notification handler code here

	CChannelMetadata dlg;
   UpdateData();
	dlg.m_csAutor       = m_csAuthor;
	dlg.m_csCopyright   = m_csCopyright;
	dlg.m_csDescription = m_csDescription;
	dlg.m_csLanguage    = m_csLanguage;
	dlg.m_csEMail       = m_csEmail;
	dlg.m_csOwner       = m_csOwner;
	dlg.m_csSubtitle    = m_csSubtitle;
	dlg.m_csSummary     = m_csSummary;
	dlg.m_csTitle       = m_csTitle;
	dlg.m_csExplicit    = m_csExplicit;
	dlg.m_csKeyword     = m_csKeywords;
	dlg.m_csLink        = m_csLink;
	dlg.m_csWebmaster   = m_csWebmaster;
   UpdateData(FALSE);

   if(dlg.DoModal() == IDOK)
   {
      UpdateData();
	   m_csAuthor       = dlg.m_csAutor;
	   m_csCopyright    = dlg.m_csCopyright;
	   m_csDescription  = dlg.m_csDescription;
	   m_csLanguage     = dlg.m_csLanguage;
	   m_csEmail        = dlg.m_csEMail;
	   m_csOwner        = dlg.m_csOwner;
	   m_csSubtitle     = dlg.m_csSubtitle;
	   m_csSummary      = dlg.m_csSummary;
	   m_csTitle        = dlg.m_csTitle;
	   m_csExplicit     = dlg.m_csExplicit;
	   m_csKeywords     = dlg.m_csKeyword;
	   m_csLink         = dlg.m_csLink;
	   m_csWebmaster    = dlg.m_csWebmaster;
      UpdateData(FALSE);

      CSetMetadata smd;
	   smd.csAuthor      = m_csAuthor;
	   smd.csCopyright   = m_csCopyright;
	   smd.csDescription = m_csDescription;
	   smd.csLanguage    = m_csLanguage;
	   smd.csEMail       = m_csEmail;
	   smd.csOwner       = m_csOwner;
	   smd.csSubtitle    = m_csSubtitle;
	   smd.csSummary     = m_csSummary;
	   smd.csTitle       = m_csTitle;
	   smd.csExplicit    = m_csExplicit;
	   smd.csKeywords    = m_csKeywords;
	   smd.csLink        = m_csLink;
	   smd.csWebMaster   = m_csWebmaster;
      
      CGeneralSettings *tab = (CGeneralSettings *)this->GetParent();
      CTransferPage *tpag = (CTransferPage *)tab->m_TabControl.m_Dialog[1];
      // use temporary storage
      CString csFile = pApp->csTmpDirectory + _T("\\mod.xml");
      if(smd.Parse(tpag->m_csChannel, csFile) == TRUE)
      {
         tab->UploadChannel(tpag->m_csChannelName, csFile);
         GetMetadata(tpag->m_csChannel);
      }
      tab->Refresh();

   }
}

BOOL CMetadataPage::GetMetadata(CString csUrl, BOOL bDisplayError)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CGetMetadata meta;
    BOOL bResult = meta.Parse(csUrl, bDisplayError);

    UpdateData();
    m_csAuthor = meta.csAuthor;
	m_csCopyright = meta.csCopyright;
	m_csEmail = meta.csEMail;
	m_csDescription = meta.csDescription;
	m_csExplicit = meta.csExplicit;
	m_csKeywords = meta.csKeywords;
	m_csLanguage = meta.csLanguage;
	m_csLink = meta.csLink;
	m_csOwner = meta.csOwner;
	m_csSubtitle = meta.csSubtitle;
	m_csSummary = meta.csSummary;
	m_csTitle = meta.csTitle;
	m_csWebmaster = meta.csWebMaster;
    UpdateData(FALSE);
   	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	return bResult;
}

void CMetadataPage::OnCancel() 
{
	// TODO: Add extra cleanup here
	//CDialog::OnCancel();
}

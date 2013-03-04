// MetadataProps.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "MetadataProps.h"
#include "MainFrm.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CMetadataProps 

IMPLEMENT_DYNCREATE(CMetadataProps, CPropertyPage)

CMetadataProps::CMetadataProps() : CPropertyPage(CMetadataProps::IDD)
{
	//{{AFX_DATA_INIT(CMetadataProps)
	//}}AFX_DATA_INIT
   
   m_csCaption.LoadString(CMetadataProps::IDD);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;
}

CMetadataProps::~CMetadataProps()
{
}

void CMetadataProps::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMetadataProps)
	DDX_Control(pDX, IDC_MD_CODEPAGE, m_edCodepage);
	DDX_Control(pDX, IDC_MD_TITLE_EDIT, m_titleEdit);
	DDX_Control(pDX, IDC_MD_PRODUCER_EDIT, m_producerEdit);
	DDX_Control(pDX, IDC_MD_KEYWORDS_EDIT, m_keywordsEdit);
	DDX_Control(pDX, IDC_MD_AUTHOR_EDIT, m_authorEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMetadataProps, CPropertyPage)
	//{{AFX_MSG_MAP(CMetadataProps)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CMetadataProps 

BOOL CMetadataProps::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
   DWORD dwIds[] = {IDC_MD_AUTHOR,
                    IDC_MD_TITLE,
                    IDC_MD_PRODUCER,
                    IDC_MD_KEYWORD,
                    IDC_LABEL_CODEPAGE,
                    -1};
   MfcUtils::Localize(this, dwIds);

	// TODO: Zusätzliche Initialisierung hier einfügen
   m_pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
	
   m_authorEdit.SetWindowText(m_pDoc->project.m_targetMetainfo.GetAuthor());
   m_titleEdit.SetWindowText(m_pDoc->project.m_targetMetainfo.GetTitle());
   m_producerEdit.SetWindowText(m_pDoc->project.m_targetMetainfo.GetCreator());
   CString keywords;
   m_pDoc->project.m_targetMetainfo.GetKeywords(keywords);
   m_keywordsEdit.SetWindowText(keywords);

   CString csCodepage;
   csCodepage.Format(_T("%d"), m_pDoc->project.GetCodePage());
   m_edCodepage.SetWindowText(csCodepage);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CMetadataProps::OnOK() 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen

   // MessageBox("CMetadataProps::OnOK()");
   CString title;
   CString author;
   CString producer;
   CString keywords;
   m_titleEdit.GetWindowText(title);
   m_authorEdit.GetWindowText(author);
   m_producerEdit.GetWindowText(producer);
   m_keywordsEdit.GetWindowText(keywords);

   m_pDoc->project.m_targetMetainfo.SetTitle(title);
   m_pDoc->project.m_targetMetainfo.SetAuthor(author);
   m_pDoc->project.m_targetMetainfo.SetCreator(producer);
   m_pDoc->project.m_targetMetainfo.SetKeywords(keywords);
   
   // The document has changed, as the metadata have
   // changed. Note that preview does not have to be
   // rebuilt!
   m_pDoc->SetModifiedFlag();

   CPropertyPage::OnOK();
}


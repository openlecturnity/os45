// PageMetadata.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "PageMetadata.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CPageMetadata 

IMPLEMENT_DYNCREATE(CPageMetadata, CPropertyPage)

CPageMetadata::CPageMetadata() : CPropertyPage(CPageMetadata::IDD)
{
	//{{AFX_DATA_INIT(CPageMetadata)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_csCaption.LoadString(CPageMetadata::IDD);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   m_pPage = NULL;
   m_pClip = NULL;
}

CPageMetadata::~CPageMetadata()
{
}

void CPageMetadata::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMetadata)
	DDX_Control(pDX, IDC_MD_TITLE_EDIT, m_edTitle);
	DDX_Control(pDX, IDC_MD_KEYWORDS_EDIT, m_edKeywords);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageMetadata, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMetadata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPageMetadata 

BOOL CPageMetadata::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
   DWORD dwIds[] = {IDC_MD_TITLE,
                    IDC_MD_KEYWORD,
                    -1};
   MfcUtils::Localize(this, dwIds);
	if (m_pPage)
   {
      m_edTitle.SetWindowText(m_pPage->GetTitle());
      CStringList keywordList;
      m_pPage->GetKeywords(keywordList);
      POSITION pos = keywordList.GetHeadPosition();
      CString csKeywords;
      while (pos)
      {
         csKeywords += keywordList.GetNext(pos);
         if (pos)
            csKeywords += "; ";
      }
      m_edKeywords.SetWindowText(csKeywords);
      keywordList.RemoveAll();
   }
	if (m_pClip)
   {
      m_edTitle.SetWindowText(m_pClip->GetTitle());
      CString csKeywords;
      m_pClip->GetKeywords(csKeywords);
      m_edKeywords.SetWindowText(csKeywords);
   }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CPageMetadata::SetCurrentPage(CPageInformation *pPage)
{
   m_pPage = pPage;
}

void CPageMetadata::SetCurrentClip(CClipInfo *pClip)
{
   m_pClip = pClip;
}

void CPageMetadata::OnOK() 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
   CString szTitle;
   m_edTitle.GetWindowText(szTitle);
   
   CString szKeywords;
   m_edKeywords.GetWindowText(szKeywords);
   CStringList keywordList;
   int nStart = 0;
   int iNextPos = 0;
   while (iNextPos != -1)
   {
      iNextPos = szKeywords.Find(_T(";"), nStart);
      if (iNextPos != -1)
      {
         CString szKeyword = szKeywords.Mid(nStart, iNextPos-nStart);
         szKeyword.TrimLeft(_T(" "));
         keywordList.AddTail(szKeyword);
         nStart = iNextPos + 1;
      }
      else 
      {
         CString szKeyword = szKeywords.Mid(nStart);
         szKeyword.TrimLeft(_T(" "));
         if (!szKeyword.IsEmpty())
            keywordList.AddTail(szKeyword);
      }
   }

   if (m_pPage)
   {
      m_pPage->SetTitle(szTitle);
      m_pPage->SetKeywords(keywordList);
   }

   if (m_pClip)
   {
      m_pClip->SetTitle(szTitle);
      m_pClip->SetKeywords(keywordList);
   }

	CPropertyPage::OnOK();
}

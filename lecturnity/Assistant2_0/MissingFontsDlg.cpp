// MissingFontsDlg.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "MainFrm.h"
#include "MissingFontsDlg.h"
#include "MfcUtils.h" // MfcUtils::Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMissingFontsDlg dialog


CMissingFontsDlg::CMissingFontsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMissingFontsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMissingFontsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMissingFontsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMissingFontsDlg)
	DDX_Control(pDX, IDC_FONT_MISSING_LIST, m_MissingFontsList);
   DDX_Control(pDX, IDC_DO_NOT_DISPLAY_DIALOG_AGAIN , m_cbbDoNotDisplayAgain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMissingFontsDlg, CDialog)
	//{{AFX_MSG_MAP(CMissingFontsDlg)
	ON_BN_CLICKED(IDOK, OnDialogMissingFontsOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMissingFontsDlg message handlers
BOOL CMissingFontsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CString csCap;
   csCap.LoadString(IDS_WARNING);
   SetWindowText(csCap);

   DWORD dwIds[] = {IDC_WARNING_MISSING_FONTS,
                    IDC_DO_NOT_DISPLAY_DIALOG_AGAIN,
                    IDOK,
                    -1};
   MfcUtils::Localize(this, dwIds);
	
   m_MissingFontsList.SetExtendedStyle(LVS_EX_GRIDLINES |LVS_EX_FULLROWSELECT);
   

   CRect listRect;
	m_MissingFontsList.GetClientRect(&listRect);

   LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT ;


   CString csString;


   if (FALSE == csString.LoadString(IDS_FONT_WARNING_FONT))
   {
      csString = _T("Font");
   }

	lvc.pszText = csString.GetBuffer(csString.GetLength());
	lvc.cx = listRect.Width() - 160;
	lvc.fmt = LVCFMT_LEFT;
	m_MissingFontsList.InsertColumn(0,&lvc);
	

	
   if (FALSE == csString.LoadString(IDS_FONT_WARNING_PAGE))
   {
      csString = _T("Page(s)");
   }

	lvc.pszText = csString.GetBuffer(csString.GetLength());
	lvc.cx = 60;
	lvc.fmt = LVCFMT_LEFT;
	m_MissingFontsList.InsertColumn(1,&lvc);


   if (FALSE == csString.LoadString(IDS_FONT_WARNING_STATUS))
   {
      csString = _T("Status");
   }

	lvc.pszText = csString.GetBuffer(csString.GetLength());
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	m_MissingFontsList.InsertColumn(2,&lvc);


   PopulateMissingFontsList();
     
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CMissingFontsDlg::CheckCheckBox()
{
   if (m_cbbDoNotDisplayAgain.GetCheck() == BST_CHECKED)
   {      
      CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
      pMainFrame->SetShowMissingFontsWarning(false);
   }
}


void CMissingFontsDlg::OnDialogMissingFontsOk() 
{
	// TODO: Add your control notification handler code here
CheckCheckBox();
EndDialog(IDYES);	
}


void CMissingFontsDlg::AddMissingFont(CString &csFontName, CString &csPages, CString &csType)
{
   psMissingFontInfo apMissingFont = new sMissingFontInfo();
   apMissingFont->csFontName = csFontName;
   apMissingFont->csPages = csPages;
   apMissingFont->csType = csType;
   m_MissingFontsArray.Add(apMissingFont);
}


void CMissingFontsDlg::PopulateMissingFontsList()
{
for(int i=0; i<m_MissingFontsArray.GetSize(); i++)
   {
   psMissingFontInfo apMissingFont = m_MissingFontsArray.GetAt(i);

   LV_ITEM lvi1;   
   
   lvi1.mask = LVIF_TEXT;
   lvi1.iItem = m_MissingFontsList.GetItemCount();
   lvi1.iSubItem = 0;
   lvi1.pszText = apMissingFont->csFontName.GetBuffer(apMissingFont->csFontName.GetLength()) ;
   apMissingFont->csFontName.ReleaseBuffer();
   lvi1.iImage = 0;
   lvi1.stateMask = 0;
   lvi1.state = 0;
   m_MissingFontsList.InsertItem(&lvi1);

   
   lvi1.mask = LVIF_TEXT;
   lvi1.iSubItem = 1;
   lvi1.pszText = apMissingFont->csPages.GetBuffer(apMissingFont->csPages.GetLength()) ;
   apMissingFont->csPages.ReleaseBuffer();
   lvi1.iImage = 0;
   lvi1.stateMask = 0;
   lvi1.state = 0;
   m_MissingFontsList.SetItem(&lvi1);

   lvi1.mask = LVIF_TEXT;
   lvi1.iSubItem = 2; 
   lvi1.pszText = apMissingFont->csType.GetBuffer(apMissingFont->csType.GetLength()) ;
   apMissingFont->csType.ReleaseBuffer();
   lvi1.iImage = 0;
   lvi1.stateMask = 0;
   lvi1.state = 0;
   m_MissingFontsList.SetItem(&lvi1);

   delete apMissingFont;
   }
m_MissingFontsArray.RemoveAll();
}

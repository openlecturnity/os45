// SGSettings.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "SGSettings.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSGSettings

IMPLEMENT_DYNAMIC(CSGSettings, CPropertySheet)

CSGSettings::CSGSettings(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   Init();
}

CSGSettings::CSGSettings(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   Init();
}

CSGSettings::CSGSettings()
{
   Init();
}

CSGSettings::~CSGSettings()
{
   delete(m_pPageSettings);
   delete(m_pPageHandling);
   delete(m_pPageHotkeys);
   delete(m_pPageCodec);

   delete(m_csTitleSettings);
   delete(m_csTitleHandling);
   delete(m_csTitleHotkeys);
   delete(m_csTitleCodec);
}


BEGIN_MESSAGE_MAP(CSGSettings, CPropertySheet)
	//{{AFX_MSG_MAP(CSGSettings)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSGSettings message handlers

void CSGSettings::Init()
{
  m_pPageSettings = new CSGSettingsDlg();
  m_pPageHandling = new CSGHandlingDlg();
  m_pPageHotkeys  = new CSGHotkeysDlg();
  m_pPageCodec    = new CSGCodecDlg();
  
  m_csTitleSettings = new CString();
  m_csTitleHandling = new CString();
  m_csTitleHotkeys = new CString();
  m_csTitleCodec = new CString();

  m_csTitleSettings->LoadString(IDD_DIALOG_SG_SETTINGS);
  m_csTitleHandling->LoadString(IDD_DIALOG_SG_HANDLING);
  m_csTitleHotkeys->LoadString(IDD_DIALOG_SG_HOTKEYS);
  m_csTitleCodec->LoadString(IDD_DIALOG_SG_CODEC);
  
  LPTSTR pstSettings = m_csTitleSettings->GetBuffer(m_csTitleSettings->GetLength());
  LPTSTR pstHandling = m_csTitleHandling->GetBuffer(m_csTitleHandling->GetLength());
  LPTSTR pstHotkeys = m_csTitleHotkeys->GetBuffer(m_csTitleHotkeys->GetLength());
  LPTSTR pstCodec = m_csTitleCodec->GetBuffer(m_csTitleCodec->GetLength());
    
  m_pPageSettings->m_psp.dwFlags &= ~PSP_HASHELP;
  m_pPageSettings->m_psp.dwFlags |= PSP_USETITLE;
  m_pPageSettings->m_psp.pszTitle = pstSettings; 

  m_pPageHandling->m_psp.dwFlags &= ~PSP_HASHELP;
  m_pPageHandling->m_psp.dwFlags |= PSP_USETITLE;
  m_pPageHandling->m_psp.pszTitle = pstHandling;

  m_pPageHotkeys->m_psp.dwFlags &= ~PSP_HASHELP;
  m_pPageHotkeys->m_psp.dwFlags |= PSP_USETITLE;
  m_pPageHotkeys->m_psp.pszTitle = pstHotkeys;

  m_pPageCodec->m_psp.dwFlags &= ~PSP_HASHELP;
  m_pPageCodec->m_psp.dwFlags |= PSP_USETITLE;
  m_pPageCodec->m_psp.pszTitle = pstCodec;

  AddPage(m_pPageSettings);
  AddPage(m_pPageHandling);
  AddPage(m_pPageHotkeys);
  AddPage(m_pPageCodec);

/*  delete(csTitleSettings);
  delete(csTitleHandling);
  delete(csTitleHotkeys);
  delete(csTitleCodec);*/

}

void CSGSettings::Init(CAssistantDoc *pAssistantDoc)
{
   m_pPageSettings->Init(pAssistantDoc);
   m_pPageHandling->Init(pAssistantDoc);
   m_pPageHotkeys->Init(pAssistantDoc);
   m_pPageCodec->Init(pAssistantDoc);
}

BOOL CSGSettings::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	return CPropertySheet::OnCommand(wParam, lParam);
}

BOOL CSGSettings::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
	DWORD dwIds[] = {IDOK,
                    IDCANCEL,
                   -1};
   MfcUtils::Localize(this, dwIds);
   
   CString csTitle;
   csTitle.LoadString(IDS_SG_SETTINGS_TITLE);
   SetWindowText(csTitle);
   SetActivePage(3);
   SetActivePage(2);
   SetActivePage(1);
   SetActivePage(0);
	return bResult;
}

BOOL CSGSettings::ContinueModal() 
{
	// TODO: Add your specialized code here and/or call the base class
   CPropertyPage* pPropertyPage = GetActivePage();
   if(pPropertyPage != NULL)
   {
      pPropertyPage->SendMessage(WM_KICKIDLE, 0, 0);
   }
	
	return CPropertySheet::ContinueModal();
}

void CSGSettings::SaveSettings()
{
   m_pPageCodec->OnOK();
   m_pPageHandling->OnOK();
   m_pPageHotkeys->OnOK();
   m_pPageSettings->OnOK();
}
//BOOL CSGSettings::Create(CWnd* pParentWnd , DWORD dwStyle , DWORD dwExStyle)
//{
//   // TODO: Add your specialized code here and/or call the base class
//   //Init();
//   return CPropertySheet::Create(pParentWnd, dwStyle, dwExStyle);
//}

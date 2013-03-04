// AVCSettings.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "MainFrm.h"
#include "AVCSettings.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAVCSettings

IMPLEMENT_DYNAMIC(CAVCSettings, CPropertySheet)

CAVCSettings::CAVCSettings(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   Init();
   m_nActiveTab = 0;
}

CAVCSettings::CAVCSettings(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   Init();
   m_nActiveTab = 0;
}
CAVCSettings::CAVCSettings()
{
   Init();
   m_nActiveTab = 0;
}

CAVCSettings::~CAVCSettings()
{
   delete(m_pPageAudio);
   delete(m_pPageVideo);
   delete(m_pPageCodec);

   delete(m_csTitleAudio);
   delete(m_csTitleVideo);
   delete(m_csTitleCodec);
}


BEGIN_MESSAGE_MAP(CAVCSettings, CPropertySheet)
	//{{AFX_MSG_MAP(CAVCSettings)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAVCSettings message handlers

void CAVCSettings::Init()
{
   m_pPageAudio = new CAudioSettings();
   m_pPageVideo = new CVideoSettings();
   m_pPageCodec = new CCodecSettings();
   //m_pPageTest = new CSGSettingsSheet();
   
   m_csTitleAudio = new CString();
   m_csTitleVideo = new CString();
   m_csTitleCodec = new CString();

   m_csTitleAudio->LoadString(IDD_DIALOG_AUDIO);
   m_csTitleVideo->LoadString(IDD_DIALOG_VIDEO);
   m_csTitleCodec->LoadString(IDD_DIALOG_CODEC);

   LPTSTR pstAudio = m_csTitleAudio->GetBuffer(m_csTitleAudio->GetLength());
   LPTSTR pstVideo = m_csTitleVideo->GetBuffer(m_csTitleVideo->GetLength());
   LPTSTR pstCodec = m_csTitleCodec->GetBuffer(m_csTitleCodec->GetLength());

   m_pPageAudio->m_psp.dwFlags &= ~PSP_HASHELP;
   m_pPageAudio->m_psp.dwFlags |= PSP_USETITLE;
   m_pPageAudio->m_psp.pszTitle = pstAudio; 
 
   m_pPageVideo->m_psp.dwFlags &= ~PSP_HASHELP;
   m_pPageVideo->m_psp.dwFlags |= PSP_USETITLE;
   m_pPageVideo->m_psp.pszTitle = pstVideo;
   
   m_pPageCodec->m_psp.dwFlags &= ~PSP_HASHELP;
   m_pPageCodec->m_psp.dwFlags |= PSP_USETITLE;
   m_pPageCodec->m_psp.pszTitle = pstCodec;

   //m_pPageTest->m_psp.dwFlags &= ~PSP_HASHELP;

   AddPage(m_pPageAudio);
   AddPage(m_pPageVideo);
   AddPage(m_pPageCodec);
   //AddPage(m_pPageTest);
   
      
   m_bIsTabCodecEnabled = false;
   
}

BOOL CAVCSettings::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
//	m_tabCtrl.SetExtendedStyle(m_tabCtrl.GetExtendedStyle()|WS_EX_STATICEDGE);
   DWORD dwIds[] = {IDOK,
                    IDCANCEL,
                   -1};
   MfcUtils::Localize(this, dwIds);
   SetActivePage(2);
   SetActivePage(1);
   SetActivePage(0);
   
   m_tabCtrl.Install(this);
   m_tabCtrl.EnableItem(2, m_bIsTabCodecEnabled);

   if((m_nActiveTab == 0) || (m_nActiveTab == 1))
   {
      SetActivePage(m_nActiveTab);
   }
	return bResult;
}

void CAVCSettings::EnableCodecTab(bool bState)
{
   m_tabCtrl.EnableItem(2, bState);
   m_bIsTabCodecEnabled = bState;
}

void CAVCSettings::UpdateCodecPage()
{
   m_pPageCodec->UpdatePage();
}

void CAVCSettings::Init(CAssistantDoc *pAssistantDoc, int nActiveTab)
{
   m_pPageAudio->Init(pAssistantDoc);
   m_pPageVideo->Init(pAssistantDoc);
   m_pPageCodec->Init(pAssistantDoc);
   //m_pPageTest->Init(pAssistantDoc);
   m_nActiveTab = nActiveTab;
}

void CAVCSettings::SaveSettings()
{
   m_pPageAudio->OnOK();
   m_pPageVideo->OnOK();
   m_pPageCodec->OnOK();
}

BOOL CAVCSettings::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(LOWORD(wParam) == IDOK)
   {
      
   }
	return CPropertySheet::OnCommand(wParam, lParam);
}

BOOL CAVCSettings::ContinueModal() 
{
	// TODO: Add your specialized code here and/or call the base class
   CPropertyPage* pPropertyPage = GetActivePage();
   if(pPropertyPage != NULL)
   {
      pPropertyPage->SendMessage(WM_KICKIDLE, 0, 0);
   }
	
   if((m_pPageVideo != NULL) && (m_pPageCodec != NULL))
   {
      EnableCodecTab(m_pPageVideo->m_chkRecordVideo.GetCheck() == 0 ? false: true);
   }
	return CPropertySheet::ContinueModal();
}

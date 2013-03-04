// AssistantSettings.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "..\Studio\Studio.h"
#endif
#include "AssistantSettings.h"


// CAssistantSettings

IMPLEMENT_DYNAMIC(CAssistantSettings, CXTPPropertySheet)

CAssistantSettings::CAssistantSettings(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CXTPPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   Init(NULL, iSelectPage, false);
}

CAssistantSettings::CAssistantSettings(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CXTPPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   Init(NULL, iSelectPage, false);
}

CAssistantSettings::CAssistantSettings(LPCTSTR pszCaption, CXTPCommandBars* pCommandBars, CWnd* pParentWnd, UINT iSelectPage, bool bCustom, bool bLiveContext)
	:CXTPPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   Init(pCommandBars, iSelectPage, bCustom, bLiveContext);
}

CAssistantSettings::~CAssistantSettings()
{
   if(m_pPageMisc)
      delete(m_pPageMisc);
   if(m_pPageCustomize)
      delete(m_pPageCustomize);
   if(m_pPageAudioVideo)
      delete(m_pPageAudioVideo);
   if(m_pPageScreengrabbing)
      delete(m_pPageScreengrabbing);

   if(m_pcsTitleMisc)
      delete(m_pcsTitleMisc);
   if(m_pcsTitleCustomize)
      delete(m_pcsTitleCustomize);
   if(m_pcsTitleAudioVideo)
      delete(m_pcsTitleAudioVideo);
   if(m_pcsTitleScreengrabbing)
      delete(m_pcsTitleScreengrabbing);
}


BEGIN_MESSAGE_MAP(CAssistantSettings, CXTPPropertySheet)
END_MESSAGE_MAP()


// CAssistantSettings message handlers

BOOL CAssistantSettings::OnInitDialog()
{
   BOOL bResult = CXTPPropertySheet::OnInitDialog();

   // TODO:  Add your specialized code here
  
   /*SetActivePage(3);
   SetActivePage(2);
   SetActivePage(1);*/
   SetActivePage(m_nSelectedPage);

  /* m_tabCtrl.Install(this);
   m_tabCtrl.SetItemSize(CSize(0, 20)); 
   m_tabCtrl.EnableItem(0, false);
   m_tabCtrl.EnableItem(1, false);
   m_tabCtrl.EnableItem(2, !m_pDocument->IsRecording());
   m_tabCtrl.EnableItem(3, !m_pDocument->IsRecording());*/
   return bResult;
}

BOOL CAssistantSettings::ContinueModal()
{
   // TODO: Add your specialized code here and/or call the base class
   CXTPPropertyPage* pPropertyPage = GetActivePage();
   if(pPropertyPage != NULL)
   {
      pPropertyPage->SendMessage(WM_KICKIDLE, 0, 0);
   }

   /*if(m_pPageScreengrabbing)
      m_tabCtrl.EnableItem(2, !m_pDocument->IsRecording());
   if(m_pPageAudioVideo)
      m_tabCtrl.EnableItem(3, !m_pDocument->IsRecording());*/
   return CXTPPropertySheet::ContinueModal();
}

void CAssistantSettings::Init(CXTPCommandBars* pCommandBars, UINT nSelectedPage, bool bCustom, bool bLiveContext)
{
   if (!bLiveContext)
       m_pPageMisc = new CMiscSettings();
   else
       m_pPageMisc = NULL;

   if (!bLiveContext)
       m_pPageCustomize = new CCustomizeSheet(pCommandBars);
   else
       m_pPageCustomize = NULL;
   m_pPageScreengrabbing = new CSGSettingsSheet();
   m_pPageAudioVideo = new CAVSettingsSheet();
   
   m_pcsTitleMisc = new CString();
   m_pcsTitleCustomize = new CString();
   m_pcsTitleScreengrabbing = new CString();
   m_pcsTitleAudioVideo = new CString();
  
   m_pcsTitleMisc->LoadString(IDD_DIALOG_MISC_SETTINGS);
   m_pcsTitleCustomize->LoadString(IDD_DIALOG_CUSTOMIZE_SETTINGS);
   m_pcsTitleScreengrabbing->LoadString(IDD_DIALOG_SGB_SETTINGS);
   m_pcsTitleAudioVideo->LoadString(IDD_DIALOG_AV_SETTINGS);

   LPTSTR pstMisc = m_pcsTitleMisc->GetBuffer(m_pcsTitleMisc->GetLength());
   LPTSTR pstCustomize = m_pcsTitleCustomize->GetBuffer(m_pcsTitleCustomize->GetLength());
   LPTSTR pstScreengrabbing = m_pcsTitleScreengrabbing->GetBuffer(m_pcsTitleScreengrabbing->GetLength());
   LPTSTR pstAudioVideo = m_pcsTitleAudioVideo->GetBuffer(m_pcsTitleAudioVideo->GetLength());

   if (m_pPageMisc != NULL) {
       m_pPageMisc->m_psp.dwFlags &= ~PSP_HASHELP;
       m_pPageMisc->m_psp.dwFlags |= PSP_USETITLE;
       m_pPageMisc->m_psp.pszTitle = pstMisc;
   }

   if (m_pPageCustomize != NULL) {
       m_pPageCustomize->m_psp.dwFlags &= ~PSP_HASHELP;
       m_pPageCustomize->m_psp.dwFlags |= PSP_USETITLE;
       m_pPageCustomize->m_psp.pszTitle = pstCustomize;
   }

   m_pPageScreengrabbing->m_psp.dwFlags &= ~PSP_HASHELP;
   m_pPageScreengrabbing->m_psp.dwFlags |= PSP_USETITLE;
   m_pPageScreengrabbing->m_psp.pszTitle = pstScreengrabbing;

   m_pPageAudioVideo->m_psp.dwFlags &= ~PSP_HASHELP;
   m_pPageAudioVideo->m_psp.dwFlags |= PSP_USETITLE;
   m_pPageAudioVideo->m_psp.pszTitle = pstAudioVideo;

   CXTPPropertyPageListNavigator* pList = new CXTPPropertyPageListNavigator();

   pList->m_szControl = CSize(GetMaxStringWidth(), 0);
   pList->SetListStyle(xtListBoxOffice2007);
   SetNavigator(pList);

   SetResizable();
   SetPageSize(CSize(481, 353));
   UINT nID = IDR_MAINFRAME;
#ifdef _STUDIO
   if(!bCustom)
       AddPage(m_pPageAudioVideo);
   if(!bCustom)
       AddPage(m_pPageScreengrabbing);

   if (m_pPageCustomize != NULL) {
       AddPage(m_pPageCustomize);
       CStudioApp* pApp = (CStudioApp*)AfxGetApp();
       if(pApp->GetCurrentMode() == CStudioApp::MODE_EDITOR)
       {
           nID = IDR_MAINFRAME;   
           m_pPageCustomize->AddTab(3);
           m_pPageCustomize->AddTab(4);
           m_pPageCustomize->AddTab(5);
           m_pPageCustomize->AddCategories(nID/*IDR_MAINFRAME_E*/);

       }
       else if(pApp->GetCurrentMode() == CStudioApp::MODE_ASSISTANT)
       {   
           m_pPageCustomize->AddTab(0);
           m_pPageCustomize->AddTab(1);
           m_pPageCustomize->AddTab(2);
           m_pPageCustomize->AddCategories(nID/*IDR_MAINFRAME_A*/);
       }
   }

#endif


  if(!bCustom && m_pPageMisc != NULL)
      AddPage(m_pPageMisc);
   
   m_nSelectedPage = nSelectedPage;
}

//protected
int CAssistantSettings::GetMaxStringWidth()
{
   int iMaxStringLen = 0;
   CString csStr = _T("x");
   if(iMaxStringLen < m_pcsTitleMisc->GetLength())
   {
      csStr = *m_pcsTitleMisc;
      iMaxStringLen = m_pcsTitleMisc->GetLength();
   }

   if(iMaxStringLen < m_pcsTitleCustomize->GetLength())
   {
      csStr = *m_pcsTitleCustomize;
      iMaxStringLen = m_pcsTitleCustomize->GetLength();
   }

   if(iMaxStringLen < m_pcsTitleScreengrabbing->GetLength())
   {
      csStr = *m_pcsTitleScreengrabbing;
      iMaxStringLen = m_pcsTitleScreengrabbing->GetLength();
   }

   if(iMaxStringLen < m_pcsTitleAudioVideo->GetLength())
   {
      csStr = *m_pcsTitleAudioVideo;
      iMaxStringLen = m_pcsTitleAudioVideo->GetLength();
   }
  

   CFont* pFont = GetFont();
   CWindowDC dc(NULL);
   CXTPFontDC font(&dc, pFont);
   csStr.Trim();
   int retSize = dc.GetTextExtent(csStr, csStr.GetLength()).cx;
   return retSize;
  
}

void CAssistantSettings::Init(CAssistantDoc *pAssistantDoc)
{
   m_pDocument = pAssistantDoc;
   m_pPageAudioVideo->Init(pAssistantDoc);
   m_pPageScreengrabbing->Init(pAssistantDoc);
}

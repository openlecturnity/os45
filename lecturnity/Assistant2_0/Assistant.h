// Assistant.h : main header file for the ASSISTANT application
//

#if !defined(AFX_ASSISTANT_H__08ED83CD_0A9A_4C44_BA4B_A431412A2DF6__INCLUDED_)
#define AFX_ASSISTANT_H__08ED83CD_0A9A_4C44_BA4B_A431412A2DF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

//#include "MainFrm.h"
#include "..\Studio\resource.h"       // main symbols
#include "securityutils.h"
#include "MissingFontsDlg.h"
#include "SplashScreen.h"
#include "SplashScreen2.h"



/////////////////////////////////////////////////////////////////////////////
// CAssistantApp:
// See Assistant.cpp for the implementation of this class
//

class CAssistantApp : public CWinApp
{
public:
   friend class CMainFrameA;  // Declare a friend class
   friend class CMissingFontsDlg;  // Declare a friend class
   
   CAssistantApp();
   ~CAssistantApp();
   
   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CAssistantApp)
public:
   virtual BOOL InitInstance();
   virtual int ExitInstance();

   
   //}}AFX_VIRTUAL
   
   // Implementation
   //{{AFX_MSG(CAssistantApp)
   afx_msg void OnAppAbout();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   
   // Returns one of the following:
   //  - FULL_VERSION (== 0L)
   //  - UNIVERSITY_VERSION (== 2L)
   //  - EVALUATION_VERSION (== 1L)
   //  - UNIVERSITY_EVALUATION_VERSION (==3L)
   // In order to check for an evaluation version, 
   // AND with EVALUATION_VERSION and check for > 0.
   int GetVersionType();
   
   bool GetShowImportWarning(){return m_bShowImportWarning;}
   bool GetShowMissingFontsWarning(){return m_bShowMissingFontsWarning;}
   bool GetShowAudioWarning(){return m_bShowAudioWarning;}
   bool GetShowUseSmartBoard(){return m_bShowUseSmartBoard;}
   bool GetFreezeImages(){return m_bFreezeImages;}
   bool GetFreezeMaster(){return m_bFreezeMaster;}
   bool GetFreezeDocument(){return m_bFreezeDocument;}
   bool GetShowMouseCursor(){return m_bShowMouseCursor;}
   bool GetShowStatusBar(){return m_bShowStatusBar;}
   bool GetShowStartupPage() {return m_bShowStartupPage;}
   UINT GetCurrentColorScheme(){return m_nCurrentColorScheme;};
   
   void SetShowImportWarning(bool bShowWarning);
   void SetShowMissingFontsWarning(bool bShowWarning);
   void SetShowUseSmartBoard(bool bShowWarning);
   void SetShowAudioWarning(bool bShowWarning);
   void SetFreezeImages(bool bFreezeImages);
   void SetFreezeMaster(bool bFreezeMaster);
   void SetFreezeDocument(bool bFreezeDocument);
   void SetShowMouseCursor(bool bShowMouseCursor);
   void SetShowStatusBar(bool bShowStatusBar);
   void SetShowStartupPage(bool bShowStartupPage);
   void SetCurrentColorScheme(UINT nColorScheme);
   void LoadStdProfileSettings(UINT nMaxMRU);
   CRecentFileList *GetRecentFileList() {return m_pRecentFileList;}
   CSecurityUtils m_securityUtils;
   
   
private:
   bool CheckVersionAndShowSplash();
   void CheckLecturnityHome();

   int            m_nVersionType;
   DWORD          m_dwDirectXVersion;
   CSplashScreenA m_splashScreen;

   CSplashScreen2 m_splashScreen2;

   bool m_bShowImportWarning;
   bool m_bShowAudioWarning;
   bool m_bShowUseSmartBoard;
   bool m_bShowMissingFontsWarning;
   bool m_bFreezeImages;
   bool m_bFreezeMaster;
   bool m_bFreezeDocument;
   bool m_bShowMouseCursor;
   bool m_bShowStatusBar;
   bool m_bShowStartupPage;
   UINT m_nCurrentColorScheme;

   
public:
   afx_msg void OnFileNew();
   afx_msg void OnFileOpen();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASSISTANT_H__08ED83CD_0A9A_4C44_BA4B_A431412A2DF6__INCLUDED_)

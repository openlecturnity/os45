// SplashScreen.cpp: Implementierungsdatei
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "securityutils.h"
#endif
#include "SplashScreen.h"
#include "MfcUtils.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSplashScreenA 


CSplashScreenA::CSplashScreenA(CWnd* pParent /*=NULL*/)
	: CDialog(CSplashScreenA::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplashScreenA)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_bIsEvalDialog       = false;
   m_bIsExpirationDialog = false;
   m_bIsExpired          = false;
   m_bRequireOk          = false;
   m_nVersionType        = FULL_VERSION;
}


void CSplashScreenA::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashScreenA)
	DDX_Control(pDX, IDC_SPLASH_STATE, m_stateStatic);
	DDX_Control(pDX, IDC_SPLASH_IMAGE, m_splashImage);
	DDX_Control(pDX, IDC_EVAL_IMAGE, m_evalImage);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDC_SPLASH_VERSION, m_versionStatic);
	DDX_Control(pDX, IDC_SPLASH_EVAL, m_evalStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashScreenA, CDialog)
	//{{AFX_MSG_MAP(CSplashScreenA)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, OnSplashOk)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSplashScreenA 

BOOL CSplashScreenA::OnInitDialog() 
{
	CDialog::OnInitDialog();

   if (m_bIsEvalDialog)
      m_bRequireOk = true;

   if (m_bIsEvalDialog)
      m_splashImage.ShowWindow(SW_HIDE);
   else
      m_evalImage.ShowWindow(SW_HIDE);

   int cy = 301;
   if (m_bIsEvalDialog || m_bIsExpirationDialog)
      cy += 23;
   else
      m_evalStatic.ShowWindow(SW_HIDE);
   // Ok button is always shown
   cy += 34;

   m_versionStatic.SetWindowPos(NULL, 0, 284, 296, 18, SWP_NOZORDER);
  
   SetDefaultText();

   m_evalStatic.SetWindowPos(NULL, 0, 302, 296, 18, SWP_NOZORDER);
   if (m_bIsEvalDialog)
   {
      CString csEval;
      if (m_nEvalDays < 0)
         csEval.LoadString(IDS_EVALEXPIRED);
      else
         csEval.Format(IDS_EVALDAYS, m_nEvalDays);
      m_evalStatic.SetWindowText(csEval);
   }
   else if (m_bIsExpirationDialog)
   {
      CString csExpiration;
      if (m_bIsExpired)
      {
         CString csExp;
         csExp.LoadString(IDS_EXPIRED);
         csExpiration.Format(IDS_EXPIRATION, csExp);
      }
      else
      {
         csExpiration.Format(IDS_EXPIRATION, m_csExpirationDate);
      }
      m_evalStatic.SetWindowText(csExpiration);
   }

   m_okButton.SetWindowPos(NULL, 108, cy-33, 80, 26, SWP_NOZORDER);
   m_stateStatic.SetWindowPos(NULL, 0, cy-33, 298, 26, SWP_NOZORDER);
   SetWindowPos(&wndTopMost, 0, 0, 298, cy, SWP_NOMOVE);

   if (!m_bRequireOk)
   {
      m_okButton.EnableWindow(FALSE);
      m_okButton.ShowWindow(SW_HIDE);

#ifdef _DEBUG
      StartAutoCloseTimer(); // help debugging; otherwise the splash screen is always on top
#endif
   }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CSplashScreenA::SetIsEvalDialog(bool bEval, int evalDays)
{
   m_bIsEvalDialog = bEval;
   m_nEvalDays     = evalDays;
}

void CSplashScreenA::SetIsExpirationDialog(bool bLimit, bool bHasExpired, CString csExpDate)
{
   m_bIsExpirationDialog = bLimit;
   m_bIsExpired         = bHasExpired;
   m_csExpirationDate    = csExpDate;
}

void CSplashScreenA::SetVersionType(int versionType)
{
   m_nVersionType = versionType;
}

void CSplashScreenA::SetRequireOk(bool bOk)
{
   m_bRequireOk = bOk;

   if (m_hWnd != NULL)
   {
      if (m_bRequireOk)
      {
         m_stateStatic.ShowWindow(SW_HIDE);
         m_okButton.ShowWindow(SW_SHOW);
         m_okButton.EnableWindow(TRUE);
      }
      else
      {
         m_stateStatic.ShowWindow(SW_SHOW);
         m_okButton.EnableWindow(FALSE);
         m_okButton.ShowWindow(SW_HIDE);
      }
   }
}

void CSplashScreenA::StartAutoCloseTimer()
{
   if (this->m_hWnd != NULL)
   {
      SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
      SetTimer(12345, 2000, NULL);
   }
}

void CSplashScreenA::SetStateText(CString &csText)
{
   if (m_stateStatic.m_hWnd != NULL)
      m_stateStatic.SetWindowText(csText);
}

void CSplashScreenA::SetDefaultText()
{ 
   CString csBetaRelease;
   csBetaRelease.LoadString(IDS_INTERNALRELEASE);

   CString csVersion;
   CSecurityUtils cSec;
   cSec.GetVersionString(csVersion, csBetaRelease);
   //cSec.GetCompactVersionString(csVersion, csBetaRelease);

   CString csBuild;
   MfcUtils::GetPrivateBuild(csBuild);

   CString csLabel = _T("LECTURNITY Assistant ") + csVersion + _T(" (") + csBuild + _T(")");
   m_versionStatic.SetWindowText(csLabel);
}

BOOL CSplashScreenA::OnEraseBkgnd(CDC* pDC) 
{
   // return CDialog::OnEraseBkgnd(pDC);

   // We wanted a white background, I still do
   CRect rect;
   GetClientRect(&rect);
   CBrush white(RGB(255, 255, 255));
   pDC->FillRect(&rect, &white);

   return TRUE;
}

void CSplashScreenA::OnSplashOk() 
{
   // Clicking OK closes the dialog
   KillTimer(12345);
   EndDialog(0);
}

HBRUSH CSplashScreenA::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   // All controls should have a white background;
   // text should be drawn in transparent mode.

   pDC->SetBkMode(TRANSPARENT);
   HBRUSH hbr = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
   //HBRUSH hbr = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
	
	return hbr;
}

void CSplashScreenA::OnTimer(UINT nIDEvent) 
{
   // A timer event is thrown if auto close is
   // enabled (m_bRequireOk is not true)
   KillTimer(12345);
   EndDialog(0);
}

// SplashScreen.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "SplashScreen.h"
#include "securityutils.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSplashScreenE 


CSplashScreenE::CSplashScreenE(CWnd* pParent /*=NULL*/)
	: CDialog(CSplashScreenE::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplashScreenE)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_bIsEvalDialog       = false;
   m_bIsExpirationDialog = false;
   m_bIsExpired          = false;
   m_bRequireOk          = false;
   m_nVersionType        = FULL_VERSION;
}


void CSplashScreenE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashScreenE)
	DDX_Control(pDX, IDC_SPLASH_IMAGE_E, m_splashImage);
	DDX_Control(pDX, IDC_EVAL_IMAGE_E, m_evalImage);
	DDX_Control(pDX, IDC_SPLASH_OK_E, m_okButton);
	DDX_Control(pDX, IDC_SPLASH_VERSION_E, m_versionStatic);
	DDX_Control(pDX, IDC_SPLASH_EVAL_E, m_evalStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashScreenE, CDialog)
	//{{AFX_MSG_MAP(CSplashScreenE)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_SPLASH_OK_E, OnSplashOk)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSplashScreenE 

BOOL CSplashScreenE::OnInitDialog() 
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
   CString csVersion;
   CString csBetaRelease;
   csBetaRelease.LoadString(IDS_INTERNALRELEASE_E);
   CSecurityUtils cSec;
   cSec.GetVersionString(csVersion, csBetaRelease);
   CString csBuild;
   MfcUtils::GetPrivateBuild(csBuild);
   CString csLabel = _T("LECTURNITY Editor ") + csVersion + _T(" (") + csBuild + _T(")");
   m_versionStatic.SetWindowText(csLabel);

   m_evalStatic.SetWindowPos(NULL, 0, 302, 296, 18, SWP_NOZORDER);
   if (m_bIsEvalDialog)
   {
      CString csEval;
      if (m_nEvalDays < 0)
         csEval.LoadString(IDS_EVALEXPIRED_E);
      else
         csEval.Format(IDS_EVALDAYS_E, m_nEvalDays);
      m_evalStatic.SetWindowText(csEval);
   }
   else if (m_bIsExpirationDialog)
   {
      CString csExpiration;
      if (m_bIsExpired)
      {
         CString csExp;
         csExp.LoadString(IDS_EXPIRED_E);
         csExpiration.Format(IDS_EXPIRATION_E, csExp);
      }
      else
      {
         csExpiration.Format(IDS_EXPIRATION_E, m_csExpirationDate);
      }
      m_evalStatic.SetWindowText(csExpiration);
   }

   m_okButton.SetWindowPos(NULL, 108, cy-33, 80, 26, SWP_NOZORDER);

   SetWindowPos(&wndTopMost, 0, 0, 298, cy, SWP_NOMOVE);

   if (!m_bRequireOk)
   {
      m_okButton.EnableWindow(FALSE);
   }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CSplashScreenE::SetIsEvalDialog(bool bEval, int evalDays)
{
   m_bIsEvalDialog = bEval;
   m_nEvalDays     = evalDays;
}

void CSplashScreenE::SetIsExpirationDialog(bool bLimit, bool bHasExpired, CString csExpDate)
{
   m_bIsExpirationDialog = bLimit;
   m_bIsExpired         = bHasExpired;
   m_csExpirationDate    = csExpDate;
}

void CSplashScreenE::SetVersionType(int versionType)
{
   m_nVersionType = versionType;
}

void CSplashScreenE::SetRequireOk(bool bOk)
{
   m_bRequireOk = bOk;

   if (m_hWnd != NULL)
   {
      if (m_bRequireOk)
         m_okButton.EnableWindow(TRUE);
      else
         m_okButton.EnableWindow(FALSE);
   }
}

void CSplashScreenE::StartAutoCloseTimer()
{
   SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
   SetTimer(12345, 2000, NULL);
}

BOOL CSplashScreenE::OnEraseBkgnd(CDC* pDC) 
{
   // return CDialog::OnEraseBkgnd(pDC);

   // We wanted a white background, I still do
   CRect rect;
   GetClientRect(&rect);
   CBrush white(RGB(255, 255, 255));
   pDC->FillRect(&rect, &white);

   return TRUE;
}

void CSplashScreenE::OnSplashOk() 
{
   // Clicking OK closes the dialog
   KillTimer(12345);
   EndDialog(0);
}

HBRUSH CSplashScreenE::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   // All controls should have a white background;
   // text should be drawn in transparent mode.

   pDC->SetBkMode(TRANSPARENT);
   HBRUSH hbr = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
   //HBRUSH hbr = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
	
	return hbr;
}

void CSplashScreenE::OnTimer(UINT nIDEvent) 
{
   // A timer event is thrown if auto close is
   // enabled (m_bRequireOk is not true)
   KillTimer(12345);
   EndDialog(0);
}

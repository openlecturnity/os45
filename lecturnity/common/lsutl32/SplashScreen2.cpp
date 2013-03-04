#include "stdafx.h"
#include "lsutl32.h"
#include "SplashScreen2.h"
#include "MfcUtils.h"
#include "imc_lecturnity_util_ui_SplashScreen2.h"
#include "JNISupport.h"
#include "EnterLicenseDialog.h"

/* REVIEW UK
 * No "magic" numbers like 550 or 300 should be used. Sizes should be determined dynamically.
 */

IMPLEMENT_DYNAMIC(CSplashScreen2, CWnd)

CSplashScreen2::CSplashScreen2()
{
   m_nEvalDays = -1;
   m_bIsEvalVersion = false;
   m_bIsLimitedVersion = false;
   m_bIsExpired = false;
   m_csExpirationDate = _T("");
   m_bRequireOk = false;
   m_bShouldRun = false;
   m_nVersionType = FULL_VERSION;
   m_rcSplash = CRect(0,0,0,0);
   m_bModalLoopStarted = false;
}

CSplashScreen2::~CSplashScreen2()
{
}


BEGIN_MESSAGE_MAP(CSplashScreen2, CWnd)
   ON_WM_ERASEBKGND()
   ON_WM_CTLCOLOR()
   ON_BN_CLICKED(IDOK, OnSplashOk)
   ON_BN_CLICKED(IDYES, OnEnterLicenseKey)
   ON_WM_TIMER()
END_MESSAGE_MAP()

// CSplashScreen2-Meldungshandler

void CSplashScreen2::CreateSplashScreen(CString csStartingState, HWND hwndParent, bool bIsAbout)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HWND hwndDesktop = ::GetDesktopWindow();
   CRect rcDesktop;
   CWnd::FromHandle(hwndDesktop)->GetWindowRect(rcDesktop);

   CRect rcSplash;
   rcSplash.left = (rcDesktop.Width()-547)/2;
   rcSplash.top = (rcDesktop.Height())/2 - (296+GetSystemMetrics(SM_CYCAPTION));
   rcSplash.right = rcSplash.left + 547;
   rcSplash.bottom = rcSplash.top + 296;
#undef _DEBUG
#ifdef _DEBUG
   // Move it out of the way
   rcSplash.OffsetRect((rcDesktop.Width()-547-50)/2, -(rcDesktop.Height()-296-50)/2);
#endif

   DWORD dwStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
   if (bIsAbout)
       dwStyle = 0;
   
   CreateEx(dwStyle, AfxRegisterWndClass(0,0,0,0), NULL, 
      WS_POPUP, rcSplash.left, rcSplash.top, 
      rcSplash.Width(), rcSplash.Height(), hwndParent, 0);
   // WS_CAPTION instead of WS_POPUP to get a move handle; however window size is too small then


   LOGFONT logFont;
   XTPPaintManager()->GetRegularFont()->GetLogFont(&logFont);
   _tcscpy(logFont.lfFaceName, _T("Calibri")); 
   logFont.lfHeight = 14; 
   m_font.CreateFontIndirect(&logFont);

   //m_splashImage = new CStatic();
   m_splashImage.Create(NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(0,0,547,296), this, IDC_SPLASHSCREEN_IMAGE);

   
   m_stateStatic.Create(NULL, SS_RIGHT | WS_CHILD | WS_VISIBLE, CRect(320,232,520,250), this, IDC_SPLASHSCREEN_STATE);
   m_stateStatic.SetFont(&m_font);

   CRect rcVersionText;
   if (!m_bIsEvalVersion) {
       rcVersionText = CRect(281, 251, 437, 270);
   } else {
       rcVersionText = CRect(161, 251, 317, 270);
   }

   m_evalStatic.Create(NULL, SS_RIGHT | WS_CHILD | WS_VISIBLE, rcVersionText, this, IDC_SPLASHSCREEN_EVAL);
   m_evalStatic.SetFont(&m_font);

   m_btnLicenseKey.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(327, 249, 437, 270), this, IDYES);
   m_btnLicenseKey.SetFont(&m_font); 
   CString csLicenseKey;
   csLicenseKey.LoadString(IDS_LICENSE_KEY);
   m_btnLicenseKey.SetWindowText(csLicenseKey);
   
   m_okButton.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(446,249,520,270), this, IDOK);
   m_okButton.SetFont(&m_font); 

   m_versionStatic.Create(NULL, SS_RIGHT | WS_CHILD | WS_VISIBLE, CRect(208,277,520,290), this, IDC_SPLASHSCREEN_VERSION);
   m_versionStatic.SetFont(&m_font);

   HBITMAP hBitmap = CXTPImageManagerIcon::LoadBitmapFromResource(MAKEINTRESOURCE(IDC_SPLASHSCREEN_IMAGE), NULL);
   m_splashImage.SetBitmap(hBitmap);
   DeleteObject(hBitmap);

   m_stateStatic.SetWindowText(csStartingState);

   CString csOk;
   csOk.LoadString(IDS_OK);
   m_okButton.SetWindowText(csOk);

   if (!m_bIsEvalVersion)
       m_btnLicenseKey.ShowWindow(SW_HIDE);

   if (m_bIsEvalVersion)
   {
      CString csEval;
      if (m_nEvalDays < 0)
         csEval.LoadString(IDS_SPLASHSCREEN_EVALEXPIRED);
      else
         csEval.Format(IDS_SPLASHSCREEN_EVALDAYS, m_nEvalDays);
      
      m_evalStatic.SetWindowText(csEval);
   }
   else if (m_bIsLimitedVersion)
   {
      CString csExpiration;
      if (m_bIsExpired)
      {
         CString csExp;
         csExp.LoadString(IDS_SPLASHSCREEN_EXPIRED);
         csExpiration.Format(IDS_SPLASHSCREEN_EXPIRATION, csExp);
      }
      else
      {
         csExpiration.Format(IDS_SPLASHSCREEN_EXPIRATION, m_csExpirationDate);
      }
      m_evalStatic.SetWindowText(csExpiration);
   }

   if (!m_bShouldRun)
      m_evalStatic.SetTextColor(RGB(255,0,0));

   CString csBetaRelease;
   csBetaRelease.LoadString(IDS_SPLASHSCREEN_INTERNALRELEASE);

   CString csVersion;
   CSecurityUtils cSec;
   cSec.GetCompactVersionString(csVersion, csBetaRelease);

   CString csBuild;
   MfcUtils::GetPrivateBuild(csBuild);

   CString csRights;
   csRights.LoadString(IDS_SPLASHSCREEN_ALLRIGHTSRESERVED);

   CString csLabel = _T("LECTURNITY ") + csVersion + _T(" | Rev. ") + csBuild ;//+_T(" | © IMC AG | ") + csRights;
   m_versionStatic.SetWindowText(csLabel);

   if (!m_bRequireOk)
   {
      m_okButton.EnableWindow(FALSE);
      m_okButton.ShowWindow(SW_HIDE);
   }

   ShowWindow(SW_SHOW);

   // Necessary to actually show anything; otherwise all elements were
   // not painted yet because the window was invisible
   RedrawWindow(); 

   GetWindowRect(m_rcSplash);
}

bool CSplashScreen2::Show(HWND hwndParent)
{  
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_bRequireOk = true;
    CreateSplashScreen(CString(), hwndParent, true);
    ShowWindow(SW_SHOW);

    return true;
}

bool CSplashScreen2::Show(CString& csInitialStatusText) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(CheckVersion()) {
        if(m_bShouldRun && !m_bIsEvalVersion) {
            // Unlimited version...

            CreateSplashScreen(csInitialStatusText, NULL, false);

            return true;
        } else {
            // Either expired or at least eval version
            // (which blocks until "ok" also if non-expired)

            Show(NULL);

            m_bModalLoopStarted = true;
            RunModalLoop(MLF_NOIDLEMSG | MLF_NOKICKIDLE);

            return m_bShouldRun; // SIDE effect: might have been changed by OnEnterLicenseKey()!
        }
    } else {
        CString msg;
        msg.LoadString(IDS_SPLASHSCREEN_LICENSE_ERROR);
        CString cap;
        cap.LoadString(IDS_SPLASHSCREEN_ERROR);

        ::MessageBox(NULL, msg, cap, MB_ICONERROR | MB_OK | MB_TOPMOST);

        return false;
    }

    return true;
}

void CSplashScreen2::SetRequireOk(bool bOk)
{
   m_bRequireOk = bOk;

   if (m_hWnd != NULL)
   {
      if (m_bRequireOk)
      {
         m_stateStatic.ShowWindow(SW_HIDE);
         m_okButton.ShowWindow(SW_SHOW);
         m_okButton.EnableWindow(TRUE);
         m_okButton.SetFont(&m_font); 
      }
      else
      {
         m_stateStatic.ShowWindow(SW_SHOW);
         m_okButton.EnableWindow(FALSE);
         m_okButton.ShowWindow(SW_HIDE);
      }
   }
}

bool CSplashScreen2::CheckVersion()
{
   bool hasErrorOccurred = false;

   CString csOwnVersion;
   csOwnVersion.LoadString(IDS_SPLASHSCREEN_VERSION);
   m_nVersionType = m_securityUtils.GetVersionType(csOwnVersion);

   if (m_nVersionType == VERSION_ERROR)
   {
      hasErrorOccurred = true;
   }
   else if ((m_nVersionType & EVALUATION_VERSION) > 0)
   {
      m_bIsEvalVersion = true;
      // Evaluation version
      CString csInstallDate;
      bool success = m_securityUtils.GetInstallationDate(csInstallDate);
      if (!success)
         csInstallDate = _T("000000");
      if (csInstallDate == _T("000000"))
      {
         hasErrorOccurred = true;
      }
      else
      {
         CString csYear = _T("00");
         csYear.SetAt(0, csInstallDate.GetAt(0));
         csYear.SetAt(1, csInstallDate.GetAt(1));
         int year = 2000 + _ttoi(csYear);

         CString csMonth = _T("00");
         csMonth.SetAt(0, csInstallDate.GetAt(2));
         csMonth.SetAt(1, csInstallDate.GetAt(3));
         int month = _ttoi(csMonth);

         CString csDay = _T("00");
         csDay.SetAt(0, csInstallDate.GetAt(4));
         csDay.SetAt(1, csInstallDate.GetAt(5));
         int day = _ttoi(csDay);

         CTime installDate(year, month, day, 0, 0, 0);

         int evaluationPeriod = m_securityUtils.GetEvaluationPeriod();
         CTimeSpan evalSpan(evaluationPeriod + 1, 0, 0, 0);
         CTime lastValidDate = installDate + evalSpan;

         CTime todaysDate = CTime::GetCurrentTime();
         CTimeSpan evalRest = lastValidDate - todaysDate;

         int restMins = evalRest.GetTotalMinutes();
         m_nEvalDays = evalRest.GetTotalHours() / 24;

         if (restMins > 0)
            m_bShouldRun = true;
      }
   }
   else
   {
      // Campus or enterprise version
      CString csExpDate;
      bool success = m_securityUtils.GetExpirationDate(csExpDate);
      if (success)
      {
         if (csExpDate != _T(""))
         {
            m_bIsLimitedVersion = true;
            CString csYear = _T("0000");
            csYear.SetAt(0, csExpDate.GetAt(0));
            csYear.SetAt(1, csExpDate.GetAt(1));
            csYear.SetAt(2, csExpDate.GetAt(2));
            csYear.SetAt(3, csExpDate.GetAt(3));
            CString csMonth = _T("00");
            csMonth.SetAt(0, csExpDate.GetAt(4));
            csMonth.SetAt(1, csExpDate.GetAt(5));
            CString csDay = _T("00");
            csDay.SetAt(0, csExpDate.GetAt(6));
            csDay.SetAt(1, csExpDate.GetAt(7));

            CString csLang;
            csLang.LoadString(IDS_SPLASHSCREEN_LANGUAGE);
            if (csLang == _T("de"))
               m_csExpirationDate = csDay + _T(".") + csMonth + _T(".") + csYear;
            else
               m_csExpirationDate = csMonth + _T("/") + csDay + _T("/") + csYear;
            int year = _ttoi(csYear);
            int month = _ttoi(csMonth);
            int day = _ttoi(csDay);

            CTime expDate(year, month, day, 0, 0, 0);
            CTime todaysDate = CTime::GetCurrentTime();
            CTimeSpan restTime = expDate - todaysDate;
            if (restTime.GetTotalHours() >= -24)
               m_bShouldRun = true;
            else
               m_bIsExpired = true;
         }
         else // unlimited version
            m_bShouldRun = true;
      }
      else
      {
         hasErrorOccurred = true;
      }
   }
   m_bRequireOk = (m_bIsEvalVersion || !m_bShouldRun);

   return !hasErrorOccurred;
}

void CSplashScreen2::OnTimer(UINT nIDEvent) 
{
   // A timer event is thrown if auto close is
   // enabled (m_bRequireOk is not true)
   KillTimer(12345);
   ShowWindow(SW_HIDE);
   
}

void CSplashScreen2::Close()
{
   SetRequireOk(true);
   if (this->m_hWnd != NULL && !m_bIsEvalVersion)
   {
      SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
      SetTimer(12345, 1500, NULL);
   }
}

void CSplashScreen2::OnSplashOk() 
{
   KillTimer(12345);
   ShowWindow(SW_HIDE);
   if(m_bModalLoopStarted)
      EndModalLoop(0);
}

void CSplashScreen2::OnEnterLicenseKey() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); // needed for CheckVersion() (and the version string!)

    // Doubled in CEnterLicenseDialog
    bool bCanWriteRegistry = 
        LMisc::IsUserAdmin() || LMisc::IsWindowsVistaOrHigher() && LMisc::CanUserElevate();

    if (!bCanWriteRegistry) {
        CString csNoAdmin;
        csNoAdmin.LoadString(IDS_NO_ADMIN);
        MessageBox(csNoAdmin, _T("LECTURNITY"), MB_ICONERROR | MB_OK);
        
        return;
    }

    bool bKeyEntered = CEnterLicenseDialog::ShowItModal(this);

    if (bKeyEntered) {
        CheckVersion();

        if (m_bShouldRun) // Only if a valid key was entered...
            OnSplashOk();
    }
}

BOOL CSplashScreen2::OnEraseBkgnd(CDC* pDC) 
{
   CRect rect;
   GetClientRect(&rect);
   CBrush white(RGB(244, 246, 250));
   pDC->FillRect(&rect, &white);

   return TRUE;
}

HBRUSH CSplashScreen2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    // All controls should have a white background;
    // text should be drawn in transparent mode. -> Does not work

    pDC->SetBkMode(TRANSPARENT);
    HBRUSH hbr = ::CreateSolidBrush(RGB(244, 246, 250));

    // Red text is handled in CTransparentStatic

    return hbr;
}

CRect& CSplashScreen2::GetWindowBounds()
{
   return m_rcSplash;
}

void CSplashScreen2::ChangeStatusText(CString csNewText)
{
   m_stateStatic.SetWindowText(csNewText);
   UpdateWindow();
}


CSplashScreen2 *g_Splash = NULL;

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_ui_SplashScreen2_Show
(JNIEnv *, jobject) {

    if (g_Splash == NULL)
        g_Splash = new CSplashScreen2;

    return g_Splash->Show(CString()); // CString() only necessary to get the right method...
}

JNIEXPORT void JNICALL Java_imc_lecturnity_util_ui_SplashScreen2_ShowAsAbout
(JNIEnv *env, jobject, jobject joWndParent) {

    HWND hwndParent = JNISupport::GetHwndFromJavaWindow(env, joWndParent);

    if (g_Splash == NULL)
        g_Splash = new CSplashScreen2;

    g_Splash->Show(hwndParent); // without parameter only necessary to get the right method...
}

JNIEXPORT void JNICALL Java_imc_lecturnity_util_ui_SplashScreen2_Hide
(JNIEnv *, jobject) {
     if (g_Splash != NULL)
         g_Splash->Close();
}

JNIEXPORT jobject JNICALL Java_imc_lecturnity_util_ui_SplashScreen2_GetScreenBounds
(JNIEnv *env, jobject) {

    jobject joRectReturn = NULL;

    if (g_Splash != NULL) {
        jclass jclRectangle = env->FindClass("java/awt/Rectangle");
        if (jclRectangle != 0) {
            jmethodID jmInit = env->GetMethodID(jclRectangle, "<init>", "()V");
            if (jmInit != 0) {
                joRectReturn = env->NewObject(jclRectangle, jmInit);
                
                CRect rcBounds = g_Splash->GetWindowBounds();

                JNISupport::SetIntToObject(env, rcBounds.left, "x", joRectReturn);
                JNISupport::SetIntToObject(env, rcBounds.top, "y", joRectReturn);
                JNISupport::SetIntToObject(env, rcBounds.Width(), "width", joRectReturn);
                JNISupport::SetIntToObject(env, rcBounds.Height(), "height", joRectReturn);
            }
        }
    }

    return joRectReturn;
}
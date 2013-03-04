#pragma once

/* REVIEW UK
 * Show() and Show(...) should do the same except the additional parameter; currently they are
 *        very different.
 */

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
    #define LSUTL32_EXPORT __declspec(dllexport)
#else 
    #ifdef _LSUTL32_STATIC
        #define LSUTL32_EXPORT 
    #else
        #define LSUTL32_EXPORT __declspec(dllimport)
    #endif
#endif

/**
 * Shows the new Splashscreen image from LECTURNITY 4.0 and on top of that several
 * ui elements like texts or an ok button. The window used is borderless.
 * 
 * This window is top-most but hides automatically after 15 seconds when
 * it is no evaluation version and no expired version.
 * In the case of an expired version the Show() method here returns "true"
 * to signal the calling process to quit immediately. Otherwise Show() directly
 * returns after making the window visible.
 *
 * This component is reused by Studio, Publisher and Player (the last two with 
 * a JNI interface).
 *
 * All needed resources (image, text, icon) are part of the lsutl32 resources.
 */

#include "securityutils.h"

class LSUTL32_EXPORT CWaitSplashScreen : public CWnd {
    DECLARE_DYNAMIC(CWaitSplashScreen)

public:
    CWaitSplashScreen();
    virtual ~CWaitSplashScreen();

    bool Show(HWND hwndParent);
    bool Show(CString& csInitialStatusText);

    /**
     * Explicitly closes the window; mostly because the program is already visible.
     */
    void Close();

    void ChangeStatusText(CString csNewText);

    /**
     * This method should return the size and position of the dialog.
     * It should give meaningful values at least after Show() returned.
     */
    CRect& GetWindowBounds();
    void SetPos(UINT nPos);
    void StepIt();

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnSplashOk();
    afx_msg void OnTimer(UINT nIDEvent);

private:

    void CreateSplashScreen(CString csStartingState, HWND hwndParent, bool bIsAbout);
    void SetRequireOk(bool bOk);

    CStatic	m_stateStatic;
    CStatic  m_splashImage;
    CButton	m_okButton;
    CStatic	m_evalStatic;
    CStatic	m_versionStatic;
    bool m_bShouldRun;
    bool m_bRequireOk;
    CSecurityUtils m_securityUtils;
    CFont m_font;
    CRect m_rcSplash;
    CXTPProgressCtrl m_wndProgressBar;

    /*int m_nVersionType;
    int m_nEvalDays;
    bool m_bIsEvalVersion;
    bool m_bIsLimitedVersion;
    bool m_bIsExpired;
    CString m_csExpirationDate;*/
};
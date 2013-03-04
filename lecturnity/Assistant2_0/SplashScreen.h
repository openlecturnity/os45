#if !defined(AFX_SPLASHSCREEN_H__C429CD74_183B_44BB_8CEF_967CBF127C2F__INCLUDED_)
#define AFX_SPLASHSCREEN_H__C429CD74_183B_44BB_8CEF_967CBF127C2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SplashScreen.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSplashScreenA 

#include "..\Studio\Resource.h"

class CSplashScreenA : public CDialog
{
// Konstruktion
public:
	CSplashScreenA(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSplashScreenA)
	enum { IDD = IDD_SPLASH };
	CStatic	m_stateStatic;
	CStatic	m_splashImage;
	CStatic	m_evalImage;
	CButton	m_okButton;
	CStatic	m_versionStatic;
	CStatic	m_evalStatic;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSplashScreenA)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

public:
   void SetIsEvalDialog(bool bEval, int evalDays);
   void SetVersionType(int versionType);
   void SetIsExpirationDialog(bool bLimit, bool bIsExpired=false, CString csExpDate=_T(""));
   void SetRequireOk(bool bOk);
   void StartAutoCloseTimer();
   void SetStateText(CString &csText);
   void SetDefaultText();

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSplashScreenA)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSplashOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   bool m_bIsEvalDialog;
   bool m_bIsExpirationDialog;
   bool m_bIsExpired;
   bool m_bRequireOk;

   int     m_nVersionType;
   int     m_nEvalDays;
   CString m_csExpirationDate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SPLASHSCREEN_H__C429CD74_183B_44BB_8CEF_967CBF127C2F__INCLUDED_

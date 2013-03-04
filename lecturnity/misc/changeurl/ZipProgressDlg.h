#if !defined(AFX_ZIPPROGRESSDLG_H__8AD450EC_1415_47DD_A91D_36730EEE2457__INCLUDED_)
#define AFX_ZIPPROGRESSDLG_H__8AD450EC_1415_47DD_A91D_36730EEE2457__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZipProgressDlg.h : Header-Datei
//

#include "resource.h"

// Forward declaration
class CChangeUrlEngine;

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CZipProgressDlg 

class CZipProgressDlg : public CDialog
{
// Konstruktion
public:
	CZipProgressDlg(CWnd* pParent, CChangeUrlEngine *pEngine);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CZipProgressDlg)
	enum { IDD = IDD_ZIP_PROGRESS };
	CButton	m_btCancel;
	CStatic	m_lbStatus;
	CProgressCtrl	m_progress;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CZipProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CZipProgressDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CChangeUrlEngine *m_pEngine;
   bool m_bCloseRequested;

public:
   bool m_bInitialized;

   void SetMax(int nMax);
   void SetProgress(const _TCHAR *tszString, int nProgress);
   void CloseDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_ZIPPROGRESSDLG_H__8AD450EC_1415_47DD_A91D_36730EEE2457__INCLUDED_

#if !defined(AFX_PUBLISHPROGRESSDLG_H__1807671B_1C1D_4A2A_A5B6_D77B559A80AB__INCLUDED_)
#define AFX_PUBLISHPROGRESSDLG_H__1807671B_1C1D_4A2A_A5B6_D77B559A80AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PublishProgressDlg.h : Header-Datei
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPublishProgressDlg 

class CPublishProgressDlg : public CDialog
{
// Konstruktion
public:
	CPublishProgressDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPublishProgressDlg)
	enum { IDD = IDD_PUBLISH_PROGRESS };
	CButton	m_btCancel;
	CProgressCtrl	m_prgTotal;
	CProgressCtrl	m_prgDetail;
	CStatic	m_lbDetail;
	CString	m_lbTimeLeft;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPublishProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPublishProgressDlg)
	afx_msg void OnCancelPublish();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PUBLISHPROGRESSDLG_H__1807671B_1C1D_4A2A_A5B6_D77B559A80AB__INCLUDED_

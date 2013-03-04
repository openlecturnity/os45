#if !defined(AFX_NEWSAVAILABLEDIALOG_H__BD4E51DA_7857_4858_9A61_1A29FE15E0F3__INCLUDED_)
#define AFX_NEWSAVAILABLEDIALOG_H__BD4E51DA_7857_4858_9A61_1A29FE15E0F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewsAvailableDialog.h : Header-Datei
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld NewsAvailableDialog 

class CNewsAvailableDialog : public CDialog
{
// Konstruktion
public:
	CNewsAvailableDialog(int nAnnouncements, CString csAnnTitle, CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CNewsAvailableDialog)
	enum { IDD = IDD_NEWSAVAILABLE };
	CStatic	m_lbAnnouncements;
	CButton	m_cbDisableAnnouncements;
	//}}AFX_DATA

public:
   bool IsDisableAnnouncementsChecked() { return m_bIsDisableAnnouncementsChecked; }

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CNewsAvailableDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CNewsAvailableDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCheckbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CString m_csAnnouncementTitle;
   int     m_nAnnouncements;
   bool    m_bIsDisableAnnouncementsChecked;
   afx_msg void OnBnClickedYes();
   afx_msg void OnBnClickedNo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_NEWSAVAILABLEDIALOG_H__BD4E51DA_7857_4858_9A61_1A29FE15E0F3__INCLUDED_

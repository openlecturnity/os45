#if !defined(AFX_AUTOUPDATEDIALOG_H__00D5744D_6FF8_43A4_99C3_AE95D32CC0BE__INCLUDED_)
#define AFX_AUTOUPDATEDIALOG_H__00D5744D_6FF8_43A4_99C3_AE95D32CC0BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoUpdateDialog.h : Header-Datei
//
#include "resource.h"
#include "announcement.h"

class SgmlElement;

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAutoUpdateDialog 

class CAutoUpdateDialog : public CDialog
{
// Konstruktion
public:
	CAutoUpdateDialog(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CAutoUpdateDialog)
	enum { IDD = IDD_AUTOUPDATE };
	CButton	m_cancelButton;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CAutoUpdateDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CAutoUpdateDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   // This method calls DoModal() if necessary; returns what
   // DoModal() returns, i.e. FALSE if nothing should be done,
   // and TRUE if the caller should exit.
   int  DoCheck();
   void SetForceCheck(bool bForce) { m_bForceCheck = bForce; }

private:
   static UINT UpdateThreadLauncher(LPVOID pParam);
   void UpdateThread();

   bool ContactUpdateServer(LBuffer &lbSgml);
   bool ExtractAnnouncements(SgmlElement *pAnnouncements);
   bool ExtractPatchInformation(SgmlElement *pRoot);
   bool MarkReadAnnouncements();
   bool CheckAnnouncements();

   // Returns true if an update was issued
   bool CheckUpdates();

   // An array of all announcements
   CArray<CAnnouncement, CAnnouncement> m_caAnnouncements;
   // The URL to the overview of the announcements.
   CString m_csOverviewUrl;

   // The version type of the current installation,
   // see KeyGenerator.h: TYPE_*_VERSION
   int m_nVersionType;

   // Handle to internet connection during ContactUpdateServer(), needed for closing on Cancel
   LInternet *m_pInternet;

   // The URL of the latest patch for this version
   CString m_csPatchUrl;
   // The patch level the patch brings this version to
   int m_nPatchLevel;
   // The size, in bytes, of the patch
   int m_nPatchSizeBytes;

   // True if the cancel button was pressed.
   // Note that the dialog is always ended from within
   // the worker thread!
   bool m_bCancelRequested;

   // True as soon as the worker thread has finished
   bool m_bWorkerFinished;

   // True if the on/off settings for the update mechanism
   // should be ignored.
   bool m_bForceCheck;

   // FALSE or TRUE, FALSE if no update was issued,
   // TRUE otherwise.
   BOOL m_bDialogResult;

   // Will be set to true if it's time for a program update check
   bool m_bTimeForUpdateCheck;
   // Will be set to true if it's time for a announcement check
   bool m_bTimeForAnnouncementCheck;

   // Is true if something new has been found, i.e. an update
   // or a new announcement. False otherwise. Is set to false
   // at the beginning of the update thread, and may be set
   // to true somewhere in the following methods.
   bool m_bHasFoundSomething;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_AUTOUPDATEDIALOG_H__00D5744D_6FF8_43A4_99C3_AE95D32CC0BE__INCLUDED_

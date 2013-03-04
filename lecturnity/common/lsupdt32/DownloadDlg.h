#if !defined(AFX_CDOWNLOADDLG_H__2CE08DEC_2F07_4093_967D_B96A1DF5B1B5__INCLUDED_)
#define AFX_CDOWNLOADDLG_H__2CE08DEC_2F07_4093_967D_B96A1DF5B1B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDownloadDlg.h : Header-Datei
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDownloadDlg 

class CDownloadDlg : public CDialog
{
// Konstruktion
public:
	CDownloadDlg(CWnd* pParent, LPCTSTR tszUrl, int nPatchSizeBytes, LPCTSTR tszTargetPath);

// Dialogfelddaten
	//{{AFX_DATA(CDownloadDlg)
	enum { IDD = IDD_DOWNLOAD };
	CButton	m_btCancel;
	CStatic	m_lbProgress;
	CProgressCtrl	m_prgDownload;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDownloadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDownloadDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   // Member variables
   // The URL for the file to download
   CString m_csSourceUrl;
   // The target path of the download, including a backslash, not containing
   // the file name.
   CString m_csTargetPath;

   // The time at the start of the download progress
   time_t m_ttStartTime;
   // The size of the download, in bytes
   DWORD m_dwDownloadSize;
   // True if user has requested to cancel
   bool m_bCancelRequested;
   // True if download has finished successfully
   bool m_bDownloadSuccessful;
   // True after the download thread has finished
   bool m_bDownloadThreadFinished;

   static UINT DLThreadLauncher(LPVOID pData);
   void DownloadThread();
   void UpdateProgress(DWORD dwBytesRead);
public:
   void GetTargetFileName(CString &csFileName);
   bool HasSucceeded();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CDOWNLOADDLG_H__2CE08DEC_2F07_4093_967D_B96A1DF5B1B5__INCLUDED_

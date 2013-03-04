#if !defined(AFX_SCREENGRABBINGPROGRESSDIALOG_H__7DFC2711_0931_4C49_8ABE_B2818901B4A5__INCLUDED_)
#define AFX_SCREENGRABBINGPROGRESSDIALOG_H__7DFC2711_0931_4C49_8ABE_B2818901B4A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


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


#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CExportProgressDialog 

class CScreengrabbingExtendedProgressDialog : public CDialog
{
// Konstruktion
public:
	CScreengrabbingExtendedProgressDialog(CWnd* pParent, HANDLE hDialogInitEvent, bool bShowLaterButton = true);   // Standardkonstruktor

protected:
// Dialogfelddaten
	//{{AFX_DATA(CExportProgressDialog)
	enum { IDD = IDD_PP_PROGRESS };
	CStatic	m_particularLabel;
	CStatic	m_overallLabel;
	CProgressCtrl	m_particularProgress;
	CProgressCtrl	m_overallProgress;
	//}}AFX_DATA

public:
   void SetOverallProgress(int percent);
   void SetOverallSteps(int steps);
   void SetOverallLabel(LPCTSTR lpText);
   void SetOverallLabel(UINT nResourceID);
   void SetParticularProgress(int percent);
   void SetParticularSteps(int steps);
   void IncParticularProgress();
   void SetParticularLabel(LPCTSTR lpText);
   void SetParticularLabel(UINT nResourceID);
   void SetTotalVideoCount(int nCount);
   void SetHasVideo(bool bHasVideo);

   bool IsCanceled() { return m_bCanceled || m_bContiniueLater; }
   bool IsWriteOnCancel() { return m_bContiniueLater; }

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CExportProgressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

   
public:
   enum
   {
      END_DIALOG = 99
   };

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CExportProgressDialog)
	virtual BOOL OnInitDialog();
   afx_msg LRESULT OnCheckEndDialog(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnExternalUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCancel();
	afx_msg void OnLater();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString m_overallString;
   CString m_particularString;
   int     m_totalVideoCount;
   int     m_currentVideoCount;
   // True if export has a normal video and not only clips
   bool    m_bHasVideo;

   HANDLE m_hDialogInitEvent;

   bool m_bCanceled;
   bool m_bContiniueLater;
   bool m_bShowLaterButton;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EXPORTPROGRESSDIALOG_H__7DFC2711_0931_4C49_8ABE_B2818901B4A5__INCLUDED_

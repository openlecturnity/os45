#if !defined(AFX_WRITINGAUDIODLG_H__3C4F70D7_0CC9_4F2C_92DB_CA59CF86194B__INCLUDED_)
#define AFX_WRITINGAUDIODLG_H__3C4F70D7_0CC9_4F2C_92DB_CA59CF86194B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Studio\resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CWritingAudioDlg 

class CWritingAudioDlg : public CDialog
{
// Konstruktion
public:
	CWritingAudioDlg(CWnd* pParent = NULL, 
      const _TCHAR *tszSourceFile=NULL, const _TCHAR *tszTargetFile=NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CWritingAudioDlg)
	enum { IDD = IDD_WRITING_AUDIO };
	CEdit	m_edTargetFile;
	CEdit	m_edSourceFile;
	CProgressCtrl	m_progress;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CWritingAudioDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

public:
   enum
   {
      END_DIALOG = 99,
      UPDATE_TARGET
   };

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CWritingAudioDlg)
	virtual BOOL OnInitDialog();
   afx_msg LRESULT OnCheckEndDialog(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString m_csSourceFile;
   CString m_csTargetFile;

public:
   bool bInitialized;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_WRITINGAUDIODLG_H__3C4F70D7_0CC9_4F2C_92DB_CA59CF86194B__INCLUDED_

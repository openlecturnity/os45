#if !defined(AFX_DEBUGDLG_H__13682EFF_98D0_4DE3_B21A_EE9DF6064296__INCLUDED_)
#define AFX_DEBUGDLG_H__13682EFF_98D0_4DE3_B21A_EE9DF6064296__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DebugDlg.h : Header-Datei
//

#include "AviSettings.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDebugDlg 

class CDebugDlg : public CDialog
{
// Konstruktion
public:
	CDebugDlg(LPCTSTR tszMessage, CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDebugDlg)
	enum { IDD = IDD_DEBUG_INFO };
	CEdit	m_edDebug;
	//}}AFX_DATA

   static void DisplayAviSettings(CWnd *pParent, CAviSettings *pAviSettings);
   static void DisplayMediaType(CWnd *pParent, AM_MEDIA_TYPE *pMt);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDebugDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDebugDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString m_csDebugMessage;

   static void CreateMediaTypeInfo(CString &csTarget, AM_MEDIA_TYPE *pMt);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DEBUGDLG_H__13682EFF_98D0_4DE3_B21A_EE9DF6064296__INCLUDED_

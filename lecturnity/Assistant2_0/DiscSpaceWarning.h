#if !defined(AFX_DISCSPACEWARNING_H__20D253C7_EFCC_4604_91A5_6AB45087A78B__INCLUDED_)
#define AFX_DISCSPACEWARNING_H__20D253C7_EFCC_4604_91A5_6AB45087A78B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiscSpaceWarning.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDiscSpaceWarning 

#include "..\Studio\Resource.h"

class CDiscSpaceWarning : public CDialog
{
// Konstruktion
public:
	CDiscSpaceWarning(CWnd* pParent = NULL);   // Standardkonstruktor

   void Update(LPCTSTR szRecorddrive, int iMinutesLeft);

// Dialogfelddaten
	//{{AFX_DATA(CDiscSpaceWarning)
	enum { IDD = IDD_DISCSPACE_WARNING };
	CStatic	m_wndText;
	CStatic	m_wndTitle;
	CStatic	m_wndMinutes;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDiscSpaceWarning)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDiscSpaceWarning)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CFont m_font;
   CStatic m_wndWarning;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DISCSPACEWARNING_H__20D253C7_EFCC_4604_91A5_6AB45087A78B__INCLUDED_

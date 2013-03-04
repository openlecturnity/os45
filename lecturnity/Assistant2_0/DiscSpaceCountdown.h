#if !defined(AFX_DISCSPACECOUNTDOWN_H__A443E6F1_84A6_4D8A_BAC3_265D25EEC7A4__INCLUDED_)
#define AFX_DISCSPACECOUNTDOWN_H__A443E6F1_84A6_4D8A_BAC3_265D25EEC7A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiscSpaceCountdown.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDiscSpaceCountdown 

#include "..\Studio\Resource.h"

class CDiscSpaceCountdown : public CDialog
{
// Konstruktion
public:
	CDiscSpaceCountdown(CWnd* pParent = NULL);   // Standardkonstruktor

   void Update(LPCTSTR szRecorddrive, int iMinutesLeft);

// Dialogfelddaten
	//{{AFX_DATA(CDiscSpaceCountdown)
	enum { IDD = IDD_DISCSPACE_COUNTDOWN };
	CStatic	m_wndSeconds;
	CStatic	m_wndText;
	CStatic	m_wndTitle;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDiscSpaceCountdown)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDiscSpaceCountdown)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
      
private:
   CFont m_font;
   CStatic m_wndWarning;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DISCSPACECOUNTDOWN_H__A443E6F1_84A6_4D8A_BAC3_265D25EEC7A4__INCLUDED_

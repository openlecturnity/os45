#include "afxwin.h"
#if !defined(AFX_TWOGBWARNING_H__F8D5582E_45D8_45B2_B129_0EB4C1B6F02D__INCLUDED_)
#define AFX_TWOGBWARNING_H__F8D5582E_45D8_45B2_B129_0EB4C1B6F02D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TwoGBWarning.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld TwoGBWarning 

#include "..\Studio\Resource.h"

class CTwoGBWarning : public CDialog
{
// Konstruktion
public:
	CTwoGBWarning(CWnd* pParent = NULL);   // Standardkonstruktor

   void Update(int iMinutesLeft);

// Dialogfelddaten
	//{{AFX_DATA(CTwoGBWarning)
	enum { IDD = IDD_2GB_WARNING };
	CStatic	m_wndTitle;
	CStatic	m_wndMinutes;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTwoGBWarning)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTwoGBWarning)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CFont m_font;
public:
   CStatic m_wndWarning;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TWOGBWARNING_H__F8D5582E_45D8_45B2_B129_0EB4C1B6F02D__INCLUDED_

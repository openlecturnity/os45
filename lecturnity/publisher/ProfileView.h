#if !defined(AFX_PROFILEVIEW_H__812E2C1E_5C4C_45D2_A887_15FA6E58FB3A__INCLUDED_)
#define AFX_PROFILEVIEW_H__812E2C1E_5C4C_45D2_A887_15FA6E58FB3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProfileView.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Formularansicht CProfileView 

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "TitleStatic.h" // MfcUtils

class CProfileView : public CFormView
{
protected:
	CProfileView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CProfileView)

// Formulardaten
public:
	//{{AFX_DATA(CProfileView)
	enum { IDD = IDD_PROFILE_VIEW };
	CButton	m_btPublish;
	CComboBox	m_cbbProfile;
	CTitleStatic	m_titleCaption;
	//}}AFX_DATA

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CProfileView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CProfileView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CProfileView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnStartPublish();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PROFILEVIEW_H__812E2C1E_5C4C_45D2_A887_15FA6E58FB3A__INCLUDED_

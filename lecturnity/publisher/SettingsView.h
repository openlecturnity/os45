#if !defined(AFX_SETTINGSVIEW_H__EC9409CA_2E92_4973_A969_67B21EF2D02D__INCLUDED_)
#define AFX_SETTINGSVIEW_H__EC9409CA_2E92_4973_A969_67B21EF2D02D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsView.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Formularansicht CSettingsView 

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "CustomPropPage.h" // MfcUtils
#include "MyTabCtrl.h" // MfcUtils
#include "TitleStatic.h" // MfcUtils

class CSettingsView : public CFormView
{
protected:
	CSettingsView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CSettingsView)

// Formulardaten
public:
	//{{AFX_DATA(CSettingsView)
	enum { IDD = IDD_SETTINGS_FORM };
	CTitleStatic	m_titleCaption;
	CComboBox	m_cbbTargetFormat;
	CMyTabCtrl	m_tabCtrl;
	//}}AFX_DATA

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSettingsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CSettingsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSettingsView)
	afx_msg void OnSettingsTabChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnChangeTargetFormat();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   // Contains the current property pages that are visible at the moment.
   CArray<CCustomPropPage *, CCustomPropPage *> m_caPropPages;

   virtual void RemoveAllPropPages();
   virtual void AddNeededPropPages();
   virtual void AddPropPage(int nID);
   virtual void PositionPropPages();
   virtual void CalcMinSize();

   int m_nLastDisplayedTarget;
   CRect m_rcMinSize;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SETTINGSVIEW_H__EC9409CA_2E92_4973_A969_67B21EF2D02D__INCLUDED_

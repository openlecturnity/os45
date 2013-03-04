// CustomPropPage.h: Schnittstelle für die Klasse CCustomPropPage.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMPROPPAGE_H__F45600C9_2626_45E4_B78F_1B3DD21241E7__INCLUDED_)
#define AFX_CUSTOMPROPPAGE_H__F45600C9_2626_45E4_B78F_1B3DD21241E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCustomPropPage : public CDialog
{
public:
	CCustomPropPage(UINT nId, CWnd *pWndParent=NULL);
	virtual ~CCustomPropPage();

   virtual BOOL Create(CWnd *pWndParent=NULL);

// Dialogfelddaten
	//{{AFX_DATA(CCustomPropPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCustomPropPage)
	protected:
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCustomPropPage)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CBrush m_bgBrush;
   bool   m_bBrushInited;

   UINT   m_nResID;
};

#endif // !defined(AFX_CUSTOMPROPPAGE_H__F45600C9_2626_45E4_B78F_1B3DD21241E7__INCLUDED_)

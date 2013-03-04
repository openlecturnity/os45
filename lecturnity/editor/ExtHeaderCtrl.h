#if !defined(AFX_EXTHEADERCTRL_H__8B961EFF_D417_4BC9_AFBA_294EB4218F9F__INCLUDED_)
#define AFX_EXTHEADERCTRL_H__8B961EFF_D417_4BC9_AFBA_294EB4218F9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtHeaderCtrl.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CExtHeaderCtrl 

class CExtHeaderCtrl : public CHeaderCtrl
{
// Konstruktion
public:
	CExtHeaderCtrl();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CExtHeaderCtrl)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CExtHeaderCtrl();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CExtHeaderCtrl)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	void OnBeginTrack(NMHDR * pNMHDR, LRESULT* pResult);
	void OnEndTrack(NMHDR * pNMHDR, LRESULT* pResult);
	void OnTrack(NMHDR * pNMHDR, LRESULT* pResult);
	void OnChangeItem(NMHDR * pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

private:
	bool m_bBeginTrack;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EXTHEADERCTRL_H__8B961EFF_D417_4BC9_AFBA_294EB4218F9F__INCLUDED_

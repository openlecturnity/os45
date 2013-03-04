#if !defined(AFX_MYTABCTRL_H__A6F60329_2D19_441F_8269_E6CCF075E54C__INCLUDED_)
#define AFX_MYTABCTRL_H__A6F60329_2D19_441F_8269_E6CCF075E54C__INCLUDED_

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


class LSUTL32_EXPORT CMyTabCtrl : public CTabCtrl
{
public:
	CMyTabCtrl();
	virtual ~CMyTabCtrl();

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
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MYTABCTRL_H__A6F60329_2D19_441F_8269_E6CCF075E54C__INCLUDED_)

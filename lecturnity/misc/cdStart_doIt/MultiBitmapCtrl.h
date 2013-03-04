#if !defined(AFX_MULTIBITMAPCTRL_H__5531B7E1_51A9_4E3F_BC0F_F83275445B18__INCLUDED_)
#define AFX_MULTIBITMAPCTRL_H__5531B7E1_51A9_4E3F_BC0F_F83275445B18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultiBitmapCtrl.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CMultiBitmapCtrl 

class CMultiBitmapCtrl : public CWnd
{
// Konstruktion
public:
	CMultiBitmapCtrl();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CMultiBitmapCtrl)
	public:
	//}}AFX_VIRTUAL
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, int nPicCount, UINT nBitmapID, CCreateContext* pContext = NULL);

   virtual void SetImageNr(int nr);

// Implementierung
public:
	virtual ~CMultiBitmapCtrl();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CMultiBitmapCtrl)
	afx_msg void OnPaint();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CImageList m_imageList;
   int        m_imageNr;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_MULTIBITMAPCTRL_H__5531B7E1_51A9_4E3F_BC0F_F83275445B18__INCLUDED_

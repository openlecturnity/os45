#if !defined(AFX_BITMAPTEXTSTATIC_H__93C8CE00_38A4_4D40_8C0A_80CC73A04154__INCLUDED_)
#define AFX_BITMAPTEXTSTATIC_H__93C8CE00_38A4_4D40_8C0A_80CC73A04154__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapTextStatic.h : Header-Datei
//

#include <afxctl.h>

/////////////////////////////////////////////////////////////////////////////
// Fenster CBitmapTextStatic 

class CBitmapTextStatic : public CWnd
{
// Konstruktion
public:
	CBitmapTextStatic();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CBitmapTextStatic)
	public:
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CBitmapTextStatic();
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, const POINT& point, CWnd* pParentWnd, UINT nID, UINT nBitmapID, CCreateContext* pContext = NULL);
   virtual BOOL SetFont(int logPixelSize, const CString &typeFace);
   virtual void SetCustomTextColor(COLORREF color) { m_textColor = color; }

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CBitmapTextStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CPoint         m_textPos;
   CPictureHolder m_pictureHolder;
   CFont          m_font;

   COLORREF       m_textColor;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_BITMAPTEXTSTATIC_H__93C8CE00_38A4_4D40_8C0A_80CC73A04154__INCLUDED_

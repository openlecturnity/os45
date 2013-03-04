#if !defined(AFX_IMAGEBUTTON_H__8CDBB96E_F783_457E_925D_887F7E4006D7__INCLUDED_)
#define AFX_IMAGEBUTTON_H__8CDBB96E_F783_457E_925D_887F7E4006D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageButton.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CImageButton 

class CImageButton : public CButton
{
// Konstruktion
public:
	CImageButton();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CImageButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CImageButton();
   BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, CRect rect, CWnd* pParentWnd, UINT nID, UINT bitmapID, int cx, int cy, int nImgs=3, HRGN hRgn = NULL);

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CImageButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	void DrawButton(CDC * pDC, CRect * pRect, UINT state);				// draws button to the screen
	BOOL HitTest(CPoint point);											// determines if point is inside the button region
	void CheckHover(CPoint point);

public:
   void SetPressed(bool isPressed) {m_bButtonPressed = isPressed;}
   void SetMouseOver(bool isMouseOver) {m_bHover = isMouseOver;}

   void SetImageList(UINT nBitmapID, int cx, int cy, int nImgs, bool refresh=false);
   BOOL EnableWindow(BOOL bEnable=TRUE);

private:
   
	UINT m_nBorder;														// width of the border in pixels for 3D highlight
	LONG m_lfEscapement;												// orientation of the caption (in tenth of the degree as in LOGFONT)
   UINT m_nID;

   CImageList m_buttonImageList;
	BOOL m_bMouseDown;													// indicated that mouse is pressed down
   BOOL m_bButtonPressed;
	BOOL m_bHover;														// indicates if mouse is over the button
	BOOL m_bCapture;													// indicates that mouse is captured in the buton
	HRGN m_hRgn;														// region in screen coordinates
	BOOL m_bNeedBitmaps;			

   CFont m_font;
   UINT m_textX;
   UINT m_textY;
   int  m_nImgs; // How many images does this button contain?
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_IMAGEBUTTON_H__8CDBB96E_F783_457E_925D_887F7E4006D7__INCLUDED_

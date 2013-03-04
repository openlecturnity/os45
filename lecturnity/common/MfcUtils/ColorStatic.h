// ColorStatic.h: Schnittstelle für die Klasse CColorStatic.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORSTATIC_H__2A1B1C89_0509_4424_8A91_A34418A39833__INCLUDED_)
#define AFX_COLORSTATIC_H__2A1B1C89_0509_4424_8A91_A34418A39833__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_COLOR_CHANGED (WM_USER+1234)

class CColorStatic : public CButton
{
public:
	CColorStatic();
	virtual ~CColorStatic();

   virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

   void SetColorRef(COLORREF colorRef);
   COLORREF GetColorRef() { return m_colorRef; }
protected:
   afx_msg void OnPaint();
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

   DECLARE_MESSAGE_MAP()

   COLORREF m_colorRef;
};

#endif // !defined(AFX_COLORSTATIC_H__2A1B1C89_0509_4424_8A91_A34418A39833__INCLUDED_)

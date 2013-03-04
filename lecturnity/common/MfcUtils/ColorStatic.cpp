// ColorStatic.cpp: Implementierung der Klasse CColorStatic.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorStatic.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CColorStatic::CColorStatic() : CButton()
{
   m_colorRef = RGB(0, 0, 0); // Black
}

CColorStatic::~CColorStatic()
{

}

BEGIN_MESSAGE_MAP(CColorStatic, CButton)
	//{{AFX_MSG_MAP(CCustomPropPage)
	ON_WM_PAINT()
   ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColorStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   OnPaint();
}

void CColorStatic::OnPaint()
{
   PAINTSTRUCT paintStruct;

   DWORD dwRes = GetUpdateRect(NULL);
   if (dwRes == 0)
      return;
   CDC *pDC = BeginPaint(&paintStruct);

   CRect rect;
   GetClientRect(&rect);
   CBrush brush(m_colorRef);
   pDC->FillRect(&rect, &brush);
   brush.DeleteObject();

   EndPaint(&paintStruct);
}

void CColorStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
   CColorDialog colorDlg(m_colorRef, CC_FULLOPEN | CC_SOLIDCOLOR, this);
   
   if (IDOK == colorDlg.DoModal())
   {
      m_colorRef = colorDlg.GetColor();
      RedrawWindow();
      GetParent()->SendMessage(WM_COLOR_CHANGED, (WPARAM) GetColorRef(), (LPARAM) this);
   }
}

void CColorStatic::SetColorRef(COLORREF colorRef)
{
   m_colorRef = colorRef;
   RedrawWindow();
}

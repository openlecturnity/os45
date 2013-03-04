#include "stdafx.h"
#include "Resource.h"
#include "PresentationBarControls.h"

BEGIN_MESSAGE_MAP(CPresentationBarControls, CXTPButton)
    ON_WM_MOUSEMOVE()
    ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CPresentationBarControls, CXTPButton)

CPresentationBarControls::CPresentationBarControls()
{
   m_bSelected = FALSE;
   m_nSplitButtonDropDownWidth = 0;

   m_nPosition = OT_BOTTOMMIDDLE;
   m_bIsHorizontal = true;

   SetFlatStyle(TRUE);
   SetTransparent(TRUE);
   SetShowFocus(FALSE);
}

CPresentationBarControls::~CPresentationBarControls()
{  
   m_pIcon = NULL;
}

void CPresentationBarControls::OnDraw(CDC* pDC)
{
   CXTPButton::OnDraw(pDC);

   CXTPClientRect rc(this);
   CXTPBufferDC memDC(*pDC, rc);

   HBRUSH hBrush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORBTN, (WPARAM)memDC.GetSafeHdc(), (LPARAM)GetSafeHwnd());
   if (hBrush)
   {
      ::FillRect(memDC.GetSafeHdc(), rc, hBrush);
   }
   else
   {
      memDC.FillSolidRect(rc, GetSysColor(COLOR_3DFACE));
   }

   CFont* pOldFont = memDC.SelectObject(CWnd::GetFont());
   memDC.SetBkMode(TRANSPARENT);

   // Background
   if(!IsDropDownStyle())
      DrawButtonBackground(&memDC);
   else
      DrawPushButtonDropDown(&memDC);

   // Icon
   DrawPushButtonIcon(&memDC);

   memDC.SelectObject(pOldFont);
}

void CPresentationBarControls::DrawPushButtonIcon(CDC* pDC)
{
	CSize sz = GetImageSize();
	if (sz == CSize(0, 0))
		return;

   CRect rcButton(GetButtonRect());
   long x, y;

   x = (rcButton.Width() - m_nSplitButtonDropDownWidth - sz.cx)/2;
   y = (rcButton.Height() - sz.cy)/2;
   
   if(m_bIsHorizontal)
   {
      x = (rcButton.Width() - m_nSplitButtonDropDownWidth - sz.cx)/2;
      y = (rcButton.Height() - sz.cy)/2;
   }
   else
   {
      x = (rcButton.Width() - sz.cx)/2;
      y = (rcButton.Height() - m_nSplitButtonDropDownWidth - sz.cy)/2;
   }

	DrawImage(pDC, CRect(CPoint(x,y), sz));
}

void CPresentationBarControls::SetPushButtonStyle(XTPPushButtonStyle nPushButtonStyle) 
{
   CXTPButton::SetPushButtonStyle(nPushButtonStyle);

   if(nPushButtonStyle == xtpButtonSplitDropDown)
      m_nSplitButtonDropDownWidth = 8;
}

void CPresentationBarControls::SetSmallIcons(bool bSmallIcons)
{
   if(bSmallIcons)
      m_nSplitButtonDropDownWidth = 4;
   else
      m_nSplitButtonDropDownWidth = 8;
}

void CPresentationBarControls::DrawButtonBackground(CDC* pDC)
{
   BOOL bPressed = IsPushed();
   BOOL bHot = IsHighlighted();
   BOOL bChecked = GetChecked();
   BOOL bEnabled = IsWindowEnabled();

   if(!bEnabled)
      m_bHot = FALSE;

   CRect rcButton(GetButtonRect());

   if (m_hBackgroundImage == NULL)
      UpdateBackground();
   pDC->BitBlt(0, 0, rcButton.Width(), rcButton.Height(), m_pBackgroundDc, 0, 0, SRCCOPY);

   CXTPOffice2007Image* pImage = NULL;

   BOOL bSmallSize = rcButton.Height() < 33;
   pImage = XTPOffice2007Images()->LoadFile(bSmallSize ? _T("TOOLBARBUTTONS22") : _T("TOOLBARBUTTONS50"));
   if (!pImage)
      return;

   int nState = -1;

   if(bEnabled)
   {
      if (bChecked && !bHot && !bPressed) nState = 2;
      else if (bChecked && bHot && !bPressed) nState = 3;
      else if (bHot && bPressed) nState = 1;
      else if (bHot || bPressed) nState = 0;
   }

   if (nState != -1)
      pImage->DrawImage(pDC, rcButton, pImage->GetSource(nState, 4), CRect(8, 8, 8, 8), COLORREF_NULL);
}

void CPresentationBarControls::SetPosition(UINT nPos)
{
   m_nPosition = nPos;
   bool bIsHorizontal = (nPos == OT_BOTTOMMIDDLE || nPos == OT_TOPMIDDLE);
   m_bIsHorizontal = bIsHorizontal;
}

void CPresentationBarControls::DrawPushButtonDropDown(CDC* pDC)
{
   BOOL bPressed = IsPushed();
   BOOL bHot = IsHighlighted();
   BOOL bEnabled = IsWindowEnabled();
   BOOL bChecked = GetChecked();
   BOOL bSelected = GetChecked() || IsPushed();// || IsHighlighted();

   if(!bEnabled)
      m_bHot = FALSE;
 
   CRect rcButton(GetButtonRect());
   pDC->BitBlt(0, 0, rcButton.Width(), rcButton.Height(), m_pBackgroundDc, 0, 0, SRCCOPY);
		
   CXTPOffice2007Image* pImageSplit, *pImageSplitDropDown;
   CRect rcSplit, rcSplitSrc, rcSplitDropDown, rcSplitDropDownSrc;

   
   if(m_bIsHorizontal)
   {
      pImageSplit = XTPOffice2007Images()->LoadFile(_T("TOOLBARBUTTONSSPLIT22"));
      pImageSplitDropDown = XTPOffice2007Images()->LoadFile(_T("TOOLBARBUTTONSSPLITDROPDOWN22"));

      rcSplit = CRect(rcButton.left, rcButton.top, rcButton.right - m_nSplitButtonDropDownWidth, rcButton.bottom);
      rcSplitDropDown = CRect(rcButton.right - m_nSplitButtonDropDownWidth, rcButton.top, rcButton.right, rcButton.bottom);
   }
   else
   {
      pImageSplit = XTPOffice2007Images()->LoadFile(_T("TOOLBARBUTTONSSPLIT50"));
      pImageSplitDropDown = XTPOffice2007Images()->LoadFile(_T("TOOLBARBUTTONSSPLITDROPDOWN50"));

      rcSplit = CRect(rcButton.left, rcButton.top, rcButton.right, rcButton.bottom - m_nSplitButtonDropDownWidth);
      rcSplitDropDown = CRect(rcButton.left, rcButton.bottom - m_nSplitButtonDropDownWidth, rcButton.right, rcButton.bottom);
   }
   rcSplitSrc  = pImageSplit->GetSource(0, 6);
   rcSplitDropDownSrc = pImageSplitDropDown->GetSource(0, 5);
   
   int nStateSplit = -1;

   BOOL bMouseInSplit = m_bSelected == TRUE_SPLITCOMMAND;
	BOOL bMouseInSplitDropDown = m_bSelected == TRUE_SPLITDROPDOWN;

   if(bEnabled)
   {
      if (bChecked)
      {
         if (!bHot && !bPressed) nStateSplit = 2;
         else if (bHot && !bPressed) nStateSplit = 3;
         else if ((bHot && bPressed)) nStateSplit = 1;
         else if (bPressed) nStateSplit = 2;
         else if (bHot || bPressed) nStateSplit = !bMouseInSplit ? 5 : 0;
      }
      else
      {
         if ((bHot && bPressed)) nStateSplit = 1;
         else if (bHot || bPressed) nStateSplit = !bMouseInSplit ? 5 : 0;
      }
   }
   
   if (nStateSplit != -1)
	{
		rcSplitSrc.OffsetRect(0, rcSplitSrc.Height() * nStateSplit);
		pImageSplit->DrawImage(pDC, rcSplit, rcSplitSrc,
			CRect(2, 2, 2, 2), 0xFF00FF, XTP_DI_COMPOSITE);
	}

   // Split button
   int nStateSplitDropDown = -1;

   if(bEnabled)
   {
      if (bChecked)
      {
         if (!bHot && !bPressed) nStateSplitDropDown = 2;
         else if (bHot && bPressed) nStateSplitDropDown = 0;
         else if (bHot || bPressed) nStateSplitDropDown = !bMouseInSplitDropDown ? 4 : 0;
         else nStateSplitDropDown = 4;
      }
      else
      {
         if (bHot && bPressed) nStateSplitDropDown = 0;
         else if (bHot || bPressed) nStateSplitDropDown = !bMouseInSplitDropDown ? 4 : 0;
      }
   }

   if (nStateSplitDropDown != -1)
	{
		rcSplitDropDownSrc.OffsetRect(0, rcSplitDropDownSrc.Height() * nStateSplitDropDown);
		pImageSplitDropDown->DrawImage(pDC, rcSplitDropDown, rcSplitDropDownSrc, CRect(2, 2, 2, 2), 
         0xFF00FF, XTP_DI_COMPOSITE);
	}

   // Glyph
   CXTPOffice2007Image* pImageGlyph = XTPOffice2007Images()->LoadFile(_T("TOOLBARBUTTONDROPDOWNGLYPH"));

	if (!pImageGlyph)
		return;

   CPoint pt;
   CRect rc;
  
   if(m_bIsHorizontal)
      pt = CPoint(rcButton.right - m_nSplitButtonDropDownWidth / 2, rcButton.CenterPoint().y);
   else
      pt = CPoint(rcButton.CenterPoint().x, rcButton.bottom - m_nSplitButtonDropDownWidth / 2 );

   if(m_nSplitButtonDropDownWidth == 8)
      rc = CRect(pt.x - 3, pt.y - 2, pt.x + 2, pt.y + 2);
   else
      rc = CRect(pt.x - 1.5, pt.y - 1, pt.x + 1, pt.y + 1);

   pImageGlyph->DrawImage(pDC, rc, pImageGlyph->GetSource(!bEnabled ? 3 : bSelected ? 1 : 0, 4),
		CRect(0, 0, 0, 0), 0xFF00FF, XTP_DI_COMPOSITE) ;
}

void CPresentationBarControls::OnMouseMove(UINT nFlags, CPoint point)
{
	CXTPButton::OnMouseMove(nFlags, point);

   CRect rcDropDown(GetButtonRect());
   if(m_bIsHorizontal)
      rcDropDown.left = rcDropDown.right - m_nSplitButtonDropDownWidth;
   else
      rcDropDown.top = rcDropDown.bottom - m_nSplitButtonDropDownWidth;

   if (IsWindowEnabled() && IsDropDownStyle())
   {
      if (rcDropDown.PtInRect(point) && m_bSelected != TRUE_SPLITDROPDOWN)
      {
         m_bSelected = TRUE_SPLITDROPDOWN;
         RedrawWindow();
      }
      else if(!rcDropDown.PtInRect(point) && m_bSelected != TRUE_SPLITCOMMAND)
      {
         m_bSelected = TRUE_SPLITCOMMAND;
         RedrawWindow();
      }
   }
}

void CPresentationBarControls::OnMouseLeave()
{
   CXTPButton::OnMouseLeave();

   m_bSelected = FALSE;
}

void CPresentationBarControls::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nPushButtonStyle == xtpButtonSplitDropDown)
	{
		CRect rcDropDown(GetButtonRect());
      if(m_bIsHorizontal)
         rcDropDown.left = rcDropDown.right - m_nSplitButtonDropDownWidth;
      else
         rcDropDown.top = rcDropDown.bottom - m_nSplitButtonDropDownWidth;

      if (rcDropDown.PtInRect(point))
		{
			DoDropDown();
			return;
		}
	}
 
	CXTPButtonBase::OnLButtonDown(nFlags, point);
}

void CPresentationBarControls::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nPushButtonStyle == xtpButtonSplitDropDown)
	{
		CRect rcDropDown(GetButtonRect());
      if(m_bIsHorizontal)
         rcDropDown.left = rcDropDown.right - m_nSplitButtonDropDownWidth;
      else
         rcDropDown.top = rcDropDown.bottom - m_nSplitButtonDropDownWidth;

      if (rcDropDown.PtInRect(point))
		{
			DoDropDown();
			return;
		}
	}
 
	CXTPButtonBase::OnLButtonUp(nFlags, point);
}

BOOL CPresentationBarControls::SetIcon(CSize /*size*/, CXTPImageManagerIcon* pIcon, BOOL bRedraw/*= TRUE*/)
{
	m_pIcon = pIcon;

	// Redraw the button.
	if (bRedraw)
		RedrawButton();

	return TRUE;
}

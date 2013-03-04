// XTPControlPopupColor.cpp : implementation of the CXTPControlPopupColor class.
//
// This file is a part of the Xtreme Toolkit Pro for MFC.
// ©1998-2003 Codejock Software, All Rights Reserved.
//
// This source code can only be used under the terms and conditions 
// outlined in the accompanying license agreement.
//
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ControlArrowStyleSelector.h"

#define astFlatFlat 0
#define astFlatArrow 1
#define astArrowFlat 2
#define astArrowArrow 3
#define astFlatTriangle 4
#define astTriangleFlat 5
#define astTriangleTriangle 6
#define astDiamondTriangle 7
#define astRoundTriangle 8
#define astDiamondDiamond 9
#define astRoundRound 10

struct CControlArrowStyleSelector::CArrowStyle
{
	CArrowStyle(int _iArrowStyle, Gdiplus::LineCap _startCap, Gdiplus::LineCap _endCap)
	{
		iArrowStyle = _iArrowStyle;
/*		toolTipID = _toolTipID;
		dashValues = (Gdiplus::REAL *)_dashValues;
		iDashValueCount = _iDashValueCount;*/
      startCap = _startCap;
      endCap   = _endCap;
	}

	int		iArrowStyle;		
/*	Gdiplus::REAL *dashValues;
	int iDashValueCount;
	UINT	toolTipID;
	CString strTip;*/
   Gdiplus::LineCap startCap;
   Gdiplus::LineCap endCap;
};

static CControlArrowStyleSelector::CArrowStyle arrowStyles[] =
{
   CControlArrowStyleSelector::CArrowStyle(astFlatFlat, Gdiplus::LineCapFlat, Gdiplus::LineCapFlat),
	CControlArrowStyleSelector::CArrowStyle(astFlatArrow, Gdiplus::LineCapFlat , Gdiplus::LineCapArrowAnchor),
	CControlArrowStyleSelector::CArrowStyle(astArrowFlat, Gdiplus::LineCapArrowAnchor , Gdiplus::LineCapFlat),
	CControlArrowStyleSelector::CArrowStyle(astArrowArrow, Gdiplus::LineCapArrowAnchor , Gdiplus::LineCapArrowAnchor),
	CControlArrowStyleSelector::CArrowStyle(astFlatTriangle, Gdiplus::LineCapFlat , Gdiplus::LineCapTriangle),
	CControlArrowStyleSelector::CArrowStyle(astTriangleFlat, Gdiplus::LineCapTriangle , Gdiplus::LineCapFlat),
   CControlArrowStyleSelector::CArrowStyle(astTriangleTriangle, Gdiplus::LineCapTriangle , Gdiplus::LineCapTriangle),
	CControlArrowStyleSelector::CArrowStyle(astDiamondTriangle, Gdiplus::LineCapDiamondAnchor , Gdiplus::LineCapTriangle),
	CControlArrowStyleSelector::CArrowStyle(astRoundTriangle, Gdiplus::LineCapRoundAnchor , Gdiplus::LineCapTriangle),
   CControlArrowStyleSelector::CArrowStyle(astDiamondDiamond, Gdiplus::LineCapDiamondAnchor , Gdiplus::LineCapDiamondAnchor),
   CControlArrowStyleSelector::CArrowStyle(astRoundRound, Gdiplus::LineCapRoundAnchor , Gdiplus::LineCapRoundAnchor)
};

IMPLEMENT_XTP_CONTROL(CControlArrowStyleSelector, CXTPControl)


CControlArrowStyleSelector::CControlArrowStyleSelector()
{
	m_nSelected = -1;
	m_iArrowStyle = astFlatFlat;
	SetFlags(xtpFlagNoMovable);

/*	int nCount = sizeof(arrowStyles) / sizeof(arrowStyles[0]);
	for (int i = 0; i < nCount; i++)
	{
		arrowStyles[i].strTip.LoadString(arrowStyles[i].toolTipID);
	}*/

	m_nPressed = -1;
}

CSize CControlArrowStyleSelector::GetSize(CDC* /*pDC*/)
{
	int nCount = sizeof(arrowStyles) / sizeof(arrowStyles[0]);
	int iWidth = XTP_AS_SELECTOR_WIDTH + 2 * XTP_AS_SELECTOR_MARGIN;
	int iHeight = XTP_AS_SELECTOR_HEIGHT * nCount + 2 * XTP_AS_SELECTOR_MARGIN;
	return CSize (iWidth, iHeight);
}
/*
CString  CControlArrowStyleSelector::GetTooltip(LPPOINT pPoint, LPRECT lpRectTip, int* nHit)
{
	if (pPoint == NULL || lpRectTip == NULL || nHit == NULL) 
		return  _T("");

	int nIndex = HitTest(*pPoint);
	if (nIndex != -1)
	{
		*nHit = arrowStyles[nIndex].toolTipID;
		*lpRectTip = GetRect(nIndex);
		return arrowStyles[nIndex].strTip;
	}
	return _T("");
}*/

CRect CControlArrowStyleSelector::GetRect(int nIndex)
{
	int iX = XTP_AS_SELECTOR_MARGIN + m_rcControl.left;
	int iWidth = XTP_AS_SELECTOR_WIDTH - 2 * XTP_AS_SELECTOR_MARGIN;	
	int iY = XTP_AS_SELECTOR_MARGIN + m_rcControl.top + nIndex * XTP_AS_SELECTOR_HEIGHT;
	int iHeight = XTP_AS_SELECTOR_HEIGHT;

	return CRect(iX, iY, iX+iWidth, iY+iHeight);
}

int CControlArrowStyleSelector::HitTest(CPoint point)
{
	int nCount = sizeof(arrowStyles) / sizeof(arrowStyles[0]);
	
	if (!m_rcControl.PtInRect(point))
		return -1;

	for (int i = 0; i < nCount; i++)
	{
		if (GetRect(i).PtInRect(point))
			return i;
	}

	return -1;
}

void CControlArrowStyleSelector::Draw(CDC* pDC)
{
	CRect rc(m_rcControl);
	int nCount = sizeof(arrowStyles) / sizeof(arrowStyles[0]);
	
	for (int i = 0; i < nCount; i++)
	{
		CRect rcButton = GetRect(i);
		
		if (i == m_nSelected || arrowStyles[i].iArrowStyle == m_iArrowStyle)
		{
			BOOL bPressed = GetPressed() && i == m_nSelected;
			//GetPaintManager()->DrawControlEntry(pDC, &rcButton, m_bEnabled, i == m_nSelected, bPressed, arrowStyles[i].iArrowStyle == m_iArrowStyle, FALSE, GetParent()->GetPosition());
         GetPaintManager()->DrawRectangle(pDC, &rcButton, i == m_nSelected, bPressed, m_bEnabled, arrowStyles[i].iArrowStyle == m_iArrowStyle, FALSE, GetParent()->GetType(), GetParent()->GetPosition());
		}
			
		Gdiplus::Graphics gdipGraphics(pDC->m_hDC);
		Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 1);
      Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));
/*		if (arrowStyles[i].iArrowStyle == msoLineRoundDot)
			pen.SetDashCap(Gdiplus::DashCapTriangle);
		else
			pen.SetDashCap(Gdiplus::DashCapFlat);
		pen.SetDashPattern(arrowStyles[i].dashValues, arrowStyles[i].iDashValueCount);
		pen.SetDashStyle(Gdiplus::DashStyleCustom);*/
      
//      Gdiplus::Point points[3] = {Gdiplus::Point(0, 0), Gdiplus::Point(0, 0), Gdiplus::Point(0, 0)};
      Gdiplus::GraphicsPath startCapPath;
      Gdiplus::GraphicsPath endCapPath;

   // Create a CustomLineCap object with capPath as the stroke path.

      int yPos = rcButton.top + XTP_AS_SELECTOR_HEIGHT / 2;

      switch(arrowStyles[i].startCap)
      {
      case Gdiplus::LineCapArrowAnchor:
         {
            Gdiplus::AdjustableArrowCap capStart(9, 9, false);
            pen.SetCustomStartCap(&capStart);
            break;
         }

      case Gdiplus::LineCapTriangle:
         {
            Gdiplus::AdjustableArrowCap capStart(6, 5, true);
            pen.SetCustomStartCap(&capStart);
            break;
         }

      case Gdiplus::LineCapDiamondAnchor:
         {
            Gdiplus::Point points[4] = {Gdiplus::Point(rcButton.left , yPos ), Gdiplus::Point(rcButton.left + 5, yPos + 5), Gdiplus::Point(rcButton.left + 10, yPos), Gdiplus::Point(rcButton.left + 5, yPos - 5)};
            startCapPath.AddPolygon(points, 4);
            break;
         }
      case Gdiplus::LineCapRoundAnchor:
         {
            Gdiplus::Rect rect(rcButton.left, yPos - 5, 10, 10);
            startCapPath.AddEllipse(rect);
            break;
         }

      default:
         break;
      }

      switch(arrowStyles[i].endCap)
      {
      case Gdiplus::LineCapArrowAnchor:
         {
            Gdiplus::AdjustableArrowCap capEnd(9, 9, false);
            pen.SetCustomEndCap(&capEnd);
            break;
         }

      case Gdiplus::LineCapTriangle:
         {
            Gdiplus::AdjustableArrowCap capEnd(6, 5, true);
            pen.SetCustomEndCap(&capEnd);
            break;
         }

      case Gdiplus::LineCapDiamondAnchor:
         {
            Gdiplus::Point points[4] = {Gdiplus::Point(rcButton.right - 10 , yPos ), Gdiplus::Point(rcButton.right - 5, yPos + 5), Gdiplus::Point(rcButton.right, yPos), Gdiplus::Point(rcButton.right - 5, yPos - 5)};
            endCapPath.AddPolygon(points, 4);
            break;
         }
      case Gdiplus::LineCapRoundAnchor:
         {
            Gdiplus::Rect rect(rcButton.right - 10, yPos - 5, 10, 10);
            endCapPath.AddEllipse(rect);
            break;
         }

      default:
         break;
      }

			
		if (!m_bEnabled) 
		{
			pen.SetColor(Gdiplus::Color(255, 128, 128, 128));
         brush.SetColor(Gdiplus::Color(255, 128, 128, 128));
		}
		
		gdipGraphics.DrawLine(&pen, Gdiplus::Point(rcButton.left + 2, yPos), Gdiplus::Point(rcButton.right-2, yPos));
      
      gdipGraphics.FillPath(&brush,&startCapPath); 
      gdipGraphics.FillPath(&brush,&endCapPath); 

	}
}

void CControlArrowStyleSelector::OnMouseMove(CPoint point)
{
	int nHit = HitTest(point);
	if (nHit != -1)
	{
		m_nSelected = nHit;
		m_pParent->Invalidate(FALSE);
		
	}
}

BOOL CControlArrowStyleSelector::OnSetSelected(int bSelected)
{
	if (!bSelected) m_nSelected = -1;

	return CXTPControl::OnSetSelected(bSelected);
}

void CControlArrowStyleSelector::OnExecute()
{
	if (m_nPressed != -1) 
	{
		m_iArrowStyle = arrowStyles[m_nPressed].iArrowStyle;
	}
	CXTPControl::OnExecute();

}


int CControlArrowStyleSelector::GetArrowStyle()
{
	return m_iArrowStyle;
}

void CControlArrowStyleSelector::SetArrowStyle(int iArrowStyle)
{
	m_iArrowStyle = iArrowStyle;
	m_pParent->Invalidate(FALSE);
}


void CControlArrowStyleSelector::OnClick(BOOL bKeyboard, CPoint pt)
{
	if (!m_bEnabled) return;
	
	int nHit = HitTest(pt);
	if (nHit == -1) return;

	m_nPressed = nHit;

	if (!bKeyboard) 
	{			
		ClickToolBarButton();
	} 
	else
	{
		OnExecute();
	}
	m_nPressed = -1;
}
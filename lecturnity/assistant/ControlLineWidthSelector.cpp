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


#include "ControlLineWidthSelector.h"

struct CControlLineWidthSelector::CLineWidth
{
	CLineWidth(double _dLineWidth)
	{
		dLineWidth = _dLineWidth;
	}

	double dLineWidth;
};

static CControlLineWidthSelector::CLineWidth lineWidths[] =
{
	CControlLineWidthSelector::CLineWidth(1.0),
	CControlLineWidthSelector::CLineWidth(2.0),
	CControlLineWidthSelector::CLineWidth(3.0),
	CControlLineWidthSelector::CLineWidth(4.0),
	CControlLineWidthSelector::CLineWidth(5.0),
	CControlLineWidthSelector::CLineWidth(6.0),
	CControlLineWidthSelector::CLineWidth(7.0),
	CControlLineWidthSelector::CLineWidth(8.0),
	CControlLineWidthSelector::CLineWidth(9.0),
	CControlLineWidthSelector::CLineWidth(10.0)
};

IMPLEMENT_XTP_CONTROL(CControlLineWidthSelector, CXTPControl)


CControlLineWidthSelector::CControlLineWidthSelector()
{
	m_nSelected = -1;
	m_dLineWidth = 1.0;
	SetFlags(xtpFlagNoMovable);

	int nCount = sizeof(lineWidths) / sizeof(lineWidths[0]);
	
	m_nPressed = -1;
}

CControlLineWidthSelector::~CControlLineWidthSelector()
{
}

CSize CControlLineWidthSelector::GetSize(CDC* /*pDC*/)
{
	int nCount = sizeof(lineWidths) / sizeof(lineWidths[0]);
	int iWidth = XTP_LW_SELECTOR_WIDTH + 2 * XTP_LW_SELECTOR_MARGIN;
	int iHeight = XTP_LW_SELECTOR_HEIGHT * nCount + 2 * XTP_LW_SELECTOR_MARGIN;
	return CSize (iWidth, iHeight);
}

CString  CControlLineWidthSelector::GetTooltip(LPPOINT pPoint, LPRECT lpRectTip, int* nHit)
{
	/*if (pPoint == NULL || lpRectTip == NULL || nHit == NULL) 
		return  _T("");

	int nIndex = HitTest(*pPoint);
	if (nIndex != -1)
	{
		*nHit = lineWidths[nIndex].toolTipID;
		*lpRectTip = GetRect(nIndex);
		return lineWidths[nIndex].strTip;
	}*/
	return _T("");
}

CRect CControlLineWidthSelector::GetRect(int nIndex)
{
	int iX = XTP_LW_SELECTOR_MARGIN + m_rcControl.left;
	int iWidth = XTP_LW_SELECTOR_WIDTH - 2 * XTP_LW_SELECTOR_MARGIN;	
	int iY = XTP_LW_SELECTOR_MARGIN + m_rcControl.top + nIndex * XTP_LW_SELECTOR_HEIGHT;
	int iHeight = XTP_LW_SELECTOR_HEIGHT;

	return CRect(iX, iY, iX+iWidth, iY+iHeight);
}

CRect CControlLineWidthSelector::GetDrawRect(int nIndex)
{
	int iX = XTP_LW_SELECTOR_MARGIN + m_rcControl.left;
	int iWidth = XTP_LW_SELECTOR_WIDTH - 2*XTP_LW_SELECTOR_MARGIN;	
	int iHeight = (int)(lineWidths[nIndex].dLineWidth+0.5);
	int iY = XTP_LW_SELECTOR_MARGIN + m_rcControl.top + nIndex * XTP_LW_SELECTOR_HEIGHT + (int)(0.5*(XTP_LW_SELECTOR_HEIGHT - iHeight));

	return CRect(iX, iY, iX+iWidth, iY+iHeight);
}

int CControlLineWidthSelector::HitTest(CPoint point)
{
	int nCount = sizeof(lineWidths) / sizeof(lineWidths[0]);
	
	if (!m_rcControl.PtInRect(point))
		return -1;

	for (int i = 0; i < nCount; i++)
	{
		if (GetRect(i).PtInRect(point))
			return i;
	}

	return -1;
}

void CControlLineWidthSelector::Draw(CDC* pDC)
{
	CRect rc(m_rcControl);
	int nCount = sizeof(lineWidths) / sizeof(lineWidths[0]);
	
	for (int i = 0; i < nCount; i++)
	{
		CRect rcButton = GetRect(i);
		
		if (i == m_nSelected || lineWidths[i].dLineWidth == m_dLineWidth)
		{
			BOOL bPressed = GetPressed() && i == m_nSelected;
			GetPaintManager()->DrawControlEntry(pDC, &rcButton, m_bEnabled, i == m_nSelected, bPressed, lineWidths[i].dLineWidth == m_dLineWidth, FALSE, GetParent()->GetPosition());
		}
			
		Gdiplus::Graphics gdipGraphics(pDC->m_hDC);
		Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), lineWidths[i].dLineWidth);

		int yPos = rcButton.top + XTP_LW_SELECTOR_HEIGHT / 2;
			
		if (!m_bEnabled) 
		{
			pen.SetColor(Gdiplus::Color(255, 128, 128, 128));
		}
		
		gdipGraphics.DrawLine(&pen, Gdiplus::Point(rcButton.left, yPos), Gdiplus::Point(rcButton.right, yPos));

	}
}

void CControlLineWidthSelector::OnMouseMove(CPoint point)
{
	int nHit = HitTest(point);
	if (nHit != -1)
	{
		m_nSelected = nHit;
		m_pParent->Invalidate(FALSE);
		
	}
}

BOOL CControlLineWidthSelector::OnSetSelected(int bSelected)
{
	if (!bSelected) m_nSelected = -1;

	return CXTPControl::OnSetSelected(bSelected);
}

void CControlLineWidthSelector::OnExecute()
{
	if (m_nPressed != -1) 
	{
		m_dLineWidth = lineWidths[m_nPressed].dLineWidth;
	}
	CXTPControl::OnExecute();
}

double CControlLineWidthSelector::GetLineWidth()
{
	return m_dLineWidth;
}
void CControlLineWidthSelector::SetLineWidth(double dLineWidth)
{
	m_dLineWidth = dLineWidth;
	m_pParent->Invalidate(FALSE);
}

void CControlLineWidthSelector::OnClick(BOOL bKeyboard, CPoint pt)
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
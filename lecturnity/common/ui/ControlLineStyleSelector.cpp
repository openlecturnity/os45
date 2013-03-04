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
#include "ControlLineStyleSelector.h"


#define msoLineSolid 0
const float arrLineSolid[]  = {(float)1.0, (float)0.0};
#define msoLineSquareDot 1
const float arrLineSquareDot[] = {(float)1.0, (float)1.0};
#define msoLineLongDash 2
const float arrLineLongDash[] = {(float)2.0, (float)2.0};

struct CControlLineStyleSelector::CLineStyle
{
	CLineStyle(int _iLineStyle, UINT _toolTipID, float *_dashValues, int _iDashValueCount)
	{
		iLineStyle = _iLineStyle;
		toolTipID = _toolTipID;
		dashValues = (Gdiplus::REAL *)_dashValues;
		iDashValueCount = _iDashValueCount;
	}

	int		iLineStyle;		
	Gdiplus::REAL *dashValues;
	int iDashValueCount;
	UINT	toolTipID;
	CString strTip;
};

static CControlLineStyleSelector::CLineStyle lineStyles[] =
{
	CControlLineStyleSelector::CLineStyle(msoLineSolid, IDS_LS_SOLID, (float *)arrLineSolid, 2),
	CControlLineStyleSelector::CLineStyle(msoLineSquareDot, IDS_LS_SQUAREDOT, (float *)arrLineSquareDot, 2),
	CControlLineStyleSelector::CLineStyle(msoLineLongDash, IDS_LS_DASH, (float *)arrLineLongDash, 2),
};

IMPLEMENT_XTP_CONTROL(CControlLineStyleSelector, CXTPControl)


CControlLineStyleSelector::CControlLineStyleSelector()
{
	m_nSelected = -1;
	m_iLineStyle = msoLineSolid;
	SetFlags(xtpFlagNoMovable);

	int nCount = sizeof(lineStyles) / sizeof(lineStyles[0]);
	for (int i = 0; i < nCount; i++)
	{
		lineStyles[i].strTip.LoadString(lineStyles[i].toolTipID);
	}

	m_nPressed = -1;
}

CSize CControlLineStyleSelector::GetSize(CDC* /*pDC*/)
{
	int nCount = sizeof(lineStyles) / sizeof(lineStyles[0]);
	int iWidth = XTP_LS_SELECTOR_WIDTH + 2 * XTP_LS_SELECTOR_MARGIN;
	int iHeight = XTP_LS_SELECTOR_HEIGHT * nCount + 2 * XTP_LS_SELECTOR_MARGIN;
	return CSize (iWidth, iHeight);
}

CString  CControlLineStyleSelector::GetTooltip(LPPOINT pPoint, LPRECT lpRectTip, int* nHit)
{
	if (pPoint == NULL || lpRectTip == NULL || nHit == NULL) 
		return  _T("");

	int nIndex = HitTest(*pPoint);
	if (nIndex != -1)
	{
		*nHit = lineStyles[nIndex].toolTipID;
		*lpRectTip = GetRect(nIndex);
		return lineStyles[nIndex].strTip;
	}
	return _T("");
}

CRect CControlLineStyleSelector::GetRect(int nIndex)
{
	int iX = XTP_LS_SELECTOR_MARGIN + m_rcControl.left;
	int iWidth = XTP_LS_SELECTOR_WIDTH - 2 * XTP_LS_SELECTOR_MARGIN;	
	int iY = XTP_LS_SELECTOR_MARGIN + m_rcControl.top + nIndex * XTP_LS_SELECTOR_HEIGHT;
	int iHeight = XTP_LS_SELECTOR_HEIGHT;

	return CRect(iX, iY, iX+iWidth, iY+iHeight);
}

int CControlLineStyleSelector::HitTest(CPoint point)
{
	int nCount = sizeof(lineStyles) / sizeof(lineStyles[0]);
	
	if (!m_rcControl.PtInRect(point))
		return -1;

	for (int i = 0; i < nCount; i++)
	{
		if (GetRect(i).PtInRect(point))
			return i;
	}

	return -1;
}

void CControlLineStyleSelector::Draw(CDC* pDC)
{
	CRect rc(m_rcControl);
	int nCount = sizeof(lineStyles) / sizeof(lineStyles[0]);
	
	for (int i = 0; i < nCount; i++)
	{
		CRect rcButton = GetRect(i);
		
		if (i == m_nSelected || lineStyles[i].iLineStyle == m_iLineStyle)
		{
			BOOL bPressed = GetPressed() && i == m_nSelected;
			//GetPaintManager()->DrawControlEntry(pDC, &rcButton, m_bEnabled, 1, bPressed, lineStyles[i].iLineStyle == m_iLineStyle, FALSE, GetParent()->GetPosition());
         GetPaintManager()->DrawRectangle(pDC, &rcButton, i == m_nSelected, bPressed, m_bEnabled, lineStyles[i].iLineStyle == m_iLineStyle, FALSE,GetParent()->GetType(), GetParent()->GetPosition());
		}
			
		Gdiplus::Graphics gdipGraphics(pDC->m_hDC);
		Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 3);
		pen.SetDashCap(Gdiplus::DashCapFlat);
		pen.SetDashPattern(lineStyles[i].dashValues, lineStyles[i].iDashValueCount);
		pen.SetDashStyle(Gdiplus::DashStyleCustom);

		int yPos = rcButton.top + XTP_LS_SELECTOR_HEIGHT / 2;
			
		if (!m_bEnabled) 
		{
			pen.SetColor(Gdiplus::Color(255, 128, 128, 128));
		}
		
		gdipGraphics.DrawLine(&pen, Gdiplus::Point(rcButton.left, yPos), Gdiplus::Point(rcButton.right, yPos));

	}
}

void CControlLineStyleSelector::OnMouseMove(CPoint point)
{
	int nHit = HitTest(point);
	if (nHit != -1)
	{
		m_nSelected = nHit;
		m_pParent->Invalidate(FALSE);
		
	}
}

BOOL CControlLineStyleSelector::OnSetSelected(int bSelected)
{
	if (!bSelected) m_nSelected = -1;

	return CXTPControl::OnSetSelected(bSelected);
}

void CControlLineStyleSelector::OnExecute()
{
	if (m_nPressed != -1) 
	{
		m_iLineStyle = lineStyles[m_nPressed].iLineStyle;
	}
	CXTPControl::OnExecute();

}


int CControlLineStyleSelector::GetLineStyle()
{
	return m_iLineStyle;
}

void CControlLineStyleSelector::SetLineStyle(int iLineStyle)
{
	m_iLineStyle = iLineStyle;
	//m_pParent->Invalidate(FALSE);
}


void CControlLineStyleSelector::OnClick(BOOL bKeyboard, CPoint pt)
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
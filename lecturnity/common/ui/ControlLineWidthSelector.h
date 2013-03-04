// XTPControlPopupColor.h : interface for the CXTPControlPopupColor class.
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

#if !defined(__CONTROLPOPUPLINEWIDTH_H__)
#define __CONTROLPOPUPLINEWIDTH_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#define XTP_LW_SELECTOR_WIDTH 150
#define XTP_LW_SELECTOR_HEIGHT 15
#define XTP_LW_SELECTOR_MARGIN 2


class CControlLineWidthSelector: public CXTPControl
{
	DECLARE_XTP_CONTROL(CControlLineWidthSelector)
public:	
	CControlLineWidthSelector();
	~CControlLineWidthSelector();

	struct CLineWidth;

	double GetLineWidth();
	void SetLineWidth(double dLineWidth);

protected:
	CSize GetSize(CDC* pDC);
	void Draw(CDC* pDC);
	void OnMouseMove(CPoint point);
	BOOL OnSetSelected(int bSelected);
	void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	CString  GetTooltip(LPPOINT pPoint = 0, LPRECT lpRectTip = 0, int* nHit = 0);
	void OnExecute();

private:
	CRect GetRect(int nIndex);
	CRect GetDrawRect(int nIndex);
	int HitTest(CPoint point);
private:
	int m_nSelected;
	int m_nPressed;
	double m_dLineWidth;
};

#endif // #if !defined(__CONTROLPOPUPLINEWIDTH_H__)

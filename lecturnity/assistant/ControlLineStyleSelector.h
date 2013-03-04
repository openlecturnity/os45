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

#if !defined(__CONTROLPOPUPLINESTYLE_H__)
#define __CONTROLPOPUPLINESTYLE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define XTP_LS_SELECTOR_WIDTH 150
#define XTP_LS_SELECTOR_HEIGHT 15
#define XTP_LS_SELECTOR_MARGIN 2

class CControlLineStyleSelector: public CXTPControl
{
	DECLARE_XTP_CONTROL(CControlLineStyleSelector)
public:	
	CControlLineStyleSelector();

	struct CLineStyle;

	int GetLineStyle();
	void SetLineStyle(int iLineStyle);

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
	int HitTest(CPoint point);

private:
	int m_nSelected;
	int m_nPressed;
	int m_iLineStyle;
};

#endif // #if !defined(__CONTROLPOPUPLINESTYLE_H__)

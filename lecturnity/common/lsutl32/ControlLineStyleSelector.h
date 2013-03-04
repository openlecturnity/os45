#if !defined(__CONTROLPOPUPLINESTYLE_H__)
#define __CONTROLPOPUPLINESTYLE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define XTP_LS_SELECTOR_WIDTH 150
#define XTP_LS_SELECTOR_HEIGHT 15
#define XTP_LS_SELECTOR_MARGIN 2

/*
// !! This doesn't work anyway: Assistant uses other ids!
#ifndef IDS_LS_SOLID
	#define IDS_LS_SOLID                    202
#endif
#ifndef IDS_LS_SQUAREDOT
	#define IDS_LS_SQUAREDOT                208
#endif
#ifndef IDS_LS_DASH
	#define IDS_LS_DASH                     212
#endif
*/

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif


class LSUTL32_EXPORT CControlLineStyleSelector: public CXTPControl
{
	DECLARE_XTP_CONTROL(CControlLineStyleSelector)
public:	
	CControlLineStyleSelector();

	int GetObjLineStyle();
	void SetObjLineStyle(int iLineStyle);

protected:
	CSize GetSize(CDC* pDC);
	void Draw(CDC* pDC);
	void OnMouseMove(CPoint point);
	BOOL OnSetSelected(int bSelected);
	void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	//CString  GetTooltip(LPPOINT pPoint = 0, LPRECT lpRectTip = 0, int* nHit = 0);

private:
	CRect GetRect(int nIndex);
	int HitTest(CPoint point);

private:
	int m_nSelected;
    Gdiplus::DashStyle m_LineStyle;
    CArray<Gdiplus::DashStyle, Gdiplus::DashStyle> m_aAllLineStyles;
};

#endif // #if !defined(__CONTROLPOPUPLINESTYLE_H__)

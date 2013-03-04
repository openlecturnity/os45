#if !defined(__CONTROLPOPUPARROWSTYLE_H__)
#define __CONTROLPOPUPARROWSTYLE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define XTP_AS_SELECTOR_WIDTH 150
#define XTP_AS_SELECTOR_HEIGHT 15
#define XTP_AS_SELECTOR_MARGIN 2


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


class LSUTL32_EXPORT CControlArrowStyleSelector: public CXTPControl
{
	DECLARE_XTP_CONTROL(CControlArrowStyleSelector)
public:	
	CControlArrowStyleSelector();

	struct CArrowStyle;

	int GetArrowStyle();
	void SetArrowStyle(int iArrowStyle);

protected:
	CSize GetSize(CDC* pDC);
	void Draw(CDC* pDC);
	void OnMouseMove(CPoint point);
	BOOL OnSetSelected(int bSelected);
	void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
//	CString  GetTooltip(LPPOINT pPoint = 0, LPRECT lpRectTip = 0, int* nHit = 0);
	void OnExecute();

private:
	CRect GetRect(int nIndex);
	int HitTest(CPoint point);

private:
	int m_nSelected;
	int m_nPressed;
	int m_iArrowStyle;
};

#endif // #if !defined(__CONTROLPOPUPLINESTYLE_H__)

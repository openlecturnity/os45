#pragma once

class CPresentationBarControls : public CXTPButton
{
	DECLARE_DYNAMIC(CPresentationBarControls)

public:
	CPresentationBarControls();
	virtual ~CPresentationBarControls();
    

protected:
   DECLARE_MESSAGE_MAP()

   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMouseLeave();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

   virtual void OnDraw(CDC* pDC);  

//   CPresentationBarControlsTheme *m_pButtonTheme;
private:
   void DrawButtonBackground(CDC* pDC);
   void DrawPushButtonDropDown(CDC* pDC);
   void DrawPushButtonIcon(CDC* pDC);
   
   BOOL m_bSelected;
   UINT m_nSplitButtonDropDownWidth;
   UINT m_nPosition;
   bool m_bIsHorizontal;

public:
   void SetPosition(UINT nPos);
   void SetPushButtonStyle(XTPPushButtonStyle nPushButtonStyle);
   void SetSmallIcons(bool bSmallIcons);
   BOOL SetIcon(CSize size, CXTPImageManagerIcon* pIcon, BOOL bRedraw = TRUE);
};


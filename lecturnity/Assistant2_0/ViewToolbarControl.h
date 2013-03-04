#pragma once

class CViewToolbarControl : public CXTPButton
{
    DECLARE_DYNAMIC(CViewToolbarControl)

public:
    CViewToolbarControl();
    virtual ~CViewToolbarControl();


protected:
    DECLARE_MESSAGE_MAP()

    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

    virtual void OnDraw(CDC* pDC);  

    //   CViewToolbarControlTheme *m_pButtonTheme;
private:
    void DrawButtonBackground(CDC* pDC);
    void DrawPushButtonDropDown(CDC* pDC);
    void DrawPushButtonIcon(CDC* pDC);
    void DrawIconText(CDC* pDC);
    void DrawPushButtonText(CDC* pDC);


public:
    void SetPosition(UINT nPos);
    void SetPushButtonStyle(XTPPushButtonStyle nPushButtonStyle);
    void SetSmallIcons(bool bSmallIcons);
    BOOL SetIcon(CSize size, CXTPImageManagerIcon* pIcon, BOOL bRedraw = TRUE);
    void UpdateBackground(CDC *pBackgroundDC, int offX, int offY);
    void SetPaintManager(CXTPPaintManager *pPaintManager) {m_pPaintManager = pPaintManager;}
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    bool IsMouseButtonPressed() {return m_bButtonPressed;}
    void SetIconText(CString csIconText);
    void ResetIconText();

private:
    bool m_bButtonPressed;
    BOOL m_bSelected;
    UINT m_nSplitButtonDropDownWidth;
    UINT m_nPosition;
    bool m_bIsHorizontal;
    CXTPPaintManager *m_pPaintManager;
    CString m_csIconText;
    bool m_bUseIconText;
};


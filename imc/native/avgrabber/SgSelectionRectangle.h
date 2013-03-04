#pragma once


// CSgSelectionRectangleListener
class SgSelectionRectangleListener 
{
public:
    SgSelectionRectangleListener() {}
    ~SgSelectionRectangleListener() {}

public:
    virtual CRect &GetSelectionRect() = 0;
    virtual void ChangeSelectionRectangle(CRect &rcSelection, bool bUpdateControls) = 0;
    virtual void SetLastCustomRect(CRect &rcLastCustom) = 0;

};

// CSgSelectionRectangle

class CSgSelectionRectangle : public CWnd
{
	DECLARE_DYNAMIC(CSgSelectionRectangle)

public:
	CSgSelectionRectangle(SgSelectionRectangleListener *pSelectionRectangleListener);
	virtual ~CSgSelectionRectangle();
    
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

protected:
	DECLARE_MESSAGE_MAP()

public:
    void DrawSelectionRect(CDC *pDC, CRect &rcWindow, CRect &rcSelection);

private:
    SgSelectionRectangleListener *m_pSelectionRectangleListener;

private:
    enum {
        SELECT_CORNER_OFFSET = 3
    };
    UINT m_nModifyPos;
    CPoint m_ptStartDrag;
    BOOL m_bLeftButtonPressed;
};



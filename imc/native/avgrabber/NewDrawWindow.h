#pragma once

#include "afxtempl.h"
#include "objects.h"
// CNewDrawWindow

class CUndoListEntry
{
public:
    CUndoListEntry(long ts, int ls, int *list) {
        timeStamp  = ts;
        listSize   = ls;
        objectList = list;
    }

    ~CUndoListEntry() {
        delete[] objectList;
    }

    long timeStamp;
    int  listSize;
    int *objectList;
};

class CNewDrawWindow : public CWnd
{
	DECLARE_DYNAMIC(CNewDrawWindow)

public:
	CNewDrawWindow(AudioCapturer *pAC, CString csFileName);
    virtual ~CNewDrawWindow();

protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnDeleteAll();

    void SetTopLeft(int x, int y);

    void SelectTool(int nTool);
    void SetPenColor(Gdiplus::ARGB color);
    void SetFillColor(Gdiplus::ARGB color);
    void SetFilled(bool bIsFilled=true);
    void SetClosed(bool bIsClosed=true);
    void SetLineWidth(int nLineWidth);
    void SetLineStyle(int nLineStyle);
    void SetArrowStyle(int nArrowStyle);
    void SetArrowConfig(int nArrowConfig);
    void SetFont(LOGFONT *pLogFont);

    void Undo();
    void Redo();

    long GetLastUndoTime();
    long GetNextRedoTime();

    void Show();// { ::ShowWindow(hWnd_, SW_SHOW); }
    void Hide();// { ::ShowWindow(hWnd_, SW_HIDE); }

	bool IsPointerLaidDown(){return m_bIsPointerLaidDown;}
	bool IsPointerMode(){return m_bIsPointerMode;}

private:
   void AddObject(DrawSdk::DrawObject *pObject);
   void EndCreatePolygon();
   void LeaveTextMode();
   void InvalidateObject(DrawSdk::DrawObject *pObject);
   void InvalidatePointer();
   void GetBackground();
   void DoClip(int x, int y);

private:
    HCURSOR m_hTelepointer;
    HCURSOR m_hTelepointerInactive;
    HCURSOR m_hText;
    HCURSOR m_hLine;
    HCURSOR m_hRectangle;
    HCURSOR m_hOval;
    HCURSOR m_hFreehand;
    HCURSOR m_hPolygon;
    HCURSOR m_hPolyline;

    HCURSOR m_hMarkerBlueCursor;
    HCURSOR m_hMarkerOrangeCursor;
    HCURSOR m_hMarkerRedCursor;
    HCURSOR m_hMarkerYellowCursor;
    HCURSOR m_hMarkerGreenCursor;
    HCURSOR m_hMarkerMagentaCursor;

    HCURSOR m_hPenBlackCursor;
    HCURSOR m_hPenBlueCursor;
    HCURSOR m_hPenCyanCursor;
    HCURSOR m_hPenGreenCursor;
    HCURSOR m_hPenMagentaCursor;
    HCURSOR m_hPenRedCursor;
    HCURSOR m_hPenWhiteCursor;
    HCURSOR m_hPenYellowCursor;

    HBITMAP  m_hBackgroundImage;
    CDC      *m_pBackgroundDc;

    CMenu m_contextMenu;

    int m_nTool;
    Gdiplus::ARGB m_clrPen;
    Gdiplus::ARGB m_clrFill;
    bool m_bIsFilled;
    bool m_bIsClosed;
    int m_iLineWidth;
    int m_iLineStyle;
    int m_iArrowStyle;
    int m_iArrowConfig;
    LOGFONT m_logFont;
    
    bool m_bUseTelepointer;

    DrawSdk::Polygon *m_pFreehand;
    DrawSdk::Polygon *m_pPolygon;
    DrawSdk::Polygon *m_pLine;
    DrawSdk::DPoint *m_pLinePoints;
    DrawSdk::Rectangle *m_pRectangle;
    DrawSdk::Oval *m_pOval;
    DrawSdk::Text *m_pText;
    CString m_csText;
    DrawSdk::Marker *m_pMarker;

    bool m_bLButtonDown;
    bool m_bRButtonDown;
    bool m_bIsPolygonMode;
    bool m_bIsTextMode;
    bool m_bIsPointerMode;
    bool m_bIsPointerLaidDown;

    CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_arDrawObjects;
    CArray<int, int> m_arObjectSet;
    CArray<CUndoListEntry *, CUndoListEntry *> m_arUndoElements;
    int m_iUndoElementPos;

    CRect m_rcClip;
    CPoint m_ptLast;
    CPoint m_ptTemp;
    CPoint m_ptCursorHotSpot;
    bool  m_bRespectTempPoint;

    AudioCapturer *m_pAC;

	FILE *m_fpObj;
	FILE *m_fpEvq;
	long m_lObjectNumber;
	long m_lLastTime;
	CArray<long,long> m_csLastObjects;
};



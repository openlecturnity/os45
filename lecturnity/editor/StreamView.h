#if !defined(AFX_STREAMVIEW_H__3D852E77_2D0A_4DF4_B693_F211309E3288__INCLUDED_)
#define AFX_STREAMVIEW_H__3D852E77_2D0A_4DF4_B693_F211309E3288__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StreamView.h : Header-Datei
//

#include "streamctrl.h"
#include "timelinectrl.h"

/////////////////////////////////////////////////////////////////////////////
// Ansicht CStreamView 

class CStreamView : public CView
{
   friend class CStreamCtrl;
protected:
	CStreamView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CStreamView)

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CStreamView)
	public:
	virtual void OnInitialUpdate();
   virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CStreamView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CStreamView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnUnitMinutes();
	afx_msg void OnUpdateUnitMinutes(CCmdUI* pCmdUI);
	afx_msg void OnUnitSeconds();
	afx_msg void OnUpdateUnitSeconds(CCmdUI* pCmdUI);
	afx_msg void OnUnitMilliseconds();
	afx_msg void OnUpdateUnitMilliseconds(CCmdUI* pCmdUI);
	afx_msg void OnShowMarks();
	afx_msg void OnUpdateShowMarks(CCmdUI* pCmdUI);
	afx_msg void OnShowVideo();
	afx_msg void OnUpdateShowVideo(CCmdUI* pCmdUI);
	afx_msg void OnShowPages();
	afx_msg void OnUpdateShowPages(CCmdUI* pCmdUI);
	afx_msg void OnShowClips();
	afx_msg void OnUpdateShowClips(CCmdUI* pCmdUI);
	afx_msg void OnShowAudio();
	afx_msg void OnUpdateShowAudio(CCmdUI* pCmdUI);
    // used by hotkeys "+"
	afx_msg void OnZoomIn();
    // used by hotkeys "-"
	afx_msg void OnZoomOut();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

   afx_msg BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()

private:
   CTimelineCtrl  m_timelineCtrl;
   CStreamCtrl    m_streamCtrl;

	CXTPToolBar m_wndToolBar;

   // Scroll bar
   CScrollBar     m_displayScroll;

   // For keyboard control
   int            m_controlIsPressed;
   int            m_shiftIsPressed;
   int            m_keyMarkStartMs;
   int            m_keyMarkEndMs;

   HCURSOR        m_hPositionCursor;
   // Helper values that holds pixel position of previously drawn position slider. 
   // This is required for performace as stream view will redraw just when the position slider needs to be redrawn. Also all other controls from this view will be redrawn.
   // When is set to -1 will force a full view redraw.
   int            m_iCursorPositionPixel;

   // user want to see stream
   bool m_bUserHasChangedMarksVisibility;
   bool m_bUserHasChangedVideoVisibility;
   bool m_bUserHasChangedAudioVisibility;
   bool m_bUserHasChangedClipsVisibility;
   bool m_bUserHasChangedSlidesVisibility;
	
   bool            m_bHavePaused;
   int             m_lastDisplayPos;
   CPictureHolder  m_positionImage;
	CBitmap m_bmpSlider;

   static int     SCROLLBAR_HEIGHT;

   void UpdateDisplay(int newDisplayStart);
   void UpdateScrollBar();

   CRect m_rcStreamCtrl;
   CRect m_rcTimelineCtrl;


public:
   enum
   {
      MIN_DISPLAY_SEC = 1000,
		LEFT_OFFSET = 112
   };
   int GetStreamCtrlWidth();
   void UpdateStreamVisibilities(CEditorDoc *pDoc);
   void ShowSlideStream();
   void ResetUserVariables();
	void ShowChildWindows(bool bShow);
	HRESULT InsertStopmark(int iCheckID, int iTimestamp);
	HRESULT InsertTargetmark(CString &csName, int iCheckID, int iTimestamp);
   void UpdateSliderPos(CPoint& point, UINT nFlags, MouseStateId idMouseState);
   void UpdateSliderPos(bool bErase=true);
   void RedrawTimelineCtrl(CDC *pDC);
   void RedrawStreamCtrl(CDC *pDC);
   
   void SetInactive();
   void ZoomTo(int nZoom);
   void ZoomIn();
   void ZoomOut();
   void ZoomAll();
   void ZoomSelected();
   CStreamCtrl* GetStreamCtrl()  {return &m_streamCtrl;}
   /** This function must be called after application theme changed.
    *  Updates all images from m_timelineCtrl and m_streamCtrl controls 
    */
   void ColorSchemeChanged();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_STREAMVIEW_H__3D852E77_2D0A_4DF4_B693_F211309E3288__INCLUDED_

#if !defined(AFX_VIDEOVIEW_H__B86AEC8C_E7F5_424F_B390_0340826D16B5__INCLUDED_)
#define AFX_VIDEOVIEW_H__B86AEC8C_E7F5_424F_B390_0340826D16B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoView.h : Header-Datei
//

#include "videocontainer.h"

/////////////////////////////////////////////////////////////////////////////
// Ansicht CVideoView 

class CVideoView : public CView
{
protected:
	CVideoView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CVideoView)

// Attribute
public:

// Operationen
public:
   virtual HWND GetSafeVideoHwnd();
   void SetZoom(double zoom);
   double GetZoom() { return m_dZoomFactor; }
   void FitVideo();
   void FitVideo(int cx, int cy);

   HRESULT ClearClipTimes();
   HRESULT AddClipTimes(UINT nStartMs, UINT nEndMs);
   void SetIsNearlyOneClip(bool b) { m_bIsNearlyOneClip = b; }

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CVideoView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CVideoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CVideoView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg void OnZoom50();
   afx_msg void OnZoom100();
   afx_msg void OnZoom200();
   afx_msg void OnZoomFit();
	afx_msg void OnUpdateVideoZoom(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   HRESULT PrepareStillImage();
   bool IsClipTime(UINT nPositionMs);

   CRect m_rcVideoRect;

   CVideoContainer m_wndVideoContainer;
   double  m_dZoomFactor;

   CMenu m_ZoomPopup;

   CArray<UINT, UINT> m_aClipTimes;
   bool m_bIsNearlyOneClip; // one clip covering nearly all the length (but not all)
   CString m_csLastStillImage;

   Gdiplus::Bitmap *m_pBitmap;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_VIDEOVIEW_H__B86AEC8C_E7F5_424F_B390_0340826D16B5__INCLUDED_

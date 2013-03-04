#if !defined(AFX_STREAMCTRL_H__B019B0DA_5632_4516_AA75_0D78F04C6F16__INCLUDED_)
#define AFX_STREAMCTRL_H__B019B0DA_5632_4516_AA75_0D78F04C6F16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVStreams.h"
#include "LmdFile.h"
#include "MarkReaderWriter.h"
#include "InteractionAreaEx.h"
class CEditorDoc;

class CStreamCtrl : public CWnd
{
public:
	enum {
		STREAM_HEIGHT = 22,		// height of mark stream
		SELECTION_OFFSET = 2,			// pixel offset to find stopp- and targetmarks
		LEFT_BORDER  = 112,				// offset from left border to draw streams
		BUTTON_DOWN  = 0,
		BUTTON_UP    = 1,
		INITIALIZE   = 0,
		MODIFY       = 1,
		BITMAP		 = 0,
		MASK		 = 1
	};
// Konstruktion
public:
	CStreamCtrl();

// Attribute
public:

// Operationen
public:
   void SetEditorDoc(CEditorDoc *pEditorDoc) { m_pEditorDoc = pEditorDoc; }

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CStreamCtrl)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CStreamCtrl();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//{{AFX_MSG(CStreamCtrl)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClipProperties();
	afx_msg void OnClipRemove();
	afx_msg void OnUpdateClipRemove(CCmdUI *pCmdUI);
	afx_msg void OnVideoProperties();
	afx_msg void OnVideoRemove();
	afx_msg void OnPageProperties();
	afx_msg void OnAudioProperties();
	afx_msg void OnMarkPage();
	afx_msg void OnMarkClip();
	afx_msg void OnChangeClipTitle();;
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnInsertStopmark();
	afx_msg void OnRemoveMark();
    afx_msg void OnRemoveAllDemoDocumentObjects();
	afx_msg void OnInsertTargetmark();
	afx_msg void OnTargetmarkProperties();
    afx_msg void OnInsertDemoObject();
   
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   void ShowMarksStream(bool doShow) {m_bShowMarksStream = doShow;}
   bool IsMarksStreamShown()  {return m_bShowMarksStream;}
   void ShowVideoStream(bool doShow) {m_bShowVideoStream = doShow;}
   bool IsVideoStreamShown()  {return m_bShowVideoStream;}
   void ShowClipsStream(bool doShow) {m_bShowClipsStream = doShow;}
   bool IsClipStreamShown() {return m_bShowClipsStream;}
   void ShowSlidesStream(bool doShow) {m_bShowSlidesStream = doShow;}
   bool IsSlideStreamShown() {return m_bShowSlidesStream;}
   void ShowAudioStream(bool doShow) {m_bShowAudioStream = doShow;}
   bool IsAudioStreamShown()  {return m_bShowAudioStream;}

   void DrawItem(CRect &rcClient, CDC *pDC);

   void CalculateBitmapRectangles(CRect &rcSegment, int xBegin, int xEnd, int fromMsec, int toMsec,
                                  int firstDisplayMsec,
                                  int left, int right, int top, int bottom, bool &bCutFirst, bool &bCutLast);
   void DrawCursor(CDC *pDC, int left, int right, int top, int bottom);

	HRESULT InsertStopmark(int iCheckID, int iTimestamp);
	HRESULT InsertTargetmark(CString &csName, int iCheckID, int iTimestamp);
   bool LoadIconForStream(UINT nID);
   bool LoadBackgroundImages();
   void RemoveSlection();
   void ColorSchemeChanged();
private:
   void UpdateSelection(int x, int status);
	
   void DrawStreamCaption(CDC *pDC, CRect &rcCaption, UINT nTextId);
	void DrawSlidesStreamCaption(CDC *pDC, CRect &rcCaption);
	void DrawStreamBackground(CDC *pDC, CRect &rcStream);
   void DrawVideoStream(CDC *pDC, int left, int top, int right, int bottom);
   void DrawSlidesStream(CDC *pDC, int left, int top, int right, int bottom);
   void DrawClipsStream(CDC *pDC, int left, int top, int right, int bottom, int yOffset);
   void DrawAudioStream(CDC *pDC, int left, int top, int right, int bottom);

   int GetMsecFromPixel(int iPixelPos, bool bAddDisplayStartMs=true);
   int GetPixelFromMsec(int iMsec, int iOffset=0);
   int GetPixelFromMsec(int iMsec, int iOffset, CRect rcClient);
   void SetCursorClipRect();
   void UpdateCurrentPos(CPoint &point);
   void CalculateStreamRectangles();

	CStopJumpMark *FindMarkInRegion(UINT nFromMs, UINT nLength);

   bool PointIsInMarksStream(CPoint &pt);
   bool PointIsInClipsStream(CPoint &pt);
   bool PointIsInVideoStream(CPoint &pt);
   bool PointIsInSlidesStream(CPoint &pt);
   bool PointIsInQuestionObjects(CPoint &pt);
   bool PointIsInGraphicObjects(CPoint &pt);
   bool PointIsInInteractionObjects(CPoint &pt);
   bool PointIsInAudioStream(CPoint &pt);

public:
   CArray<CRect, CRect> m_arClipStreamRect;
   CArray<CRect, CRect> m_arClipAudioRect;

private:   
   CEditorDoc     *m_pEditorDoc;

   // Double buffer
   CBitmap     m_doubleBuffer;
   CDC         m_doubleBufferDC;
   CGdiObject *m_pOldObject;

   // Streams

   // Pens
   CPen m_penLight;
   CPen m_penMiddle;
   CPen m_penDark;
   //Audio Waveform pen
   CPen m_cpAudioWavePen;

	// common variables
   LOGFONT m_logFont;

   // variables for moving 
   int m_cursorPositionPixel;
   int m_markStartPixel;
   int m_iPixelDiff;
   int m_lastPixel;
   int m_lastMs;
   int m_iButtonStatus;

   //variables for timeline selected object 
   bool m_bMoveSelObj;
   bool m_bResizeLeftSelObj;
   bool m_bResizeRightSelObj;
   CInteractionAreaEx* m_pUsedArea;

   // variables for mark stream
   CRect	m_rcMarksStreamRect;
   bool	m_bMoveMark;
   bool	m_bMouseMoveOutOfMarksStream;
   bool	m_bShowMarksStream;
	CStopJumpMark *m_pContextMenuMark;
    bool m_bMoveDemoMark;
    int m_iDemoMarkOrigMs;

   // context menu for stop-/targetmarks
   CMenu m_marksPopup;						// common mark context menu
   CMenu m_stopmarkPopup;					// stopmark context menu
   CMenu m_targetmarkPopup;				// targetmark context menu
   int	m_nContextmenuTimestamp;		// timestamp where context menu is opened
	int	m_nContextmenuPixelPos;
   CMenu   m_createStructPopup; // create document structure context menu


   // vriables for video stream
   CRect m_rcVideoStreamRect;
   bool	m_bShowVideoStream;

   // context menu for video 
   CMenu m_videoPopup;
	CBitmap m_bmpBackground;


   // variables for clip stream
   CPictureHolder m_clipAudioNote;
   CRect	m_rcClipStreamRect;
   CArray<CClipInfo *, CClipInfo *>m_arSelectedClips;
   int	m_iMoveDistanceMs;
   bool	m_bMoveClip;
   bool  m_bMoveAudioClip;
   int	m_iMousePositionInClipPx;
   bool	m_bShowClipsStream;

   // context menu for clips 
   CMenu m_clipPopup;
   CString m_csPopupClipFilename;
   int m_nClipPopupPosMs;


   // Variables for slides stream
   CRect m_rcSlidesStreamRect;
   bool	m_bShowSlidesStream;

   // context menu for slides 
   CMenu m_pagePopup;
   int	m_nPagePopupPosMs;


   // Variables for audio stream
   CRect m_rcAudioStreamRect;
   bool	m_bShowAudioStream;

   // context menu for audio
   CMenu m_audioPopup;


   // Mouse cursors
   HCURSOR m_hInsertMarkCursor;
   HCURSOR m_hSelectCursor;
   HCURSOR m_hMoveCursor;
   HCURSOR m_hDeleteCursor;
   HCURSOR m_hResizeHCursor;

   CXTPImageManagerIconHandle m_iconHandle;

   CXTPImageManagerIcon* m_pIconCaption;
   CXTPImageManagerIcon* m_pIconVideo;
   CXTPImageManagerIcon* m_pIconSlides;

   CXTPOffice2007Image* m_pImageBackgroundVideo;
   CXTPOffice2007Image* m_pImageBackgroundSlides;
   CXTPOffice2007Image* m_pImageBackgroundClip;
   CXTPOffice2007Image* m_pImageBackgroundAudio;
   CXTPOffice2007Image* m_pImageBackgroundCaptionStream;
   CXTPOffice2007Image* m_pImageBackgroundStream;

   bool	m_bIconCaption;
   bool  m_bIconVideo;
   bool	m_bIconSlides;
   bool m_bBackgrounds;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_STREAMCTRL_H__B019B0DA_5632_4516_AA75_0D78F04C6F16__INCLUDED_

#if !defined(AFX_TIMELINECTRL_H__A00D9D8C_04D8_4BD4_B7FC_26F787A32640__INCLUDED_)
#define AFX_TIMELINECTRL_H__A00D9D8C_04D8_4BD4_B7FC_26F787A32640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "editorDoc.h"

enum
{
   UPDATE_FITVIDEO = 21
};

enum MouseStateId
{
   STATE_MOUSEDOWN,
   STATE_MOUSEMOVE,
   STATE_MOUSEUP
};


/////////////////////////////////////////////////////////////////////////////
// Fenster CTimelineCtrl 

class CTimelineCtrl : public CWnd
{
// Konstruktion
public:
	CTimelineCtrl();

// Attribute
public:

// Operationen
public:
   void SetEditorDoc(CEditorDoc *pEditorDoc) { m_pEditorDoc = pEditorDoc; }

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTimelineCtrl)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CTimelineCtrl();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CTimelineCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    void DrawItem(CRect &rcClient, CDC *pDC);
    void ColorSchemeChanged();
private:
    void DrawCursor(CDC *pDC, int left, int right, int top, int bottom);
    bool LoadBackgroundImage();


private:
   CEditorDoc     *m_pEditorDoc;
   CBitmap        m_markerLeftImage;
   CBitmap        m_markerRightImage;
   CPen            m_greyPen;
   CFont           m_tickFont;
	CBitmap m_bmpSlider;
    CPen m_penLight;
    CPen m_penMiddle;
    CPen m_penDark;

   // Double buffer
   CBitmap         m_doubleBuffer;
   CDC             m_doubleBufferDC;
   CGdiObject     *m_pOldObject;

   bool            m_bLeftMousePressed;

   static int     m_goodSpacingsMins[];
   static int     m_goodSpacingsSecs[];
   static int     m_goodSpacingsMillis[];
   static int     m_goodSpacingsFrames[];

   int FindGoodSpacing(int *spaces, int wishSpace);
   CXTPImageManagerIconHandle m_iconHandle;
   CXTPImageManagerIcon* m_pIconCursor;
   CXTPImageManagerIcon* m_pIconMaker;

   CXTPOffice2007Image* m_pImageBackground;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TIMELINECTRL_H__A00D9D8C_04D8_4BD4_B7FC_26F787A32640__INCLUDED_

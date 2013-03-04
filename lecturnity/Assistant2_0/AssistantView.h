// AssistantView.h : interface of the CAssistantView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASSISTANTVIEW_H__1A66FB61_FBB0_4CF0_8B2B_F29FED65CDCA__INCLUDED_)
#define AFX_ASSISTANTVIEW_H__1A66FB61_FBB0_4CF0_8B2B_F29FED65CDCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "backend/mlb_objects.h"

class CMainFrameA; // forward declaration to avoid circular includes
class CDrawingToolSettings;

class CAssistantView : public CView
{
private:
   enum MouseStateId
   {
      MOUSE_DOWN,
      MOUSE_MOVE,
      MOUSE_UP
   };

protected: // create from serialization only
	CAssistantView();
	DECLARE_DYNCREATE(CAssistantView)

// Attributes
public:
	CAssistantDoc* GetDocument();

// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssistantView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   
   void LeftMouseDown(CPoint &point);
   void LeftMouseMove(CPoint &point);
   void LeftMouseUp(CPoint &point);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	//}}AFX_VIRTUAL
   

// Implementation
public:
	virtual ~CAssistantView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
   
   void ChangeTool(int iCurrentTool);
   
   HRESULT EndCreateObjects();
   HRESULT DeleteEmptyText();

protected:
   HRESULT SelectTextArea(MouseStateId nMouseState, CPoint &point);
   HRESULT SelectObjects(MouseStateId nMouseState, CPoint &point);
   HRESULT MoveObjects(MouseStateId nMouseState, CPoint &point);
   HRESULT SelectMoveObjects(MouseStateId nMouseState, CPoint &point);
   HRESULT ActivateHyperlink(Gdiplus::PointF &ptObject);
   HRESULT CreateObjects(MouseStateId nMouseState, CPoint &point);
   HRESULT UpdateColorSettings();
   HRESULT UpdateFontSettings();


// Generated message map functions
protected:
	//{{AFX_MSG(CAssistantView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnInsertImage();
   afx_msg void OnUpdateInsertImage(CCmdUI* pCmdUI);
	afx_msg void OnFillColor();
	afx_msg void OnUpdateFillColor(CCmdUI* pCmdUI);
	afx_msg void OnLineColor();
	afx_msg void OnUpdateLineColor(CCmdUI* pCmdUI);
	afx_msg void OnTextColor();
	afx_msg void OnUpdateTextColor(CCmdUI* pCmdUI);
   afx_msg void OnFontFamily(NMHDR* pNMHDR, LRESULT* pResult); 
   afx_msg void OnFontSize(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnUpdateFontList(CCmdUI* pCmdUI);
	afx_msg void OnFontSlant();
	afx_msg void OnUpdateFontSlant(CCmdUI* pCmdUI);
	afx_msg void OnFontWeight();
	afx_msg void OnUpdateFontWeight(CCmdUI* pCmdUI);
   afx_msg void OnFontSizeIncrease(NMHDR* pNMHDR, LRESULT* pResult); 
   afx_msg void OnFontSizeDecrease(NMHDR* pNMHDR, LRESULT* pResult); 
   afx_msg void OnUpdateFontSizeIncrease(CCmdUI *pCmdUI);
   afx_msg void OnUpdateFontSizeDecrease(CCmdUI *pCmdUI);
	afx_msg void OnLineWidth();
	afx_msg void OnUpdateLineWidth(CCmdUI* pCmdUI);
	afx_msg void OnLineStyle();
	afx_msg void OnUpdateLineStyle(CCmdUI* pCmdUI);
	afx_msg void OnArrowStyle();
	afx_msg void OnUpdateArrowStyle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFontFamily(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFontSize(CCmdUI* pCmdUI);
	afx_msg void OnChangeTool();
	afx_msg void OnUpdateTool(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopyTool(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnUndo();
	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnShowMosePointer();
	afx_msg void OnUpdateShowMosePointer(CCmdUI* pCmdUI);
	afx_msg void OnHideMousePointer();
	afx_msg void OnUpdateHideMousePointer(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
   
   afx_msg void OnNoFillColor();
	afx_msg void OnMoreFillColors();
   afx_msg void OnSelectFillColor(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnUpdateSelectFillColor(CCmdUI* pCmdUI);
   afx_msg void OnUpdateNoFillColor(CCmdUI* pCmdUI);

   afx_msg void OnNoLineColor();
	afx_msg void OnMoreLineColors();
	afx_msg void OnSelectLineColor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSelectLineColor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNoLineColor(CCmdUI* pCmdUI);

   afx_msg void OnNoTextColor();
	afx_msg void OnMoreTextColors();
	afx_msg void OnSelectTextColor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSelectTextColor(CCmdUI* pCmdUI);

   afx_msg void OnSelectLineWidth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSelectLineWidth(CCmdUI* pCmdUI);
   afx_msg void OnSelectLineStyle(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSelectLineStyle(CCmdUI* pCmdUI);

   afx_msg void OnSelectArrowStyle(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSelectArrowStyle(CCmdUI* pCmdUI);


   // Updates the gallery items check state.
   afx_msg void OnEnableGalleryButton(CCmdUI* pCmdUI);

   // Executes when user clicks on a pen from the Pencil's button drop down list.
   afx_msg void OnXTPGalleryPens(NMHDR* pNMHDR, LRESULT* pResult);
   //Executes when the user clicks on the Pens button from Pens ribbon group.
   afx_msg void OnButtonPen();
   // Update the state of the Marker button if the selected Marker changes from Presentation Toolbar
   afx_msg void OnUpdateButtonPen(CCmdUI* pCmdUI);

   // Executes when user clicks on a marker from the Marker's button drop down list.
   afx_msg void OnXTPGalleryMarkers(NMHDR* pNMHDR, LRESULT* pResult);
   // Executes when the user clicks on the Markers button from Markers ribbon group.
   afx_msg void OnButtonMarker();
   // Update the state of the Marker button if the selected Marker changes from Presentation Toolbar
   afx_msg void OnUpdateButtonMarker(CCmdUI* pCmdUI);
   // Just enables the Presentation popup button
   afx_msg void OnUpdateViewPresentation(CCmdUI* pCmdUI);

   afx_msg void OnCheckDisplayGrid();
   afx_msg void OnUpdateDisplayGrid(CCmdUI* pCmdUI);
   afx_msg void OnCheckSnapToGrid();
   afx_msg void OnUpdateSnapGrid(CCmdUI* pCmdUI);
   afx_msg void OnEnableLabel(CCmdUI* pCmdUI);

   afx_msg void OnUpdateFullScreen(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFullScreenCS(CCmdUI* pCmdUI);

   void OnUpdateGridSpacing(CCmdUI* pCmdUI);
   void OnGridSpacing(NMHDR* pNMHDR, LRESULT* pResult);
   void OnGridSpacingSpin(NMHDR* pNMHDR, LRESULT* pResult);


	DECLARE_MESSAGE_MAP()

   CString FormatSpacing(int nIndent);

   void SaveToolSettings(int iPointerID);
   void SendToolMessage();
   void DrawGrid(CRect rcClient, CDC *pDC);
   void SnapToGrid(long &x, long &y);
   //Update the icon of the button marker with the coresponding icon of selected marker
   void UpdateMarkerIcon();
   //Update the icon of the pencil icon with the coresponding icon of selected pencil
   void UpdatePencilIcon();

   void UpdateGridChangesOnAllViews();

   CXTPControl *GetControlOnRibbonBar(UINT nGroupId, UINT nControlId);

public:
   void SBSDKOnPen(int iLineWidth, 
                   float fTransperency, float fFillTransperency,
                   COLORREF clrLine, COLORREF clrFill);
   void SBSDKOnNoTool();
   
	void SBSDKOnRectangle(int iLineWidth, 
                         float fTransperency, float fFillTransperency,
                         COLORREF clrLine, COLORREF clrFill);
	void SBSDKOnLine(int iLineWidth,
                    float fTransperency, float fFillTransperency,
                    COLORREF clrLine, COLORREF clrFill);
	void SBSDKOnCircle(int iLineWidth,
                      float fTransperency, float fFillTransperency,
                      COLORREF clrLine, COLORREF clrFill);

    int GetCurrentTool() {return m_iCurrentTool;}

private:
   HRESULT TranslateScreenToObjectCoordinates(CPoint &ptScreen, Gdiplus::PointF &ptObject); 
   HRESULT TranslateScreenToObjectCoordinates(CRect &rcScreen, CRect &rcObject); 
   HRESULT TranslateObjectToScreenCoordinates(CRect &rcObject, CRect &rcScreen);
   HRESULT TranslateObjectToScreenCoordinates(CPoint &ptObject, CPoint &ptScreen);
   CXTPRibbonBar* GetRibbonBar();
   CMainFrameA* GetMainFrame();

   int m_iCurrentTool;
   
   bool m_bButtonPressed;
   bool m_bMoveObjects;
   bool m_bSelectObjects;
   bool m_bOneClickCopySelecteObject;

   bool m_bCreateObjects;
   bool m_bInsertImage;
   bool m_bSelectTextArea;
   bool m_bSnapObject;

   COLORREF	m_clrFill;
	bool		m_bFillColor;
   COLORREF	m_clrLine;
	bool		m_bLineColor;
   COLORREF	m_clrText;
	bool		m_bTextColor;

   bool m_bFontItalic;
   bool m_bFontBold;

   CHARFORMAT2	m_cf;

   double m_dLineWidth;
   int m_iLineStyle;
   int m_iArrowStyle;

   UINT m_nMarkerWidth;
   UINT m_nPencilWidth;
   COLORREF m_clrPen;
   UINT m_nTransperency;
   float m_fGridWidth;

   
   HCURSOR m_hMoveCursor;
	HCURSOR m_hEdit_nwse_Cursor;
	HCURSOR m_hEdit_nesw_Cursor;
	HCURSOR m_hEdit_we_Cursor;
	HCURSOR m_hEdit_ns_Cursor;
   HCURSOR m_hTelepointerCursor;
   HCURSOR m_hSmallCrossCursor;
   HCURSOR m_hNoCursor;
   HCURSOR m_hHandCurosr;	
     
   HCURSOR m_hRectangleCursor;
   HCURSOR m_hOvalCursor;
   HCURSOR m_hPolylineCursor;
   HCURSOR m_hPolygonCursor;
   HCURSOR m_hFreeHandCursor;
   HCURSOR m_hLineCursor;
   HCURSOR m_hTextCursor;
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
   HCURSOR m_hCopyCursor;
   
   HCURSOR m_hAreaSelection;

   ASSISTANT::DrawObject *m_pActiveObject;

   //Holds the list of markers. Default value is the first marker set in the list.
   CDrawingToolSettings *m_Markers;
   //Holds the list of pens. Default value is the first pen set in the list.
   CDrawingToolSettings *m_Pens;

   //View Grid Settings
   UINT m_nGridSnap;     // Snap object to grid
   UINT m_nGridSpacing;  // Grid spacing
   UINT m_nGridDisplay;  // Show/Hide grid on View


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASSISTANTVIEW_H__1A66FB61_FBB0_4CF0_8B2B_F29FED65CDCA__INCLUDED_)

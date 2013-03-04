#if !defined(AFX_WHITEBOARDVIEW_H__F380936C_6B04_4DD2_8BDF_2B3B83435531__INCLUDED_)
#define AFX_WHITEBOARDVIEW_H__F380936C_6B04_4DD2_8BDF_2B3B83435531__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WhiteboardView.h : Header-Datei
//
#include "ExtEdit.h"
#include "Feedback.h" // CFeedback
#include "InteractionArea.h"
#include "GraphicalObjectEditCtrl.h"


enum GraphicalObjectType;

class CWhiteboardView : public CView {
protected:
    CWhiteboardView();           // Dynamische Erstellung verwendet geschützten Konstruktor
    DECLARE_DYNCREATE(CWhiteboardView)

    // Operationen
public:
    void ResetVariables();

    HRESULT Cut();
    HRESULT Copy();
    HRESULT Paste();
    HRESULT Delete();

    static HRESULT ScaleAndOffsetRect(CRect &rcOrig, CPoint *ptOffset, double dZoom);

    bool IsActive();
    bool IsChangingObject();
    void SetInactive();
    void InformDeleteSelected(CInteractionAreaEx *pInteraction);
    void UpdateSelectionArray();
    void DeleteSelectionArray();
    CArray<CInteractionAreaEx *, CInteractionAreaEx *> &GetSelectedObjects(){return m_arSelectedObjects;};
	CArray<CInteractionAreaEx *, CInteractionAreaEx *> &GetTimelineDisplayObjects(){return m_arTimelineDisplayObjects;};
	void AddTimelineDisplayObject(CInteractionAreaEx *pInteraction);
	void RemoveTimelineDisplayObject(CInteractionAreaEx *pInteraction);
    bool IsInSelectionArray(CInteractionAreaEx *pInteraction);
    bool IsFibObject(CInteractionAreaEx *pInteraction);
    HRESULT GetAllQuestionObjects(CArray<CInteractionAreaEx *, CInteractionAreaEx *> &arQuestionObjects);

    bool IsClipboardEmpty();
    void ShowInteractionProperties(bool bIsClickInfo = false);
    void SetActiveInteraction(CInteractionAreaEx *pInteraction);
    void FillCharFormat(CHARFORMAT2 &cf);

    /** 
     * Creates it AND adds it with the right (zoomed) size.
     * Size changes are then also handled.
     */
    CEdit* CreateTextField(CRect& rcOrig);
    CGraphicalObjectEditCtrl* CreateGraphicalObjectTextField(CRect& rcOrig, CRect& rcCalloutDimensions);
    //CGraphicalObjectEditCtrl* CreateGraphicalObjectTextField(CRect& rcOrig, CRect& rcCalloutDimensions);
    HRESULT MoveTextField(CEdit *pEdit, int iDiffX, int iDiffY);
    HRESULT RemoveTextField(CEdit *pEdit);

    HRESULT ShowFeedback(CFeedback *pFeedback, bool bWaitForFinish = true);
    HRESULT WaitForFeedback();
    bool IsFeedbackVisible() { return m_aFeedbackQueue.GetSize() > 0; }
    HRESULT RemoveAllFeedback();
    HRESULT ResetAfterPreview();
    void SelectAllItems();
    void SelectAllItems(UINT uiCurPos);
    void DeselectAllItems();
    void InsertDefaultDemoModeObjects();
    bool DeleteAllDemoDocumentObjects();
    bool InsertDemoObject(int iTimestamp);
    bool InsertDemoGroup(CRect rcCallout);

    void SetInteractionType(int nInteractionType) {m_nInteractionType = nInteractionType;}
    void SetStartRangeSelection() {m_bStartRangeSelection = true;}

    // Überschreibungen
    // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CWhiteboardView)
public:
    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    void ClickPerformed(CPoint pPoint);
    void SetZoomFactor(double dZoom);
    double GetZoomFactor(){return m_dLastZoom;}
    int GetMenuZoomFactor(){return m_iMenuZoomPercent;}
    void ResetGraphicalObjectKey(){m_nGraphicalObjectKey = -1;}
    void ForceNextRedraw() {m_bForceRedraw = true;}
    CRect GetPageDimension() {return m_rcLastPage;}
    void TranslateToObjectCoordinates(CPoint *pptMousePos, float *pfPrecisionObjX = NULL, float *pfPrecisionObjY = NULL);
    void TranslateToObjectCoordinates(CRect &rcView);
    void TranslateFromObjectCoordinates(CRect &rcObject);
protected:
    virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
    //}}AFX_VIRTUAL

    // Implementierung
protected:
    virtual ~CWhiteboardView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generierte Nachrichtenzuordnungsfunktionen
protected:
    //{{AFX_MSG(CWhiteboardView)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnInteractionStackingOrder();
    afx_msg void OnInteractionProperties();
    afx_msg void OnEditCut();
    afx_msg void OnEditCopy();
    afx_msg void OnEditDelete();
    afx_msg void OnFillColor();
    afx_msg void OnUpdateFillColor(CCmdUI* pCmdUI);
    afx_msg void OnMoreFillColors();
    afx_msg void OnSelectFillColor(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateSelectFillColor(CCmdUI* pCmd);
    afx_msg void OnLineColor();
    afx_msg void OnUpdateLineColor(CCmdUI* pCmdUI);
    afx_msg void OnMoreLineColors();
    afx_msg void OnSelectLineColor(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateSelectLineColor(CCmdUI* pCmd);
    afx_msg void OnTextColor();
    afx_msg void OnUpdateTextColor(CCmdUI* pCmdUI);
    afx_msg void OnMoreTextColors();
    afx_msg void OnSelectTextColor(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateSelectTextColor(CCmdUI* pCmd);
    afx_msg void OnLineWidth();
    afx_msg void OnUpdateLineWidth(CCmdUI* pCmdUI);
    afx_msg void OnSelectLineWidth(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateSelectLineWidth(CCmdUI* pCmd);
    afx_msg void OnLineStyle();
    afx_msg void OnUpdateLineStyle(CCmdUI* pCmdUI);
    afx_msg void OnSelectLineStyle(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateSelectLineStyle(CCmdUI* pCmd);
    afx_msg void OnFontFamily(NMHDR* pNMHDR, LRESULT* pResult);//();
    afx_msg void OnUpdateFontFamily(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFontList(CCmdUI* pCmdUI);
    afx_msg void OnFontSize(NMHDR* pNMHDR, LRESULT* pResult);//();
    afx_msg void OnUpdateFontSize(CCmdUI* pCmdUI);
    afx_msg void OnFontWeight();
    afx_msg void OnUpdateFontWeight(CCmdUI* pCmdUI);
    afx_msg void OnFontSizeIncrease(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnFontSizeDecrease(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnFontSlant();
    afx_msg void OnUpdateFontSlant(CCmdUI* pCmdUI);
    afx_msg void OnAlignLeft();
    afx_msg void OnUpdateAlignLeft(CCmdUI* pCmdUI);
    afx_msg void OnAlignCenter();
    afx_msg void OnUpdateAlignCenter(CCmdUI* pCmdUI);
    afx_msg void OnAlignRight();
    afx_msg void OnUpdateAlignRight(CCmdUI* pCmdUI);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnDeleteQuestion();
    afx_msg void OnUpdateDeleteQuestion(CCmdUI* pCmdUI);
    afx_msg void OnShowQuestionaireSettings();
    afx_msg void OnUpdateShowQuestionnaireSettings(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFontSizeIncrease(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFontSizeDecrease(CCmdUI *pCmdUI);
    afx_msg void OnUpdateStackOrder(CCmdUI *pCmdUI);
    afx_msg void OnBringToFront();
    afx_msg void OnSendToBack();
    afx_msg void OnBringForward();
    afx_msg void OnSendBackward();
    afx_msg void OnUpdateGalleryCallouts(CCmdUI* pCmdUI);
    afx_msg void OnXTPGalleryCallouts(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateGalleryDemoObjects(CCmdUI* pCmdUI);
    afx_msg void OnXTPGalleryDemoObjects(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDeleteAllDemoDocumentObjects();
    afx_msg void OnClickInfoSettings();
    afx_msg void OnClickUseInfoTemplate();
    afx_msg LRESULT OnGoEditSelChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGoEditTextChange(WPARAM wParam, LPARAM lParam);
    

    // Only used if there is a screen grabbing in simulation mode
    afx_msg void OnZoom50();
    afx_msg void OnZoom100();
    afx_msg void OnZoom200();
    afx_msg void OnZoomFit();
    afx_msg void OnUpdateVideoZoom(CCmdUI* pCmdUI);
    afx_msg void OnChangeCalloutStyle(UINT nID);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    void CreateWbPageBitmap();
    void SetFontAndColor(CInteractionAreaEx *pInteraction);
    void ChangeFontAndColor(UINT nChangingType);
    bool ChangeGraphicalObjectFontAndColor(UINT nChangingType, CGraphicalObjectEditCtrl* pEdit);
    void ChangeButtonFontAndColor(CInteractionAreaEx *pInteraction, UINT nChangingType);
    void ChangeDimension(CInteractionAreaEx *pInteraction,
        CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects, 
        CRect &rcNewArea);
    void ChangeCaloutStyle(GraphicalObjectType goType);

private:
    enum MouseStateId {
        MOUSE_DOWN,
        MOUSE_MOVE,
        MOUSE_UP
    };
    enum ChangingTypeId {
        FAMILY,
        FONTSIZE,
        FONTSLANT,
        FONTWEIGHT,
        LINE_WIDTH,
        LINE_STYLE,
        LINE_COLOR,
        FILL_COLOR,
        TEXT_COLOR,
        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_RIGHT
    };

    void InsertInteractionArea(CRect &rcRange);
    void InsertInteractionButton(CRect &rcRange);
    /*void TranslateToObjectCoordinates(CPoint *pptMousePos, float *pfPrecisionObjX = NULL, float *pfPrecisionObjY = NULL);
    void TranslateToObjectCoordinates(CRect &rcView);
    void TranslateFromObjectCoordinates(CRect &rcObject);*/
    bool IsEditPoint(CPoint ptMouse, CInteractionAreaEx *pInteraction, int &iEditPoint);
    bool ChangeableObjectsAreaSelected();

    HRESULT PrepareClipboardData();

    HRESULT MoveInteraction(MouseStateId idMouseState, CPoint& ptMouse);
    HRESULT SelectInteraction(MouseStateId idMouseState, CPoint& ptMouse);
    HRESULT EditInteraction(CInteractionAreaEx *pActiveInteraction, MouseStateId idMouseState, CPoint& ptMouse);
    HRESULT SelectInteractionRange(MouseStateId idMouseState, CPoint& ptMouse);
    HRESULT PreviewHandling(MouseStateId idMouseState, UINT nFlags, CPoint& ptMouse);

    void ResizeChildWindows(CWnd *pWndSpecific = NULL);
    bool UpdateZoom();

    void ShowPageView();
    void OrderSelectedElements(CArray<CInteractionAreaEx*, CInteractionAreaEx*>&caSelectedElements);
    void InsertGraphicalObject(CRect &rcRange, GraphicalObjectType iGraphicalObjectType, CString csText = NULL, bool bDemoMode = false);

private:
    CBitmap *m_drawingBitmap;
    CDC *m_bitmapDC;

    bool m_bForceRedraw;

    bool m_bStartRangeSelection;

    // variables for mouse event handling
    bool m_bRangeSelectionStarted;
    bool m_bMoveInteractionStarted;
    bool m_bEditInteractionStarted;
    bool m_bSelectInteractionStarted;
    int m_iEditPoint;

    int  m_nInteractionType;
    int  m_nActivePage;

    HCURSOR m_hCrossCursor;
    HCURSOR m_hMoveCursor;
    HCURSOR m_hLinkCursor;
    HCURSOR m_hAddInteractionCursor;
    HCURSOR m_hEdit_nwse_Cursor;
    HCURSOR m_hEdit_nesw_Cursor;
    HCURSOR m_hEdit_we_Cursor;
    HCURSOR m_hEdit_ns_Cursor;

    CMenu m_popupInteraction;
    // Callouts have custom context menu.
    CMenu m_popupCallout;
    // Only used if there is a screen grabbing in simulation mode
    CMenu m_ZoomPopup;

    double m_dLastZoom;
    int m_iMenuZoomPercent;
    CRect m_rcLastPage;
    bool m_bZoomValid;

    CArray<CInteractionAreaEx *, CInteractionAreaEx *> m_arSelectedObjects;
	CArray<CInteractionAreaEx *, CInteractionAreaEx *> m_arTimelineDisplayObjects;
    CArray<CInteractionAreaEx *, CInteractionAreaEx *> m_clipBoard;

    COLORREF m_clrLine;
    COLORREF m_clrFill;
    COLORREF m_clrText;
    int m_iLineStyle;
    int m_iLineWidth;
    BOOL m_bFontBold;
    BOOL m_bFontItalic;
    int m_iAlignment;

    CMap<CWnd *, CWnd *, CRect, CRect> m_mapOrigSizes;
    long m_lfOrigFontHeight;

    CArray<CFeedback *, CFeedback *> m_aFeedbackQueue;

    CHARFORMAT2 m_CharFormat;

    int m_nGraphicalObjectKey;
    CXTPRibbonTab *m_pTabStart;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_WHITEBOARDVIEW_H__F380936C_6B04_4DD2_8BDF_2B3B83435531__INCLUDED_

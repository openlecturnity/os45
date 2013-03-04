#pragma once

#include "InteractionAreaEx.h"
#include "ClickObject.h"
#include "editorDoc.h"
#include "WhiteboardView.h"
#include "InteractionStream.h"

#define GOT_MIN_WIDTH 120
#define GOT_MIN_HEIGHT 100
#define BUTTON_OUT_SPACE 10
#define BUTTON_IN_SPACE 6
#define BUTTON_MIN_HEIGHT 32

enum GraphicalObjectType {
    GOT_CALLOUT_TOP_LEFT, 
    GOT_CALLOUT_TOP_RIGHT,
    GOT_CALLOUT_BOTTOM_LEFT,
    GOT_CALLOUT_BOTTOM_RIGHT,
    GOT_CALLOUT_LEFT_BOTTOM,
    GOT_CALLOUT_LEFT_TOP,
    GOT_CALLOUT_RIGHT_BOTTOM,
    GOT_CALLOUT_RIGHT_TOP,
    GOT_TEXT,
    GOT_NOTHING
};

class CGraphicalObject : public CInteractionAreaEx {
    DECLARE_DYNCREATE(CGraphicalObject)

private:
    CEditorProject *m_pEditorProject;
    GraphicalObjectType m_idGraphicalObjectType;

    CGraphicalObjectEditCtrl *m_pEdit;
    CWhiteboardView *m_pWbView;

    LOGFONT m_logFont;
    COLORREF m_clrText;
    COLORREF m_clrLine;
    COLORREF m_clrFill;
    int m_iLineWidth;
    int m_iLineStyle;
    //DrawSdk::Polygon *m_pPolygonObject;
    //DrawSdk::Text *m_pTextObject;
    static CArray<CString, CString> m_scaGOType;
    CLcElementInfo* m_pLcElementInfo;
    CClickObject* m_pClickObject;

    void CreateNameAndType(CString csName = NULL);
    void CalculateObjectPositions(bool bRedrawText = true);
    void InitGraphicalObjectTypes();
    GraphicalObjectType GetGraphicalObjectTypeFromString(CString csGOType);
    void GetCalloutDimensions(CRect& rcCallout, CRect rcOrig, bool bFromFile);
    bool m_bIsObjectCallout;
    bool m_bIsUserEdited;
	bool m_bChanged;

    void CreateText(CString csText);
    void CreateText(CArray<SEditLine, SEditLine> &aEditLines);
    bool m_bAutoMove;
    CInteractionAreaEx* m_pDemoButton;
    CString m_csRtf;
    CRect m_rcEdit;
	double m_dLastZoom;
	double m_dOrigZoom;
	CString m_csOrigRtf;
	CString m_csDisplayText;
public:
    bool IsCallout(){return m_bIsObjectCallout;};
    void GetTextObjects(CArray<DrawSdk::Text*, DrawSdk::Text*> &aTexts);
    bool IsUserEdited() {return m_bIsUserEdited;}
    void SetUserEdited (bool bUserEdited) {m_bIsUserEdited = bUserEdited;}
    HRESULT SetGraphicalObjectType(GraphicalObjectType goType);
    GraphicalObjectType GetGraphicalObjectType(){return m_idGraphicalObjectType;}
	void SetChanged(bool bChanged){m_bChanged = bChanged;};
protected:
    CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> m_aDrawObjects;

public:
    CGraphicalObject();
    virtual ~CGraphicalObject();

    virtual void Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing);
    HRESULT Init(GraphicalObjectType iGOType, CEditorProject *pProject, CRect rcDimensions, int iTimestampPos, CString csText = NULL, bool bDemoMode = false, CLcElementInfo *pLcInfo = NULL);
    HRESULT Init(CInteractionArea *pInteraction, CEditorProject *pProject);
    //HRESULT Init(DrawSdk::DrawObject *pObject, CEditorProject *pProject);
    virtual HRESULT AddDuringParsing(CInteractionArea *pInteraction, CEditorProject *pProject, UINT nInsertPositionMs);
    HRESULT AddDemoButton(CInteractionAreaEx* pInteraction, bool bIsNew = true);
    virtual InteractionClassId GetClassId() { return INTERACTION_CLASS_GRAPHICAL_OBJECT; }
    virtual CInteractionAreaEx* MakeNew() { return new CGraphicalObject(); }
    virtual bool IsInteractionNameText() { return false; }
    virtual HRESULT Move(float fDiffX, float fDiffY);
    virtual HRESULT SetArea(CRect *prcArea);
    virtual HRESULT AutoMove(float fDiffX, float fDiffY);
    HRESULT Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
        CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
        LPCTSTR tszExportPrefix, CEditorProject *pProject);
    DrawSdk::ColorObject *GetFilledObject();
    DrawSdk::ColorObject *GetOutlinedObject();
    virtual bool Activate(UINT nPositionMs, bool bPreviewing, bool bAlwaysActive = false);
    CInteractionAreaEx *GetDemoButton(){return m_pDemoButton;};
    virtual HRESULT SetVisibilityPeriod(CTimePeriod *pOther);
    virtual HRESULT IncrementTimes(UINT nMoveMs);
    virtual HRESULT DecrementTimes(UINT nMoveMs);

    CGraphicalObjectEditCtrl *GetEditControl(){return m_pEdit;}
    void ChangeFontAndColor(LOGFONT logFont, _TCHAR *tszFontFamily, int iFontSize, COLORREF clrText);
    CRect GetTextRect();
    void CreateEditControl(bool bDoShow = true);
    void SetGraphicalObjectText(bool bDoUpdate = true);
    void HideVisibleEditControl(UINT nPositionMs);
    void GetLogFont(LOGFONT *pLf) {memcpy(pLf, &m_logFont, sizeof(LOGFONT));}
    COLORREF GetTextColor() {return m_clrText;}
    CLcElementInfo* GetClickObjectLcElementInfo();
    static void CalculatePositionDemoMode(GraphicalObjectType iGraphicalObjectType, CRect& rcCallout, CRect rcDrawArea);
    void UpdateLcCalloutText();
    bool HasClickInfo(){return m_pClickObject != NULL;};
    CString GetLcInfoDisplayText();
    CRect GetClickObjectBounds();
    void UpdateICallout();


protected:
    virtual HRESULT CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey);
    void RemoveDrawObjects();
    void RemoveTextObjects();
    void SetCalloutObjectsParameters(DrawSdk::DPoint *pPoints, int iPointsCount, bool bRedrawText = true);
    void ZoomEdit(int iZoomPercent);
};
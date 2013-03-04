#pragma once

/* REVIEW UK
 * Should be documented: What does it do? How does it do it? E.g. How is "finish" communicated?
 * Should be modal (half modal: Assistant window is usable; rect chooser is on top)
 * There should be a method "StartSelection(pParent)" (replaces/calls Create())
 */

#include "avgrabber.h"

/////////////////////////////////////////////////////////////////////////////
// CaptureRectChooserListener


// CCaptureRectChooser
#include "SgSelectionPreparationBar.h"
#include "SgSelectionRectangle.h"


class AVGRABBER_EXPORT CNewCaptureRectChooser : public CWnd, SGSelectionListener, SgSelectionRectangleListener
{
    DECLARE_DYNAMIC(CNewCaptureRectChooser)

public:
    CNewCaptureRectChooser(AvGrabber *pAvGrabber,
                           CRect &rcLastSelection, bool bRecordFullScreen,
                           SGSettingsListener *pSettingsListener, 
                           SGSelectAreaListener *pSelectAreaListener);
    virtual ~CNewCaptureRectChooser();

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

    virtual void StartSelection();
    virtual CRect &GetSelectionRect();
    virtual void ChangeSelectionRectangle(CRect &rcSelection, bool bUpdateControls);
    virtual void SetShowPresentationBar(bool bShow);
    virtual bool GetShowPresentationBar();
    virtual void SetStickyPreparationBar(bool bSticky);

    virtual void SetRecordFullScreen(bool bRecordFullScreen);
    virtual bool IsDoRecordFullScreen() {return m_bRecordFullScreen;}
    virtual void SetLastCustomRect(CRect &rcLastCustom);
    virtual void GetLastCustomRect(CRect &rcLastCustom);

private:
    void GetAllWindows();
    CWnd *GetActualWindow(CPoint &ptMouse);

    //void DrawLayered(CDC *pDC, CRect &rcWindow, CRect &rcSelection);
    void DrawNormal(CDC *pDC, CRect &rcWindow, CRect &rcSelection);
    void DrawCrossLines(CDC *pDC);
    void DrawCoordinates(CDC *pDC, CRect &rcWindow, CRect &rcSelection);
    void DrawSelectionRect(CDC *pDC, CRect &rcWindow, CRect &rcSelection);
    void ChangeActiveMode(UINT nNewMode);
    void CopyBackgroundToBitmap();
    void RepositionPreparationBar();
    BOOL PreTranslateMessage(MSG* pMsg);
    	
private:
    SGSettingsListener *m_pSettingsListener;
    SGSelectAreaListener *m_pSelectAreaListener;

    AvGrabber *m_pAvGrabber;

    CRect m_rcSelection;

    UINT m_nActiveMode;
    BOOL m_bLeftButtonPressed;
    CRect m_rcCurrentWindow;

    bool m_bRegionSelected;

    HBITMAP  m_hBackgroundImage;
    CDC      *m_pBackgroundDc;

    HCURSOR m_hSgSelectionCursor;

    CSgSelectionPreparationBar *m_pPreparationBar;
    CSgSelectionRectangle *m_pSelectionRectangle;

    bool m_bShowPresentationBar;
    bool m_bRecordFullScreen;
    bool m_bSelectionFinished;
    bool m_bIsPreparationBarSticky;
    CRect m_rcLastCustom;
    bool m_bFromCreate;
    
    CArray<HWND, HWND> m_arWindows;

public:
    enum {
        UNDEFINED_MODE = 0,
        PREPARATION_MODE = 1,
        SELECTION_MODE = 2
    };
};



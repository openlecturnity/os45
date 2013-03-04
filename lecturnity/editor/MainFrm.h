// MainFrm.h : Schnittstelle der Klasse CMainFrameE
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__ED323620_95F1_46EA_B79D_5EC52C275B0C__INCLUDED_)
#define AFX_MAINFRM_H__ED323620_95F1_46EA_B79D_5EC52C275B0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExtSplitter.h"
#include "StreamView.h"
#include "StructureView.h"
#include "VideoView.h"
#include "ControlArrowStyleSelector.h"
#include "InteractionStructureView.h"

#include "..\Studio\XTPMDIChildWnd.h"
class CMainFrame;

class CMainFrameE : public CXTPMDIChildWnd
{
    friend class CEditorDoc;

    // Enumerations
public:
    enum
    {
        XTP_STRUCTURE_VIEW = 61224,
        XTP_VIDEO_VIEW = 61225,
        XTP_STREAM_VIEW = 61226,
        XTP_PAGEOBJECTS_VIEW = 61227
    };

    enum
    {
        COLOR_DARK,
        COLOR_LIGHT,
        COLOR_BG_DARK,
        COLOR_BG_LIGHT,
        COLOR_BORDER,
        COLOR_STREAM_BG,
        COLOR_STREAM_LINE
    };

    enum XpStyleType
    {
        XP_BLUE,
        XP_OLIVE,
        XP_SILVER,
        WINDOWS_CLASSIC
    };

    // Avoid getting the SplashScreen.
    static CDocTemplate *GetAssociatedTemplate();
    static CEditorDoc *GetEditorDocument();
    static CWhiteboardView *GetEditorView();
    static CMainFrameE* GetCurrentInstance();
    static COLORREF GetColor(UINT nColorId);
    CXTPRibbonBar* GetRibbonBar();
    CMainFrame *GetStudioMainFrame();

private:
    static CMainFrameE *s_pMainFrame;
    

private:
    enum
    {
        MIN_DISPLAY_SEC = 1000
    };

public:  
    static CString GetInitialOpenDir();
    static CString GetInitialLepDir();
    static CString GetInitialExportDir();

    static bool DoFileExport(CWnd *pParent, CEditorProject *pProject, CEditorDoc *pDoc, LPCTSTR tszTargetFilename=NULL);
    // When preview is in normal mode, this function always returns true.
    // When preview is in random test mode:
    //    if the user chooses that replay continue in normal mode, returns true 
    //    if the user chooses to continue with preview in random test mode, returns false
    bool PreviewInNormalMode(bool bForceMessage = false);

    // Constructors / Deconstructors
protected: // Nur aus Serialisierung erzeugen
    CMainFrameE();
    DECLARE_DYNCREATE(CMainFrameE)

public:
    virtual ~CMainFrameE();

    // Operations

    // Generated Message-Map-Funktionen
protected:
    //{{AFX_MSG(CMainFrameE)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnFileOpen();
    afx_msg void OnClose();
    afx_msg void OnNewSlide();
    afx_msg void OnImportAudio();
    afx_msg void OnImportVideo();
    afx_msg void OnUndo();
    afx_msg void OnRedo();
    afx_msg void OnCut();
    afx_msg void OnCopy();
    afx_msg void OnPaste();
    afx_msg void OnDelete();
    afx_msg void OnJumpMarkStart();
    afx_msg void OnJumpMarkEnd();
    afx_msg void OnMarkAll();
    afx_msg void OnMarkNone();
    afx_msg void OnCreateSgStructure();
    afx_msg void OnInsertPresentation();
    afx_msg void OnInsertPresentationBeginning();
    afx_msg void OnInsertPresentationEnd();
    afx_msg void OnClipInsert();
    afx_msg void OnClipInsertFromLrd();
    afx_msg void OnInsertStillImage();
    afx_msg void OnInsertStopmark();
    afx_msg void OnUpdateInsertStopmark(CCmdUI* pCmdUI);
    afx_msg void OnInsertTargetmark();
    afx_msg void OnUpdateInsertTargetmark(CCmdUI* pCmdUI);
    afx_msg void OnStartPreview();
    afx_msg void OnPausePreview();
    afx_msg void OnStartStopPreview();
    afx_msg void OnPreviewSelection();
    afx_msg void OnJumpPos1();
    afx_msg void OnJumpEnd();
    afx_msg void OnJumpForward();
    afx_msg void OnJumpBack();
    afx_msg void OnHelp();
    afx_msg void OnUpdateHelp(CCmdUI* pCmdUI);
    afx_msg void OnSetLeftMark();
    afx_msg void OnSetRightMark();
    afx_msg void OnExportAudio();
    afx_msg void OnRemoveVideo();
    afx_msg void OnClipDelete();
    afx_msg void OnRemoveStillImage();
    afx_msg void OnThumbBig();
    afx_msg void OnThumbMiddle();
    afx_msg void OnThumbSmall();
    afx_msg void OnMarkCurrentPage();
    afx_msg void OnMarkCurrentClip();
    afx_msg void OnChangeCurrentClipTitle();
    afx_msg void OnChangeCurrentPageTitle();
    afx_msg void OnDisplayCurrentClipExportSettings();
    afx_msg void OnUpdateStartPreview(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePausePreview(CCmdUI *pCmdUI);
    afx_msg void OnUpdateJumpButtons(CCmdUI *pCmdUI);
    afx_msg void OnUpdateSelectionButtons(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePreviewSelection(CCmdUI *pCmdUI);
    afx_msg void OnUpdateCutCopy(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePaste(CCmdUI *pCmdUI);
    afx_msg void OnUpdateJumpMark(CCmdUI *pCmdUI);
    afx_msg void OnUpdateZoomAll(CCmdUI *pCmdUI);
    afx_msg void OnUpdateZoomSelected(CCmdUI *pCmdUI);
    afx_msg void OnUpdateUndo(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRedo(CCmdUI *pCmdUI);
    afx_msg LRESULT OnHandleAvEditMessage(WPARAM, LPARAM);
    afx_msg void OnOpenWebpage();
    afx_msg void OnFileExport();
    afx_msg void OnFileExportSelection();
    afx_msg void OnUpdateExportSelection(CCmdUI* pCmdUI);
    afx_msg void OnUpdateProjectCommand(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
    afx_msg void OnExportSettings();
    afx_msg void OnUpdateExportSettings(CCmdUI* pCmdUI);
    afx_msg void OnFileImport();
    afx_msg void OnUpdateFileImport(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
    afx_msg void OnUpdateNewSlide(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCreateSgStructure(CCmdUI* pCmdUI);
    afx_msg void OnUpdateInsertPresentation(CCmdUI* pCmdUI);
    afx_msg void OnUpdateClipInsert(CCmdUI* pCmdUI);
    afx_msg void OnUpdateInsertStillImage(CCmdUI* pCmdUI);
    afx_msg void OnUpdateClipDelete(CCmdUI* pCmdUI);
    afx_msg void OnUpdateClipChange(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRemoveStillImage(CCmdUI* pCmdUI);
    afx_msg void OnUpdateImportVideo(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRemoveVideo(CCmdUI* pCmdUI);
    afx_msg void OnUpdateThumbDimension(CCmdUI* pCmdUI);
    afx_msg void OnUpdateMarkPage(CCmdUI* pCmdUI);
    afx_msg void OnExploreLecturnityHome();
    afx_msg void OnUpdateExploreLecturnityHome(CCmdUI* pCmdUI);
    afx_msg void OnShowStructure();
    afx_msg void OnUpdateShowStructure(CCmdUI* pCmdUI);
    afx_msg void OnShowVideo();
    afx_msg void OnUpdateShowVideo(CCmdUI* pCmdUI);
    afx_msg void OnShowStreams();
    afx_msg void OnUpdateShowStreams(CCmdUI* pCmdUI);
    afx_msg void OnShowPageobjects();
    afx_msg void OnUpdateShowPageobjects(CCmdUI* pCmdUI);
    afx_msg void OnAddButton();
    afx_msg void OnSysColorChange();
    afx_msg void OnUpdateStatusPosition(CCmdUI* pCmdUI);
    afx_msg void OnUpdateStatusSelection(CCmdUI* pCmdUI);
    afx_msg void OnUpdateStatusPositionCurrentTime(CCmdUI* pCmdUI);
    afx_msg void OnUpdateStatusPositionTotalLenght(CCmdUI* pCmdUI);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnUpdateRemoveClip(CCmdUI *pCmdUI);
    afx_msg void OnLrdClose();
	afx_msg void OnUpdateLrdClose(CCmdUI* pCmdUI);
    //ribbon messages start
    afx_msg void OnMenuExport();
    afx_msg void OnMenuAudio();
    afx_msg void OnMenuVideo();
    afx_msg void OnUpdateMenuAudio(CCmdUI* pCmdUI);
    afx_msg void OnUpdateMenuVideo(CCmdUI* pCmdUI);
    afx_msg void OnButtonInsertLRD();
    afx_msg void OnUpdateButtonInsertLRD(CCmdUI* pCmdUI);
    afx_msg void OnButtonInsertClip();

    afx_msg void OnZoomSelected();
    afx_msg void OnZoomAll();
    afx_msg void OnZoomSliderScroll(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateZoomSliderScroll(CCmdUI *pCmdUI);

    afx_msg void OnInsertInteraction();
    afx_msg void OnUpdateInsertInteraction(CCmdUI* pCmdUI);
    afx_msg void OnInsertTest();
    afx_msg void OnUpdateInsertTest(CCmdUI* pCmdUI);
    afx_msg void OnButtonCallout();
    afx_msg void OnUpdateButtonCallout(CCmdUI* pCmdUI);
    afx_msg void OnButtonDemoObject();
    afx_msg void OnUpdateButtonDemoObject(CCmdUI* pCmdUI);
    afx_msg void OnCreateDemoDocument();
    afx_msg void OnUpdateCreateDemoDocument(CCmdUI* pCmdUI);
    afx_msg void OnHideMousePointer();
    afx_msg void OnUpdateHideMousePointer(CCmdUI* pCmdUI);
    afx_msg void OnUpdateInsertText(CCmdUI* pCmdUI);
    afx_msg void OnInsertText();
    afx_msg void OnUpdateGalleryCallouts(CCmdUI* pCmdUI);
    afx_msg void OnXTPGalleryCallouts(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateGalleryDemoObjects(CCmdUI* pCmdUI);
    afx_msg void OnXTPGalleryDemoObjects(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnCreateManual();
    afx_msg void OnUpdateCreateManual(CCmdUI* pCmdUI);
    afx_msg void OnGraphicalObjectDefaultSettings();
    afx_msg void OnUpdateGraphicalObjectDefaultSettings(CCmdUI* pCmdUI);

    //ribbon messages end
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    afx_msg void OnCustomize();
    afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
    afx_msg int OnCreateCommandBar(LPCREATEBARSTRUCT lpCreatePopup);
    afx_msg BOOL OnPinRangeClick( UINT nID ,NMHDR * pNotifyStruct, LRESULT* result);
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

private:
    BOOL CreateToolbar();
    BOOL CreateSymbolbar();
    void CreatePanes();
    void RefreshColors();
    void ShowHideView(UINT nPaneId, UINT nId);
    void SetViewsToScreengrabbingLayout();
    void SetViewsToStandardLayout();
    void ShowHideInteractionTools(BOOL bShow);
    void ShowHideEditTools(BOOL bShow);
    void ChangeToPlayHighlight();
    HWND GetVideoHWND(bool bVideoHWND=true);
public:
    void ChangeToPause();
    void ChangeToNormalPlay();
    /**
    * Start timer that will blink Play/Pause buttons
    */
    void StartPausePreviewSpecialTimer();
    /**
    * Stop timer that will blink Play/Pause buttons
    */
    void StopPausePreviewSpecialTimer();
   void HideDocumentStructureView() { ShowHideView(XTP_STRUCTURE_VIEW, IDC_SHOW_STRUCTURE); }
   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    // Überladungen
    // Vom Klassenassistenten generierte Überladungen virtueller Funktionen
    //{{AFX_VIRTUAL(CMainFrameE)
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    //}}AFX_VIRTUAL

    void SwitchMode();

    bool IsPlusMinusKeyZoom();
    void SetPlusMinusKeyZoom(bool bPlusMinusKeyZoom);
public:
    void DisplayExportSettings(int nPage=0, const _TCHAR *tszClipFile=NULL);
    void DoRemoveVideo() { OnRemoveVideo(); }
    XpStyleType GetXpStyleType() {return m_xpStyleType;}
    void ChangeActiveView(CView *pActiveView, CView *pDeactivatedView);

    void SwitchRecordingToSimulation();
    void SwitchSimulationToRecording();

    // update functions
    void UpdateWhiteboardView();
    void UpdateStreamView();
    void UpdateVideoView();
    void UpdateStructureView();
    void UpdatePageObjectsView();

    // There are differences between document based
    // and screengrabbing documents (views, buttons, menus)
    void AdjustToProjectMode();

    void TriggerDocUpdate() { m_bPositionJumped = true; }

    void ShowTimedStatusText(CString &csMessage, UINT nTimeoutMs);

    bool IsPaneVisible(UINT nPaneId);
    void ShowPane(UINT nPaneId);
    void HidePane(UINT nPaneId);
    CWhiteboardView *GetWhiteboardView() {return m_pWhiteboardView;}
    CInteractionStructureView *GetInteractionStructureView() {return m_pInteractionStructureView;}
    CStructureView *GetStructureView() {return m_pStructureView;}
    CStreamView* GetStreamView();
    BOOL CreateStatusBarForStudio(CXTPStatusBar* pStatusBar);
    CXTPStatusBar *GetStatusBar() { return m_pStatusBarStudio;};
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    void RefreshPaneDrawing();
private:
    bool ProjectHasData();
    bool ProjectHasDataAndNotPreviewing();
    bool IsScreengrabbingDocument();

    //Ribbon implementation start
    BOOL CreateRibbonBar();
    void CreateRibbonGalleries();
    //Ribbon implementation end
public:
   void SetColorScheme(UINT nColorScheme);
   void ResetCurrentIteractionId(){m_iCurrentSelectedInteractionID = -1;};

private:

    // this is used only in _EDITOR build
    CXTPStatusBar m_wndStatusBar;

    CXTPStatusBar *m_pStatusBarStudio;
    bool m_bRecentFilelist;    
    CStructureView		*m_pStructureView;
    CVideoView			*m_pVideoView;
    bool                m_bVideoPaneClosedForSgLayout;
    CWhiteboardView	*m_pWhiteboardView;
    CStreamView			*m_pStreamView;
    CView               *m_pActiveView;
    CInteractionStructureView	*m_pInteractionStructureView;

    int     m_nExportSettingsPage;
    CString m_csExportSettingsClip;

    CString m_csSpecialStatusText;
    UINT m_nTimedStatusTimeout;


    bool m_bHasCheckedForManual;
    bool m_bHasManual;

    CXTPDockingPaneManager m_paneManager;
    bool m_bViewsAreInStandardLayout;

    XpStyleType m_xpStyleType;

    UINT m_nTimer;
    CXTPToolBar *m_pSymbolBar;
    bool m_bIsDeactivated;

    bool m_bLastCopyToWb;
    UINT m_nLastCheckedPreviewPos;

    bool m_bLastUpdateTooLong;
    bool m_bPositionJumped;

    CXTPDockingPaneLayout *m_pStandardLayout;
    CXTPDockingPaneLayout *m_pScreengrabbingLayout;
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

    
    // static: Avoid having to use a CMainFrameE instance
    static COLORREF s_clrDark;
    static COLORREF s_clrLight;

    static COLORREF s_clrBackgroundLight;
    static COLORREF s_clrBackgroundDark;

    static COLORREF s_clrBorder;

    static COLORREF s_clrStreamBg;
    static COLORREF s_clrStreamLine;
    bool m_bPlusMinusKeyZoom;
    int m_iCurrentSelectedInteractionID;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MAINFRM_H__ED323620_95F1_46EA_B79D_5EC52C275B0C__INCLUDED_)

// MainFrm.h : interface of the CMainFrameA class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__FF4EC366_938F_4F2B_84F9_247F882E9BDD__INCLUDED_)
#define AFX_MAINFRM_H__FF4EC366_938F_4F2B_84F9_247F882E9BDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _STUDIO
#include "..\Studio\XTPMDIChildWnd.h"
#endif

#include "AssistantView.h"
#include "StartupView.h"
//#include <XTToolkitPro.h> // to see
#include "ControlLineWidthSelector.h"
#include "ControlLineStyleSelector.h"
#include "ControlArrowStyleSelector.h"
#include "UtileDefines.h"
#include "AVCSettings.h"
#include "SGSettings.h"
#include "DocumentStructureView.h"
#include "RecordingsView.h"
#include "HoverButton.h"
#include "NewCaptureRectChooser.h"

#include "SBSDKComWrapper_h.h"
#include "SBSDKEventsHandler.h"

#include "PresentationBar.h"
#ifndef _STUDIO
#include "..\Studio\RibbonBars.h"
#endif

//#include "MonitorWidget.h" 
class CMonitorWidget; // avoid include problems
class CAudioWidget; // avoid include problems

struct FindButtonInfo
{
   int iToolBarID;
   int iButtonID;
   DWORD dwButtonState;
};

#ifdef _STUDIO
class CMainFrameA : public CXTPMDIChildWnd, SBSDKEventIntf, SGSettingsListener, SGSelectAreaListener
#else
class CMainFrameA : public CXTPFrameWnd, SBSDKEventIntf, SGSettingsListener, SGSelectAreaListener
#endif
{
protected: // create from serialization only
	CMainFrameA();
	DECLARE_DYNCREATE(CMainFrameA)
    
	virtual ~CMainFrameA();

// Operations
public:
    // Avoid getting the SplashScreen.
    static CDocTemplate *GetAssociatedTemplate();
    static CAssistantDoc *GetAssistantDocument();
    static CAssistantView *GetAssistantView();
    static CStartupView *GetStartupView();
    static CMainFrameA* GetCurrentInstance();
   static void CreateTabStart(CXTPCommandBars *pCommandBars, CXTPRibbonBar* pRibbon);
   static void CreateTabTools(CXTPCommandBars *pCommandBars, CXTPRibbonBar* pRibbon);
   static void CreateTabView(CXTPCommandBars *pCommandBars, CXTPRibbonBar* pRibbon);
   static void LoadRibbonIcons(CXTPCommandBars *pCommandBars);
   CPresentationBar * GetPresentationBar() {return m_pPresentationBar;}
   virtual CAudioWidget* GetWidget();

private:
    static CMainFrameA *s_pMainFrame;

public:
   CDocumentStructureView *GetStructureTree();
   CRecordingsView *GetRecordingView();
   CAssistantView *GetWhiteboardView();

   void UpdateStructureAndRecordingsView();
   void RefreshPaneDrawing();
   void SelectLecturnityHome();
   void SaveLecturnityHome(CString csLecHome = _T(""));
   void UpdateStatusPages();
   void SetPresentationBarMarker(int nMarker);
   void SetPresentationBarPen(int nPen);
   static CString InitDiskSpace();
   void UpdateStatusDiskSpace();
   void UpdateStatusDuration();
   void ShowMonitorWidget();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrameA)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
    
	BOOL GetRefreshTransparentBtn();
	void SetRefreshTransparentBtn(bool bRefresh = true);
   void SetAudioVideoTooltips();
#ifndef _STUDIO
	void UndoReduceAssistant();
	void ReduceAssistant();
#endif
	void RestoreAssistant();
	void MinimizeAssistant();
#ifndef _STUDIO
   void SetToTop();
   void ResetToTop();
#endif
   void GoToNextPage();
   void GoToPreviousPage();
   void MoveTransparentButton(LPRECT lpViewRect);
	void HideTransparentButton(BOOL bHide = TRUE);
	void RefreshTransparentButton();
   bool IsSmartboardAttached(){return m_bSMARTBoardAttached;};
   bool IsSmartboardUsable(){return m_bSMARTBoardUsable;};
   unsigned int GetCurrentLayout();
   unsigned int GetFullscreenPagefocusLayout();
   unsigned int GetFullscreenLayout();
   //void SetPresentationBarTransparent();
   //void SetPresentationBarAutoHide(CPoint point);
   bool GetStartupPageVisibility();
   // Clear recordings and Document structure views
   void RemoveViewsContent();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
   HRESULT CheckAssistantMainView();
   HRESULT ChangeActiveStructureView(CView *pView);
#ifndef _STUDIO
   void ShowHidePanesForSgRecording(BOOL bShow);
#endif
   void CloseStartPage();	  
   bool IsScreenGrabbing();
   void OpenLastRecording();

   bool IsInStartupMode();
 
   void StartStopScreenRecording();

   // Virtual methods from CaptureRectChooserListener
    virtual void FinishedSgSelection(bool bRecordFullScreen);
    virtual void CancelSgSelection();
    virtual void ShowSgSettings();
    virtual void ShowAudioSettings();
    virtual void GetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop);
    virtual void SetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop);
    virtual HRESULT TestHotKey(AVGRABBER::HotKey &hotKeyStartStop);
    virtual void GetPossibleKeys(CStringArray &arPossibleKeys);
    // From AssistantApp
   int GetVersionType();
   
   bool GetShowImportWarning();
   bool GetShowMissingFontsWarning();
   bool GetShowAudioWarning();
   bool GetShowUseSmartBoard();
   bool GetFreezeImages();
   bool GetFreezeMaster();
   bool GetFreezeDocument();
   bool GetShowMouseCursor();
   bool GetShowStatusBar();
   bool GetShowStartupPage();
   bool GetSwitchAutomaticallyToEdit();
   bool GetShowAVMonitor();
   
   void SetShowImportWarning(bool bShowWarning);
   void SetShowMissingFontsWarning(bool bShowWarning);
   void SetShowUseSmartBoard(bool bShowWarning);
   void SetShowAudioWarning(bool bShowWarning);
   void SetFreezeImages(bool bFreezeImages);
   void SetFreezeMaster(bool bFreezeMaster);
   void SetFreezeDocument(bool bFreezeDocument);
   void SetShowMouseCursor(bool bShowMouseCursor);
   void SetShowStatusBar(bool bShowStatusBar);
   void SetShowStartupPage(bool bShowStartupPage);
   void SetShowAvMonitor(bool bShowAvMonitor);
   // Audio monitor is visible just on screen recording
   void SetShowAudioMonitor(bool bShowAudioMonitor);

   CXTPCommandBars *GetCommandBars();
   CXTPRibbonBar* GetRibbonBar();

   void ShowAssistantSettings(int iPage=0);

protected:  // control bar embedded members
#ifndef _STUDIO
    ULONG_PTR m_gdiplusToken;
    CXTPStatusBar  m_wndStatusBar;
#endif

   bool m_bRecording;
   bool m_bRecordingPaused;
   
   bool m_bScreenGrabing;
   bool m_bRecordingIsSgOnly;

   bool m_bNextPageExist;
   bool m_bPreviousPageExist;
   bool m_bRedrawWindow;
   unsigned int m_uiPresentationTypeSelected;

   
   CDocumentStructureView *m_pDocumentStructureView;
   CRecordingsView *m_pRecordingsView;
   
   


// Generated message map functions
protected:
	
	//{{AFX_MSG(CMainFrameA)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPackProject();
	afx_msg void OnUpdatePackProject(CCmdUI* pCmdUI);
	afx_msg void OnExploreHome();
	afx_msg void OnUpdateExploreHome(CCmdUI* pCmdUI);
	afx_msg void OnSelectHome();
	afx_msg void OnUpdateSelectHome(CCmdUI* pCmdUI);
	afx_msg void OnNewPage();
	afx_msg void OnUpdateNewPage(CCmdUI* pCmdUI);
	afx_msg void OnDuplicatePage();
	afx_msg void OnUpdateDuplicatePage(CCmdUI* pCmdUI);
	afx_msg void OnNewChapter();
	afx_msg void OnUpdateNewChapter(CCmdUI* pCmdUI);
	afx_msg void OnClose();
    afx_msg void OnDestroy();
	afx_msg void OnHelp();
	afx_msg void OnUpdateHelp(CCmdUI* pCmdUI);
	afx_msg void OnFullScreen();
	afx_msg void OnFullScreenCS();
    afx_msg void OnAccVkESC();
	afx_msg void OnShowStructure();
	afx_msg void OnUpdateShowStructure(CCmdUI* pCmdUI);
	afx_msg void OnShowRecordings();
	afx_msg void OnUpdateShowRecordings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolShowStructRec(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePageFocusedLayout(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStandardLayout(CCmdUI* pCmdUI);
	// Updates Record ribbon button check state acordingly to m_bRecording state. This button function changes from "Recording" to "Stop Recording".
	afx_msg void OnUpdateRecordAction(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePreviousPage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNextPage(CCmdUI* pCmdUI);
	afx_msg void OnPreviousPage();
	afx_msg void OnNextPage();
	afx_msg void OnUpdatePauseRecording(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStopRecording(CCmdUI* pCmdUI);
	afx_msg void OnShowImportWarnings();
	afx_msg void OnUpdateShowImportWarnings(CCmdUI* pCmdUI);
	afx_msg void OnShowAudioWarning();
	afx_msg void OnUpdateShowAudioWarning(CCmdUI* pCmdUI);
	afx_msg void OnShowUseSmartBoard();
	afx_msg void OnUpdateShowUseSmartBoard(CCmdUI* pCmdUI);
    afx_msg void OnShowStartpage();
    afx_msg void OnUpdateShowStartpage(CCmdUI* pCmdUI);
	afx_msg void OnShowMissingFonts();
	afx_msg void OnUpdateShowMissingFonts(CCmdUI* pCmdUI);
	// Executes when [Tools->Screengrabbing->Define Area] button is pressed
	afx_msg void OnToolDefineArea();
	afx_msg void OnUpdateToolDefineArea(CCmdUI* pCmdUI);
	afx_msg void OnToolScreengrabbing();
	afx_msg void OnUpdateToolScreengrabbing(CCmdUI* pCmdUI);
	afx_msg void OnSetupPage();
	afx_msg void OnUpdateSetupPage(CCmdUI* pCmdUI);
	afx_msg void OnChangeAudiovideo();
	afx_msg void OnUpdateChangeAudiovideo(CCmdUI* pCmdUI);
	afx_msg void OnChangeSGOptions();
	afx_msg void OnUpdateChangeSGOptions(CCmdUI* pCmdUI);
	afx_msg void OnShowProperties();
	afx_msg void OnLsdClose();
	afx_msg void OnUpdateLsdClose(CCmdUI* pCmdUI);
	afx_msg void OnLsdNew();
	afx_msg void OnUpdateLsdNew(CCmdUI* pCmdUI);
	afx_msg void OnLsdOpen();
	afx_msg void OnUpdateLsdOpen(CCmdUI* pCmdUI);
	afx_msg void OnImportPPT();
	afx_msg void OnImportPPTAs();
    afx_msg void OnUpdateImportPowerPointAs(CCmdUI* pCmdUI);
    afx_msg void OnImportPPTAndRecord();
	afx_msg void OnLsdSave();
	afx_msg void OnUpdateLsdSave(CCmdUI* pCmdUI);
	afx_msg void OnLsdSaveAs();
	afx_msg void OnProjectOpen();
	afx_msg void OnCutObjects();
	afx_msg void OnUpdateCutCopy(CCmdUI* pCmdUI);
	afx_msg void OnDeleteObjects();
	afx_msg void OnUpdateDeleteCopy(CCmdUI* pCmdUI);
	afx_msg void OnCopyObjects();
	afx_msg void OnPasteObjects();
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);	
	afx_msg void OnFreezeImages();
	afx_msg void OnUpdateFreezeImages(CCmdUI* pCmdUI);
	afx_msg void OnFreezeMaster();
	afx_msg void OnUpdateFreezeMaster(CCmdUI* pCmdUI);
	afx_msg void OnFreezeDocument();
	afx_msg void OnUpdateFreezeDocument(CCmdUI* pCmdUI);
	afx_msg void OnButtonStandardLayout();
	afx_msg void OnButtonPageFocusedLayout();
	afx_msg void OnViewGrid();
	afx_msg void OnUpdateViewGrid(CCmdUI* pCmdUI);
    afx_msg void OnCloseStartpage();
    afx_msg void FinishSgSelectionFromPresentationBar();
#ifdef _DVS2005
	afx_msg LRESULT OnStandardLayout(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPageFocusedLayout(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnButtonFullScreenCS(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHotkeys(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeAudio(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeVideo(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnSmartToolChanged(WPARAM wParam, LPARAM lParam);
#else
	afx_msg void OnStandardLayout();
	afx_msg void OnPageFocusedLayout();
	afx_msg void OnButtonFullScreenCS();
	afx_msg void OnHotkeys(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChangeAudio();
	afx_msg void OnChangeVideo();
	//afx_msg void OnSmartToolChanged();
#endif
	afx_msg void OnUseSmartboard();
	afx_msg void OnUpdateUseSmartboard(CCmdUI* pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
#ifndef _STUDIO
   afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
   afx_msg void OnCustomize();
#endif
   // Executes when  "Record" Ribbon button is pressed. Button's function swaps from "Recording" to "Stop Recording".
	afx_msg void OnRecordAction();
   afx_msg void OnStartRecording();
   afx_msg void OnPauseRecording();
   afx_msg void OnStopRecording();
   //ribbon mesages start
   afx_msg void OnMenuNew();
   afx_msg void OnMenuOpen();
   afx_msg void OnMenuSave();
   afx_msg void OnMenuSaveAs();
   afx_msg void OnMenuProjectSettings();
   afx_msg void OnMenuAssistantSettings();
   afx_msg void OnUpdateMenuNew(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMenuOpen(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMenuSave(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMenuSaveAs(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMenuProjectSettings(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMenuAssistantSettings(CCmdUI* pCmdUI);
   afx_msg void OnMonitorOption();
   afx_msg void OnRevert();
   afx_msg void OnUpdateRevert(CCmdUI* pCmdUI);
   afx_msg void OnMenuImport();
#ifndef _STUDIO
   afx_msg void OnUpdateRibbonTab(CCmdUI* pCmdUI);
#endif
   afx_msg void OnPresentationButton();
public:
#ifdef _STUDIO
    afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
#endif
   //ribbon mesages end

   afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
   afx_msg void OnStatusBarSwitchView(UINT nID);
   afx_msg void OnUpdateStatusBarSwitchView(CCmdUI* pCmdUI);
#ifndef _STUDIO
   afx_msg BOOL OnPinRangeClick( UINT nID ,NMHDR * pNotifyStruct, LRESULT* result);
#endif
   afx_msg void OnUpdateMonitorWidget(CCmdUI* pCmdUI);
   afx_msg void OnShowMonitorWidget(NMHDR* pNMHDR, LRESULT* pResult); 
   afx_msg void OnUpdateAudioWidget(CCmdUI* pCmdUI);
   afx_msg void OnShowAudioWidget(NMHDR* pNMHDR, LRESULT* pResult); 
   void StartSGRecording(CRect &rcSelection);

   afx_msg void OnUpdatePresentationBar(CCmdUI* pCmdUI);
   afx_msg void OnPresentationBarContextMenu(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnIdleUpdateCmdUI();
   afx_msg void OnShowPresentationBar(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnUpdatePresentationBarShow(CCmdUI* pCmdUI);
   afx_msg void OnAutoHidePresentationBar(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnUpdatePresentationBarAutoHide(CCmdUI* pCmdUI);
   afx_msg void OnUpdateStatusBarView(CCmdUI* pCmdUI);
   afx_msg void OnCheckBoxShowHideStatusBar();
   afx_msg void OnUpdateShowHideGroup(CCmdUI* pCmdUI);
   afx_msg void OnUpdateCheckboxShowHideMonitorWidget(CCmdUI* pCmdUI);
   afx_msg void OnCheckboxShowHideMonitorWidget();
   afx_msg void OnUpdateCheckboxShowHideWidget(CCmdUI* pCmdUI);
   afx_msg void OnCheckboxShowHideWidget();
   afx_msg void OnUpdateCheckboxShowHideStartupPage(CCmdUI* pCmdUI);
   afx_msg void OnCheckboxShowHideStartupPage();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	DECLARE_MESSAGE_MAP()
   public:
      enum SettingsPageId
      {
         ASSISTANT_SETTINGS_PAGE_AV,
         ASSISTANT_SETTINGS_PAGE_SG,
         ASSISTANT_SETTINGS_PAGE_CUSTOMIZE,
         ASSISTANT_SETTINGS_PAGE_MISC
      };
private:

   enum AssistantLayouts 
   {
      StandardLayout, 
      StandardPagefocusLayout, 
      FullscreenLayout, 
      FullscreenPagefocusLayout, 
      ReducedLayout
   };
   
#ifndef _STUDIO
   void UpdatePresentationIcon(); // method for  update Presentation icon from View tab 
#endif
   void DoChangeTool(short nIndexTool);
   void ResetSgTool();
   // 
   void SwapRecordButtonImage(bool bGrayImage);

   CString GetVideoTooltip(CAssistantDoc *pAssistantDoc);
   CString GetAudioTooltip(CAssistantDoc *pAssistantDoc);

#ifndef _STUDIO
   void ReducedModeOff();
	void ReducedModeOn();
#endif

   void StartRecordingTimer();
   void StopRecordingTimer();
   HRESULT StartNewSgSelection();

   void StoreWindowsState(AssistantLayouts nCurrentLayout);
   void ChangeWindowsState(AssistantLayouts nCurrentLayout);
#ifndef _STUDIO
   void HideMenu();
   void ShowMenu();
   void HideStatusBar();
   void ShowStatusBar();
#endif
   void HidePanes();
   void ShowPanes();
#ifndef _STUDIO
   void SetColorScheme(UINT nColorScheme);
#endif
				   
   void WriteHWNDToRegistry();
   void DeleteHWNDFromRegistry();

   void CloseAllContent();
   void KillAllTimer();
   void UpdatePresentationBarPosition(UINT nPosition);

private:
	int m_nAVSettingsTab;
   CRect	m_mainRect;

#ifndef _STUDIO
   //CRect	m_mainRect;
   LONG     m_lWindowStyle;
#endif
	void FullScreenModeOff();
	void FullScreenModeOn();
	CString m_csImageName;
   CString m_csLecHome;
   
   XTPBarPosition m_RecordingBarPosition;
   CMap<UINT,UINT, CWnd* , CWnd*> m_mapPanes;
   CXTPControlCustom* pButtonViewModeControl;
   CXTPDockingPaneManager m_paneManager;
   CXTPDockingPane* m_DocStructPane;
   CXTPDockingPane* m_RecordingsPane;
   //CXTPRibbonControlRecentFileListExt * m_pRecentFilelist;
   bool m_bRecentFilelist;
   // This view is active is there is at least one open document
   CView *m_pWhiteboardView;
   // This view is active is there no open document
   CView *m_pStartupView;
   // This is the current used view
   CView *m_pCurrentView;

   CHoverButtonEx *m_pButtonStandardLayout;

#ifndef _STUDIO
   UINT m_nCurrentLocalColorScheme;
#endif

   UINT m_nAudioTimer;
   UINT m_nVideoTimer;
   UINT m_nCheckDiskSpace;
   UINT m_nCheckAudioLevel;
   UINT m_nUpdateDiscSpace;
   UINT m_nUpdateRecordTime;
   UINT m_nPauseButtonTimer;
   UINT m_nFlashPause;
   
   int m_iRecordingBitmapID;

   unsigned short m_iSelectedPresentationTool;
   short m_iPreviousLayout; // 0 for Standard Layout, 1 for PageFocused Layout, 3 for FullScrenn Layout
    
   bool m_bMinimized;
   bool m_bAlwaysOnTop;
   bool m_bRefreshTransparentBtn;
   bool m_bDisplayGrid;
   
   short m_nCurrentLayout;
   short m_nPreviousLayout;

   // variables for Standard layout
   bool m_bMenuVisibleStandard;
   bool m_bDocumentStructureVisibleStandard;
   bool m_bRecordingStructureVisibleStandard;
   bool m_bStatusBarVisibleStandard;
   
   // variables for Reduced layout
   bool m_bMenuVisibleReduced;
   bool m_bDocumentStructureVisibleReduced;
   bool m_bRecordingStructureVisibleReduced;
   bool m_bStatusBarVisibleReduced;

   bool m_bHasCheckedForManual;
   bool m_bHasManual;

   // variable to page buttons button state   
   BOOL m_bDocumentStructureVisible;

   // variable to know if the start page should be shown
   bool m_bShowStartpage;

   CMonitorWidget *m_pMonitorWidget;
   CAudioWidget *m_pAudioWidget;
   CPresentationBar *m_pPresentationBar;
   bool m_bMonitorWidget;
   bool m_bPresentationBarAutoHide;
   bool m_bShowPresentationBar;
   bool m_bParentChanged;
   bool m_bIsInFullScreen;

   UINT m_nPositionSG;
   UINT m_nPositionFS;

   // From AssistantApp

   // SMARTBoard-SDK
protected:
	ISBSDKBaseClass2 *m_pSBSDK;
	CSBSDKEventsHandler *m_pEvents;
	DWORD m_dwEventCookie;
   bool m_bSMARTBoardAttached;
   bool m_bSMARTBoardUsable;

   void GetSBSDKColorAndLineWidth(int iPointerID,
                                  COLORREF &clrLine, COLORREF &clrFill,
                                  float &fTransperency, float &fFillTransperency,
                                  int &iLineWidth);
	//Tools
   void OnNoTool(LONG iPointerID);
	void OnPen(LONG iPointerID);
	void OnEraser(LONG iPointerID);
	void OnRectangle(LONG iPointerID);
	void OnLine(LONG iPointerID);
	void OnCircle(LONG iPointerID);

   void AttachSmartBoard();
   void DetachSmartBoard();
   //Ribbon implementation start
   void CreateRibbonGalleries();
   BOOL CreateRibbonBar();
   BOOL CreateStatusBar();

   //Ribbon implementation end

   
   CNewCaptureRectChooser *m_pWndCaptureRect;
   bool m_bIsShowSGPresentationBar;
   bool m_bStartSgFromHotKeys;
   void ShowPresentationBarInSgRec(bool bFixedArea);
   void RestorePresentationBarFromSgRec();
public:
   void AddTooltip(UINT nGroupId, UINT nControlID, UINT nTextID);
   void ChangeModes(CAudioWidget *pWidget, CWnd *pParentWnd, DWORD dwExtraStyle = 0 );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__FF4EC366_938F_4F2B_84F9_247F882E9BDD__INCLUDED_)

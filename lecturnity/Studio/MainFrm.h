// MainFrm.h : Schnittstelle der Klasse CMainFrame
//


#pragma once
class CRibbonBars;
class CMainFrame : public CXTPMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	virtual ~CMainFrame();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

   // Functions from called from Assistant frame
   void FullScreenModeOn();
   void FullScreenModeOff(UINT uiPresentationTypeSelected);
   void Minimize();
   void Restore();
   void SetStatusBarText(int index, LPCTSTR lpszNewText);
   BOOL IsStatusbarVisible() {return m_wndStatusBar.IsVisible();}
   CString InitDiskSpace();
   bool IsWindowPositioningFromRegistry();
   CXTPRibbonBar* GetRibbonBar();
   CRibbonBars * GetRibbonBars() {return m_pRibbonBars;}
   CXTPStatusBar* GetStatusBar() {return &m_wndStatusBar;}
   void OpenWebPage(CString csWebPage);
   
    void UpdateEditorStatusBarAndMenu();
    bool CreateAssistantStatusBar();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

    //  Message map and overridable methods and functions
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnNextWindow();
    afx_msg void OnCustomize();
    afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg BOOL OnPinRangeClick( UINT nID ,NMHDR * pNotifyStruct, LRESULT* result);
	 afx_msg void OnLrdOpen();
	 afx_msg void OnUpdateLrdOpen(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonTab(CCmdUI* pCmdUI);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnCustomizeQuickAccess();
    afx_msg LRESULT OnToolbarContextMenu(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

private:
   BOOL CreateAssistantControl(LPCREATECONTROLSTRUCT lpCreateControl);
   BOOL CreateEditorControl(LPCREATECONTROLSTRUCT lpCreateControl);

   HRESULT ShowAssistantRibbonTabs(CXTPRibbonBar* pRibbon, bool bShow);
   HRESULT ShowEditorRibbonTabs(CXTPRibbonBar* pRibbon, bool bShow);
   HRESULT ShowAssistantApplicationMenu(CXTPRibbonBar* pRibbon, bool bShow);
   HRESULT ShowEditorApplicationMenu(CXTPRibbonBar* pRibbon, bool bShow);
   
   void SetColorScheme(UINT nColorScheme);

   void HideMenu();
   void ShowMenu();
   void HideStatusBar();
   void ShowStatusBar();
   
   void UpdatePresentationIcon(UINT uiPresentationTypeSelected); // method for  update Presentation icon from View tab 
   CString GetInitialDir();
   void ShowCustomizeDialog(int nSelectedPage);

   void LoadPresentationBar();
   
protected:
	CXTPStatusBar m_wndStatusBar;

private:
    CXTPRibbonBar* m_pRibbonBar;
    CRibbonBars* m_pRibbonBars;
    // Variables needed for full screen
    CRect m_rcNormalScreen;
    LONG m_lWindowStyle;

    bool m_bRecentFilelist;
    bool m_bWindowPositioningFromRegistry;
    bool m_bCloseAsMaximized;
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnShowImc();
    afx_msg void OnShowImcPages(UINT nID);
};



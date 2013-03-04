#pragma once

#define NAP_HEIGHT 11
#define GRIPPER_HEIGHT 6
#define AUDIO_HEIGHT 4
#define AUDIO_WIDTH 54
#define AUDIO_OFFSET 3
#define MARGIN 5
#define FONT_SIZE 10
#define INFO_OFFSET 2
#define INFO_HEIGHT 10
#define INFO_WIDTH 45
#define WIDGET_WIDTH MARGIN*2 + AUDIO_WIDTH
//#define WIDGET_HEIGHT NAP_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT

class CAudioWidget : public CWnd {
    DECLARE_DYNAMIC(CAudioWidget)

public:
    CAudioWidget();
    virtual ~CAudioWidget();

protected:
    DECLARE_MESSAGE_MAP()

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnButtonClose();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* pMinMaxInfo);
    afx_msg BOOL CAudioWidget::OnEraseBkgnd(CDC* pDC);

    INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
    BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

    virtual void DrawAudioVideo(Gdiplus::Graphics & graphics);
    virtual void DrawInformations(Gdiplus::Graphics & graphics);
    virtual void RecalculateControlsPosition();
    virtual void SaveDurationSettings();
    virtual void SavePagesSettings();
    virtual void SaveDiskSpaceSettings();
    virtual void SaveLockPositionSettings(BOOL bLockPosition);

    void UpdateMonitorInfo(BOOL bShowInfo1, BOOL bShowInfo2, BOOL bShowInfo3);
private:
    void UpdateControls();
    void CreateToolTips();
protected:
    Gdiplus::Image * LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance);

    Gdiplus::Image *m_pImage;
    Gdiplus::Image* m_pImageCloseBtn;
    Gdiplus::Image* m_pImageGripper;
    Gdiplus::Image* m_pImageNapBtn;

    HBITMAP  m_hBackgroundImage;
    CDC      *m_pBackgroundDc;

    CStatic pStaticAudio;
    CStatic pStaticDuration;
    CStatic pStaticPages;
    CStatic pStaticDiskSpace;
    CXTPButton m_pButtonClose;

    const BITMAPINFO* m_lpBitsInfo;
    void *m_lpBits;
    float m_fAudioPosition;

    BOOL m_bShowDuration;
    // Show pages option is always disabled for audio widget
    BOOL m_bShowPages;
    BOOL m_bShowDiskSpace;
    bool m_bEnableDrag;
    bool m_bMouseOver;
    bool m_bLeftButtonPressed;
    CPoint m_cpoint;

    CString m_csDuration;
    CString m_csDiskSpace;
    CString m_csPages;
    CString m_csDurationToolTip;
    CString m_csDiskSpaceToolTip;
    CString m_csPagesToolTip;
    CString m_csAudioToolTip;

    CPoint m_ptStartDrag;

    int m_nWidth;
    int m_nHeigh;
    int m_nInfoOffset;
    int m_nInfoHeight;
    int m_nInfoWidth;
    int m_nAudioHeight;
    int m_nAudioVideoOffset;
    int m_nMargin;
    int m_nFontSize;
    // True if widget position and size will be saved. False otherwise.
    bool m_bSavePosition;

    CXTPToolTipContext *m_ctrlToolTipsContext;
    CXTPCommandBars* m_pCommandBars;

    int m_nIdMenu;
    bool m_bIsOnCloseBtn;
    COLORREF m_crBorder;

public:
    void ShowDuration(BOOL bShowDuration);
    void ShowDiskSpace(BOOL bShowDiskSpace); 
    virtual void ShowPages(BOOL bShowPages); 
    void LockPosition(BOOL bLockPosition);
    void SetCommandBars(CXTPCommandBars* pCommandBars);
    void UpdateVideoThumbnail(const BITMAPINFO *lpBitsInfo,const void *lpBits);
    void SetAudioPosition(float fPosition);
    virtual void SetAudioVideoTooltips(CString csAudioTooltip, CString csVideoTooltip);
    void UpdateDuration(CString csDuration);
    void UpdateDiskSpace(CString csDiskSpace);
    virtual void UpdatePages(int nPageNo, int nTotalPages);
    virtual void SavePosition();
    virtual bool UpdateWindowPlacement();
    // This should be called after the monitor widget fade efect stops
    inline void StartSavePosition(){m_bSavePosition = true;};
    void SetParentWindow(CWnd *pParent){SetParent(pParent);};
};

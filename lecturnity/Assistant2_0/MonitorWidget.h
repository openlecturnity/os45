#include "AudioWidget.h"

#define VIDEO_HEIGHT 40
//#define WIDGET_HEIGHT NAP_HEIGHT + VIDEO_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT

class CMonitorWidget : public CAudioWidget {
    DECLARE_DYNAMIC(CMonitorWidget)
public:
    CMonitorWidget();
    virtual ~CMonitorWidget();

    virtual void SetAudioVideoTooltips(CString csAudioTooltip, CString csVideoTooltip);
    virtual void SavePosition();
    virtual bool UpdateWindowPlacement();
    virtual void ShowPages(BOOL bShowPages); 
    virtual void UpdatePages(int nPageNo, int nTotalPages);
    virtual void LimitToWindow(bool bLimitToWindow);
    virtual bool IsLimitToWindow();
    void DisableLimitToWindow(bool bDisable = true);

protected:
    DECLARE_MESSAGE_MAP()

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* pMinMaxInfo);
    BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnButtonClose();

    virtual void DrawAudioVideo(Gdiplus::Graphics & graphics);
    virtual void DrawInformations(Gdiplus::Graphics & graphics);
    virtual void RecalculateControlsPosition();
    virtual void SaveDurationSettings();
    virtual void SavePagesSettings();
    virtual void SaveDiskSpaceSettings();
    virtual void SaveLockPositionSettings(BOOL bLockPosition);
    virtual void SaveLimitToWindowSettings();

    CStatic pStaticVideo;
    HBITMAP m_hBitmapVideo;
    CString m_csVideoToolTip;
    int m_nVideoHeight;
    // Limit Widget to parent window region.
    bool m_bLimitToWindow;
    bool m_bDisableLimitToWindow;
};

#pragma once

#include "..\Assistant2_0\PresentationBar.h"

 /**
  * This class is for LIVECONTEXT recording only. 
  */
class ILiveContextListener {
public:
    ILiveContextListener() {};
    ~ILiveContextListener() {};

public:
    virtual void StartStopHotkeyCalled() = 0;
    virtual void OnPresentationBarContextMenu(NMHDR* pNMHDR, LRESULT* pResult) = 0;
    virtual void ChangeAudioValue(double dAudioValue) = 0;
    virtual void XTPGalleryPens(NMHDR* pNMHDR, LRESULT* pResult) = 0;
    virtual void XTPGalleryMarkers(NMHDR* pNMHDR, LRESULT* pResult) = 0;
    virtual void EnableGalleryButton(CCmdUI* pCmdUI) = 0;
    virtual CXTPCommandBars *GetCommandBars() = 0;
    virtual void UpdateAudioContextMenu(CCmdUI* pCmdUI) = 0;
    virtual void ChangeAudioWidget(NMHDR* pNMHDR, LRESULT* pResult) = 0; 
    virtual void UpdateAudioWidget() = 0;
};

 /**
  * This class is for LIVECONTEXT recording only.
  * This window simulates some of the CMainFrame functions.
  */
class CLiveContextHelperWnd : public CWnd
{
	DECLARE_DYNAMIC(CLiveContextHelperWnd)

public:
	CLiveContextHelperWnd(ILiveContextListener *pListener);
	virtual ~CLiveContextHelperWnd();

public:
    void SetPresentationBar(CPresentationBar *pPresentationBar) {m_pPresentationBar = pPresentationBar;}
    // Moved from destructor, as KillTimer uses m_hWnd instance, and this must be deleted before the class is deleted
    // Call it before you delete this instance.
    void KillTimers();
    void StartPauseTimer();
    void StopPauseTimer();
    void StartAudioTimer();
    void StartRecordingTimer();

protected:
    DECLARE_MESSAGE_MAP()

    afx_msg LRESULT OnHotkeys(WPARAM wParam, LPARAM lParam);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPresentationBarContextMenu(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdatePresentationBar(CCmdUI* pCmdUI);
    // Executes when user clicks on a pen from the Pencil's button drop down list.
    afx_msg void OnXTPGalleryPens(NMHDR* pNMHDR, LRESULT* pResult);
    // Executes when user clicks on a marker from the Marker's button drop down list.
    afx_msg void OnXTPGalleryMarkers(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
    // Updates the gallery items check state.
    afx_msg void OnEnableGalleryButton(CCmdUI* pCmdUI);
    afx_msg void OnUpdateAudioWidget(CCmdUI* pCmdUI);
    afx_msg void OnShowAudioWidget(NMHDR* pNMHDR, LRESULT* pResult); 

private:
    enum timerIDs {
        PRESENTATIONBAR_UPDATE = 111,
        PAUSE_BUTTON = 112,
        AUDIO_INDICATOR = 113,
        CHECK_DISCSPACE = 114,
        RECORDTIME_TIMER = 115,
        DISCSPACE_TIMER = 116
    };
    ILiveContextListener *m_pListener;
    CPresentationBar *m_pPresentationBar;
    UINT m_nPauseButtonTimer;
    UINT m_nPresentationBarUpdateTimer;
    UINT m_nFlashPause;
    UINT m_nAudioTimer;
    UINT m_nCheckDiskSpaceTimer;
    UINT m_nUpdateRecordTimer;
    UINT m_nUpdateDiscSpaceTimer;
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};



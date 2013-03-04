#pragma once

#include "LiveContextHelperWnd.h"
#include "..\Assistant2_0\PresentationBar.h"
#include "..\Assistant2_0\DrawingToolSettings.h"
#include "NewCaptureRectChooser.h"
#include "LiveContextStartScreen.h"

 /**
  * This class is for LIVECONTEXT recording only. 
  * Implementation of different listeners during screen recording.
  */
class CLiveContextRecording : public SGSettingsListener, SGSelectAreaListener, 
                                     IPresentationBarListener, ILiveContextListener
{
public:
    CLiveContextRecording(void);
    ~CLiveContextRecording(void);

public:
    bool StartScreenRecording(CLiveContextStartScreen *pLiveContextStartScreen);

public:     // Virtual methods from SGSettingsListener
    virtual void ShowSgSettings();
    virtual void ShowAudioSettings();
    virtual void GetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop);
    virtual void SetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop);
    virtual void GetPossibleKeys(CStringArray &arPossibleKeys);
    virtual HRESULT TestHotKey(AVGRABBER::HotKey &hotKeyStartStop) {return S_OK;};

public:     // Virtual methods from SGSelectAreaListener
    virtual void FinishedSgSelection(bool bRecordFullScreen);
    virtual void CancelSgSelection();
    virtual CPresentationBar *GetPresentationBar() {return m_pPresentationBar;}
    virtual CAudioWidget* GetWidget() {return m_pAudioWidget/*NULL*/;}

public:     // Virtual functions from IPresentationBarListener
    virtual void KeyEscPressed() {};
    virtual void ButtonPensPressed();
    virtual void ButtonMarkersPressed();
    virtual void ButtonTelepointerPressed();
    virtual void ButtonSimpleNavigationPressed() {};
    virtual void ButtonStartPressed();
    virtual void ButtonStopPressed();
    virtual void ButtonPausePressed();
    virtual void ButtonScreenGrabbingPressed() {}
    virtual void ButtonScreenPreviousPagePressed() {};
    virtual void ButtonScreenNextPagePressed() {};
    virtual void ButtonPageFocusedPressed() {};
    virtual void ButtonDocumentStructurePressed() {};
    virtual void ButtonRecordingsPressed() {};
    virtual void ButtonCutPressed() {};
    virtual void ButtonCopyPressed() {};
    virtual void ButtonPastePressed() {};
    virtual void ButtonUndoPressed() {};
    virtual void ButtonClickCopyPressed() {};
    virtual void ButtonTextPressed() {};
    virtual void ButtonLinePressed() {};
    virtual void ButtonPolylinePressed() {};
    virtual void ButtonFreehandPressed() {};
    virtual void ButtonEllipsePressed() {};
    virtual void ButtonRectanglePressed() {};
    virtual void ButtonPolygonPressed() {};
    virtual void ButtonInsertPagePressed() {};

public:     // Virtual functions from ILiveContextHelperListener
    virtual void StartStopHotkeyCalled();
    virtual void OnPresentationBarContextMenu(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void ChangeAudioValue(double dAudioValue);
    virtual void XTPGalleryPens(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void XTPGalleryMarkers(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void EnableGalleryButton(CCmdUI* pCmdUI);
    virtual void UpdateAudioWidget();
    virtual void UpdateAudioContextMenu(CCmdUI* pCmdUI);
    virtual void ChangeAudioWidget(NMHDR* pNMHDR, LRESULT* pResult); 
    virtual CXTPCommandBars *GetCommandBars() {return m_pCommandBars;}

private:
    static UINT UpdatePresentationBarThread(LPVOID pParam);
    void InitializeVariables();
    void ActivateAvGrabber();
    void CreateCommandBars();
    void CreatePresentationBar();
    void CreateAudioWidget();
    void SetSgHotkeys();
    HRESULT StartSgSelection();
    void StartSGRecording(CRect &rcSelection);
    void StopSGRecording();
    void ShowPresentationBarInSgRec(bool bFixedArea);
    CString GetAudioTooltip();
    void InitializeTools();
    void ChangeTool(int iCurrentTool);
    void ShowHideAudioWidget();

private:
    CNewCaptureRectChooser *m_pWndCaptureRect;
    CPresentationBar *m_pPresentationBar;
    CXTPCommandBars *m_pCommandBars;
    CAudioWidget *m_pAudioWidget;
    bool m_bRecordingFinished;
	bool m_bRecordingStarted;
    UINT m_nPositionSG;
    bool m_bIsShowSGPresentationBar;
    CLiveContextHelperWnd *m_pHelperWnd;

    int m_iCurrentTool;
    //Holds the list of markers. Default value is the first marker set in the list.
    CDrawingToolSettings *m_Markers;
    //Holds the list of pens. Default value is the first pen set in the list.
    CDrawingToolSettings *m_Pens;


};

#pragma once


#include "avgrabber.h"
#include "AudioLevelIndicator.h"
#include "TransparentStatic.h"

#include "MySliderControl.h"
#include "../../../lecturnity/Assistant2_0/PresentationBar.h"
#include "../../../lecturnity/Assistant2_0/AudioWidget.h"


/* REVIEW UK
 * No back links from helper DLLs to program main code! (here Assistant)
 *
 * BOOL m_bShowPresentationBar -> always use bool except when really BOOL is necessary
 * m_txtStartStopUseCtrl: Unfortunate naming: This is no text (but a gui component)?
 */

#define WM_CHANGE_SELECTION WM_USER + 3
class SGSelectionListener 
{
public:
    SGSelectionListener() {}
    ~SGSelectionListener() {}

public:
    virtual void StartSelection() = 0;
    virtual CRect &GetSelectionRect() = 0;
    virtual void ChangeSelectionRectangle(CRect &rcSelection, bool bUpdateControls) = 0;
    virtual void SetShowPresentationBar(bool bShow) = 0;
    virtual bool GetShowPresentationBar() = 0;
    virtual void SetStickyPreparationBar(bool bSticky) = 0;

    virtual void SetRecordFullScreen(bool bRecordFullScreen) = 0;
    virtual bool IsDoRecordFullScreen() = 0;
    virtual void SetLastCustomRect(CRect &rcLastCustom) = 0;
    virtual void GetLastCustomRect(CRect &rcLastCustom) = 0;
};

class SGSettingsListener
{
public:
    SGSettingsListener() {}
    ~SGSettingsListener() {}

public:
    virtual void ShowSgSettings() = 0;
    virtual void ShowAudioSettings() = 0;
    
    virtual void GetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop) = 0;
    virtual void SetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop) = 0;
    virtual void GetPossibleKeys(CStringArray &arPossibleKeys) = 0;
    virtual HRESULT TestHotKey(AVGRABBER::HotKey &hotKeyStartStop) = 0;
};

class SGSelectAreaListener
{
public:
    SGSelectAreaListener() {}
    ~SGSelectAreaListener() {}

public:
    virtual void FinishedSgSelection(bool bRecordFullScreen) = 0;
    virtual void CancelSgSelection() = 0;
    virtual CPresentationBar* GetPresentationBar() = 0;
    virtual CAudioWidget* GetWidget() = 0;
};

// CSgSelectionPreparationBar

class AVGRABBER_EXPORT CSgSelectionPreparationBar : public CWnd, public AudioLevelIndicatorListener
{
	DECLARE_DYNAMIC(CSgSelectionPreparationBar)

public:
	CSgSelectionPreparationBar(AvGrabber *pAvGrabber,
                               SGSelectionListener *pSelectionListener,
                               SGSettingsListener *pSettingsListener,
                               SGSelectAreaListener *pSelectAreaListener);
	virtual ~CSgSelectionPreparationBar();

protected:
	DECLARE_MESSAGE_MAP()

public:
    enum {
        WM_BEGINSELECTION = WM_USER + 1,
        WM_STARTRECORDING = WM_USER + 2,
        AUDIO_TIMER = 1,
        RADIOBUTTON_WIDTH = 150,
        BUTTON_WIDTH = 192/*180*/,
        BUTTON_WIDTH_SMALL = 152/*140*/,
        BUTTON_HEIGHT = 25,
        EDIT_WIDTH = 60,
        EDIT_HEIGHT = 20,
        X_OFFSET = 6,
        Y_OFFSET = 6,
        X_OFFSET_SMALL = 3,
        Y_OFFSET_SMALL = 3,
        X_NEW_OFFSET = 10,
        AUDIOLEVEL_WIDTH = 13,
        AUDIOLEVEL_HEIGHT = 40,
        SLIDER_WIDTH = 132,
        COMBO_WIDTH = 60,
        COMBO_HEIGHT = 25,
        CHECKBOX_WIDTH = 35,
        CHECKBOX_HEIGHT = 25,
        TEXT_WIDTH = 35,
        TEXT_WIDTH_SMALL = 12,
        SEL_MODE_WIDTH = 95,
        SEL_MODE_HEIGHT = 49,
        TEXT_HEIGHT = 15
    };

    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL PreTranslateMessage(MSG* pMsg);
    void OnShowAudioSettings();
    void OnShowSgSettings();
    void OnStartSelection();
    void OnStartRecording();
    void OnCancelSelection();

    afx_msg void OnChangeRecordFullScreen();
    afx_msg void OnChangeRecordCustom();
    afx_msg void OnShowHidePresentationBar();

    afx_msg void OnUpdateRecordFullScreen(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRecordCustom(CCmdUI *pCmdUI);
    afx_msg void OnUpdateDimensionEdit(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAudioSlider(CCmdUI *pCmdUI);
    afx_msg void OnUpdateShowHidePresentationBar(CCmdUI *pCmdUI);
    afx_msg void OnUpdateStartSelection(CCmdUI *pCmdUI);

	afx_msg void OnReleasedcaptureAudioSlider(NMHDR* pNMHDR, LRESULT* pResult);

    afx_msg void OnChangeEditX();
    afx_msg void OnChangeEditY();
    afx_msg void OnChangeEditWidth();
    afx_msg void OnChangeEditHeight();
    afx_msg void OnChangeHotKey();
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    //afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    //afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMove(int x, int y);

public:
    void UpdateSelectionRectangle(CRect &rcSelection);
    void UpdateControls(CRect &rcSelection);
    // Virtual method from AudioLevelIndicator
    virtual void ShowAudioSettings();
    virtual int GetAudioLevel();
    Gdiplus::Image * LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL);

private:
    void CreateSgSelectionButtons(UINT &nYPos);
    void CreateAudioButtons(UINT &nYPos);
    void CreateSgSettingButtons(UINT &nYPos);
    void UpdateHotKeyButton();
    void SetButtonImage(CXTPButton* pButton, UINT nID, CSize sz);
    CString LoadMyString(UINT nID);
    CRect m_rcSelection;
    CFont m_Font1;
    CFont m_Font2;
    CFont m_Font3;

private:
    SGSelectionListener *m_pSelectionListener;
    SGSettingsListener *m_pSettingsListener;
    SGSelectAreaListener *m_pSelectAreaListener;
    AvGrabber *m_pAvGrabber; 
    bool m_bFromSelection;
    bool m_bLeftButtonPressed;
    CPoint m_ptStartDrag;

    // Group rectangles
    CRect m_rcSelectionButtons;
    CRect m_rcAudioSettings;
    CRect m_rcOtherButtons;

    // Buttons for area selection
    CXTPButton m_btnRecordFullScreen;
    bool m_bRecordFullScreen;
    CXTPButton m_btnRecordCustom;
    CXTPSpinButtonCtrl m_spnLeft;
    CEdit m_edtLeft;
    CXTPSpinButtonCtrl m_spnTop;
    CEdit m_edtTop;
    CXTPSpinButtonCtrl m_spnWidth;
    CEdit m_edtWidth;
    CXTPSpinButtonCtrl m_spnHeight;
    CEdit m_edtHeight;
    CXTPButton m_btnStartSelection;
    CTransparentStatic m_txtX;
    CTransparentStatic m_txtY;
    CTransparentStatic m_txtWidth;
    CTransparentStatic m_txtHeight;

    // Buttons for audio control
    CAudioLevelIndicator *m_pAudioInputLevelIndicator;
    //CSliderCtrl m_sldAudioInputLevel;
    CMySliderControl m_sldAudioInputLevel;

    CXTPButton m_btnAudioSettings;


    // Hotkey buttons
    CComboBox m_cmbStartStop;
    CXTPButton m_btnStartStopUseCtrl;
    CXTPButton m_btnStartStopUseAlt;
    CXTPButton m_btnStartStopUseShft;
    CTransparentStatic m_txtStartStopUseCtrl;
    CTransparentStatic m_txtStartStopUseAlt;
    CTransparentStatic m_txtStartStopUseShft;
    CTransparentStatic m_txtShortcut;
    
    // Misc Buttons
    CXTPButton m_btnShowHidePresentationBar;
    //CXTPButton m_btnRecordAudio;
    BOOL m_bShowPresentationBar;
    CXTPButton m_btnShowSgSettings;
    CXTPButton m_btnStartRecording;
    CXTPButton m_btnCancelSelection;
    //CXTButton m_btnCancelSelection;
    
    AVGRABBER::HotKey m_hkStartStop;

    CPresentationBar* m_pPresentationBar;
    CRect GetCustomSelRect();
    bool m_bLastCustom;
    CAudioWidget *m_pWidgetRef;

};

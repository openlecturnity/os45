#if !defined(AFX_AUDIOLEVELINDICATOR_H__ABC75E84_FF21_4E68_BB27_93F94776B477__INCLUDED_)
#define AFX_AUDIOLEVELINDICATOR_H__ABC75E84_FF21_4E68_BB27_93F94776B477__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CAudioLevelIndicator.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AudioLevelIndicatorListener

class AudioLevelIndicatorListener
{
public:
    AudioLevelIndicatorListener() {}
    ~AudioLevelIndicatorListener() {}

public:
    virtual void ShowAudioSettings() = 0;
    virtual int GetAudioLevel() = 0;
};


/////////////////////////////////////////////////////////////////////////////
// CAudioLevelIndicator window

class CAudioLevelIndicator : public CStatic
{
    // Construction
public:
    CAudioLevelIndicator(AudioLevelIndicatorListener *pAudioLevelListener);

    // Attributes
public:

    // Operations
public:


    // Implementation
public:
    virtual ~CAudioLevelIndicator();
    void SetToolTipText(CString spText, BOOL bActivate = TRUE);
    void SetToolTipText(UINT nId, BOOL bActivate = TRUE);
    void DeleteToolTip();
    void SetMonitorGroupElement();

public:
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();

protected:
    //{{AFX_MSG(CAudioLevelIndicator)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
    //}}AFX_MSG

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAudioLevelIndicator)

protected:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    DECLARE_MESSAGE_MAP()

private:
    void InitToolTip();
    void ActivateTooltip(BOOL bActivate = TRUE);

private:
    enum {
        AUDIO_TIMER = 1
    };

    AudioLevelIndicatorListener *m_pAudioLevelListener;
    CToolTipCtrl* m_ToolTip;
    CString m_csToolText;
    BOOL m_bTracking;
    BOOL m_bMonitorElement;
    UINT m_nAudioTimer;
    double m_dAudioLevel; // between 0 and 1

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOLEVELINDICATOR_H__ABC75E84_FF21_4E68_BB27_93F94776B477__INCLUDED_)

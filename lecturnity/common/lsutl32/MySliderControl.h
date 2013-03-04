#if !defined(AFX_MYSLIDERCONTROL_H__C76FA857_51CC_4EB6_A8E2_8323BBEF10BD__INCLUDED_)
#define AFX_MYSLIDERCONTROL_H__C76FA857_51CC_4EB6_A8E2_8323BBEF10BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CMySliderControl window

class LSUTL32_EXPORT CMySliderControl : public CSliderCtrl {
    CDC			m_dcBk;
    CBitmap		m_bmpBk;
    CBitmap     *m_bmpBkOld;
    //CBitmap*	m_pbmpOldBk;
    CPen m_penThumb;
    CPen m_penThumbLight;
    CPen m_penThumbLighter;
    CPen m_penThumbDark;
    CPen m_penThumbDarker;
    COLORREF m_crThumb;
    COLORREF m_crThumbLight;
    COLORREF m_crThumbLighter;
    COLORREF m_crThumbDark;
    COLORREF m_crThumbDarker;

    // Construction
public:
    CMySliderControl();

    // Attributes
public:

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMySliderControl)
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CMySliderControl();

    // Generated message map functions
protected:
    //{{AFX_MSG(CMySliderControl)
    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSLIDERCONTROL_H__C76FA857_51CC_4EB6_A8E2_8323BBEF10BD__INCLUDED_)

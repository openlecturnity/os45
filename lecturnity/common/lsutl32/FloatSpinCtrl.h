#if !defined(AFX_FLOATSPINCTRL_H__C2BD95A2_A482_11D2_A6A7_00600844997A__INCLUDED_)
#define AFX_FLOATSPINCTRL_H__C2BD95A2_A482_11D2_A6A7_00600844997A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// CFloatSpinCtrl window
#ifdef _LSUTL32_DLL
    #define LSUTL32_EXPORT __declspec(dllexport)
#else 
    #ifdef _LSUTL32_STATIC
        #define LSUTL32_EXPORT 
    #else
        #define LSUTL32_EXPORT __declspec(dllimport)
    #endif
#endif

class LSUTL32_EXPORT CFloatSpinCtrl : public CSpinButtonCtrl {
    // Construction
public:
    CFloatSpinCtrl();
    virtual ~CFloatSpinCtrl();

    // Operations
public:
    void GetRangeAndDelta(double &lower, double& upper, double& delta);
    void SetRangeAndDelta(double lower, double upper, double delta);

    double GetPos();
    void SetPos(double val);

    int GetDecimalPlaces();
    void SetDecimalPlaces(int num_places);
    void SetFormatString(LPCTSTR lpszFormatString = NULL);

    void SetTrimTrailingZeros(BOOL bTrim) {m_bTrimTrailingZeros = bTrim;}
    BOOL GetTrimTrailingZeros() {return m_bTrimTrailingZeros;}

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFloatSpinCtrl)
protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

    // Implementation
protected:
    void InitSpinCtrl();
    void SetIntPos(double pos);
    void SetValueForBuddy(double val);
    CString FormatValue(double val);

    // Generated message map functions
protected:
    //{{AFX_MSG(CFloatSpinCtrl)
    afx_msg BOOL OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

    /// Attributes
protected:
    double m_MinVal;
    double m_MaxVal;
    double m_Delta;
    UINT m_IntRange;
    int m_NumDecPlaces;
    CString m_strFormat;
    BOOL m_bTrimTrailingZeros;
    char m_DecSymbol;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMSPINCTRL_H__C2BD95A2_A482_11D2_A6A7_00600844997A__INCLUDED_)

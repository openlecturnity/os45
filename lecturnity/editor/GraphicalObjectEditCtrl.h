#define TEXT_BORDER 10
#ifdef _WIN64
typedef DWORD_PTR FOP_DWORD, *FOP_PDWORD;
#else
typedef unsigned long FOP_DWORD, *FOP_PDWORD;
#endif

struct SEditLine
{
public:
    CString csText;
    //CPoint ptStart;
    CRect rcLine;
    int iStartIndex;
    int iEndIndex;
    COLORREF clr;
    LOGFONT lf;
	int iOrigLineNumber;
    int iOrigLineHeight;
};

class CRichEditCharacterFormat {
public:
    bool m_bIsBold;
    bool m_bIsItalic;
    int m_iFontSize;
    CString m_csFontName;
    COLORREF m_clrTextColor;

    CRichEditCharacterFormat() {}
   
    CRichEditCharacterFormat(CRichEditCharacterFormat &cf) {
        m_bIsBold = cf.m_bIsBold;
        m_bIsItalic = cf.m_bIsItalic;
        m_iFontSize = cf.m_iFontSize;
        m_csFontName = cf.m_csFontName;
        m_clrTextColor = cf.m_clrTextColor;
    }

    bool operator !=(const CRichEditCharacterFormat cf) const{
        if (m_bIsBold != cf.m_bIsBold || m_bIsItalic != cf.m_bIsItalic ||
            m_iFontSize != cf.m_iFontSize || m_csFontName != cf.m_csFontName ||
            m_clrTextColor != cf.m_clrTextColor)
            return true;
        return false;
    }
};

class CGraphicalObjectEditCtrl : public CRichEditCtrl {
    DECLARE_DYNAMIC(CGraphicalObjectEditCtrl)

    struct RichEditStreamOutStruct
    {
        CString* m_pString;
        LONG m_lOffset;
    };

    struct RichEditStreamInStruct
    {
        LPCSTR m_psz;
        LONG m_lLength;
        LONG m_lOffset;
    };


public:
    CGraphicalObjectEditCtrl();
    virtual ~CGraphicalObjectEditCtrl();

protected:
    //{{AFX_MSG(CGraphicalObjectEditCtrl)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CRect m_rcCalloutDimension;
    LOGFONT m_logFont;
    double m_dHeight;
    CPoint m_ptOffsets;
    bool m_bOffsetsChanged;
    int m_iDemoModeButtonHeight;
    CRect m_rcEdit;
public:
    void SetCalloutDimension(CRect rcCalloutDimension){m_rcCalloutDimension = rcCalloutDimension;}
    void SetLogFont(LOGFONT logFont){m_logFont = logFont;}
    void CalculatePosition(CString csWindowText);
    void SetOffsets(CPoint ptOffsets);
    CPoint GetOffsets(){return m_ptOffsets;}
    void SetDemoModeButtonHeight(int iButtonHeight) {m_iDemoModeButtonHeight = iButtonHeight;}
    void AFXAPI GetRichText(CRichEditCtrl* pWnd, CString& csOutput);
    void AFXAPI SetRichText(CRichEditCtrl* pWnd, LPCSTR szRTFInput);
    void GetEditLines(CArray<SEditLine, SEditLine>&caEditLines);
    CRect GetEditRect(){return m_rcEdit;}
	void FixPos();

public:
    afx_msg void OnEnRequestresize(NMHDR *pNMHDR, LRESULT *pResult);
protected:
    static DWORD CALLBACK RichEditStreamInCallback(FOP_DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);
    static DWORD CALLBACK RichEditStreamOutCallback(FOP_DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);
private:
    int m_nDevicePixelsX;
    int m_nDevicePixelsY;
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //protected formatting methods
protected:
    int GetLineHeight(int iLine);
    int GetWordYOffset(LOGFONT lf, int iLine, int iMaxDescent = 0);
    int GetWordDescent(LOGFONT lf, int iLine);
    CHARFORMAT GetSelCharFormat(DWORD dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE);
    void SetSelCharStyle(int MASK, int STYLE, int nStart, int nEnd);
    CRichEditCharacterFormat GetSelFormat();
    // Public formatting methods
public:
    void SetDefaultColor(COLORREF clrText);
 
    int  GetFontSize(void);

    
    bool SelectionIsBold();
	bool SelectionIsItalic();

	void SetSelectionBold();
	void SetSelectionItalic();

    long GetSelectionFontSize();
	CString GetSelectionFontName();

    void SetSelectionFontSize(int nPointSize);
	void SetSelectionFontName(CString sFontName);

    void SetSelectionColor(COLORREF color);
    COLORREF GetSelectionColor();
    void SetParagraphLeft();
    void SetParagraphCenter();
    void SetParagraphRight();

    void GetSelectionFormatParameters(bool &bIsBold, bool &bIsItalic, CString &csFontName, long & iFontSize, COLORREF &clrTextColor, int &iAlign);
public:
    afx_msg void OnEnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
};



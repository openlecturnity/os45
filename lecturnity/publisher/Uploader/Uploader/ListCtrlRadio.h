#if !defined(AFX_LISTCTRLRADIO_H__52E6413B_F4BE_4F92_9D4A_D9118C9A690E__INCLUDED_)
#define AFX_LISTCTRLRADIO_H__52E6413B_F4BE_4F92_9D4A_D9118C9A690E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlRadio.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlRadio window

class CListCtrlRadio : public CListCtrl {
    // Construction
public:
    CListCtrlRadio();

    // Attributes
public:

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListCtrlRadio)
    //}}AFX_VIRTUAL

    // Implementation
public:
    BOOL Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    void SetState(int iItem, int iSubItem, int iState);
    virtual ~CListCtrlRadio();

    // Generated message map functions
protected:
    //{{AFX_MSG(CListCtrlRadio)
    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

private:
    LV_ITEM lvi;
    _TCHAR szBuff[MAX_PATH];

    CImageList m_ilRadioButtons;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLRADIO_H__52E6413B_F4BE_4F92_9D4A_D9118C9A690E__INCLUDED_)

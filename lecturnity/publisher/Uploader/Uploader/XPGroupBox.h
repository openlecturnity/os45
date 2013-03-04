#if !defined(AFX_XPGROUPBOX_H__F70D755B_9C4B_4F4A_A1FB_AFF720C29717__INCLUDED_)
#define AFX_XPGROUPBOX_H__F70D755B_9C4B_4F4A_A1FB_AFF720C29717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XPGroupBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXPGroupBox window

class CXPGroupBox : public CButton {
    DECLARE_DYNAMIC(CXPGroupBox);

    // Construction
public:
    CXPGroupBox();

    enum XPGroupBoxStyle { 
        XPGB_FRAME,  
        XPGB_WINDOW
    };

    // Attributes
public:

    // Operations

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CXPGroupBox)
public:
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CXPGroupBox();

    // Generated message map functions
protected:

    //{{AFX_MSG(CXPGroupBox)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPGROUPBOX_H__F70D755B_9C4B_4F4A_A1FB_AFF720C29717__INCLUDED_)

#if !defined(AFX_DLGKEYWORDS_H__F8C6E689_E142_4E6C_A809_77C502426653__INCLUDED_)
#define AFX_DLGKEYWORDS_H__F8C6E689_E142_4E6C_A809_77C502426653__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
    #define LSUTL32_EXPORT __declspec(dllexport)
#else 
    #ifdef _LSUTL32_STATIC
        #define LSUTL32_EXPORT 
    #else
        #define LSUTL32_EXPORT __declspec(dllimport)
    #endif
#endif

// DlgKeywords.h : header file
//
//#include "XPGroupBox.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgKeywords dialog
class CXPGroupBox;

class LSUTL32_EXPORT CDlgKeywords : public CDialog {
    // Construction
public:
    CDlgKeywords(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDlgKeywords();
    CString GetTitle(){return m_csTitle;}
    CString GetKeywords(){return m_csKeywords;}
    void SetTitle(CString &csTitle);
    void SetKeywords(CString &csKeywords);

private:
    CString	m_csTitle;
    CString	m_csKeywords;
    CXPGroupBox	*m_gbCustomGroupBox;

    // Dialog Data
    //{{AFX_DATA(CDlgKeywords)
    enum { IDD = IDD_DIALOG_KEYWORDS };

    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgKeywords)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CDlgKeywords)
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGKEYWORDS_H__F8C6E689_E142_4E6C_A809_77C502426653__INCLUDED_)

#if !defined(AFX_SLIDESTARSETTINGS_H__F8C6E689_E142_4E6C_A809_77C502426653__INCLUDED_)
#define AFX_SLIDESTARSETTING_H__F8C6E689_E142_4E6C_A809_77C502426653__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include "afxwin.h"
struct SlidestarTransferSettings
{
    CString csServer;
    CString csUserName;
    CString csPassword;
};
class CSlidestarSettings : public CDialog
{
    // Construction
public:
    CSlidestarSettings(CWnd* pParent = NULL);   // standard constructor
    CSlidestarSettings(SlidestarTransferSettings sts, CWnd* pParent = NULL);
    virtual ~CSlidestarSettings();

private:
    // Dialog Data
    //{{AFX_DATA(CSlidestarSettings)
    enum { IDD = IDD_SLIDESTAR_SETTINGS };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSlidestarSettings)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

    // Generated message map functions
    //{{AFX_MSG(CSlidestarSettings)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnOK();

private:
    HRESULT ReadDefaultSettings();
    HRESULT WriteDefaultSettings();

private:
    CComboBox m_cmboCategory;

    CEdit m_edtHostName;
    CEdit m_edtUserName;
    CEdit m_edtPassword;
    HBRUSH m_hbrBg;
    bool m_bDefaultProfile;
    SlidestarTransferSettings m_sts;
    void SetDialogValues(void);
public:
    SlidestarTransferSettings GetSlidestarTransferSettings(){return m_sts;};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDESTARSETTING_H__F8C6E689_E142_4E6C_A809_77C502426653__INCLUDED_)

#if !defined(AFX_YOUTUBESETTINGS_H__F8C6E689_E142_4E6C_A809_77C502426653__INCLUDED_)
#define AFX_YOUTUBESETTINGS_H__F8C6E689_E142_4E6C_A809_77C502426653__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include "afxwin.h"

const UINT MAX_YTT_TITLE_LENGTH          = 60;
const UINT MAX_YTT_TITLE_BYTES           = 100;
const UINT MAX_YTT_DESCRIPTION_LENGTH    = 500;
const UINT MAX_YTT_KEYWORD_LENGTH        = 25;
const UINT MIN_YTT_KEYWORD_LENGTH        = 2;
struct YouTubeTransferSettings
    {
        CString csUser;
        CString csPassword;
        CString csTitle;
        CString csDescription;
        CString csKeywords;
        CString csCategory;
        int iPrivacy;
    };

class CYouTubeSettings : public CDialog
{
    // Construction
public:
    CYouTubeSettings(CWnd* pParent = NULL);   // standard constructor
    CYouTubeSettings(YouTubeTransferSettings ytts, CWnd* pParent = NULL);
    virtual ~CYouTubeSettings();

private:
    // Dialog Data
    //{{AFX_DATA(CYouTubeSettings)
    enum { IDD = IDD_YOUTUBE_SETTINGS };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CYouTubeSettings)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

    // Generated message map functions
    //{{AFX_MSG(CYouTubeSettings)
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

    CEdit m_edtUserName;
    CEdit m_edtPassword;
    CEdit m_edtDescription;
    CButton m_btnCheckPrivacy;
    HBRUSH m_hbrBg;
    bool m_bDefaultProfile;
    YouTubeTransferSettings m_ytts;

    CEdit m_edtTitle;
    CStatic m_lblTitle;
    CEdit m_edtKeywords;
    CStatic m_lblKeywords;
    CStatic m_lblDescription;
    CStatic m_lblCategory;
    CStatic m_lblPrivacy;
    CButton m_btnOK;
    CButton m_btnCancel;
    CStatic m_cstSeparator;
    
    void SetFullDialogValues(void);
    void SetDefaultProfileDialog(void);

    bool CheckCharacters(CString csMediaElement);
    bool CheckYouTubeTitle(CString csTitle);
    bool CheckYouTubeDescription(CString csDescription);
    bool CheckYouTubeKeywords(CString csKeywords);
    bool CheckKeyworsLength(CString csKeywords, CString &csKeywordError);
    void LoadErrorStrings();
    CString m_csCharErrorFormat;
    CString m_csTooLongFormat;
    CString m_csEmptyFormat;
    CString m_csTooShortFormat;
    CString m_csCheckError;
    CArray<CString, CString> m_acsCategories;
    void InitCategories();
    int GetCategoryIndex(CString csCategory);
    CString GetLocalizedCategory(CString csCategory);
public:
    bool CheckYouTubeRequiredParameters(CString csTitle, CString csDescription, CString csKeywords);
    YouTubeTransferSettings GetTransferSettings(){return m_ytts;};
    CString GetYouTubeRequiredErrorMessage(){return m_csCheckError;};
    
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YOUTUBESETTINGS_H__F8C6E689_E142_4E6C_A809_77C502426653__INCLUDED_)

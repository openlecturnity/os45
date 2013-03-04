#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ProfileButton.h"
#include "ProfileUtils.h"


// CPublishingFormatDialog-Dialogfeld

class CPublishingFormatDialog : public CDialog, IProfileFinishListener
{
	DECLARE_DYNAMIC(CPublishingFormatDialog)

public:

	CPublishingFormatDialog(CWnd* pParent, CString csLrdName);   // Standardkonstruktor
	virtual ~CPublishingFormatDialog();

// Dialogfelddaten
	enum { IDD = IDD_PUBLISHING_FORMAT };

    int GetCurrentSelection() {return m_iCurrentSelection;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnIdleUpdateUIMessage();
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnFormatYouTube();
    afx_msg void OnUpdateFormatYouTube(CCmdUI* pCmdUI);
    afx_msg void OnEditYouTube();
    afx_msg void OnFormatSlidestar();
    afx_msg void OnUpdateFormatSlidestar(CCmdUI* pCmdUI);
    afx_msg void OnEditSlidestar();
    afx_msg void OnFormatFlash();
    afx_msg void OnUpdateFormatFlash(CCmdUI* pCmdUI);
    afx_msg void OnFormatWindowsMedia();
    afx_msg void OnUpdateFormatWindowsMedia(CCmdUI* pCmdUI);
    afx_msg void OnFormatRealMedia();
    afx_msg void OnUpdateFormatRealMedia(CCmdUI* pCmdUI);
    afx_msg void OnFormatIPod();
    afx_msg void OnUpdateFormatIPod(CCmdUI* pCmdUI);
    afx_msg void OnFormatLPD();
    afx_msg void OnUpdateFormatLPD(CCmdUI* pCmdUI);
    afx_msg void OnFormatCustom();
    afx_msg void OnUpdateFormatCustom(CCmdUI* pCmdUI);
    afx_msg void OnEditCustom();
    afx_msg void OnProfileManager();
    afx_msg void OnCancel();	
    BOOL ContinueModal();

    virtual void PublisherFinished(CString csPublisherMessage);

private:
    int m_iCurrentSelection;
    HANDLE m_hPipe;
    HMODULE m_hResource;
    bool m_bFromContextMenu;

protected:
    virtual void OnOK();

private:
    CString m_csLrdName;

    HBRUSH m_hbrBg;
    CProfileButton m_btnFormatYouTube;
    CProfileButton m_btnFormatSlidestar;
    CXTPButton m_btnFormatFlash;
    CXTPButton m_btnFormatWM;
    CXTPButton m_btnFormatRM;
    CXTPButton m_btnFormatIPOD;
    CXTPButton m_btnFormatLPD;
    CProfileButton m_btnFormatCustom;
    
    CXTPPropertyPageStaticCaption m_captionDefault;
    CXTPPropertyPageStaticCaption m_captionCustom;
    
    CProfileManager *m_pProfileManager;
    int m_iLastError;
};

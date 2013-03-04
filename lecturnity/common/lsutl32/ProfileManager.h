#if !defined(AFX_PROFILEMANAGERDIALOG_H__668A4378_DFBB_4151_A552_1C940C254319__INCLUDED_)
#define AFX_PROFILEMANAGERDIALOG_H__668A4378_DFBB_4151_A552_1C940C254319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include "ProfileInfo.h"
#include "ProfileUtils.h"

#define E_PM_OPEN_KEY      _HRESULT_TYPEDEF_(0x80ea0007L)
#define E_PM_QUERY_VALUE   _HRESULT_TYPEDEF_(0x80ea0008L)

// CProfileManager-Dialogfeld

class CProfileManager : public CDialog, IProfileFinishListener
{
    DECLARE_DYNAMIC(CProfileManager)

public:
    CProfileManager(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation,
        CString &csLrdFilename, CWnd* pParent = NULL);   // Standardkonstruktor
    virtual ~CProfileManager();

    // Dialogfelddaten
    enum { IDD = IDD_PROFILE_MANAGER };

public:  // Functions from CDialog
    virtual BOOL OnInitDialog();
    afx_msg void OnIdleUpdateUIMessage(); // needed for ON_UPDATE_COMMAND_UI
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
    BOOL ContinueModal();
    
    virtual void PublisherFinished(CString csPublisherMessage);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual void OnCancel();
    virtual void OnOK();


    DECLARE_MESSAGE_MAP()

public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnNewProfile();
    afx_msg void OnEditProfile();
    afx_msg void OnRenameProfile();
    afx_msg void OnDeleteProfile();
    afx_msg void OnImportProfile();
    afx_msg void OnExportProfile();
    afx_msg void OnSelectionEnableButtons(CCmdUI *pCmdUI);
    afx_msg void OnActivationEnabledButtons(CCmdUI *pCmdUI);

public:  // Own functions
    HRESULT GetProfileInformation(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation);
    HRESULT GetSelectedProfile(ProfileInfo **ppInfo);
    HRESULT AppendOrEditProfile(__int64 iProfileID);
    HRESULT CancelAppendOrEdit();
    bool WaitForNew() {return m_bWaitForNewProfile;}
    bool WaitForEdit() {return m_bWaitForChangedProfile;}

private:
    HRESULT FillProfileList();
    int GetSelectedItem();
    void EndEditItem(bool bEscape);
    void ActivateProfile(int iItem);
    int AppendNewProfile(ProfileInfo *pProfileInfo);
    HRESULT GetPersonalFolder(CString &csPersonalFolder);
    HRESULT CheckProfileID(ProfileInfo *pNewInfo);
    HRESULT New(CString csLrdName);
    static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
    

private:
    HBRUSH m_hbrBg;
    bool m_bWaitForNewProfile;
    bool m_bWaitForChangedProfile;
    CListCtrl m_profileList;
    CArray<ProfileInfo *, ProfileInfo *>  m_aProfileInformation;
    CString m_csLrdFilename;
    HANDLE m_hPipe;

};

#endif // AFX_PROFILEMANAGERDIALOG_H__668A4378_DFBB_4151_A552_1C940C254319__INCLUDED_


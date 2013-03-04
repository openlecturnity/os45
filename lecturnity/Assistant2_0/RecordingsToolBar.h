#pragma once

#include "ViewToolbarControl.h"
// CRecordingsToolBar

class CRecordingsToolBar : public CXTPToolBar
{
	DECLARE_DYNAMIC(CRecordingsToolBar)

public:
	CRecordingsToolBar();
	virtual ~CRecordingsToolBar();

public:
    static void FillProfileGallery(CXTPControlGallery* pControlGallery, 
                                   CXTPControlGalleryItems *pItemsProfiles);
    void FillProfileGallery();
    void ChangeProfileImage(int iImageID);
    void ShowProfileGallery();
    void HideProfileGallery(bool bCalledFromGallery=false);

protected:
	DECLARE_MESSAGE_MAP()

private:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnUpdateReplayRecord(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditRecord(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePublishRecord(CCmdUI* pCmdUI);

    afx_msg void OnReplayRecord();
    afx_msg void OnEditRecord();
    afx_msg void OnPublishRecord();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

    INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
    BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnTimer(UINT nIDEvent);

   afx_msg void OnEnableGalleryButton(CCmdUI* pCmdUI);
   // Executes when user clicks on a profile from the Profiles's button drop down list.
   afx_msg void OnXTPGalleryProfiles(NMHDR* pNMHDR, LRESULT* pResult);
private:
    void CreateButtons();
    void CreateToolTips();
    CString GetToolTipByID(UINT uID) const ;
    void GetTooltipText(UINT nTextID, CString &csTitle, CString &csDescription) const; 
    void GetDescriptionInMarkup(CString csDescription, CString &csMarkup) const;
    BOOL InitCommandBars(CRuntimeClass* pCommandBarsClass = RUNTIME_CLASS(CXTPCommandBars));
    afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
    void SetIconText();

private:
    enum {
        SHOW_PROFILES_TIMER = 99
    };
    CXTPToolTipContext *m_ctrlToolTipsContext;
    CXTPImageManager *m_pImageManager;
    CXTPCommandBars *m_pCommandBars;
    CViewToolbarControl m_btnPlay;
    CRect m_rcButtonPlay;
    CViewToolbarControl m_btnEdit;
    CRect m_rcButtonEdit;
    CViewToolbarControl m_btnPublish;
    CRect m_rcButtonPublish;
    bool m_bButtonsNeedUpdate;
    UINT m_nCurrentColorScheme;
    bool m_bWaitForTimer;
};



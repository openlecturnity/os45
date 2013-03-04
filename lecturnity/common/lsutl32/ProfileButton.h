#pragma once

#include "controls\xtpbutton.h"
#include "ComboBoxInButton.h"
#include "ButtonInButton.h"
#include "ProfileInfo.h"

class CProfileButton : public CXTPButton
{
	DECLARE_DYNAMIC(CProfileButton)

public:
    CProfileButton(void);
    virtual ~CProfileButton(void);

public:
    DECLARE_MESSAGE_MAP()

protected:
    afx_msg void OnCbnSelchangeComboCustom();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();

public:
    void CreateProfileCombo();
    void UpdateProfileCombo();
    void CreateEditButton(int iButtonID);
    ProfileInfo *GetSelectedProfile();
    void EmptyProfileList();
    void SetProfileList(CArray<ProfileInfo *, ProfileInfo *> &aProfileInfo);
    void GetProfileList(CArray<ProfileInfo *, ProfileInfo *> &aProfileInfo);
    void EnableEditButton(BOOL bEnable=TRUE);
    
protected:
	virtual void OnDraw(CDC* pDC);
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    void CheckUpdateRegion();
    void RemoveRectFromUpdateRegion(CRect rcRemove);
    void UpdateProfileCombo(bool bGetActiveFromRegistry);

private:
    CArray<ProfileInfo *, ProfileInfo *> m_aProfileInformation;
    CComboBoxInButton m_comboCustom;
    CButtonInButton m_btnEdit;
    int m_iEditButtonID;
    CRgn m_rgnUpdate;
    bool m_bLeaveEventSet;
};

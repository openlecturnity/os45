#include "stdafx.h"
#include "Resource.h"
#include "ProfileButton.h"
#include "ProfileInfo.h"
#include "ProfileUtils.h"
#include "lutils.h"

BEGIN_MESSAGE_MAP(CProfileButton, CXTPButton)
    ON_WM_ERASEBKGND()
    ON_CBN_SELCHANGE(IDC_COMBO_CUSTOM, OnCbnSelchangeComboCustom)
    ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CProfileButton, CXTPButton)

CProfileButton::CProfileButton(void) {
    m_bLeaveEventSet = false;
    m_iEditButtonID = 0;
    m_aProfileInformation.RemoveAll();
}

CProfileButton::~CProfileButton(void) {
    if (m_rgnUpdate.m_hObject != NULL)
        m_rgnUpdate.DeleteObject();

    EmptyProfileList();
}

void CProfileButton::CreateProfileCombo() {
    CRect rcButton;
    GetClientRect(&rcButton);

    CRect rcCombo;
    rcCombo.left = rcButton.left + 58;
    rcCombo.top = (rcButton.Height() / 2) -11;
    rcCombo.right = rcButton.right - 65;
    rcCombo.bottom = rcCombo.top + 22;

    // create combo
    m_comboCustom.Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP, 
                         rcCombo, this, IDC_COMBO_CUSTOM); 

    // Set font
    CFont *pFont = GetFont();
    m_comboCustom.SetFont(pFont);

    // Recalculate update region (button-combo)
    RemoveRectFromUpdateRegion(rcCombo);

    // Fill profile list
    ProfileUtils::ReadCustomProfiles(m_aProfileInformation);
    UpdateProfileCombo(true);
}

void CProfileButton::UpdateProfileCombo() {
    // Fill profile list
    EmptyProfileList();
    ProfileUtils::ReadCustomProfiles(m_aProfileInformation);
    UpdateProfileCombo(true);
}

void CProfileButton::CreateEditButton(int iButtonID) {
    CRect rcButton;
    GetClientRect(&rcButton);

    CRect rcEdit;
    rcEdit.left = rcButton.right - 60;
    rcEdit.top = (rcButton.Height() / 2) - 11;
    rcEdit.right = rcButton.right - 5;
    rcEdit.bottom = rcEdit.top + 22;

    m_iEditButtonID = iButtonID;

    // create combo
    CString csEdit;
    csEdit.LoadString(IDS_LABEL_EDIT);
    m_btnEdit.Create(csEdit, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 
                     rcEdit, this, m_iEditButtonID); 
    CFont *pFont = GetFont();
    m_btnEdit.SetFont(pFont);

    SetTextRightOffset(65);

    // Recalculate update region (button-combo)
    RemoveRectFromUpdateRegion(rcEdit);
}

ProfileInfo *CProfileButton::GetSelectedProfile() {
    ProfileInfo *pReturnProfile = NULL;

    if (m_comboCustom.m_hWnd != NULL) {
        int iCurSel = m_comboCustom.GetCurSel();
        if (iCurSel != CB_ERR)
            pReturnProfile = (ProfileInfo *)m_comboCustom.GetItemData(iCurSel);
    }

    return pReturnProfile;
}

void CProfileButton::EmptyProfileList() {
    if (m_comboCustom.m_hWnd)
        m_comboCustom.ResetContent();

    for (int i = 0; i < m_aProfileInformation.GetSize(); ++i) {
        if (m_aProfileInformation[i] != NULL)
            delete m_aProfileInformation[i];
    }
    m_aProfileInformation.RemoveAll();
}

void CProfileButton::SetProfileList(CArray<ProfileInfo *, ProfileInfo *> &aProfileInfo) {
    EmptyProfileList();

    for (int i = 0; i < aProfileInfo.GetSize(); ++i) {
        ProfileInfo *pProfile = new ProfileInfo(aProfileInfo[i]);
        if (pProfile)
            m_aProfileInformation.Add(pProfile);
    }

    UpdateProfileCombo(false);
}

void CProfileButton::GetProfileList(CArray<ProfileInfo *, ProfileInfo *> &aProfileInfo) {
    for (int i = 0; i < m_aProfileInformation.GetSize(); ++i)
        aProfileInfo.Add(m_aProfileInformation[i]);
}
    
void CProfileButton::EnableEditButton(BOOL bEnable) {
    if (m_btnEdit.GetSafeHwnd() != NULL)
        m_btnEdit.EnableWindow(bEnable);
}

BOOL CProfileButton::OnEraseBkgnd(CDC* pDC) {
    return TRUE;
}

void CProfileButton::OnCbnSelchangeComboCustom() {
    int iCurrentSelection = m_comboCustom.GetCurSel();
    if (iCurrentSelection == CB_ERR)
        return;

    ProfileInfo *pInfo = (ProfileInfo *)m_comboCustom.GetItemData(iCurrentSelection);

    // activate selected profile
    for (int i = 0; i < m_aProfileInformation.GetSize(); ++i) {
        //pInfo == NULL is "No profile" 
        if (pInfo != NULL && pInfo == m_aProfileInformation[i])
            m_aProfileInformation[i]->SetActivated(true);
        else
            m_aProfileInformation[i]->SetActivated(false);
    }

    // The selection of a profile should set the button to selected
    if (iCurrentSelection != -1) {
        CWnd *pParent = GetParent();
        if (pParent) {
            pParent->SendMessage(WM_COMMAND, IDC_FORMAT_CUSTOM, NULL);
        }
    }

    // Mouse move set the correct background color
    CRect rcWindow;
    GetWindowRect(&rcWindow);
    CPoint ptCursor;
    GetCursorPos(&ptCursor);
    if (!rcWindow.PtInRect(ptCursor))
        OnMouseMove(0, CPoint(-1, -1));
}

void CProfileButton::OnMouseMove(UINT nFlags, CPoint point) {
    // OnMouseMove has to be called by combo box too
    // because the button should be displayed "hot" even if the mouse
    // is over the combo box.

    CXTPButtonBase::OnMouseMove(nFlags, point);

    CRect rc = GetButtonRect();

    BOOL bHot = rc.PtInRect(point);
    if (bHot != m_bHot) {
        m_bHot = bHot;
        RedrawButton();
    }
    if (!m_bLeaveEventSet) {
        TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT};
        _TrackMouseEvent(&tme);
        m_bLeaveEventSet = true;
    }
}

void CProfileButton::OnMouseLeave() {
    // If the mouse is over te combo box, the window should not be "leaved"

    CRect rcButton;
    GetWindowRect(&rcButton);
    CPoint ptCursor;
    GetCursorPos(&ptCursor);
    if (!rcButton.PtInRect(ptCursor))
        OnMouseMove(0, CPoint(-1, -1));

    m_bLeaveEventSet = false;
}

void CProfileButton::OnDraw(CDC* pDC) {
    CRect rcClip;
    pDC->GetClipBox(&rcClip);

    if (rcClip.IsRectEmpty())
        GetClientRect(&rcClip);

    CheckUpdateRegion();

    CRgn rgnUpdate;
    rgnUpdate.CreateRectRgn(rcClip.left, rcClip.top, 
                            rcClip.right, rcClip.bottom);
    rgnUpdate.CombineRgn(&rgnUpdate, &m_rgnUpdate, RGN_AND);

    pDC->SelectClipRgn(&rgnUpdate);

    CXTPButton::OnDraw(pDC);

    rgnUpdate.DeleteObject();
}

BOOL CProfileButton::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
    CWnd *pParent = GetParent();

	if (pParent &&
       message == WM_COMMAND && wParam == m_iEditButtonID) {
        pParent->SendMessage(WM_COMMAND, m_iEditButtonID);
		return TRUE;
	}
    return CXTPButton::OnWndMsg(message, wParam, lParam, pResult);
}

void CProfileButton::RemoveRectFromUpdateRegion(CRect rcRemove) {
    if (m_rgnUpdate.m_hObject == NULL) {
        CRect rcButton;
        GetClientRect(&rcButton);
        m_rgnUpdate.CreateRectRgn(rcButton.left, rcButton.top, rcButton.right, rcButton.bottom);
    }

    CRgn rgnRemove;
    rgnRemove.CreateRectRgn(rcRemove.left, rcRemove.top, rcRemove.right, rcRemove.bottom);
    m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgnRemove, RGN_DIFF);
    rgnRemove.DeleteObject();
}

void CProfileButton::UpdateProfileCombo(bool bGetActiveFromRegistry) {

    if (!m_comboCustom.m_hWnd)
        return;

    m_comboCustom.ResetContent();

    CString csActiveProfile;
    HRESULT hr = ProfileUtils::ReadActiveProfile(csActiveProfile);

    if (FAILED(hr))
        bGetActiveFromRegistry = false;

    bool bActiveProfile = false;
    for (int i = 0; i < m_aProfileInformation.GetSize(); ++i) {
        if (m_aProfileInformation[i]) {
            int iIndex = m_comboCustom.AddString(m_aProfileInformation[i]->GetTitle());
            m_comboCustom.SetItemDataPtr(iIndex, m_aProfileInformation[i]);
            if (bGetActiveFromRegistry) {
                if (m_aProfileInformation[i]->GetFilename() == csActiveProfile) {
                    m_aProfileInformation[i]->SetActivated(true);
                    m_comboCustom.SetCurSel(iIndex);
                    bActiveProfile = true;
                }
            }
            else if (m_aProfileInformation[i]->IsActivated()) {
                m_comboCustom.SetCurSel(iIndex);
                bActiveProfile = true;
            }
        }
    }
   
    CString csNoProfile;
    csNoProfile.LoadString(IDS_NO_PROFILE_SELECTED);
    m_comboCustom.InsertString(0, csNoProfile);
    m_comboCustom.SetItemDataPtr(0, NULL);
    if (!bActiveProfile)
        m_comboCustom.SetCurSel(0);
}

void CProfileButton::CheckUpdateRegion() {
    if (m_rgnUpdate.m_hObject == NULL) {
        CRect rcButton;
        GetClientRect(&rcButton);
        m_rgnUpdate.SetRectRgn(&rcButton);
    }
}

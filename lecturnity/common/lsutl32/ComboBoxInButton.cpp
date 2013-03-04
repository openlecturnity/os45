#include "stdafx.h"
#include "ComboBoxInButton.h"
#include "ProfileButton.h"

BEGIN_MESSAGE_MAP(CComboBoxInButton, CComboBox)
    ON_WM_MOUSEMOVE()
    ON_WM_ERASEBKGND()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CComboBoxInButton, CComboBox)

CComboBoxInButton::CComboBoxInButton() {
    m_bLeaveEventSet = false;
}

CComboBoxInButton::~CComboBoxInButton() {
}

void CComboBoxInButton::OnMouseMove(UINT nFlags, CPoint point) {
    
    CComboBox::OnMouseMove(nFlags, point);

    CWnd *pParent = GetParent();
    if (pParent && 
        pParent->GetRuntimeClass() == RUNTIME_CLASS(CProfileButton)) {
        CRect rcParent;
        pParent->GetWindowRect(&rcParent);
        CRect rcCombo;
        GetWindowRect(&rcCombo);
        CPoint pointInParent;
        pointInParent.x = (rcCombo.left - rcParent.left) + point.x;
        pointInParent.y = (rcCombo.top - rcParent.top) + point.y;
        ((CProfileButton *)pParent)->OnMouseMove(nFlags, pointInParent);
        if (!m_bLeaveEventSet) {
			TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT};
			_TrackMouseEvent(&tme);
            m_bLeaveEventSet = true;
        }
    } 
}

void CComboBoxInButton::OnMouseLeave() {
    CWnd *pParent = GetParent();
    if (pParent && 
        pParent->GetRuntimeClass() == RUNTIME_CLASS(CProfileButton)) {
        CRect rcParent;
        pParent->GetWindowRect(&rcParent);
        CPoint ptCursor;
        GetCursorPos(&ptCursor);

        if (!rcParent.PtInRect(ptCursor))
            ((CProfileButton *)pParent)->OnMouseMove(0, CPoint(-1, -1));
        else {
            CPoint ptButton;
            ptButton.x = ptCursor.x - rcParent.left;
            ptButton.y = ptCursor.y - rcParent.top;
            ((CProfileButton *)pParent)->OnMouseMove(0, ptButton);
        }
    }

    m_bLeaveEventSet = false;
}

BOOL CComboBoxInButton::OnEraseBkgnd(CDC* pDC) {
    return TRUE;
}

#include "stdafx.h"
#include "Resource.h"
#include "ButtonInButton.h"
#include "ProfileButton.h"
#include "ProfileInfo.h"

BEGIN_MESSAGE_MAP(CButtonInButton, CXTPButton)
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CButtonInButton, CXTPButton)

CButtonInButton::CButtonInButton(void) {
    m_bLeaveEventSet = false;
}

CButtonInButton::~CButtonInButton(void) {
}

BOOL CButtonInButton::OnEraseBkgnd(CDC* pDC) {
    return TRUE;
}

void CButtonInButton::OnMouseMove(UINT nFlags, CPoint point) {
  
    CXTPButton::OnMouseMove(nFlags, point);

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

void CButtonInButton::OnMouseLeave() {
    CWnd *pParent = GetParent();
    if (pParent && 
        pParent->GetRuntimeClass() == RUNTIME_CLASS(CProfileButton)) {

        CRect rcParent;
        pParent->GetWindowRect(&rcParent);
        CPoint ptCursor;
        GetCursorPos(&ptCursor);

        if (!rcParent.PtInRect(ptCursor)) {
            TRACE("CButtonInButton::OnMouseLeave 0: %d %d %d %d : %d %d\n",
                rcParent.left, rcParent.top, rcParent.right, rcParent.bottom, ptCursor.x, ptCursor.y);
            ((CProfileButton *)pParent)->OnMouseMove(0, CPoint(-1, -1));
        } else {
            CPoint ptButton;
            ptButton.x = ptCursor.x - rcParent.left;
            ptButton.y = ptCursor.y - rcParent.top;
            TRACE("CButtonInButton::OnMouseLeave 1: %d %d\n",
                ptButton.x, ptButton.y);
            ((CProfileButton *)pParent)->OnMouseMove(0, ptButton);
        }
    }

    m_bLeaveEventSet = false;
}


#pragma once

#include "controls\xtpbutton.h"

class CButtonInButton : public CXTPButton
{
	DECLARE_DYNAMIC(CButtonInButton)

public:
    CButtonInButton(void);
    virtual ~CButtonInButton(void);

public:
    DECLARE_MESSAGE_MAP()

protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();

private:
    bool m_bLeaveEventSet;
};

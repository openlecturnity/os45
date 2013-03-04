#pragma once


// CComboBoxInButton

class CComboBoxInButton : public CComboBox
{
	DECLARE_DYNAMIC(CComboBoxInButton)

public:
	CComboBoxInButton();
	virtual ~CComboBoxInButton();

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
    bool m_bLeaveEventSet;
};



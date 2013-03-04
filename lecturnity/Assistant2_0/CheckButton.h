#pragma once

#include <XTToolkitPro.h>
// CCheckButton

class CCheckButton : public CXTButton
{
	DECLARE_DYNAMIC(CCheckButton)

public:
    enum 
    {
       WM_STATE_CHANGED = WM_USER + 1
    };

	CCheckButton();
	virtual ~CCheckButton();

    void SetChecked(bool bChecked) {m_bChecked = bChecked;}
    bool GetChecked() {return m_bChecked;}

protected:
	DECLARE_MESSAGE_MAP()
public:
   virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
   bool m_bChecked;
   HBITMAP m_hChecked;
   HBITMAP m_hUnchecked;
   HBITMAP m_hMouseOverChecked;
   HBITMAP m_hMouseOverUnchecked;
};



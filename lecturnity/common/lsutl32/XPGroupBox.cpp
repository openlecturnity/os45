// XPGroupBox.cpp : implementation file
//

#include "stdafx.h"
#include "XPGroupBox.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXPGroupBox,CButton);

//////////////////////////////////////////////////////////////////////
CXPGroupBox::CXPGroupBox() {
    m_clrBorder = ::GetSysColor(COLOR_3DSHADOW);
    m_clrClientBackground = ::GetSysColor(COLOR_BTNFACE);
}

//////////////////////////////////////////////////////////////////////////
CXPGroupBox::~CXPGroupBox() {
}

BEGIN_MESSAGE_MAP(CXPGroupBox, CButton)
    //{{AFX_MSG_MAP(CXPGroupBox)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPGroupBox message handlers

void CXPGroupBox::OnPaint() {
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    CRect	rectClient;
    GetClientRect(rectClient);

    // Draw round rect
    CPen penFrame;
    CBrush brushBKContent(m_clrClientBackground);
    penFrame.CreatePen(PS_SOLID, 1, m_clrBorder);
    CPen* pOldPen = dc.SelectObject(&penFrame);
    dc.RoundRect(rectClient, CPoint(10, 10)); 
}

//////////////////////////////////////////////////////////////////////////
CXPGroupBox& CXPGroupBox::SetBorderColor(COLORREF clrBorder) {
    m_clrBorder = clrBorder;
    return *this;
}

//////////////////////////////////////////////////////////////////////////
CXPGroupBox& CXPGroupBox::SetBackgroundColor(COLORREF clrBKClient) {
    //m_clrTitleBackground = clrBKClient;
    m_clrClientBackground = clrBKClient;
    return *this;
}
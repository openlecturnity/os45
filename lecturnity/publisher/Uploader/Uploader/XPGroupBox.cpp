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

    // Draw white rectangle

    CPen penFrame;
    CBrush brushBKContent(RGB(255,255,255));

    penFrame.CreatePen(PS_SOLID, 1, RGB(0,0,0));
    CPen* pOldPen = dc.SelectObject(&penFrame);

    dc.Rectangle(rectClient);
}
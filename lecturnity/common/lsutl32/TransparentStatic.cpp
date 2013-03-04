// TransparentStatic.cpp : implementation file
//

#include "stdafx.h"
#include "TransparentStatic.h"


// CTransparentStatic

IMPLEMENT_DYNAMIC(CTransparentStatic, CStatic)
CTransparentStatic::CTransparentStatic() {
    m_clrForeground = RGB(0,0,0);
}

CTransparentStatic::~CTransparentStatic() {
}


BEGIN_MESSAGE_MAP(CTransparentStatic, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()


void CTransparentStatic::SetTextColor(COLORREF clrForeground) {
    m_clrForeground = clrForeground;
    // Note: no repaint done!
}


// CTransparentStatic message handlers

void CTransparentStatic::OnPaint() {
    CPaintDC dc(this); // device context for painting

    // Where to draw text 
    CRect client_rect;
    GetClientRect(client_rect);

    // Get the caption
    CString szText;
    GetWindowText(szText);

    // Get the font
    CFont *pFont, *pOldFont;
    pFont = GetFont();
    pOldFont = dc.SelectObject(pFont);

    // Map "Static Styles" to "Text Styles"
#define MAP_STYLE(src, dest) if(dwStyle & (src)) dwText |= (dest)
#define NMAP_STYLE(src, dest) if(!(dwStyle & (src))) dwText |= (dest)

    DWORD dwStyle = GetStyle(), dwText = 0;

    MAP_STYLE(SS_RIGHT, DT_RIGHT);
    MAP_STYLE(SS_CENTER, DT_CENTER);
    MAP_STYLE(SS_CENTERIMAGE, DT_VCENTER | DT_SINGLELINE);
    MAP_STYLE(SS_NOPREFIX, DT_NOPREFIX);
    MAP_STYLE(SS_WORDELLIPSIS, DT_WORD_ELLIPSIS);
    MAP_STYLE(SS_ENDELLIPSIS, DT_END_ELLIPSIS);
    MAP_STYLE(SS_PATHELLIPSIS, DT_PATH_ELLIPSIS);

    NMAP_STYLE(	SS_LEFTNOWORDWRAP |
        SS_CENTERIMAGE |
        SS_WORDELLIPSIS |
        SS_ENDELLIPSIS |
        SS_PATHELLIPSIS, DT_WORDBREAK);

    // Set transparent background
    dc.SetBkMode(TRANSPARENT);

    COLORREF clrOld = dc.SetTextColor(m_clrForeground);

    // Draw the text
    dc.DrawText(szText, client_rect, dwText);

    dc.SetTextColor(clrOld);

    // Select old font
    dc.SelectObject(pOldFont);
}

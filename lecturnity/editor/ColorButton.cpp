// ColorButton.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgColorButton

CDlgColorButton::CDlgColorButton()
{
}

CDlgColorButton::~CDlgColorButton()
{
}


BEGIN_MESSAGE_MAP(CDlgColorButton, CXTColorPicker)
	//{{AFX_MSG_MAP(CDlgColorButton)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgColorButton 

void CDlgColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rcClient;
	GetClientRect(rcClient);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rcButton(rcClient);
    
	CBrush bgBrush(GetSysColor(COLOR_BTNFACE));
	pDC->FillRect(&rcButton, &bgBrush);

	rcButton.right = rcButton.left + 18;
	pDC->DrawFrameControl(rcButton, DFCS_BUTTONPUSH, DFCS_ADJUSTRECT);
    
	CBrush brush(GetColor());
	pDC->FillRect(&rcButton, &brush);

	CRect rcArrow(rcButton.right - 5, rcButton.bottom - 3, rcButton.right+1, rcButton.bottom+1);
	pDC->FillRect(&rcArrow, &bgBrush);
	
	CBrush blackBrush(RGB(0, 0, 0));
	CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPoint triangle[3];
	triangle[0].x = rcArrow.left + 1;
	triangle[0].y = rcArrow.top + 1;
	triangle[1].x = rcArrow.right - 1;
	triangle[1].y = rcArrow.top + 1;
	triangle[2].x = rcArrow.left + 3;
	triangle[2].y = rcArrow.bottom - 1;
	CBrush *pOldBrush = pDC->SelectObject(&blackBrush);
	CPen *pOldPen = pDC->SelectObject(&blackPen);

	pDC->Polygon(triangle, 3);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);

	CRect rcText(rcClient);
	rcText.left += 20;

    CFont *pFont = XTPPaintManager()->GetRegularFont();
    CFont *pOldFont = NULL;
    if (pFont)
        pOldFont = pDC->SelectObject(pFont);
    int iOldMode = pDC->SetBkMode(OPAQUE);
    COLORREF clrOld = pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));

	CString csText;
	GetWindowText(csText);
	pDC->DrawText(csText, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    if (pOldFont)
        pDC->SelectObject(pOldFont);
    pDC->SetBkMode(iOldMode);
    pDC->SetBkColor(clrOld);
}
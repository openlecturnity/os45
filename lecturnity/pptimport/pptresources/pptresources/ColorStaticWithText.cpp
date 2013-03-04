// ColorStaticWithText.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "pptresources.h"
#include "ColorStaticWithText.h"


// CColorStatic

IMPLEMENT_DYNAMIC(CColorStaticWithText, CStatic)

CColorStaticWithText::CColorStaticWithText()
{
  
}

CColorStaticWithText::~CColorStaticWithText()
{
}


BEGIN_MESSAGE_MAP(CColorStaticWithText, CStatic)
   ON_WM_PAINT()
END_MESSAGE_MAP()



// CColorStatic-Meldungshandler


void CColorStaticWithText::OnPaint()
{
   CPaintDC dc(this); // device context for painting

   CRect rcWnd;
   GetClientRect(&rcWnd);

   CString csText;
   GetWindowText(csText);
  
   dc.SetTextColor(RGB(0, 0, 0));
   dc.SetBkColor(RGB(255, 255, 255));
   dc.SetBkMode(TRANSPARENT);
   CFont *pFont = GetFont();

   /*/
   LOGFONT logFont;
   pFont->GetLogFont(&logFont);
   logFont.lfWeight = 700;
   _tcscpy(logFont.lfFaceName, _T("Arial"));

   CFont newFont;
   newFont.CreateFontIndirect(&logFont);

   /*/

   dc.SelectObject(pFont);
   dc.DrawText(csText, csText.GetLength(), &rcWnd, 0);

}

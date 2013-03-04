// TitleStatic.cpp: Implementierung der Klasse CTitleStatic.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TitleStatic.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CTitleStatic::CTitleStatic() : CStatic()
{

}

CTitleStatic::~CTitleStatic()
{

}

BEGIN_MESSAGE_MAP(CTitleStatic, CStatic)
   ON_WM_PAINT()
END_MESSAGE_MAP()

void CTitleStatic::OnPaint()
{
   PAINTSTRUCT paintStruct;

   DWORD dwRes = GetUpdateRect(NULL);
   if (dwRes == 0)
      return;
   CDC *pDC = BeginPaint(&paintStruct);

   CRect rect;
   GetClientRect(&rect);
   /*
   CBrush brush(::GetSysColor(COLOR_ACTIVECAPTION));
   pDC->FillRect(&rect, &brush);
   CString csText;
   GetWindowText(csText);
   int nOldMode = pDC->SetBkMode(TRANSPARENT);
   pDC->DrawText(csText, &rect, DT_SINGLELINE | DT_VCENTER);
   */
   
   //::DrawCaption(GetSafeHwnd(), pDC->GetSafeHdc(), &rect, DC_ACTIVE | DC_SMALLCAP | DC_TEXT | DC_GRADIENT);
   //::DrawCaption(GetSafeHwnd(), pDC->GetSafeHdc(), &rect, DC_ACTIVE | DC_SMALLCAP | DC_TEXT);
   ::DrawCaption(GetSafeHwnd(), pDC->GetSafeHdc(), &rect, DC_SMALLCAP | DC_TEXT);

   EndPaint(&paintStruct);
}

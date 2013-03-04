#include "StdAfx.h"

#include "objects.h"

DrawSdk::Pointer::Pointer() : DrawObject()
{
}

DrawSdk::Pointer::Pointer(double x, double y, bool bMousePointer) : DrawObject(x, y, 30, 30)
{
   m_bMousePointer = bMousePointer;
}

void DrawSdk::Pointer::Init()
{
}


DrawSdk::Pointer::~Pointer()
{
}

void DrawSdk::Pointer::Draw(HDC hdc, double offX, double offY, ZoomManager *pZm) {
    double xCorr = (m_bMousePointer) ? (x_) : (x_ - width_);

    int zX = (int)xCorr;
    int zY = (int)y_;
    int zWidth  = (int)width_;
    int zHeight = (int)height_;

    if (pZm != NULL) {
        zX      = pZm->GetScreenX(xCorr);
        zY      = pZm->GetScreenY(y_);
        zWidth  = pZm->GetScreenX(width_);
        zHeight = pZm->GetScreenY(height_);
    }

    zX += offX;
    zY += offY;

    if (m_bMousePointer)
        DrawMousePointer(hdc, zX, zY, zWidth, zHeight);
    else
        DrawTelePointer(hdc, zX, zY, zWidth, zHeight);
}

void DrawSdk::Pointer::DrawTelePointer(HDC hdc, int zX, int zY, int zWidth, int zHeight) {
   POINT ps[4]; 
   HPEN hPen   = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
   HPEN oldPen = (HPEN) ::SelectObject(hdc, hPen);

   ps[0].x = zX + zWidth-1;           ps[0].y = zY + 0;
   ps[1].x = zX + 0;                  ps[1].y = zY + (int)(0.5*zHeight);
   ps[2].x = zX + (int)(0.35*zWidth); ps[2].y = zY + (int)(0.65*zHeight);
   ps[3].x = zX + (int)(0.5*zWidth);  ps[3].y = zY + zHeight-1;

   ::Polyline(hdc, ps, 4);

   if (oldPen)
      ::SelectObject(hdc, oldPen);
   ::DeleteObject(hPen);


   HBRUSH hBrush1   = ::CreateSolidBrush(RGB(255, 0, 0));
   HBRUSH hBrush2   = ::CreateSolidBrush(RGB(128, 0, 0));


   HBRUSH oldBrush = (HBRUSH)::SelectObject(hdc, hBrush1);
   
	ps[0].x = zX + zWidth-1;           ps[0].y = zY + 0;
	ps[1].x = zX + 0;                  ps[1].y = zY + (int)(0.5*zHeight);
	ps[2].x = zX + (int)(0.35*zWidth); ps[2].y = zY + (int)(0.65*zHeight);
   ::Polygon(hdc, ps, 3);

   
   ::SelectObject(hdc, hBrush2);
   
	ps[0].x = zX + zWidth-1;           ps[0].y = zY + 0;
	ps[1].x = zX + (int)(0.35*zWidth); ps[1].y = zY + (int)(0.65*zHeight);
	ps[2].x = zX + (int)(0.5*zWidth);  ps[2].y = zY + zHeight-1;
   ::Polygon(hdc, ps, 3);


   if (oldBrush)
      ::SelectObject(hdc, oldBrush);
   ::DeleteObject(hBrush1);
   ::DeleteObject(hBrush2);

}

void DrawSdk::Pointer::DrawMousePointer(HDC hdc, int zX, int zY, int zWidth, int zHeight) {
   POINT ps[10]; 
   HPEN hPen   = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
   HPEN oldPen = (HPEN) ::SelectObject(hdc, hPen);

   ps[0].x = zX + 0;                        ps[0].y = zY + 0;
   ps[1].x = zX + 0;                        ps[1].y = zY + (int)(0.53333*zHeight);
   ps[2].x = zX + (int)(0.13333*zWidth);    ps[2].y = zY + (int)(0.40000*zHeight);
   ps[3].x = zX + (int)(0.13333*zWidth);    ps[3].y = zY + (int)(0.43333*zHeight);
   ps[4].x = zX + (int)(0.26667*zWidth);    ps[4].y = zY + (int)(0.66667*zHeight);
   ps[5].x = zX + (int)(0.30000*zWidth);    ps[5].y = zY + (int)(0.66667*zHeight);
   ps[6].x = zX + (int)(0.33333*zWidth);    ps[6].y = zY + (int)(0.63333*zHeight);
   ps[7].x = zX + (int)(0.23333*zWidth);    ps[7].y = zY + (int)(0.40000*zHeight);
   ps[8].x = zX + (int)(0.23333*zWidth);    ps[8].y = zY + (int)(0.36667*zHeight);
   ps[9].x = zX + (int)(0.36667*zWidth);    ps[9].y = zY + (int)(0.36667*zHeight);

   ::Polyline(hdc, ps, 10);

   if (oldPen)
      ::SelectObject(hdc, oldPen);
   ::DeleteObject(hPen);

   HBRUSH hBrush1   = ::CreateSolidBrush(RGB(255, 255, 255));

   HBRUSH oldBrush = (HBRUSH)::SelectObject(hdc, hBrush1);

   ::Polygon(hdc, ps, 10);

   if (oldBrush)
      ::SelectObject(hdc, oldBrush);
   ::DeleteObject(hBrush1);
}

void DrawSdk::Pointer::GetLogicalBoundingBox(RECT *pRect)
{
   if (NULL == pRect)
      return;
   DrawObject::GetLogicalBoundingBox(pRect);

   int nShift = (m_bMousePointer) ? (0) : (pRect->right - pRect->left);
   pRect->left -= nShift;
   if (pRect->left < 0)
      pRect->left = 0;
   pRect->right -= nShift;
}

void DrawSdk::Pointer::Write(CharArray *pArray, bool bUseFullImagePath)
{
   if (pArray == NULL)
      return;

   static _TCHAR tszType[100];
   tszType[0] = 0;

   if (m_bMousePointer)
       _stprintf_s(tszType, 100,  _T(" type=\"mouse\""));

   static _TCHAR tszEntry[512];
   tszEntry[0] = 0;

   _stprintf(tszEntry, 
       _T("<POINTER x=\"%d\" y=\"%d\" status=\"1\"%s></POINTER>\n"), 
      (int)x_, (int)y_, tszType);
 
   pArray->AppendData(tszEntry);
}

bool DrawSdk::Pointer::IsMousePointer() {
    return m_bMousePointer;
}

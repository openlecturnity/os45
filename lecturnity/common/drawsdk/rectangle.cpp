#include "StdAfx.h"

#include "objects.h"
  
DrawSdk::Rectangle::Rectangle() : ColorObject()
{
}

DrawSdk::Rectangle::Rectangle(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle) : ColorObject(x, y, width, height, outlineColor, fillColor, lineWidth, lineStyle)
{
}

DrawSdk::Rectangle::Rectangle(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle) : ColorObject(x, y, width, height, outlineColor, lineWidth, lineStyle)
{
}

DrawSdk::Rectangle::~Rectangle()
{
}

DrawSdk::DrawObject* DrawSdk::Rectangle::Copy()
{
   Rectangle *pNew = NULL;
   
   if (isFilled_)
      pNew = new Rectangle(x_, y_, width_, height_, m_argbLineColor, m_argbFillColor, lineWidth_, 0);
   else
      pNew = new Rectangle(x_, y_, width_, height_, m_argbLineColor, lineWidth_, lineStyle_);

   pNew->m_gdipDashStyle = m_gdipDashStyle; // not stored as number
   CopyInteractiveFields(pNew);
   return pNew;
}

bool DrawSdk::Rectangle::HasSameValues(Rectangle *pOther)
{
   if (pOther == NULL)
      return false;

   return x_ == pOther->x_ && y_ == pOther->y_ && width_ == pOther->width_
      && height_ == pOther->height_ && m_argbLineColor == pOther->m_argbLineColor
      && m_argbFillColor == pOther->m_argbFillColor && lineWidth_ == pOther->lineWidth_
      && lineStyle_ == pOther->lineStyle_;
}


void DrawSdk::Rectangle::Draw(HDC hdc, double offX, double offY, ZoomManager *pZm)
{
   if (width_ == 0 && height_ == 0)
      return;

   Gdiplus::Graphics graphics(hdc);
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
   graphics.TranslateTransform(offX, offY);
   if (pZm != NULL)
   {
      double zoom = pZm->GetZoomFactor();
      graphics.ScaleTransform(zoom, zoom);
   }

   double dX = x_;
   double dY = y_;
   double dWidth = width_;
   double dHeight = height_;

   // Gdiplus only draws rectangles with positive width and height
   if (width_ < 0)
   {
      dX     = x_ + width_;
      dWidth = -width_;
   }
   if (height_ < 0)
   {
      dY = y_ + height_;
      dHeight = -height_;
   }
   //graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   bool bDrawLine = true;
   if (isFilled_)
   {
      Gdiplus::Color fillColor(m_argbFillColor);
      Gdiplus::SolidBrush fillBrush(fillColor);
      if (m_argbFillColor == m_argbLineColor)
      {
         graphics.FillRectangle(&fillBrush, (Gdiplus::REAL)(dX-(double)lineWidth_/2), (Gdiplus::REAL)(dY-(double)lineWidth_/2), 
            (Gdiplus::REAL)(dWidth+lineWidth_), (Gdiplus::REAL)(dHeight+lineWidth_));
         bDrawLine = false;
      }
      else
      {
         if (dWidth > lineWidth_ && dHeight > lineWidth_)
         {
            graphics.FillRectangle(&fillBrush, (Gdiplus::REAL)(dX+(double)lineWidth_/2), (Gdiplus::REAL)(dY+(double)lineWidth_/2), 
               (Gdiplus::REAL)(dWidth-lineWidth_), (Gdiplus::REAL)(dHeight-lineWidth_));
         }
         else
         {
            graphics.FillRectangle(&fillBrush, (Gdiplus::REAL)dX, (Gdiplus::REAL)dY, 
               (Gdiplus::REAL)dWidth, (Gdiplus::REAL)dHeight);
         }
      }
   }
   
   if (bDrawLine && lineWidth_ > 0)
   {
      Gdiplus::Color lineColor(m_argbLineColor);
      Gdiplus::Pen pen(lineColor, lineWidth_);
      pen.SetDashStyle(m_gdipDashStyle);
      graphics.DrawRectangle(&pen, (Gdiplus::REAL)dX, (Gdiplus::REAL)dY, 
         (Gdiplus::REAL)dWidth, (Gdiplus::REAL)dHeight);
   }

}

void DrawSdk::Rectangle::Write(CharArray *pArray, bool bUseFullImagePath)
{
   if (pArray == NULL)
      return;
  
   static _TCHAR tszVisibility[100];
   tszVisibility[0] = 0;
   if (HasVisibility())
      _stprintf(tszVisibility, _T(" visible=\"%s\""), GetVisibility());

   static _TCHAR tszBuffer[1000];
   tszBuffer[0] = 0;

   if (isFilled_)
   {
      _stprintf(tszBuffer, 
         _T("<FILLEDRECT x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" rgb=\"0x%08x\"%s></FILLEDRECT>\n"),
         (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, m_argbFillColor, tszVisibility);
   }
   else
   {
      _stprintf(tszBuffer, 
         _T("<OUTLINERECT x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" linewidth=\"%d\" linewidthf=\"%.2f\" linestyle=\"%d\" rgb=\"0x%08x\"%s></OUTLINERECT>\n"),
         (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, 
         (int)lineWidth_, lineWidth_, lineStyle_, m_argbLineColor, tszVisibility);
   
   }
 
   pArray->AppendData(tszBuffer);
}

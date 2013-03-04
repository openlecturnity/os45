#include "StdAfx.h"

#include "objects.h"
  
DrawSdk::Oval::Oval() : ColorObject()
{
}

DrawSdk::Oval::Oval(double x, double y, double width, double height, COLORREF outlineColor, COLORREF fillColor, double lineWidth, int lineStyle) : ColorObject(x, y, width, height, outlineColor, fillColor, lineWidth, lineStyle)
{
}

DrawSdk::Oval::Oval(double x, double y, double width, double height, COLORREF outlineColor, double lineWidth, int lineStyle) : ColorObject(x, y, width, height, outlineColor, lineWidth, lineStyle)
{
}

DrawSdk::Oval::Oval(Gdiplus::RectF *rect, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle) : ColorObject(rect, outlineColor, fillColor, lineWidth, lineStyle)
{
}

DrawSdk::Oval::Oval(Gdiplus::RectF *rect, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle) : ColorObject(rect, outlineColor, lineWidth, lineStyle)
{
}

DrawSdk::Oval::~Oval()
{
}

void DrawSdk::Oval::Draw(HDC hdc, double offX, double offY, ZoomManager *pZm)
{ 
   if (width_ == 0 && height_ == 0)
      return;

   Gdiplus::Graphics graphics(hdc);
   graphics.TranslateTransform(offX, offY);
   if (pZm != NULL)
   {
      double zoom = pZm->GetZoomFactor();
      graphics.ScaleTransform(zoom, zoom);
   }

   // there are problems if circles have netagive width and height
   double dX = x_;
   double dY = y_;
   double dWidth = width_;
   double dHeight = height_;
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
   
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   bool bDrawLine = true;
   if (isFilled_)
   {
      Gdiplus::Color fillColor(m_argbFillColor);
      Gdiplus::SolidBrush fillBrush(fillColor);
      if (m_argbFillColor == m_argbLineColor)
      {
         graphics.FillEllipse(&fillBrush, (Gdiplus::REAL)(dX-lineWidth_/2), (Gdiplus::REAL)(dY-lineWidth_/2), 
            (Gdiplus::REAL)(dWidth+lineWidth_), (Gdiplus::REAL)(dHeight+lineWidth_));
         bDrawLine = false;
      }
      else
      {
         graphics.FillEllipse(&fillBrush, (Gdiplus::REAL)(dX+lineWidth_/2), (Gdiplus::REAL)(dY+lineWidth_/2), 
            (Gdiplus::REAL)(dWidth-lineWidth_+1), (Gdiplus::REAL)(dHeight-lineWidth_+1));
      }
   }
   
   if (bDrawLine)
   {
      Gdiplus::Color lineColor(m_argbLineColor);
      Gdiplus::Pen pen(lineColor, lineWidth_);
      pen.SetDashStyle(m_gdipDashStyle);
      graphics.DrawEllipse(&pen, (Gdiplus::REAL)dX, (Gdiplus::REAL)dY, 
         (Gdiplus::REAL)dWidth, (Gdiplus::REAL)dHeight);
   }
}

void DrawSdk::Oval::Write(CharArray *pArray, bool bUseFullImagePath)
{
   if (pArray == NULL)
      return;

   static _TCHAR tszEntry[512];
   tszEntry[0] = 0;

   if (isFilled_)
   {
      _stprintf(tszEntry, 
         _T("<FILLEDCIRC x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" rgb=\"0x%08x\"></FILLEDCIRC>\n"),
         (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, m_argbFillColor);
   }
   else
   {
      _stprintf(tszEntry, 
         _T("<OUTLINECIRC x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" linewidth=\"%d\" linewidthf=\"%.2f\" linestyle=\"%d\" rgb=\"0x%08x\"></OUTLINECIRC>\n"),
         (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, 
         (int)lineWidth_, lineWidth_, lineStyle_, m_argbLineColor);
   }


   pArray->AppendData(tszEntry);
}

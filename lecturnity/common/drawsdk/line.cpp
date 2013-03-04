#include "StdAfx.h"

#include "objects.h"

  
DrawSdk::Line::Line() : ColorObject()
{
}

DrawSdk::Line::Line(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle, int arrowStyle, int arrowConfig, int ori, int dir) : ColorObject(x, y, width, height, outlineColor, lineWidth, lineStyle)
{
   ori_ = ori;
   dir_ = dir;
   arrowStyle_ = arrowStyle;
   ComputeArrowConfig(arrowConfig);

   if (dir)
      SwapArrows();

   if (dir_ == 1)
   {
      x_     = x_+width_;
      width_ = (-1)*width_;
   }

   if ((dir_ == 1 && ori_ == 1) ||
       (dir_ == 0 && ori_ == 0))
   {
      y_ = y_ + height_;
      height_ = (-1)*height_;
   }

}

DrawSdk::Line::~Line()
{
}

DrawSdk::DrawObject* DrawSdk::Line::Copy()
{
   // TODO what about ori, dir and width, height already changed in the constructor?
   Line *pNew = new Line(x_, y_, width_, height_, m_argbLineColor, lineWidth_, lineStyle_, arrowStyle_, 0, ori_, dir_);
   CopyInteractiveFields(pNew);
   
   // TODO arrowConfig is set to 0 (because it is currently not stored)
   pNew->iBeginArrowStyle = iBeginArrowStyle;
   pNew->iBeginArrowDimension = iBeginArrowDimension;
   pNew->iEndArrowStyle = iEndArrowStyle;
   pNew->iEndArrowDimension = iEndArrowDimension;
      
   return pNew;
}

void DrawSdk::Line::Draw(HDC hdc, double offX, double offY, ZoomManager *pZm)
{ 
   Gdiplus::Graphics graphics(hdc);
   graphics.TranslateTransform(offX, offY);
   bool drawArrows = true;
   if (pZm != NULL)
   {
      double zoom = pZm->GetZoomFactor();
      graphics.ScaleTransform(zoom, zoom);
      if (lineWidth_ * zoom < 0.5)
         drawArrows = false;
   }
   
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   Gdiplus::Color lineColor(m_argbLineColor);
   Gdiplus::Pen pen(lineColor, lineWidth_);
   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
   pen.SetDashStyle(m_gdipDashStyle);
   pen.SetAlignment(Gdiplus::PenAlignmentCenter);

   SetArrowCaps(pen);
  
   graphics.DrawLine(&pen, (Gdiplus::REAL)x_, (Gdiplus::REAL)y_, (Gdiplus::REAL)(x_ + width_), (Gdiplus::REAL)(y_ + height_));
}
 

void DrawSdk::Line::Write(CharArray *pArray, bool bUseFullImagePath)
{
   if (pArray == NULL)
      return;
   
   static _TCHAR tszVisibility[100];
   tszVisibility[0] = 0;
   if (HasVisibility())
      _stprintf(tszVisibility, _T(" visible=\"%s\""), GetVisibility());

   int arrowStyle = arrowStyle_;
   int arrowConfig = iBeginArrowStyle*1000 + iBeginArrowDimension*100 + 
                     iEndArrowStyle*10 + iEndArrowDimension;
   double x = x_;
   double y = y_;
   double width = width_;
   double height = height_;

   if (ori_ != -1 && dir_ != -1)
   {
      if (dir_ == 1)
      {
         x     = x+width;
         width = (-1)*width;
      }

      if ((dir_ == 1 && ori_ == 1) ||
          (dir_ == 0 && ori_ == 0))
      {
         y = y + height;
         height = (-1)*height;
      }

      if (dir_ == 1)
      {
         
         if (arrowStyle_ == 1)
            arrowStyle = 2;
         else if (arrowStyle_ == 2)
            arrowStyle = 1;
         
         arrowConfig = iEndArrowStyle*1000 + iEndArrowDimension*100 + 
                       iBeginArrowStyle*10 + iBeginArrowDimension;
      }
   }

   static _TCHAR tszTotal[1500];
   tszTotal[0] = 0;

   _stprintf(tszTotal, 
      _T("<LINE x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" ori=\"%d\" dir=\"%d\" linewidth=\"%d\" linewidthf=\"%.2f\" linestyle=\"%d\" arrowstyle=\"%d\" arrowconfig=\"%d\" rgb=\"0x%08x\"%s></LINE>\n"),
      (int)x, (int)y, (int)width, (int)height, x, y, width, height, 
      ori_, dir_, (int)lineWidth_, lineWidth_, lineStyle_, arrowStyle, arrowConfig, m_argbLineColor, tszVisibility);
   
   pArray->AppendData(tszTotal);
}

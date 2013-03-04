#include "StdAfx.h"

#include "objects.h"

#define PTS (*((std::vector<DPoint *> *) pts_))
  
DrawSdk::Marker::Marker() : Polygon()
{
   Init();
}

DrawSdk::Marker::Marker(DPoint *pts, int ptCount, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle) : Polygon(pts, ptCount, outlineColor, lineWidth, lineStyle, false)
{
   
   Init();
 
   BYTE alpha = (BYTE) (((DWORD)(m_argbLineColor >> 24) & 0x000000ff));
   if (alpha == 255) // outlineColor is in COLORREF-Format. Alpha is set to 255 by ColorObject. Set alpha to 7f
   {
      m_argbLineColor -= 0x80000000; // ff - 80 = 7f
   }
   CalculateDimension();
}

DrawSdk::Marker::~Marker()
{
}

void DrawSdk::Marker::Init()
{
   autoClose_ = false;
}

void DrawSdk::Marker::Draw(HDC hdc, double offX, double offY, ZoomManager *pZm)
{ 
   if (ptCount_ == 0)
      return;

   Gdiplus::Graphics graphics(hdc);
   graphics.TranslateTransform(offX, offY);
   if (pZm != NULL)
   {
      double zoom = pZm->GetZoomFactor();
      graphics.ScaleTransform(zoom, zoom);
   }
   

   // calculate point position dependent on zoom factor
   Gdiplus::PointF *zPts = (Gdiplus::PointF *)malloc(ptCount_*sizeof(Gdiplus::PointF));
   
   for (int i = 0; i < ptCount_; ++i)
   {
      zPts[i].X = PTS[i]->x;
      zPts[i].Y = PTS[i]->y;
   }


   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

   if (ptCount_ == 1 || AllPointsEqual())
   {
      Gdiplus::Color color(m_argbLineColor);
      Gdiplus::SolidBrush brush(color);
      int iHalfLineWidth = (lineWidth_ / 2) >= 1 ? (lineWidth_ / 2) : 1;
      graphics.FillRectangle(&brush, (Gdiplus::REAL)(zPts[0].X-iHalfLineWidth), (Gdiplus::REAL)(zPts[0].Y-iHalfLineWidth), (Gdiplus::REAL)(lineWidth_), (Gdiplus::REAL)(lineWidth_));
      free(zPts);
      return;
   }
   
   Gdiplus::Color lineColor(m_argbLineColor);
   Gdiplus::Pen pen(lineColor, lineWidth_);
   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare , Gdiplus::DashCapRound);
   pen.SetLineJoin(Gdiplus::LineJoinRound);
   
   graphics.DrawLines(&pen, zPts, ptCount_);

   free(zPts);
}

void DrawSdk::Marker::Write(CharArray *pArray, bool bUseFullImagePath)
{
   if (pArray == NULL)
      return;
   
   if (!bboxCalced_)
      CalculateDimension();

   static _TCHAR tszEntry[512];
   tszEntry[0] = 0;

   _stprintf(tszEntry, _T("<POLYLINES x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" count=\"%d\" linewidth=\"%d\" linewidthf=\"%.2f\" rgb=\"0x%08x\">\n"),
      (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, 
      ptCount_, (int)lineWidth_, lineWidth_, m_argbLineColor);
   
   pArray->AppendData(tszEntry);

   for (int i = 0; i < ptCount_; ++i)
   {
      _stprintf(tszEntry, _T("<pt x=\"%d\" y=\"%d\" xf=\"%.2f\" yf=\"%.2f\"></pt>\n"), 
         (int)PTS[i]->x, (int)PTS[i]->y, PTS[i]->x, PTS[i]->y);
      pArray->AppendData(tszEntry);
   }
   
   pArray->AppendData(_T("</POLYLINES>\n"));
}

void DrawSdk::Marker::ToFile(FILE *fp)
{
	if(fp == NULL)
		return;
   if (!bboxCalced_)
      CalculateDimension();
   
   fprintf(fp, "<POLYLINES x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" count=\"%d\" linewidth=\"%d\" linewidthf=\"%.2f\" rgb=\"0x%08x\">\n",
      (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, 
      ptCount_, (int)lineWidth_, lineWidth_, m_argbLineColor);

   for (int i = 0; i < ptCount_; ++i)
   {
	  fprintf(fp, "<pt x=\"%d\" y=\"%d\" xf=\"%.2f\" yf=\"%.2f\"></pt>\n", 
         (int)PTS[i]->x, (int)PTS[i]->y, PTS[i]->x, PTS[i]->y);
   }
   
   fprintf(fp,"</POLYLINES>\n");
}

#include "StdAfx.h"

#include "objects.h"

#define PTS (*((std::vector<DPoint *> *) pts_))
  
DrawSdk::Polygon::Polygon() : ColorObject()
{
   Init();

}

DrawSdk::Polygon::Polygon(DPoint *pts, int ptCount, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle, int arrowStyle, int arrowConfig, bool autoClose) : ColorObject(0, 0, 0, 0, outlineColor, lineWidth, lineStyle)
{
   Init();

   lineStyle_ = lineStyle;
   autoClose_ = autoClose;
   bboxCalced_ = false;

   arrowStyle_ = arrowStyle;
   ComputeArrowConfig(arrowConfig);

   SetPoints(pts, ptCount);
}

DrawSdk::Polygon::Polygon(DPoint *pts, int ptCount, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle) : ColorObject(0, 0, 0, 0, outlineColor, fillColor, lineWidth, lineStyle)
{
   Init();
   lineStyle_ = lineStyle;

   SetPoints(pts, ptCount);
}

DrawSdk::Polygon::~Polygon()
{
   DeletePoints();
   delete ((std::vector<DPoint *> *) pts_);
   pts_ = NULL;
}

void DrawSdk::Polygon::Init()
{
   ptCount_    = 0;
   autoClose_  = true;
   isFreehand_ = false;
   lineStyle_  = 0;

   pts_ = (void *) (new std::vector<DPoint *>);
   bboxCalced_ = false;
}

DrawSdk::DrawObject*  DrawSdk::Polygon::Copy()
{

    int iPointsNp = GetPoints(NULL, 0);
    DPoint *pts = (DPoint *)calloc(iPointsNp, sizeof(DPoint));
    //malloc(
    GetPoints(pts, ptCount_);
    Polygon *pNew = NULL;
    if(GetIsFilled())
    {
        pNew = new Polygon(pts, iPointsNp, m_argbLineColor, m_argbFillColor, lineWidth_, 0);
    }
    else
    {
        pNew = new Polygon(pts, iPointsNp, m_argbLineColor, lineWidth_);
    }
   
     
    //pNew->SetPoints((DrawSdk::DPoint *)pts_, ptCount_);
    pNew->SetLineStyle(GetLineStyle());
    pNew->SetAutoClose(GetIsAutoClose());
    pNew->SetIsFreehand(GetIsFreehand());
    CopyInteractiveFields(pNew);
    free(pts);
    return pNew;
}

void DrawSdk::Polygon::Move(double deltaX, double deltaY)
{
   x_ += deltaX;
   y_ += deltaY;


   for (int i = 0; i < PTS.size(); ++i)
   {
      PTS[i]->x += deltaX;
      PTS[i]->y += deltaY;
   }
}

void DrawSdk::Polygon::DeletePoints()
{
   for (int i = 0; i < PTS.size(); ++i)
   {
      DPoint *pt = PTS[i];
      delete pt;
   }

   PTS.clear();
   ptCount_ = 0;

   bboxCalced_ = false;
}

bool DrawSdk::Polygon::AllPointsEqual()
{
   if (PTS.size() < 2)
      return true;

   int x = PTS[0]->x;
   int y = PTS[0]->y;
   for (int i = 1; i < PTS.size(); ++i)
   {
      if (PTS[i]->x != x || PTS[i]->y != y)
         return false;
   }
   return true;
}

void DrawSdk::Polygon::SetPoints(DPoint *pts, int ptCount)
{
   DeletePoints();

   ptCount_ = ptCount;
   for (int i = 0; i < ptCount_; ++i)
   {
      DPoint *newPoint = new DPoint(pts[i].x, pts[i].y);
      PTS.push_back(newPoint);
   }

   // CalculateDimension();
   bboxCalced_ = false;
}

void DrawSdk::Polygon::AddPoint(DPoint &pts)
{
   DPoint *newPoint = new DPoint(pts.x, pts.y);
   PTS.push_back(newPoint);
   ++ptCount_;

   // CalculateDimension();
   bboxCalced_ = false;
}

void DrawSdk::Polygon::CalculateDimension()
{ 
   RECT bRect;

   if (ptCount_ <= 0)
   {
      bRect.left   = 0;
      bRect.top    = 0;
      bRect.right  = 0;
      bRect.bottom = 0;
   }
   else
   {
      bRect.left   = (int)PTS[0]->x;
      bRect.top    = (int)PTS[0]->y;
      bRect.right  = (int)PTS[0]->x;
      bRect.bottom = (int)PTS[0]->y;

      for (int i = 1; i < ptCount_; ++i)
      {
         if (PTS[i]->x < bRect.left)
            bRect.left = (int)PTS[i]->x;
         if (PTS[i]->y < bRect.top)
            bRect.top = (int)PTS[i]->y;
         if (PTS[i]->x > bRect.right)
            bRect.right = (int)PTS[i]->x;
         if (PTS[i]->y > bRect.bottom)
            bRect.bottom = (int)PTS[i]->y;
      }
   }

   x_ = (double)(bRect.left);
   y_ = (double)(bRect.top);
   width_  = (double)(bRect.right - bRect.left);
   height_ = (double)(bRect.bottom - bRect.top);

   bboxCalced_ = true;
}

int DrawSdk::Polygon::GetPoints(DPoint *pts, int ptCount)
{
   if (pts == NULL || ptCount == 0)
   {
      return PTS.size();
   }

   for (int i = 0; i < ptCount; ++i)
   {
      pts[i].x = PTS[i]->x;
      pts[i].y = PTS[i]->y;
   }

   return 0;
}

int DrawSdk::Polygon::GetCount()
{ 
   return ptCount_;
}

double DrawSdk::Polygon::GetWidth()
{
   if (!bboxCalced_)
      CalculateDimension();
   return width_;
}

double DrawSdk::Polygon::GetHeight()
{
   if (!bboxCalced_)
      CalculateDimension();
   return height_;
}

double DrawSdk::Polygon::GetX()
{
   if (!bboxCalced_)
      CalculateDimension();
   return x_;
}

double DrawSdk::Polygon::GetY()
{
   if (!bboxCalced_)
      CalculateDimension();
   return y_;
}

void DrawSdk::Polygon::Draw(HDC hdc, double offX, double offY, ZoomManager *pZm)
{ 
   if (ptCount_ == 0)
      return;

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
      if (isFreehand_)
      {
         graphics.FillEllipse(&brush, (Gdiplus::REAL)(zPts[0].X-iHalfLineWidth), (Gdiplus::REAL)(zPts[0].Y-iHalfLineWidth), (Gdiplus::REAL)(lineWidth_), (Gdiplus::REAL)(lineWidth_));
      }
      else
      {
         graphics.FillRectangle(&brush, (Gdiplus::REAL)(zPts[0].X-iHalfLineWidth), (Gdiplus::REAL)(zPts[0].Y-iHalfLineWidth), (Gdiplus::REAL)(lineWidth_), (Gdiplus::REAL)(lineWidth_));
      }
      free(zPts);
      return;
   }
   
   if (isFilled_)
   {
      Gdiplus::Color color(m_argbFillColor);
      Gdiplus::SolidBrush brush(color);
      graphics.FillPolygon(&brush, zPts, ptCount_, Gdiplus::FillModeWinding);
   }

   if(lineWidth_ > 0)
   {
	   Gdiplus::Color lineColor(m_argbLineColor);
	   Gdiplus::Pen pen(lineColor, lineWidth_);
	   bool bIsClosed = false;
	   if (zPts[0].X == zPts[ptCount_-1].X && 
		   zPts[0].Y == zPts[ptCount_-1].Y)
		   bIsClosed = true;

	   if (autoClose_ || bIsClosed)
	   {
		   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
		   pen.SetLineJoin(Gdiplus::LineJoinMiter);
		   pen.SetDashStyle(m_gdipDashStyle);
		   graphics.DrawPolygon(&pen, zPts, ptCount_);
	   }
	   else
	   {
		   BYTE a = lineColor.GetA();
		   BYTE r = lineColor.GetR();
		   BYTE g = lineColor.GetG();
		   BYTE b = lineColor.GetB();
		   if (isFreehand_)
		   {
			   pen.SetLineCap(Gdiplus::LineCapRound , Gdiplus::LineCapRound, Gdiplus::DashCapRound);
			   pen.SetLineJoin(Gdiplus::LineJoinRound);
		   }
		   else
		   { 
			   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
			   pen.SetLineJoin(Gdiplus::LineJoinRound);

			   SetArrowCaps(pen);
			   pen.SetDashStyle(m_gdipDashStyle);
		   }

		   graphics.DrawLines(&pen, zPts, ptCount_);
	   }
   }
   
   free(zPts);
}

void DrawSdk::Polygon::Write(CharArray *pArray, bool bUseFullImagePath)
{
   if (pArray == NULL)
      return;

   static _TCHAR tszSingleLine[512];
   tszSingleLine[0] = 0;

   if (!bboxCalced_)
      CalculateDimension();
   
   if (autoClose_)
   {
      _stprintf (tszSingleLine, 
         _T("<FILLEDPOLY x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" count=\"%d\" rgb=\"0x%08x\">\n"),
         (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, ptCount_, m_argbFillColor);
   }
   else
   {
      if (isFreehand_)
      {         
         _stprintf(tszSingleLine, 
            _T("<FREEHAND x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" count=\"%d\" linewidth=\"%d\" linewidthf=\"%.2f\" linestyle=\"0\" rgb=\"0x%08x\">\n"),
            (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, 
            ptCount_, (int)lineWidth_, lineWidth_, m_argbLineColor);
   
      }
      else
      {
         _stprintf(tszSingleLine, 
            _T("<POLYLINES x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" count=\"%d\" linewidth=\"%d\" linewidthf=\"%.2f\" linestyle=\"%d\" rgb=\"0x%08x\">\n"),
            (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, 
            ptCount_, (int)lineWidth_, lineWidth_, lineStyle_, m_argbLineColor);
      }
   }

   pArray->AppendData(tszSingleLine);

   for (int i = 0; i < ptCount_; ++i)
   {
      _stprintf(tszSingleLine, _T("<pt x=\"%d\" y=\"%d\" xf=\"%.2f\" yf=\"%.2f\"></pt>\n"), 
         (int)PTS[i]->x, (int)PTS[i]->y, PTS[i]->x, PTS[i]->y);
      pArray->AppendData(tszSingleLine);
   }
   
   if (autoClose_)
      pArray->AppendData(_T("</FILLEDPOLY>\n"));       
   else
   {
      if (isFreehand_)
         pArray->AppendData(_T("</FREEHAND>\n"));
      else
         pArray->AppendData(_T("</POLYLINES>\n"));
   }
   
}

void DrawSdk::Polygon::ToFile(FILE *fp)
{
	if(fp == NULL)
		return;

   if (!bboxCalced_)
      CalculateDimension();
   
   if (autoClose_)
   {
      fprintf(fp,"<FILLEDPOLY x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" count=\"%d\" rgb=\"0x%08x\">\n",
         (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, ptCount_, m_argbFillColor);
   }
   else
   {
      if (isFreehand_)
      {         
         fprintf(fp,"<FREEHAND x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" count=\"%d\" linewidth=\"%d\" linewidthf=\"%.2f\" linestyle=\"0\" rgb=\"0x%08x\">\n",
            (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, 
            ptCount_, (int)lineWidth_, lineWidth_, m_argbLineColor);
   
      }
      else
      {
         fprintf(fp,"<POLYLINES x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" count=\"%d\" linewidth=\"%d\" linewidthf=\"%.2f\" linestyle=\"%d\" rgb=\"0x%08x\">\n",
            (int)x_, (int)y_, (int)width_, (int)height_, x_, y_, width_, height_, 
            ptCount_, (int)lineWidth_, lineWidth_, lineStyle_, m_argbLineColor);
      }
   }

   for (int i = 0; i < ptCount_; ++i)
   {
      fprintf(fp,"<pt x=\"%d\" y=\"%d\" xf=\"%.2f\" yf=\"%.2f\"></pt>\n", 
         (int)PTS[i]->x, (int)PTS[i]->y, PTS[i]->x, PTS[i]->y);
   }
   
   if (autoClose_)
      fprintf(fp, "</FILLEDPOLY>\n");
   else
   {
      if (isFreehand_)
         fprintf(fp, "</FREEHAND>\n");
      else
        fprintf(fp, "</POLYLINES>\n");
   }
   
}
/****************************CDC *pDC, *****************************************

  program : la_marker.cpp
  authors : Gabriela Maass
  date    : 14.03.2003
  desc    : Functions to draw and manipulate objects on a tcl-canvas
  
**********************************************************************/
#include "StdAfx.h"

#include "PageObject.h"
#include "DrawObjects.h"

ASSISTANT::Marker::Marker(double _x, double _y, int _zPosition, LPCTSTR _color, int _linewidth, UINT _id, 
                          LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects) 
: PolyObject(_x, _y, 0, 0, _zPosition, _color, _T("none"), _linewidth, _T(" "), _id, _hyperlink, _currentDirectory, _linkedObjects) 
{
   visible = true;
   
   BYTE alpha = (BYTE) (((DWORD)(m_argbLineColor >> 24) & 0x000000ff));
   if (alpha == 255) // outlineColor is in COLORREF-Format. Alpha is set to 255 by ColorObject. Set alpha to 7f
   {
      m_argbLineColor -= 0x80000000; // ff - 80 = 7f
   }
}

ASSISTANT::Marker::~Marker() 
{
   for (int i = 0; i < points.GetSize(); ++i) 
   {
      delete points[i];
   }
   points.RemoveAll();
}

ASSISTANT::DrawObject *ASSISTANT::Marker::Copy(bool keepZPosition)
{
   ASSISTANT::Marker *ret = new Marker(m_dX, m_dY, -1, color, lineWidth, 0, hyperlink_, currentDirectory_, m_ssLinkedObjects);
	
  if (isInternLink)
     ret->LinkIsIntern(true);
  else
     ret->LinkIsIntern(false);

   for (int i = 0; i < points.GetSize(); ++i) 
   {
      ret->AppendNewPoint((int)points[i]->X,(int)points[i]->Y);
   }
	
   return (DrawObject *)ret;
}

void ASSISTANT::Marker::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) 
      return;
   
   if (points.GetSize() == 0)
      return;


   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform((Gdiplus::REAL)dOffX, (Gdiplus::REAL)dOffY);
   
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   
   Gdiplus::PointF *zPts = (Gdiplus::PointF *)malloc(points.GetSize()*sizeof(Gdiplus::PointF));
   for (int i = 0; i < points.GetSize(); ++i)
   {
      zPts[i].X = (Gdiplus::REAL)points[i]->X;
      zPts[i].Y = (Gdiplus::REAL)points[i]->Y;
   }

   
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

   if (points.GetSize() == 1 || AllPointsEqual())
   {
      Gdiplus::Color color(m_argbLineColor);
      Gdiplus::SolidBrush brush(color);
      int iHalfLineWidth = (lineWidth / 2) >= 1 ? (lineWidth / 2) : 1;
      
      graphics.FillRectangle(&brush, (Gdiplus::REAL)(zPts[0].X-iHalfLineWidth), (Gdiplus::REAL)(zPts[0].Y-iHalfLineWidth), 
         (Gdiplus::REAL)(lineWidth), (Gdiplus::REAL)(lineWidth));
      delete zPts;
      return;
   }
   
   Gdiplus::Color lineColor(m_argbLineColor);
   Gdiplus::Pen pen(lineColor, (Gdiplus::REAL)lineWidth);
   
   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapRound);
   pen.SetLineJoin(Gdiplus::LineJoinRound);
   
   graphics.DrawLines(&pen, zPts, points.GetSize());

   delete zPts;
}

void ASSISTANT::Marker::Hide()
{
   if (!visible) 
      return;
	
   /*
   if (selected) 
      DeleteSelection(canvas);
	
   canvas->DeleteObject(tkref.c_str());
   canvas->DeleteLineSegments(tkref.c_str());
   */
   visible = false;
	
}

ASSISTANT::DrawObject *ASSISTANT::Marker::Load(SGMLElement *hilf) 
{
   Marker
      *obj;
   TCHAR
      *tmp;
   int
      x, y,
		zPosition, 
      lineWidth,
      iPPTId;
   TCHAR 
      *color,
      *hyperlink,
      *currentDirectory,
      *linkedObjects;
   
   if (hilf->Parameter.GetLength() == 0) 
      return NULL;
   
   tmp = hilf->GetAttribute("x");
   if (tmp) x = _ttoi(tmp);
   else     x = 0;
	
   tmp = hilf->GetAttribute("y");
   if (tmp) y = _ttoi(tmp);
   else     y = 0;
	
   tmp = hilf->GetAttribute("color");
   if (tmp) color = _tcsdup(tmp);
   else     color = _tcsdup(_T("#7fffff00"));
	
   tmp = hilf->GetAttribute("lineWidth");
   if (tmp) lineWidth = _ttoi(tmp);
   else     lineWidth = 20;
	
   tmp = hilf->GetAttribute("address");
   if (tmp) hyperlink = _tcsdup(tmp);
   else     hyperlink = NULL;
   
   tmp = hilf->GetAttribute("path");
   if (tmp) currentDirectory = _tcsdup(tmp);
   else     currentDirectory = NULL;
   
   tmp = hilf->GetAttribute("linkedObjects");
   if (tmp) linkedObjects = _tcsdup(tmp);
   else     linkedObjects = NULL;
   
   tmp = hilf->GetAttribute("id");
   if (tmp) iPPTId = _ttoi(tmp);
   else     iPPTId = -1;
   
   tmp = hilf->GetAttribute("ZPosition");
   if (tmp) zPosition = _ttoi(tmp);
   else     zPosition = -1;
	
   obj = new ASSISTANT::Marker(x, y, zPosition, color, lineWidth, 0, hyperlink, currentDirectory, linkedObjects);
   

   /*
   p = (char *)hilf->Parameter;
   while (*p != '\0') {
      while ((*p != '{') && (*p!='\0')) p++;
      if (*p=='\0') break;
      p++;
      tmp = p;
      while (*p != ' ') p++;
      *p = '\0';
      x = atoi(tmp);
      p++;
      tmp = p;
      while (*p != '}') p++;
      *p = '\0';
      y = atoi(tmp);
      obj->AppendNewPoint(x,y);
      p++;
   }
   */
   
   if (obj && iPPTId > 0)
      obj->SetPPTObjectId(iPPTId);
   
   delete color;
	
   if (hyperlink)
      delete hyperlink;

   if (currentDirectory)
      delete currentDirectory;
   
   if (linkedObjects)
      delete linkedObjects;
   
   return obj;
}


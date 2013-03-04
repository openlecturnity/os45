/*********************************************************************

  program : mlb_ovals.cpp
  authors : Gabriela Maass
  date    : 3.12.1999
  desc    : Functions to draw and manipulate objects on a tcl-canvas
  
**********************************************************************/

#include "StdAfx.h"
#include <math.h>
#include "PageObject.h"
#include "DrawObjects.h"


ASSISTANT::Oval::Oval(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, 
                      LPCTSTR _fillColor, int _lineWidth, LPCTSTR _lineStyle, UINT _id, 
                      LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                      : ColoredObject(_x, _y, _width, _height, _zPosition, _color, _fillColor, _lineWidth, _lineStyle, _id, _hyperlink, _currentDirectory, _linkedObjects) 
{
}

ASSISTANT::Oval::~Oval() {
}

void ASSISTANT::Oval::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;

   if (m_dWidth == 0 && m_dHeight == 0)
      return;

   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform((Gdiplus::REAL)dOffX, (Gdiplus::REAL)dOffY);
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   // there are problems if circles have netagive width and height
   double dX = m_dX;
   double dY = m_dY;
   double dWidth = m_dWidth;
   double dHeight = m_dHeight;
   if (m_dWidth < 0)
   {
      dX     = m_dX + m_dWidth;
      dWidth = -m_dWidth;
   }
   if (m_dHeight < 0)
   {
      dY = m_dY + m_dHeight;
      dHeight = -m_dHeight;
   }
   
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   bool bDrawLine = true;
   if (fillColor != "none")
   {
      Gdiplus::Color fillColor(m_argbFillColor);
      Gdiplus::SolidBrush fillBrush(fillColor);
      if (m_argbFillColor == m_argbLineColor)
      {
         graphics.FillEllipse(&fillBrush, (Gdiplus::REAL)(dX-lineWidth/2), (Gdiplus::REAL)(dY-lineWidth/2), 
            (Gdiplus::REAL)(dWidth+lineWidth), (Gdiplus::REAL)(dHeight+lineWidth));
         bDrawLine = false;
      }
      else
      {
         graphics.FillEllipse(&fillBrush, (Gdiplus::REAL)(dX+lineWidth/2), (Gdiplus::REAL)(dY+lineWidth/2), 
            (Gdiplus::REAL)(dWidth-lineWidth+1), (Gdiplus::REAL)(dHeight-lineWidth+1));
      }
   }
   
   if (bDrawLine)
   {
      Gdiplus::Color lineColor(m_argbLineColor);
      Gdiplus::Pen pen(lineColor, (Gdiplus::REAL)lineWidth);
      pen.SetDashStyle(m_gdipDashStyle);
      graphics.DrawEllipse(&pen, (Gdiplus::REAL)dX, (Gdiplus::REAL)dY, 
         (Gdiplus::REAL)dWidth, (Gdiplus::REAL)dHeight);
   }


   
   if (!hyperlink_.IsEmpty() || HasObjectLinks())
   {
      //_canvas->ObjectIsHyperlink(tkref);
   }
}

ASSISTANT::DrawObject *ASSISTANT::Oval::Copy(bool keepZPosition)
{
   ASSISTANT::Oval 
      *ret;
   
	ret = new Oval(m_dX, m_dY, m_dWidth, m_dHeight, -1, color, fillColor,
		lineWidth, lineStyle, 0, hyperlink_, currentDirectory_, m_ssLinkedObjects);
   
  if (isInternLink)
     ret->LinkIsIntern(true);
  else
     ret->LinkIsIntern(false);

	if (keepZPosition)
		ret->SetOrder(m_fOrder);
	
   return (DrawObject *)ret;
}

ASSISTANT::DrawObject *ASSISTANT::Oval::Load(SGMLElement *hilf) 
{
   Oval
      *obj;
   TCHAR
      *tmp;
   int
      x, y,
		zPosition,
      width, height,
      lineWidth,
      iPPTId;
   TCHAR 
      *color,
      *fillColor,
      *lineStyle,
      *hyperlink,
      *internLink,
      *currentDirectory,
      *linkedObjects;
   
   tmp = hilf->GetAttribute("x");
   if (tmp) x = _ttoi(tmp);
   else     x = 0;
   
   tmp = hilf->GetAttribute("y");
   if (tmp) y = _ttoi(tmp);
   else     y = 0;
   
   tmp = hilf->GetAttribute("width");
   if (tmp) width = _ttoi(tmp);
   else     width = 0;
   
   tmp = hilf->GetAttribute("height");
   if (tmp) height = _ttoi(tmp);
   else     height = 0;
   
   tmp = hilf->GetAttribute("color");
   if (tmp) color = _tcsdup(tmp);
   else     color = _tcsdup(_T("#ff0000"));
   
   tmp = hilf->GetAttribute("fillColor");
   if (tmp) fillColor = _tcsdup(tmp);
   else     fillColor = _tcsdup(_T("none"));
   
   tmp = hilf->GetAttribute("lineWidth");
   if (tmp) lineWidth = _ttoi(tmp);
   else     lineWidth = 2;
   
   tmp = hilf->GetAttribute("lineStyle");
   if (tmp) lineStyle = _tcsdup(tmp);
   else     lineStyle = _tcsdup(_T(" "));
   
   tmp = hilf->GetAttribute("address");
   if (tmp) hyperlink = _tcsdup(tmp);
   else     hyperlink = NULL;
   
   tmp = hilf->GetAttribute("intern");
   if (tmp) internLink = _tcsdup(tmp);
   else     internLink = NULL;
   
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

   if (hyperlink != NULL)
   {
      obj = new Oval(x, y, width, height, zPosition, color, fillColor, lineWidth, lineStyle,
         0, hyperlink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new Oval(x, y, width, height, zPosition, color, fillColor, lineWidth, lineStyle,
         0, internLink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(true);
   }
   
   if (obj && iPPTId > 0)
      obj->SetPPTObjectId(iPPTId);

   delete color;
   delete fillColor;
   delete lineStyle;
   
   if (hyperlink)
      delete hyperlink;
   
   if (internLink)
      delete internLink;
   
   if (currentDirectory)
      delete currentDirectory;
   
   if (linkedObjects)
      delete linkedObjects;
   
   return obj;
}


/*********************************************************************
program : mlb_rectangles.cpp
authors : Gabriela Maass
date    : 3.12.1999
desc    : Functions to draw and manipulate objects on a tcl-canvas

**********************************************************************/

#include "StdAfx.h"

#include "mlb_objects.h"
#include "la_project.h" 
#include "mlb_page.h"
#include "mlb_misc.h"


ASSISTANT::Rectangle::Rectangle(double _x, double _y, double _width, double _height, int _zPosition, 
                                Gdiplus::ARGB argbLineColor, Gdiplus::ARGB argbFillColor, 
                                int _lineWidth, Gdiplus::DashStyle gdipLineStyle, uint32 _id, 
                                LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                     : ColoredObject(_x, _y, _width, _height, _zPosition, 
                                     argbLineColor, argbFillColor, _lineWidth, gdipLineStyle, _id, 
                                     _hyperlink, _currentDirectory, _linkedObjects) 
{  
   isBackground = false;
}

ASSISTANT::Rectangle::Rectangle(double _x, double _y, double _width, double _height, int _zPosition, 
                                Gdiplus::ARGB argbLineColor, uint32 _id, 
                                LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects) 
                     : ColoredObject(_x, _y, _width, _height, _zPosition, 
                                     argbLineColor, argbLineColor, 1, Gdiplus::DashStyleSolid, _id, 
                                     _hyperlink, _currentDirectory, _linkedObjects)
{
   isBackground = true;
}

ASSISTANT::Rectangle::~Rectangle() {
}

void ASSISTANT::Rectangle::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;

   if (m_dWidth == 0 && m_dHeight == 0)
      return;

   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   double dX = m_dX;
   double dY = m_dY;
   double dWidth = m_dWidth;
   double dHeight = m_dHeight;

   // Gdiplus only draws rectangles with positive width and height
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
   if (m_argbFillColor != 0)
   {
      Gdiplus::Color fillColor(m_argbFillColor);
      Gdiplus::SolidBrush fillBrush(fillColor);
      if (m_argbFillColor == m_argbLineColor)
      {
         graphics.FillRectangle(&fillBrush, (Gdiplus::REAL)(dX-lineWidth/2), (Gdiplus::REAL)(dY-lineWidth/2), 
            (Gdiplus::REAL)(dWidth+lineWidth), (Gdiplus::REAL)(dHeight+lineWidth));
         bDrawLine = false;
      }
      else
      {
         if (dWidth > lineWidth && dHeight > lineWidth)
         {
            graphics.FillRectangle(&fillBrush, (Gdiplus::REAL)(dX+lineWidth/2), (Gdiplus::REAL)(dY+lineWidth/2), 
               (Gdiplus::REAL)(dWidth-lineWidth+1), (Gdiplus::REAL)(dHeight-lineWidth+1));
         }
         else
         {
            graphics.FillRectangle(&fillBrush, (Gdiplus::REAL)dX, (Gdiplus::REAL)dY, 
               (Gdiplus::REAL)dWidth, (Gdiplus::REAL)dHeight);
         }
      }
   }
   
   if (bDrawLine)
   {
      Gdiplus::Color lineColor(m_argbLineColor);
      Gdiplus::Pen pen(lineColor, lineWidth);
      pen.SetDashStyle(m_gdipLineStyle);
      graphics.DrawRectangle(&pen, (Gdiplus::REAL)dX, (Gdiplus::REAL)dY, 
         (Gdiplus::REAL)dWidth, (Gdiplus::REAL)dHeight);
   }

}

ASSISTANT::DrawObject *ASSISTANT::Rectangle::Copy(bool keepZPosition)
{
   ASSISTANT::Rectangle 
      *ret;
   
   ret = new Rectangle(m_dX, m_dY, m_dWidth, m_dHeight, -1, 
                       m_argbLineColor, m_argbFillColor,
                       lineWidth, m_gdipLineStyle, Project::active->GetObjectID(), 
                       hyperlink_, currentDirectory_, m_ssLinkedObjects);
   
   if (isInternLink)
      ret->LinkIsIntern(true);
   else
      ret->LinkIsIntern(false);
   
   if (keepZPosition)
      ret->SetOrder(order);
   
   return (DrawObject *)ret;
}

void ASSISTANT::Rectangle::SaveObject(FILE *fp, int level, const _TCHAR *path)
{
   if (!visible) return;
   
   for (int i = 0; i < level; i++) 
      fwprintf(fp, L"  ");
   
   CString hyperlinkString;
   MakeHyperlinkString(hyperlinkString);
   WCHAR *wszHyperlink = ASSISTANT::ConvertTCharToWChar(hyperlinkString);
   
   CString csColor;
   ASSISTANT::GdipARGBToString(m_argbLineColor, csColor);
   WCHAR *wszLineColor = ASSISTANT::ConvertTCharToWChar(csColor);

   CString csFillColor;
   ASSISTANT::GdipARGBToString(m_argbFillColor, csFillColor);
   WCHAR *wszFillColor = ASSISTANT::ConvertTCharToWChar(csFillColor);

   CString csLineStyle;
   ASSISTANT::GdipLineStyleToString(m_gdipLineStyle, csLineStyle);
   WCHAR *wszLineStyle = ASSISTANT::ConvertTCharToWChar(csLineStyle);

   fwprintf(fp, L"<RECTANGLE x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" color=\"%s\" fillColor=\"%s\" lineWidth=\"%d\" lineStyle=\"%s\" ZPosition=\"%d\"%s></RECTANGLE>\n",
               (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight,
               wszLineColor, wszFillColor, lineWidth, wszLineStyle, (int)order, wszHyperlink);
   
   if (wszHyperlink)
      delete wszHyperlink;

   if (wszLineColor)
      delete wszLineColor;

   if (wszFillColor)
      delete wszFillColor;

   if (wszLineStyle)
      delete wszLineStyle;

}

void ASSISTANT::Rectangle::SaveAsAOF(CFileOutput *fp)
{
   if (!visible || recorded) 
      return;
   
   if (m_dWidth == 0 && m_dHeight == 0)
      return;
   
   recorded = true;
   
   double dX, dY, dWidth, dHeight;
   GetNormalizedCoordinates(dX, dY, dWidth, dHeight);

   recID = ASSISTANT::Project::active->recCount;

   // Calculate Color
   CString csColor;
   ASSISTANT::GdipARGBToObjString(m_argbLineColor, csColor);

   CString csFillColor;
   ASSISTANT::GdipARGBToObjString(m_argbFillColor, csFillColor);

   int iLineStyle = ASSISTANT::GdipLineStyleToObjStyle(m_gdipLineStyle);

   CString line;

   if (isBackground) 
   {
      line.Format(_T("<FILLEDRECT x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f rgb=0x%s></FILLEDRECT>\n"),
                  (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                  dX, dY, dWidth, dHeight,
                  csFillColor);
      fp->Append(line);

      ASSISTANT::Project::active->recCount++;
      return;
   }
   
   if (m_argbFillColor != 0) /* Filled Rectangle */
   {
      line.Format(_T("<FILLEDRECT x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f rgb=0x%s></FILLEDRECT>\n"),
                  (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                  dX, dY, dWidth, dHeight,
                  csFillColor);
      fp->Append(line);

      ASSISTANT::Project::active->recCount++;
   }
   
   line.Format(_T("<OUTLINERECT x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f linewidth=%d linestyle=%d rgb=0x%s></OUTLINERECT>\n"),
               (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
               dX, dY, dWidth, dHeight,
               lineWidth, iLineStyle, csColor);
   fp->Append(line);

   ASSISTANT::Project::active->recCount++;
}

int ASSISTANT::Rectangle::RecordableObjects()
{
   if (m_dWidth==0 && m_dHeight==0) visible = false;
   if (!visible) return 0;
   
   if (m_argbFillColor == 0) /* Outline Rectangle */
      return 1;
   else 
      return 2;
}


bool ASSISTANT::Rectangle::Overlap(int _x, int _y, float _zoomFactor)
{ 
   int
      x1, y1,
      x2, y2;
   
   if (!visible) return false;
   
   GetBBox(&x1,&y1,&x2,&y2);
   if ((_x < x1*_zoomFactor) || (_y < y1*_zoomFactor) || (_x > x2*_zoomFactor) || (_y > y2*_zoomFactor)) return false;
   
   return true; //canvas->Overlap(tkref, _x, _y);
}


void ASSISTANT::Rectangle::AdjustGui() 
{
}

bool ASSISTANT::Rectangle::ContainsPoint(Gdiplus::PointF &ptMouse)
{
   if (!visible)
      return false; 

   CRect rcObject;
   GetBoundingBox(rcObject);

   CRgn rgnRectangle;

   double dMaxDistance = (double)lineWidth / 2 + 0.5;
   BOOL bRet = TRUE;
   if (IsFilled())
   {
      bRet = rgnRectangle.CreateRectRgnIndirect(rcObject);
      
      if (!bRet)
         return false;
      if (rgnRectangle.PtInRegion((int)ptMouse.X, (int)ptMouse.Y))
         return true;
      
   }
   else
   {
      CRect rcOutside = rcObject;
      rcOutside.InflateRect(dMaxDistance, dMaxDistance, dMaxDistance, dMaxDistance);
      bRet = rgnRectangle.CreateRectRgnIndirect(rcOutside);
      if (!bRet)
         return false;

      CRgn rgnRectangle2;
      CRect rcInside = rcObject;
      rcInside.DeflateRect(dMaxDistance, dMaxDistance, dMaxDistance, dMaxDistance);
      bRet = rgnRectangle2.CreateRectRgnIndirect(rcInside);
      if (!bRet)
         return false;

      rgnRectangle.CombineRgn(&rgnRectangle, &rgnRectangle2, RGN_DIFF);
      if (rgnRectangle.PtInRegion((int)ptMouse.X, (int)ptMouse.Y))
         return true;

   }

   return false;  
}


ASSISTANT::DrawObject *ASSISTANT::Rectangle::Load(SGMLElement *hilf) 
{
   Rectangle 
      *obj;
   TCHAR
      *tmp;
   int
      x, y,
      width, height,
      zPosition,
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
   
   
   Gdiplus::ARGB argbLineColor = ASSISTANT::StringToGdipARGB(color);
   Gdiplus::ARGB argbFillColor = ASSISTANT::StringToGdipARGB(fillColor);
   Gdiplus::DashStyle gdipLineStyle = ASSISTANT::StringToGdipLineStyle(lineStyle);

   if (hyperlink)
   {
      obj = new Rectangle(x, y, width, height, zPosition,
                           argbLineColor, argbFillColor, 
                           lineWidth, gdipLineStyle, Project::active->GetObjectID(), 
                           hyperlink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new Rectangle(x, y, width, height, zPosition, 
                          argbLineColor, argbFillColor, 
                          lineWidth, gdipLineStyle, Project::active->GetObjectID(), 
                          internLink, currentDirectory,linkedObjects);
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







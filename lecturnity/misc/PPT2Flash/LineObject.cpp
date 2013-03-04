/**********************************************************************

  program : mlb_line.cpp
  authors : Gabriela Maass
  date    : 3.12.1999
  desc    : Functions to draw and manipulate objects on a tcl-canvas
  
**********************************************************************/

#include "StdAfx.h"

#include "DrawObjects.h"


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

ASSISTANT::Line::Line(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, 
                      int _lineWidth, LPCTSTR _lineStyle, LPCTSTR _capStyle, LPCTSTR _joinStyle, 
                      LPCTSTR _arrowStyle, int _arrowConfig, UINT _id, 
                      LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                      : ColoredObject(_x, _y, _width, _height, _zPosition, _color, _T("none"), _lineWidth, _lineStyle, _id, _hyperlink, _currentDirectory, _linkedObjects)
{
   
   CString 
      zahl;
   
   capStyle    = _capStyle;
   joinStyle   = _joinStyle;
   arrowStyle  = _arrowStyle;
   arrowConfig = _arrowConfig;
   if (arrowConfig == 0 && arrowConfig != NULL)
   {
      if (_tcscmp(arrowStyle, _T("first")) == 0)
          arrowConfig = 1500;
      else if (_tcscmp(arrowStyle, _T("last")) == 0)
         arrowConfig = 15;
      else if (_tcscmp(arrowStyle, _T("both")) == 0)
         arrowConfig = 1515;
   }
   ComputeArrowConfig(arrowConfig);
   //if ((width==0) && (height==0)) width = 1;
}

ASSISTANT::Line::~Line() {
}

ASSISTANT::DrawObject *ASSISTANT::Line::Copy(bool keepZPosition)
{
   ASSISTANT::Line *ret;
   
	ret = new Line(m_dX, m_dY, m_dWidth, m_dHeight, -1,
		color, lineWidth, lineStyle, capStyle,
		joinStyle, arrowStyle, arrowConfig, 0, hyperlink_, currentDirectory_, m_ssLinkedObjects);
   
  if (isInternLink)
     ret->LinkIsIntern(true);
  else
     ret->LinkIsIntern(false);

	if (keepZPosition)
		ret->SetOrder(m_fOrder);
	
   return (DrawObject *)ret;
}

void ASSISTANT::Line::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform((Gdiplus::REAL)dOffX, (Gdiplus::REAL)dOffY);
   bool drawArrows = true;
      
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);
   
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   Gdiplus::Color lineColor(m_argbLineColor);
  
   Gdiplus::Pen pen(lineColor, (Gdiplus::REAL)lineWidth);
   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
   pen.SetDashStyle(m_gdipDashStyle);
   pen.SetAlignment(Gdiplus::PenAlignmentCenter);

   SetArrowCaps(pen);

   graphics.DrawLine(&pen, (Gdiplus::REAL)m_dX, (Gdiplus::REAL)m_dY, (Gdiplus::REAL)(m_dX + m_dWidth), (Gdiplus::REAL)(m_dY + m_dHeight));

}

void ASSISTANT::Line::Resize(float fDiffX, float fDiffY, int iPointIndex)
{
   switch (iPointIndex)
   {
   case 0:
      m_dX += fDiffX;
      m_dY += fDiffY;
      m_dWidth -= fDiffX;
      m_dHeight -= fDiffY;
      break;
   case 1:
      m_dY += fDiffY;
      m_dHeight -= fDiffY;
      break;
   case 2:
      m_dY += fDiffY;
      m_dWidth += fDiffX;
      m_dHeight -= fDiffY;
      break;
   case 3:
      m_dWidth += fDiffX;
      break;
   case 4:
      m_dWidth += fDiffX;
      m_dHeight += fDiffY;
      break;
   case 5:
      m_dHeight += fDiffY;
      break;
   case 6:
      m_dX += fDiffX;
      m_dWidth -= fDiffX;
      m_dHeight += fDiffY;
      break;
   case 7:
      m_dX += fDiffX;
      m_dWidth -= fDiffX;
      break;
   case 8:
      m_dX += fDiffX;
      m_dY += fDiffY;
      m_dWidth  -= fDiffX;
      m_dHeight -= fDiffY;
      break;
   case 9:
      m_dWidth  += fDiffX;
      m_dHeight += fDiffY;
      break;
   }
}

ASSISTANT::DrawObject *ASSISTANT::Line::Load(SGMLElement *hilf) 
{
   Line 
      *obj;
   TCHAR
      *tmp;
   int
      x, y,
		zPosition,
      width, height,
      lineWidth;
   TCHAR 
      *color,
      *lineStyle,
      *capStyle,
      *joinStyle,
      *arrowStyle,
      *hyperlink,
      *internLink,
      *currentDirectory,
      *linkedObjects;
   int
      iArrowConfig,
      iPPTId;
   
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
   
   tmp = hilf->GetAttribute("lineWidth");
   if (tmp) lineWidth = _ttoi(tmp);
   else     lineWidth = 2;
   
   tmp = hilf->GetAttribute("lineStyle");
   if (tmp) lineStyle = _tcsdup(tmp);
   else     lineStyle = _tcsdup(_T(" "));
   
   tmp = hilf->GetAttribute("capStyle");
   if (tmp) capStyle = _tcsdup(tmp);
   else     capStyle = _tcsdup(_T("round"));
   
   tmp = hilf->GetAttribute("joinStyle");
   if (tmp) joinStyle = _tcsdup(tmp);
   else     joinStyle = _tcsdup(_T("round"));
   
   tmp = hilf->GetAttribute("arrowStyle");
   if (tmp) arrowStyle = _tcsdup(tmp);
   else     arrowStyle = _tcsdup(_T("none"));
   
   tmp = hilf->GetAttribute("arrowConfig");
   if (tmp) iArrowConfig = _ttoi(tmp);
   else     iArrowConfig = 0;
   
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
      obj = new Line(x, y, width, height, zPosition, color, lineWidth, lineStyle,
         capStyle, joinStyle, arrowStyle, iArrowConfig,
         0, hyperlink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new Line(x, y, width, height, zPosition, color, lineWidth, lineStyle,
         capStyle, joinStyle, arrowStyle, iArrowConfig,
         0, internLink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(true);
   }
   
   if (obj && iPPTId > 0)
      obj->SetPPTObjectId(iPPTId);
   
   delete color;
   delete lineStyle;
   delete capStyle;
   delete joinStyle;
   delete arrowStyle;
   
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

bool ASSISTANT::Line::IsIdentic(ASSISTANT::DrawObject *obj)
{
   // the same object is identic
   if (obj == this)
      return true;

   if (!ColoredObject::IsIdentic(obj))
      return false;

   if (GetCapStyle() != ((Line *)obj)->GetCapStyle())
      return false;

   if (GetJoinStyle() != ((Line *)obj)->GetJoinStyle())
      return false;
   
   if (GetArrowStyle() != ((Line *)obj)->GetArrowStyle())
      return false;
   
   if (GetArrowConfig() != ((Line *)obj)->GetArrowConfig())
      return false;

   // all parameters are identic
   return true;
}
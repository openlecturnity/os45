/*********************************************************************

  program : mlb_polyobjects.cpp
  authors : Gabriela Maass
  date    : 3.12.1999
  desc    : Functions to draw and manipulate objects on a tcl-canvas
  
**********************************************************************/

#include "StdAfx.h"

#include "PageObject.h"
#include "DrawObjects.h"


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

ASSISTANT::PolyObject::PolyObject(double _x, double _y, double _width, double _height, int _zPosition, 
                                  LPCTSTR _color, LPCTSTR _fillColor, int _lineWidth, LPCTSTR _lineStyle, UINT _id, 
                                  LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                                  : ColoredObject(_x, _y, _width, _height, _zPosition, _color, _fillColor, _lineWidth, _lineStyle, _id, _hyperlink, _currentDirectory, _linkedObjects) 
{
   selectedPoint = 0;
   isFreehand = false;
}

void ASSISTANT::PolyObject::SetX(double _x)
{
   double diffX = _x - m_dX;
   
   m_dX = _x;
   for (int i = 0; i < points.GetSize(); ++i) 
   {
      points[i]->X += (Gdiplus::REAL)diffX;
   }
}

void ASSISTANT::PolyObject::SetY(double _y)
{
   double diffY = _y - m_dY;
   
   m_dY = _y;
   for (int i = 0; i < points.GetSize(); ++i) 
   {
      points[i]->Y +=(Gdiplus::REAL) diffY;
   }
}

void ASSISTANT::PolyObject::AppendNewPoint(int _x, int _y)
{
   if (!visible) 
      return;
   Gdiplus::PointF
      *pt;
   int
      x2, y2;
   
   if (_x < 0)
      _x = 0;
   if (_y < 0)
      _y = 0;

   x2 = (int)(m_dX + m_dWidth);
   y2 = (int)(m_dY + m_dHeight);
   if (_x < m_dX) 
      m_dX = _x;
   if (_y < m_dY) 
      m_dY = _y;
   if (_x > x2) 
      x2 = _x;
   if (_y > y2) 
      y2 = _y;
   
   m_dWidth  = x2 - m_dX;
   m_dHeight = y2 - m_dY;
   
   pt = new Gdiplus::PointF((Gdiplus::REAL)_x, (Gdiplus::REAL)_y);
   
   points.Add(pt);
}  

void ASSISTANT::PolyObject::DeleteSegments()
{
   if (!visible) 
      return;
   
   //_canvas->DeleteLineSegments(tkref);
}

CArray<Gdiplus::PointF *, Gdiplus::PointF *> *ASSISTANT::PolyObject::GetPoints()
{
   return &points;
}

bool ASSISTANT::PolyObject::IsIdentic(DrawObject *obj)
{
   // the same object is identic
   if (obj == this)
      return true;

   if (!ColoredObject::IsIdentic(obj))
      return false;

   if (points.GetSize() != ((PolyObject *)obj)->points.GetSize())
      return false;

   for (int i = 0; i < points.GetSize(); ++i)
   {
           if (points[i]->X != ((PolyObject *)obj)->points[i]->X || 
               points[i]->Y != ((PolyObject *)obj)->points[i]->Y)
              return false;
   }

   // all parameters are identic
   return true;
}

bool ASSISTANT::PolyObject::AllPointsEqual()
{
   if (points.GetSize() < 2)
      return true;

   int x = (int)points[0]->X;
   int y = (int)points[0]->Y;
   for (int i = 1; i < points.GetSize(); ++i)
   {
      if (points[i]->X != x || points[i]->Y != y)
         return false;
   }
   return true;
}

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/


ASSISTANT::PolyLine::PolyLine(double _x, double _y, double _width, double _height, int _zPosition, 
                              LPCTSTR _color, int _lineWidth, LPCTSTR _lineStyle, LPCTSTR _capStyle, LPCTSTR _joinStyle, 
                              LPCTSTR _arrowStyle, int _arrowConfig, bool _smooth, UINT _id, 
                              LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects, bool _isFreehand, bool _isPencil)
                              : PolyObject(_x, _y, _width, _height, _zPosition, _color, _T("none"), _lineWidth, _lineStyle, _id, _hyperlink, _currentDirectory, _linkedObjects) 
{
   
   CString 
      zahl;
   smooth = _smooth;
   
   capStyle    = _capStyle;
   joinStyle   = _joinStyle;
   arrowStyle  = _arrowStyle;
   arrowConfig = _arrowConfig;
   ComputeArrowConfig(arrowConfig);
   
   isFreehand = _isFreehand;
   isPencil = _isPencil;
}

ASSISTANT::PolyLine::~PolyLine() 
{
   for (int i = 0; i < points.GetSize(); ++i) 
   {
      delete points[i];
   }
   points.RemoveAll();
}

int ASSISTANT::PolyLine::GetType() 
{
   if (isFreehand)
      return FREEHAND;
   else 
      return POLYLINE;
   
}


ASSISTANT::DrawObject *ASSISTANT::PolyLine::Copy(bool keepZPosition)
{
   ASSISTANT::PolyLine
      *ret;
   
	ret = new PolyLine(m_dX, m_dY, m_dWidth, m_dHeight, -1,
		                color, lineWidth, lineStyle,
		                capStyle, joinStyle, arrowStyle, arrowConfig, 
		                smooth, 0, hyperlink_, currentDirectory_, m_ssLinkedObjects);
	
  if (isInternLink)
     ret->LinkIsIntern(true);
  else
     ret->LinkIsIntern(false);

	if (keepZPosition)
		ret->SetOrder(m_fOrder);

   for (int i = 0; i < points.GetSize(); ++i) 
   {
      ret->AppendNewPoint((int)points[i]->X, (int)points[i]->Y);
   }
   
   return (DrawObject *)ret;
}

void ASSISTANT::PolyLine::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) 
      return;
   
   if (points.GetSize() == 0)
      return;
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform((Gdiplus::REAL)dOffX, (Gdiplus::REAL)dOffY);
   bool drawArrows = true;
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);
   if (lineWidth * _zoomFactor < 0.5)
      drawArrows = false;

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
      if (isFreehand)
      {
         graphics.FillEllipse(&brush, (Gdiplus::REAL)(zPts[0].X-iHalfLineWidth), (Gdiplus::REAL)(zPts[0].Y-iHalfLineWidth), 
                                      (Gdiplus::REAL)(lineWidth), (Gdiplus::REAL)(lineWidth));
      }
      else
      {
         graphics.FillRectangle(&brush, (Gdiplus::REAL)(zPts[0].X-iHalfLineWidth), (Gdiplus::REAL)(zPts[0].Y-iHalfLineWidth), 
                                        (Gdiplus::REAL)(lineWidth), (Gdiplus::REAL)(lineWidth));
      }
      delete zPts;
      return;
   }
   
   Gdiplus::Color lineColor(m_argbLineColor);
   Gdiplus::Pen pen(lineColor, (Gdiplus::REAL)lineWidth);
   
   BYTE a = lineColor.GetA();
   BYTE r = lineColor.GetR();
   BYTE g = lineColor.GetG();
   BYTE b = lineColor.GetB();
   if (isFreehand)
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
   
   graphics.DrawLines(&pen, zPts, points.GetSize());
   
   delete zPts;
}

void ASSISTANT::PolyLine::Hide()
{
   if (!visible) 
      return;
   
   //canvas->DeleteObject(tkref);
   //canvas->DeleteLineSegments(tkref);
   visible = false;
   
}

void ASSISTANT::PolyLine::GetLastPointsRect(CRect &rcLastPoints)
{
   rcLastPoints.SetRect(0, 0, 0, 0);

   if (points.GetSize() > 0)
   {
      double dBorder = (double)(lineWidth+0.5) / 2;
      if (points.GetSize() == 1)
         rcLastPoints.SetRect((int)(points[0]->X - dBorder), (int)(points[0]->Y - dBorder), 
                              (int)(points[0]->X + dBorder), (int)(points[0]->Y + dBorder)); 
      else
      {
         CPoint ptLast((int)points[points.GetSize() - 1]->X, (int)points[points.GetSize() - 1]->Y);
         CPoint ptPreLast((int)points[points.GetSize() - 2]->X, (int)points[points.GetSize() - 2]->Y);
         rcLastPoints.SetRect(ptLast, ptPreLast);
         rcLastPoints.NormalizeRect();
         rcLastPoints.InflateRect(lineWidth+1, lineWidth+1); 
      }
   }

}

ASSISTANT::DrawObject *ASSISTANT::PolyLine::Load(SGMLElement *hilf) 
{
   PolyLine
      *obj;
   TCHAR
      *tmp,
      *p;
   int
      x, y,
      width, height,
		zPosition,
      lineWidth,
      iPPTId;
   bool
      bIsFreehand;
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
      arrowConfig;
   
   if (hilf->Parameter.GetLength() == 0) 
      return NULL;
   
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
   if (tmp) arrowConfig = _ttoi(tmp);
   else     arrowConfig = 0;

   tmp = hilf->GetAttribute("address");
   if (tmp) hyperlink = _tcsdup(tmp);
   else     hyperlink = NULL;
   
   tmp = hilf->GetAttribute("intern");
   if (tmp) internLink = _tcsdup(tmp);
   else     internLink = NULL;
   
   tmp = hilf->GetAttribute("linkedObjects");
   if (tmp)
      linkedObjects = _tcsdup(tmp);
   else
      linkedObjects = NULL;

   tmp = hilf->GetAttribute("id");
   if (tmp) iPPTId = _ttoi(tmp);
   else     iPPTId = -1;
   
   tmp = hilf->GetAttribute("path");
   if (tmp) currentDirectory = _tcsdup(tmp);
   else     currentDirectory = NULL;
   
   tmp = hilf->GetAttribute("ZPosition");
   if (tmp) zPosition = _ttoi(tmp);
   else     zPosition = -1;

   tmp = hilf->GetAttribute("freehand");
   if (tmp) bIsFreehand = _ttoi(tmp) == 0 ? false: true;
   else     bIsFreehand = false;


   if (hyperlink)
   {
      obj = new ASSISTANT::PolyLine(x, y, width, height, zPosition, color, lineWidth, lineStyle,
         capStyle, joinStyle, arrowStyle, arrowConfig, 
         false, 0, hyperlink, currentDirectory, linkedObjects, bIsFreehand);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new ASSISTANT::PolyLine(x, y, width, height, zPosition, color, lineWidth, lineStyle,
         capStyle, joinStyle, arrowStyle, arrowConfig,
         false, 0, internLink, currentDirectory, linkedObjects, bIsFreehand);
      obj->LinkIsIntern(true);
   }
   
   TCHAR *zwParameter = (TCHAR *)malloc((hilf->Parameter.GetLength()+1)*sizeof(TCHAR));
   _tcsncpy(zwParameter, hilf->Parameter, hilf->Parameter.GetLength());
   zwParameter[hilf->Parameter.GetLength()] = '\0';
   p = zwParameter;
   while (*p != _T('\0')) {
      while ((*p != _T('{')) && (*p!=_T('\0'))) p++;
      if (*p==_T('\0')) break;
      p++;
      tmp = p;
      while (*p != _T(' ')) p++;
      *p = _T('\0');
      x = _ttoi(tmp);
      p++;
      tmp = p;
      while (*p != _T('}')) p++;
      *p = _T('\0');
      y = _ttoi(tmp);
      obj->AppendNewPoint(x,y);
      p++;
   }
   delete zwParameter;

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


bool ASSISTANT::PolyLine::IsIdentic(DrawObject *obj)
{
   // the same object is identic
   if (obj == this)
      return true;

   if (!PolyObject::IsIdentic(obj))
      return false;

   if (GetCapStyle() != ((PolyLine *)obj)->GetCapStyle())
      return false;
   
   if (GetJoinStyle() != ((PolyLine *)obj)->GetJoinStyle())
      return false;
   
   if (GetArrowStyle() != ((PolyLine *)obj)->GetArrowStyle())
      return false;
   
   if (GetArrowConfig() != ((PolyLine *)obj)->GetArrowConfig())
      return false;

   // all parameters are identic
   return true;
}

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/



ASSISTANT::Polygon::Polygon(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, 
                            LPCTSTR _fillColor, int _lineWidth, LPCTSTR _lineStyle, UINT _id, 
                            LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                            : PolyObject(_x,_y,_width,_height, _zPosition,_color,_fillColor,_lineWidth,_lineStyle, _id, _hyperlink, _currentDirectory, _linkedObjects) 
{
}

ASSISTANT::Polygon::~Polygon() 
{
   for (int i = 0; i < points.GetSize(); ++i) 
   {
      delete points[i];
   }
   points.RemoveAll();
}

ASSISTANT::DrawObject *ASSISTANT::Polygon::Copy(bool keepZPosition)
{
   ASSISTANT::Polygon
      *ret;
   
	ret = new Polygon(m_dX, m_dY, m_dWidth, m_dHeight, -1,
		color, fillColor, lineWidth,
		lineStyle, 0, hyperlink_, currentDirectory_, m_ssLinkedObjects);
	
  if (isInternLink)
     ret->LinkIsIntern(true);
  else
     ret->LinkIsIntern(false);

	if (keepZPosition)
		ret->SetOrder(m_fOrder);
	
   for (int i = 0; i < points.GetSize(); ++i) 
      ret->AppendNewPoint((int)points[i]->X, (int)points[i]->Y);
   
   return (DrawObject *)ret;
}

void ASSISTANT::Polygon::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) 
      return;
   
   if (points.GetSize() == 0)
      return;

   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform((Gdiplus::REAL)dOffX, (Gdiplus::REAL)dOffY);
   
   bool drawArrows = true;
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);
   if (lineWidth * _zoomFactor < 0.5)
      drawArrows = false;

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
      if (isFreehand)
      {
         graphics.FillEllipse(&brush, (Gdiplus::REAL)(zPts[0].X-iHalfLineWidth), (Gdiplus::REAL)(zPts[0].Y-iHalfLineWidth), 
                                      (Gdiplus::REAL)(lineWidth), (Gdiplus::REAL)(lineWidth));
      }
      else
      {
         graphics.FillRectangle(&brush, (Gdiplus::REAL)(zPts[0].X-iHalfLineWidth), (Gdiplus::REAL)(zPts[0].Y-iHalfLineWidth), 
                                        (Gdiplus::REAL)(lineWidth), (Gdiplus::REAL)(lineWidth));
      }
      delete zPts;
      return;
   }
   
   if (fillColor != _T("none"))
   {
      Gdiplus::Color color(m_argbFillColor);
      Gdiplus::SolidBrush brush(color);
      graphics.FillPolygon(&brush, zPts, points.GetSize(), Gdiplus::FillModeWinding);
   }

   
   Gdiplus::Color lineColor(m_argbLineColor);
   Gdiplus::Pen pen(lineColor, (Gdiplus::REAL)lineWidth);
   
   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
   pen.SetLineJoin(Gdiplus::LineJoinMiter);
   pen.SetDashStyle(m_gdipDashStyle);
   graphics.DrawPolygon(&pen, zPts, points.GetSize());
   
   
   delete zPts;
}

ASSISTANT::DrawObject *ASSISTANT::Polygon::Load(SGMLElement *hilf) 
{
   Polygon
      *obj;
   TCHAR
      *tmp,
      *p;
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
   
   if (hilf->Parameter.GetLength() == 0) 
      return NULL;
   
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
   if (tmp)
      linkedObjects = _tcsdup(tmp);
   else
      linkedObjects = NULL;
   
   tmp = hilf->GetAttribute("id");
   if (tmp) iPPTId = _ttoi(tmp);
   else     iPPTId = -1;
   
   tmp = hilf->GetAttribute("ZPosition");
   if (tmp) zPosition = _ttoi(tmp);
   else     zPosition = -1;
   

   if (hyperlink != NULL)
   {
      obj = new ASSISTANT::Polygon(x,y,width,height,zPosition, color,fillColor,lineWidth,lineStyle,
         0, hyperlink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new ASSISTANT::Polygon(x,y,width,height,zPosition,color,fillColor,lineWidth,lineStyle,
         0, internLink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(true);
   }
   
   TCHAR *zwParameter = (TCHAR *)malloc((hilf->Parameter.GetLength()+1) * sizeof(TCHAR));
   _tcsncpy(zwParameter, hilf->Parameter, hilf->Parameter.GetLength());
   zwParameter[hilf->Parameter.GetLength()] = '\0';
   p = zwParameter;
   while (*p != _T('\0')) {
      while ((*p != _T('{')) && (*p!=_T('\0'))) p++;
      if (*p==_T('\0')) break;
      p++;
      tmp = p;
      while (*p != _T(' ')) p++;
      *p = _T('\0');
      x = _ttoi(tmp);
      p++;
      tmp = p;
      while (*p != _T('}')) p++;
      *p = _T('\0');
      y = _ttoi(tmp);
      obj->AppendNewPoint(x,y);
      p++;
   }
   delete zwParameter;

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


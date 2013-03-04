/*********************************************************************

  program : mlb_polyobjects.cpp
  authors : Gabriela Maass
  date    : 3.12.1999
  desc    : Functions to draw and manipulate objects on a tcl-canvas
  
**********************************************************************/

#include "StdAfx.h"

#include "mlb_page.h"
#include "la_project.h"
#include "mlb_objects.h"
#include "mlb_misc.h"


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

ASSISTANT::PolyObject::PolyObject(double _x, double _y, double _width, double _height, int _zPosition, 
                                  Gdiplus::ARGB argbLineColor, Gdiplus::ARGB argbFillColor,
                                  int _lineWidth, Gdiplus::DashStyle gdipLineStyle, uint32 _id, 
                                  LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                                  : ColoredObject(_x, _y, _width, _height, _zPosition, 
                                                  argbLineColor, argbFillColor, _lineWidth, gdipLineStyle, _id, 
                                                  _hyperlink, _currentDirectory, _linkedObjects) 
{
   selectedPoint = 0;
   isFreehand = false;
}

void ASSISTANT::PolyObject::Move(float _diffX, float _diffY)
{
   if (!visible) 
      return;
   
   recorded = false;
   
   m_dX += _diffX;
   m_dY += _diffY;
   
   for (int i = 0; i < points.GetSize(); ++i)
   {
      points[i]->x += _diffX;
      points[i]->y += _diffY;
   }
} 

void ASSISTANT::PolyObject::SetX(double _x)
{
   double diffX = _x - m_dX;
   
   m_dX = _x;
   for (int i = 0; i < points.GetSize(); ++i) 
   {
      points[i]->x += diffX;
   }
}

void ASSISTANT::PolyObject::SetY(double _y)
{
   double diffY = _y - m_dY;
   
   m_dY = _y;
   for (int i = 0; i < points.GetSize(); ++i) 
   {
      points[i]->y += diffY;
   }
}

void ASSISTANT::PolyObject::DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) 
      return;
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   double dOffset = 3 / _zoomFactor;
   
   
   Gdiplus::Color clrWhite(255, 255, 255, 255);
   Gdiplus::Color clrBlack(255, 0, 0, 0);

   Gdiplus::REAL fLineWidth = 6.0 / _zoomFactor;
   Gdiplus::REAL fRadius = 3.0 / _zoomFactor;

   Gdiplus::REAL fPenWidth = 0.5f;
   Gdiplus::Pen blackPen(clrBlack, fPenWidth);
   Gdiplus::SolidBrush whiteBrush(clrWhite);
   
   for (int i = 0; i < points.GetSize(); ++i)  
   {
      graphics.FillEllipse(&whiteBrush, 
         (Gdiplus::REAL)(points[i]->x-dOffset), (Gdiplus::REAL)(points[i]->y-dOffset), 
         (Gdiplus::REAL)(2*dOffset), (Gdiplus::REAL)(2*dOffset));
      graphics.DrawEllipse(&blackPen, 
         (Gdiplus::REAL)(points[i]->x-dOffset), (Gdiplus::REAL)(points[i]->y-dOffset), 
         (Gdiplus::REAL)(2*dOffset), (Gdiplus::REAL)(2*dOffset));
   }
}

bool ASSISTANT::PolyObject::IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditType, int &iPointIndex, double dZoomFactor)
{
   if (!visible || !selected) 
      return false;
   
   double dTolerance = 3 / dZoomFactor;

   nEditType = 0;
   iPointIndex = -1;

   /*/
   for (int i = 0; i < points.GetSize(); ++i) 
   {
      if ((ptObject.X > points[i]->x-dTolerance) && (ptObject.X < points[i]->x+dTolerance)) 
      {
         if ((ptObject.Y > points[i]->y-dTolerance) && (ptObject.Y < points[i]->y+dTolerance)) 
         {
            nEditType = MOVE;
            iPointIndex = i;
            return true;
         }
      }
   }
   /*/
   
   if (ContainsPoint(ptObject))
   {
      nEditType = MOVE;
      return true;
   }

   return false;
}

void ASSISTANT::PolyObject::MovePoint( int point, int _x, int _y, float _zoomFactor) 
{
   
   if (selectedPoint==NULL || !visible) 
      return;
   int
      x1, y1,
      x2, y2;
   
   recorded = false;
   
   points[selectedPoint]->x = _x;
   points[selectedPoint]->y = _y;
   
   x1 = x2 = _x;
   y1 = y2 = _y;
   for (int i = 0; i < points.GetSize(); ++i) 
   {
      if (points[i]->x < x1) 
         x1 = points[i]->x;
      if (points[i]->y < y1) 
         y1 = points[i]->y;
      if (points[i]->x > x2) 
         x2 = points[i]->x;
      if (points[i]->y > y2) 
         y2 = points[i]->y;
   }
   
   m_dX = x1;
   m_dY = y1;
   m_dWidth  = x2-x1;
   m_dHeight = y2-y1;
   
   /*/
   ReDraw(_zoomFactor); 
   /*/
}


void ASSISTANT::PolyObject::AppendNewPoint(double _x, double _y)
{
   if (!visible) 
      return;
   Point
      *pt;
   double
      x2, y2;
   
   if (_x < 0)
      _x = 0;
   if (_y < 0)
      _y = 0;

   x2 = m_dX + m_dWidth;
   y2 = m_dY + m_dHeight;
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
   
   pt = new Point(_x, _y);
   
   points.Add(pt);
}  

void ASSISTANT::PolyObject::DrawNewSegment(CView *pView, UINT nOffX, UINT nOffY, float _zoomFactor, 
                                           CPoint &ptNew, bool bInitialize)
{
   if (points.GetSize() == 0) 
      return;

   static CPoint ptLast;
   
   if (bInitialize)
   {
      ptLast.x = -1;
      ptLast.y = -1;
      return;
   }
   int iLastIndex = points.GetUpperBound();
   int x1 = ((double)points[iLastIndex]->x * _zoomFactor) + nOffX;
   int y1 = ((double)points[iLastIndex]->y * _zoomFactor) + nOffY;
   
   int zLineWidth = ASSISTANT::CalculateZoomedLineWidth(lineWidth, _zoomFactor);
   
   if (ptLast.x >= 0 && ptLast.y >= 0)
   {
      CRect rcRedraw(x1, y1, ptLast.x, ptLast.y);
      int iOffset = (zLineWidth+0.5) * 2;
      rcRedraw.NormalizeRect();
      rcRedraw.InflateRect(iOffset, iOffset, iOffset, iOffset);
      pView->RedrawWindow(&rcRedraw);
   }
   
   
   CDC *pDC = pView->GetDC();
   
   int x2 = ptNew.x;
   int y2 = ptNew.y;
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   
   Gdiplus::Color color(m_argbLineColor);
   Gdiplus::Pen pen(color, zLineWidth);
   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
   pen.SetDashStyle(m_gdipLineStyle);

   graphics.DrawLine(&pen, x1, y1, x2, y2);
   
   
   pView->ReleaseDC(pDC);
   
   ptLast = ptNew;
}

void ASSISTANT::PolyObject::Redraw(CView *pView, UINT nOffX, UINT nOffY, float _zoomFactor)
{
   if (points.GetSize() == 0) 
      return;

   
   CRect rcObject;
   GetBoundingBox(rcObject);

   rcObject.left = ((double)rcObject.left * _zoomFactor) + nOffX;
   rcObject.top = ((double)rcObject.top * _zoomFactor) + nOffY;
   rcObject.right = ((double)rcObject.right * _zoomFactor) + nOffX;
   rcObject.bottom = ((double)rcObject.bottom * _zoomFactor) + nOffY;
   
   int zLineWidth = ASSISTANT::CalculateZoomedLineWidth(lineWidth, _zoomFactor);
   
   int iOffset = (zLineWidth+0.5) * 2;
   rcObject.NormalizeRect();
   rcObject.InflateRect(iOffset, iOffset, iOffset, iOffset);
   pView->RedrawWindow(&rcObject);


   CDC *pDC = pView->GetDC();

   Draw(pDC, _zoomFactor, nOffX, nOffY);

   pView->ReleaseDC(pDC);
   
}

void ASSISTANT::PolyObject::DeleteSegments()
{
   if (!visible) 
      return;
   
   //_canvas->DeleteLineSegments(tkref);
}

bool ASSISTANT::PolyObject::Overlap(int _x, int _y, float _zoomFactor)
{
   if (!visible) 
      return false;
   
   int
      x1, y1,
      x2, y2;
   
   GetBBox(&x1,&y1,&x2,&y2);
   if ((_x < x1*_zoomFactor) || (_y < y1*_zoomFactor) || (_x > x2*_zoomFactor) || (_y > y2*_zoomFactor)) 
      return false;
   
   return true; //_canvas->Overlap(tkref,_x,_y);
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
           if (points[i]->x != ((PolyObject *)obj)->points[i]->x || 
               points[i]->y != ((PolyObject *)obj)->points[i]->y)
              return false;
   }

   // all parameters are identic
   return true;
}

bool ASSISTANT::PolyObject::AllPointsEqual()
{
   if (points.GetSize() < 2)
      return true;

   int x = points[0]->x;
   int y = points[0]->y;
   for (int i = 1; i < points.GetSize(); ++i)
   {
      if (points[i]->x != x || points[i]->y != y)
         return false;
   }
   return true;
}

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/


ASSISTANT::PolyLine::PolyLine(double _x, double _y, double _width, double _height, int _zPosition, 
                              Gdiplus::ARGB argbLineColor, int _lineWidth, Gdiplus::DashStyle gdipLineStyle, 
                              LPCTSTR _arrowStyle, int _arrowConfig, uint32 _id, 
                              LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects, 
                              bool _isFreehand, bool _isPencil)
                              : PolyObject(_x, _y, _width, _height, _zPosition, argbLineColor, 0, 
                                           _lineWidth, gdipLineStyle, _id, 
                                           _hyperlink, _currentDirectory, _linkedObjects) 
{
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
      return GenericObject::FREEHAND;
   else 
      return GenericObject::POLYLINE;
   
}


ASSISTANT::DrawObject *ASSISTANT::PolyLine::Copy(bool keepZPosition)
{
   ASSISTANT::PolyLine
      *ret;
   
	ret = new PolyLine(m_dX, m_dY, m_dWidth, m_dHeight, -1,
		                m_argbLineColor, lineWidth, m_gdipLineStyle,
		                arrowStyle, arrowConfig, Project::active->GetObjectID(), 
                      hyperlink_, currentDirectory_, m_ssLinkedObjects);
	
  if (isInternLink)
     ret->LinkIsIntern(true);
  else
     ret->LinkIsIntern(false);

	if (keepZPosition)
		ret->SetOrder(order);

   for (int i = 0; i < points.GetSize(); ++i) 
   {
      ret->AppendNewPoint(points[i]->x,points[i]->y);
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
   graphics.TranslateTransform(dOffX, dOffY);
   bool drawArrows = true;
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);
   if (lineWidth * _zoomFactor < 0.5)
      drawArrows = false;

   Gdiplus::PointF *zPts = (Gdiplus::PointF *)malloc(points.GetSize()*sizeof(Gdiplus::PointF));
   for (int i = 0; i < points.GetSize(); ++i)
   {
      zPts[i].X = points[i]->x;
      zPts[i].Y = points[i]->y;
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
      free(zPts);
      return;
   }
   
   Gdiplus::Color lineColor(m_argbLineColor);
   Gdiplus::Pen pen(lineColor, lineWidth);
   
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
      pen.SetDashStyle(m_gdipLineStyle);
   }
   
   graphics.DrawLines(&pen, zPts, points.GetSize());
   
   free(zPts);
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
         rcLastPoints.SetRect(points[0]->x - dBorder, points[0]->y - dBorder, 
                              points[0]->x + dBorder, points[0]->y + dBorder); 
      else
      {
         CPoint ptLast(points[points.GetSize() - 1]->x, points[points.GetSize() - 1]->y);
         CPoint ptPreLast(points[points.GetSize() - 2]->x, points[points.GetSize() - 2]->y);
         rcLastPoints.SetRect(ptLast, ptPreLast);
         rcLastPoints.NormalizeRect();
         rcLastPoints.InflateRect(lineWidth+1, lineWidth+1); 
      }
   }

}

void ASSISTANT::PolyLine::GetUpdateRect(CRect &rcObject)
{
   rcObject.SetRect(0, 0, 0, 0);

   if (points.GetSize() > 0)
   {
      rcObject.left = (int)m_dX;
      rcObject.right = (int)(m_dX + m_dWidth);
      rcObject.top = (int)m_dY;
      rcObject.bottom = (int)(m_dY + m_dHeight);

      rcObject.NormalizeRect();

      rcObject.InflateRect(lineWidth, lineWidth);
   }

}

void ASSISTANT::PolyLine::SaveObject(FILE *fp, int level, LPCTSTR path)
{
   if (!visible) 
      return;
   
   WCHAR *tab = (WCHAR *)malloc((level*2+1)*sizeof(WCHAR));
   *tab = L'\0';
   for (int i = 0; i < level; i++) 
      wcscat(tab, L"  ");
   
   CString ptBuf;
   TCHAR buf[20];
   for (i = 0; i < points.GetSize(); ++i)
   {
      _stprintf(buf, _T("{%d %d} "),(int)points[i]->x, (int)points[i]->y);
      ptBuf += buf;
   }
    
   CString hyperlinkString;
   MakeHyperlinkString(hyperlinkString);
   WCHAR *wszHyperlink = ASSISTANT::ConvertTCharToWChar(hyperlinkString);
   
   CString csColor;
   ASSISTANT::GdipARGBToString(m_argbLineColor, csColor);
   WCHAR *wszLineColor = ASSISTANT::ConvertTCharToWChar(csColor);

   CString csLineStyle;
   ASSISTANT::GdipLineStyleToString(m_gdipLineStyle, csLineStyle);
   WCHAR *wszLineStyle = ASSISTANT::ConvertTCharToWChar(csLineStyle);


   if (arrowStyle != "none") 
   {
      fwprintf(fp, L"%s<POLYLINE x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" color=\"%s\" lineWidth=\"%d\" lineStyle=\"%s\" length=\"%d\" arrowStyle=\"%s\" arrowConfig=\"%d\" ZPosition=\"%d\"%s>\n",
                   tab, (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight,
                   wszLineColor, lineWidth, wszLineStyle, ptBuf.GetLength(),
                   arrowStyle, arrowConfig, (int)order, hyperlinkString);
   }
   else 
   {
      if (isFreehand)
      {
         fwprintf(fp, L"%s<POLYLINE x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" color=\"%s\" lineWidth=\"%d\" lineStyle=\"%s\" length=\"%d\" freehand=\"%d\" ZPosition=\"%d\"%s>\n",
                      tab, (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight,
                      wszLineColor, lineWidth, wszLineStyle, ptBuf.GetLength(), 
                      isFreehand, (int)order, hyperlinkString);
      }
      else
      {
         fwprintf(fp, L"%s<POLYLINE x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" color=\"%s\" lineWidth=\"%d\" lineStyle=\"%s\" length=\"%d\" ZPosition=\"%d\"%s>\n",
                      tab, (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight,
                      wszLineColor, lineWidth, wszLineStyle, ptBuf.GetLength(), 
                      (int)order, hyperlinkString);
      }
   }
   
   
   if (wszHyperlink)
      delete wszHyperlink;

   if (wszLineColor)
      delete wszLineColor;

   if (wszLineStyle)
      delete wszLineStyle;

   fwprintf(fp, L"%s  \"",tab);

   WCHAR *wszPtBuf = ASSISTANT::ConvertTCharToWChar(ptBuf);
   fwrite(wszPtBuf, sizeof(WCHAR), wcslen(wszPtBuf), fp);
   if (wszPtBuf)
      delete wszPtBuf;

   fwprintf(fp, L"\"\n%s</POLYLINE>\n", tab);
   
   ptBuf.Empty();
   if (tab)
      delete tab;
}

int ASSISTANT::PolyLine::RecordableObjects()
{
   if (!visible || (points.GetSize() < 1))
   {
      visible = false;
      return 0;
   }
   
   if (arrowStyle == _T("none") || isFreehand) 
      return 1;
   
   if (arrowStyle == _T("first") || arrowStyle == _T("last")) 
   {
      if (points.GetSize() <= 2)
         return 1;
      else
         return 2;
   }
   
   if (arrowStyle == _T("both")) 
   {
      if (points.GetSize() <= 2)
         return 1;
      else
         return 3;
   }
   
   return 1;
}

void ASSISTANT::PolyLine::SaveAsAOF(CFileOutput *fp)
{
   if (!visible || recorded || (points.GetSize() < 1)) 
      return;

   recorded = true;
   
   ///////////////////////////////////////////////////////////////////////////////////
   /////      !! Bei Änderungen immer den Record-Counter im Auge behalten !!     /////
   /////  Angaben in RecordableObjects() und Page::WriteObjectFile() überprüfen  /////
   ///////////////////////////////////////////////////////////////////////////////////


   recID = ASSISTANT::Project::active->recCount;
   
   // Calculate Color
   CString csColor;
   ASSISTANT::GdipARGBToObjString(m_argbLineColor, csColor);

   int iLineStyle = ASSISTANT::GdipLineStyleToObjStyle(m_gdipLineStyle);

   CString line;

   if (isFreehand) 
   {
      ASSISTANT::Project::active->recCount++;
      line.Format(_T("<FREEHAND x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f count=%d linewidth=%d linestyle=%d rgb=0x%s>\n"),
                  (int)m_dX ,(int)m_dY, (int)m_dWidth, (int)m_dHeight, 
                  m_dX, m_dY, m_dWidth, m_dHeight,
                  points.GetSize(), lineWidth, iLineStyle, csColor);
      fp->Append(line);

      for (int i = 0; i < points.GetSize(); ++i)
      {
         line.Format(_T("<pt x=%d y=%d xf=%2.2f yf=%2.2f></pt>\n"), 
                     (int)points[i]->x, (int)points[i]->y, 
                     points[i]->x, points[i]->y);
         fp->Append(line);
      }
      line = _T("</FREEHAND>\n");
      fp->Append(line);
   }
   else if (arrowStyle == _T("none")) 
   {
      ASSISTANT::Project::active->recCount++;
      line.Format(_T("<POLYLINES x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f count=%d linewidth=%d linestyle=%d rgb=0x%s>\n"),
                 (int)m_dX ,(int)m_dY, (int)m_dWidth, (int)m_dHeight, 
                  m_dX ,m_dY, m_dWidth, m_dHeight,
                 points.GetSize(), lineWidth, iLineStyle, csColor);
      fp->Append(line);

      for (int i = 0; i < points.GetSize(); ++i)
      {
         line.Format(_T("<pt x=%d y=%d xf=%2.2f yf=%2.2f></pt>\n"),
                     (int)points[i]->x, (int)points[i]->y, 
                     points[i]->x, points[i]->y);
         fp->Append(line);
      }
      line = _T("</POLYLINES>\n");
      fp->Append(line);
   }
   else if (arrowStyle == _T("first")) 
   {
      if (points.GetSize() == 1)
      {
         ASSISTANT::Project::active->recCount++;
         line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=1 arrowconfig=%d rgb=0x%s></LINE>\n"),
                     (int)m_dX, (int)m_dY, 0, 0,
                     m_dX, m_dY, (double)0.0, (double)0.0,
                     0, 0, lineWidth, iLineStyle, GetBeginArrowConfig(arrowConfig), csColor);
         fp->Append(line);
      }
      else
      {
         int dir = 0;
         int ori = 0;
         double dX = points[0]->x;
         double dY = points[0]->y;
         double dWidth = points[1]->x - dX;
         double dHeight = points[1]->y - dY;

         Line::CalculateLineParameters(dX, dY, dWidth, dHeight, ori, dir);
        
         if (dir) 
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=1 arrowconfig=%d rgb=0x%s></LINE>\n"),
                        (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                        dX, dY, dWidth, dHeight,
                        ori, dir, lineWidth, iLineStyle, 
                        SwapArrowConfig(GetBeginArrowConfig(arrowConfig)), csColor);
            fp->Append(line);
         }
         else  
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=2 arrowconfig=%d rgb=0x%s></LINE>\n"),
                       (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                        dX, dY, dWidth, dHeight,
                       ori, dir, lineWidth, iLineStyle, 
                       GetBeginArrowConfig(arrowConfig), csColor);
            fp->Append(line);
         }
         if (points.GetSize() > 2)
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<POLYLINES x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f count=%d linewidth=%d linestyle=%d polyArrowStyle=2 rgb=0x%s>\n"),
                       (int)m_dX ,(int)m_dY, (int)m_dWidth, (int)m_dHeight, 
                       m_dX ,m_dY, m_dWidth, m_dHeight,
                       points.GetSize()-1, lineWidth, iLineStyle, csColor);
            fp->Append(line);
            
            for (int i = 1; i < points.GetSize(); ++i)
            {
               line.Format(_T("<pt x=%d y=%d xf=%2.2f yf=%2.2f></pt>\n"), 
                           (int)points[i]->x, (int)points[i]->y, 
                           points[i]->x, points[i]->y);
               fp->Append(line);
            }
            line = _T("</POLYLINES>\n");
            fp->Append(line);
         } 
      }
   }
   else if (arrowStyle == _T("last")) 
   {
      if (points.GetSize() == 1)
      {
         ASSISTANT::Project::active->recCount++;
         line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=2 arrowconfig=%d rgb=0x%s></LINE>\n"),
                     (int)m_dX, (int)m_dY, 0, 0,
                     m_dX, m_dY, (double)0.0, (double)0.0,
                     0, 0, lineWidth, iLineStyle, 
                     GetEndArrowConfig(arrowConfig), csColor);
         fp->Append(line);
      }
      else
      {
         int dir = 0;
         int ori = 0;
         double dX = points[points.GetSize() - 2]->x;
         double dY = points[points.GetSize() - 2]->y;
         double dWidth = points[points.GetSize() - 1]->x - dX;
         double dHeight = points[points.GetSize() - 1]->y - dY;

         Line::CalculateLineParameters(dX, dY, dWidth, dHeight, ori, dir);

         if (dir) 
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=2 arrowconfig=%d rgb=0x%s></LINE>\n"),
                        (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                        dX, dY, dWidth, dHeight,
                        ori, dir, lineWidth, iLineStyle, 
                        SwapArrowConfig(GetEndArrowConfig(arrowConfig)), csColor);
            fp->Append(line);
         }
         else 
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=1 arrowconfig=%d rgb=0x%s></LINE>\n"),
                        (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                        dX, dY, dWidth, dHeight,
                        ori, dir, lineWidth, iLineStyle, 
                        GetEndArrowConfig(arrowConfig), csColor);
            fp->Append(line);
         }
         if (points.GetSize() > 2)
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<POLYLINES x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f count=%d linewidth=%d linestyle=%d polyArrowStyle=1 rgb=0x%s>\n"),
                        (int)m_dX ,(int)m_dY, (int)m_dWidth, (int)m_dHeight, 
                        m_dX, m_dY, m_dWidth, m_dHeight,
                        points.GetSize()-1, lineWidth, iLineStyle, csColor);
            fp->Append(line);
            for (int i = 0; i < points.GetSize()-1; ++i)
            {
               line.Format(_T("<pt x=%d y=%d xf=%2.2f yf=%2.2f></pt>\n"), 
                           (int)points[i]->x, (int)points[i]->y, 
                           points[i]->x, points[i]->y);
               fp->Append(line);
            }
            line = _T("</POLYLINES>\n");
            fp->Append(line);
         }
      }
   }
   else if (arrowStyle == _T("both")) 
   {
      if (points.GetSize() == 1)
      {
         ASSISTANT::Project::active->recCount++;
         line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=3 arrowconfig=%d rgb=0x%s></LINE>\n"),
                     (int)m_dX, (int)m_dY, 0, 0,
                     m_dX, m_dY, (double)0.0, (double)0.0,
                     0, 0, lineWidth, iLineStyle, arrowConfig, csColor);
         fp->Append(line);
      }
      else if (points.GetSize() == 2)
      {
         int dir = 0;
         int ori = 0;
         double dX = points[0]->x;
         double dY = points[0]->y;
         double dWidth = points[1]->x - dX;
         double dHeight = points[1]->y - dY;

         Line::CalculateLineParameters(dX, dY, dWidth, dHeight, ori, dir);

         if (dir) 
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=3 arrowconfig=%d rgb=0x%s></LINE>\n"),
                       (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                       dX, dY, dWidth, dHeight,
                       ori, dir, lineWidth, iLineStyle, 
                       SwapArrowConfig(arrowConfig), csColor);
            fp->Append(line);
         }
         else  
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=3 arrowconfig=%d rgb=0x%s></LINE>\n"),
                       (int)dX, (int)dY, (int)dWidth, (int)dHeight, 
                       dX, dY, dWidth, dHeight,
                       ori, dir, lineWidth, iLineStyle, 
                       arrowConfig, csColor);
            fp->Append(line);
         }
      }
      else
      {
         int dir = 0;
         int ori = 0;
         double dX = points[0]->x;
         double dY = points[0]->y;
         double dWidth = points[1]->x - dX;
         double dHeight = points[1]->y - dY;
         
         Line::CalculateLineParameters(dX, dY, dWidth, dHeight, ori, dir);

         if (dir) 
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=1 arrowconfig=%d rgb=0x%s></LINE>\n"),
                       (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                       dX, dY, dWidth, dHeight,
                       ori, dir, lineWidth, iLineStyle, 
                       SwapArrowConfig(GetBeginArrowConfig(arrowConfig)), csColor);
            fp->Append(line);
         }
         else  
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=2 arrowconfig=%d rgb=0x%s></LINE>\n"),
                        (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                        dX, dY, dWidth, dHeight,
                        ori, dir, lineWidth, iLineStyle, 
                        GetBeginArrowConfig(arrowConfig), csColor);
            fp->Append(line);
         }
         
         dir = 0;
         ori = 0;
         dX = points[points.GetSize() - 2]->x;
         dY = points[points.GetSize() - 2]->y;
         dWidth = points[points.GetSize() - 1]->x - dX;
         dHeight = points[points.GetSize() - 1]->y - dY;
        
         Line::CalculateLineParameters(dX, dY, dWidth, dHeight, ori, dir);

         if (dir) 
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=2 arrowconfig=%d rgb=0x%s></LINE>\n"),
                       (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                        dX, dY, dWidth, dHeight,
                        ori, dir, lineWidth, iLineStyle, 
                        SwapArrowConfig(GetEndArrowConfig(arrowConfig)), csColor);
            fp->Append(line);
         }
         else 
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=1 arrowconfig=%d rgb=0x%s></LINE>\n"),
                       (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                        dX, dY, dWidth, dHeight,
                       ori, dir, lineWidth, iLineStyle, 
                       GetEndArrowConfig(arrowConfig), csColor);
            fp->Append(line);
         }

         if (points.GetSize() > 3)
         {
            ASSISTANT::Project::active->recCount++;
            line.Format(_T("<POLYLINES x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f count=%d linewidth=%d linestyle=%d polyArrowStyle=3 rgb=0x%s>\n"),
                        (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight, 
                        m_dX, m_dY, m_dWidth, m_dHeight,
                        points.GetSize()-2, lineWidth, iLineStyle, csColor);
            fp->Append(line);
            
            for (int i = 1; i < points.GetSize()-1; ++i)
            {
               line.Format(_T("<pt x=%d y=%d xf=%2.2f yf=%2.2f></pt>\n"), 
                           (int)points[i]->x, (int)points[i]->y, 
                           points[i]->x, points[i]->y);
               fp->Append(line);
            }
            line = _T("</POLYLINES>\n");
            fp->Append(line);
         }
         
      }
   } 
}

void ASSISTANT::PolyLine::ChangeLineColor(Gdiplus::ARGB argbColor)
{
   if (!visible || isPencil) return;
   recorded = false;
   
   m_argbLineColor = argbColor;
}

void ASSISTANT::PolyLine::ChangeLineWidth(int _lineWidth)
{
   if (!visible || isPencil) return;

   recorded = false;
   
   lineWidth = _lineWidth;
}

void ASSISTANT::PolyLine::ChangeLineStyle(Gdiplus::DashStyle gdipLineStyle)
{
   if (!visible || isFreehand || isPencil) return;
   
   recorded = false;

   m_gdipLineStyle = gdipLineStyle;
}

void ASSISTANT::PolyLine::ChangeArrowStyle(LPCTSTR _style)
{
   if (!visible || isFreehand || isPencil) return;
   recorded = false;
   
   arrowStyle = _style;
   
   int iArrowStyle = ASSISTANT::GetIntArrowStyle(arrowStyle);
}

void ASSISTANT::PolyLine::ChangeArrowConfig(int _config)
{
   if (!visible) 
      return;
   
   recorded   = false;
   
   arrowConfig = _config;
   ComputeArrowConfig(arrowConfig);
}

void ASSISTANT::PolyLine::AdjustGui() 
{
/*/
   char buf[20];
   
   if (!isPencil) {
      GUI::Attributes::SetOutlineColor(color);
      sprintf(buf,"%d",lineWidth);
      GUI::Attributes::SetLineWidth(buf);
      GUI::Attributes::SetLineStyle(lineStyle);
      GUI::Attributes::SetArrowStyle(arrowStyle);
      
      if (isFreehand) 
         GUI::Attributes::SetTool("Freehand");
      else
         GUI::Attributes::SetTool("Polyline");
   }
/*/
   
}

bool ASSISTANT::PolyLine::ContainsRect(CRect &rcSelection)
{
   if (!visible)
      return false; 

   CRect rcObject;
   GetBoundingBox(rcObject);

   if (rcSelection.Width() == 0 && rcSelection.Height() == 0)
   {
      Gdiplus::PointF ptSelection(rcSelection.left, rcSelection.top);
      
      if (ContainsPoint(ptSelection))
         return true;
      else
         return false;
   }

   return false;
}


bool ASSISTANT::PolyLine::ContainsPoint(Gdiplus::PointF &ptMouse)
{
   if (!visible)
      return false; 

   if (points.GetSize() == 0)
      return false;

   
   double dMaxDistance = (double)lineWidth / 2 + 0.5;
   bool bContainsPoint = false;
   for (int i = 0; i < points.GetSize()-1 && !bContainsPoint; ++i)
   {
      
      Gdiplus::PointF ptLineBegin(points[i]->x, points[i]->y);
      Gdiplus::PointF ptLineEnd(points[i+1]->x, points[i+1]->y);
      double dDistance = ASSISTANT::ComputeDistance(ptLineBegin, ptLineEnd, ptMouse);
      
      if (dDistance < dMaxDistance)
         bContainsPoint = true;
   }

   return bContainsPoint;
}

ASSISTANT::DrawObject *ASSISTANT::PolyLine::Load(SGMLElement *hilf) 
{
   PolyLine
      *obj;
   _TCHAR 
      *tmp;
   int
      x, y,
      width, height,
		zPosition,
      lineWidth,
      iPPTId;
   bool
      bIsFreehand;
   _TCHAR 
      *color,
      *lineStyle,
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


   Gdiplus::ARGB argbLineColor = ASSISTANT::StringToGdipARGB(color);
   Gdiplus::DashStyle gdipLineStyle = ASSISTANT::StringToGdipLineStyle(lineStyle);

   if (hyperlink)
   {
      obj = new ASSISTANT::PolyLine(x, y, width, height, zPosition, 
                                    argbLineColor, lineWidth, gdipLineStyle,
                                    arrowStyle, arrowConfig, Project::active->GetObjectID(), 
                                    hyperlink, currentDirectory, linkedObjects, bIsFreehand);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new ASSISTANT::PolyLine(x, y, width, height, zPosition, 
                                    argbLineColor, lineWidth, gdipLineStyle,
                                    arrowStyle, arrowConfig, Project::active->GetObjectID(), 
                                    internLink, currentDirectory, linkedObjects, bIsFreehand);
      obj->LinkIsIntern(true);
   }
   
   _TCHAR *zwParameter = (_TCHAR *)malloc((hilf->Parameter.GetLength()+1) * sizeof(_TCHAR));
   _tcsncpy(zwParameter, hilf->Parameter, hilf->Parameter.GetLength());
   zwParameter[hilf->Parameter.GetLength()] = _T('\0');

   _TCHAR *p = zwParameter;
   while (*p != _T('\0')) 
   {
      while ((*p != _T('{')) && (*p != _T('\0'))) p++;
      if (*p == _T('\0')) 
         break;

      p++;
      tmp = p;
      while (*p != _T(' ')) 
         p++;
      *p = _T('\0');
      x = _ttoi(tmp);

      p++;
      tmp = p;
      while (*p != _T('}')) 
         p++;

      *p = _T('\0');
      y = _ttoi(tmp);
      obj->AppendNewPoint(x,y);
      p++;
   }

   free(zwParameter);

   if (obj && iPPTId > 0)
      obj->SetPPTObjectId(iPPTId);

   delete color;
   delete lineStyle;
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



ASSISTANT::Polygon::Polygon(double _x, double _y, double _width, double _height, int _zPosition, 
                            Gdiplus::ARGB argbLineColor, Gdiplus::ARGB argbFillColor, 
                            int _lineWidth, Gdiplus::DashStyle gdipLineStyle, uint32 _id, 
                            LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                            : PolyObject(_x, _y, _width, _height, _zPosition,
                                         argbLineColor, argbFillColor, _lineWidth, gdipLineStyle, _id, 
                                         _hyperlink, _currentDirectory, _linkedObjects) 
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
		               m_argbLineColor, m_argbFillColor, lineWidth, m_gdipLineStyle, Project::active->GetObjectID(), 
                     hyperlink_, currentDirectory_, m_ssLinkedObjects);
	
  if (isInternLink)
     ret->LinkIsIntern(true);
  else
     ret->LinkIsIntern(false);

	if (keepZPosition)
		ret->SetOrder(order);
	
   for (int i = 0; i < points.GetSize(); ++i) 
      ret->AppendNewPoint(points[i]->x, points[i]->y);
   
   return (DrawObject *)ret;
}

void ASSISTANT::Polygon::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) 
      return;
   
   if (points.GetSize() == 0)
      return;

   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);
   
   bool drawArrows = true;
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);
   if (lineWidth * _zoomFactor < 0.5)
      drawArrows = false;

   Gdiplus::PointF *zPts = (Gdiplus::PointF *)malloc(points.GetSize()*sizeof(Gdiplus::PointF));
   for (int i = 0; i < points.GetSize(); ++i)
   {
      zPts[i].X = points[i]->x;
      zPts[i].Y = points[i]->y;
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
      free(zPts);
      return;
   }
   
   if (m_argbFillColor != 0)
   {
      Gdiplus::Color color(m_argbFillColor);
      Gdiplus::SolidBrush brush(color);
      graphics.FillPolygon(&brush, zPts, points.GetSize(), Gdiplus::FillModeWinding);
   }

   
   Gdiplus::Color lineColor(m_argbLineColor);
   Gdiplus::Pen pen(lineColor, lineWidth);
   
   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
   pen.SetLineJoin(Gdiplus::LineJoinMiter);
   pen.SetDashStyle(m_gdipLineStyle);
   graphics.DrawPolygon(&pen, zPts, points.GetSize());
   
   
   free(zPts);
}

void ASSISTANT::Polygon::SaveObject(FILE *fp, int level, const _TCHAR *path)
{
   if (!visible) 
      return;
   
   WCHAR *tab = (WCHAR *)malloc((level*2+1)*sizeof(WCHAR));
   *tab = L'\0';
   for (int i = 0; i < level; i++) 
      wcscat(tab, L"  ");
   
   CString ptBuf;
   TCHAR buf[20];
   for (i = 0; i < points.GetSize(); ++i)
   {
      _stprintf(buf, _T("{%d %d} "),(int)points[i]->x, (int)points[i]->y);
      ptBuf += buf;
   }
   
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

   fwprintf(fp, L"%s<POLYGON x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" color=\"%s\" fillColor=\"%s\" lineWidth=\"%d\" lineStyle=\"%s\" length=\"%d\" ZPosition=\"%d\"%s>\n",
                tab, (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight,
                wszLineColor, wszFillColor, lineWidth, wszLineStyle,
                ptBuf.GetLength(), (int)order, hyperlinkString);

   fwprintf(fp, L"%s  \"", tab);

   WCHAR *wszPtBuf = ASSISTANT::ConvertTCharToWChar(ptBuf);
   fwrite(wszPtBuf, sizeof(WCHAR), wcslen(wszPtBuf), fp);

   fwprintf(fp, L"\"\n%s</POLYGON>\n", tab);
   
   ptBuf.Empty();
   if (wszPtBuf)
      delete wszPtBuf;

   if (wszHyperlink)
      delete wszHyperlink;

   if (wszLineColor)
      delete wszLineColor;

   if (wszFillColor)
      delete wszFillColor;

   if (wszLineStyle)
      delete wszLineStyle;

   if (tab)
      delete tab;
}

int ASSISTANT::Polygon::RecordableObjects()
{  
   if (!visible || (points.GetSize() < 1))
   {
      visible = false;
      return 0;
   }
   
   if (m_argbFillColor == 0) /* Outline */
      return 1;
   else
      return 2;
   
}

void ASSISTANT::Polygon::SaveAsAOF(CFileOutput *fp)
{
   if (!visible || recorded || (points.GetSize() < 1)) 
      return;
   
   recorded = true;
   
   recID = ASSISTANT::Project::active->recCount;
   
   CString line;

   if (m_argbFillColor != 0) 
   {
      CString csFillColor;
      ASSISTANT::GdipARGBToObjString(m_argbFillColor, csFillColor);

      ASSISTANT::Project::active->recCount++;

      line.Format(_T("<FILLEDPOLY x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f count=%d rgb=0x%s>\n"),
                  (int)m_dX ,(int)m_dY, (int)m_dWidth, (int)m_dHeight, 
                  m_dX ,m_dY, m_dWidth, m_dHeight,
                  points.GetSize(), csFillColor);
      fp->Append(line);

      for (int i = 0; i < points.GetSize(); ++i) 
      {
         line.Format(_T("<pt x=%d y=%d xf=%2.2f yf=%2.2f></pt>\n"), 
                     (int)points[i]->x, (int)points[i]->y, 
                     points[i]->x, points[i]->y);
         fp->Append(line);
      }

      line = _T("</FILLEDPOLY>\n");
      fp->Append(line);
   }
   
   CString csColor;
   ASSISTANT::GdipARGBToObjString(m_argbLineColor, csColor);

   int iLineStyle = ASSISTANT::GdipLineStyleToObjStyle(m_gdipLineStyle);

   ASSISTANT::Project::active->recCount++;

   line.Format(_T("<POLYLINES x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f count=%d linewidth=%d linestyle=%d rgb=0x%s>\n"),
               (int)m_dX ,(int)m_dY, (int)m_dWidth, (int)m_dHeight, 
               m_dX ,m_dY, m_dWidth, m_dHeight,
               points.GetSize()+1, lineWidth, iLineStyle, csColor);
   fp->Append(line);

   for (int i = 0; i < points.GetSize(); ++i) 
   {
      line.Format(_T("<pt x=%d y=%d xf=%2.2f yf=%2.2f></pt>\n"), 
                  (int)points[i]->x, (int)points[i]->y, 
                  points[i]->x, points[i]->y);
      fp->Append(line);
   }

   i = 0;
   line.Format(_T("<pt x=%d y=%d xf=%2.2f yf=%2.2f></pt>\n"), 
               (int)points[i]->x, (int)points[i]->y, 
               points[i]->x, points[i]->y);
   fp->Append(line);

   line = _T("</POLYLINES>\n");
   fp->Append(line);
   
   
}

void ASSISTANT::Polygon::AdjustGui() 
{
}

bool ASSISTANT::Polygon::ContainsRect(CRect &rcSelection)
{
   if (!visible)
      return false; 

   CRect rcObject;
   GetBoundingBox(rcObject);

   if (rcSelection.Width() == 0 && rcSelection.Height() == 0)
   {
      Gdiplus::PointF ptSelection(rcSelection.left, rcSelection.top);
      
      if (ContainsPoint(ptSelection))
         return true;
      else
         return false;
   }

   if (m_argbFillColor != 0)
   {
      CArray<CPoint , CPoint > aBoundingPoints;
      
      for (int i = 0; i < points.GetSize(); ++i)
      {
         aBoundingPoints.Add(CPoint(points[i]->x-lineWidth, points[i]->y));
      }
      
      CRgn rgnLineBounding;
      BOOL bRet = rgnLineBounding.CreatePolygonRgn(aBoundingPoints.GetData(), aBoundingPoints.GetSize(), WINDING);
      
      if (!bRet)
         return false;
      
      if (rgnLineBounding.RectInRegion(rcSelection))
         return true;
      
   }

   return false;
}


bool ASSISTANT::Polygon::ContainsPoint(Gdiplus::PointF &ptMouse)
{
   if (!visible)
      return false; 

   if (points.GetSize() == 0)
      return false;

   
   bool bContainsPoint = false;
   if (m_argbFillColor != 0)
   {
      CArray<CPoint , CPoint > aBoundingPoints;
      
      for (int i = 0; i < points.GetSize(); ++i)
      {
         aBoundingPoints.Add(CPoint(points[i]->x-lineWidth, points[i]->y));
      }
      
      CRgn rgnLineBounding;
      BOOL bRet = rgnLineBounding.CreatePolygonRgn(aBoundingPoints.GetData(), aBoundingPoints.GetSize(), WINDING);
      
      if (!bRet)
         return false;
      
      if (rgnLineBounding.PtInRegion((int)ptMouse.X, (int)ptMouse.Y))
         bContainsPoint = true;
      
   }
   else // unfilled polygon
   {
      if (points.GetSize() == 0)
         return false;
      
      
      double dMaxDistance = (double)lineWidth / 2 + 0.5;
      for (int i = 0; i < points.GetSize()-1 && !bContainsPoint; ++i)
      {
         
         Gdiplus::PointF ptLineBegin(points[i]->x, points[i]->y);
         Gdiplus::PointF ptLineEnd(points[i+1]->x, points[i+1]->y);
         double dDistance = ASSISTANT::ComputeDistance(ptLineBegin, ptLineEnd, ptMouse);
         
         if (dDistance < dMaxDistance)
            bContainsPoint = true;
      }
   }


   return bContainsPoint;
}

ASSISTANT::DrawObject *ASSISTANT::Polygon::Load(SGMLElement *hilf) 
{
   Polygon
      *obj;
   _TCHAR
      *tmp;
   int
      x, y,
      width, height,
		zPosition, 
      lineWidth,
      iPPTId;
   _TCHAR 
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
   

   Gdiplus::ARGB argbLineColor = ASSISTANT::StringToGdipARGB(color);
   Gdiplus::ARGB argbFillColor = ASSISTANT::StringToGdipARGB(fillColor);
   Gdiplus::DashStyle gdipLineStyle = ASSISTANT::StringToGdipLineStyle(lineStyle);

   if (hyperlink != NULL)
   {
      obj = new ASSISTANT::Polygon(x, y, width, height, zPosition, 
                                   argbLineColor, argbFillColor, lineWidth, gdipLineStyle, 
                                   Project::active->GetObjectID(), 
                                   hyperlink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new ASSISTANT::Polygon(x, y, width, height, zPosition,
                                   argbLineColor, argbFillColor, lineWidth, gdipLineStyle, 
                                   Project::active->GetObjectID(), 
                                   internLink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(true);
   }
   
   _TCHAR *zwParameter = (_TCHAR *)malloc((hilf->Parameter.GetLength()+1) * sizeof(_TCHAR));
   _tcsncpy(zwParameter, hilf->Parameter, hilf->Parameter.GetLength());
   zwParameter[hilf->Parameter.GetLength()] = _T('\0');
   _TCHAR *p = zwParameter;
   while (*p != _T('\0')) {
      while ((*p != _T('{')) && (*p!=_T('\0'))) 
         p++;

      if (*p == _T('\0')) break;
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
   free(zwParameter);

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


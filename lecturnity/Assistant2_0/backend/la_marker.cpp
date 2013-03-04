/****************************CDC *pDC, *****************************************

  program : la_marker.cpp
  authors : Gabriela Maass
  date    : 14.03.2003
  desc    : Functions to draw and manipulate objects on a tcl-canvas
  
**********************************************************************/
#include "StdAfx.h"

#include "mlb_page.h"
#include "la_project.h"
#include "mlb_objects.h"
#include "mlb_misc.h"

ASSISTANT::Marker::Marker(double _x, double _y, int _zPosition, 
                          Gdiplus::ARGB argbLineColor, int _linewidth, uint32 _id, 
                          LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects) 
                  : PolyObject(_x, _y, 0, 0, _zPosition, 
                               argbLineColor, 0, _linewidth, Gdiplus::DashStyleSolid, _id, 
                               _hyperlink, _currentDirectory, _linkedObjects) 
{
   visible = true;
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
   ASSISTANT::Marker *ret = 
      new Marker(m_dX, m_dY, -1, 
                 m_argbLineColor, lineWidth, Project::active->GetObjectID(), 
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

void ASSISTANT::Marker::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) 
      return;
   
   if (points.GetSize() == 0)
      return;


   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);
   
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   
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
      
      graphics.FillRectangle(&brush, (Gdiplus::REAL)(zPts[0].X-iHalfLineWidth), (Gdiplus::REAL)(zPts[0].Y-iHalfLineWidth), 
         (Gdiplus::REAL)(lineWidth), (Gdiplus::REAL)(lineWidth));
      free(zPts);
      return;
   }
   
   Gdiplus::Color lineColor(m_argbLineColor);
   Gdiplus::Pen pen(lineColor, lineWidth);
   
   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapRound);
   pen.SetLineJoin(Gdiplus::LineJoinRound);
   
   graphics.DrawLines(&pen, zPts, points.GetSize());

   free(zPts);
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
void ASSISTANT::Marker::SaveObject(FILE *fp, int level, LPCTSTR path)
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
      Point *pPt = points[i];
      _stprintf(buf, _T("{%d %d} "),(int)pPt->x,(int)pPt->y);
      ptBuf += buf;
   }
	
   CString csColor;
   ASSISTANT::GdipARGBToString(m_argbLineColor, csColor);
   WCHAR *wszLineColor = ASSISTANT::ConvertTCharToWChar(csColor);

   fwprintf(fp,L"%s<MARKER x=\"%d\" y=\"%d\" color=\"%s\" lineWidth=\"%d\" length=\"%d\" ZPosition=\"%d\">\n",
               tab, (int)m_dX, (int)m_dY, wszLineColor, lineWidth, 
               ptBuf.GetLength(), (int)order);

   fwprintf(fp, L"%s  \"", tab);

   WCHAR *wszPtBuf = ASSISTANT::ConvertTCharToWChar(ptBuf);

   fwrite(ptBuf, sizeof(WCHAR), wcslen(wszPtBuf), fp);

   if (wszLineColor)
      delete wszLineColor;

   if (wszPtBuf)
      delete wszPtBuf;

   fwprintf(fp, L"\"\n%s</MARKER>\n", tab);
	
   if (tab)
      delete tab;
}

int ASSISTANT::Marker::RecordableObjects()
{
   if (!visible || (points.GetSize() < 1))
   {
      visible = false;
      return 0;
   }
	
   return 1;
}

void ASSISTANT::Marker::SaveAsAOF(CFileOutput *fp)
{
   if (!visible || recorded || (points.GetSize() < 1)) 
      return;
	
   recorded = true;
	
   recID = ASSISTANT::Project::active->recCount;
   ASSISTANT::Project::active->recCount++;
	
   CString csColor;
   ASSISTANT::GdipARGBToObjString(m_argbLineColor, csColor);
   
   CString line;

   line.Format(_T("<POLYLINES x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f count=%d linewidth=%d rgb=0x%s>\n"),
               (int)m_dX ,(int)m_dY, (int)m_dWidth, (int)m_dHeight, 
               m_dX ,m_dY, m_dWidth, m_dHeight, 
               points.GetSize(), lineWidth, csColor);

   fp->Append(line);

   for (int i = 0; i < points.GetSize(); ++i)
   {
      line.Format(_T("<pt x=%d y=%d xf=%2.2f yf=%2.2f></pt>\n"),
                  (int)points[i]->x, (int)points[i]->y, 
                  points[i]->x, points[i]->y);

      fp->Append(line);
   }

   line.Format(_T("</POLYLINES>\n"));

   fp->Append(line);
	
}

void ASSISTANT::Marker::ChangeLineColor(Gdiplus::ARGB argbLineColor)
{
	if (!visible) 
		return;
	
	recorded = false;
	
	m_argbLineColor = argbLineColor;
}

void ASSISTANT::Marker::ChangeLineWidth(int _lineWidth)
{
	if (!visible) 
		return;
	
	recorded = false;
	
	lineWidth = _lineWidth;
}

ASSISTANT::DrawObject *ASSISTANT::Marker::Load(ASSISTANT::SGMLElement *hilf) 
{
   Marker
      *obj;
   _TCHAR
      *tmp;
   int
      x, y,
		zPosition, 
      lineWidth,
      iPPTId;
   _TCHAR 
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
	
   
   Gdiplus::ARGB argbLineColor = ASSISTANT::StringToGdipARGB(color);

   obj = new ASSISTANT::Marker(x, y, zPosition, 
                               argbLineColor, lineWidth, Project::active->GetObjectID(), 
                               hyperlink, currentDirectory, linkedObjects);
   
   TCHAR *p = (TCHAR *)(LPCTSTR)hilf->Parameter;
   while (*p != _T('\0')) 
   {
      while ((*p != _T('{')) && (*p != _T('\0'))) 
         p++;

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

      obj->AppendNewPoint(x, y);

      p++;
   }
   
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


bool ASSISTANT::Marker::ContainsPoint(Gdiplus::PointF &ptMouse)
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

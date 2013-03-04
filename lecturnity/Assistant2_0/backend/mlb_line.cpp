/**********************************************************************

  program : mlb_line.cpp
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

ASSISTANT::Line::Line(double _x, double _y, double _width, double _height, int _zPosition, 
                      Gdiplus::ARGB argbLineColor, int _lineWidth, Gdiplus::DashStyle gdipLineStyle,
                      LPCTSTR _arrowStyle, int _arrowConfig, uint32 _id, 
                      LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                      : ColoredObject(_x, _y, _width, _height, _zPosition, argbLineColor, 0, 
                                      _lineWidth, gdipLineStyle, _id, _hyperlink, _currentDirectory, _linkedObjects)
{
   arrowStyle  = _arrowStyle;
   arrowConfig = _arrowConfig;
   ComputeArrowConfig(arrowConfig);
   //if ((width==0) && (height==0)) width = 1;
}

ASSISTANT::Line::~Line() {
}

ASSISTANT::DrawObject *ASSISTANT::Line::Copy(bool keepZPosition)
{
   ASSISTANT::Line *ret;
   
	ret = new Line(m_dX, m_dY, m_dWidth, m_dHeight, -1,
		            m_argbLineColor, lineWidth, m_gdipLineStyle, 
		            arrowStyle, arrowConfig, Project::active->GetObjectID(), hyperlink_, currentDirectory_, m_ssLinkedObjects);
   
  if (isInternLink)
     ret->LinkIsIntern(true);
  else
     ret->LinkIsIntern(false);

	if (keepZPosition)
		ret->SetOrder(order);
	
   return (DrawObject *)ret;
}

void ASSISTANT::Line::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);
   bool drawArrows = true;
      
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);
   
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   Gdiplus::Color lineColor(m_argbLineColor);
  
   Gdiplus::Pen pen(lineColor, lineWidth);
   pen.SetLineCap(Gdiplus::LineCapSquare , Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
   pen.SetDashStyle(m_gdipLineStyle);
   pen.SetAlignment(Gdiplus::PenAlignmentCenter);

   SetArrowCaps(pen);

   graphics.DrawLine(&pen, (Gdiplus::REAL)m_dX, (Gdiplus::REAL)m_dY, (Gdiplus::REAL)(m_dX + m_dWidth), (Gdiplus::REAL)(m_dY + m_dHeight));

}

void ASSISTANT::Line::DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   selectPoints[0].X = m_dX;
   selectPoints[0].Y = m_dY;
   selectPoints[1].X = m_dX+m_dWidth;
   selectPoints[1].Y = m_dY+m_dHeight;
   
   Gdiplus::Color clrWhite(255, 255, 255, 255);
   Gdiplus::Color clrBlack(255, 0, 0, 0);

   Gdiplus::REAL fLineWidth = 6.0 / _zoomFactor;
   Gdiplus::REAL fRadius = 3.0 / _zoomFactor;

   Gdiplus::REAL fPenWidth = 0.5f;
   Gdiplus::Pen blackPen(clrBlack, fPenWidth);
   Gdiplus::SolidBrush whiteBrush(clrWhite);
   for (int i = 0; i < 2; ++i) 
   {
      graphics.FillEllipse(&whiteBrush, 
         (selectPoints[i].X - fRadius), (selectPoints[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
      graphics.DrawEllipse(&blackPen, 
         (selectPoints[i].X - fRadius), (selectPoints[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
   }
}

bool ASSISTANT::Line::IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditType, int &iPointIndex, double dZoomFactor)
{
   if (!visible || !selected) 
      return false;
   
   double dTolerance = 3 / dZoomFactor;

   nEditType = 0;
   iPointIndex = -1;
   for (int i = 0; i < 2; i++) 
   {
      if ((ptObject.X > selectPoints[i].X-dTolerance) && (ptObject.X < selectPoints[i].X+dTolerance)) 
      {
         if ((ptObject.Y > selectPoints[i].Y-dTolerance) && (ptObject.Y < selectPoints[i].Y+dTolerance)) 
         {
            if (i == 0)
               nEditType = SIZE_NWSE;
            else if (i == 1)
               nEditType = SIZE_NESW;

            iPointIndex = i+8;
            return true;
         }
      }
   }
   
   if (ContainsPoint(ptObject))
   {
      nEditType = MOVE;
      return true;
   }
   
   return false;
}

void ASSISTANT::Line::MovePoint(int point, int _x, int _y, float _zoomFactor)
{
   recorded = false;
   
   if (point == 1) 
   {
      m_dWidth  = m_dX + m_dWidth - _x;
      m_dHeight = m_dY + m_dHeight - _y;
      
      m_dX = _x;
      m_dY = _y;
   } 
   else 
   {
      m_dWidth  = _x - m_dX;
      m_dHeight = _y - m_dY;
   }
   
   /*/
   ReDraw(_zoomFactor);
   /*/
}

void ASSISTANT::Line::Resize(float fDiffX, float fDiffY, int iPointIndex)
{
   SetUnrecorded();

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

void ASSISTANT::Line::SaveObject(FILE *fp, int level, LPCTSTR path)
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

   CString csLineStyle;
   ASSISTANT::GdipLineStyleToString(m_gdipLineStyle, csLineStyle);
   WCHAR *wszLineStyle = ASSISTANT::ConvertTCharToWChar(csLineStyle);

   if (arrowStyle != _T("none")) {
      fwprintf(fp, L"<LINE x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" color=\"%s\" lineWidth=\"%d\" lineStyle=\"%s\" arrowStyle=\"%s\" arrowConfig=\"%d\" ZPosition=\"%d\"%s></LINE>\n",
                   (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight, 
                   wszLineColor, lineWidth, wszLineStyle, arrowStyle, arrowConfig, 
                   (int)order, wszHyperlink);
   }
   else {
      fwprintf(fp, L"<LINE x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" color=\"%s\" lineWidth=\"%d\" lineStyle=\"%s\" ZPosition=\"%d\"%s></LINE>\n",
                   (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight,
                   wszLineColor, lineWidth, wszLineStyle, (int)order, wszHyperlink);
   }

   if (wszHyperlink)
      delete wszHyperlink;

   if (wszLineColor)
      delete wszLineColor;

   if (wszLineStyle)
      delete wszLineStyle;
}

int ASSISTANT::Line::RecordableObjects()
{
   if (m_dWidth==0 && m_dHeight==0) visible = false;
   
   if (!visible) return 0;
   
   return 1;
}

void ASSISTANT::Line::SaveAsAOF(CFileOutput *fp)
{
   if (!visible || recorded) return;
   
   recorded = true;
   
   int dir = 0;
   int ori = 0;
   double dX = m_dX;
   double dY = m_dY;
   double dWidth = m_dWidth;
   double dHeight = m_dHeight;
   CalculateLineParameters(dX, dY, dWidth, dHeight, ori, dir);
   
   recID = ASSISTANT::Project::active->recCount;
   ASSISTANT::Project::active->recCount++;
   
   // Calculate Color
   CString csColor;
   ASSISTANT::GdipARGBToObjString(m_argbLineColor, csColor);

   int iLineStyle = ASSISTANT::GdipLineStyleToObjStyle(m_gdipLineStyle);

   CString line;

   if (arrowStyle == _T("none")) 
   {
      line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=0 rgb=0x%s></LINE>\n"),
                  (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                  dX ,dY, dWidth, dHeight,
                  ori, dir, lineWidth, iLineStyle, csColor);
      fp->Append(line);
   }
   else if (arrowStyle == _T("first")) 
   {
      if (dir) 
      {
         line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=1 arrowconfig=%d rgb=0x%s></LINE>\n"),
                     (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                     dX ,dY, dWidth, dHeight,
                     ori, dir, lineWidth, iLineStyle, 
                     SwapArrowConfig(arrowConfig), csColor);
         fp->Append(line);
      }
      else  
      {
         line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=2 arrowconfig=%d rgb=0x%s></LINE>\n"),
                     (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                     dX, dY, dWidth, dHeight,
                     ori, dir, lineWidth, iLineStyle, 
                     arrowConfig, csColor);
         fp->Append(line);
      }
   }
   else if (arrowStyle == _T("last")) 
   {
      if (dir) 
      {
         line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=2 arrowconfig=%d rgb=0x%s></LINE>\n"),
                     (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                     dX, dY, dWidth, dHeight,
                     ori, dir, lineWidth, iLineStyle, 
                     SwapArrowConfig(arrowConfig), csColor);
         fp->Append(line);
      }
      else 
      {
         line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=1 arrowconfig=%d rgb=0x%s></LINE>\n"),
                     (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                     dX ,dY, dWidth, dHeight,
                     ori, dir, lineWidth, iLineStyle, 
                     arrowConfig, csColor);  
         fp->Append(line);
      }
   }
   else if (arrowStyle == _T("both")) 
   {
      if (dir)
      {
         line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=3 arrowconfig=%d rgb=0x%s></LINE>\n"),
                     (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                     dX, dY, dWidth, dHeight,
                     ori, dir, lineWidth, iLineStyle, 
                     SwapArrowConfig(arrowConfig), csColor); 
         fp->Append(line);
      }
      else
      {
         line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f ori=%d dir=%d linewidth=%d linestyle=%d arrowstyle=3 arrowconfig=%d rgb=0x%s></LINE>\n"),
                     (int)dX ,(int)dY, (int)dWidth, (int)dHeight, 
                     dX ,dY, dWidth, dHeight,
                     ori, dir, lineWidth, iLineStyle, 
                     arrowConfig, csColor);
         fp->Append(line);
      }
   }
   
}

void ASSISTANT::Line::ChangeArrowStyle(LPCTSTR _style)
{
   if (!visible) 
      return;
   
   recorded   = false;
   
   arrowStyle = _style;
   
   int iArrowStyle = ASSISTANT::GetIntArrowStyle(arrowStyle);
}

void ASSISTANT::Line::ChangeArrowConfig(int _config)
{
   if (!visible) 
      return;
   
   recorded   = false;
   
   arrowConfig = _config;
   ComputeArrowConfig(arrowConfig);
}

bool ASSISTANT::Line::Overlap(int _x, int _y, float _zoomFactor)
{
   int
      x1, y1,
      x2, y2;
   
   if (!visible) return false;
   
   GetBBox(&x1,&y1,&x2,&y2);
   if ((_x < x1*_zoomFactor) || (_y < y1*_zoomFactor) || 
       (_x > x2*_zoomFactor) || (_y > y2*_zoomFactor)) 
       return false;
   
   return true; //canvas->Overlap(tkref,_x,_y);
}

void ASSISTANT::Line::AdjustGui()
{
   /*/
   char buf[20];
   
   GUI::Attributes::SetOutlineColor(color);
   sprintf(buf,"%d",lineWidth);
   GUI::Attributes::SetLineWidth(buf);
   GUI::Attributes::SetLineStyle(lineStyle);
   GUI::Attributes::SetCapStyle(capStyle);
   GUI::Attributes::SetJoinStyle(joinStyle);
   GUI::Attributes::SetArrowStyle(arrowStyle);
   
   GUI::Attributes::SetTool("Line");
   /*/
}

bool ASSISTANT::Line::ContainsPoint(Gdiplus::PointF &ptMouse)
{
   if (!visible)
      return false; 

   BOOL bRet = FALSE;
   
   Gdiplus::PointF ptLineBegin(m_dX, m_dY);
   Gdiplus::PointF ptLineEnd(m_dX+m_dWidth, m_dY+m_dHeight);
   double dDistance = ASSISTANT::ComputeDistance(ptLineBegin, ptLineEnd, ptMouse);

   double dMaxDistance = (double)lineWidth / 2 + 0.5;
   if (dDistance < dMaxDistance)
      return true;
   else
      return false;
}

ASSISTANT::DrawObject *ASSISTANT::Line::Load(SGMLElement *hilf) 
{
   Line 
      *obj;
   _TCHAR
      *tmp;
   double
      x, y,
      width, height;
   int
		zPosition,
      lineWidth;
   _TCHAR 
      *color,
      *lineStyle,
      *arrowStyle,
      *hyperlink,
      *internLink,
      *currentDirectory,
      *linkedObjects;
   int
      iArrowConfig,
      iPPTId;
   TCHAR 
      *szEndPtr;   // needed for _tcstod
   
   tmp = hilf->GetAttribute("x");
   if (tmp) x = _tcstod(tmp, &szEndPtr);
   else     x = 0;
   
   tmp = hilf->GetAttribute("y");
   if (tmp) y = _tcstod(tmp, &szEndPtr);
   else     y = 0;
   
   tmp = hilf->GetAttribute("width");
   if (tmp) width = _tcstod(tmp, &szEndPtr);
   else     width = 0;
   
   tmp = hilf->GetAttribute("height");
   if (tmp) height = _tcstod(tmp, &szEndPtr);
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
   
   Gdiplus::ARGB argbLineColor = ASSISTANT::StringToGdipARGB(color);
   Gdiplus::DashStyle gdipLineStyle = ASSISTANT::StringToGdipLineStyle(lineStyle);

   if (hyperlink != NULL)
   {
      obj = new Line(x, y, width, height, zPosition, argbLineColor, lineWidth, 
                     gdipLineStyle, arrowStyle, iArrowConfig,
                     Project::active->GetObjectID(), hyperlink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new Line(x, y, width, height, zPosition, argbLineColor, lineWidth, 
                     gdipLineStyle, arrowStyle, iArrowConfig,
                     Project::active->GetObjectID(), internLink, currentDirectory, linkedObjects);
      obj->LinkIsIntern(true);
   }
   
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

bool ASSISTANT::Line::IsIdentic(DrawObject *obj)
{
   // the same object is identic
   if (obj == this)
      return true;

   if (!ColoredObject::IsIdentic(obj))
      return false;

   if (GetArrowStyle() != ((Line *)obj)->GetArrowStyle())
      return false;
   
   if (GetArrowConfig() != ((Line *)obj)->GetArrowConfig())
      return false;

   // all parameters are identic
   return true;
}

void ASSISTANT::Line::CalculateLineParameters(double &dX, double &dY, 
                                              double &dWidth, double &dHeight, 
                                              int &ori, int &dir)
{
   dir = 0;
   ori = (dWidth * dHeight > 0);

   if (dWidth < 0) 
   {
      dX = dX + dWidth;
      dWidth = dWidth * (-1);
      dir = 1;
   }

   if (dWidth == 0 && dHeight > 0) 
      dir = 1;

   if (dHeight < 0) 
   {
      dY = dY + dHeight;
      dHeight = dHeight * (-1);
   }

}

/*********************************************************************
program : mlb_objects.cpp
authors : Gabriela Maass
date    : 3.12.1999
desc    : Functions to draw and manipulate objects on a tcl-canvas

**********************************************************************/

#include "StdAfx.h"
#include  <math.h>


#include "mlb_objects.h"
#include "mlb_page.h"
#include "mlb_misc.h"


/**********************************************************************
Name:      ASSISTANT::DrawObject::DrawObject(double _x, double _y, double _width, double _height, int _id)
Param:     _x, _y:          Coordinates (in Pixel)
_width, _height: Dimensions (in Pixel)
_id:         Identification-number of the draw-object
Desc:      initializes variables
Return:  
**********************************************************************/
ASSISTANT::DrawObject::DrawObject(double _x, double _y, double _width, double _height, int _zPosition, uint32 _id, const _TCHAR *_hyperlink, const _TCHAR *_currentDirectory, const _TCHAR *_linkedObjects) 
: GenericObject(_id) 
{
   m_dX          = _x;
   m_dY          = _y;
   m_dWidth      = _width;
   m_dHeight     = _height;
   visible    = true;
   selected   = false;  
   recorded   = false;
   isInternLink = false;
   if (_hyperlink)
      hyperlink_ = _hyperlink;
   else
      hyperlink_.Empty();
   if (_currentDirectory)
      currentDirectory_ = _currentDirectory;
   else
      currentDirectory_.Empty();
   
   if (_linkedObjects)
      m_ssLinkedObjects = _linkedObjects;
   else
      m_ssLinkedObjects.Empty();

   m_iPPTId = -1;

	SetOrder((float)_zPosition);
   
   group = NULL;

   m_bIsMasterObject = false;
   m_bIsDocumentObject = false;
}

ASSISTANT::DrawObject::~DrawObject() 
{
   hyperlink_.Empty();
   currentDirectory_.Empty();
}

void ASSISTANT::DrawObject::DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform((Gdiplus::REAL)dOffX, (Gdiplus::REAL)dOffY);
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);


   double dOffset = 3 / _zoomFactor;
   
   selectPoints[0].X = (Gdiplus::REAL)m_dX;
   selectPoints[0].Y = (Gdiplus::REAL)m_dY;
   selectPoints[1].X = (Gdiplus::REAL)(m_dX+(m_dWidth/2));
   selectPoints[1].Y = (Gdiplus::REAL)m_dY;
   selectPoints[2].X = (Gdiplus::REAL)(m_dX+m_dWidth);
   selectPoints[2].Y = (Gdiplus::REAL)m_dY;
   selectPoints[3].X = (Gdiplus::REAL)(m_dX+m_dWidth);
   selectPoints[3].Y = (Gdiplus::REAL)(m_dY+(m_dHeight/2));
   selectPoints[4].X = (Gdiplus::REAL)(m_dX+m_dWidth);
   selectPoints[4].Y = (Gdiplus::REAL)(m_dY+m_dHeight);
   selectPoints[5].X = (Gdiplus::REAL)(m_dX+(m_dWidth/2));
   selectPoints[5].Y = (Gdiplus::REAL)(m_dY+m_dHeight);
   selectPoints[6].X = (Gdiplus::REAL)m_dX;
   selectPoints[6].Y = (Gdiplus::REAL)(m_dY+m_dHeight);
   selectPoints[7].X = (Gdiplus::REAL)m_dX;
   selectPoints[7].Y = (Gdiplus::REAL)(m_dY+(m_dHeight/2));
   
   Gdiplus::Color clrWhite(255, 255, 255, 255);
   Gdiplus::Color clrBlack(255, 0, 0, 0);

   Gdiplus::REAL fLineWidth = (Gdiplus::REAL)(6.0 / _zoomFactor);
   Gdiplus::REAL fRadius = (Gdiplus::REAL)(3.0 / _zoomFactor);

   Gdiplus::REAL fPenWidth = 0.5f;
   Gdiplus::Pen blackPen(clrBlack, fPenWidth);
   Gdiplus::SolidBrush whiteBrush(clrWhite);
   
   for (int i = 0; i < 8; i++) 
   {
      graphics.FillEllipse(&whiteBrush, 
         (selectPoints[i].X - fRadius), (selectPoints[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
      graphics.DrawEllipse(&blackPen, 
         (selectPoints[i].X - fRadius), (selectPoints[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
   }
}

void ASSISTANT::DrawObject::Zoom(float _zoomFactor)
{
   return;
}

bool ASSISTANT::DrawObject::IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditType, int &iPointIndex, double dZoomFactor)
{
   if (!visible || !selected) 
      return false;
   
   double dTolerance = 3 / dZoomFactor;
   
   nEditType = 0;
   iPointIndex = -1;
   for (int i = 0; i < 8; i++) 
   {
      if ((ptObject.X > selectPoints[i].X-dTolerance) && (ptObject.X < selectPoints[i].X+dTolerance)) 
      {
         if ((ptObject.Y > selectPoints[i].Y-dTolerance) && (ptObject.Y < selectPoints[i].Y+dTolerance)) 
         {
            if (i == 0 || i == 4)
               nEditType = SIZE_NWSE;
            if (i == 1 || i == 5)
               nEditType = SIZE_NS;
            if (i == 2 || i == 6)
               nEditType = SIZE_NESW;
            if (i == 3 || i == 7)
               nEditType = SIZE_WE;
            iPointIndex = i;
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

void ASSISTANT::DrawObject::ComputeCoords(double _x, double _y, double& _newX, double& _newY, double& _newWidth, double& _newHeight)
{
   double 
      x1, y1,
      x2, y2;
   
   if (!visible) return;
   
   x1 = m_dX;
   x2 = m_dX+m_dWidth;
   y1 = m_dY;
   y2 = m_dY+m_dHeight;
   
   
   switch (selectCount) {
   case 0:
   case 7:
   case 6:
      if (_x > x2) {
         x1 = x2;
         x2 = _x;
         selectCount = (10-selectCount)%8;
      }
      else 
         x1 = _x;
      break;
   case 2:
   case 3:
   case 4:
      if (_x < x1) {
         x2 = x1;
         x1 = _x;
         selectCount = (10-selectCount)%8;
      }
      else 
         x2 = _x;
      break;
   }
   
   switch (selectCount) {
   case 0:
   case 1:
   case 2:
      if (_y > y2) {
         y1 = y2;
         y2 = _y;
         selectCount = 6-selectCount;
      }
      else
         y1 = _y;
      break;
   case 6:
   case 5:
   case 4:
      if (_y < y1) {
         y2 = y1;
         y1 = _y;
         selectCount = 6-selectCount;
      }
      else
         y2 = _y;
      break;
   }
   
   _newX = m_dX = x1;
   _newY = m_dY = y1;
   _newWidth  = m_dWidth  = (x2-x1);
   _newHeight = m_dHeight = (y2-y1);
}

void ASSISTANT::DrawObject::SetSelected(bool bIsSelected)
{
   if (group) 
   {
      selected = false;
      group->SetSelected(bIsSelected);
   }
   else 
   {
      selected = bIsSelected;
   }
}

bool ASSISTANT::DrawObject::IsSelected()
{
   if (group) 
      return group->IsSelected();
   else 
      return selected;
}

bool ASSISTANT::DrawObject::HasHyperlink(Gdiplus::PointF &ptMouse)
{
   if (!ContainsPoint(ptMouse) || hyperlink_.IsEmpty()) 
      return false; 
   
   return true;
}

bool ASSISTANT::DrawObject::HasObjectLinks(Gdiplus::PointF &ptMouse)
{
   if (ContainsPoint(ptMouse) && m_ssLinkedObjects.IsEmpty())
      return false;
   else
      return true;
}

void ASSISTANT::DrawObject::MakeHyperlinkString(CString &hyperlinkString)
{ 
   hyperlinkString.Empty();
   if (!hyperlink_.IsEmpty())
   {
      if (isInternLink)
      {
         hyperlinkString = _T(" intern=\"");
         hyperlinkString += hyperlink_;
         hyperlinkString += _T("\"");
      }
      else
      {
         CString zwHyperlink;
         zwHyperlink = _T(" address=\"");
         zwHyperlink += hyperlink_;
         zwHyperlink += _T("\" path=\"");
         zwHyperlink += currentDirectory_;
         zwHyperlink += _T("\"");
         

         for (int i = 0; i < zwHyperlink.GetLength(); ++i)
         {
            if (zwHyperlink[i] == _T('\\'))
               hyperlinkString += _T("\\");
            hyperlinkString += zwHyperlink[i];
         }
         zwHyperlink.Empty();
      }
   }
   if (!m_ssLinkedObjects.IsEmpty())
   {
         hyperlinkString += _T(" linkedObjects=\"");
         hyperlinkString += m_ssLinkedObjects;
         hyperlinkString += _T("\"");
         if (m_iPPTId > 0)
         {
            _TCHAR szId[256];
            _itot(m_iPPTId, szId, 10);
            hyperlinkString += _T(" id=\"");
            hyperlinkString += szId;
            hyperlinkString += _T("\"");
         }
   }
}

/**********************************************************************
Name:      ASSISTANT::DrawObject::Hide(GUI::Canvas *canvas)
Param:     widget:          name of the tcl-canvas
Desc:      delete the object from the tcl-canvas; automatically redrawn
Return:  
**********************************************************************/
void ASSISTANT::DrawObject::Hide()
{
   if (!visible) return;
   
   visible = false;
   selected = false;
}

/**********************************************************************
Name:      ASSISTANT::DrawObject::ModifyCoords(GUI::Canvas *canvas, int _x, int _y, int _width, int _height)
Param:     canvas:          GUI-Canvas
_x, _y:          new coordinates
_width, _height: new dimensions
Desc:      change the coordinates of the object; automatically redrawn
Return:  
**********************************************************************/
void ASSISTANT::DrawObject::ModifyCoords(double _x, double _y, double _width, double _height, float _zoomFactor)
{
   if (!visible) return;
   
   recorded = false;
   
   m_dX      = _x;
   m_dY      = _y;
   m_dWidth  = _width;
   m_dHeight = _height;
}

/**********************************************************************
Name:      ASSISTANT::DrawObject::Resize(GUI::Canvas *canvas, int _width, int _height)
Param:     canvas:          GUI-Canvas
_width, _height: new dimensions
Desc:      resize the object; automatically redrawn
Return:  
**********************************************************************/
void ASSISTANT::DrawObject::Resize(int _width, int _height)
{
   if (!visible) return;
   
   recorded = false;
   
   m_dWidth  = _width;
   m_dHeight = _height;
   
   //_canvas->ModifyCoords(tkref,(int)x,(int)y,width,height);
}

void ASSISTANT::DrawObject::Resize(float fDiffX, float fDiffY, int iPointIndex)
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
   }
}

/**********************************************************************
Name:      ASSISTANT::DrawObject::Move(GUI::Canvas *canvas, int diffX, int diffY)
Param:     canvas:          GUI-Canvas
diffX, diffY:    the x and y difference
Desc:      move the object; automatically redrawn
Return:  
**********************************************************************/
void ASSISTANT::DrawObject::Move(float _diffX, float _diffY)
{
   if (!visible) return;
   
   recorded = false;
   
   m_dX += _diffX;
   m_dY += _diffY;

}

/**********************************************************************
Name:      ASSISTANT::DrawObject::SelectObject(int _x, int _y)
Param:     length: length of selectList
selectList: list of selected objects
Desc:      scale the object from 0,0 with the factor "scaleFactor"; automatically redrawn
Return:    -1 if tkRef of DrawObject is not in selectList, else 1
**********************************************************************/
int ASSISTANT::DrawObject::Select(int _x, int _y, float _zoomFactor)
{
   int 
      x1, y1,
      x2, y2;
   
   if (!visible) return 0;
   GetBBox(&x1,&y1,&x2,&y2);
   if ((_x*_zoomFactor < x1) || (_y*_zoomFactor < y1) || (_x*_zoomFactor > x2) || (_y*_zoomFactor > y2)) return 0;
   
   return 1;
}

ASSISTANT::DrawObject *ASSISTANT::DrawObject::GetGroup() 
{
   if (group)
      return group->GetGroup();
   else
      return NULL; 
}

void ASSISTANT::DrawObject::GetBBox(int *minX, int *minY, int *maxX, int *maxY)
{  
   if ((m_dX+m_dWidth) < m_dX) {
      *minX = (int)(m_dX+m_dWidth-3);
      *maxX = (int)(m_dX+3);
   }
   else {
      *minX = (int)(m_dX-3);
      *maxX = (int)(m_dX+m_dWidth+3);
   }
   if ((m_dY+m_dHeight) < m_dY) {
      *minY = (int)(m_dY+m_dHeight-3);
      *maxY = (int)(m_dY+3);
   }
   else {
      *minY = (int)(m_dY-3);
      *maxY = (int)(m_dY+m_dHeight+3);
   }
   
}

void ASSISTANT::DrawObject::GetBoundingBox(CRect &rcObject)
{
   rcObject.left = (int)m_dX;
   rcObject.right = (int)(m_dX + m_dWidth);
   rcObject.top = (int)m_dY;
   rcObject.bottom = (int)(m_dY + m_dHeight);
   
   rcObject.NormalizeRect();
}

bool ASSISTANT::DrawObject::IsInRegion(int _x1, int _y1, int _x2, int _y2)
{
   int 
      x1, y1,
      x2, y2;
   
   if (!visible) return false;
   
   GetBBox(&x1,&y1,&x2,&y2);
   if ((x1 < _x1) || (y1 < _y1) || (x2 > (_x2)) || (y2 > (_y2))) return false;
   
   return true;
}

bool ASSISTANT::DrawObject::Overlap(int _x, int _y, float _zoomFactor) 
{
   int 
      x1, y1,
      x2, y2;
   
   if (!visible) return false;
   
   GetBBox(&x1,&y1,&x2,&y2);
   if ((_x < x1*_zoomFactor) || 
      (_y < y1*_zoomFactor) || 
      (_x > x2*_zoomFactor) || 
      (_y > y2*_zoomFactor)) 
      return false;
   
   return true; //_canvas->Overlap(tkref, _x, _y);
}


void ASSISTANT::DrawObject::InsertInVisibilityOrder(CArray<DrawObject *, DrawObject *> &sortedList)
{
	if (sortedList.GetSize() == 0)
		sortedList.Add(this);
	else
	{
      for (int i = sortedList.GetSize()-1; i >= 0; --i)
      {
         if (GetOrder() >= sortedList[i]->GetOrder())
            break;
      }
		// "insert" inserts before "pos". Only if pos is the position of the first element and 
		// the order of the inserted object ist smaller than the position of the first list element 
		// insert before. In all other cases increment pos before inserting.
      
		if (sortedList[i]->GetOrder() <= GetOrder())
			++i;

		sortedList.InsertAt(i, this);
	}
   
}

bool ASSISTANT::DrawObject::IsIdentic(DrawObject *obj)
{
   // the same object is identic
   if (obj == this)
      return true;

   if (GetType() != obj->GetType())
      return false;

   if (GetX() != obj->GetX())
      return false;

   if (GetY() != obj->GetY())
      return false;

   if (GetWidth() != obj->GetWidth())
      return false;

   if (GetHeight() != obj->GetHeight())
      return false;
   
   // all parameters are identic
   return true;
}


bool ASSISTANT::DrawObject::IsInRect(CRect &rcSelection)
{
   if (!visible)
      return false; 

   CRect rcObject;
   GetBoundingBox(rcObject);

   if (rcObject.Width() > 0 && rcObject.Height() > 0)
   {
      CRect rcIntersection;
      rcIntersection.IntersectRect(rcSelection, rcObject);
      if (rcIntersection == rcObject)
         return true;
      else
         return false;
   }
   else if (rcObject.Width() == 0 && rcObject.Height() == 0)
   {
      CPoint objectPoint(rcObject.left, rcObject.top);
      BOOL bPtIsInRect = rcSelection.PtInRect(objectPoint);
      if (bPtIsInRect)
         return true;
      else
         return false;
   }
   else // (rcObject.Width() == 0 || rcObject.Height() == 0)
   {
      CPoint objectPoint1(rcObject.left, rcObject.top);
      CPoint objectPoint2(rcObject.right, rcObject.bottom);
      BOOL bPtIsInRect1 = rcSelection.PtInRect(objectPoint1);
      BOOL bPtIsInRect2 = rcSelection.PtInRect(objectPoint2);
      if (bPtIsInRect1 && bPtIsInRect2)
         return true;
      else
         return false;
   }
}

bool ASSISTANT::DrawObject::ContainsRect(CRect &rcSelection)
{
   if (!visible)
      return false; 

   CRect rcObject;
   GetBoundingBox(rcObject);

   if (rcSelection.Width() == 0 && rcSelection.Height() == 0)
   {
      CPoint ptSelection(rcSelection.left, rcSelection.top);
      if (rcObject.PtInRect(ptSelection))
         return true;
      else
         return false;
   }
   else
   {
      CRect rcIntersection;
      rcIntersection.IntersectRect(rcSelection, rcObject);
      
      
      if (rcIntersection == rcSelection)
         return true;
      else
         return false;
   }
}

bool ASSISTANT::DrawObject::ContainsPoint(Gdiplus::PointF &ptMouse)
{
   if (!visible)
      return false; 

   CRect rcObject;
   GetBoundingBox(rcObject);
   
   CPoint pt((int)ptMouse.X, (int)ptMouse.Y);

   if (rcObject.PtInRect(pt))
      return true;
   else
      return false;
   
}

void ASSISTANT::DrawObject::GetNormalizedCoordinates(double &dX, double &dY, 
                                                     double &dWidth, double &dHeight)
{
   dX = m_dX;
   dY = m_dY; 
   dWidth = m_dWidth;
   dHeight = m_dHeight;

   if (m_dWidth < 0) 
   {
      dX = m_dX + m_dWidth;
      dWidth = m_dWidth*(-1);
   } 

   if (m_dHeight < 0) 
   {
      dY = m_dY + m_dHeight;
      dHeight = m_dHeight*(-1);
   } 
   
}


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/


ASSISTANT::ColoredObject::ColoredObject(double _x, double _y, double _width, double _height, int _zPosition, 
                                        Gdiplus::ARGB argbLineColor, Gdiplus::ARGB argbFillColor, 
                                        int _lineWidth, Gdiplus::DashStyle gdipLineStyle, uint32 _id, 
                                        LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                                        : DrawObject(_x,_y,_width,_height, _zPosition,_id, _hyperlink, _currentDirectory, _linkedObjects) 
{
   // object color as GDI+ ARGB
   m_argbLineColor = argbLineColor;
   m_argbFillColor = argbFillColor;
   
   lineWidth = _lineWidth;
   m_gdipLineStyle = gdipLineStyle;
}

ASSISTANT::ColoredObject::~ColoredObject()
{
}

bool ASSISTANT::ColoredObject::IsFilled() 
{
   if (m_argbFillColor == 0) 
      return false;
   else 
      return true;
}

void ASSISTANT::ColoredObject::ChangeLineColor(Gdiplus::ARGB argbLineColor)
{
   if (!visible) 
      return;
   
   recorded = false;
   
   m_argbLineColor = argbLineColor;
}

void ASSISTANT::ColoredObject::ChangeFillColor(Gdiplus::ARGB argbFillColor)
{
   if (!visible) 
      return;
   
   recorded = false;
   
   m_argbFillColor = argbFillColor;
}

void ASSISTANT::ColoredObject::ChangeLineWidth(int _lineWidth)
{
   if (!visible) 
      return;
   
   recorded = false;
   
   lineWidth = _lineWidth;
}

void ASSISTANT::ColoredObject::ChangeLineStyle(Gdiplus::DashStyle gdipLineStyle)
{
   if (!visible) 
      return;
   
   recorded = false;
   
   m_gdipLineStyle = gdipLineStyle;
}

void ASSISTANT::ColoredObject::ComputeArrowConfig(int arrowConfig)
{
   iBeginArrowStyle = arrowConfig / 1000;
   iBeginArrowDimension = (arrowConfig % 1000) / 100;
   iEndArrowStyle = (arrowConfig % 100) / 10;
   iEndArrowDimension = (arrowConfig % 10);
}

void ASSISTANT::ColoredObject::SetArrowCaps(Gdiplus::Pen &pen)
{
   if (iBeginArrowStyle != 0)
   {
      Gdiplus::CustomLineCap *custBeginCap = GetLineCap(iBeginArrowDimension, iBeginArrowStyle);
      if (custBeginCap)
      {
         pen.SetCustomStartCap(custBeginCap);
         delete custBeginCap;
      }
   }
   
   if (iEndArrowStyle != 0)
   {
      Gdiplus::CustomLineCap *custEndCap = GetLineCap(iEndArrowDimension, iEndArrowStyle);
      if (custEndCap)
      {
         pen.SetCustomEndCap(custEndCap);
         delete custEndCap;
      }
   }   
}


Gdiplus::CustomLineCap *ASSISTANT::ColoredObject::GetLineCap(int iArrowDimension, int iArrowStyle)
{
   Gdiplus::CustomLineCap *pLineCap = NULL;

   Gdiplus::REAL rLength = 3;
   Gdiplus::REAL rWidth = 3; 
   switch (iArrowDimension)
   {
   case 1:
      rLength = 2;
      rWidth = 2;
      break;
   case 2:
      rWidth = 2;
      break;
   case 3:
      rLength = 5;
      rWidth = 2;
      break;
   case 4:
      rLength = 2;
      break;
   case 5:
      break;
   case 6:
      rLength = 5;
      break;
   case 7:
      rLength = 2;
      rWidth = 5;
      break;
   case 8:
      rWidth = 5;
      break;
   case 9:
      rLength = 5;
      rWidth = 5;
      break;
   }

   Gdiplus::PointF pts[5];
   Gdiplus::GraphicsPath arrowPath;
   switch (iArrowStyle)
   {
   case 1:
      pLineCap = (Gdiplus::CustomLineCap *)new Gdiplus::AdjustableArrowCap(rLength, rWidth, true);
      break;
   case 2:
      pts[0] = Gdiplus::PointF(-rWidth/2, -rLength);
      pts[1] = Gdiplus::PointF(0.0, 0.0);
      pts[2] = Gdiplus::PointF(rWidth/2, -rLength);
      arrowPath.AddLines(pts, 3);
      pLineCap = new Gdiplus::CustomLineCap(NULL, &arrowPath, Gdiplus::LineCapRound);
      pLineCap->SetStrokeJoin(Gdiplus::LineJoinMiter);
      pLineCap->SetStrokeCaps(Gdiplus::LineCapRound, Gdiplus::LineCapRound);
      break;
   case 3:
      pLineCap = (Gdiplus::CustomLineCap *)new Gdiplus::AdjustableArrowCap(rLength, rWidth, true);
      ((Gdiplus::AdjustableArrowCap *)pLineCap)->SetMiddleInset(1);
      break;
   case 4: // use two triangles to create the diamond
      pts[0] = Gdiplus::PointF(-rWidth/2, 0.0);
      pts[1] = Gdiplus::PointF(0.0, rLength/2);
      pts[2] = Gdiplus::PointF(rWidth/2, 0.0);
      arrowPath.AddPolygon(pts, 3);
      pts[0] = Gdiplus::PointF(-rWidth/2, 0.0);
      pts[1] = Gdiplus::PointF(0.0, -rLength/2);
      pts[2] = Gdiplus::PointF(rWidth/2, 0.0);
      arrowPath.AddPolygon(pts, 3);

      pLineCap = new Gdiplus::CustomLineCap(&arrowPath, NULL);
      break;
   case 5:
      arrowPath.AddEllipse(Gdiplus::RectF(-rWidth/2, -rLength/2, rWidth, rLength));
      pLineCap = new Gdiplus::CustomLineCap(&arrowPath, NULL);
      break;
   }

   if (pLineCap)   
      pLineCap->SetWidthScale(1.0);

   return pLineCap;
}

bool ASSISTANT::ColoredObject::IsIdentic(DrawObject *obj)
{
   // the same object is identic
   if (obj == this)
      return true;

   if (!DrawObject::IsIdentic(obj))
      return false;

   if (GetLineColor() != ((ColoredObject *)obj)->GetLineColor())
      return false;

   if (GetFillColor() != ((ColoredObject *)obj)->GetFillColor())
      return false;
   
   if (GetLineStyle() != ((ColoredObject *)obj)->GetLineStyle())
      return false;

   if (GetLineWidth() != ((ColoredObject *)obj)->GetLineWidth())
      return false;

   // all parameters are identic
   return true;
}
   
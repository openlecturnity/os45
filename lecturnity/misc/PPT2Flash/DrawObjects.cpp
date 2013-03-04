/*********************************************************************
program : mlb_objects.cpp
authors : Gabriela Maass
date    : 3.12.1999
desc    : Functions to draw and manipulate objects on a tcl-canvas

**********************************************************************/

#include "StdAfx.h"
#include  <math.h>


#include "DrawObjects.h"
#include "PageObject.h"


/**********************************************************************
Name:      ASSISTANT::DrawObject::DrawObject(double _x, double _y, double _width, double _height, int _id)
Param:     _x, _y:          Coordinates (in Pixel)
_width, _height: Dimensions (in Pixel)
_id:         Identification-number of the draw-object
Desc:      initializes variables
Return:  
**********************************************************************/
ASSISTANT::DrawObject::DrawObject(double _x, double _y, double _width, double _height, int _zPosition, UINT _id, 
                                  LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
{
   m_dX          = _x;
   m_dY          = _y;
   m_dWidth      = _width;
   m_dHeight     = _height;

   visible    = true;
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
}

ASSISTANT::DrawObject::~DrawObject() 
{
   hyperlink_.Empty();
   currentDirectory_.Empty();
}

void ASSISTANT::DrawObject::Zoom(float _zoomFactor)
{
   return;
}


bool ASSISTANT::DrawObject::HasHyperlink()
{
   if (hyperlink_.IsEmpty()) 
      return false; 
   
   return true;
}

bool ASSISTANT::DrawObject::HasObjectLinks()
{
   if (m_ssLinkedObjects.IsEmpty())
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
            char szId[256];
            itoa(m_iPPTId, szId, 10);
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

Gdiplus::ARGB ASSISTANT::DrawObject::MakeARGB(LPCTSTR color)
{
   CString sColor = color;
   if (sColor[0] == _T('#')) // color value from LSD
   {
      sColor.Replace(_T("#"), _T("0x"));
   }
   else
   {
      sColor = sColor.Right(sColor.GetLength() - 1);
   }
   Gdiplus::ARGB argbColor = _tcstoul(sColor, NULL, 16);
   BYTE red = (BYTE) (((DWORD)(argbColor >> 16) & 0x000000ff));
   BYTE green = (BYTE) (((DWORD)(argbColor >> 8) & 0x000000ff));
   BYTE blue = (BYTE) (((DWORD)(argbColor) & 0x000000ff));

   BYTE alpha = (BYTE)(((DWORD)(argbColor >> 24) & 0x000000ff));
   if (alpha == 0) // no alpha value given, set to solid
      argbColor += 0xff000000;

   return argbColor;

}

int ASSISTANT::DrawObject::CalculateAofLinestyle(LPCTSTR lineStyle)
{
   if (_tcslen(lineStyle) == 0 || _tcscmp(lineStyle, _T(" ")) == 0)
      return 0;
   else if (_tcscmp(lineStyle, _T(".")) == 0)
      return 1;
   else 
      return 2;
}

COLORREF ASSISTANT::DrawObject::CalculateColor(Color &color)
{
   BYTE red, green, blue;
   
   CString sColor = color;
   sColor = sColor.Right(sColor.GetLength() - 1);
   DWORD cColor = _tcstoul(sColor, NULL, 16);
   red = (BYTE) (((DWORD)(cColor >> 16) & 0x000000ff));
   green = (BYTE) (((DWORD)(cColor >> 8) & 0x000000ff));
   blue = (BYTE) (((DWORD)(cColor) & 0x000000ff));

   COLORREF clrColor = RGB(red, green, blue);

   return clrColor;
}

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/


ASSISTANT::ColoredObject::ColoredObject(double _x, double _y, double _width, double _height, int _zPosition, 
                                        LPCTSTR _color, LPCTSTR _fillColor, int _lineWidth, LPCTSTR _lineStyle, UINT _id, 
                                        LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                                        : DrawObject(_x,_y,_width,_height, _zPosition,_id, _hyperlink, _currentDirectory, _linkedObjects) 
{
   // object color in tcl-format "#rrggbb"
   color     = _color;
   fillColor = _fillColor;
  
   // object color as GDI+ ARGB
   m_argbLineColor = MakeARGB(color);
   if (fillColor != _T("none"))
      m_argbFillColor = MakeARGB(fillColor);
   
   lineWidth = _lineWidth;
   lineStyle = _lineStyle;
   m_gdipLineStyle = CalculateAofLinestyle(lineStyle);
   ConvertLinestyleToDash();
}

ASSISTANT::ColoredObject::~ColoredObject()
{
   if (!color.IsEmpty())     
      color.Empty();
   
   if (!fillColor.IsEmpty()) 
      fillColor.Empty();
   
   if (!lineStyle.IsEmpty())
      lineStyle.Empty();
}

bool ASSISTANT::ColoredObject::IsFilled() 
{
   if (fillColor == _T("none")) 
      return false;
   else 
      return true;
}

void ASSISTANT::ColoredObject::ConvertLinestyleToDash() 
{
   if (m_gdipLineStyle == 1)
      m_gdipDashStyle = Gdiplus::DashStyleDot;
   else if (m_gdipLineStyle == 2)
      m_gdipDashStyle = Gdiplus::DashStyleDash;
   else
      m_gdipDashStyle = Gdiplus::DashStyleSolid;
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
   
#include "StdAfx.h"

#include "objects.h"

DrawSdk::CharArray::CharArray(int iInitialSize, int iIncreaseSize)
{
   if (iInitialSize <= 0)
      iInitialSize = 1024;

   m_iBufferSize = iInitialSize;
   m_iIncreaseSize = iIncreaseSize;
   m_iBufferContent = 0;

   m_tszContent = new _TCHAR[iInitialSize];
}

DrawSdk::CharArray::~CharArray()
{
   if (m_tszContent != NULL)
      delete[] m_tszContent;
   m_tszContent = NULL;
}

void DrawSdk::CharArray::AppendData(_TCHAR *tszData, int iLength)
{
   int iStringLength = iLength < 0 ? _tcslen(tszData): iLength;

   if (m_iBufferContent + iStringLength > m_iBufferSize)
   {
      // make room in the buffer

      int iNewSize = m_iBufferSize;
      
      do
      {
         if (m_iIncreaseSize < 0)
            iNewSize *= 2;
         else
            iNewSize += m_iIncreaseSize;
      }
      while (m_iBufferContent + iStringLength > iNewSize);

      _TCHAR *tszNewArray = new _TCHAR[iNewSize];
      _tcsncpy(tszNewArray, m_tszContent, m_iBufferContent);
      delete[] m_tszContent;
      m_tszContent = tszNewArray;
      m_iBufferSize = iNewSize;
   }

   // "normal" mode: just copy it to the buffer
   _tcsncpy(m_tszContent + m_iBufferContent, tszData, iStringLength);
   m_iBufferContent += iStringLength;
}


DrawSdk::DPoint::DPoint()
{
   x = 0.0;
   y = 0.0;
}

DrawSdk::DPoint::DPoint(double xPos, double yPos)
{
   x = xPos;
   y = yPos;
}


DrawSdk::DrawObject::DrawObject() 
{
   Init();
}

DrawSdk::DrawObject::DrawObject(double x, double y, double width, double height)
{
   Init();

   x_ = x;
   y_ = y;
   width_  = width;
   height_ = height;

   objectNumber_ = -1;
}

DrawSdk::DrawObject::DrawObject(Gdiplus::RectF *rect)
{
   Init();

   x_ = rect->X;
   y_ = rect->Y;
   width_  = rect->Width;
   height_ = rect->Height;

   objectNumber_ = -1;
}

DrawSdk::DrawObject::~DrawObject()
{
   if (m_tszVisibility != NULL)
      delete[] m_tszVisibility;
}

void DrawSdk::DrawObject::Init()
{
   x_ = 0;
   y_ = 0;
   width_  = 0;
   height_ = 0;
   m_bDndMoveable = false;
   m_tszVisibility = NULL;
}

void DrawSdk::DrawObject::CopyInteractiveFields(DrawObject *pObject)
{
   if (pObject != NULL)
   {
      pObject->m_bDndMoveable = m_bDndMoveable;
      
      if (pObject->m_tszVisibility != NULL)
         delete[] pObject->m_tszVisibility;

      if (m_tszVisibility != NULL)
      {
         int iLength = _tcslen(m_tszVisibility);
         pObject->m_tszVisibility = new _TCHAR[iLength + 1];
         _tcsncpy(pObject->m_tszVisibility, m_tszVisibility, iLength + 1);
      }
   }
}

_TCHAR *DrawSdk::DrawObject::GetVisibility(_TCHAR *tszTarget)
{
   if (tszTarget != NULL)
   {
      if (m_tszVisibility != NULL)
      {
         _tcscpy(tszTarget, m_tszVisibility);
         return tszTarget;
      }
      else
         return NULL;
   }
   else
      return m_tszVisibility;
}
      
bool DrawSdk::DrawObject::SetVisibility(LPCTSTR tszVisible)
{
   if (tszVisible != NULL)
   {
      if (m_tszVisibility != NULL)
         delete[] m_tszVisibility;

      int iLength = _tcslen(tszVisible);
      m_tszVisibility = new _TCHAR[iLength + 1];

      _tcsncpy(m_tszVisibility, tszVisible, iLength + 1);
   }

   return true;
}

void DrawSdk::DrawObject::Move(double deltaX, double deltaY)
{
   x_ += deltaX;
   y_ += deltaY;
}

bool DrawSdk::DrawObject::Contains(double dPosX, double dPosY)
{
   return dPosX >= x_ && dPosX <= x_ + width_ && dPosY >= y_ && dPosY <= y_ + height_;
}

void DrawSdk::DrawObject::GetLogicalBoundingBox(RECT *pRect)
{
   if (!pRect)
      return;

   pRect->left = (int) GetX();
   pRect->top = (int) GetY();
   pRect->right = pRect->left + (int) GetWidth();
   pRect->bottom = pRect->top + (int) GetHeight();

   if (pRect->left > pRect->right)
   {
      int tmp = pRect->left;
      pRect->left = pRect->right;
      pRect->right = tmp;
   }
   if (pRect->top > pRect->bottom)
   {
      int tmp = pRect->top;
      pRect->top = pRect->bottom;
      pRect->bottom = tmp;
   }
}

void DrawSdk::DrawObject::GetScreenBoundingBox(RECT *pRect, double xOff, double yOff, ZoomManager *pZoom)
{
   if (NULL == pRect)
      return;

   GetLogicalBoundingBox(pRect);
   pRect->left = (int) (xOff + pZoom->GetScreenX(pRect->left));
   pRect->top = (int) (yOff + pZoom->GetScreenY(pRect->top));
   pRect->right = (int) (xOff + pZoom->GetScreenX(pRect->right));
   pRect->bottom = (int) (yOff + pZoom->GetScreenY(pRect->bottom));
}


Gdiplus::ARGB DrawSdk::DrawObject::ReverseColor(Gdiplus::ARGB color)
{
   Gdiplus::ARGB argbColor = color;

   /*
   BYTE alpha = (B
   BYTE red   = (BYTE) (((Gdiplus::ARGB)(argbColor >> 16) & 0x000000ff));
   BYTE green = (BYTE) (((Gdiplus::ARGB)(argbColor >> 8) & 0x000000ff));
   BYTE blue  = (BYTE) (((Gdiplus::ARGB)(argbColor) & 0x000000ff));

   argbColor = Gdiplus::Color::MakeARGB(red, green, blue);
   */

   return argbColor;

}

/******************************/

DrawSdk::ColorObject::ColorObject() 
{
   Init();
   SetFillColor();
}

DrawSdk::ColorObject::ColorObject(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle) : DrawObject(x, y, width, height)
{
   Init();

   lineWidth_ = lineWidth;
   _ASSERT(lineWidth >= 0 && lineWidth <= 1000);
   lineStyle_ = lineStyle;

   SetLineColor(outlineColor);
   SetFillColor(fillColor);
   ConvertLinestyleToDash();

}

DrawSdk::ColorObject::ColorObject(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle) : DrawObject(x, y, width, height)
{
   Init();

   lineWidth_ = lineWidth;
   _ASSERT(lineWidth >= 0 && lineWidth <= 1000);
   lineStyle_ = lineStyle;

   SetLineColor(outlineColor);
   SetFillColor();
   ConvertLinestyleToDash();
}

DrawSdk::ColorObject::ColorObject(Gdiplus::RectF *rect, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle) : DrawObject(rect)
{
   Init();

   lineWidth_ = lineWidth;
   _ASSERT(lineWidth >= 0 && lineWidth <= 1000);
   lineStyle_ = lineStyle;

   SetLineColor(outlineColor);
   SetFillColor(fillColor);
   ConvertLinestyleToDash();
}

DrawSdk::ColorObject::ColorObject(Gdiplus::RectF *rect, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle) : DrawObject(rect)
{
   Init();

   lineWidth_ = lineWidth;
   _ASSERT(lineWidth >= 0 && lineWidth <= 1000);
   lineStyle_ = lineStyle;

   SetLineColor(outlineColor);
   SetFillColor();
   ConvertLinestyleToDash();
}

DrawSdk::ColorObject::~ColorObject()
{
}

void DrawSdk::ColorObject::Init()
{
   m_argbLineColor      = 0;
   m_argbFillColor      = 0;
   m_gdipDashStyle      = Gdiplus::DashStyleSolid;
   lineWidth_           = 0; 
   lineStyle_           = 0;
   arrowStyle_          = 0;
   iBeginArrowStyle     = 0;
   iBeginArrowDimension = 0;
   iEndArrowStyle       = 0;
   iEndArrowDimension   = 0;
   penStyle_            = PS_SOLID;
   brushStyle_          = BS_SOLID;
   brushHatch_          = 0;
   transparent_         = false;
   useXor_              = false;
   isFilled_            = false;
}

COLORREF DrawSdk::ColorObject::GetOutlineColorRef()
{ 
   BYTE a = (m_argbLineColor >> 24) & 0x000000ff;
   BYTE r = (m_argbLineColor >> 16) & 0x000000ff;
   BYTE g = (m_argbLineColor >> 8) & 0x000000ff;
   BYTE b = (m_argbLineColor) & 0x000000ff;

   COLORREF ret = RGB(r, g, b);

   return ret;
}

COLORREF DrawSdk::ColorObject::GetFillColorRef()
{  
   BYTE a = (m_argbFillColor >> 24) & 0x000000ff;
   BYTE r = (m_argbFillColor >> 16) & 0x000000ff;
   BYTE g = (m_argbFillColor >> 8) & 0x000000ff;
   BYTE b = (m_argbFillColor) & 0x000000ff;

   COLORREF ret = RGB(r, g, b);

   return ret;
}

void DrawSdk::ColorObject::SetLineColor(Gdiplus::ARGB outlineColor) 
{
   BYTE alpha = (BYTE) (((DWORD)(outlineColor >> 24) & 0x000000ff));
   if (alpha == 0) // outlineColor is in COLORREF-Format
   {
      BYTE r = (outlineColor) & 0x000000ff;
      BYTE g = (outlineColor >> 8) & 0x000000ff;
      BYTE b = (outlineColor >> 16) & 0x000000ff;
      m_argbLineColor = Gdiplus::Color::MakeARGB(255, r, g, b);
   }
   else
   {
      m_argbLineColor = outlineColor;
   }
}

void DrawSdk::ColorObject::SetFillColor() 
{
   isFilled_ = false;
}

void DrawSdk::ColorObject::SetFillColor(Gdiplus::ARGB fillColor) 
{
   BYTE alpha = (BYTE) (((DWORD)(fillColor >> 24) & 0x000000ff));
   if (alpha == 0) // outlineColor is in COLORREF-Format
   {
      BYTE r = (fillColor) & 0x000000ff;
      BYTE g = (fillColor >> 8) & 0x000000ff;
      BYTE b = (fillColor >> 16) & 0x000000ff;
      m_argbFillColor = Gdiplus::Color::MakeARGB(255, r, g, b);
   }
   else
   {
      m_argbFillColor = fillColor;
   }

   isFilled_ = true;
}

void DrawSdk::ColorObject::SetLineStyle(int lineStyle)
{
   lineStyle_ = lineStyle;
   ConvertLinestyleToDash();
}

void DrawSdk::ColorObject::SetArrowStyle(int arrowStyle)
{
   arrowStyle_ = arrowStyle;
}

void DrawSdk::ColorObject::SetArrowConfig(int arrowConfig)
{
   ComputeArrowConfig(arrowConfig);
}

int DrawSdk::ColorObject::GetArrowConfig()
{
   int iArrowConfig;
   iArrowConfig = iBeginArrowStyle * 1000 + iBeginArrowDimension * 100 + 
                  iEndArrowStyle * 10 + iEndArrowDimension;

   return iArrowConfig;
}

void DrawSdk::ColorObject::ConvertLinestyleToDash() 
{
   if (lineStyle_ == 1)
      m_gdipDashStyle = Gdiplus::DashStyleDot;
   else if (lineStyle_ == 2)
      m_gdipDashStyle = Gdiplus::DashStyleDash;
   else
      m_gdipDashStyle = Gdiplus::DashStyleSolid;
}

void DrawSdk::ColorObject::GetLogicalBoundingBox(RECT *pRect)
{
   if (NULL == pRect)
      return;
   DrawObject::GetLogicalBoundingBox(pRect);
   int nPad = lineWidth_ + 1; //(lineWidth_ / 2) + 1;
   if (nPad < 4)
      nPad = 4;
   pRect->left -= nPad;
   if (pRect->left < 0)
      pRect->left = 0;
   pRect->top -= nPad;
   if (pRect->top < 0)
      pRect->top = 0;
   pRect->right += nPad;
   pRect->bottom += nPad;
}
  
void DrawSdk::ColorObject::ComputeArrowConfig(int arrowConfig)
{
   iBeginArrowStyle = arrowConfig / 1000;
   iBeginArrowDimension = (arrowConfig % 1000) / 100;
   iEndArrowStyle = (arrowConfig % 100) / 10;
   iEndArrowDimension = (arrowConfig % 10);
}

void DrawSdk::ColorObject::SwapArrows()
{
   if (arrowStyle_ == 1)
      arrowStyle_ = 2;
   else if (arrowStyle_ == 2)
      arrowStyle_ = 1;
   
   byte tmpByte = iBeginArrowStyle;
   iBeginArrowStyle = iEndArrowStyle;
   iEndArrowStyle = tmpByte;

   tmpByte = iBeginArrowDimension;
   iBeginArrowDimension = iEndArrowDimension;
   iEndArrowDimension = tmpByte;
}

Gdiplus::CustomLineCap *DrawSdk::ColorObject::GetLineCap(int iArrowDimension, int iArrowStyle)
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

void DrawSdk::ColorObject::SetArrowCaps(Gdiplus::Pen &pen)
{
   if (arrowStyle_ == 0)
      return;

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


DrawSdk::TargetArea::TargetArea(double dX, double dY, double dWidth, double dHeight, LPCTSTR tszName, int iBelongsKey) : DrawObject(dX, dY, dWidth, dHeight)
{
   m_iBelongsKey = iBelongsKey;

   m_tszName = NULL;

   if (tszName != NULL)
   {
      int iLength = _tcslen(tszName);
      m_tszName = new _TCHAR[iLength + 1];
      ZeroMemory((void *)m_tszName, (iLength + 1) * sizeof _TCHAR);
      _tcsncpy(m_tszName, tszName, iLength);
   }
   else
   {
      m_tszName = new _TCHAR[1];
      m_tszName[0] = 0;
   }
}

DrawSdk::TargetArea::~TargetArea()
{
   if (m_tszName != NULL)
      delete[] m_tszName;
}

DrawSdk::DrawObject* DrawSdk::TargetArea::Copy()
{
   TargetArea *pNew = new TargetArea(x_, y_, width_, height_, m_tszName, m_iBelongsKey);
   CopyInteractiveFields(pNew);
   return pNew;
}

double DrawSdk::TargetArea::GetCenterX()
{
   return x_ + (width_ - 1) / 2;
}

double DrawSdk::TargetArea::GetCenterY()
{
   return y_ + (height_ - 1) / 2;
}

void DrawSdk::TargetArea::Draw(HDC hdc, double dOffX, double dOffY, ZoomManager *pZm)
{
   if (width_ == 0 && height_ == 0)
      return;

   Gdiplus::Graphics graphics(hdc);
   graphics.TranslateTransform(dOffX, dOffY);
   if (pZm != NULL)
   {
      double zoom = pZm->GetZoomFactor();
      graphics.ScaleTransform(zoom, zoom);
   }
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

   Gdiplus::Rect rcSize(x_, y_, width_, height_);
   // not drawn by the user: width and height should not be negative
   rcSize.Width -= 1;
   rcSize.Height -= 1;

   Gdiplus::HatchBrush brshBackground(Gdiplus::HatchStyleNarrowVertical, 
      Gdiplus::Color(128, 247, 215, 191), Gdiplus::Color(0, 247, 215, 191));
   graphics.FillRectangle(&brshBackground, rcSize);
   
   Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 1.0f);
   // otherwise nothing gets painted:
   pen.SetDashStyle(Gdiplus::DashStyleSolid);
   graphics.DrawRectangle(&pen, rcSize);
   
   /* old code: simple rectangle
   double dX = x_;
   double dY = y_;
   double dWidth = width_;
   double dHeight = height_;

   // Gdiplus only draws rectangles with positive width and height
   if (width_ < 0)
   {
      dX     = x_ + width_;
      dWidth = -width_;
   }
   if (height_ < 0)
   {
      dY = y_ + height_;
      dHeight = -height_;
   }

   Gdiplus::Color lineColor(255, 0, 0, 0);
   Gdiplus::Pen pen(lineColor, 1);
   pen.SetDashStyle(Gdiplus::DashStyleSolid);
   graphics.DrawRectangle(&pen, (Gdiplus::REAL)dX, (Gdiplus::REAL)dY, 
      (Gdiplus::REAL)dWidth, (Gdiplus::REAL)dHeight);
   //*/
}

void DrawSdk::TargetArea::Write(CharArray *pArray, bool bUseFullImagePath)
{
   if (pArray == NULL)
      return;

   static _TCHAR tszVisibility[100];
   tszVisibility[0] = 0;
   if (HasVisibility())
      _stprintf(tszVisibility, _T(" visible=\"%s\""), GetVisibility());
   
   static _TCHAR tszBelongs[100];
   tszBelongs[0] = 0;
   _stprintf(tszBelongs, _T(" belongs=\"%d\""), m_iBelongsKey);
   
   static _TCHAR tszAreaDefine[500];
   tszAreaDefine[0] = 0;
   _stprintf(tszAreaDefine, 
      _T("<TARGET x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" name=\"%s\"%s%s></TARGET>\n"),
      (int)x_, (int)y_, (int)width_, (int)height_, m_tszName, tszBelongs, tszVisibility);

   pArray->AppendData(tszAreaDefine);
}


/*********************************************************************

  program  : mlb_text.cpp
  authors  : Gabriela Maass
  date     : 3.12.1999 last changes 14.03.2001   
  revision : $Id: mlb_text.cpp,v 1.34 2009-07-18 18:07:11 fiscu Exp $
  desc     : Functions to draw and manipulate objects on a tcl-canvas
  
**********************************************************************/

#include "StdAfx.h"
#include "mlb_objects.h"
#include "mlb_page.h"
#include "la_project.h"
#include "mlb_misc.h"
#include "lutils.h"

//#include <wingdi.h>
//#define DEBUG

#define FLAG_UNDERLINE 16
   
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/


ASSISTANT::Text::Text(double _x, double _y, double _width, double _height, int _zPosition, 
                      Gdiplus::ARGB argbLineColor, LPCTSTR _textString, LPCTSTR _family, LPCTSTR _size, 
                      LPCTSTR _weight, LPCTSTR _slant, int _textStyle, uint32 _id, 
                      LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects, Gdiplus::ARGB argbLinkColor)
                : ColoredObject(_x, _y, _width, _height, _zPosition, 
                                argbLineColor, 0, 0, Gdiplus::DashStyleSolid, _id, 
                                _hyperlink, _currentDirectory, _linkedObjects)
{
   fontFamily   = _family;
   fontSize     = _size;
   fontWeight   = _weight;
   fontSlant    = _slant;
   
   textStyle = _textStyle;
   
   offY = 0;
   drawSize = _ttoi(_size);
   
   text.Empty();
   if (_textString) 
   {
      text = _textString;
   }
   
   actPos = EndPos();
   if (text.IsEmpty())
      firstPos = true;
   else
      firstPos = false;
   
   m_bHasSupportedFont = true;
   m_bTextWidthIsStatic = true;
   
   m_bIsUnderlined = (textStyle & FLAG_UNDERLINE) == 0 ? false : true;
   
   // Compute dimension
   FillLogFont(m_logFont, fontFamily, drawSize, fontWeight, fontSlant, m_bIsUnderlined);
   fAscent  = DrawSdk::Text::GetTextAscent(&m_logFont);
   fDescent = DrawSdk::Text::GetTextDescent(&m_logFont);
   if (m_dWidth == 0)
   {
      m_bTextWidthIsStatic = false;
      if (text.GetLength() > 0)
         m_dWidth = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
   }
   m_dHeight = fAscent + fDescent;
   
   if (_hyperlink)
      hyperlink_ = _hyperlink;
   else
      hyperlink_.Empty();
   
   m_argbActivatedLinkColor = argbLinkColor;
   
   linkWasActivated = false;

   m_bHasReturnAtEnd = false;
}

ASSISTANT::Text::~Text() 
{
   fontFamily.Empty();
   fontSize.Empty();
   fontWeight.Empty();
   fontSlant.Empty();
   textPosition.Empty();
   text.Empty();
}


double ASSISTANT::Text::GetWidth()
{
   return m_dWidth;
}


double ASSISTANT::Text::GetHeight()
{
   return m_dHeight;
}


float ASSISTANT::Text::GetAscent()
{
   return fAscent;
}


float ASSISTANT::Text::GetDescent()
{
   return fDescent;
} 


bool ASSISTANT::Text::IsVisible()
{
   if (visible) 
      return true; 
   else
      return false;
}


void ASSISTANT::Text::Update()
{ 
   m_dWidth  = GetWidth();
   m_dHeight = GetHeight();
}

LPCTSTR ASSISTANT::Text::GetString()
{ 
   return text;
}


int ASSISTANT::Text::EndPos()
{ 
   return (text.GetLength()-1);
}


Gdiplus::ARGB ASSISTANT::Text::GetColor()
{ 
   return m_argbLineColor;
}


LPCTSTR ASSISTANT::Text::GetFamily()
{ 
   //if (fontFamily == _T("Courier New"))
   //   return _T("Courier");
   
   return fontFamily;
}


LPCTSTR ASSISTANT::Text::GetSize()
{ 
   return fontSize;
}


LPCTSTR ASSISTANT::Text::GetWeight()
{ 
   return fontWeight;
}


LPCTSTR ASSISTANT::Text::GetSlant()
{ 
   return fontSlant;
}


void ASSISTANT::Text::ChangeString(LPCTSTR _textString)
{ 
   if (text == _textString)
      return;

   text.Empty();
   text = _textString;
   m_bTextWidthIsStatic = false;
   if (text.GetLength() > 0)
      m_dWidth  = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
   else
      m_dWidth = 0;
}

void ASSISTANT::Text::InsertNewChars(LPCTSTR _string)
{
   bool textEmpty = text.IsEmpty() == 0 ? false : true;
   
   int len = _tcslen(_string);
   _TCHAR *p = (_TCHAR *)_string;
   for (int i = 0; i < len; i++) 
   {
      ++actPos; 
      if (actPos > text.GetLength())
         actPos = text.GetLength();
      if (firstPos) 
      {
         text.Insert(0 , p);
         actPos = 0;
         firstPos = false;
      }
      else 
         text.Insert(actPos,p);
      if (m_bTextWidthIsStatic)
      {
         double zwWidth = DrawSdk::Text::GetCharWidth(*p, &m_logFont);
         m_dWidth += zwWidth;
      }
      else
      {
         if (text.GetLength() > 0)
            m_dWidth  = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
         else
            m_dWidth = 0;
      }
      ++p;
   }
   
   if (text.IsEmpty())
      firstPos = true;
   else
      firstPos = false;
   
}


bool ASSISTANT::Text::DeleteChar()
{
   if (!visible || text.IsEmpty()) 
      return false;
   
   
   if (actPos > 0)
   {
      --actPos;
      return DeleteNextChar();
   }
   
   if (actPos == 0)
   {
      if (text.GetLength() == 1)
      {
         text.Empty();
         m_bTextWidthIsStatic = false;
         m_dWidth  = 0;
      }
      else 
      {
         if (m_bTextWidthIsStatic)
         {
            int zwWidth = DrawSdk::Text::GetCharWidth(text[actPos], &m_logFont);
            m_dWidth -= zwWidth;
         }
         else
         {
            if (text.GetLength() > 0)
               m_dWidth  = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
            else
               m_dWidth = 0;
         }
         
         text.Delete(actPos, 1);
      }
      
      actPos = 0;
      firstPos = true;
      
      return true;
   }
   
   return false;
}


bool ASSISTANT::Text::DeleteNextChar()
{
   if (!visible || text.IsEmpty()) 
      return false;
   
   if (firstPos)
   {
      if (m_bTextWidthIsStatic)
      {
         int zwWidth = DrawSdk::Text::GetCharWidth(text[0], &m_logFont);
         m_dWidth -= zwWidth;
      }
      else
      {
         if (text.GetLength() > 0)
            m_dWidth  = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
         else
            m_dWidth = 0;
      }
      
      text.Delete(0,1);
      actPos = 0;
      return true;
   }
   
   if (actPos < EndPos())
   {
      if (m_bTextWidthIsStatic)
      {
         int zwWidth = DrawSdk::Text::GetCharWidth(text[actPos+1], &m_logFont);
         m_dWidth -= zwWidth;
      }
      else
      {
         if (text.GetLength() > 0)
            m_dWidth  = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
         else
            m_dWidth = 0;
      }
      
      text.Delete(actPos+1,1);
      return true;
   }
   
   return false;
}

void ASSISTANT::Text::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;
   
   if (text.IsEmpty())
      return;

   if (group != NULL && group->IsSelected() &&
       group->GetType() == GROUP && ((Group *)group)->IsTextGroup())
   {
      return;
   }

   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

   Gdiplus::FontFamily fontFamily(m_logFont.lfFaceName);
   //gdipFont.GetFamily(&fontFamily);

   int fontStyle = GetValidFontStyle(fontFamily);

   //Gdiplus::Font gdipFont(pDC->m_hDC, &m_logFont);
   Gdiplus::Font gdipFont(&fontFamily, abs(m_logFont.lfHeight), fontStyle, Gdiplus::UnitPixel);

   Gdiplus::REAL ascent = fontFamily.GetCellAscent(fontStyle) * 
                          (gdipFont.GetSize() / fontFamily.GetEmHeight(fontStyle));

   Gdiplus::PointF drawPoint;
   drawPoint.X = m_dX;
   drawPoint.Y = m_dY - ascent;

   // convert text string to WCHAR format
   int iStringLength = text.GetLength();
   WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
#ifdef _UNICODE
   wcscpy(wcString, text);
#else
   MultiByteToWideChar(CP_ACP, 0, text, iStringLength+1, 
                       wcString, iStringLength+1);
#endif


   // compute character positions
   Gdiplus::StringFormat measureStringFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
   measureStringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);

   Gdiplus::RectF bbox;
   graphics.MeasureString(wcString, iStringLength, &gdipFont, Gdiplus::PointF(0.0, 0.0), &measureStringFormat, &bbox);

   Gdiplus::RectF *pCharacterRects = new Gdiplus::RectF[iStringLength];
   bool bPositionCorrection = true;
   int iEnlargmentCount = 0;
   for (int i = 0; i < iStringLength; ++i)
   {
      Gdiplus::CharacterRange characterRange;
      characterRange.First = i;
      characterRange.Length = 1;

      measureStringFormat.SetMeasurableCharacterRanges(1, &characterRange);

      Gdiplus::Region characterRegion;
      graphics.MeasureCharacterRanges(wcString, iStringLength, &gdipFont, bbox, 
                                      &measureStringFormat, 1, &characterRegion);
      if (characterRegion.IsEmpty(&graphics))
      {
         if (iEnlargmentCount < 10)
         {
            --i;
            bbox.Width += 10;
            ++iEnlargmentCount;
         }
         else
            bPositionCorrection = false;
      }
      else
      {
         characterRegion.GetBounds(&pCharacterRects[i], &graphics);
      }
   }

   if (bPositionCorrection)
   {
      double xOffset = pCharacterRects[0].X;

      for (int i = 0; i < iStringLength; ++i)
         pCharacterRects[i].Offset(-xOffset, 0);
   }

   // compute scale factor for x position
   Gdiplus::CharacterRange stringRange(0, iStringLength);
   measureStringFormat.SetMeasurableCharacterRanges(1, &stringRange);
   Gdiplus::Region stringRegion;
   graphics.MeasureCharacterRanges(wcString, iStringLength,
                                   &gdipFont, bbox, &measureStringFormat, 1, &stringRegion);
   // get bounding rectangle
   Gdiplus::RectF stringRect;
   stringRegion.GetBounds(&stringRect, &graphics);

   Gdiplus::REAL realTextWidth = stringRect.Width;
   if (iStringLength > 1 && m_dWidth != 0 && realTextWidth != m_dWidth)
   {
      Gdiplus::REAL scaleX = (m_dWidth / realTextWidth);

      if (bPositionCorrection)
      {
         for (int j = 0; j < iStringLength; ++j)
         {        
            pCharacterRects[j].X = pCharacterRects[j].X * scaleX;
         }
      }
      else
      {
         graphics.ScaleTransform(scaleX, 1.0);
         drawPoint.X = m_dX / scaleX;
      }
   }

   // the text should begin exact at m_dX, m_dY
   drawPoint.X -= stringRect.X;

   // Move and Scale
   graphics.TranslateTransform(dOffX, dOffY);
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   Gdiplus::Color clrText(m_argbLineColor);
   Gdiplus::SolidBrush solidBrush(clrText);

   measureStringFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoClip);
   Gdiplus::PointF charPoint;
   if (bPositionCorrection)
   {
      for (int j = 0; j < iStringLength; ++j)
      { 
         charPoint.X = drawPoint.X + pCharacterRects[j].X;
         charPoint.Y = drawPoint.Y;
         graphics.DrawString(&wcString[j], 1, &gdipFont, charPoint, &measureStringFormat, &solidBrush);
      }
   }
   else
      graphics.DrawString(wcString, iStringLength, &gdipFont, drawPoint, &measureStringFormat, &solidBrush);
 
   if (pCharacterRects)
      delete[] pCharacterRects;

   if (wcString)
      free(wcString);
}

void ASSISTANT::Text::ChangeFont(LPCTSTR _family, LPCTSTR _size, 
                                 LPCTSTR _weight, LPCTSTR _slant) 
{
   if (!visible) return;
   recorded = false;
   
   if ((fontFamily  == _family) &&
      (fontSize     == _size) &&
      (fontWeight   == _weight) &&
      (fontSlant    == _slant)) 
   {
      return;
   }
   
   fontFamily   = _family;
   if (fontFamily == _T("Courier"))
      fontFamily = _T("Courier New");
   
   fontSize     = _size;
   fontWeight   = _weight;
   fontSlant    = _slant;
   
   offY = 0;
   drawSize = _ttoi(_size);
   
   FillLogFont(m_logFont, fontFamily, drawSize, fontWeight, fontSlant);
   fAscent  = DrawSdk::Text::GetTextAscent(&m_logFont);
   fDescent = DrawSdk::Text::GetTextDescent(&m_logFont);
   m_bTextWidthIsStatic = false;
   if (text.GetLength() > 0)
      m_dWidth  = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
   else
      m_dWidth = 0;
   
   
   m_dHeight = fAscent + fDescent;
}

void ASSISTANT::Text::ChangeFontFamily(LPCTSTR szFontFamily)
{
   _tcscpy(m_logFont.lfFaceName, szFontFamily);
   
   fAscent  = DrawSdk::Text::GetTextAscent(&m_logFont);
   fDescent = DrawSdk::Text::GetTextDescent(&m_logFont);

   m_bTextWidthIsStatic = false;

   if (text.GetLength() > 0)
      m_dWidth  = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
   else
      m_dWidth = 0;
   
   m_dHeight = fAscent + fDescent;
}

void ASSISTANT::Text::ChangeFontSize(int iFontSize)
{
   m_logFont.lfHeight = -1 * abs(iFontSize);

   fAscent  = DrawSdk::Text::GetTextAscent(&m_logFont);
   fDescent = DrawSdk::Text::GetTextDescent(&m_logFont);

   m_bTextWidthIsStatic = false;

   if (text.GetLength() > 0)
      m_dWidth  = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
   else
      m_dWidth = 0;
   
   m_dHeight = fAscent + fDescent;
}

void ASSISTANT::Text::ChangeLineColor(Gdiplus::ARGB argbLineColor)
{
   if (!visible) 
      return;
   
   recorded = false;
   
   m_argbLineColor = argbLineColor;
}

int ASSISTANT::Text::GetPos()
{
   return actPos;
}

bool ASSISTANT::Text::GoRight(int actY) 
{
   if (!visible)
      return false;
   
   if (text.IsEmpty()) 
   {
      if (actY != GetY())
         return true;
      else 
         return false;
   }
   
   if (actPos == text.GetLength())
      return false;
   
   
   if (firstPos)
   {
      firstPos = false;
      return true;
   }
   
   ++actPos;
   if (actPos != text.GetLength()) 
   {
      return true;
   }
   
   return false;
}


bool ASSISTANT::Text::GoLeft(int actY) 
{
   if (!visible) 
      return false;
   
   if (text.IsEmpty())
   {
      if (actY != GetY())
         return true;
      else 
         return false;
   } 
   
   if (firstPos) 
      return false;
   
   if (actPos != 0) 
   {
      --actPos;
      return true;
   }
   
   return false; 
}


bool ASSISTANT::Text::GoEnd() 
{
   if (!visible || text.IsEmpty()) 
      return false;
   
   firstPos = false;
   actPos   = EndPos();
   
   return true;
}


bool ASSISTANT::Text::GoHome() 
{
   if (!visible || text.IsEmpty()) 
      return false;
   
   firstPos = true;
   actPos   = 0;
   
   return true;
}

void ASSISTANT::Text::GetBBox(int *minX, int *minY, int *maxX, int *maxY)
{  
   int textWidth = m_dWidth;
   if (textWidth == 0 && text.GetLength() > 0)
      textWidth = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
   *minX = m_dX - 3;
   *minY = m_dY - fAscent - 3;
   *maxX = m_dX + textWidth + 3;
   *maxY = m_dY + fDescent + 3;
}

void ASSISTANT::Text::GetBoundingBox(CRect &rcObject)
{
   int textWidth = m_dWidth;
   if (textWidth == 0 && text.GetLength() > 0)
      textWidth = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);

   rcObject.left = m_dX;
   rcObject.right = m_dX + textWidth;
   rcObject.top = m_dY - fAscent;
   rcObject.bottom = m_dY + fDescent;
   
   rcObject.NormalizeRect();
}

void ASSISTANT::Text::GetBoundingBox(Gdiplus::RectF &gdipRcObject)
{
   int textWidth = m_dWidth;
   if (textWidth == 0 && text.GetLength() > 0)
      textWidth = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);

   gdipRcObject.X = m_dX;
   gdipRcObject.Width = textWidth;
   gdipRcObject.Y = m_dY - fAscent;
   gdipRcObject.Height = m_dHeight;
   
}

bool ASSISTANT::Text::Overlap(int _x, int _y, float _zoomFactor) 
{
   int 
      x1, y1,
      x2, y2;
   
   if (!visible || text.IsEmpty()) return false;
   
   GetBBox(&x1,&y1,&x2,&y2);
   
   if ((_x < x1*_zoomFactor) || (_y < y1*_zoomFactor) || (_x > x2*_zoomFactor) || (_y > y2*_zoomFactor)) 
      return false;
   
   return true;
}

bool ASSISTANT::Text::IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditTyp, int &iPointIndex, double dZoomFactor) 
{
   return 0;
}

void ASSISTANT::Text::Hide()
{
   if (!visible) 
      return;

   visible = false;
   
   /*/
   _canvas->DeleteObject(tkref);
   /*/
}


ASSISTANT::DrawObject *ASSISTANT::Text::Copy(bool keepZPosition)
{
   ASSISTANT::Text 
      *ret;
   
   ret = new Text(m_dX, m_dY, m_dWidth, m_dHeight, -1, 
                  m_argbLineColor, text, fontFamily,
                  fontSize, fontWeight, fontSlant,
                  textStyle, Project::active->GetObjectID(), 
                  hyperlink_, currentDirectory_, m_ssLinkedObjects, m_argbActivatedLinkColor);
   
   if (isInternLink)
      ret->LinkIsIntern(true);
   else
      ret->LinkIsIntern(false);
   
   if (keepZPosition)
      ret->SetOrder(order);
   
   return (DrawObject *)ret;
}

void ASSISTANT::Text::Move(float _diffX, float _diffY)
{
   if (!visible) return;
   
   recorded = false;
   
   m_dX += _diffX;
   m_dY += _diffY;
}

int ASSISTANT::Text::Select(int _x, int _y, float _zoomFactor)
{
   //   std::list<ASSISTANT::Char *>::iterator 
   //     temp;
   //   int
   //     x1, y1, 
   //     x2, y2;
   
   //   if (!visible) return 0;
   
   //   GetBBox(&x1,&y1,&x2,&y2);
   //   if ((_x < x1) || (_y < y1) || (_x > x2) || (_y > y2)) return 0;
   
   //   temp = text.begin();
   //   while (temp != text.end()) {
   //     if ((*temp)->SelectObject(length,selectList) > 0) {
   //       actPos = temp;
   //       if ((*temp)->SelectObject(_x,_y)) {
   // 	return true;
   //       }
   //       selected = true;
   //       return 1;
   //     }
   //     temp++;
   //   }
   
   return 0;
}

void ASSISTANT::Text::SaveObject(FILE *fp, int level, const _TCHAR *path)
{
   if (!visible) return;
   
   WCHAR *tab = (WCHAR *)malloc((level*2+1)*sizeof(WCHAR));
   *tab = L'\0';
   for (int i = 0; i < level; i++) 
      wcscat(tab, L"  ");
   
   CString hyperlinkString;
   MakeHyperlinkString(hyperlinkString);
   WCHAR *wszHyperlink = ASSISTANT::ConvertTCharToWChar(hyperlinkString);

   CString csColor;
   ASSISTANT::GdipARGBToString(m_argbLineColor, csColor);
   WCHAR *wszLineColor = ASSISTANT::ConvertTCharToWChar(csColor);
   
   WCHAR *wszFontSize = ASSISTANT::ConvertTCharToWChar(fontSize);
   WCHAR *wszFontFamily = ASSISTANT::ConvertTCharToWChar(fontFamily);
   WCHAR *wszFontWeight = ASSISTANT::ConvertTCharToWChar(fontWeight);
   WCHAR *wszFontSlant = ASSISTANT::ConvertTCharToWChar(fontSlant);

   fwprintf(fp, L"%s<TEXT x=\"%d\" y=\"%d\" width=\"%d\" color=\"%s\" size=\"%s\" family=\"%s\" weight=\"%s\" slant=\"%s\" length=\"%d\" style=\"%d\" ZPosition=\"%d\"%s>\n",
                tab, (int)m_dX, (int)m_dY, (int)m_dWidth, 
                wszLineColor, wszFontSize, wszFontFamily, wszFontWeight, wszFontSlant,
                text.GetLength(), textStyle, (int)order,  wszHyperlink);
   
   fwprintf(fp, L"%s  \"",tab);
   
   fwprintf (fp, L"%s",text);
   
   fwprintf(fp, L"\"\n");
   fwprintf(fp, L"%s</TEXT>\n",tab);
   
   if (wszHyperlink)
      delete wszHyperlink;

   if (wszLineColor)
      delete wszLineColor;

   if (wszFontSize)
      delete wszFontSize;

   if (wszFontFamily)
      delete wszFontFamily;

   if (wszFontWeight)
      delete wszFontWeight;
   
   if (wszFontSlant)
      delete wszFontSlant;

   if (tab)
      delete tab;
}


void ASSISTANT::Text::SaveAsAOF(CFileOutput *fp)
{
   CString
      zwColor;
   
   if (!visible || text.IsEmpty() || recorded) return;
   
   recorded = true;
   
   recID = ASSISTANT::Project::active->recCount;
   ASSISTANT::Project::active->recCount++;
   
   FontList *fontList = ASSISTANT::Project::active->GetFontList();
   
   bool isBold = false;
   bool isItalic = false;
   if (fontWeight == _T("bold"))
   isBold = true;
   if (fontSlant == _T("italic"))
   isItalic = true;

   CString ssTmpFontFamily = fontFamily;
   if (!m_bHasSupportedFont)
   ssTmpFontFamily = _T("Arial");

   CString fontFilename;
   fontList->GetFontFilename(ssTmpFontFamily, fontFilename, isBold, isItalic);
   // We don't have to backslash anything in the file name string as it
   // is put in double quotes anyway.
   CString csFontFilename;
   StringManipulation::TransformForSgml(fontFilename, csFontFilename);

   CString csColor;
   ASSISTANT::GdipARGBToObjString(m_argbLineColor, csColor);

   CString line;

   line.Format(_T("<TEXT x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f>\n"),
               (int)m_dX, (int)(m_dY+offY), (int)m_dWidth, (int)m_dHeight,
               m_dX, m_dY+offY, m_dWidth, m_dHeight);
   fp->Append(line);

   line.Format(_T("<TEXTITEM x=%d y=%d xf=%2.2f yf=%2.2f offx=0 offy=0 font=\"%s\" size=%d weight=%s slant=%s style=%d "),
               (int)m_dX, (int)(m_dY+offY),
               m_dX, m_dY+offY,
               ssTmpFontFamily, drawSize, fontWeight, 
               fontSlant, textStyle);
   fp->Append(line);

   if (csFontFilename.GetLength() > 0)
   {
      line.Format(_T("ttf=\"%s\" rgb=0x%s>\n"), csFontFilename, csColor);
      fp->Append(line);
   }
   else
   {
      line.Format(_T("rgb=0x%s>\n"), csColor);
      fp->Append(line);
   }

   ssTmpFontFamily.Empty();

   for (int i = 0; i < text.GetLength(); ++i)
   {
      if (text[i] == _T('<') || text[i] == _T('>') || text[i] == _T('\\')) 
         line.Format(_T("\\%c"),text[i]);
      else
         line.Format(_T("%c"),text[i]);
      fp->Append(line);
   }

   line = _T("\n</TEXTITEM>\n</TEXT>\n");
   fp->Append(line);
}

int ASSISTANT::Text::RecordableObjects()
{
   if (!visible || text.IsEmpty()) return 0;
   
   return 1;
}

void ASSISTANT::Text::AdjustGui() 
{
}


void ASSISTANT::Text::SetActive(int idx) 
{
   if ((idx < 0) || (idx >= text.GetLength())) return;
   
   actPos = idx;
}


void ASSISTANT::Text::SetFirstActive() 
{
   actPos = 0;
   firstPos = true;
}


void ASSISTANT::Text::SetLastActive() 
{
   actPos = text.GetLength();
   
   if (text.IsEmpty()) 
      firstPos = true;
   else
   {
      --actPos;
      firstPos = false;
   }
}


void ASSISTANT::Text::SetLeftActive() 
{
   if (firstPos) 
      return;
   
   if (actPos != 0) 
   {
      --actPos;
      return;
   }
   
   firstPos = true;
}


void ASSISTANT::Text::SetRightActive() 
{
   if (text.IsEmpty()) 
      return;
   
   if (firstPos) 
   {
      firstPos = false;
      return;
   }
   if (actPos != text.GetLength()) 
   {
      firstPos = false;
      ++actPos;
   }
}

void ASSISTANT::Text::SetFontUnsupported()
{
   m_bHasSupportedFont = false;
   // Arial is default font
   FillLogFont(m_logFont, _T("Arial"), drawSize, fontWeight, fontSlant);
   fAscent  = DrawSdk::Text::GetTextAscent(&m_logFont);
   fDescent = DrawSdk::Text::GetTextDescent(&m_logFont);
   if (!m_bTextWidthIsStatic)
      if (text.GetLength() > 0)
         m_dWidth  = DrawSdk::Text::GetTextWidth(text, text.GetLength(), &m_logFont);
      else
         m_dWidth = 0;
   m_dHeight = fAscent+fDescent;
}  


ASSISTANT::DrawObject *ASSISTANT::Text::Load(SGMLElement *hilf) 
{
   Text
      *obj;
   TCHAR
      *tmp;
   double
      x, y,
      width, height;
   int
      zPosition,
      iPPTId;
   TCHAR 
      *color,
      *size,
      *family,
      *weight,
      *slant,
      *hyperlink,
      *internLink,
      *currentDirectory,
      *linkedObjects,
      *linkColor;
   TCHAR 
      *szEndPtr;   // needed for _tcstod

   tmp = hilf->GetAttribute("x");
   if (tmp) 
      x = _tcstod(tmp, &szEndPtr);
   else     
      x = 0;
   
   tmp = hilf->GetAttribute("y");
   if (tmp) 
      y = _tcstod(tmp, &szEndPtr);
   else     
      y = 0;
   
   tmp = hilf->GetAttribute("width");
   if (tmp) 
      width = _tcstod(tmp, &szEndPtr);
   else     
      width = 0;
   
   tmp = hilf->GetAttribute("height");
   if (tmp) 
      height = _tcstod(tmp, &szEndPtr);
   else     
      height = 0;
   
   tmp = hilf->GetAttribute("color");
   if (tmp) 
      color = _tcsdup(tmp);
   else     
      color = _tcsdup(_T("#ff0000"));
   
   tmp = hilf->GetAttribute("size");
   if (tmp) 
      size = _tcsdup(tmp);
   else     
      size = _tcsdup(_T("20"));
   
   tmp = hilf->GetAttribute("family");
   if (tmp) 
      family = _tcsdup(tmp);
   else     
      family = _tcsdup(_T("Arial"));
   
   tmp = hilf->GetAttribute("weight");
   if (tmp) 
      weight = _tcsdup(tmp);
   else     
      weight = _tcsdup(_T("normal"));
   
   tmp = hilf->GetAttribute("slant");
   if (tmp) 
      slant = _tcsdup(tmp);
   else     
      slant = _tcsdup(_T("roman"));
   
   tmp = hilf->GetAttribute("address");
   if (tmp) 
      hyperlink = _tcsdup(tmp);
   else
      hyperlink = NULL;
   
   tmp = hilf->GetAttribute("intern");
   if (tmp) 
      internLink = _tcsdup(tmp);
   else
      internLink = NULL;
   
   tmp = hilf->GetAttribute("path");
   if (tmp) 
      currentDirectory = _tcsdup(tmp);
   else
      currentDirectory = NULL;
   
   tmp = hilf->GetAttribute("linkedObjects");
   if (tmp)
      linkedObjects = _tcsdup(tmp);
   else
      linkedObjects = NULL;
   
   tmp = hilf->GetAttribute("id");
   if (tmp) iPPTId = _ttoi(tmp);
   else     iPPTId = -1;
   
   tmp = hilf->GetAttribute("linkcolor");
   if (tmp) 
      linkColor = _tcsdup(tmp);
   else
      linkColor = NULL;
   
   int textStyle = 0;
   tmp = hilf->GetAttribute("style");
   if (tmp) 
      textStyle = _ttoi(tmp);
   
   tmp = hilf->GetAttribute("ZPosition");
   if (tmp) zPosition = _ttoi(tmp);
   else     zPosition = -1;
   
   Gdiplus::ARGB argbLineColor = ASSISTANT::StringToGdipARGB(color);
   Gdiplus::ARGB argbLinkColor = ASSISTANT::StringToGdipARGB(linkColor);

   if (hyperlink != NULL)
   {
      obj = new Text(x, y, width, height, zPosition, 
                     argbLineColor, hilf->Parameter, family, size, weight, slant,
                     textStyle, Project::active->GetObjectID(), 
                     hyperlink, currentDirectory, linkedObjects, argbLinkColor);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new Text(x, y, width, height, zPosition, 
                     argbLineColor, hilf->Parameter, family, size, weight, slant,
                     textStyle, Project::active->GetObjectID(), 
                     internLink, currentDirectory, linkedObjects, argbLinkColor);
      obj->LinkIsIntern(true);
   }
   
   if (obj && iPPTId > 0)
      obj->SetPPTObjectId(iPPTId);

   delete color;
   delete size;
   delete family;
   delete weight;
   delete slant;
   if (hyperlink)
      delete hyperlink;
   
   if (internLink)
      delete internLink;
   
   if (currentDirectory)
      delete currentDirectory;
   
   if (linkedObjects)
      delete linkedObjects;
   
   if (linkColor)
      delete linkColor;
   
   return obj;
}

bool ASSISTANT::Text::IsIdentic(DrawObject *obj)
{
   // the same object is identic
   if (obj == this)
      return true;
   
   // The call of ColoredObject::IsIdentic does not work 
   // if the objects is not already drawn, because of width and height calculation.
   
   if (GetType() != obj->GetType())
      return false;
   
   if (GetX() != obj->GetX())
      return false;
   
   if (GetY() != obj->GetY())
      return false;
   
   if (GetColor() != ((Text *)obj)->GetColor())
      return false;
   
   if (_tcscmp(GetString(), ((Text *)obj)->GetString()) != 0)
      return false;
   
   if (_tcscmp(GetSize(), ((Text *)obj)->GetSize()) != 0)
      return false;
   
   if (_tcscmp(GetWeight(), ((Text *)obj)->GetWeight()) != 0)
      return false;
   
   if (_tcscmp(GetSlant(), ((Text *)obj)->GetSlant()) != 0)
      return false;
   
   // all parameters are identic
   return true;
}




int ASSISTANT::Text::GetValidFontStyle(Gdiplus::FontFamily &fontFamily)
{
   int iFontStyle;

   if (fontWeight == _T("bold") && fontSlant == _T("italic") &&
       fontFamily.IsStyleAvailable(Gdiplus::FontStyleBoldItalic))
      iFontStyle = Gdiplus::FontStyleBoldItalic;
   else if (fontWeight == _T("bold") &&
            fontFamily.IsStyleAvailable(Gdiplus::FontStyleBold))
      iFontStyle = Gdiplus::FontStyleBold;
   else if (fontSlant == _T("italic") &&
            fontFamily.IsStyleAvailable(Gdiplus::FontStyleItalic))
      iFontStyle = Gdiplus::FontStyleItalic;
   else if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleRegular))
      iFontStyle = Gdiplus::FontStyleRegular;
   else if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleBold))
      iFontStyle = Gdiplus::FontStyleBold;
   else if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleItalic))
      iFontStyle = Gdiplus::FontStyleItalic;
   else if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleBoldItalic))
      iFontStyle = Gdiplus::FontStyleBoldItalic;
   else if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleUnderline))
      iFontStyle = Gdiplus::FontStyleUnderline;
   else if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleStrikeout))
      iFontStyle = Gdiplus::FontStyleStrikeout;

   if (m_bIsUnderlined && fontFamily.IsStyleAvailable(Gdiplus::FontStyleUnderline))
      iFontStyle |= Gdiplus::FontStyleUnderline;

   return iFontStyle;
}

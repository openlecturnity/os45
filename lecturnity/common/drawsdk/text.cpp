#include "StdAfx.h"


#include "objects.h"
  
DrawSdk::Text::Text()
{
   Init();
}

DrawSdk::Text::Text(double x, double y, Gdiplus::ARGB outlineColor, const _TCHAR *textString, int textLength, LOGFONT *font) : ColorObject(x, y, 0, 0, outlineColor, 0)
{
   Init();

   SetFont(font);
   SetString((_TCHAR *)textString, textLength);

   
}

DrawSdk::Text::Text(double x, double y, Gdiplus::ARGB outlineColor, const _TCHAR *textString, int textLength, LOGFONT *font, const _TCHAR *ttfName) : ColorObject(x, y, 0, 0, outlineColor, 0)
{
   Init();

   if (ttfName)
   {
      ttfName_ = new _TCHAR[_tcslen(ttfName) + 1]; //(char *)malloc(strlen(ttfName)+1);
      _tcsncpy(ttfName_, ttfName, _tcslen(ttfName));
      ttfName_[_tcslen(ttfName)] = '\000';
   }

   SetFont(font);
   SetString((_TCHAR *)textString, textLength);
}

DrawSdk::Text::Text(double x, double y, double width, double height, 
                    Gdiplus::ARGB outlineColor, const _TCHAR *textString, int textLength, 
                    LOGFONT *font, TextTypeId idTextType) : ColorObject(x, y, width, height, outlineColor, 0)
{
   Init();

   SetFont(font);
   SetString((_TCHAR *)textString, textLength);
   // Text objects from assistant needs their original width and height
   width_ = width;
   height_ = height;

   m_idTextType = idTextType;
}

DrawSdk::Text::Text(double x, double y, double width, double height, 
                    Gdiplus::ARGB outlineColor, const _TCHAR *textString, int textLength, 
                    LOGFONT *font, const _TCHAR *ttfName, TextTypeId idTextType) : ColorObject(x, y, width, height, outlineColor, 0)
{
   Init();

   if (ttfName)
   {
      ttfName_ = new _TCHAR[_tcslen(ttfName) + 1]; //(char *)malloc(strlen(ttfName)+1);
      _tcsncpy(ttfName_, ttfName, _tcslen(ttfName));
      ttfName_[_tcslen(ttfName)] = '\000';
   }

   SetFont(font);
   SetString((_TCHAR *)textString, textLength);
   // Text objects from assistant needs their original width and height
   width_ = width;
   height_ = height;

   m_idTextType = idTextType;
}

DrawSdk::Text::~Text()
{
   if (ttfName_)
      delete[] ttfName_;
   if (textString_)
      delete[] textString_;
   if (m_pCharacterPosition)
      delete m_pCharacterPosition;
}

DrawSdk::DrawObject* DrawSdk::Text::Copy()
{
   Text *pNew = new Text(x_, y_, width_, height_, m_argbLineColor, 
      textString_, textLength_, &logFont_, ttfName_, m_idTextType);
   pNew->EndsWithReturn(m_bEndsWithReturn); 
   pNew->DrawSpecial(m_bDrawSpecial);
   CopyInteractiveFields(pNew);
   return pNew;
}

void DrawSdk::Text::Init()
{
   ttfName_       = NULL;
   //textString_[0] = (TCHAR) '\000';
   textString_    = NULL;
   textLength_    = 0;
   cursorPos_     = -1;
   drawCursor_    = false;
   m_pCharacterPosition = NULL;
   m_idTextType  = TEXT_TYPE_NOTHING;

   m_bEndsWithReturn = false;
   m_bDrawSpecial = false;
}

void DrawSdk::Text::Draw(HDC hdc, double offX, double offY, ZoomManager *pZm)
{
   if (textString_ == NULL)
      return;

   int iStringLength = _tcslen(textString_);
   
   //start GDI drawing of text
   if (m_bDrawSpecial) {
       int iStrLen = iStringLength;
       TCHAR c = textString_[iStrLen - 1];
       //if( c == 13)// don't draw \r at the end
       //    iStrLen --;
       POINT ptText;
       ptText.x = x_;
       ptText.y = y_;
       

       //Apply transformation in order to find the correct position of 
       //drawing point
       int iSavedDc = SaveDC(hdc);

       SetGraphicsMode(hdc, GM_ADVANCED);
       SetMapMode(hdc, MM_TEXT);

       XFORM xForm;

       xForm.eM11 = (FLOAT) pZm->GetZoomFactor(); 
       xForm.eM12 = (FLOAT) 0.0; 
       xForm.eM21 = (FLOAT) 0.0; 
       xForm.eM22 = (FLOAT) pZm->GetZoomFactor(); 
       xForm.eDx  = (FLOAT) offX; 
       xForm.eDy  = (FLOAT) offY; 
       SetWorldTransform(hdc, &xForm); 
       LPtoDP(hdc, &ptText, 1);
       RestoreDC(hdc, iSavedDc);
       LOGFONT lf;
       memcpy(&lf, &logFont_, sizeof(LOGFONT));
       lf.lfHeight = -1 * (long)(abs(lf.lfHeight) * pZm->GetZoomFactor() + 0.5);
       //HFONT font = CreateFontIndirect(&lf);
       //HGDIOBJ oldFont = SelectObject(hdc, font);

       
       Gdiplus::Graphics gr(hdc);
       gr.SetTextContrast(6);
       gr.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

       Gdiplus::FontFamily ff(logFont_.lfFaceName);
       int fStyle = GetValidFontStyle(ff, &logFont_);

       Gdiplus::Font gFont(&ff, ceil(abs(logFont_.lfHeight * pZm->GetZoomFactor())), fStyle, Gdiplus::UnitPixel);
       Gdiplus::StringFormat mStringFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
       Gdiplus::Color clr(m_argbLineColor);
       Gdiplus::SolidBrush sBrush(clr);
       Gdiplus::PointF ptDraw;
       //ptDraw.X = ptText.x;// - 2;
       //ptDraw.Y = ptText.y ;//+ 0;
       WCHAR *wcStr = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
#ifdef _UNICODE
   wcscpy(wcStr, textString_);
#else
   MultiByteToWideChar(CP_ACP, 0, textString_, iStrLen+1, 
                       wcStr, iStrLen+1);
#endif

   


    Gdiplus::RectF box;
   Gdiplus::CharacterRange strRange(0, iStrLen);
   mStringFormat.SetMeasurableCharacterRanges(1, &strRange);
   Gdiplus::Region strRegion;
   gr.MeasureString(wcStr, iStrLen, &gFont, ptDraw, &mStringFormat, &box);
   gr.MeasureCharacterRanges(wcStr, iStrLen,
                                   &gFont, box, &mStringFormat, 1, &strRegion);

   Gdiplus::REAL dAscent = ff.GetCellAscent(fStyle) * 
                          (gFont.GetSize() / ff.GetEmHeight(fStyle));
   // get bounding rectangle
   Gdiplus::RectF strRect;
   strRegion.GetBounds(&strRect, &gr);
   Gdiplus::RectF *aChRects = new Gdiplus::RectF[iStrLen];
   double dOffset = 0.0;
   for(int i = 0; i < iStrLen; i++) {
           
           //GetCharWidth(wcStr[i], 
           //GetCharWidth32(hdc, wcStr[i], wcStr[i], &w);
           double w = GetCharWidth(wcStr[i], &lf);
           aChRects[i].X = ptText.x + dOffset - strRect.X;
           aChRects[i].Y = ptText.y - dAscent;
           aChRects[i].Width = w;
           aChRects[i].Height = height_;
           dOffset += w;
           int xx=0;

       }
   for (int i = 0; i < iStrLen; i++) {
       Gdiplus::PointF pd;
       pd.X = aChRects[i].X;
       pd.Y = aChRects[i].Y;
       gr.DrawString(&wcStr[i], 1, &gFont, pd, &mStringFormat, &sBrush); 
   }

   if (wcStr)
       free (wcStr); 

   if (aChRects)
       delete []aChRects;
   
  // ptDraw.X -= (box.Width - strRect.Width)/2;
   /*gr.TranslateTransform(offX, offY);

   if (pZm != NULL)
      gr.ScaleTransform(pZm->GetZoomFactor(), pZm->GetZoomFactor());*/
  // gr.DrawString(wcStr, iStrLen, &gFont, ptDraw, &mStringFormat, &sBrush); 
       

       
       //LOGFONT lf;
       //memcpy(&lf, &logFont_, sizeof(LOGFONT));
       //lf.lfHeight = -1 * (abs(lf.lfHeight) * pZm->GetZoomFactor());
       //HFONT font = CreateFontIndirect(&lf);
       //HGDIOBJ oldFont = SelectObject(hdc, font);
       //COLORREF clrText = GetOutlineColorRef();
       //SetTextColor(hdc, clrText);
       //SetBkMode(hdc, TRANSPARENT);

       ////
       //TextOut(hdc, ptText.x, ptText.y, textString_, iStrLen);//iStringLength);
       //SelectObject(hdc, oldFont);


       return;
   }
   //end GDI drawing of text
 
   Gdiplus::Graphics graphics(hdc);
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
   graphics.SetTextContrast(9);

   Gdiplus::FontFamily fontFamily(logFont_.lfFaceName);

   int fontStyle = GetValidFontStyle(fontFamily, &logFont_);

   Gdiplus::Font gdipFont(&fontFamily, abs(logFont_.lfHeight), fontStyle, Gdiplus::UnitPixel);

   Gdiplus::REAL ascent = fontFamily.GetCellAscent(fontStyle) * 
                          (gdipFont.GetSize() / fontFamily.GetEmHeight(fontStyle));

   Gdiplus::PointF drawPoint;
   drawPoint.X = x_;
   drawPoint.Y = y_ - ascent;

   // convert text string to WCHAR format
   /*int*/ iStringLength = _tcslen(textString_);
   WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
#ifdef _UNICODE
   wcscpy(wcString, textString_);
#else
   MultiByteToWideChar(CP_ACP, 0, textString_, iStringLength+1, 
                       wcString, iStringLength+1);
#endif

   // compute character positions
   Gdiplus::StringFormat measureStringFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
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
   if (m_idTextType == TEXT_TYPE_NOTHING)
   {
      if (iStringLength > 1 && width_ != 0 && realTextWidth != width_)
      {
         Gdiplus::REAL scaleX = (width_ / realTextWidth);

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
            drawPoint.X = x_ / scaleX;
         }
      }
   }

   // the text should begin exact at m_dX, m_dY
   drawPoint.X -= stringRect.X;

   // Move and Scale
   graphics.TranslateTransform(offX, offY);

   if (pZm != NULL)
      graphics.ScaleTransform(pZm->GetZoomFactor(), pZm->GetZoomFactor());

   Gdiplus::Color color(m_argbLineColor);
   Gdiplus::SolidBrush solidBrush(color);

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

   if (drawCursor_)
   {
      if (cursorPos_ != -1)
         DrawCursor(hdc, cursorPos_, pZm);
      else
         DrawCursor(hdc, textLength_, pZm);
   }
}

void DrawSdk::Text::SetFont(LOGFONT *font)
{
   if (font == NULL)
   {
      return;
   }

   memcpy(&logFont_, font, sizeof(LOGFONT));

   CalculateDimension();
}

void DrawSdk::Text::ChangeFontFamily(const _TCHAR *tszFontFamily)
{
   _tcscpy(logFont_.lfFaceName, tszFontFamily);

   CalculateDimension();
}

void DrawSdk::Text::ChangeFontSize(int iFontSize)
{
   logFont_.lfHeight = iFontSize;

   CalculateDimension();
}

void DrawSdk::Text::DrawCursor(HDC hdc, int position, ZoomManager *pZm)
{
   if (position < 0 || position > textLength_)
      return;

   int zX = (int)x_;
   int zY = (int)y_;
   int zWidth  = (int)width_;
   int zHeight = (int)height_;

   HFONT hFont = NULL;
   LOGFONT zwFont;
   memcpy(&zwFont, &logFont_, sizeof(LOGFONT));
   if (pZm != NULL)
   {
      zwFont.lfHeight = pZm->GetScreenX(zwFont.lfHeight);
      zX = pZm->GetScreenX(x_);
      zY = pZm->GetScreenY(y_);
      zWidth = pZm->GetScreenX(width_);
   }
   int cursorX = zX-1;
   int cursorY0 = zY - GetTextAscent(&zwFont);
   int cursorY1 = zY + GetTextDescent(&zwFont);  
   if (position != 0) 
   {
      //LPTSTR textRange = textString_.GetBufferSetLength(position);
      //CSize rangeSize = cdc->GetTextExtent(textRange, position);
      //cursorX += rangeSize.cx;
      //textString_.ReleaseBuffer();
      //SIZE size;
      double posX = GetXPos(zX, zWidth, textString_, &zwFont, position);

      //::GetTextExtentPoint(hdc, textString_, position, &size);
      cursorX = posX; //size.cx;
   }
   else
   {
      cursorX = zX;
   }

   Gdiplus::Graphics graphics(hdc);
   Gdiplus::Pen pen(Gdiplus::Color(m_argbLineColor), 1);
   graphics.DrawLine(&pen, cursorX, cursorY0, cursorX, cursorY1);
}

void DrawSdk::Text::SetStringNumber(int iNumber)
{
   _TCHAR tszNumber[40];
   tszNumber[0] = 0;
   _itot(iNumber, tszNumber, 10);

   SetString(tszNumber, _tcslen(tszNumber));
}

void DrawSdk::Text::SetString(const _TCHAR *textString, int textLength)
{
   //_tcscpy(textString_, textString);
   //textLength_ = textLength;
   if (textString_)
      delete[] textString_;
   textString_ = new TCHAR[textLength + 1];
   _tcsncpy(textString_, textString, textLength);
   textString_[textLength] = (TCHAR)'\0';
   textLength_ = textLength;

   CalculateDimension();
}

const TCHAR *DrawSdk::Text::GetString()
{
   return (LPCTSTR)textString_;
}

const _TCHAR *DrawSdk::Text::GetTtfName()
{
   return ttfName_;
}

void DrawSdk::Text::CalculateDimension()
{
   if (textString_ == NULL)
   {
      width_ = 0;
      height_ = 0;
      return;
   }

   int iStringLength = _tcslen(textString_);

   width_ = GetTextWidth(textString_, iStringLength, &logFont_);
   height_ = GetTextHeight(textString_, iStringLength, &logFont_);
}

void DrawSdk::Text::GetLogicalBoundingBox(RECT *pRect)
{
   Gdiplus::REAL textWidth = width_;
   if (textWidth == 0)
   {
      HDC hdc = ::CreateCompatibleDC(NULL);
      Gdiplus::Font font(hdc, &logFont_);
      
      Gdiplus::RectF bbox; 
      int iStringLength = _tcslen(textString_);
      // +1 for null termination
      WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR));

#ifdef _UNICODE
      wcscpy(wcString, textString_);
#else
      MultiByteToWideChar( CP_ACP, 0, textString_, iStringLength+1, 
         wcString, iStringLength+1);
#endif
      
      Gdiplus::Graphics graphics(hdc);
      graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
      graphics.MeasureString(wcString, textLength_, &font, Gdiplus::PointF(0.0, 0.0), &bbox);
      textWidth = bbox.Width;
      free(wcString);
      ::DeleteDC(hdc);
   }

   pRect->left = x_;
   pRect->top = y_ - GetTextAscent( &logFont_);
   pRect->right = x_ + textWidth;
   pRect->bottom = y_ + GetTextDescent(&logFont_);

   return;
}

void DrawSdk::Text::Write(CharArray *pArray, bool bUseFullImagePath)
{
   Write(pArray, bUseFullImagePath, true, -1);
}

void DrawSdk::Text::Write(CharArray *pArray, bool bUseFullImagePath, 
                          bool bUseStrictXml, int nTimeBeginMs)
{
   // Extended "Write" method: Timestamp as additional parameter
   if (pArray == NULL)
      return;

   static _TCHAR tszTextType[100];
   tszTextType[0] = 0;
   GetTextTypeString(tszTextType);
   
   static _TCHAR tszVisibility[100];
   tszVisibility[0] = 0;
   if (HasVisibility())
      _stprintf(tszVisibility, _T(" visible=\"%s\""), GetVisibility());

   static _TCHAR tszSpecialDraw[100];
   tszSpecialDraw[0] = 0;
   if (m_bDrawSpecial)
       _stprintf(tszSpecialDraw, _T(" specialDraw=\"%s\""), _T("true"));

   // write text entry
   static _TCHAR tszBuffer[1000];
   tszBuffer[0] = 0;
   _stprintf(tszBuffer, 
      _T("<TEXT x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\"%s%s%s>\n"),
      (int)x_, (int)y_, (int)width_, (int)height_, 
      x_, y_, width_, height_, 
      tszTextType, tszVisibility, tszSpecialDraw);

   pArray->AppendData(tszBuffer);
   
   // find out which slant the font use
   _TCHAR tszSlant[10];
   if (logFont_.lfItalic)
      _stprintf(tszSlant, _T("italic"));
   else
      _stprintf(tszSlant, _T("roman"));

   
   // find out which weight the font use
   _TCHAR tszWeight[10];
   if (logFont_.lfWeight == FW_BOLD)
      _stprintf(tszWeight, _T("bold"));
   else
      _stprintf(tszWeight, _T("normal"));

   // write textitem entry
   int style = 0;
   if (logFont_.lfUnderline == TRUE)
      style = 16;

   _TCHAR *tszOutputTtf = ttfName_;
   if (tszOutputTtf == NULL)
      tszOutputTtf = GetTtfName(logFont_);

   int iFontSize = abs(logFont_.lfHeight);

   static _TCHAR tszItemBuffer[1500];
   tszItemBuffer[0] = 0;
   if (tszOutputTtf != NULL)
   {  
      _stprintf(tszItemBuffer, 
         _T("<TEXTITEM x=\"%d\" y=\"%d\" xf=\"%.2f\" yf=\"%.2f\" offx=\"0\" offy=\"0\" font=\"%s\" size=\"%d\" weight=\"%s\" slant=\"%s\" style=\"%d\" ttf=\"%s\" rgb=\"0x%08x\">\n"),
         (int)x_, (int)y_, x_, y_, logFont_.lfFaceName, 
         iFontSize, tszWeight, tszSlant, style, tszOutputTtf, m_argbLineColor);
   }
   else
   {    _stprintf(tszItemBuffer, 
         _T("<TEXTITEM x=\"%d\" y=\"%d\" xf=\"%.2f\" yf=\"%.2f\" offx=\"0\" offy=\"0\" font=\"%s\" size=\"%d\" weight=\"%s\" slant=\"%s\" style=\"%d\" rgb=\"0x%08x\">\n"),
         (int)x_, (int)y_, x_, y_, logFont_.lfFaceName, 
         iFontSize, tszWeight, tszSlant, style, m_argbLineColor);
   }
   pArray->AppendData(tszItemBuffer);
   
   
   // make the text string escaped

   int iLength = _tcslen(textString_);
   _TCHAR *tszEscaped;
   int iTarget = 0;

   if (bUseStrictXml)
   {
      // '<', '>', '&', '"' and ''' must be replaced
      // by "&lt;", "&gt;", "&amp;", "&quot;" and "&apos;"
      tszEscaped = new _TCHAR[iLength * 6 + 1];

      for (int i=0; i<iLength; ++i)
      {
         if (textString_[i] == _T('<'))
         {
            tszEscaped[iTarget++] = _T('&');
            tszEscaped[iTarget++] = _T('l');
            tszEscaped[iTarget++] = _T('t');
            tszEscaped[iTarget++] = _T(';');
         }
         else if (textString_[i] == _T('>'))
         {
            tszEscaped[iTarget++] = _T('&');
            tszEscaped[iTarget++] = _T('g');
            tszEscaped[iTarget++] = _T('t');
            tszEscaped[iTarget++] = _T(';');
         }
         else if (textString_[i] == _T('&'))
         {
            tszEscaped[iTarget++] = _T('&');
            tszEscaped[iTarget++] = _T('a');
            tszEscaped[iTarget++] = _T('m');
            tszEscaped[iTarget++] = _T('p');
            tszEscaped[iTarget++] = _T(';');
         }
         else if (textString_[i] == _T('"'))
         {
            tszEscaped[iTarget++] = _T('&');
            tszEscaped[iTarget++] = _T('q');
            tszEscaped[iTarget++] = _T('u');
            tszEscaped[iTarget++] = _T('o');
            tszEscaped[iTarget++] = _T('t');
            tszEscaped[iTarget++] = _T(';');
         }
         else if (textString_[i] == _T('\''))
         {
            tszEscaped[iTarget++] = _T('&');
            tszEscaped[iTarget++] = _T('a');
            tszEscaped[iTarget++] = _T('p');
            tszEscaped[iTarget++] = _T('o');
            tszEscaped[iTarget++] = _T('s');
            tszEscaped[iTarget++] = _T(';');
         }
         else
            tszEscaped[iTarget++] = textString_[i];
      }
   }
   else
   {
      // '<', '>' and '\' must be escaped
      tszEscaped = new _TCHAR[iLength * 2 + 1];

      for (int i=0; i<iLength; ++i)
      {
         if (textString_[i] == _T('<') || textString_[i] == _T('>') || textString_[i] == _T('\\'))
            tszEscaped[iTarget++] = _T('\\');

         tszEscaped[iTarget++] = textString_[i];
      }
   }

   tszEscaped[iTarget] = 0;

   pArray->AppendData(tszEscaped);

   delete[] tszEscaped;

   // close textitem
   pArray->AppendData(_T("\n</TEXTITEM>\n"));
   
   // time (optional)
   if (nTimeBeginMs > -1)
   {
      _stprintf(tszItemBuffer, _T("<TIME beginMs=\"%d\" />\n"), nTimeBeginMs);
      pArray->AppendData(tszItemBuffer);
   }

   // close text
   pArray->AppendData(_T("</TEXT>\n"));
}

int DrawSdk::Text::GetTmAscent()
{
   return GetTextAscent(&logFont_);
}

double DrawSdk::Text::GetTextWidth(const _TCHAR *textString, int textLength, LOGFONT *logFont)
{
   if (textLength == 0)
      return 0;

   HDC hdc = ::CreateCompatibleDC(NULL);

   //Gdiplus::Graphics graphics(hdc);

   WCHAR *wcString = (WCHAR *)malloc((textLength+1)*sizeof(WCHAR));

#ifdef _UNICODE
   wcscpy(wcString, textString);
#else
   MultiByteToWideChar(CP_ACP, 0, textString, textLength+1, 
                                  wcString, textLength+1);
#endif

   
   Gdiplus::RectF bbox;
   Gdiplus::Font font(hdc, logFont);

   Gdiplus::Status status = font.GetLastStatus();

   if (status != Gdiplus::NotTrueTypeFont) {
       Gdiplus::Graphics graphics(hdc);
       graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
       Gdiplus::StringFormat strFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

       graphics.MeasureString(wcString, textLength, &font, Gdiplus::PointF(0.0, 0.0), &strFormat, &bbox);

       Gdiplus::CharacterRange charRange(0, textLength);
       strFormat.SetMeasurableCharacterRanges(1, &charRange);
       Gdiplus::Region pRangeRegion[1];
       graphics.MeasureCharacterRanges(wcString, textLength,
           &font, bbox, &strFormat, 1, pRangeRegion);
       // get bounding rectangle
       Gdiplus::RectF rect;
       pRangeRegion[0].GetBounds(&rect, &graphics);

       if (wcString)
           free(wcString);

       ::DeleteObject(hdc);
       ::DeleteDC(hdc);

       return rect.Width;
   } else { // font is not true type
       HFONT hFont = CreateFontIndirect(logFont);
       HGDIOBJ hOldFont = ::SelectObject(hdc, hFont);
       MAT2 mat = { 0 };		// Prepare unit matrix, no transformation.
       mat.eM11.value = 1;
       mat.eM22.value = 1;

       GLYPHMETRICS gm;

       // Get the GLYPHMETRICS for an untransformed glyph.
       int iWidth = 0;
       for (int i = 0; i < textLength + 1; i ++) {
           UINT nChar = (UINT) wcString[i];
           DWORD nBytes = ::GetGlyphOutline(hdc, nChar, GGO_METRICS, & gm, 0, NULL, & mat);
           if (nBytes == GDI_ERROR) // this hould not happen
               return textLength * logFont->lfHeight; // set a default return value to avoid application hang

           // Determine the width of the character cell.
           // We simple add up the horizontal and vertical values, to get
           // something sensible in most cases.
           // Typically for Latin fonts, gm.gmCellIncY will be 0.
           int nCell = gm.gmCellIncX + gm.gmCellIncY;
           iWidth += nCell;
       }
       if (wcString)
           free(wcString);
       ::SelectObject(hdc, hOldFont);
       ::ReleaseDC(NULL, hdc);
       return iWidth;
   }
   //return bbox.Width;
}

double DrawSdk::Text::GetTextHeight(const _TCHAR *textString, int textLength, LOGFONT *logFont)
{
   HDC hdc = ::CreateCompatibleDC(NULL);

   Gdiplus::Graphics graphics(hdc);

   WCHAR *wcString = (WCHAR *)malloc((textLength+1)*sizeof(WCHAR));
#ifdef _UNICODE
   wcscpy(wcString, textString);
#else
   MultiByteToWideChar(CP_ACP, 0, textString, textLength+1, 
                                  wcString, textLength+1);
#endif

   Gdiplus::RectF bbox;
   Gdiplus::Font font(hdc, logFont);
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
   graphics.MeasureString(wcString, textLength, &font, Gdiplus::PointF(0.0, 0.0), &bbox);

   if (wcString)
      free(wcString);

   ::DeleteDC(hdc);

   return bbox.Height;
}

void DrawSdk::Text::GetTextDimension(const _TCHAR *textString, int textLength, LOGFONT *logFont, UINT &nWidth, UINT &nHeight)
{
   HDC hdc = ::CreateCompatibleDC(NULL);
 
   Gdiplus::Font font(hdc, logFont);

   int iStringLength = _tcslen(textString);
   // +1 for null termination
   WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR));
   
#ifdef _UNICODE
   wcscpy(wcString, textString);
#else
   MultiByteToWideChar( CP_ACP, 0, textString, iStringLength+1, 
      wcString, iStringLength+1);
#endif

   Gdiplus::Graphics graphics(hdc);
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

   Gdiplus::RectF bbox;
   graphics.MeasureString(wcString, iStringLength, &font, Gdiplus::PointF(0.0, 0.0), &bbox);
   free(wcString);

   nWidth = (UINT)bbox.Width;
   nHeight = (UINT)bbox.Height;

   ::DeleteDC(hdc);
}

void DrawSdk::Text::GetBoundingBox(const _TCHAR *textString, int textLength, LOGFONT *logFont, Gdiplus::RectF &bbox)
{
   bbox.X = 0;
   bbox.Y = 0;
   bbox.Width = 0;
   bbox.Height = 0;

   if (textLength == 0)
      return;

   HDC hdc = ::CreateCompatibleDC(NULL);

   //Gdiplus::Graphics graphics(hdc);

   WCHAR *wcString = (WCHAR *)malloc((textLength+1)*sizeof(WCHAR));

#ifdef _UNICODE
   wcscpy(wcString, textString);
#else
   MultiByteToWideChar(CP_ACP, 0, textString, textLength+1, 
                                  wcString, textLength+1);
#endif

   Gdiplus::Font font(hdc, logFont);
   
   Gdiplus::Graphics graphics(hdc);
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
   Gdiplus::StringFormat strFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

   graphics.MeasureString(wcString, textLength, &font, Gdiplus::PointF(0.0, 0.0), &strFormat, &bbox);

   Gdiplus::CharacterRange charRange(0, textLength);
   strFormat.SetMeasurableCharacterRanges(1, &charRange);
   Gdiplus::Region pRangeRegion[1];
   graphics.MeasureCharacterRanges(wcString, textLength,
                                   &font, bbox, &strFormat, 1, pRangeRegion);
   if (wcString)
      free(wcString);

   ::DeleteObject(hdc);
   ::DeleteDC(hdc);

   return;
}

void DrawSdk::Text::CalculateCenterPos(const _TCHAR *textString, int textLength, LOGFONT *pLogFont, 
                                          Gdiplus::RectF &gdipRcArea, Gdiplus::PointF &gdipPtTextPos)
{
   double dTextWidth = GetTextWidth(textString, textLength, pLogFont);
   double dTextHeight = GetTextHeight(textString, textLength, pLogFont);
   double dAscent = GetTextAscent(pLogFont);

   gdipPtTextPos.X = gdipRcArea.X + (gdipRcArea.Width - dTextWidth) / 2;
   gdipPtTextPos.Y = gdipRcArea.Y + (gdipRcArea.Height - dTextHeight) / 2 + dAscent;

}

double DrawSdk::Text::GetTextAscent(LOGFONT *logFont)
{  
   HDC hdc = ::CreateCompatibleDC(NULL);
   
   Gdiplus::Font font(hdc, logFont);
   Gdiplus::FontFamily fontFamily;
   font.GetFamily(&fontFamily);


   int fontStyle = GetValidFontStyle(fontFamily);

   Gdiplus::REAL ascent = fontFamily.GetCellAscent(fontStyle) * (font.GetSize() / fontFamily.GetEmHeight(fontStyle)) ;

   ::DeleteDC(hdc);
   return ascent;
}

double DrawSdk::Text::GetTextDescent(LOGFONT *logFont)
{  
   HDC hdc = ::CreateCompatibleDC(NULL);
   
   Gdiplus::Font font(hdc, logFont);
   Gdiplus::FontFamily fontFamily;
   font.GetFamily(&fontFamily);

   int fontStyle = GetValidFontStyle(fontFamily);
   Gdiplus::REAL descent = fontFamily.GetCellDescent(fontStyle) * (font.GetSize() / fontFamily.GetEmHeight(fontStyle)) ;

   ::DeleteDC(hdc);
   return descent;
}

double DrawSdk::Text::GetCharWidth(_TCHAR ascii, LOGFONT *logFont)
{ 
   HDC hdc = ::CreateCompatibleDC(NULL);
   
   HFONT hFont = ::CreateFontIndirect(logFont);
   ::SelectObject(hdc, hFont);

   double charWidth;

   ABCFLOAT characterWidth;
   WCHAR wcAscii;
#ifdef _UNICODE
   wcAscii = ascii;
#else
   mbtowc(&wcAscii, &ascii, 1); 
#endif
   
   BOOL success = GetCharABCWidthsFloat(hdc, ascii, ascii, &characterWidth);  
   if (!success)
   {
      RECT rect;
      _TCHAR text[2];
      text[0] = ascii;
      text[1] = _T('\0');
      DrawText(hdc, text, 1, &rect, DT_CALCRECT);
      charWidth = rect.right-rect.left;
   }
   else
   {
      charWidth = (characterWidth.abcfA + characterWidth.abcfB + characterWidth.abcfC);
   }

   ::DeleteObject(hFont);
   ::DeleteDC(hdc);
   
   return  charWidth;
}
     
double DrawSdk::Text::GetXPos(double xStart, double textWidth, const _TCHAR *textString, LOGFONT *logFont, int index)
{
   if (_tcslen(textString) == 0)
      return xStart;
   
   HDC hdc = ::CreateCompatibleDC(NULL);

   int iStringLength = _tcslen(textString);
   WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR));
#ifdef _UNICODE
   wcscpy(wcString, textString);
#else
   MultiByteToWideChar( CP_ACP, 0, textString, iStringLength+1, 
                        wcString, iStringLength+1);
#endif

   Gdiplus::Font font(hdc, logFont);
   Gdiplus::Graphics graphics(hdc);
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

   Gdiplus::RectF bbox;
   Gdiplus::StringFormat measureStringFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
   graphics.MeasureString(wcString, iStringLength, &font, Gdiplus::PointF(0.0, 0.0), &measureStringFormat, &bbox);

   Gdiplus::CharacterRange charRange(0, iStringLength);
   measureStringFormat.SetMeasurableCharacterRanges(1, &charRange);
   Gdiplus::Region pRangeRegion[1];
   graphics.MeasureCharacterRanges(wcString, iStringLength,
                                   &font, bbox, &measureStringFormat, 1, pRangeRegion);
   // get bounding rectangle
   Gdiplus::RectF rect;
   pRangeRegion[0].GetBounds(&rect, &graphics);

   Gdiplus::REAL realTextWidth = rect.Width;
   Gdiplus::REAL scaleX = 1.0;
   if (iStringLength > 1 && textWidth != 0 && realTextWidth != textWidth)
   {
      scaleX = (textWidth / realTextWidth);
   }
   // the text should begin at x, y
   
   Gdiplus::REAL offsetX = -rect.X;
   
   charRange = Gdiplus::CharacterRange(0, index);
   Gdiplus::StringFormat strFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
   strFormat.SetMeasurableCharacterRanges(1, &charRange);
   graphics.MeasureCharacterRanges(wcString, iStringLength,
      &font, bbox, &strFormat, 1, pRangeRegion);
   
   // get bounding rectangle
   pRangeRegion[0].GetBounds(&rect, &graphics);

   ::DeleteDC(hdc);
   if (wcString)
      free(wcString);

   double xPos = xStart + (offsetX + rect.X + rect.Width) * scaleX;
      
   return xPos;
}

int DrawSdk::Text::GetTextIndex(double xStart, double textWidth, const _TCHAR *textString, LOGFONT *logFont, int xPos)
{ 
   if (_tcslen(textString) == 0)
      return 0;

   HDC hdc = ::CreateCompatibleDC(NULL);

   int iStringLength = _tcslen(textString);
   WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR));
#ifdef _UNICODE
   wcscpy(wcString, textString);
#else
   MultiByteToWideChar( CP_ACP, 0, textString, iStringLength+1, 
                        wcString, iStringLength+1);
#endif

   Gdiplus::Font font(hdc, logFont);
   Gdiplus::Graphics graphics(hdc);
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

   Gdiplus::RectF bbox;
   Gdiplus::StringFormat measureStringFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
   graphics.MeasureString(wcString, iStringLength, &font, Gdiplus::PointF(0.0, 0.0), &measureStringFormat, &bbox);

   Gdiplus::CharacterRange charRange(0, iStringLength);
   measureStringFormat.SetMeasurableCharacterRanges(1, &charRange);
   Gdiplus::Region pRangeRegion[1];
   graphics.MeasureCharacterRanges(wcString, iStringLength,
                                   &font, bbox, &measureStringFormat, 1, pRangeRegion);
   // get bounding rectangle
   Gdiplus::RectF rect;
   pRangeRegion[0].GetBounds(&rect, &graphics);

   Gdiplus::REAL realTextWidth = rect.Width;
   Gdiplus::REAL scaleX = 1.0;
   if (iStringLength > 1 && textWidth != 0 && realTextWidth != textWidth)
   {
      scaleX = (textWidth / realTextWidth);
   }
   // the text should begin at x, y
   Gdiplus::REAL offsetX = -rect.X;

   Gdiplus::StringFormat strFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
   double dXPos = xStart;
   int iIndex = 0;
   bool bIndexFound = false;
   for (int i = 0; i < iStringLength; ++i)
   {
      Gdiplus::CharacterRange charRange(i, 1);
      Gdiplus::Status status = strFormat.SetMeasurableCharacterRanges(1, &charRange);
      Gdiplus::Region rangeRegion;
      int iCount = strFormat.GetMeasurableCharacterRangeCount();
      if (iCount == 1)
      {
         graphics.MeasureCharacterRanges(wcString, iStringLength,
            &font, bbox, &strFormat, 1, &rangeRegion);
      }
   
      // get bounding rectangle
      Gdiplus::RectF rect;
      rangeRegion.GetBounds(&rect, &graphics);
      dXPos = xStart + (offsetX + rect.X + rect.Width) * scaleX;
      if (dXPos >= xPos)
      {
         iIndex = i;
         bIndexFound = true;
         break;
      }
   }
   
   if (!bIndexFound)
      iIndex = iStringLength;

   ::DeleteDC(hdc);
   if (wcString)
      free(wcString);

   return iIndex;
}

int DrawSdk::Text::GetValidFontStyle(Gdiplus::FontFamily &fontFamily, LOGFONT *pLogFont)
{
   bool bIsBold = false;
   bool bIsItalic = false;
   bool bIsUnderlined = false;

   if (pLogFont)
   {
      bIsBold = (pLogFont->lfWeight == FW_BOLD) ? true : false;
      bIsItalic = (pLogFont->lfItalic == TRUE) ? true : false;
      bIsUnderlined = (pLogFont->lfUnderline == TRUE) ? true : false;
   }

   int iFontStyle = Gdiplus::FontStyleRegular;

   if (bIsBold && bIsItalic &&
       fontFamily.IsStyleAvailable(Gdiplus::FontStyleBoldItalic))
      iFontStyle = Gdiplus::FontStyleBoldItalic;
   else if (bIsBold &&
            fontFamily.IsStyleAvailable(Gdiplus::FontStyleBold))
      iFontStyle = Gdiplus::FontStyleBold;
   else if (bIsItalic &&
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

   if (bIsUnderlined && fontFamily.IsStyleAvailable(Gdiplus::FontStyleUnderline))
      iFontStyle |= Gdiplus::FontStyleUnderline;

   return iFontStyle;
}

//static
DrawSdk::TextTypeId DrawSdk::Text::GetTextTypeForString(LPCTSTR tszTextType)
{
   TextTypeId idTextType = TEXT_TYPE_NOTHING;
   if (tszTextType != NULL)
   {
      if (_tcscmp(tszTextType, _T("time")) == 0)
         idTextType = TEXT_TYPE_CHANGE_TIME;
      else if (_tcscmp(tszTextType, _T("tries")) == 0)
         idTextType = TEXT_TYPE_CHANGE_TRIES;
      else if (_tcscmp(tszTextType, _T("number")) == 0)
         idTextType = TEXT_TYPE_CHANGE_NUMBER;
      else if (_tcscmp(tszTextType, _T("q-correct")) == 0)
         idTextType = TEXT_TYPE_EVAL_Q_CORRECT;
      else if (_tcscmp(tszTextType, _T("q-points")) == 0)
         idTextType = TEXT_TYPE_EVAL_Q_POINTS;
      else if (_tcscmp(tszTextType, _T("qq-passed")) == 0)
         idTextType = TEXT_TYPE_EVAL_QQ_PASSED;
      else if (_tcscmp(tszTextType, _T("qq-failed")) == 0)
         idTextType = TEXT_TYPE_EVAL_QQ_FAILED;
      else if (_tcscmp(tszTextType, _T("qq-achieved")) == 0)
         idTextType = TEXT_TYPE_EVAL_QQ_ACHIEVED;
      else if (_tcscmp(tszTextType, _T("qq-achieved-p")) == 0)
         idTextType = TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT;
      else if (_tcscmp(tszTextType, _T("qq-total")) == 0)
         idTextType = TEXT_TYPE_EVAL_QQ_TOTAL;
      else if (_tcscmp(tszTextType, _T("qq-required")) == 0)
         idTextType = TEXT_TYPE_EVAL_QQ_REQUIRED;
      else if (_tcscmp(tszTextType, _T("qq-required-p")) == 0)
         idTextType = TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT;
      else if (_tcscmp(tszTextType, _T("blank-s")) == 0)
         idTextType = TEXT_TYPE_BLANK_SUPPORT;
      else if (_tcscmp(tszTextType, _T("radio-s")) == 0)
         idTextType = TEXT_TYPE_RADIO_SUPPORT;
      else if (_tcscmp(tszTextType, _T("title")) == 0)
         idTextType = TEXT_TYPE_QUESTION_TITLE;
      else if (_tcscmp(tszTextType, _T("question")) == 0)
         idTextType = TEXT_TYPE_QUESTION_TEXT;
      else if (_tcscmp(tszTextType, _T("number-s")) == 0)
         idTextType = TEXT_TYPE_NUMBER_SUPPORT;
      else if (_tcscmp(tszTextType, _T("tries-s")) == 0)
         idTextType = TEXT_TYPE_TRIES_SUPPORT;
      else if (_tcscmp(tszTextType, _T("time-s")) == 0)
         idTextType = TEXT_TYPE_TIME_SUPPORT;
      else if (_tcscmp(tszTextType, _T("target-s")) == 0)
         idTextType = TEXT_TYPE_TARGET_SUPPORT;
   
   }
   return idTextType;
}

// private
void DrawSdk::Text::GetTextTypeString(_TCHAR *tszTarget)
{
   if (tszTarget != NULL)
   {
      if (m_idTextType == TEXT_TYPE_CHANGE_TIME)
         _stprintf(tszTarget, _T(" type=\"time\""));
      else if (m_idTextType == TEXT_TYPE_CHANGE_TRIES)
         _stprintf(tszTarget, _T(" type=\"tries\""));
      else if (m_idTextType == TEXT_TYPE_CHANGE_NUMBER)
         _stprintf(tszTarget, _T(" type=\"number\""));
      else if (m_idTextType == TEXT_TYPE_EVAL_Q_CORRECT)
         _stprintf(tszTarget, _T(" type=\"q-correct\""));
      else if (m_idTextType == TEXT_TYPE_EVAL_Q_POINTS)
         _stprintf(tszTarget, _T(" type=\"q-points\""));
      else if (m_idTextType == TEXT_TYPE_EVAL_QQ_PASSED)
         _stprintf(tszTarget, _T(" type=\"qq-passed\""));
      else if (m_idTextType == TEXT_TYPE_EVAL_QQ_FAILED)
         _stprintf(tszTarget, _T(" type=\"qq-failed\""));
      else if (m_idTextType == TEXT_TYPE_EVAL_QQ_ACHIEVED)
         _stprintf(tszTarget, _T(" type=\"qq-achieved\""));
      else if (m_idTextType == TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT)
         _stprintf(tszTarget, _T(" type=\"qq-achieved-p\""));
      else if (m_idTextType == TEXT_TYPE_EVAL_QQ_TOTAL)
         _stprintf(tszTarget, _T(" type=\"qq-total\""));
      else if (m_idTextType == TEXT_TYPE_EVAL_QQ_REQUIRED)
         _stprintf(tszTarget, _T(" type=\"qq-required\""));
      else if (m_idTextType == TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT)
         _stprintf(tszTarget, _T(" type=\"qq-required-p\""));
      else if (m_idTextType == TEXT_TYPE_BLANK_SUPPORT)
         _stprintf(tszTarget, _T(" type=\"blank-s\""));
      else if (m_idTextType == TEXT_TYPE_RADIO_SUPPORT)
         _stprintf(tszTarget, _T(" type=\"radio-s\""));
      else if (m_idTextType == TEXT_TYPE_QUESTION_TITLE)
         _stprintf(tszTarget, _T(" type=\"title\""));
      else if (m_idTextType == TEXT_TYPE_QUESTION_TEXT)
         _stprintf(tszTarget, _T(" type=\"question\""));
      else if (m_idTextType == TEXT_TYPE_NUMBER_SUPPORT)
         _stprintf(tszTarget, _T(" type=\"number-s\""));
      else if (m_idTextType == TEXT_TYPE_TRIES_SUPPORT)
         _stprintf(tszTarget, _T(" type=\"tries-s\""));
      else if (m_idTextType == TEXT_TYPE_TIME_SUPPORT)
         _stprintf(tszTarget, _T(" type=\"time-s\""));
      else if (m_idTextType == TEXT_TYPE_TARGET_SUPPORT)
         _stprintf(tszTarget, _T(" type=\"target-s\""));
      else
         _stprintf(tszTarget, _T(" type=\"unknown\""));
   }
}


LPTSTR DrawSdk::Text::GetTtfName(LOGFONT &lf)
{   
   static _TCHAR tszTtfName[512];

   _TCHAR strFontFilesPath[ MAX_PATH ];


   ::GetWindowsDirectory(strFontFilesPath, MAX_PATH);
   int pathLen = _tcslen(strFontFilesPath);

   if(strFontFilesPath[pathLen-1] != _T('\\'))
       _tcscat(strFontFilesPath, _T("\\Fonts\\"));
   else
       _tcscat(strFontFilesPath, _T("Fonts\\"));

   HKEY hKeyMachine; 
   LONG regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
                     _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), NULL,KEY_READ,&hKeyMachine); 
	if ( regErr != ERROR_SUCCESS )
   {
      LONG regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
                        _T("Software\\Microsoft\\Windows\\CurrentVersion\\Fonts"), NULL,KEY_READ,&hKeyMachine); 
      if ( regErr != ERROR_SUCCESS )
      {
         return NULL;
      }
	}

   DWORD subValues;
   regErr = RegQueryInfoKey(hKeyMachine, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
   if ( regErr != ERROR_SUCCESS )
   {
      return NULL;
   }

   HRESULT hr = ::CoInitialize(NULL);
   ULONG_PTR token;
   Gdiplus::GdiplusStartupInput gdipsi;
   ZeroMemory(&gdipsi, sizeof Gdiplus::GdiplusStartupInput);
   gdipsi.GdiplusVersion = 1;
   Gdiplus::GdiplusStartup(&token, &gdipsi, NULL);
   HDC hdc = ::CreateCompatibleDC(NULL);

   DWORD dwType;
   _TCHAR tszEntryName[512];
   _TCHAR tszEntryValue[512];

   _TCHAR tszLfFaceName[512];
   _tcscpy(tszLfFaceName, lf.lfFaceName);
   
   HFONT font = CreateFontIndirect(&lf);
   UINT cbData;       
   LPOUTLINETEXTMETRIC lpoltm;     
   SelectObject(hdc, font);
   cbData = GetOutlineTextMetrics (hdc, 0, NULL);
   DeleteObject(font);
   _TCHAR tszFontFace[512];
   if (cbData == 0) 
   {
      _tcscpy(tszFontFace, lf.lfFaceName);
   }
   else
   {
      lpoltm = (LPOUTLINETEXTMETRIC)LocalAlloc (LPTR, cbData);
      GetOutlineTextMetrics (hdc, cbData, lpoltm);  
      _tcscpy(tszFontFace, (_TCHAR *)((DWORD) lpoltm + lpoltm->otmpFaceName));

      //#ifdef _DEBUG
      //                     if (strstr(faceName.c_str(), "Arial") != NULL)
      //                        MessageBox(NULL, faceName.c_str(), _T("Face name"), MB_OK);
      //#endif
   }

   bool bFoundFontInfo = false;
   for (int i = 0; i < subValues && !bFoundFontInfo; i++) 
   {       
      unsigned long entryNameLength = 512 * sizeof _TCHAR;
      unsigned long entryValueLength = 512 * sizeof _TCHAR;
      if (RegEnumValue(hKeyMachine, i, tszEntryName, &entryNameLength, 
         NULL, &dwType, (unsigned char *)tszEntryValue, &entryValueLength) != ERROR_SUCCESS)
      {
         continue;
      }
      
      _TCHAR tszFaceName[512];
      _tcscpy(tszFaceName, tszEntryName);
      _TCHAR *p = _tcsrchr(tszFaceName, '(');
      if (p != NULL)
         *p = '\0';

	  if (tszFaceName[_tcslen(tszFaceName) - 1] == _T(' '))
		  tszFaceName[_tcslen(tszFaceName) - 1] = _T('\0');

      if (_tcscmp(tszFontFace, tszFaceName) == 0)
      {
         bFoundFontInfo = true;
         UINT nStringLength = _tcslen(tszEntryValue);
         _tcscpy(tszTtfName, tszEntryValue);
         tszTtfName[nStringLength] = '\0';
      }
      else
      {
          //BUGFIX for BLECPUB-1878
          /*
          * Sometimes GetOutlineTextMetrics returns a font name value different than the value from registry 
          * Check if used logfont face name is part of the string returned by lpoltm->otmpFaceName;
          * If so use the font file read from registry to construct the font used in the file
          * Apply GetOutlineTextMetrics to the contstructed font
          * If the string retuned by the last call of GetOutlineTextMetrics matches the string returned in the first call
          * then the font file is found, else continue search
          */
          _TCHAR *ptszFoundString;
          // try to fine used logfont face name string in the string returned by GetOutlineTextMetrics
          ptszFoundString = _tcsstr(tszFaceName, tszLfFaceName);
          if (ptszFoundString != NULL)
          {
              _TCHAR tszNewFontFace[512];
              bool success = true;
              bool bFoundNewFontInfo = false;

              _TCHAR tszFontFileName[512];

              Gdiplus::PrivateFontCollection pfc;
              _tcscpy(tszFontFileName, strFontFilesPath);
              _tcscat(tszFontFileName, tszEntryValue);

              Gdiplus::Status stat = pfc.AddFontFile(tszFontFileName);
              success = (stat == Gdiplus::Ok);
              int nFams = 0;
              if (success)
                  nFams = pfc.GetFamilyCount();
              if (nFams > 0)
              {
                  Gdiplus::FontFamily *aFams = new Gdiplus::FontFamily[nFams];
                  int nFound = 0;
                  stat = pfc.GetFamilies(nFams, aFams, &nFound);
                  success = (stat == Gdiplus::Ok);
                  WCHAR wszName[LF_FACESIZE];
                  if (success)
                  {
                      stat = aFams[0].GetFamilyName(wszName);
                      success = (stat == Gdiplus::Ok);
                  }

                  INT fontStyle = -1;
                  if (success)
                  {
                      if (FALSE != aFams[0].IsStyleAvailable(Gdiplus::FontStyleRegular))
                          fontStyle = Gdiplus::FontStyleRegular;
                      else if (FALSE != aFams[0].IsStyleAvailable(Gdiplus::FontStyleBold))
                          fontStyle = Gdiplus::FontStyleBold;
                      else if (FALSE != aFams[0].IsStyleAvailable(Gdiplus::FontStyleItalic))
                          fontStyle = Gdiplus::FontStyleItalic;
                      else if (FALSE != aFams[0].IsStyleAvailable(Gdiplus::FontStyleBoldItalic))
                          fontStyle = Gdiplus::FontStyleBoldItalic;
                  }

                  if (fontStyle != -1)
                  {
                      Gdiplus::Graphics graphics(hdc);
                      Gdiplus::Font font(&aFams[0], 10.0, fontStyle);
#ifdef _UNICODE
                      LOGFONTW lf;
                      stat = font.GetLogFontW(&graphics, &lf);
                      success = (stat == Gdiplus::Ok);
                      HFONT hfont = NULL;
                      if (success)
                          hfont = ::CreateFontIndirectW(&lf);
#else
                      LOGFONTA lf;
                      stat = font.GetLogFontA(&graphics, &lf);
                      success = (stat == Gdiplus::Ok);
                      HFONT hfont = NULL;
                      if (success)
                          hfont = ::CreateFontIndirectA(&lf);
#endif
                      success = (hfont != NULL);
                      if (success)
                      {
                          HGDIOBJ hOld = ::SelectObject(hdc, hfont);

                          UINT nSize = ::GetOutlineTextMetrics(hdc, 0, NULL);
                          if (nSize > 0)
                          {
                              LPOUTLINETEXTMETRIC lpOtm = (LPOUTLINETEXTMETRIC) new char[nSize];

                              nSize = ::GetOutlineTextMetrics(hdc, nSize, lpOtm);

                              if (nSize != 0)
                              {
                                  _tcscpy(tszNewFontFace, (_TCHAR *)((DWORD) lpOtm + lpOtm->otmpFaceName));

                                  if (_tcscmp(tszNewFontFace, tszFontFace) == 0)
                                  {
                                      bFoundNewFontInfo = true;
                                  }
                              }

                              delete[] lpOtm;
                          }

                          ::SelectObject(hdc, hOld);
                          ::DeleteObject(hfont);
                      }
                  }

                  delete[] aFams;
              }
              if (bFoundNewFontInfo)
              {
                  bFoundFontInfo = true;
                  UINT nStringLength = _tcslen(tszEntryValue);
                  _tcscpy(tszTtfName, tszEntryValue);
                  tszTtfName[nStringLength] = '\0';
              }
          }
      }

   }

   ::DeleteDC(hdc);
   Gdiplus::GdiplusShutdown(token);
   ::CoUninitialize();

   RegCloseKey(hKeyMachine);

   if (bFoundFontInfo)
      return tszTtfName;
   else
      return NULL;

}

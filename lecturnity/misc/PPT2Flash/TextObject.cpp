
#include "StdAfx.h"
#include "DrawObjects.h"


ASSISTANT::Text::Text(double _x, double _y, double _width, double _height, int _zPosition, 
                      LPCTSTR _color, LPCTSTR _textString, LPCTSTR _family, LPCTSTR _size, 
                      LPCTSTR _weight, LPCTSTR _slant, LPCTSTR _position, int _textStyle, UINT _id, 
                      LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects, LPCTSTR linkColor)
                      : ColoredObject(_x, _y, _width, _height, _zPosition, _color, _T("none"), 0, _T("none"), _id, _hyperlink, _currentDirectory, _linkedObjects)
{
 
   fontFamily   = _family;
   fontSize     = _size;
   fontWeight   = _weight;
   fontSlant    = _slant;
   textPosition = _position;
   
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
   
   // Compute dimension
   FillLogFont(m_logFont, fontFamily, drawSize, fontWeight, fontSlant);
   fAscent  = GetTextAscent(&m_logFont);
   fDescent = GetTextDescent(&m_logFont);
   if (m_dWidth == 0)
   {
      m_bTextWidthIsStatic = false;
      if (text.GetLength() > 0)
         m_dWidth = GetTextWidth(text, text.GetLength(), &m_logFont);
   }
   m_dHeight = fAscent + fDescent;
   
   if (_hyperlink)
      hyperlink_ = _hyperlink;
   else
      hyperlink_.Empty();
   
   if (linkColor)
      activatedLinkColor = linkColor;
   else
      activatedLinkColor.Empty();
   
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
   activatedLinkColor.Empty();
}


double ASSISTANT::Text::GetWidth()
{
   return m_dWidth;
}


double ASSISTANT::Text::GetHeight()
{
   return m_dHeight;
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


char ASSISTANT::Text::GetChar(int idx) 
{
   if ((idx < 0) || (idx > EndPos())) return '\0';
   
   return text[idx];
}


int ASSISTANT::Text::EndPos()
{ 
   return (text.GetLength()-1);
}


LPCTSTR ASSISTANT::Text::GetColor()
{ 
   return color;
}


LPCTSTR ASSISTANT::Text::GetFamily()
{ 
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


LPCTSTR ASSISTANT::Text::GetPosition()
{ 
   return textPosition;
}


void ASSISTANT::Text::ChangeString(LPCTSTR _textString)
{ 
   text.Empty();
   text = _textString;
   m_bTextWidthIsStatic = false;
   if (text.GetLength() > 0)
      m_dWidth  = GetTextWidth(text, text.GetLength(), &m_logFont);
   else
      m_dWidth = 0;
}

void ASSISTANT::Text::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;
   
   if (text.IsEmpty())
      return;

   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

   // Move and Scale
   graphics.TranslateTransform((Gdiplus::REAL)dOffX, (Gdiplus::REAL)dOffY);
   
   Gdiplus::Font gdipFont(pDC->m_hDC, &m_logFont);
   Gdiplus::FontFamily fontFamily;
   gdipFont.GetFamily(&fontFamily);

   int fontStyle = GetValidFontStyle(fontFamily);
   Gdiplus::REAL ascent = fontFamily.GetCellAscent(fontStyle) * 
                          (gdipFont.GetSize() / fontFamily.GetEmHeight(fontStyle));

   Gdiplus::PointF drawPoint;
   drawPoint.X = (Gdiplus::REAL)m_dX;
   drawPoint.Y = (Gdiplus::REAL)(m_dY - ascent);

   // convert text string to WCHAR format
   int iStringLength = text.GetLength();
   WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
#ifdef _UNICODE
   wcscpy(wcString, text);
#else
   MultiByteToWideChar(CP_ACP, 0, text, iStringLength+1, 
                       wcString, iStringLength+1);
#endif

   // compute real text width
   Gdiplus::StringFormat measureStringFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
   Gdiplus::RectF bbox;
   
   graphics.MeasureString(wcString, iStringLength, &gdipFont, Gdiplus::PointF(0.0, 0.0), &measureStringFormat, &bbox);

   Gdiplus::CharacterRange charRange(0, iStringLength);
   measureStringFormat.SetMeasurableCharacterRanges(1, &charRange);
   Gdiplus::Region pRangeRegion[1];
   graphics.MeasureCharacterRanges(wcString, iStringLength,
                                   &gdipFont, bbox, &measureStringFormat, 1, pRangeRegion);
   // get bounding rectangle
   Gdiplus::RectF rect;
   pRangeRegion[0].GetBounds(&rect, &graphics);

   Gdiplus::REAL realTextWidth = rect.Width;
   if (iStringLength > 1 && m_dWidth != 0 && realTextWidth != m_dWidth)
   {
      Gdiplus::REAL scaleX = (Gdiplus::REAL)(m_dWidth / realTextWidth);
      graphics.ScaleTransform(scaleX, 1.0);
      drawPoint.X = (Gdiplus::REAL)(m_dX / scaleX); //(x_*scaleX - x_);
   }
   // the text should begin at x, y
   drawPoint.X -= rect.X;

   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   Gdiplus::Color clrText(m_argbLineColor);
   Gdiplus::SolidBrush solidBrush(clrText);
   graphics.DrawString(wcString, iStringLength, &gdipFont, drawPoint, &measureStringFormat, &solidBrush);
   
   if (wcString)
      delete wcString;
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
   int textWidth = (int)m_dWidth;
   if (textWidth == 0 && text.GetLength() > 0)
      textWidth = (int)GetTextWidth(text, text.GetLength(), &m_logFont);
   *minX = (int)(m_dX - 3);
   *minY = (int)(m_dY - fAscent - 3);
   *maxX = (int)(m_dX + textWidth + 3);
   *maxY = (int)(m_dY + fDescent + 3);
}

void ASSISTANT::Text::GetBoundingBox(CRect &rcObject)
{
   int textWidth = (int)m_dWidth;
   if (textWidth == 0 && text.GetLength() > 0)
      textWidth = (int)GetTextWidth(text, text.GetLength(), &m_logFont);

   rcObject.left = (int)m_dX;
   rcObject.right = (int)(m_dX + textWidth);
   rcObject.top = (int)(m_dY - fAscent);
   rcObject.bottom = (int)(m_dY + fDescent);
   
   rcObject.NormalizeRect();
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
   
   ret = new Text(m_dX, m_dY, m_dWidth, m_dHeight, -1, color, text, fontFamily,
      fontSize, fontWeight, fontSlant, textPosition,
      textStyle, 0, hyperlink_, currentDirectory_, m_ssLinkedObjects, activatedLinkColor);
   
   if (isInternLink)
      ret->LinkIsIntern(true);
   else
      ret->LinkIsIntern(false);
   
   return (DrawObject *)ret;
}

void ASSISTANT::Text::SetFontUnsupported()
{
   m_bHasSupportedFont = false;
   // Arial is default font
   FillLogFont(m_logFont, _T("Arial"), drawSize, fontWeight, fontSlant);
   fAscent  = GetTextAscent(&m_logFont);
   fDescent = GetTextDescent(&m_logFont);
   if (!m_bTextWidthIsStatic)
      if (text.GetLength() > 0)
         m_dWidth  = GetTextWidth(text, text.GetLength(), &m_logFont);
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
   int
      x, y,
      width, height,
      zPosition,
      iPPTId;
   TCHAR 
      *color,
      *size,
      *family,
      *weight,
      *slant,
      *position,
      *hyperlink,
      *internLink,
      *currentDirectory,
      *linkedObjects,
      *linkColor;
   
   tmp = hilf->GetAttribute("x");
   if (tmp) 
      x = _ttoi(tmp);
   else     
      x = 0;
   
   tmp = hilf->GetAttribute("y");
   if (tmp) 
      y = _ttoi(tmp);
   else     
      y = 0;
   
   tmp = hilf->GetAttribute("width");
   if (tmp) 
      width = _ttoi(tmp);
   else     
      width = 0;
   
   tmp = hilf->GetAttribute("height");
   if (tmp) 
      height = _ttoi(tmp);
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
   
   tmp = hilf->GetAttribute("position");
   if (tmp) 
      position = _tcsdup(tmp);
   else     
      position = _tcsdup(_T("normal"));
   
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
   
   if (hyperlink != NULL)
   {
      obj = new Text(x,y,width,height,zPosition, color,hilf->Parameter,family,size,weight,slant,position,
         textStyle, 0, hyperlink, currentDirectory, linkedObjects, linkColor);
      obj->LinkIsIntern(false);
   }
   else
   {
      obj = new Text(x,y,width,height,zPosition, color,hilf->Parameter,family,size,weight,slant,position,
         textStyle, 0, internLink, currentDirectory, linkedObjects, linkColor);
      obj->LinkIsIntern(true);
   }
   
   if (obj && iPPTId > 0)
      obj->SetPPTObjectId(iPPTId);

   delete color;
   delete size;
   delete family;
   delete weight;
   delete slant;
   delete position;
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
   
   if (_tcscmp(GetColor(), ((Text *)obj)->GetColor()) != 0)
      return false;
   
   if (_tcscmp(GetString(), ((Text *)obj)->GetString()) != 0)
      return false;
   
   if (_tcscmp(GetSize(), ((Text *)obj)->GetSize()) != 0)
      return false;
   
   if (_tcscmp(GetWeight(), ((Text *)obj)->GetWeight()) != 0)
      return false;
   
   if (_tcscmp(GetSlant(), ((Text *)obj)->GetSlant()) != 0)
      return false;
   
   if (_tcscmp(GetPosition(), ((Text *)obj)->GetPosition()) != 0)
      return false;
   
   // all parameters are identic
   return true;
}

int ASSISTANT::Text::GetValidFontStyle(Gdiplus::FontFamily &fontFamily)
{
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleRegular))
      return Gdiplus::FontStyleRegular;
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleBold))
      return Gdiplus::FontStyleBold;
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleItalic))
      return Gdiplus::FontStyleItalic;
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleBoldItalic))
      return Gdiplus::FontStyleBoldItalic;
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleUnderline))
      return Gdiplus::FontStyleUnderline;
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleStrikeout))
      return Gdiplus::FontStyleStrikeout;
   return 0;
}

void ASSISTANT::Text::FillLogFont(LOGFONT &logFont, 
                     LPCTSTR fontFamily, int fontSize, 
                     LPCTSTR fontWeight, LPCTSTR fontSlant)
{
   ZeroMemory(&(logFont), sizeof(LOGFONT));  
   
   logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE; 
   
   logFont.lfHeight = (-1) * abs(fontSize);
   
   logFont.lfWidth       = 0;
   logFont.lfEscapement  = 0 ;
   logFont.lfOrientation = 0 ;
   
   if (_tcscmp(fontWeight, _T("bold")) == 0)
      logFont.lfWeight = FW_BOLD;
   else
      logFont.lfWeight = FW_NORMAL;
   
   if (_tcscmp(fontSlant, _T("italic")) == 0)
      logFont.lfItalic = true;
   else
      logFont.lfItalic = false;
   
   logFont.lfUnderline     = FALSE ;
   logFont.lfStrikeOut     = FALSE ;
   logFont.lfCharSet       = DEFAULT_CHARSET ;
   logFont.lfOutPrecision  = OUT_TT_PRECIS; //OUT_TT_ONLY_PRECIS ;   
   logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS ;   
   logFont.lfQuality       = PROOF_QUALITY; 
   
   _tcscpy(logFont.lfFaceName, fontFamily);  
}



LPTSTR ASSISTANT::Text::GetTtfName()
{   
   static _TCHAR tszTtfName[512];


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

   HDC hdc = ::CreateCompatibleDC(NULL);

   DWORD dwType;
   _TCHAR tszEntryName[512];
   _TCHAR tszEntryValue[512];
   
   HFONT font = CreateFontIndirect(&m_logFont);
   UINT cbData;       
   LPOUTLINETEXTMETRIC lpoltm;     
   SelectObject(hdc, font);
   cbData = GetOutlineTextMetrics (hdc, 0, NULL);
   DeleteObject(font);
   _TCHAR tszFontFace[512];
   if (cbData == 0) 
   {
      _tcscpy(tszFontFace, m_logFont.lfFaceName);
   }
   else
   {
      lpoltm = (LPOUTLINETEXTMETRIC)LocalAlloc (LPTR, cbData);
      GetOutlineTextMetrics (hdc, cbData, lpoltm);  
      _tcscpy(tszFontFace, (_TCHAR *)((DWORD) lpoltm + lpoltm->otmpFaceName));
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

   }

   ::DeleteDC(hdc);
   ::CoUninitialize();

   RegCloseKey(hKeyMachine);

   if (bFoundFontInfo)
      return tszTtfName;
   else
      return NULL;

}


double ASSISTANT::Text::GetTextAscent(LOGFONT *logFont)
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

double ASSISTANT::Text::GetTextDescent(LOGFONT *logFont)
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

double ASSISTANT::Text::GetTextWidth(const _TCHAR *textString, int textLength, LOGFONT *logFont)
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
   
   Gdiplus::Graphics graphics(hdc);
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
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
      delete wcString;

   ::DeleteObject(hdc);
   ::DeleteDC(hdc);

   return rect.Width;
}



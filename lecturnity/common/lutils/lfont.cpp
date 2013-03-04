// LFont.cpp: Implementierung der Klasse LFont.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LFont.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

LFont::LFont()
{

}

LFont::~LFont()
{

}


void LFont::FillWithDefaultValues(LOGFONT *pLf, LPCTSTR tszFontFamily)
{
   memset(pLf, 0, sizeof(LOGFONT));

   pLf->lfHeight = -16;
   pLf->lfWidth = 0;
   pLf->lfEscapement = 0;
   pLf->lfOrientation = 0;
   pLf->lfWeight = FW_NORMAL;
   pLf->lfItalic = FALSE;
   pLf->lfUnderline = FALSE;
   pLf->lfStrikeOut = FALSE;
   pLf->lfCharSet = DEFAULT_CHARSET;
   pLf->lfOutPrecision = OUT_TT_PRECIS;
   pLf->lfClipPrecision = CLIP_DEFAULT_PRECIS;
   pLf->lfQuality = ANTIALIASED_QUALITY;
   pLf->lfPitchAndFamily = FF_DONTCARE;
   if (tszFontFamily != NULL)
      _tcscpy(pLf->lfFaceName, tszFontFamily);
   else
      _tcscpy(pLf->lfFaceName, _T("Arial"));
}
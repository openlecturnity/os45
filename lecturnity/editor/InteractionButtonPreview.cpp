// InteractionButtonPreview.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "lutils.h"
#include "InteractionButtonPreview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInteractionButtonPreview

CInteractionButtonPreview::CInteractionButtonPreview()
{
   LFont::FillWithDefaultValues(&m_lfButton);

   m_clrFill = RGB(255, 255, 255);
   m_clrLine = RGB(197, 198, 184);

   m_pGdipImage = NULL;

   m_clrText = RGB(0, 0, 0);

   m_bIsImageButton = false;

   m_rcArea.SetRectEmpty();

   m_iLineStyle = 0;
   m_iLineWidth = 1;
   m_aLineStyles.Add(Gdiplus::DashStyleSolid);
   m_aLineStyles.Add(Gdiplus::DashStyleDot);
   m_aLineStyles.Add(Gdiplus::DashStyleDash);
}

CInteractionButtonPreview::~CInteractionButtonPreview()
{
   if (m_pGdipImage)
      delete m_pGdipImage;
   m_aLineStyles.RemoveAll();
}


BEGIN_MESSAGE_MAP(CInteractionButtonPreview, CStatic)
	//{{AFX_MSG_MAP(CInteractionButtonPreview)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CInteractionButtonPreview 

void CInteractionButtonPreview::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
   CRect rcClient;
   GetClientRect(rcClient);

   Gdiplus::RectF gdipRcClient((Gdiplus::REAL)rcClient.left, (Gdiplus::REAL)rcClient.top, 
                               (Gdiplus::REAL)rcClient.Width(), (Gdiplus::REAL)rcClient.Height()); 
   
   // Get text dimension
   Gdiplus::Graphics graphics(dc);
   
   // draw background rectangle
   Gdiplus::SolidBrush fillBrush(Gdiplus::Color(255, 255, 255, 255));
   graphics.FillRectangle(&fillBrush, gdipRcClient);
   
   Gdiplus::Pen pen(Gdiplus::Color(255, 197, 198, 184), 1);
   graphics.DrawRectangle(&pen, gdipRcClient.X, gdipRcClient.Y, 
      gdipRcClient.Width-1,gdipRcClient.Height-1);

   Gdiplus::RectF gdipRcRectangle;
   gdipRcRectangle.X = (Gdiplus::REAL)rcClient.left;
   gdipRcRectangle.Width = (Gdiplus::REAL)rcClient.Width();
   gdipRcRectangle.Y = (Gdiplus::REAL)rcClient.top;
   gdipRcRectangle.Height = (Gdiplus::REAL)rcClient.Height();

   int iStringLength = m_csButtonText.GetLength();
   WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
#ifdef _UNICODE
   wcscpy(wcString, m_csButtonText);
#else
   MultiByteToWideChar(CP_ACP, 0, m_csButtonText, iStringLength+1, 
      wcString, iStringLength+1);
#endif

   if (m_bIsImageButton)
   {
      if (m_pGdipImage)
      {
         gdipRcRectangle.X += 1;
         gdipRcRectangle.Y += 1;
         gdipRcRectangle.Width -= 2;
         gdipRcRectangle.Height -= 2;
         float dZoomX = gdipRcRectangle.Width / m_pGdipImage->GetWidth();
         float dZoomY = gdipRcRectangle.Height / m_pGdipImage->GetHeight();
         float dZoom = dZoomX > dZoomY ? dZoomY : dZoomX;
         if (dZoom > 1.0) 
            dZoom = 1.0;

         float dOffX = (gdipRcRectangle.X + (gdipRcRectangle.Width - m_pGdipImage->GetWidth() * dZoom) / 2);
         if (dOffX < 0)
            dOffX = 0;

         float dOffY = (gdipRcRectangle.Y + (gdipRcRectangle.Height - m_pGdipImage->GetHeight() * dZoom) / 2);
         if (dOffY < 0)
            dOffY = 0;

         graphics.TranslateTransform(dOffX, dOffY);
         graphics.ScaleTransform(dZoom, dZoom);

         graphics.DrawImage(m_pGdipImage, 0, 0, m_pGdipImage->GetWidth(), m_pGdipImage->GetHeight());
         

         Gdiplus::Color color;
         color.SetFromCOLORREF(m_clrText);
         Gdiplus::SolidBrush solidBrush(color);
         Gdiplus::Font gdipFont(dc, &m_lfButton);

         graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
         Gdiplus::StringFormat measureStringFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
         Gdiplus::RectF bbox;
         graphics.MeasureString(wcString, iStringLength, &gdipFont, Gdiplus::PointF(0.0, 0.0), 
                                &measureStringFormat, &bbox);

         Gdiplus::PointF gdipPtText;
         gdipPtText.X = (float)((m_pGdipImage->GetWidth() - bbox.Width) / 2);
         gdipPtText.Y = (float)((m_pGdipImage->GetHeight() - bbox.Height) / 2);

         graphics.DrawString(wcString, iStringLength, &gdipFont, gdipPtText, &solidBrush);
      }
      else
      {
         Gdiplus::Color color;
         color.SetFromCOLORREF(m_clrText);
         Gdiplus::SolidBrush solidBrush(color);
         Gdiplus::Font gdipFont(dc, &m_lfButton);

         graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
         Gdiplus::StringFormat measureStringFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
         Gdiplus::RectF bbox;
         graphics.MeasureString(wcString, iStringLength, &gdipFont, Gdiplus::PointF(0.0, 0.0), 
                                &measureStringFormat, &bbox);

         Gdiplus::PointF gdipPtText;
         gdipPtText.X = (float)((gdipRcRectangle.Width - bbox.Width) / 2);
         gdipPtText.Y = (float)((gdipRcRectangle.Height - bbox.Height) / 2);

         graphics.DrawString(wcString, iStringLength, &gdipFont, gdipPtText, &solidBrush);
      }

   }
   else
   {
      // draw button rectangle
      Gdiplus::Color fillColor;
      fillColor.SetFromCOLORREF(m_clrFill);
      Gdiplus::SolidBrush fillBrush(fillColor);
      graphics.FillRectangle(&fillBrush, gdipRcRectangle);
      
      Gdiplus::Color lineColor;
      lineColor.SetFromCOLORREF(m_clrLine);
      Gdiplus::Pen pen(lineColor, 1);
      pen.SetWidth((Gdiplus::REAL) m_iLineWidth);
      pen.SetDashStyle(m_aLineStyles.GetAt(m_iLineStyle));
      graphics.DrawRectangle(&pen, gdipRcRectangle.X, gdipRcRectangle.Y, 
                                   gdipRcRectangle.Width-1,gdipRcRectangle.Height-1);

      
      Gdiplus::Color color;
      color.SetFromCOLORREF(m_clrText);
      Gdiplus::SolidBrush solidBrush(color);
      Gdiplus::Font gdipFont(dc, &m_lfButton);
      
      graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
      Gdiplus::StringFormat measureStringFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
      Gdiplus::RectF bbox;
      graphics.MeasureString(wcString, iStringLength, &gdipFont, Gdiplus::PointF(0.0, 0.0), 
         &measureStringFormat, &bbox);
      
      Gdiplus::PointF gdipPtText;
      gdipPtText.X = (float)((gdipRcRectangle.Width - bbox.Width) / 2);
      gdipPtText.Y = (float)((gdipRcRectangle.Height - bbox.Height) / 2);
      
      graphics.DrawString(wcString, iStringLength, &gdipFont, gdipPtText, &solidBrush);
   }

 
   if (wcString)
      free(wcString);

}

void CInteractionButtonPreview::GetText(CString &csText, CRect &rcArea, LOGFONT &logFont)
{
	csText = m_csButtonText;
	rcArea = m_rcArea;
   memcpy(&logFont, &m_lfButton, sizeof(LOGFONT));
}

void CInteractionButtonPreview::SetButtonFont(LOGFONT *pLfButton)
{
   memcpy(&m_lfButton, pLfButton, sizeof(LOGFONT));
   RedrawWindow();
}

void CInteractionButtonPreview::SetColors(COLORREF clrFill, 
                                          COLORREF clrLine, COLORREF clrText)
{
   m_clrFill = clrFill;
   m_clrLine = clrLine;
   m_clrText = clrText;

   RedrawWindow();
}

void CInteractionButtonPreview::SetTextColor(COLORREF clrText)
{
   m_clrText = clrText;

   RedrawWindow();
}

void CInteractionButtonPreview::SetLineColor(COLORREF clrLine)
{
   m_clrLine = clrLine;

   RedrawWindow();
}
void CInteractionButtonPreview::SetFillColor(COLORREF clrFill)
{
   m_clrFill = clrFill;

   RedrawWindow();
}

void CInteractionButtonPreview::SetLineWidth(int iLineWidth)
{
    m_iLineWidth = iLineWidth;
    RedrawWindow();
}
void CInteractionButtonPreview::SetLineStyle(int iLineStyle)
{
    m_iLineStyle = iLineStyle;
    RedrawWindow();
}

void CInteractionButtonPreview::SetImage(CString &csImageFileName)
{
   if (m_pGdipImage != NULL)
      delete m_pGdipImage;

   m_pGdipImage = NULL;

   if (!csImageFileName.IsEmpty())
   {
      int iStringLength = csImageFileName.GetLength();
      WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
#ifdef _UNICODE
      wcscpy(wcString, csImageFileName);
#else
      MultiByteToWideChar(CP_ACP, 0, csImageFileName, iStringLength+1, 
         wcString, iStringLength+1);
#endif
      
      m_pGdipImage = Gdiplus::Image::FromFile(wcString, FALSE);

      if (wcString)
         free(wcString);
   }

   RedrawWindow();
}

void CInteractionButtonPreview::GetImageDimension(CSize &siImage)
{
   if (m_pGdipImage)
   {
      siImage.cx = m_pGdipImage->GetWidth();
      siImage.cy = m_pGdipImage->GetHeight();
   }
   else
   {
      siImage.cx = 0;
      siImage.cy = 0;
   }
}

void CInteractionButtonPreview::IsImageButton(bool bIsImageButton)
{
   m_bIsImageButton = bIsImageButton;

   RedrawWindow();
}

void CInteractionButtonPreview::CalculateImageRect(Gdiplus::RectF gdipRcClient, CRect rcArea, 
                                                   Gdiplus::RectF &gdipRcImage)
{
   if (m_pGdipImage)
   {
      int iImageWidth = m_pGdipImage->GetWidth();
      int iImageHeight = m_pGdipImage->GetHeight();

      if (iImageWidth <= gdipRcClient.Width)
         gdipRcImage.X = (gdipRcClient.Width - iImageWidth) / 2;
      else
         gdipRcImage.X = gdipRcClient.X;

      if (iImageHeight <= gdipRcClient.Height)
         gdipRcImage.Y = (gdipRcClient.Height - iImageHeight) / 2;
      else
         gdipRcImage.Y = gdipRcClient.Y;
   }
   else
   {
      gdipRcImage.X = (Gdiplus::REAL)rcArea.left;
      gdipRcImage.Y = (Gdiplus::REAL)rcArea.top;
      gdipRcImage.Width = (Gdiplus::REAL)rcArea.Width();
      gdipRcImage.Height = (Gdiplus::REAL)rcArea.Height();
      gdipRcImage.Offset(gdipRcClient.GetLeft(), gdipRcClient.GetTop());
   }

}

void CInteractionButtonPreview::CalculateRectangleRect(Gdiplus::RectF gdipRcClient, Gdiplus::RectF gdipRcText, 
                                                       Gdiplus::RectF &gdipRcRectangle)
{
   if (gdipRcText.Width <= gdipRcClient.Width)
      gdipRcRectangle.X = (gdipRcClient.Width - gdipRcText.Width) / 2;
   else
      gdipRcRectangle.X = (Gdiplus::REAL)gdipRcClient.X;
   gdipRcRectangle.Width = gdipRcText.Width;

   
   if (gdipRcText.Height <= gdipRcClient.Height)
      gdipRcRectangle.Y = (gdipRcClient.Height - gdipRcText.Height) / 2;
   else
      gdipRcRectangle.Y = (Gdiplus::REAL)gdipRcClient.Y;
   gdipRcRectangle.Height = gdipRcText.Height;
}

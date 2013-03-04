// ImageStatic.cpp: Implementierung der Klasse CImageStatic.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageStatic.h"

#define DRAWSDK_EXPORT /* LIB import */
#include "objects.h" // DrawSdk

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CImageStatic::CImageStatic() : CStatic()
{
   m_pImage = NULL;
   m_bHasCalcedSize = false;
   m_bDrawImageSize = true;

   m_font.CreatePointFont(85, _T("Tahoma"), NULL);
}

CImageStatic::~CImageStatic()
{
}

BEGIN_MESSAGE_MAP(CImageStatic, CStatic)
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_DESTROY()
END_MESSAGE_MAP()

void CImageStatic::ReadImage(const _TCHAR *tszFileName)
{
   if (m_pImage)
      delete m_pImage;
   m_pImage = NULL;
   if (tszFileName)
   {
      if (_tcscmp(tszFileName, _T("")) != 0)
      {
         m_bHasCalcedSize = false;
         m_pImage = new DrawSdk::Image(0, 0, 0, 0, tszFileName);
         m_origWidth = m_pImage->GetImageWidth();
         m_origHeight = m_pImage->GetImageHeight();
      }
   }

   RedrawWindow();
}

void CImageStatic::OnDestroy()
{
   if (m_pImage)
      delete m_pImage;
   m_pImage = NULL;
   m_font.DeleteObject();

   CStatic::OnDestroy();
}

void CImageStatic::OnSize(UINT nType, int cy, int cx)
{
   m_bHasCalcedSize = false;
   RedrawWindow();
}

void CImageStatic::OnPaint()
{
   PAINTSTRUCT paintStruct;

   DWORD dwRes = GetUpdateRect(NULL);
   if (dwRes == 0)
      return;
   CDC *pDC = BeginPaint(&paintStruct);

   CRect rect;
   GetClientRect(&rect);
   CBrush brush(::GetSysColor(COLOR_3DFACE));
   pDC->FillRect(&rect, &brush);
   brush.DeleteObject();

   if (m_pImage)
   {
      if (!m_bHasCalcedSize)
      {
         int clientWidth = rect.Width();
         int clientHeight = rect.Height();

         int drawWidth = -1;
         int drawHeight = -1;
         if (m_origWidth <= clientWidth && m_origHeight <= clientHeight)
         {
            drawWidth = m_origWidth;
            drawHeight = m_origHeight;
         }
         else
         {
            double fH = ((double) clientWidth) / ((double) m_origWidth);
            double fV = ((double) clientHeight) / ((double) m_origHeight);
            double f = min(fV, fH);

            drawWidth = (int) (f * ((double) m_origWidth));
            drawHeight = (int) (f * ((double) m_origHeight));
         }

         m_pImage->SetWidth(drawWidth);
         m_pImage->SetHeight(drawHeight);
      }
      m_pImage->Draw(pDC->GetSafeHdc());

      if (m_bDrawImageSize)
      {
         CString csSize;
         csSize.Format(_T("%dx%d"), m_origWidth, m_origHeight);

         int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
         CFont *pOldFont = pDC->SelectObject(&m_font);
         pDC->DrawText(csSize, &rect, DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
         if (pOldFont)
            pDC->SelectObject(pOldFont);
         pDC->SetBkMode(nOldBkMode);
      }
   }
   else
   {
      pDC->Rectangle(&rect);
      CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
      CPen *pOldPen = pDC->SelectObject(&pen);
      pDC->MoveTo(rect.left, rect.top);
      pDC->LineTo(rect.right - 1, rect.bottom - 1);
      pDC->MoveTo(rect.left, rect.bottom - 1);
      pDC->LineTo(rect.right - 1, rect.top);
      if (pOldPen)
         pDC->SelectObject(pOldPen);
      pen.DeleteObject();
   }

   EndPaint(&paintStruct);
}

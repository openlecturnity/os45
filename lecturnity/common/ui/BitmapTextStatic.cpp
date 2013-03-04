// BitmapTextStatic.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BitmapTextStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapTextStatic

CBitmapTextStatic::CBitmapTextStatic()
{
   m_textColor = RGB(255, 255, 255);
}

CBitmapTextStatic::~CBitmapTextStatic()
{
}


BEGIN_MESSAGE_MAP(CBitmapTextStatic, CWnd)
	//{{AFX_MSG_MAP(CBitmapTextStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CBitmapTextStatic 

BOOL CBitmapTextStatic::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, const POINT& point, CWnd* pParentWnd, UINT nID, UINT nBitmapID, CCreateContext* pContext) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
   BOOL res = 	CWnd::Create(NULL, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

   if (FALSE != res)
   {
      m_textPos = point;
      res = m_pictureHolder.CreateFromBitmap(nBitmapID);
   }

   return res;
}

BOOL CBitmapTextStatic::SetFont(int logPixelSize, const CString &typeFace)
{
   CDC dc;
   dc.CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
   int dpiY = dc.GetDeviceCaps(LOGPIXELSY);
   int fontSize = logPixelSize * 96 / dpiY;
   if (m_font.m_hObject)
      m_font.DeleteObject();
   BOOL res = m_font.CreatePointFont(fontSize, typeFace, &dc);
   if (FALSE != res)
      CWnd::SetFont(&m_font);
   return res;
}

void CBitmapTextStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
   CRect rect;
   GetClientRect(&rect);

   m_pictureHolder.Render(&dc, &rect, &rect);

   if (!m_font.m_hObject)
   {
      int dpiY = dc.GetDeviceCaps(LOGPIXELSY);
      int fontSize = 85 * 96 / dpiY;
      m_font.CreatePointFont(fontSize, _T("Arial"), &dc);
   }
   if (m_font.m_hObject)
   {
      CString text;
      GetWindowText(text);

      int oldMode = dc.SetBkMode(TRANSPARENT);
      CFont *pOldFont = dc.SelectObject(&m_font);
      // CFont *pOldFont = dc.SelectObject(GetFont());
      COLORREF oldColor = dc.SetTextColor(m_textColor);

      rect += m_textPos;

      dc.DrawText(text, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

      dc.SetTextColor(oldColor);
      dc.SelectObject(pOldFont);
      dc.SetBkMode(oldMode);
   }
	// Kein Aufruf von CWnd::OnPaint() für Zeichnungsnachrichten!!!
}

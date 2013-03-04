// ImageButton.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ImageButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageButton

CImageButton::CImageButton()
{
   m_nBorder        = 1;
   m_lfEscapement   = 0;
   m_hRgn           = 0;
   m_bHover         = false;
   m_bCapture       = false;
   m_bMouseDown     = false;
   m_bButtonPressed = false;
   m_bNeedBitmaps   = true;
}

CImageButton::~CImageButton()
{
}


BEGIN_MESSAGE_MAP(CImageButton, CButton)
	//{{AFX_MSG_MAP(CImageButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CImageButton 
BOOL CImageButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, CRect rect, CWnd* pParentWnd, UINT nID, UINT bitmapID, int cx, int cy, int nImgs, HRGN hRgn)
{
	// store region in member variable
	DeleteObject(m_hRgn);
	m_hRgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
   m_nImgs = nImgs;

	CRect box(0, 0, 0, 0);
   
	if (hRgn != 0) 
		CombineRgn(m_hRgn, hRgn, 0, RGN_COPY);

	// make sure that region bounding rect is located in (0, 0)
	GetRgnBox(m_hRgn, &box);
	OffsetRgn(m_hRgn, -box.left, -box.top);
	GetRgnBox(m_hRgn, &box);

	box.OffsetRect(CPoint(rect.left, rect.top));

   m_buttonImageList.Create(cx, cy, ILC_COLOR24, 1, nImgs );
   CBitmap bitmap;
   bitmap.LoadBitmap(bitmapID);
   m_buttonImageList.Add(&bitmap, RGB(0, 0, 255));
   bitmap.DeleteObject();
   
   m_nID = nID;

   m_textX = 5;
   m_textY = 3;

	return CButton::Create(lpszCaption, dwStyle, box, pParentWnd, nID);
}


void CImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Code einfügen, um das angegebene Element zu zeichnen
	
	// prepare DC
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct -> hDC);
	CRect rect;
	GetClientRect(rect);


	// draw button to the screen

	DrawButton(pDC, &rect, lpDrawItemStruct->itemState);

   if (!m_font.m_hObject)
   {
      int dpiY = pDC->GetDeviceCaps(LOGPIXELSY);
      int fontSize = 85 * 96 / dpiY;
      m_font.CreatePointFont(fontSize, "Arial", pDC);
   }
   if (m_font.m_hObject)
   {
      int oldMode = pDC->SetBkMode(TRANSPARENT);
      CFont *pOldFont = pDC->SelectObject(&m_font);

      CString caption;
      GetWindowText(caption);

      rect.left += m_textX;
      rect.top  += m_textY;

      if (m_bHover)
         pDC->SetTextColor(RGB(218, 133, 0));
      else
         pDC->SetTextColor(RGB(0, 0, 0));
      pDC->DrawText(caption, &rect, 0);

      pDC->SelectObject(pOldFont);
      pDC->SetBkMode(oldMode);
   }
}

void CImageButton::DrawButton(CDC * pDC, CRect * pRect, UINT state)
{
   if (state & ODS_DISABLED)
   {
      m_buttonImageList.Draw(pDC, m_nImgs > 3 ? 3 : 0, CPoint(0, 0), ILD_NORMAL);
   }
	else if (state & ODS_SELECTED)
   {
      m_buttonImageList.Draw(pDC, 2, CPoint(0, 0), ILD_NORMAL);
   }
	else {
		if (m_bHover)
      {
         m_buttonImageList.Draw(pDC, 1, CPoint(0, 0), ILD_NORMAL);
      }
		else 
      {
         m_buttonImageList.Draw(pDC, 0, CPoint(0, 0), ILD_NORMAL);
      }
	}	
   
}

void CImageButton::PreSubclassWindow() 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
	// change window style to allow owner draw
	ModifyStyle(0, BS_OWNERDRAW | BS_PUSHBUTTON);	

	CButton::PreSubclassWindow();
}

void CImageButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	// Test if mouse is above the button.
   
	if (!m_bMouseDown)
		CheckHover(point);

	CButton::OnMouseMove(nFlags, point);
}

void CImageButton::CheckHover(CPoint point)
{
   
   if (HitTest(point)) {
		SetCapture();
	   if (!m_bHover) {
		   m_bHover = true;
		   RedrawWindow();
         this->GetParent()->SendMessage(WM_USER, m_nID);
	   }
   }
   else {
		ReleaseCapture();
	   m_bHover = false;
	   RedrawWindow();
      this->GetParent()->SendMessage(WM_USER, -1);
   }
}


BOOL CImageButton::HitTest(CPoint point)
{
	BOOL result = false;

	// Obtain handle to window region.
	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
	GetWindowRgn(hRgn);
	CRect rgnRect;
	GetRgnBox(hRgn, &rgnRect);

	// First check if point is in region bounding rect.
	// Then check if point is in the region in adition to being in the bouding rect.
	result = PtInRect(&rgnRect, point) && PtInRegion(hRgn, point.x, point.y);

	// Clean up and exit.
	DeleteObject(hRgn);

   /*
   CString message;
   message.Format("HitTest: %d, %d %d : %d %d %d %d", m_nID, point.x, point.y, rgnRect.left, rgnRect.top, rgnRect.right, rgnRect.bottom);
   MessageBox(message);
   */

	return result;
}


void CImageButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	// record that mouse is down

	m_bMouseDown = true;
   SetCapture();
	m_bHover = false;
	CButton::OnLButtonDown(nFlags, point);
}

void CImageButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	// record that mouse is released
	CButton::OnLButtonUp(nFlags, point);

	m_bMouseDown = false;
   ReleaseCapture();
	m_bHover = false;
	CheckHover(point);
}


int CImageButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	// assign new region to a window
	m_bNeedBitmaps = true;
	SetWindowRgn(m_hRgn, true);

	return 0;
}

BOOL CImageButton::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	return TRUE;
}

LRESULT CImageButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	// I have noticed that default windows buttons can be clicked very quickly.
	// Double or single click both result in a button being pushed down.
	// For owner drawn buttons this is not the case. Double click does
	// not push button down. Here is a solution for the problem:
	// double click message is substituted for single click.

	if (message == WM_LBUTTONDBLCLK)
		message = WM_LBUTTONDOWN; 


	return CButton::DefWindowProc(message, wParam, lParam);
}


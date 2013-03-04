// HoverButton.cpp : implementation file
//

#include "stdafx.h"
#include "HoverButton.h"
#include "..\Studio\Resource.h"

#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CHoverButtonEx

CHoverButtonEx::CHoverButtonEx(UINT nOnPressMessage)
{
	m_bHover = FALSE;
	m_bTracking = FALSE;
	m_track = NULL;
	m_ToolTip = NULL;
	m_ButtonSize.cx = 0;
	m_ButtonSize.cy = 0;
	m_bAllowMove = FALSE;

   m_nWidth = 0;
   m_nHeight = 0;  
   m_bLoadBkgrnd = FALSE;

   m_WMOnPressButton = nOnPressMessage;
   
   m_pBackground = NULL;
   m_pImageNormal = NULL;
   m_pImageMOver = NULL;
   m_pImagePressed = NULL;
}

CHoverButtonEx::~CHoverButtonEx()
{
	delete m_ToolTip;
}
IMPLEMENT_DYNAMIC(CHoverButtonEx, CBitmapButton)

BEGIN_MESSAGE_MAP(CHoverButtonEx, CBitmapButton)
	//{{AFX_MSG_MAP(CHoverButtonEx)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
 //	CHoverButtonEx message handlers

BOOL CHoverButtonEx::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


void CHoverButtonEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	//	TODO: Add your message handler code here and/or call default
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
		m_point = point;
	}
   
	CBitmapButton::OnMouseMove(nFlags, point);
}

BOOL CHoverButtonEx::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_ToolTip != NULL)
		if (::IsWindow(m_ToolTip->m_hWnd)) // Incase m_ToolTip isn't NULL, check to see if its a valid window
			m_ToolTip->RelayEvent(pMsg);		
	return CButton::PreTranslateMessage(pMsg);
}

// Set the tooltip with a string resource
void CHoverButtonEx::SetToolTipText(UINT nId, BOOL bActivate)
{
	// load string resource
	m_tooltext.LoadString(nId);
	// If string resource is not empty
	if (m_tooltext.IsEmpty() == FALSE) SetToolTipText(m_tooltext, bActivate);

}

// Set the tooltip with a CString
void CHoverButtonEx::SetToolTipText(CString spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText.IsEmpty()) return;

	// Initialize ToolTip
	InitToolTip();
	m_tooltext = spText;

	// If there is no tooltip defined then add it
	if (m_ToolTip->GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip->AddTool(this, m_tooltext, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip->UpdateTipText(m_tooltext, this, 1);
	m_ToolTip->SetDelayTime(2000);
	ActivateTooltip(bActivate);
}

void CHoverButtonEx::InitToolTip()
{
	if (m_ToolTip == NULL)
	{
		m_ToolTip = new CToolTipCtrl;
		// Create ToolTip control
		m_ToolTip->Create(this);
	}
} // End of InitToolTip

// Activate the tooltip
void CHoverButtonEx::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip->GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip->Activate(bActivate);
} // End of EnableTooltip


void CHoverButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

	if ((m_ButtonSize.cx == 0) || (m_ButtonSize.cy == 0)) // If our Button size if 0, we have no bitmap so draw Regular...
	{
		UINT style = GetButtonStyle();
		style &= ~BS_OWNERDRAW;
		SetButtonStyle(style); //No Bitmap so remove OwnerDraw and Refresh
		return;
	}
   
	if (m_pBackground == NULL || !m_bLoadBkgrnd)
   {
      CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
      
      CDC memDC;
      memDC.CreateCompatibleDC(pDC);
      
	   CPalette *pPalette = NULL;
      pPalette = pDC->GetCurrentPalette();
      
      CBitmap bmpBg;
      CRect r;
      GetClientRect(&r);

      CSize sz(r.Width(), r.Height());
      bmpBg.CreateCompatibleBitmap(pDC, sz.cx, sz.cy);
      CBitmap * oldbm = memDC.SelectObject(&bmpBg);
      memDC.BitBlt(0, 0, sz.cx, sz.cy, pDC, 0, 0, SRCCOPY);

      if (m_pBackground)
         delete m_pBackground;
      m_pBackground = NULL;

      m_pBackground = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)bmpBg.GetSafeHandle(), (HPALETTE)pPalette->GetSafeHandle());

      m_bLoadBkgrnd = TRUE;
	}

	if(lpDrawItemStruct->itemState & ODS_SELECTED)
	{
      DrawTransparentBitmap(lpDrawItemStruct->hDC, 2, 0, 0, m_ButtonSize.cx, m_ButtonSize.cy);
	}
	else
	{
		if(m_bHover)
		{
         DrawTransparentBitmap(lpDrawItemStruct->hDC, 1, 0, 0, m_ButtonSize.cx, m_ButtonSize.cy);
		}
      else
		{
         DrawTransparentBitmap(lpDrawItemStruct->hDC, 0, 0, 0, m_ButtonSize.cx, m_ButtonSize.cy);
		}	
	}
	// clean up
}

// This routine loads raw bitmap data from a resource.
// The width and height must be defined as there is no bitmap header
//
// Note that the resource type is "PNG" here, but can be changed to 
// any name, or passed as a parameter if desired
BOOL CHoverButtonEx::LoadPng()
{
   UINT style = GetButtonStyle();//If Bitmaps are being loaded and the BS_OWNERDRAW is not set
   // then reset style to OwnerDraw.
   if (!(style & BS_OWNERDRAW))
   {
      style |= BS_OWNERDRAW;
      SetButtonStyle(style);
   }
   
   
   m_pImageNormal = Load(IDR_TRANS_NORMAL);
   m_pImageMOver = Load(IDR_TRANS_MOVER);
   m_pImagePressed = Load(IDR_TRANS_PRESSED);
   
   
   m_nWidth = m_pImageNormal->GetWidth();
   m_nHeight = m_pImageNormal->GetHeight();;
   
   m_ButtonSize.cx = m_nWidth;
   m_ButtonSize.cy = m_nHeight;
   
   SetWindowPos( NULL, 0,0, m_ButtonSize.cx, m_ButtonSize.cy, SWP_NOMOVE | SWP_NOOWNERZORDER);
   
   return TRUE;
}

Gdiplus::Bitmap *CHoverButtonEx::Load(int iId)
{
   HINSTANCE hInst = AfxGetInstanceHandle();

   HRSRC hResource = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(iId), _T("PNG"));
   if (!hResource)
      return NULL;
   
   DWORD imageSize = ::SizeofResource(hInst, hResource);
   if (!imageSize)
      return NULL;
   
   const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
   if (!pResourceData)
      return NULL;
   
   Gdiplus::Bitmap *pBitmap = NULL;
   
   m_hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
   if (m_hBuffer)
   {
      void* pBuffer = ::GlobalLock(m_hBuffer);
      if (pBuffer)
      {
         CopyMemory(pBuffer, pResourceData, imageSize);
         
         IStream* pStream = NULL;
         if (::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
         {
            pBitmap = Gdiplus::Bitmap::FromStream(pStream);
            pStream->Release();
            if (pBitmap)
            { 
               if (pBitmap->GetLastStatus() == Gdiplus::Ok)
               {
                  ::GlobalUnlock(m_hBuffer);
                  ::GlobalFree(m_hBuffer);
                  return pBitmap;
               }
               delete pBitmap;
               pBitmap = NULL;
            }
         }
         pBitmap = NULL;
         ::GlobalUnlock(m_hBuffer);
      }
      ::GlobalFree(m_hBuffer);
      m_hBuffer = NULL;
   }

   return NULL;
}

// Draws the button bitmap at given coordinates
void CHoverButtonEx::DrawTransparentBitmap(HDC hDC, int iState,         
                                           int xStart,  int yStart,
                                           int wWidth,  int wHeight) 
{
   Gdiplus::Graphics graphics(hDC);

   if (m_pBackground)
      graphics.DrawImage(m_pBackground, 0, 0, m_nWidth, m_nHeight);

   if (iState == 0)
   {
      graphics.DrawImage(m_pImageNormal, 0, 0, m_nWidth, m_nHeight);
   }
   else if (iState == 1)
   {
      graphics.DrawImage(m_pImageMOver, 0, 0, m_nWidth, m_nHeight);
   }
   else if (iState == 2)
   {
      graphics.DrawImage(m_pImagePressed, 0, 0, m_nWidth, m_nHeight);
   }
}

LRESULT CHoverButtonEx::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
   m_bHover = TRUE;

   /* This line corrects a problem with the tooltips not displaying when 
   the mouse passes over them, if the parent window has not been clicked yet.
   Normally this isn't an issue, but when developing multi-windowed apps, this 
   bug would appear. Setting the ActiveWindow to the parent is a solution to that.
   */
   ::SetActiveWindow(GetParent()->GetSafeHwnd());
   Invalidate();
   DeleteToolTip();
   // Create a new Tooltip with new Button Size and Location
   SetToolTipText(m_tooltext);
   if (m_ToolTip != NULL)
   {
      if (::IsWindow(m_ToolTip->m_hWnd))
         //Display ToolTip
         m_ToolTip->Update();
   }
      
   return 0;
}


LRESULT CHoverButtonEx::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;

	m_bHover = FALSE;

	Invalidate();

	return 0;
}

void CHoverButtonEx::OnRButtonDown( UINT nFlags, CPoint point )
{
   if (IsMoveable() == TRUE)
   {
      m_point = point;
      CRect rect;
      GetWindowRect(rect);//Tell the tracker where we are
      ScreenToClient(rect);
      m_track = new CRectTracker(&rect, CRectTracker::dottedLine | 
         CRectTracker::resizeInside |
         CRectTracker::hatchedBorder);
      if (nFlags & MK_CONTROL) // If Ctrl + Right-Click then Resize object
      {
         GetWindowRect(rect);
         GetParent()->ScreenToClient(rect);
         m_track->TrackRubberBand(GetParent(), rect.TopLeft());
         m_track->m_rect.NormalizeRect();
      }
      else // If not Ctrl + Right-Click, then Move Object
      {
         m_track->Track(GetParent(), point);
      }
      rect = LPRECT(m_track->m_rect); 
      MoveWindow(&rect,TRUE);//Move Window to our new position
      m_track = NULL;
      rect = NULL;
   }
   CBitmapButton::OnRButtonDown(nFlags, point);
}

void CHoverButtonEx::DeleteToolTip()
{
	// Destroy Tooltip incase the size of the button has changed.
	if (m_ToolTip != NULL)
	{
		delete m_ToolTip;
		m_ToolTip = NULL;
	}
}

void CHoverButtonEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRect rect;
   GetClientRect(rect);
   if(rect.PtInRect(point))
   {
      bool bParentFound = false;
      CWnd *phwndParent=GetParent();
      if(phwndParent)
      {
         do
         {
            if(DYNAMIC_DOWNCAST(CFrameWnd, phwndParent))
               bParentFound = true;
            else
               phwndParent=phwndParent->GetParent();
         }while(!bParentFound || !phwndParent);
      }
      if(bParentFound)
      {
	     phwndParent->PostMessage(m_WMOnPressButton,0,0);
      }
   }

	CBitmapButton::OnLButtonUp(nFlags, point);
}

void CHoverButtonEx::OnDestroy() 
{
	CBitmapButton::OnDestroy();
	
   if (m_pBackground)
      delete m_pBackground;
   if (m_pImageNormal)
      delete m_pImageNormal;
   if (m_pImageMOver)
      delete m_pImageMOver;
   if (m_pImagePressed)
      delete m_pImagePressed;
}

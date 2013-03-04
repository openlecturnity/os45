// AudioThumbnail.cpp : implementation file
//

#include "stdafx.h"
#include "AudioThumbnail.h"
#include "assistant.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudioThumbnail

CAudioThumbnail::CAudioThumbnail(UINT nOnPressMessage)
{
   m_WMOnPressButton = nOnPressMessage;
   m_ToolTip = NULL;
   m_bTracking = FALSE;
   m_bMonitorElement = FALSE;
}

CAudioThumbnail::~CAudioThumbnail()
{
m_AudioBitmapsArray.RemoveAll();
delete m_ToolTip;
}

BEGIN_MESSAGE_MAP(CAudioThumbnail, CStatic)
	//{{AFX_MSG_MAP(CAudioThumbnail)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
   ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAudioThumbnail::SetBitmaps(
      UINT nBmp0,UINT nBmp1, UINT nBmp2, UINT nBmp3, UINT nBmp4, UINT nBmp5,
      UINT nBmp6, UINT nBmp7, UINT nBmp8, UINT nBmp9, UINT nBmp10,
      UINT nBmp11, UINT nBmp12, UINT nBmp13, UINT nBmp14, UINT nBmp15,
      UINT nBmp16, UINT nBmp17, UINT nBmp18, UINT nBmp19, UINT nBmp20, 
      UINT nBmp21, UINT nBmp22, UINT nBmp23, UINT nBmp24, UINT nBmp25,
      UINT nBmp26, UINT nBmp27, UINT nBmp28, UINT nBmp29, UINT nBmp30,
      UINT nBmp31, UINT nBmp32, UINT nBmp33, UINT nBmp34, UINT nBmp35,
      UINT nBmp36, UINT nBmp37, UINT nBmp38, UINT nBmp39

      )
{   

m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp0)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp1)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp2)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp3)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp4)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp5)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp6)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp7)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp8)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp9)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp10)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp11)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp12)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp13)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp14)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp15)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp16)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp17)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp18)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp19)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp20)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp21)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp22)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp23)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp24)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp25)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp26)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp27)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp28)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp29)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp30)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp31)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp32)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp33)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp34)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp35)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp36)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp37)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp38)));
m_AudioBitmapsArray.Add(::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(nBmp39)));



}
void CAudioThumbnail::SetPosition(float fPosition)
{
//must be in range 0-1;
   
   if((fPosition>=0)&&(fPosition<=1))
   {
      int nPosition = (int)(fPosition*39);
      this->SetBitmap(m_AudioBitmapsArray.GetAt(nPosition));
      Invalidate();
   }
}
/////////////////////////////////////////////////////////////////////////////
// CAudioThumbnail message handlers

BOOL CAudioThumbnail::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

// Set the tooltip with a string resource
void CAudioThumbnail::SetToolTipText(UINT nId, BOOL bActivate)
{
	// load string resource
	m_csToolText.LoadString(nId);
	// If string resource is not empty
	if (m_csToolText.IsEmpty() == FALSE) SetToolTipText(m_csToolText);

}

// Set the tooltip with a CString
void CAudioThumbnail::SetToolTipText(CString spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText.IsEmpty()) return;

	// Initialize ToolTip
	InitToolTip();
	m_csToolText = spText;

	// If there is no tooltip defined then add it
	if (m_ToolTip->GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip->AddTool(this, m_csToolText, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip->UpdateTipText(m_csToolText, this, 1);
   ActivateTooltip(bActivate);
}

void CAudioThumbnail::InitToolTip()
{
	if (m_ToolTip == NULL)
	{
		m_ToolTip = new CToolTipCtrl;
		// Create ToolTip control
		m_ToolTip->Create(this);
      m_ToolTip->SendMessage(TTM_SETMAXTIPWIDTH, 0, 300);
	}
} // End of InitToolTip

// Activate the tooltip
void CAudioThumbnail::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip->GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip->Activate(bActivate);
} // End of EnableTooltip

void CAudioThumbnail::DeleteToolTip()
{
	// Destroy Tooltip incase the size of the button has changed.
	if (m_ToolTip != NULL)
	{
		delete m_ToolTip;
		m_ToolTip = NULL;
	}
}

void CAudioThumbnail::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!m_bMonitorElement)
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
	CStatic::OnLButtonDblClk(nFlags, point);
}

void CAudioThumbnail::SetMonitorGroupElement()
{
	m_bMonitorElement = TRUE;
}

BOOL CAudioThumbnail::PreTranslateMessage(MSG* pMsg) 
{
   // TODO: Add your specialized code here and/or call the base class  
   if (m_ToolTip != NULL)
      if (::IsWindow(m_ToolTip->m_hWnd)) // Incase m_ToolTip isn't NULL, check to see if its a valid window
         m_ToolTip->RelayEvent(pMsg);	
      return CStatic::PreTranslateMessage(pMsg);
}

LRESULT CAudioThumbnail::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	// TODO: Add your message handler code here and/or call default
	/* This line corrects a problem with the tooltips not displaying when 
	the mouse passes over them, if the parent window has not been clicked yet.
	Normally this isn't an issue, but when developing multi-windowed apps, this 
	bug would appear. Setting the ActiveWindow to the parent is a solution to that.
	*/
	::SetActiveWindow(GetParent()->GetSafeHwnd());
//	Invalidate();
	DeleteToolTip();
	// Create a new Tooltip with new Button Size and Location
	SetToolTipText(m_csToolText);
	if (m_ToolTip != NULL)
		if (::IsWindow(m_ToolTip->m_hWnd))
			//Display ToolTip
			m_ToolTip->Update();

	return 0;
}


LRESULT CAudioThumbnail::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
//	Invalidate();
	return 0;
}

void CAudioThumbnail::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CStatic::OnMouseMove(nFlags, point);
}

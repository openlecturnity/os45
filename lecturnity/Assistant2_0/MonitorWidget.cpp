#include "stdafx.h"
#include "MainFrm.h"
#include "MonitorWidget.h"
#include "..\Studio\Resource.h"

BEGIN_MESSAGE_MAP(CMonitorWidget, CAudioWidget)
   ON_WM_CREATE()
   ON_WM_MOUSEMOVE()
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
   ON_WM_GETMINMAXINFO()
   ON_BN_CLICKED(IDB_MONITORWIDGET_CLOSE, OnButtonClose)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMonitorWidget, CWnd)

CMonitorWidget::CMonitorWidget(): CAudioWidget()
{
   m_hBitmapVideo = NULL;
   m_csVideoToolTip = _T("");
   m_nVideoHeight = NAP_HEIGHT + 2*AUDIO_OFFSET + VIDEO_HEIGHT;
   m_nIdMenu = IDR_MONITOR_WIDGET;
   m_bLimitToWindow = false;
   m_bDisableLimitToWindow = false;
}
CMonitorWidget::~CMonitorWidget()
{
   ::DeleteObject(m_hBitmapVideo);
}

int CMonitorWidget::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   m_nHeigh = NAP_HEIGHT + VIDEO_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT;
   m_nHeigh += (INFO_HEIGHT + INFO_OFFSET)*2 + INFO_HEIGHT;

   int nTop = 0;
   //Video
   pStaticVideo.Create(_T(""),WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, ID_VIEW_VIDEO);
   nTop = NAP_HEIGHT + AUDIO_OFFSET;
   pStaticVideo.SetWindowPos(NULL, MARGIN, nTop, AUDIO_WIDTH, VIDEO_HEIGHT, SWP_NOZORDER);

   int nRes = CAudioWidget::OnCreate(lpCreateStruct);

   m_hBitmapVideo = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MONITOR_VIDEO));
   m_bShowDuration = (AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetShowDuration"), 1) == 1);
   m_bShowPages = (AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetShowPages"), 1) == 1);
   m_bShowDiskSpace = (AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetShowDiskSpace"), 1) == 1 );
   m_bEnableDrag = !(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetLockPosition"), 0) == 1 );
   m_bLimitToWindow = (AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetLimitToWindow"), 0) == 1 );

   RecalculateControlsPosition();
   return nRes;
}

void CMonitorWidget::RecalculateControlsPosition()
{
   CRect rcDuration, rcPages, rcDiskSpace, rcVideo;

   pStaticDuration.GetWindowRect(rcDuration);
   pStaticPages.GetWindowRect(rcPages);
   pStaticDiskSpace.GetWindowRect(rcDiskSpace);
   pStaticVideo.GetWindowRect(rcVideo);
   
   ScreenToClient(rcDuration);
   ScreenToClient(rcPages);
   ScreenToClient(rcDiskSpace);
   ScreenToClient(rcVideo);

   //Video
   int nHeight = m_nHeigh;

   if(m_bShowDuration)
      nHeight -= m_nInfoHeight;
   if(m_bShowPages)
   {
      nHeight -= m_nInfoHeight;
      if(m_bShowDuration)
         nHeight -= m_nInfoOffset;
   }
   if(m_bShowDiskSpace)
   {
      nHeight -= m_nInfoHeight;
      if(m_bShowDuration || m_bShowPages)
         nHeight -= m_nInfoOffset;
   }

   m_nAudioVideoOffset = (nHeight*2)/(NAP_HEIGHT + VIDEO_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT);

   int ratio = m_nWidth < nHeight ? m_nWidth : nHeight;

   int width = (ratio * AUDIO_WIDTH) / (WIDGET_WIDTH);
   int left = (m_nWidth-width)/2;
   m_nMargin = left;
   int height = (ratio * VIDEO_HEIGHT) / (NAP_HEIGHT + VIDEO_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT);
   int top = (nHeight-height)/2;

   m_nVideoHeight = height + top;

   pStaticVideo.SetWindowPos(NULL, left, top, width, height, SWP_NOZORDER);

   //Audio
   int heightAudio = (ratio*AUDIO_HEIGHT)/(NAP_HEIGHT + VIDEO_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT);
   m_nAudioHeight = heightAudio + m_nAudioVideoOffset;
   int topAudio =  m_nVideoHeight + m_nAudioVideoOffset;

   pStaticAudio.SetWindowPos(NULL, left, topAudio, width, heightAudio, SWP_NOZORDER);

   //Informations   
   pStaticDuration.ShowWindow(m_bShowDuration ? SW_SHOW : SW_HIDE);
   if(m_bShowDuration)
   {
      int nTopDuration = m_nVideoHeight + m_nAudioHeight;  
      pStaticDuration.SetWindowPos(NULL, m_nMargin, nTopDuration, m_nInfoWidth, m_nInfoHeight, SWP_NOZORDER);
   }

   pStaticPages.ShowWindow(m_bShowPages ? SW_SHOW : SW_HIDE);
   if(m_bShowPages)
   {  
      rcPages.top = m_nVideoHeight + m_nAudioHeight;
      if(m_bShowDuration)
         rcPages.top += m_nInfoHeight + m_nInfoOffset;
      pStaticPages.SetWindowPos(NULL, m_nMargin, rcPages.top, m_nInfoWidth, m_nInfoHeight, SWP_NOZORDER);
   }

   pStaticDiskSpace.ShowWindow(m_bShowDiskSpace ? SW_SHOW : SW_HIDE);
   if(m_bShowDiskSpace)
   {
      rcDiskSpace.top = m_nVideoHeight + m_nAudioHeight;
      if(m_bShowDuration)
         rcDiskSpace.top += m_nInfoHeight + m_nInfoOffset;
      if(m_bShowPages)
         rcDiskSpace.top += m_nInfoHeight + m_nInfoOffset;
      pStaticDiskSpace.SetWindowPos(NULL, m_nMargin, rcDiskSpace.top, m_nInfoWidth, m_nInfoHeight, SWP_NOZORDER);
   }

   m_pButtonClose.SetWindowPos(NULL, m_nWidth-14, 1, 12, 11, SWP_NOZORDER);
}

void CMonitorWidget::DrawAudioVideo(Gdiplus::Graphics & graphics)
{
   CRect rcVideo;
   pStaticVideo.GetWindowRect(rcVideo);
   ScreenToClient(rcVideo);

   if(m_lpBitsInfo && m_lpBits)
   {
      Gdiplus::Bitmap bitmapVideo(m_lpBitsInfo, m_lpBits);
      graphics.DrawImage(&bitmapVideo, rcVideo.left, rcVideo.top, rcVideo.Width(), rcVideo.Height());
   }
   else
   {
      Gdiplus::Bitmap bitmap(m_hBitmapVideo, NULL);
      graphics.DrawImage(&bitmap, rcVideo.left, rcVideo.top, rcVideo.Width(), rcVideo.Height());
   }

   CAudioWidget::DrawAudioVideo(graphics);
}

void CMonitorWidget::DrawInformations(Gdiplus::Graphics & graphics)
{
   CRect rcDuration, rcPages, rcDiskSpace;
   pStaticDuration.GetWindowRect(rcDuration);
   pStaticPages.GetWindowRect(rcPages);
   pStaticDiskSpace.GetWindowRect(rcDiskSpace);
   
   ScreenToClient(rcDuration);
   ScreenToClient(rcPages);
   ScreenToClient(rcDiskSpace);

   Gdiplus::SolidBrush textBrush(Gdiplus::Color(150, 0,0,0));
   Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(150, 255, 255, 255));
   Gdiplus::Font font(_T("Arial"), m_nFontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

   if(m_bShowDuration)
   {
      Gdiplus::PointF textOrigin(rcDuration.left, rcDuration.top);
      Gdiplus::PointF textOriginShadow(rcDuration.left, rcDuration.top+1);

      graphics.DrawString(m_csDuration, m_csDuration.GetLength(), &font, textOriginShadow, &shadowBrush);
      graphics.DrawString(m_csDuration, m_csDuration.GetLength(), &font, textOrigin, &textBrush);
   }

   if(m_bShowPages)
   {
      Gdiplus::PointF textOrigin(rcPages.left, rcPages.top);
      Gdiplus::PointF textOriginShadow(rcPages.left, rcPages.top+1);

      graphics.DrawString(m_csPages, m_csPages.GetLength(), &font, textOriginShadow, &shadowBrush);
      graphics.DrawString(m_csPages, m_csPages.GetLength(), &font, textOrigin, &textBrush);
   }

   if(m_bShowDiskSpace)
   {
      Gdiplus::PointF textOrigin(rcDiskSpace.left, rcDiskSpace.top);
      Gdiplus::PointF textOriginShadow(rcDiskSpace.left, rcDiskSpace.top+1);

      graphics.DrawString(m_csDiskSpace, m_csDiskSpace.GetLength(), &font, textOriginShadow, &shadowBrush);
      graphics.DrawString(m_csDiskSpace, m_csDiskSpace.GetLength(), &font, textOrigin, &textBrush);
   }
}

void CMonitorWidget::SetAudioVideoTooltips(CString csAudioTooltip, CString csVideoTooltip)
{
   m_csVideoToolTip = csVideoTooltip;
   CAudioWidget::SetAudioVideoTooltips(csAudioTooltip, csVideoTooltip);
}

BOOL CMonitorWidget::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
   ASSERT(pNMHDR->code == TTN_NEEDTEXT);
   TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
   if (!(pTTT->uFlags & TTF_IDISHWND))
      return FALSE;

   UINT_PTR hWnd = pNMHDR->idFrom;
   // idFrom is actually the HWND of the tool
   UINT nID = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);
   
   if(ID_VIEW_VIDEO == nID)
   {
      pTTT->lpszText = (LPWSTR)(LPCTSTR) m_csVideoToolTip;
   }
   else
      return CAudioWidget::OnToolTipText(0, pNMHDR, pResult);

   pTTT->hinst = AfxGetInstanceHandle();

   *pResult = 0;
   // bring the tooltip window above other popup windows
   ::SetWindowPos(pNMHDR->hwndFrom,  HWND_TOP, 0, 0, 0, 0,
      SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
   return TRUE;    // message was handled
}

void CMonitorWidget::OnGetMinMaxInfo( MINMAXINFO FAR* pMinMaxInfo )
{
   // Set the Minimum Track Size
   pMinMaxInfo->ptMinTrackSize.x = WIDGET_WIDTH;
   pMinMaxInfo->ptMinTrackSize.y = NAP_HEIGHT + VIDEO_HEIGHT + AUDIO_HEIGHT + 3*AUDIO_OFFSET + GRIPPER_HEIGHT;

   if(m_bShowDuration)
      pMinMaxInfo->ptMinTrackSize.y += INFO_HEIGHT;
   if(m_bShowPages)
   {
      pMinMaxInfo->ptMinTrackSize.y += INFO_HEIGHT;
      if(m_bShowDuration)
         pMinMaxInfo->ptMinTrackSize.y += INFO_OFFSET;
   }
   if(m_bShowDiskSpace)
   {
      pMinMaxInfo->ptMinTrackSize.y += INFO_HEIGHT;
      if(m_bShowDuration || m_bShowPages)
         pMinMaxInfo->ptMinTrackSize.y += INFO_OFFSET;
   }
}
void CMonitorWidget::SaveDurationSettings()
{
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("WidgetShowDuration"), m_bShowDuration);
}
void CMonitorWidget::SavePagesSettings()
{
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("WidgetShowPages"), m_bShowPages);
}
void CMonitorWidget::SaveDiskSpaceSettings()
{
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("WidgetShowDiskSpace"), m_bShowDiskSpace);
}
void CMonitorWidget::SaveLockPositionSettings(BOOL bLockPosition)
{
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("WidgetLockPosition"), bLockPosition);
}
void CMonitorWidget::SaveLimitToWindowSettings()
{
   AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("WidgetLimitToWindow"), m_bLimitToWindow ? TRUE : FALSE);
}

void CMonitorWidget::SavePosition()
{
   if ( m_bSavePosition == false )
      return;
   WINDOWPLACEMENT wp;
   BOOL success = GetWindowPlacement(&wp);
   if (success == TRUE)
      AfxGetApp()->WriteProfileBinary(_T("Assistant Settings"), _T("WidgetPlacement"),
      (LPBYTE) &wp, sizeof WINDOWPLACEMENT);
}

bool CMonitorWidget::UpdateWindowPlacement()
{
   WINDOWPLACEMENT *pWp;
   UINT bytes;
   BOOL success = AfxGetApp()->GetProfileBinary(_T("Assistant Settings"), _T("WidgetPlacement"), 
      (LPBYTE *) &pWp, &bytes);
   if (success == FALSE)
      return false;
   ASSERT(bytes == sizeof WINDOWPLACEMENT);
   SetWindowPlacement(pWp);
   delete[] pWp;
   pWp = NULL;
   return true;
}

void CMonitorWidget::ShowPages(BOOL bShowPages) 
{
   m_bShowPages = bShowPages;
   UpdateMonitorInfo(m_bShowPages, m_bShowDuration, m_bShowDiskSpace);
   SavePagesSettings();
}

void CMonitorWidget::UpdatePages(int nPageNo, int nTotalPages)
{
   m_csPages = _T("0/0");
   m_csPages.Format(_T("%d/%d"), nPageNo, nTotalPages);
   m_csPagesToolTip.Format(IDS_MONITOR_PAGES, nPageNo, nTotalPages);
}
void CMonitorWidget::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rc;
   GetWindowRect(rc);
   ClientToScreen(&point);

   int cx = m_ptStartDrag.x - point.x;
   int cy = m_ptStartDrag.y - point.y;

   int nLeft = rc.left - cx;
   int nTop = rc.top - cy;
   int nRight = rc.right - cx;
   int nBottom = rc.bottom - cy;

   if(m_bLimitToWindow && m_bDisableLimitToWindow == false)
   {
      CRect rcWnd;
      CWnd *pParentRef = GetParent();
      if ( pParentRef != NULL )
      {
         pParentRef->GetWindowRect(rcWnd);

         if(rcWnd.left > nLeft)
            nLeft = rcWnd.left;
         if (rcWnd.top > nTop)
            nTop = rcWnd.top;
         if (rcWnd.right < nRight)
            nLeft = rcWnd.right - rc.Width();
         if (rcWnd.bottom < nBottom)
            nTop = rcWnd.bottom - rc.Height();
      }
   }

   if(nLeft != rc.left || nTop != rc.top)
   {
      if(m_bLeftButtonPressed && m_bEnableDrag)
      {
         SetWindowPos(NULL, nLeft, nTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
         m_ptStartDrag = point;
         SavePosition();
      }
   }
   CWnd::OnMouseMove(nFlags, point);
}
bool CMonitorWidget::IsLimitToWindow()
{
   return m_bLimitToWindow && m_bDisableLimitToWindow == false;
}

void CMonitorWidget::LimitToWindow(bool bLimitToWindow)
{
   m_bLimitToWindow = bLimitToWindow;
   SaveLimitToWindowSettings();
   if(bLimitToWindow && m_bDisableLimitToWindow == false)
   {
      CRect rcMainWnd, rcWnd;
      CWnd *pParentRef = GetParent();
      if ( pParentRef == NULL )
      {
         ASSERT(FALSE);
         return;
      }
      pParentRef->GetWindowRect(rcMainWnd);
      GetWindowRect(rcWnd);

      int nLeft = rcWnd.left;
      int nTop = rcWnd.top;

      if(rcMainWnd.left > rcWnd.left)
         nLeft = rcMainWnd.left;
      if (rcMainWnd.top > rcWnd.top)
         nTop = rcMainWnd.top;
      if (rcMainWnd.right < rcWnd.right)
         nLeft = rcMainWnd.right - rcWnd.Width();
      if (rcMainWnd.bottom < rcWnd.bottom)
         nTop = rcMainWnd.bottom - rcWnd.Height();

      SetWindowPos(NULL, nLeft, nTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);     
   }     
}


void CMonitorWidget::OnButtonClose()
{
   ShowWindow(SW_HIDE);
   SavePosition();

   CMainFrameA *pMainFrameA = CMainFrameA::GetCurrentInstance();

   if (pMainFrameA != NULL)
      pMainFrameA->SetShowAvMonitor(false);
   // OBS: don't save CMonitorWidget visible/hidden value in this regs, as this is saved when app closes.
}
void CMonitorWidget::DisableLimitToWindow(bool bDisable)
{
   m_bDisableLimitToWindow = bDisable;
}
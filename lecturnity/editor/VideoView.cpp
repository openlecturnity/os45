#include "stdafx.h"
#include "VideoView.h"
#include "editorDoc.h"
#include "MainFrm.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVideoView

IMPLEMENT_DYNCREATE(CVideoView, CView)

CVideoView::CVideoView()
{
   // Note: The video zoom factor should perhaps be
   // administrated in the editor project rather
   // than in this CView object. It is similar to
   // the m_nThumbWidth in the CEditorProject class.
   // TODO: Move this member to CEditorProject
   m_dZoomFactor = 1.0;

   m_aClipTimes.SetSize(0, 16);
   m_bIsNearlyOneClip = false;
   m_pBitmap = NULL;
}

CVideoView::~CVideoView()
{
   if (m_pBitmap != NULL)
      delete m_pBitmap;
}


BEGIN_MESSAGE_MAP(CVideoView, CView)
	//{{AFX_MSG_MAP(CVideoView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
   ON_COMMAND(ID_ZOOM_50, OnZoom50)
   ON_COMMAND(ID_ZOOM_100, OnZoom100)
   ON_COMMAND(ID_ZOOM_200, OnZoom200)
   ON_COMMAND(ID_ZOOM_FIT, OnZoomFit)
   ON_UPDATE_COMMAND_UI(ID_ZOOM_50, OnUpdateVideoZoom)
   ON_UPDATE_COMMAND_UI(ID_ZOOM_100, OnUpdateVideoZoom)
   ON_UPDATE_COMMAND_UI(ID_ZOOM_200, OnUpdateVideoZoom)
   ON_UPDATE_COMMAND_UI(ID_ZOOM_FIT, OnUpdateVideoZoom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


HRESULT CVideoView::ClearClipTimes()
{
   m_aClipTimes.RemoveAll();

   return S_OK;
}

HRESULT CVideoView::AddClipTimes(UINT nStartMs, UINT nEndMs)
{
   if (nEndMs <= nStartMs)
      return E_INVALIDARG;

   if (m_aClipTimes.GetSize() > 0 && nStartMs < m_aClipTimes[m_aClipTimes.GetSize() - 1])
      return E_UNEXPECTED;

   m_aClipTimes.Add(nStartMs);
   m_aClipTimes.Add(nEndMs);

   return S_OK;
}

// private
bool CVideoView::IsClipTime(UINT nPositionMs)
{
   for (int i=0; i<m_aClipTimes.GetSize(); i += 2)
   {
      if (nPositionMs >= m_aClipTimes[i] && nPositionMs <= m_aClipTimes[i+1])
         return true;
   }

   if (m_bIsNearlyOneClip) { // m_aClipTimes.GetSize() == 2) -> there might be more than one segment
       // Special case for the clip being as long as the document but not starting at 0 (Bug #5048) 

       if (m_aClipTimes[0] > 0 && nPositionMs <= m_aClipTimes[0])
           return true;
   }

   return false;
}

// private
HRESULT CVideoView::PrepareStillImage()
{
   CEditorDoc* pDoc = (CEditorDoc *)GetDocument();
   
   CString csCurrentStill;
   if (pDoc->project.HasStillImage() && !pDoc->project.HasVideo())
   {
      csCurrentStill = pDoc->project.GetStillImage();

      if (m_csLastStillImage.Compare(csCurrentStill) != 0)
      {
         // different still image: delete old one, create new
         m_csLastStillImage = csCurrentStill;

         if (m_pBitmap != NULL)
            delete m_pBitmap;
         
         LPCTSTR tszFilename = (LPCTSTR)csCurrentStill;
         int iLength = csCurrentStill.GetLength();
         WCHAR wszFilename[MAX_PATH];
#ifndef _UNICODE
         ::MultiByteToWideChar(CP_ACP, 0, csCurrentStill, iLength + 1, wszFilename, iLength + 1);
#else
         wcscpy(wszFilename, csCurrentStill);
#endif
         
         m_pBitmap = Gdiplus::Bitmap::FromFile(wszFilename);
      }
      // else same still image: nothing to do
   }
   else if (m_pBitmap != NULL)
   {
      delete m_pBitmap;
      m_pBitmap = NULL;
   }

   return S_OK;
}  

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CVideoView 

void CVideoView::OnDraw(CDC* pDC)
{
	CEditorDoc* pDoc = (CEditorDoc *)GetDocument();
  	
   COLORREF bgColor = pDoc->GetMainFrame()->GetColor(CMainFrameE::COLOR_BG_DARK);

   CRect rcClient;
   GetClientRect(&rcClient);

   CXTMemDC memDC(pDC, rcClient, bgColor); // make painting double buffered


   bool bHasVideo = pDoc->project.HasVideo();

   if (!bHasVideo)
   {
      bool bHasStill = pDoc->project.HasStillImage();
      if (bHasStill)
         PrepareStillImage();

      bool bHasClips = pDoc->project.HasSgClips();

      UINT nCurrentMs = pDoc->m_curPosMs;

      if (bHasClips && IsClipTime(nCurrentMs))
      {
         // Note: Showing or hiding the m_wndVideoContainer is handled by OnUpdate().
      }
      else if (bHasStill)
      {
         // only paint still image if there is nothing else

         CRect rcWindow;
         GetClientRect(&rcWindow);
         
         if (m_pBitmap != NULL)
         {
            CRect rcImage;
            rcImage.left = 0; rcImage.top = 0;
            rcImage.right = m_pBitmap->GetWidth() - 1;
            rcImage.bottom = m_pBitmap->GetHeight() - 1;

            MfcUtils::FitRectInRect(rcWindow, rcImage);

            Gdiplus::Graphics graphics(memDC.m_hDC);
            graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
         
            graphics.DrawImage(m_pBitmap, rcImage.left, rcImage.top, rcImage.Width() + 1, rcImage.Height() + 1);
         }
      }
   }
   // else the video and thus the video window is always visible
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CVideoView

#ifdef _DEBUG
void CVideoView::AssertValid() const
{
	CView::AssertValid();
}

void CVideoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CVideoView 

void CVideoView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();

   switch (lHint)
   {
   
   case UPDATE_FITVIDEO:
      FitVideo();
      RedrawWindow();
      break;

   default:
      bool bPreviewVisible = m_wndVideoContainer.IsWindowVisible() == TRUE;
      bool bShouldBeVisible = pDoc->project.HasVideo() || IsClipTime(pDoc->m_curPosMs);
      bool bRedrawn = false;
      if (bShouldBeVisible != bPreviewVisible)
      {
         m_wndVideoContainer.ShowWindow(bShouldBeVisible ? SW_SHOW : SW_HIDE);
         RedrawWindow(NULL, NULL, RDW_NOCHILDREN | RDW_INVALIDATE);
         bRedrawn = true;
      }
 
      if (!pDoc->IsPreviewActive() && !bRedrawn)
      {
         RedrawWindow(NULL, NULL, RDW_NOCHILDREN);
      }
      break;
   }
}

void CVideoView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
   FitVideo(cx, cy);
}

void CVideoView::FitVideo()
{
   CRect rect;
   GetClientRect(&rect);
   FitVideo(rect.right - rect.left, rect.bottom - rect.top);
}

void CVideoView::SetZoom(double zoom)
{
   m_dZoomFactor = zoom;
   FitVideo();
}

void CVideoView::FitVideo(int cx, int cy)
{
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
   if (pDoc)
   {
      CSize size;
      HRESULT hr = pDoc->GetPreviewVideoSize(&size);
      if (hr == S_OK) // S_FALSE means no content/video preview (yet)
      {
         // also considers the user given zoom factor

         CSize newSize(size);
         newSize.cx = (int) (m_dZoomFactor * newSize.cx);
         newSize.cy = (int) (m_dZoomFactor * newSize.cy);

         double xf = ((double) cx) / ((double) newSize.cx);
         double yf = ((double) cy) / ((double) newSize.cy);

         if (xf < 1.0 || yf < 1.0)
         {
            double minf = xf < yf ? xf : yf;
            newSize.cx = (int) (minf * ((double) newSize.cx) + .5);
            newSize.cy = (int) (minf * ((double) newSize.cy) + .5);
         }

         int leftOff = (cx - newSize.cx) / 2;
         int topOff  = (cy - newSize.cy) / 2;
         m_wndVideoContainer.SetWindowPos(NULL, leftOff, topOff, newSize.cx, newSize.cy, SWP_NOZORDER);

         bool bShouldBeVisible = pDoc->project.HasVideo() || IsClipTime(pDoc->m_curPosMs);
         if (bShouldBeVisible)
            m_wndVideoContainer.ShowWindow(SW_SHOW);

         // TODO m_rcVideoRect is totally unused/undefined. Why is it used and for what?
         //      However this parameter is ignored in AVEdit::FitVideoWindow().
         hr = pDoc->ResizePreviewVideoWindow(&m_rcVideoRect);
      }
      else
      {
         m_wndVideoContainer.ShowWindow(SW_HIDE);
      }
   }
}

BOOL CVideoView::OnEraseBkgnd(CDC* pDC) 
{
   return FALSE;
}

void CVideoView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
   CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
   if (!pDoc)
      return;

	pDoc->BuildPreview(GetSafeVideoHwnd());
}

HWND CVideoView::GetSafeVideoHwnd()
{
   if (this == NULL)
      return NULL;
   return m_wndVideoContainer.GetSafeHwnd();
}

int CVideoView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
   m_wndVideoContainer.Create(NULL, WS_CHILD, CRect(0, 0, 100, 100), this);
   m_ZoomPopup.LoadMenu(IDR_VIDEOZOOM);
	
	return 0;
}

void CVideoView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	UINT nIDCheck = ID_ZOOM_50;
   if (m_dZoomFactor > 0.75) nIDCheck = ID_ZOOM_100;
   if (m_dZoomFactor > 1.5) nIDCheck = ID_ZOOM_200;
   if (m_dZoomFactor > 3) nIDCheck = ID_ZOOM_FIT;
   m_ZoomPopup.CheckMenuRadioItem(ID_ZOOM_50, ID_ZOOM_FIT, nIDCheck, MF_BYCOMMAND);

   CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
   BOOL enable = pDoc->project.IsEmpty() ? TRUE : FALSE;
   for (UINT id=ID_ZOOM_50; id<=ID_ZOOM_FIT; ++id)
      m_ZoomPopup.EnableMenuItem(id, enable);
	
   CMenu *pPopup = m_ZoomPopup.GetSubMenu(0);
   if (pPopup)
      pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CVideoView::OnZoom50()
{
   SetZoom(0.5);
}

void CVideoView::OnZoom100()
{
   SetZoom(1.0);
}

void CVideoView::OnZoom200()
{
   SetZoom(2.0);
}

void CVideoView::OnZoomFit()
{
   SetZoom(100.0); // an arbitrary but big number
}

void CVideoView::OnUpdateVideoZoom(CCmdUI *pCmdUI)
{
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
   bool bEnable = false;

   if (pDoc && !pDoc->project.IsEmpty())
      bEnable = true;

   double zoom = GetZoom();
   int check = 0;
   switch (pCmdUI->m_nID)
   {
   case ID_ZOOM_50:
      if (zoom <= 0.75) check = 1; break;
   case ID_ZOOM_100:
      if (zoom > 0.75 && zoom <= 1.5) check = 1; break;
   case ID_ZOOM_200:
      if (zoom > 1.5 && zoom < 3.0) check = 1; break;
   case ID_ZOOM_FIT:
      if (zoom > 3.0) check = 1; break;
   }

   pCmdUI->SetRadio(check);
   pCmdUI->Enable(bEnable);
}

#include "global.h"

#include "LanguageSupport.h"

#pragma warning ( disable : 4786 )

using namespace std;

CaptureRectChooser::CaptureRectChooser()
{
   if (instance != NULL)
      throw exception("CaptureRectChooser is already instanciated");
   instance = this;
   hChooseEvent_ = NULL;
   wndClassRegistered_ = registerWndClassEx();
   
   m_iDesktopWidth = 0;
   m_iDesktopHeight = 0;
   m_pBmpSelectionWindowBackground = NULL;
   m_pBmpSelectionRectangleBackground = NULL;
   
   m_bWindowSelectionHasBeenDisplayed = false;
   m_bRectangleSelectionHasBeenDisplayed = false;
}

bool CaptureRectChooser::registerWndClassEx()
{
   HINSTANCE hInstance = g_hDllInstance;

   // Register a class for the transparent window:
   wndClassEx.cbSize      = sizeof WNDCLASSEX;
   wndClassEx.style       = 0; // no special style required
   wndClassEx.lpfnWndProc = WindowProc;
   wndClassEx.cbClsExtra  = 0;
   wndClassEx.cbWndExtra  = 0;
   wndClassEx.hInstance   = hInstance;
   wndClassEx.hIcon       = NULL; // LoadIcon(NULL, IDI_APPLICATION);
   HCURSOR hCursor = LoadCursor(NULL, IDC_CROSS);
   wndClassEx.hCursor     = hCursor; // LoadCursor(NULL, IDC_CROSS);
   wndClassEx.hbrBackground = NULL; //GetStockObject(BLACK_BRUSH);
   wndClassEx.lpszMenuName  = NULL;
   wndClassEx.lpszClassName = CAPTUREWNDCLASS;
   wndClassEx.hIconSm       = NULL;

   ATOM atom = RegisterClassEx(&wndClassEx);
   return (atom != NULL);
}

CaptureRectChooser::~CaptureRectChooser()
{
   if (m_pBmpSelectionWindowBackground)
      delete m_pBmpSelectionWindowBackground;
   m_pBmpSelectionWindowBackground = NULL;

   if (m_pBmpSelectionRectangleBackground)
      delete m_pBmpSelectionRectangleBackground;
   m_pBmpSelectionRectangleBackground = NULL;

   UnregisterClass(CAPTUREWNDCLASS, g_hDllInstance);
   instance = NULL;
}

bool CaptureRectChooser::chooseCaptureRect(RECT *pRect)
{
   // initalize some variables:
   hwndList_.clear();
   isInDragMode_          = false;
   isMousePressed_        = false;
   lastHWnd_              = NULL;
   isLastWndMaximized_    = false;
   isCurrentWndMaximized_ = false;
   m_bWindowSelectionHasBeenDisplayed = false;
   m_bRectangleSelectionHasBeenDisplayed = false;
   isActivated_           = false;
   bWasCancelled_         = false;

   // Create an event so that we know when the user is ready:
   hChooseEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);
   DWORD dwThreadId = _beginthread(windowThread, 0, this);
   HRESULT hRes = WaitForSingleObject(hChooseEvent_, INFINITE);
   CloseHandle(hChooseEvent_);

   *pRect = selectedRegion_;
#ifdef _DEBUG
   cout << "Selected region: (" << (*pRect).left << ", " << (*pRect).top << ") to ";
   cout << "(" << (*pRect).right << ", " << (*pRect).bottom << ")" << endl;
#endif

   return !bWasCancelled_;
}

void __cdecl CaptureRectChooser::windowThread(void *lpData)
{
   CLanguageSupport::SetThreadLanguage();

   CaptureRectChooser *pCrc = (CaptureRectChooser *) lpData;

   // retrieve the screen resolution
   HDC hdc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
   pCrc->m_iDesktopWidth = GetDeviceCaps(hdc, HORZRES);
   pCrc->m_iDesktopHeight = GetDeviceCaps(hdc, VERTRES);
   DeleteDC(hdc);

   pCrc->hWnd_ = CreateWindowEx
      (WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, // dwExStyle
       CAPTUREWNDCLASS, // lpClassName
       CAPTUREWNDCLASS, // lpWindowName
       WS_POPUP | WS_VISIBLE, // dwStyle
       0, 0, pCrc->m_iDesktopWidth, pCrc->m_iDesktopHeight,
       NULL, // hWndParent
       NULL, // hMenu
       g_hDllInstance, // hInstance
       pCrc); // lpParam

   if (pCrc->hWnd_)
   {
      ShowWindow(pCrc->hWnd_, SW_SHOW);
      // SetWindowPos(hWnd_, HWND_TOPMOST, 0, 0, width, height, 0);
   }
   else
   {
#ifdef _DEBUG
      cout << "+-+-+-+-+-+ HWND returned by CreateWindowEx is NULL +-+-+-+-+-+" << endl;
#endif
   }

#ifdef _DEBUG
   cout << "Entering CaptureRectChooser window loop..." << endl;
#endif

   MSG msg;
   while (GetMessage(&msg, pCrc->hWnd_, 0, 0) > 0)
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   DestroyWindow(pCrc->hWnd_);

   SetEvent(pCrc->hChooseEvent_);

#ifdef _DEBUG
   cout << "Exiting CaptureRectChooser window loop..." << endl;
#endif
}

LRESULT CALLBACK CaptureRectChooser::WindowProc(HWND hWnd, UINT uMsg, 
                                                WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
   case WM_ACTIVATE:
      instance->hwndList_.clear();
      break;

   case WM_QUIT:
      PostQuitMessage(0);
      break;

   case WM_LBUTTONDOWN:
      instance->onLMouseDown(wParam, lParam);
      break;

   case WM_LBUTTONUP:
      instance->onLMouseUp(wParam, lParam);
      break;

   case WM_RBUTTONDOWN:
      instance->onRMouseDown(wParam, lParam);
      break;

   case WM_PAINT:
      instance->isActivated_ = true;
      break;

   case WM_MOUSEMOVE:
      instance->onMouseMove(wParam, lParam);
      return 0;

   case WM_SETCURSOR:
      instance->onSetCursor();
      return TRUE;

   case WM_CHAR:
      {
         TCHAR chCharCode = (TCHAR) wParam;
         switch (chCharCode)
         {
         case 0x08: // backspace
         case 0x1b: // escape
            instance->bWasCancelled_ = true;
            DestroyWindow(hWnd);
            break;
         }

      }
      break;
   }

   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void CaptureRectChooser::DrawWindowSelection(RECT &rect, bool bErase)
{
   int iPenWidth = 4;

   HDC hdc = ::GetDC(hWnd_);
   Gdiplus::Graphics graphics(hdc);

   if (bErase)
   { 
      // Copy background bitmap back to window
      if (m_pBmpSelectionWindowBackground)
      {
         Gdiplus::Rect rcBmpClip(rect.left, rect.top, (rect.right-rect.left), (rect.bottom-rect.top));
         Gdiplus::Region region(rcBmpClip);
         if (rcBmpClip.Width > 2*iPenWidth && rcBmpClip.Height > 2*iPenWidth)
         {
            Gdiplus::Rect rcBmpInnerClip(rcBmpClip.X + iPenWidth, rcBmpClip.Y + iPenWidth, 
                                         rcBmpClip.Width - 2*iPenWidth, rcBmpClip.Height - 2*iPenWidth);
            region.Exclude(rcBmpInnerClip);
         }

         graphics.SetClip(&region);

         graphics.DrawImage(m_pBmpSelectionWindowBackground, 
                            (Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top,
                            (Gdiplus::REAL)(rect.right-rect.left), (Gdiplus::REAL)(rect.bottom-rect.top));
      }
   }
   else
   {
      // Save background to erase rectangle

      // Delete old background
      if (m_pBmpSelectionWindowBackground != NULL)
         delete m_pBmpSelectionWindowBackground;
      m_pBmpSelectionWindowBackground = NULL;

      // Save new background
      m_pBmpSelectionWindowBackground = new Gdiplus::Bitmap((rect.right-rect.left), (rect.bottom-rect.top));
      Gdiplus::Graphics *pBmpGraphics = Gdiplus::Graphics::FromImage(m_pBmpSelectionWindowBackground);

      HDC bgHDC = ::GetDC(NULL);
      HDC bmpHDC = pBmpGraphics->GetHDC();
      
      ::BitBlt(bmpHDC, 0, 0, 
         (rect.right-rect.left), (rect.bottom-rect.top), bgHDC, 
         rect.left, rect.top, SRCCOPY);

      pBmpGraphics->ReleaseHDC(bmpHDC);
      ::DeleteDC(bgHDC);
      delete pBmpGraphics;

      // Draw Rectangle
      graphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
      
      Gdiplus::Color color;
      color.SetValue(Gdiplus::Color::MakeARGB(255, 0, 255, 0));

      Gdiplus::Pen pen(color, iPenWidth);
      pen.SetAlignment(Gdiplus::PenAlignmentInset);
      Gdiplus::RectF gdipRect((Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, 
         (Gdiplus::REAL)(rect.right-rect.left), (Gdiplus::REAL)(rect.bottom-rect.top)); 
      graphics.DrawRectangle(&pen, gdipRect);
   }

   ::ReleaseDC(hWnd_, hdc);

   return;
}

void CaptureRectChooser::DrawSelectionRectangle(RECT &rect, bool bErase)
{
   int iPenWidth = 2;

   HDC hdc = ::GetDC(hWnd_);
   Gdiplus::Graphics graphics(hdc);

   if (bErase)
   { 
      // Copy background bitmap back to window
      if (m_pBmpSelectionRectangleBackground)
      {
         Gdiplus::Rect rcBmpClip(rect.left, rect.top, (rect.right-rect.left), (rect.bottom-rect.top));
         Gdiplus::Region region(rcBmpClip);
         if (rcBmpClip.Width > 2*iPenWidth && rcBmpClip.Height > 2*iPenWidth)
         {
            Gdiplus::Rect rcBmpInnerClip(rcBmpClip.X + iPenWidth, rcBmpClip.Y + iPenWidth, 
                                         rcBmpClip.Width - 2*iPenWidth, rcBmpClip.Height - 2*iPenWidth);
            region.Exclude(rcBmpInnerClip);
         }

         graphics.SetClip(&region);

         graphics.DrawImage(m_pBmpSelectionRectangleBackground, 
                            rect.left, rect.top,
                            rect.left, rect.top,
                            (rect.right-rect.left), (rect.bottom-rect.top), Gdiplus::UnitPixel );
      }
   }
   else
   {
      // Save background to erase rectangle
      if (m_pBmpSelectionRectangleBackground == NULL)
      {
         m_pBmpSelectionRectangleBackground = new Gdiplus::Bitmap(m_iDesktopWidth, m_iDesktopHeight);
         Gdiplus::Graphics *pBmpGraphics = Gdiplus::Graphics::FromImage(m_pBmpSelectionRectangleBackground);

         // Get Background
         HDC bgHDC = ::GetDC(NULL);
         HDC bmpHDC = pBmpGraphics->GetHDC();
         
         ::BitBlt(bmpHDC, 0, 0, 
            m_iDesktopWidth, m_iDesktopHeight, bgHDC, 
            0, 0, SRCCOPY);

         pBmpGraphics->ReleaseHDC(bmpHDC);
         ::DeleteDC(bgHDC);
         delete pBmpGraphics;
      }

      // Draw Rectangle
      graphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
      
      Gdiplus::Color color;
      color.SetValue(Gdiplus::Color::MakeARGB(255, 255, 128, 0));

      Gdiplus::Pen pen(color, iPenWidth);
      pen.SetAlignment(Gdiplus::PenAlignmentInset);
      Gdiplus::RectF gdipRect((Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, 
         (Gdiplus::REAL)(rect.right-rect.left), (Gdiplus::REAL)(rect.bottom-rect.top)); 
      graphics.DrawRectangle(&pen, gdipRect);
   }

   ::ReleaseDC(hWnd_, hdc);

}

void CaptureRectChooser::onMouseMove(WPARAM fwKeys, LPARAM lMousePos)
{
   POINTS sPoint = MAKEPOINTS(lMousePos);
   POINT point;
   point.x = sPoint.x;
   point.y = sPoint.y;

   if (isActivated_)
   {
      if (isMousePressed_)
      {
         int nMinDragX = ::GetSystemMetrics(SM_CXDRAG);
         int nMinDragY = ::GetSystemMetrics(SM_CYDRAG);
         int nDraggedX = abs(point.x - startPoint_.x);
         int nDraggedY = abs(point.y - startPoint_.y);

         isInDragMode_ = (nDraggedX > nMinDragX) || (nDraggedY > nMinDragY);

         if (isInDragMode_)
         {
            if (m_bWindowSelectionHasBeenDisplayed)
            {
               DrawWindowSelection(m_rcMark, true);
               if (m_pBmpSelectionWindowBackground)
                  delete m_pBmpSelectionWindowBackground;
               m_pBmpSelectionWindowBackground = NULL;
               m_bWindowSelectionHasBeenDisplayed = false;
            }

            if (m_bRectangleSelectionHasBeenDisplayed)
            {
               DrawSelectionRectangle(m_rcMark, true);
               m_bRectangleSelectionHasBeenDisplayed = false;
            }

            if (point.x <= startPoint_.x)
            {
               m_rcMark.left = point.x;
               m_rcMark.right = startPoint_.x;
            }
            else
            {
               m_rcMark.left = startPoint_.x;
               m_rcMark.right = point.x;
            }
            if (point.y <= startPoint_.y)
            {
               m_rcMark.top = point.y;
               m_rcMark.bottom = startPoint_.y;
            }
            else
            {
               m_rcMark.top = startPoint_.y;
               m_rcMark.bottom = point.y;
            }

            DrawSelectionRectangle(m_rcMark, false);

            m_bRectangleSelectionHasBeenDisplayed = true;
         }
      }
      else
      {
         // This is the Window select mode
         HWND hHoverWnd = MyWindowFromPoint(point);

         if (hHoverWnd != lastHWnd_ || !m_bWindowSelectionHasBeenDisplayed)
         {
            if (m_bRectangleSelectionHasBeenDisplayed)
            {
               DrawSelectionRectangle(m_rcMark, true);
               if (m_pBmpSelectionRectangleBackground)
                  delete m_pBmpSelectionRectangleBackground;
               m_pBmpSelectionRectangleBackground = NULL;
               m_bRectangleSelectionHasBeenDisplayed = false;
            }

            if (m_bWindowSelectionHasBeenDisplayed)
            {
               DrawWindowSelection(m_rcMark, true);
               m_bWindowSelectionHasBeenDisplayed = false;
            }

            if (hHoverWnd)
            {
               // real window, retrieve bounds
               ::GetWindowRect(hHoverWnd, &m_rcMark);
            }
            else
            {
               // Full desktop
               m_rcMark.left   = 0;
               m_rcMark.top    = 0;
               m_rcMark.right  = ::GetSystemMetrics(SM_CXSCREEN);
               m_rcMark.bottom = ::GetSystemMetrics(SM_CYSCREEN);
            }

            DrawWindowSelection(m_rcMark, false);
            m_bWindowSelectionHasBeenDisplayed = true;
            
            lastHWnd_ = hHoverWnd;
         }
      }
   }
}

void CaptureRectChooser::onLMouseDown(WPARAM fwKeys, LPARAM lMousePos)
{
   isMousePressed_ = true;
   POINTS points = MAKEPOINTS(lMousePos);

   startPoint_.x = points.x;
   startPoint_.y = points.y;
}

void CaptureRectChooser::onLMouseUp(WPARAM fwKeys, LPARAM lMousePos)
{
   if (isMousePressed_)
   {
      // We're done, what has the user selected?
      if (isInDragMode_)
      {
         // he has selected a custom area
         selectedRegion_ = m_rcMark;
      }
      else
      {
         // the selected region is a window region
         selectedRegion_ = m_rcMark;

         ::BringWindowToTop(lastHWnd_);
      }

      DestroyWindow(hWnd_);
   }

   isMousePressed_ = false;
   isInDragMode_   = false;
}

void CaptureRectChooser::onRMouseDown(WPARAM fwKeys, LPARAM lMousePos)
{
   // this works like "cancel"
   isMousePressed_ = false;
   isInDragMode_   = false;
}

void CaptureRectChooser::onSetCursor()
{
   if (isInDragMode_)
      SetCursor(LoadCursor(NULL, IDC_CROSS));
   else
      SetCursor(LoadCursor(NULL, IDC_ARROW));
}


bool IsNotMinimized(int nCmdShow)
{
   switch (nCmdShow)
   {
   case SW_SHOW:
   case SW_SHOWNORMAL:
   case SW_SHOWMAXIMIZED:
   case SW_SHOWDEFAULT:
   case SW_MAX:
      return true;

   default:
      return false;
   }
}

bool IsMaximized(int nCmdShow)
{
   switch (nCmdShow)
   {
   case SW_SHOWMAXIMIZED:
      return true;

   default:
      return false;
   }
}

bool isPointInRect(RECT &rect, POINT &pt)
{
   return (pt.x >= rect.left && pt.x < rect.right) &&
          (pt.y >= rect.top  && pt.y < rect.bottom);
}

void CaptureRectChooser::fillHwndList()
{
   hwndList_.clear();

   HWND hWnd = ::GetTopWindow(NULL);
   while (hWnd != NULL)
   {
      HWND owner = ::GetWindow(hWnd, GW_OWNER);

      if (owner == NULL)
      {
         WINDOWPLACEMENT placement;
         placement.length = sizeof(WINDOWPLACEMENT);
         if (::GetWindowPlacement(hWnd, &placement) == TRUE)
         {
            LONG style = ::GetWindowLong(hWnd, GWL_STYLE);
            if (((style & WS_CAPTION) > 0) &&
                ((style & WS_VISIBLE) > 0) &&
                IsNotMinimized(placement.showCmd) &&
                ::IsWindowVisible(hWnd))
            {
               HWNDPOSITION hwndPos;
               hwndPos.hWnd = hWnd;
               ::GetWindowRect(hWnd, &hwndPos.rect);
               hwndPos.showCmd = placement.showCmd;
               hwndList_.push_back(hwndPos);
            }
         }
      }

      hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
   }
}

HWND CaptureRectChooser::MyWindowFromPoint(POINT &point)
{
   //hwndList_.clear();
   if (hwndList_.empty())
      fillHwndList();

   vector<HWNDPOSITION>::iterator iter = hwndList_.begin();
   bool isFound = false;
   HWND hWnd = NULL;
   while (iter != hwndList_.end() && !isFound)
   {
      if (isPointInRect((*iter).rect, point) && (*iter).hWnd != hWnd_)
      {
         hWnd = (*iter).hWnd;
         isCurrentWndMaximized_ = IsMaximized((*iter).showCmd);
         isFound = true;
      }
      ++iter;
   }

   if (hWnd == NULL)
   {
      // full screen
      isCurrentWndMaximized_ = false;
   }

   return hWnd;
}


CaptureRectChooser* CaptureRectChooser::instance = NULL;
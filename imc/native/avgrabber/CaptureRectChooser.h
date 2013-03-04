#ifndef __CAPTURERECTCHOOSER
#define __CAPTURERECTCHOOSER

#define CAPTUREWNDCLASS _T("CaptureRectChooser")

#include <gdiplus.h>

typedef struct
{
   HWND hWnd;
   RECT rect;
   int showCmd;
} HWNDPOSITION;

class CaptureRectChooser
{
private:
   HWND hWnd_;
   WNDCLASSEX wndClassEx;
   bool wndClassRegistered_;
   HANDLE hChooseEvent_;
   bool bWasCancelled_;

   POINT startPoint_;

   bool  isMousePressed_;
   bool  isInDragMode_;
   // needed to find out if the mouse is moved into a new window.
   HWND  lastHWnd_;
   bool  isLastWndMaximized_;
   bool  isCurrentWndMaximized_;
   RECT  m_rcMark;
   bool  m_bWindowSelectionHasBeenDisplayed;
   bool  m_bRectangleSelectionHasBeenDisplayed;

   int m_iDesktopWidth;
   int m_iDesktopHeight;

   bool  isActivated_;
   std::vector<HWNDPOSITION> hwndList_;

   RECT selectedRegion_;

   Gdiplus::Bitmap *m_pBmpSelectionWindowBackground;
   Gdiplus::Bitmap *m_pBmpSelectionRectangleBackground;

   bool registerWndClassEx();

   void fillHwndList();
   HWND MyWindowFromPoint(POINT &point);

   void DrawWindowSelection(RECT &rect, bool bErase);
   void DrawSelectionRectangle(RECT &rect, bool bErase);
   void onMouseMove(WPARAM fwKeys, LPARAM lMousePos);
   void onLMouseDown(WPARAM fwKeys, LPARAM lMousePos);
   void onLMouseUp(WPARAM fwKeys, LPARAM lMousePos);
   void onRMouseDown(WPARAM fwKeys, LPARAM lMousePos);
   void onSetCursor();
   
   static void __cdecl windowThread(void *lpData);
   static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, 
                                      WPARAM wParam, LPARAM lParam);

   static CaptureRectChooser *instance;
public:
   CaptureRectChooser();
   ~CaptureRectChooser();

   bool chooseCaptureRect(RECT *pRect);
};

#endif

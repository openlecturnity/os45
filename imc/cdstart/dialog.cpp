#include "StdAfx.h"

#include "resource.h"
#include "errormessages.h"
#include "dialog.h"

AUTORUN::DialogWindow::DialogWindow(HINSTANCE hInstance)
{
   hInstance_ = hInstance;
  
  
}

bool AUTORUN::DialogWindow::Init()
{ 
   if (!InitInstance())
      return false;

   hbm_ = LoadBitmap(hInstance_,MAKEINTRESOURCE(IDB_BITMAP1));

   hFont_ = CreateFont(-11, 5, 0, 0, 
      FW_MEDIUM, FALSE, FALSE, FALSE,
      ANSI_CHARSET, OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
      DEFAULT_PITCH|FF_DONTCARE, "Arial");

   bitmapRect.left = 2;
   bitmapRect.top = 2;
   bitmapRect.right = 322;
   bitmapRect.bottom = 322;
  
   separatorRect.left = 2;
   separatorRect.top = 323;
   separatorRect.right = 322;
   separatorRect.bottom = 325;
  
   buttonRect.left = 2;
   buttonRect.top = 328;
   buttonRect.right = 322;
   buttonRect.bottom = 378;
   
   textRect.left = 15;
   textRect.top = 325;
   textRect.right = 320;
   textRect.bottom = 338;

   hdc_ = GetWindowDC(mainWindow_);

   return true;

}

BOOL AUTORUN::DialogWindow::InitInstance() 
{ 
   HWND  
      deskHwnd;
   RECT
      deskRect;
   int
      xPos,
      yPos;

   deskHwnd = GetDesktopWindow();
   GetWindowRect(deskHwnd,&deskRect);
 
   xPos = (deskRect.right - deskRect.left - 325) / 2;
   yPos = (deskRect.bottom - deskRect.top - 380) / 2;

   mainWindow_ = ::CreateWindowEx(0,  "LecturnityClass", "Lecturnity",
      WS_POPUP | WS_DLGFRAME | WS_VISIBLE, xPos, yPos,
      325,380, 
      HWND_DESKTOP, NULL, hInstance_, NULL);  

   if (!mainWindow_)
   {
      MESSAGES::ErrorMessages::ShowMessage(NULL, IDS_ERR_NOWINDOW, MB_ICONERROR);
      return false;
   }

   hButton_ = CreateWindowEx( 0, "BUTTON", "Abbrechen", 
      BS_PUSHBUTTON | BS_ICON | WS_CHILD | WS_VISIBLE, 115, 338,
      100,30, 
      mainWindow_, NULL, hInstance_, NULL);  

   if (!hButton_)
   {
      MESSAGES::ErrorMessages::ShowMessage(NULL, IDS_ERR_NOWINDOW, MB_ICONERROR);
      return false;
   }

   SendMessage(hButton_,BM_SETIMAGE,IMAGE_ICON,
      (LPARAM)(HANDLE)LoadImage(hInstance_,MAKEINTRESOURCE(IDI_CANCEL),
      IMAGE_ICON,85,18,LR_DEFAULTCOLOR));
   /*
   if (MESSAGES::ErrorMessages::systemLanguage == MESSAGES::GERMAN)
   {
      SendMessage(hButton_,BM_SETIMAGE,IMAGE_ICON,
         (LPARAM)(HANDLE)LoadImage(hInstance_,MAKEINTRESOURCE(IDI_ICON2),
         IMAGE_ICON,85,18,LR_DEFAULTCOLOR));
   }
   else
   {
      SendMessage(hButton_,BM_SETIMAGE,IMAGE_ICON,
         (LPARAM)(HANDLE)LoadImage(hInstance_,MAKEINTRESOURCE(IDI_ICON4),
         IMAGE_ICON,85,18,LR_DEFAULTCOLOR));
   }
   */
   
   ShowWindow(mainWindow_, SW_SHOWNORMAL); 

   return true;  
} 

void AUTORUN::DialogWindow::Destroy()
{

  DeleteObject(hFont_);
  DeleteObject(hbm_);
  ReleaseDC(mainWindow_,hdc_);

  PostQuitMessage(0);
}

void AUTORUN::DialogWindow::DestroyButton()
{
   RECT
      windowRect;

   if (hButton_)
   {
      DestroyWindow(hButton_);
   
      GetWindowRect(mainWindow_,&windowRect);
      SetWindowPos(mainWindow_, HWND_NOTOPMOST, windowRect.left,windowRect.top,340,338,SWP_NOZORDER|SWP_NOMOVE);

      hButton_ = NULL;
   }
}

void AUTORUN::DialogWindow::HideButton()
{
   RECT
      windowRect;

   if (hButton_)
   {
      GetWindowRect(mainWindow_,&windowRect);
      SetWindowPos(mainWindow_, HWND_NOTOPMOST, windowRect.left,windowRect.top,340,338,SWP_NOZORDER|SWP_NOMOVE);
   }
}

void AUTORUN::DialogWindow::PaintBackground()
{
   HBRUSH 
      hbr,
      hbrOld;

/*
   hbr = CreateSolidBrush(0x00ffffff);
   hbrOld = (HBRUSH)SelectObject(hdc_,hbr);
   FillRect(hdc_, &bitmapRect, hbr);
   SelectObject(hdc_,hbrOld);
   DeleteObject(hbr);
*/

   if (hButton_ != NULL)
   {
      hbr = CreatePatternBrush(LoadBitmap(hInstance_,MAKEINTRESOURCE(IDB_BITMAP3)));
      hbrOld = (HBRUSH)SelectObject(hdc_,hbr);
      FillRect(hdc_, &separatorRect, hbr);
      SelectObject(hdc_,hbrOld);
      DeleteObject(hbr);
   
      hbr = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
      hbrOld = (HBRUSH)SelectObject(hdc_,hbr);
      FillRect(hdc_, &buttonRect, hbr);
      SelectObject(hdc_,hbrOld);
      DeleteObject(hbr);
   }
}

void AUTORUN::DialogWindow::PaintBitmap()
{
   HDC 
      hdcMem;
   BITMAP 
      bm; 

   hdcMem = CreateCompatibleDC(NULL);
   SelectObject(hdcMem,hbm_);

   GetObject(hbm_,sizeof(bm),&bm);
   BitBlt(hdc_,bitmapRect.left,bitmapRect.top,bm.bmWidth,bm.bmHeight,hdcMem,0,0,SRCCOPY);

   DeleteDC(hdcMem);
}


void AUTORUN::DialogWindow::DeleteText()
{  
   HBRUSH 
      hbr,
      hbrOld;
   
   hbr = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
   hbrOld = (HBRUSH)SelectObject(hdc_,hbr);

   FillRect(hdc_, &textRect, hbr);

   SelectObject(hdc_,hbrOld);
   DeleteObject(hbr);

 //  DeleteObject(hbr);
}

void AUTORUN::DialogWindow::PaintText(char *text)
{
   SelectObject(hdc_,hFont_);
   //SetBkMode(hdc_,0x00ffffff);
   //SetTextColor(hdc_,0x00909090);
   SetBkMode(hdc_, TRANSPARENT);
   SetTextColor(hdc_, RGB(0x50, 0x50, 0x50));

   DrawText(hdc_,text,strlen(text),&textRect,DT_CENTER | DT_NOCLIP);
   

}

void AUTORUN::DialogWindow::RePaint(char *text)
{
   static std::string
      lastText;
   static int
      i = 0;

   if (text) 
   {
      DeleteText();
      PaintText(text);
      lastText = text;
   }
   else if (!lastText.empty()) {
      PaintBackground();
      DeleteText();
      PaintBitmap();
      PaintText((char *)lastText.c_str());
   }
   else
   {
      PaintBackground();
      PaintBitmap();
   }
}
  
void AUTORUN::DialogWindow::Back()
{
   RECT
      windowRect;

   GetWindowRect(mainWindow_,&windowRect);
   SetWindowPos(mainWindow_, HWND_BOTTOM, 0,0,0,0,SWP_NOMOVE);
 
}

void AUTORUN::DialogWindow::Top()
{
   RECT
      windowRect;

   GetWindowRect(mainWindow_,&windowRect);
   SetWindowPos(mainWindow_, HWND_TOPMOST, windowRect.left,windowRect.top,340,340,SWP_NOMOVE);
 
}


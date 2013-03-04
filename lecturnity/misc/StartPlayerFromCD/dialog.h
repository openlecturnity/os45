#ifndef __AUTOSTART_DIALOG__
#define __AUTOSTART_DIALOG__

#define DESTROY_BUTTON 99
#define HIDE_BUTTON 98

namespace AUTORUN
{
   class DialogWindow
   {

   private:
      HBITMAP 
         hbm_;
      HWND
         mainWindow_,
         hButton_;
      HINSTANCE 
         hInstance_;
      HFONT
         hFont_;
      PAINTSTRUCT 
         ps_;
      RECT
         textRect,
         bitmapRect,
         buttonRect,
         separatorRect;
      HDC
         hdc_;

   public:

      DialogWindow(HINSTANCE hInstance);
      ~DialogWindow() {}

      bool Init();
      BOOL InitInstance(); 
      BOOL Create();
      void Destroy();
      void DestroyButton();
      void HideButton();
      void PaintBackground();
      void PaintBitmap();
      void DeleteText();
      void PaintText(char *text);
      void RePaint(char *text);
      void Back();
      void Top();

      HWND GetWindow() {return mainWindow_;}
      HWND GetButton() {return hButton_;}
   };
}


#endif
// lsupdate.cpp : Definiert den Einsprungpunkt für die Anwendung.
//

#include "stdafx.h"

#include "LanguageSupport.h"     // lsutl32

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   //LecturnityUpdateConfigure();
   HMODULE hLsUpdt = ::LoadLibrary(_T("lsupdt32.dll"));
   bool bFail = false;
      
   if (hLsUpdt)
   {   
      FARPROC LecturnityUpdateConfigure = ::GetProcAddress(hLsUpdt, "LecturnityUpdateConfigure");
      if (LecturnityUpdateConfigure)
         LecturnityUpdateConfigure();
      else
         bFail = true;
   }
   else
      bFail = true;

   if (bFail)
   {
      ::MessageBox(NULL, _T("The files lsupdt32.dll, filesdk.dll or drawsdk.dll are missing.\nApplication will terminate."), 
         _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONERROR);
   }

	return 0;
}

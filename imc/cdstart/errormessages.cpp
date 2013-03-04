#include "StdAfx.h"
#include "errormessages.h"
#include "resource.h"

void MESSAGES::ErrorMessages::ShowMessage(HWND hwnd, DWORD dwErrMsgId, DWORD dwFlags)
{
   char tmp[1024];
   ::LoadString(g_hInstance, dwErrMsgId, tmp, 1024);
   char cap[128];
   if ((dwFlags & MB_ICONERROR) > 0)
      ::LoadString(g_hInstance, IDS_ERROR, cap, 128);
   else if ((dwFlags & MB_ICONWARNING) > 0)
      ::LoadString(g_hInstance, IDS_WARNING, cap, 128);
   else
      ::LoadString(g_hInstance, IDS_INFORMATION, cap, 128);

   ::MessageBox(hwnd, tmp, cap, MB_OK | dwFlags);
}

#ifndef __ERROR__
#define __ERROR__

namespace MESSAGES
{
   class ErrorMessages
   {
   public:
      ErrorMessages() {}
      static void ShowMessage(HWND hwnd, DWORD dwErrMsgId, DWORD dwFlags = MB_ICONINFORMATION);
   };

}

#endif
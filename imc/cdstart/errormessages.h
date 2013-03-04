#ifndef __ERROR__
#define __ERROR__

#include <windows.h> 

namespace MESSAGES
{
/*
   enum language {
      GERMAN = 0,
      ENGLISH = 1
   };
*/

   class ErrorMessages
   {
   public:
      /*
   enum message {
      NO_WINDOW,
      NO_STATUS,
      NO_TEMP_DIR,
      SRC_NO_EXISTS,
      FULL_DISC,
      NO_COPY,
      NO_OVERWRITE,
      NO_HTML,
      NO_CD_PROCESS,
      NO_INSTALLED_PROCESS,
      NO_SETUP_PROCESS,
      NO_LEGAL_JAVA,
      FATAL_ERROR,
      NO_PLAYER,
      ERROR_OPEN,
      ERROR_RUNPLAYER,
      SLOW_MEDIUM
   };
      static int systemLanguage;
      */

      ErrorMessages() {}
      static void ShowMessage(HWND hwnd, DWORD dwErrMsgId, DWORD dwFlags = MB_ICONINFORMATION);
   };

}

#endif
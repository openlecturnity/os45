#include "global.h"

LPCTSTR Local::localizeString(int id)
{
	int res = LoadString(g_hDllInstance, id, tempBuffer_, 1024);

   if (g_hDllInstance == NULL)
   {
      _stprintf(tempBuffer_, _T("<string resources not found>\000"));
   }
   else if (res == 0)
	{
      _stprintf(tempBuffer_, _T("<undefined: %d>\000"), id);
	}
   return tempBuffer_;
}

void Local::setLangCode(int langCode)
{
	langCode_ = langCode;
}

int	      Local::langCode_  = 0;
_TCHAR      Local::tempBuffer_[1024];
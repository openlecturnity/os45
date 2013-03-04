#ifndef __LOCALIZER
#define __LOCALIZER

//#include <windows.h>
//#include <iostream>
//#include "global.h"

class AVGRABBER_EXPORT Local
{
private:
	static _TCHAR tempBuffer_[1024];
	static int langCode_; // en = 1, fr = 2...

public:
	Local() {}
	~Local() {}

   static LPCTSTR localizeString(int id);
	static void setLangCode(int langCode);
};

#endif

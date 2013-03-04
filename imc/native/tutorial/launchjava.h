#ifndef __JavaLauncher
#define __JavaLauncher

#include <Atlbase.h>
#include "resource.h"

class JavaLauncher
{
private:
   CString htmlFileName_;

public:
	JavaLauncher(CString htmlFileName);
	~JavaLauncher();

   bool doLaunch();
};

#endif // __JavaLauncher


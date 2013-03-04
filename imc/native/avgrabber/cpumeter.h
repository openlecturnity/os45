#ifndef __CPU_METER
#define __CPU_METER

//#include "global.h"
//#include <windows.h>

class CPUUsageReader {
public:
	CPUUsageReader();
	~CPUUsageReader();
	int read();

private:
	bool fNTMethod;
	HKEY hkeyKernelCPU;

	unsigned __int64 kt_last;
	unsigned __int64 ut_last;
	unsigned __int64 st_last;
};

#endif

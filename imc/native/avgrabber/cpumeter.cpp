#include "global.h"
//#include "avgrabber.h"
//#include "cpumeter.h"

bool EnableCPUTracking() {
	HKEY hOpen;
	DWORD cbData;
	DWORD dwType;
	LPBYTE pByte;
	DWORD rc;

	bool fSuccess = true;

    if ( (rc = RegOpenKeyEx(HKEY_DYN_DATA, _T("PerfStats\\StartStat"), 0,
					KEY_READ, &hOpen)) == ERROR_SUCCESS) {

		// query to get data size
		if ( (rc = RegQueryValueEx(hOpen,_T("KERNEL\\CPUUsage"),NULL,&dwType,
				NULL, &cbData )) == ERROR_SUCCESS) {

			pByte = (LPBYTE)malloc(cbData);

			rc = RegQueryValueEx(hOpen, _T("KERNEL\\CPUUsage"),NULL,&dwType, pByte,
                              &cbData );

			free(pByte);
		} else
			fSuccess = false;

		RegCloseKey(hOpen);
	} else
		fSuccess = false;

	return fSuccess;
}

bool DisableCPUTracking() {
	HKEY hOpen;
	DWORD cbData;
	DWORD dwType;
	LPBYTE pByte;
	DWORD rc;

	bool fSuccess = true;

    if ( (rc = RegOpenKeyEx(HKEY_DYN_DATA, _T("PerfStats\\StopStat"), 0,
					KEY_READ, &hOpen)) == ERROR_SUCCESS) {

		// query to get data size
		if ( (rc = RegQueryValueEx(hOpen, _T("KERNEL\\CPUUsage"), NULL, &dwType,
				NULL, &cbData )) == ERROR_SUCCESS) {

			pByte = (LPBYTE)malloc(cbData);

			rc = RegQueryValueEx(hOpen, _T("KERNEL\\CPUUsage"), NULL, &dwType, pByte,
                              &cbData );

			free(pByte);
		} else
			fSuccess = false;

		RegCloseKey(hOpen);
	} else
		fSuccess = false;

	return fSuccess;
}

CPUUsageReader::CPUUsageReader() {
	FILETIME ftCreate, ftExit;

	hkeyKernelCPU = NULL;
	fNTMethod = false;

	if (GetProcessTimes(GetCurrentProcess(), &ftCreate, &ftExit, (FILETIME *)&kt_last, (FILETIME *)&ut_last)) {

		// Using Windows NT/2000 method
		GetSystemTimeAsFileTime((FILETIME *)&st_last);

		fNTMethod = true;

	} else {

		// Using Windows 95/98 method

		HKEY hkey;

		if (EnableCPUTracking()) {

			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_DYN_DATA, _T("PerfStats\\StatData"), 0, KEY_READ, &hkey)) {
				hkeyKernelCPU = hkey;
			} else
				DisableCPUTracking();
		}
	}
}

CPUUsageReader::~CPUUsageReader() {
	if (hkeyKernelCPU) {
		RegCloseKey(hkeyKernelCPU);
		DisableCPUTracking();
	}
}

int CPUUsageReader::read() {

	if (hkeyKernelCPU) {
		DWORD type;
		DWORD dwUsage;
		DWORD size = sizeof dwUsage;

		if (ERROR_SUCCESS == RegQueryValueEx(hkeyKernelCPU, _T("KERNEL\\CPUUsage"), 0, &type, (LPBYTE)&dwUsage, (LPDWORD)&size))
			return (int)dwUsage;
		
		return -1;
	} else if (fNTMethod) {
		FILETIME ftCreate, ftExit;
		unsigned __int64 kt, st, ut;
		int cpu;

		GetProcessTimes(GetCurrentProcess(), &ftCreate, &ftExit, (FILETIME *)&kt, (FILETIME *)&ut);
		GetSystemTimeAsFileTime((FILETIME *)&st);

		if (st == st_last)
			return 100;
		else
			cpu = (int) ((100 * (kt + ut - kt_last - ut_last) + (st - st_last)/2) / (st - st_last));

		kt_last = kt;
		ut_last = ut;
		st_last = st;

		return cpu;
	}

	return -1;
}

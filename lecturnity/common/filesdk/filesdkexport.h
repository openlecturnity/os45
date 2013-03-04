#ifndef __FILESDK_EXPORT_H__
#define __FILESDK_EXPORT_H__
// File SDK export

extern bool __declspec(dllimport) CALLBACK  FileSdk_VerifyInternationalFilename(HWND hWndParent, _TCHAR *tszFileName, bool bSave);
extern bool __declspec(dllimport) CALLBACK  FileSdk_ExploreLecturnityHome();
extern bool __declspec(dllimport) CALLBACK  FileSdk_Check16BitOrHigher();

#endif

#ifndef __VML_EXPORT_H
#define __VML_EXPORT_H

#include <windows.h>

#ifdef _VMLSDK_DLL
   #define VMLSDK_EXPORT __declspec(dllexport)
#else
   #define VMLSDK_EXPORT __declspec(dllimport)
#endif


#endif // __VML_EXPORT_H
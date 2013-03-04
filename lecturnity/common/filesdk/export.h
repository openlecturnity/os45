#ifndef __EXPORT_H
#define __EXPORT_H

//#include <windows.h>

#ifdef _FILESDK_DLL
   #define FILESDK_EXPORT __declspec(dllexport)
#else
   #ifdef _FILESDK_STATIC
      #define FILESDK_EXPORT
   #else
      #define FILESDK_EXPORT __declspec(dllimport)
   #endif
#endif


#endif // __EXPORT_H
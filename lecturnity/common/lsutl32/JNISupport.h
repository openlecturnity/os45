#ifndef __JNISUPPORT_H__
#define __JNISUPPORT_H__

#include "jni.h"

#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif

class LSUTL32_EXPORT JNISupport
{
public:
   JNISupport(void);
public:
   ~JNISupport(void);

public:
   static _TCHAR *GetTCharFromString(JNIEnv *env, jstring string);
   static jstring GetStringFromTChar(JNIEnv *env, const _TCHAR *tszString);
   static HWND GetHwndFromJavaWindow(JNIEnv *env, jobject jWindow);
   static HRESULT GetStringFromObject(JNIEnv *env, jobject jo, char *szName, CString &csTarget);
   static HRESULT GetIntFromObject(JNIEnv *env, jobject jo, char *szName, int &iTarget);
   static HRESULT GetLongFromObject(JNIEnv *env, jobject jo, char *szName, __int64 &iTarget);
   static HRESULT SetStringToObject(JNIEnv *env, CString &csSource, char *szName, jobject joTarget);
   static HRESULT SetIntToObject(JNIEnv *env, int iSource, char *szName, jobject joTarget);
   static HRESULT SetLongToObject(JNIEnv *env, long iSource, char *szName, jobject joTarget);
   static HRESULT GetRandomValue(JNIEnv *env, long &lNewRandomValue);

// private: // PZI: set to public (return value is availalable to public via registry anyway)
   static CString GetInstallDir();

};

#endif //__JNISUPPORT_H__

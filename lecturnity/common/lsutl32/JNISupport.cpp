#include "stdafx.h"
#include "JNISupport.h"
#include "jawt_md.h"
#include "lutils.h"


JNISupport::JNISupport(void)
{

}

JNISupport::~JNISupport(void)
{
}

typedef jboolean (JNICALL *procGetAwt)(JNIEnv*, JAWT*);

HWND JNISupport::GetHwndFromJavaWindow(JNIEnv *env, jobject jWindow)
{
   HRESULT hr = S_OK;
   HWND hWnd = 0;

   CString csInstallDir = GetInstallDir();
   CString csAwtDll = csInstallDir + _T("\\jre5\\bin\\jawt.dll");
   HINSTANCE hinstAWT = ::LoadLibrary(csAwtDll);

   if (hinstAWT == NULL)
      hr = E_FAIL; //MessageBox(NULL, _T("Error Loading AWT"), NULL, MB_OK);

   procGetAwt pFuncGetAwt;
   if (SUCCEEDED(hr))
   {
      pFuncGetAwt = (procGetAwt)GetProcAddress(hinstAWT, "_JAWT_GetAWT@8");
      if (!pFuncGetAwt) 
         hr = E_FAIL;
   }

   JAWT awt;
   if (SUCCEEDED(hr))
   {
      awt.version = JAWT_VERSION_1_3;
      if (pFuncGetAwt(env, &awt) == JNI_FALSE)
         hr = E_FAIL;
   }

   JAWT_DrawingSurfaceInfo* dsi;
   JAWT_Win32DrawingSurfaceInfo* dsi_win;

   JAWT_DrawingSurface *surface = NULL;
   
   if (SUCCEEDED(hr))
   {
      surface = awt.GetDrawingSurface(env, jWindow);
      if(surface == NULL)
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
   {		
      jint lock = surface->Lock(surface);

      if((lock & JAWT_LOCK_ERROR) == 0)
      {						
         dsi = surface->GetDrawingSurfaceInfo(surface);						
         dsi_win = (JAWT_Win32DrawingSurfaceInfo*)dsi->platformInfo;						
         hWnd = dsi_win->hwnd;						
         surface->FreeDrawingSurfaceInfo(dsi);						
         surface->Unlock(surface);												
      } else {
         hr = E_FAIL;
      } 
   }

   if (surface)
      awt.FreeDrawingSurface(surface);

   if (hinstAWT)
      FreeLibrary(hinstAWT);

   return hWnd;
}

CString JNISupport::GetInstallDir()
{
   _TCHAR tszInstallDir[_MAX_PATH];
   DWORD dwSize = _MAX_PATH;
   bool success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
      _T("Software\\imc AG\\LECTURNITY"), _T("InstallDir"),
      tszInstallDir, &dwSize);
   if (success)
   {
      CString csInstallDir(tszInstallDir);
      if (csInstallDir[csInstallDir.GetLength() - 1] != '\\')
         csInstallDir += CString(_T("\\"));
      return csInstallDir;
   }
   else
   {
      // this should not happen, try relative path here
      return CString(_T(".\\"));
   }
}


_TCHAR *JNISupport::GetTCharFromString(JNIEnv *env, jstring string)
{
   if (string == NULL)
      return NULL;
   jboolean jbIsCopy = JNI_FALSE;
   const WCHAR *wszString = (WCHAR *)env->GetStringChars(string, &jbIsCopy);
   int nLen = env->GetStringLength(string);
   _TCHAR *tszRet = NULL;
#ifdef _UNICODE
   // Unicode, we're cool.
   tszRet = new WCHAR[nLen + 1];
   wcscpy(tszRet, wszString);
#else
   // ANSI
   tszRet = new char[nLen + 1];
   ::WideCharToMultiByte(CP_ACP, 0, wszString, -1, tszRet, nLen + 1, NULL, NULL);
#endif
   env->ReleaseStringChars(string, (jchar *)wszString);
   wszString = NULL;
   return tszRet;
}

jstring JNISupport::GetStringFromTChar(JNIEnv *env, const _TCHAR *tszString)
{
   jstring string = NULL;
#ifdef _UNICODE
   string = env->NewString((jchar *)tszString, wcslen(tszString));
#else
   // ANSI -> UNICODE
   int nLen = strlen(tszString);
   WCHAR *wszString = new WCHAR[nLen + 1];
   ::MultiByteToWideChar(CP_ACP, 0, tszString, -1, wszString, nLen+1);
   string = env->NewString((jchar *)wszString, nLen);
#endif
   return string;
}

HRESULT JNISupport::GetStringFromObject(JNIEnv *env, jobject jo, char *szName, CString &csTarget)
{
   jclass jclContainer = env->GetObjectClass(jo);
   if (jclContainer == NULL)
      return E_UNEXPECTED;

   jfieldID fidValue = env->GetFieldID(jclContainer, szName, "Ljava/lang/String;");
   if (fidValue == 0)
      return E_UNEXPECTED;

   jstring jsValue = (jstring)env->GetObjectField(jo, fidValue);
   _TCHAR *tszValue = JNISupport::GetTCharFromString(env, jsValue);
   csTarget = tszValue;
   if (tszValue != NULL)
      delete[] tszValue;

   return S_OK;
}

HRESULT JNISupport::GetIntFromObject(JNIEnv *env, jobject jo, char *szName, int &iTarget)
{
   jclass jclContainer = env->GetObjectClass(jo);
   if (jclContainer == NULL)
      return E_UNEXPECTED;

   jfieldID fidValue = env->GetFieldID(jclContainer, szName, "I");
   if (fidValue == 0)
      return E_UNEXPECTED;

   iTarget = env->GetIntField(jo, fidValue);

   return S_OK;
}

HRESULT JNISupport::GetLongFromObject(JNIEnv *env, jobject jo, char *szName, __int64 &iTarget)
{
   jclass jclContainer = env->GetObjectClass(jo);
   if (jclContainer == NULL)
      return E_UNEXPECTED;

   jfieldID fidValue = env->GetFieldID(jclContainer, szName, "J");
   if (fidValue == 0)
      return E_UNEXPECTED;

   iTarget = env->GetLongField(jo, fidValue);

   return S_OK;
}

HRESULT JNISupport::SetStringToObject(JNIEnv *env, CString &csSource, char *szName, jobject joTarget)
{
   jclass jclContainer = env->GetObjectClass(joTarget);
   if (jclContainer == NULL)
      return E_UNEXPECTED;

   jfieldID fidValue = env->GetFieldID(jclContainer, szName, "Ljava/lang/String;");
   if (fidValue == 0)
      return E_UNEXPECTED;

   char *szUtf8 = LString::TCharToUtf8((LPCTSTR)csSource);
   jstring jsValue  = env->NewStringUTF(szUtf8);
   env->SetObjectField(joTarget, fidValue, jsValue);
   delete szUtf8;

   return S_OK;
}

HRESULT JNISupport::SetIntToObject(JNIEnv *env, int iSource, char *szName, jobject joTarget)
{
   jclass jclContainer = env->GetObjectClass(joTarget);
   if (jclContainer == NULL)
      return E_UNEXPECTED;

   jfieldID fidValue = env->GetFieldID(jclContainer, szName, "I");
   if (fidValue == 0)
      return E_UNEXPECTED;

   env->SetIntField(joTarget, fidValue, iSource);

   return S_OK;
}

HRESULT JNISupport::SetLongToObject(JNIEnv *env, long iSource, char *szName, jobject joTarget)
{
   jclass jclContainer = env->GetObjectClass(joTarget);
   if (jclContainer == NULL)
      return E_UNEXPECTED;

   jfieldID fidValue = env->GetFieldID(jclContainer, szName, "J");
   if (fidValue == 0)
      return E_UNEXPECTED;

   env->SetLongField(joTarget, fidValue, iSource);

   return S_OK;
}

HRESULT JNISupport::GetRandomValue(JNIEnv *env, long &lNewRandomValue)
{
   HRESULT hr = S_OK;

   jclass cls = env->FindClass("java/util/Random");
   if (cls == NULL)
      hr = E_FAIL;

   jmethodID midInit = NULL;
   if (SUCCEEDED(hr))
   {
      midInit = env->GetMethodID(cls, "<init>", "()V");
      if (midInit == NULL)
         hr = E_FAIL;
   }

   jobject jobj = NULL;
   if (SUCCEEDED(hr))
   {
      jobj = env->NewObject(cls, midInit);
      if (jobj == NULL)
         hr = E_FAIL;
   }

   jmethodID midNextLong = NULL;
   if (SUCCEEDED(hr))
   {
      midNextLong = env->GetMethodID(cls, "nextLong", "()J");
      if (midNextLong == NULL)
         hr = E_FAIL;
   }

   jlong jRandom = 0;
   if (SUCCEEDED(hr))
   {
      jRandom = env->CallLongMethod(jobj, midNextLong);
   }

   lNewRandomValue = jRandom;

   return hr;
}
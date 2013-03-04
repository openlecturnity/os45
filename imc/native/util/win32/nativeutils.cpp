#include <windows.h>
#include <winreg.h>
#include <io.h>
#include <time.h>
#include <vfw.h>
#include <tchar.h>

#include "imc_lecturnity_util_NativeUtils.h"
#include "KeyGenerator.h"
#include "lutils.h"


KerberokHandler g_kerberokHandler;


#define NATIVEUTILS_REVISION "$Revision: 1.55 $"



int ConvertJavaCharacters(JNIEnv *env, jstring jString, _TCHAR *tszString, int nCharCount)
{
   const WCHAR *wszTemp = (WCHAR *)env->GetStringChars(jString, NULL);

   int iLength = wcslen(wszTemp);
   int iToCopy = iLength < nCharCount -1 ? iLength : nCharCount - 1;
   
#ifdef _UNICODE
   wcsncpy_s(tszString, nCharCount, wszTemp, iToCopy);
#else
   ::WideCharToMultiByte(CP_ACP, 0, wszTemp, iLength, tszString, nCharCount, NULL, NULL);
#endif

   tszString[iToCopy] = 0;

   env->ReleaseStringChars(jString, (jchar *)wszTemp);
   
   return iToCopy;
}

jstring CreateJavaString(JNIEnv *env, _TCHAR *tszInput)
{
   int iLength = _tcslen(tszInput);

#ifdef _UNICODE
   return env->NewString((jchar *)tszInput, iLength);
#else
   return env->NewStringUTF(tszInput);
#endif
}


JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getDllRevision
  (JNIEnv *env, jclass)
{
   jstring jsDllRevision = env->NewStringUTF(NATIVEUTILS_REVISION);
   return jsDllRevision;
}


JNIEXPORT jlong JNICALL Java_imc_lecturnity_util_NativeUtils_getSpaceLeftOnDevice
  (JNIEnv *env, jclass jc, jstring path)
{
   ULARGE_INTEGER
      freeBytesAvailableToCaller,
      totalNumberOfBytes,
      totalNumberOfFreeBytes;
	_TCHAR tszPath[MAX_PATH];

	ConvertJavaCharacters(env, path, tszPath, MAX_PATH);

   if (!GetDiskFreeSpaceEx(tszPath, &freeBytesAvailableToCaller, &totalNumberOfBytes, &totalNumberOfFreeBytes))
      return (jlong)0;

   return (jlong)freeBytesAvailableToCaller.QuadPart;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getInstallationDate
  (JNIEnv *env, jclass jc)
{
   _TCHAR tszInstallDate[16];
   bool ignore = g_kerberokHandler.GetInstallationDate(tszInstallDate);
   
   return CreateJavaString(env, tszInstallDate);
}



JNIEXPORT jint JNICALL Java_imc_lecturnity_util_NativeUtils_getVersionType
  (JNIEnv *env, jclass jc, jstring jsOwnVersion)
{
   _TCHAR tszOwnVersion[32];
   ConvertJavaCharacters(env, jsOwnVersion, tszOwnVersion, 32);

   int iKerberokType = g_kerberokHandler.GetVersionType(tszOwnVersion);

   return iKerberokType;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getVersionString
  (JNIEnv *env, jclass jc, jstring jsBetaRelease)
{
   _TCHAR tszBetaRelease[32];
   ConvertJavaCharacters(env, jsBetaRelease, tszBetaRelease, 32);

   _TCHAR tszVersion[64];
   g_kerberokHandler.GetVersionString(tszVersion, tszBetaRelease);

   return CreateJavaString(env, tszVersion);
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getVersionStringShort
  (JNIEnv *env, jclass jc)
{
   _TCHAR tszVersion[64];
   g_kerberokHandler.GetVersionStringShort(tszVersion);

   return CreateJavaString(env, tszVersion);
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getVersionStringShortShort
(JNIEnv *env, jclass) {
   _TCHAR tszVersion[64];
   g_kerberokHandler.GetVersionStringShortShort(tszVersion);

   return CreateJavaString(env, tszVersion);
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getVersionRevision
(JNIEnv *env, jclass) {
   _TCHAR tszRevision[64]; ZeroMemory(tszRevision, 64 * sizeof _TCHAR);

   LMisc::GetPrivateBuild(tszRevision);

   return CreateJavaString(env, tszRevision);
}


JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getExpirationDate
  (JNIEnv *env, jclass jc)
{
   _TCHAR tszExpDate[32];
   bool success = g_kerberokHandler.GetExpirationDate(tszExpDate);
   if (success)
   {
      // GetExpirationDate may either return "" (no expiration date) or a date.
      return CreateJavaString(env, tszExpDate);
   }

   // Default; error.
   return NULL;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getLanguageCode
  (JNIEnv *env, jclass jc)
{
   _TCHAR tszLangCode[16];
   // first looks in the registry and if that fails (cd) reads
   // the language from a file on the cd
   bool ignore = g_kerberokHandler.GetLanguageCode(tszLangCode);
   
   return CreateJavaString(env, tszLangCode);
}



JNIEXPORT jint JNICALL Java_imc_lecturnity_util_NativeUtils_getEvaluationPeriod
  (JNIEnv *env, jclass jc)
{
   return g_kerberokHandler.GetEvaluationPeriod();
}


JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_isProtectedDocument
  (JNIEnv *env, jclass jc, jstring jdocName)
{
   _TCHAR tszDocName[MAX_PATH];
   ConvertJavaCharacters(env, jdocName, tszDocName, MAX_PATH);

   return g_kerberokHandler.IsProtectedDocument(tszDocName);
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_startFile
  (JNIEnv *env, jclass jc, jstring jFileName)
{
   _TCHAR tszFileName[MAX_PATH];

   ConvertJavaCharacters(env, jFileName, tszFileName, MAX_PATH);

	HINSTANCE hResult = ShellExecute(NULL, _T("open"), tszFileName, _T(""), _T(""), SW_SHOWNORMAL);

   if ((int)hResult <= 32)
      return JNI_FALSE;

   return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_isUserAdmin
  (JNIEnv *, jclass)
{
   return (jboolean)LMisc::IsUserAdmin();
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getEnvVariable
  (JNIEnv *env, jclass jc, jstring envName)
{
   _TCHAR tszEnvName[300];
   ConvertJavaCharacters(env, envName, tszEnvName, 300);

	_TCHAR *tszValue = _tgetenv(tszEnvName);

   return CreateJavaString(env, tszValue);
}



JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getPlayerProperty
  (JNIEnv *env, jclass jc, jstring jKey)
{
   HKEY hKeyUser; 
	BYTE *aData = NULL;

   LONG regErr = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\imc AG\\LECTURNITY\\Player"), NULL, KEY_READ, &hKeyUser); 

	if (regErr == ERROR_SUCCESS)
   {
      _TCHAR tszKey[300];
      ConvertJavaCharacters(env, jKey, tszKey, 300);

	   DWORD dwType = 0;
	   DWORD dwBufSize = 0;

      regErr = ::RegQueryValueEx(hKeyUser, tszKey, NULL, &dwType, NULL, &dwBufSize );

	   if (dwType != REG_SZ)
         regErr = E_FAIL;

	   aData = new BYTE[dwBufSize + 8];

      if (regErr == ERROR_SUCCESS)
         regErr = ::RegQueryValueEx(hKeyUser, tszKey, NULL, &dwType, aData, &dwBufSize);

	   RegCloseKey(hKeyUser);

      _TCHAR tszValue[600];
      if (regErr == ERROR_SUCCESS)
      {
         int iLength = dwBufSize / sizeof _TCHAR - 1;
         _tcsncpy_s(tszValue, 600, (_TCHAR *)aData, iLength);
         tszValue[iLength] = 0;
      }

      delete[] aData;
      
      if (regErr == ERROR_SUCCESS)
         return CreateJavaString(env, tszValue);
      else
         return NULL;
   }

   return NULL;
}

JNIEXPORT void JNICALL Java_imc_lecturnity_util_NativeUtils_setPlayerProperty
  (JNIEnv *env, jclass jc, jstring jKey, jstring jValue)
{
   HKEY hKeyUser; 
	
   LONG regErr = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\imc AG\\LECTURNITY\\Player"), NULL, KEY_SET_VALUE, &hKeyUser); 

	if (regErr != ERROR_SUCCESS)
   {
      DWORD dwDummy;
      regErr = ::RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\imc AG\\LECTURNITY\\Player"),
                  NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyUser, &dwDummy); 

	   if (regErr != ERROR_SUCCESS)
        return;
	}

   _TCHAR tszKey[300];
   ConvertJavaCharacters(env, jKey, tszKey, 300);

   _TCHAR tszValue[600];
   ConvertJavaCharacters(env, jValue, tszValue, 600);

   regErr = ::RegSetValueEx(hKeyUser, tszKey, 0, REG_SZ, (BYTE *)tszValue, (_tcslen(tszValue) + 1) * sizeof _TCHAR);

   RegCloseKey(hKeyUser);

   return;
}


JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getPublisherProperty
  (JNIEnv *env, jclass jc, jstring jsKey)
{
   // ??? Note: fixed lengths in these 4 methods

   _TCHAR tszKey[300];
   tszKey[0] = 0;
   ConvertJavaCharacters(env, jsKey, tszKey, 300);

   _TCHAR tszValue[600];
   tszValue[0] = 0;
   DWORD dwSize = 600;
   bool bSuccess = LRegistry::ReadSettingsString(_T("Publisher"), tszKey, tszValue, &dwSize, NULL);

   if (bSuccess)
      return CreateJavaString(env, tszValue);
   else
      return NULL;
}

JNIEXPORT void JNICALL Java_imc_lecturnity_util_NativeUtils_setPublisherProperty
  (JNIEnv *env, jclass jc, jstring jsKey, jstring jsValue)
{

   _TCHAR tszKey[300];
   tszKey[0] = 0;
   ConvertJavaCharacters(env, jsKey, tszKey, 300);

   _TCHAR tszValue[600];
   tszValue[0] = 0;
   ConvertJavaCharacters(env, jsValue, tszValue, 600);

   LRegistry::WriteSettingsString(_T("Publisher"), tszKey, tszValue);
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getApplicationDataDirectory
  (JNIEnv *env, jclass jc)
{
   HKEY hKeyUser; 
	BYTE *aData = NULL;

   LONG regErr = RegOpenKeyEx (HKEY_CURRENT_USER, 
      _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), NULL, KEY_READ, &hKeyUser); 

	if (regErr == ERROR_SUCCESS)
   {
      DWORD	dwType;
      DWORD dwBufSize = 0;
      
      regErr = RegQueryValueEx(hKeyUser, _T("AppData"), NULL, &dwType, NULL, &dwBufSize );

	   aData = new BYTE[dwBufSize + 8];

      if (regErr == ERROR_SUCCESS)
	      regErr = RegQueryValueEx(hKeyUser, _T("AppData"), NULL, &dwType, aData, &dwBufSize );

	   _TCHAR tszValue[600];
      if (regErr == ERROR_SUCCESS)
      {
         int iLength = dwBufSize / sizeof _TCHAR - 1;
         _tcsncpy_s(tszValue, 600, (_TCHAR *)aData, iLength);
         tszValue[iLength] = 0;
      }

      delete[] aData;

      RegCloseKey(hKeyUser);

      if (regErr == ERROR_SUCCESS)
         return CreateJavaString(env, tszValue);
      else
         return CreateJavaString(env, _T("c:"));
   }


   return CreateJavaString(env, _T("c:"));
}



JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getRegistryValue
  (JNIEnv *env, jclass jc, jstring jRoot, jstring jPath, jstring jKey)
{ 
   HKEY hKeyRoot;
   HKEY hKeyResult;

   _TCHAR tszRegRoot[20];
   ConvertJavaCharacters(env, jRoot, tszRegRoot, 20);
   
   _TCHAR tszRegPath[400];
   ConvertJavaCharacters(env, jPath, tszRegPath, 400);

   _TCHAR tszRegKey[300];
   ConvertJavaCharacters(env, jKey, tszRegKey, 300);

   // translate string into appropriate HKEY-value
   if (_tcsncmp(tszRegRoot, _T("HKCR"), 4) == 0)
   {
      hKeyRoot = HKEY_CLASSES_ROOT;
   }
   else if (_tcsncmp(tszRegRoot, _T("HKCU"), 4) == 0) 
   {
      hKeyRoot = HKEY_CURRENT_USER;
   }
   else if (_tcsncmp(tszRegRoot, _T("HKLM"), 4) == 0) 
   {
      hKeyRoot = HKEY_LOCAL_MACHINE;
   }
   else if (_tcsncmp(tszRegRoot, _T("HKUS"), 4) == 0) 
   {
      hKeyRoot = HKEY_USERS;
   }
   else if (_tcsncmp(tszRegRoot, _T("HKCC"), 4) == 0) 
   {
      hKeyRoot = HKEY_CURRENT_CONFIG;
   }

   // open registry key
   LONG regErr = RegOpenKeyEx (hKeyRoot, tszRegPath, NULL, KEY_READ, &hKeyResult); 

   if (regErr == ERROR_SUCCESS)
   {
      BYTE *aData;
      DWORD dwType;
	   DWORD dwBufSize = 0;

      regErr = RegQueryValueEx(hKeyResult, tszRegKey, NULL, &dwType, NULL, &dwBufSize);
      
	   aData = new BYTE[dwBufSize + 8];

      if (regErr == ERROR_SUCCESS)
         regErr = RegQueryValueEx(hKeyResult, tszRegKey, NULL, &dwType, aData, &dwBufSize);

	   RegCloseKey(hKeyResult);

      _TCHAR tszValue[600];
      if (regErr == ERROR_SUCCESS)
      {
         int iLength = dwBufSize / sizeof _TCHAR - 1;
         _tcsncpy_s(tszValue, 600, (_TCHAR *)aData, iLength);
         tszValue[iLength] = 0;
      }
      // TODO what if this is no string data?

      delete[] aData;

      if (regErr == ERROR_SUCCESS)
         return CreateJavaString(env, tszValue);
      else
         return NULL;
   }

   return NULL;
}




JNIEXPORT jlong JNICALL Java_imc_lecturnity_util_NativeUtils_getCurrentMicroseconds
  (JNIEnv *env, jclass jc)
{
   LARGE_INTEGER liStart;     
   LARGE_INTEGER liFreq; 

   QueryPerformanceCounter(&liStart); 
   QueryPerformanceFrequency(&liFreq);
   double microseconds = ((double)liStart.LowPart/(double)liFreq.LowPart) * 1000000; 

   return (jlong)microseconds;
}


/*
 * Class:     imc_lecturnity_util_NativeUtils
 * Method:    retrieveVideoSize
 * Signature: (Ljava/lang/String;Limc/epresenter/filesdk/VideoClipInfo;)Z
 */
JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_retrieveVideoInfo
  (JNIEnv *env, jclass, jstring jsPath, jobject jvideoClipInfo)
{
   _TCHAR tszFileName[MAX_PATH];
   int nPathLen = ConvertJavaCharacters(env, jsPath, tszFileName, MAX_PATH);

   //_tprintf(_T("retrieveVideoInfo(): File name %s\n"), tszFileName);

   jclass jclVideoClipInfo = env->FindClass("imc/epresenter/filesdk/VideoClipInfo");
   if (jclVideoClipInfo == NULL)
      return (jboolean) false;

   jfieldID fid = env->GetFieldID(jclVideoClipInfo, "clipFileName", "Ljava/lang/String;");
   jstring jsClipFileName = (jstring)env->GetObjectField(jvideoClipInfo, fid);

   _TCHAR tszClipName[MAX_PATH];
   ConvertJavaCharacters(env, jsClipFileName, tszClipName, MAX_PATH);

   //_tprintf(_T("retrieveVideoInfo(): Clip name %s\n"), tszClipName);

   _tcscat_s(tszFileName, MAX_PATH, tszClipName);
 
   AVIFileInit();

   //_tprintf(_T("retrieveVideoInfo(): Opening video file %s\n"), tszFileName);

   bool bAviFileOpen = false;
   PAVIFILE pAviFile;
   AVIFILEINFO aviFileInfo;
   HRESULT hr = AVIFileOpen(&pAviFile, tszFileName, OF_READ, NULL);
   if (SUCCEEDED(hr))
   {
      bAviFileOpen = true;
      hr = AVIFileInfo(pAviFile, &aviFileInfo, sizeof AVIFILEINFO);
   }
   else
   {
      printf("AVIFileOpen() failed, error == 0x%x\n", hr);
   }

   PAVISTREAM pVideoStream;
   if (SUCCEEDED(hr))
      hr = AVIFileGetStream(pAviFile, &pVideoStream, streamtypeVIDEO, 0);

   AVISTREAMINFO aviVideoInfo;
   if (SUCCEEDED(hr))
      hr = AVIStreamInfo(pVideoStream, &aviVideoInfo, sizeof AVISTREAMINFO);


   if (SUCCEEDED(hr))
   {
      int nWidth = (int) aviFileInfo.dwWidth;
      int nHeight = (int) aviFileInfo.dwHeight;

      double fFrameRateInvers = ((double) aviFileInfo.dwScale) / ((double) aviFileInfo.dwRate);
      double dFrameRate = aviFileInfo.dwRate/(double)aviFileInfo.dwScale;
      int nLengthMs = (int) (((double) aviFileInfo.dwLength) * fFrameRateInvers * 1000.0);

      long lFccCodec = (long)aviVideoInfo.fccHandler;

      fid = env->GetFieldID(jclVideoClipInfo, "videoWidth", "I");
      env->SetIntField(jvideoClipInfo, fid, nWidth);
      fid = env->GetFieldID(jclVideoClipInfo, "videoHeight", "I");
      env->SetIntField(jvideoClipInfo, fid, nHeight);
      fid = env->GetFieldID(jclVideoClipInfo, "videoDurationMs", "I");
      env->SetIntField(jvideoClipInfo, fid, nLengthMs);
      fid = env->GetFieldID(jclVideoClipInfo, "videoFramerateTenths", "I");
      env->SetIntField(jvideoClipInfo, fid, (int)(dFrameRate*10 + 0.5));
      fid = env->GetFieldID(jclVideoClipInfo, "fccCodec", "I");
      env->SetIntField(jvideoClipInfo, fid, (jint)(lFccCodec));

   }
   else
   {
      printf("AVIFileInfo() failed, error == 0x%x\n", hr);
   }

   if (pVideoStream != NULL)
      AVIStreamRelease(pVideoStream);

   if (bAviFileOpen)
   {
      AVIFileRelease(pAviFile);
      bAviFileOpen = false;
   }
   pAviFile = NULL;
   AVIFileExit();

   return (jboolean)SUCCEEDED(hr);
}

JNIEXPORT jbyteArray JNICALL Java_imc_lecturnity_util_NativeUtils_retrieveFirstVideoFrame
  (JNIEnv *env, jclass, jstring jsVideoFile)
{
   PAVIFILE pAviFile     = NULL;
   PAVISTREAM pAviStream = NULL;
   PGETFRAME pGetFrame   = NULL;
   AVIFILEINFO aviFileInfo;
   void *pPixelData      = NULL;
   jbyteArray jbaPixels  = NULL;
   
   _TCHAR tszAviFileName[MAX_PATH];
   ConvertJavaCharacters(env, jsVideoFile, tszAviFileName, MAX_PATH);

   BITMAPINFO bi;

   AVIFileInit();

   HRESULT res = AVIFileOpen(&pAviFile, tszAviFileName, OF_READ, NULL);
   if (SUCCEEDED(res))
   {
      res = AVIFileGetStream(pAviFile, &pAviStream, streamtypeVIDEO, 0);
   }
   else
   {
      printf("AVIFileOpen failed: 0x%08x\n", res);
   }

   if (SUCCEEDED(res))
   {
      res = AVIFileInfo(pAviFile, &aviFileInfo, sizeof AVIFILEINFO);
   }
   else
   {
      printf("AVIFileGetStream failed: 0x%08x\n", res);
   }

   if (SUCCEEDED(res))
   {
      pGetFrame = AVIStreamGetFrameOpen(pAviStream, NULL);
   }
   else
   {
      printf("AVIStreamGetFrameOpen failed: 0x%08x\n", res);
   }

   if (SUCCEEDED(res) && NULL == pGetFrame)
   {
      //printf("Trying once more with AVIStreamGetFrameOpen... ");

      // Ok, we did not manage to initialize AVIStreamGetFrame with
      // BITMAPINFO set to NULL. Let's try to read out the format
      // and set a suitable format and see if it works this way. This
      // fixes problems with the DIVX/DX50 FCC handler.
      BITMAPINFO *pBMI = NULL;
      LONG cbBMI = 0;
      HRESULT hr2 = AVIStreamFormatSize(pAviStream, 0, &cbBMI);
      if (SUCCEEDED(hr2))
      {
         pBMI = (BITMAPINFO *) (new BYTE[cbBMI]);
         ZeroMemory(pBMI, cbBMI);
         hr2 = AVIStreamReadFormat(pAviStream, 0, pBMI, &cbBMI);
      }
      if (SUCCEEDED(hr2))
      {
         pBMI->bmiHeader.biSize = sizeof BITMAPINFOHEADER;
         pBMI->bmiHeader.biWidth = aviFileInfo.dwWidth;
         pBMI->bmiHeader.biHeight = aviFileInfo.dwHeight;
         pBMI->bmiHeader.biPlanes = 1;
         pBMI->bmiHeader.biBitCount = 24;
         pBMI->bmiHeader.biCompression = BI_RGB;
         int padding = (3*pBMI->bmiHeader.biWidth) % 4 == 0 ? 0 : 4 - (3*pBMI->bmiHeader.biWidth) % 4;
         pBMI->bmiHeader.biSizeImage = pBMI->bmiHeader.biWidth * 3 * pBMI->bmiHeader.biHeight +
            pBMI->bmiHeader.biHeight * padding;

         // Try again with given BITMAPINFO now
         pGetFrame = AVIStreamGetFrameOpen(pAviStream, &pBMI->bmiHeader);
      }

      if (pBMI)
         delete[] pBMI;
      pBMI = NULL;
   }

   if (pGetFrame)
   {
      // This is the format we need:
      BITMAPINFOHEADER *bih = &bi.bmiHeader;
      bih->biSize = sizeof BITMAPINFOHEADER;
      bih->biBitCount = 24;
      bih->biClrImportant = 0;
      bih->biClrUsed = 0;
      bih->biCompression = BI_RGB;
      bih->biWidth = aviFileInfo.dwWidth;
      bih->biHeight = aviFileInfo.dwHeight;
      bih->biPlanes = 1;

      // if width * 3 is not a multiple of 4, each image scan line has to be
      // padded to a multiple of 4 bytes.
      int padding = (aviFileInfo.dwWidth * 3) % 4;
      if (padding != 0)
         padding = 4 - padding;
      bih->biSizeImage = bih->biWidth * bih->biHeight * 3 + bih->biHeight * padding;
      // bih->biSizeImage = bih->biWidth * bih->biHeight * 3;
      bih->biXPelsPerMeter = 96;
      bih->biYPelsPerMeter = 96;

      // And this is the format we get:
		BITMAPINFOHEADER *pBi = (BITMAPINFOHEADER *) AVIStreamGetFrame(pGetFrame, 0);
      if (pBi != NULL)
      {

         // FILE *file = fopen("C:\\TEMP\\rgb.raw", "wb");

         // if width * 3 is not a multiple of 4, each image scan line has to be
         // padded to a multiple of 4 bytes.
         int padding = (pBi->biWidth * 3) % 4;
         if (padding != 0)
            padding = 4 - padding;
         int biSizeImage = pBi->biWidth * pBi->biHeight * 3 + pBi->biHeight * padding;

         //printf("image width : %d\n", pBi->biWidth);
         //printf("image height: %d\n", pBi->biHeight);
         //printf("image size  : %d vs %d\n", pBi->biSizeImage, biSizeImage);


         //printf("biCompression == %d\n", pBi->biCompression);
         //if (pBi->biCompression == BI_BITFIELDS)
         //  printf("BI_BITFIELDS\n");

         //printf("biBitCount == %d\n", pBi->biBitCount);
         //printf("biClrUsed  == %d\n", pBi->biClrUsed);

         pPixelData = ((char *) pBi) + (pBi->biSize);

         // Now create a DIB section and copy the AVI image into that:
         HDC hdc = ::CreateCompatibleDC(NULL);
         void *pImgData = NULL;
         HBITMAP hImg = ::CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &pImgData, NULL, 0);
         ::SelectObject(hdc, hImg);

         BITMAPINFO biTemp;
         biTemp.bmiHeader = *pBi;

         int stretchRes = ::StretchDIBits(hdc, 0, 0, bih->biWidth, bih->biHeight, 
                                          0, 0, bih->biWidth, bih->biHeight, 
                                          pPixelData, &biTemp, DIB_RGB_COLORS, SRCCOPY);


         if (stretchRes != GDI_ERROR)
         {
            jbaPixels = env->NewByteArray(biSizeImage);
            env->SetByteArrayRegion(jbaPixels, 0, biSizeImage, (jbyte *) pImgData);
         }
         else
         {
            jbaPixels = NULL;
         }

         ::DeleteObject(hImg);
         ::DeleteDC(hdc);
      }
      else
      {
         jbaPixels = NULL;
      }
   }
   else
   {
      printf("AVIStreamGetFrameOpen failed: 0x%08x\n", res);
   }

   // Cleanup
   if (pGetFrame)
   {
      AVIStreamGetFrameClose(pGetFrame);
      pGetFrame = NULL;
   }
   if (pAviStream)
   {
      AVIStreamRelease(pAviStream);
      pAviStream = NULL;
   }
   if (pAviFile)
   {
      AVIFileRelease(pAviFile);
      pAviFile = NULL;
   }

   AVIFileExit();

   //printf("jbaPixels == %08x\n", (DWORD) jbaPixels);

   return jbaPixels;
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_installTwlcCodec
  (JNIEnv *env, jclass cls, jstring jsDllFileName)
{
   _TCHAR tszFileName[MAX_PATH];

   ConvertJavaCharacters(env, jsDllFileName, tszFileName, MAX_PATH);

   BOOL res = ICInstall(mmioStringToFOURCC(_T("VIDC"), 0), 
                        mmioStringToFOURCC(_T("twlc"), 0), 
                        (LPARAM) tszFileName, NULL, ICINSTALL_DRIVER);

   return (jboolean) res;
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_deinstallTwlcCodec
  (JNIEnv *env, jclass cls)
{
   BOOL res = ICRemove(mmioStringToFOURCC(_T("VIDC"), 0), 
                       mmioStringToFOURCC(_T("twlc"), 0), 0);

   return (jboolean) res;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_util_NativeUtils_lecturnityUpdateCheck
  (JNIEnv *env, jclass cls)
{
   LAutoUpdate lAutoUpdate;
   return (jint) (lAutoUpdate.CheckForUpdates());
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_util_NativeUtils_lecturnityUpdateConfigure
  (JNIEnv *env, jclass cls)
{
   LAutoUpdate lAutoUpdate;
   return (jint) (lAutoUpdate.Configure());
}


//////////////////////////////////////////////////////////////////////////////

// From MS Redistribution Example Code
#define MAX_TIMEOUT_MS 30 * 60 * 1000
#define TIME_INCREMENT 250

int DoInstallRedistributionPackage(_TCHAR* tszApplicationName, _TCHAR* tszCommandLine, jboolean bWaitForCompletion)
{
   // From MS Redistribution Example Code
   STARTUPINFO StartUpInfo;
   PROCESS_INFORMATION ProcessInfo;
   
   StartUpInfo.cb = sizeof(StartUpInfo);
   StartUpInfo.lpReserved = NULL;
   StartUpInfo.dwFlags = 0;
   StartUpInfo.cbReserved2 = 0;
   StartUpInfo.lpReserved2 = NULL; 
   StartUpInfo.lpDesktop = NULL;
   StartUpInfo.lpTitle = NULL;
   StartUpInfo.dwX = 0;
   StartUpInfo.dwY = 0;
   StartUpInfo.dwXSize = 0;
   StartUpInfo.dwYSize = 0;
   StartUpInfo.dwXCountChars = 0;
   StartUpInfo.dwYCountChars = 0;
   StartUpInfo.dwFillAttribute = 0;
   StartUpInfo.dwFlags = 0;
   StartUpInfo.wShowWindow = 0;
   StartUpInfo.hStdInput = NULL;
   StartUpInfo.hStdOutput = NULL;
   StartUpInfo.hStdError = NULL;
   
   // Run the installer with the (Quiet for All dialogs) and Reboot:Never 
   // flags. The installation should be silent, and the setup routine  
   // will be notified whether the computer must be restarted.
   if (!CreateProcess(tszApplicationName, tszCommandLine, 
                  NULL, NULL, FALSE, 0, NULL, NULL, 
                  &StartUpInfo, &ProcessInfo))
   {
      DWORD myError = GetLastError();
      return imc_lecturnity_util_NativeUtils_NO_REDIST_PACKAGE;
   }
   
   CloseHandle(ProcessInfo.hThread);
   
   if (bWaitForCompletion )
   {
      DWORD dwTimePassed = 0;
      
      while(TRUE)
      {
         if(WAIT_TIMEOUT != WaitForMultipleObjects(1, &ProcessInfo.hProcess, FALSE, TIME_INCREMENT))
         {
            break;
         }
         
         if (dwTimePassed > MAX_TIMEOUT_MS)
         {
            TerminateProcess(ProcessInfo.hProcess, E_FAIL);
            break;
         }
         
         dwTimePassed += TIME_INCREMENT;
      }
   }
   
   CloseHandle (ProcessInfo.hProcess);
   
   return imc_lecturnity_util_NativeUtils_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

/****************************************************************************
* Java_imc_lecturnity_converter_WindowsMediaConverter_systemNeedsReboot
*
* JNI native method to get the information if the system has to be rebooted.
*/
JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_systemNeedsReboot(JNIEnv* env,
                                                                                  jclass  cl)
{
   // From MS Redistribution Example Code
   
   BOOL bNeedExists = FALSE;
   OSVERSIONINFO osvi;
   osvi.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
   GetVersionEx(&osvi);
   
   if (VER_PLATFORM_WIN32_NT != osvi.dwPlatformId)
   {
      // there was a check for the file wininit.ini but with really old code (_lopen; 16 Bit)
      // there will be no non-32-bit Windows for the moment (and especially no 16 Bit)

      return true;
   }
   else
   {
      HKEY hKey = NULL;
      
      if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
         _T("System\\CurrentControlSet\\Control\\Session Manager"), 0, KEY_READ, &hKey))
      {
         if (ERROR_SUCCESS == RegQueryValueEx( hKey, _T("PendingFileRenameOperations"), 
            NULL, NULL, NULL, NULL))
         {
            bNeedExists = TRUE;
         }
         
         RegCloseKey(hKey);
      }
   }
   
   return (jboolean)bNeedExists;
}

//////////////////////////////////////////////////////////////////////////////

/****************************************************************************
* Java_imc_lecturnity_converter_WindowsMediaConverter_doInstallWmEncoderRedistribution
*
* JNI native method to install the Windows Media Encoder Redistribution Package
*/
JNIEXPORT jint JNICALL Java_imc_lecturnity_util_NativeUtils_doInstallWmEncoderRedistribution(JNIEnv*  env, 
                                                                                             jclass   cl, 
                                                                                             jstring  sInstallDir,
                                                                                             jboolean bWaitForCompletion, 
                                                                                             jboolean bIsQuiet)
{
   _TCHAR tszInstallDir[MAX_PATH];
   ConvertJavaCharacters(env, sInstallDir, tszInstallDir, MAX_PATH);

   _TCHAR tszApplicationName[MAX_PATH];
   _TCHAR tszCommandLine[MAX_PATH];
   
   _tcscpy_s(tszApplicationName, MAX_PATH, tszInstallDir);
   _tcscat_s(tszApplicationName, MAX_PATH, _T("WMEDist.exe"));
   
   if (bIsQuiet)
      _tcscpy_s(tszCommandLine, MAX_PATH, _T("WMEDist.exe /Q:A /R:N"));
   else
      _tcscpy_s(tszCommandLine, MAX_PATH, _T("WMEDist.exe /R:N"));

   printf("in doInstallWmeRedistribution()\n");
   _tprintf(_T("- %s\n"), tszApplicationName);
   _tprintf(_T("- %s\n"), tszCommandLine);
   
   int res = DoInstallRedistributionPackage(tszApplicationName, tszCommandLine, bWaitForCompletion);
   if (res != imc_lecturnity_util_NativeUtils_SUCCESS)
      return (jint)res;
   
   if (Java_imc_lecturnity_util_NativeUtils_systemNeedsReboot(env, cl))
   {
      // Write some code here to ensure your application 
      // restarts the computer, and delays dll registrations 
      // until after the restart, if possible. For example, 
      // set a global flag for use by the application.
      return (jint)imc_lecturnity_util_NativeUtils_SYSTEM_NEEDS_REBOOT;
   }
   
   return (jint)imc_lecturnity_util_NativeUtils_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

/****************************************************************************
* Java_imc_lecturnity_converter_WindowsMediaConverter_doInstallWmFormatRedistribution
*
* JNI native method to install the Windows Media Format Redistribution Package
*/
JNIEXPORT jint JNICALL Java_imc_lecturnity_util_NativeUtils_doInstallWmFormatRedistribution(JNIEnv*  env, 
                                                                                            jclass   cl, 
                                                                                            jstring  sInstallDir,
                                                                                            jboolean bWaitForCompletion, 
                                                                                            jboolean bIsQuiet)
{
   _TCHAR tszInstallDir[MAX_PATH];
   ConvertJavaCharacters(env, sInstallDir, tszInstallDir, MAX_PATH);

   _TCHAR tszApplicationName[MAX_PATH];
   _TCHAR tszCommandLine[MAX_PATH];
   
   _tcscpy_s(tszApplicationName, MAX_PATH, tszInstallDir);
   _tcscat_s(tszApplicationName, MAX_PATH, _T("WMFDist.exe"));
   
   if (bIsQuiet)
      _tcscpy_s(tszCommandLine, MAX_PATH, _T("WMFDist.exe /Q:A /R:N"));
   else
      _tcscpy_s(tszCommandLine, MAX_PATH, _T("WMFDist.exe /R:N"));
   
   printf("in doInstallWmfRedistribution()\n");
   _tprintf(_T("- %s\n"), tszApplicationName);
   _tprintf(_T("- %s\n"), tszCommandLine);
   
   int res = DoInstallRedistributionPackage(tszApplicationName, tszCommandLine, bWaitForCompletion);
   if (res != imc_lecturnity_util_NativeUtils_SUCCESS)
      return (jint)res;
   
   if (Java_imc_lecturnity_util_NativeUtils_systemNeedsReboot(env, cl))
   {
      // Write some code here to ensure your application 
      // restarts the computer, and delays dll registrations 
      // until after the restart, if possible. For example, 
      // set a global flag for use by the application.
      return (jint)imc_lecturnity_util_NativeUtils_SYSTEM_NEEDS_REBOOT;
   }
   
   return (jint)imc_lecturnity_util_NativeUtils_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_imc_lecturnity_util_NativeUtils_getSystemCodepage
  (JNIEnv *, jclass)
{
   return (jint) ::GetACP();
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_isFileLarger2GB
  (JNIEnv *env, jclass, jstring jFileName)
{
   _TCHAR tszFileName[MAX_PATH];
   ConvertJavaCharacters(env, jFileName, tszFileName, MAX_PATH);

   __int64 i64FileSize = LIo::GetFileSize(tszFileName);

   if (i64FileSize != -1i64)
   {
      if (i64FileSize > 2147483647)
         return true;
   }

   // default to false if failed.
   return false;
}


JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_checkNeedsFinalization
(JNIEnv *env, jclass, jstring jsLrdPathName) {
    _TCHAR tszFileName[MAX_PATH];
    ConvertJavaCharacters(env, jsLrdPathName, tszFileName, MAX_PATH);

    bool bResult = LMisc::CheckNeedsFinalization(tszFileName);

    return bResult;
}



JNIEXPORT jstring JNICALL Java_imc_lecturnity_util_NativeUtils_getFinalizedPathName
(JNIEnv *env, jclass, jstring jsLrdPathName) {

    _TCHAR tszFileName[MAX_PATH];
    ConvertJavaCharacters(env, jsLrdPathName, tszFileName, MAX_PATH);
    _TCHAR tszNewFileName[MAX_PATH]; tszNewFileName[0] = 0;

    bool bNeed = LMisc::CheckNeedsFinalization(tszFileName, tszNewFileName);
    // TODO: These two calls (see above) could be caching but that requires
    //       more logic here.

    if (!bNeed && tszNewFileName[0] != 0) // some new file name was found
        return CreateJavaString(env, tszNewFileName);
    else
        return NULL;
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_sendDptMessage
(JNIEnv *env, jclass, jstring jsMessage) {
    static _TCHAR tszMessage[5000];
    tszMessage[0] = 0;

    ConvertJavaCharacters(env, jsMessage, tszMessage, 5000);

    LIo::MessagePipe pipe;
    HRESULT hrPipe = pipe.Init(_T("dpt_msg_pipe"), false);
    if (SUCCEEDED(hrPipe))
        hrPipe = pipe.WriteMessage(tszMessage);

    return SUCCEEDED(hrPipe);
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_sendPublishMessage
(JNIEnv *env, jclass, jstring jsMessage) {
    static _TCHAR tszMessage[5000];
    tszMessage[0] = 0;

    ConvertJavaCharacters(env, jsMessage, tszMessage, 5000);

    LIo::MessagePipe pipe;
    HRESULT hrPipe = pipe.Init(_T("publisher_msg_pipe"), false);
    if (SUCCEEDED(hrPipe))
        hrPipe = pipe.WriteMessage(tszMessage);

    return SUCCEEDED(hrPipe);
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_util_NativeUtils_failWithLiveContextInstallation(
    JNIEnv* env, jclass  cl, jstring jsMessage) {

    _TCHAR tszFailureMessage[1000];
    ConvertJavaCharacters(env, jsMessage, tszFailureMessage, 1000);
    
    return KerberokHandler::FailWithLiveContextInstallation(tszFailureMessage);
}
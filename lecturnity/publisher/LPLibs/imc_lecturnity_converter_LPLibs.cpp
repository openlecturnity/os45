#include "StdAfx.h"
#include "imc_lecturnity_converter_LPLibs.h"

#include "PublisherLibs.h"

#include <jawt.h>
#include <jawt_md.h>

#include "Lrd2VideoConverter.h"
#include "Video2Flv.h"

#include "JNISupport.h"
#include "LanguageSupport.h"
#include "MfcUtils.h"

// Note: this converter is handled differently than SCORM, Templates and Flash:
// the respective methods are not defined as DLL exports and there is no
// Video.cpp and VideoEngine.cpp. You need these if you want to use LPLibs.dll
// not only from Java but also from another exe or dll. But this is not 
// yet the case and thus these exports superfluos code.
CLrd2VideoConverter *g_pVideoConverter = NULL;

CVideo2Flv *g_pFlvConverter = NULL;

#define RETRIEVE_STRING(function, tstring) \
   _TCHAR *##tstring;\
   { \
      DWORD dwTmp = 0; \
      ##function(NULL, &dwTmp); \
      ##tstring = new _TCHAR[dwTmp+1]; \
      dwTmp++; \
      ##function(##tstring, &dwTmp); \
   } \


JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_init
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   return (jint) LPLibs_Init();
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_cleanUp
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   return (jint) LPLibs_CleanUp();
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormInit
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   return (jint) Scorm_Init();
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormCleanUp
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   return (jint) Scorm_CleanUp();
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_scormGetSettings
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Scorm_GetSettings, tszSettings);
   jstring jsSettings = JNISupport::GetStringFromTChar(env, tszSettings);
   delete[] tszSettings;
   return jsSettings;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormUseSettings
  (JNIEnv *env, jclass, jstring jsSettings)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszSettings = JNISupport::GetTCharFromString(env, jsSettings);
   UINT nRes = Scorm_UseSettings(tszSettings);
   if (tszSettings)
      delete[] tszSettings;
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormConfigure
  (JNIEnv *env, jclass, jobject window, jboolean jbDoNotSave)
{
   CLanguageSupport::SetThreadLanguage();

   HWND hWndParent = JNISupport::GetHwndFromJavaWindow(env, window);
   return (jint) Scorm_Configure(hWndParent, jbDoNotSave != 0);
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormSetTargetFileName
  (JNIEnv *env, jclass, jstring fileName)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszTargetFile = JNISupport::GetTCharFromString(env, fileName);
   UINT nRes = Scorm_SetTargetFileName(tszTargetFile);
   if (tszTargetFile)
      delete[] tszTargetFile;
   return nRes;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_scormGetTargetFilePath
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Scorm_GetTargetFilePath, tszTargetFilePath);
   jstring filePath = JNISupport::GetStringFromTChar(env, tszTargetFilePath);
   delete[] tszTargetFilePath;
   return filePath;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormSetIndexFileName
  (JNIEnv *env, jclass, jstring realFileName, jstring fileName)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszRealFileName = JNISupport::GetTCharFromString(env, realFileName);
   _TCHAR *tszFileName = JNISupport::GetTCharFromString(env, fileName);
   UINT nRes = Scorm_SetIndexFileName(tszRealFileName, tszFileName);
   if (tszFileName)
      delete[] tszFileName;
   if (tszRealFileName)
      delete[] tszRealFileName;
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormAddResourceFile
  (JNIEnv *env, jclass, jstring realFileName, jstring fileName)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszRealFileName = JNISupport::GetTCharFromString(env, realFileName);
   _TCHAR *tszFileName = JNISupport::GetTCharFromString(env, fileName);
   UINT nRes = Scorm_AddResourceFile(tszRealFileName, tszFileName);
   if (tszFileName)
      delete[] tszFileName;
   if (tszRealFileName)
      delete[] tszRealFileName;
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormAddMetadataKeyword
  (JNIEnv *env, jclass, jstring keyword)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszKeyword = JNISupport::GetTCharFromString(env, keyword);
   UINT nRes = Scorm_AddMetadataKeyword(tszKeyword);
   if (tszKeyword)
      delete[] tszKeyword;
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormAddGeneralKeyword
  (JNIEnv *env, jclass, jstring keyword, jstring value)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszKeyword = JNISupport::GetTCharFromString(env, keyword);
   _TCHAR *tszValue = JNISupport::GetTCharFromString(env, value);
   UINT nRes = Scorm_AddGeneralKeyword(tszKeyword, tszValue);
   if (tszValue)
      delete[] tszValue;
   if (tszValue)
      delete[] tszKeyword;
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormAddFullTextString
  (JNIEnv *env, jclass, jstring fullText)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszFullText = JNISupport::GetTCharFromString(env, fullText);
   UINT nRes = Scorm_AddFullTextString(tszFullText);
   if (tszFullText)
      delete[] tszFullText;
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormCreatePackage
  (JNIEnv *env, jclass, jobject window)
{
   CLanguageSupport::SetThreadLanguage();

   HWND hWndParent = JNISupport::GetHwndFromJavaWindow(env, window);
   return (jint) Scorm_CreatePackage(hWndParent);
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormGetProgress
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   DWORD dwProgress = 0;
   Scorm_GetProgress(&dwProgress);
   return (jint) dwProgress;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_scormCancel
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   return (jint) Scorm_Cancel();
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_scormGetLanguage
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Scorm_GetLanguage, tszLanguage);
   jstring language = JNISupport::GetStringFromTChar(env, tszLanguage);
   delete[] tszLanguage;
   return language;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_scormGetVersion
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Scorm_GetVersion, tszVersion);
   jstring version = JNISupport::GetStringFromTChar(env, tszVersion);
   delete[] tszVersion;
   return version;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_scormGetStatus
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Scorm_GetStatus, tszStatus);
   jstring status = JNISupport::GetStringFromTChar(env, tszStatus);
   delete[] tszStatus;
   return status;
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_converter_LPLibs_scormGetFullTextAsKeywords
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   BOOL bFullText = FALSE;
   Scorm_GetFullTextAsKeywords(&bFullText);
   return bFullText == TRUE ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateInit
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   return (jint) Template_Init();
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateCleanUp
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   return (jint) Template_CleanUp();
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateReadSettings
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   return (jint) Template_ReadSettings();
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateReadSettingsFromFile
  (JNIEnv *env, jclass, jstring jsTemplatePath)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszTemplatePath = JNISupport::GetTCharFromString(env, jsTemplatePath);
   UINT nRes = Template_ReadSettingsFromFile(tszTemplatePath, true);
   if (tszTemplatePath != NULL)
      delete[] tszTemplatePath;
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateUseSettings
  (JNIEnv *env, jclass, jstring jsTemplateSettings)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszTemplateSettings = JNISupport::GetTCharFromString(env, jsTemplateSettings);
   UINT nRes = Template_SetSettings(tszTemplateSettings);
   if (tszTemplateSettings != NULL)
      delete[] tszTemplateSettings;
   return nRes;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetSettings
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Template_GetSettings, tszSettings);
   jstring jsSettings = JNISupport::GetStringFromTChar(env, tszSettings);
   delete[] tszSettings;
   return jsSettings;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetActivePath
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Template_GetCurrentTemplatePath, tszPath);
   jstring jsPath = JNISupport::GetStringFromTChar(env, tszPath);
   delete[] tszPath;
   return jsPath;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateConfigure
  (JNIEnv *env, jclass, jobject window, jboolean jbDoNotSave)
{
   CLanguageSupport::SetThreadLanguage();

   HWND hWndParent = JNISupport::GetHwndFromJavaWindow(env, window);
   return (jint) Template_Configure(hWndParent, jbDoNotSave != 0);
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateAddKeyword
  (JNIEnv *env, jclass, jstring keyword, jstring value)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszKeyword = JNISupport::GetTCharFromString(env, keyword);
   _TCHAR *tszValue = JNISupport::GetTCharFromString(env, value);
   UINT nRes = Template_AddKeyword(tszKeyword, tszValue);
   if (tszValue)
      delete[] tszValue;
   if (tszKeyword)
      delete[] tszKeyword;
   return nRes;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetTitle
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Template_GetTitle, tszTitle);
   jstring title = JNISupport::GetStringFromTChar(env, tszTitle);
   delete[] tszTitle;
   return title;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetDescription
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Template_GetDescription, tszDescription);
   jstring desc = JNISupport::GetStringFromTChar(env, tszDescription);
   delete[] tszDescription;
   return desc;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetAuthor
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Template_GetAuthor, tszAuthor);
   jstring author = JNISupport::GetStringFromTChar(env, tszAuthor);
   delete[] tszAuthor;
   return author;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetCopyright
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Template_GetCopyright, tszCopyright);
   jstring copyright = JNISupport::GetStringFromTChar(env, tszCopyright);
   delete[] tszCopyright;
   return copyright;
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_converter_LPLibs_templateIsScormCompliant
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   BOOL bIsScormCompliant = FALSE;
   Template_IsScormCompliant(&bIsScormCompliant);
   return (bIsScormCompliant == TRUE ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_converter_LPLibs_templateIsUtfTemplate
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   BOOL bIsUtfTemplate = FALSE;
   Template_IsUtfTemplate(&bIsUtfTemplate);
   return (bIsUtfTemplate == TRUE ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateDoParse
  (JNIEnv *env, jclass, jobject window)
{
   CLanguageSupport::SetThreadLanguage();

   HWND hWndParent = JNISupport::GetHwndFromJavaWindow(env, window);
   return (jint) Template_DoParse(hWndParent);
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateSetTargetDirectory
  (JNIEnv *env, jclass, jstring targetDir)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszTargetDir = JNISupport::GetTCharFromString(env, targetDir);
   UINT nRes = Template_SetTargetDirectory(tszTargetDir);
   if (tszTargetDir)
      delete[] tszTargetDir;
   return nRes;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetTargetDirectory
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Template_GetTargetDirectory, tszDirectory);
   jstring directory = JNISupport::GetStringFromTChar(env, tszDirectory);
   delete[] tszDirectory;
   return directory;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetFileCount
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   DWORD dwCount = 0;
   Template_GetFileCount(&dwCount);
   return (jint) dwCount;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetFile
  (JNIEnv *env, jclass, jint index)
{
   CLanguageSupport::SetThreadLanguage();

   DWORD dwSize = 0;
   UINT nRes = Template_GetFile(index, NULL, &dwSize);
   if (S_OK == nRes)
   {
      dwSize++;
      _TCHAR *tszFileName = new _TCHAR[dwSize];
      Template_GetFile(index, tszFileName, &dwSize);
      jstring fileName = JNISupport::GetStringFromTChar(env, tszFileName);
      delete[] tszFileName;
      return fileName;
   }

   return (jstring) NULL;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetIndexFile
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   RETRIEVE_STRING(Template_GetIndexFile, tszIndexFile);
   jstring fileName = JNISupport::GetStringFromTChar(env, tszIndexFile);
   delete[] tszIndexFile;
   return fileName;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateSetDocumentType
  (JNIEnv *env, jclass, jstring docType)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszDocType = JNISupport::GetTCharFromString(env, docType);
   UINT nRes = Template_SetDocumentType(tszDocType);
   if (tszDocType)
      delete[] tszDocType;
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateSetScormEnabled
  (JNIEnv *env, jclass, jboolean jbScormEnabled)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nRes = Template_SetScormEnabled((jbScormEnabled == JNI_TRUE) ? true : false);
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetScormEnabled
  (JNIEnv *env, jclass, jboolean jbScormEnabled)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nRes = Flash_SetScormEnabled((jbScormEnabled == JNI_TRUE) ? true : false);
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateSetTargetFormat
  (JNIEnv *env, jclass, jstring targetFormat)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszTargetFormat = JNISupport::GetTCharFromString(env, targetFormat);
   UINT nRes = Template_SetTargetFormat(tszTargetFormat);
   if (tszTargetFormat)
      delete[] tszTargetFormat;
   return nRes;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetMaxSlidesWidth
  (JNIEnv *, jclass, jint screenWidth, jint videoWidth, jint stillImageWidth)
{
   CLanguageSupport::SetThreadLanguage();

   int nMaxSlidesWidth = 100;
   Template_GetMaxSlidesWidth(screenWidth, videoWidth, stillImageWidth, &nMaxSlidesWidth);
   return (jint) nMaxSlidesWidth;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetMaxSlidesHeight
  (JNIEnv *, jclass, jint screenHeight, jint videoHeight, jint stillImageHeight)
{
   CLanguageSupport::SetThreadLanguage();

   int nMaxSlidesHeight = 100;
   Template_GetMaxSlidesHeight(screenHeight, videoHeight, stillImageHeight, &nMaxSlidesHeight);
   return (jint) nMaxSlidesHeight;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetGlobalSetting
  (JNIEnv *env, jclass, jstring settingsName)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszSettingsName = JNISupport::GetTCharFromString(env, settingsName);
   _TCHAR *tszSetting = NULL;
   DWORD dwSize = 0;
   if (S_OK == Template_GetGlobalSetting(tszSettingsName, NULL, &dwSize))
   {
      tszSetting = new _TCHAR[dwSize + 1];
      dwSize += 1;
      Template_GetGlobalSetting(tszSettingsName, tszSetting, &dwSize);
   }

   if (tszSetting)
   {
      jstring jsSetting = JNISupport::GetStringFromTChar(env, tszSetting);
      delete[] tszSetting;

      return jsSetting;
   }
   else
   {
      return (jstring) NULL;
   }
}

JNIEXPORT jintArray JNICALL Java_imc_lecturnity_converter_LPLibs_templateGetStandardNavigationStates
  (JNIEnv *env, jclass clazz)
{
   CLanguageSupport::SetThreadLanguage();

   jintArray jarrStates;
   jsize jarrLen = imc_lecturnity_converter_LPLibs_NUMBER_OF_NAVIGATION_STATES;
   jint *jaiStates = new jint[jarrLen];
   int *aiStates = new int[jarrLen];

   int i = 0;
   for (i = 0; i < jarrLen; ++i)
   {
      jaiStates[i] = (jint)0;
      aiStates[i] = 0;
   }

   Template_GetStandardNavigationStates(aiStates);

   for (i = 0; i < jarrLen; ++i)
      jaiStates[i] = (jint)aiStates[i];

   jarrStates = env->NewIntArray(jarrLen);

   env->SetIntArrayRegion(jarrStates, 0, jarrLen, jaiStates);

   delete[] aiStates;
   delete[] jaiStates;

   return jarrStates;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_templatePutStandardNavigationStatesToLrd
  (JNIEnv *env, jclass clazz, jstring strLrdFile, jintArray jNavigationStates)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszLrdFile = JNISupport::GetTCharFromString(env, strLrdFile);

   jboolean isCopy;
   jint* intArrayElements = env->GetIntArrayElements(jNavigationStates, &isCopy);

   int nSize = imc_lecturnity_converter_LPLibs_NUMBER_OF_NAVIGATION_STATES;
   int *aiStandardNavigationStates = new int[nSize];
   for (int i = 0; i < nSize; ++i)
      aiStandardNavigationStates[i] = (int)intArrayElements[i];

   UINT nRes =  Template_PutStandardNavigationStatesToLrd(tszLrdFile, aiStandardNavigationStates);

   delete[] aiStandardNavigationStates;
   aiStandardNavigationStates = NULL;

   if (tszLrdFile)
      delete[] tszLrdFile;

   return (jint)nRes;
}




JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetInputFiles
  (JNIEnv *env, jclass, jstring sLrdFile, jstring sEvqFile, jstring sObjFile, jstring sLadFile)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszLrdFile = JNISupport::GetTCharFromString(env, sLrdFile);
   _TCHAR *tszEvqFile = JNISupport::GetTCharFromString(env, sEvqFile);
   _TCHAR *tszObjFile = JNISupport::GetTCharFromString(env, sObjFile);
   _TCHAR *tszLadFile = JNISupport::GetTCharFromString(env, sLadFile);
   UINT nResult = Flash_SetInputFiles(tszLrdFile, tszEvqFile, tszObjFile, tszLadFile);
   if (tszLrdFile)
      delete[] tszLrdFile;
   if (tszEvqFile)
      delete[] tszEvqFile;
   if (tszObjFile)
      delete[] tszObjFile;
   if (tszLadFile)
      delete[] tszLadFile;

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetInputAudioIsFromFlv
  (JNIEnv *env, jclass, jboolean jbIsAudioFlv, jstring sAudioFlvFile)
{
   CLanguageSupport::SetThreadLanguage();

   bool bIsAudioFlv = (jbIsAudioFlv == JNI_TRUE) ? true : false;
   _TCHAR *tszAudioFlvFile = JNISupport::GetTCharFromString(env, sAudioFlvFile);

   UINT nResult = Flash_SetInputAudioIsFromFlv(bIsAudioFlv, tszAudioFlvFile);

   if (tszAudioFlvFile)
      delete[] tszAudioFlvFile;

   return nResult;
}

/*
JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetInputAudioFile
  (JNIEnv *env, jclass, jstring sAudioFile, jstring sAudioFlvFile)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszAudioFile = JNISupport::GetTCharFromString(env, sAudioFile);
   _TCHAR *tszAudioFlvFile = JNISupport::GetTCharFromString(env, sAudioFlvFile);

   UINT nResult = Flash_SetInputAudioFile(tszAudioFile, tszAudioFlvFile);

   if (tszAudioFile)
      delete[] tszAudioFile;
   if (tszAudioFlvFile)
      delete[] tszAudioFlvFile;

   return nResult;
}
*/

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetInputVideoFile
  (JNIEnv *env, jclass, jstring sVideoFile, jstring sVideoFlvFile, jint iVideoWidth, jint iVideoHeight, jboolean jbDoPadding)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszVideoFile = JNISupport::GetTCharFromString(env, sVideoFile);
   _TCHAR *tszVideoFlvFile = JNISupport::GetTCharFromString(env, sVideoFlvFile);
   bool bDoPadding = (jbDoPadding == JNI_TRUE) ? true : false;

   UINT nResult = Flash_SetInputVideoFile(tszVideoFile, tszVideoFlvFile, (int)iVideoWidth, (int)iVideoHeight, bDoPadding);

   if (tszVideoFile)
      delete[] tszVideoFile;
   if (tszVideoFlvFile)
      delete[] tszVideoFlvFile;

   return nResult;
}


JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetNumberOfClips
  (JNIEnv *, jclass, jint iNumOfClips)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_SetNumberOfClips(iNumOfClips);

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetInputClipFile
  (JNIEnv *env, jclass, jint iIndex, jstring sClipFile, jstring sClipFlvFile, jint nClipWidth, jint nClipHeight, jboolean jbDoPadding, jlong lClipBeginMs, jlong lClipEndMs)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszClipFile = JNISupport::GetTCharFromString(env, sClipFile);
   _TCHAR *tszClipFlvFile = JNISupport::GetTCharFromString(env, sClipFlvFile);
   bool bDoPadding = (jbDoPadding == JNI_TRUE) ? true : false;

   UINT nResult = Flash_SetInputClipFile(iIndex, tszClipFile, tszClipFlvFile, nClipWidth, nClipHeight, bDoPadding, lClipBeginMs, lClipEndMs);

   if (tszClipFile)
      delete[] tszClipFile;
   if (tszClipFlvFile)
      delete[] tszClipFlvFile;

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetInputStillImageFile
  (JNIEnv *env, jclass, jstring sStillImageFile, jint iWidth, jint iHeight)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszStillImageFile = JNISupport::GetTCharFromString(env, sStillImageFile);

   UINT nResult = Flash_SetInputStillImageFile(tszStillImageFile, (int)iWidth, (int)iHeight);

   if (tszStillImageFile)
      delete[] tszStillImageFile;

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetOutputFile
  (JNIEnv *env, jclass, jstring sSwfFile)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszSwfFile = JNISupport::GetTCharFromString(env, sSwfFile);
   UINT nResult = Flash_SetOutputFile(tszSwfFile);
   if (tszSwfFile)
      delete[] tszSwfFile;

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetRtmpPath
  (JNIEnv *env, jclass, jstring sRtmpPath)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszRtmpPath = JNISupport::GetTCharFromString(env, sRtmpPath);
   UINT nResult = Flash_SetRtmpPath(tszRtmpPath);
   if (tszRtmpPath)
      delete[] tszRtmpPath;

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetWbInputDimension
  (JNIEnv *, jclass, jint nWidth, jint nHeight)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_SetWbInputDimension(nWidth, nHeight);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetWbOutputDimension
  (JNIEnv *, jclass, jint nWidth, jint nHeight)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_SetWbOutputDimension(nWidth, nHeight);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetMovieDimension
  (JNIEnv *, jclass, jint nWidth, jint nHeight)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_SetMovieDimension(nWidth, nHeight);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetFlashVersion
  (JNIEnv *, jclass, jint nFlashVersion)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_SetFlashVersion(nFlashVersion);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetAutostartMode
  (JNIEnv *, jclass, jboolean jbAutostartMode)
{
   CLanguageSupport::SetThreadLanguage();

   bool bAutostartMode = (jbAutostartMode == JNI_TRUE) ? true : false;
   UINT nResult = Flash_SetAutostartMode(bAutostartMode);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetSgStandAloneMode
  (JNIEnv *, jclass, jboolean jbSgStandAloneMode)
{
   CLanguageSupport::SetThreadLanguage();

   bool bSgStandAloneMode = (jbSgStandAloneMode == JNI_TRUE) ? true : false;
   UINT nResult = Flash_SetSgStandAloneMode(bSgStandAloneMode);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetDrawEvalNote
  (JNIEnv *, jclass, jboolean jbDrawEvalNote)
{
   CLanguageSupport::SetThreadLanguage();

   bool bDrawEvalNote = (jbDrawEvalNote == JNI_TRUE) ? true : false;
   UINT nResult = Flash_SetDrawEvalNote(bDrawEvalNote);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetPspDesign
  (JNIEnv *, jclass, jboolean jbPspDesign)
{
   CLanguageSupport::SetThreadLanguage();

   bool bPspDesign = (jbPspDesign == JNI_TRUE) ? true : false;
   UINT nResult = Flash_SetPspDesign(bPspDesign);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetShowClipsInVideoArea
  (JNIEnv *, jclass, jboolean jbShowClipsInVideoArea)
{
   CLanguageSupport::SetThreadLanguage();

   bool bShowClipsInVideoArea = (jbShowClipsInVideoArea == JNI_TRUE) ? true : false;
   UINT nResult = Flash_SetShowClipsInVideoArea(bShowClipsInVideoArea);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashHideLecturnityIdentity
  (JNIEnv *, jclass, jboolean jbHideLecIdentity)
{
   CLanguageSupport::SetThreadLanguage();

   bool bHideLecIdentity = (jbHideLecIdentity == JNI_TRUE) ? true : false;
   UINT nResult = Flash_HideLecturnityIdentity(bHideLecIdentity);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashEnableOutputDir
  (JNIEnv *env, jclass, jstring sOutputDir)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszOutputDir = JNISupport::GetTCharFromString(env, sOutputDir);
   UINT nResult = Flash_EnableOutputDir(tszOutputDir);
   if (tszOutputDir)
      delete[] tszOutputDir;

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashResetAllDimensions
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_ResetAllDimensions();
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashGetOutputWidth
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nOutputWidth = Flash_GetMovieWidth();
   return nOutputWidth;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashGetOutputHeight
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nOutputHeight = Flash_GetMovieHeight();
   return nOutputHeight;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashGetContentDimensions
  (JNIEnv *env, jclass, jobject jDimMovieSize, jobject jDimVideoSizeIn, jobject jDimPagesSizeIn, jobject jDimLogoSize, 
   jboolean jbDisplayHeaderText, jboolean jbIsControlbarVisible, jboolean jbShowVideoOnPages, 
   jobject jDimVideoSizeOut, jobject jDimPagesSizeOut)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = S_OK;

   jclass jclDimension = env->FindClass("java/awt/Dimension");
   if (NULL == jclDimension)
      nResult = E_FAIL;

   jfieldID fid = 0;
   SIZE sizeMovie;
   SIZE sizeVideoIn;
   SIZE sizePagesIn;
   SIZE sizeLogo;
   SIZE sizeVideoOut;
   SIZE sizePagesOut;

   if (SUCCEEDED(nResult))
   {
      fid = env->GetFieldID(jclDimension, "width", "I");
      sizeMovie.cx = env->GetIntField(jDimMovieSize, fid);
      if (NULL != jDimPagesSizeIn)
         sizePagesIn.cx = env->GetIntField(jDimPagesSizeIn, fid);
      else
         sizePagesIn.cx = -1;
      if (NULL != jDimLogoSize)
         sizeLogo.cx = env->GetIntField(jDimLogoSize, fid);
      else
         sizeLogo.cx = -1;
      if (NULL != jDimVideoSizeIn)
         sizeVideoIn.cx = env->GetIntField(jDimVideoSizeIn, fid);
      else
         sizeVideoIn.cx = -1;
      sizeVideoOut.cx = env->GetIntField(jDimVideoSizeOut, fid);
      sizePagesOut.cx = env->GetIntField(jDimPagesSizeOut, fid);

      fid = env->GetFieldID(jclDimension, "height", "I");
      sizeMovie.cy = env->GetIntField(jDimMovieSize, fid);
      if (NULL != jDimPagesSizeIn)
         sizePagesIn.cy = env->GetIntField(jDimPagesSizeIn, fid);
      else
         sizePagesIn.cy = -1;
      if (NULL != jDimLogoSize)
         sizeLogo.cy = env->GetIntField(jDimLogoSize, fid);
      else
         sizeLogo.cy = -1;
      if (NULL != jDimVideoSizeIn)
         sizeVideoIn.cy = env->GetIntField(jDimVideoSizeIn, fid);
      else
         sizeVideoIn.cy = -1;
      sizeVideoOut.cy = env->GetIntField(jDimVideoSizeOut, fid);
      sizePagesOut.cy = env->GetIntField(jDimPagesSizeOut, fid);
   }

   nResult = Flash_GetContentDimensions(&sizeMovie, &sizeVideoIn, &sizePagesIn, &sizeLogo, 
                                        jbDisplayHeaderText > 0, jbIsControlbarVisible > 0, jbShowVideoOnPages > 0, 
                                        &sizeVideoOut, &sizePagesOut);

   if (SUCCEEDED(nResult))
   {
      fid = env->GetFieldID(jclDimension, "width", "I");
      env->SetIntField(jDimVideoSizeOut, fid, sizeVideoOut.cx);
      env->SetIntField(jDimPagesSizeOut, fid, sizePagesOut.cx);

      fid = env->GetFieldID(jclDimension, "height", "I");
      env->SetIntField(jDimVideoSizeOut, fid, sizeVideoOut.cy);
      env->SetIntField(jDimPagesSizeOut, fid, sizePagesOut.cy);
   }

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashObtainMovieInsets
  (JNIEnv *env, jclass, jobject jInsets, jobject jDimWbSize, jboolean jbDisplayHeaderText, jobject jDimLogoSize, 
   jobject jDimVideoSize, jboolean jbIsControlbarVisible)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = S_OK;

   jclass jclInsets = env->FindClass("java/awt/Insets");
   if (NULL == jclInsets)
      nResult = E_FAIL;
   jclass jclDimension = env->FindClass("java/awt/Dimension");
   if (NULL == jclDimension)
      nResult = E_FAIL;

   jfieldID fid = 0;
   RECT rectInsets;
   SIZE sizeWbInput;
   SIZE sizeLogo;
   SIZE sizeVideo;
      
   
   if (SUCCEEDED(nResult))
   {
      fid = env->GetFieldID(jclDimension, "width", "I");
      sizeWbInput.cx = env->GetIntField(jDimWbSize, fid);
      if (NULL != jDimLogoSize)
         sizeLogo.cx = env->GetIntField(jDimLogoSize, fid);
      else
         sizeLogo.cx = -1;
      if (NULL != jDimVideoSize)
         sizeVideo.cx = env->GetIntField(jDimVideoSize, fid);
      else
         sizeVideo.cx = -1;

      fid = env->GetFieldID(jclDimension, "height", "I");
      sizeWbInput.cy = env->GetIntField(jDimWbSize, fid);
      if (NULL != jDimLogoSize)
         sizeLogo.cy = env->GetIntField(jDimLogoSize, fid);
      else
         sizeLogo.cy = -1;
      if (NULL != jDimVideoSize)
         sizeVideo.cy = env->GetIntField(jDimVideoSize, fid);
      else
         sizeVideo.cy = -1;
   }
   
   if (SUCCEEDED(nResult))
   {
      nResult = Flash_ObtainInsets(&rectInsets, &sizeWbInput, jbDisplayHeaderText > 0, &sizeLogo, 
         &sizeVideo, jbIsControlbarVisible > 0);
   }
   
   if (SUCCEEDED(nResult))
   {
      fid = env->GetFieldID(jclInsets, "top", "I");
      env->SetIntField(jInsets, fid, rectInsets.top);
      fid = env->GetFieldID(jclInsets, "left", "I");
      env->SetIntField(jInsets, fid, rectInsets.left);
      fid = env->GetFieldID(jclInsets, "bottom", "I");
      env->SetIntField(jInsets, fid, rectInsets.bottom);
      fid = env->GetFieldID(jclInsets, "right", "I");
      env->SetIntField(jInsets, fid, rectInsets.right);
   }

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashConvertImages
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_ConvertImages();
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashConvertFonts
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_ConvertFonts();
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashConvertAudioToMp3
  (JNIEnv *env, jclass, jstring sMp3File, jint iAudioBitrate)
{
    CLanguageSupport::SetThreadLanguage();

    _TCHAR *tszMp3File = JNISupport::GetTCharFromString(env, sMp3File);

    UINT nResult = Flash_ConvertAudioToMp3Ds(tszMp3File, iAudioBitrate);

    if (tszMp3File != NULL)
        delete[] tszMp3File;

    return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashSetStandardNavigationStates
  (JNIEnv *, jclass, 
  jint nNavigationControlBar, 
  jint nNavigationStandardButtons, 
  jint nNavigationTimeLine, 
  jint nNavigationTimeDisplay, 
  jint nNavigationDocumentStructure, 
  jint nNavigationPluginContextMenu)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_SetStandardNavigationStates(nNavigationControlBar, nNavigationStandardButtons, 
      nNavigationTimeLine, nNavigationTimeDisplay, nNavigationDocumentStructure, nNavigationPluginContextMenu);
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashBuildMovie
  (JNIEnv *env, jclass, jint jiBasicColor, jint jiBackColor, jstring sAuthor, jstring sTitle, jint jiTextColor, 
   jstring sLogoFile, jstring sLogoUrl, jint iFlashFormatType, jboolean jbShowVideoOnPages)
{
   CLanguageSupport::SetThreadLanguage();

   CMap<CString, LPCTSTR, CString, LPCTSTR> mapMetadata;

   // should be more generic (see below), but for now....
   if (NULL != sAuthor && NULL != sTitle)
   {
      _TCHAR *tszAuthor = JNISupport::GetTCharFromString(env, sAuthor);
      _TCHAR *tszTitle = JNISupport::GetTCharFromString(env, sTitle);
      CString strKeyA = _T("author");
      CString strKeyT = _T("title");
      CString strAuthor = tszAuthor;
      CString strTitle = tszTitle;
      mapMetadata.SetAt(strKeyA, strAuthor);
      mapMetadata.SetAt(strKeyT, strTitle);
      if (tszAuthor)
         delete[] tszAuthor;
      if (tszTitle)
         delete[] tszTitle;
   }
   // else do not display title/author

   /*
   if (NULL != jMetadata)
   {
      jclass classMetadata = env->GetObjectClass(jMetadata);
      
      jfieldID idTitle = env->GetFieldID(classMetadata, "title", "Ljava/lang/String;");
   }
   */

   _TCHAR *tszLogoImage = NULL;
   if (NULL != sLogoFile)
      tszLogoImage = JNISupport::GetTCharFromString(env, sLogoFile);
   // else do not display a logo

   _TCHAR *tszLogoUrl = NULL;
   if (NULL != sLogoUrl)
      tszLogoUrl = JNISupport::GetTCharFromString(env, sLogoUrl);
   // else do not use a logo url

   bool bShowVideoOnPages = (jbShowVideoOnPages > 0) ? true : false;

///   UINT nResult = Flash_BuildMovie(&mapMetadata, RGB2BGR(jiBasicColor), RGB2BGR(jiBackColor), RGB2BGR(jiTextColor), tszLogoImage, tszLogoUrl);
   // Gdiplus::ARGB is used instead of COLORREF --> RGB2BGR not needed
   UINT nResult = Flash_BuildMovie(&mapMetadata, jiBasicColor, jiBackColor, jiTextColor, tszLogoImage, tszLogoUrl, iFlashFormatType, bShowVideoOnPages);

   if (tszLogoImage)
      delete[] tszLogoImage;
   if (tszLogoUrl)
      delete[] tszLogoUrl;

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashCreateSlidestar
  (JNIEnv *env, jclass, jstring sThumbFiles)
{
   CLanguageSupport::SetThreadLanguage();
   _TCHAR *tszThumbFiles = JNISupport::GetTCharFromString(env, sThumbFiles);
   return (jint) Flash_CreateSlidestar(CString(tszThumbFiles));
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashCreateFlexFiles
  (JNIEnv *env, jclass, jstring sBaseUrl, jstring sStreamUrl, jboolean jbShowVideoOnPages, jboolean jbShowClipsInVideoArea, jboolean jbStripFlvSuffix, jboolean jbDoAutostart, jboolean jbScormEnabled, 
   jboolean jbUseOriginalSlideSize)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszBaseUrl = JNISupport::GetTCharFromString(env, sBaseUrl);
   _TCHAR *tszStreamUrl = JNISupport::GetTCharFromString(env, sStreamUrl);
   bool bShowVideoOnPages = (jbShowVideoOnPages > 0) ? true : false;
   bool bShowClipsInVideoArea = (jbShowClipsInVideoArea > 0) ? true : false;
   bool bStripFlvSuffix = (jbStripFlvSuffix > 0) ? true : false;
   bool bDoAutostart = (jbDoAutostart > 0) ? true : false;
   bool bScormEnabled = (jbScormEnabled > 0) ? true : false;
   bool bUseOriginalSlideSize = (jbUseOriginalSlideSize > 0) ? true : false;

   return (jint) Flash_CreateFlexFiles(tszBaseUrl, tszStreamUrl, bShowVideoOnPages, bShowClipsInVideoArea, bStripFlvSuffix, bDoAutostart, bScormEnabled, bUseOriginalSlideSize);
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashPrepareScorm
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   return (jint) Flash_PrepareScorm();
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashCancel
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_Cancel();
   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashCleanup
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   UINT nResult = Flash_Cleanup();
   return nResult;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_flashLastErrorDetail
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   DWORD dwSize = 0;
   UINT nResult = Flash_LastErrorDetail(NULL, &dwSize);
   if (S_OK == nResult && dwSize > 0)
   {
      dwSize++;
      _TCHAR *tszError = new _TCHAR[dwSize];
      Flash_LastErrorDetail(tszError, &dwSize);
      jstring errorString = JNISupport::GetStringFromTChar(env, tszError);
      delete[] tszError;
      return errorString;
   }
   
   return (jstring) NULL;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_flashGetFontListFontNotFound
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   DWORD dwSize = 0;
   UINT nResult = Flash_GetFontListFontNotFound(NULL, &dwSize);
   if (S_OK == nResult && dwSize > 0)
   {
      dwSize++;
      _TCHAR *tszFontList = new _TCHAR[dwSize];
      Flash_GetFontListFontNotFound(tszFontList, &dwSize);
      jstring fontList = JNISupport::GetStringFromTChar(env, tszFontList);
      delete[] tszFontList;
      return fontList;
   }
 
   return (jstring) NULL;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_flashGetFontListGlyphNotFound
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   DWORD dwSize = 0;
   UINT nResult = Flash_GetFontListGlyphNotFound(NULL, &dwSize);
   if (S_OK == nResult && dwSize > 0)
   {
      dwSize++;
      _TCHAR *tszFontList = new _TCHAR[dwSize];
      Flash_GetFontListGlyphNotFound(tszFontList, &dwSize);
      jstring fontList = JNISupport::GetStringFromTChar(env, tszFontList);
      delete[] tszFontList;
      return fontList;
   }
 
   return (jstring) NULL;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_flashGetFontListStylesNotSupported
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   DWORD dwSize = 0;
   UINT nResult = Flash_GetFontListStylesNotSupported(NULL, &dwSize);
   if (S_OK == nResult && dwSize > 0)
   {
      dwSize++;
      _TCHAR *tszFontList = new _TCHAR[dwSize];
      Flash_GetFontListStylesNotSupported(tszFontList, &dwSize);
      jstring fontList = JNISupport::GetStringFromTChar(env, tszFontList);
      delete[] tszFontList;
      return fontList;
   }
 
   return (jstring) NULL;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_flashGetPreloaderFilename
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   DWORD dwSize = 0;
   UINT nResult = Flash_GetPreloaderFilename(NULL, &dwSize);
   if (S_OK == nResult && dwSize > 0)
   {
      dwSize++;
      _TCHAR *tszPreloaderFilename = new _TCHAR[dwSize];
      Flash_GetPreloaderFilename(tszPreloaderFilename, &dwSize);
      jstring preloaderFilename = JNISupport::GetStringFromTChar(env, tszPreloaderFilename);
      delete[] tszPreloaderFilename;
      return preloaderFilename;
   }
 
   return (jstring) NULL;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_LPLibs_flashGetTooShortPagesTimes
  (JNIEnv *env, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   DWORD dwSize = 0;
   UINT nResult = Flash_GetTooShortPagesTimes(NULL, &dwSize);
   if (S_OK == nResult && dwSize > 0)
   {
      dwSize++;
      _TCHAR *tszTooShortPagesTimes = new _TCHAR[dwSize];
      Flash_GetTooShortPagesTimes(tszTooShortPagesTimes, &dwSize);
      jstring tooShortPagesTimes = JNISupport::GetStringFromTChar(env, tszTooShortPagesTimes);
      delete[] tszTooShortPagesTimes;
      return tooShortPagesTimes;
   }
 
   return (jstring) NULL;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flashShowColorDialog
  (JNIEnv *, jclass, jint jiCurrentColor)
{
   CLanguageSupport::SetThreadLanguage();

   COLORREF color = RGB2BGR(jiCurrentColor);
   DWORD retColor = jiCurrentColor;
   CWnd wndParent;
   wndParent.Attach(::GetForegroundWindow());
   CColorDialog colorDlg(color, CC_RGBINIT | CC_FULLOPEN, &wndParent);

   if (IDOK == colorDlg.DoModal())
      retColor = BGR2RGB(colorDlg.GetColor());

   wndParent.Detach();

   return (jint)retColor;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_videoPrepare
  (JNIEnv *env, jclass, jstring jsInputFile, jstring jsOutputFile, 
  jint jiFrameRateTenth, jint jiVideoQFactor, jint jiAudioRateKbit, 
  jint jiOutputWidth, jint jiOutputHeight, jboolean jbEmbedClips, 
  jboolean jbVideoInsteadOfPages, jboolean jbTargetIPod)
{
   CLanguageSupport::SetThreadLanguage();

   if (g_pVideoConverter == NULL)
      g_pVideoConverter = new CLrd2VideoConverter();

   bool bIsIgnored = false;

   _TCHAR *tszInput = JNISupport::GetTCharFromString(env, jsInputFile);
   HRESULT hr = g_pVideoConverter->SetInputFile(tszInput);
     

   _TCHAR *tszOutput = JNISupport::GetTCharFromString(env, jsOutputFile);
   if (SUCCEEDED(hr))
   {
      hr = g_pVideoConverter->PrepareConversion(
         tszOutput, bIsIgnored, (float)jiFrameRateTenth/10.0f, 
         (UINT)jiVideoQFactor, (UINT)jiAudioRateKbit,
         (UINT)jiOutputWidth, (UINT)jiOutputHeight, jbEmbedClips != 0, 
         jbVideoInsteadOfPages != 0, jbTargetIPod != 0);

      if (FAILED(hr))
      {
         HRESULT hrFilter = g_pVideoConverter->CheckForFilters();
         if (FAILED(hrFilter))
            hr = hrFilter;
         // TODO merge with PrepareConversion or something else.
      }
   }

   if (tszInput != NULL)
      delete[] tszInput;
   if (tszOutput != NULL)
      delete[] tszOutput;

   if (SUCCEEDED(hr))
      return (jint)0;
   else
   {
      if (hr == E_VID_FILE_NOT_FOUND)
         hr = imc_lecturnity_converter_LPLibs_INPUT_FILE_MISSING;
      else if (hr == E_VID_FILTERS_MISSING)
         hr = imc_lecturnity_converter_LPLibs_VIDEO_FILTERS_MISSING;

      return (jint)hr;
   }
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_videoConvertStart
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   if (g_pVideoConverter == NULL)
      return imc_lecturnity_converter_LPLibs_UNSPECIFIC_ERROR;


   HRESULT hr = g_pVideoConverter->StartConversion();

   if (SUCCEEDED(hr))
      return (jint)0;
   else
   {
      // TODO translation to java error values.
      return (jint)hr;
   }
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_videoConvertCancel
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   if (g_pVideoConverter == NULL)
      return imc_lecturnity_converter_LPLibs_UNSPECIFIC_ERROR;

   
   HRESULT hr = g_pVideoConverter->AbortConversion();

   if (SUCCEEDED(hr))
      return (jint)0;
   else
   {
      // TODO translation to java error values.
      return (jint)hr;
   }
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_videoGetProgress
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   if (g_pVideoConverter == NULL)
      return imc_lecturnity_converter_LPLibs_UNSPECIFIC_ERROR;

   
   int iProgress = g_pVideoConverter->GetProgress();

   if (iProgress >= 0)
      return (jint)iProgress;
   else
   {
      // TODO translation to java error values.
      return (jint)iProgress;
   }
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_videoCleanup
  (JNIEnv *, jclass)
{
   CLanguageSupport::SetThreadLanguage();

   if (g_pVideoConverter != NULL)
   {
      delete g_pVideoConverter;
      g_pVideoConverter = NULL;
   }

   return (jint)imc_lecturnity_converter_LPLibs_SUCCESS;
}

JNIEXPORT jobjectArray JNICALL Java_imc_lecturnity_converter_LPLibs_getListOfInteractiveOpenedFiles
  (JNIEnv *env, jclass, jstring sEvqFile, jstring sObjFile)
{
   CLanguageSupport::SetThreadLanguage();

   jobjectArray jaFileList = NULL;

   _TCHAR *tszEvqFile = JNISupport::GetTCharFromString(env, sEvqFile);
   _TCHAR *tszObjFile = JNISupport::GetTCharFromString(env, sObjFile);

   UINT nResult = Flash_SetInputFiles(NULL, tszEvqFile, tszObjFile, NULL);

   if (nResult == S_OK)
   {
      int iNumOfFiles = (int)Flash_GetListOfInteractiveOpenedFiles(NULL);

      if(iNumOfFiles > 0)
      {
         CString* acsFileList = new CString[iNumOfFiles];
         Flash_GetListOfInteractiveOpenedFiles(acsFileList);

         jaFileList = (jobjectArray)env->NewObjectArray(iNumOfFiles, 
                                                        env->FindClass("java/lang/String"),
                                                        env->NewStringUTF(""));

         for (int i = 0; i < iNumOfFiles; ++i)
         {
            jstring jsFile = JNISupport::GetStringFromTChar(env, acsFileList[i]);
            env->SetObjectArrayElement(jaFileList, (jint)i, jsFile);
         }

      }
   }

   return jaFileList;
}

  JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_lpdPrepareCopiesOfInteractiveOpenedFiles
  (JNIEnv *env, jclass, jstring sEvqFile, jstring sObjFile, jstring sOutFile)
{
   CLanguageSupport::SetThreadLanguage();

   _TCHAR *tszEvqFile = JNISupport::GetTCharFromString(env, sEvqFile);
   _TCHAR *tszObjFile = JNISupport::GetTCharFromString(env, sObjFile);
   _TCHAR *tszOutFile = JNISupport::GetTCharFromString(env, sOutFile);

   // Use methods from the Flash Engine

   UINT nResult = Flash_SetInputFiles(NULL, tszEvqFile, tszObjFile, NULL);

   if (nResult == S_OK)
      nResult = Flash_SetOutputFile(tszOutFile);

   if (nResult == S_OK)
      nResult = Flash_PrepareCopiesOfInteractiveOpenedFiles();

   if (tszEvqFile)
      delete[] tszEvqFile;
   if (tszObjFile)
      delete[] tszObjFile;
   if (tszOutFile)
      delete[] tszOutFile;

   return nResult;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flvPrepare
  (JNIEnv *env, jclass, jstring jsAvi, jint jiAviMs, jstring jsLad, jstring jsFlv, 
  jint jiVideoRate, jint jiAudioRate, jint jiOutputWidth, jint jiOutputHeight, 
  jboolean jbCombineWithClips, jboolean jbIncreaseKeyframes, jboolean jbDoPadding,
  jboolean jbForVersion9)
{
   if (g_pFlvConverter != NULL)
      delete g_pFlvConverter;

   g_pFlvConverter = new CVideo2Flv();

   REFERENCE_TIME rtVideoOffset = (LONGLONG)jiAviMs * 10*1000;
   _TCHAR *tszAviFile = JNISupport::GetTCharFromString(env, jsAvi);
   _TCHAR *tszLadFile = JNISupport::GetTCharFromString(env, jsLad);
   _TCHAR *tszFlvFile = JNISupport::GetTCharFromString(env, jsFlv);
   
   bool bIncreaseKeyframes = jbIncreaseKeyframes ? true : false;
   HRESULT hr = Flash_SetIncreaseKeyframes(bIncreaseKeyframes);

   if (SUCCEEDED(hr))
      hr = g_pFlvConverter->SetInputFiles(tszAviFile, rtVideoOffset, tszLadFile, 0);
   
   if (SUCCEEDED(hr))
   {
      hr = g_pFlvConverter->SetOutputParams(
         tszFlvFile, jiOutputWidth, jiOutputHeight, jiVideoRate, jiAudioRate, 
         jbIncreaseKeyframes != 0, jbDoPadding != 0, jbForVersion9 != 0);
   }

   if (!jbCombineWithClips)
   {
      if (SUCCEEDED(hr))
         hr = g_pFlvConverter->PrepareInput();

      if (SUCCEEDED(hr))
      {
         hr = g_pFlvConverter->PrepareConversion();
         
         if (FAILED(hr))
         {
            if (hr == VFW_E_TYPE_NOT_ACCEPTED)
               hr = imc_lecturnity_converter_LPLibs_AUDIO_8_16_KHZ_NOT_SUPPORTED;
            else if (hr == E_ACCESSDENIED)
               hr = imc_lecturnity_converter_LPLibs_FLV_NO_OVERWRITE;
            else if (FAILED(g_pFlvConverter->CheckForFilters()))
               hr = imc_lecturnity_converter_LPLibs_VIDEO_FILTERS_MISSING;
         }
      }
   }

   if (tszAviFile != NULL)
      delete[] tszAviFile;
   if (tszLadFile != NULL)
      delete[] tszLadFile;
   if (tszFlvFile != NULL)
      delete[] tszFlvFile;


   if (SUCCEEDED(hr))
      return (jint)0;
   else
      return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flvAddClip
  (JNIEnv *env, jclass, jstring jsAvi, jint jiAviMs, jboolean jbIsLastClip)
{
   if (g_pFlvConverter == NULL)
      return (jint)E_UNEXPECTED;

   REFERENCE_TIME rtClipOffset = (LONGLONG)jiAviMs * 10*1000;
   _TCHAR *tszAviFile = JNISupport::GetTCharFromString(env, jsAvi);
   
   HRESULT hr = S_OK;
   if (tszAviFile != NULL)
      hr = g_pFlvConverter->AddClip(tszAviFile, rtClipOffset);

   if (jbIsLastClip)
   {
      if (SUCCEEDED(hr))
         hr = g_pFlvConverter->PrepareInput();

      if (SUCCEEDED(hr))
      {
         hr = g_pFlvConverter->PrepareConversion();

         if (FAILED(hr))
         {
            if (hr == VFW_E_TYPE_NOT_ACCEPTED)
               hr = imc_lecturnity_converter_LPLibs_AUDIO_8_16_KHZ_NOT_SUPPORTED;
            else if (hr == E_ACCESSDENIED)
               hr = imc_lecturnity_converter_LPLibs_FLV_NO_OVERWRITE;
            else if (FAILED(g_pFlvConverter->CheckForFilters()))
               hr = imc_lecturnity_converter_LPLibs_VIDEO_FILTERS_MISSING;
         }
      }
   }

   if (tszAviFile != NULL)
      delete[] tszAviFile;
 
   if (SUCCEEDED(hr))
      return (jint)0;
   else
      return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flvConvertStart
  (JNIEnv *env, jclass)
{
   if (g_pFlvConverter == NULL)
      return (jint)E_UNEXPECTED;

   HRESULT hr = g_pFlvConverter->StartConversion();

   if (SUCCEEDED(hr))
      return (jint)0;
   else
      return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flvConvertCancel
  (JNIEnv *env, jclass)
{
   if (g_pFlvConverter == NULL)
      return (jint)E_UNEXPECTED;

   g_pFlvConverter->AbortConversion();

   return (jint)imc_lecturnity_converter_LPLibs_SUCCESS;
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_converter_LPLibs_flvCheckOverwrite
  (JNIEnv *env, jclass, jstring jsOutputFlv)
{
   _TCHAR *tszOutputFlv = JNISupport::GetTCharFromString(env, jsOutputFlv);

   jboolean jbCanOverwrite = CVideo2Flv::CheckOverwrite(tszOutputFlv);

   if (tszOutputFlv != NULL)
      delete tszOutputFlv;

   return jbCanOverwrite;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flvGetProgress
  (JNIEnv *env, jclass)
{
   if (g_pFlvConverter == NULL)
      return (jint)0;

   return (jint)g_pFlvConverter->GetProgress();
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_LPLibs_flvCleanup
  (JNIEnv *env, jclass)
{
   if (g_pFlvConverter != NULL)
   {
      delete g_pFlvConverter;
      g_pFlvConverter = NULL;
   }

   return (jint)imc_lecturnity_converter_LPLibs_SUCCESS;
}

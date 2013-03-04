#include "StdAfx.h"

#include "imc_lecturnity_converter_wizard_UploaderAccess.h"
#include "Uploader.h"
#include "JNISupport.h"

// this is global/static but it is only needed to preserve settings over two method calls
// like for example ShowTransferConfigDialog() + GetTransferData()
CUploader *s_pUploader = NULL;
bool s_bStartSessionCalled = false;
//Refix for bug5252
//In profile mode at first attempt IsSlidestar is called when s_pUploader is NULL
//Previous call of IsSlidestar is taken into consideration with s_bIsSlidestarCalled
bool s_bIsSlidestarCalled = false;

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_ShowYtTransferConfigDialog
  (JNIEnv *env, jclass, jobject joTransferSettings, jobject joWndParent)
{
   HRESULT hr = S_OK;

   CString csUser;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strUser", csUser);

   CString csPass;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strPass", csPass);

   CString csTitle;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strTitle", csTitle);

   CString csDescription;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strDescription", csDescription);

   CString csKeywords;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strKeywords", csKeywords);

   CString csCategory;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strCategory", csCategory);

   int iPrivacy;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iPrivacy", iPrivacy);
   
   if (SUCCEEDED(hr))
   {
      if (s_pUploader != NULL)
         delete s_pUploader;

      s_pUploader = new CUploader();
	  
      HWND hwnd = NULL;
      hwnd = JNISupport::GetHwndFromJavaWindow(env, joWndParent);
      if(hwnd !=NULL)
      {
         s_pUploader->SetParentHwnd(hwnd);
      }
      hr = s_pUploader->ShowYtTransferConfigDialog(csUser, csPass, csTitle, csDescription, csKeywords, csCategory, iPrivacy);
      // can also be E_FAIL
   }

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_GetYtTransferData
  (JNIEnv *env, jclass, jobject joTransferSettings)
{
   HRESULT hr = S_OK;

   CString csServer;
   CString csCategory;
   int iPrivacy = 0;
   CString csUser;
   CString csPass;
   CString csTitle;
   CString csDescription;
   CString csKeywords;

   if (s_pUploader != NULL)
   {
       hr = s_pUploader->GetYtTransferData(csUser, csPass, csTitle, csDescription, csKeywords, csCategory, &iPrivacy);
       _tprintf(_T("!!!Upl cat=%s priv=%d \n"), csCategory, iPrivacy);
   }
   else
      hr = E_UNEXPECTED;

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csUser, "m_strUser", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csPass, "m_strPass", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csTitle, "m_strTitle", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csDescription, "m_strDescription", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csKeywords, "m_strKeywords", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csCategory, "m_strCategory", joTransferSettings);

   if (SUCCEEDED(hr))
       hr = JNISupport::SetIntToObject(env, iPrivacy, "m_iPrivacy", joTransferSettings);

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_ShowTransferConfigDialog
  (JNIEnv *env, jclass, jobject joTransferSettings, jobject joWndParent, jboolean jbIsSlidestar)
{
   HRESULT hr = S_OK;

   CString csServer;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strServer", csServer);

   int iProtocol = 0;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iProtocol", iProtocol);

   int iPort = 0;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iPort", iPort);

   CString csUser;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strUser", csUser);

   CString csPass;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strPass", csPass);

   CString csDir;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strDir", csDir);
   
   if (SUCCEEDED(hr))
   {
      if (s_pUploader != NULL)
         delete s_pUploader;

      s_pUploader = new CUploader();
	  
	  // BUGFIX 5307: SLIDESTAR>Server access settings - Data directory sometimes enabled:
	  // using s_bDirDisabled introduced with Bugfix 4562 only worked the first time
	  // now using additonal parameter jbIsSlidestar instead
      if(jbIsSlidestar)  //if(s_bDirDisabled)
      {
         s_pUploader->IsSlidestar(true);
      //   s_bDirDisabled = false;
      }
      HWND hwnd = NULL;
      hwnd = JNISupport::GetHwndFromJavaWindow(env, joWndParent);
      if(hwnd !=NULL)
      {
         s_pUploader->SetParentHwnd(hwnd);
      }
      hr = s_pUploader->ShowTransferConfigDialog(csServer, iProtocol, iPort, csUser, csPass, csDir);
      // can also be E_FAIL
   }

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_GetTransferData
  (JNIEnv *env, jclass, jobject joTransferSettings)
{
   HRESULT hr = S_OK;

   CString csServer;
   int iProtocol = 0;
   int iPort = 0;
   CString csUser;
   CString csPass;
   CString csDir;

   if (s_pUploader != NULL)
      hr = s_pUploader->GetTransferData(csServer, &iProtocol, &iPort, csUser, csPass, csDir);
   else
      hr = E_UNEXPECTED;

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csServer, "m_strServer", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetIntToObject(env, iProtocol, "m_iProtocol", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetIntToObject(env, iPort, "m_iPort", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csUser, "m_strUser", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csPass, "m_strPass", joTransferSettings);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csDir, "m_strDir", joTransferSettings);

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_ShowPodcastConfigDialog
  (JNIEnv *env, jclass, jstring jsChannelName, jstring jsChannelUrl, jstring jsMediaUrl,
  jobject joSettingsChannel, jobject joSettingsMedia, jobject joWndParent) 
{
   HRESULT hr = S_OK;

   CString csName;
   CString csChannelUrl;
   CString csMediaUrl;
   CString csChannelServer;
   int iChannelProtocol = 0;
   int iChannelPort = 0;
   CString csChannelUser;
   CString csChannelPass;
   CString csChannelDir;
   CString csMediaServer;
   int iMediaProtocol = 0;
   int iMediaPort = 0;
   CString csMediaUser;
   CString csMediaPass;
   CString csMediaDir;


   if (SUCCEEDED(hr))
   {
      _TCHAR *tszName = JNISupport::GetTCharFromString(env, jsChannelName);
      csName = tszName;
      if (tszName != NULL)
         delete[] tszName;

      _TCHAR *tszChannelUrl = JNISupport::GetTCharFromString(env, jsChannelUrl);
      csChannelUrl = tszChannelUrl;
      if (tszChannelUrl != NULL)
         delete[] tszChannelUrl;

      _TCHAR *tszMediaUrl = JNISupport::GetTCharFromString(env, jsMediaUrl);
      csMediaUrl = tszMediaUrl;
      if (tszMediaUrl != NULL)
         delete[] tszMediaUrl;
   }

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsChannel, "m_strServer", csChannelServer);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joSettingsChannel, "m_iProtocol", iChannelProtocol);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joSettingsChannel, "m_iPort", iChannelPort);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsChannel, "m_strUser", csChannelUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsChannel, "m_strPass", csChannelPass);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsChannel, "m_strDir", csChannelDir);

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsMedia, "m_strServer", csMediaServer);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joSettingsMedia, "m_iProtocol", iMediaProtocol);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joSettingsMedia, "m_iPort", iMediaPort);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsMedia, "m_strUser", csMediaUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsMedia, "m_strPass", csMediaPass);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsMedia, "m_strDir", csMediaDir);

   if (SUCCEEDED(hr))
   {
      if (s_pUploader != NULL)
         delete s_pUploader;

      s_pUploader = new CUploader();
      HWND hwnd = NULL;
      hwnd = JNISupport::GetHwndFromJavaWindow(env, joWndParent);
      if(hwnd !=NULL)
      {
         s_pUploader->SetParentHwnd(hwnd);
      }
      hr = s_pUploader->ShowPodcastChannelConfigDialog(
         csName, csChannelUrl, csMediaUrl, 
         csChannelServer, iChannelProtocol, iChannelPort, csChannelUser, csChannelPass, csChannelDir,
         csMediaServer, iMediaProtocol, iMediaPort, csMediaUser, csMediaPass, csMediaDir);
      // can also be E_FAIL
   }

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_GetPodcastData
  (JNIEnv *env, jclass, jobject joChannelName, jobject joChannelUrl, jobject joMediaUrl,
  jobject joSettingsChannel, jobject joSettingsMedia)
{
   CString csName;
   CString csChannelUrl;
   CString csMediaUrl;
   CString csChannelServer;
   int iChannelProtocol = 0;
   int iChannelPort = 0;
   CString csChannelUser;
   CString csChannelPass;
   CString csChannelDir;
   CString csMediaServer;
   int iMediaProtocol = 0;
   int iMediaPort = 0;
   CString csMediaUser;
   CString csMediaPass;
   CString csMediaDir;

   HRESULT hr = S_OK;

   if (s_pUploader != NULL)
   {
      hr = s_pUploader->GetPodcastChannelData(csName, csChannelUrl, csMediaUrl,
         csChannelServer, &iChannelProtocol, &iChannelPort, csChannelUser, csChannelPass, csChannelDir,
         csMediaServer, &iMediaProtocol, &iMediaPort, csMediaUser, csMediaPass, csMediaDir);
   }
   else
      hr = E_UNEXPECTED;

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csName, "m_strContent", joChannelName);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csChannelUrl, "m_strContent", joChannelUrl);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csMediaUrl, "m_strContent", joMediaUrl);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csChannelServer, "m_strServer", joSettingsChannel);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetIntToObject(env, iChannelProtocol, "m_iProtocol", joSettingsChannel);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetIntToObject(env, iChannelPort, "m_iPort", joSettingsChannel);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csChannelUser, "m_strUser", joSettingsChannel);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csChannelPass, "m_strPass", joSettingsChannel);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csChannelDir, "m_strDir", joSettingsChannel);

   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csMediaServer, "m_strServer", joSettingsMedia);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetIntToObject(env, iMediaProtocol, "m_iProtocol", joSettingsMedia);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetIntToObject(env, iMediaPort, "m_iPort", joSettingsMedia);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csMediaUser, "m_strUser", joSettingsMedia);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csMediaPass, "m_strPass", joSettingsMedia);
   if (SUCCEEDED(hr))
      hr = JNISupport::SetStringToObject(env, csMediaDir, "m_strDir", joSettingsMedia);

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_IsSlidestar
  (JNIEnv *env, jclass, jboolean jbIsSlidestar)
{
   HRESULT hr = S_OK;
   if (s_pUploader != NULL)
	   hr = s_pUploader->IsSlidestar(jbIsSlidestar?true:false);
   else
   {
       s_bIsSlidestarCalled = jbIsSlidestar?true:false;
      hr = E_UNEXPECTED;
   }

   return (jint)hr;
}


JNIEXPORT jboolean JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_IsNotEmptyDir
  (JNIEnv *env, jclass, jstring jsTarget, jobject joTransferSettings, jobject joWndParent)
{
   HRESULT hr = S_OK;

   // ??? how about deleting s_pUploader?

   if (s_pUploader == NULL)
      s_pUploader = new CUploader();

   CString csServer;
   int iProtocol = 0;
   int iPort = 0;
   CString csUser;
   CString csPass;
   CString csTarget;

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strServer", csServer);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iProtocol", iProtocol);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iPort", iPort);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strUser", csUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strPass", csPass);
   if (SUCCEEDED(hr))
      csTarget = JNISupport::GetTCharFromString(env, jsTarget);

   bool bIsDir = true; // initial value
   bool bIsEmptyDir = true; // initial value
   bool* pbIsDir = &bIsDir;
   bool* pbIsEmptyDir = &bIsEmptyDir;

   if (SUCCEEDED(hr))
   {
      HWND hwnd = NULL;
      hwnd = JNISupport::GetHwndFromJavaWindow(env, joWndParent);
      if(hwnd !=NULL)
      {
         s_pUploader->SetParentHwnd(hwnd);
      }

      hr = s_pUploader->CheckTargetExists(csServer, iProtocol, iPort, csUser, csPass, csTarget, pbIsDir, pbIsEmptyDir);
   }

   bool bIsNotEmty;
   if (SUCCEEDED(hr) && (*pbIsDir == true))
      bIsNotEmty = !(*pbIsEmptyDir);
   else 
      bIsNotEmty = false;

   return (jboolean)bIsNotEmty;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_CheckTargetExists
  (JNIEnv *env, jclass, jstring jsTarget, jobject joTransferSettings, jobject joWndParent)
{
   HRESULT hr = S_OK;

   // ??? how about deleting s_pUploader?

   if (s_pUploader == NULL)
      s_pUploader = new CUploader();

   CString csServer;
   int iProtocol = 0;
   int iPort = 0;
   CString csUser;
   CString csPass;
   CString csTarget;

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strServer", csServer);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iProtocol", iProtocol);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iPort", iPort);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strUser", csUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strPass", csPass);
   if (SUCCEEDED(hr))
      csTarget = JNISupport::GetTCharFromString(env, jsTarget);

   if (SUCCEEDED(hr))
   {
      HWND hwnd = NULL;
      hwnd = JNISupport::GetHwndFromJavaWindow(env, joWndParent);
      if(hwnd !=NULL)
      {
         s_pUploader->SetParentHwnd(hwnd);
      }
      bool bIsDir = false; // initial value
      bool bIsEmptyDir = false; // initial value
      bool* pbIsDir = &bIsDir;
      bool* pbIsEmptyDir = &bIsEmptyDir;

      hr = s_pUploader->CheckTargetExists(csServer, iProtocol, iPort, csUser, csPass, csTarget, pbIsDir, pbIsEmptyDir);
   }

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_CheckYtUpload
  (JNIEnv *env, jclass, jobject joTransferSettings, jstring jsRecLength, jobject joWndParent)
{
   HRESULT hr = S_OK;

   // ??? how about deleting s_pUploader?
   _tprintf(_T("!!!Upl enter CheckYtUpload method\n"));

   if (s_pUploader == NULL)
      s_pUploader = new CUploader();

   CString csUser;
   CString csPass;
   CString csRecLength;

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strUser", csUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strPass", csPass);
   if (SUCCEEDED(hr))
      csRecLength = JNISupport::GetTCharFromString(env, jsRecLength);

   _tprintf(_T("!!!Upl in CheckYtUpload method user = %s - length = %s\n"), csUser, csRecLength);

   if (SUCCEEDED(hr))
   {
      HWND hwnd = NULL;
      hwnd = JNISupport::GetHwndFromJavaWindow(env, joWndParent);
      if(hwnd !=NULL)
      {
         s_pUploader->SetParentHwnd(hwnd);
      }
     
      hr = s_pUploader->CheckYtUpload(csUser, csPass, csRecLength);
      _tprintf(_T("!!!Upl CheckYtUpload, hr = %d\n"), hr);
   }
   if(SUCCEEDED(hr))
   {
       if(s_bStartSessionCalled)
       {
           s_pUploader->StartSession();
           s_bStartSessionCalled = false;
       }
   }

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_CheckUpload
  (JNIEnv *env, jclass, jobject joTransferSettings, jobject joWndParent)
{
   HRESULT hr = S_OK;

   // ??? how about deleting s_pUploader?

   if (s_pUploader == NULL)
      s_pUploader = new CUploader();

   CString csServer;
   int iProtocol = 0;
   int iPort = 0;
   CString csUser;
   CString csPass;
   CString csDir;

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strServer", csServer);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iProtocol", iProtocol);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iPort", iPort);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strUser", csUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strPass", csPass);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strDir", csDir);

   if (SUCCEEDED(hr))
   {
      HWND hwnd = NULL;
      hwnd = JNISupport::GetHwndFromJavaWindow(env, joWndParent);
      if(hwnd !=NULL)
      {
         s_pUploader->SetParentHwnd(hwnd);
      }
      if(s_bIsSlidestarCalled)
      {
          s_pUploader->IsSlidestar(true);
          s_bIsSlidestarCalled = false;
      }
      hr = s_pUploader->CheckUpload(csServer, iProtocol, iPort, csUser, csPass, csDir);
   }
   if(SUCCEEDED(hr))
   {
       if(s_bStartSessionCalled)
       {
           s_pUploader->StartSession();
           s_bStartSessionCalled = false;
       }
   }

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_StartUpload
  (JNIEnv *env, jclass, jobject joTransferSettings, jobjectArray jaFiles)
{
   HRESULT hr = S_OK;

   if (s_pUploader == NULL)
      s_pUploader = new CUploader();

   CString csServer;
   int iProtocol = 0;
   int iPort = 0;
   CString csUser;
   CString csPass;
   CString csDir;

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strServer", csServer);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iProtocol", iProtocol);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iPort", iPort);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strUser", csUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strPass", csPass);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strDir", csDir);

   CArray<CString, CString> aFiles;
   int iFileCount = env->GetArrayLength(jaFiles);
   for (int i=0; i<iFileCount && SUCCEEDED(hr); ++i)
   {
      jobject joOneFile = env->GetObjectArrayElement(jaFiles, i);
    
      CString csOneFile;
      _TCHAR *tszOneFile = JNISupport::GetTCharFromString(env, (jstring)joOneFile);
      csOneFile = tszOneFile;
      if (tszOneFile != NULL)
         delete[] tszOneFile;

      if (csOneFile.GetLength() > 0)
         aFiles.Add(csOneFile);
      else
         hr = E_INVALIDARG;
   }

   if (SUCCEEDED(hr))
   {
      if (s_pUploader != NULL)
      {
         hr = s_pUploader->StartUpload(csServer, iProtocol, iPort, csUser, csPass, csDir, aFiles);
      }
      else
         hr = E_UNEXPECTED;
   }

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_StartYtUpload
  (JNIEnv *env, jclass, jobject joTransferSettings, jobjectArray jaFiles)
{
   HRESULT hr = S_OK;

   if (s_pUploader == NULL)
      s_pUploader = new CUploader();

   CString csUser;
   CString csPass;
   CString csTitle;
   CString csDescription;
   CString csKeywords;
   CString csCategory;
   int iPrivacy = 0;
   

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strUser", csUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strPass", csPass);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strTitle", csTitle);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strDescription", csDescription);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strKeywords", csKeywords);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joTransferSettings, "m_strCategory", csCategory);
   if (SUCCEEDED(hr))
       hr = JNISupport::GetIntFromObject(env, joTransferSettings, "m_iPrivacy", iPrivacy);

   CArray<CString, CString> aFiles;
   int iFileCount = env->GetArrayLength(jaFiles);
   for (int i=0; i<iFileCount && SUCCEEDED(hr); ++i)
   {
      jobject joOneFile = env->GetObjectArrayElement(jaFiles, i);
    
      CString csOneFile;
      _TCHAR *tszOneFile = JNISupport::GetTCharFromString(env, (jstring)joOneFile);
      csOneFile = tszOneFile;
      if (tszOneFile != NULL)
         delete[] tszOneFile;

      if (csOneFile.GetLength() > 0)
         aFiles.Add(csOneFile);
      else
         hr = E_INVALIDARG;
   }
    _tprintf(_T("!!!Upl inainte de  StartYtUpload, hr = %d\n"), hr);
   if (SUCCEEDED(hr))
   {
      if (s_pUploader != NULL)
      {
          hr = s_pUploader->StartYtUpload(csUser, csPass, csTitle, csDescription, csKeywords, csCategory, iPrivacy, aFiles);
          _tprintf(_T("!!!Upl dupa  StartYtUpload, hr = %d\n"), hr);
      }
      else
         hr = E_UNEXPECTED;
   }

   return (jint)hr;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_StartPodcast
  (JNIEnv *env, jclass, jstring jsChannelUrl, jstring jsMediaUrl, jstring jsTitle, jstring jsAuthor,
  jobject joSettingsChannel, jobject joSettingsMedia, jobjectArray jaFiles)
{
   HRESULT hr = S_OK;

   if (s_pUploader == NULL)
      s_pUploader = new CUploader();

   CString csChannelUrl;
   CString csMediaUrl;
   CString csChannelServer;
   int iChannelProtocol = 0;
   int iChannelPort = 0;
   CString csChannelUser;
   CString csChannelPass;
   CString csChannelDir;
   CString csMediaServer;
   int iMediaProtocol = 0;
   int iMediaPort = 0;
   CString csMediaUser;
   CString csMediaPass;
   CString csMediaDir;
   CString csTitle;
   CString csAuthor;

   if (SUCCEEDED(hr))
   {
      _TCHAR *tszChannelUrl = JNISupport::GetTCharFromString(env, jsChannelUrl);
      csChannelUrl = tszChannelUrl;
      if (tszChannelUrl != NULL)
         delete[] tszChannelUrl;

      _TCHAR *tszMediaUrl = JNISupport::GetTCharFromString(env, jsMediaUrl);
      csMediaUrl = tszMediaUrl;
      if (tszMediaUrl != NULL)
         delete[] tszMediaUrl;

      _TCHAR *tszTitle = JNISupport::GetTCharFromString(env, jsTitle);
      csTitle = tszTitle;
      if (tszTitle != NULL)
         delete[] tszTitle;

      _TCHAR *tszAuthor = JNISupport::GetTCharFromString(env, jsAuthor);
      csAuthor = tszAuthor;
      if (tszAuthor != NULL)
         delete[] tszAuthor;
   }

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsChannel, "m_strServer", csChannelServer);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joSettingsChannel, "m_iProtocol", iChannelProtocol);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joSettingsChannel, "m_iPort", iChannelPort);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsChannel, "m_strUser", csChannelUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsChannel, "m_strPass", csChannelPass);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsChannel, "m_strDir", csChannelDir);

   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsMedia, "m_strServer", csMediaServer);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joSettingsMedia, "m_iProtocol", iMediaProtocol);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, joSettingsMedia, "m_iPort", iMediaPort);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsMedia, "m_strUser", csMediaUser);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsMedia, "m_strPass", csMediaPass);
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, joSettingsMedia, "m_strDir", csMediaDir);


   CArray<CString, CString> aFiles;
   int iFileCount = env->GetArrayLength(jaFiles);
   for (int i=0; i<iFileCount && SUCCEEDED(hr); ++i)
   {
      jobject joOneFile = env->GetObjectArrayElement(jaFiles, i);
      
      CString csOneFile;
      _TCHAR *tszOneFile = JNISupport::GetTCharFromString(env, (jstring)joOneFile);
      csOneFile = tszOneFile;

      _tprintf(tszOneFile);

      if (tszOneFile != NULL)
         delete[] tszOneFile;
      if (csOneFile.GetLength() > 0)
         aFiles.Add(csOneFile);
      else
         hr = E_INVALIDARG;
   }

   if (SUCCEEDED(hr))
   {
      hr = s_pUploader->StartPodcast(
         csChannelServer, iChannelProtocol, iChannelPort, csChannelUser, csChannelPass, csChannelDir,
         csMediaServer, iMediaProtocol, iMediaPort, csMediaUser, csMediaPass, csMediaDir, 
         csMediaUrl, csChannelUrl, aFiles, csTitle, csAuthor); // ??? reversed order of Urls?
   }

   return (jint)hr;
}

JNIEXPORT jdouble JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_GetUploadProgress
(JNIEnv *env, jclass)
{
   double dProgress = 0;

   if (s_pUploader != NULL)
   {
      HRESULT hr = s_pUploader->GetProgress(&dProgress);
      if (SUCCEEDED(hr))
         return (jdouble)dProgress;
   }

   return (jdouble)0;
}

JNIEXPORT jstring JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_GetLastError
  (JNIEnv *env, jclass)
{
   if (s_pUploader != NULL)
   {
      CString csErrorMsg;
      s_pUploader->GetErrorString(csErrorMsg);

      jstring jsErrorMsg = JNISupport::GetStringFromTChar(env, (LPCTSTR)csErrorMsg);

      return jsErrorMsg;
   }

   return NULL;
}

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_CancelSession
  (JNIEnv *env, jclass)
{
   if (s_pUploader != NULL)
   {
      s_pUploader->CancelSession();
   }
}

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_StartSession
  (JNIEnv *env, jclass)
{
   if (s_pUploader != NULL)
   {
      s_pUploader->StartSession();
   }
   else
   {
       s_bStartSessionCalled = true;
   }
}

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_wizard_UploaderAccess_FinishSession
  (JNIEnv *env, jclass)
{
   if (s_pUploader != NULL)
   {
      s_pUploader->FinishSession();
   }
}
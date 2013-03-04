#include "stdafx.h"
#include <io.h>

#include "LpLibs.h"
#include "lutils.h"
#include "JNISupport.h"

#include "PublisherProfile.h"
#include "ProfileManager.h"

#include "MfcUtils.h"

ProfileInfo::ProfileInfo()
{
   m_iProfileID = 0;
   m_iProfileVersion = 0;
   m_iProfileType = -1;

   m_csFilename = "";
   m_csTitle = "";
   m_csTargetFormat = "";
   m_csStorageDistribution = "";

   m_bActivated = false;

   m_bCreatedByAdmin = false;
}

ProfileInfo::ProfileInfo(CString &csFilename)
{
   m_iProfileID = 0;
   m_iProfileVersion = 0;
   m_iProfileType = -1;

   m_csFilename = csFilename;
   m_csTitle = "";
   m_csTargetFormat = "";
   m_csStorageDistribution = "";

   m_bActivated = false;

   m_bCreatedByAdmin = false;

}

ProfileInfo::ProfileInfo(ProfileInfo *pProfileInfo)
{
   m_iProfileID = pProfileInfo->GetID();
   m_iProfileVersion = pProfileInfo->GetVersion();
   m_iProfileType = pProfileInfo->GetType();

   m_csFilename = pProfileInfo->GetFilename();
   m_csTitle = pProfileInfo->GetTitle();
   m_csTargetFormat = pProfileInfo->GetTargetFormat();
   m_csStorageDistribution = pProfileInfo->GetStorageDistribution();

   m_bActivated = pProfileInfo->IsActivated();

   m_bCreatedByAdmin = pProfileInfo->IsCreatedByAdmin();

   pProfileInfo->GetKeysAndValues(m_aKeys, m_aValues);
}

ProfileInfo::ProfileInfo(CStringArray &aKeys, CStringArray &aValues)
{
   m_iProfileID = 0;
   m_iProfileVersion = 0;
   m_iProfileType = -1;

   m_csFilename = "";
   m_csTitle = "";
   m_csTargetFormat = "";
   m_csStorageDistribution = "";

   m_bActivated = false;

   m_aKeys.Append(aKeys);
   m_aValues.Append(aValues);

   ExtractTargetFormat();
   ExtractStorageDistribution();
}

ProfileInfo::~ProfileInfo(void)
{
   m_aKeys.RemoveAll();
   m_aValues.RemoveAll();
}

void ProfileInfo::GetKeysAndValues(CStringArray &aKeys, CStringArray &aValues)
{
   for (int i = 0; i < m_aKeys.GetCount(); ++i)
   {
      aKeys.Add(m_aKeys[i]);
      aValues.Add(m_aValues[i]);
   }
}

void ProfileInfo::SetTitle(CString &csTitle) 
{
   m_csTitle = csTitle;

   for (int i = 0; i < m_aKeys.GetSize(); ++i)
   {
      if (m_aKeys[i] == _T("strTitle"))
      {
          m_aValues[i] = m_csTitle;
      }
   }
}

HRESULT ProfileInfo::Write(LPCTSTR szFilename)
{
   HRESULT hr = S_OK;

   if (szFilename == NULL && m_csFilename.IsEmpty())
   {
      HKEY hKey = HKEY_LOCAL_MACHINE;
      if (!PublisherProfile::UserIsAdministrator())
         hKey = HKEY_CURRENT_USER;

      CString csProfileDirectory;
      hr = PublisherProfile::GetProfileDirectory(hKey, csProfileDirectory, true);

      if (SUCCEEDED(hr))
         m_csFilename.Format(_T("%s\\Profile_%d_%u.lpp"), csProfileDirectory, m_iProfileType, m_iProfileID);
   }

   CString csFilename = m_csFilename;
   if (SUCCEEDED(hr) && szFilename != NULL)
   {
      csFilename = szFilename;
   }

   LURESULT lr = S_OK;
   LFile *pProfileFile = NULL;
   if (SUCCEEDED(hr))
   {
      pProfileFile = new LFile(csFilename);
      if (pProfileFile == NULL)
         hr = E_PM_FILE_CREATE;
   }

   if (SUCCEEDED(hr))
   {
      lr = pProfileFile->Create(LFILE_TYPE_TEXT_UNICODE, TRUE);
      if (lr != S_OK)
         hr = E_PM_FILE_CREATE;
      if (!pProfileFile->Exists())
         hr = E_PM_FILE_NOTEXIST;
   }

   CString csLine;
   if (SUCCEEDED(hr))
   {
      csLine.Format(_T("lpp_version=%d\n"), m_iProfileVersion);
      lr = pProfileFile->WriteText(csLine, csLine.GetLength());
      if (lr != S_OK)
         hr = E_PM_FILE_WRITE;
   }

   if (SUCCEEDED(hr))
   {
      csLine.Format(_T("lpp_id=%d\n"), m_iProfileID);
      lr = pProfileFile->WriteText(csLine, csLine.GetLength());
      if (lr != S_OK)
         hr = E_PM_FILE_WRITE;
   }

   if (SUCCEEDED(hr))
   {

      csLine.Format(_T("lpp_title=%s\n"), m_csTitle);
      lr = pProfileFile->WriteText(csLine, csLine.GetLength());
      if (lr != S_OK)
         hr = E_PM_FILE_WRITE;
   }

   if (SUCCEEDED(hr))
   {

      csLine.Format(_T("lpp_type=%d\n"), m_iProfileType);
      lr = pProfileFile->WriteText(csLine, csLine.GetLength());
      if (lr != S_OK)
         hr = E_PM_FILE_WRITE;
   }

   if (SUCCEEDED(hr))
   {
      for (int i = 0; i < m_aValues.GetSize() && SUCCEEDED(hr); ++i)
      {
         csLine.Format(_T("%s=%s\n"), m_aKeys[i], m_aValues[i]);
         lr = pProfileFile->WriteText(csLine, csLine.GetLength());
         if (lr != S_OK)
            hr = E_PM_FILE_WRITE;
      }
   }

   if (pProfileFile)
   {
      pProfileFile->Close();
      delete pProfileFile;
   }

   return hr;
}

HRESULT ProfileInfo::Read()
{
   HRESULT hr = S_OK;

   LFile profileFile(m_csFilename);

   if (!profileFile.Exists())
      hr = E_PM_FILE_NOTEXIST;

   if (SUCCEEDED(hr))
   {
      LURESULT lr = profileFile.Open();
      if (lr == LFILE_ERR_OPEN || lr == LFILE_ERR_ALREADY_OPEN)
         hr = E_PM_FILE_OPEN;
   }
   
   if (SUCCEEDED(hr))
   {
      LBuffer fileBuffer(256);

      DWORD dwBytesRead;
      CStringW csLine;
      UINT uiLinesRead = 0;

      // Read BOM (first 2 bytes)
      LURESULT lr = profileFile.ReadRaw(&fileBuffer, 0, 2, &dwBytesRead);
      if (lr != S_OK)
         hr = E_PM_FILE_READ;
 

      bool bFirstBlock = true;
      if (SUCCEEDED(hr))
      {
         do
         {
            lr = profileFile.ReadRaw(&fileBuffer, 0, fileBuffer.GetSize(), &dwBytesRead);
            if (lr != S_OK)
               hr = E_PM_FILE_READ;

            if (SUCCEEDED(hr) && bFirstBlock)
            {
               WCHAR *pIdent = (WCHAR *)fileBuffer.GetBuffer();
               if (wcsncmp(pIdent, L"lpp_", 4) != 0)
                  hr = E_PM_WRONG_FORMAT;
               bFirstBlock = false;
            }
            
            if (SUCCEEDED(hr))
            {
               WCHAR *pBuffer = (WCHAR *)fileBuffer.GetBuffer();
               UINT dwCharsRead = dwBytesRead / sizeof(WCHAR);
               for (int i = 0; i <  dwCharsRead; ++i)
               {
                  if (pBuffer[i] == L'\n')
                  {
                     CString csKey;
                     CString csValue;
                     //Ignore lines which begins with % or #
                     if (csLine[0] != L'%' && csLine[0] != L'#')
                     {
                        int iBlankPos = csLine.Find(L'=');
                        if (iBlankPos >= 0)
                        {
   #ifdef _UNICODE
                           csKey = csLine.Left(iBlankPos);
                           csValue = csLine.Right(csLine.GetLength() - (iBlankPos+1));
   #else
                           CStringW csKeyW = csLine.Left(iBlankPos);
                           int nLen = csKeyW.GetLength();
                           char *szRet = new char[nLen + 1];
                           WideCharToMultiByte(CP_ACP, 0, csKeyW, -1, 
                              szRet, nLen + 1, NULL, NULL);
                           csKey = szRet;
                           delete szRet;

                           CStringW csValueW = csLine.Right(csLine.GetLength() - (iBlankPos+1)); 
                           nLen = csValueW.GetLength();
                           szRet = new char[nLen + 1];
                           WideCharToMultiByte(CP_ACP, 0, csValueW, -1, 
                              szRet, nLen + 1, NULL, NULL);
                           csValue = szRet;
                           delete szRet;
   #endif
                           if (csKey == _T("lpp_id"))
                           {
                              m_iProfileID = _ttol((LPCTSTR)csValue);
                           }
                           else if (csKey == _T("lpp_version"))
                           {
                              m_iProfileVersion = _ttoi((LPCTSTR)csValue);
                           }
                           else if (csKey == _T("lpp_title"))
                           {
                              m_csTitle = csValue;
                           }
                           else if (csKey == _T("lpp_type"))
                           {
                              m_iProfileType = _ttoi((LPCTSTR)csValue);
                           }
                           else
                           {
                              m_aKeys.Add(csKey);
                              m_aValues.Add(csValue);
                           }
                        }
                     }
                     csLine.Empty();
                  }
                  else
                  {
                     csLine += pBuffer[i];
                  }
               }
            }

         } while (dwBytesRead == fileBuffer.GetSize());
      }

      profileFile.Close();
   }
   
   ExtractTargetFormat();
   ExtractStorageDistribution();

   return hr;
}

HRESULT ProfileInfo::DeleteProfile()
{
   HRESULT hr = S_OK;

   if (m_csFilename.IsEmpty())
      hr = E_FAIL;

   BOOL bRet = FALSE;
   if (SUCCEEDED(hr))
   {
      bRet = ::DeleteFile(m_csFilename);
      if (bRet == 0)
         hr = E_FAIL;
   }

   return hr;
}

void ProfileInfo::ExtractTargetFormat()
{
   int iExportType = -1;
   for (int i = 0; i < m_aKeys.GetSize(); ++i)
   {
      if (m_aKeys[i] == _T("iExportType"))
         iExportType = _ttoi(m_aValues[i]);
   }

   m_csTargetFormat.Empty();

   switch (iExportType)
   {
   case EXPORT_TYPE_LECTURNITY:
      m_csTargetFormat.LoadString(IDS_EXPORT_LPD);
      break;
   case EXPORT_TYPE_REAL:
      m_csTargetFormat.LoadString(IDS_EXPORT_REAL);
      break;
   case EXPORT_TYPE_WMT:
      m_csTargetFormat.LoadString(IDS_EXPORT_WM);
      break;
   case EXPORT_TYPE_FLASH:
      m_csTargetFormat.LoadString(IDS_EXPORT_FLASH);
      break;
   case EXPORT_TYPE_VIDEO:
      m_csTargetFormat.LoadString(IDS_EXPORT_IPOD);
      break;
   }
}

void ProfileInfo::ExtractStorageDistribution()
{
   int iReplayType = -1;
   for (int i = 0; i < m_aKeys.GetSize(); ++i)
   {
      if (m_aKeys[i] == _T("iReplayType"))
         iReplayType = _ttoi(m_aValues[i]);
   }
   
   int iServerType = -1;
   for (int i = 0; i < m_aKeys.GetSize(); ++i)
   {
      if (m_aKeys[i] == _T("iServerType"))
         iServerType = _ttoi(m_aValues[i]);
   }
   
   int iTransferType = -1;
   for (int i = 0; i < m_aKeys.GetSize(); ++i)
   {
      if (m_aKeys[i] == _T("iTransferType"))
         iTransferType = _ttoi(m_aValues[i]);
   }

   switch (iReplayType)
   {
   case REPLAY_LOCAL:
      m_csStorageDistribution.LoadString(IDS_SD_LOCAL);
      break;
   case REPLAY_SERVER:
      switch (iServerType)
      {
      case SRV_FILE_SERVER:
         m_csStorageDistribution.LoadString(IDS_SD_FILE_SERVER);
         break;
      case SRV_WEB_SERVER:
         if (iTransferType == TRANSFER_NETWORK_DRIVE)
            m_csStorageDistribution.LoadString(IDS_SD_WEBSERVER);
         else
            m_csStorageDistribution.LoadString(IDS_SD_WEBSERVER_FT);
         break;
      case SRV_STREAMING_SERVER:
         if (iTransferType == TRANSFER_NETWORK_DRIVE)
            m_csStorageDistribution.LoadString(IDS_SD_WEB_STREAMING);
         else
            m_csStorageDistribution.LoadString(IDS_SD_WEB_STREAMING_FT);
         break;
      case SRV_PODCAST_SERVER:
         m_csStorageDistribution.LoadString(IDS_SD_PODCAST);
         break;
      case SRV_SLIDESTAR:
         m_csStorageDistribution.LoadString(IDS_SD_SLIDESTAR);
         break;
      case SRV_YOUTUBE:
          m_csStorageDistribution.LoadString(IDS_SD_YOUTUBE);
          break;
      }
      break;
   }
}

/************************************************************************/
/************************************************************************/
/************************************************************************/

PublisherProfile::PublisherProfile(void)
{
   m_pManageDialog = NULL;
   
   m_iLastError = S_OK;
}

PublisherProfile::~PublisherProfile(void)
{
   if (m_pManageDialog)
      delete m_pManageDialog;
}


UINT PublisherProfile::WriteProfileSettings(JNIEnv *env, jobject jProfileInfo, jobjectArray jaEntries)
{
   HRESULT hr = S_OK;
   m_iLastError = S_OK;

   bool bWriteWithoutManager = m_pManageDialog == NULL; // used to write default profile
   //if (m_pManageDialog == NULL)
   //{
   //   hr = E_FAIL;
   //}

   bool bWaitForNew = false;
   bool bWaitForEdit = false;
   if (SUCCEEDED(hr) && !bWriteWithoutManager)
   {
      bWaitForNew = m_pManageDialog->WaitForNew();
      bWaitForEdit = m_pManageDialog->WaitForEdit();
   }

   if (!bWaitForEdit && !bWaitForNew && !bWriteWithoutManager)
      hr = E_FAIL;

   CStringArray aKeys;
   CStringArray aValues;
   if (SUCCEEDED(hr))
      hr = GetStringsFromArray(env, jaEntries, aKeys, aValues);

   ProfileInfo *pInfo = NULL;
   if (SUCCEEDED(hr))
   {
      pInfo = new ProfileInfo(aKeys, aValues);
      if (pInfo == NULL)
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
   {
      hr = FillCProfileInfo(env, jProfileInfo, pInfo);
      if (FAILED(hr))
      {
         if (bWaitForNew)
            ShowErrorMessage(IDS_ERROR_JNI_INSERT_NEW);
         else if (bWaitForEdit)
            ShowErrorMessage(IDS_ERROR_JNI_INSERT_EDIT);
      }
   }

   if (SUCCEEDED(hr))
   {
      if(!bWriteWithoutManager)
         hr = m_pManageDialog->AppendOrEditProfile(pInfo);
      else {
         // used to write default profile
         bool bUserIsAdmin = PublisherProfile::UserIsAdministrator();
         pInfo->SetCreatedByAdmin(bUserIsAdmin);

         hr = pInfo->Write(); 
         if (FAILED(hr))
         {
            if (hr == E_PM_FILE_CREATE || hr == E_PM_FILE_NOTEXIST)
               ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_PERMISSION);
            else if (hr == E_PM_CREATE_SUBDIR)
               ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_CREATESUBDIR);
            else if (hr == E_PM_FILE_WRITE)
               ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_WRITE);
            else if (hr == E_PM_GET_APPLDIR)
               ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_APPLICATIONDIR);
         }
      }
   }

   m_iLastError = hr;

   if (pInfo)
      delete pInfo;

   if (FAILED(hr))
      return S_FALSE;
   else
      return S_OK;
}

jobjectArray PublisherProfile::ReadProfileSettings(JNIEnv *env, jobject jProfileInfo, bool bActivate)
{
   HRESULT hr = S_OK;
   m_iLastError = S_OK;

   ProfileInfo *pProfileInfoHere = NULL;
   jobjectArray jaProfileSettings = NULL;

   if (m_pManageDialog == NULL)
   {
      ProfileInfo tmpProfileInfo;
      if (SUCCEEDED(hr))
      {
         hr = FillCProfileInfo(env, jProfileInfo, &tmpProfileInfo);
      }

      // Find out profile file name
      CArray<ProfileInfo *, ProfileInfo *> aProfileInformation;
      if (SUCCEEDED(hr))
         hr = ReadProfiles(aProfileInformation);
     
      int iProfileIndex = -1;
      if (SUCCEEDED(hr))
      {
         bool bProfileFound = false;
         for (int i = 0; i < aProfileInformation.GetSize() && !bProfileFound; ++i)
         {
            if (tmpProfileInfo.GetID() == aProfileInformation[i]->GetID())
            {
               iProfileIndex = i;
               bProfileFound = true;
               
               if(bActivate)
               {
                  // Save active profile in registry
                  bool bRet = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
                     _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles"),
                     _T("Active"), aProfileInformation[i]->GetFilename());
               }
            }
         }
         if (iProfileIndex < 0)
            hr = E_FAIL;
      }


      if (SUCCEEDED(hr))
         pProfileInfoHere = aProfileInformation[iProfileIndex];
   }
   else
   {
      if (SUCCEEDED(hr))
         hr = m_pManageDialog->GetSelectedProfile(&pProfileInfoHere);
   }

   if (SUCCEEDED(hr))
   {
      hr = FillJavaProfileInfo(env, jProfileInfo, pProfileInfoHere);
   }

   if (SUCCEEDED(hr))
   {
      CStringArray aKeys;
      CStringArray aValues;
      pProfileInfoHere->GetKeysAndValues(aKeys, aValues);
      jaProfileSettings = SetStringsToArray(env, aKeys, aValues);
   }

   m_iLastError = hr;

   return jaProfileSettings;
}

jobject PublisherProfile::ManageProfiles(JNIEnv *env, jobject joPanel, jstring jsLrdFile)
{
   HRESULT hr = S_OK;
   m_iLastError = S_OK;

   if (env == NULL)
      hr = E_FAIL;
   
   // Extract lrd file name
   CString csLrdFilename;
   if (SUCCEEDED(hr))
   {
      _TCHAR *tszLrd = JNISupport::GetTCharFromString(env, jsLrdFile);
      csLrdFilename = tszLrd;
      if (tszLrd)
         delete[] tszLrd;
   }

   // Read all available profiles
   CArray<ProfileInfo *, ProfileInfo *> aProfileInformation;
   if (SUCCEEDED(hr))
   {
      hr = ReadProfiles(aProfileInformation);
      if (FAILED(hr))
         ShowErrorMessage(IDS_ERROR_ACTION_ACCESS, IDS_ERROR_APPLICATIONDIR);
   }

   // Get active profile
   _TCHAR tszActiveProfile[MAX_PATH];
   if (SUCCEEDED(hr))
   {
      unsigned long lLength = MAX_PATH;
      bool bRet = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
                        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles"), 
                        _T("Active"), tszActiveProfile, &lLength);
      
      if (bRet == FALSE)
      {
         tszActiveProfile[0] = _T('\0');
         //ShowErrorMessage(IDS_ERROR_READ_ACTIVEPROFILE);
         //hr = E_FAIL;
      }
      
   }

   if (SUCCEEDED(hr))
   {
      bool bProfileActivated = false;
      for (int i = 0; i < aProfileInformation.GetCount() && !bProfileActivated; ++i)
      {
         if (aProfileInformation[i]->GetFilename() == tszActiveProfile)
         {
            aProfileInformation[i]->SetActivated(true);
            bProfileActivated = true;
         }
      }
   }

   HWND hWnd = NULL;
   if (SUCCEEDED(hr))
   {
      hWnd = JNISupport::GetHwndFromJavaWindow(env, joPanel);
      if (hWnd == NULL)
      {
         ShowErrorMessage(IDS_ERROR_JNI_MAINWINDOW);
         hr = E_FAIL;
      }
   }

   jobject joActivated = NULL;
   if (SUCCEEDED(hr))
   {
      CWnd *pWnd = CWnd::FromHandle(hWnd);
      m_pManageDialog = new CProfileManager(env, aProfileInformation, csLrdFilename, pWnd);

      INT_PTR ret = m_pManageDialog->DoModal();

      if (ret == IDOK)
      {
         CArray<ProfileInfo *, ProfileInfo *> aProfileInformation;
         m_pManageDialog->GetProfileInformation(aProfileInformation);

         // Delete active profile entry
         bool bRet = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
            _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles"),
            _T("Active"), _T(""));

         for (int i = 0; i < aProfileInformation.GetCount() && SUCCEEDED(hr); ++i)
         {
            if (aProfileInformation[i]->IsActivated())
            {
               // Save active profile in registry
               bool bRet = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
                  _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles"),
                  _T("Active"), aProfileInformation[i]->GetFilename());

               if (bRet == false)
                  hr = E_FAIL;

               if (SUCCEEDED(hr))
               {
                  // remove selected format (used for direct publishing) from registry
                  // direct publishing will now use active profile
                  LRegistry::WriteDwordRegistryEntry(HKEY_CURRENT_USER,
                     _T("Software\\imc AG\\LECTURNITY\\Publisher"), _T("DirectPublishFormat"), PublishingFormat::TYPE_NOTHING);

                  // Fill java profile info class
                  jclass jclProfileInfo = 
                     env->FindClass("imc/lecturnity/converter/ProfiledSettings$ProfileInfo");
                  if (jclProfileInfo != 0)
                  {
                     jmethodID jmInit = env->GetMethodID(jclProfileInfo, "Init", "()V");
                     if (jmInit != 0)
                     {
                        joActivated = env->NewObject(jclProfileInfo, jmInit);
                        hr = FillJavaProfileInfo(env, joActivated, aProfileInformation[i]);
                     }
                  }
               }
               else
                  hr = E_FAIL;
            }
         }
         if (FAILED(hr))
            ShowErrorMessage(IDS_ERROR_WRITE_ACTIVEPROFILE);
      }
   }

   delete m_pManageDialog;
   m_pManageDialog = NULL;

   m_iLastError = hr;

   return joActivated;
}

jobject PublisherProfile::GetActiveProfile(JNIEnv *env)
{
   HRESULT hr = S_OK;
   m_iLastError = S_OK;

   jobject joActivated = NULL;

   // Get active profile from registry
   _TCHAR tszActiveProfile[MAX_PATH];
   unsigned long lLength = MAX_PATH;
   bool bRet = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
                     _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles"), 
                     _T("Active"), tszActiveProfile, &lLength);

   if (bRet == false)
   {
      tszActiveProfile[0] = _T('\0');
      //ShowErrorMessage(IDS_ERROR_READ_ACTIVEPROFILE);
      //hr = E_FAIL;
   }

   CString csActiveProfile;
   if (SUCCEEDED(hr))
   {
      csActiveProfile = tszActiveProfile;
      if (csActiveProfile.IsEmpty())
         hr = E_FAIL;
   }

   // Fill java profile info class
   ProfileInfo *pInfo = NULL;
   if (SUCCEEDED(hr))
   {
      pInfo = new ProfileInfo(csActiveProfile);
      if (pInfo == NULL)
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
      hr = pInfo->Read();

   jclass jclProfileInfo = NULL;
   if (SUCCEEDED(hr))
   {
      jclProfileInfo = 
         env->FindClass("imc/lecturnity/converter/ProfiledSettings$ProfileInfo");
      if (jclProfileInfo == 0)
         hr = E_FAIL;
   }

   jmethodID jmInit = NULL;
   if (SUCCEEDED(hr))
   {
      jmInit = env->GetMethodID(jclProfileInfo, "Init", "()V");
      if (jmInit == 0)
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
   {
      joActivated = env->NewObject(jclProfileInfo, jmInit);
      hr = FillJavaProfileInfo(env, joActivated, pInfo);
   }

   if (pInfo)
      delete pInfo;

   m_iLastError = hr;

   return joActivated;
}

void PublisherProfile::CancelAppendOrEdit()
{
   HRESULT hr = S_OK;
   m_iLastError = S_OK;

   if (m_pManageDialog == NULL)
      hr = E_FAIL;

   if (SUCCEEDED(hr))
      hr = m_pManageDialog->CancelAppendOrEdit();

   m_iLastError = hr;
}

HRESULT PublisherProfile::GetStringsFromArray(JNIEnv *env, jobjectArray jEntryArray, CStringArray &aKeys, CStringArray &aValues)
{
   HRESULT hr = S_OK;

   int iSize = env->GetArrayLength(jEntryArray);

   for (int i = 0; i < iSize && SUCCEEDED(hr); i++) 
   {
      jobject jEntry = (jobject)env->GetObjectArrayElement(jEntryArray, i);
      if (jEntry == NULL)
         hr = E_PM_JNI_FAILED;

      if (SUCCEEDED(hr))
      {
         CString csKey;
         JNISupport::GetStringFromObject(env, jEntry, "m_strKey", csKey);
         aKeys.Add(csKey);
      }

      if (SUCCEEDED(hr))
      {
         CString csValue;
         JNISupport::GetStringFromObject(env, jEntry, "m_strValue", csValue);
         aValues.Add(csValue);
      }
   }

   return hr;
}

jobjectArray PublisherProfile::SetStringsToArray(JNIEnv *env, CStringArray &aKeys, CStringArray &aValues)
{
   HRESULT hr = S_OK;

   jclass jclProfileEntry = env->FindClass("imc/lecturnity/converter/ProfiledSettings$ProfileEntry");
   if (jclProfileEntry == NULL)
      hr = E_PM_JNI_FAILED;


   jmethodID setInitmethod = 0;
   if (SUCCEEDED(hr))
   {
      setInitmethod = env->GetMethodID(jclProfileEntry, "Init", "()V");
      if (setInitmethod == 0)
         hr = E_PM_JNI_FAILED;
   }

   jobjectArray jEntries = NULL;
   if (SUCCEEDED(hr))
   {
      jEntries = (jobjectArray)env->NewObjectArray(aKeys.GetSize(),
                                                   jclProfileEntry,
                                                   0);
      if (jEntries == NULL)
         hr = E_PM_JNI_FAILED;
   }

   if (SUCCEEDED(hr))
   {

      for (int i = 0; i < aKeys.GetSize() && SUCCEEDED(hr); i++) 
      {
         jobject jNewObject = env->NewObject(jclProfileEntry, setInitmethod);
         if (jNewObject == 0)
            hr = E_PM_JNI_FAILED;

         if (SUCCEEDED(hr))
         {
            JNISupport::SetStringToObject(env, aKeys[i], "m_strKey", jNewObject); 
            JNISupport::SetStringToObject(env, aValues[i], "m_strValue", jNewObject); 

            env->SetObjectArrayElement(jEntries, i, jNewObject);
         }
      }
   }

   if (FAILED(hr))
      _tprintf(_T("PublisherProfiles: Filling return array failed: %x\n"), hr);

   if (SUCCEEDED(hr))
      return jEntries; 
   else
      return NULL;
}
HRESULT PublisherProfile::GetApplicationDataDirectory(HKEY hMainKey, CString &csApplicationData)
{
   HRESULT hr = S_OK;

   _TCHAR szFolderPath[MAX_PATH];

   BOOL bRet = TRUE;
   if (hMainKey == HKEY_LOCAL_MACHINE)
      bRet = SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_COMMON_APPDATA , TRUE);
   else
      bRet = SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_APPDATA, TRUE);

   csApplicationData = szFolderPath;

   if (bRet == FALSE)
      hr = E_PM_GET_APPLDIR;

   return hr;
}

HRESULT PublisherProfile::AddProfileSubDirectory(CString &csApplicationDataDirectory, CString &csProfileDirectory, bool bCreate)
{
   HRESULT hr = S_OK;
   bool bCreateSuccess = false;

   if (SUCCEEDED(hr))
   {
      csProfileDirectory = csApplicationDataDirectory;
      if (bCreate && !DirectoryExists(csProfileDirectory, _T("Lecturnity")))
      {
         bCreateSuccess = LIo::CreateDirs(csProfileDirectory, _T("Lecturnity"));
         if (!bCreateSuccess)
            hr = E_PM_CREATE_SUBDIR;
      }
   }

   if (SUCCEEDED(hr))
   {
      csProfileDirectory += _T("\\Lecturnity");
      if (bCreate && !DirectoryExists(csProfileDirectory, _T("Publisher")))
      {
         bCreateSuccess = LIo::CreateDirs(csProfileDirectory, _T("Publisher"));
         if (!bCreateSuccess)
            hr = E_PM_CREATE_SUBDIR;
      }
   }

   if (SUCCEEDED(hr))
   {
      csProfileDirectory += _T("\\Publisher");
      if (bCreate && !DirectoryExists(csProfileDirectory, _T("Profiles")))
      {
         bCreateSuccess = LIo::CreateDirs(csProfileDirectory, _T("Profiles"));
         if (!bCreateSuccess)
            hr = E_PM_CREATE_SUBDIR;
      }
   }

   if (SUCCEEDED(hr))
   {
      csProfileDirectory += _T("\\Profiles");
   }

   return hr;
}

HRESULT PublisherProfile::GetProfileDirectory(HKEY hKey, CString &csProfileDirectory, bool bCreate)
{
   HRESULT hr = S_OK;

   CString csApplicationData;

   hr = GetApplicationDataDirectory(hKey, csApplicationData);
  
   if (SUCCEEDED(hr))
      hr = AddProfileSubDirectory(csApplicationData, csProfileDirectory, bCreate);

   return hr;
}

HRESULT PublisherProfile::ReadProfiles(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation)
{
   HRESULT hr = S_OK;

   // Search for profiles in All Users application directory
   CString csAllUserApplicationData;
   hr = GetProfileDirectory(HKEY_LOCAL_MACHINE, csAllUserApplicationData, false);

   if (SUCCEEDED(hr))
   {
      CString csAllUserApplicationDataFiles = csAllUserApplicationData + _T("\\*.lpp");
  
      WIN32_FIND_DATA ffd;
      HANDLE hFile = FindFirstFile((LPCTSTR)csAllUserApplicationDataFiles, &ffd);

      if (hFile != INVALID_HANDLE_VALUE)
      {             
         do 
         {
            CString csFile = csAllUserApplicationData + _T("\\") + ffd.cFileName;
            ProfileInfo *pNewProfileInfo = new ProfileInfo(csFile);
            pNewProfileInfo->Read();
            pNewProfileInfo->SetCreatedByAdmin(true);
            aProfileInformation.Add(pNewProfileInfo);
         } while (FindNextFile(hFile, &ffd));
      } 
      FindClose(hFile);
   }

   // Search for profiles in user application directory
   CString csUserApplicationData;
   if (SUCCEEDED(hr))
   {
      hr = GetProfileDirectory(HKEY_CURRENT_USER, csUserApplicationData, false);
   }

   
   if (SUCCEEDED(hr))
   {
      CString csUserApplicationDataFiles = csUserApplicationData + _T("\\*.lpp");

      WIN32_FIND_DATA ffd;
      HANDLE hFile = FindFirstFile((LPCTSTR)csUserApplicationDataFiles, &ffd);

      if (hFile != INVALID_HANDLE_VALUE)
      {  
         do 
         {
            CString csFile = csUserApplicationData + _T("\\") + ffd.cFileName;
            ProfileInfo *pNewProfileInfo = new ProfileInfo(csFile);
            pNewProfileInfo->Read();
            pNewProfileInfo->SetCreatedByAdmin(false);
            aProfileInformation.Add(pNewProfileInfo);
         } while (FindNextFile(hFile, &ffd));
      } 

      FindClose(hFile);
   }

   return hr;
}

HRESULT PublisherProfile::FillJavaProfileInfo(JNIEnv *env, jobject jProfileInfo, ProfileInfo *pProfileInfo)
{
   HRESULT hr = S_OK;

   char *szUtf8 = NULL;
   jclass jclProfileInfo = NULL;
   if (SUCCEEDED(hr))
   {
      jclProfileInfo = env->FindClass("imc/lecturnity/converter/ProfiledSettings$ProfileInfo");
      if (jclProfileInfo == 0)
         hr = E_PM_JNI_FAILED;
   }

   // ID
   jfieldID fidProfileID = 0;
   if (SUCCEEDED(hr))
   {
      fidProfileID = env->GetFieldID(jclProfileInfo, "m_id", "J");
      if (fidProfileID == 0)
         hr = E_PM_JNI_FAILED;
   }
      
   if (SUCCEEDED(hr))
   {
      env->SetLongField(jProfileInfo, fidProfileID, pProfileInfo->GetID());
   }

   // Format version
   if (SUCCEEDED(hr))
   {
      JNISupport::SetIntToObject(env, pProfileInfo->GetVersion(), "m_iFormatVersion", jProfileInfo);
   }
   
   // Title
   if (SUCCEEDED(hr))
   {
      JNISupport::SetStringToObject(env, pProfileInfo->GetTitle(), "m_strTitle", jProfileInfo);
   }

   // Profile document type
   if (SUCCEEDED(hr))
   {
      JNISupport::SetIntToObject(env, pProfileInfo->GetType(), "m_iDocumentType", jProfileInfo);
   }

   return hr;
}

HRESULT PublisherProfile::FillCProfileInfo(JNIEnv *env, jobject jProfileInfo, ProfileInfo *pProfileInfo)
{
   HRESULT hr = S_OK;

   _TCHAR *tszValue = NULL;

   jclass jclProfileInfo = env->FindClass("imc/lecturnity/converter/ProfiledSettings$ProfileInfo");
   if (jclProfileInfo == 0)
      hr = E_FAIL;

   long iProfileID = 0;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetLongFromObject(env, jProfileInfo, "m_id", iProfileID);
   if (SUCCEEDED(hr))
      pProfileInfo->SetID(iProfileID);

   int iProfileVersion = 0;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, jProfileInfo, "m_iFormatVersion", iProfileVersion);
   if (SUCCEEDED(hr))
      pProfileInfo->SetVersion(iProfileVersion);

   CString csTitle;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetStringFromObject(env, jProfileInfo, "m_strTitle", csTitle);
   if (SUCCEEDED(hr))
      pProfileInfo->SetTitle(csTitle);

   int iProfileType = 0;
   if (SUCCEEDED(hr))
      hr = JNISupport::GetIntFromObject(env, jProfileInfo, "m_iDocumentType", iProfileType);
   if (SUCCEEDED(hr))
      pProfileInfo->SetType(iProfileType);

   return hr;
}

bool PublisherProfile::UserIsAdministrator()
{
   BOOL bIsUserAdmin = ::IsUserAnAdmin();

   bool bRet = bIsUserAdmin ? true : false;

   /*

   CString csAllUserDirectory;
   HRESULT hr = GetProfileDirectory(HKEY_LOCAL_MACHINE, csAllUserDirectory);

   if (SUCCEEDED(hr))
   {
#ifdef _UNICODE
      if (_waccess((LPCTSTR)csAllUserDirectory, 06) == 0)
#else
      if (_access((LPCSTR)csAllUserDirectory, 06) == 0)
#endif
         bRet = true;
   }
*/
   return bRet;
}

bool PublisherProfile::DirectoryExists(const _TCHAR *tszBaseDirectory, const _TCHAR *tszSubDirectory)
{
   bool bRet = false;

   CString csDirectory = tszBaseDirectory;
   csDirectory += "\\";
   csDirectory += tszSubDirectory;

#ifdef _UNICODE
   if (_waccess((LPCTSTR)csDirectory, 06) == 0)
#else
   if (_access((LPCSTR)csDirectory, 06) == 0)
#endif
      bRet = true;

   return bRet;
}

void PublisherProfile::ShowErrorMessage(UINT iActionMessageID, UINT iErrorMessageID)
{
   CString csCaption;
   csCaption.LoadString(IDS_ERROR);

   CString csError;
   if (iErrorMessageID > 0)
      csError.LoadString(iErrorMessageID);

   CString csMessage;
   csMessage.LoadString(iActionMessageID);
   if (iErrorMessageID > 0)
      csMessage.Format(_T("%s\n\n%s"), csMessage, csError);
   MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
}

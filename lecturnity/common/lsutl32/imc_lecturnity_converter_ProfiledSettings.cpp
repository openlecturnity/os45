#include "stdafx.h"
#include "Resource.h"
#include "imc_lecturnity_converter_ProfiledSettings.h"
#include "ProfileUtils.h"
#include "ProfileInfo.h"
#include "JNISupport.h"
#include "LanguageSupport.h"
#include "MfcUtils.h"

#include "lutils.h"     //lutils

static int g_iLastError = S_OK;

HRESULT FillJavaProfileInfo(JNIEnv *env, jobject jProfileInfo, ProfileInfo *pProfileInfo) {
    HRESULT hr = S_OK;

    char *szUtf8 = NULL;
    jclass jclProfileInfo = NULL;
    if (SUCCEEDED(hr)) {
        jclProfileInfo = env->FindClass("imc/lecturnity/converter/ProfiledSettings$ProfileInfo");
        if (jclProfileInfo == 0)
            hr = E_PM_JNI_FAILED;
    }

    // ID
    jfieldID fidProfileID = 0;
    if (SUCCEEDED(hr)) {
        fidProfileID = env->GetFieldID(jclProfileInfo, "m_id", "J");
        if (fidProfileID == 0)
            hr = E_PM_JNI_FAILED;
    }

    if (SUCCEEDED(hr)) {
        env->SetLongField(jProfileInfo, fidProfileID, pProfileInfo->GetID());
    }

    // Format version
    if (SUCCEEDED(hr))
    {
        JNISupport::SetIntToObject(env, pProfileInfo->GetVersion(), "m_iFormatVersion", jProfileInfo);
    }

    // Title
    if (SUCCEEDED(hr)) {
        JNISupport::SetStringToObject(env, pProfileInfo->GetTitle(), "m_strTitle", jProfileInfo);
    }

    // Profile document type
    if (SUCCEEDED(hr)) {
        JNISupport::SetIntToObject(env, pProfileInfo->GetType(), "m_iDocumentType", jProfileInfo);
    }

    // File
    if (SUCCEEDED(hr)) {
        JNISupport::SetStringToObject(env, pProfileInfo->GetFilename(), "m_strFile", jProfileInfo);
    }

    return hr;
}

HRESULT FillCProfileInfo(JNIEnv *env, jobject jProfileInfo, ProfileInfo *pProfileInfo) {
    HRESULT hr = S_OK;

    _TCHAR *tszValue = NULL;

    jclass jclProfileInfo = env->FindClass("imc/lecturnity/converter/ProfiledSettings$ProfileInfo");
    if (jclProfileInfo == 0)
        hr = E_FAIL;

    __int64 iProfileID = 0;
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

    CString csFile;
    if (SUCCEEDED(hr))
        hr = JNISupport::GetStringFromObject(env, jProfileInfo, "m_strFile", csFile);
    if (SUCCEEDED(hr))
        pProfileInfo->SetFilename(csFile);

    return hr;
}

jobjectArray SetStringsToArray(JNIEnv *env, CStringArray &aKeys, CStringArray &aValues) {
    HRESULT hr = S_OK;

    jclass jclProfileEntry = env->FindClass("imc/lecturnity/converter/ProfiledSettings$ProfileEntry");
    if (jclProfileEntry == NULL)
        hr = E_PM_JNI_FAILED;


    jmethodID setInitmethod = 0;
    if (SUCCEEDED(hr)) {
        setInitmethod = env->GetMethodID(jclProfileEntry, "Init", "()V");
        if (setInitmethod == 0)
            hr = E_PM_JNI_FAILED;
    }

    jobjectArray jEntries = NULL;
    if (SUCCEEDED(hr)) {
        jEntries = (jobjectArray)env->NewObjectArray(aKeys.GetSize(),
                                                     jclProfileEntry, 0);
        if (jEntries == NULL)
            hr = E_PM_JNI_FAILED;
    }

    if (SUCCEEDED(hr)) {
        for (int i = 0; i < aKeys.GetSize() && SUCCEEDED(hr); i++) {
            jobject jNewObject = env->NewObject(jclProfileEntry, setInitmethod);
            if (jNewObject == 0)
                hr = E_PM_JNI_FAILED;

            if (SUCCEEDED(hr)) {
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

HRESULT GetStringsFromArray(JNIEnv *env, jobjectArray jEntryArray, CStringArray &aKeys, CStringArray &aValues) {
    HRESULT hr = S_OK;

    int iSize = env->GetArrayLength(jEntryArray);

    for (int i = 0; i < iSize && SUCCEEDED(hr); i++) {
        jobject jEntry = (jobject)env->GetObjectArrayElement(jEntryArray, i);
        if (jEntry == NULL)
            hr = E_PM_JNI_FAILED;

        if (SUCCEEDED(hr)) {
            CString csKey;
            JNISupport::GetStringFromObject(env, jEntry, "m_strKey", csKey);
            aKeys.Add(csKey);
        }

        if (SUCCEEDED(hr)) {
            CString csValue;
            JNISupport::GetStringFromObject(env, jEntry, "m_strValue", csValue);
            aValues.Add(csValue);
        }
    }

    return hr;
}

JNIEXPORT jobject JNICALL Java_imc_lecturnity_converter_ProfiledSettings_GetActiveProfile
(JNIEnv *env, jclass) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;
    g_iLastError = S_OK;

    CLanguageSupport::SetThreadLanguage();

    CString csActiveProfile;
    hr = ProfileUtils::ReadActiveProfile(csActiveProfile);

    // Fill java profile info class
    ProfileInfo *pInfo = NULL;
    if (SUCCEEDED(hr)) {
        pInfo = new ProfileInfo(csActiveProfile);
        if (pInfo == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr))
        hr = pInfo->Read();

    jclass jclProfileInfo = NULL;
    if (SUCCEEDED(hr)) { 
        jclProfileInfo = 
            env->FindClass("imc/lecturnity/converter/ProfiledSettings$ProfileInfo");
        if (jclProfileInfo == 0)
            hr = E_FAIL;
    }

    jmethodID jmInit = NULL;
    if (SUCCEEDED(hr)) {
        jmInit = env->GetMethodID(jclProfileInfo, "Init", "()V");
        if (jmInit == 0)
            hr = E_FAIL;
    }

    jobject joActivated = NULL;
    if (SUCCEEDED(hr)) {
        joActivated = env->NewObject(jclProfileInfo, jmInit);
        hr = FillJavaProfileInfo(env, joActivated, pInfo);
    }

    if (pInfo)
        delete pInfo;

    g_iLastError = hr;

    return joActivated;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_ProfiledSettings_WriteToProfile
(JNIEnv *env, jobject, jobject jProfileInfo, jobjectArray jaEntries) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CLanguageSupport::SetThreadLanguage();

    HRESULT hr = S_OK;
    g_iLastError = S_OK;

    CStringArray aKeys;
    CStringArray aValues;
    if (SUCCEEDED(hr))
        hr = GetStringsFromArray(env, jaEntries, aKeys, aValues);

    ProfileInfo *pInfo = NULL;
    if (SUCCEEDED(hr)) {
        pInfo = new ProfileInfo(aKeys, aValues);
        if (pInfo == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        hr = FillCProfileInfo(env, jProfileInfo, pInfo);
    }

    if (SUCCEEDED(hr)) {
        // used to write default profile
        bool bUserIsAdmin = LMisc::IsUserAdmin();
        pInfo->SetCreatedByAdmin(bUserIsAdmin);

        hr = pInfo->Write(); 
        if (FAILED(hr)) {
            if (hr == E_PM_FILE_CREATE || hr == E_PM_FILE_NOTEXIST)
                ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_PERMISSION);
            else if (hr == E_PM_CREATE_SUBDIR)
                ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_CREATESUBDIR);
            else if (hr == E_PM_FILE_WRITE)
                ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_WRITE);
            else if (hr == E_PM_GET_APPLDIR)
                ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_APPLICATIONDIR);
        }
    }

    g_iLastError = hr;

    __int64 iProfileID = 0;
    if (pInfo) {
        iProfileID = pInfo->GetID();
        delete pInfo;
    }

    LIo::MessagePipe pipe;
    HRESULT hrPipe = pipe.Init(_T("publisher_msg_pipe"), false);
    if (SUCCEEDED(hrPipe)) {
        _TCHAR tszMessage[256];
        _stprintf(tszMessage, 
            _T("Profile %I64d %s"), iProfileID, FAILED(hr) ? _T("CANCEL") : _T("SUCCESS"));

        pipe.WriteMessage(tszMessage);
    }

    return hr;
}

JNIEXPORT jobjectArray JNICALL Java_imc_lecturnity_converter_ProfiledSettings_ReadFromProfile(JNIEnv *env, jobject, jobject jProfileInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CLanguageSupport::SetThreadLanguage();

    HRESULT hr = S_OK;
    g_iLastError = S_OK;

    ProfileInfo *pProfileInfoHere = NULL;
    jobjectArray jaProfileSettings = NULL;

    ProfileInfo tmpProfileInfo;
    if (SUCCEEDED(hr)) {
        hr = FillCProfileInfo(env, jProfileInfo, &tmpProfileInfo);
    }
	
	// BUGFIX 5680: Live Context profile not found
    if (SUCCEEDED(hr) && !tmpProfileInfo.GetFilename().IsEmpty()) {
        // try to read from specified file, e.g. Live Context does store profiles in different folders
        HRESULT hresult = tmpProfileInfo.Read();
        if (SUCCEEDED(hresult)) {
           pProfileInfoHere = &tmpProfileInfo;
        }
    }

    if (!pProfileInfoHere) {
       // Find out profile file name
       CArray<ProfileInfo *, ProfileInfo *> aProfileInformation;
       if (SUCCEEDED(hr))
           hr = ProfileUtils::ReadProfiles(aProfileInformation);

       int iProfileIndex = -1;
       if (SUCCEEDED(hr)) {
           bool bProfileFound = false;
           for (int i = 0; i < aProfileInformation.GetSize() && !bProfileFound; ++i) {
               if (tmpProfileInfo.GetID() == aProfileInformation[i]->GetID()) {
                   iProfileIndex = i;
                   bProfileFound = true;
               }
           }
           if (iProfileIndex < 0)
               hr = E_FAIL;
       }


       if (SUCCEEDED(hr))
           pProfileInfoHere = aProfileInformation[iProfileIndex];
    }

    if (SUCCEEDED(hr)) {
        hr = FillJavaProfileInfo(env, jProfileInfo, pProfileInfoHere);
    }

    if (SUCCEEDED(hr)) {
        CStringArray aKeys;
        CStringArray aValues;
        pProfileInfoHere->GetKeysAndValues(aKeys, aValues);
        jaProfileSettings = SetStringsToArray(env, aKeys, aValues);
    }

    g_iLastError = hr;

    return jaProfileSettings;
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_ProfiledSettings_GetLastProfileErrorCode
(JNIEnv *, jobject) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

        CLanguageSupport::SetThreadLanguage();

    return g_iLastError;
}

JNIEXPORT jobject JNICALL Java_imc_lecturnity_converter_ProfiledSettings_ShowFormatDialog(JNIEnv *env, jclass jc, jobject joWindow, jstring jsLrdFile) {
    HWND hWndParent = JNISupport::GetHwndFromJavaWindow(env, joWindow);

    _TCHAR *tszLrdile = JNISupport::GetTCharFromString(env, jsLrdFile);

    jint iSelectedFormat = (jint)MfcUtils::ShowPublisherFormatSelection(hWndParent, tszLrdile);

    if (tszLrdile)
        delete[] tszLrdile;

    return Java_imc_lecturnity_converter_ProfiledSettings_GetActiveProfile(env, jc);
}

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_ProfiledSettings_SignalCancel(JNIEnv *env, jclass) {

    LIo::MessagePipe pipe;
    HRESULT hrPipe = pipe.Init(_T("publisher_msg_pipe"), false);
    if (SUCCEEDED(hrPipe)) {
        pipe.WriteMessage(_T("Profile 0 CANCEL"));
    }
}
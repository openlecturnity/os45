#include "StdAfx.h"
#include "MfcUtils.h"
#include "TemplateEngine.h"
#include "resource.h"

#include "TemplateDlg.h"

#include "lutils.h"
#include "SgmlParser.h" // File SDK
// For exporting 'standard navigation' to LPDs -> modify LRD
#include "qedit.h" // DXSDK
#include "SgmlParser.h" // File SDK
#include "LrdFile.h" // File SDK

/* ************************************************* */

CKeywordReplace::CKeywordReplace(const _TCHAR *tszKeyword, const _TCHAR *tszReplace)
{
   csKeyword = tszKeyword;
   csReplace = tszReplace;
}

/* ************************************************************* *
 * ************************************************************* */

CTemplateEngine::CTemplateEngine()
{
   m_bSaveToRegistry = true;
   m_csId = _T("");
   m_csTitle = _T("");
   m_csDescription = _T("");
   m_csProfileTemplatePath = _T("");
   m_csSourcePath = _T("");
   m_csCommonSourcePath = _T("");
   m_csAuthor = _T("");
   m_csCopyright = _T("");
   m_csPreviewImageFileName = _T("");
   m_bIsScormCompliant = false;
   m_bScormEnabled = false;
   m_bIsUtfTemplate = false;
   m_aiStandardNavigationStates = new int[NUMBER_OF_NAVIGATION_STATES];
   for (int i = 0; i < NUMBER_OF_NAVIGATION_STATES; ++i)
      m_aiStandardNavigationStates[i] = 0;
}

CTemplateEngine::~CTemplateEngine()
{
   CloseTemplate();

   delete[] m_aiStandardNavigationStates;
   m_aiStandardNavigationStates = NULL;
}

UINT CTemplateEngine::ReadSettings()
{
   _TCHAR tszLastTemplate[_MAX_PATH];
   tszLastTemplate[0] = 0;
   DWORD dwSize = _MAX_PATH;

   HRESULT hrLastTemplate = GetCurrentTemplatePath(tszLastTemplate, &dwSize);

   CString csLastTemplate(tszLastTemplate);

   if (hrLastTemplate != S_OK)
   {
      // No template specified. Use a default value.

      _TCHAR tszInstallDir[_MAX_PATH];
      DWORD dwSize = _MAX_PATH;
      bool success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
         _T("Software\\imc AG\\LECTURNITY"), _T("InstallDir"), tszInstallDir, &dwSize);

      if (success)
      {
         CString csInstallDir(tszInstallDir);
         CheckLastSlash(csInstallDir);
         CString csTemplateDir = csInstallDir + CString(_T("Publisher\\Web Templates\\WebPlayer1\\"));
         LRegistry::WriteSettingsString(_T("Publisher\\Templates"), _T("LastTemplate"),
            csTemplateDir);

         csLastTemplate = csTemplateDir;
      }
   }

   return ReadSettingsFromFile(csLastTemplate, false); // might still be "<undefined>" or empty
}

UINT CTemplateEngine::ReadSettingsFromFile(const _TCHAR *tszPath, bool bOverride)
{
   if (tszPath == NULL)
      return E_POINTER;

   if (_tcslen(tszPath) == 0)
      return E_INVALIDARG;

   m_csId = _T("");
   m_csTitle = _T("");
   m_csSourcePath = _T("");
   m_csAuthor = _T("");
   m_csDescription = _T("");
   m_csCopyright = _T("");
   m_bIsScormCompliant = false;
   m_bIsUtfTemplate = false;
   m_bHasTemplateChanged = false;
   m_aConfigKeywords.RemoveAll();

   CString csDummy;
   UINT res = LoadTemplate(tszPath, TEMPLATE_RESET_NORESET, false, csDummy);
   if (bOverride)
      OverrideCurrentPath(tszPath);
 
   if (S_OK != res)
   {
      // This is not good: No (default) template could be found.

      // Warn the user.
      CString csMsg;
      csMsg.LoadString(IDS_TMP_NOTEMPLATE);
      ::MessageBox(NULL, csMsg, _T("Publisher"), MB_OK | MB_ICONWARNING);

      // Pop up the configuration dialog. Perhaps that helps? Who knows.
      res = Configure(NULL, true);
   }

   return res;
}

UINT CTemplateEngine::WriteSettings(bool bForAllUsers, bool bSavePath)
{
   StoreKeywordConfiguration(bForAllUsers);

   if (bSavePath)
   {
      if (LMisc::IsUserAdmin() && LMisc::IsWindowsNT())
         LRegistry::WriteSettingsString(_T("Publisher\\Templates"), _T("LastTemplate"), m_csSourcePath, true);
      else
         LRegistry::WriteSettingsString(_T("Publisher\\Templates"), _T("LastTemplate"), m_csSourcePath);
   }

   return S_OK;
}


UINT CTemplateEngine::GetSettings(_TCHAR *tszSettings, DWORD *pdwSize)
{
#ifndef _UNICODE
   this will not compile; _UNICODE required;
#endif

   if (pdwSize == NULL)
      return E_POINTER;

   int iTotalSize = 0;

   for (int i=0; i<m_aConfigKeywords.GetSize(); ++i)
   {
      int iType = m_aConfigKeywords[i]->GetType();
      WCHAR *wszName = m_aConfigKeywords[i]->m_pConfig->wszVarName;

      CString csRepresentation = m_aConfigKeywords[i]->GetStringRepresentation();

      CString csReturn;
      csReturn.Format(_T("%d:%s:%d:%s"), iType, wszName, csRepresentation.GetLength(), csRepresentation);
      int iThisLength = csReturn.GetLength();

      if (tszSettings != NULL)
      {
         if (iTotalSize + 1 + iThisLength <= *pdwSize)
         {
            _tcsncpy(tszSettings + iTotalSize, csReturn, iThisLength);
            if (i != m_aConfigKeywords.GetSize() - 1)
               _tcsncpy(tszSettings + iTotalSize + iThisLength, _T(";"), 1);
         }
         else
            return E_UNEXPECTED; // unsufficient size
      }

      iTotalSize += iThisLength;

      if (i != m_aConfigKeywords.GetSize() - 1)
         ++iTotalSize; // separator ";"
   }

   if (tszSettings != NULL)
      tszSettings[iTotalSize] = 0;

   *pdwSize = iTotalSize + 1;

   return S_OK;
}

int GetNextNumberPositive(_TCHAR *tszInput, int& iAdvance)
{
   int MAX_NUM_LENGTH = 60; // some arbitrary boundary avoiding endless loops

   int iEndPos = -1;
   int iValue = 0;
  
   for (int i=0; i<MAX_NUM_LENGTH; ++i)
   {
      if (tszInput[i] >= _T('0') && tszInput[i] <= _T('9'))
      {
         if (i > 0)
            iValue *= 10;
         iValue += (int)tszInput[i] - (int)_T('0');

         iEndPos = i;
         ++iAdvance;
      }
      else
         break;
   }

   if (iEndPos > -1)
      return iValue;
   else
      return -1; // error
}

CString GetName(_TCHAR *tszInput, _TCHAR cSeparator, int& iAdvance)
{
   static TE_VAR t;
   int MAX_NAME_LENGTH = sizeof (t.wszVarName) / sizeof WCHAR;

   int iLength = 0;
   for (int i=0; i<MAX_NAME_LENGTH; ++i)
   {
      if (tszInput[i] != 0 && tszInput[i] != cSeparator)
         ++iLength;
      else
         break;
   }

   CString csReturn(tszInput, iLength);
   iAdvance += iLength;

   return csReturn;
}


UINT CTemplateEngine::SetSettings(const _TCHAR *tszSettings)
{
#ifndef _UNICODE
   this will not compile; _UNICODE required;
#endif

   if (tszSettings == NULL)
      return E_POINTER;

   if (_tcslen(tszSettings) == 0)
      return E_INVALIDARG;

   // for easier lookup in the loop below 
   CMap<CString, LPCTSTR, CTemplateKeyword *, CTemplateKeyword *> mapCurrentSettings;
   mapCurrentSettings.InitHashTable(m_aConfigKeywords.GetSize()*2+1);

   for (int i=0; i<m_aConfigKeywords.GetSize(); ++i)
   {
      WCHAR *wszName = m_aConfigKeywords[i]->m_pConfig->wszVarName;
      CString csName = wszName;

      mapCurrentSettings.SetAt(csName, m_aConfigKeywords[i]);
   }

   _TCHAR *tszInput = (_TCHAR *)tszSettings;

   while (tszInput[0] != 0)
   {
      int iAdvance = 0;
      int iType = GetNextNumberPositive(tszInput, iAdvance);
      if (iType < 0)
         return E_UNEXPECTED;
      tszInput += iAdvance + 1;
      iAdvance = 0;
      CString csName = GetName(tszInput, _T(':'), iAdvance);
      tszInput += iAdvance + 1;
      iAdvance = 0;
      int iContentLength = GetNextNumberPositive(tszInput, iAdvance);
      if (iContentLength < 0)
         return E_UNEXPECTED;
      tszInput += iAdvance + 1;
      CString csContent(tszInput, iContentLength);
      tszInput += iContentLength;

      if (tszInput[0] == _T(';'))
         tszInput += 1;

      // only set settings that exist in the template
      CTemplateKeyword *pValue = NULL;
      mapCurrentSettings.Lookup(csName, pValue);
      if (pValue != NULL)
      {
         int iThatType = pValue->GetType();

         if (iType != iThatType)
            return E_UNEXPECTED; // same name but different type; shouldn't happen

         bool bSuccess = pValue->ParseFromString(csContent); // replaces existing values
         if (!bSuccess)
            return E_UNEXPECTED;
      }
   }


   return S_OK;
}

UINT CTemplateEngine::GetCurrentTemplatePath(_TCHAR *tszPath, DWORD *pdwSize)
{
   if (pdwSize == NULL)
      return E_POINTER;

   _TCHAR *tszDefault = _T("<undefined>");
   CString csLastTemplate = tszDefault;

   if (m_csProfileTemplatePath.GetLength() <= 0)
   {
      _TCHAR tszLastTemplate[_MAX_PATH];
      DWORD dwRegSize = _MAX_PATH;
      LRegistry::ReadSettingsString(_T("Publisher\\Templates"), _T("LastTemplate"),
         tszLastTemplate, &dwRegSize, tszDefault);

      csLastTemplate = tszLastTemplate;
   }
   else
   {
      csLastTemplate = m_csProfileTemplatePath;
   }

   if (csLastTemplate == tszDefault) {
      // Bugfix 5178:
      // set an empty string (otherwise it is undefined and contains nonsense)
      if (tszPath != NULL)
         _tcscpy(tszPath, _T(""));
      return S_FALSE; // E_NO_TEMPLATE
   }

   int iLength = csLastTemplate.GetLength() + 1;

   if (tszPath != NULL && *pdwSize >= iLength)
      _tcsncpy(tszPath, (LPCTSTR)csLastTemplate, iLength);
   else
      *pdwSize = iLength;

   return S_OK;
}

UINT CTemplateEngine::Init()
{
   m_keywordList.RemoveAll();
   m_writtenFiles.RemoveAll();
   m_csDocType = _T("");
   m_csErrorMessage = _T("");
   m_csCommonSourcePath = _T("");
   m_csTargetPath = _T("");
   m_csTargetFormat = _T("");
   m_csIndexFileName = _T("");
   m_bIsTargetTemporary = false;
   m_bScormEnabled = false;
 // select the commonFiles folder
   _TCHAR tszInstallDir[_MAX_PATH];
   DWORD dwSize = _MAX_PATH;
   bool success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
      _T("Software\\imc AG\\LECTURNITY"), _T("InstallDir"),
      tszInstallDir, &dwSize);
   CString csInstallDir(tszInstallDir);
   if (success)
   {
      CheckLastSlash(csInstallDir);
      m_csCommonSourcePath = csInstallDir + CString(_T("Publisher\\CommonFiles\\"));   
   }

   return S_OK;
}

UINT CTemplateEngine::CleanUp()
{
   if (!m_bIsTargetTemporary)
      DeleteAllWrittenFiles();
   else
      LIo::DeleteDir(m_csTargetPath);
   return S_OK;
}

UINT CTemplateEngine::GetTitle(_TCHAR *tszName, DWORD *pdwSize)
{
   RETURN_STRING(m_csTitle, tszName, pdwSize);
}

UINT CTemplateEngine::GetAuthor(_TCHAR *tszAuthor, DWORD *pdwSize)
{
   RETURN_STRING(m_csAuthor, tszAuthor, pdwSize);
}

UINT CTemplateEngine::GetCopyright(_TCHAR *tszCopyright, DWORD *pdwSize)
{
   RETURN_STRING(m_csCopyright, tszCopyright, pdwSize);
}

UINT CTemplateEngine::GetDescription(_TCHAR *tszDesc, DWORD *pdwSize)
{
   RETURN_STRING(m_csDescription, tszDesc, pdwSize);
}

UINT CTemplateEngine::GetPreviewImage(_TCHAR *tszFileName, DWORD *pdwSize)
{
   RETURN_STRING((m_csSourcePath + m_csPreviewImageFileName), tszFileName, pdwSize);
}

UINT CTemplateEngine::IsScormCompliant(BOOL *pbCompliant)
{
   if (pbCompliant == NULL)
      return E_INVALIDARG;

   *pbCompliant = (m_bIsScormCompliant ? TRUE : FALSE);

   return S_OK;
}

UINT CTemplateEngine::IsUtfTemplate(BOOL *pbUtf)
{
   if (NULL == pbUtf)
      return E_INVALIDARG;

   *pbUtf = (m_bIsUtfTemplate ? TRUE : FALSE);

   return S_OK;
}


UINT CTemplateEngine::GetTargetDirectory(_TCHAR *tszDirectory, DWORD *pdwSize)
{
   RETURN_STRING(m_csTargetPath, tszDirectory, pdwSize);
}

UINT CTemplateEngine::CheckTemplate(CString csPath, CString& csTitle)
{
   return LoadTemplate(csPath, TEMPLATE_RESET_NORESET, true, csTitle);
}
   
UINT CTemplateEngine::LoadTemplate(const _TCHAR *tszTemplateDir, int nReset, bool bDry, CString& csTitleOut)
{
   if (m_aConfigKeywords.GetSize() > 0)
   {
      CString csCurrentDir;
      RETRIEVE_CSTRING(GetCurrentTemplatePath, csCurrentDir);

      if (csCurrentDir == tszTemplateDir)
      {
         csTitleOut = m_csTitle;

         return S_OK; // nothing to be done
      }
   }


   if (!bDry && m_pConfig)
      CloseTemplate();
      

   SgmlFile *pConfig = new SgmlFile;
   CString csSourcePath = tszTemplateDir;
   CheckLastSlash(csSourcePath);

   bool success = pConfig->Read(csSourcePath + _T("config.xml"), _T("<templateConfig"));

   SgmlElement *pRoot = NULL;
   if (success)
   {
      pRoot = pConfig->GetRoot();
      success = (pRoot != NULL);
   }
   CString csLanguage;
   csLanguage.LoadString(IDS_LANGUAGE);

   CString csTitle;
   CString csDescription;
   CString csAuthor;
   CString csCopyright;
   CString csPreviewImageFileName;

   // Read out some general information on the template
   if (success)
   {
      csTitle.LoadString(TP_UNDEFINED);
      const _TCHAR *tszTmp = pRoot->GetValue(CString(_T("title_")) + csLanguage);
      if (tszTmp)
         csTitle = tszTmp;
      csDescription.LoadString(TP_UNDEFINED);
      tszTmp = pRoot->GetValue(CString(_T("description_")) + csLanguage);
      if (tszTmp)
         csDescription = tszTmp;
      csAuthor.LoadString(TP_UNDEFINED);
      tszTmp = pRoot->GetValue(_T("author"));
      if (tszTmp)
         csAuthor = tszTmp;
      csCopyright.LoadString(TP_UNDEFINED);
      tszTmp = pRoot->GetValue(_T("copyright"));
      if (tszTmp)
         csCopyright = tszTmp;
      csPreviewImageFileName = _T("");
      tszTmp = pRoot->GetValue(_T("screenshot"));
      if (tszTmp)
         csPreviewImageFileName = tszTmp;
   }

   CString csId;

   SgmlElement *pId = NULL;
   bool bHasId = false;
   if (success)
   {
      pId = pRoot->Find(_T("id"));
      if (pId)
      {
         const _TCHAR *tszTmp = pId->GetParameter();
         if (tszTmp)
         {
            csId = tszTmp;
            bHasId = true;
         }
      }
   }

   bool bIsScormCompliant = false;

   SgmlElement *pScormCompliant = NULL;
   if (success)
   {
      pScormCompliant = pRoot->Find(_T("isScormCompliant"));
      if (pScormCompliant)
      {
         const _TCHAR *tszTmp = pScormCompliant->GetParameter();
         if (_tcsicmp(tszTmp, _T("true")) == 0)
            bIsScormCompliant = true;
      }
   }

   bool bIsUtfTemplate = false;

   SgmlElement *pUtf = NULL;
   if (success)
   {
      pUtf = pRoot->Find(_T("isUtfTemplate"));
      if (pUtf)
      {
         const _TCHAR *tszTmp = pUtf->GetParameter();
         if (_tcsicmp(tszTmp, _T("true")) == 0)
            bIsUtfTemplate = true;
      }
   }

   if (!bDry)
      m_csId = csId; // needed in ConfigureConfigurableKeywords()

   SgmlElement *pConfigVars = NULL;
   // A template needs an ID in order to save and restore
   // the template configuration to the registry.
   if (success && bHasId && !bDry)
   {
      pConfigVars = pRoot->Find(_T("configurableKeywords"));
      if (pConfigVars)
      {
         ReadConfigurableKeywords(pConfigVars);
         // Read settings from registry if we are not supposed to reset them
         ConfigureConfigurableKeywords(nReset);
      }
   }

   // Now for global configuration variables which may
   // be present in the template
   if (success && !bDry)
   {
      SgmlElement *pGlobalSettings = NULL;
      pGlobalSettings = pRoot->Find(_T("globalSettings"));
      if (pGlobalSettings)
      {
         ReadGlobalSettings(pGlobalSettings);
      }
   }

   // Try to find the formulas for the maximum slides
   // size. First define standard sizes.

   CString csMaxSlidesWidth;
   CString csMaxSlidesHeight;

   csMaxSlidesWidth = _T("%SlidesWidth% - 424"); // 1024x768: 600x450
   csMaxSlidesHeight = _T("%SlidesHeight% - 318");
   SgmlElement *pNormalDoc = NULL;
   if (success)
   {
      pNormalDoc = pRoot->Find(_T("normalDocument"));
      SgmlElement *pSizes = NULL;
      if (pNormalDoc)
      {
         pSizes = pNormalDoc->Find(_T("slidesSize"));
         SgmlElement *pWidth  = NULL;
         SgmlElement *pHeight = NULL;
         if (pSizes)
         {
            pWidth  = pSizes->Find(_T("width"));
            pHeight = pSizes->Find(_T("height"));
         }

         if (pWidth)
            csMaxSlidesWidth  = pWidth->GetParameter();
         if (pHeight)
            csMaxSlidesHeight = pHeight->GetParameter();
      }
   }

   // If we just reset a template, it may have changed
   bool bHasTemplateChanged = (nReset != TEMPLATE_RESET_NORESET);


   csTitleOut = csTitle;

   
   if (!bDry)
   {
      // Note: closing and deleting is done above
      m_pConfig = pConfig;

      m_csSourcePath = csSourcePath;

      m_csTitle = csTitle;
      m_csDescription = csDescription;
      m_csAuthor = csAuthor;
      m_csCopyright = csCopyright;
      m_csPreviewImageFileName = csPreviewImageFileName;
      m_bIsScormCompliant = bIsScormCompliant;
      m_bIsUtfTemplate = bIsUtfTemplate;
      m_csMaxSlidesWidth = csMaxSlidesWidth;
      m_csMaxSlidesHeight = csMaxSlidesHeight;

      m_bHasTemplateChanged = bHasTemplateChanged;

      OverrideCurrentPath(tszTemplateDir);
   }

   return success ? S_OK : E_FAIL;
}

void CTemplateEngine::ReadConfigurableKeywords(SgmlElement *pConfigVars) {
    CString csLanguage;
    csLanguage.LoadString(IDS_LANGUAGE);

    CString csFriendlyName;
    csFriendlyName.Format(_T("name_%s"), csLanguage);

    CString csDefaultContent;
    csDefaultContent.Format(_T("default_%s"), csLanguage);

    CArray<SgmlElement *, SgmlElement *> aConfigElements;
    pConfigVars->GetElements(aConfigElements);
    for (int i = 0; i < aConfigElements.GetCount(); ++i) {
        SgmlElement *pKeyword = aConfigElements[i];
        if  (pKeyword != NULL) {
            const _TCHAR *tszName = pKeyword->GetValue(_T("name"));
            const _TCHAR *tszFriendlyName = pKeyword->GetValue(csFriendlyName);
            if (tszFriendlyName == NULL)
                tszFriendlyName = tszName;
            const _TCHAR *tszType = pKeyword->GetValue(_T("type"));
            if (tszName && tszType) {
                if (_tcsicmp(tszType, _T("image")) == 0) {
                    int nMinWidth = pKeyword->GetIntValue(_T("minwidth"));
                    int nMinHeight = pKeyword->GetIntValue(_T("minheight"));
                    int nMaxWidth = pKeyword->GetIntValue(_T("maxwidth"));
                    int nMaxHeight = pKeyword->GetIntValue(_T("maxheight"));
                    CTemplateImageKeyword *pImgKeyword = new CTemplateImageKeyword
                        (tszName, tszFriendlyName, nMinWidth, nMinHeight,
                        nMaxWidth, nMaxHeight);
                    // No default possible here
                    m_aConfigKeywords.Add(pImgKeyword);
                } else if (_tcsicmp(tszType, _T("integer")) == 0) {
                    int nMinValue = pKeyword->GetIntValue(_T("minvalue"));
                    int nMaxValue = pKeyword->GetIntValue(_T("maxvalue"));
                    if (nMaxValue == 0)
                        nMaxValue = 100; // Default
                    const _TCHAR *tszTemp = pKeyword->GetParameter();
                    int nDefault = 0;
                    if (tszTemp)
                        nDefault = _ttoi(tszTemp);
                    CTemplateIntegerKeyword *pIntKeyword = new CTemplateIntegerKeyword
                        (tszName, tszFriendlyName, nMinValue, nMaxValue);
                    pIntKeyword->SetValue(nDefault);
                    m_aConfigKeywords.Add(pIntKeyword);
                } else if (_tcsicmp(tszType, _T("color")) == 0) {
                    const _TCHAR *tszTemp = pKeyword->GetParameter();
                    COLORREF rgbDefault = RGB(0, 0, 0); // Black is default
                    if (tszTemp) {
                        _TCHAR *tszTemp2 = _tcsdup(tszTemp);
                        int nLen = _tcslen(tszTemp2);
                        _tcsupr(tszTemp2);
                        if (nLen == 7)
                            tszTemp++; // skip '#'
                        if (nLen >= 6) {
                            BYTE r = (HEXTOINT(tszTemp[0]) << 4) | HEXTOINT(tszTemp[1]);
                            BYTE g = (HEXTOINT(tszTemp[2]) << 4) | HEXTOINT(tszTemp[3]);
                            BYTE b = (HEXTOINT(tszTemp[4]) << 4) | HEXTOINT(tszTemp[5]);
                            rgbDefault = RGB(r, g, b);
                        }
                        delete[] tszTemp2;
                    }
                    CTemplateColorKeyword *pColorKeyword = new CTemplateColorKeyword(tszName, tszFriendlyName);
                    pColorKeyword->SetColor(rgbDefault);
                    m_aConfigKeywords.Add(pColorKeyword);
                } else if (_tcsicmp(tszType, _T("general")) == 0) {
                    const _TCHAR *tszTemp = pKeyword->GetParameter();
                    const _TCHAR *tszDefault = pKeyword->GetValue(csDefaultContent);
                    if (tszDefault)
                        tszTemp = tszDefault;
                    CTemplateGeneralKeyword *pGeneralKeyword = new CTemplateGeneralKeyword(tszName, tszFriendlyName);
                    if (tszTemp)
                        pGeneralKeyword->SetGeneral(tszTemp);
                    m_aConfigKeywords.Add(pGeneralKeyword);
                } else if (_tcsicmp(tszType, _T("text")) == 0) {
                    const _TCHAR *tszTemp = pKeyword->GetParameter();
                    const _TCHAR *tszDefault = pKeyword->GetValue(csDefaultContent);
                    if (tszDefault)
                        tszTemp = tszDefault;
                    int nMaxLength = pKeyword->GetIntValue(_T("maxlength"));
                    CTemplateTextKeyword *pTextKeyword = new CTemplateTextKeyword(tszName, tszFriendlyName, nMaxLength);
                    if (tszTemp)
                        pTextKeyword->SetText(tszTemp);
                    m_aConfigKeywords.Add(pTextKeyword);
                } else if (_tcsicmp(tszType, _T("select")) == 0) {
                    const _TCHAR *tszTemp = pKeyword->GetParameter();
                    CString csOptions(_T("options_"));
                    csOptions += csLanguage;
                    const _TCHAR *tszVisibleOptions = pKeyword->GetValue(csOptions);
                    bool bHasVisibleOptions = (tszVisibleOptions != NULL);
                    const _TCHAR *tszOptions = pKeyword->GetValue(_T("options"));
                    bool bHasOptions = (tszOptions != NULL);
                    if (!bHasVisibleOptions && bHasOptions)
                        tszVisibleOptions = tszOptions;
                    else if (bHasVisibleOptions && !bHasOptions)
                        tszOptions = tszVisibleOptions;
                    if (bHasVisibleOptions || bHasOptions) {
                        CTemplateSelectKeyword *pSelectKeyword = 
                            new CTemplateSelectKeyword(tszName, tszFriendlyName, tszVisibleOptions, tszOptions);
                        if (tszTemp)
                            pSelectKeyword->SetText(tszTemp);
                        m_aConfigKeywords.Add(pSelectKeyword);
                    }
                }
            }
        }
    }
}

void CTemplateEngine::ConfigureConfigurableKeywords(int nReset)
{
   if (nReset == TEMPLATE_RESET_TEMPLATEDEFAULT)
   {
      // We do not need to read anything now; all variables are
      // already configured the way we need it.
      return;
   }
   // Read the configurable keyword settings from the registry
   // We're in this method and thus an ID has to be present.
   CString csKey;
   csKey.Format(_T("Publisher\\Templates\\%s"), m_csId);

   bool bReadHKLM = false;
   if (nReset == TEMPLATE_RESET_SYSTEMDEFAULT)
      bReadHKLM = true;

   int nCount = m_aConfigKeywords.GetSize();
   for (int i=0; i<nCount; ++i)
   {
      CTemplateKeyword *pKey = m_aConfigKeywords[i];
      char *pData = NULL;
      DWORD dwSize = 0;
      bool success = LRegistry::ReadSettingsBinary(csKey,
         pKey->m_csName, NULL, &dwSize, bReadHKLM);
      if (success)
      {
         pData = new char[dwSize];
         success = LRegistry::ReadSettingsBinary(csKey,
            pKey->m_csName, pData, &dwSize, bReadHKLM);
      }

      if (success)
      {
         pKey->Restore(pData, dwSize);
      }

      if (pData)
         delete[] pData;
      pData = NULL;
   }
}

void CTemplateEngine::StoreKeywordConfiguration(bool bForAllUsers)
{
   CString csKey;
   csKey.Format(_T("Publisher\\Templates\\%s"), m_csId);

   int nCount = m_aConfigKeywords.GetSize();
   for (int i=0; i<nCount; ++i)
   {
      CTemplateKeyword *pKey = m_aConfigKeywords[i];
      char *pData = NULL;
      DWORD dwSize = 0;
      bool success = pKey->Serialize(NULL, &dwSize);
      if (success)
      {
         pData = new char[dwSize];
         success = pKey->Serialize(pData, &dwSize);
      }

      if (success)
      {
         if (bForAllUsers)
         {
            // Writes to HKLM instead of HKCU and deletes any entries under HKCU
            LRegistry::WriteSettingsBinary(csKey, pKey->m_csName, pData, dwSize, true);
         }
         else
         {
            LRegistry::WriteSettingsBinary(csKey, pKey->m_csName, pData, dwSize, false);
         }
      }

      if (pData)
         delete[] pData;
      pData = NULL;
   }
}

void CTemplateEngine::UpdateStandardNavigationStates()
{
   // Just updates the contents of m_aiStandardNavigationStates to
   // match the settings on the dialog.

   int nCount = m_aConfigKeywords.GetSize();
   for (int i = 0; i < nCount; ++i)
   {
      CTemplateKeyword *pKey = m_aConfigKeywords[i];
      if (pKey->GetType() == TE_TYPE_SELECT)
      {
         CTemplateSelectKeyword *pSelectKey = (CTemplateSelectKeyword*)pKey;

         if (pSelectKey->m_nOptionCount > 0)
         {
            CString csKeyName = pSelectKey->m_csName;
            CString csText;
               RETRIEVE_CSTRING(pSelectKey->GetText, csText);

            int nSel = -1;
            int k = 0;
            for (k = 0; k < pSelectKey->m_nOptionCount && nSel == -1; ++k)
            {
               if (csText == pSelectKey->m_caOptions[k])
                  nSel = k;
            }
            if (nSel == -1)
               nSel = 0;

            CString csKeyword;
            for (k = 0; k < NUMBER_OF_NAVIGATION_STATES; ++k)
            {
               csKeyword.Format(_T("%%%s%%"), CS_NAVIGATION_KEYWORDS[k]);
               if (csKeyName.Find(csKeyword) == 0)
                  m_aiStandardNavigationStates[k] = nSel;
            }
         }
      }
   }
}

void CTemplateEngine::ReadGlobalSettings(SgmlElement *pGlobalSettings) {
    if (pGlobalSettings == NULL)
        return;

    CArray<SgmlElement *, SgmlElement *> aSettings;
    pGlobalSettings->GetElements(aSettings);
    for (int i = 0; i < aSettings.GetCount(); ++i) {
        SgmlElement *pSetting = aSettings[i];
        if (pSetting != NULL) {
            const _TCHAR *tszName = pSetting->GetValue(_T("name"));
            const _TCHAR *tszValue = pSetting->GetParameter();

            if (tszName && tszValue) {
                m_csaGlobalSettingsNames.Add(CString(tszName));
                m_csaGlobalSettingsValues.Add(CString(tszValue));
            }
        }
    }
}

UINT CTemplateEngine::CloseTemplate()
{
   if (!m_pConfig)
      return S_FALSE;

   delete m_pConfig;
   m_pConfig = NULL;

   // TODO: Write settings of template to registry. Do it.

   m_csId = _T("");
   m_csTitle = _T("");
   m_csDescription = _T("");
   m_bIsScormCompliant = false;
   m_bIsUtfTemplate = false;

   int nConfigVars = m_aConfigKeywords.GetSize();
   for (int i=0; i<nConfigVars; ++i)
      delete m_aConfigKeywords[i];
   m_aConfigKeywords.RemoveAll();

   m_csaGlobalSettingsNames.RemoveAll();
   m_csaGlobalSettingsValues.RemoveAll();

   return S_OK;
}

UINT CTemplateEngine::Configure(HWND hWndParent, bool bDoNotSave)
{
   // Note down the current directory in order
   // to reset it after configuring. The Java Publisher
   // is sometimes a little bit stingy if you change the pwd
   // during a JNI call.
   DWORD dwBufLen = 0;
   dwBufLen = ::GetCurrentDirectory(0, NULL);
   _TCHAR *tszCurDir = new _TCHAR[dwBufLen + 1];
   ::GetCurrentDirectory(dwBufLen + 1, tszCurDir);

   ULONG_PTR token = NULL;
   Gdiplus::GdiplusStartupInput gsi;
   ZeroMemory(&gsi, sizeof Gdiplus::GdiplusStartupInput);
   gsi.GdiplusVersion = 1;
   Gdiplus::Status status = Gdiplus::GdiplusStartup(&token, &gsi, NULL);

   CWnd *pParent = NULL;
   if (hWndParent)
   {
      pParent = new CWnd;
      pParent->Attach(hWndParent);
   }

   CTemplateDlg templateDlg(IDD_TEMPLATE_DLG, pParent);
   m_bSaveToRegistry = !bDoNotSave;
   templateDlg.DoModal();
   m_bSaveToRegistry = true;

   if (pParent)
   {
      pParent->Detach();
      delete pParent;
      pParent = NULL;
   }

   Gdiplus::GdiplusShutdown(token);

   ::SetCurrentDirectory(tszCurDir);

   return S_OK;
}

UINT CTemplateEngine::AddKeyword(const _TCHAR *tszKeyword, const _TCHAR *tszReplace)
{
   bool bFound = false;
   int nIndex = 0;
   for (int i=0; i<m_keywordList.GetSize(); ++i)
   {
      if (m_keywordList[i].csKeyword == tszKeyword)
      {
         nIndex = i;
         bFound = true;
      }
   }

   // We do not want doubly defined keywords.
   if (bFound)
      m_keywordList.RemoveAt(nIndex);

   m_keywordList.Add(CKeywordReplace(tszKeyword, tszReplace));

   return S_OK;
}
/* ------------------------------------------------------- */

UINT CTemplateEngine::SetScormEnabled(bool bScormEnabled)
{
   m_bScormEnabled = bScormEnabled;

   return S_OK;
}
/* ------------------------------------------------------- */

UINT CTemplateEngine::SetDocumentType(const _TCHAR *tszDocType)
{
   m_csDocType = tszDocType;

   return S_OK;
}

/* ------------------------------------------------------- */

UINT CTemplateEngine::SetTargetFormat(const _TCHAR *tszTargetFormat)
{
   m_csTargetFormat = tszTargetFormat;

   return S_OK;
}

/* ------------------------------------------------------- */

UINT CTemplateEngine::SetTargetDirectory(const _TCHAR *tszTargetPath)
{
   if (tszTargetPath)
   {
      m_csTargetPath = tszTargetPath;
      CheckLastSlash(m_csTargetPath);
      m_bIsTargetTemporary = false;
   }
   else
   {
      // Marker for temporary directory
      m_csTargetPath = _T("");
      m_bIsTargetTemporary = true;

      _TCHAR tszPath[MAX_PATH];
      _TCHAR tszDir[MAX_PATH];
      bool success = LIo::CreateTempDir(_T("TPL"), tszPath, tszDir);
      if (success)
      {
         CString csPath(tszPath);
         CheckLastSlash(csPath);
         csPath += CString(tszDir);
         CheckLastSlash(csPath);
         m_csTargetPath = csPath;
      }
      else
      {
         return E_FAIL;
      }
   }

   return S_OK;
}

/* ------------------------------------------------------- */

/*
void CTemplateEngine::SetTemplateSourcePath(const _TCHAR *tszSourcePath)
{
   m_csSourcePath = tszSourcePath;
   CheckLastSlash(m_csSourcePath);
}
*/

void CTemplateEngine::CheckLastSlash(CString &csPath)
{
   if (csPath.GetLength() == 0 || csPath[csPath.GetLength() - 1] != '\\')
      csPath += CString(_T("\\"));
}

/* ------------------------------------------------------- */

/*
bool CTemplateEngine::HasTemplatePath()
{
   return (m_csSourcePath != _T(""));
}
*/

/* ------------------------------------------------------- */

/*
void CTemplateEngine::SelectTemplatePath()
{
   CString csTitle;
   csTitle.LoadString(IDS_TEMPLATE);
   CPropertySheet ps(csTitle);
   CSelectTemplatePage stp;
   ps.AddPage(&stp);
   ps.DoModal();
}
*/

/* ------------------------------------------------------- */

UINT CTemplateEngine::DoParse(HWND hWndParent)
{
   int res = S_OK;

   if (m_pConfig == NULL)
      return E_FAIL;

   //SgmlFile sgmlConfig;
   //CString csConfigFileName = m_csSourcePath + _T("config.xml");
   //bool success = sgmlConfig.Read(csConfigFileName, "<templateConfig");
   bool success = true;
   /*
   if (!success)
   {
      // TPERR_FILE_NOT_FOUND
      // Die Datei '%s' wurde nicht gefunden, oder sie ist fehlerhaft.
      // The file '%s' was not found, or it is corrupt.
      MakeErrorMessage(TPERR_FILE_NOT_FOUND, csConfigFileName);
   }
   */

   SgmlElement *pRoot = NULL;
   SgmlElement *pDocRoot = NULL;
   SgmlElement *pTypeRoot = NULL;

   if (success)
   {
      //pRoot = sgmlConfig.GetRoot();
      pRoot = m_pConfig->GetRoot();
      pDocRoot = pRoot->Find(m_csDocType);
      success = (pDocRoot != NULL);
      if (!success)
      {
         // TPERR_SECTION_NOT_FOUND
         // config.xml: Der Abschnitt '%s' wurde nicht gefunden.\nMöglicherweise ist das Template defekt.
         // config.xml: The section '%s' was not found.\nThe template is possibly corrupt.
         MakeErrorMessage(TPERR_SECTION_NOT_FOUND, m_csDocType);
      }
   }

   if (success)
   {
      pTypeRoot = pDocRoot->Find(m_csTargetFormat);
      success = (pTypeRoot != NULL);
      if (!success)
      {
         // TPERR_SECTION_NOT_FOUND
         // config.xml: Der Abschnitt '%s' wurde nicht gefunden.\nMöglicherweise ist das Template defekt.
         // config.xml: The section '%s' was not found.\nThe template is possibly corrupt.
         MakeErrorMessage(TPERR_SECTION_NOT_FOUND, m_csTargetFormat);
      }
   }

   // Add the configurable keywords
   if (success)
   {
      int nCount = m_aConfigKeywords.GetSize();
      for (int i=0; i<nCount; ++i)
      {
         CTemplateKeyword *pKey = m_aConfigKeywords[i];
         //_tprintf(_T("Keyword '%s' ==> '%s'\n"), pKey->m_csName, pKey->GetReplaceString());
         AddKeyword(pKey->m_csName, pKey->GetReplaceString());
      }
   }

   // Evaluate the keywords for the entire template
   if (success)
   {
      SgmlElement *pKeywords = pRoot->Find(_T("replaceKeywords"));
      if (pKeywords)
      {
         success = EvaluateKeywords(pKeywords);
         // Note: An error message may have been issued in EvaluateKeywords
      }
   }

   // Evaluate the keywords for the document type
   if (success)
   {
      SgmlElement *pKeywords = pDocRoot->Find(_T("replaceKeywords"));
      if (pKeywords)
      {
         success = EvaluateKeywords(pKeywords);
         // Note: An error message may have been issued in EvaluateKeywords
      }
   }

   // Now evaluate the keywords for the target type
   if (success)
   {
      SgmlElement *pKeywords = pTypeRoot->Find(_T("replaceKeywords"));
      if (pKeywords)
      {
         success = EvaluateKeywords(pKeywords);
         // Note: An error message may have been issued in EvaluateKeywords
      }
   }

   // Ok, we have read in all the keywords, now let's see what
   // we are to do about the files.
   if (success)
   {
      SgmlElement *pStaticFiles = pDocRoot->Find(_T("staticFiles"));
      if (pStaticFiles)
         success = HandleStaticFiles(pStaticFiles);
   }

   if (success)
   {
      if(m_bScormEnabled)
      {
      SgmlElement *pCommonFiles = pDocRoot->Find(_T("commonFiles"));
      if (pCommonFiles)
         success = HandleCommonFiles(pCommonFiles);
      }
      
   }

   if (success)
   {
      if(m_bScormEnabled)
      {
      SgmlElement *pScormFiles = pDocRoot->Find(_T("scormFiles"));
      if (pScormFiles)
         success = HandleStaticFiles(pScormFiles);
      }
      
   }

   if (success)
   {
      SgmlElement *pTemplateFiles = pDocRoot->Find(_T("templateFiles"));
      if (pTemplateFiles)
         success = HandleTemplateFiles(pTemplateFiles);
   }

   if (success)
   {
      SgmlElement *pStaticFiles = pTypeRoot->Find(_T("staticFiles"));
      if (pStaticFiles)
         success = HandleStaticFiles(pStaticFiles);
   }

   if (success)
   {
      SgmlElement *pTemplateFiles = pTypeRoot->Find(_T("templateFiles"));
      if (pTemplateFiles)
         success = HandleTemplateFiles(pTemplateFiles);
   }

   if (success)
   {
      LTextBuffer tbSmil(_T("%SmilData%"));
      ReplaceAllKeywords(tbSmil);
      if (tbSmil.GetSize() > 10)
      {
         CString csSmilDocName;
         csSmilDocName.Format(_T("%s%s.smi"), m_csTargetPath, _T("%BaseDocumentName%"));
         LTextBuffer tbSmilDocName(csSmilDocName);
         ReplaceAllKeywords(tbSmilDocName);
         csSmilDocName = tbSmilDocName.GetString();
           
         m_writtenFiles.Add(csSmilDocName);

         // Write SMIL file as ANSI.
         success = tbSmil.SaveFile(csSmilDocName, LFILE_TYPE_TEXT_ANSI);
         if (!success)
         {
            MakeErrorMessage(TPERR_WRITE_TEMPLATE, csSmilDocName);
         }
      }
   }

   // Are there any configurable images that need to be copied?
   int nKeyCount = m_aConfigKeywords.GetSize();
   for (int i=0; i<nKeyCount; ++i)
   {
      if (m_aConfigKeywords[i]->m_pConfig->dwType == TE_TYPE_IMAGE)
      {
         TE_VAR_IMAGE *pImage = (TE_VAR_IMAGE *) m_aConfigKeywords[i]->m_pConfig;
         // CString converts to and forth from WCHAR* automatically
         CString csFileName(pImage->wszFileName);
         if (csFileName != _T(""))
         {
            CString csOnlyFileName = csFileName;
            StringManipulation::GetFilename(csOnlyFileName);
            CString csTarget = m_csTargetPath + csOnlyFileName;
            if (LIo::CopyFile(csFileName, csTarget))
               m_writtenFiles.Add(csTarget);
            else
            {
               // TPERR_IMAGE_NOT_FOUND
               CString csMsg;
               csMsg.Format(TPERR_IMAGE_NOT_FOUND, csFileName);
               MessageBox(NULL, csMsg, _T("Publisher"), MB_OK | MB_ICONWARNING);
            }
         }
      }
   }

   if (!success)
   {
      ::MessageBox(hWndParent, m_csErrorMessage, _T("Publisher"), MB_OK | MB_ICONERROR);
      res = ERROR_CANCELLED;
   }

   return res;
}

UINT CTemplateEngine::GetFileCount(DWORD *pdwCount)
{
   if (pdwCount == NULL)
      return E_INVALIDARG;
   *pdwCount = m_writtenFiles.GetSize();
   return S_OK;
}

UINT CTemplateEngine::GetFile(UINT nIndex, _TCHAR *tszFileName, DWORD *pdwSize)
{
   int nCount = m_writtenFiles.GetSize();
   if (nIndex >= nCount ||
       nIndex < 0)
      return E_INVALIDARG;

   CString csFileName = m_writtenFiles[nIndex];
   // Chop off path
   csFileName = csFileName.Mid(m_csTargetPath.GetLength());

   RETURN_STRING(csFileName, tszFileName, pdwSize);
}

UINT CTemplateEngine::GetIndexFile(_TCHAR *tszFileName, DWORD *pdwSize)
{
   RETURN_STRING(m_csIndexFileName, tszFileName, pdwSize);
}

UINT CTemplateEngine::GetMaxSlidesWidth(int nScreenWidth, int nVideoWidth,
                                         int nStillImageWidth, int *pMaxSlidesWidth)
{
   if (pMaxSlidesWidth == NULL)
      return E_INVALIDARG;
   LTextBuffer tbWidth(m_csMaxSlidesWidth);
   //::MessageBox(::GetForegroundWindow(), tbWidth.GetString(), _T("tbWidth"), MB_OK);
   CString csTmp;
   csTmp.Format(_T("%d"), nScreenWidth);
   tbWidth.ReplaceAll(_T("%ScreenWidth%"), csTmp);
   csTmp.Format(_T("%d"), nVideoWidth);
   tbWidth.ReplaceAll(_T("%VideoWidth%"), csTmp);
   csTmp.Format(_T("%d"), nStillImageWidth);
   tbWidth.ReplaceAll(_T("%StillImageWidth%"), csTmp);

   // Configurable keywords
   int nCount = m_aConfigKeywords.GetSize();
   for (int i=0; i<nCount; ++i)
   {
      CTemplateKeyword *pKey = m_aConfigKeywords[i];
      tbWidth.ReplaceAll(pKey->m_csName, pKey->GetReplaceString());
   }

   tbWidth.Commit();

   //::MessageBox(::GetForegroundWindow(), tbWidth.GetString(), _T("tbWidth"), MB_OK);
   LExpressionParser expParser(tbWidth.GetString());
   bool success = expParser.EvaluateInt(pMaxSlidesWidth);
   if (!success)
   {
      *pMaxSlidesWidth = nScreenWidth - 424;
      return E_FAIL;
   }
   
   return S_OK;
}

UINT CTemplateEngine::GetMaxSlidesHeight(int nScreenHeight, int nVideoHeight,
                                         int nStillImageHeight, int *pMaxSlidesHeight)
{
   if (pMaxSlidesHeight == NULL)
      return E_INVALIDARG;
   LTextBuffer tbHeight(m_csMaxSlidesHeight);
   //::MessageBox(::GetForegroundWindow(), tbHeight.GetString(), _T("tbHeight"), MB_OK);
   CString csTmp;
   csTmp.Format(_T("%d"), nScreenHeight);
   tbHeight.ReplaceAll(_T("%ScreenHeight%"), csTmp);
   csTmp.Format(_T("%d"), nVideoHeight);
   tbHeight.ReplaceAll(_T("%VideoHeight%"), csTmp);
   csTmp.Format(_T("%d"), nStillImageHeight);
   tbHeight.ReplaceAll(_T("%StillImageHeight%"), csTmp);

   // Configurable keywords
   int nCount = m_aConfigKeywords.GetSize();
   for (int i=0; i<nCount; ++i)
   {
      CTemplateKeyword *pKey = m_aConfigKeywords[i];
      tbHeight.ReplaceAll(pKey->m_csName, pKey->GetReplaceString());
   }

   tbHeight.Commit();

   //::MessageBox(::GetForegroundWindow(), tbHeight.GetString(), _T("tbHeight"), MB_OK);
   LExpressionParser expParser(tbHeight.GetString());
   bool success = expParser.EvaluateInt(pMaxSlidesHeight);
   if (!success)
   {
      *pMaxSlidesHeight = nScreenHeight - 318;
      return E_FAIL;
   }
   
   return S_OK;
}

UINT CTemplateEngine::GetGlobalSetting(const _TCHAR *tszSettingsName, 
                                       _TCHAR *tszSetting, DWORD *pdwSize)
{
   // Try to find the settings name in the list...
   int nIndex = -1;
   for (int i=0; i<m_csaGlobalSettingsNames.GetSize() && nIndex == -1; ++i)
   {
      if (m_csaGlobalSettingsNames[i] == tszSettingsName)
         nIndex = i;
   }

   if (nIndex == -1)
      return E_FAIL;

   RETURN_STRING(m_csaGlobalSettingsValues[nIndex], tszSetting, pdwSize);
}

void CTemplateEngine::DeleteAllWrittenFiles()
{
   for (int i=m_writtenFiles.GetSize()-1; i>=0;--i)
   {
      if (m_writtenFiles[i].GetAt(m_writtenFiles[i].GetLength()-1) != '\\')
         ::DeleteFile(m_writtenFiles[i]);
      else
         ::RemoveDirectory(m_writtenFiles[i]);
      // this not overly important so we do not care for the error
      // code; e.g. if the file does not exist -> even better
      //
      // though it is important to do it in reverse order (backrolling)
      // as non-existant directories comes first in the array
   }
}

void CTemplateEngine::OverrideCurrentPath(const _TCHAR *tszPath, bool bReset)
{
   if (!bReset)
   {
      if (tszPath == NULL)
      {
         RETRIEVE_CSTRING(GetCurrentTemplatePath, m_csProfileTemplatePath);
      }
      else
         m_csProfileTemplatePath = tszPath;
   }
   else
   {
      m_csProfileTemplatePath = _T("");
   }
}

bool CTemplateEngine::EvaluateKeywords(SgmlElement *pKeywords) {
    bool success = true;

    if (pKeywords == NULL)
        return false;

    CArray<SgmlElement *, SgmlElement *> aKeywords;
    pKeywords->GetElements(aKeywords);

    for (int i = 0; i < aKeywords.GetCount() && success; ++i) {
        SgmlElement *pKeyword = aKeywords[i];
        if  (pKeyword) {
            const _TCHAR *tszName = pKeyword->GetValue(_T("name"));
            const _TCHAR *tszEval = pKeyword->GetValue(_T("evaluate"));
            const _TCHAR *tszContent = pKeyword->GetParameter();

            if (tszName) {
                bool bEvaluate = false;
                if (tszEval)
                    bEvaluate = (_tcscmp(_T("true"), tszEval) == 0);
                if (!bEvaluate) {
                    ///AddKeyword(tszName, tszContent);
                    /// replaced by

                    CString csContent;
                    csContent = tszContent;
                    // The content may contain keywords
                    LTextBuffer ltbContent(csContent);
                    ReplaceAllKeywords(ltbContent);
                    csContent = ltbContent.GetString();
                    AddKeyword(tszName, csContent);
                } else {
                    LTextBuffer lText(tszContent);
                    ReplaceAllKeywords(lText);
                    LExpressionParser lExp(lText.GetString());
                    int value = 0;
                    success = lExp.EvaluateInt(&value);
                    if (!success) {
                        // TPERR_EVAL_ERROR
                        // config.xml: Beim Evaluieren des Schlüsselwortes '%s' ist ein Fehler aufgetreten.
                        // config.xml: An error occurred while evaluating the keyword '%s'.
                        CString csKeyword(tszName);
                        MakeErrorMessage(TPERR_EVAL_ERROR, csKeyword);
                    }
                    if (success) {
                        CString csReplace;
                        csReplace.Format(_T("%d"), value);
                        AddKeyword(tszName, csReplace);
                    }
                }
            }
        }
    }

    return success;
}

void CTemplateEngine::ReplaceAllKeywords(LTextBuffer &lBuffer)
{
   for (int i=0; i<m_keywordList.GetSize(); ++i)
   {
      lBuffer.ReplaceAll(m_keywordList[i].csKeyword, m_keywordList[i].csReplace);
   }
   lBuffer.Commit();
}

bool CTemplateEngine::HandleCommonFiles(SgmlElement *pCommonFiles) {
    if (pCommonFiles == NULL)
        return false;

    bool bSuccess = true;
    
    CArray<SgmlElement *, SgmlElement *> aFiles;
    pCommonFiles->GetElements(aFiles);

    for (int i = 0; i < aFiles.GetCount() && bSuccess; ++i) {
        SgmlElement *pFile = aFiles[i];
        if (pFile != NULL) {
            CString csSource;
            CString csTarget;
            bSuccess = GetFileNames(pFile, csSource, csTarget, false, true);

            if (csTarget != m_csTargetPath + m_csIndexFileName)
                m_writtenFiles.Add(csTarget);

            if (bSuccess)
                bSuccess = LIo::CopyFile(csSource, csTarget);
            if (!bSuccess) {
                // TPERR_COPY_FAILED
                // config.xml: Beim Kopieren der Datei '%s' ist ein Fehler aufgetreten.
                // config.xml: Copying the file '%s' failed.
                MakeErrorMessage(TPERR_COPY_FAILED, csSource);
            }
        }
    }

    return bSuccess;
}

bool CTemplateEngine::HandleStaticFiles(SgmlElement *pStaticFiles) {
    if (pStaticFiles == NULL)
        return false;

    bool bSuccess = true;

    CArray<SgmlElement *, SgmlElement *> aFiles;
    pStaticFiles->GetElements(aFiles);

    for (int i = 0; i < aFiles.GetCount() && bSuccess; ++i) {
        SgmlElement *pFile = aFiles[i];
        if (pFile != NULL) {
            CString csSource;
            CString csTarget;
            bSuccess = GetFileNames(pFile, csSource, csTarget, false, false);

            if (csTarget != m_csTargetPath + m_csIndexFileName)
                m_writtenFiles.Add(csTarget);

            if (bSuccess)
                bSuccess = LIo::CopyFile(csSource, csTarget);
            if (!bSuccess) {
                if (csSource.Find(_T("initialize_vi.png")) > -1) {
                    csSource.Replace(_T("initialize_vi.png"), _T("initialize_en.png"));
                    csTarget.Replace(_T("initialize_vi.png"), _T("initialize_en.png"));

                    bSuccess = LIo::CopyFile(csSource, csTarget);
                }
            }

            if (!bSuccess) {      
                // TPERR_COPY_FAILED
                // config.xml: Beim Kopieren der Datei '%s' ist ein Fehler aufgetreten.
                // config.xml: Copying the file '%s' failed.
                MakeErrorMessage(TPERR_COPY_FAILED, csSource);
            }
        }
    }

    return bSuccess;
}

bool CTemplateEngine::HandleTemplateFiles(SgmlElement *pTemplateFiles) {
    if (pTemplateFiles == NULL)
        return false;

    bool bSuccess = true;

    CArray<SgmlElement *,SgmlElement *> aFiles;
    pTemplateFiles->GetElements(aFiles);
    for (int i = 0; i < aFiles.GetCount() && bSuccess; ++i) {
        SgmlElement *pFile = aFiles[i];
        if (pFile != NULL) {
            CString csSource;
            CString csTarget;
            bSuccess = GetFileNames(pFile, csSource, csTarget, true, false);

            if (bSuccess) {
                LTextBuffer ltbSource;
                bSuccess = ltbSource.LoadFile(csSource);
                if (!bSuccess) {
                    // TPERR_READ_TEMPLATE
                    // config.xml: Die Vorlagen-Datei '%s' konnte nicht gelesen werden.
                    // config.xml: Reading the template file '%s' failed.
                    MakeErrorMessage(TPERR_READ_TEMPLATE, csSource);
                }

                if (bSuccess) {
                    ReplaceAllKeywords(ltbSource);

                    if (csTarget != m_csTargetPath + m_csIndexFileName)
                        m_writtenFiles.Add(csTarget);

                    // Write everything as UTF-8!
                    bSuccess = ltbSource.SaveFile(csTarget, LFILE_TYPE_TEXT_UTF8);
                    if (!bSuccess)
                    {
                        // TPERR_WRITE_TEMPLATE
                        // config.xml: Das Schreiben der Datei '%s' schlug fehl.
                        // config.xml: Writing the file '%s' failed.
                        MakeErrorMessage(TPERR_WRITE_TEMPLATE, csTarget);
                    }
                }
            }
        }
    }

    return bSuccess;
}

int CTemplateEngine::FindInArray(CArray<CString, CString> &fileList, CString csSearch)
{
   for (int i=0; i<fileList.GetSize(); ++i)
   {
      if (fileList[i].Compare(csSearch) == 0)
         return i;
   }

   return -1;
}

bool CTemplateEngine::GetFileNames(SgmlElement *pFile, CString &csSourceFile, 
                                      CString &csTargetFile, bool bTemplate, bool bCommonFiles)
{
   bool success = true;

   /*
   const _TCHAR *tszFile = pFile->GetParameter();
   const _TCHAR *tszPrefix = pFile->GetValue("prefix");
   const _TCHAR *tszWriteToRoot = pFile->GetValue("writeToRoot");
   const _TCHAR *tszIsIndex = pFile->GetValue("isIndex");

   //if (tszPrefix && tszWriteToRoot)
   //   _tprintf(_T("<file>: %s, prefix: '%s', writeToRoot: '%s'\n"), tszFile, tszPrefix, tszWriteToRoot);
   //else if (tszPrefix)
   //   _tprintf(_T("<file>: %s, prefix: '%s'\n"), tszFile, tszPrefix);
   //else if (tszWriteToRoot)
   //   _tprintf(_T("<file>: %s, writeToRoot: '%s'\n"), tszFile, tszWriteToRoot);

   bool bWriteToRoot = false;
   if (tszWriteToRoot)
      bWriteToRoot = (_tcscmp("true", tszWriteToRoot) == 0);
   bool bIsIndex = false;
   if (tszIsIndex)
      bIsIndex = (_tcscmp("true", tszIsIndex) == 0);


   bool bHasPrefix = false;
   CString csPrefix;
   if (tszPrefix)
   {
      // The prefix may contain keywords
      csPrefix = tszPrefix;
      LTextBuffer ltbPrefix(csPrefix);
      ReplaceAllKeywords(ltbPrefix);
      csPrefix = ltbPrefix.GetString();
      bHasPrefix = true;
   }
   
   if (tszFile)
   {
      CString csSrcFile;
      csSrcFile = tszFile;
      // The source file may contain keywords
      LTextBuffer ltbSrcFile(csSrcFile);
      ReplaceAllKeywords(ltbSrcFile);
      csSrcFile = ltbSrcFile.GetString();

      csSrcFile.Replace('/', '\\');
      int nLastSlash = csSrcFile.ReverseFind('\\');
      CString csFileName = csSrcFile;
      if (nLastSlash > 0)
      {
         // Has directory/ies
         CString csDirs = csSrcFile.Mid(0, nLastSlash);
         csFileName = csSrcFile.Mid(nLastSlash + 1);
         if (!bWriteToRoot)
         {
            // TODO: this "look if the path is already in the array" algorithm
            // is O(n^2)!

            // 1.7.0: Do not add the directory names to the written files
            // list! This makes GetFile() return the directories also, which
            // is not in the specification! CleanUp() now deletes the entire
            // directory anyways, so there is no need to do this.
            // In case SetTargetDirectory() is used with a non-NULL argument
            // (not used right now), the directories would remain in the
            // target directory.

            
//            CString csMarker("\\");
//
//            // have a look if the path is more than one (new) diretory deep
//            int nFirstSlash = csDirs.Find('\\');
//            while (nFirstSlash > 0)
//            {
//               CString csDirSub = csDirs.Mid(0, nFirstSlash);
//               CString csFullPath = m_csTargetPath + csDirSub + csMarker;
//               if (FindInArray(m_writtenFiles, csFullPath) < 0)
//               {
//                  m_writtenFiles.Add(csFullPath);
//               }
//               nFirstSlash = csDirs.Find('\\', nFirstSlash+1);
//            }
//
//            // the directory itself
//            CString csFullPath = m_csTargetPath + csDirs + csMarker;
//            if (FindInArray(m_writtenFiles, csFullPath) < 0)
//            {
//               m_writtenFiles.Add(csFullPath);
//            }


            // this is the "meat line":
            success = LIo::CreateDirs(m_csTargetPath, csDirs);
            if (!success)
            {
               // TPERR_CREATE_DIR
               // config.xml: Beim Erstellen eines Verzeichnisses in '%s' ist ein Fehler aufgetreten.\nIst das Verzeichnis schreibgeschützt?
               // config.xml: Creating a directory in '%s' failed.\nIs the directory write protected?
               MakeErrorMessage(TPERR_CREATE_DIR, m_csTargetPath);
            }
         }

         if (success)
         {
            if (bHasPrefix)
            {
               CString csTmp = csPrefix;

               if (bTemplate)
               {
                  // In case of "index" as a file name, remove "index"
                  if (csFileName.Mid(0, 5) == _T("index"))
                     csFileName = csFileName.Mid(5);
               }

               csTmp += csFileName;
               if (bWriteToRoot)
                  csFileName = csTmp;
               else
                  csFileName = csDirs + CString(_T("\\")) + csTmp;
            }
            else
            {
               CString csTmp = csFileName;
               if (bWriteToRoot)
                  csFileName = csTmp;
               else
                  csFileName = csDirs + CString(_T("\\")) + csTmp;
            }
         }
      }
      else
      {
         // Document from root
         // In case of "index" as a file name, replace index with prefix
         if (csFileName.Mid(0, 5) == _T("index") && bHasPrefix)
         {
            csFileName = csFileName.Mid(5);
            csFileName = csPrefix + csFileName;
         }

      }
      
      if (success)
      {
         if (bTemplate)
         {
            // If the file name ends with ".tmpl", remove that
            if (csFileName.Mid(csFileName.GetLength() - 5) == _T(".tmpl"))
               csFileName = csFileName.Mid(0, csFileName.GetLength() - 5);
         }

         csSourceFile = m_csSourcePath + csSrcFile;
         csTargetFile = m_csTargetPath + csFileName;

         if (bIsIndex)
            m_csIndexFileName = csFileName;

         // _tprintf(_T("%s --> %s\n"), csSourceFile, csTargetFile);
      }
   }
   else
   {
      success = false;
      // TPERR_READ_FILES
      // config.xml: Beim Auslesen der zu verarbeitenden Dateien ist ein Fehler aufgetreten.
      // config.xml: An error occurred while reading which files to handle.
      MakeErrorMessage(TPERR_READ_FILES);
   }
   */

   _TCHAR tszSourceFile[2*_MAX_PATH];
   _TCHAR tszTargetFile[2*_MAX_PATH];
   bool bIsIndex = false;
   if(bCommonFiles)
      success = SgmlFile::ResolveFileNode(pFile, tszSourceFile, tszTargetFile, bTemplate, &bIsIndex,
         true, m_csCommonSourcePath, m_csTargetPath);
   else
      success = SgmlFile::ResolveFileNode(pFile, tszSourceFile, tszTargetFile, bTemplate, &bIsIndex,
         true, m_csSourcePath, m_csTargetPath);
   if (success)
   {
      LTextBuffer ltbTargetFile(tszTargetFile);
      ReplaceAllKeywords(ltbTargetFile);
      csTargetFile = ltbTargetFile.GetString();
      LTextBuffer ltbSourceFile(tszSourceFile);
      ReplaceAllKeywords(ltbSourceFile);
      csSourceFile = ltbSourceFile.GetString();

      if (bIsIndex)
      {
         m_csIndexFileName = csTargetFile.Mid(m_csTargetPath.GetLength());
      }
   }
   else
   {
      // TPERR_READ_FILES
      // config.xml: Beim Auslesen der zu verarbeitenden Dateien ist ein Fehler aufgetreten.
      // config.xml: An error occurred while reading which files to handle.
      MakeErrorMessage(TPERR_READ_FILES);
   }

   return success;
}

void CTemplateEngine::MakeErrorMessage(UINT nMsgId, const _TCHAR *tszParam)
{
   if (tszParam)
      m_csErrorMessage.Format(nMsgId, tszParam);
   else
      m_csErrorMessage.LoadString(nMsgId);
}


bool CTemplateEngine::FileExists(LPCTSTR tszFile)
{
   // unfortunately doesn't work: returns always true

   DWORD dwFileAtts = ::GetFileAttributes(tszFile);

   HANDLE hFile = ::CreateFile(tszFile, 
      0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

   return INVALID_HANDLE_VALUE != hFile || (dwFileAtts & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

void CTemplateEngine::AddMissingFile(CString csFile)
{
   // unfortunately doesn't work: as FileExists() doesn't work

   _TCHAR tszFileName[MAX_PATH*2];
   _tcscpy(tszFileName, csFile);

   CArray<CString, CString> nonExistant;

   _TCHAR *tszSuffix = _tcsrchr(tszFileName, '\\');
   while(NULL != tszSuffix && !FileExists(tszFileName))
   {
      // there must be at least one \ remaining in tszFile (i.e. c:\)

      CString csNono(tszFileName);
      // TODO: Wie geht denn das hier mit der Speicherverwaltung??
      // Diese Variable ist doch nur auf dem Stack. MD: Macht nichts; sie
      // wird für das Array sowieso kopiert (wg <CString, CString> und nicht
      // <CString *, CString >. Add(&nonExistant) würde dann zu einem großen
      // Problem. Hier ist das unproblematisch. Kein Memory Leak.
      nonExistant.Add(csNono);

      tszSuffix[0] = '\0';

      tszSuffix = _tcsrchr(tszFileName, '\\');
   }

   if (nonExistant.GetSize() > 0)
   {
      for (int i=nonExistant.GetSize()-1; i>=0; --i)
      {
         m_writtenFiles.Add(nonExistant[i]);
         // it is important that them are added in reverse order 
         // so that created directories are before the contained files
      }
   }
}

void CTemplateEngine::GetTemplateKeywords(CArray<CTemplateKeyword *, CTemplateKeyword *> &caKeywords)
{
   int nSize = m_aConfigKeywords.GetSize();
   for (int i=0; i<nSize; ++i)
   {
      caKeywords.Add(m_aConfigKeywords[i]);
   }
}

void CTemplateEngine::SetStandardNavigationStates(int *aiStandardNavigationStates)
{
   for (int i = 0; i < NUMBER_OF_NAVIGATION_STATES; ++i)
      m_aiStandardNavigationStates[i] = aiStandardNavigationStates[i];
}

UINT CTemplateEngine::GetStandardNavigationStates(int *aiStandardNavigationStates) 
{
   if (NULL == aiStandardNavigationStates)
      return E_INVALIDARG;

   // Bug 3032: Navigation states must be read from registry and updated in the array
   UpdateStandardNavigationStates();
   for (int i = 0; i < NUMBER_OF_NAVIGATION_STATES; ++i)
      aiStandardNavigationStates[i] = m_aiStandardNavigationStates[i]; 

   return S_OK;
};

// For exporting 'standard navigation' to LPDs -> modify LRD
UINT CTemplateEngine::PutStandardNavigationStatesToLrd(const _TCHAR *tszLrdFile, int *aiStandardNavigationStates) 
{
   CLrdFile *pLrdFile = new CLrdFile();

   pLrdFile->Open(tszLrdFile);
   pLrdFile->PutNavigationStates(aiStandardNavigationStates);
   pLrdFile->Write(tszLrdFile);

   if (pLrdFile)
      delete pLrdFile;
   pLrdFile = NULL;

   _tprintf(_T("\"Navigation States\" successfully added to LRD file.\n"));
   return S_OK;
};

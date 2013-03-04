// ScormEngine.cpp: Implementierung der Klasse CScormEngine.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ScormEngine.h"

#include "ScormGeneralPage.h"
#include "ScormRightsPage.h"
#include "ScormCatalogentryPage.h"
#include "ScormClassificationPage.h"

#include "MySheet.h" // MfcUtils: CMySheet
#include "lutils.h" // LIo

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CScormEngine::CScormEngine()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());


   // m_scormSettings doesn't need to be initialized (stack object)
   // it does this on its own in its constructor

}

CScormEngine::~CScormEngine()
{
   // nothing to be done for m_scormSettings: it is a stack object
}

UINT CScormEngine::Init()
{
   m_bSaveToRegistry = true;

   m_csIndexFileName = _T("");
   m_csRealIndexFileName = _T("");

   m_aSourceFiles.RemoveAll();
   m_aTargetFiles.RemoveAll();
   m_aKeywords.RemoveAll();
   m_aGeneralKeywords.RemoveAll();
   m_aGeneralValues.RemoveAll();
   m_setFulltextWords.clear();
   
   m_csBaseTempPath = _T("");
   m_csRelTempPath = _T("");
   m_csTargetDir = _T("");

   m_dwProgress = 0;
   m_dwCurrentFile = 0;
   m_bCancelRequested = false;

   return S_OK;
}

UINT CScormEngine::ReadSettings()
{
   return m_scormSettings.ReadRegistry();
}

UINT CScormEngine::WriteSettings()
{
   UINT nReturn = S_OK;
   
   if (m_bSaveToRegistry)
      m_scormSettings.WriteRegistry();

   return nReturn;
}


UINT CScormEngine::GetSettings(_TCHAR *tszSettings, DWORD *pdwSize)
{
   if (pdwSize == NULL)
      return E_POINTER;

   CString csRepresentation = m_scormSettings.GetRepresentation();

   if (tszSettings != NULL && *pdwSize >= csRepresentation.GetLength() + 1)
      _tcscpy(tszSettings, csRepresentation);
   else
      *pdwSize = csRepresentation.GetLength() + 1;

   return S_OK;
}

UINT CScormEngine::UseSettings(const _TCHAR *tszSettings)
{
   if (tszSettings == NULL)
      return E_POINTER;

   if (_tcslen(tszSettings) == 0)
      return E_INVALIDARG;

   CString csSettings = tszSettings;

   return m_scormSettings.ParseFrom(csSettings);
}

UINT CScormEngine::CleanUp()
{
   if (m_csTargetDir != _T(""))
      LIo::DeleteDir(m_csTargetDir);

   return S_OK;
}

UINT CScormEngine::Configure(HWND hWndParent, bool bDoNotSave)
{
   CWnd wndParent;
   wndParent.Attach(hWndParent);
   CMySheet scormSheet(_T("SCORM"), &wndParent);

   CScormGeneralPage p1;
   CScormRightsPage p2;
   CScormCatalogentryPage p3;
   CScormClassificationPage p4;
   scormSheet.AddPage(&p1);
   scormSheet.AddPage(&p2);
   scormSheet.AddPage(&p3);
   scormSheet.AddPage(&p4);

   m_bSaveToRegistry = !bDoNotSave;
   scormSheet.DoModal();
   m_bSaveToRegistry = true;

   wndParent.Detach();

   return S_OK;
}

UINT CScormEngine::SetTargetFileName(const _TCHAR *tszTargetFile)
{
   if (tszTargetFile == NULL)
      return E_INVALIDARG;

   m_csTargetFile = tszTargetFile;
   return S_OK;
}

UINT CScormEngine::TCharFromCString(CString &csString, _TCHAR *tszTarget, DWORD *pdwSize)
{
   int nLen = csString.GetLength();

   if (tszTarget == NULL)
   {
      if (pdwSize == NULL)
         return E_INVALIDARG;

      *pdwSize = nLen + 1;
      return S_OK;
   }

   if (pdwSize == NULL)
      return E_INVALIDARG;

   if (*pdwSize < nLen + 1)
      return ERROR_BUFFER_OVERFLOW;

   _tcscpy(tszTarget, csString);

   return S_OK;
}

UINT CScormEngine::GetTargetFilePath(_TCHAR *tszTargetPath, DWORD *pdwSize)
{
   return TCharFromCString((m_csTargetDir + m_csTargetFile + _T(".zip")), tszTargetPath, pdwSize);
}

UINT CScormEngine::GetLanguage(_TCHAR *tszLanguage, DWORD *pdwSize)
{
   // CStrings convert automatically from wchar* and char* to _TCHAR.
   return TCharFromCString(CString(m_scormSettings.szLanguage), tszLanguage, pdwSize);
}

UINT CScormEngine::GetVersion(_TCHAR *tszVersion, DWORD *pdwSize)
{
   // CStrings convert automatically from wchar* and char* to _TCHAR.
   //return TCharFromCString(CString(m_scormSettings.szVersion), tszVersion, pdwSize);
   // Bug 3026: Return hard coded "1.3", until SCORM version is changeable
   return TCharFromCString(CString(_T("1.3")), tszVersion, pdwSize);
}

UINT CScormEngine::GetStatus(_TCHAR *tszVersion, DWORD *pdwSize)
{
   // CStrings convert automatically from wchar* and char* to _TCHAR.
   return TCharFromCString(CString(m_scormSettings.szStatus), tszVersion, pdwSize);
}

UINT CScormEngine::GetFullTextAsKeywords(BOOL *pbFullText)
{
   if (pbFullText == NULL)
      return E_INVALIDARG;

   *pbFullText = m_scormSettings.bFulltextAsKeyword ? TRUE : FALSE;

   return S_OK;
}

UINT CScormEngine::SetIndexFileName(const _TCHAR *tszRealFileName, const _TCHAR *tszFileName)
{
   if (tszRealFileName == NULL ||
       tszFileName == NULL)
      return E_INVALIDARG;

   m_csRealIndexFileName = tszRealFileName;
   m_csIndexFileName = tszFileName;
   return S_OK;
}

UINT CScormEngine::AddResourceFile(const _TCHAR *tszRealFileName, const _TCHAR *tszFileName)
{
   if (tszRealFileName == NULL ||
       tszFileName == NULL)
      return E_INVALIDARG;

   m_aSourceFiles.Add(tszRealFileName);
   m_aTargetFiles.Add(tszFileName);
   return S_OK;
}

UINT CScormEngine::AddMetadataKeyword(const _TCHAR *tszKeyword)
{
   if (tszKeyword == NULL)
      return E_INVALIDARG;

   m_aKeywords.Add(CString(tszKeyword));

   return S_OK;
}

UINT CScormEngine::AddGeneralKeyword(const _TCHAR *tszKey, const _TCHAR *tszValue)
{
   if (tszKey == NULL ||
       tszValue == NULL)
      return E_INVALIDARG;

   m_aGeneralKeywords.Add(CString(tszKey));
   m_aGeneralValues.Add(CString(tszValue));

   return S_OK;
}

UINT CScormEngine::AddFullTextString(const _TCHAR *tszFullText)
{
   if (tszFullText == NULL)
      return E_INVALIDARG;

   CString csWork(tszFullText);
   // Remove all superfluous characters
   // The first five are not allowed in XML files.
   csWork.Remove(_T('"'));
   csWork.Remove(_T('\''));
   csWork.Remove(_T('&'));
   csWork.Remove(_T('<'));
   csWork.Remove(_T('>'));
   // The rest are just messing up the file
   csWork.Remove(_T(','));
   csWork.Remove(_T('.'));
   csWork.Remove(_T(';'));
   csWork.Remove(_T(':'));
   csWork.Remove(_T('?'));
   csWork.Remove(_T('!'));
   csWork.Remove(_T('\t')); // Tab stop
   csWork.Remove(_T('/'));
   csWork.Remove(_T('\\'));
   csWork.Remove(_T('('));
   csWork.Remove(_T(')'));

   int nStartPos = 0;
   int nFoundPos = csWork.Find(_T(' '));
   while (nFoundPos >= 0)
   {
      if (nFoundPos > nStartPos + 1)
      {
         m_setFulltextWords.insert(csWork.Mid(nStartPos, nFoundPos-nStartPos));
      }
      nStartPos = nFoundPos + 1;
      nFoundPos = csWork.Find(_T(' '), nStartPos);
   }

   if (nStartPos < csWork.GetLength() - 1)
   {
      m_setFulltextWords.insert(csWork.Mid(nStartPos));
   }

   return S_OK;
}

UINT CScormEngine::CreatePackage(HWND hWndParent)
{
   // Reset progress (should be zero anyway)
   m_dwProgress = 0;
   m_dwCurrentFile = 0;
   m_bCancelRequested = false;

   // Create a temporary directory
   _TCHAR tszPath[MAX_PATH];
   _TCHAR tszDirectory[MAX_PATH];
   bool success = LIo::CreateTempDir(_T("SCO"), tszPath, tszDirectory);

   if (success)
   {
      m_csBaseTempPath = tszPath;
      m_csRelTempPath = tszDirectory;
      m_csTargetDir = m_csBaseTempPath + m_csRelTempPath + CString(_T("\\"));
   }

   //MessageBox(hWndParent, tszDirectory, tszPath, MB_OK | MB_ICONINFORMATION);

   if (success)
      DefineScormKeywords();

   CString csImsManifest;
   if (success)
      success = LoadXmlResource(csImsManifest, IDR_XML_IMSMANIFEST);

   if (success && !m_bCancelRequested)
   {
      // Let's write the imsmanifest File.
      LTextBuffer tbImsManifest(csImsManifest);
      int nKwCount = m_aGeneralKeywords.GetSize();
      for (int i=0; i<nKwCount; ++i)
         tbImsManifest.ReplaceAll(m_aGeneralKeywords[i], m_aGeneralValues[i]);

      success = tbImsManifest.SaveFile(m_csTargetDir + _T("imsmanifest.xml"), 
         LFILE_TYPE_TEXT_UTF8, false); // Do not write a UTF8 BOM.
   }

/*   CString csScoMetadata;
   if (success)
      success = LoadXmlResource(csScoMetadata, IDR_XML_SCOMETADATA);

   if (success && !m_bCancelRequested)
   {
      // And now the scometadata.xml file.
      LTextBuffer tbScoMetadata(csScoMetadata);
      int nKwCount = m_aGeneralKeywords.GetSize();
      for (int i=0; i<nKwCount; ++i)
         tbScoMetadata.ReplaceAll(m_aGeneralKeywords[i], m_aGeneralValues[i]);

      success = tbScoMetadata.SaveFile(m_csTargetDir + _T("scometadata.xml"),
         LFILE_TYPE_TEXT_UTF8, false); // No UTF8 BOM, thank you.
   }*/

   if (success && !m_bCancelRequested)
   {
      CZipArchive zip;
      CScormZipCallback zipCallback(this);
      zip.SetCallback(&zipCallback);
      zip.Open(m_csTargetDir + m_csTargetFile + _T(".zip"), CZipArchive::zipCreate);
      zip.AddNewFile(m_csTargetDir + _T("imsmanifest.xml"), _T("imsmanifest.xml"));
//      zip.AddNewFile(m_csTargetDir + _T("scometadata.xml"), _T("scometadata.xml"));
      zip.AddNewFile(m_csRealIndexFileName, m_csIndexFileName);
      int nFileCount = m_aSourceFiles.GetSize();
      for (int i=0; i<nFileCount; ++i)
         zip.AddNewFile(m_aSourceFiles[i], m_aTargetFiles[i]);
      zip.Close();
   }

   if (m_bCancelRequested)
      return ERROR_CANCELLED;

   return success ? S_OK : E_FAIL;
}

UINT CScormEngine::GetProgress(DWORD *pdwProgress)
{
   if (pdwProgress == NULL)
      return E_INVALIDARG;

   int nTotalFiles = m_aSourceFiles.GetSize() + 2 ;//+ 1; // scometadata + imsmanifest
   double dProgress = ((double) m_dwCurrentFile) / ((double) nTotalFiles);
   *pdwProgress = (DWORD) (100.0 * dProgress);

   return S_OK;
}

UINT CScormEngine::Cancel()
{
   return S_OK;
}

CString CScormEngine::ConvertSlashes(const _TCHAR *tszFileName)
{
   CString csTmp(tszFileName);
   csTmp.Replace(_T('\\'), _T('/'));
   return csTmp;
}

void CScormEngine::DefineScormKeywords()
{
   CString strScormStatus = m_scormSettings.szStatus;
   strScormStatus.MakeLower();
   AddGeneralKeyword(_T("%ScormLanguage%"), CString(m_scormSettings.szLanguage));
   AddGeneralKeyword(_T("%ScormVersion%"), CString(m_scormSettings.szVersion));
//   AddGeneralKeyword(_T("%ScormStatus%"), CString(m_scormSettings.szStatus));
   AddGeneralKeyword(_T("%ScormStatus%"), strScormStatus);
   AddGeneralKeyword(_T("%ScormCost%"), m_scormSettings.bCost ? _T("yes") : _T("no"));
   AddGeneralKeyword(_T("%ScormCopyrightRestrictions%"), m_scormSettings.bCopyright ? _T("yes") : _T("no"));
   AddGeneralKeyword(_T("%IndexFileName%"), ConvertSlashes(m_csIndexFileName));

   CString csResources;
   int nResourceCount = m_aTargetFiles.GetSize();
   int i = 0;
   for (i = 0; i < nResourceCount; ++i)
   {
      CString csResource;
      csResource.Format(_T("      <file href=\"%s\" />\n"), ConvertSlashes(m_aTargetFiles[i]));
      csResources += csResource;
   }
   AddGeneralKeyword(_T("%ScoResources%"), csResources);
   csResources.Empty();

   // ---- keywords ----
   CString csKeywords;
   int nKeywordsLeft = 40;
   int nKeywordCount = m_aKeywords.GetSize();
   for (i = 0; i < nKeywordCount && nKeywordsLeft > 0; ++i)
   {
      --nKeywordsLeft;
      CString csKeyword;
 //     csKeyword.Format(_T("    <keyword>\n      <langstring xml:lang=\"%s\">%s</langstring>\n    </keyword>\n"), CString(m_scormSettings.szLanguage), m_aKeywords[i]);
      csKeyword.Format(_T("    <keyword>\n      <string language=\"%s\">%s</string>\n    </keyword>\n"), CString(m_scormSettings.szLanguage), m_aKeywords[i]);
      csKeywords += csKeyword;
   }

   std::set<CString>::iterator iter = m_setFulltextWords.begin();
   // Bugfix #2305: page titles&keywords are always added, independent from full text
   //bool bFullTextReady = !m_scormSettings.bFulltextAsKeyword;
   bool bFullTextReady = false;
   while (!bFullTextReady)
   {
      int nCurrentTextSize = 0;
      CString csKeywordList;
      while (nCurrentTextSize < 3900 && iter != m_setFulltextWords.end())
      {
         CString csCurrent = *iter;
         csKeywordList += csCurrent + _T(" ");
         nCurrentTextSize += csCurrent.GetLength() + 1;
         ++iter;
      }

      if (nCurrentTextSize > 0)
      {
         --nKeywordsLeft;
         CString csKeyword;
//         csKeyword.Format(_T("    <keyword>\n      <langstring xml:lang=\"%s\">%s</langstring>\n    </keyword>\n"), CString(m_scormSettings.szLanguage), csKeywordList);
         csKeyword.Format(_T("    <keyword>\n      <string language=\"%s\">%s</string>\n    </keyword>\n"), CString(m_scormSettings.szLanguage), csKeywordList);
         csKeywords += csKeyword;
      }

      if (iter == m_setFulltextWords.end() ||
          nKeywordsLeft == 0)
         bFullTextReady = true;
   }

   AddGeneralKeyword(_T("%Keywords%"), csKeywords);
   // ---- keywords end ----

   // ---- general catalog/entry ----
   AddGeneralKeyword(_T("%ScormGnrlCatalog%"), CString(m_scormSettings.m_tszGnrlCeCatalog));
   AddGeneralKeyword(_T("%ScormGnrlEntry%"), CString(m_scormSettings.m_tszGnrlCeEntry));
   // ---- general catalog/entry end ----

   // ---- classification ----
   AddGeneralKeyword(_T("%ScormClsfDescriptionDiscipline%"), CString(m_scormSettings.m_atszClsfDescription[0]));
   AddGeneralKeyword(_T("%ScormClsfKeywordDiscipline%"), CString(m_scormSettings.m_atszClsfKeyword[0]));
   AddGeneralKeyword(_T("%ScormClsfDescriptionIdea%"), CString(m_scormSettings.m_atszClsfDescription[1]));
   AddGeneralKeyword(_T("%ScormClsfKeywordIdea%"), CString(m_scormSettings.m_atszClsfKeyword[1]));
   AddGeneralKeyword(_T("%ScormClsfDescriptionPrerequisite%"), CString(m_scormSettings.m_atszClsfDescription[2]));
   AddGeneralKeyword(_T("%ScormClsfKeywordPrerequisite%"), CString(m_scormSettings.m_atszClsfKeyword[2]));
   AddGeneralKeyword(_T("%ScormClsfDescriptionEducationalObjective%"), CString(m_scormSettings.m_atszClsfDescription[3]));
   AddGeneralKeyword(_T("%ScormClsfKeywordEducationalObjective%"), CString(m_scormSettings.m_atszClsfKeyword[3]));
   AddGeneralKeyword(_T("%ScormClsfDescriptionAccessibilityRestrictions%"), CString(m_scormSettings.m_atszClsfDescription[4]));
   AddGeneralKeyword(_T("%ScormClsfKeywordAccessibilityRestrictions%"), CString(m_scormSettings.m_atszClsfKeyword[4]));
   AddGeneralKeyword(_T("%ScormClsfDescriptionEducationalLevel%"), CString(m_scormSettings.m_atszClsfDescription[5]));
   AddGeneralKeyword(_T("%ScormClsfKeywordEducationalLevel%"), CString(m_scormSettings.m_atszClsfKeyword[5]));
   AddGeneralKeyword(_T("%ScormClsfDescriptionSkillLevel%"), CString(m_scormSettings.m_atszClsfDescription[6]));
   AddGeneralKeyword(_T("%ScormClsfKeywordSkillLevel%"), CString(m_scormSettings.m_atszClsfKeyword[6]));
   AddGeneralKeyword(_T("%ScormClsfDescriptionSecurityLevel%"), CString(m_scormSettings.m_atszClsfDescription[7]));
   AddGeneralKeyword(_T("%ScormClsfKeywordSecurityLevel%"), CString(m_scormSettings.m_atszClsfKeyword[7]));
   // ---- classification end ----
}

bool CScormEngine::LoadXmlResource(CString &csXml, UINT nResource)
{
   HRSRC hRes = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nResource), _T("XML"));
   bool success = (hRes != NULL);

   DWORD dwSize = 0;
   if (success)
   {
      dwSize = ::SizeofResource(AfxGetResourceHandle(), hRes);
      success = (dwSize > 0);
   }

   HGLOBAL hGlobRes = NULL;
   if (success)
   {
      hGlobRes = ::LoadResource(AfxGetResourceHandle(), hRes);
      success = (hGlobRes != NULL);
   }

   LPVOID pData = NULL;
   if (success)
   {
      pData = ::LockResource(hGlobRes);
      success = (pData != NULL);
   }

   if (success)
   {
      // char * is on purpose; XML data is in ANSI or UTF8 encoding
      char *szXml = new char[dwSize + 1];
      memcpy(szXml, pData, dwSize);
      szXml[dwSize] = (char) 0;

      // CString converts to _TCHAR automatically.
      csXml = szXml;
      delete[] szXml;
   }

   return success;
}

//////////////////////////////////////////////////////////////////////
//                         CScormZipCallback
//////////////////////////////////////////////////////////////////////

CScormZipCallback::CScormZipCallback(CScormEngine *pScormEngine) : CZipActionCallback()
{
   m_pScormEngine = pScormEngine;
}

void CScormZipCallback::Init(LPCTSTR lpszFileInZip, LPCTSTR lpszExternalFile)
{
   m_pScormEngine->m_dwCurrentFile++;
}

void CScormZipCallback::SetTotal(DWORD uTotalToDo)
{
}

bool CScormZipCallback::Callback(int iProgress)
{
   // Return false to cancel

   return !m_pScormEngine->m_bCancelRequested;
}

//////////////////////////////////////////////////////////////////////
//                         CScormSettings2
//////////////////////////////////////////////////////////////////////

CScormSettings2::CScormSettings2()
{
   ZeroMemory(&m_ScormSettingsOld, sizeof SCORMSETTINGS);
   
   // Initialize the old settings to something nice
   m_ScormSettingsOld.cbSize = sizeof SCORMSETTINGS;
   // Note the usage of char * here. It's on purpose!!
   strcpy(m_ScormSettingsOld.szLanguage, "");
   strcpy(m_ScormSettingsOld.szStatus, "Final");
   strcpy(m_ScormSettingsOld.szVersion, "1.3");
   m_ScormSettingsOld.bCost = false;
   m_ScormSettingsOld.bCopyright = true;
   m_ScormSettingsOld.bFulltextAsKeyword = true;


   // make this and the old structure the same
   szLanguage = m_ScormSettingsOld.szLanguage;
   szStatus = m_ScormSettingsOld.szStatus;
   szVersion = m_ScormSettingsOld.szVersion;

   bCost = m_ScormSettingsOld.bCost;
   bCopyright = m_ScormSettingsOld.bCopyright;
   bFulltextAsKeyword = m_ScormSettingsOld.bFulltextAsKeyword;


   // Initialize the new settings to something nullish
   ZeroMemory(m_tszGnrlCeCatalog, SCORM_GNRL_CE_CATALOG_SIZE * sizeof _TCHAR);
   ZeroMemory(m_tszGnrlCeEntry, SCORM_GNRL_CE_ENTRY_SIZE * sizeof _TCHAR);

   m_nPurposeIndex = 0;
   
   int i = 0;
   for (i = 0; i < SCORM_PURPOSE_ARRAY_SIZE; ++i)
   {
      ZeroMemory(m_atszClsfDescription[i], SCORM_CLSF_DESCRIPTION_SIZE * sizeof _TCHAR);
      ZeroMemory(m_atszClsfKeyword[i], SCORM_CLSF_KEYWORD_SIZE * sizeof _TCHAR);
   }

   // Initialize helper array with identifier strings
   for (i = 0; i < SCORM_PURPOSE_ARRAY_SIZE; ++i)
      ZeroMemory(m_atszRegistryClsfNames[i], SCORM_MAX_IDENTIFIER_LENGTH * sizeof _TCHAR);
   
   _tcscpy(m_atszRegistryClsfNames[SCORM_CLSF_IDX_DISCIPLINE], _T("ClsfDiscipline"));
   _tcscpy(m_atszRegistryClsfNames[SCORM_CLSF_IDX_IDEA], _T("ClsfIdea"));
   _tcscpy(m_atszRegistryClsfNames[SCORM_CLSF_IDX_PREREQUISITE], _T("ClsfPrerequisite"));
   _tcscpy(m_atszRegistryClsfNames[SCORM_CLSF_IDX_EDUCATIONAL_OBJECTIVE], _T("ClsfEducationalObjective"));
   _tcscpy(m_atszRegistryClsfNames[SCORM_CLSF_IDX_ACCESSIBILITY_RESTRICTIONS], _T("ClsfAccessibilityRestrictions"));
   _tcscpy(m_atszRegistryClsfNames[SCORM_CLSF_IDX_EDUCATIONAL_LEVEL], _T("ClsfEducationalLevel"));
   _tcscpy(m_atszRegistryClsfNames[SCORM_CLSF_IDX_SKILL_LEVEL], _T("ClsfSkillLevel"));
   _tcscpy(m_atszRegistryClsfNames[SCORM_CLSF_IDX_SECURITY_LEVEL], _T("ClsfSecurityLevel"));
}

HRESULT CScormSettings2::CopyClassificationDataTo(CScormSettings2 &scormSettings2)
{
   /* needed for a more general "CopyDataTo()"
   strcpy(scormSettings2.szLanguage, szLanguage);
   strcpy(scormSettings2.szStatus, szStatus);
   strcpy(scormSettings2.szVersion, szVersion);
   scormSettings2.bCost                   = bCost;
   scormSettings2.bCopyright              = bCopyright;
   scormSettings2.bFulltextAsKeyword      = bFulltextAsKeyword;

   _tcscpy(scormSettings2.m_tszGnrlCeCatalog, m_tszGnrlCeCatalog);
   _tcscpy(scormSettings2.m_tszGnrlCeEntry, m_tszGnrlCeEntry);
   */

   for (int i=0; i<SCORM_PURPOSE_ARRAY_SIZE; ++i)
   {
      _tcscpy(scormSettings2.m_atszClsfDescription[i], m_atszClsfDescription[i]);
      _tcscpy(scormSettings2.m_atszClsfKeyword[i], m_atszClsfKeyword[i]);
   }

   scormSettings2.m_nPurposeIndex = m_nPurposeIndex;

   return S_OK;
}

CScormSettings2::~CScormSettings2()
{
   // nothing to be done everything is a primitive type or a stack object
}

HRESULT CScormSettings2::ReadRegistry()
{
   HRESULT hr = S_OK;

   SCORMSETTINGS scormSettings;
   ZeroMemory(&scormSettings, sizeof SCORMSETTINGS);

   if (SUCCEEDED(hr))
   {
      DWORD dwSize = sizeof SCORMSETTINGS;
      if (true == LRegistry::ReadSettingsBinary(_T("Publisher\\SCORM"), _T("Settings"),
         (char *) &scormSettings, &dwSize))
      {
         if (scormSettings.cbSize == sizeof SCORMSETTINGS)
            m_ScormSettingsOld = scormSettings;
         else
            hr = E_FAIL;

      }
      else
         hr = E_FAIL;
   }
   
   if (SUCCEEDED(hr))
   {
      // sync old and new settings; 
      // the strings are always in-sync as they point to the same memory location
      bCost = m_ScormSettingsOld.bCost;
      bCopyright = m_ScormSettingsOld.bCopyright;
      bFulltextAsKeyword = m_ScormSettingsOld.bFulltextAsKeyword;
   }

   if (SUCCEEDED(hr))
   {
      DWORD dwCatalogSize = sizeof m_tszGnrlCeCatalog;
      DWORD dwEntrySize = sizeof m_tszGnrlCeEntry;

      // Return value is unimportant here
      // because keys may or may not be present in the registry
      LRegistry::ReadSettingsString(_T("Publisher\\SCORM"), _T("GeneralCeCatalog"), 
         m_tszGnrlCeCatalog, &dwCatalogSize, NULL);
      LRegistry::ReadSettingsString(_T("Publisher\\SCORM"), _T("GeneralCeEntry"), 
         m_tszGnrlCeEntry, &dwEntrySize, NULL);
   }

   if (SUCCEEDED(hr))
   {
      for (int i=0; i<SCORM_PURPOSE_ARRAY_SIZE; ++i)
      {
         DWORD dwClsfDesriptionSize = sizeof m_atszClsfDescription[i];
         DWORD dwClsfKeywordSize = sizeof m_atszClsfKeyword[i];

         // Return value is unimportant here
         // because keys may or may not be present in the registry
         _TCHAR tszKeyName[512];
         _tcscpy(tszKeyName, m_atszRegistryClsfNames[i]);
         _tcscat(tszKeyName, _T("Description"));
         LRegistry::ReadSettingsString(_T("Publisher\\SCORM"), tszKeyName, 
            m_atszClsfDescription[i], &dwClsfDesriptionSize, NULL);

         _tcscpy(tszKeyName, m_atszRegistryClsfNames[i]);
         _tcscat(tszKeyName, _T("Keyword"));
         LRegistry::ReadSettingsString(_T("Publisher\\SCORM"), tszKeyName, 
            m_atszClsfKeyword[i], &dwClsfKeywordSize, NULL);
      }
   }

   if (SUCCEEDED(hr))
   {
      // Read the purpose index
      DWORD dwPurposeIndex;
      LRegistry::ReadSettingsDword(_T("Publisher\\SCORM"), _T("ClsfPurposeIndex"), 
         &dwPurposeIndex, 0, NULL);
      m_nPurposeIndex = (int)dwPurposeIndex;
   }

   return hr;
}

HRESULT CScormSettings2::WriteRegistry()
{
   HRESULT hr = S_OK;

   if (SUCCEEDED(hr))
   {
      // sync old and new settings; 
      // the strings are always synced as they point to the same memory location
      m_ScormSettingsOld.bCost = bCost;
      m_ScormSettingsOld.bCopyright = bCopyright;
      m_ScormSettingsOld.bFulltextAsKeyword = bFulltextAsKeyword;
   }

   if (SUCCEEDED(hr))
   {
      if (false == LRegistry::WriteSettingsBinary(_T("Publisher\\SCORM"), _T("Settings"), 
         (char *) &m_ScormSettingsOld, sizeof SCORMSETTINGS))
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
   {
      // if (strlen(szGnrlCatalog) > 0) not needed as we may want 
      // to reset the registry entry with an emtpy string

      if (false == LRegistry::WriteSettingsString(_T("Publisher\\SCORM"), 
         _T("GeneralCeCatalog"), m_tszGnrlCeCatalog))
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
   {
      if (false == LRegistry::WriteSettingsString(_T("Publisher\\SCORM"), 
         _T("GeneralCeEntry"), m_tszGnrlCeEntry))
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
   {
      for (int i=0; i<SCORM_PURPOSE_ARRAY_SIZE; ++i)
      {
         // the key names may look like for example 
         // ClsfDisciplineDescription, ClsfDisciplineKeyword
         
         _TCHAR tszKeyName[512];
         _tcscpy(tszKeyName, m_atszRegistryClsfNames[i]);
         _tcscat(tszKeyName, _T("Description"));
         LRegistry::WriteSettingsString(_T("Publisher\\SCORM"), 
            tszKeyName, m_atszClsfDescription[i]);
         
         _tcscpy(tszKeyName, m_atszRegistryClsfNames[i]);
         _tcscat(tszKeyName, _T("Keyword"));
         LRegistry::WriteSettingsString(_T("Publisher\\SCORM"), 
            tszKeyName, m_atszClsfKeyword[i]);
      }
   }
 
   if (SUCCEEDED(hr))
   {
      DWORD dwPurposeIndex = m_nPurposeIndex;
      if (false == LRegistry::WriteSettingsDword(_T("Publisher\\SCORM"), 
         _T("ClsfPurposeIndex"), dwPurposeIndex))
         hr = E_FAIL;
   }


   return hr;
}

bool MaskCharacters(CString& csInput)
{
   bool bSomethingChanged = false;
   for (int i=0; i<csInput.GetLength(); ++i)
   {
      if (csInput[i] == _T(';') || csInput[i] == _T('\\'))
      {
         csInput.Insert(i, _T('\\'));
         ++i;
         bSomethingChanged = true;
      }
   }

   return bSomethingChanged;
}

CString CScormSettings2::GetRepresentation()
{
#ifndef _UNICODE
   will not compile without _UNICODE;
#endif

   CString csFormat;
   csFormat += _T("%S;%S;%S;%d;%d;%d;%s;%s;%d;%d;");

   CString csCatalog = m_tszGnrlCeCatalog;
   MaskCharacters(csCatalog);
   CString csEntry = m_tszGnrlCeEntry;
   MaskCharacters(csEntry);

   CString csReturn;
   csReturn.Format(csFormat, szLanguage, szVersion, szStatus, bFulltextAsKeyword ? 1 : 0,
      bCost ? 1 : 0, bCopyright ? 1 : 0, csCatalog, csEntry, m_nPurposeIndex,
      SCORM_PURPOSE_ARRAY_SIZE); // as some sort of "version" identifier

   for (int i=0; i<SCORM_PURPOSE_ARRAY_SIZE; ++i)
   {
      CString csDescription = m_atszClsfDescription[i];
      MaskCharacters(csDescription);
      CString csKeyword = m_atszClsfKeyword[i];
      MaskCharacters(csKeyword);

      CString csAppend;
      csAppend.Format(_T("%s;%s;"), csDescription, csKeyword);

      csReturn += csAppend;
   }

   return csReturn;
}

CString GetNextToken(CString& csInput, int& iStartPos)
{
   int iEndPos = -1;

   for (int i=iStartPos; i<csInput.GetLength(); ++i)
   {
      if (csInput[i] == _T('\\'))
         ++i; // ignore next
      else if (csInput[i] == _T(';'))
      {
         iEndPos = i;
         break;
      }
   }

   if (iEndPos == -1) // no ';' found
      iEndPos = csInput.GetLength();

   int iLength = iEndPos - iStartPos;

   CString csReturn(((LPCTSTR)csInput) + iStartPos, iLength);

   for (int i=0; i<csReturn.GetLength(); ++i)
   {
      if (csReturn[i] == _T('\\'))
         csReturn.Delete(i); // loop does continue with the one after the next character
      else if (csReturn[i] == _T(';'))
         break;
   }

   iStartPos = iEndPos + 1;
   return csReturn;
}

HRESULT CScormSettings2::ParseFrom(CString& csRepresentation)
{
#ifndef _UNICODE
   will not compile without _UNICODE;
#endif

   int iStartPos = 0;

   CString csToken = GetNextToken(csRepresentation, iStartPos);
   WideCharToMultiByte(CP_ACP, 0, csToken, -1, szLanguage, -1, NULL, NULL);
   csToken = GetNextToken(csRepresentation, iStartPos);
   WideCharToMultiByte(CP_ACP, 0, csToken, -1, szVersion, -1, NULL, NULL);
   csToken = GetNextToken(csRepresentation, iStartPos);
   WideCharToMultiByte(CP_ACP, 0, csToken, -1, szStatus, -1, NULL, NULL);
   csToken = GetNextToken(csRepresentation, iStartPos);
   bFulltextAsKeyword = csToken == _T("1");
   csToken = GetNextToken(csRepresentation, iStartPos);
   bCost = csToken == _T("1");
   csToken = GetNextToken(csRepresentation, iStartPos);
   bCopyright = csToken == _T("1");
   csToken = GetNextToken(csRepresentation, iStartPos);
   _tcscpy_s(m_tszGnrlCeCatalog, SCORM_GNRL_CE_CATALOG_SIZE, csToken);
   csToken = GetNextToken(csRepresentation, iStartPos);
   _tcscpy_s(m_tszGnrlCeEntry, SCORM_GNRL_CE_ENTRY_SIZE, csToken);
   csToken = GetNextToken(csRepresentation, iStartPos);
   m_nPurposeIndex = _ttoi(csToken);
   csToken = GetNextToken(csRepresentation, iStartPos);
   int iDelimiter = _ttoi(csToken);

   if (iDelimiter != SCORM_PURPOSE_ARRAY_SIZE)
      return E_UNEXPECTED; // something is wrong with the string

   for (int i=0; i<SCORM_PURPOSE_ARRAY_SIZE; ++i)
   {
      csToken = GetNextToken(csRepresentation, iStartPos);
      _tcscpy_s(m_atszClsfDescription[i], SCORM_CLSF_DESCRIPTION_SIZE, csToken);
      csToken = GetNextToken(csRepresentation, iStartPos);
      _tcscpy_s(m_atszClsfKeyword[i], SCORM_CLSF_KEYWORD_SIZE, csToken);
   }

   return S_OK;
}
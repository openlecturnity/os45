#ifndef __TEMPLATEENGINE_H__F59E6750_B79C_4cf6_8711_B35FF67CCB00
#define __TEMPLATEENGINE_H__F59E6750_B79C_4cf6_8711_B35FF67CCB00

#include "TemplateKeywords.h"

//#define E_NO_TEMPLATE _HRESULT_TYPEDEF_(0x80a40001L)

class SgmlFile;
class SgmlElement;
class LTextBuffer;

class CKeywordReplace
{
public:
   CKeywordReplace() {}
   CKeywordReplace(const _TCHAR *szKeyword, const _TCHAR *szReplace);

   CString csKeyword;
   CString csReplace;
};

class CTemplateEngine// : public CWinApp
{
   // friend class CSelectTemplatePage;

public:
   CTemplateEngine();
   ~CTemplateEngine();

   UINT Init();
   UINT CleanUp();
   UINT ReadSettings();
   UINT ReadSettingsFromFile(const _TCHAR *tszPath, bool bOverride);
   UINT WriteSettings(bool bForAllUsers, bool bSavePath);
   UINT GetSettings(_TCHAR *tszSettings, DWORD *pdwSize);
   UINT SetSettings(const _TCHAR *tszSettings);
   UINT GetCurrentTemplatePath(_TCHAR *tszPath, DWORD *pdwSize);

   UINT CheckTemplate(CString csPath, CString& csTitle);
   UINT LoadTemplate(const _TCHAR *tszTemplate, int nReset, bool bDry, CString& csTitleOut); // =TEMPLATE_RESET_NORESET
   UINT CloseTemplate();

   UINT Configure(HWND hWndParent, bool bDoNotSave);
   UINT AddKeyword(const _TCHAR *tszKeyword, const _TCHAR *tszReplace);
   UINT GetTitle(_TCHAR *tszTitle, DWORD *pdwSize);
   UINT GetDescription(_TCHAR *tszName, DWORD *pdwSize);
   UINT GetAuthor(_TCHAR *tszAuthor, DWORD *pdwSize);
   UINT GetCopyright(_TCHAR *tszCopyright, DWORD *pdwSize);
   UINT GetPreviewImage(_TCHAR *tszFileName, DWORD *pdwSize);
   UINT IsScormCompliant(BOOL *pbCompliant);
   UINT IsUtfTemplate(BOOL *pbUtf);
   UINT DoParse(HWND hWndParent);
   UINT SetScormEnabled(bool bScormEnabled);
   UINT SetTargetDirectory(const _TCHAR *tszDirectory);
   UINT GetTargetDirectory(_TCHAR *tszDirectory, DWORD *pdwSize);
   UINT GetFileCount(DWORD *pdwCount);
   UINT GetFile(UINT nIndex, _TCHAR *tszFileName, DWORD *pdwSize);
   UINT GetIndexFile(_TCHAR *tszFileName, DWORD *pdwSize);

   UINT SetDocumentType(const _TCHAR *tszDocType);
   UINT SetTargetFormat(const _TCHAR *tszTargetFormat);

   UINT GetMaxSlidesWidth(int nScreenWidth, int nVideoWidth, int nStillImageWidth, int *pMaxSlidesWidth);
   UINT GetMaxSlidesHeight(int nScreenHeight, int nVideoHeight, int nStillImageHeight, int *pMaxSlidesHeight);

   UINT GetGlobalSetting(const _TCHAR *tszSettingsName, _TCHAR *tszSetting, DWORD *pdwSize);
   
   UINT GetStandardNavigationStates(int *aiStandardNavigationStates);
   UINT PutStandardNavigationStatesToLrd(const _TCHAR *tszLrdFile, int *aiStandardNavigationStates);

   // Not part of the external interface
   void GetTemplateKeywords(CArray<CTemplateKeyword *, CTemplateKeyword *> &caKeywords);
   void SetTemplateChanged(bool bChanged=true) { m_bHasTemplateChanged = bChanged; }
   bool HasTemplateChanged() { return m_bSaveToRegistry && m_bHasTemplateChanged; }
   void UpdateStandardNavigationStates();
   void SetStandardNavigationStates(int *aiStandardNavigationStates);

   /**
    * In case of an Cancel: Remove all yet written/copied files.
    */
   void DeleteAllWrittenFiles();

   void OverrideCurrentPath(const _TCHAR *tszPath = NULL, bool bReset = false);

   bool IsSaveToRegistry() { return m_bSaveToRegistry; }

private:
   //bool IsConfigurationValidForTemplate(SgmlElement *pRoot);

   void StoreKeywordConfiguration(bool bForAllUsers);
   void ReadConfigurableKeywords(SgmlElement *pConfigVars);
   void ReadGlobalSettings(SgmlElement *pGlobalSettings);
   void ConfigureConfigurableKeywords(int nReset);
   void ReplaceAllKeywords(LTextBuffer &lBuffer);
   bool EvaluateKeywords(SgmlElement *pKeywords);
   bool HandleStaticFiles(SgmlElement *pStaticFiles);
   bool HandleCommonFiles(SgmlElement *pScormFiles);
   bool HandleTemplateFiles(SgmlElement *pTemplateFiles);
   void CheckLastSlash(CString &csPath);
   bool GetFileNames(SgmlElement *pFile, CString &csSourceFile, CString &csTargetFile, bool bTemplate, bool bScormFiles);
   void AddMissingFile(CString csFile);
   bool FileExists(LPCTSTR tszFile);
   int  FindInArray(CArray<CString, CString> &fileList, CString csSearch);

   CArray<CKeywordReplace, CKeywordReplace> m_keywordList;
   CArray<CString, CString> m_writtenFiles;
   
   bool m_bSaveToRegistry; // in profile mode nothing should be saved
   CString m_csProfileTemplatePath;

   SgmlFile *m_pConfig;
   // Template information
   CString m_csId;
   CString m_csSourcePath;
   CString m_csCommonSourcePath;
   CString m_csTitle;
   CString m_csDescription;
   CString m_csAuthor;
   CString m_csCopyright;
   CString m_csPreviewImageFileName;
   // Formulas!
   CString m_csMaxSlidesWidth;
   CString m_csMaxSlidesHeight;
   bool    m_bIsScormCompliant;
   bool    m_bScormEnabled;
   bool    m_bIsUtfTemplate;
   bool    m_bHasTemplateChanged;
   CArray<CTemplateKeyword *, CTemplateKeyword *> m_aConfigKeywords;

   // Runtime information
   CString m_csDocType;
   CString m_csTargetFormat;
   CString m_csTargetPath;
   CString m_csIndexFileName;
   bool m_bIsTargetTemporary;

   void MakeErrorMessage(UINT nMsgId, const _TCHAR *tszParam = NULL);
   CString m_csErrorMessage;

   CArray<CString, CString> m_csaGlobalSettingsNames;
   CArray<CString, CString> m_csaGlobalSettingsValues;

   // 8 'Standard Navigation' states
   int *m_aiStandardNavigationStates;
};

#endif // __TEMPLATEENGINE_H__F59E6750_B79C_4cf6_8711_B35FF67CCB00

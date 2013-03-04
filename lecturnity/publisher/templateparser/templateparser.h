#ifndef __TEMPLATEPARSER_H__314AD9BB_963E_4a31_8DDC_C1D313055273
#define __TEMPLATEPARSER_H__314AD9BB_963E_4a31_8DDC_C1D313055273

#include "ltextbuffer.h"
#include "selecttemplatepage.h"

class SgmlElement;

class CKeywordReplace
{
public:
   CKeywordReplace() {}
   CKeywordReplace(const char *szKeyword, const char *szReplace);

   CString csKeyword;
   CString csReplace;
};

class CTemplateParserApp : public CWinApp
{
   friend class CSelectTemplatePage;

public:
   CTemplateParserApp();
   ~CTemplateParserApp();
   
   void AddKeyword(const char *szKeyword, const char *szReplace);
   void SetDocumentType(const char *szDocType);
   void SetTargetFormat(const char *szTargetFormat);
   void SetTargetPath(const char *szTargetPath);
   void SetTemplateSourcePath(const char *szSourcePath);
   bool HasTemplatePath();
   void SelectTemplatePath();
   int  WriteHtmlFiles();

   /**
    * In case of an Cancel: Remove all yet written/copied files.
    */
   void DeleteAllWrittenFiles();

private:
   bool IsConfigurationValidForTemplate(SgmlElement *pRoot);
   void ReplaceAllKeywords(LTextBuffer &lBuffer);
   bool EvaluateKeywords(SgmlElement *pKeywords);
   bool HandleStaticFiles(SgmlElement *pStaticFiles);
   bool HandleTemplateFiles(SgmlElement *pTemplateFiles);
   void CheckLastSlash(CString &csPath);
   bool GetFileNames(SgmlElement *pFile, CString &csSourceFile, CString &csTargetFile, bool bTemplate);
   void AddMissingFile(CString csFile);
   bool FileExists(LPCTSTR szFile);
   int  FindInArray(CArray<CString, CString> &fileList, CString csSearch);

   CArray<CKeywordReplace, CKeywordReplace> m_keywordList;
   CArray<CString, CString> m_writtenFiles;

   CString m_csDocType;
   CString m_csTargetFormat;
   CString m_csTargetPath;
   CString m_csSourcePath;

   void MakeErrorMessage(UINT nMsgId, const _TCHAR *szParam = NULL);
   CString m_csErrorMessage;
};

#endif // __TEMPLATEPARSER_H__314AD9BB_963E_4a31_8DDC_C1D313055273
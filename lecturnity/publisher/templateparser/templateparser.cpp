// templateparser.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//

#include "stdafx.h"
// Internal includes
#include "templateparser.h"
#include "resource.h"
#include "java_templateparser.h"
#include "selecttemplatepage.h"

// External includes
#include "SgmlParser.h" // SgmlFile, SgmlElement
#include "lexpressionparser.h" // LExpressionParser
#include "lio.h" // LIo
#include "lregistry.h" // LRegistry

//extern const char *szLECTURNITY;

CTemplateParserApp dllApp;



/* ------------------------------------------------------- */

int convertUTFCharacters(JNIEnv *env, jstring jString, _TCHAR** pszString)
{
   const WCHAR * wszTemp = env->GetStringChars(jString, NULL);
   int nLen = wcslen(wszTemp);
   *pszString = new _TCHAR[nLen + 1];
#ifdef _UNICODE
   wcscpy(*pszString, wszTemp);
#else
   int nTemp = WideCharToMultiByte(CP_ACP, 0, wszTemp, -1, *pszString, nLen + 1, NULL, NULL);
#endif
   env->ReleaseStringChars(jString, wszTemp);
   nTemp = _tcslen(*pszString);
   return nTemp;
}

/* ------------------------------------------------------- */

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_StreamingMediaConverter_parserAddKeyword
  (JNIEnv *env, jclass, jstring jKeyword, jstring jReplace)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
   char *szKeyword = NULL;
   char *szReplace = NULL;
   convertUTFCharacters(env, jKeyword, &szKeyword);
   convertUTFCharacters(env, jReplace, &szReplace);

   dllApp.AddKeyword(szKeyword, szReplace);

   delete[] szKeyword;
   delete[] szReplace;
}

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_StreamingMediaConverter_parserSetDocumentType
  (JNIEnv *env, jclass, jstring jDocType)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   char *szDocType = NULL;
   convertUTFCharacters(env, jDocType, &szDocType);

   dllApp.SetDocumentType(szDocType);

   delete[] szDocType;
}

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_StreamingMediaConverter_parserSetTargetFormat
  (JNIEnv *env, jclass, jstring jTargetFormat)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   char *szTargetFormat = NULL;
   convertUTFCharacters(env, jTargetFormat, &szTargetFormat);

   dllApp.SetTargetFormat(szTargetFormat);

   delete[] szTargetFormat;
}

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_StreamingMediaConverter_parserSetTargetPath
  (JNIEnv *env, jclass, jstring jTargetPath)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   char *szTargetPath = NULL;
   convertUTFCharacters(env, jTargetPath, &szTargetPath);

   dllApp.SetTargetPath(szTargetPath);

   delete[] szTargetPath;
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_converter_StreamingMediaConverter_parserHasTemplatePath
  (JNIEnv *, jclass)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   return (jboolean) dllApp.HasTemplatePath();
}

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_StreamingMediaConverter_parserSelectTemplatePath
  (JNIEnv *, jclass)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   dllApp.SelectTemplatePath();
}

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_StreamingMediaConverter_parserWriteHtmlFiles
  (JNIEnv *, jobject)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
   return dllApp.WriteHtmlFiles();
}

JNIEXPORT void JNICALL Java_imc_lecturnity_converter_StreamingMediaConverter_parserDeleteAllWrittenFiles
  (JNIEnv *, jclass)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
   dllApp.DeleteAllWrittenFiles();
}

CKeywordReplace::CKeywordReplace(const char *szKeyword, const char *szReplace)
{
   csKeyword = szKeyword;
   csReplace = szReplace;
}

/* ------------------------------------------------------- */

CTemplateParserApp::CTemplateParserApp()
{
   m_keywordList.RemoveAll();
   m_writtenFiles.RemoveAll();
   m_csDocType = "";
   m_csErrorMessage = "";
   m_csSourcePath = "";
   m_csTargetPath = "";
   m_csTargetFormat = "";

   char szLastTemplate[_MAX_PATH];
   DWORD dwSize = _MAX_PATH;
   char *szDefault = "<undefined>";
   LRegistry::ReadSettingsString(_T("Publisher\\Templates"), _T("LastTemplate"),
      szLastTemplate, &dwSize, szDefault);
   CString csLastTemplate(szLastTemplate);
   if (csLastTemplate != szDefault)
   {
      m_csSourcePath = csLastTemplate;
   }
   else
   {
      char szInstallDir[_MAX_PATH];
      DWORD dwSize = _MAX_PATH;
      bool success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
         _T("Software\\imc AG\\LECTURNITY"), _T("InstallDir"),
         szInstallDir, &dwSize);
      if (success)
      {
         CString csInstallDir(szInstallDir);
         CheckLastSlash(csInstallDir);
         CString csTemplateDir = csInstallDir + CString(_T("Publisher\\Web Templates\\WebPlayer1\\"));
         LRegistry::WriteSettingsString(_T("Publisher\\Templates"), _T("LastTemplate"),
            csTemplateDir);
         m_csSourcePath = csTemplateDir;
      }
   }
}

CTemplateParserApp::~CTemplateParserApp()
{
}

void CTemplateParserApp::AddKeyword(const char *szKeyword, const char *szReplace)
{
   bool bFound = false;
   int nIndex = 0;
   for (int i=0; i<m_keywordList.GetSize(); ++i)
   {
      if (m_keywordList[i].csKeyword == szKeyword)
      {
         nIndex = i;
         bFound = true;
      }
   }

   // We do not want doubly defined keywords.
   if (bFound)
      m_keywordList.RemoveAt(nIndex);

   m_keywordList.Add(CKeywordReplace(szKeyword, szReplace));
}

/* ------------------------------------------------------- */

void CTemplateParserApp::SetDocumentType(const char *szDocType)
{
   m_csDocType = szDocType;
}

/* ------------------------------------------------------- */

void CTemplateParserApp::SetTargetFormat(const char *szTargetFormat)
{
   m_csTargetFormat = szTargetFormat;
}

/* ------------------------------------------------------- */

void CTemplateParserApp::SetTargetPath(const char *szTargetPath)
{
   m_csTargetPath = szTargetPath;
   CheckLastSlash(m_csTargetPath);
}

/* ------------------------------------------------------- */

void CTemplateParserApp::SetTemplateSourcePath(const char *szSourcePath)
{
   m_csSourcePath = szSourcePath;
   CheckLastSlash(m_csSourcePath);
}

void CTemplateParserApp::CheckLastSlash(CString &csPath)
{
   if (csPath[csPath.GetLength() - 1] != '\\')
      csPath += CString(_T("\\"));
}

/* ------------------------------------------------------- */

bool CTemplateParserApp::HasTemplatePath()
{
   return (m_csSourcePath != _T(""));
}

/* ------------------------------------------------------- */

void CTemplateParserApp::SelectTemplatePath()
{
   CString csTitle;
   csTitle.LoadString(IDS_TEMPLATE);
   CPropertySheet ps(csTitle);
   CSelectTemplatePage stp;
   ps.AddPage(&stp);
   ps.DoModal();
}

/* ------------------------------------------------------- */

int CTemplateParserApp::WriteHtmlFiles()
{
   int res = imc_lecturnity_converter_StreamingMediaConverter_SUCCESS;

   SgmlFile sgmlConfig;
   CString csConfigFileName = m_csSourcePath + _T("config.xml");
   bool success = sgmlConfig.Read(csConfigFileName, "<templateConfig");
   if (!success)
   {
      // TPERR_FILE_NOT_FOUND
      // Die Datei '%s' wurde nicht gefunden, oder sie ist fehlerhaft.
      // The file '%s' was not found, or it is corrupt.
      MakeErrorMessage(TPERR_FILE_NOT_FOUND, csConfigFileName);
   }

   SgmlElement *pRoot = NULL;
   SgmlElement *pDocRoot = NULL;
   SgmlElement *pTypeRoot = NULL;

   if (success)
   {
      pRoot = sgmlConfig.GetRoot();
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

   // Evaluate the keywords for the entire template
   if (success)
   {
      SgmlElement *pKeywords = pRoot->Find("replaceKeywords");
      if (pKeywords)
      {
         success = EvaluateKeywords(pKeywords);
         // Note: An error message may have been issued in EvaluateKeywords
      }
   }

   // Evaluate the keywords for the document type
   if (success)
   {
      SgmlElement *pKeywords = pDocRoot->Find("replaceKeywords");
      if (pKeywords)
      {
         success = EvaluateKeywords(pKeywords);
         // Note: An error message may have been issued in EvaluateKeywords
      }
   }

   // Now evaluate the keywords for the target type
   if (success)
   {
      SgmlElement *pKeywords = pTypeRoot->Find("replaceKeywords");
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
      SgmlElement *pStaticFiles = pDocRoot->Find("staticFiles");
      if (pStaticFiles)
         success = HandleStaticFiles(pStaticFiles);
   }

   if (success)
   {
      SgmlElement *pTemplateFiles = pDocRoot->Find("templateFiles");
      if (pTemplateFiles)
         success = HandleTemplateFiles(pTemplateFiles);
   }

   if (success)
   {
      SgmlElement *pStaticFiles = pTypeRoot->Find("staticFiles");
      if (pStaticFiles)
         success = HandleStaticFiles(pStaticFiles);
   }

   if (success)
   {
      SgmlElement *pTemplateFiles = pTypeRoot->Find("templateFiles");
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

         success = tbSmil.SaveFile(csSmilDocName);
         if (!success)
         {
            MakeErrorMessage(TPERR_WRITE_TEMPLATE, csSmilDocName);
         }
      }
   }

   if (!success)
   {
      ::MessageBox(::GetForegroundWindow(), m_csErrorMessage, _T("Publisher"), MB_OK | MB_ICONERROR);
      res = imc_lecturnity_converter_StreamingMediaConverter_CANCELLED;
   }

   return res;
}

void CTemplateParserApp::DeleteAllWrittenFiles()
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

bool CTemplateParserApp::EvaluateKeywords(SgmlElement *pKeywords)
{
   bool success = true;

   SgmlElement *pKeyword = pKeywords->Find("keyword");

   while (pKeyword && success)
   {
      const char *szName = pKeyword->GetValue("name");
      const char *szEval = pKeyword->GetValue("evaluate");
      const char *szContent = pKeyword->GetParameter();
      
      if (szName)
      {
         bool bEvaluate = false;
         if (szEval)
            bEvaluate = (stricmp("true", szEval) == 0);
         if (!bEvaluate)
         {
            ///AddKeyword(szName, szContent);
            /// replaced by

            CString csContent;
            csContent = szContent;
            // The content may contain keywords
            LTextBuffer ltbContent(csContent);
            ReplaceAllKeywords(ltbContent);
            csContent = ltbContent.GetString();
            AddKeyword(szName, csContent);
         }
         else
         {
            LTextBuffer lText(szContent);
            ReplaceAllKeywords(lText);
            LExpressionParser lExp(lText.GetString());
            int value = 0;
            success = lExp.EvaluateInt(&value);
            if (!success)
            {
               // TPERR_EVAL_ERROR
               // config.xml: Beim Evaluieren des Schlüsselwortes '%s' ist ein Fehler aufgetreten.
               // config.xml: An error occurred while evaluating the keyword '%s'.
               CString csKeyword(szName);
               MakeErrorMessage(TPERR_EVAL_ERROR, csKeyword);
            }
            if (success)
            {
               CString csReplace;
               csReplace.Format(_T("%d"), value);
               AddKeyword(szName, csReplace);
            }
         }
      }
      
      pKeyword = pKeyword->GetNext();
   }

   return success;
}

void CTemplateParserApp::ReplaceAllKeywords(LTextBuffer &lBuffer)
{
   for (int i=0; i<m_keywordList.GetSize(); ++i)
   {
      lBuffer.ReplaceAll(m_keywordList[i].csKeyword, m_keywordList[i].csReplace);
   }
   lBuffer.Commit();
}

bool CTemplateParserApp::HandleStaticFiles(SgmlElement *pStaticFiles)
{
   bool success = true;

   SgmlElement *pFile = pStaticFiles->Find("file");
   while (pFile && success)
   {
      CString csSource;
      CString csTarget;
      success = GetFileNames(pFile, csSource, csTarget, false);

      m_writtenFiles.Add(csTarget);

      success = LIo::CopyFile(csSource, csTarget);
      if (!success)
      {
         // TPERR_COPY_FAILED
         // config.xml: Beim Kopieren der Datei '%s' ist ein Fehler aufgetreten.
         // config.xml: Copying the file '%s' failed.
         MakeErrorMessage(TPERR_COPY_FAILED, csSource);
      }
      
      pFile = pFile->GetNext();
   }

   return success;
}

bool CTemplateParserApp::HandleTemplateFiles(SgmlElement *pTemplateFiles)
{
   bool success = true;

   SgmlElement *pFile = pTemplateFiles->Find("file");
   while (pFile && success)
   {
      CString csSource;
      CString csTarget;
      success = GetFileNames(pFile, csSource, csTarget, true);

      if (success)
      {
         LTextBuffer ltbSource;
         success = ltbSource.LoadFile(csSource);
         if (!success)
         {
            // TPERR_READ_TEMPLATE
            // config.xml: Die Vorlagen-Datei '%s' konnte nicht gelesen werden.
            // config.xml: Reading the template file '%s' failed.
            MakeErrorMessage(TPERR_READ_TEMPLATE, csSource);
         }
         
         if (success)
         {
            ReplaceAllKeywords(ltbSource);

            m_writtenFiles.Add(csTarget);

            success = ltbSource.SaveFile(csTarget);
            if (!success)
            {
               // TPERR_WRITE_TEMPLATE
               // config.xml: Das Schreiben der Datei '%s' schlug fehl.
               // config.xml: Writing the file '%s' failed.
               MakeErrorMessage(TPERR_WRITE_TEMPLATE, csTarget);
            }
         }
      }

      pFile = pFile->GetNext();
   }

   return success;
}

int CTemplateParserApp::FindInArray(CArray<CString, CString> &fileList, CString csSearch)
{
   for (int i=0; i<fileList.GetSize(); ++i)
   {
      if (fileList[i].Compare(csSearch) == 0)
         return i;
   }

   return -1;
}

bool CTemplateParserApp::GetFileNames(SgmlElement *pFile, CString &csSourceFile, 
                                      CString &csTargetFile, bool bTemplate)
{
   bool success = true;

   const char *szFile = pFile->GetParameter();
   const char *szPrefix = pFile->GetValue("prefix");
   const char *szWriteToRoot = pFile->GetValue("writeToRoot");
   /*
   if (szPrefix && szWriteToRoot)
      printf("<file>: %s, prefix: '%s', writeToRoot: '%s'\n", szFile, szPrefix, szWriteToRoot);
   else if (szPrefix)
      printf("<file>: %s, prefix: '%s'\n", szFile, szPrefix);
   else if (szWriteToRoot)
      printf("<file>: %s, writeToRoot: '%s'\n", szFile, szWriteToRoot);
   */

   bool bWriteToRoot = false;
   if (szWriteToRoot)
      bWriteToRoot = (stricmp("true", szWriteToRoot) == 0);

   bool bHasPrefix = false;
   CString csPrefix;
   if (szPrefix)
   {
      // The prefix may contain keywords
      csPrefix = szPrefix;
      LTextBuffer ltbPrefix(csPrefix);
      ReplaceAllKeywords(ltbPrefix);
      csPrefix = ltbPrefix.GetString();
      bHasPrefix = true;
   }
   
   if (szFile)
   {
      CString csSrcFile;
      csSrcFile = szFile;
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

            CString csMarker("\\");

            // have a look if the path is more than one (new) diretory deep
            int nFirstSlash = csDirs.Find('\\');
            while (nFirstSlash > 0)
            {
               CString csDirSub = csDirs.Mid(0, nFirstSlash);
               CString csFullPath = m_csTargetPath + csDirSub + csMarker;
               if (FindInArray(m_writtenFiles, csFullPath) < 0)
               {
                  m_writtenFiles.Add(csFullPath);
               }
               nFirstSlash = csDirs.Find('\\', nFirstSlash+1);
            }

            // the directory itself
            CString csFullPath = m_csTargetPath + csDirs + csMarker;
            if (FindInArray(m_writtenFiles, csFullPath) < 0)
            {
               m_writtenFiles.Add(csFullPath);
            }


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

         // printf("%s --> %s\n", csSourceFile, csTargetFile);
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

   return success;
}

void CTemplateParserApp::MakeErrorMessage(UINT nMsgId, const _TCHAR *szParam)
{
   if (szParam)
      m_csErrorMessage.Format(nMsgId, szParam);
   else
      m_csErrorMessage.LoadString(nMsgId);
}


bool CTemplateParserApp::FileExists(LPCTSTR szFile)
{
   // unfortunately doesn't work: returns always true

   DWORD dwFileAtts = ::GetFileAttributes(szFile);

   HANDLE hFile = ::CreateFile(szFile, 
      0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

   return INVALID_HANDLE_VALUE != hFile || (dwFileAtts & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

void CTemplateParserApp::AddMissingFile(CString csFile)
{
   // unfortunately doesn't work: as FileExists() doesn't work

   char szFileName[MAX_PATH*2];
   strcpy (szFileName, (LPCTSTR)csFile);

   CArray<CString, CString> nonExistant;

   char *szSuffix = strrchr(szFileName, '\\');
   while(NULL != szSuffix && !FileExists(szFileName))
   {
      // there must be at least one \ remaining in szFile (i.e. c:\)

      CString csNono(szFileName);
      // TODO: Wie geht denn das hier mit der Speicherverwaltung??
      // Diese Variable ist doch nur auf dem Stack.
      nonExistant.Add(csNono);

      szSuffix[0] = '\0';

      szSuffix = strrchr(szFileName, '\\');
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

#include "StdAfx.h"

#include "changeurlengine.h"
#include "resource.h"
#include "ZipProgressDlg.h"

#include "lutils.h"
#include "SgmlParser.h"
#include "ZipArchive.h"

CChangeUrlEngine::CChangeUrlEngine()
{
   m_bTempDirUsed = false;
}

CChangeUrlEngine::~CChangeUrlEngine()
{
   CleanupPrevious();
}

void CChangeUrlEngine::CleanupPrevious()
{
   if (m_bTempDirUsed)
   {
      LIo::DeleteDir(m_csScormTempDir);
      m_bTempDirUsed = false;
   }
}

void CChangeUrlEngine::LogMessage(UINT nID, UINT nBeep)
{
   CString csTmp;
   csTmp.LoadString(nID);
   LogMessage(csTmp, nBeep);
}

void CChangeUrlEngine::LogMessage(UINT nID, LPCTSTR tszParam, UINT nBeep)
{
   CString csTmp;
   csTmp.Format(nID, tszParam);
   LogMessage(csTmp, nBeep);
}

bool CChangeUrlEngine::QuestionMessage(UINT nID)
{
   CString csTmp;
   csTmp.LoadString(nID);
   return QuestionMessage(csTmp);
}

void CChangeUrlEngine::WaitForProgressDlg()
{
#ifndef CHANGEURLCMD
   while (!m_pProgressDlg) ::Sleep(100);
   while (!m_pProgressDlg->m_bInitialized) ::Sleep(100);
#endif
}

UINT CChangeUrlEngine::ExtractZipThread(LPVOID pParam)
{
   CChangeUrlEngine *pEngine = (CChangeUrlEngine *) pParam;
   pEngine->ExtractZip();
   return 0L;
}

void CChangeUrlEngine::ExtractZip()
{
   WaitForProgressDlg();

   TRY
   {
      CZipArchive zipFile;
      
      zipFile.Open(m_csThreadFileName);
      //zipFile.Open(_T("C:\\FJjflkdjfld.zip"));
      
      int iFileCount = zipFile.GetCount();

#ifndef CHANGEURLCMD
      m_pProgressDlg->SetMax(iFileCount);
#endif

      for (int i=0; i<iFileCount && !m_bThreadCancelRequest; ++i)
      {
#ifndef CHANGEURLCMD
         m_pProgressDlg->SetProgress(_T(""), i);
#endif
         zipFile.ExtractFile(i, m_csThreadTmpPath);
      }
   }
   CATCH (CException, e)
   {
      _TCHAR tszMsg[1024];
      DWORD dwSize = 1024;
      e->GetErrorMessage(tszMsg, dwSize);
      LogMessage(IDS_ERR_ZIPERROR, tszMsg, MB_ICONERROR);
      m_bThreadSuccess = false;
   }
   END_CATCH

   if (m_bThreadCancelRequest)
      m_bThreadSuccess = false;

#ifndef CHANGEURLCMD
   m_pProgressDlg->CloseDialog();
#endif
}

UINT CChangeUrlEngine::CreateZipThread(LPVOID pParam)
{
   CChangeUrlEngine *pEngine = (CChangeUrlEngine *) pParam;
   pEngine->CreateZip();
   return 0L;
}

void CChangeUrlEngine::CreateZip()
{
   WaitForProgressDlg();

   bool success = true;
   CString csOutFileName = m_csThreadTmpPath;
   csOutFileName += _T("tmp.zip");

   TRY
   {
      CZipArchive zipFile;
      zipFile.Open(m_csThreadFileName);
      CZipArchive zipOutFile;
      zipOutFile.Open(csOutFileName, CZipArchive::zipCreate);
      
      CZipFileHeader fileHeader;
      
      int iFileCount = zipFile.GetCount();
#ifndef CHANGEURLCMD
      m_pProgressDlg->SetMax(iFileCount);
#endif

      for (int i=0; i<iFileCount && success && !m_bThreadCancelRequest; ++i)
      {
         success = zipFile.GetFileInfo(fileHeader, i);
         CString csOneFile = fileHeader.GetFileName();
         CString csSrcFile = m_csScormTempDir;
         csSrcFile += csOneFile;
#ifndef CHANGEURLCMD
         m_pProgressDlg->SetProgress(csOneFile, i);
#endif
         zipOutFile.AddNewFile(csSrcFile, csOneFile);
      }
      
      zipFile.Close();
      zipOutFile.Close();

      if (!success)
      {
         CString csUnknown;
         csUnknown.LoadString(IDS_UNKNOWN);
         LogMessage(IDS_ERR_ZIPERROR, csUnknown, MB_ICONERROR);
      }
   }
   CATCH (CException, e)
   {
      _TCHAR tszMsg[1024];
      DWORD dwSize = 1024;
      e->GetErrorMessage(tszMsg, dwSize);
      LogMessage(IDS_ERR_ZIPERROR, tszMsg, MB_ICONERROR);
      success = false;
   }
   END_CATCH

   if (success && !m_bThreadCancelRequest)
   {
      success = LIo::CopyFile(csOutFileName, m_csThreadFileName);
      if (!success)
      {
         CString csUnknown;
         csUnknown.LoadString(IDS_UNKNOWN);
         LogMessage(IDS_ERR_ZIPERROR, csUnknown, MB_ICONERROR);
      }
   }
   m_bThreadSuccess = success;

#ifndef CHANGEURLCMD
   m_pProgressDlg->CloseDialog();
#endif
}

void CChangeUrlEngine::RequestZipCancel()
{
   m_bThreadCancelRequest = true;
}

bool CChangeUrlEngine::CheckScormPackage(CString &csPath, CString &csFileName)
{
   _TCHAR tszTempPath[2*_MAX_PATH];
   _TCHAR tszTempDir[2*_MAX_PATH];
   bool success = LIo::CreateTempDir(_T("LLU"), tszTempPath, tszTempDir);
   if (!success)
   {
      // TODO: Error
      return false;
   }

   CString csTmpDir(_T(""));
   if (success)
   {
      csTmpDir = tszTempPath;
      if (csTmpDir.GetAt(csTmpDir.GetLength() - 1) != _T('\\'))
         csTmpDir += _T('\\');
      csTmpDir += tszTempDir;
      csTmpDir += _T('\\');
   }

   if (success)
   {
#ifndef CHANGEURLCMD
      m_pProgressDlg = new CZipProgressDlg(GetCWnd(), this);
#endif

      m_bThreadCancelRequest = false;
      m_bThreadSuccess = true;
      m_csThreadTmpPath = csTmpDir;
      m_csThreadFileName = csPath + csFileName;
#ifndef CHANGEURLCMD
      AfxBeginThread(ExtractZipThread, this);
#else
      ExtractZip();
#endif

#ifndef CHANGEURLCMD
      m_pProgressDlg->DoModal();

      delete m_pProgressDlg;
      m_pProgressDlg = NULL;
#endif
      success = m_bThreadSuccess;
   }

   if (success)
   {
      csPath = csTmpDir;

      if (_tcsnicmp(csFileName.Right(4), _T(".zip"), 4) == 0)
      {
         csFileName.Truncate(csFileName.GetLength()-4);
         csFileName = csFileName + CString(_T(".html"));
      }
      else
         csFileName = _T("dummy.html");

      m_csScormTempDir = csTmpDir;
      m_bTempDirUsed = true;
   }
   else
   {
      LIo::DeleteDir(csTmpDir);
   }

   return success;
}

bool CChangeUrlEngine::ResolveHtmlFileNames()
{
   CleanupPrevious();

   m_nDocType = -1;
   m_bHasClips = false;
   m_nNumberOfClips = 0;
   m_bTempDirUsed = false;
   m_csScormTempDir = _T("");

   bool success = true;

   CString csFile = GetDocumentName();
   if (success && (_taccess(csFile, 06) != 0))
   {
      LogMessage(IDS_ERR_FILENOTFOUND, csFile, MB_ICONERROR);
      return false;
   }

   //m_edFilename.GetWindowText(csFile);
   //CFile cfTmp(csFile, CFile::modeRead);
   //CString csFileName = cfTmp.GetFileName();
   int nLastSlash = csFile.ReverseFind(_T('\\'));
   CString csFileName = csFile.Mid(nLastSlash + 1);
   CString csPath = csFile.Left(csFile.GetLength() - csFileName.GetLength());

   CString csScorm;
   csScorm.LoadString(IDS_NO);
   if (csFileName.Right(4) == _T(".zip"))
   {
#ifndef CHANGEURLCMD
      CWaitCursor wait;
#endif
      if (CheckScormPackage(csPath, csFileName))
         csScorm.LoadString(IDS_YES);
      else
      {
         success = false;
      }
#ifndef CHANGEURLCMD
      wait.Restore();
#endif
   }
   SetScormSupport(csScorm);

   if (success && csFileName.Right(5) != _T(".html"))
   {
      LogMessage(IDS_ERR_NOTHTML, MB_ICONERROR);
      success = false;
   }

   CString csBaseName(_T(""));

   // Try to find out if it's a new-type document (post 1.6.1.p1) or an
   // old-type document. Assume new type if config.xml is available.
   // Each of these cases have to fill the following variables:
   //
   //   - m_nDocType
   //   - m_nHasClips
   //   - m_nNumberOfClips
   //   - csBaseName
   //   - m_csMasterPlayerDocument
   //   - m_csClipPlayersDocument
   //   - m_csIsStandaloneMode
   //   - m_csDocVersion
   //
   // and call the methods
   // 
   //   - SetDocumentName()
   //   - SetDocVersion()
   //
   // Other methods all rely on these variables in order to work
   // correctly.

   // New since 3.0.p1: Configurable parameters in Flash documents 
   CString csFlashJs = csPath + CString(_T("js\\flash.js"));
   if (_taccess(csFlashJs, 06) == 0)
   {
      // Flash document
      m_nDocType = DOCTYPE_FLASH;

      csBaseName = csFileName.Left(csFileName.GetLength() - 5);
      ResolveDocumentNames(csPath, csBaseName);

      m_csMasterPlayerDocument = CString(_T("js\\flash.js"));
      m_csClipPlayersDocument = CString(_T("js\\flash.js"));

      if (!m_bTempDirUsed)
         SetDocumentName(csPath + csBaseName + CString(_T(".html")));
   }
   else 
   {
      // Check for "config.xml" (new-type document (post 1.6.1.p1))
      CString csConfigXml = csPath + CString(_T("config.xml"));
      if (_taccess(csConfigXml, 06) == 0)
      {
         // New style (1.6.1.p1 or later) document
         m_bOldStyleDocument = false;
         // 3.0.p1: This information message is not necessary any more
         //LogMessage(IDS_MSG_NEWTYPE, MB_ICONINFORMATION);

         // Let's read out the base file name from the variables.js file
         // csBaseName = csFileName.Left(csFileName.GetLength() - 5);

         // Document of new type (post 1.6.1.p1)
         CString csTmp = csPath + CString(_T("js\\variables.js"));
         if (success && (_taccess(csTmp, 06) != 0))
         {
            LogMessage(IDS_ERR_FILENOTFOUND, csTmp, MB_ICONERROR);
            success = false;
         }

         csTmp = csPath + CString(_T("js\\control_wm.js"));
         if (success && (_taccess(csTmp, 06) != 0))
         {
            csTmp = csPath + CString(_T("js\\control_rm.js"));
            if (success && (_taccess(csTmp, 06) != 0))
            {
               LogMessage(IDS_ERR_NORMORWM_NEW, MB_ICONERROR);
               success = false;
            }
            else
            {
               m_nDocType = DOCTYPE_REAL;
            }
         }
         else
         {
            m_nDocType = DOCTYPE_WMT;
         }

         if (success)
         {
            LTextBuffer tbVariables;
            success = tbVariables.LoadFile(csPath + CString(_T("js\\variables.js")));

            _TCHAR tszTmp[512];

            // Let's start by reading out the base file name of this document
            if (success)
            {
               success = tbVariables.FindLineContaining(_T("g_baseDocumentName"), tszTmp);
               if (success)
               {
                  CString csTmp(tszTmp);
                  int nFirstAp = csTmp.Find('\'');
                  if (nFirstAp != -1)
                  {
                     int nSecondAp = csTmp.ReverseFind('\'');
                     if (nSecondAp != -1)
                     {
                        csBaseName = csTmp.Mid(nFirstAp + 1, (nSecondAp - nFirstAp) - 1); 
                     }
                     else
                        success = false;
                  }
                  else 
                     success = false;
               }

               if (!success)
               {
                  // Fall back to the name of the document HTML file
                  // In this case, the program probably will fail anyway
                  // when the replacements take place
                  csBaseName = csFileName.Left(csFileName.GetLength() - 5);
               }
            }

            if (success)
            {
               tbVariables.ResetBufferPointer();
               success = tbVariables.FindLineContaining(_T("g_hasClips"), tszTmp);
            }
            if (success)
            {
               if (_tcsstr(tszTmp, _T("true")) != NULL)
               {
                  m_bHasClips = true;

                  tbVariables.ResetBufferPointer();
                  success = tbVariables.FindLineContaining(_T("g_numOfClips"), tszTmp);
                  if (success)
                  {
                     _TCHAR *tszEqual = _tcschr(tszTmp, (_TCHAR) '=');
                     if (tszEqual != NULL)
                     {
                        m_nNumberOfClips = _ttoi(tszEqual + 1);
                     }
                     else
                     {
                        LogMessage(IDS_ERR_NUMOFCLIPS, MB_ICONERROR);
                        success = false;
                     }
                  }
                  else
                  {
                     LogMessage(IDS_ERR_NUMOFCLIPS, MB_ICONERROR);
                     success = false;
                  }
               }
               else
               {
                  // If any clips, they're inside the normal
                  // video file. No need to bother.
                  m_bHasClips = false;
               }
            }
            
            if (success)
            {
               tbVariables.ResetBufferPointer();
               success = tbVariables.FindLineContaining(_T("g_isStandaloneMode"), tszTmp);
               if (!success)
               {
                  LogMessage(IDS_ERR_STANDALONEMODE, MB_ICONWARNING);
                  success = true;
               }
               else
               {
                  if (_tcsstr(tszTmp, _T("true")) != NULL)
                  {
                     m_bIsStandaloneMode = true;
                  }
                  else
                  {
                     m_bIsStandaloneMode = false;
                  }
               }
            }
         }

         if (success)
         {
            // Master and Clips players are in the same document
            // for the new type of documents.

            // Possible ToDo: Parse the config.xml file in order
            // to find all the HTML files and then search for the
            // keywords in these files. In future, Clip Players and
            // the Master Player _may_ be in different files, i.e.
            // different frames. Not that probable right now.
            ResolveDocumentNames(csPath, csBaseName);
   //         m_csMasterPlayerDocument = csBaseName;
   //         m_csClipPlayersDocument = csBaseName;
   //         SetDocumentName(csPath + csBaseName + CString(_T(".html")));
         }
      }
      else
      {
         // Old style document (pre 1.6.1.p1) with fixed template
         m_bOldStyleDocument = true;
         // 3.0.p1: This information message is not necessary any more
         //LogMessage(IDS_MSG_OLDTYPE, MB_ICONINFORMATION);

         if (success)
         {
            if (csFileName.Right(17) == _T("_headerframe.html"))
               csBaseName = csFileName.Left(csFileName.GetLength() - 17);
            else if (csFileName.Right(16) == _T("_indexframe.html"))
               csBaseName = csFileName.Left(csFileName.GetLength() - 16);
            else if (csFileName.Right(16) == _T("_videoframe.html"))
               csBaseName = csFileName.Left(csFileName.GetLength() - 16);
            else if (csFileName.Right(15) == _T("_viewframe.html"))
               csBaseName = csFileName.Left(csFileName.GetLength() - 15);
            else
               csBaseName = csFileName.Left(csFileName.GetLength() - 5); // strip .html

            CString csTmp = csPath + csBaseName + CString(_T(".html"));
            if (_taccess(csTmp, 06) != 0)
            {
               LogMessage(IDS_ERR_FILENOTFOUND, csTmp, MB_ICONERROR);
               success = false;
            }
            csTmp = csPath + csBaseName + CString(_T("_videoframe.html"));
            if (success && (_taccess(csTmp, 06) != 0))
            {
               LogMessage(IDS_ERR_FILENOTFOUND, csTmp, MB_ICONERROR);
               success = false;
            }
            csTmp = csPath + csBaseName + CString(_T("_viewframe.html"));
            if (success && (_taccess(csTmp, 06) != 0))
            {
               LogMessage(IDS_ERR_FILENOTFOUND, csTmp, MB_ICONERROR);
               success = false;
            }

            csTmp = csPath + CString(_T("control_all.js"));
            if (success && (_taccess(csTmp, 06) != 0))
            {
               LogMessage(IDS_ERR_FILENOTFOUND, csTmp, MB_ICONERROR);
               success = false;
            }

            csTmp = csPath + CString(_T("control_wm.js"));
            if (success && (_taccess(csTmp, 06) != 0))
            {
               csTmp = csPath + CString(_T("control_rm.js"));
               if (success && (_taccess(csTmp, 06) != 0))
               {
                  LogMessage(IDS_ERR_NORMORWM, MB_ICONERROR);
                  success = false;
               }
               else
               {
                  m_nDocType = DOCTYPE_REAL;
               }
            }
            else
            {
               m_nDocType = DOCTYPE_WMT;
            }
         }

         if (success)
         {
            //m_edFilename.SetWindowText(csPath + csBaseName + CString(_T(".html")));
            SetDocumentName(csPath + csBaseName + CString(_T(".html")));

            LTextBuffer tbControlAll;
            tbControlAll.LoadFile(csPath + CString(_T("control_all.js")));

            _TCHAR tszTmp[512];
            success = tbControlAll.FindLineContaining(_T("isClipsSeparated"), tszTmp);
            if (success)
            {
               if (_tcsstr(tszTmp, _T("true")) != NULL)
               {
                  // Clips separated from video/audio
                  m_bHasClips = true;

                  tbControlAll.ResetBufferPointer();
                  success = tbControlAll.FindLineContaining(_T("numOfClips"), tszTmp);
                  if (success)
                  {
                     _TCHAR *tszEqual = _tcschr(tszTmp, (_TCHAR) '=');
                     if (tszEqual != NULL)
                     {
                        m_nNumberOfClips = _ttoi(tszEqual + 1);
                     }
                     else
                     {
                        LogMessage(IDS_ERR_NUMOFCLIPS, MB_ICONERROR);
                        success = false;
                     }
                  }
                  else
                  {
                     LogMessage(IDS_ERR_NUMOFCLIPS, MB_ICONERROR);
                     success = false;
                  }
               }
               else
               {
                  // If any clips, they're inside the normal
                  // video file. No need to bother.
                  m_bHasClips = false;
               }
            }
            else
            {
               LogMessage(IDS_ERR_READ, _T("control_all.js"), MB_ICONERROR);
               success = false;
            }
         }

         if (success && m_nDocType == DOCTYPE_WMT && m_bHasClips)
         {
            // Check for the popup HTML files
            for (UINT i=0; i<m_nNumberOfClips && success; ++i)
            {
               CString csTmpFormat = csPath + csBaseName + CString(_T("_popup%d.html"));
               CString csTmp;
               csTmp.Format(csTmpFormat, i);
               if (_taccess(csTmp, 06) != 0)
               {
                  LogMessage(IDS_ERR_FILENOTFOUND, csTmp, MB_ICONERROR);
                  success = false;
               }
            }
         }

         // Copy the Master Player document name and Clip Players
         // document name to the appropriate variables
         if (success)
         {
            m_csMasterPlayerDocument = csBaseName + CString(_T("_videoframe"));
            m_csClipPlayersDocument = csBaseName + CString(_T("_viewframe"));
         }

         // Standalone Mode (Screengrabbing mode) does not matter for
         // old style documents. So let's set it to false
         m_bIsStandaloneMode = false;
         // Version is... unknown for this type of document
         m_csDocVersion.LoadString(IDS_UNKNOWN);
         SetDocVersion(m_csDocVersion);
      }
   }

   if (success)
   {
      m_csBaseName = csBaseName;
      m_csPath     = csPath;
      CString csDocType;
      if (m_nDocType == DOCTYPE_FLASH)
      {
         csDocType.LoadString(IDS_FLASH);
      }
      else if (m_nDocType == DOCTYPE_REAL)
      {
         csDocType.LoadString(IDS_REALMEDIA);
      }
      else
      {
         csDocType.LoadString(IDS_WINDOWSMEDIA);
      }
      SetDocType(csDocType);
   }
   else
   {
      CString csDocType;
      csDocType.LoadString(IDS_UNKNOWN);
      SetDocType(csDocType);
      SetDocVersion(csDocType); // Also Unknown
      SetScormSupport(csDocType);
      SetSmilFileName(_T(""));
   }

   return success;
}

bool CChangeUrlEngine::ResolveDocumentNames(const _TCHAR *tszPath, const _TCHAR *tszBaseName)
{
   // Try to find the right file names for the clip HTML file
   // and the master player HTML file. Read the config.xml for that
   // purpose. Check for the SMIL file too, if necessary.
   CString csBaseName = tszBaseName;
   CString csPath     = tszPath;
   CString csIndexFile(_T(""));

   bool success = true;

   SgmlFile configFile;
   CString csConfigFileName = csPath;
   csConfigFileName += _T("\\config.xml");
   success = configFile.Read(csConfigFileName);
   SgmlElement *pRoot = NULL;
   SgmlElement *pDocNode = NULL;
   SgmlElement *pTemplateFiles = NULL;
   SgmlElement *pTemplateFiles2 = NULL;

   if (success)
   {
      pRoot = configFile.GetRoot();
      if (!pRoot)
      {
         // TODO
         success = false;
      }
   }

   if (success)
   {
      if (false == m_bIsStandaloneMode)
         pDocNode = pRoot->Find(_T("normalDocument"));
      else
         pDocNode = pRoot->Find(_T("standaloneDocument"));
      if (!pDocNode)
      {
         // TODO: Error
         success = false;
      }
   }

   if (success)
   {
      pTemplateFiles = pDocNode->Find(_T("templateFiles"));

      SgmlElement *pMediaSpecific = NULL;
      if (m_nDocType == DOCTYPE_REAL)
         pMediaSpecific = pDocNode->Find(_T("realMedia"));
      else
         pMediaSpecific = pDocNode->Find(_T("windowsMedia"));

      if (pMediaSpecific)
      {
         pTemplateFiles2 = pMediaSpecific->Find(_T("templateFiles"));
      }
   }

   _TCHAR tszSourceFile[2*_MAX_PATH];
   _TCHAR tszTargetFile[2*_MAX_PATH];

   m_csMasterPlayerDocument = _T("");
   m_csClipPlayersDocument = _T("");

   if (success && pTemplateFiles)
   {
      SgmlElement *pFileNode = pTemplateFiles->GetSon();
      while (success && pFileNode)
      {
         bool bIsIndex = false;
         success = SgmlFile::ResolveFileNode(pFileNode, tszSourceFile, tszTargetFile, true,
            &bIsIndex, false, NULL, csPath);

         LTextBuffer tbTargetFile(tszTargetFile);
         tbTargetFile.ReplaceAll(_T("%BaseDocumentName%"), csBaseName);
         tbTargetFile.Commit();
         _tcscpy(tszTargetFile, tbTargetFile.GetString()); 

         if (success)
         {
            if (bIsIndex)
               csIndexFile = tszTargetFile;
            CheckFileForPlugins(tszTargetFile, csPath);
         }

         pFileNode = pFileNode->GetNext();
      }
   }

   if (success && pTemplateFiles2)
   {
      SgmlElement *pFileNode = pTemplateFiles2->GetSon();
      while (success && pFileNode)
      {
         bool bIsIndex = false;
         success = SgmlFile::ResolveFileNode(pFileNode, tszSourceFile, tszTargetFile, true,
            &bIsIndex, false, NULL, csPath);

         LTextBuffer tbTargetFile(tszTargetFile);
         tbTargetFile.ReplaceAll(_T("%BaseDocumentName%"), csBaseName);
         tbTargetFile.Commit();
         _tcscpy(tszTargetFile, tbTargetFile.GetString()); 

         if (success)
         {
            if (bIsIndex)
               csIndexFile = tszTargetFile;
            CheckFileForPlugins(tszTargetFile, csPath);
         }

         pFileNode = pFileNode->GetNext();
      }
   }

   if (success)
   {
      if (m_csMasterPlayerDocument == _T("") ||
          (m_bHasClips && m_csClipPlayersDocument == _T("")))
      {
         // TODO: error message "Plugin code not found"
      }
   }

   // Read out the version of LECTURNITY with which this
   // document was created.
   if (csIndexFile == _T(""))
   {
      // If we couldn't find the index file name above,
      // try to guess it.
      csIndexFile = csPath + csBaseName;
      csIndexFile += _T(".html");
   }

   // Do we need to find the SMIL file?
   if (m_nDocType == DOCTYPE_REAL && m_bHasClips)
   {
      // Oh, yeah.
      CString csSmilFileName = csPath + csBaseName;
      csSmilFileName += _T(".smi");
      if (_taccess(csSmilFileName, 06) == 00)
      {
         SetSmilFileName(csSmilFileName);
      }
      else
      {
         success = RequestSmilFileName(csSmilFileName);
         if (success)
         {
            SetSmilFileName(csSmilFileName);
         }
         else
         {
            // TODO: Error "SMIL File not found"
         }
      }
   }
   else
   {
      SetSmilFileName(_T("-"));
   }

   // Special case: Flash - success is false because there is no "config.xml"
   if (m_nDocType == DOCTYPE_FLASH)
      success = true;


   if (!m_bTempDirUsed)
      SetDocumentName(csIndexFile);

   LTextBuffer tbIndexFile;
   if (success)
   {
      success = tbIndexFile.LoadFile(csIndexFile);
      if (!success)
      {
         // TODO: error
      }
   }

   // Now get the version info
   if (success)
   {
      _TCHAR tszTmp[4096];
      m_csDocVersion.LoadString(IDS_UNKNOWN);
      const _TCHAR *tszNeedle = _T("imc, lecturnity, version: ");
      success = tbIndexFile.FindLineContaining(tszNeedle, tszTmp, 4096);
      if (success)
      {
         _TCHAR *tszPos = _tcsstr(tszTmp, tszNeedle);
         // tszNeedle has to exist, see above
         tszPos += _tcslen(tszNeedle);
         _TCHAR *tszEnd = _tcsstr(tszPos, _T(","));
         if (tszEnd == NULL)
            tszEnd = _tcsstr(tszPos, _T("\""));
         if (tszEnd == NULL)
            tszEnd = tszPos + _tcslen(tszPos);
         *tszEnd = _TCHAR(0);
         m_csDocVersion = tszPos;
      }
      SetDocVersion(m_csDocVersion);
   }

   return success;
}

void CChangeUrlEngine::CheckFileForPlugins(const _TCHAR *tszFileName, const _TCHAR *tszPath)
{
   LTextBuffer tbFile;
   CString csFileName(tszFileName);
   CString csPath(tszPath);
   CString csStrippedName = csFileName.Mid(csPath.GetLength());
   csStrippedName = csStrippedName.Left(csStrippedName.ReverseFind(_T('.'))); // Strip .html

   bool success = tbFile.LoadFile(tszFileName);
   if (success)
   {
      if (m_nDocType == DOCTYPE_REAL)
      {
         if (tbFile.Find(_T("<embed name=\"PlayerImageWindow\"")))
         {
            m_csMasterPlayerDocument = csStrippedName;
            tbFile.ResetBufferPointer();
         }
         
         if (m_bHasClips && tbFile.Find(_T("<embed name=\"ClipImageWindow0\"")))
         {
            m_csClipPlayersDocument = csStrippedName;
         }
      }
      else
      {
         // DOCTYPE_WMT
         if (tbFile.Find(_T("<object ID=\"MasterPlayer\"")))
         {
            m_csMasterPlayerDocument = csStrippedName;
            tbFile.ResetBufferPointer();
         }
         
         if (m_bHasClips && tbFile.Find(_T("<object id=\"ClipPlayer0\"")))
         {
            m_csClipPlayersDocument = csStrippedName;
         }
      }
   }
}

bool CChangeUrlEngine::RetrieveCurrentType()
{
   bool success = true;

   int nReplayType = -1;

   CString csUrlHttp(_T(""));
   CString csUrlStreaming(_T(""));

   if (m_nDocType == DOCTYPE_FLASH)
   {
      // Flash export
      // only editable for documents created with 3.0.p1 or later
      LTextBuffer tbFlashPlugin;
      // CString csTmp = m_csPath + m_csBaseName + CString(_T("_videoframe.html"));
      CString csTmp = m_csPath + m_csMasterPlayerDocument;
      success = tbFlashPlugin.LoadFile(csTmp);
      if (!success)
         LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);

      _TCHAR tszTmp[512];
      if (success)
      {
         // Set buffer pointer to a suitable position to be able to search for...
         success = tbFlashPlugin.FindLineContaining(_T("streamUrl="), tszTmp);
         if (!success)
            LogMessage(IDS_ERR_NOSTREAMABLEFLASH, csTmp, MB_ICONERROR);
      }

      if (success)
      {
         // Find the URL
         _TCHAR *tszValue = _tcsstr(tszTmp, _T("streamUrl="));
         // Has to be there (see above)

         tszValue += 10;
         _TCHAR *tszAmpersand = _tcschr(tszValue, (_TCHAR) '&');
         if (tszAmpersand != NULL)
         {
            _TCHAR tszUrl[512];
            _tcsncpy(tszUrl, tszValue, tszAmpersand - tszValue);
            tszUrl[tszAmpersand - tszValue] = (_TCHAR) 0;

            if ( (_tcsstr(tszUrl, _T("rtmp://")) != NULL)
                || (_tcsstr(tszUrl, _T("rtsp://")) != NULL)
                ||(_tcsstr(tszUrl, _T("mms://")) != NULL) )
               nReplayType = REPLAY_TYPE_STREAMING;
            else if ((_tcsstr(tszUrl, _T("http://")) != NULL) ||
                     (_tcsstr(tszUrl, _T("https://")) != NULL))
               nReplayType = REPLAY_TYPE_HTTP;
            else // assume local
               nReplayType = REPLAY_TYPE_LOCAL;

            if (nReplayType == REPLAY_TYPE_STREAMING ||
                nReplayType == REPLAY_TYPE_HTTP)
            {
               _TCHAR *tszSlash = _tcsrchr(tszUrl, (_TCHAR) '/');
               if (tszSlash != NULL)
               {
                  tszUrl[tszSlash - tszUrl] = (_TCHAR) 0;

                  if (nReplayType == REPLAY_TYPE_STREAMING)
                     csUrlStreaming = tszUrl;
                  else
                     csUrlHttp = tszUrl;
               }
               else
               {
                  LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
                  success = false;
               }
            }
         }
      }
   }
   else if (m_nDocType == DOCTYPE_REAL)
   {
      // Real Export
      LTextBuffer tbVideoFrame;
      // CString csTmp = m_csPath + m_csBaseName + CString(_T("_videoframe.html"));
      CString csTmp = m_csPath + m_csMasterPlayerDocument + CString(_T(".html"));
      success = tbVideoFrame.LoadFile(csTmp);
      if (!success)
         LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);

      _TCHAR tszTmp[512];
      if (success)
      {
         // Set buffer pointer to a suitable position to be able to search for...
         success = tbVideoFrame.FindLineContaining(_T("<embed name=\"PlayerImageWindow\""), tszTmp);
         if (!success)
            LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
      }
      if (success)
      {
         // ... 'src="'
         success = tbVideoFrame.FindLineContaining(_T("src=\""), tszTmp);
         if (!success)
            LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
      }

      if (success)
      {
         if ( (_tcsstr(tszTmp, _T("rtsp://")) != NULL)
             || (_tcsstr(tszTmp, _T("rtmp://")) != NULL)
             ||(_tcsstr(tszTmp, _T("mms://")) != NULL) )
            nReplayType = REPLAY_TYPE_STREAMING;
         else if ((_tcsstr(tszTmp, _T("http://")) != NULL) ||
                  (_tcsstr(tszTmp, _T("https://")) != NULL))
            nReplayType = REPLAY_TYPE_HTTP;
         else // assume local replay
            nReplayType = REPLAY_TYPE_LOCAL;
      }

      if (success)
      {
         if (nReplayType == REPLAY_TYPE_HTTP)
         {
            _TCHAR *tszSlash = _tcsrchr(tszTmp, (_TCHAR) '/');
            if (tszSlash == NULL)
            {
               LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
               success = false;
            }

            if (success)
            {
               _TCHAR *tszQuote = _tcschr(tszTmp, (_TCHAR) '"');
               if (tszQuote == NULL)
               {
                  LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
                  success = false;
               }
               else
               {
                  _TCHAR tszRtsp[512];
                  _tcsncpy(tszRtsp, tszQuote + 1, tszSlash - tszQuote - 1);
                  tszRtsp[tszSlash - tszQuote - 1] = (_TCHAR) 0;
                  csUrlHttp = tszRtsp;
               }
            }
         }
         else if (nReplayType == REPLAY_TYPE_STREAMING)
         {
            if (!m_bHasClips)
            {
               // No clips, we just have the rtsp:// link
               // to the main audio/video file
               _TCHAR *tszSlash = _tcsrchr(tszTmp, (_TCHAR) '/');
               if (tszSlash == NULL)
               {
                  LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
                  success = false;
               }

               if (success)
               {
                  _TCHAR *tszQuote = _tcschr(tszTmp, (_TCHAR) '"');
                  _TCHAR tszRtsp[512];
                  _tcsncpy(tszRtsp, tszQuote + 1, tszSlash - tszQuote - 1);
                  tszRtsp[tszSlash - tszQuote - 1] = (_TCHAR) 0;
                  csUrlStreaming = tszRtsp;
               }
            }
            else
            {
               if (m_csDocVersion != _T("-") &&
                  m_csDocVersion.Compare(_T("1.7.0")) >= 0)
               {
                  csUrlHttp = _T("");
               }
               else
               {
                  // Complicated; the URL in tszTmp is the rtsp:// URL
                  // for the SMI file

                  _TCHAR *tszSlash = _tcsrchr(tszTmp, (_TCHAR) '/');
                  if (tszSlash == NULL)
                  {
                     LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
                     success = false;
                  }

                  if (success)
                  {
                     _TCHAR *tszQuote = _tcschr(tszTmp, (_TCHAR) '"');
                     _TCHAR tszRtsp[512];
                     _tcsncpy(tszRtsp, tszQuote + 1, tszSlash - tszQuote - 1);
                     tszRtsp[tszSlash - tszQuote - 1] = (_TCHAR) 0;
                     // Replace rtsp:// with http:// for better understanding
                     _tcsncpy(tszRtsp, _T("http://"), 7);
                     csUrlHttp = tszRtsp;
                  }
               }

               // And now for the real rtsp address
               if (success)
               {
                  LTextBuffer tbSmil;
                  //csTmp = m_csPath + m_csBaseName + CString(_T(".smi"));
                  csTmp = m_csSmilFileName;
                  success = tbSmil.LoadFile(csTmp);
                  if (success)
                  {
                     success = tbSmil.FindLineContaining(_T("<video src=\""), tszTmp);
                     if (success)
                     {
                        _TCHAR *tszQuote1 = _tcschr(tszTmp, (_TCHAR) '"');
                        _TCHAR *tszQuote2 = _tcschr(tszQuote1 + 1, (_TCHAR) '"');
                        if (tszQuote2 != NULL)
                        {
                           _TCHAR tszRtsp[512];
                           _tcsncpy(tszRtsp, tszQuote1 + 1, tszQuote2 - tszQuote1 - 1);
                           tszRtsp[tszQuote2 - tszQuote1 - 1] = (_TCHAR) 0;

                           _TCHAR *tszSlash = _tcsrchr(tszRtsp, (_TCHAR) '/');
                           if (tszSlash)
                           {
                              tszRtsp[tszSlash - tszRtsp] = (_TCHAR) 0;
                              csUrlStreaming = tszRtsp;
                           }
                           else
                           {
                              success = false;
                              LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
                           }
                        }
                        else
                        {
                           success = false;
                           LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
                        }
                     }
                     else
                        LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
                  }
                  else
                     LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
               }
            }
         }

      }
   }
   else
   {
      // Windows Media Export
      LTextBuffer tbVideoFrame;
      //CString csTmp = m_csPath + m_csBaseName + CString(_T("_videoframe.html"));
      CString csTmp = m_csPath + m_csMasterPlayerDocument + CString(_T(".html"));
      success = tbVideoFrame.LoadFile(csTmp);

      if (!success)
         LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);

      _TCHAR tszTmp[512];
      if (success)
      {
         // try WM 6.4 parameter name
         success = tbVideoFrame.FindLineContaining(_T("<param name=\"FileName\" value=\""), tszTmp);
         if (!success)
            // since 3.0.p1: try WM 7.0 parameter name
            success = tbVideoFrame.FindLineContaining(_T("<param name=\"URL\" value=\""), tszTmp);
         if (!success)
            LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
      }

      if (success)
      {
         // Find the URL
         _TCHAR *tszValue = _tcsstr(tszTmp, _T("value=\""));
         // Has to be there (see above)

         tszValue += 7;
         _TCHAR *tszQuote = _tcschr(tszValue, (_TCHAR) '"');
         if (tszQuote != NULL)
         {
            _TCHAR tszUrl[512];
            _tcsncpy(tszUrl, tszValue, tszQuote - tszValue);
            tszUrl[tszQuote - tszValue] = (_TCHAR) 0;

            if ( (_tcsstr(tszUrl, _T("mms://")) != NULL)
                || (_tcsstr(tszUrl, _T("rtsp://")) != NULL)
                ||(_tcsstr(tszUrl, _T("rtmp://")) != NULL) )
               nReplayType = REPLAY_TYPE_STREAMING;
            else if ((_tcsstr(tszUrl, _T("http://")) != NULL) ||
                     (_tcsstr(tszUrl, _T("https://")) != NULL))
               nReplayType = REPLAY_TYPE_HTTP;
            else // assume local
               nReplayType = REPLAY_TYPE_LOCAL;

            if (nReplayType == REPLAY_TYPE_STREAMING ||
                nReplayType == REPLAY_TYPE_HTTP)
            {
               _TCHAR *tszSlash = _tcsrchr(tszUrl, (_TCHAR) '/');
               if (tszSlash != NULL)
               {
                  tszUrl[tszSlash - tszUrl] = (_TCHAR) 0;

                  if (nReplayType == REPLAY_TYPE_STREAMING)
                     csUrlStreaming = tszUrl;
                  else
                     csUrlHttp = tszUrl;
               }
               else
               {
                  LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
                  success = false;
               }
            }
         }
         else
         {
            LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
            success = false;
         }
      }
   }

   if (success)
   {
      /*
      m_frmStep2.EnableWindow(TRUE);
      m_rbLocal.EnableWindow(TRUE);
      m_rbHttp.EnableWindow(TRUE);
      m_rbStreaming.EnableWindow(TRUE);

      if (nReplayType == REPLAY_TYPE_LOCAL)
      {
         m_rbLocal.SetCheck(BST_CHECKED);
         OnTypeLocal();
      }
      else if (nReplayType == REPLAY_TYPE_HTTP)
      {
         m_rbHttp.SetCheck(BST_CHECKED);
         OnTypeHttp();
      }
      else if (nReplayType == REPLAY_TYPE_STREAMING)
      {
         m_rbStreaming.SetCheck(BST_CHECKED);
         OnTypeStreaming();
      }

      m_edUrlHttp.SetWindowText(csUrlHttp);
      m_edUrlStreaming.SetWindowText(csUrlStreaming);

      EnableStep3(FALSE);
      */

      //m_edUrlHttp.SetWindowText(csUrlHttp);
      //m_edUrlStreaming.SetWindowText(csUrlStreaming);
      SetUrlHttp(csUrlHttp);
      SetUrlStreaming(csUrlStreaming);
      m_nOriginalReplayType = nReplayType;
      EnableStep2(TRUE, nReplayType);
   }
   else
   {
      DisableControls();
   }

   return success;
}

bool CChangeUrlEngine::UpdateUrls()
{
   // Now we're coming to the marmalade.
   bool success = true;

   CString csUrlHttp = GetUrlHttp();
   CString csUrlStreaming = GetUrlStreaming();

   // Check both Strings on '/' at the end - and remove them (if any - because they are added with the file name)
   csUrlHttp.TrimRight('/');
   csUrlStreaming.TrimRight('/');
   // Exception: Flash - here '/' is added again
   if (m_nDocType == DOCTYPE_FLASH)
   {
      if (csUrlHttp.GetLength() > 0)
         csUrlHttp.AppendChar('/');
      if (csUrlStreaming.GetLength() > 0)
         csUrlStreaming.AppendChar('/');
   }

   UINT nTargetType = GetTargetType();
   if (nTargetType == REPLAY_TYPE_HTTP)
   {
      if (_tcsnicmp(csUrlHttp, _T("http://"), 7) != 0)
      {
         if (m_nDocType == DOCTYPE_WMT)
         {
            if (_tcsnicmp(csUrlHttp, _T("https://"), 8) != 0)
            {
               LogMessage(IDS_ERR_HTTPHASTOBEHTTP, MB_ICONERROR);
               success = false;
            }
         }
         else
         {
            LogMessage(IDS_ERR_HTTPHASTOBEHTTPFORREAL, MB_ICONERROR);
            success = false;
         }
      }
   }
   else if (nTargetType == REPLAY_TYPE_STREAMING)
   {
      bool bDoContinue = true;

      if (m_nDocType == DOCTYPE_FLASH)
      {
         if (_tcsnicmp(csUrlStreaming, _T("rtmp://"), 7) != 0)
         {
            // 3.0.p1: Warning, no error message
            //LogMessage(IDS_ERR_RTMPHASTOBERTMP, MB_ICONERROR);
            //success = false;
            bDoContinue = QuestionMessage(IDS_FLASHSTREAM_SHOULDBE_RTMP);
         }
      }
      else if (m_nDocType == DOCTYPE_REAL)
      {
         if (_tcsnicmp(csUrlStreaming, _T("rtsp://"), 7) != 0)
         {
            // 3.0.p1: Warning, no error message
            //LogMessage(IDS_ERR_RTSPHASTOBERTSP, MB_ICONERROR);
            //success = false;
            bDoContinue = QuestionMessage(IDS_RMSTREAM_SHOULDBE_RTSP);
         }
      }
      else
      {
         // WMT
         if (_tcsnicmp(csUrlStreaming, _T("mms://"), 6) != 0)
         {
            // 3.0.p1: Warning, no error message
            //LogMessage(IDS_ERR_MMSHASTOBEMMS, MB_ICONERROR);
            //success = false;
            bDoContinue = QuestionMessage(IDS_WMTSTREAM_SHOULDBE_MMS);
         }
      }

      if (bDoContinue)
         success = true;
      else
      {
         success = false;
         return success;
      }
   }

   // Let's do a integrity and legality check of the change.
   // That's only important if we have a >=1.7.0 document.
   if (success &&
       (m_csDocVersion != _T("-") &&
        m_csDocVersion.Compare(_T("1.7.0")) >= 0))
   {
      if (m_nDocType == DOCTYPE_FLASH)
      {
         // HTTP == Local in the Flash case
         if (nTargetType == REPLAY_TYPE_HTTP)
            nTargetType = REPLAY_TYPE_LOCAL;
         
         // 3.0.p1: No restriction in editing (SCORM) ZIP files any more
         /*
         if (!m_bTempDirUsed)
         {
            // Non-SCORM case
            // HTTP == Local in the Flash case
            if (nTargetType == REPLAY_TYPE_HTTP)
               nTargetType = REPLAY_TYPE_LOCAL;
         }
         else
         {
            // SCORM case
            if (m_nOriginalReplayType == REPLAY_TYPE_LOCAL ||
                m_nOriginalReplayType == REPLAY_TYPE_HTTP)
            {
               LogMessage(IDS_SCORM_NOT_EDITABLE, MB_ICONERROR);
               success = false;
            }
            else // m_nOriginalReplayType == REPLAY_TYPE_STREAMING
            {
               if (nTargetType != REPLAY_TYPE_STREAMING)
               {
                  LogMessage(IDS_ERR_SCORM_STREAMING, MB_ICONERROR);
                  success = false;
               }
            }
         }
         */
      }
      else if (m_nDocType == DOCTYPE_REAL)
      {
         if (!m_bHasClips)
         {
            // No SMIL File
            // Local and HTTP are equal; relative paths.
            if (nTargetType == REPLAY_TYPE_HTTP)
               nTargetType = REPLAY_TYPE_LOCAL;
            
            // 3.0.p1: No restriction in editing (SCORM) ZIP files any more
            /*
            if (m_bTempDirUsed)
            {
               // SCORM case
               if (m_nOriginalReplayType == REPLAY_TYPE_LOCAL ||
                   m_nOriginalReplayType == REPLAY_TYPE_HTTP)
               {
                  LogMessage(IDS_SCORM_NOT_EDITABLE, MB_ICONERROR);
                  success = false;
               }
               else // m_nOriginalReplayType == REPLAY_TYPE_STREAMING
               {
                  if (nTargetType != REPLAY_TYPE_STREAMING)
                  {
                     LogMessage(IDS_ERR_SCORM_STREAMING, MB_ICONERROR);
                     success = false;
                  }
               }

            }
            else
            {
               // Non-SCORM case
               // Local and HTTP are equal; relative paths.
               if (nTargetType == REPLAY_TYPE_HTTP)
                  nTargetType = REPLAY_TYPE_LOCAL;
            }
            */
         }
         else
         {
            // SMIL File case

            // 3.0.p1: No restriction in editing (SCORM) ZIP files any more
            /*
            if (m_bTempDirUsed)
            {
               // SCORM case
               if ((m_nOriginalReplayType == REPLAY_TYPE_HTTP && nTargetType == REPLAY_TYPE_STREAMING) ||
                   (m_nOriginalReplayType == REPLAY_TYPE_STREAMING && nTargetType == REPLAY_TYPE_HTTP) ||
                   (nTargetType == REPLAY_TYPE_LOCAL))
               {
                  LogMessage(IDS_ERR_SCORM_SERVER, MB_ICONERROR);
                  success = false;
               }
            }
            else
            {
               // Non-SCORM case
               // All options are valid, so ok.
            }
            */
         }
      }
      else
      {
         // m_nDocType == DOCTYPE_WMT
         // HTTP == Local in the WMT case
         if (nTargetType == REPLAY_TYPE_HTTP)
            nTargetType = REPLAY_TYPE_LOCAL;

         // 3.0.p1: No restriction in editing (SCORM) ZIP files any more
         /*
         if (!m_bTempDirUsed)
         {
            // Non-SCORM case
            // HTTP == Local in the WMT case
            if (nTargetType == REPLAY_TYPE_HTTP)
               nTargetType = REPLAY_TYPE_LOCAL;
         }
         else
         {
            // SCORM case
            if (m_nOriginalReplayType == REPLAY_TYPE_LOCAL ||
                m_nOriginalReplayType == REPLAY_TYPE_HTTP)
            {
               LogMessage(IDS_SCORM_NOT_EDITABLE, MB_ICONERROR);
               success = false;
            }
            else // m_nOriginalReplayType == REPLAY_TYPE_STREAMING
            {
               if (nTargetType != REPLAY_TYPE_STREAMING)
               {
                  LogMessage(IDS_ERR_SCORM_STREAMING, MB_ICONERROR);
                  success = false;
               }
            }
         }
         */
      }
   }


   if (success && m_nDocType == DOCTYPE_FLASH)
   {
      // DOCTYPE_FLASH
      LTextBuffer tbFlashJs;
      CString csTmp = m_csPath + m_csMasterPlayerDocument;
      success = tbFlashJs.LoadFile(csTmp);
      if (!success)
         LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);

      CString csReplace;
      if (success)
      {
         switch (nTargetType)
         {
         case REPLAY_TYPE_LOCAL:
            csReplace.Format(_T(""));
            break;
         case REPLAY_TYPE_HTTP:
            csReplace.Format(_T("%s"), csUrlHttp);
            break;
         case REPLAY_TYPE_STREAMING:
            csReplace.Format(_T("%s"), csUrlStreaming);
         }
      }

      if (success) // <object> tag
      {
         success = tbFlashJs.ReplaceAfter(
            _T("streamUrl="),
            _T("&"), csReplace);
         // Enforce enabling the stripping of the .flv suffix 
         success = tbFlashJs.ReplaceAfter(
            _T("streamStripFlvSuffix="),
            _T("&"), _T("true"));
      }

      if (success) // <embed> tag
      {
         success = tbFlashJs.ReplaceAfter(
            _T("streamUrl="),
            _T("&"), csReplace);
         // Enforce enabling the stripping of the .flv suffix 
         success = tbFlashJs.ReplaceAfter(
            _T("streamStripFlvSuffix="),
            _T("&"), _T("true"));
      }

      if (!success)
         LogMessage(IDS_ERR_REPLACE, csTmp, MB_ICONERROR);

      if (success)
      {
         success = tbFlashJs.SaveFile(csTmp, LFILE_TYPE_TEXT_ANSI); // + CString(_T(".new.html")));
         if (!success)
            LogMessage(IDS_ERR_WRITE, csTmp, MB_ICONERROR);
      }
   }
   else if (success && m_nDocType == DOCTYPE_REAL)
   {
      // DOCTYPE_REAL
      LTextBuffer tbVideoFrame;
      //CString csTmp = m_csPath + m_csBaseName + CString(_T("_videoframe.html"));
      CString csTmp = m_csPath + m_csMasterPlayerDocument + CString(_T(".html"));
      CString csSmiRef(_T(""));
      success = tbVideoFrame.LoadFile(csTmp);
      if (success)
      {
         success = tbVideoFrame.Find(_T("<embed name=\"PlayerImageWindow\""));
         if (success)
         {
            if (!m_bHasClips)
            {
               // No Clips, easy case.
               CString csReplace;
               switch (nTargetType)
               {
               case REPLAY_TYPE_LOCAL:
                  csReplace.Format(_T("%s.rm"), m_csBaseName);
                  break;
               case REPLAY_TYPE_HTTP:
                  csReplace.Format(_T("%s/%s.rm"), csUrlHttp, m_csBaseName);
                  break;
               case REPLAY_TYPE_STREAMING:
                  csReplace.Format(_T("%s/%s.rm"), csUrlStreaming, m_csBaseName);
               }

               success = tbVideoFrame.ReplaceAfter(_T("src=\""), _T("\""), csReplace);
               if (!success)
                  LogMessage(IDS_ERR_REPLACE, csTmp, MB_ICONERROR);

               if (success)
               {
                  success = tbVideoFrame.SaveFile(csTmp, LFILE_TYPE_TEXT_ANSI); // + CString(_T(".new.html")));
                  if (!success)
                     LogMessage(IDS_ERR_WRITE, csTmp, MB_ICONERROR);
               }
            }
            else
            {
               // Ok, we have some clips; may be a little bit tricky now.
               // We have to link to the SMI file in the video frame.
               CString csReplace;
               switch (nTargetType)
               {
               case REPLAY_TYPE_LOCAL:
                  csReplace.Format(_T("%s.smi"), m_csBaseName);
                  break;
               case REPLAY_TYPE_HTTP:
                  csReplace.Format(_T("%s/%s.smi"), csUrlHttp, m_csBaseName);
                  break;
               case REPLAY_TYPE_STREAMING:
                  {
                     if (m_csDocVersion != _T("-") &&
                         m_csDocVersion.Compare(_T("1.7.0")) >= 0)
                     {
                        // In versions 1.7.0 and later, we don't need that
                        // strange workaround from below.
                        csReplace.Format(_T("%s/%s.smi"), csUrlStreaming, m_csBaseName);
                     }
                     else
                     {
                        // Very special case: Use the HTTP server URL, but replace
                        // http:// with rtsp://
                        if (_tcsnicmp(csUrlHttp, _T("http://"), 7) != 0)
                        {
                           LogMessage(IDS_ERR_HTTPHASTOBEHTTPFORREAL, MB_ICONERROR);
                           success = false;
                        }
                        else
                        {
                           _TCHAR tszTmp[512];
                           _tcscpy(tszTmp, csUrlHttp);
                           _tcsncpy(tszTmp, _T("rtsp://"), 7);
                           csReplace.Format(_T("%s/%s.smi"), tszTmp, m_csBaseName);
                        }
                     }
                  }
               }

               if (success)
               {
                  // We need this reference later on
                  csSmiRef = csReplace;
                  success = tbVideoFrame.ReplaceAfter(_T("src=\""), _T("\""), csReplace);
               }

               if (success)
               {
                  success = tbVideoFrame.SaveFile(csTmp, LFILE_TYPE_TEXT_ANSI); // + CString(_T(".new.html")));
                  if (!success)
                     LogMessage(IDS_ERR_WRITE, csTmp, MB_ICONERROR);
               }
            }
         }
      }
      else
      {
         LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
      }

      // That was _videoframe.html; now for _viewframe.html
      // We only have to change anything there if we have clips
      if (success && m_bHasClips)
      {
         LTextBuffer tbViewFrame;
         //csTmp = m_csPath + m_csBaseName + CString(_T("_viewframe.html"));
         csTmp = m_csPath + m_csClipPlayersDocument + CString(_T(".html"));
         success = tbViewFrame.LoadFile(csTmp);
         if (success)
         {
            // We need to replace the SMI reference in the _viewframe.html
            // numberOfClips times; it's always the same reference.
            for (UINT i=0; i<m_nNumberOfClips && success; ++i)
            {
               CString csFind;
               csFind.Format(_T("<embed name=\"ClipImageWindow%d\""), i);
               success = tbViewFrame.Find(csFind);
               if (success)
                  success = tbViewFrame.ReplaceAfter(_T("src=\""), _T("\""), csSmiRef);
            }

            if (!success)
               LogMessage(IDS_ERR_REPLACE, csTmp, MB_ICONERROR);

            if (success)
            {
               success = tbViewFrame.SaveFile(csTmp, LFILE_TYPE_TEXT_ANSI); // + CString(_T(".new.html")));
               if (!success)
                  LogMessage(IDS_ERR_WRITE, csTmp, MB_ICONERROR);
            }
         }
         else
         {
            LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
         }
      }

      if (success && m_bHasClips)
      {
         // That was easy as pie, now let's deal with the SMI file itself.
         // It only exists if we have clips.
         LTextBuffer tbSmiFile;
         // csTmp = m_csPath + m_csBaseName + CString(_T(".smi"));
         csTmp = m_csSmilFileName;
         success = tbSmiFile.LoadFile(csTmp);
         if (success)
         {
            // First replacement is for the master player
            CString csReplace;
            switch (nTargetType)
            {
            case REPLAY_TYPE_LOCAL:
               csReplace.Format(_T("%s.rm"), m_csBaseName);
               break;
            case REPLAY_TYPE_HTTP:
               csReplace.Format(_T("%s/%s.rm"), csUrlHttp, m_csBaseName);
               break;
            case REPLAY_TYPE_STREAMING:
               csReplace.Format(_T("%s/%s.rm"), csUrlStreaming, m_csBaseName);
            }

            success = tbSmiFile.ReplaceAfter(_T("<video src=\""), _T("\""), csReplace);

            // Now follow all of the clip players
            for (UINT i=0; i<m_nNumberOfClips && success; ++i)
            {
               CString csClipName;
               csClipName.Format(_T("%s_clip%d.rm"), m_csBaseName, i);
               CString csReplace;

               switch (nTargetType)
               {
               case REPLAY_TYPE_LOCAL:
                  csReplace.Format(_T("%s"), csClipName);
                  break;
               case REPLAY_TYPE_HTTP:
                  csReplace.Format(_T("%s/%s"), csUrlHttp, csClipName);
                  break;
               case REPLAY_TYPE_STREAMING:
                  csReplace.Format(_T("%s/%s"), csUrlStreaming, csClipName);
               }

               success = tbSmiFile.ReplaceAfter(_T("video src=\""), _T("\""), csReplace);
            }

            if (!success)
               LogMessage(IDS_ERR_REPLACE, csTmp, MB_ICONERROR);

            if (success)
            {
               success = tbSmiFile.SaveFile(csTmp, LFILE_TYPE_TEXT_ANSI); // + CString(_T(".new.smi")));
               if (!success)
                  LogMessage(IDS_ERR_WRITE, csTmp, MB_ICONERROR);
            }
         }
         else
         {
            LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);
         }
      }
   }
   else if (success)
   {
      // DOCTYPE_WMT
      // Let's go.
      {
         // First file to be changed is the _videoframe.html
         LTextBuffer tbVideoFrame;
         //CString csTmp = m_csPath + m_csBaseName + CString(_T("_videoframe.html"));
         CString csTmp = m_csPath + m_csMasterPlayerDocument + CString(_T(".html"));
         success = tbVideoFrame.LoadFile(csTmp);
         if (!success)
            LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);

         CString csReplace;
         if (success)
         {
            switch (nTargetType)
            {
            case REPLAY_TYPE_LOCAL:
               csReplace.Format(_T("%s.wm"), m_csBaseName);
               break;
            case REPLAY_TYPE_HTTP:
               csReplace.Format(_T("%s/%s.wm"), csUrlHttp, m_csBaseName);
               break;
            case REPLAY_TYPE_STREAMING:
               csReplace.Format(_T("%s/%s.wm"), csUrlStreaming, m_csBaseName);
            }
         }

         // New since 3.0.p1: 
         // both WM 6.4 and WM 7.0 object models are used

         if (success)
            // new since 3.0.p1; this parameter does not exist in older versions
            bool dummySuccess = tbVideoFrame.ReplaceAfter(
            _T("document.writeln('   src=\""), _T("\""), csReplace);
         if (success)
            // try WM 6.4 parameter name
            success = tbVideoFrame.ReplaceAfter(
               _T("<param name=\"FileName\" value=\""),
               _T("\""), csReplace);
         if (!success)
            // try WM 7.0 parameter name (new since 3.0.p1)
            success = tbVideoFrame.ReplaceAfter(
               _T("<param name=\"URL\" value=\""),
               _T("\""), csReplace);
         if (success)
            success = tbVideoFrame.ReplaceAfter(
               _T("src=\""), _T("\""), csReplace);

         if (!success)
            LogMessage(IDS_ERR_REPLACE, csTmp, MB_ICONERROR);

         if (success)
         {
            success = tbVideoFrame.SaveFile(csTmp, LFILE_TYPE_TEXT_ANSI); // + CString(_T(".new.html")));
            if (!success)
               LogMessage(IDS_ERR_WRITE, csTmp, MB_ICONERROR);
         }
      }

      if (m_bHasClips && success)
      {
         // Now let's head for the _viewframe.html; it's only necessary
         // to change it if we have clips on top of slides.
         LTextBuffer tbViewFrame;
         //CString csTmp = m_csPath + m_csBaseName + CString(_T("_viewframe.html"));
         CString csTmp = m_csPath + m_csClipPlayersDocument + CString(_T(".html"));
         success = tbViewFrame.LoadFile(csTmp);
         if (!success)
            LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);

         if (success)
         {
            CString csReplace;

            for (UINT i=0; i<m_nNumberOfClips && success; ++i)
            {
               CString csFindPlayer;
               
               // New since 3.0.p1: 
               // both WM 6.4 and WM 7.0 object models are used
               
               // Check if we have a 3.0.p1 (or later) document
               csFindPlayer.Format(_T("if (isFirefox && isFirefoxWmPluginInstalled)"), i);
               bool bIsNewWm7Format = tbViewFrame.Find(csFindPlayer);
               
               if (bIsNewWm7Format)
               {
                  // 3.0.p1 or later
                  csFindPlayer.Format(_T("<embed ID=\"ClipPlayer%d\""), i);
                  success = tbViewFrame.Find(csFindPlayer);
                  if (success)
                  {
                     switch (nTargetType)
                     {
                     case REPLAY_TYPE_LOCAL:
                        csReplace.Format(_T("%s_clip%d.wm"), m_csBaseName, i);
                        break;
                     case REPLAY_TYPE_HTTP:
                        csReplace.Format(_T("%s/%s_clip%d.wm"), csUrlHttp, m_csBaseName, i);
                        break;
                     case REPLAY_TYPE_STREAMING:
                        csReplace.Format(_T("%s/%s_clip%d.wm"), csUrlStreaming, m_csBaseName, i);
                     }

                     success = tbViewFrame.ReplaceAfter(
                        _T("src=\""), _T("\""), csReplace);

                     if (success)
                     {
                        csFindPlayer.Format(_T("<object id=\"ClipPlayer%d\""), i);
                        success = tbViewFrame.Find(csFindPlayer);
                     }

                     if (success)
                        success = tbViewFrame.ReplaceAfter(
                           _T("<param name=\"URL\" value=\""),
                           _T("\""), csReplace);
                     if (success)
                        success = tbViewFrame.ReplaceAfter(
                           _T("src=\""), _T("\""), csReplace);
                  }
               }
               else
               {
                  // older than 3.0.p1
                  csFindPlayer.Format(_T("<object id=\"ClipPlayer%d\""), i);
                  success = tbViewFrame.Find(csFindPlayer);
                  if (success)
                  {
                     switch (nTargetType)
                     {
                     case REPLAY_TYPE_LOCAL:
                        csReplace.Format(_T("%s_clip%d.wm"), m_csBaseName, i);
                        break;
                     case REPLAY_TYPE_HTTP:
                        csReplace.Format(_T("%s/%s_clip%d.wm"), csUrlHttp, m_csBaseName, i);
                        break;
                     case REPLAY_TYPE_STREAMING:
                        csReplace.Format(_T("%s/%s_clip%d.wm"), csUrlStreaming, m_csBaseName, i);
                     }

                     success = tbViewFrame.ReplaceAfter(
                        _T("<param name=\"FileName\" value=\""),
                        _T("\""), csReplace);
                  }

                  if (success && !m_bOldStyleDocument)
                  {
                     // New style documents have special case for Netscape 4
                     // Search again for the <div> tag with name "ClipLayer%d"
                     csFindPlayer.Format(_T("<div id=\"ClipLayer%d\""), i);
                     success = tbViewFrame.Find(csFindPlayer);
                     if (success)
                     {
                        success = tbViewFrame.ReplaceAfter(
                           _T("src=\""), _T("\""), csReplace);
                     }
                  }
               }

            }

            if (!success)
               LogMessage(IDS_ERR_REPLACE, csTmp, MB_ICONERROR);
         }

         if (success)
         {
            success = tbViewFrame.SaveFile(csTmp, LFILE_TYPE_TEXT_ANSI); // + CString(_T(".new.html")));
            if (!success)
               LogMessage(IDS_ERR_WRITE, csTmp, MB_ICONERROR);
         }
      }

      if (m_bHasClips && m_bOldStyleDocument && success)
      {
         // Now for the popup Windows (for Netscape 4)
         for (UINT i=0; i<m_nNumberOfClips && success; ++i)
         {
            LTextBuffer tbPopup;
            CString csPopupName;
            csPopupName.Format(_T("%s_popup%d.html"), m_csBaseName, i);
            CString csTmp = m_csPath + csPopupName;
            success = tbPopup.LoadFile(csTmp);

            if (!success)
               LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);

            if (success)
            {
               success = tbPopup.Find(_T("<embed"));
               if (!success)
                  LogMessage(IDS_ERR_REPLACE, csTmp, MB_ICONERROR);
            }
            if (success)
            {
               CString csReplace;
               switch (nTargetType)
               {
               case REPLAY_TYPE_LOCAL:
                  csReplace.Format(_T("%s_clip%d.wm"), m_csBaseName, i);
                  break;
               case REPLAY_TYPE_HTTP:
                  csReplace.Format(_T("%s/%s_clip%d.wm"), csUrlHttp, m_csBaseName, i);
                  break;
               case REPLAY_TYPE_STREAMING:
                  csReplace.Format(_T("%s/%s_clip%d.wm"), csUrlStreaming, m_csBaseName, i);
               }

               success = tbPopup.ReplaceAfter(_T("src=\""), _T("\""), csReplace);
               if (!success)
                  LogMessage(IDS_ERR_REPLACE, csTmp, MB_ICONERROR);
            }

            if (success)
            {
               success = tbPopup.SaveFile(csTmp, LFILE_TYPE_TEXT_ANSI); // + CString(_T(".new.html")));
               if (!success)
                  LogMessage(IDS_ERR_WRITE, csTmp, MB_ICONERROR);
            }
         }
      }
   }

   if (success)
      // Write a flag into the Document, marking it as modified with LocationUpdate
      success = MarkDocumentAsUpdated();

   if (success && m_bTempDirUsed)
   {
#ifndef CHANGEURLCMD
      m_pProgressDlg = new CZipProgressDlg(GetCWnd(), this);
#endif

      m_bThreadCancelRequest = false;
      m_bThreadSuccess = true;
#ifndef CHANGEURLCMD
      AfxBeginThread(CreateZipThread, this);
#else
      CreateZip();
#endif

#ifndef CHANGEURLCMD
      m_pProgressDlg->DoModal();

      delete m_pProgressDlg;
      m_pProgressDlg = NULL;
#endif

      success = m_bThreadSuccess;
   }

   if (success)
   {
      m_nOriginalReplayType = nTargetType;

      LogMessage(IDS_MSG_ALLSUCCESS, MB_ICONINFORMATION);
   }
   else
   {
      LogMessage(IDS_ERR_NOSUCCESS, MB_ICONWARNING);
   }

   return success;
}

bool CChangeUrlEngine::MarkDocumentAsUpdated()
{
   // This method modifies the flag 'g_bModifiedByLocationUpdate' in the 
   // file "variables.js" in RM/WM documents and sets it to 'true'. 
   // If this variable (or "variables.js" in Flash case) does not exist 
   // then nothing is done

   bool success = true;

   if (success && m_nDocType == DOCTYPE_FLASH)
      // Flash documents have no "variables.js" --> nothing to do
      return success;

   LTextBuffer tbVariablesJs;
   CString csTmp = m_csPath + _T("js/variables.js");
   success = tbVariablesJs.LoadFile(csTmp);
   if (!success)
      LogMessage(IDS_ERR_READ, csTmp, MB_ICONERROR);

   CString csReplace;
   if (success)
   {
      bool bIsAlreadyMarked = tbVariablesJs.Find(_T("g_bModifiedByLocationUpdate = true"));

      if (!bIsAlreadyMarked)
      {
         // Reset the buffer pointer to be able to start a new search
         tbVariablesJs.ResetBufferPointer();

         if (success)
         {
            CString csReplace;
            csReplace.SetString(_T("true"));
            bool bReplaceDone = tbVariablesJs.ReplaceAfter(_T("g_bModifiedByLocationUpdate = "), _T(";"), csReplace);

            if (bReplaceDone)
            {
               success = tbVariablesJs.SaveFile(csTmp, LFILE_TYPE_TEXT_ANSI); // + CString(_T(".new.html")));
               if (!success)
                  LogMessage(IDS_ERR_WRITE, csTmp, MB_ICONERROR);
            }
         }
      }
   }

   return success;
}

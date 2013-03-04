// Ftp.cpp: implementation of the CFtp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "podcaster.h"
#include "Ftp.h"
#include "LanguageSupport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtp::CFtp()
{
   m_FtpConnection = NULL;
   m_FtpFile = NULL;
}

CFtp::~CFtp()
{
   Close();

   // close file
   if(m_FtpFile != NULL)
   {
      m_FtpFile->Close();
      delete m_FtpFile;
      m_FtpFile = NULL;
   }
}

BOOL CFtp::Open(CString csServer, int nPort, CString csUsername, CString csPassword)
{
   BOOL bResult = FALSE;
   try
   {
      m_FtpConnection = m_Session.GetFtpConnection(csServer, csUsername, csPassword, nPort);     
      if(m_FtpConnection != NULL)
      {
         bResult = m_FtpConnection->GetCurrentDirectory(m_csCurrentDirectory);
         if(bResult)
         {
            // prepare directory
            if(m_csCurrentDirectory.IsEmpty())
            {
               m_csCurrentDirectory += _T("/");
            }
            else
            {
               if(m_csCurrentDirectory[m_csCurrentDirectory.GetLength() - 1] != _T('/'))
               {
                  m_csCurrentDirectory += _T("/");
               }
            }
         }
      }
   }
   catch(CInternetException *exc)
   {
      GetInetError(exc->m_dwError);
   }   
   return bResult;
}

void CFtp::Close()
{
   if(m_FtpFile != NULL)
   {
      m_FtpFile->Close();
      delete m_FtpFile;
      m_FtpFile = NULL;
   }
}

CString CFtp::GetInetError(DWORD dwErrorNo)
{
   HRESULT hr = S_OK;
   LCID langId;
   hr = CLanguageSupport::GetLanguageId(langId);
   if (hr != S_OK)
   {
      CString csTitle;
      csTitle.LoadString(IDS_ERROR);
      CString csMessage;
      if (hr == E_LS_WRONG_REGISTRY)
         csMessage.LoadString(IDS_ERROR_INSTALLATION);
      else if (hr == E_LS_LANGUAGE_NOT_SUPPORTED)
         csMessage.LoadString(IDS_ERROR_LANGUAGE);
      else if (hr == E_LS_TREAD_LOCALE)
         csMessage.LoadString(IDS_ERROR_SET_LANGUAGE);

      MessageBox(NULL, csMessage, csTitle, MB_OK | MB_TOPMOST);
   }

   CString csErrorMessage, csFormat;
   LPTSTR tszMessage;
   
   HANDLE hModule = ::GetModuleHandle(_T("WinINet.dll"));
    
   if(hModule != NULL)
   { /* got module */
      if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
             FORMAT_MESSAGE_FROM_HMODULE,
             (LPCVOID)hModule,
             dwErrorNo,
             langId,//0, // default language
             (LPTSTR)&tszMessage,
             0,
             NULL) != 0)
      { /* format succeeded */
         csErrorMessage.Format(_T("%s"), tszMessage);
         csErrorMessage.Replace(_T("\r\n"), _T(""));
         ::LocalFree(tszMessage);
         //MessageBox(NULL, csErrorMessage, _T("Podcaster"), MB_OK + MB_ICONSTOP);
		 AfxMessageBox(csErrorMessage, MB_OK + MB_ICONSTOP + MB_TOPMOST);
         return csErrorMessage;
       } /* format succeeded */
   } /* got module */

   csFormat.LoadString(IDS_FTPERROR);
   csErrorMessage.Format(csFormat, dwErrorNo);
   //MessageBox(NULL, csErrorMessage, _T("Podcaster"), MB_OK + MB_ICONSTOP);
   AfxMessageBox(csErrorMessage, MB_OK + MB_ICONSTOP + MB_TOPMOST);
   return csErrorMessage;
} // getInetError

BOOL CFtp::Upload(CString csSrcFileName, CString csDstFileName, CString csDirectory)
{
   BOOL bResult = FALSE;
   try
   {
      bResult = m_FtpConnection->SetCurrentDirectory(
         m_csCurrentDirectory + csDirectory);
      if(bResult)
      {
         bResult = m_FtpConnection->PutFile(csSrcFileName, csDstFileName);
         if(!bResult)
         {
            ErrorMessage(IDS_ERRUPLOADXML, csSrcFileName, csDstFileName);
         }
      }
      else
      {
         ErrorMessage(IDS_ERRDIR1, csDirectory);
      }
   }
   catch(CInternetException *exc)
   {
      GetInetError(exc->m_dwError);
   }   
   return bResult;
}

#define BLOCKSIZE 4096
BOOL CFtp::Upload(CString csSrcFileName, CString csDstFileName, CString csDirectory, CProgressCtrl &m_prgProgress) 
{
   // TODO: Add your control notification handler code here
   BOOL bResult = FALSE;
   BYTE buffer[BLOCKSIZE];
   DWORD dwRead;
 
   DWORD m_dwMediaFileLength = 0;
   //if(File.Open(csSrcFileName,  CFile::modeRead | CFile::shareDenyWrite, &FileException))
   bResult = File.Open(csSrcFileName,  CFile::modeRead, NULL);
   if(bResult)
   {
      // get file length for use in XML file
      m_dwMediaFileLength = File.GetLength();
      // Read in 4096-byte blocks,
      m_prgProgress.SetPos(0);
      m_prgProgress.SetRange32(0, File.GetLength()/BLOCKSIZE);
      m_prgProgress.SetStep(BLOCKSIZE);

      try
      {
         bResult = m_FtpConnection->SetCurrentDirectory(m_csCurrentDirectory + csDirectory);
         if(bResult)
         {
            m_FtpFile = m_FtpConnection->OpenFile(csDstFileName, GENERIC_WRITE);
            if(m_FtpFile != NULL)
            {
               // do the upload using BLOCSIZE bloks
               do
               {
                  dwRead = File.Read(buffer, BLOCKSIZE);
                  if(dwRead > 0)
                  {
                     // no error return ?
                     m_FtpFile->Write(buffer, dwRead);
                     m_prgProgress.SetPos(m_prgProgress.GetPos() + 1);
                  }
               } while (dwRead > 0);

               m_FtpFile->Close();
               delete m_FtpFile;
               m_FtpFile = NULL;
               
               bResult = TRUE;
            }
            else
            {
               ErrorMessage(IDS_ERR_FTP_CREATE_FILE, csSrcFileName);
            }
         }
         else
         {
            ErrorMessage(IDS_ERRDIR2, csDirectory);
         }
      }
      catch(CInternetException *exc)
      {
         GetInetError(exc->m_dwError);
      }   

      File.Close();
   }
   else
   {
      ErrorMessage(IDS_ERROPENFILE, csSrcFileName);
   }
   return bResult;
}

BOOL CFtp::Download(CString csSrcFileName, CString csDstFileName, CString csDirectory)
{
   BOOL bResult = FALSE;
   try
   {
      bResult = m_FtpConnection->SetCurrentDirectory(m_csCurrentDirectory + csDirectory);
      if(bResult)
      {
         bResult = m_FtpConnection->GetFile(csSrcFileName, csDstFileName, FALSE);
         if(!bResult)
         {
            ErrorMessage(IDS_ERRUPLOADXML, csSrcFileName, csDstFileName);
         }
      }
      else
      {
         ErrorMessage(IDS_ERRDIR1, csDirectory);
      }
   }
   catch(CInternetException *exc)
   {
      GetInetError(exc->m_dwError);
   }   
   return bResult;
}

BOOL CFtp::Delete(CString csDstFileName, CString csDirectory)
{
   BOOL bResult = FALSE;
   try
   {
      bResult = m_FtpConnection->SetCurrentDirectory(m_csCurrentDirectory + csDirectory);
      if(bResult)
      {
         bResult = m_FtpConnection->Remove(csDstFileName);
      }
      else
      {
         ErrorMessage(IDS_ERRDIR1, csDirectory);
      }
   }
   catch(CInternetException *Exception)
   {
      GetInetError(Exception->m_dwError);
   }   
   return bResult;
}

void CFtp::CleanUp()
{
   File.Close();
   Close();
}

int CFtp::ErrorMessage(UINT uId)
{
   CString csMessage;
   csMessage.LoadString(uId);
   MessageBox(NULL, csMessage, _T("Podcaster"), MB_OK + MB_ICONSTOP);
   return 0;
}

int CFtp::ErrorMessage(UINT uId, CString csParam1)
{
   CString csMessage, csFormat;
   csFormat.LoadString(uId);
   csMessage.Format(csFormat, csParam1);
   MessageBox(NULL, csMessage, _T("Podcaster"), MB_OK + MB_ICONSTOP);
   return 0;
}

int CFtp::ErrorMessage(UINT uId, CString csParam1, CString csParam2)
{
   CString csMessage, csFormat;
   csFormat.LoadString(uId);
   csMessage.Format(csFormat, csParam1, csParam2);
   MessageBox(NULL, csMessage, _T("Podcaster"), MB_OK + MB_ICONSTOP);
   return 0;
}


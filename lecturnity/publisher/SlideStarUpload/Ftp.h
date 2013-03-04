// Ftp.h: interface for the CFtp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTP_H__3F9C2A64_214B_4BCB_919C_F7335F28894B__INCLUDED_)
#define AFX_FTP_H__3F9C2A64_214B_4BCB_919C_F7335F28894B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <WinInet.h>
#include "afxinet.h"

class CFtp  
{
private:
   CInternetSession m_Session;
   CFtpConnection *m_FtpConnection;
   CInternetFile* m_FtpFile;
   CString m_csCurrentDirectory;

   CString GetInetError(DWORD dwErrorNo);
   
   //CFileException ex;
   CFile File;

public:
   CFtp();
   virtual ~CFtp();
   BOOL Open(CString csServer, int nPort, CString csUsername, CString csPassword);
   void Close();
   BOOL Download(CString csSrcFileName, CString csDstFileName, CString csDirectory);
   BOOL Upload(CString csSrcFileName, CString csDstFileName, CString csDirectory);
   BOOL Upload(CString csSrcFileName,  CString csDstFileName, CString csDirectory, CProgressCtrl &ctrlProgress);
   BOOL Delete(CString csDstFileName, CString csDirectory);

   int ErrorMessage(UINT uId);
   int ErrorMessage(UINT uId, CString csParam1);
   int ErrorMessage(UINT uId, CString csParam1, CString csParam2);
   void CleanUp();
};

#endif // !defined(AFX_FTP_H__3F9C2A64_214B_4BCB_919C_F7335F28894B__INCLUDED_)

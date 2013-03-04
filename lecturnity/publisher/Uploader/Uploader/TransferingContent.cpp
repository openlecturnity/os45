#include "StdAfx.h"
#include "Ftp.h"
#include "Ssh.h"
#include "YouTube.h"
#include "TransferingContent.h"
#include "LanguageSupport.h"
#include "XmlValidator.h"
#include "AddItem.h"
#include "FileExists.h"

CArray<SavedData, SavedData>aSData;
CArray<SavedData, SavedData>aSdSubDir;
CCriticalSection critSection;
CTransferingContent::CTransferingContent(void)
{
   m_hParentHwnd = NULL;
   m_iErrCode = 0;
}

CTransferingContent::~CTransferingContent(void)
{
}

CTransferingContent& CTransferingContent::GetInstance()
{
   static CTransferingContent cTContent;
   return cTContent;
}

UINT CTransferingContent::Cleanup(bool bIsCancelled)
{
   bool bCancel = false;
   bool bIsThreadActive = false;
   double dProg = 0;
   CString s;
   
   if(bIsCancelled)
   {
      for(int i = 0; i < aSData.GetSize(); i++)
      {
         Upload(aSData.GetAt(i).tp, NULL, aSData.GetAt(i).csFileOnServer, &dProg, &bCancel, &bIsThreadActive, true);
      }

      for(int i = 0; i < aSdSubDir.GetSize(); i++)
      {
         RemoveSubDirectory(aSdSubDir.GetAt(i).tp, aSdSubDir.GetAt(i).csFileOnServer);
      }
   }

   aSData.RemoveAll();
   aSdSubDir.RemoveAll();
   return S_OK;
}


HRESULT CTransferingContent::Upload(TransferParam tp, CString csSrc, CString csDst, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, bool bDelete /*= false*/)
{
   BOOL bResult = FALSE;
   *pbIsThreadActive = true;
   if (tp.iService == UPLOAD_FTP)
   {
      CFtp ftp;
      bResult = ftp.Open(tp.csServer, tp.nPort, tp.csUsername, tp.csPassword);
      if (!bResult)
      {
         m_csErrStr = ftp.GetErrString();
         m_iErrCode = ftp.GetErrCode();
		 *pbIsThreadActive = false;
		 if (m_iErrCode == 87 || m_iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || m_iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || m_iErrCode == ERROR_INTERNET_LOGIN_FAILURE)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == ERROR_INTERNET_INVALID_URL || m_iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || m_iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == 105 || m_iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || m_iErrCode == ERROR_INTERNET_CONNECTION_RESET || m_iErrCode == ERROR_FTP_DROPPED || m_iErrCode == ERROR_INTERNET_TIMEOUT)
			 return ERR_NETWORK_ERROR;
		 else
			 return ERR_GENERIC_ERROR;/*bResult*/;
      }
      if(!bDelete)
      {
         if(*pbIsCancel == true)
         {
			 *pbIsThreadActive = false;
            return ERR_NO_ERROR;
         }
         bResult = ftp.Upload(csSrc, csDst, tp.csDirectory, dProgress, pbIsCancel, pbIsThreadActive);
      }
      else
      {
         bResult = ftp.Delete(csDst, tp.csDirectory);
      }
      *dProgress = 1.0;
      ftp.Close();
	  *pbIsThreadActive = false;
      if(!bResult)
      {
         m_csErrStr = ftp.GetErrString();
         m_iErrCode = ftp.GetErrCode();
         if (m_iErrCode == 87 || m_iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || m_iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || m_iErrCode == ERROR_INTERNET_LOGIN_FAILURE)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == ERROR_INTERNET_INVALID_URL || m_iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || m_iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == 105 || m_iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || m_iErrCode == ERROR_INTERNET_CONNECTION_RESET || m_iErrCode == ERROR_FTP_DROPPED || m_iErrCode == ERROR_INTERNET_TIMEOUT)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == 98765)
			 return ERR_CANNOT_OVERWRITE_FILE;
		 else
			 return ERR_GENERIC_ERROR;/*bResult*/;
      }
      return ERR_NO_ERROR;
   }
   else
   {
      CSsh ssh;
      CString csFullName;
      if(tp.iService == UPLOAD_SCP)
      {
         csFullName.Format(_T("\'%s%s\'"), tp.csDirectory, csDst);
      }
      else
      {
         csFullName.Format(_T("%s%s"), tp.csDirectory, csDst);
      }

      if(!bDelete)
      {
         if(*pbIsCancel == true)
         {
			 *pbIsThreadActive = false;
            return ERR_NO_ERROR;
         }
         bResult = ssh.Upload(csSrc, csFullName, tp, dProgress, pbIsCancel, pbIsThreadActive);
      }
      else
      {
         bResult = ssh.Delete(csFullName, tp, dProgress, pbIsCancel, pbIsThreadActive);
      }
      *dProgress = 1.0;

      if(!bResult)
      {
		  *pbIsThreadActive = false;
         m_csErrStr = ssh.GetErrStr();
         m_iErrCode = ssh.GetErrCode();
		 if (m_iErrCode == 105 || m_iErrCode == 6 || m_iErrCode == 38 || m_iErrCode == 48 || m_iErrCode == 64 || m_iErrCode == 71 || m_iErrCode == 79)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == WSAEADDRNOTAVAIL || m_iErrCode == WSAENETDOWN || m_iErrCode == WSAENETUNREACH || m_iErrCode == WSAENETRESET || m_iErrCode == WSAETIMEDOUT)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == 22)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == WSAECONNREFUSED || m_iErrCode == WSAEHOSTDOWN || m_iErrCode == WSAEHOSTUNREACH)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == WSAEACCES || m_iErrCode == 8)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == 2 || m_iErrCode == 3)
			 return ERR_CANNOT_CREATE_FILE_DIR;
		 else
			 return ERR_GENERIC_ERROR;
      }
   }
   *pbIsThreadActive = false;
   return ERR_NO_ERROR;
}

HRESULT CTransferingContent::YtUpload(YtTransferParam tp, CString csFile,  CString csAuthToken, double *pdProgress, bool *pbIsCancel, bool *pbIsThreadActive)
{
    BOOL bResult = FALSE;
    CYouTube yt;
    if(!csAuthToken.IsEmpty())
    {
        yt.SetAuthToken(csAuthToken);
    }
    else
    {
        bResult = yt.Authentify(tp.csUsername, tp.csPassword);
        if(!bResult)
        {
            m_csErrStr = yt.GetErrString();
            m_iErrCode = yt.GetErrCode();
            return ERR_AUTHENTICATION_FAILED;
            // todo add extra error codes
            
        }
    }

    if(*pbIsCancel == true)
    {
        *pbIsThreadActive = false;
        return ERR_NO_ERROR;
    }

    bResult = yt.Upload(csFile, tp, pdProgress, pbIsCancel);
    *pdProgress = 1.0;

    if(bResult != S_OK)
    {
        m_csErrStr = yt.GetErrString();
        m_iErrCode = yt.GetErrCode();
        return ERR_HTTP_REQ;
    }

    *pbIsThreadActive = false;
    return ERR_NO_ERROR;
}

UINT CTransferingContent::Podcast(TransferParam MediaTP, TransferParam ChannelTP, CString csMediaSrc, CString csMediaDst, CString csTitle, CString csAuthor, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive)
{
	*pbIsThreadActive = true;
   BOOL bResult = ERR_NO_ERROR;
   //add item to the channel file
   TCHAR tstrTmpDirectory[MAX_PATH + 1];
   CString csTmpDirectory;
   int n = ::GetTempPath(MAX_PATH, tstrTmpDirectory);
   if(n != 0)
   {
      csTmpDirectory = CString(tstrTmpDirectory, n);
   }
   else
   {
	   *pbIsThreadActive = false;
      return ERR_CHANNEL_CHANGE;
   }

   CString csFile = csTmpDirectory + _T("\\tmp.xml");
   if(!AddItem(ChannelTP.csUrl, csMediaDst, MediaTP.csUrl, csMediaSrc, csTitle, csAuthor, csFile))
   {
	   *pbIsThreadActive = false;
      return ERR_CHANNEL_CHANGE;
   }
   else
   {
      // normalize the file
      CXmlValidator valid;
      bResult = valid.Validate(csFile, csFile);
      if(!bResult)
      {
		  *pbIsThreadActive = false;
         return FALSE;
      }

   }
   CString csChannelDest = GetFileName(ChannelTP.csUrl);

   if (MediaTP.iService == UPLOAD_FTP)
   {
      CFtp ftp;
      bResult = ftp.Open(MediaTP.csServer, MediaTP.nPort, MediaTP.csUsername, MediaTP.csPassword);
      if(!bResult)
      {
         m_csErrStr = ftp.GetErrString();
         m_iErrCode = ftp.GetErrCode();
		 *pbIsThreadActive = false;
         if (m_iErrCode == 87 || m_iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || m_iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || m_iErrCode == ERROR_INTERNET_LOGIN_FAILURE)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == ERROR_INTERNET_INVALID_URL || m_iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || m_iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == 105 || m_iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || m_iErrCode == ERROR_INTERNET_CONNECTION_RESET || m_iErrCode == ERROR_FTP_DROPPED || m_iErrCode == ERROR_INTERNET_TIMEOUT)
			 return ERR_NETWORK_ERROR;
		 else
			 return ERR_GENERIC_ERROR;/*bResult*/;
      }

      // upload media file
      bResult = ftp.Upload(csMediaSrc, csMediaDst, MediaTP.csDirectory, dProgress, pbIsCancel, pbIsThreadActive);
      ftp.Close();		 
      if(!bResult)
      {
         m_csErrStr = ftp.GetErrString();
         m_iErrCode = ftp.GetErrCode();
		 *pbIsThreadActive = false;
         if (m_iErrCode == 87 || m_iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || m_iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || m_iErrCode == ERROR_INTERNET_LOGIN_FAILURE)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == ERROR_INTERNET_INVALID_URL || m_iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || m_iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == 105 || m_iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || m_iErrCode == ERROR_INTERNET_CONNECTION_RESET || m_iErrCode == ERROR_FTP_DROPPED || m_iErrCode == ERROR_INTERNET_TIMEOUT)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == 98765)
			 return ERR_CANNOT_OVERWRITE_FILE;
		 else
			 return ERR_GENERIC_ERROR;/*bResult*/;
         return ERR_FTP_UPLOAD;
      }
   }
   else
   {
      CSsh ssh;
      CString csMediaFullName;
      csMediaFullName.Format(_T("\'%s\'\'%s\'"), MediaTP.csDirectory, csMediaDst);
      bResult = ssh.Upload(csMediaSrc, csMediaFullName, MediaTP, dProgress, pbIsCancel, pbIsThreadActive);

      if(!bResult)
      {
         m_csErrStr = ssh.GetErrStr();
         m_iErrCode = ssh.GetErrCode();
		 *pbIsThreadActive = false;
		 if (m_iErrCode == 105 || m_iErrCode == 6 || m_iErrCode == 38 || m_iErrCode == 48 || m_iErrCode == 64 || m_iErrCode == 71 || m_iErrCode == 79)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == WSAEADDRNOTAVAIL || m_iErrCode == WSAENETDOWN || m_iErrCode == WSAENETUNREACH || m_iErrCode == WSAENETRESET || m_iErrCode == WSAETIMEDOUT)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == 22)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == WSAECONNREFUSED || m_iErrCode == WSAEHOSTDOWN || m_iErrCode == WSAEHOSTUNREACH)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == WSAEACCES || m_iErrCode == 8)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == 2 || m_iErrCode == 3)
			 return ERR_CANNOT_CREATE_FILE_DIR;
		 else
			 return ERR_GENERIC_ERROR;
      }

   }
   if (ChannelTP.iService == UPLOAD_FTP)
   {
      CFtp ftp;
      // upload channel file
      bResult = ftp.Open(ChannelTP.csServer, ChannelTP.nPort, ChannelTP.csUsername, ChannelTP.csPassword);
      if(!bResult)
      {
         m_csErrStr = ftp.GetErrString();
         m_iErrCode = ftp.GetErrCode();
		 *pbIsThreadActive = false;
         if (m_iErrCode == 87 || m_iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || m_iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || m_iErrCode == ERROR_INTERNET_LOGIN_FAILURE)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == ERROR_INTERNET_INVALID_URL || m_iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || m_iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == 105 || m_iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || m_iErrCode == ERROR_INTERNET_CONNECTION_RESET || m_iErrCode == ERROR_FTP_DROPPED || m_iErrCode == ERROR_INTERNET_TIMEOUT)
			 return ERR_NETWORK_ERROR;
		 else
			 return ERR_GENERIC_ERROR;/*bResult*/;
      }

      CString csChannelSrc = csTmpDirectory + _T("\\tmp.xml");
      bResult = ftp.Upload(csChannelSrc, csChannelDest, ChannelTP.csDirectory);
      *dProgress = 1.0;
      ftp.Close();
      if(!bResult)
      {
         m_csErrStr = ftp.GetErrString();
         m_iErrCode = ftp.GetErrCode();
		 *pbIsThreadActive = false;
         if (m_iErrCode == 87 || m_iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || m_iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || m_iErrCode == ERROR_INTERNET_LOGIN_FAILURE)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == ERROR_INTERNET_INVALID_URL || m_iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || m_iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == 105 || m_iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || m_iErrCode == ERROR_INTERNET_CONNECTION_RESET || m_iErrCode == ERROR_FTP_DROPPED || m_iErrCode == ERROR_INTERNET_TIMEOUT)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == 98765)
			 return ERR_CANNOT_OVERWRITE_FILE;
		 else
			 return ERR_GENERIC_ERROR;/*bResult*/;
      }
   }
   else
   {
      // upload channel file
      CSsh ssh;
      CString csChannelFullName;
      csChannelFullName.Format(_T("\'%s\'\'%s\'"), ChannelTP.csDirectory, csChannelDest);

	  double *dProg = new double();
	  *dProg = 0;
      CString csChannelSrc = csTmpDirectory + _T("\\tmp.xml");
      bResult = ssh.Upload(csChannelSrc, csChannelFullName, ChannelTP, dProg, pbIsCancel, pbIsThreadActive);
	  delete dProg;
      if(!bResult)
      {
         m_csErrStr = ssh.GetErrStr();
         m_iErrCode = ssh.GetErrCode();
		 *pbIsThreadActive = false;
		 if (m_iErrCode == 105 || m_iErrCode == 6 || m_iErrCode == 38 || m_iErrCode == 48 || m_iErrCode == 64 || m_iErrCode == 71 || m_iErrCode == 79)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == WSAEADDRNOTAVAIL || m_iErrCode == WSAENETDOWN || m_iErrCode == WSAENETUNREACH || m_iErrCode == WSAENETRESET || m_iErrCode == WSAETIMEDOUT)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == 22)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == WSAECONNREFUSED || m_iErrCode == WSAEHOSTDOWN || m_iErrCode == WSAEHOSTUNREACH)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == WSAEACCES || m_iErrCode == 8)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == 2 || m_iErrCode == 3)
			 return ERR_CANNOT_CREATE_FILE_DIR;
		 else
			 return ERR_GENERIC_ERROR;
      }
      *dProgress = 1.0; 
   }
   ::DeleteFile(csTmpDirectory + _T("\\tmp.xml"));
   *pbIsThreadActive = false;
   return ERR_NO_ERROR;
}

DWORD CTransferingContent::GetFileLength(CString csFile)
{
   DWORD dwLength = 0;

   CFile file;
   if(file.Open(csFile,  CFile::modeRead | CFile::shareDenyWrite))
   {
      dwLength = (DWORD)file.GetLength();
      file.Close();
   }

   return dwLength;
}

BOOL CTransferingContent::AddItem(LPCTSTR srcFileName, CString csItemFileName, CString csItemDirectoryUrl, CString csItemSrc, CString csTitle, CString csAuthor, LPCTSTR dstFileName)
{
   BOOL bInserted = FALSE;
   CTime data = CTime::GetCurrentTime();

   CAddItem item;
   {
      CString csURL = csItemDirectoryUrl + csItemFileName + CString(_T("?")) + data.Format(_T("%Y-%m-%dT%H:%M:%S"));

      bInserted = IsMediaUrlOk(csURL);

      if(bInserted)
      {
         item.csTitle  = csTitle;
         item.csAuthor = csAuthor,
            item.csType = _T("video/mp4");
         item.csLength.Format(_T("%lu"), GetFileLength(csItemSrc));
         item.csUrl = csURL;
         item.csDate = data.Format(_T("%a, %d %b %Y %H:%M:%S"));
         bInserted = item.Insert(srcFileName, dstFileName);
      }      
   }

   return bInserted;
}

BOOL CTransferingContent::IsMediaUrlOk(CString &csURL)
{
   BOOL bResult;
   DWORD dwServiceType;
   CString csServer, csObject;
   INTERNET_PORT nPort;

   bResult = AfxParseURL(csURL, dwServiceType, csServer, csObject, nPort);
   if(bResult)
   {
      bResult = (dwServiceType == AFX_INET_SERVICE_HTTP);
      if(bResult)
      {
         bResult = !csServer.IsEmpty();
         if(bResult)
         {
            bResult = !csObject.IsEmpty();               
         }
      }
   }

   return bResult;
}

HRESULT CTransferingContent::YtAuthentify(CString csUser, CString csPass, CString &csAuthToken, CString &csAuthUser)
{
    CYouTube yt;
    _tprintf(_T("!!!Upl in CTransferingContent::YtAuthentify before Authentify user = %s \n"), csUser);
    HRESULT res = S_FALSE;
    if(yt.Authentify(csUser, csPass))
    {
        yt.GetAuthToken(csAuthToken);
        yt.GetAuthUser(csAuthUser);
        res = S_OK;
    }
    else
    {
        m_csErrStr = yt.GetErrString();
        m_iErrCode = yt.GetErrCode();
        if (m_iErrCode == 87)
            return ERR_AUTHENTICATION_FAILED;
        else if (m_iErrCode == ERROR_INTERNET_INVALID_URL || m_iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || m_iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
            return ERR_SERVER_DOES_NOT_EXIST;
        else if (m_iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
            return ERR_SERVER_NOT_REACHABLE;
        else if (m_iErrCode == 105 || m_iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || m_iErrCode == ERROR_INTERNET_CONNECTION_RESET  || m_iErrCode == ERROR_INTERNET_TIMEOUT)
            return ERR_NETWORK_ERROR;
        else if (m_iErrCode == 98765)
            return ERR_CANNOT_OVERWRITE_FILE;
        else
            return ERR_GENERIC_ERROR;/*bResult*/;
    }
    return res;
}

HRESULT CTransferingContent::CheckTarget(CString csServer, int iProtocol, int iPort, CString csUser, CString csPassword, CString csTargetDirOrFile, bool* pbIsDir, bool* pbIsEmptyDir)
{
	bool bExists;
	HRESULT hr;
	if (iProtocol)
	{
		CSsh ssh;
		if(!ssh.CheckTarget(csServer, iProtocol, iPort, csUser, csPassword, csTargetDirOrFile, bExists, pbIsDir, pbIsEmptyDir))
		{
			m_csErrStr = ssh.GetErrStr();
         m_iErrCode = ssh.GetErrCode();
		 if (m_iErrCode == 105 || m_iErrCode == 6 || m_iErrCode == 38 || m_iErrCode == 48 || m_iErrCode == 64 || m_iErrCode == 71 || m_iErrCode == 79)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == WSAEADDRNOTAVAIL || m_iErrCode == WSAENETDOWN || m_iErrCode == WSAENETUNREACH || m_iErrCode == WSAENETRESET || m_iErrCode == WSAETIMEDOUT)
			 return ERR_NETWORK_ERROR;
		 else if (m_iErrCode == 22)
			 return ERR_SERVER_DOES_NOT_EXIST;
		 else if (m_iErrCode == WSAECONNREFUSED || m_iErrCode == WSAEHOSTDOWN || m_iErrCode == WSAEHOSTUNREACH)
			 return ERR_SERVER_NOT_REACHABLE;
		 else if (m_iErrCode == WSAEACCES || m_iErrCode == 8)
			 return ERR_AUTHENTICATION_FAILED;
		 else if (m_iErrCode == 2 || m_iErrCode == 3)
			 return ERR_CANNOT_CREATE_FILE_DIR;
		 else
			 return ERR_GENERIC_ERROR;
		}
	}
	else
	{
		CFtp ftp;
		if (!ftp.Open(csServer, iPort, csUser, csPassword))
		{
			 m_csErrStr = ftp.GetErrString();
			 m_iErrCode = ftp.GetErrCode();
			 if (m_iErrCode == 87 || m_iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || m_iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || m_iErrCode == ERROR_INTERNET_LOGIN_FAILURE)
				 return ERR_AUTHENTICATION_FAILED;
			 else if (m_iErrCode == ERROR_INTERNET_INVALID_URL || m_iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || m_iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
				 return ERR_SERVER_DOES_NOT_EXIST;
			 else if (m_iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
				 return ERR_SERVER_NOT_REACHABLE;
			 else if (m_iErrCode == 105 || m_iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || m_iErrCode == ERROR_INTERNET_CONNECTION_RESET || m_iErrCode == ERROR_FTP_DROPPED || m_iErrCode == ERROR_INTERNET_TIMEOUT)
				 return ERR_NETWORK_ERROR;
			 else
				 return ERR_GENERIC_ERROR;/*bResult*/;
		}
		if (!ftp.CheckTarget(csTargetDirOrFile, bExists, pbIsDir, pbIsEmptyDir))
		{
			 m_csErrStr = ftp.GetErrString(); 
			 m_iErrCode = ftp.GetErrCode();
			 if (m_iErrCode == 87 || m_iErrCode == ERROR_INTERNET_INCORRECT_USER_NAME || m_iErrCode == ERROR_INTERNET_INCORRECT_PASSWORD || m_iErrCode == ERROR_INTERNET_LOGIN_FAILURE)
				 return ERR_AUTHENTICATION_FAILED;
			 else if (m_iErrCode == ERROR_INTERNET_INVALID_URL || m_iErrCode == ERROR_INTERNET_UNRECOGNIZED_SCHEME || m_iErrCode == ERROR_INTERNET_NAME_NOT_RESOLVED)
				 return ERR_SERVER_DOES_NOT_EXIST;
			 else if (m_iErrCode == ERROR_INTERNET_CANNOT_CONNECT)
				 return ERR_SERVER_NOT_REACHABLE;
			 else if (m_iErrCode == 105 || m_iErrCode == ERROR_INTERNET_CONNECTION_ABORTED || m_iErrCode == ERROR_INTERNET_CONNECTION_RESET || m_iErrCode == ERROR_FTP_DROPPED || m_iErrCode == ERROR_INTERNET_TIMEOUT)
				 return ERR_NETWORK_ERROR;
			 else if (m_iErrCode == 98765)
				 return ERR_CANNOT_OVERWRITE_FILE;
			 else
				 return ERR_GENERIC_ERROR;/*bResult*/;
		}
	}
	if (bExists)
		hr = S_OK;
	else
      hr = E_FAIL;
	return hr;
}
CString CTransferingContent::GetFileName(CString csFilePath)
{
   CString csFileName;
   csFileName = csFilePath;

   int iSize = csFilePath.GetLength();
   for(int i = iSize - 1;i > 0;i--)
   {
      if ((csFilePath[i] == _T('\\')) || (csFilePath[i] == _T('/')))
      {
         csFileName = csFilePath.Right(iSize - i - 1);
         break;
      }
   }

   return csFileName;
}

double CTransferingContent::GetUploadProgress()
{
   return m_dProgress;
}

UINT __cdecl SessionThreadUpload(LPVOID pParam)
{
   HRESULT hr = S_OK;
   hr = CLanguageSupport::SetThreadLanguage();
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

   CTransferingContent *tc = (CTransferingContent *)pParam;

   *tc->m_utp.pbIsThreadActive = true;
   for(int i = 0; i < tc->m_utp.aFiles.GetSize(); i ++)
   {
      *tc->m_utp.dProgress = 0.0;
      CString src = tc->m_utp.aFiles.GetAt(i);
      CString dest = _T("myfile.lzp");
      int pos = src.ReverseFind(_T('\\'));
      if(pos != -1)
      {
         dest = src.Right(src.GetLength() - pos - 1);
      }
      else
      {
         dest = src;
      }
      *tc->m_utp.piCurrFile = i;
      if(*tc->m_utp.pbIsCancel == true)
      {
         *tc->m_utp.pbIsThreadActive = false;
         return S_OK;
      }
     SavedData sd;
     sd.tp = tc->m_utp.tp;
     sd.csFileOnServer = dest;
     aSData.Add(sd);

	 hr = tc->Upload(tc->m_utp.tp, src, dest, tc->m_utp.dProgress, tc->m_utp.pbIsCancel, tc->m_utp.pbIsThreadActive, tc->m_utp.bDelete);

	  if (hr != S_OK)
	  {
		  aSData.RemoveAt(aSData.GetSize()-1);
	  }
      if(*tc->m_utp.pbIsCancel == true)
      {
         *tc->m_utp.pbIsThreadActive = false;
         return S_OK;
      }
   }
   *tc->m_utp.pbIsThreadActive = false;
   return hr;
}

UINT __cdecl SessionThreadYtUpload(LPVOID pParam)
{
    HRESULT hr = S_OK;
    hr = CLanguageSupport::SetThreadLanguage();
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

    CTransferingContent *tc = (CTransferingContent *)pParam;

    *tc->m_ytp.pbIsThreadActive = true;
    if(*tc->m_ytp.pbIsCancel == true)
    {
        *tc->m_utp.pbIsThreadActive = false;
         return S_OK;
    }
    *tc->m_ytp.piCurrFile = 0;
    *tc->m_ytp.pdProgress = 0.0;
    hr = tc->YtUpload(tc->m_ytp.yTP, tc->m_ytp.csFile, tc->m_ytp.csAuthToken, tc->m_ytp.pdProgress, tc->m_ytp.pbIsCancel, tc->m_ytp.pbIsThreadActive); 

    tc->m_ytp.pbIsThreadActive = false;
    return hr;
}

UINT __cdecl SessionThreadPodcast(LPVOID pParam)
{
   HRESULT hr = S_OK;
   hr = CLanguageSupport::SetThreadLanguage();
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

   CTransferingContent *tc = (CTransferingContent *)pParam;
   *tc->m_ptp.pbIsThreadActive = true;
   for(int i = 0; i < tc->m_ptp.aFiles.GetSize(); i ++)
   {
      *tc->m_ptp.dProgress = 0.0;
      CString src = tc->m_ptp.aFiles.GetAt(i);
      CString dest = _T("myfile.mp4");
      int pos = src.ReverseFind(_T('\\'));
      if(pos != -1)
      {
         dest = src.Right(src.GetLength() - pos - 1);
      }
      else
      {
         dest = src;
      }
      *tc->m_ptp.piCurrFile = i;
      if(*tc->m_ptp.pbIsCancel == true)
      {
         *tc->m_ptp.pbIsThreadActive = false;
         return S_OK;
      }
      int iAction = 2;
      bool bDir = false, bdirEmpty = false;
      CString csTarg = tc->m_ptp.MediaTP.csDirectory + dest;
#ifdef _UPLOADER_EXE
      HRESULT h = tc->CheckTarget(tc->m_ptp.MediaTP.csServer, tc->m_ptp.MediaTP.iService, tc->m_ptp.MediaTP.nPort, tc->m_ptp.MediaTP.csUsername, tc->m_ptp.MediaTP.csPassword, csTarg, &bDir, &bdirEmpty); 
      if(h == S_OK && bDir == false)
      {
         if(*tc->m_ptp.pbIsCancel == true)
         {
            *tc->m_ptp.pbIsThreadActive = false;
            return S_OK;
         }
         HWND hParent = tc->GetParentHwnd();
         CWnd *pParent = CWnd::FromHandle(hParent);
         CFileExists fed(true, pParent);
         fed.SetFilename(dest);
         fed.DoModal();
         iAction = fed.GetAction();
      }
#endif
      if(iAction == 2)// && h == S_OK)
      {
		 SavedData sd;
		 sd.tp = tc->m_ptp.MediaTP;
         sd.csFileOnServer = dest;
         aSData.Add(sd);

		 hr = tc->Podcast(tc->m_ptp.MediaTP, tc->m_ptp.ChannelTP, src, dest, tc->m_ptp.csTitle, tc->m_ptp.csAuthor, tc->m_ptp.dProgress, tc->m_ptp.pbIsCancel, tc->m_ptp.pbIsThreadActive);
		 if (hr != S_OK)
		  {
			  aSData.RemoveAt(aSData.GetSize()-1);
		  }

      }
      else
      {
         *tc->m_ptp.dProgress = 1.0;
      }
      
      if(*tc->m_ptp.pbIsCancel == true)
      {
         *tc->m_ptp.pbIsThreadActive = false;
         return S_OK;
      }
   }
   *tc->m_ptp.pbIsThreadActive = false;
   return hr;
}

UINT CTransferingContent::MyStartUpload(TransferParam tp, CArray<CString, CString>&aUplFiles,double *dProg, int* piCurrFile, bool* pbIsCancel, bool* pbIsThreadActive, bool bDelete /*= false*/)
{
   if(*pbIsCancel == true)
      return S_OK;
   m_csErrStr = _T("");
   m_iErrCode = 0;
   m_utp.tp = tp;
   m_utp.aFiles.RemoveAll();
   for(int i = 0; i < aUplFiles.GetSize(); i++)
   {
      m_utp.aFiles.Add(aUplFiles.GetAt(i));
   }
   m_utp.bDelete = bDelete;
   m_utp.dProgress = dProg;
   m_utp.piCurrFile = piCurrFile;
   m_utp.pbIsCancel = pbIsCancel;
   m_utp.pbIsThreadActive = pbIsThreadActive;
   m_Thread = AfxBeginThread(SessionThreadUpload,
      this,
      THREAD_PRIORITY_NORMAL,
      0,
      CREATE_SUSPENDED);   
   m_Thread->m_bAutoDelete = TRUE;
   m_Thread->ResumeThread();
   return S_OK;
}

UINT CTransferingContent::MyStartPodcast(TransferParam MediaTP, TransferParam ChannelTP, CArray<CString, CString>&aUplFiles, CString csTitle, CString csAuthor, double *dProg, int* piCurrFile, bool* pbIsCancel, bool* pbIsThreadActive)
{
   m_csErrStr = _T("");
   m_iErrCode = 0;
   m_ptp.MediaTP = MediaTP;
   m_ptp.ChannelTP = ChannelTP;
   m_ptp.csTitle = csTitle;
   m_ptp.csAuthor = csAuthor;
   m_ptp.aFiles.RemoveAll();
   for(int i = 0; i < aUplFiles.GetSize(); i++)
   {
      m_ptp.aFiles.Add(aUplFiles.GetAt(i));
   }
   m_ptp.piCurrFile = piCurrFile;
   m_ptp.dProgress = dProg;
   m_ptp.pbIsCancel = pbIsCancel;
   m_ptp.pbIsThreadActive = pbIsThreadActive;
   m_Thread = AfxBeginThread(SessionThreadPodcast,
      this,
      THREAD_PRIORITY_NORMAL,
      0,
      CREATE_SUSPENDED);   
   m_Thread->m_bAutoDelete = TRUE;
   m_Thread->ResumeThread();
   return S_OK;
}

UINT CTransferingContent::MyStartYtUpload(YtTransferParam tp, CString csFile,  CString csAuthToken, double *pdProgress, int* piCurrFile, bool *pbIsCancel, bool *pbIsThreadActive)
{
    if(*pbIsCancel == true)
        return S_OK;
    m_csErrStr = _T("");
    m_iErrCode = 0;
    m_ytp.yTP = tp;
    m_ytp.csFile = csFile;
    m_ytp.csAuthToken = csAuthToken;
    m_ytp.pdProgress = pdProgress;
    m_ytp.piCurrFile = piCurrFile;
    m_ytp.pbIsCancel = pbIsCancel;
    m_ytp.pbIsThreadActive = pbIsThreadActive;
   
    m_Thread = AfxBeginThread(SessionThreadYtUpload,
        this,
        THREAD_PRIORITY_NORMAL,
        0,
        CREATE_SUSPENDED);   
    m_Thread->m_bAutoDelete = TRUE;
    m_Thread->ResumeThread();
    return S_OK;
}

BOOL CTransferingContent::CreateSubDirectory(TransferParam tpTransfer, CString csSubDirName)
{
   BOOL bResult = FALSE;
   CUploaderApp *pApp;
   pApp = (CUploaderApp *)AfxGetApp();
   CFile cfDummy;
   CString csDummyDirName = pApp->GetTempDirectory() + _T("\\") + csSubDirName;
 
   if(CreateDirectory(csDummyDirName, NULL))
   {
      if(tpTransfer.iService == UPLOAD_FTP)
      {
         CFtp ftp;
         
         bResult = ftp.Open(tpTransfer.csServer, tpTransfer.nPort, tpTransfer.csUsername, tpTransfer.csPassword);
         if(bResult)
         {
            bResult = ftp.CreateSubDirectory(tpTransfer.csDirectory, csSubDirName);
            ftp.Close();
         }
      }
      else
      {
         CSsh ssh;
         double dProg;
         bool pbIsCancel = false;
		 bool pbIsThreadActive = false;
         
         bResult = ssh.Upload(csDummyDirName, tpTransfer.csDirectory, tpTransfer, &dProg, &pbIsCancel, &pbIsThreadActive);
      }
      RemoveDirectory(csDummyDirName);
      if(bResult)
      {
         SavedData sd;
         sd.tp = tpTransfer;
         sd.csFileOnServer = csSubDirName;
         aSdSubDir.Add(sd);
      }
   }
   return bResult;
}

BOOL CTransferingContent::RemoveSubDirectory(TransferParam tpTransfer, CString csSubDirName)
{
   BOOL bResult = FALSE;
   if(tpTransfer.iService == UPLOAD_FTP)
      {
         CFtp ftp;
         
         bResult = ftp.Open(tpTransfer.csServer, tpTransfer.nPort, tpTransfer.csUsername, tpTransfer.csPassword);
         if(bResult)
         {
            bResult = ftp.Delete(csSubDirName, tpTransfer.csDirectory, true);
            ftp.Close();
         }
      }
      else
      {
         CSsh ssh;
         double dProg;
         bool pbIsCancel = true;
		 bool pbIsThreadActive = false;
         CString csSubDir;
         if(tpTransfer.iService == UPLOAD_SCP)
         {
            csSubDir = _T("\'") + tpTransfer.csDirectory + _T("\'\'") + csSubDirName + _T("\'");            
         }
         else
         {
            csSubDir = tpTransfer.csDirectory + csSubDirName;
         }         
         bResult = ssh.Delete(csSubDir, tpTransfer, &dProg, &pbIsCancel, &pbIsThreadActive);
      }
   return bResult;
}

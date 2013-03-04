#include "StdAfx.h"
#include "lutils.h"

LInternet::LInternet()
{
   m_hHttp     = NULL;
   m_hInternet = NULL;
}

LInternet::~LInternet()
{
   Close();
}

LURESULT LInternet::OpenUrl(const _TCHAR *tszUrl, DWORD *pdwSysErr, bool bDirect)
{
   LURESULT dwErrorCode = S_OK;
   DWORD dwSystemError = 0;
   
   // Now attempt to connect to the Internet.
   dwSystemError = ::InternetAttemptConnect(0);
   bool success = (dwSystemError == ERROR_SUCCESS);
   if (!success)
      dwErrorCode = LINTERNET_ERR_INTERNETATTEMPT;

   /*
   // This has been commented out because it does not
   // work with proxies. This function does not talk to the proxy,
   // but directly to the given IP address. Not good.
   if (success)
   {
      // There is a possibility for us to contact the internet, now
      // let's check the specific URL.
      //BOOL icres = ::InternetCheckConnection(csUrl, FLAG_ICC_FORCE_CONNECTION, 0);
      BOOL icres = ::InternetCheckConnection(NULL, FLAG_ICC_FORCE_CONNECTION, 0);
      success = (TRUE == icres);
      if (!success)
      {
         dwErrorCode = IDS_ERR_INTERNETCONNECT;
         dwSystemError = ::GetLastError();
      }
   }
   */

   m_hInternet = NULL;
   m_hHttp     = NULL;
   if (success)
   {
      DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
      if (bDirect)
         dwAccessType = INTERNET_OPEN_TYPE_DIRECT;

      // Now let's open a real Internet handle; this does
      // not yet require the URL. Use default proxy settings.
      m_hInternet = ::InternetOpen(_T("LECTURNITY Auto-Update"), 
         dwAccessType,
         NULL, NULL, // Proxy settings
         0);
      success = (m_hInternet != NULL);
      if (!success)
      {
         dwErrorCode = LINTERNET_ERR_INTERNETOPEN;
         dwSystemError = ::GetLastError();
      }
   }
   
   if (success)
   {
      // We have a connection to the internet now; let's open
      // the URL (csUrl) in order to read the file under that
      // address.
      m_hHttp = ::InternetOpenUrl(m_hInternet,
         tszUrl,
         NULL,
         0,
         INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD,
         0);
      success = (m_hHttp != NULL);
      if (!success)
      {
         dwErrorCode = LINTERNET_ERR_INTERNETURL;
         dwSystemError = ::GetLastError();
      }
   }

   if (success)
   {
      // Did we really get a 200 return code?
      char szBuffer[32];
      DWORD dwBufferSize = 31; // Trailing NULL character has to be extra according to spec.
      DWORD dwIndex = 0;
      BOOL ret = ::HttpQueryInfoA(m_hHttp, HTTP_QUERY_STATUS_CODE, szBuffer, &dwBufferSize, &dwIndex);
      if (ret == TRUE)
      {
         DWORD dwReturnCode = atol(szBuffer);
         if (dwReturnCode != HTTP_STATUS_OK) // 200
         {
            if (dwReturnCode == HTTP_STATUS_NOT_FOUND) // 404
               dwErrorCode = LINTERNET_ERR_NOTFOUND;
            else
               dwErrorCode = LINTERNET_ERR_READ;
            success = false;
         }
      }
      // if ret is FALSE, try to load the content anyway. Can't be more than garbage
   }

   if (pdwSysErr)
      *pdwSysErr = dwSystemError;

   return dwErrorCode;
}

LURESULT LInternet::GetLength(DWORD *pdwLength)
{
   if (m_hHttp == NULL)
      return LINTERNET_ERR_NOHANDLE;
   if (pdwLength == NULL)
      return LINTERNET_ERR_INVALIDPARAMS;

   DWORD dwLength     = 0;
   DWORD dwLengthSize = sizeof DWORD;
   DWORD dwIndex      = 0;
   BOOL ret = ::HttpQueryInfo(m_hHttp, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
      &dwLength, &dwLengthSize, &dwIndex);

   if (ret == TRUE)
   {
      *pdwLength = dwLength;
      return S_OK;
   }

   return LINTERNET_ERR_READ;
}

LURESULT LInternet::Read(void *pBuffer, DWORD dwReadSize, DWORD *pdwReadBytes, 
                         DWORD *pdwSystemError /*=NULL*/)
{
   DWORD ret = 0;
   if (m_hHttp == NULL)
      return LINTERNET_ERR_NOHANDLE;
   if (pdwReadBytes == NULL)
      return LINTERNET_ERR_INVALIDPARAMS;

   ret = ::InternetReadFile(m_hHttp, pBuffer, dwReadSize, pdwReadBytes);
   if (ret == TRUE)
   {
      return S_OK;
   }

   if (pdwSystemError)
      *pdwSystemError = ::GetLastError();

   return LINTERNET_ERR_READ;
}

LURESULT LInternet::Close()
{
   if (m_hHttp)
      ::InternetCloseHandle(m_hHttp);
   m_hHttp = NULL;
   if (m_hInternet)
      ::InternetCloseHandle(m_hInternet);
   m_hInternet = NULL;

   return S_OK;
}

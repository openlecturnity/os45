#ifndef __LINTERNET_H__D25865B5_2952_466f_AF00_450EE874F9FA
#define __LINTERNET_H__D25865B5_2952_466f_AF00_450EE874F9FA

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

#define LINTERNET_OK                   S_OK
#define LINTERNET_ERRBASE              0x1000

#define LINTERNET_ERR_INTERNETATTEMPT  (LINTERNET_ERRBASE+0)
#define LINTERNET_ERR_INTERNETOPEN     (LINTERNET_ERRBASE+1)
#define LINTERNET_ERR_INTERNETURL      (LINTERNET_ERRBASE+2)
#define LINTERNET_ERR_NOTFOUND         (LINTERNET_ERRBASE+3)
#define LINTERNET_ERR_READ             (LINTERNET_ERRBASE+4)
#define LINTERNET_ERR_NOHANDLE         (LINTERNET_ERRBASE+5)
#define LINTERNET_ERR_INVALIDPARAMS    (LINTERNET_ERRBASE+6)

#ifndef HINTERNET
  #include <wininet.h>
#endif

class LInternet
{
public:
   LInternet();
   ~LInternet();

   LURESULT OpenUrl(const _TCHAR *tszUrl, DWORD *pdwSysErr=NULL, bool bDirect=false);
   LURESULT GetLength(DWORD *pdwLength);
   LURESULT Read(void *pBuffer, DWORD nReadSize, DWORD *pnReadBytes, 
                 DWORD *pdwSystemError=NULL);
   LURESULT Close();

private:
   HINTERNET m_hInternet;
   HINTERNET m_hHttp;
};

#endif // __LINTERNET_H__D25865B5_2952_466f_AF00_450EE874F9FA
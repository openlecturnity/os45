#ifndef __SECURITYUTILS_H__
#define __SECURITYUTILS_H__

#include "StdAfx.h"

#undef  FULL_VERSION
#define FULL_VERSION 0L
#undef  EVALUATION_VERSION
#define EVALUATION_VERSION 1L
#undef  UNIVERSITY_VERSION
#define UNIVERSITY_VERSION 2L
#undef  UNIVERSITY_EVALUATION_VERSION
#define UNIVERSITY_EVALUATION_VERSION 3L
#undef  VERSION_ERROR
#define VERSION_ERROR -1L

class KerberokHandler;

// Note: This class is just a wrapper to
// the new keygenlib KerberokHandler class.
// If you are going to use any of the security
// functions, rather check there, or use this
// class right as it is.
class LSUTL32_EXPORT CSecurityUtils
{
public:
   CSecurityUtils();
   ~CSecurityUtils();

public:
   bool GetInstallationDate(CString &csInstallDate);
   int  GetEvaluationPeriod();
   int  GetVersionType(CString csOwnVersion);
   void GetVersionString(CString &csVersionString, CString csBetaRelease);
   void GetCompactVersionString(CString &csVersionString, CString csBetaRelease);
   void GetVersionStringShort(CString &csVersionString);
   bool GetExpirationDate(CString &csExpirationDate);
   bool GetLanguageCode(CString &csLangCode);
   bool GetLecturnityHome(CString &csLecturnityHome);

private:
   KerberokHandler *m_pKerberok;
};

#endif // __SECURITYUTILS_H__
/*********************************************************************

 program  : securityutils.cpp
 authors  : Gabriela Maass/Martin Danielsson
 date     : 28.10.2003
 revision : $Id: securityutils.cpp,v 1.7 2009-06-23 08:21:54 kuhn Exp $
 desc     : Functions to evaluate the validity of a Program

 **********************************************************************/

#include "StdAfx.h"
#include <io.h>
#include <atlbase.h>

#include "securityutils.h"
#include "KeyGenerator.h"

CSecurityUtils::CSecurityUtils()
{
   m_pKerberok = NULL;
   m_pKerberok = new KerberokHandler();
}

CSecurityUtils::~CSecurityUtils()
{
   if (m_pKerberok)
      delete m_pKerberok;
   m_pKerberok = NULL;
}

bool CSecurityUtils::GetInstallationDate(CString &csInstallDate)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   _TCHAR tszInstallDate[128];
   bool ret = m_pKerberok->GetInstallationDate(tszInstallDate);
   csInstallDate = tszInstallDate;
   return ret;
}

int CSecurityUtils::GetVersionType(CString csOwnVersion)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   return m_pKerberok->GetVersionType(csOwnVersion);
}

void CSecurityUtils::GetVersionString(CString &csVersionString, CString csBetaRelease)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   _TCHAR tszVersion[128];
   m_pKerberok->GetVersionString(tszVersion, csBetaRelease);

   csVersionString = tszVersion;
}

void CSecurityUtils::GetCompactVersionString(CString &csVersionString, CString csBetaRelease)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   _TCHAR tszVersion[128];
   m_pKerberok->GetCompactVersionString(tszVersion, csBetaRelease);

   csVersionString = tszVersion;
}

void CSecurityUtils::GetVersionStringShort(CString &csVersionString)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   _TCHAR tszVersion[128];
   m_pKerberok->GetVersionStringShort(tszVersion);

   csVersionString = tszVersion;
}

bool CSecurityUtils::GetExpirationDate(CString &csExpirationDate)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   _TCHAR tszExpirationDate[128];
   bool ret = m_pKerberok->GetExpirationDate(tszExpirationDate);
   csExpirationDate = tszExpirationDate;
   return ret;
}

int CSecurityUtils::GetEvaluationPeriod()
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   return m_pKerberok->GetEvaluationPeriod();
}

bool CSecurityUtils::GetLanguageCode(CString &csLanguageCode)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   _TCHAR tszLanguageCode[128];
   bool ret = m_pKerberok->GetLanguageCode(tszLanguageCode);
   csLanguageCode = tszLanguageCode;
   return ret;
}

bool CSecurityUtils::GetLecturnityHome(CString &csLecturnityHome)
{
#ifndef _LSUTL32_STATIC
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   _TCHAR tszLecturnityHome[MAX_PATH];
   bool ret = m_pKerberok->GetLecturnityHome(tszLecturnityHome);
   csLecturnityHome = tszLecturnityHome;
   return ret;
}

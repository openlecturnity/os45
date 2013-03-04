// Registry.cpp: implementation of the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "podcaster.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry()
{
   HKEY hKey = NULL;
}

CRegistry::~CRegistry()
{

}

LONG CRegistry::Create(CString csKey)
{
   CString csOpenKey = PODCASTER_KEY;
   csOpenKey += csKey;
   lResult = ::RegCreateKeyEx(HKEY_CURRENT_USER,
         csOpenKey,
         0,
         NULL,
         REG_OPTION_NON_VOLATILE,
         KEY_ALL_ACCESS,
         NULL,
         &hKey,
         NULL);

   if(lResult != ERROR_SUCCESS)
   {
      lResult;
   }
   
   return lResult;
}

LONG CRegistry::Open(CString csKey)
{
   CString csOpenKey = PODCASTER_KEY;
   csOpenKey += csKey;
   lResult = ::RegOpenKeyEx(HKEY_CURRENT_USER,
         csOpenKey,
         0,
         KEY_ALL_ACCESS,
         &hKey);

   if(lResult != ERROR_SUCCESS)
   {
      lResult;
   }
   
   return lResult;
}

LONG CRegistry::Close()
{
   lResult = -1;
   if(hKey != NULL)
   {
      lResult = ::RegCloseKey(hKey);
   }
   return lResult;
}

LONG CRegistry::WriteProfileInt(CString pszSubKey, int iValue)
{
   lResult = -1;
   if(hKey != NULL)
   {
      lResult = ::RegSetValueEx(hKey,
         pszSubKey,
         NULL,
         REG_DWORD,
         (BYTE *)&iValue,
         sizeof(iValue)
      );
   }
   return lResult;
}
   
LONG CRegistry::WriteProfileString(CString pszSubKey, CString csValue)
{
   LPCTSTR sValue = (LPCTSTR)csValue;
   int iLength = csValue.GetLength() * sizeof(TCHAR);
   lResult = -1;
   if(hKey != NULL)
   {
      lResult = ::RegSetValueEx(hKey,
         pszSubKey,
         NULL,
         REG_SZ,
         (BYTE *)sValue,
         iLength
      );
   }
   return lResult;
}

int CRegistry::GetProfileInt(CString pszSubKey, int iDefaultValue)
{
   int iValue = iDefaultValue;
   DWORD cbData = sizeof(int);
   DWORD dwType = REG_DWORD; 
   if(hKey != NULL)
   {
      LONG lResult = ::RegQueryValueEx(hKey,
         pszSubKey,
         NULL,
         &dwType,
         (BYTE *)&iValue,
         &cbData
      );
   }
   return iValue;
}

CString CRegistry::GetProfileString(CString pszSubKey, CString csDefaultValue)
{
   TCHAR buffer[MAX_PATH];
   CString csValue = csDefaultValue;
   DWORD cbData = MAX_PATH - 1;
   DWORD dwType = REG_SZ; 
   if(hKey != NULL)
   {
      lResult = ::RegQueryValueEx(hKey,
         pszSubKey,
         NULL,
         &dwType,
         (BYTE *)buffer,
         &cbData
      );
      if(lResult == ERROR_SUCCESS)
      {
         csValue = CString(buffer, (cbData / sizeof(TCHAR)));
      }
      else
      {
         csValue = csDefaultValue;
      }
   }
   return csValue;
}

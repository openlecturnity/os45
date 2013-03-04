#include "StdAfx.h"
#include "lutils.h"

const TCHAR *tszLECTURNITY = _T("SOFTWARE\\imc AG\\LECTURNITY");

bool LRegistry::ReadRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue,
                                              DWORD dwType, char *pData, DWORD *pcbSize)
{
   HKEY hkKey   = NULL;
   LONG res = ::RegOpenKeyEx(hkHive, szKey, 0, KEY_READ, &hkKey);
   if (res == ERROR_SUCCESS)
   {
      res = ::RegQueryValueEx(hkKey, szValue, NULL, &dwType, (LPBYTE) pData, pcbSize);
      RegCloseKey(hkKey);
   }

   return (res == ERROR_SUCCESS);
}

bool LRegistry::ReadBinaryRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue,
                                        char *pData, DWORD *pcbSize)
{
   return ReadRegistryEntry(hkHive, szKey, szValue, REG_BINARY, pData, pcbSize);
}

bool LRegistry::ReadStringRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, 
                                        _TCHAR *tszString, DWORD *pcbSize)
{
   bool success = ReadRegistryEntry(hkHive, szKey, szValue, REG_SZ, (char *)tszString, pcbSize);
   if (!success)
      success = ReadRegistryEntry(hkHive, szKey, szValue, REG_EXPAND_SZ, (char *)tszString, pcbSize);
   return success;
}

bool LRegistry::ReadDwordRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, DWORD *pdwEntry)
{
   DWORD dwSize = sizeof DWORD;
   return ReadRegistryEntry(hkHive, szKey, szValue, REG_DWORD, (char *) pdwEntry, &dwSize);
}

bool LRegistry::ReadBoolRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, bool *pbEntry)
{
   DWORD dwEntry;
   bool success = ReadDwordRegistryEntry(hkHive, szKey, szValue, &dwEntry);
   if (success)
      success = (pbEntry != NULL);
   if (success)
      *pbEntry = (dwEntry != 0);

   return success;
}

bool LRegistry::WriteRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, 
                                   DWORD dwType, const char *pData, DWORD dwSize)
{
   HKEY hKey = NULL;
   DWORD dwDisposition = 0;
   LONG res = ::RegCreateKeyEx(hkHive, szKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_WRITE, 
      NULL, &hKey, &dwDisposition);
   if (res == ERROR_SUCCESS)
   {
      res = ::RegSetValueEx(hKey, szValue, 0, dwType, (const LPBYTE) pData, dwSize);
      RegCloseKey(hKey);
   }

   return (res == ERROR_SUCCESS);
}

bool LRegistry::WriteBinaryRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, 
                                         const char *pData, DWORD dwSize)
{
   return WriteRegistryEntry(hkHive, szKey, szValue, REG_BINARY, pData, dwSize);
}

bool LRegistry::WriteStringRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, 
                                         const _TCHAR *tszString)
{
   int nByteLength = (_tcslen(tszString) + 1) * sizeof _TCHAR; // Include the 0-Terminating char
   return WriteRegistryEntry(hkHive, szKey, szValue, REG_SZ, (char *)tszString, nByteLength);
}

bool LRegistry::WriteDwordRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, 
                                        DWORD dwEntry)
{
   return WriteRegistryEntry(hkHive, szKey, szValue, REG_DWORD, (char *) (&dwEntry), sizeof DWORD);
}

bool LRegistry::WriteBoolRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, 
                                       bool bEntry)
{
   DWORD dwEntry = bEntry ? 1 : 0;
   return WriteDwordRegistryEntry(hkHive, szKey, szValue, dwEntry);
}

void CreateRegistryKey(IN LPCTSTR szSubkey, OUT LPTSTR szKey)
{
   if (szSubkey && _tcscmp(szSubkey, _T("")) != 0)
      _stprintf(szKey, _T("%s\\%s"), tszLECTURNITY, szSubkey);
   else
      _tcscpy(szKey, tszLECTURNITY);
}

// Checks HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue first, then
// HKLM\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue, after that it takes the default
// Always returns true.
bool LRegistry::ReadSettingsString(LPCTSTR szSubkey, LPCTSTR szValue, 
                                   _TCHAR *tszString, DWORD *pdwSize, const _TCHAR *tszDefault, 
                                   bool bReadHKLM /*=false*/)
{
   TCHAR szKey[512];
   CreateRegistryKey(szSubkey, szKey);
   DWORD dwBackup = *pdwSize;
   bool success = false;
   if (!bReadHKLM)
      success = ReadStringRegistryEntry(HKEY_CURRENT_USER, szKey, szValue, tszString, pdwSize);
   if (!success)
   {
      *pdwSize = dwBackup;
      success = ReadStringRegistryEntry(HKEY_LOCAL_MACHINE, szKey, szValue, tszString, pdwSize);
   }

   if (!success && tszDefault != NULL)
   {
      _tcscpy(tszString, tszDefault);
      success = true;
   }

   return success;
}

// Checks HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue first, then
// HKLM\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue. If that fails, too,
// false is returned, otherwise true.
bool LRegistry::ReadSettingsBinary(LPCTSTR szSubkey, LPCTSTR szValue, 
                                   char *pData, DWORD *pdwSize, 
                                   bool bReadHKLM /*=false*/)
{
   TCHAR szKey[512];
   CreateRegistryKey(szSubkey, szKey);

   DWORD dwBackup = *pdwSize;
   bool success = false;
   if (!bReadHKLM)
      success = ReadBinaryRegistryEntry(HKEY_CURRENT_USER, szKey, szValue, pData, pdwSize);
   if (!success)
   {
      *pdwSize = dwBackup;
      success = ReadBinaryRegistryEntry(HKEY_LOCAL_MACHINE, szKey, szValue, pData, pdwSize);
   }

   return success;
}

// Checks HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue first, then
// HKLM\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue, after that it takes the default
// Always returns true.
bool LRegistry::ReadSettingsDword(LPCTSTR szSubkey, LPCTSTR szValue, 
                                  DWORD *pdwEntry, DWORD dwDefault, 
                                  bool bReadHKLM /*=false*/)
{
   TCHAR szKey[512];
   CreateRegistryKey(szSubkey, szKey);
   bool success = false;
   if (!bReadHKLM)
      success = ReadDwordRegistryEntry(HKEY_CURRENT_USER, szKey, szValue, pdwEntry);
   if (!success)
      success = ReadDwordRegistryEntry(HKEY_LOCAL_MACHINE, szKey, szValue, pdwEntry);
   if (!success)
   {
      *pdwEntry = dwDefault;
      success = true;
   }

   return success;
}

// Checks HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue first, then
// HKLM\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue, after that it takes the default
// Always returns true.
bool LRegistry::ReadSettingsBool(LPCTSTR szSubkey, LPCTSTR szValue, 
                                 bool *pbEntry, bool bDefault, 
                                 bool bReadHKLM /*=false*/)
{
   TCHAR szKey[512];
   CreateRegistryKey(szSubkey, szKey);
   bool success = false;
   if (!bReadHKLM)
      success = ReadBoolRegistryEntry(HKEY_CURRENT_USER, szKey, szValue, pbEntry);
   if (!success)
      success = ReadBoolRegistryEntry(HKEY_LOCAL_MACHINE, szKey, szValue, pbEntry);
   if (!success)
   {
      *pbEntry = bDefault;
      success = true;
   }

   return success;
}

// Writes to HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>: szValue
bool LRegistry::WriteSettingsString(LPCTSTR szSubkey, LPCTSTR szValue, 
                                    const _TCHAR *tszString, bool bForAll)
{
   TCHAR szKey[512];
   CreateRegistryKey(szSubkey, szKey);

   bool success = false;
   if (bForAll)
   {
      success = WriteStringRegistryEntry(HKEY_LOCAL_MACHINE, szKey, szValue, tszString);
      if (success)
         DeleteRegistryValue(HKEY_CURRENT_USER, szKey, szValue);
   }

   if (!success)
   {
      success = WriteStringRegistryEntry(HKEY_CURRENT_USER, szKey, szValue, tszString);
   }
   return success;
}

// Writes to HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>: szValue
bool LRegistry::WriteSettingsBinary(LPCTSTR szSubkey, LPCTSTR szValue, 
                                    const char *pData, DWORD dwSize, bool bForAll)
{
   TCHAR szKey[512];
   CreateRegistryKey(szSubkey, szKey);

   bool success = false;
   if (bForAll)
   {
      success = WriteBinaryRegistryEntry(HKEY_LOCAL_MACHINE, szKey, szValue, pData, dwSize);
      if (success)
         DeleteRegistryValue(HKEY_CURRENT_USER, szKey, szValue);
   }

   if (!success)
   {
      success = WriteBinaryRegistryEntry(HKEY_CURRENT_USER, szKey, szValue, pData, dwSize);
   }

   return success;
}

// Writes to HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>: szValue
bool LRegistry::WriteSettingsDword(LPCTSTR szSubkey, LPCTSTR szValue, 
                                   DWORD dwEntry, bool bForAll)
{
   TCHAR szKey[512];
   CreateRegistryKey(szSubkey, szKey);

   bool success = false;
   if (bForAll)
   {
      success = WriteDwordRegistryEntry(HKEY_LOCAL_MACHINE, szKey, szValue, dwEntry);
      if (success)
         DeleteRegistryValue(HKEY_CURRENT_USER, szKey, szValue);
   }

   if (!success)
   {
      success = WriteDwordRegistryEntry(HKEY_CURRENT_USER, szKey, szValue, dwEntry);
   }

   return success;
}

// Writes to HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>: szValue
bool LRegistry::WriteSettingsBool(LPCTSTR szSubkey, LPCTSTR szValue, 
                                  bool bEntry, bool bForAll)
{
   TCHAR szKey[512];
   CreateRegistryKey(szSubkey, szKey);

   bool success = false;
   if (bForAll)
   {
      success = WriteBoolRegistryEntry(HKEY_LOCAL_MACHINE, szKey, szValue, bEntry);
      if (success)
         DeleteRegistryValue(HKEY_CURRENT_USER, szKey, szValue);
   }

   if (!success)
   {
      success = WriteBoolRegistryEntry(HKEY_CURRENT_USER, szKey, szValue, bEntry);
   }

   return success;
}

// Moves one registry key to another position, including subkeys
bool LRegistry::MoveRegistryKey(HKEY hkFrom, LPCTSTR szFrom, 
                                HKEY hkTo, LPCTSTR szTo)
{
   bool success = CopyRegistryKey(hkFrom, szFrom, hkTo, szTo);
   if (success)
      DeleteRegistryKey(hkFrom, szFrom);
   return success;
}

// Copies one registry key to another position, including subkeys
bool LRegistry::CopyRegistryKey(HKEY hkFromMajor, LPCTSTR szFrom, 
                                HKEY hkToMajor, LPCTSTR szTo)
{
   HKEY hKeyFrom = NULL;
   HKEY hKeyTo   = NULL;

   LONG res = RegOpenKeyEx(hkFromMajor, szFrom, 0, KEY_READ, &hKeyFrom);
   if (res == ERROR_SUCCESS)
   {
      // Create the new key, or open it if it already exists.
      DWORD dwDisp = 0;
      res = RegCreateKeyEx(hkToMajor, szTo, 0, NULL, REG_OPTION_NON_VOLATILE, 
         KEY_ALL_ACCESS, NULL, &hKeyTo, &dwDisp);
   }

   if (res == ERROR_SUCCESS)
   {
      // First copy all the values in the current key
      DWORD dwIndex = 0;
      LONG res2 = ERROR_SUCCESS;
      while (ERROR_SUCCESS == res2 && res == ERROR_SUCCESS)
      {
         TCHAR szValueName[1024];
         DWORD dwValueNameSize = 1024;
         DWORD dwType = 0;
         LPBYTE pData = NULL;
         DWORD dwDataSize = 0;
         // query necessary size for pData
         // enumerates only the values not the (possible) sub-keys
         res2 = RegEnumValue(hKeyFrom, dwIndex, szValueName, &dwValueNameSize, NULL,
            &dwType, NULL, &dwDataSize);
         if (res2 == ERROR_SUCCESS)
         {
            dwValueNameSize = 1024;
            if (dwDataSize != 0)
               pData = new BYTE[dwDataSize];
            // actually fill pData:
            res2 = RegEnumValue(hKeyFrom, dwIndex, szValueName, &dwValueNameSize, NULL, 
               &dwType, pData, &dwDataSize);
         }
      
         if (res2 == ERROR_SUCCESS)
         {
            res2 = RegSetValueEx(hKeyTo, szValueName, 0, dwType, pData, dwDataSize);
         }
      
         if (pData)
            delete[] pData;
         pData = NULL;
      
         dwIndex++;
      }
   
      // Then call recursively for all subkeys in the current key
      dwIndex = 0;
      res2 = ERROR_SUCCESS;
      while (res2 == ERROR_SUCCESS)
      {
         TCHAR szSubKey[1024];
         DWORD dwSubKeySize = 1024;
         TCHAR szClass[1024];
         DWORD dwClassSize = 1024;
         FILETIME fileTime;

         res2 = RegEnumKeyEx(hKeyFrom, dwIndex, szSubKey, &dwSubKeySize, NULL,
            szClass, &dwClassSize, &fileTime);
         if (res2 == ERROR_SUCCESS)
         {
            res = CopyRegistryKey(hKeyFrom, szSubKey, hKeyTo, szSubKey);
         }
         dwIndex++;
      }
   }

   if (hKeyFrom)
      RegCloseKey(hKeyFrom);
   if (hKeyTo)
      RegCloseKey(hKeyTo);

   return (res == ERROR_SUCCESS);
}

// Deletes a registry key, including its subkeys
void LRegistry::DeleteRegistryKey(HKEY hkMain, LPCTSTR szKey)
{
   DWORD dwIndex = 0;
   HKEY hKey = NULL;
   LONG res = RegOpenKeyEx(hkMain, szKey, 0, KEY_ALL_ACCESS, &hKey);

   while (res == ERROR_SUCCESS)
   {
      TCHAR szSubKey[1024];
      DWORD dwSubKeySize = 1024;
      FILETIME fileTime;

      res = RegEnumKeyEx(hKey, dwIndex, szSubKey, &dwSubKeySize, NULL, NULL, NULL, &fileTime);
      if (res == ERROR_SUCCESS)
      {
         DeleteRegistryKey(hKey, szSubKey);
      }
      // dwIndex remains 0, as we have deleted the first sub key
   }

   if (hKey)
      RegCloseKey(hKey);

   RegDeleteKey(hkMain, szKey);
}

bool LRegistry::DeleteRegistryValue(HKEY hkMain, LPCTSTR szSubKey, LPCTSTR szValue)
{
   HKEY hkKey = NULL;
   LONG res = ::RegOpenKeyEx(hkMain, szSubKey, 0, KEY_ALL_ACCESS, &hkKey);
   if (ERROR_SUCCESS == res)
   {
      res = ::RegDeleteValue(hkKey, szValue);
      ::RegCloseKey(hkKey);
      hkKey = NULL;
   }

   return (ERROR_SUCCESS == res);
}

bool LRegistry::EnumRegistryValues(HKEY hkMain, LPCTSTR tszSubKey, ENTRY **paEntries, DWORD *dwCount, bool bSort)
{
   if (dwCount == NULL)
      return false;

   HKEY hkKey = NULL;
   LONG res = ::RegOpenKeyEx(hkMain, tszSubKey, 0, KEY_READ, &hkKey);
   if (ERROR_SUCCESS == res)
   {
      DWORD dwValueCount = 0;
      DWORD dwEnumCounter = 0;
      DWORD dwType = 0;
      _TCHAR tszName[256];
      while (ERROR_SUCCESS == res)
      {
         DWORD dwSize = 255;
         res = ::RegEnumValue(hkKey, dwEnumCounter, tszName, &dwSize, NULL, &dwType, NULL, NULL);
         if (ERROR_SUCCESS == res)
         {
            if (_tcscmp(tszName, _T("")) != 0)
            {
               if (paEntries)
               {
                  paEntries[dwValueCount]->lsName = tszName;
                  paEntries[dwValueCount]->dwType = dwType;
               }
               dwValueCount++;
            }
            dwEnumCounter++;
         }
      }

      if (ERROR_NO_MORE_ITEMS == res)
      {
         res = ERROR_SUCCESS;
         *dwCount = dwValueCount;
      }
      else
      {
         *dwCount = 0;
      }
   }

   if (ERROR_SUCCESS == res && bSort && paEntries)
   {
      ENTRY *pSmallest      = NULL;
      int    nSmallestIndex = 0;
      int    nCount = (int) *dwCount;
      // Insert Sort
      for (int i=0; i<nCount; ++i)
      {
         pSmallest = paEntries[i];
         nSmallestIndex = i;

         for (int j=i+1; j<nCount; ++j)
         {
            if (_tcsicmp((LPCTSTR) paEntries[j]->lsName, (LPCTSTR) pSmallest->lsName) < 0)
            {
               pSmallest = paEntries[j];
               nSmallestIndex = j;
            }
         }

         if (nSmallestIndex != i)
         {
            paEntries[nSmallestIndex] = paEntries[i];
            paEntries[i] = pSmallest;
         }
      }
   }

   return (ERROR_SUCCESS == res);
}


bool LRegistry::EnumRegistryKeys(HKEY hkMain, LPCTSTR tszSubKey, ENTRY **paEntries, DWORD *dwCount, bool bSort)
{
   if (dwCount == NULL)
      return false;

   HKEY hkKey = NULL;
   LONG res = ::RegOpenKeyEx(hkMain, tszSubKey, 0, KEY_READ, &hkKey);
   if (ERROR_SUCCESS == res)
   {
      DWORD dwValueCount = 0;
      DWORD dwEnumCounter = 0;
      _TCHAR tszName[256];
      while (ERROR_SUCCESS == res)
      {
         DWORD dwSize = 255;
         res = ::RegEnumKey(hkKey, dwEnumCounter, tszName, dwSize);
         if (ERROR_SUCCESS == res)
         {
            if (_tcscmp(tszName, _T("")) != 0)
            {
               if (paEntries)
               {
                  paEntries[dwValueCount]->lsName = tszName;
               }
               dwValueCount++;
            }
            dwEnumCounter++;
         }
      }

      if (ERROR_NO_MORE_ITEMS == res)
      {
         res = ERROR_SUCCESS;
         *dwCount = dwValueCount;
      }
      else
      {
         *dwCount = 0;
      }
   }

   if (ERROR_SUCCESS == res && bSort && paEntries)
   {
      ENTRY *pSmallest      = NULL;
      int    nSmallestIndex = 0;
      int    nCount = (int) *dwCount;
      // Insert Sort
      for (int i=0; i<nCount; ++i)
      {
         pSmallest = paEntries[i];
         nSmallestIndex = i;

         for (int j=i+1; j<nCount; ++j)
         {
            if (_tcsicmp((LPCTSTR) paEntries[j]->lsName, (LPCTSTR) pSmallest->lsName) < 0)
            {
               pSmallest = paEntries[j];
               nSmallestIndex = j;
            }
         }

         if (nSmallestIndex != i)
         {
            paEntries[nSmallestIndex] = paEntries[i];
            paEntries[i] = pSmallest;
         }
      }
   }

   return (ERROR_SUCCESS == res);
}
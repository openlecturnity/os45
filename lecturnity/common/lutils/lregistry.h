#ifndef __LREGISTRY_H__A2D09CE1_4021_4fc2_8191_4ED7007CF528
#define __LREGISTRY_H__A2D09CE1_4021_4fc2_8191_4ED7007CF528

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

namespace LRegistry
{
   typedef struct
   {
      LString lsName;
      DWORD   dwType;
   } ENTRY;

   // General purpose Registry functions
   bool ReadRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, DWORD dwType, char *pData, DWORD *pcbSize);
   bool ReadBinaryRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, char *pData, DWORD *pcbSize);
   bool ReadStringRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, _TCHAR *tszString, DWORD *pcbSize);
   bool ReadDwordRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, DWORD *pdwEntry);
   bool ReadBoolRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, bool *pbEntry);

   bool WriteRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, DWORD dwType, const char *pData, DWORD dwSize);
   bool WriteBinaryRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, const char *pData, DWORD dwSize);
   bool WriteStringRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, const _TCHAR *tszString);
   bool WriteDwordRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, DWORD dwEntry);
   bool WriteBoolRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, bool bEntry);

   // Special purpose Registry functions

   // Checks HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue first, then
   // HKLM\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue, after that it takes the default
   // Always returns true, if szDefault is non-NULL.
   bool ReadSettingsString(LPCTSTR szSubkey, LPCTSTR szValue, _TCHAR *tszString, DWORD *pdwSize, const _TCHAR *tszDefault, bool bReadHKLM=false);

   // Checks HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue first, then
   // HKLM\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue. If that fails, too,
   // false is returned, otherwise true.
   bool ReadSettingsBinary(LPCTSTR szSubkey, LPCTSTR szValue, char *pData, DWORD *pdwSize, bool bReadHKLM=false);

   // Checks HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue first, then
   // HKLM\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue, after that it takes the default
   // Always returns true.
   bool ReadSettingsDword(LPCTSTR szSubkey, LPCTSTR szValue, DWORD *pdwEntry, DWORD dwDefault, bool bReadHKLM=false);

   // Checks HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue first, then
   // HKLM\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>:szValue, after that it takes the default
   // Always returns true.
   bool ReadSettingsBool(LPCTSTR szSubkey, LPCTSTR szValue, bool *pbEntry, bool bDefault, bool bReadHKLM=false);

   // Writes to HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>: szValue
   bool WriteSettingsString(LPCTSTR szSubkey, LPCTSTR szValue, const _TCHAR *tszString, bool bForAll=false);

   // Writes to HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>: szValue
   bool WriteSettingsBinary(LPCTSTR szSubkey, LPCTSTR szValue, const char *pData, DWORD dwSize, bool bForAll=false);

   // Writes to HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>: szValue
   bool WriteSettingsDword(LPCTSTR szSubkey, LPCTSTR szValue, DWORD dwEntry, bool bForAll=false);

   // Writes to HKCU\\SOFTWARE\\imc AG\\LECTURNITY\\<szSubkey>: szValue
   bool WriteSettingsBool(LPCTSTR szSubkey, LPCTSTR szValue, bool bEntry, bool bForAll=false);

   // Moves one registry key to another position, including subkeys
   bool MoveRegistryKey(HKEY hkFrom, LPCTSTR szSubKeyFrom, HKEY hkTo, LPCTSTR szSubKeyTo);

   // Copies one registry key to another position, including subkeys
   bool CopyRegistryKey(HKEY hkFrom, LPCTSTR szSubKeyFrom, HKEY hkTo, LPCTSTR szSubKeyTo);

   // Deletes a registry key, including its subkeys
   void DeleteRegistryKey(HKEY hkMain, LPCTSTR szSubKey);

   // Deletes a registry value in the specified subkey
   bool DeleteRegistryValue(HKEY hkMain, LPCTSTR szSubKey, LPCTSTR szValue);

   // Enumerated the registry values in the specified subkey.
   // If paEntries is NULL, the needed count is returned in
   // dwCount. Returns true if successful or false if not
   // successful, e.g. if dwCount is NULL.
   bool EnumRegistryValues(HKEY hkMain, LPCTSTR tszSubKey, ENTRY **paEntries, DWORD *dwCount, bool bSort=false);
   
   // Enumerated the registry keys in the specified subkey.
   // If paEntries is NULL, the needed count is returned in
   // dwCount. Returns true if successful or false if not
   // successful, e.g. if dwCount is NULL.
   bool EnumRegistryKeys(HKEY hkMain, LPCTSTR tszSubKey, ENTRY **paEntries, DWORD *dwCount, bool bSort=false);
};

#endif // __LREGISTRY_H__A2D09CE1_4021_4fc2_8191_4ED7007CF528

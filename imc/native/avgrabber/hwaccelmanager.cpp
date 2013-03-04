#include "global.h"
//#include <windows.h>
//#include <tchar.h>
//#include <stdio.h>
//#include "hwaccelmanager.h"

#define MY_ASSERT(x, y) \
   if (!(x))\
   {\
      ::MessageBox(NULL, y, NULL, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);\
      return false;\
   }

HwAccelManager::HwAccelManager()
{
   OSVERSIONINFO osVerInfo;
   memset(&osVerInfo, 0, sizeof OSVERSIONINFO);
   osVerInfo.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
   if (::GetVersionEx(&osVerInfo) == FALSE)
   {
      isEnabled_ = false;
      return;
   }

   bool isNT = osVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT;
   if (!isNT)
   {
      // works only on Win2K, Win XP
      isEnabled_ = false;
      return;
   }

   // Win2K and XP (and later) have a major version >= 5.
   bool is2KorXP = (osVerInfo.dwMajorVersion >= 5);

   isEnabled_ = is2KorXP;
}

HwAccelManager::~HwAccelManager()
{
}

bool HwAccelManager::IsEnabled()
{
   return isEnabled_;
}

bool HwAccelManager::OpenAccelerationLevel(PHKEY phkDevice, LPDWORD pdwAccLevel)
{
   int iDeviceLeft = 0;
   int iDevId = 0;
   bool bPrimaryDeviceFound = false;

   DISPLAY_DEVICE oDevice;
   ZeroMemory(&oDevice, sizeof(oDevice));
   oDevice.cb = sizeof(oDevice);
   do
   {
      // find correct display device and its registry key

      iDeviceLeft = ::EnumDisplayDevices(NULL, iDevId, &oDevice, 0); 
   
      if(iDeviceLeft == 0)
         break;

      if(oDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
      {
         bPrimaryDeviceFound = true;
         break;
      }
   }
   while (iDeviceLeft > 0);


   MY_ASSERT((bPrimaryDeviceFound), _T("Could not get information on current video adapter."));

   TCHAR *tszRM = _T("\\Registry\\Machine\\"); // must be present; will be cut out

   MY_ASSERT((_tcsnicmp(tszRM, oDevice.DeviceKey, 18) == 0),
             _T("Could not retrieve specific information on video adapter."));

   TCHAR tszDeviceKey[1024];
   _tcscpy(tszDeviceKey, oDevice.DeviceKey + 18);
   // this is usually something like: HKLM\System\CurrentControlSet\Control\Video\[some GUID]\0000

   LONG result = ::RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, tszDeviceKey, 0,
      KEY_ALL_ACCESS, phkDevice);
   MY_ASSERT((result == ERROR_SUCCESS), _T("Failed to read video hardware acceleration level."));

   *pdwAccLevel = 0;
   DWORD dwType       = REG_DWORD;
   DWORD dwLength     = sizeof DWORD;
   result = ::RegQueryValueEx(*phkDevice, 
                              _T("Acceleration.Level"),
                              NULL,
                              &dwType,
                              (LPBYTE) pdwAccLevel,
                              &dwLength);
   if (result == ERROR_SUCCESS)
   {
   }
   else if (result == ERROR_FILE_NOT_FOUND)
   {
      *pdwAccLevel = 0;
   }
   else
   {
      ::RegCloseKey(*phkDevice);
      *phkDevice = NULL;
      ::MessageBox(NULL, _T("An unknown error occurred while setting hardware acceleration level."),
                   NULL, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
      return false;
   }

   return true;
}

bool HwAccelManager::SetAccelerationLevel(long ulMode)
{
   if (!isEnabled_)
      return false;

   HKEY hkDevice;
   DWORD dwAccLevel;

   if (!OpenAccelerationLevel(&hkDevice, &dwAccLevel))
      return false;

   if ((long) dwAccLevel != ulMode)
   {
      if (ulMode != 0x0)
      {
         dwAccLevel     = ulMode;
         DWORD dwLength = sizeof DWORD;
         LONG result = ::RegSetValueEx(hkDevice,
                                      _T("Acceleration.Level"),
                                       0,
                                       REG_DWORD,
                                       (LPBYTE) &dwAccLevel,
                                       dwLength);
         ::RegCloseKey(hkDevice);
         MY_ASSERT((result == ERROR_SUCCESS), 
                   _T("Could not set video hardware acceleration level."));
      }
      else
      {
         // delete Acceleration.Level key
         LONG result = ::RegDeleteValue(hkDevice, _T("Acceleration.Level"));
         ::RegCloseKey(hkDevice);
         MY_ASSERT((result == ERROR_SUCCESS),
                   _T("Could not fully enable video hardware acceleration level."));
      }

      // Now tickle Windows to reload settings
      DEVMODE devMode;
      BOOL success = ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
      MY_ASSERT((success == TRUE),
                _T("Could not retrieve current video device settings."));
      ::ChangeDisplaySettings(&devMode, CDS_RESET);
   }
   else
   {
      ::RegCloseKey(hkDevice);
   }

   return true;
}

long HwAccelManager::GetAccelerationLevel()
{
   // The meaning of this value (at least w2k, xp; w2k3 might be different):
   // 0x0 (or missing) full acceleration activated
   // 0x5 fully disabled

   if (!isEnabled_)
      return false;

   HKEY hkDevice    = NULL;
   DWORD dwAccLevel = 0;

   bool success = OpenAccelerationLevel(&hkDevice, &dwAccLevel);
   if (hkDevice)
      ::RegCloseKey(hkDevice);

   if (success)
      return dwAccLevel;
   else
      return -1;
}

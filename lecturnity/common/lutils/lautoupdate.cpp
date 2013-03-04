#include "StdAfx.h"
#include "lutils.h"

LAutoUpdate::LAutoUpdate()
{
   m_hDrawSdk = NULL;
   m_hFileSdk = NULL;
   m_hLsUpdt32 = NULL; 
}

UINT LAutoUpdate::CheckForUpdates()
{
   UINT nReturn = 0;
   bool success = OpenLibraries();
   if (success)
   {
      FARPROC LecturnityUpdateCheck = ::GetProcAddress(
         m_hLsUpdt32, "LecturnityUpdateCheck");
      if (LecturnityUpdateCheck)
         nReturn = LecturnityUpdateCheck();
   }

   CloseLibraries();

   return nReturn;
}

UINT LAutoUpdate::Configure()
{
   UINT nReturn = 0;
   bool success = OpenLibraries();
   if (success)
   {
      FARPROC LecturnityUpdateConfigure = ::GetProcAddress(
         m_hLsUpdt32, "LecturnityUpdateConfigure");
      if (LecturnityUpdateConfigure)
         nReturn = LecturnityUpdateConfigure();
   }
   CloseLibraries();

   return nReturn;
}

bool LAutoUpdate::OpenLibraries()
{
   // First load the GDI+ library. It's needed
   // by the Draw SDK.
   bool success = OpenLibrary(_T("gdiplus.dll"), &m_hGdiPlus);
   // Then load the Draw SDK, it does not
   // have any dependencies more than GDI+.
   if (success)
      success = OpenLibrary(_T("drawsdk.dll"), &m_hDrawSdk);
   // Then load the File SDK
   if (success)
      success = OpenLibrary(_T("filesdk.dll"), &m_hFileSdk);
   // Then finally lsupdt32.dll
   if (success)
      success = OpenLibrary(_T("lsupdt32.dll"), &m_hLsUpdt32);

#ifdef _DEBUG
   if (!success)
   {
      ::MessageBox(::GetForegroundWindow(), 
         _T("Eine oder mehrere DLLs für LECTURNITY Auto-Update\nkonnten nicht geladen werden."),
         _T("[Debug]"), MB_OK | MB_ICONWARNING);
   }
#endif

   return success;
}

void LAutoUpdate::CloseLibraries()
{
   if (m_hLsUpdt32)
      ::FreeLibrary(m_hLsUpdt32);
   m_hLsUpdt32 = NULL;
   if (m_hFileSdk)
      ::FreeLibrary(m_hFileSdk);
   m_hFileSdk = NULL;
   if (m_hDrawSdk)
      ::FreeLibrary(m_hDrawSdk);
   m_hDrawSdk = NULL;
   if (m_hGdiPlus)
      ::FreeLibrary(m_hGdiPlus);
   m_hGdiPlus = NULL;
}

bool LAutoUpdate::OpenLibrary(IN _TCHAR *szLibName, OUT HINSTANCE *phInstance)
{
   LString lsLibName = szLibName;
   // Try current directory first.
   HINSTANCE hTemp = ::LoadLibrary((LPCTSTR) lsLibName);

   if (hTemp == NULL)
   {
      // Then check ..\\AutoUpdate directory
      lsLibName = _T("..\\AutoUpdate\\");
      lsLibName += szLibName;
      hTemp = ::LoadLibrary((LPCTSTR) lsLibName);
   }

   if (hTemp == NULL)
   {
      // What the fuck...?
      // Read out the Installation Dir of LECTURNITY
      // from the registry and try to load the desired
      // DLLs from there.
      LString lsInstallDir(MAX_PATH);
      DWORD dwSize = MAX_PATH * sizeof _TCHAR;
      bool success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
         _T("SOFTWARE\\imc AG\\LECTURNITY"), _T("InstallDir"), lsInstallDir.Access(), &dwSize);
      if (success)
      {
         lsLibName = lsInstallDir;
         lsLibName += _T("AutoUpdate\\");
         lsLibName += szLibName;
         hTemp = ::LoadLibrary((LPCTSTR) lsLibName);
      }
   }

   if (hTemp && phInstance)
      *phInstance = hTemp;

   return (hTemp != NULL);
}
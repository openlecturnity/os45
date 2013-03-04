#include "StdAfx.h"
#include "launchjava.h"
#include "lutils.h" // LRegistry functions
#include "KeyGenerator.h" // szLECTURNITY_REGISTRY

JavaLauncher::JavaLauncher(CString htmlFileName)
{
   htmlFileName_ = htmlFileName;
}

JavaLauncher::~JavaLauncher()
{
}

bool JavaLauncher::doLaunch()
{
   CRegKey registry;

   HINSTANCE instance = AfxGetInstanceHandle();
   char error[128];
   LoadString(instance, IDS_ERROR, error, 512);
   char errorMsg[512];

   char szInstallDir[MAX_PATH];
   DWORD dwSize = MAX_PATH;
   bool success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
      szLECTURNITY_REGISTRY, _T("InstallDir"), szInstallDir, &dwSize);

   if (success)
   {
      CString cmdLine = CString("\"");
      cmdLine += szInstallDir;
      cmdLine += "\\Player\\player.exe\" \"";
      cmdLine += htmlFileName_ + "\"";

      int lastSlash = htmlFileName_.ReverseFind('\\');
      if (lastSlash != -1)
      {
         cmdLine += " \"";
         CString htmlFilePath = htmlFileName_.Left(lastSlash + 1);
         cmdLine += htmlFilePath;
         cmdLine += "Tutorial.lpd\"";
      }

   // MessageBox(NULL, cmdLine, "Command", MB_OK);

      UINT execRes = WinExec(cmdLine, SW_SHOW);
      if (execRes > 31) // success
         return true;

      switch(execRes)
      {
      case 0:
         LoadString(instance, IDS_ERR_MEM, errorMsg, 512);
         break;

      default:
         LoadString(instance, IDS_ERR_NOTFOUND, errorMsg, 512);
         break;
      }
   }
   else
   {
      LoadString(instance, IDS_ERR_NOLECTURNITY, errorMsg, 512);
   }

   MessageBox(NULL, errorMsg, error, MB_OK|MB_ICONEXCLAMATION);
   return false;
}

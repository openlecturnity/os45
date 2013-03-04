#include "StdAfx.h"

#include "registry.h"
#include "queries.h"
#include "KeyGenerator.h"
#include "lregistry.h"

bool ReadJavaEntry(std::string &javaPath)
{
   // Which java version is the current version on this system?
   char  szJavaVersion[32];
   DWORD dwSize = 32;
   bool success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE, 
      _T("SOFTWARE\\JavaSoft\\Java Runtime Environment"), _T("CurrentVersion"), 
      szJavaVersion, &dwSize);

   // Is it a 1.3 version?
   if (success)
   {
      success = (strncmp(szJavaVersion, "1.3", 3) == 0);
   }

   char szJavaHome[MAX_PATH];
   dwSize = MAX_PATH;
   if (success)
   {
      // Yes, we have a 1.3 version of the JRE; now try to find out
      // where the installation is.
      success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE, 
         _T("Software\\JavaSoft\\Java Runtime Environment\\1.3"), _T("JavaHome"),
         szJavaHome, &dwSize);
   }

   // Success? If so, copy the java path into javaPath
   if (success)
      javaPath = szJavaHome;

   return success;
}

bool ReadPlayerEntry(std::string &playerPath)
{
   // Try to read out the default command for LPD files;
   // they have the ProdId imc.Lecturnity.Presentation.
   char szPlayerCommand[MAX_PATH * 2];
   DWORD dwSize = MAX_PATH * 2;
   bool success = LRegistry::ReadStringRegistryEntry(HKEY_CLASSES_ROOT,
      _T("imc.Lecturnity.Presentation\\Shell\\Open\\Command"), _T(""),
      szPlayerCommand, &dwSize);

   // If we were successful, the command should look something like
   // this:
   // "C:\Programme\imc AG\LECTURNITY\Player\player.exe" "%1"
   // Now we strip the quotation marks and the "%1".
   if (success)
   {
      playerPath = szPlayerCommand;
      std::string::size_type pos;

      // Find the space in front of the "%1"
      pos = playerPath.find_last_of(" ");
      // Replace everything behind that space with an empty string
      // Nothing happens if pos is npos.
      playerPath.replace(pos, std::string::npos, "");
      // Replace the quotes with empty strings
      pos = playerPath.find_first_of("\"");
      if (pos != std::string::npos)
         playerPath.replace(pos,pos+1,"");
      pos = playerPath.find_first_of("\"");
      if (pos != std::string::npos)
         playerPath.replace(pos,pos+1,"");
   }

   return success;
}

bool ComparePlayerVersion(std::string playerVersion)
{
   char szCurrentVersion[32];
   // Retrieve the current short version string,
   // In case an error occurs, the version string is "?.?.?"
   KerberokHandler::GetVersionStringShort(szCurrentVersion);

   // Only if the installed version matches the version on the CD,
   // take the installed Player version to open the document
   // If szCurrentVersion is invalid, CompareVersionStrings
   // will not return 0 for sure; we know that playerVersion
   // is valid.
   bool success = (KerberokHandler::CompareVersionStrings(szCurrentVersion, 
                                                          playerVersion.c_str()) == 0);

   char szInstallDir[MAX_PATH];
   DWORD dwSize = MAX_PATH;
   if (success)
   {
      // Read out the installation directory of LECTURNITY
      success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
         _T("SOFTWARE\\imc AG\\LECTURNITY"), _T("InstallDir"),
         szInstallDir, &dwSize);
   }

   std::string
      playerPath;

   if (success)
   {
      // Ok, we know where the Player should be.
      // Is it there?
      playerPath = szInstallDir;
      playerPath += "\\Player\\player.exe";

      success = FileExists(playerPath.c_str());
   }
      
   return success;
}

#include "StdAfx.h"
#include "queries.h"
#include "dialog.h"
#include "registry.h"

extern AUTORUN::DialogWindow
   *dialog;

bool FileExists(const char* FileName)
{
   struct stat my_stat;
   return (stat(FileName, &my_stat) == 0);
}

bool javaIsInstalled(std::string &javaCommand)
{
   std::string
      javaPath;

   if (ReadJavaEntry(javaPath))
   {
      javaCommand = javaPath;
      javaCommand += "\\javaw.exe";
      if (!FileExists(javaCommand.c_str()))
         return false;

      return true;
   }

   return false;
}

bool javaIsOnCD(const char *cdRoot, std::string &javaCommand)
{
   javaCommand = cdRoot;
   javaCommand += "\\bin\\jre\\bin\\javaw.exe";

   if (!FileExists(javaCommand.c_str()))
      return false;

   return true;
}

bool playerIsInstalled(IN const char *cdRoot, OUT std::string &playerCommand)
{
   std::string versionFile;
   std::string versionString;
   int         size;
   FILE       *fp;
   char        buf[500];

   versionFile = cdRoot;
   versionFile += "\\version.dat";
   fp = fopen(versionFile.c_str(),"rb");

   if (!fp)
   {
      //::MessageBox(NULL, "fopen failed.", "playerIsInstalled", MB_OK);
      return false;
   }

   // Version.dat simply contains the version
   // of LECTURNITY which created the current
   // version. This is also (probably) the version
   // of the Player on the CD. We use this version
   // to compare to the (perhaps) installed LECTURNITY
   // Player version.
   size = fread(buf,1,500,fp);
   buf[size] = '\0';
   versionString = buf;
   fclose(fp);

   // ComparePlayerVersion returns true if there is a
   // suitable LECTURNITY Player installed on this system,
   // if the installed Player has the same version as
   // was stored in Version.dat (versionString).
   if (ComparePlayerVersion(versionString))
   {
      // Read the current shell command for the LPD
      // document type. This is somewhat redundant.
      if (ReadPlayerEntry(playerCommand))
      {
         if (!FileExists(playerCommand.c_str()))
            return false;
         
         return true;
      }
   }

   return false;
}

int playerIsOnCD(const char *cdRoot, std::string &playerCommand)
{
   playerCommand = cdRoot;
   playerCommand += "\\bin\\player.jar";

   int tries = 0;
   if (!FileExists(playerCommand.c_str()))
   {
      //::MessageBox(NULL, "Player is not on CD.", "Fälä", MB_OK);
      return false;
   }

   //::MessageBox(NULL, "Player is on CD.", "Fälä", MB_OK);

   return true;
}

bool playerSetupIsAvailable(const char *cdRoot, std::string &setupName)
{
   setupName = cdRoot;
   setupName += "\\setup.exe";

   if (!FileExists(setupName.c_str()))
      return false;

   return true;
}

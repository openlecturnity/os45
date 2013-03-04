// tcpip.cpp : Definiert den Einsprungpunkt für die Konsolenanwendung.
//
#include <stdio.h> 
#include <string>
#include <direct.h>

#ifdef WIN32

#include <windows.h>
#include <io.h> 

#else

#define INVALID_SOCKET -1
#include <unistd.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int SOCKET;

#endif

#include "la_registry.h"

#define DEUTSCH 0
#define ENGLISCH 1

int language;

#define NO_CORRECT_JAVA 0
#define START_PROCESS   1

void PrintError(int errorNumber)
{
   if (language == DEUTSCH)
   {
      switch (errorNumber)
      {
      case NO_CORRECT_JAVA:
         MessageBox(NULL, "Programm kann nicht gestartet werden.\nJava wurde nicht korrekt ínstalliert.\n", "Fehler im Publisher Startprogramm", MB_OK); 
      case START_PROCESS:
         MessageBox(NULL, "Der Prozess kann nicht gestartet werden.", "Fehler im Publisher Startprogramm", MB_OK);
      }
   }
   else 
   {
      switch (errorNumber)
      {
      case NO_CORRECT_JAVA:
         MessageBox(NULL, "Can't start program.\nThere is no correct Java installation.", "Error in Publisher Controller", MB_OK); 
      case START_PROCESS:
         MessageBox(NULL, "Can't start process.", "Error in Publisher Controller", MB_OK); 
      }
   }
}

void PrintMessage(char *message)
{
#ifdef WIN32

   if (language == DEUTSCH)
      MessageBox(NULL,message,"Meldung vom Player Startprogramm",MB_OK);
   else
      MessageBox(NULL,message,"Message from Player Control",MB_OK);

#else

   fprintf(stderr,"Message from playerControl: %s\n",message);

#endif
}


char *FindProgram(char *programName, char *path)
{
    
   DWORD
      drives;
   HINSTANCE
      result;
   char
      j, buf[10],
      *command;
   unsigned int
      i,
      isDrive,
      driveType;

   drives = GetLogicalDrives();
 
   command = (char *)malloc(200);
   if (path) 
   {
      sprintf(command,"%s\\%s",path,programName);   
      if (_access(command,0) == 0)
         return command;
   }

   for (i = 0, j = 'a'; i<26; ++i, ++j) 
   {
      isDrive = (drives>>i) & (unsigned int)1;
      if (isDrive) 
      {
         sprintf(buf,"%c:",j); 
         driveType = GetDriveType(buf);
         if (driveType == DRIVE_FIXED) 
         {
            sprintf(buf,"%c:",j); 
            result = FindExecutable(programName,buf,command);
            if ((int)result <= 32) 
            {
               return NULL;
            }
         }
      }
   }

   return command;
}

void GetInstalledComponents(bool &hasCdPublisher,
                            bool &hasRealMedia,
                            bool &hasWindowsMedia,
                            bool &hasMp4Video)
{
   // Default: Nothing!
   hasCdPublisher  = false;
   hasRealMedia    = false;
   hasWindowsMedia = false;

   HKEY key;
   LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                              "SOFTWARE\\imc AG\\LECTURNITY",
                              0,
                              KEY_READ,
                              &key);
   if (result != ERROR_SUCCESS)
      return;

   /*
   char buffer[128];
   DWORD szBuffer = 128;
   DWORD type = REG_SZ;

   result = RegQueryValueEx(key, "CurrentVersion", NULL,
                            &type, (unsigned char *) buffer, &szBuffer);
   RegCloseKey(key);
   key = NULL;
   if (result != ERROR_SUCCESS)
      return;

   std::string keyString = "SOFTWARE\\imc AG\\LECTURNITY\\";
   keyString += buffer; // version

   result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                         keyString.c_str(),
                         0,
                         KEY_READ,
                         &key);
   if (result != ERROR_SUCCESS)
      return;
   */

   DWORD type = REG_DWORD;
   DWORD value = 0;
   DWORD szValue = sizeof(DWORD);

   result = RegQueryValueEx(key, "CdPublisher", NULL,
                            &type, (LPBYTE) &value, &szValue);
   if (result == ERROR_SUCCESS)
      hasCdPublisher = (value == 0x00000001);

   result = RegQueryValueEx(key, "RealMedia", NULL,
                            &type, (LPBYTE) &value, &szValue);
   if (result == ERROR_SUCCESS)
      hasRealMedia = (value == 0x00000001);

   result = RegQueryValueEx(key, "WindowsMedia", NULL,
                            &type, (LPBYTE) &value, &szValue);
   if (result == ERROR_SUCCESS)
      hasWindowsMedia = (value == 0x00000001);

   result = RegQueryValueEx(key, "Mp4Video", NULL,
                            &type, (LPBYTE) &value, &szValue);
   if (result == ERROR_SUCCESS)
      hasMp4Video = (value == 0x00000001);

   RegCloseKey(key);
   key = NULL;

#ifdef _DEBUG
   /*
   if (hasCdPublisher)
      MessageBox(NULL, "Has CD Publisher.", "Info", MB_OK);
   else
      MessageBox(NULL, "Does not have CD Publisher.", "Info", MB_OK);

   if (hasRealMedia)
      MessageBox(NULL, "Has Real Media.", "Info", MB_OK);
   else
      MessageBox(NULL, "Does not have Real Media.", "Info", MB_OK);

   if (hasWindowsMedia)
      MessageBox(NULL, "Has Windows Media.", "Info", MB_OK);
   else
      MessageBox(NULL, "Does not have Windows Media.", "Info", MB_OK);

   if (hasMp4Video)
      MessageBox(NULL, "Has Mp4 Export.", "Info", MB_OK);
   else
      MessageBox(NULL, "Does not have Mp4 Export.", "Info", MB_OK);

  */
#endif // _DEBUG


}

void StartConverter(char *programPath, int argc, char *argv[])
{ 
   STARTUPINFO
      startUpInfo;
   PROCESS_INFORMATION
      processInfo;
   std::string
      commandLine,
      command; 
   std::string
      lecturnityPath,
      jrePath;
   std::string::size_type
      pos;
   char 
     full[_MAX_PATH],
     *zw;

   GetStartupInfo(&startUpInfo);
 
   jrePath = programPath;
   jrePath += "\\..\\jre\\bin\\javaw.exe";
   if ( _fullpath( full, jrePath.c_str(), _MAX_PATH ) != NULL )
     jrePath = full;


   lecturnityPath = programPath;
   pos = lecturnityPath.find_last_of("\\");
   lecturnityPath.replace(pos,std::string::npos,"");

   command = jrePath;

   if (!(_access(command.c_str(),0) == 0))
   {
      zw = FindProgram("javaw.exe",NULL);
      if (zw == NULL)
      {
         PrintError(NO_CORRECT_JAVA); 
         return;
      }
      else 
      {
         command = zw;
         delete zw;
      }
   }

   // First, add the basic command (java.exe with path)
   commandLine = "\""+command+"\"";

   // Workaround for Bug #838
   commandLine += " -Xms20M";

   // How much memory do we have, and how much should
   // we "reserve" for Java?
   MEMORYSTATUS memory;

   GlobalMemoryStatus(&memory);

   int maxMemory = 64;
   int totalMemory = memory.dwTotalPhys/(1024*1024);

   // maximum memory usage should not exceed 512 MB
   if (totalMemory >= 576) // 512 + 64
      maxMemory = 512; 
   else if (totalMemory >= 383) 
      maxMemory = totalMemory-128;
   else if (totalMemory >= 159)
      maxMemory = totalMemory-64;
   else if (totalMemory >= 127)
      maxMemory = totalMemory-48;

   if (maxMemory > 64)
   {
      char buf[32];
      sprintf(buf, "%d", maxMemory);

      commandLine += " -Xmx";
      commandLine += buf;
      commandLine += "M";

      int maxPixelCount = ((maxMemory/20)*1000000);
      sprintf(buf, "%d", maxPixelCount);

      commandLine += " -Dpixel.limit=";
      commandLine += buf;
   }

   commandLine += " ";
   
   // Find out which components are installed:
   bool hasCdPublisher  = false;
   bool hasRealMedia    = false;
   bool hasWindowsMedia = false;
   bool hasMp4Video     = false;
   GetInstalledComponents(hasCdPublisher, hasRealMedia, hasWindowsMedia, hasMp4Video);

   // Build the command line:
   if (hasCdPublisher)
      commandLine += "-Dopt.cd=true ";
   if (hasRealMedia)
      commandLine += "-Dopt.real=true ";
   if (hasWindowsMedia)
      commandLine += "-Dopt.wmt=true ";
   if (hasMp4Video)
      commandLine += "-Dopt.mp4=true ";
   commandLine += "-Dinstall.path=\"";
   commandLine += lecturnityPath;
   commandLine += "\" -jar \"";
   commandLine += programPath;
   commandLine += "\\publisher.jar\" ";
   for (int i = 1; i< argc; ++i) 
   {
      commandLine += " \"";
      commandLine += argv[i];
      commandLine += "\"";
   }

   if (CreateProcess((char *)command.c_str(),(char *)commandLine.c_str(),NULL,NULL,false,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&startUpInfo,&processInfo) == 0) 
   { 
      PrintError(START_PROCESS);
      return;
   }

}

void PrintUsage()
{
  char 
    buf[500];

  sprintf(buf,"publisher.exe");
  PrintMessage(buf);

}

void PrintVersion()
{

  PrintMessage("Lecturnity Publisher Controller Version 1.4.1");

}

int main(int argc, char *argv[])
{
   std::string::size_type
      pos;
   std::string
      programPath,
      path;
   char 
      full[_MAX_PATH];

   ASSISTANT::regEntry 
      entry;
   ASSISTANT::RegistryEntry
      registry;

   registry.ReadSecurityEntry(&entry);
   if (strcmp(entry.language,"en") == 0)
      language = ENGLISCH;
   else
      language = DEUTSCH;

   path = argv[0];
   pos = path.find_last_of("\\");
   if (pos == std::string::npos) 
   {
      path = ".";
   }
   else 
   {   
      path.replace(pos,std::string::npos,"");
   }

   if ( _fullpath( full, path.c_str(), _MAX_PATH ) != NULL )
      programPath = full;
   else
      programPath = ".";

   _chdir(programPath.c_str());

   StartConverter((char *)programPath.c_str(),argc,argv);

   return 1;
}

#ifdef WIN32

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
		     LPSTR lpszCmdLine,int nCmdShow)
{
   main(__argc,__argv);

	return 1;
}

#endif

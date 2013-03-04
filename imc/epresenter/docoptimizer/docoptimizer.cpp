// tcpip.cpp : Definiert den Einsprungpunkt für die Konsolenanwendung.
//
#include <stdio.h> 
#include <string>
#include <direct.h>

#include <windows.h>
#include <io.h> 

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
         MessageBox(NULL, "Programm kann nicht gestartet werden.\nJava wurde nicht korrekt ínstalliert.\n", "Fehler im Document Optimizer", MB_OK); 
      case START_PROCESS:
         MessageBox(NULL, "Der Prozess kann nicht gestartet werden.", "Fehler im Document Optimizer", MB_OK);
      }
   }
   else 
   {
      switch (errorNumber)
      {
      case NO_CORRECT_JAVA:
         MessageBox(NULL, "Can't start program.\nThere is no correct Java installation.", "Error in Document Optimizer", MB_OK); 
      case START_PROCESS:
         MessageBox(NULL, "Can't start process.", "Error in Document Optimizer", MB_OK); 
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
   commandLine = "\"";
   commandLine += command;
   commandLine += "\" -jar \"";
   commandLine += programPath;
   commandLine += "\\docopt.jar\" ";
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

  sprintf(buf,"docopt.exe");
  PrintMessage(buf);

}

void PrintVersion()
{

  PrintMessage("Lecturnity Document Optimizer Version 1.4.0");

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

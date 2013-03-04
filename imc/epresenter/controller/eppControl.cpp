// tcpip.cpp : Definiert den Einsprungpunkt für die Konsolenanwendung.
//
#include <stdio.h> 
#include <string>
#include <direct.h>

#include <windows.h>
#include <io.h> 
#include <tchar.h>

#include "CRC_32.h"


#include "la_registry.h"

#define DEUTSCH 0
#define ENGLISCH 1

int language;

// Da wollte jemand keinen Header anlegen!
bool InformOrStartJavaPlayer(const char *documentName, const char *tutorialHtmlName, 
                  const char *programPath, const char *time, bool showHelp);

void PrintError(char *message)
{
   if (language == DEUTSCH)
      MessageBox(NULL,message,"Fehler im Player Startprogramm",MB_OK);
   else
      MessageBox(NULL,message,"Error in Player Control",MB_OK);
}


void PrintMessage(char *message)
{
   if (language == DEUTSCH)
      MessageBox(NULL,message,"Meldung vom Player Startprogramm",MB_OK);
   else
      MessageBox(NULL,message,"Message from Player Control",MB_OK);
}


void SendStart(char *docName)
{ 
   int      
      cliLen;
   int
      tcpSocket;
   struct sockaddr_in   
      cliAddr;
   std::string
      sendString;

   WSADATA 
      wsdata;
   WORD    
      wVersionRequested;  
   int 
      rc;

   wVersionRequested = MAKEWORD(1,1);     
   rc = WSAStartup(wVersionRequested, &wsdata);
  
   tcpSocket = socket(PF_INET, SOCK_STREAM, 0);
   if (tcpSocket == INVALID_SOCKET) 
   {
      if (language == DEUTSCH)
         PrintError("Erstellung einer Verbindung zum Player nicht möglich.");
      else
         PrintError("No connection to player.");
      return;
   }

   cliAddr.sin_family      = PF_INET;
   cliAddr.sin_port        = htons(21777);
   cliAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   cliLen                  = sizeof(cliAddr);

   if (connect(tcpSocket,(struct sockaddr *)&cliAddr,sizeof(cliAddr)) < 0) 
   {
      if (language == DEUTSCH)
         PrintError("Keine Verbindung zum Player möglich");
      else
         PrintError("No connection to player.");
      return;
   }
   else 
   {
      sendString = "start \"";
      sendString += docName;
      sendString += "\"\n";
      if (send(tcpSocket,sendString.c_str(),sendString.size(),0) < 0) 
      {
         if (language == DEUTSCH)
            PrintError("Keine Kommunikation mit dem Player möglich.");
         else
            PrintError("No communication with player.");
      } 
      close(tcpSocket);
   }

   return;
}

void SendStop(char *docName)
{ 
   int      
      cliLen;
   int
      tcpSocket;
   struct sockaddr_in   
      cliAddr;
   std::string
      sendString;

   WSADATA 
      wsdata;
   WORD    
      wVersionRequested;  
   int 
      rc;

   wVersionRequested = MAKEWORD(1,1);     
   rc = WSAStartup(wVersionRequested, &wsdata);
  
   tcpSocket = socket(PF_INET, SOCK_STREAM, 0);
   if (tcpSocket == INVALID_SOCKET) 
   {
      if (language == DEUTSCH)
         PrintError("Erstellung einer Verbindung zum Player nicht möglich.");
      else
         PrintError("No connection to player.");
      return; 
   }

   cliAddr.sin_family      = PF_INET;
   cliAddr.sin_port        = htons(21777);
   cliAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   cliLen                  = sizeof(cliAddr);

   if (connect(tcpSocket,(struct sockaddr *)&cliAddr,sizeof(cliAddr)) < 0) 
   {
      if (language == DEUTSCH)
         PrintError("Keine Verbindung zum Player möglich");
      else
         PrintError("No connection to player.");
      return;
   }
   else 
   {
      sendString = "stop \"";
      sendString += docName;
      sendString += "\"\n";
      if (send(tcpSocket,sendString.c_str(),sendString.size(),0) < 0) 
      {
         if (language == DEUTSCH)
            PrintError("Keine Kommunikation mit dem Player möglich.");
         else
            PrintError("No communication with player.");
      } 
      close(tcpSocket);
   }

   return;
}

void SendTime(char *time, char *docName, std::string programPath)
{ 
   int      
      cliLen;
   int
      tcpSocket;
   struct sockaddr_in   
      cliAddr;
   std::string
      sendString;

   WSADATA 
      wsdata;
   WORD    
      wVersionRequested;  
   int 
      rc;

   wVersionRequested = MAKEWORD(1,1);     
   rc = WSAStartup(wVersionRequested, &wsdata);
  
   tcpSocket = socket(PF_INET, SOCK_STREAM, 0);
   if (tcpSocket == INVALID_SOCKET) 
   {
      if (language == DEUTSCH)
         PrintError("Erstellung einer Verbindung zum Player nicht möglich.");
      else
         PrintError("No connection to player.");
      return; 
   }

   cliAddr.sin_family      = PF_INET;
   cliAddr.sin_port        = htons(21777);
   cliAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   cliLen                  = sizeof(cliAddr);

   if (connect(tcpSocket,(struct sockaddr *)&cliAddr,sizeof(cliAddr)) < 0) 
   {
      close(tcpSocket);

      if (!InformOrStartJavaPlayer(docName, NULL, programPath.c_str(), time, false)) 
      {
         if (language == DEUTSCH)
            PrintError("Player kann nicht gestartet werden");
         else
            PrintError("Can't start player.");
      }
      return;
   }
  

   sendString = "time \"";
   sendString += docName;
   sendString += "\" ";
   sendString += time;
   sendString += "\n"; 

   if (send(tcpSocket,sendString.c_str(),sendString.size(),0) < 0) 
   {
      if (language == DEUTSCH)
         PrintError("Keine Kommunikation mit dem Player möglich.");
      else
         PrintError("No communication with player.");
   } 

   close(tcpSocket);

   return;
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

bool sendStringToSocket(std::string &sendString)
{
  
   WSADATA wsdata; 
   WORD wVersionRequested = MAKEWORD(1,1);     
   int rc = WSAStartup(wVersionRequested, &wsdata);
  
   SOCKET tcpSocket = socket(PF_INET, SOCK_STREAM, 0);
   if (tcpSocket == INVALID_SOCKET) 
   {
      fprintf (stderr, "Erstellung einer Verbindung zum Player nicht möglich"); 
      return false; 
   }

   struct sockaddr_in cliAddr;
   cliAddr.sin_family      = PF_INET;
   cliAddr.sin_port        = htons(21777);
   cliAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   int  cliLen             = sizeof(cliAddr);

   if (connect(tcpSocket,(struct sockaddr *)&cliAddr,sizeof(cliAddr)) < 0) 
   {
      close(tcpSocket);
      return false;
   }

   if (send(tcpSocket,sendString.c_str(),sendString.size(),0) < 0) 
   {
      if (language == DEUTSCH)
         PrintError("Keine Kommunikation mit dem Player möglich.");
      else
         PrintError("No communication with player.");

      close(tcpSocket);

      return false;
   }

   close(tcpSocket);
   
   return true;
}

bool InformOrStartJavaPlayer(const char *documentName, const char *tutorialHtmlName, 
                  const char *programPath, const char *time, bool showHelp)
{
   PROCESS_INFORMATION
      processInfo;
   char 
      *zw,
      full[_MAX_PATH],
      *environment = NULL;
   std::string
      commandLine,
      command,
      jrePath; 


   bool isTutorial = tutorialHtmlName != NULL;

   //
   // try to communicate with already existant player instance
   //
   if (!isTutorial)
   {
      std::string sendString = "nop\n";
      if (documentName != NULL)
      {
         sendString = "load \"";
         sendString += documentName;
         sendString += "\" 0\n";
      }
      else if (showHelp)
         sendString = "help\n";
      else
         sendString = "nop\n";
      bool communicated = sendStringToSocket(sendString);
      if (communicated)
         return true;
   }
   
   //
   // needed for CreateProcess()
   //
   STARTUPINFO startUpInfo;
   GetStartupInfo(&startUpInfo);
 
   //
   // try to find installed java relative to player.exe (normal case)
   //
   jrePath = programPath;
   jrePath += "\\..\\jre\\bin\\javaw.exe";
   if ( _fullpath( full, jrePath.c_str(), _MAX_PATH ) != NULL )
      jrePath = full;
 
   command = jrePath;

   //
   // try to find other java
   //
   if (!(_access(command.c_str(),0) == 0))
   {
      zw = FindProgram("javaw.exe",NULL);
      if (zw == NULL)
      {
         if (language == DEUTSCH)
            PrintError("Programm kann nicht gestartet werden.\nJava wurde nicht korrekt installiert."); 
         else
            PrintError("Can't start program.\nThere is no correct Java installation.\n"); 
         return false;
      }
      else 
      {
         command = zw;
         delete zw;
      }
      environment = getenv("CLASSPATH");
   }

   commandLine = "\""+command+"\"";


   // Workaround for Bug #838
   // audio replay seems to be influenced by (major) garbage collections
   // on certain computers, therefore increase initial heap size
   commandLine += " -Xms20M";

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

   commandLine += " -cp \"";
   commandLine += programPath;
   commandLine += "\\player.jar;";
   if (environment != NULL)
	   commandLine += environment;
   commandLine += "\" -Dhelp.path=\"";
   commandLine += programPath;
   commandLine += "\\player.pdf\"";
   if (documentName == NULL) 
   {
      commandLine += " imc.epresenter.player.Manager";
      if (showHelp)
         commandLine += " -help";
	}
   else 
   {
      commandLine += " imc.epresenter.player.Manager";
      if (time != NULL) 
      {
         commandLine += " -time ";
         commandLine += time;
      }
      if (tutorialHtmlName != NULL)
      {
         commandLine += " \""; 
         commandLine += tutorialHtmlName;
         commandLine += "\"";
      }
      commandLine += " \""; 
      commandLine += documentName;
      commandLine += "\"";
   }


   //::MessageBox(::GetForegroundWindow(), (char *)commandLine.c_str(), _T("Aufruf:"), MB_OK);

   if (CreateProcess((char *)command.c_str(),(char *)commandLine.c_str(),NULL,NULL,false,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&startUpInfo,&processInfo) == 0) 
   { 
      if (language == DEUTSCH)
         PrintError("Der Prozess kann nicht gestartet werden.");
      else
         PrintError("Can't start process."); 
      return false;
   }

   return true;
}


void PrintUsage()
{
   char 
      buf[500];

   sprintf(buf,"player.exe [-help] / [-showHelp] / [-time docName timeValue] / [-start docName] / [-stop docName] / [-show] / [-version] / [Presentationname]");
   PrintError(buf);

}

void PrintVersion()
{

  PrintMessage("Lecturnity Player Controller Version 1.5.0");

}


//
// Extracts the program name from the GetCommandLine()
// output string.
// Copied from StartPlayerFromCD (double code)
//
void GetProgramName(OUT char *szProgramName)
{
   LPCSTR szCmdLine = GetCommandLine();
  
   char szTemp[MAX_PATH];

   // Let's first extract the file name with
   // the complete path into szTemp.
   bool hasQuotes = (szCmdLine[0] == '"');
   if (hasQuotes)
   {
      char *szNextQuote = strchr(szCmdLine + 1, '"');
      int nLen = szNextQuote - szCmdLine - 1;
      strncpy(szTemp, szCmdLine + 1, nLen);
      szTemp[nLen] = '\0';
   }
   else
   {
      char *szNextSpace = strchr(szCmdLine, ' ');
      if (szNextSpace)
      {
         int nLen = szNextSpace - szCmdLine;
         strncpy(szTemp, szCmdLine, nLen);
         szTemp[nLen] = '\0';
      }
      else
      {
         // No quotes, no parameters
         strcpy(szTemp, szCmdLine);
      }
   }

   char *szLastSlash = strrchr(szTemp, '\\');
   if (szLastSlash)
   {
      // There is a backslash in the name, 
      // take everything after that.
      strcpy(szProgramName, szLastSlash + 1);
   }
   else
   {
      // No slashes in the path, simply copy szTemp
      // to szProgramName
      strcpy(szProgramName, szTemp);
   }
}

     
#ifdef _DEBUG 
void fillRandomly(char* buffer, int len, int base)
{
   for (int i=0; i<len; ++i)
      buffer[i] = (rand() >> 3) % base;
}

int CreateLecturnityDat()
{
   _TCHAR szCurrentDir[MAX_PATH];
   ::GetCurrentDirectory(MAX_PATH, szCurrentDir);

   _tcscat(szCurrentDir, _T("\\*.lpd"));
   WIN32_FIND_DATA sFindData;
   ZeroMemory(&sFindData, sizeof WIN32_FIND_DATA);
   HANDLE hSearch = ::FindFirstFile(szCurrentDir, &sFindData);

   if (hSearch != INVALID_HANDLE_VALUE)
   {
      const int KEY_LENGTH = 512;
      const int CALC_LENGTH = 256;
      
      srand(GetTickCount());
      char aKey[512];
      fillRandomly(aKey, 512, 256);

   
      aKey[0] = 'd';
      aKey[1] = 'e'; // this should be the language code

      int iKeyPointer = 0;

      HANDLE hDatFile = ::CreateFile(_T("lecturnity.dat"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
         FILE_ATTRIBUTE_NORMAL, NULL);
   
      DWORD dwWrittenBytes = 0;
      if (hDatFile != INVALID_HANDLE_VALUE)
         ::WriteFile(hDatFile, aKey, KEY_LENGTH, &dwWrittenBytes, NULL);

      bool bFileFound = hDatFile != INVALID_HANDLE_VALUE;

      while (bFileFound)
      {
         HANDLE hLpdFile = ::CreateFile(sFindData.cFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
         
         if (hLpdFile != INVALID_HANDLE_VALUE)
         {
            BYTE aFileStart[CALC_LENGTH];
            ::SetFilePointer(hLpdFile, 4, 0, FILE_BEGIN);
            
            DWORD dwReadBytes = 0;
            ::ReadFile(hLpdFile, aFileStart, CALC_LENGTH, &dwReadBytes, NULL);
            
            CRC_32 crc32;

            // doesn't work in Release build (FindNextFile() fails):
            DWORD crc = crc32.CalcCRC(aFileStart, CALC_LENGTH);
            char hexCrc[8];
            memset(hexCrc, 0, 8);
            ultoa(crc, hexCrc, 16);
            
            for (int i=0; i<8; ++i)
               hexCrc[i] ^= aKey[iKeyPointer++ % KEY_LENGTH];
            
            ::WriteFile(hDatFile, hexCrc, 8, &dwWrittenBytes, NULL);
            
            
            char szName[MAX_PATH];
            ZeroMemory(szName, MAX_PATH);
            strcpy(szName, sFindData.cFileName);
            int iLength = strlen(szName);
            for (i=0; i<iLength+1; ++i)
               szName[i] ^= aKey[iKeyPointer++ % KEY_LENGTH];
            
            ::WriteFile(hDatFile, szName, iLength+1, &dwWrittenBytes, NULL);

                     
            ::CloseHandle(hLpdFile);
         }
         

         bFileFound = (::FindNextFile(hSearch, &sFindData) != FALSE);

      }

      if (hDatFile != INVALID_HANDLE_VALUE)
         ::CloseHandle(hDatFile);
   
   
      ::FindClose(hSearch);
   }

   return 1;
}
#endif // _DEBUG

int main(int argc, char *argv[])
{

   char szProgramName[MAX_PATH];
   GetProgramName(szProgramName);

#ifdef _DEBUG
   if (stricmp(szProgramName, "datgen.exe") == 0)
   {
      // "undocumented" feature: create lecturnity.dat for all *.lpd in local directory

      return CreateLecturnityDat();
   }
   else
#endif // _DEBUG
   {
      // normal case: invoked as "player.exe"
      
      std::string
         filename,
         htmlname,
         suffix,
         programPath;
      std::string::size_type
         pos;
      std::string
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
      
      //
      // determine program path
      //
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
      
      //
      // change to directory with player.exe
      //
      _chdir(programPath.c_str());
      
      
      //
      // start player (java) or communicate with an existant instance
      // 
      if (argc < 2 || strcmp("-show", argv[1]) == 0) 
      {
         InformOrStartJavaPlayer(NULL, NULL, programPath.c_str(), NULL, false);
      }
      else 
      {
         if (strncmp("-start",argv[1],6) == 0)
         {
            if (argc < 3) 
            {
               PrintUsage();
               return -1;
            }
            SendStart(argv[2]);
         }
         else if (strncmp("-stop",argv[1],5) == 0) 
         {
            if (argc < 3) 
            {
               PrintUsage();
               return -1;
            }
            SendStop(argv[2]);
         }
         else if (strncmp("-time",argv[1],5) == 0) 
         {
            if (argc < 4) 
            {
               PrintUsage();
               return 1;
            }
            SendTime(argv[3] ,argv[2],programPath);
         }
         else if (strncmp("-showHelp",argv[1],9) == 0) 
         {
            InformOrStartJavaPlayer(NULL, NULL, programPath.c_str(), NULL, true); 
         }
         else if (strncmp("-help",argv[1],5) == 0) 
         {
            PrintUsage();
         }
         else if (strncmp("-version",argv[1],8) == 0) 
         {
            PrintVersion();     
         }
         else // argc >= 2 and nothing special
         {
            if (argc >= 3) // two or more parameters
            {
               htmlname = argv[1];
               filename = argv[2];
               
               InformOrStartJavaPlayer(filename.c_str(), htmlname.c_str(), programPath.c_str(), NULL, false);
            }
            else // only one parameter
            {  
               filename = argv[1];

               InformOrStartJavaPlayer(filename.c_str(), NULL, programPath.c_str(), NULL, false);
            }
         } 
      }
      return 1;

   }
}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
		     LPSTR lpszCmdLine,int nCmdShow)
{
  main(__argc,__argv);

	return 1;
}

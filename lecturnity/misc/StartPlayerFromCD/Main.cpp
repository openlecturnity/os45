#include "StdAfx.h"

#include "errormessages.h"
#include "resource.h"
#include "registry.h"
#include "dialog.h"
#include "queries.h"

AUTORUN::DialogWindow
   *dialog;


#define COPY_SUCCESS    0
#define SRC_NOT_EXISTS  1
#define DICS_FULL       2
#define NOT_COPIED      3
#define NOT_OVERWRITTEN 4
#define DISC_FULL       5

typedef struct {
   HANDLE processHandle;
   HWND   hwnd;
   char  *cdRoot;
   AUTORUN::DialogWindow *dialog;
} ThreadParameters;
  

typedef struct {
   std::string javaCommand;
   std::string cdRoot;
   HWND   hwnd;
   bool isInstalledJava;
   char docNames[2048];
   char szCallingName[MAX_PATH];
} PlayerThreadParameters;
  

static DWORD 
   playerThreadID = 0,
   setupThreadID = 0,
   waitThreadID = 0,
   runThreadID = 0;

// This variable is declared extern and global
// in StdAfx.h. Naughty. But neccessary in order
// to have the HINSTANCE pointer global in such
// a project (non MFC).
HINSTANCE g_hInstance = NULL;


// A collection of high-level security functions
//KerberokHandler g_kerberokHandler;
// A collection of low-level security functions
//KerberokBackend::KerberokLowLevel g_kerberokLowLevel;

void StartInstalledPlayer(const char *cdRoot, const char *command);


#ifdef _DEBUG
#define _DEBUGFILE
#endif

   void DebugMsg(char *szFormat)
   {
#ifdef _DEBUGFILE
      /*
      static FILE *debugFile = NULL;

      if (!debugFile)
         debugFile = fopen("C:\\autorun_debug.txt", "wa");
      //fprintf(debugFile, "Thread 0x%08x: ", GetCurrentThreadId());
      fprintf(debugFile, szFormat);
      fflush(debugFile);
      */
#endif
   }


/************************************/
/********** MISCELLANEOUS  **********/
/************************************/

// Returns the free disk space on the device to which path points to.
// path should point to a file. Otherwise, this will not work.
unsigned __int64 GetFreeDiskSpace(const char *path)
{
   // Here we rely on the fact that "\\<filename>" is added to the path
   // before the call of GetFreeDiskSpace
   const char *pLastSlash = strrchr(path, '\\');
   char szPath[MAX_PATH];
   strncpy(szPath, path, (int) (pLastSlash - path)); // Mmmm, pointer arithmetics
   // Null-terminate it
   szPath[(int) (pLastSlash - path)] = '\0';

   ULARGE_INTEGER freeBytesToCaller;
   ULARGE_INTEGER totalFree;
   BOOL success = ::GetDiskFreeSpaceEx(szPath, &freeBytesToCaller, &totalFree, NULL);
 
   return freeBytesToCaller.QuadPart;

   // The old implementation:
   //DWORD
   //   sectors,bytes,freeclusters,totalclusters;
   //GetDiskFreeSpace("c:\\",&sectors, &bytes, &freeclusters, &totalclusters);
   //double result = (double)(bytes * sectors * freeclusters)/1000;
   //return (int)result;
}

// Extracts the program name from the GetCommandLine()
// output string.
void GetProgramName(OUT char *szProgramName)
{
   LPCSTR szCmdLine = GetCommandLine();
   //char *szCmdLine = "d:\\out.dat";
   //char *szCmdLine = "\\out.exe";
   //char *szCmdLine = "d:\\jueche\\holla\\oi.exe";
   //char *szCmdLine = "leer.str";
   //char *szCmdLine = "leer.str dgh sfg sdg ההה \\";
   //char *szCmdLine = "\"C:\\Program Files\\Strange Directory\\Cool Program.exe\" test";
   
   char szTemp[MAX_PATH];

   // Let's first extract the file name with
   // the complete path into szTemp.
   bool hasQuotes = (szCmdLine[0] == '"');
   if (hasQuotes)
   {
      const char *szNextQuote = strchr(szCmdLine + 1, '"');
      int nLen = szNextQuote - szCmdLine - 1;
      strncpy(szTemp, szCmdLine + 1, nLen);
      szTemp[nLen] = '\0';
   }
   else
   {
      const char *szNextSpace = strchr(szCmdLine, ' ');
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


int CopyJarFile(HWND hwnd, const char *src, const char *dest)
{

   struct _stat
      srcStatBuffer,
      destStatBuffer;

   //MessageBox(hwnd,"COPY",NULL,MB_OK);
   if (!FileExists(src))
      return SRC_NOT_EXISTS;

   if (!FileExists(dest)) 
   {
      //MessageBox(hwnd,"IF",NULL,MB_OK);
      if (_stat(src, &srcStatBuffer ) == 0) 
      {
         if (srcStatBuffer.st_size > GetFreeDiskSpace(dest))
         {
            return DISC_FULL;
         }
         else 
         {
            if (!CopyFile(src,dest,true))
            {
               return NOT_COPIED;
            }  
            SetFileAttributes(dest,FILE_ATTRIBUTE_NORMAL);
         }
      }
   }
   else {
      //MessageBox(hwnd,"ELSE",NULL,MB_OK);
      if (_stat(dest, &destStatBuffer ) == 0)
      {
         if (_stat(src, &srcStatBuffer ) == 0)
         {
            //char buf[100];
            //sprintf(buf,"src: %d, dest: %d",srcStatBuffer.st_mtime,destStatBuffer.st_mtime);
            //MessageBox(hwnd,buf,NULL,MB_OK);
            if (srcStatBuffer.st_mtime != destStatBuffer.st_mtime)
            {
               if (!CopyFile(src,dest,false))
               {
                  return NOT_OVERWRITTEN;
               }
               SetFileAttributes(dest,FILE_ATTRIBUTE_NORMAL);
            }
         }
      }
   }

   return COPY_SUCCESS;
}



/******************************/
/********** THREADS  **********/
/******************************/

LONG SetupRunThread( LPVOID lpThreadData)
{ 
   DWORD 
      exitCode;
   ThreadParameters
      *threadParam;
   std::string
      playerCommand;


   threadParam = (ThreadParameters *)lpThreadData;

   threadParam->dialog->Back();

   while (GetExitCodeProcess(threadParam->processHandle, &exitCode) != 0)
   {
      if (exitCode != STILL_ACTIVE)
      {
         break;
      }
      Sleep(100);
   }
   
   threadParam->dialog->Top();

   if (playerIsInstalled(threadParam->cdRoot, playerCommand))
   {
      StartInstalledPlayer(threadParam->cdRoot, playerCommand.c_str());
   }
   else 
   {
      MESSAGES::ErrorMessages::ShowMessage(threadParam->dialog->GetWindow(), IDS_ERR_RUNPLAYER, MB_ICONERROR);
      SendMessage(threadParam->dialog->GetWindow(),WM_DESTROY,0,0);
   }

   return 0;
}

/*
LONG JavaRunThread( LPVOID lpThreadData)
{ 
   DWORD 
      exitCode;
   ThreadParameters
      *threadParam;


   threadParam = (ThreadParameters *)lpThreadData;
  
   while (GetExitCodeProcess(threadParam->processHandle, &exitCode) != 0)
   {
      if (exitCode != STILL_ACTIVE)
      {
         SendMessage(threadParam->hwnd,WM_DESTROY,0,0);
      }
       Sleep(100);
   }


   SendMessage(threadParam->hwnd,WM_DESTROY,0,0);

   return 0;
}
*/

LONG WaitForThread( LPVOID lpThreadData)
{ 
   int    cliLen;
   int    tcpSocket;
   sockaddr_in cliAddr;
   WSADATA wsdata;
   WORD    wVersionRequested;  
   int     rc;
   HWND    hwnd;
   ThreadParameters *threadParam;
   DWORD   exitCode;
   
   threadParam = (ThreadParameters *)lpThreadData;
   //hwnd = (HWND)lpThreadData;
   hwnd = threadParam->hwnd;
   
   wVersionRequested = MAKEWORD(1,1);     
   rc = WSAStartup(wVersionRequested, &wsdata);
   
   tcpSocket = socket(PF_INET, SOCK_STREAM, 0);
   if (tcpSocket == INVALID_SOCKET) {
      MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_NOSTATUS, MB_ICONWARNING);
      return -1; 
   }
   
   cliAddr.sin_family      = PF_INET;
   cliAddr.sin_port        = htons(21777);
   cliAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   cliLen                  = sizeof(cliAddr);
   
   bool connectSuccessful = false;
   bool javaHasExited     = false;

   DebugMsg("Trying to connect to the player...\n");

   // Try to connect to the player repeatedly
   while (!connectSuccessful && !javaHasExited)
   {
      Sleep(100);
      
      connectSuccessful = (connect(tcpSocket,(struct sockaddr *)&cliAddr,sizeof(cliAddr)) >= 0);

      if (!connectSuccessful)
      {
         // We did not manage to connect to the player;
         // Is the Java Thread still running?
         if (GetExitCodeProcess(threadParam->processHandle, &exitCode) != 0)
         {
            
            if (exitCode != STILL_ACTIVE)
            {
               char buff[512];
               sprintf(buff, _T("Process not active anymore. Code: %d\n"), exitCode);
               
               DebugMsg(buff);

               javaHasExited = true;
            } 
            DebugMsg(".");
         }
         else
         {
            DebugMsg("GetExitCodeProcess failed.\n");
            // GetExitCodeProcess failed, we assume java has exited
            javaHasExited = true;
         }
      }
      else
         DebugMsg("Connection successful.\n");

   }

   if (javaHasExited && !connectSuccessful)
   {
      // Error message: Java has exited, the Player was
      // not loaded successfully.
      MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_CDPROCESS, MB_ICONERROR);
   }
   
   SendMessage(hwnd, WM_DESTROY, 0, 0);
   
   return 0;
}

LONG CallPlayerThread ( LPVOID lpThreadData )
{
   DebugMsg("Entering routine CallPlayerThread...\n");

   STARTUPINFO
      startUpInfo;
   std::string
      javaPath,
      jmfPath,
      programPath,
      playerPath,
      cdRoot;
   std::string
      command,
      commandLine,
      documentNames;
   bool
      isTemp,
      isKickstartCopied,
      isPlayerCopied;
   HWND
      hwnd;
   PROCESS_INFORMATION
      processInfo;
   HANDLE
      javaProcess;
   HANDLE 
      waitThread;
//      runThread; 
   MSG
      msg;
   static ThreadParameters
      threadParam;
   PlayerThreadParameters
      *playerParams;

   playerParams = (PlayerThreadParameters *)lpThreadData;

   hwnd = playerParams->hwnd;
   cdRoot = playerParams->cdRoot;

   GetStartupInfo(&startUpInfo);
   
   programPath = cdRoot;
   programPath += "\\bin";

   _chdir(programPath.c_str());

   playerPath = programPath;

   jmfPath = programPath;
   jmfPath += "\\jre\\lib\\ext\\";

   char t[512];
   LoadString(g_hInstance, IDS_SEARCHFORJAVA, t, 512);
   SendMessage(hwnd,WM_COMMAND,0,(DWORD)(LPCSTR)t);

   command = playerParams->javaCommand;

   if (!playerParams->isInstalledJava)
   {
      std::string 
         tmpDir,
         kickstartJarSrc,
         kickstartJarDest,
         playerJarSrc,
         playerJarDest;
      char
         *envBuf=NULL;
      DWORD
         nSize;
      int
         result;

      isTemp = true;

      nSize = GetEnvironmentVariable("TMP",envBuf,0);
      if (nSize > 0)
      {
         envBuf = (char *)malloc(nSize+1);
         GetEnvironmentVariable("TMP",envBuf,nSize);
         tmpDir = envBuf;
         delete (envBuf);
      }
      else
      {
         nSize = GetEnvironmentVariable("TEMP",envBuf,0);
         if (nSize > 0)
         {
            envBuf = (char *)malloc(nSize+1);
            GetEnvironmentVariable("TEMP",envBuf,nSize); 
            tmpDir = envBuf;
            delete (envBuf);
         }
         else
         {
            MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_TEMPDIR, MB_ICONINFORMATION);
            isTemp = false;
         }
      }



      if (isTemp) 
      {
         kickstartJarSrc = programPath;
         kickstartJarSrc += "\\kickstart.jar";
         kickstartJarDest = tmpDir;
         kickstartJarDest += "\\kickstart.jar";

         char t[512];
         ::LoadString(g_hInstance, IDS_INITQUICKSTART, t, 512);
         SendMessage(hwnd,WM_COMMAND,0,(DWORD)(LPSTR) t);

         result = CopyJarFile(hwnd, kickstartJarSrc.c_str(),kickstartJarDest.c_str());
         if (result != COPY_SUCCESS)
         {
            switch (result)
            {
            case SRC_NOT_EXISTS:
               //MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_NOEXISTS);
               isKickstartCopied = false;
               break;
            case DISC_FULL:
               MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_FULLDISK, MB_ICONINFORMATION);
               isKickstartCopied = false;
               break;
            case NOT_COPIED:
               MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_NOCOPY, MB_ICONINFORMATION);
               isKickstartCopied = false;
               break;
            case NOT_OVERWRITTEN:
               MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_OVERWRITE, MB_ICONINFORMATION);
               isKickstartCopied = false;
               break;
            }
         }


         isPlayerCopied = true;
         playerJarSrc = programPath;
         playerJarSrc += "\\player.jar";
         playerJarDest = tmpDir;
         playerJarDest += "\\player.jar";

         result = CopyJarFile(hwnd, playerJarSrc.c_str(), playerJarDest.c_str());  if (result != COPY_SUCCESS)
         {
            switch (result)
            {
            case SRC_NOT_EXISTS:
               //MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_NOEXISTS);
               SendMessage(hwnd,WM_DESTROY,0,0);
               isPlayerCopied = false;
               break;
            case DISC_FULL:
               MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_FULLDISK, MB_ICONINFORMATION);
               isPlayerCopied = false;
               break;
            case NOT_COPIED:
               MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_NOCOPY, MB_ICONINFORMATION);
               isPlayerCopied = false;
               break;
            case NOT_OVERWRITTEN:
               MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_OVERWRITE, MB_ICONINFORMATION);
               isPlayerCopied = false;
               break;
            }
         }


      }

      command = playerParams->javaCommand;
      commandLine = "\"";
      commandLine += command;
      commandLine += "\" ";

      if (isKickstartCopied)
      {
         commandLine += "-Xbootclasspath/p:\"";
         commandLine += tmpDir;
         commandLine += "\\kickstart.jar\"";
      }
      if (isPlayerCopied)
         playerPath = tmpDir;
   }
   else 
   {
      command = playerParams->javaCommand;
      commandLine = "\"";
      commandLine += command;
      commandLine += "\" ";
   }


   //
   // AUTORUN/STARTPLAYER specifics
   //
   // Was the program called as autorun.exe?
   if (stricmp("autorun.exe", playerParams->szCallingName) == 0)
   {
      // Yes, then we're being called from a CD which
      // was created with the LECTURNITY CD Publisher.
      // Add an HTML file name.
      documentNames = cdRoot;
      documentNames += "\\docs\\start.html";
      if (!FileExists(documentNames.c_str())) 
      {
         documentNames = cdRoot;
         documentNames += "\\docs\\index.html";
         if (!FileExists(documentNames.c_str())) 
         {
            MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_NOHTML, MB_ICONWARNING);
            SendMessage(hwnd,WM_DESTROY,0,0);
         }
      }

      documentNames = "\"" + documentNames + "\"";
   }
   else // Assume startplayer.exe
   {
      // In this case we'll add the content of
      // the playerParams->docNames to the command line.
      // These are document file names which come directly
      // from the lpszCmdLine in WinMain
      if (strlen(playerParams->docNames) > 0)
         documentNames = playerParams->docNames;
      else
         documentNames = "";
   }

   commandLine += " -Xms20M";

   MEMORYSTATUS memory;
   GlobalMemoryStatus(&memory);

   int maxMemory = 64;
   int totalMemory = memory.dwTotalPhys/(1024*1024);
  
   if (totalMemory >= 1024)
      maxMemory = 976;
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
   commandLine += jmfPath;
   commandLine += "jmf.jar;";
   commandLine += playerPath;
   commandLine += "\\player.jar\"";
   commandLine += " -Dhelp.path=\"";
   commandLine += programPath;
   commandLine += "\\player.pdf\"";
   commandLine += " imc.epresenter.player.Manager ";
   commandLine += documentNames;

   //char t[512];
   ::LoadString(g_hInstance, IDS_STARTJAVA, t, 512);
   SendMessage(hwnd,WM_COMMAND,0,(DWORD)(LPSTR) t);



   DebugMsg("Creating Process with the following command and command line...\n");
   char buff2[2048];
   sprintf(buff2, "%s: %s\n", (char *)command.c_str(), (char *)commandLine.c_str());
   DebugMsg(buff2);

   if (CreateProcess((char *)command.c_str(),(char *)commandLine.c_str(),NULL,NULL,false,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&startUpInfo,&processInfo) == 0) 
   {   
      MESSAGES::ErrorMessages::ShowMessage(hwnd, IDS_ERR_CDPROCESS, MB_ICONERROR);
      SendMessage(hwnd,WM_DESTROY,0,0);
   }

   javaProcess = OpenProcess(PROCESS_TERMINATE, TRUE, processInfo.dwProcessId);
   if (!javaProcess)
   {
      char buff[512];
      _stprintf(buff, "Could not open process: %0xd\n", ::GetLastError());
      //::MessageBox(hwnd, buff, _T("Caption"), MB_OK);

      DebugMsg(buff);

   }
     
   DWORD exitCode = 17;
   if (GetExitCodeProcess(processInfo.hProcess, &exitCode) != 0)
   {
      //::MessageBox(hwnd, buff, _T("Caption"), MB_OK);
   }
   else
      DebugMsg("Could not get exit code.\n");

   char buff3[512];
   sprintf(buff3, "Current exit code %d.\n", exitCode);
 
   DebugMsg(buff3);


   threadParam.hwnd = hwnd;
   threadParam.processHandle = processInfo.hProcess;


   DebugMsg("Starting wait thread...\n");
   
   /* Wait until player response or exit */
   waitThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)WaitForThread,
			  &threadParam, CREATE_SUSPENDED, &waitThreadID); 

   if(!waitThread)  {
      return 0;
   } 

   SetThreadPriority( waitThread, THREAD_PRIORITY_BELOW_NORMAL );
   ResumeThread( waitThread );

   /* Wait until java is exited */
   /* now done in "waitThread"
   threadParam.hwnd = hwnd;
   threadParam.processHandle = processInfo.hProcess;
   runThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)JavaRunThread,
			  &threadParam, CREATE_SUSPENDED, &runThreadID); 

   if(!runThread)  {
      return 0;
   } 

   SetThreadPriority( runThread, THREAD_PRIORITY_BELOW_NORMAL );
   ResumeThread( runThread );
   */

   // If we receive a message, it has to be the message from
   // the "Cancel" button that the user wants to cancel the
   // start process of the LECTURNITY Player. In that case,
   // terminate the java process, and then destroy the main window.
   // Destroying the main window means that the main window
   // loop (in WinMain()) is exited, and this makes the entire
   // program exit.
   while (GetMessage(&msg, NULL, 0x00, 0x00))
   {
      TerminateProcess(javaProcess,0 );
      SendMessage(hwnd, WM_DESTROY, 0, 0);
   }

   return 1;
}



/*************************************/
/********** STARTPROGRAMME  **********/
/*************************************/

void StartInstalledPlayer(const char *cdRoot, const char *command)
{
   DebugMsg("Starting installed player...\n");

   STARTUPINFO
      startUpInfo;
   std::string
      htmlName,
      commandLine;
   PROCESS_INFORMATION
      processInfo;

   GetStartupInfo(&startUpInfo);

   htmlName = cdRoot;
   htmlName += "\\docs\\start.html";
   if (!FileExists(htmlName.c_str())) 
   {
      htmlName = cdRoot;
      htmlName += "\\docs\\index.html";
      if (!FileExists(htmlName.c_str())) 
      {
         MESSAGES::ErrorMessages::ShowMessage(dialog->GetWindow(), IDS_ERR_NOHTML, MB_ICONWARNING);
         SendMessage(dialog->GetWindow(),WM_DESTROY,0,0);
      }
   }
  
   commandLine = "\"";
   commandLine += command;
   commandLine += "\" \"";
   commandLine += htmlName;
   commandLine += "\"";

   char t[512];
   ::LoadString(g_hInstance, IDS_STARTPLAYER, t, 512);
   SendMessage(dialog->GetWindow(), WM_COMMAND, 0, (DWORD)(LPSTR) t);


   if (CreateProcess(command,(char *)commandLine.c_str(),NULL,NULL,false,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&startUpInfo,&processInfo) == 0) 
   {   
      MESSAGES::ErrorMessages::ShowMessage(dialog->GetWindow(), IDS_ERR_NOINSTALLEDPROCESS, MB_ICONERROR);
      SendMessage(dialog->GetWindow(),WM_DESTROY,0,0);
   }
   
   SendMessage(dialog->GetWindow(),WM_COMMAND,DESTROY_BUTTON,0);
   Sleep(4000);
   SendMessage(dialog->GetWindow(),WM_DESTROY,0,0);
}

void StartPlayerFromCD(const char *cdRoot, const char *docNames=NULL)
{
   DebugMsg("Starting Player from CD...\n");
   HANDLE 
      hThread;  
   static PlayerThreadParameters
      playerParams;
   std::string
      javaCommand;

   playerParams.hwnd            = dialog->GetWindow();
   playerParams.cdRoot          = cdRoot;
   playerParams.javaCommand     = "";
   playerParams.isInstalledJava = false;
   if (docNames != NULL)
      strcpy(playerParams.docNames, docNames);
   else
      playerParams.docNames[0] = '\0';

   GetProgramName(playerParams.szCallingName);

   if (javaIsOnCD(cdRoot,javaCommand))
   {
      //MessageBox(dialog->GetWindow(),"On CD",NULL,MB_OK);
      playerParams.javaCommand = javaCommand;
   }
   else if (javaIsInstalled(javaCommand))
   { 
      //MessageBox(dialog->GetWindow(),"Installed",NULL,MB_OK);
      playerParams.javaCommand = javaCommand;
      playerParams.isInstalledJava = true;
   }
   else 
   {
      MESSAGES::ErrorMessages::ShowMessage(dialog->GetWindow(), IDS_ERR_NOLEGALJAVA, MB_ICONERROR);
      SendMessage(dialog->GetWindow(),WM_DESTROY,0,0);
   }
  
   hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)CallPlayerThread,
      &playerParams, 0, &playerThreadID); 

   if(!hThread)  
   {
      MESSAGES::ErrorMessages::ShowMessage(dialog->GetWindow(), IDS_ERR_CDPROCESS, MB_ICONERROR);
      return;
   } 
   
}

void StartPlayerSetup(const char *cdRoot, const char *setupFileName)
{
   HANDLE 
      setupThread; 
   static ThreadParameters
      threadParam;
   STARTUPINFO
      startUpInfo;
   PROCESS_INFORMATION
      processInfo;
   HANDLE
      setupProcess;

   GetStartupInfo(&startUpInfo);

   char t[512];
   LoadString(g_hInstance, IDS_STARTSETUP, t, 512);
   SendMessage(dialog->GetWindow(), WM_COMMAND, 0, (DWORD)(LPCSTR)t);


   if (CreateProcess((char *)setupFileName,NULL,NULL,NULL,false,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&startUpInfo,&processInfo) == 0) 
   {  
      MESSAGES::ErrorMessages::ShowMessage(dialog->GetWindow(), IDS_ERR_NOSETUPPROCESS, MB_ICONERROR);
      SendMessage(dialog->GetWindow(),WM_DESTROY,0,0);
   }
       
   setupProcess = OpenProcess(PROCESS_TERMINATE, TRUE, processInfo.dwProcessId);
   
   threadParam.hwnd = dialog->GetWindow();
   threadParam.processHandle = processInfo.hProcess;
   threadParam.cdRoot = (char *)cdRoot;
   threadParam.dialog = dialog;

   setupThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)SetupRunThread,
			  &threadParam, 0, &setupThreadID); 

   SendMessage(dialog->GetWindow(),WM_COMMAND,HIDE_BUTTON,0);

   return;
}



/**************************************/
/********** INITIALISIERUNG  **********/
/**************************************/

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT uMessage,WPARAM wParam, LPARAM lParam) 
{ 
   switch (uMessage)    
   {    
   case WM_CREATE:  
      dialog->RePaint(NULL);
      break;   
   case WM_PAINT:
      dialog->RePaint(NULL);
      break;
   case WM_DESTROY: 
      dialog->Destroy();
      break;    
   case WM_LBUTTONDOWN:
      if (!dialog->GetButton())
      {
         dialog->Destroy();
      }
      break;
   case WM_COMMAND:
      if (wParam == DESTROY_BUTTON)
      {
         dialog->DestroyButton();
         dialog->RePaint(NULL);
      }
      else if (wParam == HIDE_BUTTON)
      {
         dialog->HideButton();
         dialog->RePaint(NULL);
      }
      else if ((HWND)lParam == dialog->GetButton())
      {
         if (playerThreadID != 0)
            PostThreadMessage(playerThreadID, WM_COMMAND, 0, 0);
      }
      else
      {
         dialog->RePaint((char *)lParam);
      }
      break;
   case WM_SHOWWINDOW:
      dialog->RePaint(NULL);
      break;
   default:  
      break;    
   } 

   return DefWindowProc(hWnd, uMessage, wParam, lParam);
}  

BOOL InitApplication(HINSTANCE hInstance) 
{ 
   WNDCLASS  
      wc;  

   wc.style          = 0; 
   wc.lpfnWndProc    = (WNDPROC)MainWndProc; 
   wc.cbClsExtra     = 0; 
   wc.cbWndExtra     = 0; 
   wc.hInstance      = hInstance; 
   wc.hIcon          = NULL; 
   wc.hCursor        = LoadCursor(NULL, IDC_ARROW); 
   wc.hbrBackground  = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
   wc.lpszMenuName   = "Lecturnity"; 
   wc.lpszClassName  = "LecturnityClass";  

   return RegisterClass(&wc); 
}  



/***********************************/
/********** MAINPROGRAMM  **********/
/***********************************/

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
		     LPSTR lpszCmdLine,int nCmdShow)
{
   std::string path;
   std::string cdRoot;
   std::string playerCommand;
   std::string setupName;
   std::string::size_type pos;
   MSG         msg; 
   char        full[_MAX_PATH];
   int         result;

   // We need the hInstance in order to use the
   // Loadxxx functions
   g_hInstance = hInstance;

   // Pfad auslesen
   path = __argv[0];
   pos = path.find_last_of("\\");
   if (pos == std::string::npos) 
      path = "";
   else 
      path.replace(pos,std::string::npos,"");

   if (_fullpath(full, path.c_str(), _MAX_PATH ) != NULL)
   {
      cdRoot = full;
      if (cdRoot[cdRoot.length() - 1] == '\\')
         cdRoot = cdRoot.substr(0, cdRoot.length() - 1);
   }
   else
      cdRoot = "";

   // Fenster initialisieren
   if(!hPrevInstance)    
      if(!InitApplication(hInstance))       
         return FALSE;  

   dialog = new AUTORUN::DialogWindow(hInstance);
   if (!dialog)
   {
      MESSAGES::ErrorMessages::ShowMessage(NULL, IDS_ERR_FATAL, MB_ICONERROR);
      return -1;
   }

   if (!dialog->Init())
   {
      MESSAGES::ErrorMessages::ShowMessage(NULL, IDS_ERR_FATAL, MB_ICONERROR);
      return -1;
   }
   

   char szProgramName[MAX_PATH];
   GetProgramName(szProgramName);

   if (stricmp(szProgramName, "autorun.exe") == 0)
   {
   
      // Player starten

      // Prefer the Player on the CD to any
      // locally installed Players. Normally the CD
      // is tested, and so it is normally safe to take
      // the CD Player.
      if ((result = playerIsOnCD(cdRoot.c_str(),playerCommand)) != 0) 
      {
         if (result == 1)
            StartPlayerFromCD(cdRoot.c_str());
         else
         {
            MESSAGES::ErrorMessages::ShowMessage(dialog->GetWindow(), IDS_ERR_SLOWMEDIUM, MB_ICONERROR);
            SendMessage(dialog->GetWindow(), WM_DESTROY,0,0);
         }
      }
      else if (playerIsInstalled(cdRoot.c_str(), playerCommand))
      {
         StartInstalledPlayer(cdRoot.c_str(), playerCommand.c_str());
      }
      else if (playerSetupIsAvailable(cdRoot.c_str(), setupName))
      {
         StartPlayerSetup(cdRoot.c_str(), setupName.c_str());
      }
      else
      {
         MESSAGES::ErrorMessages::ShowMessage(dialog->GetWindow(), IDS_ERR_NOPLAYER, MB_ICONERROR);
         SendMessage(dialog->GetWindow(), WM_DESTROY, 0, 0);
      }
   }
   else// if (stricmp(szProgramName, "startplayer.exe") == 0)
   {
      // Default to the behaviour of the old "StartPlayerFromCD.exe".
      if ((result = playerIsOnCD(cdRoot.c_str(),playerCommand)) != 0 ) 
      {
         //MessageBox(dialog->GetWindow(),"PLAYER ON CD",NULL,MB_OK);
         if (result == 1)
         {
            StartPlayerFromCD(cdRoot.c_str(), lpszCmdLine);
         }
         else
         {
            MESSAGES::ErrorMessages::ShowMessage(dialog->GetWindow(), IDS_ERR_SLOWMEDIUM, MB_ICONERROR);
            SendMessage(dialog->GetWindow(), WM_DESTROY,0,0);
         }
      }
      else
      {
         MESSAGES::ErrorMessages::ShowMessage(dialog->GetWindow(), IDS_ERR_NOPLAYER, MB_ICONERROR);
         SendMessage(dialog->GetWindow(), WM_DESTROY, 0, 0);
      }
   }

   while(GetMessage(&msg, NULL, 0x00, 0x00))    
   {    
      TranslateMessage(&msg);    
      DispatchMessage(&msg);    
   } 

	return 1;
}
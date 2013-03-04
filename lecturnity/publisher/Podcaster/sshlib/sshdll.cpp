// sshdll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

// globals
int iError;
static int iDelete = 0;
static int iCheck = 0;
static int* piExists = NULL; 
static double *dProg = NULL;
static int iArgc = 0;
static char **cArgv = NULL;
static HWND hWnd = NULL;
static HANDLE hThread = NULL;

extern "C" int pscp_main(int argc, char *argv[], HWND hWnd, int iDelete, double *dProgress, int * piError, int iCheck, int* piExists);

// the session thread
static UINT __cdecl SshThread(LPVOID pParam)
{
	int err = pscp_main(iArgc, cArgv, hWnd, iDelete, dProg, &iError, iCheck, piExists);
   return iError;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
   switch(ul_reason_for_call)
   {
      case DLL_PROCESS_DETACH:
         if(hThread != NULL)
         {
            ::TerminateThread(hThread, -1);
            ::WaitForSingleObject(hThread, INFINITE);
            hThread = NULL;
         }
         break;
   }
   return TRUE;
}

int __declspec(dllexport) __stdcall dossh(int argc, char **argv, HWND hWndPar, int isDelete, double *dProgress, int isCheck, int* piExist)
{
   iError = 1;
   hWnd = hWndPar;
   iArgc = argc;
   cArgv = argv;
   iDelete = isDelete;
   dProg = dProgress;
   iCheck = isCheck;
   piExists = piExist;

   hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SshThread, NULL, 0, NULL);  
   ::WaitForSingleObject(hThread, INFINITE);
   hThread = NULL;

   return iError;
}

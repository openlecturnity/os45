#include "StdAfx.h"
#include <stdio.h>
#include "kerberok.h"

/* old code for crashing a version not installed by 
   imc Freiburg (without the appropriate registry entry)
void NKerberok::kerberokCheck()
{
   bool letKerberokLoose = false;

	DWORD	dwKerberok = 0;
   DWORD dwType     = 0;
   DWORD dwBufSize  = 0;
	LONG	regErr;

   HKEY  hKeyMachine; 

   regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, _T("Software\\Kerberok"), NULL, KEY_READ, &hKeyMachine); 
	if ( regErr != ERROR_SUCCESS )
   {
      letKerberokLoose = true;
	}
   else
   {
   	regErr = RegQueryValueEx(hKeyMachine, _T("Fence"), NULL, &dwType, NULL, &dwBufSize );

   	if ( regErr != ERROR_SUCCESS )
      {
         letKerberokLoose = true;
   	}

      RegCloseKey(hKeyMachine);
   }   

   if (letKerberokLoose)
   {
      srand((unsigned) time(NULL));

      DWORD dwDelay = 1000 + rand() % 5000;
      Sleep(dwDelay);

      char t[512];
      sprintf(t, "Inconsistent video driver state.\nDriver state: 0x%08x", rand());
      MessageBoxA(NULL, t, "Error", MB_OK | MB_ICONERROR);
      const void *pNichts = (void *) malloc(4);
      fwrite(pNichts, 1, 4, NULL);
   }
}

  */
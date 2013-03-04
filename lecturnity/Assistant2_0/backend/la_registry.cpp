/*********************************************************************

 program  : la_registry.cpp
 authors  : Gabriela Maass
 date     : 08.01.2002
 revision : $Id: la_registry.cpp,v 1.6 2007-07-27 14:06:32 maass Exp $
 desc     : 

 **********************************************************************/

#pragma warning( disable: 4786)

#include "StdAfx.h"

#include "la_registry.h"
#include "mlb_misc.h"
#include "lutils.h" // LRegistry (lutils)

#define ASSISTANT_REGISTRY _T("Software\\imc AG\\LECTURNITY\\Assistant")

bool ASSISTANT::WindowsEntry::GetShellFolders(const _TCHAR *subFolder, CString &returnValue)
{
	DWORD		
      dwType,
      dwBufSize = 0;
   TCHAR 
      *binData;
   HKEY 
      hKeyUser; 

   LONG regErr = RegOpenKeyEx(HKEY_CURRENT_USER, 
                              _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellFolders"), 
                              NULL, KEY_READ, &hKeyUser); 
	if ( regErr != ERROR_SUCCESS )
   {
      return false;
	}

	regErr = RegQueryValueEx(hKeyUser, subFolder, NULL, &dwType, NULL, &dwBufSize );

	if ( regErr != ERROR_SUCCESS )
   {
      RegCloseKey(hKeyUser);
      return false;
	}
	
	if ( dwType != REG_SZ )
	{
      RegCloseKey(hKeyUser);
      return false; 
	}

	binData = (TCHAR *)malloc( dwBufSize + 8 );
	if ( binData == NULL )
	{
      RegCloseKey(hKeyUser);
      return false;
	}

	regErr = RegQueryValueEx(hKeyUser, subFolder, NULL, &dwType, (BYTE *)binData, &dwBufSize );

	if ( regErr != ERROR_SUCCESS )
	{
		free( binData );
      RegCloseKey(hKeyUser);
      return false;
	}

   returnValue = binData;

   free(binData);

   RegCloseKey(hKeyUser);

   return true;
}

ASSISTANT::AudioEntry::AudioEntry()
{
}

int ASSISTANT::AudioEntry::Read()
{
   int 
      retOnError = -1,
      retOnWav = 0,
      retOnLad = 1,
      ret;
	DWORD		
      dwType,
      dwBufSize = 0;
	LONG		
      regErr;
   TCHAR 
      *binData;
   HKEY 
      hKeyMachine; 

   regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, _T("Software\\Kerberok"), NULL,KEY_READ,&hKeyMachine); 
	if ( regErr != ERROR_SUCCESS )
   {
      //MessageBox(NULL,"Schlüssel existiert nicht",NULL,MB_OK);
      return retOnError;
	}

	regErr = RegQueryValueEx(hKeyMachine, _T("Audio"), NULL, &dwType, NULL, &dwBufSize );

	if ( regErr != ERROR_SUCCESS )
   {
      //MessageBox(NULL,"Schlüsseleintrag existiert nicht",NULL,MB_OK);
      RegCloseKey(hKeyMachine);
      return retOnError;
	}
	
	if ( dwType != REG_SZ )
	{
      //MessageBox(NULL,"Ein Schlüsseleintrag wurde verändert",NULL,MB_OK);
      RegCloseKey(hKeyMachine);
      return retOnError; 
	}

	binData = (TCHAR *)malloc( dwBufSize + 8 );
	if ( binData == NULL )
	{
      //MessageBox(NULL,"Probleme beim Allokieren von Speicher",NULL,MB_OK);
      RegCloseKey(hKeyMachine);
      return retOnError;
	}

	regErr = RegQueryValueEx(hKeyMachine, _T("Audio"), NULL, &dwType, (BYTE *)binData, &dwBufSize );

	if ( regErr != ERROR_SUCCESS )
	{
      //MessageBox(NULL,"Fehler beim Lesen eines Schlüssels",NULL,MB_OK);
		free( binData );
      RegCloseKey(hKeyMachine);
      return retOnError;
	}

   if (_tcsncmp(binData, _T("LAD"), 3) == 0)
      ret = retOnLad;
   else if (_tcsncmp(binData, _T("WAV"), 3) == 0)
      ret = retOnWav;
   else 
      ret = retOnError;

   free(binData);

   RegCloseKey(hKeyMachine);

   return ret;
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

      

bool ASSISTANT::SuiteEntry::ReadProgramPath(CString &programPath)
{
   _TCHAR tszInstallDir[MAX_PATH];
   DWORD dwSize = MAX_PATH;
   bool success = 
      LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
                                         _T("SOFTWARE\\imc AG\\LECTURNITY"), _T("InstallDir"),
                                         tszInstallDir, &dwSize);

   if (success)
      programPath = tszInstallDir;
   else
      programPath = _T("..\\");

   return success;
}


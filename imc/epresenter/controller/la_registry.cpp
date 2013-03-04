/*********************************************************************

 program  : la_registry.cpp
 authors  : Gabriela Maass
 date     : 08.01.2002
 revision : $Id: la_registry.cpp,v 1.1 2002-06-11 13:36:58 maass Exp $
 desc     : 

 **********************************************************************/

#include <windows.h>
#include <time.h>
#include <string>

#include "la_registry.h"

ASSISTANT::RegistryEntry::RegistryEntry()
{
   strcpy(passWord,"passwortlecturnity\0");
   strcpy(checkPassWord,"lecturnitypasswort\0");
   passWordLength = 18;
}

int ASSISTANT::RegistryEntry::ReadSecurityEntry(ASSISTANT::regEntry *entry)
{
   int 
      retOnError = -1,
      retOnSuccess = 1;
	DWORD		
      dwType,
      dwBufSize = 0;
	LONG		
      regErr;
   DWORD 
      i;	
   char 
      *binData;
   HKEY 
      hKeyMachine; 

   regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Software\\Kerberok", NULL,KEY_READ,&hKeyMachine); 
	if ( regErr != ERROR_SUCCESS )
   {
      //MessageBox(NULL,"Schlüssel existiert nicht",NULL,MB_OK);
      return retOnError;
	}

	regErr = RegQueryValueEx(hKeyMachine, "Security", NULL, &dwType, NULL, &dwBufSize );

	if ( regErr != ERROR_SUCCESS )
   {
      //MessageBox(NULL,"Schlüsseleintrag existiert nicht",NULL,MB_OK);
      RegCloseKey(hKeyMachine);
      return retOnError;
	}
	
	if ( dwType != REG_BINARY )
	{
      //MessageBox(NULL,"Ein Schlüsseleintrag wurde verändert",NULL,MB_OK);
      RegCloseKey(hKeyMachine);
      return retOnError; 
	}

	binData = (char *)malloc( dwBufSize + 8 );
	if ( binData == NULL )
	{
      //MessageBox(NULL,"Probleme beim Allokieren von Speicher",NULL,MB_OK);
      RegCloseKey(hKeyMachine);
      return retOnError;
	}

	regErr = RegQueryValueEx(hKeyMachine, "Security", NULL, &dwType, (unsigned char *)binData, &dwBufSize );

	if ( regErr != ERROR_SUCCESS )
	{
      //MessageBox(NULL,"Fehler beim Lesen eines Schlüssels",NULL,MB_OK);
		free( binData );
      RegCloseKey(hKeyMachine);
      return retOnError;
	}

   for (i=0; i < dwBufSize; ++i)
   {
      binData[i] = (char)(binData[i] ^ passWord[i%passWordLength]);
   }

   std::string 
      regData;
   std::string::size_type
      posAnf,
      posEnd;

   regData = binData;
   free(binData);

   // DATUM
   posAnf = 0;
   posEnd = regData.find('.',posAnf);
   strcpy(entry->year,regData.substr(posAnf,(posEnd-posAnf)).c_str());

   posAnf = posEnd+1;
   posEnd = regData.find('.',posAnf);
   strcpy(entry->month,regData.substr(posAnf,(posEnd-posAnf)).c_str());

   posAnf = posEnd+1;
   posEnd = regData.find(';',posAnf);
   strcpy(entry->day,regData.substr(posAnf,(posEnd-posAnf)).c_str());

   // VERSION
   posAnf = posEnd+1;
   posEnd = regData.find('.',posAnf);
   strcpy(entry->major,regData.substr(posAnf,(posEnd-posAnf)).c_str());

   posAnf = posEnd+1;
   posEnd = regData.find('.',posAnf);
   strcpy(entry->minor,regData.substr(posAnf,(posEnd-posAnf)).c_str());

   posAnf = posEnd+1;
   posEnd = regData.find('.',posAnf);
   if (posEnd == std::string::npos)
      posEnd = regData.find(':',posAnf);
   strcpy(entry->micro,regData.substr(posAnf,(posEnd-posAnf)).c_str());
   posEnd = regData.find(':',posAnf);

   // TYP
   posAnf = posEnd+1;
   posEnd = regData.find(',',posAnf);
   strcpy(entry->typ,regData.substr(posAnf,(posEnd-posAnf)).c_str());
   
   // EVALUATIONSPERIODE
   posAnf = posEnd+1;
   posEnd = regData.find('!',posAnf);
   if (posEnd == std::string::npos)
      posEnd = std::string::npos;
   strcpy(entry->period,regData.substr(posAnf,(posEnd-posAnf)).c_str());

   // SPRACHE
   if (posEnd != std::string::npos)
   {
      posAnf = posEnd+1;
      posEnd = std::string::npos;
      strcpy(entry->language,regData.substr(posAnf,(posEnd-posAnf)).c_str());
   }
   else {
      strcpy(entry->language,"de");
   }
   
   regData.erase();

   RegCloseKey(hKeyMachine);

   return retOnSuccess;
}

int ASSISTANT::RegistryEntry::ReadChecksumEntry(ASSISTANT::regEntry *entry)
{
   int 
      retOnError = -1,
      retOnSuccess = 1;
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
	char		
      *binData;
	DWORD		
      dwType;
	DWORD		
      dwBufSize = 0;
   unsigned int 
      i;
   DWORD
      dwDummy;

   regErr = RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\Kerberok", NULL, KEY_READ,&hKeyUser); 

	if ( regErr != ERROR_SUCCESS )
   {
      regErr = RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\Kerberok",
                  NULL,"",REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyUser, &dwDummy); 

	   if ( regErr != ERROR_SUCCESS )
      {
         //MessageBox(NULL,"Schlüssel kann nicht erstellt werden.",NULL,MB_OK);
         return -1;
      }

      strcpy(entry->year,"00\0");
      strcpy(entry->month,"00\0");
      strcpy(entry->day,"00\0");
	}
   else 
   {

	   regErr = RegQueryValueEx(hKeyUser, "Checksum", NULL, &dwType, NULL, &dwBufSize );

	   if ( dwType != REG_BINARY )
	   {
         //MessageBox(NULL,"Ein Schlüsseleintrag wurde verändert",NULL,MB_OK);
         RegCloseKey(hKeyUser);
         return -1;  
	   }

	   binData = (char *)malloc( dwBufSize + 8 );
	   if ( binData == NULL )
	   {
         //MessageBox(NULL,"Probleme beim Allokieren von Speicher",NULL,MB_OK);
         RegCloseKey(hKeyUser);
         return -1;  
	   }

	   regErr = RegQueryValueEx(hKeyUser, "Checksum", NULL, &dwType, (unsigned char *)binData, &dwBufSize );

	   if ( regErr != ERROR_SUCCESS )
	   {
		   free( binData );
         //MessageBox(NULL,"Fehler beim Lesen eines Schlüssels",NULL,MB_OK);
         RegCloseKey(hKeyUser);
         return -1;  
	   }

      for (i=0; i < dwBufSize; ++i)
      {
         binData[i] = (char)(binData[i] ^ checkPassWord[i%passWordLength]);
      }

      
      std::string 
         regData;
      std::string::size_type
         posAnf,
         posEnd;

      regData = binData;
      free(binData);

      posAnf = 0;
      posEnd = regData.find('.',posAnf);
      strcpy(entry->year,regData.substr(posAnf,(posEnd-posAnf)).c_str());
      posAnf = posEnd+1;
      posEnd = regData.find('.',posAnf);
      strcpy(entry->month,regData.substr(posAnf,(posEnd-posAnf)).c_str());
      posAnf = posEnd+1;
      posEnd = posAnf+2;
      strcpy(entry->day,regData.substr(posAnf,(posEnd-posAnf)).c_str());

   }

   RegCloseKey(hKeyUser);
   return 1;
}

int ASSISTANT::RegistryEntry::WriteChecksumEntry(ASSISTANT::regEntry *entry)
{
   int 
      retOnError = -1,
      retOnSuccess = 1;
   HKEY 
      hKeyUser;
   DWORD 
      newEntryLength=8;
   char 
      newEntry[8];
	LONG		
      regErr;
   int 
      i;
	DWORD		
      dwType = REG_BINARY;


   regErr = RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\Kerberok", NULL, KEY_SET_VALUE,&hKeyUser); 
	if ( regErr != ERROR_SUCCESS )
   {
      //MessageBox(NULL,"Schlüssel existiert nicht",NULL,MB_OK);
      return -1;
	}

   std::string
      zwEntry;

   zwEntry = entry->year;
   zwEntry += ".";
   zwEntry += entry->month;
   zwEntry += ".";
   zwEntry += entry->day;

   strcpy(newEntry,zwEntry.c_str());
   newEntryLength = zwEntry.length();
		
	for (i = 0; i < newEntryLength; ++i) 
	{
      newEntry[i] = newEntry[i] ^ checkPassWord[i%passWordLength];
	}
		
   regErr = RegSetValueEx(hKeyUser,"Checksum",0,dwType,(unsigned char *)newEntry,newEntryLength);

	if ( regErr != ERROR_SUCCESS )
   {
      //MessageBox (NULL,"Eintrag konnte nicht geschrieben werden",NULL,MB_OK);
      RegCloseKey(hKeyUser);
      return -1;
	}

   RegCloseKey(hKeyUser);

   return 1;
}


int ASSISTANT::RegistryEntry::ComputeTimeDiff(ASSISTANT::regEntry *entry)
{
   time_t
      actTime,
      regTime;
   long
      seconds;
   struct tm
      newTime;

   newTime.tm_year = atoi(entry->year)+100;
   newTime.tm_mon  = atoi(entry->month)-1;
   newTime.tm_mday = atoi(entry->day);

   newTime.tm_hour  = 0;
   newTime.tm_isdst = 0;
   newTime.tm_min   = 0;
   newTime.tm_sec   = 0;
   newTime.tm_wday  = 0;
   newTime.tm_yday  = 0;

   regTime = mktime(&newTime);
   
   time(&actTime);
   seconds = actTime - regTime;

   return seconds;
}

int ASSISTANT::RegistryEntry::GetActualDate(ASSISTANT::regEntry *entry)
{
   char
      date[30],
      day[3],
      month[3],
      year[3];
   int
      i;

   _strdate(date);
   for (i = 0; i < 2; ++i)
   {
     month[i] = date[i]; 
   }
   month[i] = '\0';

   for (i = 0; i < 2; ++i)
   {
      day[i] = date[i+3];
   }
   day[i] = '\0';

   for (i = 0; i < 2; ++i)
   {
      year[i] = date[i+6];
   }
   year[i] = '\0';

   strcpy(entry->year, year);
   strcpy(entry->month, month);
   strcpy(entry->day, day);

   return 1;
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
   char 
      *binData;
   HKEY 
      hKeyMachine; 

   regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Software\\Kerberok", NULL,KEY_READ,&hKeyMachine); 
	if ( regErr != ERROR_SUCCESS )
   {
      //MessageBox(NULL,"Schlüssel existiert nicht",NULL,MB_OK);
      return retOnError;
	}

	regErr = RegQueryValueEx(hKeyMachine, "Audio", NULL, &dwType, NULL, &dwBufSize );

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

	binData = (char *)malloc( dwBufSize + 8 );
	if ( binData == NULL )
	{
      //MessageBox(NULL,"Probleme beim Allokieren von Speicher",NULL,MB_OK);
      RegCloseKey(hKeyMachine);
      return retOnError;
	}

	regErr = RegQueryValueEx(hKeyMachine, "Audio", NULL, &dwType, (unsigned char *)binData, &dwBufSize );

	if ( regErr != ERROR_SUCCESS )
	{
      //MessageBox(NULL,"Fehler beim Lesen eines Schlüssels",NULL,MB_OK);
		free( binData );
      RegCloseKey(hKeyMachine);
      return retOnError;
	}

   if (strncmp(binData,"LAD",3) == 0)
      ret = retOnLad;
   else if (strncmp(binData,"WAV",3) == 0)
      ret = retOnWav;
   else 
      ret = retOnError;

   free(binData);

   RegCloseKey(hKeyMachine);

   return ret;
}

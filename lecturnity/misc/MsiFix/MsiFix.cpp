// MsiFix.cpp : Definiert den Einsprungpunkt für die Konsolenanwendung.
//

#include "stdafx.h"

int ChangeMenueLanguage(MSIHANDLE hProduct, LPCTSTR tszInputFile)
{
   UINT nError = ERROR_SUCCESS;

   PMSIHANDLE hView = 0;
   LPCTSTR tszSQL = _T("SELECT * FROM Registry");

   if (nError == ERROR_SUCCESS)
      nError = ::MsiDatabaseOpenView(hProduct, tszSQL, &hView);
   
   if (nError == ERROR_SUCCESS)
      nError = ::MsiViewExecute(hView, 0);
   
   int iReturnValue = 0;

   bool bSomeChange = false;
   bool bSomeError = false;
   if (nError == ERROR_SUCCESS)
   {
      PMSIHANDLE hRecord = 0;
      
      UINT nValueError = ::MsiViewFetch(hView, &hRecord);
      
      int iFieldCount = 0;
      if (nValueError == ERROR_SUCCESS)
         iFieldCount = ::MsiRecordGetFieldCount(hRecord);
      
      if (iFieldCount >= 6)
      {
         _TCHAR tszKey[2048];
         //_TCHAR tszValue[2048];
         while (nValueError == ERROR_SUCCESS)
         {
            DWORD dwKeyLength = 2048;
            UINT nGetError = ::MsiRecordGetString(hRecord, 3, tszKey, &dwKeyLength);

            //_tprintf(_T("%d %s "), i, tszFilename);

            if (nValueError == ERROR_SUCCESS && dwKeyLength > 0 
               && (_tcsncmp(tszKey, _T(".mp4"), 4) == 0 || _tcsncmp(tszKey, _T("imc."), 4) == 0))
            {
               //DWORD dwValueLength = 2048;
               //if (nGetError == ERROR_SUCCESS)
               //   nGetError = ::MsiRecordGetString(hRecord, 5, tszValue, &dwValueLength);
            
               //if (nValueError == ERROR_SUCCESS)
               {
                  if (_tcscmp(tszKey, _T("imc.Lecturnity.Recording\\Shell\\Publish")) == 0)
                  {
                     UINT nModError = ::MsiRecordSetString(hRecord, 5, _T("ZZZ123"));

                     if (nModError == ERROR_SUCCESS)
                        nModError = ::MsiViewModify(hView, MSIMODIFY_REPLACE, hRecord);

                     if (nModError == ERROR_SUCCESS)
                     {
                        bSomeChange = true;
                        _tprintf(_T("Updated record for key %s.\n"), tszKey);
                     }
                     else
                     {
                        bSomeError = true;
                        _tprintf(_T("!!! Faild to updated record for key %s.\n"), tszKey);
                        break;
                     }

                     bSomeChange = true;
                  }
                  else if (_tcscmp(tszKey, _T("imc.Lecturnity.Recording\\Shell\\Edit")) == 0)
                  {
                     //bSomeChange = true;
                  }
                  else if (_tcscmp(tszKey, _T(".mp4\\Shell\\Podcast")) == 0)
                  {
                     //bSomeChange = true;
                  }
                  else if (_tcscmp(tszKey, _T(".mp4\\Shell\\Podcast settings")) == 0)
                  {
                     //bSomeChange = true;
                  }
               }
            }

            //::MsiCloseHandle(hRecord); // handled by PMSIHANDLE?
            nValueError = ::MsiViewFetch(hView, &hRecord);
         }
         
         
         ::MsiViewClose(hView);
         //::MsiCloseHandle(hView); // handled by PMSIHANDLE?
         
         if (bSomeChange)
         {
            UINT nCommitError = ::MsiDatabaseCommit(hProduct);

            if (nCommitError == ERROR_SUCCESS)
               _tprintf(_T("Successfully updated %s.\n"), tszInputFile);
            else
            {
               _tprintf(_T("!!! Could not commit or write changes."));
               iReturnValue = 6;
            }
         }
         else
         {
            _tprintf(_T("No changes to %s.\n"), tszInputFile);
         }
      }
      else
      {
         _tprintf(_T("!!! Table \"Registry\" malformed? No enough fields in a row."));
         iReturnValue = 4;
      }
   
   }
   else
   {
      _tprintf(_T("!!! Cannot create or execute view."));
      iReturnValue = 5;
   }

   return iReturnValue;
}


int FixVersion(MSIHANDLE hProduct, LPCTSTR tszInputFile)
{
   UINT nError = ERROR_SUCCESS;

   PMSIHANDLE hView = 0;
   LPCTSTR tszSQL = _T("SELECT * FROM File");

   if (nError == ERROR_SUCCESS)
      nError = ::MsiDatabaseOpenView(hProduct, tszSQL, &hView);
   
   if (nError == ERROR_SUCCESS)
      nError = ::MsiViewExecute(hView, 0);
   
   int iReturnValue = 0;

   bool bSomeChange = false;
   bool bSomeError = false;
   if (nError == ERROR_SUCCESS)
   {
      PMSIHANDLE hRecord = 0;
      
      UINT nValueError = ::MsiViewFetch(hView, &hRecord);
      
      int iFieldCount = 0;
      if (nValueError == ERROR_SUCCESS)
         iFieldCount = ::MsiRecordGetFieldCount(hRecord);
      
      if (iFieldCount >= 6)
      {
         _TCHAR tszFilename[2048];
         _TCHAR tszVersion[2048];
         _TCHAR tszLanguage[2048];
         while (nValueError == ERROR_SUCCESS)
         {
            DWORD dwFileLength = 2048;
            UINT nGetError = ::MsiRecordGetString(hRecord, 3, tszFilename, &dwFileLength);
            DWORD dwVersionLength = 2048;
            if (nGetError == ERROR_SUCCESS)
               nGetError = ::MsiRecordGetString(hRecord, 5, tszVersion, &dwVersionLength);
            DWORD dwLanguageLength = 2048;
            if (nGetError == ERROR_SUCCESS)
               nGetError = ::MsiRecordGetString(hRecord, 6, tszLanguage, &dwLanguageLength);

            if (nGetError != ERROR_SUCCESS)
            {
               bSomeError = true;
               _tprintf(_T("!!! Could not get fields for a record.\n"));
               break;
            }
            
            bool bHasNoLanguage = _tcslen(tszLanguage) == 0 || _tcsncmp(tszLanguage, _T("0"), 1) == 0;
            if (_tcslen(tszVersion) > 0 && bHasNoLanguage)
            {
               // it has a version but no language: this disturbs MSI; remove the version
               
               UINT nModError = ::MsiRecordSetString(hRecord, 5, _T(""));
               if (nModError == ERROR_SUCCESS)
                  nModError = ::MsiRecordSetString(hRecord, 6, _T(""));
               
               if (nModError == ERROR_SUCCESS)
                  nModError = ::MsiViewModify(hView, MSIMODIFY_REPLACE, hRecord);
               
               if (nModError == ERROR_SUCCESS)
               {
                  bSomeChange = true;
                  _tprintf(_T("Updated record for file %s: Was %s, %s.\n"), tszFilename, tszVersion, tszLanguage);
               }
               else
               {
                  bSomeError = true;
                  _tprintf(_T("!!! Faild to updated record for file %s.\n"), tszFilename);
                  break;
               }
            }
            
            
            //::MsiCloseHandle(hRecord); // handled by PMSIHANDLE?
            nValueError = ::MsiViewFetch(hView, &hRecord);
         }
         
         
         ::MsiViewClose(hView);
         //::MsiCloseHandle(hView); // handled by PMSIHANDLE?
         
         if (bSomeChange)
         {
            UINT nCommitError = ::MsiDatabaseCommit(hProduct);

            if (nCommitError == ERROR_SUCCESS)
               _tprintf(_T("Successfully updated %s.\n"), tszInputFile);
            else
            {
               _tprintf(_T("!!! Could not commit or write changes."));
               iReturnValue = 6;
            }
         }
         else
         {
            _tprintf(_T("No changes to %s.\n"), tszInputFile);
         }

      }
      else
      {
         _tprintf(_T("!!! Table \"File\" malformed? No enough fields in a row."));
         iReturnValue = 4;
      }

   }
   else
   {
      _tprintf(_T("!!! Cannot create or execute view."));
      iReturnValue = 5;
   }

   return iReturnValue;
}

void CheckWrongPatch()
{
   _TCHAR tszVersion[30];
   ZeroMemory(tszVersion, 30 * sizeof _TCHAR);
   _TCHAR tszLanguage[10];
   ZeroMemory(tszVersion, 10 * sizeof _TCHAR);
   _TCHAR tszTransformPath[MAX_PATH];
   ZeroMemory(tszTransformPath, MAX_PATH * sizeof _TCHAR);
   const _TCHAR *tszLecCode = _T("{F3CE844F-2AF2-45C6-841E-5D390F452349}");
   DWORD dwSize = 0;

   dwSize = 30;
   UINT nError = ::MsiGetProductInfo(tszLecCode, INSTALLPROPERTY_VERSIONSTRING, tszVersion, &dwSize);

   dwSize = 10;
   if (nError == ERROR_SUCCESS)
      nError = ::MsiGetProductInfo(tszLecCode, INSTALLPROPERTY_LANGUAGE, tszLanguage, &dwSize);

   dwSize = MAX_PATH;
   if (nError == ERROR_SUCCESS)
      nError = ::MsiGetProductInfo(tszLecCode, INSTALLPROPERTY_TRANSFORMS, tszTransformPath, &dwSize);

   if (nError == ERROR_SUCCESS)
   {
      // all information retrieved

      if (_tcscmp(tszVersion, _T("2.0.0003")) == 0)
      {
         if (_tcscmp(tszLanguage, _T("0")) == 0 && _tcslen(tszTransformPath) > 0) 
         {
            // 2.0.p3 and new mixed language installer

            _TCHAR tszLang[10];
            ZeroMemory(tszLang, 10 * sizeof _TCHAR);
            bool bLang = true; //KerberokHandler::GetLanguageCode(tszLang);

            if (bLang && _tcsicmp(tszLang, _T("de")) == 0)
            {
               // installed version is German
            }
         }
      }
   }
}

int _tmain(int argc, _TCHAR* argv[])
{
   //CheckWrongPatch();
   //return 0;

   
   MSIHANDLE hProduct = 0;

   /* Reads out product properties.
   UINT nError = ::MsiOpenProduct(_T("{F3CE844F-2AF2-45C6-841E-5D390F452349}"), &hProduct);

   if (nError == ERROR_SUCCESS)
   {
      DWORD dwValueLength = 0;
      _TCHAR *tszIdent = _T("ALLUSERS");
      ::MsiGetProductProperty(hProduct, tszIdent, NULL, &dwValueLength);

      if (dwValueLength > 0)
      {
         _TCHAR tszValue[1024];
         dwValueLength = 1024;
         ::MsiGetProductProperty(hProduct, tszIdent, tszValue, &dwValueLength);
         int iDummy = 0;
      }


      ::MsiCloseHandle(hProduct);
   }
   */

   if (argc < 2)
   {
      _tprintf(_T("This program removes the version in the msi on files with version but no language setting.\n"));
      _tprintf(_T("Usage %s <arbitrary.msi>\n"), argv[0]);
      return 1;
   }

   _TCHAR *tszInputFile =  argv[1];

   if (_taccess(tszInputFile, 04) != 0)
   {
      _tprintf(_T("Cannot find or read %s\n"), tszInputFile);
      return 2;
   }

   if (_taccess(tszInputFile, 06) != 0)
   {
      _tprintf(_T("Cannot write %s\n"), tszInputFile);
      return 3;
   }


   LPCTSTR szPersistMode = MSIDBOPEN_DIRECT; // + MSIDBOPEN_PATCHFILE;
   UINT nError = ::MsiOpenDatabase(tszInputFile, szPersistMode, &hProduct);

   int iReturnValue = 0;

   if (nError == ERROR_SUCCESS)
      iReturnValue = FixVersion(hProduct, tszInputFile);
   //if (nError == ERROR_SUCCESS)
   //   iReturnValue = ChangeMenueLanguage(hProduct, tszInputFile);

   ::MsiCloseHandle(hProduct);
   

	return iReturnValue;
}

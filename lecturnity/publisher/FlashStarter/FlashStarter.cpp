// FlashStarter.cpp 
// 
/* "FlashStarter" is a tool which enables the replay of a LEC 4 created 
 * Flash Document when the document is copied to a local directory 
 * outside the "trusted Flash area". 
 * The tool work in two steps: 
 * 1) The current directory is added to the "trusted Flash area". 
 * 2) The HTML page which contains the LEC Flash Document is opened 
 *    in the default browser. 
 *    For step 2) the HTML file is determined in 2 ways: 
 *    2a) Read HTML file from a config file. 
 *        If this fails then step 2b) is performed: 
 *    2b) Search for HTML files in the current directory and 
 *        choose the newest one (older ones must be corrupt)
 */


#include "stdafx.h"
#include "FlashStarter.h"

#include "lutils.h" // Using LFile for file date comparison
#include <mmsystem.h> // Needed for definition of timeGetTime

#include <direct.h> // for getcwd
#include <stdlib.h> // for MAX_PATH

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//using namespace std;

HANDLE fHandle;
WIN32_FIND_DATA wfd; 

// Method which returns the current working directory
// (this is generally the application path)
void GetCurrentPath(_TCHAR* buffer) {
    _tgetcwd(buffer, _MAX_PATH);
}

CString GetHtmlFileFromDirectory(const _TCHAR *tszCurrentDir) {
    // Search for ".html" files in the given directory 
    // and choose the newset one (if there are mor then one file found)
    CString csHtmlFile = _T("");

    CString csCurrentPath;
    csCurrentPath.Format(_T("%s\\"), tszCurrentDir);

    // TODO: May be it is possible to make FindFirstFile((csCurrentPath + _T("*.html")), &wfd);
    fHandle = FindFirstFile((csCurrentPath + _T("*")), &wfd);
    do {
        CString csTmp = wfd.cFileName;
        // Ignore (sub) directories
        if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            CString csFile = wfd.cFileName;
            if (csFile.Find(_T(".html")) > 0) {
                // HTML file found
                if (csHtmlFile.GetLength() > 0 ) {
                    // Check file date --> choose the newest one
                    LFile lfHtmlFile(csCurrentPath + csHtmlFile);
                    LFile lfFile(csCurrentPath + csFile);
                    if (lfFile.IsEqualOrNewerThan(&lfHtmlFile)) {
                        csHtmlFile.Empty();
                        csHtmlFile.Append(csFile);
                    }
                } else {
                    csHtmlFile.Empty();
                    csHtmlFile.Append(csFile);
                }
            }
        }
    } while (FindNextFile(fHandle,&wfd));
    FindClose(fHandle);

    return csHtmlFile;
}

CString ReadHtmlFileFromConfigFile(const _TCHAR *tszCurrentDir) {
    // Open and read a config file which contains the name of the ".html" file
    CString csHtmlFile = _T("");

    // Config file must be "/content/document.cfg"
    CString csConfigFile;
    csConfigFile.Format(_T("%s\\content\\document.cfg"), tszCurrentDir);

    // Open the config file
    // The config file is (and must be) written in UTF-8 format
    LFile lfCfgFile(csConfigFile);
    HRESULT hr = lfCfgFile.Open();
    if (hr == S_OK) {
        // Read the content
        LBuffer fileBuffer(1024);
        DWORD dwBytesRead;
        // Read the UTF-8 BOM (3 bytes)
        hr = lfCfgFile.ReadRaw(&fileBuffer, 0, 3, &dwBytesRead);
        if (SUCCEEDED(hr)) {
            // Read the content (which should be the ".html" file name)
            hr = lfCfgFile.ReadRaw(&fileBuffer, 0, fileBuffer.GetSize(), &dwBytesRead);
            if (SUCCEEDED(hr)) {
                // Convert UTF-8 into WideChar
                LPCSTR tszBuffer = (LPCSTR)fileBuffer.GetBuffer();
                int iLength = ::MultiByteToWideChar(CP_UTF8, 0, tszBuffer, dwBytesRead, NULL, 0);
                WCHAR *wszDummy = new WCHAR[iLength+1];
                ::MultiByteToWideChar(CP_UTF8, 0, tszBuffer, dwBytesRead, wszDummy, iLength);
                wszDummy[iLength] = (WCHAR)'\0';
                csHtmlFile.Format(_T("%s"), wszDummy);
                // Delete wszDummy
                if (wszDummy != NULL)
                    delete wszDummy;

                // Test if html file exists
                LFile lfHtmlFile(csHtmlFile);
                bool bHtmlfileExists = lfHtmlFile.Exists();
                if (!bHtmlfileExists) {
                    // Reset the HTML file name to an empty string 
                    // (to be able to use the alternative HTML file search lateron)
                    csHtmlFile.Empty();
                }
            }
        }
        lfCfgFile.Close();
    } else {
        // TODO: error handling in case the config file can not be opened?
       _tprintf(_T("Config file %s can not be opened"), csConfigFile);
    }

    return csHtmlFile;
}

/*
 * Checks the file(s) in <app data dir>\Macromedia\Flash Player\#Security\FlashPlayerTrust\
 * if the given path must be enabled.
 * Otherwise local replay with the new Flash Player 2 will result in an
 * error message or "hanging" at "Initialize".
 */
UINT EnableOutputDir(const _TCHAR *tszEnableDir) {
    // This method is copied from FlashEngine.cpp (LPLibs)
    // If the current path is not already enabled in a file inside the "FlashPlayerTrust"
    // directory then an additional file is written which contains the given path 
    // --> the given path is added to the "trusted Flash area". 
    HRESULT hr = S_OK;

    if (tszEnableDir != NULL) {
        int iPathLength = (int)_tcslen(tszEnableDir);

        if (iPathLength > 0) {
            // file or directory is specified

            CString csTodoDir = tszEnableDir;
            csTodoDir.MakeLower();

            _TCHAR tszAppDataDir[MAX_PATH];
            HRESULT hr = SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, tszAppDataDir);

            if (SUCCEEDED(hr)) {
                _tcscat(tszAppDataDir, _T("\\Macromedia\\Flash Player\\#Security\\FlashPlayerTrust\\"));

                // make sure the security directory exists; also creates intermediate directories
                ::SHCreateDirectoryEx(NULL, tszAppDataDir, NULL);

                const _TCHAR *tszTodoDir = (const _TCHAR*)csTodoDir;
                ULONG lHashValue = 0;
                for (int i=0; i<csTodoDir.GetLength(); ++i)
                    lHashValue = (int)tszTodoDir[i] + (lHashValue << 6) + (lHashValue << 16) - lHashValue;

                _TCHAR tszSpecificFilename[MAX_PATH];
                _stprintf(tszSpecificFilename, _T("LecturnityEnable_%u.txt"), lHashValue);
                
                _tcscat(tszAppDataDir, tszSpecificFilename);

                bool bEnableFileExists = _taccess(tszAppDataDir, 00) == 0;
                bool bWriteFile = !bEnableFileExists;
                
                // Always creates/opens this file.
                HANDLE hAllowFile = ::CreateFile(tszAppDataDir, FILE_WRITE_DATA, FILE_SHARE_READ,
                    NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

                if (hAllowFile != INVALID_HANDLE_VALUE) {
                    DWORD dwFileSize = 0;
                    dwFileSize = ::GetFileSize(hAllowFile, NULL);

                    if (bEnableFileExists) {
                        DWORD dwExpectedSize = csTodoDir.GetLength() * sizeof _TCHAR;
                        if (sizeof _TCHAR == 2)
                            dwExpectedSize += 2;

                        if (dwExpectedSize != dwFileSize)
                            bWriteFile = true;

                        // This special case _might_ double entries; however
                        // it does this only if two different paths collide with one hash value
                    }

                    if (bWriteFile) {
                        // Normally only writes something if the file does not exist.

                        // Always append (see special case above).
                        ::SetFilePointer(hAllowFile, 0, NULL, FILE_END);

                        DWORD dwWrite = 0;
                        if (sizeof _TCHAR == 2 && !bEnableFileExists) {
                            // Unicode BOM
                            BYTE aBom[2] = { 0xff, 0xfe };
                            ::WriteFile(hAllowFile, aBom, 2, &dwWrite, NULL);
                        }

                        if (bEnableFileExists) {
                            ::WriteFile(hAllowFile, _T("\r\n"), 2 * sizeof _TCHAR, &dwWrite, NULL);
                        }

                        BOOL bWriteSuccess = ::WriteFile(
                            hAllowFile, tszTodoDir, csTodoDir.GetLength() * sizeof _TCHAR, &dwWrite, NULL);

                        if (!bWriteSuccess)
                            hr = E_FAIL;
                    }

                    ::CloseHandle(hAllowFile);

#ifdef _DEBUG
                    _tprintf(_T("Current Path\n%s\nis written to\n%s\n"), tszEnableDir, tszAppDataDir);
#endif
                } else {
                    hr = E_FAIL;
                    ::DeleteFile(tszAppDataDir);
                }
            }
        }
    }

    return hr;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    /* "FlashStarter" is a tool which enables the replay of a LEC 4 created 
     * Flash Document when the document is copied to a local directory 
     * outside the "trusted Flash area". 
     * The tool work in two steps: 
     * 1) The current directory is added to the "trusted Flash area". 
     * 2) The HTML page which contains the LEC Flash Document is opened 
     *    in the default browser. 
     *    For step 2) the HTML file is determined in 2 ways: 
     *    2a) Read HTML file from a config file. 
     *        If this fails then step 2b) is performed: 
     *    2b) Search for HTML files in the current directory and 
     *        choose the newest one (older ones must be corrupt)
     */

    HRESULT hr = S_OK;

	 // Initialize MFC (Error message in case of failing)
	 if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
        _tprintf(_T("MFC initializing failed!\n"));
        hr = -1;
	 } else {
        // Step 1) Add the current directory to the "trusted Flash area"
      
        // _MAX_PATH is the maximum length allowed for a path
        _TCHAR tszCurrentPath[_MAX_PATH];
      
        // Step 1a) Get the current path
        // (which is the path there this tool is executed from)
        GetCurrentPath(tszCurrentPath);
        //_tprintf(_T("Current Path: %s\n"), tszCurrentPath);

        // Step 1b) Add the current path to the "trusted Flash area"
        hr = EnableOutputDir(tszCurrentPath);

        if (SUCCEEDED(hr)) {
            // Step 2) Determine the HTML file name which has to be opened

            // Step 2a) Read HTML file from a config file ("/content/document.cfg")
            CString csHtmlFile = _T("");
            csHtmlFile = ReadHtmlFileFromConfigFile(tszCurrentPath);

            if (csHtmlFile.GetLength() <= 0) {
                // Step 2a) has failed
                // (No config file found - or it is empty)
            
                // Step 2b) Look for a (the newest) HTML file in the current directory
                csHtmlFile = GetHtmlFileFromDirectory(tszCurrentPath);
            }
#ifdef _DEBUG
            _tprintf(_T("HTML file: %s\n"), csHtmlFile);
#endif

            if (csHtmlFile.GetLength() > 0) {
                // A HTML file was found
                // --> Open it with the default browser
                HINSTANCE hResult = ShellExecute(NULL, _T("open"), csHtmlFile, _T(""), _T(""), SW_SHOWNORMAL);
                if ((int)hResult <= 32) {
                    // TODO: Error handling?
                    hr = E_FAIL;
                }
            } else {
                // TODO: Error handling for the case of no HTML file?
                _tprintf(_T("\nError: No HTML find found which can be opened!\n"));
            }
        } else {
            // TODO: Error handling for the case that the current directory 
            //       can not be added to the "trusted Flash area"?
            _tprintf(_T("\nError: 'EnableOutputDir' failed!\n"));
        }
	 }

	 return hr;
}


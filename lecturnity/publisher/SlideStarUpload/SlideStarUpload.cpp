// SlideStarUpload.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SlideStarUpload.h"
#include "SlideStarUploadDlg.h"
#include "SettingsDlg.h"
//#include "LrdDialog.h"
#include "LrdDialog2.h"
//#include "ChannelMetadata.h"
//#include "GeneralSettings.h"
#include "Registry.h"
#include "TransferSettings.h"
//#include "ParseLrd.h"
#include "LanguageSupport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlideStarUploadApp

BEGIN_MESSAGE_MAP(CSlideStarUploadApp, CWinApp)
   //{{AFX_MSG_MAP(CSlideStarUploadApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG
   ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlideStarUploadApp construction

//CSlideStarUploadApp::CSlideStarUploadApp() : CSingleInstanceApp(TEXT("{49E4CD26-9489-4474-91B0-2A30F6CA3097}"))
CSlideStarUploadApp::CSlideStarUploadApp() : CSingleInstanceApp(TEXT("{49E4CD26-9489-4474-91B0-2A30F6CA3093}"))
{
   // TODO: add construction code here,
   // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSlideStarUploadApp object

CSlideStarUploadApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSlideStarUploadApp initialization
BOOL CSlideStarUploadApp::InitInstance()
{
   HRESULT hr = S_OK;
   
   hr = CLanguageSupport::SetThreadLanguage();
   if (hr != S_OK)
   {
      CString csTitle;
      csTitle.LoadString(IDS_ERROR);
      CString csMessage;
      if (hr == E_LS_WRONG_REGISTRY)
         csMessage.LoadString(IDS_ERROR_INSTALLATION);
      else if (hr == E_LS_LANGUAGE_NOT_SUPPORTED)
         csMessage.LoadString(IDS_ERROR_LANGUAGE);
      else if (hr == E_LS_TREAD_LOCALE)
         csMessage.LoadString(IDS_ERROR_SET_LANGUAGE);

      MessageBox(NULL, csMessage, csTitle, MB_OK | MB_TOPMOST);
   }

   if (!AmITheFirst())
   {
        return FALSE;
   }
   AfxEnableControlContainer();

   // Standard initialization
   // If you are not using these features and wish to reduce the size
   //  of your final executable, you should remove from the following
   //  the specific initialization routines you do not need.

#ifdef _AFXDLL
   Enable3dControls();         // Call this when using MFC in a shared DLL
#else
   Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif
   ::CoInitialize(NULL);
   
   //[HKEY_CURRENT_USER\Software\imc AG\LECTURNITY\Publisher\SlideStarUpload]
   // create registry key
   CRegistry reg;
   if(reg.Create(_T("")) != ERROR_SUCCESS)
   {
      SystemError();
      return FALSE;
   }

   // set the temporary location to store temporary files
   //int n = ::GetEnvironmentVariable(_T("TEMP"), tstrTmpDirectory, MAX_PATH);
   //if(n == 0)
   //{
   //   n = ::GetEnvironmentVariable(_T("TMP"), tstrTmpDirectory, MAX_PATH);
   //}

   csTmpDirectory.Empty();
   int n = ::GetTempPath(MAX_PATH, tstrTmpDirectory);
   if(n != 0)
   {
      csTmpDirectory = CString(tstrTmpDirectory, n);
   }
   
   // parse comand line
   ParseCommandLine(m_cmdInfo);
   
   // set the registry path
   //SetRegistryKey(_T("imc AG\\LECTURNITY\\Publisher"));

   //csAuthor.Empty();
   //csTitle.Empty();

   int nResponse;
   CLrdDialog2 main;
   //CGeneralSettings sett;
   CTransferSettings sett;
   CSlideStarUploadDlg trans;
   //CString csChannel = trans.GetActiveChannel();
   bool bExistSettings = trans.ExistSettings();

   if(m_cmdInfo.m_dwFlags == 0)
   {
      m_pMainWnd = &main;
      nResponse = main.DoModal();
   }
   else
   {
      if(m_cmdInfo.m_dwFlags == CMD_UNKNOWN)
      {
         CString csExtension = m_cmdInfo.m_csUnknown.Right(4);
         if(csExtension.CompareNoCase(_T(".lzp")) == 0)
         {
			if (!bExistSettings)
            {
               // no settings
               m_pMainWnd = &sett;
               nResponse = sett.DoModal();
            }
            else
            {
                  main.csIniZipFile.Format(_T("%s"), m_cmdInfo.m_csUnknown);
                  m_pMainWnd = &main;
                  nResponse = main.DoModal();
            }
         }
         else
         {
            // not a lzp file
            m_pMainWnd = &main;
            nResponse = main.DoModal();
         }
      }
      else
      {
         if(m_cmdInfo.m_dwFlags & CMD_SETTINGS)
         {
            m_pMainWnd = &sett;
            nResponse = sett.DoModal();
         }
         else
         {
		    if (!bExistSettings)
            {
               // no settings
               m_pMainWnd = &sett;
               nResponse = sett.DoModal();
            }
            else
            {
               if( (m_cmdInfo.m_dwFlags & CMD_TRANSFER) || 
                   (m_cmdInfo.m_dwFlags & CMD_TRANSFER2) || 
                   (m_cmdInfo.m_dwFlags & CMD_TRANSFER3) )
               {
                  trans.csAuthor.Format(_T("%s"), m_cmdInfo.m_csAuthor);
                  trans.csTitle.Format(_T("%s"), m_cmdInfo.m_csTitle);
                  trans.csMediaFile.Format(_T("%s"), m_cmdInfo.m_csMediaFileName);
                  m_pMainWnd = &trans;
                  nResponse = trans.DoModal();
               }
            }
         }
      }
   }

   if (nResponse == IDOK)
   {
      // TODO: Place code here to handle when the dialog is
      //  dismissed with OK
   }
   else if (nResponse == IDCANCEL)
   {
      // TODO: Place code here to handle when the dialog is
      //  dismissed with Cancel
   }

   // Since the dialog has been closed, return FALSE so that we exit the
   //  application, rather than start the application's message pump.

   // delete temporary files
   //::DeleteFile(csTmpDirectory + _T("\\tmp.xml"));
   //::DeleteFile(csTmpDirectory + _T("\\tmp2.xml"));
   //::DeleteFile(csTmpDirectory + _T("\\tmp3.xml"));
   //::DeleteFile(csTmpDirectory + _T("\\new.xml"));
   //::DeleteFile(csTmpDirectory + _T("\\mod.xml"));

   ::CoUninitialize();

   return FALSE;
}

//BOOL CSlideStarUploadApp::FindLrdFile()
//{
//   CParseLrd lrd;
//   CString csLrdFile;
//
//   TCHAR m_tszCurrentDirectory[MAX_PATH];
//
//   csLrdFile.Empty();
//
//   WIN32_FIND_DATA w32fd;
//   ZeroMemory(&w32fd, sizeof(w32fd));
//   w32fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
//   
//   HANDLE hFind = ::FindFirstFile(_T("*.lrd"), &w32fd);
//   if (hFind != INVALID_HANDLE_VALUE)
//   {
//      ::FindClose(hFind);
//      ::GetCurrentDirectory(MAX_PATH, m_tszCurrentDirectory);
//      csLrdFile.Format(_T("%s\\%s"), m_tszCurrentDirectory, w32fd.cFileName);
//      
//      // if found parse LRD
//      lrd.Parse(csLrdFile);
//      csAuthor = CString(lrd.GetAuthor());
//      csTitle  = CString(lrd.GetTitle());
//   
//      return TRUE;
//   }
//
//   return FALSE;
//}

void CSlideStarUploadApp::SystemError()
{
   LPVOID lpMsgBuf;
   CString csErrorMessage, csFormat;
   DWORD dwErrorNo = ::GetLastError();
   
   if(dwErrorNo == 0)
   {
      return;
   }

   if(::FormatMessage( 
       FORMAT_MESSAGE_ALLOCATE_BUFFER | 
       FORMAT_MESSAGE_FROM_SYSTEM | 
       FORMAT_MESSAGE_IGNORE_INSERTS,
       NULL,
       dwErrorNo,
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
       (LPTSTR) &lpMsgBuf,
       0,
       NULL ) == 0)
   {
      // Handle the error.
      csFormat.LoadString(IDS_FTPERROR);
      csErrorMessage.Format(csFormat, dwErrorNo);
      ::MessageBox(NULL, csErrorMessage, _T("SlideStarUpload"), MB_OK|MB_ICONSTOP);
      return;
   }

   // Display the string.
   ::MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("SlideStarUpload"), MB_OK|MB_ICONSTOP);
   // Free the buffer.
   ::LocalFree(lpMsgBuf);
}

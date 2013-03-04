// CAutoUpdateDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lsupdt32.h"
#include "AutoUpdateDialog.h"
#include "SgmlParser.h"
#include "KeyGenerator.h" // KerberokHandler
#include "PatchAvailableDialog.h"
#include "NewsAvailableDialog.h"
#include "NotAdminDialog.h"
#include "DownloadDlg.h"
#include "MfcUtils.h"     // MftUtils
#include "LanguageSupport.h"     // lsutl32

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAutoUpdateDialog 


CAutoUpdateDialog::CAutoUpdateDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoUpdateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoUpdateDialog)
	//}}AFX_DATA_INIT

   m_bForceCheck = false;
   m_pInternet = NULL;
}


void CAutoUpdateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoUpdateDialog)
	DDX_Control(pDX, IDC_CANCEL, m_cancelButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoUpdateDialog, CDialog)
	//{{AFX_MSG_MAP(CAutoUpdateDialog)
	ON_COMMAND(IDC_CANCEL, OnCancel)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CAutoUpdateDialog 

int  CAutoUpdateDialog::DoCheck()
{
   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();

   int nMajor, nMinor, nMicro, nPatch;
   bool bVersionOk = KerberokHandler::ReadCurrentVersion(nMajor, nMinor, nMicro, nPatch);

   if (bVersionOk)
   {
      // Retrieve current time in seconds since 1970.
      DWORD dwCurrentTime = (DWORD)time(NULL);

      // Is Auto Update enabled at all? Or are we forced to
      // make a check?
      bool success = (pDll->IsAnnouncementsEnabled() || pDll->IsAutoUpdateEnabled()) || m_bForceCheck;
      
      if (success)
      {
         // Is it time for a new check?
         // Note: 86400 seconds is one day.
         // We consider it time for a new check if we are
         // forced to check, too.
         int nSecondsPerDay = 86400;
         
         m_bTimeForUpdateCheck = m_bForceCheck ||
            (pDll->IsAutoUpdateEnabled() &&
            (pDll->GetLastUpdateTime() + nSecondsPerDay * pDll->GetCheckEvery()) < dwCurrentTime);
         m_bTimeForAnnouncementCheck = m_bForceCheck ||
            (pDll->IsAnnouncementsEnabled() &&
            (pDll->GetLastAnnouncementTime() + nSecondsPerDay * pDll->GetCheckEvery()) < dwCurrentTime);
         
         success = (m_bTimeForUpdateCheck || m_bTimeForAnnouncementCheck);
      }

      if (success)
      {
         // It's time to do something
         return DoModal();
      }
   }
   else
   {
      CString csError;
      csError.LoadString(IDS_ERR_LICENSEERROR);
      MessageBox(csError, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONERROR);
      m_bWorkerFinished = true;
   }

   return 0;
}

BOOL CAutoUpdateDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

   DWORD dwIds[] = {IDC_LABEL_SEARCHFORUPDATES,
                    IDC_LABEL_PATIENCE,
                    IDC_CANCEL,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CString csTitle;
   csTitle.LoadString(IDD_AUTOUPDATE);
   SetWindowText(csTitle);

   m_bWorkerFinished = false;
   m_bCancelRequested = false;
   m_bDialogResult = FALSE;

   AfxBeginThread(UpdateThreadLauncher, this); //, 0, 0, 0, NULL);

   SetTimer(0, 250, NULL);

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

// ----------------------------------------------------------------------------

UINT CAutoUpdateDialog::UpdateThreadLauncher(LPVOID pParam)
{
   CLanguageSupport::SetThreadLanguage();

   ((CAutoUpdateDialog *) pParam)->UpdateThread();
   return 0;
}

void CAutoUpdateDialog::UpdateThread()
{
   // Reset the "found something" variable
   m_bHasFoundSomething = false;

   // Gather some necessary information, like the version
   // type and if updating should be possible at all.
   // If the following fails, it not possible to retrieve
   // the current patch level, and thus everything fails.
   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();
   m_nVersionType = KerberokHandler::GetVersionType(pDll->GetVersion());

   bool success = (m_nVersionType != TYPE_VERSION_ERROR);
   if (!success)
   {
      CString csError;
      csError.LoadString(IDS_ERR_LICENSEERROR);
      MessageBox(csError, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONERROR);

      // Nothing else is done after this, success is false.
      // In order to avoid double code, we do not put a
      // "return" here.
   }

  
   // LBuffer supports growing buffer sizes. Specify
   // 16K blocks for resizing.
   LBuffer lSgml(16384); // Initial size is 16K
   lSgml.SetResizeStepSize(16384);
   
   // ContactUpdateServer tries to contact the update server
   // and stores the SGML data for the update information
   // into the LBuffer variable.
   if (success)
      success = ContactUpdateServer(lSgml);

/*
   char *szSgml = 
      "<updates>"
         "<version>1.6.0</version>"
         "<patchlevel>3</patchlevel>"
         "<url>http://132.230.139.59/updates/dummy.msp</url>"
         "<size>1234567</size>"
         "<announcements>"
            "<overview>http://132.230.139.59/csc/index.php?id=150&L=0</overview>"
            "<announcement>"
               "<title>Release-Notes: LECTURNITY 1.6.0.p3</title>"
               "<annurl>http://132.230.139.59/csc/index.php?id=151&l=0</annurl>"
               "<releasenotes>True</releasenotes>"
           "</announement>"
         "</announcements>"
      "</updates>";
*/

   // Only if something could be downloaded successfully,
   // we try to process the data. In other cases, an error
   // message has already been issued by ContactUpdateServer.
   if (success)
   {
      SgmlFile parser;
      SgmlElement *pAnnouncements = NULL;

      if (success && !m_bCancelRequested)
         success = parser.ReadMemory(lSgml.GetByteAccess(), lSgml.GetSize(), _T("<updates>"));

      if (success && !m_bCancelRequested)
         success = ExtractPatchInformation(parser.GetRoot());

      if (success && !m_bCancelRequested)
      {
         pAnnouncements = parser.Find(_T("announcements"));
         success = (pAnnouncements != NULL);
      }

      if (success && !m_bCancelRequested)
         success = ExtractAnnouncements(pAnnouncements);

      if (success && !m_bCancelRequested)
         success = MarkReadAnnouncements();

      // Now let's see if we could extract all information
      // we needed.
      if (!success && !m_bCancelRequested)
      {
         CString csError;
         csError.LoadString(IDS_ERR_INVALIDDATA);
         MessageBox(csError, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONERROR);
      }

      if (success)
      {
         // If it's time to check for updates, do that now
         bool bUpdateIssued = false;
         if (m_bTimeForUpdateCheck)
         {
            bUpdateIssued = CheckUpdates();
            // If an update was issued, the dialog's result
            // has to be TRUE
            if (bUpdateIssued)
               m_bDialogResult = TRUE;
         }

         // If it's time to check for new announcements, do that now.
         // If an update was issued (an URL passed on to msiexec.exe),
         // then nothing more should be done.
         if (m_bTimeForAnnouncementCheck && !bUpdateIssued)
         {
            bool ignore = CheckAnnouncements();
         }
      }
   }

   if (success && !m_bCancelRequested && !m_bHasFoundSomething && m_bForceCheck)
   {
      // If we're doing a forced check, like from the Configuration
      // tool, then we want to output an information message in case
      // nothing was found.
      CString csMessage;
      csMessage.LoadString(IDS_MSG_NOUPDATES);
      MessageBox(csMessage, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONINFORMATION);
   }

   if (m_bCancelRequested)
   {
      CString csMessage;
      csMessage.LoadString(IDS_MSG_CANCEL);
      MessageBox(csMessage, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONINFORMATION);
   }

   m_bWorkerFinished = true;
}

// ----------------------------------------------------------------------------

bool CAutoUpdateDialog::ContactUpdateServer(LBuffer &lSgml)
{
   // Assemble the URL for the update file
   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();

   CString csUrlFormatDefault;
   csUrlFormatDefault.LoadString(IDS_URL);
   LString lsUrlFormat;
   lsUrlFormat.Allocate(1024);
   DWORD dwSize = 1024;
   LRegistry::ReadSettingsString(_T("AutoUpdate"), _T("URL"), (LPTSTR) lsUrlFormat, &dwSize, 
      (LPCTSTR) csUrlFormatDefault);

   CString csType = _T("eval");
   if (m_nVersionType == TYPE_FULL_VERSION || m_nVersionType == TYPE_CAMPUS_VERSION)
      csType = _T("full");

   CString csLanguage = pDll->GetLanguage();
   if (csLanguage.Compare(_T("de")) != 0)
      csLanguage = _T("en"); // en is the default, de the only possible exception

   CString csUrl;
   csUrl.Format((LPCTSTR) lsUrlFormat, pDll->GetVersion(), csLanguage, csType);

   // We store the ID of an error message in this
   // variable.
   LURESULT dwErrorCode = S_OK;
   DWORD dwSystemError  = 0;

   m_pInternet = new LInternet; // pointer: needed by OnCancel()

   dwErrorCode = m_pInternet->OpenUrl(csUrl, &dwSystemError);
   bool success = (dwErrorCode == S_OK);

   if (success)
   {
      DWORD dwBytesToRead = 2048;
      DWORD dwBytesRead   = 1;
      
      char pBuffer[2048];
      LURESULT ret = S_OK;
      while (dwBytesRead > 0)
      {
         ret = m_pInternet->Read(pBuffer, dwBytesToRead, &dwBytesRead, &dwSystemError);
         if (S_OK == ret)
         {
            if (dwBytesRead > 0)
            {
               lSgml.AddBuffer(pBuffer, dwBytesRead);
            }
         }
         else
         {
            success = false;
            dwBytesRead = 0;
         }
      }
   }

   // Is harmless if nothing is open
   m_pInternet->Close();

   delete m_pInternet;
   m_pInternet = NULL;

   if (success)
   {
      // probably nothing to be done:
      // data is in LBuffer and will be parsed outside

      /*
      // Transfer SGML data from LBuffer to char *
      int nAddedSize = lBuffer.GetAddedSize();
      lsSgml.Allocate(nAddedSize + 1);

      // Big fat note & later TODO: This assumes that LString contains a char * buffer;
      // this will not work with the UNICODE flag defined!
      // Possible later remedy: LString contains Unicode (UCS-2) characters, and the
      // buffer contains UTF-8 characters. Then use MultiByteToWideChar.
      memcpy((LPSTR) lsSgml, lBuffer.GetBuffer(), nAddedSize);

      ((LPSTR) lsSgml)[nAddedSize] = '\000';
      */
   }
   else if (!m_bCancelRequested) // error could occur because it was cancelled
   {
      // An error occurred; "translate" the error message into something
      // we can handle
      switch (dwErrorCode)
      {
      case LINTERNET_ERR_INTERNETATTEMPT:
         dwErrorCode = IDS_ERR_INTERNETATTEMPT;
         break;
      case LINTERNET_ERR_INTERNETOPEN:
         dwErrorCode = IDS_ERR_INTERNETOPEN;
         break;
      case LINTERNET_ERR_INTERNETURL:
         dwErrorCode = IDS_ERR_INTERNETURL;
         break;
      case LINTERNET_ERR_NOTFOUND:
         dwErrorCode = IDS_ERR_INTERNETNOTFOUND;
         break;
      case LINTERNET_ERR_READ:
         dwErrorCode = IDS_ERR_INTERNETREAD;
         break;
      }

      // Output an error message.
      CString csError;
      csError.LoadString(dwErrorCode);
      CString csSystemError;

      CString csMessage;

      if (dwSystemError != 0)
      {
         LPVOID lpMsgBuf;
         if (FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dwSystemError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL))
         {
            csSystemError.Format(IDS_ERR_SYSTEM, (LPCTSTR) lpMsgBuf);
            LocalFree(lpMsgBuf);
            csMessage.Format(_T("%s\n\n%s"), csError, csSystemError);
         }
         else
         {
            csMessage = csError;
         }
      }
      else
         csMessage = csError;


      // suppress the display of an error message in some cases
      // e.g. if it is an automatic update check during program start
      bool bSuppressErrorMessage = false;
      _TCHAR tszCallingName[MAX_PATH];
      DWORD dwCopied = GetModuleFileName(NULL, tszCallingName, MAX_PATH);

      // look if it is NOT "lsupdate.exe" calling us
      if (dwCopied > 0 && NULL == _tcsstr(tszCallingName, _T("lsupdate.exe")))
      {
         if (dwErrorCode == IDS_ERR_INTERNETATTEMPT ||
            dwErrorCode == IDS_ERR_INTERNETOPEN || dwErrorCode == IDS_ERR_INTERNETURL ||
            dwErrorCode == IDS_ERR_INTERNETNOTFOUND || dwErrorCode == IDS_ERR_INTERNETREAD)
            bSuppressErrorMessage = true;
      }


      if (!bSuppressErrorMessage)
         MessageBox(csMessage, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONERROR);
   }

   return success;
}

// ----------------------------------------------------------------------------

bool CAutoUpdateDialog::ExtractPatchInformation(SgmlElement *pRoot)
{
   m_csPatchUrl = "";
   m_nPatchLevel = 0;
   m_nPatchSizeBytes = 0;

   bool bDeMarkerExists = false;
   bool bIsDeVersion = false;

   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();

   CString csLanguage = pDll->GetLanguage();
   if (csLanguage.Compare(_T("de")) == 0)
   {
      bIsDeVersion = true;

      // check for marker file

      LString lsMarkerFile;
      lsMarkerFile.Allocate(MAX_PATH);
      DWORD dwSize = MAX_PATH;
      bool bRegSuccess = LRegistry::ReadSettingsString(
         _T(""), _T("InstallDir"), (LPTSTR)lsMarkerFile, &dwSize, NULL, true);

      if (bRegSuccess)
      {
         if (lsMarkerFile[lsMarkerFile.Length() - 1] != _T('\\'))
            lsMarkerFile += _T("\\");
         lsMarkerFile += _T("Backend\\marker_de.txt");

         if (_taccess(lsMarkerFile, 00) == 0)
            bDeMarkerExists = true;
      }
   }
      
   CString csOtherPatchUrl = _T("");
   int nOtherPatchSize = 0;

   CArray<SgmlElement *, SgmlElement *> aElements;
   pRoot->GetElements(aElements);
   for (int i = 0; i < aElements.GetSize(); ++i) {
       SgmlElement *pHelp = aElements[i];
       if (pHelp != NULL) {
           if (_tcsicmp(pHelp->GetName(), _T("patchlevel")) == 0) {
               m_nPatchLevel = _ttoi(pHelp->GetParameter());
           } else if (_tcsicmp(pHelp->GetName(), _T("url")) == 0) {
               m_csPatchUrl = pHelp->GetParameter();
           } else if (_tcsicmp(pHelp->GetName(), _T("url-other")) == 0) {
               csOtherPatchUrl = pHelp->GetParameter();
           } else if (_tcsicmp(pHelp->GetName(), _T("size")) == 0) {
               m_nPatchSizeBytes = _ttoi(pHelp->GetParameter());
           } else if (_tcsicmp(pHelp->GetName(), _T("size-other")) == 0) {
               nOtherPatchSize = _ttoi(pHelp->GetParameter());
           }
       }
   }

   // A german version downloads a german patchinfo.xml.
   // This patchinfo.xml links to the (old) German patch file with "url".
   // A new installation (>= 2.0.p3) is always based on the English install package.
   // So for a new installation (!bDeMarkerExists) in the 
   // German case (bIsDeVersion) the other url has to be used for the patch file.
   //
   if (bIsDeVersion && !bDeMarkerExists && csOtherPatchUrl.GetLength() > 0)
   {
      m_csPatchUrl = csOtherPatchUrl;
      m_nPatchSizeBytes = nOtherPatchSize;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool CAutoUpdateDialog::ExtractAnnouncements(SgmlElement *pAnnouncements)
{
   // Reset the list of announcements:
   m_caAnnouncements.RemoveAll();

   CArray<SgmlElement *, SgmlElement *> aElements;
   pAnnouncements->GetElements(aElements);
   for (int i = 0; i < aElements.GetCount(); ++i) {
       SgmlElement *pAnn = aElements[i];
       if (pAnn != NULL) {
           if (_tcsicmp(pAnn->GetName(), _T("announcement")) == 0) {
               CString csTitle;
               CString csUrl;
               bool    bReleaseNote;

               bool bHasTitle = false;
               bool bHasUrl   = false;
               bool bHasReleaseNote = false;


               CArray<SgmlElement *, SgmlElement *> aAnnElements;
               pAnn->GetElements(aAnnElements);
               for (int j = 0; j < aAnnElements.GetCount(); ++j) {
                   SgmlElement *pHelp = aAnnElements[j];
                   if (pHelp != NULL) {
                       if (_tcsicmp(pHelp->GetName(), _T("title")) == 0) {
                           csTitle = pHelp->GetParameter();
                           bHasTitle = true;
                       } else if (_tcsicmp(pHelp->GetName(), _T("annurl")) == 0) {
                           csUrl = pHelp->GetParameter();
                           bHasUrl = true;
                       } else if (_tcsicmp(pHelp->GetName(), _T("releasenotes")) == 0) {
                           bReleaseNote = (_tcsicmp(pHelp->GetParameter(), _T("true")) == 0);
                           bHasReleaseNote = true;
                       }
                   }
               }

               // A title and an URL are indispensable
               if (bHasTitle && bHasUrl)
                   m_caAnnouncements.Add(CAnnouncement(csTitle, csUrl, bReleaseNote));
           }
           else if (_tcsicmp(pAnn->GetName(), _T("overview")) == 0) {
               m_csOverviewUrl = pAnn->GetParameter();
           }
       }
   }

   return true;
}

// ----------------------------------------------------------------------------

bool CAutoUpdateDialog::MarkReadAnnouncements()
{
   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();

   CString csLastAnnouncement = pDll->GetLastAnnouncementUrl();

   // Search for that announcement in the announcement list
   int nPos = -1;
   int i = 0;
   for (i=0; i<m_caAnnouncements.GetSize(); ++i)
   {
      if (m_caAnnouncements[i].m_csUrl == csLastAnnouncement)
         nPos = i;
   }

   // Mark all announcements up to that position as viewed
   if (nPos != -1)
   {
      for (i=0; i<=nPos; ++i)
      {
         m_caAnnouncements[i].SetViewed(true);
      }
   }

   return true;
}

// ----------------------------------------------------------------------------

bool CAutoUpdateDialog::CheckAnnouncements()
{
   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();

   // Count undisplayed announcements.
   int nUndisplayedAnnouncements = 0;
   for (int i=0; i<m_caAnnouncements.GetSize(); ++i)
   {
      if (!m_caAnnouncements[i].IsViewed())
         ++nUndisplayedAnnouncements;
   }

   // Are there undisplayed announcements?
   if (nUndisplayedAnnouncements > 0)
   {
      // There are undisplayed announcements. As there are undisplayed
      // announcements, there have to exist announcements at all.

      // Something has been found, so let's set that variable
      m_bHasFoundSomething = true;

      // We'll save the newest announcement as the last announcement,
      // even if the user decides not to view them.
      int nLastAnnIndex = m_caAnnouncements.GetSize() - 1;
      pDll->SetLastAnnouncementUrl(m_caAnnouncements[nLastAnnIndex].m_csUrl);

      CNewsAvailableDialog dlgNewsAvailable(
         nUndisplayedAnnouncements, 
         m_caAnnouncements[nLastAnnIndex].m_csTitle,
         this);
      int nReturn = dlgNewsAvailable.DoModal();

      // Does the user want to look at any announcements?
      if (IDOK == nReturn) // Yes
      {
         if (nUndisplayedAnnouncements > 1)
         {
            // There are more than one announcement to display,
            // display the overview page.
            DWORD dwIgnore = (DWORD) ::ShellExecute(GetSafeHwnd(), _T("open"), 
               m_csOverviewUrl, NULL, NULL, SW_SHOW);
         }
         else
         {
            // Only one announcement is undisplayed, which has
            // to be the last one. Display that.
            DWORD dwIgnore = (DWORD) ::ShellExecute(GetSafeHwnd(), _T("open"),
               m_caAnnouncements[nLastAnnIndex].m_csUrl, NULL, NULL, SW_SHOW);
         }
      }
      // else: ignore, don't do anything

      // Does the user want to turn off announcement checking?
      if (dlgNewsAvailable.IsDisableAnnouncementsChecked())
         pDll->EnableAnnouncements(false);
   }

   // Update time for last announcement check to current time
   pDll->SetLastAnnouncementTime((DWORD)time(NULL));

   return false;
}

// ----------------------------------------------------------------------------

bool CAutoUpdateDialog::CheckUpdates()
{
   CLsUpdt32Dll *pDll = (CLsUpdt32Dll *) AfxGetApp();

   // Retrieve the patch level of the installed LECTURNITY
   int nInstalledPatchLevel = KerberokHandler::GetPatchLevel();

   // By default update the LastUpdate registry entry
   bool bUpdateLastUpdate = true;
   // Will be set to true if an update is issued
   bool bUpdateIssued     = false;

   // Is the patch available newer than the installed version?
   if (m_nPatchLevel > nInstalledPatchLevel)
   {
      // Yes, do something. We know that the user will
      // get a message of some kind here.
      m_bHasFoundSomething = true;

      // Does the user have administrator rights or can he get them (elevation)?
      // For elevation this is then handled by LaunchProgram()
      if (LMisc::IsUserAdmin() || LMisc::IsWindowsVistaOrHigher() && LMisc::CanUserElevate())
      {
         // Find the latest release notes.
         CString csRNUrl = _T("http://www.lecturnity.de/csc");
         // We need the position of the release notes in order
         // to know whether there are more unread announcements
         // to display afterwards.
         int nRNPos = -1;
         for (int i = m_caAnnouncements.GetSize()-1; i>=0 && nRNPos == -1; --i)
         {
            if (true == m_caAnnouncements[i].m_bReleaseNote)
            {
               csRNUrl = m_caAnnouncements[i].m_csUrl;
               nRNPos = i;
            }
         }

         CPatchAvailableDialog dlgPatchAvailable(csRNUrl, m_nPatchSizeBytes, this);
         int nReturn = dlgPatchAvailable.DoModal();

         // Did the user look at the release notes?
         if (nRNPos != -1 && dlgPatchAvailable.HasDisplayedReleaseNotes())
         {
            // Only if the release notes are the first unread
            // announcement the release notes url is saved
            // as the last displayed announcement. Otherwise
            // there are other undisplayed announcements before
            // the release notes. These should be displayed, too.
            bool bMarkAsViewed = false;

            if (nRNPos == 0)
               bMarkAsViewed = true;
            else if (nRNPos > 0 && m_caAnnouncements[nRNPos-1].IsViewed())
               bMarkAsViewed = true;

            if (bMarkAsViewed)
            {
               m_caAnnouncements[nRNPos].SetViewed(true);
               pDll->SetLastAnnouncementUrl(csRNUrl);
            }
         }

         // Does the user want to disable Auto Update (Check box in
         // the "Patch Available" dialog)?
         if (dlgPatchAvailable.IsDisableAutoUpdateChecked())
         {
            pDll->EnableAutoUpdate(false);
         }

         // Now, what next?
         if (nReturn == IDOK) // Yes
         {
            _TCHAR tszTempPath[_MAX_PATH];
            ::GetTempPath(_MAX_PATH, tszTempPath);

            CDownloadDlg dlgDownload(this, m_csPatchUrl, m_nPatchSizeBytes, tszTempPath);
            dlgDownload.DoModal();

            if (dlgDownload.HasSucceeded())
            {
               // The user wants to install the update
               CString csMessage;
               // Is it a forced check, i.e. did the Configuration program
               // launch the check? Then display another message than otherwise
               if (m_bForceCheck)
                  csMessage.LoadString(IDS_MSG_CONFIGURETERMINATE);
               else
                  csMessage.LoadString(IDS_MSG_PROGRAMTERMINATES);

               MessageBox(csMessage, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONINFORMATION);

               CString csPatchFileName;
               dlgDownload.GetTargetFileName(csPatchFileName);

               // msiexec is called like this:
               // msiexec.exe /p "<path to patch>"
               CString csParameter;
               csParameter.Format(_T("/p \"%s\""), csPatchFileName);

               // Let's find msiexec.exe; it should be in the
               // WINDOWS\System[32] directory.
               CString csMsiExec;
               _TCHAR szSysDir[MAX_PATH + 1];
               UINT res = ::GetSystemDirectory(szSysDir, MAX_PATH + 1);
               if (res != 0)
                  csMsiExec.Format(_T("%s\\msiexec.exe"), szSysDir);
               else // GetSystemDirectory failed, Windows has to try to find msiexec.exe in %PATH%
                  csMsiExec = _T("msiexec.exe");
               // The else case is (was) the normal case, but it doesn't work with Win98.

               if (!LMisc::LaunchProgram(csMsiExec, csParameter, true))
               {
                  // Calling msiexec.exe failed. Notify user of that.
                  CString csError;
                  csError.LoadString(IDS_ERR_MSIEXEC);
                  MessageBox(csError, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONERROR);
               }
               else
               {
                  // msiexec.exe was launched successfully. Notify
                  // the dialog that an update was issued. In this case,
                  // the calling program (may be a LECTURNITY component
                  // or the configuration program) is to be terminated
                  // immediately.
                  bUpdateIssued = true;

                  // setting last update check to today is done below
                  //pDll->SetLastUpdateTime(time(NULL));
               }
            }
         }
         else if (nReturn == IDIGNORE) // No
         {
            // The user does not want to install the update;
            // Nothing is done, the LastUpdate registry entry
            // is set to the current date. This makes LECTURNITY
            // look for updates in CheckEvery days the next time.
         }
         else if (nReturn == IDCANCEL) // Later
         {
            // The user wants to install the patch "later".
            // This means that the update mechanism is invoked
            // at the next start of any LECTURNITY component.
            bUpdateLastUpdate = false;
         }
      }
      else
      {
         // User is not admin, tell him so.
         CNotAdminDialog dlgNotAdmin(this);
         dlgNotAdmin.DoModal();

         // Check the states of the check boxes of that dialog
         if (dlgNotAdmin.IsDisableAutoUpdateChecked())
            pDll->EnableAutoUpdate(false);
         if (!dlgNotAdmin.IsCheckInDaysChecked())
            bUpdateLastUpdate = false;
      }
   }

   if (bUpdateLastUpdate)
   {
      // Update the LastUpdate time in the
      // registry
      pDll->SetLastUpdateTime((DWORD)time(NULL));
   }

   return bUpdateIssued;
}

// ----------------------------------------------------------------------------

void CAutoUpdateDialog::OnCancel() 
{
   m_cancelButton.EnableWindow(FALSE);
   m_bCancelRequested = true;

   if (m_pInternet != NULL)
       m_pInternet->Close(); // otherwise can block forever (very long) in ContactUpdateServer;
                             // and cancel does nothing
}

// ----------------------------------------------------------------------------

void CAutoUpdateDialog::OnTimer(UINT nIDEvent) 
{
   if (m_bWorkerFinished)
   {
      KillTimer(0);
      EndDialog(m_bDialogResult);
   }
}


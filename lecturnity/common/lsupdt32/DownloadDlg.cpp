// DownloadDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lsupdt32.h"
#include "DownloadDlg.h"

#include "MfcUtils.h"     // MftUtils
#include "LanguageSupport.h"     // lsutl32

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDownloadDlg 


CDownloadDlg::CDownloadDlg(CWnd* pParent, LPCTSTR tszUrl, int nPatchSizeBytes, LPCTSTR tszTargetPath)
	: CDialog(CDownloadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDownloadDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_csSourceUrl = tszUrl;
   m_csTargetPath = tszTargetPath;
   if (m_csTargetPath[m_csTargetPath.GetLength()-1] != (TCHAR) '\\')
      m_csTargetPath += CString(_T("\\"));

   m_bDownloadSuccessful = false;
   m_bCancelRequested = false;
   m_bDownloadThreadFinished = false;
   m_dwDownloadSize = nPatchSizeBytes; // will be updated later (except proxy)
}


void CDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownloadDlg)
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_LABEL_DL_PROGRESS, m_lbProgress);
	DDX_Control(pDX, IDC_DL_PROGRESS, m_prgDownload);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDownloadDlg, CDialog)
	//{{AFX_MSG_MAP(CDownloadDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDownloadDlg 

BOOL CDownloadDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   DWORD dwIds[] = {IDC_LABEL_PD_PATIENCE,
                    IDC_LABEL_DL_PROGRESS,
                    IDCANCEL,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CString csTitle;
   csTitle.LoadString(IDD_DOWNLOAD);
   SetWindowText(csTitle);

   m_lbProgress.SetWindowText(_T(""));
   m_prgDownload.SetRange(0, 100);

   AfxBeginThread(DLThreadLauncher, this);
   SetTimer(54321, 250, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

UINT CDownloadDlg::DLThreadLauncher(LPVOID pData)
{
   CLanguageSupport::SetThreadLanguage();

   ((CDownloadDlg *) pData)->DownloadThread();
   return 0L;
}

void CDownloadDlg::DownloadThread()
{
   // Where should we try to put the file
   CString csFileName;
   // Extract file name from URL
   int nLastSlash = m_csSourceUrl.ReverseFind((TCHAR) '/');
   // That's our file name, in the given directory
   csFileName = m_csSourceUrl.Mid(nLastSlash + 1);
   m_csTargetPath = m_csTargetPath + csFileName;
   
   bool bFileError = false;

   LInternet lInternet;
   DWORD dwSysErr = 0;
   LURESULT lr = lInternet.OpenUrl(m_csSourceUrl, &dwSysErr);
   DWORD dwLength = 0;

   // Save the start time of the transfer
   m_ttStartTime = time(NULL);
   // Check length of download
   if (S_OK == lr)
   {
      DWORD dwLengthFromHeader = 0;
      lr = lInternet.GetLength(&dwLengthFromHeader);

      if (S_OK == lr)
         m_dwDownloadSize = dwLengthFromHeader;
      else
      {
         // try again with a direct connection; 
         // maybe a problematic proxy is activated (ie in imc)

         lInternet.Close();

         lr = lInternet.OpenUrl(m_csSourceUrl, &dwSysErr, true);

         if (S_OK == lr)
         {
            lr = lInternet.GetLength(&dwLengthFromHeader);
            if (S_OK == lr)
               m_dwDownloadSize = dwLengthFromHeader;
         }
         // else m_dwDownloadSize keeps the initial value
         // but not being able to determine the size is bad: the error is not reset
      }
   }

   if (S_OK == lr)
      UpdateProgress(0);

   LFile lFile(m_csTargetPath);
   if (S_OK == lr)
   {
      lr = lFile.Create(LFILE_TYPE_BINARY);
      if (S_OK != lr)
         bFileError = true;
   }

   if (S_OK == lr)
   {
      const DWORD dwBufSize = 16384;

      DWORD dwBytesToRead = dwBufSize;
      DWORD dwBytesRead = 1;
      DWORD dwTotalBytesRead = 0;
      char pBuffer[dwBufSize];
      while (dwBytesRead > 0 && !m_bCancelRequested)
      {
         lr = lInternet.Read(pBuffer, dwBytesToRead, &dwBytesRead, NULL);
         if (S_OK == lr)
         {
            if (dwBytesRead > 0)
            {
               dwTotalBytesRead += dwBytesRead;

               lr = lFile.WriteRaw(pBuffer, dwBytesRead);
               if (S_OK == lr)
               {
                  UpdateProgress(dwTotalBytesRead);
               }
               else
               {
                  bFileError = true;
                  dwBytesRead = 0;
               }
            }
         }
      }
   }
   lFile.Close();

   if (S_OK == lr && !m_bCancelRequested)
   {
      m_bDownloadSuccessful = true;
   }
   else if (!m_bCancelRequested)
   {
      // Assemble some error message
      CString csMsg;
      if (bFileError)
         csMsg.LoadString(IDS_ERR_WRITEFILE);
      else
      {
         if (LINTERNET_ERR_NOTFOUND == lr)
            csMsg.LoadString(IDS_ERR_PATCHNOTFOUND);
         else
            csMsg.LoadString(IDS_ERR_READPATCH);
      }

      MessageBox(csMsg, _T("LECTURNITY Auto-Update"), MB_OK | MB_ICONWARNING);

      m_bCancelRequested = true;
   }

   if (!m_bDownloadSuccessful)
      lFile.Delete();

   m_bDownloadThreadFinished = true;
}

void CDownloadDlg::GetTargetFileName(CString &csFileName)
{
   csFileName = m_csTargetPath;
}

bool CDownloadDlg::HasSucceeded()
{
   return m_bDownloadSuccessful;
}

void CDownloadDlg::UpdateProgress(DWORD dwBytesRead)
{
   time_t ttCurrent = time(NULL);
   time_t ttDelta = (ttCurrent - m_ttStartTime);
   if (ttDelta == 0)
      ttDelta = 1; // To avoid Division by Zero, ttDelta is in seconds

   float fDlSpeed = (((float) dwBytesRead) / ((float) ttDelta)) / 1024.0f;

   CString csProgress;
   csProgress.Format(IDS_MSG_DL_PROGRESS, dwBytesRead / 1024, m_dwDownloadSize / 1024, fDlSpeed);
   m_lbProgress.SetWindowText(csProgress);

   float fPercent = ((float) dwBytesRead) / ((float) m_dwDownloadSize);
   int nPercent = (int) (100.0f * fPercent);
   m_prgDownload.SetPos(nPercent);
}

void CDownloadDlg::OnCancel() 
{
   // TODO: Zusätzlichen Bereinigungscode hier einfügen

   m_bCancelRequested = true;
   m_btCancel.EnableWindow(FALSE);
}

void CDownloadDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
   if (m_bCancelRequested || m_bDownloadThreadFinished)
   {
      while (!m_bDownloadThreadFinished)
         Sleep(100);

      KillTimer(54321);

      EndDialog(IDOK);
   }
	
	CDialog::OnTimer(nIDEvent);
}

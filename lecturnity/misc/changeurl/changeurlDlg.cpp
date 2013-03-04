// changeurlDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "changeurl.h"
#include "changeurlDlg.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeurlDlg Dialogfeld

CChangeurlDlg::CChangeurlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeurlDlg::IDD, pParent), CChangeUrlEngine()
{
	//{{AFX_DATA_INIT(CChangeurlDlg)
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChangeurlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeurlDlg)
	DDX_Control(pDX, IDC_DOCTYPE, m_lbDocType);
	DDX_Control(pDX, IDC_SCORM, m_lbScorm);
	DDX_Control(pDX, IDC_SMIL, m_edSmilFile);
	DDX_Control(pDX, IDC_VERSION, m_lbVersion);
	DDX_Control(pDX, IDC_TYPE_STREAMING, m_rbStreaming);
	DDX_Control(pDX, IDC_TYPE_LOCAL, m_rbLocal);
	DDX_Control(pDX, IDC_TYPE_HTTP, m_rbHttp);
	DDX_Control(pDX, IDC_LABEL_URLSTREAMING, m_lbUrlStreaming);
	DDX_Control(pDX, IDC_LABEL_URLHTTP, m_lbUrlHttp);
	DDX_Control(pDX, IDC_LABEL_URL_HINT, m_lbUrlHint);
	DDX_Control(pDX, IDC_LABEL_MODIFY, m_lbModify);
	DDX_Control(pDX, IDC_URL_STREAMING, m_edUrlStreaming);
	DDX_Control(pDX, IDC_URL_HTTP, m_edUrlHttp);
	DDX_Control(pDX, IDC_MODIFY, m_btModify);
	DDX_Control(pDX, IDC_FRAME_STEP3, m_frmStep3);
	DDX_Control(pDX, IDC_FRAME_STEP2, m_frmStep2);
	DDX_Control(pDX, IDC_FRAME_STEP1, m_frmStep1);
	DDX_Control(pDX, IDC_FILENAME, m_edFilename);
	DDX_Control(pDX, IDC_BROWSE, m_btBrowse);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChangeurlDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeurlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TYPE_LOCAL, OnTypeLocal)
	ON_BN_CLICKED(IDC_TYPE_HTTP, OnTypeHttp)
	ON_BN_CLICKED(IDC_TYPE_STREAMING, OnTypeStreaming)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_MODIFY, OnModify)
	ON_EN_CHANGE(IDC_URL_HTTP, OnChangeUrl)
	ON_BN_CLICKED(IDC_CLOSEAPP, OnCloseApp)
	ON_EN_CHANGE(IDC_URL_STREAMING, OnChangeUrl)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeurlDlg Nachrichten-Handler

BOOL CChangeurlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

   DWORD dwIds[] = {
      IDC_BROWSE,
      IDC_TYPE_LOCAL,
      IDC_TYPE_HTTP,
      IDC_TYPE_STREAMING,
      IDC_LABEL_URLHTTP,
      IDC_LABEL_URLSTREAMING,
      IDC_LABEL_URL_HINT,
      IDC_MODIFY,
      IDC_FRAME_STEP1,
      IDC_FRAME_STEP2,
      IDC_FRAME_STEP3,
      IDC_LABEL_MODIFY,
      IDC_CLOSEAPP,
      IDC_FILENAME_LABEL,
      IDC_DOCTYPE_LABEL,
      IDC_VERSION_LABEL,
      IDC_SCORM_LABEL,
      IDC_SMIL_LABEL,
      -1};

   MfcUtils::Localize(this, dwIds);

   DisableControls();

   // Wihtout this call, it is not possible to scroll to the right. Stoopid.
   //m_listStatus.SetHorizontalExtent(1024);
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CChangeurlDlg::DisableControls()
{
   // Step 2 controls
   EnableStep2(FALSE);

   // Step 3 controls
   EnableStep3(FALSE);
}

void CChangeurlDlg::EnableStep3(BOOL bEnable)
{
   // Step 3 controls
   m_frmStep3.EnableWindow(bEnable);
   m_lbModify.EnableWindow(bEnable);
   m_btModify.EnableWindow(bEnable); 
}

void CChangeurlDlg::LogMessage(CString csMsg, UINT nBeep)
{
   // 3.0.p1: Modification: LogMessage not only used for error messages, 
   // but also for question and information messages
   /*
   if (nBeep != MB_ICONERROR)
   {
      //m_listStatus.AddString(tszMsg);
   }
   else
   {
      CString csError;
      csError.LoadString(IDS_ERROR);
      //m_listStatus.AddString(csError + CString(_T(": ")) + CString(tszMsg));
   }

   //m_listStatus.RedrawWindow();
   //int nMin, nMax;
   //m_listStatus.GetScrollRange(SB_VERT, &nMin, &nMax);
   //m_listStatus.SetScrollPos(SB_VERT, nMax);
   */

   if (nBeep)
   {
      CString csType;
      if (nBeep == MB_ICONERROR)
      {
         csType.LoadString(IDS_ERROR);
         MessageBox(csMsg, csType, MB_OK | MB_ICONERROR);
      }
      else if (nBeep == MB_ICONWARNING)
      {
         csType.LoadString(IDS_WARNING);
         MessageBox(csMsg, csType, MB_OK | MB_ICONWARNING);
      }
      else if (nBeep == MB_ICONINFORMATION)
      {
         ::MessageBeep(nBeep);
         csType.LoadString(IDS_INFORMATION);
         MessageBox(csMsg, csType, MB_OK | MB_ICONINFORMATION);
      }
      else
         ::MessageBeep(nBeep);
   }
}

bool CChangeurlDlg::QuestionMessage(CString csMsg)
{
   CString csQuestion;
   csQuestion.LoadString(IDS_QUESTION);

   if (MessageBox(csMsg, csQuestion, MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
      return true;
   else
      return false;
}

void CChangeurlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CChangeurlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CChangeurlDlg::SetWaitCursor(bool bDisplay)
{
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CChangeurlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool CChangeurlDlg::GetFileFromFileDialog(CString &csFileName, DWORD dwFilterId)
{
   CString csFileFilter;
   csFileFilter.LoadString(dwFilterId);

   _TCHAR tszFileFilter[512];
   _tcscpy(tszFileFilter, csFileFilter);
   int nLen = _tcslen(tszFileFilter);
   for (int i=0; i<nLen; ++i)
   {
      if (tszFileFilter[i] == (_TCHAR) '|')
         tszFileFilter[i] = (_TCHAR) 0;
   }

   _TCHAR tszFile[MAX_PATH];
   ZeroMemory(tszFile, sizeof _TCHAR * MAX_PATH);
   _TCHAR tszFileTitle[MAX_PATH];
   ZeroMemory(tszFileTitle, sizeof _TCHAR * MAX_PATH);

   OPENFILENAME ofn;
   ZeroMemory(&ofn, sizeof OPENFILENAME);
   ofn.lStructSize = sizeof OPENFILENAME;
   ofn.hwndOwner = GetSafeHwnd();
   ofn.lpstrFilter = tszFileFilter;
   ofn.lpstrFile = tszFile;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrFileTitle = tszFileTitle;
   ofn.nMaxFileTitle = MAX_PATH;
   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

   bool bSuccess = (TRUE == ::GetOpenFileName(&ofn));

   if (bSuccess)
      csFileName = tszFile;

   return bSuccess;
}

void CChangeurlDlg::OnBrowse() 
{
   CString csFileName;
   bool bSuccess = GetFileFromFileDialog(csFileName, IDS_FILEFILTER);

   if (bSuccess)
   {
      DisableControls();
      m_edFilename.SetWindowText(csFileName);

      bool success = ResolveHtmlFileNames();
      if (success)
         success = RetrieveCurrentType();

      if (success)
         CChangeUrlEngine::LogMessage(IDS_MSG_LOADSUCCESS, m_csBaseName);
      else
         CChangeUrlEngine::LogMessage(IDS_ERR_LOADNOSUCCESS, m_csBaseName);
   }
}

void CChangeurlDlg::OnDropFiles(HDROP hDropInfo) 
{
   _TCHAR tszFileName[2*_MAX_PATH];
   DragQueryFile(hDropInfo, 0, tszFileName, 2*_MAX_PATH);

   CString csFileName(tszFileName);
   DisableControls();
   m_edFilename.SetWindowText(csFileName);

   bool success = ResolveHtmlFileNames();
   if (success)
      success = RetrieveCurrentType();

   if (success)
      CChangeUrlEngine::LogMessage(IDS_MSG_LOADSUCCESS, m_csBaseName);
   else
      CChangeUrlEngine::LogMessage(IDS_ERR_LOADNOSUCCESS, m_csBaseName);
	
	CDialog::OnDropFiles(hDropInfo);
}

CString CChangeurlDlg::GetDocumentName()
{
   CString csDocName;
   m_edFilename.GetWindowText(csDocName);
   return csDocName;
}

void CChangeurlDlg::SetDocumentName(LPCTSTR tszDocName)
{
   m_edFilename.SetWindowText(tszDocName);
}

CString CChangeurlDlg::GetUrlHttp()
{
   CString csUrlHttp;
   m_edUrlHttp.GetWindowText(csUrlHttp);
   return csUrlHttp;
}

void CChangeurlDlg::SetUrlHttp(LPCTSTR tszUrlHttp)
{
   m_edUrlHttp.SetWindowText(tszUrlHttp);
}

CString CChangeurlDlg::GetUrlStreaming()
{
   CString csUrlStreaming;
   m_edUrlStreaming.GetWindowText(csUrlStreaming);
   return csUrlStreaming;
}

void CChangeurlDlg::SetUrlStreaming(LPCTSTR tszUrlStreaming)
{
   m_edUrlStreaming.SetWindowText(tszUrlStreaming);
}

UINT CChangeurlDlg::GetTargetType()
{
   if (m_rbHttp.GetCheck() == BST_CHECKED)
      return REPLAY_TYPE_HTTP;
   else if (m_rbStreaming.GetCheck() == BST_CHECKED)
      return REPLAY_TYPE_STREAMING;
   return REPLAY_TYPE_LOCAL;
}

void CChangeurlDlg::EnableStep2(BOOL bEnable, UINT nReplayType)
{
   m_frmStep2.EnableWindow(bEnable);
   m_rbLocal.EnableWindow(bEnable);
   m_rbHttp.EnableWindow(bEnable);
   m_rbStreaming.EnableWindow(bEnable);

   if (bEnable)
   {
      if (nReplayType == REPLAY_TYPE_LOCAL)
      {
         m_rbLocal.SetCheck(BST_CHECKED);
         OnTypeLocal();
      }
      else if (nReplayType == REPLAY_TYPE_HTTP)
      {
         m_rbHttp.SetCheck(BST_CHECKED);
         OnTypeHttp();
      }
      else if (nReplayType == REPLAY_TYPE_STREAMING)
      {
         m_rbStreaming.SetCheck(BST_CHECKED);
         OnTypeStreaming();
      }
   }
   else
   {
      m_lbUrlHttp.EnableWindow(FALSE);
      m_edUrlHttp.EnableWindow(FALSE);
      m_lbUrlStreaming.EnableWindow(FALSE);
      m_edUrlStreaming.EnableWindow(FALSE);
      m_lbUrlHint.EnableWindow(FALSE);
   }

   if (m_csDocVersion != _T("-") &&
       m_csDocVersion.Compare(_T("1.7.0")) >= 0 &&
       bEnable != FALSE) 
   {
      CString csTmp;
      // Special case! Doc version >= 1.7.0, so we'll have to take
      // care here
      if (m_nDocType == DOCTYPE_FLASH)
      {
         csTmp.LoadString(IDS_LOCALHTTP);
         m_rbLocal.SetWindowText(csTmp);
         m_rbHttp.EnableWindow(FALSE);

         if (m_bTempDirUsed && (m_nOriginalReplayType == REPLAY_TYPE_LOCAL))
         {
            CString csMsg;
            csMsg.LoadString(IDS_WARN_STRICTSCORM);
            bool result = QuestionMessage(csMsg);
            if (!result)
            {
               EnableStep2(FALSE);
               return;
            }
         }

         // 3.0.p1: No restriction in editing (SCORM) ZIP files any more
         /*
         if (!m_bTempDirUsed)
         {
            // Non-SCORM
            csTmp.LoadString(IDS_LOCALHTTP);
            m_rbLocal.SetWindowText(csTmp);
            m_rbHttp.EnableWindow(FALSE);
         }
         else
         {
            // SCORM
            if (nReplayType != REPLAY_TYPE_STREAMING)
            {
               // Not possible [DOUBLE CODE]
               CString csMsg;
               //csMsg.LoadString(IDS_SCORM_NOT_EDITABLE);
               //MessageBox(csMsg, NULL, MB_OK | MB_ICONINFORMATION);
               //EnableStep2(FALSE);
               csMsg.LoadString(IDS_WARN_STRICTSCORM);
               bool result = QuestionMessage(csMsg);
               if (!result)
               {
                  EnableStep2(FALSE);
                  return;
               }
            }
            else
            {
               // Only changing the Streaming Server address is possible, nothing else.
               m_rbLocal.EnableWindow(FALSE);
               m_rbHttp.EnableWindow(FALSE);
            }
         }
         */
      }
      else if (m_nDocType == DOCTYPE_REAL)
      {
         if (!m_bHasClips)
         {
            // No clips, no SMIL file
            csTmp.LoadString(IDS_LOCALHTTP);
            m_rbLocal.SetWindowText(csTmp);
            m_rbHttp.EnableWindow(FALSE);

            if (m_bTempDirUsed && (m_nOriginalReplayType == REPLAY_TYPE_LOCAL))
            {
               CString csMsg;
               csMsg.LoadString(IDS_WARN_STRICTSCORM);
               bool result = QuestionMessage(csMsg);
               if (!result)
               {
                  EnableStep2(FALSE);
                  return;
               }
            }

            // 3.0.p1: No restriction in editing (SCORM) ZIP files any more
            /*
            if (!m_bTempDirUsed)
            {
               // Non-SCORM
               csTmp.LoadString(IDS_LOCALHTTP);
               m_rbLocal.SetWindowText(csTmp);
               m_rbHttp.EnableWindow(FALSE);
            }
            else
            {
               // SCORM
               if (nReplayType != REPLAY_TYPE_STREAMING)
               {
                  // Not possible [DOUBLE CODE]
                  CString csMsg;
                  //csMsg.LoadString(IDS_SCORM_NOT_EDITABLE);
                  //MessageBox(csMsg, NULL, MB_OK | MB_ICONINFORMATION);
                  //EnableStep2(FALSE);
                  csMsg.LoadString(IDS_WARN_STRICTSCORM);
                  bool result = QuestionMessage(csMsg);
                  if (!result)
                  {
                     EnableStep2(FALSE);
                     return;
                  }
               }
               else
               {
                  // Only changing the Streaming Server address is possible, nothing else.
                  m_rbLocal.EnableWindow(FALSE);
                  m_rbHttp.EnableWindow(FALSE);
               }
            }
            */
         }
         else
         {
            // Has clips, i.e. SMIL file

            // 3.0.p1: No restriction in editing (SCORM) ZIP files any more
            /*
            if (m_bTempDirUsed)
            {
               // SCORM package
               if (nReplayType == REPLAY_TYPE_HTTP)
               {
                  m_rbLocal.EnableWindow(FALSE);
                  m_rbStreaming.EnableWindow(FALSE);
               }
               else if (nReplayType == REPLAY_TYPE_STREAMING)
               {
                  m_rbLocal.EnableWindow(FALSE);
                  m_rbHttp.EnableWindow(FALSE);
               }
            }
            else
            {
               // Nothing has to be changed in this case: SMIL file,
               // not SCORM package.
            }
            */
         }
      }
      else
      {
         // m_nDocType == DOCTYPE_WMT
         csTmp.LoadString(IDS_LOCALHTTP);
         m_rbLocal.SetWindowText(csTmp);
         m_rbHttp.EnableWindow(FALSE);

         if (m_bTempDirUsed && (m_nOriginalReplayType == REPLAY_TYPE_LOCAL))
         {
            CString csMsg;
            csMsg.LoadString(IDS_WARN_STRICTSCORM);
            bool result = QuestionMessage(csMsg);
            if (!result)
            {
               EnableStep2(FALSE);
               return;
            }
         }

         // 3.0.p1: No restriction in editing (SCORM) ZIP files any more
         /*
         if (!m_bTempDirUsed)
         {
            // Non-SCORM
            csTmp.LoadString(IDS_LOCALHTTP);
            m_rbLocal.SetWindowText(csTmp);
            m_rbHttp.EnableWindow(FALSE);
         }
         else
         {
            // SCORM
            if (nReplayType != REPLAY_TYPE_STREAMING)
            {
               // Not possible [DOUBLE CODE]
               CString csMsg;
               //csMsg.LoadString(IDS_SCORM_NOT_EDITABLE);
               //MessageBox(csMsg, NULL, MB_OK | MB_ICONINFORMATION);
               //EnableStep2(FALSE);
               csMsg.LoadString(IDS_WARN_STRICTSCORM);
               bool result = QuestionMessage(csMsg);
               if (!result)
               {
                  EnableStep2(FALSE);
                  return;
               }
            }
            else
            {
               // Only changing the Streaming Server address is possible, nothing else.
               m_rbLocal.EnableWindow(FALSE);
               m_rbHttp.EnableWindow(FALSE);
            }
         }
         */
      }
   }
   else
   {
      CString csTmp;
      csTmp.LoadString(IDC_TYPE_LOCAL);
      m_rbLocal.SetWindowText(csTmp);
      csTmp.LoadString(IDC_TYPE_HTTP);
      m_rbHttp.SetWindowText(csTmp);
      csTmp.LoadString(IDC_TYPE_STREAMING);
      m_rbStreaming.SetWindowText(csTmp);
   }

   EnableStep3(FALSE);
}

void CChangeurlDlg::OnTypeLocal() 
{
	m_rbHttp.SetCheck(BST_UNCHECKED);
   m_rbStreaming.SetCheck(BST_UNCHECKED);

   m_lbUrlHttp.EnableWindow(FALSE);
   m_edUrlHttp.EnableWindow(FALSE);
   m_lbUrlStreaming.EnableWindow(FALSE);
   m_edUrlStreaming.EnableWindow(FALSE);
   m_lbUrlHint.EnableWindow(FALSE);

   EnableStep3();
}

void CChangeurlDlg::OnTypeHttp() 
{
   m_rbLocal.SetCheck(BST_UNCHECKED);
   m_rbStreaming.SetCheck(BST_UNCHECKED);

   m_lbUrlHttp.EnableWindow(TRUE);
   m_edUrlHttp.EnableWindow(TRUE);
   m_lbUrlStreaming.EnableWindow(FALSE);
   m_edUrlStreaming.EnableWindow(FALSE);
   m_lbUrlHint.EnableWindow(TRUE);

   EnableStep3();
}

void CChangeurlDlg::OnTypeStreaming() 
{
   m_rbLocal.SetCheck(BST_UNCHECKED);
	m_rbHttp.SetCheck(BST_UNCHECKED);

   BOOL bEnable = TRUE;
   if ((m_nDocType == DOCTYPE_WMT) ||
       (m_nDocType == DOCTYPE_REAL && !m_bHasClips))
      bEnable = FALSE;
   m_lbUrlHttp.EnableWindow(bEnable);
   m_edUrlHttp.EnableWindow(bEnable);
   m_lbUrlStreaming.EnableWindow(TRUE);
   m_edUrlStreaming.EnableWindow(TRUE);
   m_lbUrlHint.EnableWindow(TRUE);

   if (m_csDocVersion != _T("-") &&
       m_csDocVersion.Compare(_T("1.7.0")) >= 0)
   {
      m_lbUrlHttp.EnableWindow(FALSE);
      m_edUrlHttp.EnableWindow(FALSE);
   }

   EnableStep3();
}

void CChangeurlDlg::OnModify() 
{
   bool res = UpdateUrls();
   if (res == true)
      m_btModify.EnableWindow(FALSE);
}

void CChangeurlDlg::OnChangeUrl() 
{
   EnableStep3();
}

void CChangeurlDlg::OnCloseApp() 
{
   EndDialog(0);	
}


void CChangeurlDlg::SetDocType(LPCTSTR tszDocType)
{
   if (tszDocType)
      m_lbDocType.SetWindowText(tszDocType);
   else
      m_lbDocType.SetWindowText(_T(""));
}

void CChangeurlDlg::SetDocVersion(LPCTSTR tszDocVersion)
{
   if (tszDocVersion)
      m_lbVersion.SetWindowText(tszDocVersion);
   else
      m_lbVersion.SetWindowText(_T(""));
}

void CChangeurlDlg::SetScormSupport(LPCTSTR tszScormSupport)
{
   if (tszScormSupport)
      m_lbScorm.SetWindowText(tszScormSupport);
   else
      m_lbScorm.SetWindowText(_T(""));
}

void CChangeurlDlg::SetSmilFileName(LPCTSTR tszSmilFileName)
{
   CChangeUrlEngine::SetSmilFileName(tszSmilFileName);

   if (tszSmilFileName)
      m_edSmilFile.SetWindowText(tszSmilFileName);
   else
      m_edSmilFile.SetWindowText(_T(""));
}

bool CChangeurlDlg::RequestSmilFileName(CString &csSmilFileName)
{
   CString csMsg;
   csMsg.LoadString(IDS_MSG_SMILNOTFOUND);
   MessageBox(csMsg, NULL, MB_OK | MB_ICONINFORMATION);

   CString csFileName;
   bool success = GetFileFromFileDialog(csFileName, IDS_SMIFILEFILTER);
   if (success)
   {
      csSmilFileName = csFileName;
   }
   else
   {
      csSmilFileName = _T("");
   }

   return success;
}

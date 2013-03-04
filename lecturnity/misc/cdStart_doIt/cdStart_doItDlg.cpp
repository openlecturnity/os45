#include "stdafx.h"
#include "cdStart_doIt.h"
#include "cdStart_doItDlg.h"

#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// #define _ORDER_BUTTON
#undef _ORDER_BUTTON

#define ID_EXPLAIN_CDDEMO 0
#define ID_EXPLAIN_GUIDED 1
#define ID_EXPLAIN_INSTALL 2
#define ID_EXPLAIN_SYSREQ 3
#define ID_EXPLAIN_EXIT 4
#define ID_EXPLAIN_ORDER 5
#define ID_EXPLAIN_EMPTY 6

/////////////////////////////////////////////////////////////////////////////
// CCdStart_doItDlg Dialogfeld

CCdStart_doItDlg::CCdStart_doItDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCdStart_doItDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCdStart_doItDlg)
		// HINWEIS: Der Klassenassistent fügt hier Member-Initialisierung ein
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
   
   m_iPressedButton = -1;
   m_bIntroStopped = true;
   m_introEnd = -1;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_wDeviceID = 0;
   m_bUse4Buttons = true;
}

void CCdStart_doItDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCdStart_doItDlg)
		// HINWEIS: Der Klassenassistent fügt an dieser Stelle DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCdStart_doItDlg, CDialog)
	//{{AFX_MSG_MAP(CCdStart_doItDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SYSREQ, OnSysReq)
	ON_BN_CLICKED(IDC_TUTORIAL, OnGuidedTour)
	ON_BN_CLICKED(IDC_INSTALL, OnInstall)
	ON_BN_CLICKED(IDC_CDDEMO, OnCdDemo)
	ON_BN_CLICKED(IDC_STOP_INTRO, OnStopIntro)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
   ON_COMMAND(ID_TUTORIAL_LECTURNITY, OnLecturnityTutorial)
   ON_COMMAND(ID_TUTORIAL_PDF, OnPdfTutorial)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


bool CCdStart_doItDlg::CheckAudioReplay(LPCTSTR szAudioFilename)
{
   //
   // Check if this system is able to replay our compressed audio:
   //
   MCI_OPEN_PARMS mciOpenParams;
   
   // Open the device by specifying the device and filename.
   mciOpenParams.lpstrDeviceType = "waveaudio";
   mciOpenParams.lpstrElementName = szAudioFilename;
   if (::mciSendCommand(NULL, MCI_OPEN,
      MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPVOID) &mciOpenParams))
   {
      // Failed to open device and/or file. Use the fallback file.
        
      return false;
   }
   else
   {
      // Close the successfully opened device.
      ::mciSendCommand(mciOpenParams.wDeviceID, MCI_CLOSE, 0, NULL);

      return true;
   }

   
}

bool CCdStart_doItDlg::ReplayAudio(LPCTSTR szAudioFilename)
{
   StopAudio(); // only does something if a device is open


   MCI_OPEN_PARMS mciOpenParams;
   
   // Open the device by specifying the device and filename.
   mciOpenParams.lpstrDeviceType = "waveaudio";
   mciOpenParams.lpstrElementName = szAudioFilename;
   if (::mciSendCommand(NULL, MCI_OPEN,
      MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPVOID) &mciOpenParams))
   {
      return false;
   }
   
   // The device opened successfully; get the device ID.
   m_wDeviceID = mciOpenParams.wDeviceID;
   
   MCI_PLAY_PARMS mciPlayParams; // no callback specified
   if (::mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_NOTIFY, 
      (DWORD)(LPVOID) &mciPlayParams))
   {
      ::mciSendCommand(m_wDeviceID, MCI_CLOSE, 0, NULL);
      m_wDeviceID = 0;
      return false;
   }

   return true;
}

void CCdStart_doItDlg::StopAudio()
{
   if (0 != m_wDeviceID)
   {
     ::mciSendCommand(m_wDeviceID, MCI_CLOSE, 0, NULL); 
     m_wDeviceID = 0;
   }

}

/////////////////////////////////////////////////////////////////////////////
// CCdStart_doItDlg Nachrichten-Handler

BOOL CCdStart_doItDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

   char lpFilename[512];
   GetModuleFileName(AfxGetApp()->m_hInstance,  lpFilename,  512);
   m_szProgramPath = lpFilename;
   int pos = m_szProgramPath.ReverseFind('\\');
   m_szProgramPath = m_szProgramPath.Left(pos);

   // get the bitmap for the background:
   m_backgroundPicture.CreateFromBitmap(IDB_BACKGROUND);

	// Create the player window like you would any Windows control.
	m_mciWnd.Create(WS_CHILD | WS_VISIBLE  | MCIWNDF_NOPLAYBAR | MCIWNDF_NOERRORDLG | MCIWNDF_NOTIFYERROR,
                   CRect(284, 130, 800, 130+135), this, 1234);


   m_skipButton.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0, 130, 284, 130+135), this, IDC_STOP_INTRO, IDB_SKIP, 284, 135);
	
	// Open the AVI file.
 
	// get path of skin file.
	CString strAVIPath = m_szProgramPath;
   strAVIPath += "\\lecturnity_intro.avi";
	m_mciWnd.Open(strAVIPath);
   m_mciWnd.SetTimeFormat("ms");
	// Play the opened AVI file.

   
   RECT parentRect;
   GetWindowRect(&parentRect);
   RECT clientRect;
   GetClientRect(&clientRect);
   int cx = 800 + (parentRect.right - clientRect.right);
   int cy = 440 + (parentRect.bottom - clientRect.bottom);
   SetWindowPos(NULL, 0, 0, cx, cy, NULL);

   CRgn hRgn;
   hRgn.CreateRectRgnIndirect(CRect(0, 0, 27, 28));
	m_button[4].Create(_T(""), WS_VISIBLE | WS_CHILD, CRect(765, 402, 765+27, 402+28), this, IDC_CANCEL, IDB_CANCEL, 27, 28, 3, hRgn);
   hRgn.DeleteObject();

#ifdef _ORDER_BUTTON
   hRgn.CreateRectRgnIndirect(CRect(0, 0, 132, 28));
	m_button[5].Create(_T(""), WS_VISIBLE | WS_CHILD, CRect(622, 402, 622+132, 402+28), this, IDC_ORDER, IDB_ORDER, 132, 28, 3, hRgn);
   hRgn.DeleteObject();
#endif

   m_hideBitmap.LoadBitmap(IDB_WHITE);
   m_hideRectangle.Create(NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, CRect(500, 20, 800, 90), this);
   m_hideRectangle.SetBitmap(m_hideBitmap);
 
   InitButtons();

   CString audioName;
   audioName = m_szProgramPath;
   audioName += "\\intro-lang.wav";

   if (!CheckAudioReplay((LPCTSTR)audioName))
   {
      // Failed to open the file. Use the fallback file.
      audioName = m_szProgramPath;
      audioName += "\\intro-lang-fb.wav";
   }

   ReplayAudio(audioName);


   m_introEnd = m_mciWnd.GetEnd();
   m_mciWnd.Play();
   
   
   m_bIntroStopped = false;

   SetTimer(12345, m_introEnd, NULL);

   return TRUE;
}

BOOL CCdStart_doItDlg::InitButtons()
{
   int iArraySize = m_bUse4Buttons ? 4 : 3;
   
   CRgn hRgn;
   hRgn.CreateRectRgnIndirect(CRect(0, 0, 200, 135));

   int idx = 0;
   int *bitmapId = new int[iArraySize];
   bitmapId[idx++] = IDB_CDDEMO;
   if (m_bUse4Buttons)
      bitmapId[idx++] = IDB_TUTORIAL;
   bitmapId[idx++] = IDB_INSTALL;
   bitmapId[idx++] = IDB_SYSREQ;

   idx = 0;
   int *commandId = new int[iArraySize];
   commandId[idx++] = IDC_CDDEMO;
   if (m_bUse4Buttons)
      commandId[idx++] = IDC_TUTORIAL;
   commandId[idx++] = IDC_INSTALL;
   commandId[idx++] = IDC_SYSREQ;
  
   idx = 0;
   int *titleBitmap = new int[iArraySize];
   titleBitmap[idx++] = IDB_TITLE_CDDEMO;
   if (m_bUse4Buttons)
      titleBitmap[idx++] = IDB_TITLE_TUTORIAL;
   titleBitmap[idx++] = IDB_TITLE_INSTALL;
   titleBitmap[idx++] = IDB_TITLE_SYSREQ;
   
   int buttonX = 0;
   for (int i = 0; i < iArraySize; ++i)
   {
      BOOL bCreate1 = m_button[i].Create(_T(""), WS_CHILD, 
         CRect(buttonX, 130, buttonX+200, 265), this, 
         commandId[i], bitmapId[i], 200, 135, 3, hRgn);
      BOOL bCreate2 = m_title[i].Create(WS_CHILD, 
         CRect(buttonX, 107, buttonX+200, 130), this, 0, 2, titleBitmap[i]);
      
      buttonX += 200;
   }

   hRgn.DeleteObject();

   m_explanationText.Create(WS_VISIBLE | WS_CHILD, CRect(160, 300, 607, 412), this, 0xffff, 7, IDB_TEXT_BP, NULL);
   m_explanationText.SetImageNr(6);

   // Crete popup for Tutorial
   m_tutorialPopup.LoadMenu(IDR_TUTORIAL_POPUP);

   delete[] bitmapId;
   delete[] commandId;
   delete[] titleBitmap;

   return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CCdStart_doItDlg::OnPaint() 
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
      CPaintDC dc(this);
      CRect rect;
      GetClientRect(&rect);
      m_backgroundPicture.Render(&dc, &rect, &rect);
/*
      CRect rect;
      GetClientRect(&rect);
      CBrush white(RGB(255, 255, 255));
      dc.FillRect(&rect, &white);
	   CDialog::OnPaint();
*/
	}
   
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CCdStart_doItDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CCdStart_doItDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return  CDialog::Create(IDD, pParentWnd);
}

BOOL CCdStart_doItDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CDialog::PreCreateWindow(cs);
}

void CCdStart_doItDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


LRESULT CCdStart_doItDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
   // setting or resetting mouse over images and the explanation text images

	if (m_bIntroStopped)
   {
	   if (message == WM_USER)
      {
         int i = 0;
         for (i=0; i<5; ++i)
         {
            if (m_button[i].GetDlgCtrlID() == wParam)
            {
               m_button[i].SetMouseOver(true);
               m_title[i].SetImageNr(1);
            }
            else
            {
               m_button[i].SetMouseOver(false);
               m_title[i].SetImageNr(0);
            }
         }

         if (wParam == IDC_CDDEMO)
            m_explanationText.SetImageNr(ID_EXPLAIN_CDDEMO);
         else if (wParam == IDC_INSTALL)
            m_explanationText.SetImageNr(ID_EXPLAIN_INSTALL);
         else if (wParam == IDC_SYSREQ)
            m_explanationText.SetImageNr(ID_EXPLAIN_SYSREQ);
         else if (wParam == IDC_TUTORIAL)
            m_explanationText.SetImageNr(ID_EXPLAIN_GUIDED);
         else if (wParam == IDC_CANCEL)
            m_explanationText.SetImageNr(ID_EXPLAIN_EXIT);
         else if (wParam == IDC_ORDER)
            m_explanationText.SetImageNr(ID_EXPLAIN_ORDER);
         else
            m_explanationText.SetImageNr(ID_EXPLAIN_EMPTY);
      }
   }
   else if (message == WM_ERASEBKGND)
   {
/*
      CPaintDC dc(this);
      CRect rect;
      GetClientRect(&rect);
      CBrush white(RGB(255, 255, 255));
      CBrush *pOld = dc.SelectObject(&white);
      dc.Rectangle(0, 0, rect.right, rect.bottom);
      dc.SelectObject(pOld);
*/
      return 1;
   }

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CCdStart_doItDlg::OnSysReq() 
{
   CString csHtmlPath;
   csHtmlPath.LoadString(IDS_HTMLPATH);
   CString documentName;
   documentName = m_szProgramPath;
   documentName += csHtmlPath + "systemreq.htm";
	HINSTANCE handle = ShellExecute(NULL, "open", documentName, "", "", SW_SHOWNORMAL);

   if ((int)handle <= 32)
   {
      CString message;
      message.LoadString(IDS_ERR_SYSREQ);
      CString cap;
      cap.LoadString(IDS_ERROR);
      MessageBox (message, cap, MB_OK | MB_ICONEXCLAMATION);
   }
   
}

void CCdStart_doItDlg::OnGuidedTour() 
{
   StopAudio();
 
   /* There were once two tutorials: multimedia and pdf: display popup
   CPoint pos;

   if (GetCursorPos(&pos) == NULL)
   {
      pos.x = 300;
      pos.y = 195;
      ClientToScreen(&pos);
   }

   CMenu *pPopup = m_tutorialPopup.GetSubMenu(0);
   if (pPopup)
      pPopup->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);
   */

   CString csStartPlayer = _T("\"");
   csStartPlayer += m_szProgramPath;
   csStartPlayer += _T("\\startplayer.exe\"");

   CString csDocumentName;
   csDocumentName.LoadStringA(IDS_LPDPATH); // ie. "\docs\Guided_Tour_English.lpd"

   CString csDocumentPath = _T("\"");
   csDocumentPath += m_szProgramPath;
   csDocumentPath += csDocumentName;

   HINSTANCE handle = ShellExecute(NULL, "open", csStartPlayer, csDocumentPath, "", SW_SHOWNORMAL);

   if ((int)handle <= 32)
   {
      CString message;
      message.LoadString(IDS_ERR_GUIDEDTOUR);
      CString cap;
      cap.LoadString(IDS_ERROR);
      MessageBox(message, cap, MB_OK | MB_ICONEXCLAMATION);
   }
}

void CCdStart_doItDlg::OnLecturnityTutorial() 
{

   CString executableName;
   executableName = "\"";;
   executableName += m_szProgramPath;
   executableName += "\\startplayer.exe\"";
 
   CString documentName;
   documentName = "\"";
   documentName += m_szProgramPath;
   documentName += "\\tutorial\\tutorial1_800x660.html\"";
   documentName += " \"";
   documentName += m_szProgramPath;
   documentName += "\\tutorial\\Tutorial.lpd\"";
 

	HINSTANCE handle = ShellExecute(NULL, "open", executableName, documentName, "", SW_SHOWNORMAL);

   if ((int)handle <= 32)
   {
      CString message;
      message.LoadString(IDS_ERR_TUTORIALLECTURNITY);
      CString cap;
      cap.LoadString(IDS_ERROR);
      MessageBox (message, cap, MB_OK | MB_ICONEXCLAMATION);
   }
   
   
}

void CCdStart_doItDlg::OnPdfTutorial() 
{
   CString csPdfPath;
   csPdfPath.LoadString(IDS_PDFPATH);
   CString documentName;
   documentName = m_szProgramPath;
   documentName += csPdfPath + "tutorial.pdf";
	HINSTANCE handle = ShellExecute(NULL, "open", documentName, "", "", SW_SHOWNORMAL);

   if ((int)handle <= 32)
   {
      CString message;
      message.LoadString(IDS_ERR_TUTORIALPDF);
      CString cap;
      cap.LoadString(IDS_ERROR);
      MessageBox (message, cap, MB_OK | MB_ICONEXCLAMATION);
   }
}

void CCdStart_doItDlg::OnInstall() 
{
   CString documentName;
   documentName = m_szProgramPath;
   documentName += "\\setup.exe";
	HINSTANCE handle = ShellExecute(NULL, "open", documentName, "", "", SW_SHOWNORMAL);

   if ((int)handle <= 32)
   {
      CString message;
      message.LoadString(IDS_ERR_INSTALL);
      CString cap;
      cap.LoadString(IDS_ERROR);
      MessageBox (message, cap, MB_OK | MB_ICONEXCLAMATION);
   }

}

void CCdStart_doItDlg::OnCdDemo() 
{
   StopAudio();


   CString executableName;
   executableName = m_szProgramPath;
   executableName += "\\startplayer.exe";
   
   CString documentName;
   documentName = "\"";
   documentName += m_szProgramPath;
   documentName += "\\docs\\index.html\"";
 

	HINSTANCE handle = ShellExecute(NULL, "open", executableName, documentName, "", SW_SHOWNORMAL);

   if ((int)handle <= 32)
   {
      CString message;
      message.LoadString(IDS_ERR_CDDEMO);
      CString cap;
      cap.LoadString(IDS_ERROR);
      MessageBox (message, cap, MB_OK | MB_ICONEXCLAMATION);
   }
}

void CCdStart_doItDlg::OnOrder() 
{
   CString csHtmlPath;
   csHtmlPath.LoadString(IDS_HTMLPATH);
   CString documentName;
   documentName = m_szProgramPath;
   documentName += csHtmlPath + "order.htm";
	HINSTANCE handle = ShellExecute(NULL, "open", documentName, "", "", SW_SHOWNORMAL);

   if ((int)handle <= 32)
   {
      CString message;
      message.LoadString(IDS_ERR_ORDER);
      CString cap;
      cap.LoadString(IDS_ERROR);
      MessageBox (message, cap, MB_OK | MB_ICONEXCLAMATION);
   }
   
}

void CCdStart_doItDlg::OnCancel() 
{
   StopAudio();


   if (!m_bIntroStopped)
      StopIntro();

   CDialog::OnCancel();
}

void CCdStart_doItDlg::StopIntro()
{
   if (!m_bIntroStopped)
   {
      m_bIntroStopped = true;
      KillTimer(12345);
	   m_mciWnd.Stop();
      m_mciWnd.ShowWindow(SW_HIDE);
      m_skipButton.ShowWindow(SW_HIDE);
      m_hideRectangle.ShowWindow(SW_HIDE);

      int iArraySize = m_bUse4Buttons ? 4 : 3;
      for (int i=0; i<iArraySize; ++i)
      {
         m_button[i].ModifyStyle(0, WS_VISIBLE, 0);
         m_title[i].ModifyStyle(0, WS_VISIBLE, 0);
      }

      RedrawWindow();
   }
}

void CCdStart_doItDlg::OnStopIntro() 
{
	CString audioName;
   audioName = m_szProgramPath;
   audioName += "\\intro-kurz.wav";

   if (!CheckAudioReplay((LPCTSTR)audioName))
   {
      // Failed to open the file. Use the fallback file.
      audioName = m_szProgramPath;
      audioName += "\\intro-kurz-fb.wav";
   }

   StopIntro();


   // also stops the already playing sound
   ReplayAudio(audioName);
}

void CCdStart_doItDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 12345)
   {
      StopIntro();
      KillTimer(12345);
      return;
   }

   CDialog::OnTimer(nIDEvent);
}

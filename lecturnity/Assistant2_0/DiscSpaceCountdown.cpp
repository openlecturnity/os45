// DiscSpaceCountdown.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "assistant.h"
#include "DiscSpaceCountdown.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDiscSpaceCountdown 


CDiscSpaceCountdown::CDiscSpaceCountdown(CWnd* pParent /*=NULL*/)
	: CDialog(CDiscSpaceCountdown::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiscSpaceCountdown)
	//}}AFX_DATA_INIT
}


void CDiscSpaceCountdown::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CDiscSpaceCountdown)
   DDX_Control(pDX, IDC_DISCSPACE_COUNTDOWN, m_wndSeconds);
   DDX_Control(pDX, IDC_DISCSPACE_COUNTDOWN_TEXT, m_wndText);
   DDX_Control(pDX, IDC_DISCSPACE_COUNTDOWN_TITLE, m_wndTitle);
   //}}AFX_DATA_MAP
   DDX_Control(pDX, IDC_WARNING_ICON, m_wndWarning);
}


BEGIN_MESSAGE_MAP(CDiscSpaceCountdown, CDialog)
	//{{AFX_MSG_MAP(CDiscSpaceCountdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDiscSpaceCountdown 

BOOL CDiscSpaceCountdown::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD dwIds[] = {IDC_DISCSPACE_COUNTDOWN_TITLE,
                    IDC_DISCSPACE_COUNTDOWN_TEXT,
                    IDC_DISCSPACE_COUNTDOWN,
                    IDC_DISCSPACE_COUNTDOWN_TEXT2,
                   -1};

   MfcUtils::Localize(this, dwIds);

   HICON hIcon = LoadIcon(NULL, IDI_WARNING);
   m_wndWarning.SetIcon(hIcon);

   CString csTitle;
   csTitle.LoadString(IDS_WARNING);
   SetWindowText(csTitle);

   CFont *pWindowFont = XTPPaintManager()->GetRegularFont();
   
   if (pWindowFont != NULL)
   {
      LOGFONT lf;
      pWindowFont->GetLogFont(&lf);
      m_font.DeleteObject();
      lf.lfWeight = FW_BOLD;
      m_font.CreateFontIndirect(&lf);
      m_wndTitle.SetFont(&m_font);
      m_wndSeconds.SetFont(&m_font);
   }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CDiscSpaceCountdown::Update(LPCTSTR szRecordDrive, int iSecondsLeft)
{
   
   CString csText;
   csText.Format(IDC_DISCSPACE_COUNTDOWN_TEXT, szRecordDrive);
   m_wndText.SetWindowText(csText);
   
   csText.Format(IDC_DISCSPACE_COUNTDOWN, iSecondsLeft);
   m_wndSeconds.SetWindowText(csText);

}

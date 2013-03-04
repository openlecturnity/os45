// TwoGBCountdown.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "assistant.h"
#include "TwoGBCountdown.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTwoGBCountdown 


CTwoGBCountdown::CTwoGBCountdown(CWnd* pParent /*=NULL*/)
	: CDialog(CTwoGBCountdown::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTwoGBCountdown)
	//}}AFX_DATA_INIT
}


void CTwoGBCountdown::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTwoGBCountdown)
   DDX_Control(pDX, IDC_2GB_COUNTDOWN_TITLE, m_wndTitle);
   DDX_Control(pDX, IDC_2GB_COUNTDOWN, m_wndSeconds);
   DDX_Control(pDX, IDC_WARNING_ICON, m_wndWarning);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTwoGBCountdown, CDialog)
	//{{AFX_MSG_MAP(CTwoGBCountdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTwoGBCountdown 

BOOL CTwoGBCountdown::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD dwIds[] = {IDC_2GB_COUNTDOWN_TITLE,
                    IDC_2GB_COUNTDOWN_TEXT,
                    IDC_2GB_COUNTDOWN,
                    IDC_2GB_COUNTDOWN_TEXT2,
                   -1};

   MfcUtils::Localize(this, dwIds);

   
   CFont *pWindowFont = XTPPaintManager()->GetRegularFont();

   HICON hIcon = LoadIcon(NULL, IDI_WARNING);
   m_wndWarning.SetIcon(hIcon);

   CString csTitle;
   csTitle.LoadString(IDS_WARNING);
   SetWindowText(csTitle);

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

void CTwoGBCountdown::Update(int iSecondsLeft)
{
   
   CString csText;
   csText.Format(IDC_2GB_COUNTDOWN, iSecondsLeft);
   m_wndSeconds.SetWindowText(csText);

}

// AudioLevelWarning.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "assistant.h"
#include "AudioLevelWarning.h"
#include "MfcUtils.h"   // lsutl32
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAudioLevelWarning 


CAudioLevelWarning::CAudioLevelWarning(CWnd* pParent /*=NULL*/)
	: CDialog(CAudioLevelWarning::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAudioLevelWarning)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CAudioLevelWarning::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioLevelWarning)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAudioLevelWarning, CDialog)
	//{{AFX_MSG_MAP(CAudioLevelWarning)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CAudioLevelWarning 

BOOL CAudioLevelWarning::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   DWORD dwIds[] = {IDC_AUDIO_LEVEL_TEXT,
                    IDHELP,
                    -1};

   MfcUtils::Localize(this, dwIds);

   ModifyStyleEx(NULL, WS_EX_TOPMOST);

   CString csTitle;
   csTitle.LoadString(IDD_AUDIO_LEVEL_WARNING);
   SetWindowText(csTitle);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CAudioLevelWarning::OnHelp() 
{
   if (m_tooltipHelp.m_hWnd == NULL)
      m_tooltipHelp.Create(CHelpInfo::IDD, this);

   if (m_tooltipHelp.IsWindowVisible())
      m_tooltipHelp.ShowWindow(SW_HIDE);
   else
   {
      CRect rcHelpButton;
      CButton *pHelpButton = (CButton *)GetDlgItem(IDHELP);
      pHelpButton->GetWindowRect(&rcHelpButton);
      m_tooltipHelp.SetWindowPos(NULL, rcHelpButton.right+10, rcHelpButton.top - 10, 0, 0, SWP_NOSIZE);
      m_tooltipHelp.ShowWindow(SW_SHOW);
   }
}

void CAudioLevelWarning::OnShowWindow(BOOL bShow, UINT nStatus) 
{
    CDialog::OnShowWindow(bShow, nStatus);

    if (!bShow && 
        m_tooltipHelp.m_hWnd != NULL && 
        m_tooltipHelp.IsWindowVisible())
        m_tooltipHelp.ShowWindow(SW_HIDE);


    CRect rcWarning;
    GetWindowRect(&rcWarning);

    CRect rcMainWindow;
    CWnd *pMainFrame = AfxGetMainWnd();
    if (pMainFrame != NULL && !pMainFrame->IsIconic()) 
        pMainFrame->GetWindowRect(&rcMainWindow);
    else
        MfcUtils::GetDesktopRectangle(rcMainWindow);

    int x = rcMainWindow.left + (rcMainWindow.Width() / 2) - (rcWarning.Width() / 2);
    int y = rcMainWindow.top + (rcMainWindow.Height() / 2) - (rcWarning.Height() / 2);

    MoveWindow(x, y, rcWarning.Width(), rcWarning.Height());
}

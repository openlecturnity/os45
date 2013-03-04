#include "stdafx.h"
#include "ScreengrabbingExtendedProgressDialog.h"
//#include "MainFrm.h"
#include "resource.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CScreengrabbingExtendedProgressDialog 


CScreengrabbingExtendedProgressDialog::CScreengrabbingExtendedProgressDialog(CWnd* pParent, HANDLE hDialogInitEvent, bool bShowLaterButton)
	: CDialog(CScreengrabbingExtendedProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScreengrabbingExtendedProgressDialog)
	//}}AFX_DATA_INIT

   m_totalVideoCount = 0;
   m_currentVideoCount = 0;

   m_hDialogInitEvent = hDialogInitEvent;
   m_bCanceled = false;
   m_bContiniueLater = false;

   m_bShowLaterButton = bShowLaterButton;
}

void CScreengrabbingExtendedProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreengrabbingExtendedProgressDialog)
	DDX_Control(pDX, IDC_EP_PARTICULAR, m_particularLabel);
	DDX_Control(pDX, IDC_EP_OVERALL, m_overallLabel);
	DDX_Control(pDX, IDC_PP_PROGRESS2, m_particularProgress);
	DDX_Control(pDX, IDC_PP_PROGRESS1, m_overallProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreengrabbingExtendedProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CScreengrabbingExtendedProgressDialog)
   ON_MESSAGE(WM_USER, OnCheckEndDialog)
   //ON_MESSAGE(WM_AVEDIT, OnExternalUpdate) // PZI: removed
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_LATER, OnLater)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CScreengrabbingExtendedProgressDialog 

BOOL CScreengrabbingExtendedProgressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   DWORD dwIds[] = {IDC_EP_PARTICULAR,
                    IDC_EP_OVERALL,
                    IDC_CANCEL,
                    IDC_LATER,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_overallProgress.SetRange(0, 100);
   m_particularProgress.SetRange(0, 100);
   m_overallProgress.SetPos(0);
   m_particularProgress.SetPos(0);

   // disable "Continue later" button (used by Publisher)
   if(!m_bShowLaterButton) {
      CButton* pButton = (CButton*) GetDlgItem(IDC_LATER);
      pButton->ShowWindow(SW_HIDE);
   }

   RECT rect;
   GetWindowRect(&rect);
   rect.top -= 200;
   rect.bottom -= 200;
   MoveWindow(&rect);

   if(m_hDialogInitEvent)
      SetEvent(m_hDialogInitEvent);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CScreengrabbingExtendedProgressDialog::SetOverallLabel(LPCTSTR lpText)
{
   if (m_overallLabel.m_hWnd)
      m_overallLabel.SetWindowText(lpText);
}

void CScreengrabbingExtendedProgressDialog::SetOverallLabel(UINT nResourceID)
{
   //CString string;
   m_overallString.LoadString(nResourceID);
   SetOverallLabel(m_overallString);
}

void CScreengrabbingExtendedProgressDialog::SetOverallProgress(int percent)
{
   if (m_overallProgress.m_hWnd)
      m_overallProgress.SetPos(percent);
}

void CScreengrabbingExtendedProgressDialog::SetOverallSteps(int steps)
{
   if (m_overallProgress.m_hWnd)
      m_overallProgress.SetRange(0, steps);
}

void CScreengrabbingExtendedProgressDialog::SetParticularLabel(LPCTSTR lpText)
{
   if (m_particularLabel.m_hWnd)
      m_particularLabel.SetWindowText(lpText);
}

void CScreengrabbingExtendedProgressDialog::SetParticularLabel(UINT nResourceID)
{
   m_particularString.LoadString(nResourceID);
   SetParticularLabel(m_particularString);
}

void CScreengrabbingExtendedProgressDialog::SetParticularProgress(int percent)
{
   if (m_particularProgress.m_hWnd)
      m_particularProgress.SetPos(percent);
}

void CScreengrabbingExtendedProgressDialog::SetParticularSteps(int steps)
{
   if (m_particularProgress.m_hWnd)
      m_particularProgress.SetRange(0, steps);
}

void CScreengrabbingExtendedProgressDialog::IncParticularProgress() {
   if (m_particularProgress.m_hWnd)
      m_particularProgress.OffsetPos(1);
}

void CScreengrabbingExtendedProgressDialog::SetTotalVideoCount(int nCount)
{
   m_totalVideoCount = nCount;
}

void CScreengrabbingExtendedProgressDialog::SetHasVideo(bool bHasVideo)
{
   m_bHasVideo = bHasVideo;
}

LRESULT CScreengrabbingExtendedProgressDialog::OnCheckEndDialog(WPARAM wParam, LPARAM lParam)
{
   if (wParam == END_DIALOG)
      EndDialog(0);

   return 0;
}

LRESULT CScreengrabbingExtendedProgressDialog::OnExternalUpdate(WPARAM wParam, LPARAM lParam)
{
   return 0;
}

void CScreengrabbingExtendedProgressDialog::OnCancel() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   TRACE("############# CANCELLED ######################\n");
   m_bCanceled = true;
}

void CScreengrabbingExtendedProgressDialog::OnLater() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   TRACE("############# CONTINUE LATER ######################\n");
   m_bContiniueLater = true;
}

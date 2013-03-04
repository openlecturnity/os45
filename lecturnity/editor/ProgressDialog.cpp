// ProgressDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ProgressDialog.h"
#include "MainFrm.h"
#include "AVStreams.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Klasse CProgress zur Steuerung von CProgressdialog

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProgressDialogE 


CProgressDialogE::CProgressDialogE(CWnd* pParent /*=NULL*/, CEditorProject *pProject /*=NULL*/)
	: CDialog(CProgressDialogE::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialogE)
	//}}AFX_DATA_INIT

   m_pProject = pProject;
}

void CProgressDialogE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDialogE)
	DDX_Control(pDX, IDC_PROGRESS_LABEL_E, m_progressLabel);
	DDX_Control(pDX, IDC_PROGRESS1_E, m_progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDialogE, CDialog)
	//{{AFX_MSG_MAP(CProgressDialogE)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CProgressDialogE 

void CProgressDialogE::SetLabel(LPCTSTR lpText)
{
   if (m_progressLabel.m_hWnd)
      m_progressLabel.SetWindowText(lpText);
}

void CProgressDialogE::SetLabel(UINT nResourceID)
{
   //CString string;
   m_labelString.LoadString(nResourceID);
   SetLabel(m_labelString);
}


void CProgressDialogE::SetRange(int lower, int upper)
{
   if (IsWindow(m_progress.m_hWnd))
      m_progress.SetRange(lower, upper);
}

void CProgressDialogE::SetStep(int step)
{
   if (IsWindow(m_progress.m_hWnd))
      m_progress.SetStep(step);
}

void CProgressDialogE::SetPos(int pos)
{
   if (IsWindow(m_progress.m_hWnd))
      m_progress.SetPos(pos);
}

void CProgressDialogE::OffsetPos(int offset)
{
   if (IsWindow(m_progress.m_hWnd))
      m_progress.OffsetPos(offset);
}

LRESULT CProgressDialogE::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
   if (message == WM_USER && wParam == END_DIALOG)
   {
      EndWaitCursor();
      EndDialog(0);
   }
   else if (message == WM_AUDIO_PROGRESS)
   {
      m_progress.SetPos((int) lParam);
   }

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CProgressDialogE::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   DWORD dwIds[] = {IDC_PROGRESS_LABEL_E, -1};
   MfcUtils::Localize(this, dwIds);

   /*
   if (m_pProject == NULL) {
       CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
       if (pDoc != NULL)
           m_pProject = &pDoc->project;
   }
   */
   
   if (m_pProject && m_pProject->m_hDialogInitEvent)
      SetEvent(m_pProject->m_hDialogInitEvent);
	
    BeginWaitCursor();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

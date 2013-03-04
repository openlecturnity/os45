#include "stdafx.h"
#include "ExportProgressDialog.h"
#include "editorDoc.h"
#include "MainFrm.h"
#include "..\Studio\resource.h"
#include "avedit.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CExportProgressDialog 


CExportProgressDialog::CExportProgressDialog(CEditorProject *pProject, CWnd* pParent /*=NULL*/)
	: CDialog(CExportProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportProgressDialog)
	//}}AFX_DATA_INIT

    m_pProject = pProject;
    m_totalVideoCount = 0;
    m_currentVideoCount = 0;
    m_bExportCancelled = false;
}


void CExportProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportProgressDialog)
	DDX_Control(pDX, IDC_EP_PARTICULAR_E, m_particularLabel);
	DDX_Control(pDX, IDC_EP_OVERALL_E, m_overallLabel);
	DDX_Control(pDX, IDC_PROGRESS2, m_particularProgress);
	DDX_Control(pDX, IDC_PROGRESS1_E, m_overallProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CExportProgressDialog)
   ON_MESSAGE(WM_USER, OnCheckEndDialog)
   ON_MESSAGE(WM_AVEDIT, OnExternalUpdate)
	ON_BN_CLICKED(IDC_CANCEL_E, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CExportProgressDialog 

BOOL CExportProgressDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();
	
   DWORD dwIds[] = {IDC_EP_PARTICULAR_E,
                    IDC_EP_OVERALL_E,
                    IDC_CANCEL_E,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_overallProgress.SetRange(0, 100);
   m_particularProgress.SetRange(0, 100);
   m_overallProgress.SetPos(0);
   m_particularProgress.SetPos(0);

   //m_overallLabel.SetWindowText("");
   m_particularLabel.SetWindowText(_T(""));

   CEditorProject *pProject = m_pProject;
   if (pProject == NULL) {
       ASSERT(false); // probabyl shouldn't happen
       CEditorDoc *pDoc = (CEditorDoc *) CMainFrameE::GetEditorDocument();
       if (pDoc != NULL)
           pProject = &(pDoc->project);
   }

   if (pProject != NULL)
       SetEvent(pProject->m_hDialogInitEvent);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CExportProgressDialog::SetOverallLabel(LPCTSTR lpText)
{
   if (m_overallLabel.m_hWnd)
      m_overallLabel.SetWindowText(lpText);
}

void CExportProgressDialog::SetOverallLabel(UINT nResourceID)
{
   //CString string;
   m_overallString.LoadString(nResourceID);
   SetOverallLabel(m_overallString);
}

void CExportProgressDialog::SetOverallProgress(int percent)
{
   if (m_overallProgress.m_hWnd)
      m_overallProgress.SetPos(percent);
}

void CExportProgressDialog::SetOverallSteps(int steps)
{
   if (m_overallProgress.m_hWnd)
      m_overallProgress.SetRange(0, steps);
}

void CExportProgressDialog::SetParticularLabel(LPCTSTR lpText)
{
   if (m_particularLabel.m_hWnd)
      m_particularLabel.SetWindowText(lpText);
}

void CExportProgressDialog::SetParticularLabel(UINT nResourceID)
{
   m_particularString.LoadString(nResourceID);
   SetParticularLabel(m_particularString);
}

void CExportProgressDialog::SetParticularProgress(int percent)
{
   if (m_particularProgress.m_hWnd)
      m_particularProgress.SetPos(percent);
}

void CExportProgressDialog::SetTotalVideoCount(int nCount)
{
   m_totalVideoCount = nCount;
}

void CExportProgressDialog::SetHasVideo(bool bHasVideo)
{
   m_bHasVideo = bHasVideo;
}

LRESULT CExportProgressDialog::OnCheckEndDialog(WPARAM wParam, LPARAM lParam)
{
   if (wParam == END_DIALOG)
      EndDialog(0);

   return 0;
}

LRESULT CExportProgressDialog::OnExternalUpdate(WPARAM wParam, LPARAM lParam)
{
   switch (wParam)
   {
   case AVEDIT_NEXTVIDEO:
      {
         int curPos = m_overallProgress.GetPos();
         SetOverallProgress(curPos + 1);
      }
      if (m_currentVideoCount == 0 && m_bHasVideo)
      {
         m_particularString.LoadString(IDS_EXPORT_VIDEO);
         SetParticularLabel(m_particularString);
      }
      else
      {
         m_particularString.Format(IDS_EXPORT_CLIPLABEL, 
            m_currentVideoCount + (m_bHasVideo ? 0 : 1), 
            m_totalVideoCount - (m_bHasVideo? 1 : 0));
         SetParticularLabel(m_particularString);
      }

      m_currentVideoCount++;
      break;

   case AVEDIT_EXPORTPROGRESS:
      SetParticularProgress(lParam);
      break;
   }

   return 0;
}

void CExportProgressDialog::OnCancel() 
{
    if (m_pProject != NULL)
        m_pProject->CancelExport();
    m_bExportCancelled = true;
}

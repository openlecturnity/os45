// SGHandlingDlg.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "SGHandlingDlg.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSGHandlingDlg property page

IMPLEMENT_DYNCREATE(CSGHandlingDlg, CPropertyPage)

CSGHandlingDlg::CSGHandlingDlg() : CPropertyPage(CSGHandlingDlg::IDD)
{
	//{{AFX_DATA_INIT(CSGHandlingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSGHandlingDlg::~CSGHandlingDlg()
{
   m_pAssistantDoc = NULL;
}

void CSGHandlingDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSGHandlingDlg)
	DDX_Control(pDX, IDC_CHECK_SUSPEND_VIDEO, m_chkSuspendVideo);
	DDX_Control(pDX, IDC_CHECK_REDUCE_ASSISTANT, m_chkReduceAssistant);
	DDX_Control(pDX, IDC_CHECK_QUICK_CAPTURE, m_chkQuickCapture);
	DDX_Control(pDX, IDC_CHECK_MINIMIZE_ASSISTANT, m_chkMinimizeAssistant);
	DDX_Control(pDX, IDC_CHECK_DROPPED_FRAMES, m_chkDroppedFrames);
	DDX_Control(pDX, IDC_CHECK_DISPLAY_ACCELERATION, m_chkDisplayAcceleration);
	DDX_Control(pDX, IDC_CHECK_CLEAR_ANNOTATION, m_chkClearAnnotation);
	DDX_Control(pDX, IDC_CHECK_BORDER_WHILE_PANNING, m_chkBorderPanning);
	DDX_Control(pDX, IDC_CHECK_ALWAYS_ON_TOP, m_chkAlvaysOnTop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSGHandlingDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSGHandlingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSGHandlingDlg message handlers

BOOL CSGHandlingDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here
	DWORD dwIds[] = {IDC_CHECK_CLEAR_ANNOTATION,
                    IDC_CHECK_MINIMIZE_ASSISTANT,
                    IDC_CHECK_REDUCE_ASSISTANT,
                    IDC_CHECK_ALWAYS_ON_TOP,
                    IDC_CHECK_DROPPED_FRAMES,
                    IDC_CHECK_BORDER_WHILE_PANNING,          
                    IDC_CHECK_DISPLAY_ACCELERATION,         
                    IDC_CHECK_QUICK_CAPTURE,             
                    IDC_CHECK_SUSPEND_VIDEO,                          
                    -1};
   MfcUtils::Localize(this, dwIds);

   bool bClearAnnotation = true;
   bool bMinimizeAssistant = false;
   bool bReduceAssistant = true;
   bool bAlwaysOnTop = false;
   bool bReportDroppedFrames = false;
   bool bBorderPanning = true;
   bool bDisableAcceleration = false;
   bool bQuickCapture = false;
   bool bSuspendVideo = false;

   if (m_pAssistantDoc)
      m_pAssistantDoc->GetSgHandling(bClearAnnotation, bReportDroppedFrames, bBorderPanning,
                                     bDisableAcceleration, bQuickCapture, bSuspendVideo);

   m_chkClearAnnotation.SetCheck(bClearAnnotation);
   m_chkMinimizeAssistant.SetCheck(bMinimizeAssistant);
   m_chkReduceAssistant.SetCheck(bReduceAssistant);
   m_chkAlvaysOnTop.SetCheck(bAlwaysOnTop);
   m_chkDroppedFrames.SetCheck(bReportDroppedFrames);
   m_chkBorderPanning.SetCheck(bBorderPanning);
   m_chkDisplayAcceleration.SetCheck(bDisableAcceleration);
   m_chkQuickCapture.SetCheck(bQuickCapture);
   m_chkSuspendVideo.SetCheck(bSuspendVideo);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSGHandlingDlg::OnOK() 
{
   bool bClearAnnotation = m_chkClearAnnotation.GetCheck() == 0 ? false: true;
   bool bMinimizeAssistant = m_chkMinimizeAssistant.GetCheck() == 0 ? false: true;
   bool bReduceAssistant = m_chkReduceAssistant.GetCheck() == 0 ? false: true;
   bool bAlwaysOnTop = m_chkAlvaysOnTop.GetCheck() == 0 ? false: true;
   bool bReportDroppedFrames = m_chkDroppedFrames.GetCheck() == 0 ? false: true;
   bool bBorderPanning = m_chkBorderPanning.GetCheck() == 0 ? false: true;
   bool bDisableAcceleration = m_chkDisplayAcceleration.GetCheck() == 0 ? false: true;
   bool bQuickCapture = m_chkQuickCapture.GetCheck() == 0 ? false: true;
   bool bSuspendVideo = m_chkSuspendVideo.GetCheck() == 0 ? false: true;

   if (m_pAssistantDoc)
      m_pAssistantDoc->SetSgHandling(bClearAnnotation, bReportDroppedFrames, bBorderPanning,
                                     bDisableAcceleration, bQuickCapture, bSuspendVideo);

	CPropertyPage::OnOK();
}

void CSGHandlingDlg::Init(CAssistantDoc *pAssistantDoc)
{
   m_pAssistantDoc = pAssistantDoc;
}
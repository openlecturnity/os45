// SGHotkeysDlg.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "SGHotkeysDlg.h"
#include "backend\mlb_types.h"
#include "backend\mlb_misc.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSGHotkeysDlg property page

IMPLEMENT_DYNCREATE(CSGHotkeysDlg, CPropertyPage)

CSGHotkeysDlg::CSGHotkeysDlg() : CPropertyPage(CSGHotkeysDlg::IDD)
{
	//{{AFX_DATA_INIT(CSGHotkeysDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_pAssistantDoc = NULL;
}

CSGHotkeysDlg::~CSGHotkeysDlg()
{
}

void CSGHotkeysDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSGHotkeysDlg)
	DDX_Control(pDX, IDC_COMBO_START, m_cmbStartStop);
	DDX_Control(pDX, IDC_COMBO_REDUCE, m_cmbReduceRestore);
	DDX_Control(pDX, IDC_COMBO_MINIMIZE, m_cmbMinimizeRestore);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSGHotkeysDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSGHotkeysDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSGHotkeysDlg message handlers

BOOL CSGHotkeysDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
   DWORD dwIds[] = {IDC_STATIC_GB_SG_HOTKEYS_START,
                    IDC_STATIC_GB_SG_HOTKEYS_REDUCE,
                    IDC_STATIC_GB_SG_HOTKEYS_MINIMIZE,
                    IDC_STATIC_HOTKEYS_START_CTRL,
                    IDC_STATIC_HOTKEYS_START_SHIFT,
                    IDC_STATIC_HOTKEYS_START_ALT,          
                    IDC_STATIC_HOTKEYS_REDUCE_CTRL,         
                    IDC_STATIC_HOTKEYS_REDUCE_SHIFT,             
                    IDC_STATIC_HOTKEYS_REDUCE_ALT,
                    IDC_STATIC_HOTKEYS_MINIMIZE_CTRL,
                    IDC_STATIC_HOTKEYS_MINIMIZE_SHIFT,
                    IDC_STATIC_HOTKEYS_MINIMIZE_ALT,
                    -1};
   MfcUtils::Localize(this, dwIds);


   CStringArray arPossibleKeys;
   ASSISTANT::GetPossibleKeys(arPossibleKeys);
   
   AVGRABBER::HotKey hkStartStop;
   hkStartStop.vKeyString = arPossibleKeys[0];
   hkStartStop.hasCtrl = false;
   hkStartStop.hasShift = false;
   hkStartStop.hasAlt = false;

   AVGRABBER::HotKey hkReduce;
   hkReduce.vKeyString = arPossibleKeys[0];
   hkReduce.hasCtrl = false;
   hkReduce.hasShift = false;
   hkReduce.hasAlt = false;

   AVGRABBER::HotKey hkMinimize;
   hkMinimize.vKeyString = arPossibleKeys[0];
   hkMinimize.hasCtrl = false;
   hkMinimize.hasShift = false;
   hkMinimize.hasAlt = false;


   if (m_pAssistantDoc)
      m_pAssistantDoc->GetSgHotkeys(hkStartStop, hkReduce, hkMinimize);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->GetSgHotkeys(hkStartStop, hkReduce, hkMinimize);

	for(int i = 0; i < arPossibleKeys.GetSize(); i++)
   {
      m_cmbStartStop.AddString(arPossibleKeys[i]);
      m_cmbReduceRestore.AddString(arPossibleKeys[i]);
      m_cmbMinimizeRestore.AddString(arPossibleKeys[i]);

      if (hkStartStop.vKeyString == arPossibleKeys[i])
         m_cmbStartStop.SetCurSel(i);
      if (hkReduce.vKeyString == arPossibleKeys[i])
         m_cmbReduceRestore.SetCurSel(i);
      if (hkMinimize.vKeyString == arPossibleKeys[i])
         m_cmbMinimizeRestore.SetCurSel(i);
   }

   CButton *pCheckButton = NULL;
   
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_CTRL);
   pCheckButton->SetCheck(hkStartStop.hasCtrl);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_SHIFT);
   pCheckButton->SetCheck(hkStartStop.hasShift);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_ALT);
   pCheckButton->SetCheck(hkStartStop.hasAlt);
   
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_REDUCE_CTRL);
   pCheckButton->SetCheck(hkReduce.hasCtrl);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_REDUCE_SHIFT);
   pCheckButton->SetCheck(hkReduce.hasShift);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_REDUCE_ALT);
   pCheckButton->SetCheck(hkReduce.hasAlt);
    
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_MINIMIZE_CTRL);
   pCheckButton->SetCheck(hkMinimize.hasCtrl);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_MINIMIZE_SHIFT);
   pCheckButton->SetCheck(hkMinimize.hasShift);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_MINIMIZE_ALT);
   pCheckButton->SetCheck(hkMinimize.hasAlt);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSGHotkeysDlg::Init(CAssistantDoc *pAssistantDoc)
{
   m_pAssistantDoc = pAssistantDoc;
}

void CSGHotkeysDlg::OnOK() 
{
   CButton *pCheckButton = NULL;

   AVGRABBER::HotKey hkStartStop;
   m_cmbStartStop.GetWindowText(hkStartStop.vKeyString);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_CTRL);
   hkStartStop.hasCtrl = pCheckButton->GetCheck() == 0 ? false: true;
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_SHIFT);
   hkStartStop.hasShift = pCheckButton->GetCheck() == 0 ? false: true;
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_ALT);
   hkStartStop.hasAlt = pCheckButton->GetCheck() == 0 ? false: true;
   
   AVGRABBER::HotKey hkReduce;
   m_cmbReduceRestore.GetWindowText(hkReduce.vKeyString);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_REDUCE_CTRL);
   hkReduce.hasCtrl = pCheckButton->GetCheck() == 0 ? false: true;
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_REDUCE_SHIFT);
   hkReduce.hasShift = pCheckButton->GetCheck() == 0 ? false: true;
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_REDUCE_ALT);
   hkReduce.hasAlt = pCheckButton->GetCheck() == 0 ? false: true;
   
   AVGRABBER::HotKey hkMinimize;
   m_cmbMinimizeRestore.GetWindowText(hkMinimize.vKeyString);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_MINIMIZE_CTRL);
   hkMinimize.hasCtrl = pCheckButton->GetCheck() == 0 ? false: true;
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_MINIMIZE_SHIFT);
   hkMinimize.hasShift = pCheckButton->GetCheck() == 0 ? false: true;
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_MINIMIZE_ALT);
   hkMinimize.hasAlt = pCheckButton->GetCheck() == 0 ? false: true;
   
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetSgHotkeys(hkStartStop, hkReduce, hkMinimize);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetSgHotkeys(hkStartStop, hkReduce, hkMinimize);

	CPropertyPage::OnOK();
}

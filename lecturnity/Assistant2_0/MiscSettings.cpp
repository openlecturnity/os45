// MiscSettings.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "..\Studio\Studio.h"
#endif
#include "MiscSettings.h"
#include "MainFrm.h"
#include "MfcUtils.h"
#include "backend\mlb_misc.h"
#include "BrowseFolder\XFolderDialog.h"


// CMiscSettings dialog

IMPLEMENT_DYNAMIC(CMiscSettings, CXTPPropertyPage)

CMiscSettings::CMiscSettings()
	: CXTPPropertyPage(CMiscSettings::IDD)
{
   m_bCheckStartupInit = false;
   m_bCheckFontInit = false;
   m_bCheckAudioInit = false;
   m_bCheckImportInit = false;
   m_bCheckSmartBoardPenInit = false;
   m_bCheckSwitchByStop = true;
   m_nCurComboSel = 0;
}

CMiscSettings::~CMiscSettings()
{
}

void CMiscSettings::DoDataExchange(CDataExchange* pDX)
{
	CXTPPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_MISC_COMBO_COLOR, m_cmbColor);
}


BEGIN_MESSAGE_MAP(CMiscSettings, CXTPPropertyPage)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
   ON_COMMAND(IDC_MISC_BUTTON_SEL_HOME, OnSelectHome)
    ON_UPDATE_COMMAND_UI(IDC_MISC_BUTTON_SEL_HOME, OnUpdateSelectHome)    

END_MESSAGE_MAP()


// CMiscSettings message handlers

BOOL CMiscSettings::OnInitDialog()
{
   CXTPPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   DWORD dwIds[] = {IDC_MISC_STATIC_GROUP_COLOR,
                    IDC_MISC_STATIC_LABEL_COLOR,
                    IDC_MISC_STATIC_GROUP_SHOW,
                    IDC_MISC_CHECK_STARTUP,
                    IDC_MISC_CHECK_FONT,
                    IDC_MISC_CHECK_AUDIO,
                    IDC_MISC_CHECK_IMPORT,
                    IDC_MISC_CHECK_SMARTBOARD_PEN,
                    IDC_MISC_STATIC_GROUP_UTILS,
                    IDC_MISC_BUTTON_SEL_HOME,
                    IDC_MISC_CHECK_USE_SMARTBOARD,
                    IDC_MISC_STATIC_GROUP_TITLE,
                    IDC_MISC_CHECK_SWITCHBYSTOP,
                   -1};

   MfcUtils::Localize(this, dwIds);

   Init();

   
   m_caption[0].SubclassDlgItem(IDC_MISC_STATIC_GROUP_COLOR, this);
   m_caption[1].SubclassDlgItem(IDC_MISC_STATIC_GROUP_SHOW, this);
   m_caption[2].SubclassDlgItem(IDC_MISC_STATIC_GROUP_UTILS, this);
   m_caption[3].SubclassDlgItem(IDC_MISC_STATIC_GROUP_TITLE, this);

   SetResize(IDC_MISC_STATIC_GROUP_COLOR, SZ_TOP_LEFT, SZ_TOP_RIGHT);
   SetResize(IDC_MISC_STATIC_GROUP_SHOW, SZ_TOP_LEFT, SZ_TOP_RIGHT);
   SetResize(IDC_MISC_STATIC_GROUP_UTILS, SZ_TOP_LEFT, SZ_TOP_RIGHT);
   SetResize(IDC_MISC_STATIC_GROUP_TITLE, SZ_TOP_LEFT, SZ_TOP_RIGHT);
    m_caption[3].SetBold();
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CMiscSettings::OnOK()
{
   // TODO: Add your specialized code here and/or call the base class
#ifndef _STUDIO   
   CMainFrameA* pMainFrame = CMainFrameA::GetCurrentInstance();
   pMainFrame->UpdateStatusDiskSpace();
#else
   SendMessage(WM_COMMAND, ID_STATUS_DISKSPACE);
#endif   
   SaveSettings();
   CXTPPropertyPage::OnOK();
}

void CMiscSettings::OnKickIdle()
{
   UpdateDialogControls(this, false);
}

void CMiscSettings::Init()
{
#ifndef _STUDIO
   CAssistantApp* pApp = (CAssistantApp*)AfxGetApp();
#else
   CStudioApp* pApp = (CStudioApp*)AfxGetApp();
#endif
   
#ifdef _STUDIO
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
#endif

   CButton* pControl;

   m_nCurComboSel = pApp->GetCurrentColorScheme();

   for(int i = 0; i < 4; i++)
   {
      m_caption[i].SetScheme(m_nCurComboSel);
     m_caption[i].SetTitleAlign(xtpTitleAlignLeft);
     m_caption[i].SetCaptionType(xtpCaptionNormal);
   }
 
   m_caption[3].SetTitleAlign(xtpTitleAlignCustom);
   m_caption[3].SetCaptionType(xtpCaptionHeader);
   m_caption[3].SetIconID(xtpGeneral);
   
  
   CString csStr;
   csStr.LoadString(IDS_MISC_COLOR_BLUE);
   m_cmbColor.AddString(csStr);
   csStr.LoadString(IDS_MISC_COLOR_SILVER);
   m_cmbColor.AddString(csStr);
   csStr.LoadString(IDS_MISC_COLOR_BLACK);
   m_cmbColor.AddString(csStr);
   csStr.LoadString(IDS_MISC_COLOR_AQUA);
   m_cmbColor.AddString(csStr);
   m_cmbColor.SetCurSel(m_nCurComboSel);

#ifndef _STUDIO
   m_bCheckStartupInit = pApp->GetShowStartupPage();
#else
   if (pMainFrame)
       m_bCheckStartupInit = pMainFrame->GetShowStartupPage();
   else
       m_bCheckStartupInit = true;
#endif
   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_STARTUP);
   pControl->SetCheck(m_bCheckStartupInit);

#ifndef _STUDIO
   m_bCheckFontInit = pApp->GetShowMissingFontsWarning();
#else
   if (pMainFrame)
       m_bCheckFontInit = pMainFrame->GetShowMissingFontsWarning();
   else
       m_bCheckFontInit = true;
#endif
   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_FONT);
   pControl->SetCheck(m_bCheckFontInit);

#ifndef _STUDIO
   m_bCheckAudioInit = pApp->GetShowAudioWarning();
#else
   if (pMainFrame)
       m_bCheckAudioInit = pMainFrame->GetShowAudioWarning();
   else
       m_bCheckAudioInit = true;
#endif
   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_AUDIO);
   pControl->SetCheck(m_bCheckAudioInit);

#ifndef _STUDIO
   m_bCheckImportInit = pApp->GetShowImportWarning();
#else
   if (pMainFrame)
       m_bCheckImportInit = pMainFrame->GetShowImportWarning();
   else
       m_bCheckImportInit = true;
#endif

   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_IMPORT);
   pControl->SetCheck(m_bCheckImportInit);

#ifndef _STUDIO
   m_bCheckSmartBoardPenInit = pApp->GetShowUseSmartBoard();
#else
   if (pMainFrame)
      m_bCheckSmartBoardPenInit = pMainFrame->GetShowUseSmartBoard();
   else
      m_bCheckSmartBoardPenInit = true;;
#endif
   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_SMARTBOARD_PEN);
   pControl->SetCheck(m_bCheckSmartBoardPenInit);

#ifdef _STUDIO
   if (pMainFrame)
       m_bCheckSwitchByStop = pMainFrame->GetSwitchAutomaticallyToEdit();
   else
       m_bCheckSwitchByStop = true;
#endif
   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_SWITCHBYSTOP);
   pControl->SetCheck(m_bCheckSwitchByStop);

   bool bEnableUseSmartboard = CMainFrameA::GetCurrentInstance()->IsSmartboardUsable();
   bool bCheckUseSmartboard = CMainFrameA::GetCurrentInstance()->IsSmartboardAttached();
   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_USE_SMARTBOARD);
   pControl->SetCheck(bCheckUseSmartboard);
   pControl->EnableWindow(bEnableUseSmartboard);
}

void CMiscSettings::SaveSettings()
{
   CButton* pControl;

   if(m_nCurComboSel != m_cmbColor.GetCurSel())
   {
#ifndef _STUDIO
      CAssistantApp* pApp = (CAssistantApp*)AfxGetApp();
#else
      CStudioApp* pApp = (CStudioApp*)AfxGetApp();
#endif
      pApp->SetCurrentColorScheme(m_cmbColor.GetCurSel());
   }
   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_FONT);
   if(m_bCheckFontInit != pControl->GetCheck())
      SendMessage(WM_COMMAND, ID_SHOW_MISSING_FONTS);

   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_AUDIO);
   if(m_bCheckAudioInit != pControl->GetCheck())
      SendMessage(WM_COMMAND, ID_SHOW_AUDIO_WARNING);

   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_IMPORT);
   if(m_bCheckImportInit != pControl->GetCheck())
      SendMessage(WM_COMMAND, ID_SHOW_IMPORT_WARNINGS);

   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_SMARTBOARD_PEN);
   if(m_bCheckSmartBoardPenInit != pControl->GetCheck())
      SendMessage(WM_COMMAND, ID_SHOW_USESMARTBOARD);

   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_STARTUP);
   if(m_bCheckStartupInit != pControl->GetCheck())
      SendMessage(WM_COMMAND, IDC_SHOW_STARTPAGE);

   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_SWITCHBYSTOP);
#ifdef _STUDIO
   BOOL bCheckSwitchByStop = pControl->GetCheck() == BST_CHECKED ? TRUE : FALSE;
   if (m_bCheckSwitchByStop != bCheckSwitchByStop) {
       CAssistantDoc *pAssistantDoc = CMainFrameA::GetAssistantDocument();
       if (pAssistantDoc)
           pAssistantDoc->SetSwitchAutomaticallyToEdit(bCheckSwitchByStop == TRUE ? true : false);
   }
   if (CMainFrameA::GetCurrentInstance() && !m_csNewLecturnityHome.IsEmpty()) {
       CMainFrameA::GetCurrentInstance()->SaveLecturnityHome(m_csNewLecturnityHome);
   }
#endif

   pControl = (CButton*)GetDlgItem(IDC_MISC_CHECK_USE_SMARTBOARD);
   if(pControl->GetCheck())
      SendMessage(WM_COMMAND, ID_USE_SMARTBOARD);
}

void CMiscSettings::OnSelectHome() {
#ifndef _STUDIO
    CMainFrameA* pMainFrame = CMainFrameA::GetCurrentInstance();
    pMainFrame->UpdateStatusDiskSpace();
#else
    SendMessage(WM_COMMAND, ID_STATUS_DISKSPACE);
#endif

    bool bResult = ASSISTANT::GetLecturnityHome(m_csNewLecturnityHome);
    if (!bResult) {
        m_csNewLecturnityHome = _T("");
    }

    CXFolderDialog fDlg(m_csNewLecturnityHome);
    int nOsVesrion = 0;

    fDlg.SetOsVersion((CXFolderDialog::XFILEDIALOG_OS_VERSION)nOsVesrion);

    CString csTitle;
    csTitle.LoadString(IDS_SELECT_LEC_HOME);
    fDlg.SetTitle(csTitle);

    if (fDlg.DoModal() == IDOK) {
        m_csNewLecturnityHome = fDlg.GetPath();
    } else {
        m_csNewLecturnityHome = _T("");
    }
}

void CMiscSettings::OnUpdateSelectHome(CCmdUI* pCmdUI) {
    BOOL bEnable = TRUE;

    if (CStudioApp::IsPowerTrainerMode())
        bEnable = FALSE;

#ifdef _STUDIO
    if (CStudioApp::IsLiveContextMode()) { 
        bEnable = FALSE;
    }
#endif

    pCmdUI->Enable(bEnable);
}

// SGSettingsSheet.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "..\Studio\Studio.h"
#endif
#include "SGSettingsSheet.h"
#include "backend\la_project.h"
#include "MfcUtils.h"


// CSGSettingsSheet dialog

IMPLEMENT_DYNAMIC(CSGSettingsSheet, CXTPPropertyPage)

CSGSettingsSheet::CSGSettingsSheet()
	: CXTPPropertyPage(CSGSettingsSheet::IDD)
{
   m_pAssistantDoc = NULL;
}

CSGSettingsSheet::~CSGSettingsSheet()
{
}

void CSGSettingsSheet::DoDataExchange(CDataExchange* pDX)
{
	CXTPPropertyPage::DoDataExchange(pDX);
   //General variables begin
   DDX_Control(pDX, IDC_STATIC_FIXED_WIDTH, m_stFixedWidth);
	DDX_Control(pDX, IDC_STATIC_FR_ADAPTIVE_WIDTH, m_stAdaptiveWidth);
	DDX_Control(pDX, IDC_STATIC_FR_ADAPTIVE_HEIGHT, m_stAdaptiveHeight);
	DDX_Control(pDX, IDC_STATIC_FIXED_REG_Y, m_stFixedY);
	DDX_Control(pDX, IDC_STATIC_FIXED_REG_X, m_stFixedX);
	DDX_Control(pDX, IDC_STATIC_FIXED_REG_HEIGHT, m_stFixedHeight);
	DDX_Control(pDX, IDC_SPIN_FR_STATIC, m_spnFrStatic);
	DDX_Control(pDX, IDC_SPIN_FR_ADAPTIVE, m_spnFrAdaptive);
	DDX_Control(pDX, IDC_EDIT_FR_STATIC, m_edtFrStatic);
	DDX_Control(pDX, IDC_EDIT_FR_ADAPTIVE_WIDTH, m_edtFrAdaptiveWidth);
	DDX_Control(pDX, IDC_EDIT_FR_ADAPTIVE_HEIGHT, m_edtFrAdaptiveHeight);
	DDX_Control(pDX, IDC_EDIT_FR_ADAPTIVE, m_edtFrAdaptive);
	DDX_Control(pDX, IDC_EDIT_FIXED_Y, m_edtFixedY);
	DDX_Control(pDX, IDC_EDIT_FIXED_X, m_edtFixedX);
	DDX_Control(pDX, IDC_EDIT_FIXED_WIDTH, m_edtFixedWidth);
	DDX_Control(pDX, IDC_EDIT_FIXED_HEIGHT, m_edtFixedHeight);
	DDX_Control(pDX, IDC_CHECK_MOUSE_VISUAL, m_chkMouseVisual);
	DDX_Control(pDX, IDC_CHECK_MOUSE_ACOUSTIC, m_chkMouseAcoustic);
	DDX_Control(pDX, IDC_CHECK_FIXED_REGION, m_chkFixedRegion);
   //General variables end
   //Handling variables begin
   DDX_Control(pDX, IDC_CHECK_SUSPEND_VIDEO, m_chkSuspendVideo);
	DDX_Control(pDX, IDC_CHECK_QUICK_CAPTURE, m_chkQuickCapture);
	DDX_Control(pDX, IDC_CHECK_DROPPED_FRAMES, m_chkDroppedFrames);
	DDX_Control(pDX, IDC_CHECK_DISPLAY_ACCELERATION, m_chkDisplayAcceleration);
	DDX_Control(pDX, IDC_CHECK_CLEAR_ANNOTATION, m_chkClearAnnotation);
	DDX_Control(pDX, IDC_CHECK_BORDER_WHILE_PANNING, m_chkBorderPanning);
   //Handling variables end
   //Hotkeys variables begin
   DDX_Control(pDX, IDC_COMBO_START, m_cmbStartStop);
   //Hotkeys variables end
   //Codec variables begin
   DDX_Control(pDX, IDC_STATIC_SG_FRAMES, m_stFrames);
	DDX_Control(pDX, IDC_STATIC_SG_DISTANCE, m_stDistance);
	DDX_Control(pDX, IDC_STATIC_SG_ADAPTION_KB, m_stDatarateKb);
	DDX_Control(pDX, IDC_STATIC_SG_DATARATE, m_stDatarate);
	DDX_Control(pDX, IDC_SPIN_SG_QUALITY, m_spnQuality);
	DDX_Control(pDX, IDC_SLIDER_SG_QUALITY, m_sldQuality);
	DDX_Control(pDX, IDC_EDIT_SG_QUALITY, m_edtQuality);
	DDX_Control(pDX, IDC_EDIT_SG_KEYFRAME, m_edtKeyframe);
	DDX_Control(pDX, IDC_EDIT_SG_DATARATE, m_edtDatarate);
	DDX_Control(pDX, IDC_COMBO_SG_CODEC, m_cmbSgCodec);
	DDX_Control(pDX, IDC_CHECK_SG_KEYFRAME, m_chkKeyframe);
	DDX_Control(pDX, IDC_CHECK_SG_DATARATE, m_chkDatarate);
	DDX_Control(pDX, IDC_BUTTON_SG_CONFGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_BUTTON_SG_CODEC_ABOUT, m_btnAbout);
   //Codec variables end
}

BEGIN_MESSAGE_MAP(CSGSettingsSheet, CXTPPropertyPage)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
   //General Messages begin
   ON_BN_CLICKED(IDC_RADIO_FRAMERATE_STATIC, OnRadioFramerateStatic)
	ON_BN_CLICKED(IDC_RADIO_FRAMERATE_ADAPTIVE, OnRadioFramerateAdaptive)
	ON_BN_CLICKED(IDC_CHECK_FIXED_REGION, OnCheckFixedRegion)
	ON_EN_UPDATE(IDC_EDIT_FR_ADAPTIVE, OnUpdateEditFrAdaptive)
	ON_EN_UPDATE(IDC_EDIT_FR_STATIC, OnUpdateEditFrStatic)
	ON_EN_UPDATE(IDC_EDIT_FIXED_HEIGHT, OnUpdateCheckFixedRegion)
   //General Messages begin
   //Codec Messages begin
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SG_QUALITY, OnDeltaposSpinSgQuality)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SG_QUALITY, OnCustomdrawSliderSgQuality)
	ON_BN_CLICKED(IDC_CHECK_SG_KEYFRAME, OnCheckSgKeyframe)
	ON_BN_CLICKED(IDC_CHECK_SG_DATARATE, OnCheckSgDatarate)
	ON_BN_CLICKED(IDC_BUTTON_SG_CODEC_ABOUT, OnButtonSgCodecAbout)
	ON_BN_CLICKED(IDC_BUTTON_SG_CONFGURE, OnButtonSgConfgure)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_SG_CONFGURE, OnUpdateButtonCodecConfigure)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_SG_CODEC_ABOUT, OnUpdateButtonCodecAbout)
	ON_CBN_SELCHANGE(IDC_COMBO_SG_CODEC, OnSelchangeComboSgCodec)
   //Codec Messages begin
END_MESSAGE_MAP()


// CSGSettingsSheet message handlers
void CSGSettingsSheet::Init(CAssistantDoc* pDocument)
{
    m_pAssistantDoc = pDocument;
}

BOOL CSGSettingsSheet::OnInitDialog()
{
   CXTPPropertyPage::OnInitDialog();

   ModifyStyle(0, /*WS_CLIPSIBLINGS*//*WS_CLIPCHILDREN | */WS_VSCROLL);
   LocalizeGeneral();
   InitGeneral();

   LocalizeHandling();
   InitHandling();

   LocalizeHotkeys();
   InitHotkeys();

   LocalizeCodec();
   InitCodec();
   
#ifdef _STUDIO
   CStudioApp* pApp = (CStudioApp*)AfxGetApp();
#else
   CAssistantApp* pApp = (CAssistantApp*)AfxGetApp();
#endif
   UINT nColorScheme = pApp->GetCurrentColorScheme();

   // Page title
   CString csStr;
   csStr.LoadString(IDC_SGB_STATIC_TITLE);
  
   m_caption[0].SetScheme(nColorScheme);
   m_caption[0].SetTitleAlign(xtpTitleAlignCustom);
   m_caption[0].SetCaptionType(xtpCaptionHeader);
   m_caption[0].SetIconID(xtpScreenGrabbing);
   m_caption[0].SubclassDlgItem(IDC_SGB_STATIC_TITLE, this);

   if (!csStr.IsEmpty())
       m_caption[0].SetCaption(csStr);
   m_caption[0].SetBold();
   SetResize(IDC_SGB_STATIC_TITLE, SZ_TOP_LEFT, SZ_TOP_RIGHT);

   // Sub titles
   for (int i = 1; i < 5; i++) {
       m_caption[i].SetScheme(nColorScheme);
       m_caption[i].SetTitleAlign(xtpTitleAlignLeft);
       m_caption[i].SetCaptionType(xtpCaptionNormal);
   }

   m_caption[1].SubclassDlgItem(IDC_SGB_STATIC_GENERAL_H, this);
   SetResize(IDC_SGB_STATIC_GENERAL_H, SZ_TOP_LEFT, SZ_TOP_RIGHT);

   m_caption[2].SubclassDlgItem(IDC_SGB_STATIC_CLICKINFO, this);
   SetResize(IDC_SGB_STATIC_CLICKINFO, SZ_TOP_LEFT, SZ_TOP_RIGHT);

   m_caption[3].SubclassDlgItem(IDC_SGB_STATIC_CODEC, this);
   SetResize(IDC_SGB_STATIC_CODEC, SZ_TOP_LEFT, SZ_TOP_RIGHT);

   m_caption[4].SubclassDlgItem(IDC_SGB_STATIC_HOTKEYS, this);
   SetResize(IDC_SGB_STATIC_HOTKEYS, SZ_TOP_LEFT, SZ_TOP_RIGHT);

   
   ::GetFocus();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CSGSettingsSheet::OnKickIdle()
{
   UpdateDialogControls(this, false);
}

void CSGSettingsSheet::OnOK()
{
   // TODO: Add your specialized code here and/or call the base class

   SaveSettingsGeneral();
   SaveSettingsHandling();
   SaveSettingsHotkeys();
   SaveSettingsCodec();
   CXTPPropertyPage::OnOK();
}

void CSGSettingsSheet::OnCancel() {
    if (ASSISTANT::Project::active != NULL) {
        CString csCodec = m_arCodecList[m_nInitialCodexIndex];
        ASSISTANT::Project::active->SetSgCodec(csCodec, false);
        m_bHasAbout = ASSISTANT::Project::active->SgCodecHasAbout();
        m_bHasConfigure = ASSISTANT::Project::active->SgCodecCanBeConfigured();
    }
    CXTPPropertyPage::OnCancel();
}

BOOL CSGSettingsSheet::OnSetActive()
{
   // TODO: Add your specialized code here and/or call the base class
   CWnd* pControl = GetDlgItem(IDC_CHECK_MOUSE_VISUAL);
   pControl->SetFocus();

   return CXTPPropertyPage::OnSetActive();
}
//General methods begin
void CSGSettingsSheet::LocalizeGeneral()
{
   DWORD dwIds[] = {IDC_STATIC_SG_GB_SETTINGS_MOUSE,
                    IDC_STATIC_SG_GB_SETTINGS_FRAMERATE,
                    IDC_STATIC_SG_GB_REGION,
                    IDC_CHECK_MOUSE_VISUAL,
                    IDC_CHECK_MOUSE_ACOUSTIC,
                    IDC_RADIO_FRAMERATE_STATIC,          
                    IDC_RADIO_FRAMERATE_ADAPTIVE,         
                    IDC_CHECK_FIXED_REGION,             
                    IDC_STATIC_FR_ADAPTIVE_WIDTH,                          
                    IDC_STATIC_FR_ADAPTIVE_HEIGHT,         
                    IDC_STATIC_FIXED_REG_X,
                    IDC_STATIC_FIXED_REG_Y,
                    IDC_STATIC_FIXED_REG_HEIGHT,
                    IDC_STATIC_FIXED_WIDTH,
                    IDC_SGB_STATIC_GENERAL_H,
                    -1};
   MfcUtils::Localize(this, dwIds);
}

void CSGSettingsSheet::InitGeneral()
{
   bool bMouseVisual = true;
   bool bMouseAcoustic = true;
   if (m_pAssistantDoc)
      m_pAssistantDoc->GetMouseEffects(bMouseVisual, bMouseAcoustic);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->GetMouseEffects(bMouseVisual, bMouseAcoustic);
   m_chkMouseVisual.SetCheck(bMouseVisual);
   m_chkMouseAcoustic.SetCheck(bMouseAcoustic);

   bool bStaticFramerate = true;
   float fStaticFramerate = 10;
   float fAdaptiveFramerate = 10;
   UINT nWidth = 320;
   UINT nHeight = 240;
   if (m_pAssistantDoc)
      m_pAssistantDoc->GetFramerateSettings(bStaticFramerate, fStaticFramerate, 
                                            fAdaptiveFramerate, nWidth, nHeight);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->GetFramerateSettings(bStaticFramerate, fStaticFramerate, 
                                            fAdaptiveFramerate, nWidth, nHeight);
      
   if (bStaticFramerate)
   {
      CButton* radioStatic = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_STATIC);
      radioStatic->SetCheck(bStaticFramerate);
   }
   else
   {
      CButton* radioAdaptive = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_ADAPTIVE);
      radioAdaptive->SetCheck(!bStaticFramerate);
   }

	m_spnFrStatic.SetRange(0, 30);
   m_spnFrStatic.SetBuddy(&m_edtFrStatic);
   m_spnFrStatic.SetPos((int)fStaticFramerate);

   m_spnFrAdaptive.SetRange(0, 30);
   m_spnFrAdaptive.SetBuddy(&m_edtFrAdaptive);
   m_spnFrAdaptive.SetPos((int)fAdaptiveFramerate);

   m_edtFrStatic.LimitText(2);
   m_edtFrAdaptive.LimitText(2);

   CString csWindow;
   csWindow.Format(_T("%d"), nWidth);
	m_edtFrAdaptiveWidth.SetWindowText(csWindow);
   csWindow.Format(_T("%d"), nHeight);
	m_edtFrAdaptiveHeight.SetWindowText(csWindow);

   bool bRegionFixed = false;
   UINT nFixedX = 0;
   UINT nFixedY = 0;
   UINT nFixedWidth = 320;
   UINT nFixedHeight = 240;
   if (m_pAssistantDoc)
      m_pAssistantDoc->GetRegionSettings(bRegionFixed, nFixedX, nFixedY, nFixedWidth, nFixedHeight);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->GetRegionSettings(bRegionFixed, nFixedX, nFixedY, nFixedWidth, nFixedHeight);
   m_chkFixedRegion.SetCheck(bRegionFixed);

   csWindow.Format(_T("%d"), nFixedX);
   m_edtFixedX.SetWindowText(csWindow);
   csWindow.Format(_T("%d"), nFixedY);
   m_edtFixedY.SetWindowText(csWindow);
   csWindow.Format(_T("%d"), nFixedWidth);
   m_edtFixedWidth.SetWindowText(csWindow);
   csWindow.Format(_T("%d"), nFixedHeight);
   m_edtFixedHeight.SetWindowText(csWindow);
}

void CSGSettingsSheet::OnRadioFramerateStatic() 
{
	// TODO: Add your control notification handler code here
	m_edtFrAdaptive.EnableWindow(false);
   m_stAdaptiveWidth.EnableWindow(false);
   m_edtFrAdaptiveWidth.EnableWindow(false);
   m_stAdaptiveHeight.EnableWindow(false);
   m_edtFrAdaptiveHeight.EnableWindow(false);
   m_spnFrAdaptive.EnableWindow(false);

   m_spnFrStatic.EnableWindow(true);
   m_edtFrStatic.EnableWindow(true);
}

void CSGSettingsSheet::OnRadioFramerateAdaptive() 
{
	// TODO: Add your control notification handler code here
   m_spnFrStatic.EnableWindow(false);
	m_edtFrStatic.EnableWindow(false);

   m_edtFrAdaptive.EnableWindow(true);
   m_stAdaptiveWidth.EnableWindow(true);
   m_edtFrAdaptiveWidth.EnableWindow(true);
   m_stAdaptiveHeight.EnableWindow(true);
   m_edtFrAdaptiveHeight.EnableWindow(true);
   m_spnFrAdaptive.EnableWindow(true);
}

void CSGSettingsSheet::OnCheckFixedRegion() 
{
	// TODO: Add your control notification handler code here
   m_stFixedX.EnableWindow(m_chkFixedRegion.GetCheck());
   m_edtFixedX.EnableWindow(m_chkFixedRegion.GetCheck());
   m_stFixedY.EnableWindow(m_chkFixedRegion.GetCheck());
   m_edtFixedY.EnableWindow(m_chkFixedRegion.GetCheck());
   m_stFixedWidth.EnableWindow(m_chkFixedRegion.GetCheck());
   m_edtFixedWidth.EnableWindow(m_chkFixedRegion.GetCheck());
   m_stFixedHeight.EnableWindow(m_chkFixedRegion.GetCheck());
   m_edtFixedHeight.EnableWindow(m_chkFixedRegion.GetCheck());
}

void CSGSettingsSheet::OnUpdateEditFrAdaptive() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CString csText;
   m_edtFrAdaptive.GetWindowText(csText);
   int nVal = _ttoi(csText);
   if(nVal > 30)
   {
      nVal = 30;
      csText.Format(_T("%d"), nVal);

      m_edtFrAdaptive.SetWindowText(csText);
   }
   
   m_edtFrAdaptive.SetSel(csText.GetLength(), -1);


   CButton* radioAdaptiv = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_ADAPTIVE);
   bool bAdaptiveFramerate = radioAdaptiv->GetCheck() == 0 ? false : true;

   m_edtFrAdaptive.EnableWindow(bAdaptiveFramerate);
   m_stAdaptiveWidth.EnableWindow(bAdaptiveFramerate);
   m_edtFrAdaptiveWidth.EnableWindow(bAdaptiveFramerate);
   m_stAdaptiveHeight.EnableWindow(bAdaptiveFramerate);
   m_edtFrAdaptiveHeight.EnableWindow(bAdaptiveFramerate);
   m_spnFrAdaptive.EnableWindow(bAdaptiveFramerate);
}

void CSGSettingsSheet::OnUpdateEditFrStatic() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
   CString csText;

   m_edtFrStatic.GetWindowText(csText);
   int nVal = _ttoi(csText);
   if(nVal > 30)
   {
      nVal = 30;
      csText.Format(_T("%d"), nVal);
      m_edtFrStatic.SetWindowText(csText);
   }
   int iLen = csText.GetLength();
   m_edtFrStatic.SetSel(csText.GetLength(), -1);

   
   CButton* radioStatic = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_STATIC);
   bool bStaticFramerate = radioStatic->GetCheck() == 0 ? false : true;
   
   m_spnFrStatic.EnableWindow(bStaticFramerate);
   m_edtFrStatic.EnableWindow(bStaticFramerate);
}

void CSGSettingsSheet::OnUpdateCheckFixedRegion() 
{
   
   bool bRegionFixed = m_chkFixedRegion.GetCheck() == 0 ? false: true;
   
   m_stFixedX.EnableWindow(bRegionFixed);
   m_edtFixedX.EnableWindow(bRegionFixed);
   m_stFixedY.EnableWindow(bRegionFixed);
   m_edtFixedY.EnableWindow(bRegionFixed);
   m_stFixedWidth.EnableWindow(bRegionFixed);
   m_edtFixedWidth.EnableWindow(bRegionFixed);
   m_stFixedHeight.EnableWindow(bRegionFixed);
   m_edtFixedHeight.EnableWindow(bRegionFixed);
}

void CSGSettingsSheet::SaveSettingsGeneral()
{
   CString csWindow;

   int nDefaultStaticFramerate = 5;
   int nDefaultAdaptiveFramerate = 10;

   bool bMouseVisual =  m_chkMouseVisual.GetCheck() == 0 ? false: true;
   bool bMouseAcoustic = m_chkMouseAcoustic.GetCheck() == 0 ? false: true;
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetMouseEffects(bMouseVisual, bMouseAcoustic);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetMouseEffects(bMouseVisual, bMouseAcoustic);


   CButton* radioStatic = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_STATIC);
   bool bStaticFramerate = radioStatic->GetCheck() == 0 ? false: true;

   m_edtFrStatic.GetWindowText(csWindow);
   if(csWindow == _T(""))
   {
      m_spnFrStatic.SetPos(nDefaultStaticFramerate);
   }

   m_edtFrAdaptive.GetWindowText(csWindow);
   if(csWindow == _T(""))
   {
      m_spnFrAdaptive.SetPos(nDefaultAdaptiveFramerate);
   }

   float fStaticFramerate = (float)m_spnFrStatic.GetPos();
   float fAdaptiveFramerate = (float)m_spnFrAdaptive.GetPos();
   m_edtFrAdaptiveWidth.GetWindowText(csWindow);
   UINT nWidth = _ttoi(csWindow);
   m_edtFrAdaptiveHeight.GetWindowText(csWindow);
   UINT nHeight = _ttoi(csWindow);
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetFramerateSettings(bStaticFramerate, fStaticFramerate, fAdaptiveFramerate,
                                            nWidth, nHeight);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetFramerateSettings(bStaticFramerate, fStaticFramerate, fAdaptiveFramerate,
                                            nWidth, nHeight);
      
   bool bRegionFixed = m_chkFixedRegion.GetCheck() == 0 ? false: true;

   m_edtFixedX.GetWindowText(csWindow);
   UINT nFixedX = _ttoi(csWindow);
   m_edtFixedY.GetWindowText(csWindow);
   UINT nFixedY = _ttoi(csWindow);
   m_edtFixedWidth.GetWindowText(csWindow);
   UINT nFixedWidth = _ttoi(csWindow);
   m_edtFixedHeight.GetWindowText(csWindow);
   UINT nFixedHeight = _ttoi(csWindow);
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetRegionSettings(bRegionFixed, nFixedX, nFixedY, nFixedWidth, nFixedHeight);
   else if (ASSISTANT::Project::active != NULL)
       ASSISTANT::Project::active->SetRegionSettings(bRegionFixed, nFixedX, nFixedY, nFixedWidth, nFixedHeight);
}
//General methods end

//Handling methods begin
void CSGSettingsSheet::LocalizeHandling()
{
   DWORD dwIds[] = {IDC_CHECK_CLEAR_ANNOTATION,
                    IDC_CHECK_DROPPED_FRAMES,
                    IDC_CHECK_BORDER_WHILE_PANNING,          
                    IDC_CHECK_DISPLAY_ACCELERATION,         
                    IDC_CHECK_QUICK_CAPTURE,             
                    IDC_CHECK_SUSPEND_VIDEO,                          
                    -1};
   MfcUtils::Localize(this, dwIds);
}

void CSGSettingsSheet::InitHandling()
{
   bool bClearAnnotation = true;
   bool bReportDroppedFrames = false;
   bool bBorderPanning = true;
   bool bDisableAcceleration = false;
   bool bQuickCapture = false;
   bool bSuspendVideo = false;

   if (m_pAssistantDoc)
      m_pAssistantDoc->GetSgHandling(bClearAnnotation, bReportDroppedFrames, bBorderPanning,
                                     bDisableAcceleration, bQuickCapture, bSuspendVideo);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->GetSgHandling(bClearAnnotation, bReportDroppedFrames, bBorderPanning,
                                     bDisableAcceleration, bQuickCapture, bSuspendVideo);

   m_chkClearAnnotation.SetCheck(bClearAnnotation);
   m_chkDroppedFrames.SetCheck(bReportDroppedFrames);
   m_chkBorderPanning.SetCheck(bBorderPanning);
   m_chkDisplayAcceleration.SetCheck(bDisableAcceleration);
   m_chkQuickCapture.SetCheck(bQuickCapture);
   m_chkSuspendVideo.SetCheck(bSuspendVideo);
}

void CSGSettingsSheet::SaveSettingsHandling()
{
   bool bClearAnnotation = m_chkClearAnnotation.GetCheck() == 0 ? false: true;
   bool bReportDroppedFrames = m_chkDroppedFrames.GetCheck() == 0 ? false: true;
   bool bBorderPanning = m_chkBorderPanning.GetCheck() == 0 ? false: true;
   bool bDisableAcceleration = m_chkDisplayAcceleration.GetCheck() == 0 ? false: true;
   bool bQuickCapture = m_chkQuickCapture.GetCheck() == 0 ? false: true;
   bool bSuspendVideo = m_chkSuspendVideo.GetCheck() == 0 ? false: true;

   if (m_pAssistantDoc)
      m_pAssistantDoc->SetSgHandling(bClearAnnotation, bReportDroppedFrames, bBorderPanning,
                                     bDisableAcceleration, bQuickCapture, bSuspendVideo);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetSgHandling(bClearAnnotation, bReportDroppedFrames, bBorderPanning,
                                     bDisableAcceleration, bQuickCapture, bSuspendVideo);

}

//Handling methods end

//Hokeys methods begin
void CSGSettingsSheet::LocalizeHotkeys()
{
   DWORD dwIds[] = {IDC_STATIC_GB_SG_HOTKEYS_START,
                    IDC_STATIC_HOTKEYS_START_CTRL,
                    IDC_STATIC_HOTKEYS_START_SHIFT,
                    IDC_STATIC_HOTKEYS_START_ALT,
                    IDC_SGB_STATIC_HOTKEYS,
                    -1};
   MfcUtils::Localize(this, dwIds);
}

void CSGSettingsSheet::InitHotkeys()
{
   CString csNone;
   csNone.LoadString(IDS_SG_HOTKEYS_CMB_NONE);
   CString aString[31] = {_T("None"), _T("F9"), _T("F10"), _T("F11"), _T("F12"), _T("A"), _T("B"), 
                          _T("C"), _T("D"), _T("E"), _T("F"), _T("G"), _T("H"), _T("I"), _T("J"), 
                          _T("K"), _T("L"), _T("M"), _T("N"), _T("O"), _T("P"), _T("Q"), _T("R"), 
                          _T("S"), _T("T"), _T("U"), _T("V"), _T("W"), _T("X"), _T("Y"), _T("Z")};
   aString[0] = csNone;
   
   AVGRABBER::HotKey hkStartStop;
   hkStartStop.vKeyString = csNone;
   hkStartStop.hasCtrl = false;
   hkStartStop.hasShift = false;
   hkStartStop.hasAlt = false;

   AVGRABBER::HotKey hkReduce;
   hkReduce.vKeyString = csNone;
   hkReduce.hasCtrl = false;
   hkReduce.hasShift = false;
   hkReduce.hasAlt = false;

   AVGRABBER::HotKey hkMinimize;
   hkMinimize.vKeyString = csNone;
   hkMinimize.hasCtrl = false;
   hkMinimize.hasShift = false;
   hkMinimize.hasAlt = false;


   if (m_pAssistantDoc)
      m_pAssistantDoc->GetSgHotkeys(hkStartStop, hkReduce, hkMinimize);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->GetSgHotkeys(hkStartStop, hkReduce, hkMinimize);

	for(int i = 0; i < 31; i++)
   {
      m_cmbStartStop.AddString(aString[i]);

      if (hkStartStop.vKeyString == aString[i])
         m_cmbStartStop.SetCurSel(i);
   }

   CButton *pCheckButton = NULL;
   
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_CTRL);
   pCheckButton->SetCheck(hkStartStop.hasCtrl);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_SHIFT);
   pCheckButton->SetCheck(hkStartStop.hasShift);
   pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_ALT);
   pCheckButton->SetCheck(hkStartStop.hasAlt);
}

void CSGSettingsSheet::SaveSettingsHotkeys()
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
   hkReduce.hasCtrl = false;
   hkReduce.hasShift = false;
   hkReduce.hasAlt = false;
   
   AVGRABBER::HotKey hkMinimize;
   hkMinimize.hasCtrl = false;
   hkMinimize.hasShift = false;
   hkMinimize.hasAlt = false;
   
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetSgHotkeys(hkStartStop, hkReduce, hkMinimize);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetSgHotkeys(hkStartStop, hkReduce, hkMinimize);
}
//Hotkeys methods end
//Codec methods begin
void CSGSettingsSheet::LocalizeCodec()
{
   DWORD dwIds[] = {IDC_STATIC_GB_SG_CODEC_SETTINGS,
                    IDC_STATIC_GB_SG_CODEC_QUALITY,
                    IDC_STATIC_CODEC_SG_SETTINGS_CODEC,
                    IDC_BUTTON_SG_CODEC_ABOUT,
                    IDC_BUTTON_SG_CONFGURE,
                    IDC_CHECK_SG_KEYFRAME,          
                    IDC_STATIC_SG_DISTANCE,         
                    IDC_STATIC_SG_FRAMES,             
                    IDC_CHECK_SG_DATARATE,
                    IDC_STATIC_SG_DATARATE,
                    IDC_STATIC_SG_ADAPTION_KB,
                    IDC_SGB_STATIC_CODEC,
                    -1};
   MfcUtils::Localize(this, dwIds);
}

void CSGSettingsSheet::InitCodec()
{
   UINT nCodecIndex = 0;
   UINT nCodecQuality = 75;
   bool bUseKeyframes = true;
   UINT nKeyframeDistance = 15;
   bool bUseDatarate = false;
   UINT nDatarate = 500;
   if (m_pAssistantDoc)
      m_pAssistantDoc->GetCodecSettings(m_arCodecList, nCodecIndex, m_bHasAbout, m_bHasConfigure,
                                        m_bSupportsQuality, nCodecQuality, 
                                        m_bSupportsKeyframes, bUseKeyframes, nKeyframeDistance, 
                                        m_bSupportsDatarate, bUseDatarate, nDatarate);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->GetCodecSettings(m_arCodecList, nCodecIndex, m_bHasAbout, m_bHasConfigure,
                                        m_bSupportsQuality, nCodecQuality, 
                                        m_bSupportsKeyframes, bUseKeyframes, nKeyframeDistance, 
                                        m_bSupportsDatarate, bUseDatarate, nDatarate);

   for (int i = 0; i < m_arCodecList.GetSize(); ++i)
      m_cmbSgCodec.AddString(m_arCodecList[i]);

   m_nInitialCodexIndex = nCodecIndex;
   m_cmbSgCodec.SetCurSel(nCodecIndex);

	m_spnQuality.SetRange(0, 100);
   m_spnQuality.SetBuddy(&m_edtQuality);
   m_spnQuality.SetPos(nCodecQuality);
   
   m_sldQuality.SetRange(0, 100);
   m_sldQuality.SetTicFreq(5);
   m_sldQuality.SetPos(nCodecQuality);
   
   CString csWindow;
   m_chkKeyframe.SetCheck(bUseKeyframes);
   csWindow.Format(_T("%d"), nKeyframeDistance);
   m_edtKeyframe.SetWindowText(csWindow);
	m_stDistance.EnableWindow(m_chkKeyframe.GetCheck());
   m_edtKeyframe.EnableWindow(m_chkKeyframe.GetCheck());
   m_stFrames.EnableWindow(m_chkKeyframe.GetCheck());

   m_chkDatarate.SetCheck(bUseDatarate);
   csWindow.Format(_T("%d"), nDatarate);
   m_edtDatarate.SetWindowText(csWindow);
	m_stDatarate.EnableWindow(m_chkDatarate.GetCheck());
   m_edtDatarate.EnableWindow(m_chkDatarate.GetCheck());
   m_stDatarateKb.EnableWindow(m_chkDatarate.GetCheck());
}

void CSGSettingsSheet::OnDeltaposSpinSgQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int iPos = m_spnQuality.GetPos();
   m_sldQuality.SetPos(iPos);
	*pResult = 0;
}

void CSGSettingsSheet::OnCustomdrawSliderSgQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int iPos = m_sldQuality.GetPos();
   m_spnQuality.SetPos(iPos);
	*pResult = 0;
}

void CSGSettingsSheet::OnCheckSgKeyframe() 
{
	// TODO: Add your control notification handler code here
	m_stDistance.EnableWindow(m_chkKeyframe.GetCheck());
   m_edtKeyframe.EnableWindow(m_chkKeyframe.GetCheck());
   m_stFrames.EnableWindow(m_chkKeyframe.GetCheck());
}

void CSGSettingsSheet::OnCheckSgDatarate() 
{
	// TODO: Add your control notification handler code here
	m_stDatarate.EnableWindow(m_chkDatarate.GetCheck());
   m_edtDatarate.EnableWindow(m_chkDatarate.GetCheck());
   m_stDatarateKb.EnableWindow(m_chkDatarate.GetCheck());
}

void CSGSettingsSheet::OnButtonSgCodecAbout() 
{
   if (m_pAssistantDoc)
      m_pAssistantDoc->ShowSgCodecAbout();
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->ShowSgCodecAbout();
}

void CSGSettingsSheet::OnButtonSgConfgure() 
{
   if (m_pAssistantDoc)
      m_pAssistantDoc->ShowSgCodecConfigure();
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->ShowSgCodecConfigure();
}

void CSGSettingsSheet::OnUpdateButtonCodecConfigure(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bHasConfigure);
}

void CSGSettingsSheet::OnUpdateButtonCodecAbout(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bHasAbout);
}

void CSGSettingsSheet::OnSelchangeComboSgCodec() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      UINT nCodecIndex = m_cmbSgCodec.GetCurSel();
      CString csCodec = m_arCodecList[nCodecIndex];
      ASSISTANT::Project::active->SetSgCodec(csCodec, false);
      m_bHasAbout = ASSISTANT::Project::active->SgCodecHasAbout();
      m_bHasConfigure = ASSISTANT::Project::active->SgCodecCanBeConfigured();
   }
}

void CSGSettingsSheet::SaveSettingsCodec()
{
   CString csCodec;
   m_cmbSgCodec.GetWindowText(csCodec);
   
   UINT nCodecQuality = m_sldQuality.GetPos();

   CString csWindow;

   bool bUseKeyframes = m_chkKeyframe.GetCheck() == 0 ? false : true;
   m_edtKeyframe.GetWindowText(csWindow);
   UINT nKeyframeDistance = _ttoi(csWindow);
   
   bool bUseDatarate = m_chkDatarate.GetCheck() == 0 ? false : true;
   m_edtDatarate.GetWindowText(csWindow);
   UINT nDatarate = _ttoi(csWindow);

   if (m_pAssistantDoc)
      m_pAssistantDoc->SetCodecSettings(csCodec, nCodecQuality, 
                                        bUseKeyframes, nKeyframeDistance, 
                                        bUseDatarate, nDatarate);
   else if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetCodecSettings(csCodec, nCodecQuality, 
                                        bUseKeyframes, nKeyframeDistance, 
                                        bUseDatarate, nDatarate);
}

//Codec methods end

void CSGSettingsSheet::OnBnClickedStaticGbSgHotkeysStart2()
{
    // TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
}

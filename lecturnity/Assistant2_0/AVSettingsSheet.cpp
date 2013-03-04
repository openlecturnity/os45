// AVSettingsSheet.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "..\Studio\Studio.h"
#endif
#include "AVSettingsSheet.h"
#include "backend\la_project.h"
#include "MfcUtils.h"
#include "MainFrm.h"

// CAVSettingsSheet dialog

IMPLEMENT_DYNAMIC(CAVSettingsSheet, CXTPPropertyPage)

CAVSettingsSheet::CAVSettingsSheet()
	: CXTPPropertyPage(CAVSettingsSheet::IDD)
{
   m_bRecordVideo = false;
   m_bShowMonitor = false;
   m_bUseAlternativeSync = false;
   m_bSetAvOffset = false;
   m_iAvOffset = 0;
   m_nXResolution = 0;
   m_nYResolution = 0;
   m_nBitPerColor = 0;
   m_nFrameRateIndex = 0;
   m_bUseVFW = true;
   m_nVFWIndex = 0;
   m_nWDMIndex = 0;

   m_cursorWait = LoadCursor(NULL, IDC_WAIT); 

   m_nCodecIndex = 0;
   m_nQuality = 75;
   m_bUseKeyframes = true;
   m_nKeyframeDistance = 15;
   m_bUseDatarateAdaption = false;
   m_nDatarate = 500;
   m_bHasAbout = false;
   m_bHasConfigure = false;
}

CAVSettingsSheet::~CAVSettingsSheet()
{
  
}

void CAVSettingsSheet::DoDataExchange(CDataExchange* pDX)
{
	CXTPPropertyPage::DoDataExchange(pDX);
   //Audio variables begin
   DDX_Control(pDX, IDC_AUDIO_SLIDER, m_sldVolume);
	DDX_Control(pDX, IDC_AUDIO_SOURCE, m_cmbAudioSource);
	DDX_Control(pDX, IDC_AUDIO_DEVICE, m_cmbAudioDevice);
	DDX_Control(pDX, IDC_AUDIO_SAMPLE_WIDTH, m_cmbAudioSampleWidth);
	DDX_Control(pDX, IDC_AUDIO_SAMPLE_RATE, m_cmbAudioSampleRate);
   //Audio variables end
   //Video variables begin
   DDX_Control(pDX, IDC_COMBO_VIDEO_FRAMERATE, m_cmbFramerate);
	DDX_Control(pDX, IDC_COMBO_VIDEO_DRIVER, m_cmbDriverModel);
	DDX_Control(pDX, IDC_STATIC_RESOLUTION, m_stResolution);
	DDX_Control(pDX, IDC_STATIC_COLORDEPTH, m_stColordepth);
	DDX_Control(pDX, IDC_EDIT_AV_OFFSET, m_edtAvOffset);
	DDX_Control(pDX, IDC_STATIC_AV_MS, m_stAvMs);
	DDX_Control(pDX, IDC_CHECK_AV_OFFSET, m_chkAvOffset);
	DDX_Control(pDX, IDC_CHECK_ALTERNATIVE_AV_SYNC, m_chkAltAVSync);
	DDX_Control(pDX, IDC_CHECK_VIDEO_MONITOR, m_chkVideoMonitor);
	DDX_Control(pDX, IDC_CHECK_RECORD_VIDEO, m_chkRecordVideo);
   //Video variables end
   //Codec variables begin
   DDX_Control(pDX, IDC_COMBO_CODECS2, m_cmbCodecs);
	DDX_Control(pDX, IDC_STATIC_KEYFRAME_DISTANCE, m_stKeyframeDistance);
	DDX_Control(pDX, IDC_STATIC_KEYFRAME_FRAMES, m_stKeyframeFrames);
	DDX_Control(pDX, IDC_STATIC_DATARATE_TARGET, m_stDatarateTarget);
	DDX_Control(pDX, IDC_STATIC_DATARATE_KB, m_stDatarateKb);
	DDX_Control(pDX, IDC_EDIT_DATARATE, m_edtDatarate);
	DDX_Control(pDX, IDC_EDIT_KEYFRAME, m_edtKeyrame);
	DDX_Control(pDX, IDC_CHECK_CODEC_KEYFRAME, m_chkKeyframe);
	DDX_Control(pDX, IDC_CHECK_CODEC_DATARATE, m_chkDatarate);
	DDX_Control(pDX, IDC_SPIN_QUALITY, m_spnQuality);
	DDX_Control(pDX, IDC_SLIDER_QUALITY, m_sldQuality);
	DDX_Control(pDX, IDC_EDIT_QUALITY, m_edtQuality);
   //Codec variables end
}


BEGIN_MESSAGE_MAP(CAVSettingsSheet, CXTPPropertyPage)
    ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
    //Audio messages begin
    ON_CBN_SELCHANGE(IDC_AUDIO_DEVICE, OnSelchangeAudioDevice)
    ON_CBN_SELCHANGE(IDC_AUDIO_SAMPLE_RATE, OnSelchangeAudioSampleRate)
    ON_CBN_SELCHANGE(IDC_AUDIO_SAMPLE_WIDTH, OnSelchangeAudioSampleWidth)
    ON_CBN_SELCHANGE(IDC_AUDIO_SOURCE, OnSelchangeAudioSource)
    ON_UPDATE_COMMAND_UI(IDC_AUDIO_SOURCE, OnUpdateAudioSource)
    ON_UPDATE_COMMAND_UI(IDC_AUDIO_SLIDER, OnUpdateAudioSlider)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_AUDIO_SLIDER, OnReleasedcaptureAudioSlider)
    //Audio messages end
    //Video messages begin
    ON_BN_CLICKED(IDC_CHECK_RECORD_VIDEO, OnCheckRecordVideo)
    ON_BN_CLICKED(IDC_CHECK_VIDEO_MONITOR, OnCheckVideoMonitor)
    ON_BN_CLICKED(IDC_CHECK_AV_OFFSET, OnCheckAvOffset)
    ON_EN_CHANGE(IDC_EDIT_AV_OFFSET, OnChangeEditAvOffset)
    ON_BN_CLICKED(IDC_CHECK_ALTERNATIVE_AV_SYNC, OnCheckAlternativeAvSync)
    ON_UPDATE_COMMAND_UI(IDC_CHECK_ALTERNATIVE_AV_SYNC, OnUpdateCheckAlternativeAvSync)
    ON_UPDATE_COMMAND_UI(IDC_CHECK_RECORD_VIDEO, OnUpdateChkRecordVideo)
    ON_UPDATE_COMMAND_UI(IDC_CHECK_VIDEO_MONITOR, OnUpdateChkVideoMonitor)
    ON_UPDATE_COMMAND_UI(IDC_CHECK_ALTERNATIVE_AV_SYNC, OnUpdateChkAlternativeAVSync)
    ON_UPDATE_COMMAND_UI(IDC_CHECK_AV_OFFSET, OnUpdateChkAVOffset)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_AV_OFFSET, OnUpdateEdtAVOffset)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_AV_MS, OnUpdateLblAVms)
    ON_UPDATE_COMMAND_UI(IDC_COMBO_VIDEO_FRAMERATE, OnUpdateCmbFramerate)
    ON_UPDATE_COMMAND_UI(IDC_COMBO_VIDEO_DRIVER, OnUpdateBtnWindowsDriver)
    ON_UPDATE_COMMAND_UI_RANGE(IDC_STATIC_VIDEO_RESOLUTION, IDC_STATIC_VIDEO_FRAMERATE, OnUpdateVideoLabels)
    ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_DRIVER, OnSelchangeComboVideoDriver)
    ON_BN_CLICKED(IDC_BUTTON_WDM_DV, OnButtonWdmDv)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_WDM_DV, OnUpdateButtonWdmDv)
    ON_BN_CLICKED(IDC_BUTTON_WDM_FORMAT, OnButtonWdmFormat)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_WDM_FORMAT, OnUpdateButtonWdmFormat)
    ON_BN_CLICKED(IDC_BUTTON_WDM_SETTINGS, OnButtonWdmSettings)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_WDM_SETTINGS, OnUpdateButtonWdmSettings)
    ON_BN_CLICKED(IDC_BUTTON_WDM_SOURCE, OnButtonWdmSource)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_WDM_SOURCE, OnUpdateButtonWdmSource)
    ON_BN_CLICKED(IDC_CHECK_AV_OFFSET, OnAvOffset)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_RESOLUTION, OnUpdateResolutionSource)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_COLORDEPTH, OnUpdateColorDepthSource)
    ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_FRAMERATE, OnSelchangeComboVideoFramerate)
    ON_CBN_EDITCHANGE(IDC_COMBO_VIDEO_FRAMERATE, OnEditchangeComboVideoFramerate)
    //Video messages end
    //Codec messages begin
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_QUALITY, OnDeltaposSpinQuality)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_QUALITY, OnCustomdrawSliderQuality)
    ON_BN_CLICKED(IDC_CHECK_CODEC_KEYFRAME, OnCheckCodecKeyframe)
    ON_BN_CLICKED(IDC_CHECK_CODEC_DATARATE, OnCheckCodecDatarate)
    ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_KEYFRAME_DISTANCE, OnUpdateLblDistance)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_KEYFRAME, OnUpdateEdtDistance)
    ON_UPDATE_COMMAND_UI(IDC_CHECK_CODEC_KEYFRAME, OnUpdateChkFrames)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_KEYFRAME_FRAMES, OnUpdateLblFrames)
    ON_UPDATE_COMMAND_UI(IDC_CHECK_CODEC_DATARATE, OnUpdateChkTargetDatarate)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_DATARATE_TARGET, OnUpdateLblTargetDatarate)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_DATARATE, OnUpdateEdtTargetDatarate)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_DATARATE_KB, OnUpdateLblKBS)
    ON_EN_CHANGE(IDC_EDIT_QUALITY, OnChangeEditQuality)
    ON_EN_CHANGE(IDC_EDIT_KEYFRAME, OnChangeEditKeyframe)
    ON_EN_CHANGE(IDC_EDIT_DATARATE, OnChangeEditDatarate)
    ON_CBN_SELCHANGE(IDC_COMBO_CODECS2, OnSelchangeComboCodecs)
    ON_BN_CLICKED(IDC_BUTTON_CODEC_ABOUT2, OnButtonCodecAbout)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_CODEC_ABOUT2, OnUpdateButtonCodecAbout)
    ON_BN_CLICKED(IDC_BUTTON_CODEC_CONFIGURE, OnButtonCodecConfigure)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_CODEC_CONFIGURE, OnUpdateButtonCodecConfigure)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_CODEC_GB_SETTINGS, OnUpdateGroupSettings)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_CODEC_SETTINGS_CODEC, OnUpdateStaticCodec)
    ON_UPDATE_COMMAND_UI(IDC_COMBO_CODECS2, OnUpdateComboCodec)
    ON_UPDATE_COMMAND_UI(IDC_STATIC_CODEG_GB_QUALITY, OnUpdateGroupQuality)
    ON_UPDATE_COMMAND_UI(IDC_SLIDER_QUALITY, OnUpdateSliderQuality)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_QUALITY, OnUpdateEdtQuality)
    ON_UPDATE_COMMAND_UI(IDC_SPIN_QUALITY, OnUpdateSpinQuality)
   //Codec messages end
END_MESSAGE_MAP()


// CAVSettingsSheet message handlers
 void CAVSettingsSheet::Init(CAssistantDoc* pDocument)
{
   m_pAssistantDoc = pDocument;

   //Init Audio begin
   m_arDevices.RemoveAll();
   m_nDevice = 0;

   m_arSampleWidths.RemoveAll();
   m_nSampleWidth = 0;

   m_arSampleRates.RemoveAll();
   m_nSampleRate = 0;

   m_arSources.RemoveAll();
   m_nSource = 0;

   if (m_pAssistantDoc != NULL)
   {
      m_pAssistantDoc->GetAudioDevices(m_arDevices, m_nDevice);
      m_pAssistantDoc->GetSampleWidths(m_nDevice, m_arSampleWidths, m_nSampleWidth);
      m_pAssistantDoc->GetSampleRates(m_nDevice, m_arSampleRates, m_nSampleRate);
      m_pAssistantDoc->GetAudioSources(m_arSources, m_nSource);
   } else if (ASSISTANT::Project::active != NULL) {
      ASSISTANT::Project::active->GetAudioDevices(m_arDevices, m_nDevice);
      ASSISTANT::Project::active->GetSampleWidths(m_nDevice, m_arSampleWidths, m_nSampleWidth);
      ASSISTANT::Project::active->GetSampleRates(m_nDevice, m_arSampleRates, m_nSampleRate);
      ASSISTANT::Project::active->GetAudioSources(m_arSources, m_nSource);
   }
   m_nInitialDevice = m_nDevice;
   m_nInitialSource = m_nSource;
   //Init Audio end
}
BOOL CAVSettingsSheet::OnInitDialog()
{
   CXTPPropertyPage::OnInitDialog();

   ModifyStyle(0, /*WS_CLIPSIBLINGS*//*WS_CLIPCHILDREN | */WS_VSCROLL);
   LocalizeAudio();
   InitAudio();

   LocalizeVideo();
   InitVideo();

   LocalizeCodec();
   InitCodec();

#ifdef _STUDIO
   CStudioApp* pApp = (CStudioApp*)AfxGetApp();
#else
   CAssistantApp* pApp = (CAssistantApp*)AfxGetApp();
#endif
   UINT nColorScheme = pApp->GetCurrentColorScheme();

   CString csStr;
   csStr.LoadString(IDC_AVCS_STATIC_TITLE);
   for(int i = 0; i < 4; i++)
   {
     m_caption[i].SetScheme(nColorScheme);
     m_caption[i].SetTitleAlign(xtpTitleAlignLeft);
     m_caption[i].SetCaptionType(xtpCaptionNormal);
   }
 
   m_caption[3].SetTitleAlign(xtpTitleAlignCustom);
   m_caption[3].SetCaptionType(xtpCaptionHeader);
   m_caption[3].SetIconID(xtpAudioVideo);

   m_caption[0].SubclassDlgItem(IDC_AVCS_STATIC_AUDIO, this);
   m_caption[1].SubclassDlgItem(IDC_AVCS_STATIC_VIDEO, this);
   m_caption[2].SubclassDlgItem(IDC_AVCS_STATIC_CODEC, this);
   m_caption[3].SubclassDlgItem(IDC_AVCS_STATIC_TITLE, this);
   if(csStr.IsEmpty()==false)
   {
     m_caption[3].SetCaption(csStr);
   }
   SetResize(IDC_AVCS_STATIC_AUDIO, SZ_TOP_LEFT, SZ_TOP_RIGHT);
   SetResize(IDC_AVCS_STATIC_VIDEO, SZ_TOP_LEFT, SZ_TOP_RIGHT);
   SetResize(IDC_AVCS_STATIC_CODEC, SZ_TOP_LEFT, SZ_TOP_RIGHT);
   SetResize(IDC_AVCS_STATIC_TITLE, SZ_TOP_LEFT, SZ_TOP_RIGHT);

   m_caption[3].SetBold();

   UpdateRecordVideoHint();
   
   ::GetFocus();
   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CAVSettingsSheet::OnOK()
{
    if (m_bRecordVideo && m_pAssistantDoc && m_arCodecs.GetSize() > 0)
        m_pAssistantDoc->SetCodec(m_arCodecs[m_nCodecIndex], true);

   SaveSettingsAudio();
   SaveSettingsVideo();
  
   CMainFrameA *pFrame = CMainFrameA::GetCurrentInstance();
   if (pFrame != NULL) {
       pFrame->SetAudioVideoTooltips();
   }

   CXTPPropertyPage::OnOK();
}

void CAVSettingsSheet::OnCancel() {
    if (m_pAssistantDoc) {
        // Restore initial audio source
        m_pAssistantDoc->SetAudioDevice(m_nInitialDevice);
        m_pAssistantDoc->SetAudioSource(m_nInitialSource);

        m_pAssistantDoc->ReadAudioSettings();
        m_pAssistantDoc->ReadVideoSettings(false);
        m_pAssistantDoc->ShowVideoMonitor(false);
    } else if (ASSISTANT::Project::active != NULL) {
        // Restore initial audio source
        ASSISTANT::Project::active->SetAudioDevice(m_nInitialDevice);
        ASSISTANT::Project::active->SetAudioSource(m_nInitialSource);

        ASSISTANT::Project::active->ReadAudioSettings();
        ASSISTANT::Project::active->ReadVideoSettings(false);
        ASSISTANT::Project::active->ShowVideoMonitor(false);
    }

    CXTPPropertyPage::OnCancel();
}

void CAVSettingsSheet::OnKickIdle()
{
   UpdateDialogControls(this, false);
}

BOOL CAVSettingsSheet::OnSetActive()
{
   // TODO: Add your specialized code here and/or call the base class
   CWnd* pControl = GetDlgItem(IDC_CHECK_RECORD_VIDEO);
   pControl->SetFocus();

   return CXTPPropertyPage::OnSetActive();
}

//Audio methodds begin
void CAVSettingsSheet::LocalizeAudio()
{
   DWORD dwIds[] = {IDC_STATIC_AUDIO_DEVICE,
                    IDC_STATIC_AUDIO_SAMPL_WIDTH,
                    IDC_STATIC_AUDIO_SAMPLE_RATE,
                    IDC_STATIC_AUDIO_SOURCE,
                    IDC_AVCS_STATIC_AUDIO,
                   -1};

   MfcUtils::Localize(this, dwIds);
}

void CAVSettingsSheet::InitAudio()
{
   for (int i = 0; i < m_arDevices.GetSize(); ++i)
      m_cmbAudioDevice.AddString(m_arDevices[i]);
   m_cmbAudioDevice.SetCurSel(m_nDevice);

   for (i = 0; i < m_arSampleWidths.GetSize(); ++i)
   {
      CString csSampleWidth;
      csSampleWidth.Format(_T("%d"), m_arSampleWidths[i]);
      m_cmbAudioSampleWidth.AddString(csSampleWidth);
   }
   m_cmbAudioSampleWidth.SetCurSel(m_nSampleWidth);
   
   for (i = 0; i < m_arSampleRates.GetSize(); ++i)
   {
      CString csSampleRate;
      csSampleRate.Format(_T("%d"), m_arSampleRates[i]);
      m_cmbAudioSampleRate.AddString(csSampleRate);
   }
   m_cmbAudioSampleRate.SetCurSel(m_nSampleRate);

   for (i = 0; i < m_arSources.GetSize(); ++i)
      m_cmbAudioSource.AddString(m_arSources[i]);
   m_cmbAudioSource.SetCurSel(m_nSource);

   SetVolumeRange(0, 100);
}

void CAVSettingsSheet::SaveSettingsAudio() {
    if (m_pAssistantDoc) {
        m_pAssistantDoc->ActivateAudioDevice();
        m_pAssistantDoc->WriteAudioSettings();
    } else if (ASSISTANT::Project::active != NULL) {
        ASSISTANT::Project::active->ActivateAudioDevice();
        ASSISTANT::Project::active->WriteAudioSettings();
    }
}

void CAVSettingsSheet::OnSelchangeAudioDevice() 
{
    if (m_pAssistantDoc != NULL || ASSISTANT::Project::active != NULL)
   {
      m_arSampleWidths.RemoveAll();
      m_nSampleWidth = 0;
      
      m_arSampleRates.RemoveAll();
      m_nSampleRate = 0;
      
      m_nDevice = m_cmbAudioDevice.GetCurSel();
      if (m_pAssistantDoc != NULL)
          m_pAssistantDoc->SetAudioDevice(m_nDevice);
      else 
          ASSISTANT::Project::active->SetAudioDevice(m_nDevice);
      
      m_cmbAudioSampleWidth.ResetContent();
      if (m_pAssistantDoc != NULL)
          m_pAssistantDoc->GetSampleWidths(m_nDevice, m_arSampleWidths, m_nSampleWidth);
      else 
         ASSISTANT::Project::active->GetSampleWidths(m_nDevice, m_arSampleWidths, m_nSampleWidth);
      for (int i = 0; i < m_arSampleWidths.GetSize(); ++i)
      {
         CString csSampleWidth;
         csSampleWidth.Format(_T("%d"), m_arSampleWidths[i]);
         m_cmbAudioSampleWidth.AddString(csSampleWidth);
      }
      m_cmbAudioSampleWidth.SetCurSel(m_nSampleWidth);
      
      m_cmbAudioSampleRate.ResetContent();
      if (m_pAssistantDoc != NULL)
          m_pAssistantDoc->GetSampleRates(m_nDevice, m_arSampleRates, m_nSampleRate);
      else
          ASSISTANT::Project::active->GetSampleRates(m_nDevice, m_arSampleRates, m_nSampleRate);
      for (i = 0; i < m_arSampleRates.GetSize(); ++i)
      {
         CString csSampleRate;
         csSampleRate.Format(_T("%d"), m_arSampleRates[i]);
         m_cmbAudioSampleRate.AddString(csSampleRate);
      }
      m_cmbAudioSampleRate.SetCurSel(m_nSampleRate);

      if (m_pAssistantDoc != NULL)
          m_pAssistantDoc->ActivateAudioDevice();
      else
          ASSISTANT::Project::active->ActivateAudioDevice();

      m_cmbAudioSource.ResetContent();
      if (m_pAssistantDoc != NULL)
          m_pAssistantDoc->GetAudioSources(m_arSources, m_nSource);
      else
          ASSISTANT::Project::active->GetAudioSources(m_arSources, m_nSource);

      for (i = 0; i < m_arSources.GetSize(); ++i)
         m_cmbAudioSource.AddString(m_arSources[i]);
      m_cmbAudioSource.SetCurSel(m_nSource);
      
      m_cmbAudioSampleWidth.ResetContent();
      if (m_pAssistantDoc != NULL)
          m_pAssistantDoc->GetSampleWidths(m_nDevice, m_arSampleWidths, m_nSampleWidth);
      else
          ASSISTANT::Project::active->GetSampleWidths(m_nDevice, m_arSampleWidths, m_nSampleWidth);
      for (i = 0; i < m_arSampleWidths.GetSize(); ++i)
      {
         CString csSampleWidth;
         csSampleWidth.Format(_T("%d"), m_arSampleWidths[i]);
         m_cmbAudioSampleWidth.AddString(csSampleWidth);
      }
      m_cmbAudioSampleWidth.SetCurSel(m_nSampleWidth);
      
      m_cmbAudioSampleRate.ResetContent();
      if (m_pAssistantDoc != NULL)
          m_pAssistantDoc->GetSampleRates(m_nDevice, m_arSampleRates, m_nSampleRate);
      else
          ASSISTANT::Project::active->GetSampleRates(m_nDevice, m_arSampleRates, m_nSampleRate);
      for (i = 0; i < m_arSampleRates.GetSize(); ++i)
      {
         CString csSampleRate;
         csSampleRate.Format(_T("%d"), m_arSampleRates[i]);
         m_cmbAudioSampleRate.AddString(csSampleRate);
      }
      m_cmbAudioSampleRate.SetCurSel(m_nSampleRate);
   }
}

void CAVSettingsSheet::OnSelchangeAudioSampleRate() 
{
   m_nSampleRate = m_cmbAudioSampleRate.GetCurSel();
   if (m_pAssistantDoc != NULL)
       m_pAssistantDoc->SetSampleRate(m_nSampleRate);
   else if (ASSISTANT::Project::active != NULL)
       ASSISTANT::Project::active->SetSampleRate(m_nSampleRate);
}

void CAVSettingsSheet::OnSelchangeAudioSampleWidth() 
{
   m_nSampleWidth = m_cmbAudioSampleWidth.GetCurSel();
   if (m_pAssistantDoc != NULL)
       m_pAssistantDoc->SetSampleWidth(m_nSampleWidth);
   else if (ASSISTANT::Project::active != NULL)
       ASSISTANT::Project::active->SetSampleWidth(m_nSampleWidth);
}

void CAVSettingsSheet::OnSelchangeAudioSource() 
{
   m_nSource = m_cmbAudioSource.GetCurSel();
   if (m_pAssistantDoc != NULL)
       m_pAssistantDoc->SetAudioSource(m_nSource);
   else if (ASSISTANT::Project::active != NULL)
       ASSISTANT::Project::active->SetAudioSource(m_nSource);
}

void CAVSettingsSheet::OnUpdateAudioSource(CCmdUI *pCmdUI)
{
   if (m_arSources.GetSize() > 0)
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
   
   if (m_pAssistantDoc && GetKeyState(VK_LBUTTON) >= 0) // left mouse not key pressed)
   {
      UINT nSource = 0;
      if (m_pAssistantDoc != NULL)
          nSource = m_pAssistantDoc->GetAudioSource();
      else if (ASSISTANT::Project::active != NULL)
          nSource = ASSISTANT::Project::active->GetAudioSource();
      if (nSource != m_nSource)
         m_cmbAudioSource.SetCurSel(m_nSource);
   }
}

void CAVSettingsSheet::OnUpdateAudioSlider(CCmdUI *pCmdUI) {
   if (m_arSources.GetSize() > 0) {
      m_sldVolume.ShowWindow(SW_SHOW);
      pCmdUI->Enable(true);
   } else {
      pCmdUI->Enable(false);
      m_sldVolume.ShowWindow(SW_HIDE);
   }

   if ((m_pAssistantDoc || ASSISTANT::Project::active != NULL) && 
       GetKeyState(VK_LBUTTON) >= 0) {  // left mouse not key pressed)
           UINT nMixerVolume = 0;
           if (m_pAssistantDoc != NULL)
               nMixerVolume = m_pAssistantDoc->GetMixerVolume();
           else
               nMixerVolume = ASSISTANT::Project::active->GetMixerVolume();
           SetVolume(nMixerVolume);
   }
}

void CAVSettingsSheet::SetVolume(int nValue)
{
   if(nValue < m_nMinRange)
   {
      nValue = m_nMinRange;
   }

   if(nValue > m_nMaxRange)
   {
      nValue = m_nMaxRange;
   }

   m_sldVolume.SetPos(100 - nValue);
}

int CAVSettingsSheet::GetVolume()
{
   return(100 - m_sldVolume.GetPos());
}

void CAVSettingsSheet::SetVolumeRange(int nMin, int nMax)
{
   m_sldVolume.SetRange(nMin, nMax);
   m_nMinRange = nMin;
   m_nMaxRange = nMax;
   CString s;
}

void CAVSettingsSheet::OnReleasedcaptureAudioSlider(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if (m_pAssistantDoc || ASSISTANT::Project::active != NULL) {
      UINT nCode = pNMHDR->code;
      if (m_pAssistantDoc)
        m_pAssistantDoc->SetMixerVolume(GetVolume());
      else
        ASSISTANT::Project::active->SetMixerVolume(GetVolume());
   }

	*pResult = 0;
}
//Audio methods end

//Video methodds begin
void CAVSettingsSheet::LocalizeVideo()
{
   DWORD dwIds[] = {IDC_STATIC_VIDEO_GB_SETTINGS,
                    IDC_STATIC_VIDEO_RESOLUTION,
                    IDC_STATIC_VIDEO_COLORDEPTH,
                    IDC_STATIC_VIDEO_FRAMERATE,
                    IDC_STATIC_AV_MS,
                    IDC_CHECK_RECORD_VIDEO,          
                    IDC_CHECK_VIDEO_MONITOR,         
                    IDC_CHECK_ALTERNATIVE_AV_SYNC,             
                    IDC_CHECK_AV_OFFSET,            
                    IDC_BUTTON_WDM_SETTINGS, 
                    IDC_BUTTON_WDM_SOURCE,
                    IDC_BUTTON_WDM_FORMAT,
                    IDC_BUTTON_WDM_DV,
                    IDC_STATIC_VIDEO_GB_DRIVER,
                    IDC_AVCS_STATIC_VIDEO,
                    -1};
   MfcUtils::Localize(this, dwIds);
}

void CAVSettingsSheet::InitVideo()
{
#ifdef _STUDIO
    if (CStudioApp::IsLiveContextMode()) {
        m_chkRecordVideo.EnableWindow(FALSE);
        m_chkRecordVideo.SetCheck(FALSE);

        m_chkAltAVSync.EnableWindow(FALSE);
        m_chkAltAVSync.SetCheck(FALSE);
    } else 
#endif
        // be aware that next "if" is on the above "else" for Studio
        if (m_bRecordVideo && m_pAssistantDoc != NULL)
    {
        //CAVCSettings* pParent = (CAVCSettings*)GetParent();

        SetCursor(m_cursorWait);
        HRESULT hr = m_pAssistantDoc->EnableVideoRecording(m_bRecordVideo, false);

        if (!SUCCEEDED(hr)){
            m_chkRecordVideo.SetCheck(false);
        }

        SetCursor(NULL);
    }

    UpdateVideoPage();
    UpdateCodecPage();
}

void CAVSettingsSheet::SaveSettingsVideo()
{
   if (m_pAssistantDoc != NULL) {
      m_pAssistantDoc->ShowVideoMonitor(false);
      m_pAssistantDoc->WriteVideoSettings();
   } else if (ASSISTANT::Project::active != NULL) {
      ASSISTANT::Project::active->ShowVideoMonitor(false);
      ASSISTANT::Project::active->WriteVideoSettings();
   }
}

void CAVSettingsSheet::OnCheckRecordVideo() 
{
	//CAVCSettings* pParent = (CAVCSettings*)GetParent();
    m_bRecordVideo = false;
    if (m_pAssistantDoc != NULL) {
        m_bRecordVideo = m_chkRecordVideo.GetCheck() == 0 ? false: true;

        SetCursor(m_cursorWait);
        HRESULT hr = m_pAssistantDoc->EnableVideoRecording(m_bRecordVideo, true);

        if (!SUCCEEDED(hr))
            m_chkRecordVideo.SetCheck(false);

        LocalizeCodec();
        InitCodec();

        UpdateVideoPage();
        UpdateCodecPage();

        SetCursor(NULL);

        UpdateRecordVideoHint();
        Invalidate();
        RedrawWindow();
    }

}

void CAVSettingsSheet::UpdateVideoPage()
{
   m_bRecordVideo = false;
   m_bShowMonitor = false;
   m_bUseAlternativeSync = false;
   m_bSetAvOffset = false;
   m_iAvOffset = 0;
   m_nXResolution = 0;
   m_nYResolution = 0;
   m_nBitPerColor = 0;
   m_arFrameRates.RemoveAll();
   m_nFrameRateIndex = 0;
   m_bUseVFW = true;
   m_arVFWSources.RemoveAll();
   m_nVFWIndex = 0;
   m_arWDMSources.RemoveAll();
   m_nWDMIndex = 0;

   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->GetVideoSettings(m_bRecordVideo, m_bShowMonitor, m_bUseAlternativeSync,
                                        m_bSetAvOffset, m_iAvOffset);
      
      m_pAssistantDoc->GetVideoParameter(m_bUseVFW, m_arVFWSources, m_nVFWIndex,
                                         m_arWDMSources, m_nWDMIndex,
                                         m_nXResolution, m_nYResolution, m_nBitPerColor, 
                                         m_arFrameRates, m_nFrameRateIndex);
   }

   CButton *pRecordVideo = (CButton *)GetDlgItem(IDC_CHECK_RECORD_VIDEO);
   pRecordVideo->SetCheck(m_bRecordVideo);

   CButton *pShowMonitorButton = (CButton *)GetDlgItem(IDC_CHECK_VIDEO_MONITOR);
   pShowMonitorButton->SetCheck(m_bShowMonitor);
   
   CButton *pUseAlternativeSyncButton = (CButton *)GetDlgItem(IDC_CHECK_ALTERNATIVE_AV_SYNC);
   pUseAlternativeSyncButton->SetCheck(m_bUseAlternativeSync);

   CButton *pSetAvOffsetButton = (CButton *)GetDlgItem(IDC_CHECK_AV_OFFSET);
   pSetAvOffsetButton->SetCheck(m_bSetAvOffset);
   
	CEdit* pAvOffsetEdit = (CEdit*) GetDlgItem(IDC_EDIT_AV_OFFSET);
   CString csAvOffset;
   csAvOffset.Format(_T("%d"), m_iAvOffset);
   pAvOffsetEdit->SetWindowText(csAvOffset);

   CString csResolution;
   csResolution.Format(_T("%d x %d"), m_nXResolution, m_nYResolution);
   CStatic *pResolution = (CStatic *)GetDlgItem(IDC_STATIC_RESOLUTION);
   pResolution->SetWindowText(csResolution);
   
   CString csBitPerColor;
   csBitPerColor.Format(_T("%d"), m_nBitPerColor);
   CStatic *pBitPerColor = (CStatic *)GetDlgItem(IDC_STATIC_COLORDEPTH);
   pBitPerColor->SetWindowText(csBitPerColor);

   CComboBox *pFrameRates = (CComboBox *)GetDlgItem(IDC_COMBO_VIDEO_FRAMERATE);
   pFrameRates->ResetContent();
   for (int i = 0; i < m_arFrameRates.GetSize(); ++i)
   {
      CString csFramerate;
      csFramerate.Format(_T("%2.2f"), m_arFrameRates[i]);
      pFrameRates->AddString(csFramerate);
   }
   pFrameRates->SetCurSel(m_nFrameRateIndex);

   CComboBox *pWDMSource = (CComboBox *)GetDlgItem(IDC_COMBO_VIDEO_DRIVER);
   pWDMSource->ResetContent();
   for (i = 0; i < m_arWDMSources.GetSize(); ++i)
      pWDMSource->AddString(m_arWDMSources[i]);
   pWDMSource->SetCurSel(m_nWDMIndex);

}

void CAVSettingsSheet::OnCheckVideoMonitor() 
{
   if (m_pAssistantDoc)
   {
      bool bShow = m_chkVideoMonitor.GetCheck() == 0 ? false: true;
      m_pAssistantDoc->ShowVideoMonitor(bShow);
   }
}

void CAVSettingsSheet::OnCheckAvOffset() 
{
   m_bSetAvOffset = m_chkAvOffset.GetCheck() == 0 ? false: true;

   CString csAvOffset;
   m_edtAvOffset.GetWindowText(csAvOffset);
   m_iAvOffset = _ttoi(csAvOffset);

   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->SetUseAvOffset(m_bSetAvOffset);
   }

	m_edtAvOffset.EnableWindow(m_chkAvOffset.GetCheck());
}

void CAVSettingsSheet::OnChangeEditAvOffset() 
{
   if (m_pAssistantDoc)
   {
      CString csText;
      m_edtAvOffset.GetWindowText(csText);
      m_iAvOffset = _ttoi(csText);
      m_pAssistantDoc->SetAvOffset(m_iAvOffset);
   }
}

void CAVSettingsSheet::OnCheckAlternativeAvSync() 
{
   if (m_pAssistantDoc)
   {
      m_bUseAlternativeSync = m_chkAltAVSync.GetCheck() == 0 ? false: true;
      m_pAssistantDoc->SetAlternativeAvSync(m_bUseAlternativeSync);
   }
}

void CAVSettingsSheet::OnUpdateCheckAlternativeAvSync(CCmdUI *pCmdUI) 
{
   if (m_bUseAlternativeSync)
      pCmdUI->SetCheck(true);
   else
      pCmdUI->SetCheck(false);

}

void CAVSettingsSheet::OnSelchangeComboVideoDriver() 
{
   int iCurrentDevice = m_cmbDriverModel.GetCurSel();

   if (iCurrentDevice != -1)
   {
      CString csWdmDevice = m_arWDMSources[iCurrentDevice];
      m_pAssistantDoc->ChangeWdmDevice(csWdmDevice);
      UpdateCodecPage();
   }
}

void CAVSettingsSheet::OnUpdateChkRecordVideo(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(m_bRecordVideo);

//   EnableCodecPage(m_bRecordVideo);
}

void CAVSettingsSheet::OnUpdateChkVideoMonitor(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CAVSettingsSheet::OnUpdateChkAlternativeAVSync(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CAVSettingsSheet::OnUpdateChkAVOffset(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CAVSettingsSheet::OnUpdateEdtAVOffset(CCmdUI *pCmdUI)
{
   pCmdUI->Enable((m_bRecordVideo)&&(m_bSetAvOffset));
}

void CAVSettingsSheet::OnUpdateLblAVms(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CAVSettingsSheet::OnUpdateVideoLabels(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CAVSettingsSheet::OnUpdateCmbFramerate(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CAVSettingsSheet::OnUpdateBtnWindowsDriver(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(!m_bUseVFW);
}

void CAVSettingsSheet::OnButtonWdmDv() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Project::active->ShowWdmDv();  
      UpdateCodecPage();
   }
}

void CAVSettingsSheet::OnUpdateButtonWdmDv(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (ASSISTANT::Project::active != NULL)
      bEnable = ASSISTANT::Project::active->HasWdmDv();

   pCmdUI->Enable(bEnable);
}

void CAVSettingsSheet::OnButtonWdmFormat() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Project::active->ShowWdmFormat();
      UpdateCodecPage();
   }
}

void CAVSettingsSheet::OnUpdateButtonWdmFormat(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (ASSISTANT::Project::active != NULL)
      bEnable = ASSISTANT::Project::active->HasWdmFormat();

#ifdef _STUDIO
    if (CStudioApp::IsLiveContextMode()) {
        bEnable = false;
    }
#endif

   pCmdUI->Enable(bEnable);
}


void CAVSettingsSheet::OnButtonWdmSettings() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Project::active->ShowWdmSettings();
      UpdateCodecPage();
   }
}

void CAVSettingsSheet::OnUpdateButtonWdmSettings(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (ASSISTANT::Project::active != NULL)
      bEnable = ASSISTANT::Project::active->HasWdmSettings();

#ifdef _STUDIO
    if (CStudioApp::IsLiveContextMode()) {
        bEnable = false;
    }
#endif

   pCmdUI->Enable(bEnable);
}

void CAVSettingsSheet::OnButtonWdmSource() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Project::active->ShowWdmSource();  
      UpdateCodecPage();
   }
}

void CAVSettingsSheet::OnUpdateButtonWdmSource(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (ASSISTANT::Project::active != NULL)
      bEnable = ASSISTANT::Project::active->HasWdmSource();
#ifdef _STUDIO
    if (CStudioApp::IsLiveContextMode()) {
        bEnable = false;
    }
#endif

   pCmdUI->Enable(bEnable);
}

void CAVSettingsSheet::OnAvOffset() 
{
   m_bUseAlternativeSync = !m_bUseAlternativeSync;
   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetAlternativeAvSync(m_bUseAlternativeSync);
}

void CAVSettingsSheet::OnUpdateResolutionSource(CCmdUI *pCmdUI)
{
   CString csResolution;
   if (ASSISTANT::Project::active && m_bRecordVideo)
   {
      ASSISTANT::Project::active->GetVideoResolution(m_nXResolution, m_nYResolution);
      csResolution.Format(_T("%d x %d"), m_nXResolution, m_nYResolution);
      pCmdUI->SetText(csResolution);
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->SetText(_T("0 x 0"));
      pCmdUI->Enable(FALSE);
   }
}

void CAVSettingsSheet::OnUpdateColorDepthSource(CCmdUI *pCmdUI)
{
   CString csResolution;
   if (ASSISTANT::Project::active && m_bRecordVideo)
   {
      ASSISTANT::Project::active->GetColorDepth(m_nBitPerColor);
      CString csBitPerColor;
      csBitPerColor.Format(_T("%d"), m_nBitPerColor);
      pCmdUI->SetText(csBitPerColor);
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->SetText(_T("0"));
      pCmdUI->Enable(FALSE);
   }
}

void CAVSettingsSheet::OnSelchangeComboVideoFramerate() 
{
   m_nFrameRateIndex = m_cmbFramerate.GetCurSel();
   float fFramerate = m_arFrameRates[m_nFrameRateIndex];

   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetVideoFramerate(fFramerate);
}

void CAVSettingsSheet::OnEditchangeComboVideoFramerate() 
{
   CString csFramerate;
   m_cmbFramerate.GetWindowText(csFramerate);
   TCHAR *stopstring;
   float fFramerate = _tcstod(csFramerate, &stopstring);
   
   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetVideoFramerate(fFramerate);
}

void CAVSettingsSheet::UpdateCodecPage()
{
    m_nCodecIndex = 0;
    m_arCodecs.RemoveAll();
    m_nQuality = 75;
    m_bUseKeyframes = true;
    m_nKeyframeDistance = 15;
    m_bUseDatarateAdaption = false;
    m_nDatarate = 500;

    if (m_pAssistantDoc)
    {
        m_pAssistantDoc->GetCodecSettings(m_arCodecs, m_nCodecIndex, m_nQuality, 
            m_bUseKeyframes, m_nKeyframeDistance,
            m_bUseDatarateAdaption, m_nDatarate,
            m_bHasAbout, m_bHasConfigure);
    }


    m_cmbCodecs.ResetContent();
    for (int i = 0; i < m_arCodecs.GetSize(); ++i)
        m_cmbCodecs.AddString(m_arCodecs[i]);
    m_cmbCodecs.SetCurSel(m_nCodecIndex);

    m_spnQuality.SetPos(m_nQuality);
    m_sldQuality.SetPos(m_nQuality);

    CString csText;
    m_chkKeyframe.SetCheck(m_bUseKeyframes);
    csText.Format(_T("%d"), m_nKeyframeDistance);
    m_edtKeyrame.SetWindowText(csText);

    m_chkDatarate.SetCheck(m_bUseDatarateAdaption);
    csText.Format(_T("%d"), m_nDatarate);
    m_edtDatarate.SetWindowText(csText);
}

void CAVSettingsSheet::UpdateRecordVideoHint()
{
   CString csRecordVideoHint = _T("");
   if ( m_bRecordVideo == true )
      csRecordVideoHint.LoadString(IDC_RECORD_VIDEO_HINT);
   CStatic* stRecordVideoHint = (CStatic*) GetDlgItem(IDC_RECORD_VIDEO_HINT);
   stRecordVideoHint->SetWindowText(csRecordVideoHint);
}

//Video methods end

//Codec methods begin
void CAVSettingsSheet::LocalizeCodec()
{
   DWORD dwIds[] = {IDC_STATIC_CODEC_GB_SETTINGS,
                    IDC_STATIC_CODEG_GB_QUALITY,
                    IDC_STATIC_CODEC_SETTINGS_CODEC,
                    IDC_BUTTON_CODEC_ABOUT2,
                    IDC_BUTTON_CODEC_CONFIGURE,
                    IDC_CHECK_CODEC_KEYFRAME,          
                    IDC_STATIC_KEYFRAME_DISTANCE,         
                    IDC_STATIC_KEYFRAME_FRAMES,             
                    IDC_CHECK_CODEC_DATARATE,                          
                    IDC_STATIC_DATARATE_TARGET,         
                    IDC_STATIC_DATARATE_KB,         
                    IDC_AVCS_STATIC_CODEC,
                    -1};
   MfcUtils::Localize(this, dwIds);
}

void CAVSettingsSheet::InitCodec()
{
   m_spnQuality.SetRange(0, 100);
   m_spnQuality.SetBuddy(&m_edtQuality);
   
   m_sldQuality.SetRange(0, 100);
   m_sldQuality.SetTicFreq(5);
   
   UpdatePageCodec();
}

void CAVSettingsSheet::SaveSettingsCodec()
{
}

void CAVSettingsSheet::OnDeltaposSpinQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int iPos = m_spnQuality.GetPos();
   m_sldQuality.SetPos(iPos);

   if (m_pAssistantDoc)
      m_pAssistantDoc->SetCodecQuality(iPos);

	*pResult = 0;
}

void CAVSettingsSheet::OnCustomdrawSliderQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int iPos = m_sldQuality.GetPos();
   m_spnQuality.SetPos(iPos);

   if (m_pAssistantDoc)
      m_pAssistantDoc->SetCodecQuality(iPos);

	*pResult = 0;
}

void CAVSettingsSheet::OnCheckCodecKeyframe() 
{
   m_bUseKeyframes = m_chkKeyframe.GetCheck() == 0 ? false : true;
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetUseKeyframes(m_bUseKeyframes);
}

void CAVSettingsSheet::OnCheckCodecDatarate() 
{
   m_bUseDatarateAdaption = m_chkDatarate.GetCheck() == 0 ? false : true;
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetUseDatarate(m_bUseDatarateAdaption);
}

void CAVSettingsSheet::OnUpdateChkFrames(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_pAssistantDoc)
      bEnable = m_pAssistantDoc->KeyframesPossible();

   if (!bEnable && m_chkKeyframe.GetCheck() == 1)
      m_chkKeyframe.SetCheck(0);

   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(bEnable);
   else
      pCmdUI->Enable(false);

}

void CAVSettingsSheet::OnUpdateLblDistance(CCmdUI *pCmdUI)
{
   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(m_chkKeyframe.GetCheck());
   else
      pCmdUI->Enable(false);
}

void CAVSettingsSheet::OnUpdateEdtDistance(CCmdUI *pCmdUI)
{
   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(m_chkKeyframe.GetCheck());
   else
      pCmdUI->Enable(false);
}

void CAVSettingsSheet::OnUpdateLblFrames(CCmdUI *pCmdUI)
{
   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(m_chkKeyframe.GetCheck());
   else
      pCmdUI->Enable(false);
}

void CAVSettingsSheet::OnUpdateChkTargetDatarate(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_pAssistantDoc)
      bEnable = m_pAssistantDoc->DataratePossible();

   if (!bEnable && m_chkDatarate.GetCheck() == 1)
      m_chkDatarate.SetCheck(0);

   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(bEnable);
   else
      pCmdUI->Enable(false);
}

void CAVSettingsSheet::OnUpdateLblTargetDatarate(CCmdUI *pCmdUI)
{
   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(m_chkDatarate.GetCheck());
   else
      pCmdUI->Enable(false);
}

void CAVSettingsSheet::OnUpdateEdtTargetDatarate(CCmdUI *pCmdUI)
{
   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(m_chkDatarate.GetCheck());
   else
      pCmdUI->Enable(false);
}

void CAVSettingsSheet::OnUpdateLblKBS(CCmdUI *pCmdUI)
{
   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(m_chkDatarate.GetCheck());
   else
      pCmdUI->Enable(false);
}

void CAVSettingsSheet::OnChangeEditQuality() 
{
   if (m_pAssistantDoc)
   {
      CString csText;
      m_edtQuality.GetWindowText(csText);
      m_nQuality = _ttoi(csText);
      m_pAssistantDoc->SetCodecQuality(m_nQuality);
   }
}

void CAVSettingsSheet::OnChangeEditKeyframe() 
{
   if (m_pAssistantDoc)
   {
      CString csText;
      m_edtKeyrame.GetWindowText(csText);
      m_nKeyframeDistance = _ttoi(csText);
      m_pAssistantDoc->SetKeyframes(m_nKeyframeDistance);
   }
}

void CAVSettingsSheet::OnChangeEditDatarate() 
{
   if (m_pAssistantDoc)
   {
      CString csText;
      m_edtDatarate.GetWindowText(csText);
      m_nDatarate = _ttoi(csText);
      m_pAssistantDoc->SetDatarate(m_nDatarate);
   }
}

void CAVSettingsSheet::OnSelchangeComboCodecs() 
{
   m_nCodecIndex = m_cmbCodecs.GetCurSel();

   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->SetCodec(m_arCodecs[m_nCodecIndex], false);
      if (ASSISTANT::Project::active)
      {
         m_bHasAbout = ASSISTANT::Project::active->CodecHasAbout();
         m_bHasConfigure = ASSISTANT::Project::active->CodecCanBeConfigured();
      }
   }
}

void CAVSettingsSheet::UpdatePageCodec()
{
   m_nCodecIndex = 0;
   m_arCodecs.RemoveAll();
   m_nQuality = 75;
   m_bUseKeyframes = true;
   m_nKeyframeDistance = 15;
   m_bUseDatarateAdaption = false;
   m_nDatarate = 500;

   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->GetCodecSettings(m_arCodecs, m_nCodecIndex, m_nQuality, 
                                        m_bUseKeyframes, m_nKeyframeDistance,
                                        m_bUseDatarateAdaption, m_nDatarate,
                                        m_bHasAbout, m_bHasConfigure);
   }


   m_cmbCodecs.ResetContent();
   for (int i = 0; i < m_arCodecs.GetSize(); ++i)
      m_cmbCodecs.AddString(m_arCodecs[i]);
   m_cmbCodecs.SetCurSel(m_nCodecIndex);

   m_spnQuality.SetPos(m_nQuality);
   m_sldQuality.SetPos(m_nQuality);

   CString csText;
   m_chkKeyframe.SetCheck(m_bUseKeyframes);
   csText.Format(_T("%d"), m_nKeyframeDistance);
   m_edtKeyrame.SetWindowText(csText);

   m_chkDatarate.SetCheck(m_bUseDatarateAdaption);
   csText.Format(_T("%d"), m_nDatarate);
   m_edtDatarate.SetWindowText(csText);
}


void CAVSettingsSheet::OnButtonCodecAbout() 
{
   if (ASSISTANT::Project::active)
      ASSISTANT::Project::active->ShowCodecAbout();
}

void CAVSettingsSheet::OnUpdateButtonCodecAbout(CCmdUI *pCmdUI)
{
   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(m_bHasAbout);
   else
      pCmdUI->Enable(false);

}

void CAVSettingsSheet::OnButtonCodecConfigure() 
{
   if (ASSISTANT::Project::active)
      ASSISTANT::Project::active->ShowCodecConfigure();
}

void CAVSettingsSheet::OnUpdateButtonCodecConfigure(CCmdUI *pCmdUI)
{
   if(m_chkRecordVideo.GetCheck())
      pCmdUI->Enable(m_bHasConfigure);
   else
      pCmdUI->Enable(false);
}

void CAVSettingsSheet::OnUpdateGroupSettings(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkRecordVideo.GetCheck());
}

void CAVSettingsSheet::OnUpdateStaticCodec(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkRecordVideo.GetCheck());
}

void CAVSettingsSheet::OnUpdateComboCodec(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkRecordVideo.GetCheck());
}

void CAVSettingsSheet::OnUpdateGroupQuality(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkRecordVideo.GetCheck());
}

void CAVSettingsSheet::OnUpdateSliderQuality(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkRecordVideo.GetCheck());
}

void CAVSettingsSheet::OnUpdateEdtQuality(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkRecordVideo.GetCheck());
}

void CAVSettingsSheet::OnUpdateSpinQuality(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkRecordVideo.GetCheck());
}

//Codec methods end

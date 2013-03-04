// AudioSettings.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "AudioSettings.h"
#include "AssistantDoc.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudioSettings property page

IMPLEMENT_DYNCREATE(CAudioSettings, CPropertyPage)

CAudioSettings::CAudioSettings() : CPropertyPage(CAudioSettings::IDD)
{
	//{{AFX_DATA_INIT(CAudioSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAudioSettings::~CAudioSettings()
{
}

void CAudioSettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioSettings)
	DDX_Control(pDX, IDC_AUDIO_SLIDER, m_sldVolume);
	DDX_Control(pDX, IDC_AUDIO_SOURCE, m_cmbAudioSource);
	DDX_Control(pDX, IDC_AUDIO_DEVICE, m_cmbAudioDevice);
	DDX_Control(pDX, IDC_AUDIO_SAMPLE_WIDTH, m_cmbAudioSampleWidth);
	DDX_Control(pDX, IDC_AUDIO_SAMPLE_RATE, m_cmbAudioSampleRate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAudioSettings, CPropertyPage)
	//{{AFX_MSG_MAP(CAudioSettings)
	ON_CBN_SELCHANGE(IDC_AUDIO_DEVICE, OnSelchangeAudioDevice)
	ON_CBN_SELCHANGE(IDC_AUDIO_SAMPLE_RATE, OnSelchangeAudioSampleRate)
	ON_CBN_SELCHANGE(IDC_AUDIO_SAMPLE_WIDTH, OnSelchangeAudioSampleWidth)
	ON_CBN_SELCHANGE(IDC_AUDIO_SOURCE, OnSelchangeAudioSource)
   ON_UPDATE_COMMAND_UI(IDC_AUDIO_SOURCE, OnUpdateAudioSource)
   ON_UPDATE_COMMAND_UI(IDC_AUDIO_SLIDER, OnUpdateAudioSlider)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_AUDIO_SLIDER, OnReleasedcaptureAudioSlider)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioSettings message handlers

BOOL CAudioSettings::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	DWORD dwIds[] = {IDC_STATIC_AUDIO_DEVICE,
                    IDC_STATIC_AUDIO_SAMPL_WIDTH,
                    IDC_STATIC_AUDIO_SAMPLE_RATE,
                    IDC_STATIC_AUDIO_SOURCE,
                   -1};

   MfcUtils::Localize(this, dwIds);


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
   
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAudioSettings::Init(CAssistantDoc *pAssistantDoc)
{
   m_pAssistantDoc = pAssistantDoc;

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
   }
   
}

void CAudioSettings::OnOK() 
{
   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->ActivateAudioDevice();
      m_pAssistantDoc->WriteAudioSettings();
   }


	CPropertyPage::OnOK();
}

void CAudioSettings::OnCancel() 
{
   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->ReadAudioSettings();
   }

	CPropertyPage::OnCancel();
}

void CAudioSettings::OnSelchangeAudioDevice() 
{
   if (m_pAssistantDoc != NULL)
   {
      m_arSampleWidths.RemoveAll();
      m_nSampleWidth = 0;
      
      m_arSampleRates.RemoveAll();
      m_nSampleRate = 0;
      
      m_nDevice = m_cmbAudioDevice.GetCurSel();
      m_pAssistantDoc->SetAudioDevice(m_nDevice);
      
      m_cmbAudioSampleWidth.ResetContent();
      m_pAssistantDoc->GetSampleWidths(m_nDevice, m_arSampleWidths, m_nSampleWidth);
      for (int i = 0; i < m_arSampleWidths.GetSize(); ++i)
      {
         CString csSampleWidth;
         csSampleWidth.Format(_T("%d"), m_arSampleWidths[i]);
         m_cmbAudioSampleWidth.AddString(csSampleWidth);
      }
      m_cmbAudioSampleWidth.SetCurSel(m_nSampleWidth);
      
      m_cmbAudioSampleRate.ResetContent();
      m_pAssistantDoc->GetSampleRates(m_nDevice, m_arSampleRates, m_nSampleRate);
      for (i = 0; i < m_arSampleRates.GetSize(); ++i)
      {
         CString csSampleRate;
         csSampleRate.Format(_T("%d"), m_arSampleRates[i]);
         m_cmbAudioSampleRate.AddString(csSampleRate);
      }
      m_cmbAudioSampleRate.SetCurSel(m_nSampleRate);

      m_pAssistantDoc->ActivateAudioDevice();

      m_cmbAudioSource.ResetContent();
      m_pAssistantDoc->GetAudioSources(m_arSources, m_nSource);
      for (i = 0; i < m_arSources.GetSize(); ++i)
         m_cmbAudioSource.AddString(m_arSources[i]);
      m_cmbAudioSource.SetCurSel(m_nSource);
      
      m_cmbAudioSampleWidth.ResetContent();
      m_pAssistantDoc->GetSampleWidths(m_nDevice, m_arSampleWidths, m_nSampleWidth);
      for (i = 0; i < m_arSampleWidths.GetSize(); ++i)
      {
         CString csSampleWidth;
         csSampleWidth.Format(_T("%d"), m_arSampleWidths[i]);
         m_cmbAudioSampleWidth.AddString(csSampleWidth);
      }
      m_cmbAudioSampleWidth.SetCurSel(m_nSampleWidth);
      
      m_cmbAudioSampleRate.ResetContent();
      m_pAssistantDoc->GetSampleRates(m_nDevice, m_arSampleRates, m_nSampleRate);
      for (i = 0; i < m_arSampleRates.GetSize(); ++i)
      {
         CString csSampleRate;
         csSampleRate.Format(_T("%d"), m_arSampleRates[i]);
         m_cmbAudioSampleRate.AddString(csSampleRate);
      }
      m_cmbAudioSampleRate.SetCurSel(m_nSampleRate);
   }
}

void CAudioSettings::OnSelchangeAudioSampleRate() 
{
   m_nSampleRate = m_cmbAudioSampleRate.GetCurSel();
   m_pAssistantDoc->SetSampleRate(m_nSampleRate);
}

void CAudioSettings::OnSelchangeAudioSampleWidth() 
{
   m_nSampleWidth = m_cmbAudioSampleWidth.GetCurSel();
   m_pAssistantDoc->SetSampleWidth(m_nSampleWidth);
}

void CAudioSettings::OnSelchangeAudioSource() 
{
   m_nSource = m_cmbAudioSource.GetCurSel();
   m_pAssistantDoc->SetAudioSource(m_nSource);
}

void CAudioSettings::OnUpdateAudioSource(CCmdUI *pCmdUI)
{
   if (m_arSources.GetSize() > 0)
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
   
   if (m_pAssistantDoc && GetKeyState(VK_LBUTTON) >= 0) // left mouse not key pressed)
   {
      UINT nSource = m_pAssistantDoc->GetAudioSource();
      if (nSource != m_nSource)
         m_cmbAudioSource.SetCurSel(m_nSource);
   }
}

void CAudioSettings::OnUpdateAudioSlider(CCmdUI *pCmdUI)
{
   if (m_arSources.GetSize() > 0)
   {
      m_sldVolume.ShowWindow(SW_SHOW);
      pCmdUI->Enable(true);
   }
   else
   {
      pCmdUI->Enable(false);
      m_sldVolume.ShowWindow(SW_HIDE);
   }

   if (m_pAssistantDoc && GetKeyState(VK_LBUTTON) >= 0) // left mouse not key pressed)
   {
      UINT nMixerVolume = m_pAssistantDoc->GetMixerVolume();
      SetVolume(nMixerVolume);
   }
  
}

void CAudioSettings::SetVolume(int nValue)
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

int CAudioSettings::GetVolume()
{
   return(100 - m_sldVolume.GetPos());
}

void CAudioSettings::SetVolumeRange(int nMin, int nMax)
{
   m_sldVolume.SetRange(nMin, nMax);
   m_nMinRange = nMin;
   m_nMaxRange = nMax;
   CString s;
}

void CAudioSettings::OnKickIdle()
{
   UpdateDialogControls(this, false);
}

void CAudioSettings::OnReleasedcaptureAudioSlider(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_pAssistantDoc)
   {
      UINT nCode = pNMHDR->code;


      m_pAssistantDoc->SetMixerVolume(GetVolume());
   }

	*pResult = 0;
}

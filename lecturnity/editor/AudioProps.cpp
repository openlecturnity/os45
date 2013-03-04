// AudioProps.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AudioProps.h"
//#include "editor.h"
#include "MainFrm.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CAudioProps 

IMPLEMENT_DYNCREATE(CAudioProps, CPropertyPage)

CAudioProps::CAudioProps() : CPropertyPage(CAudioProps::IDD)
{
	//{{AFX_DATA_INIT(CAudioProps)
	//}}AFX_DATA_INIT
}

CAudioProps::~CAudioProps()
{
}

void CAudioProps::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioProps)
	DDX_Control(pDX, IDC_SAMPLESIZE, m_sampleSizeCombo);
	DDX_Control(pDX, IDC_MONOSTEREO, m_monoStereoCombo);
	DDX_Control(pDX, IDC_SAMPLERATE, m_sampleRateCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAudioProps, CPropertyPage)
	//{{AFX_MSG_MAP(CAudioProps)
	ON_CBN_SELCHANGE(IDC_SAMPLERATE, OnChangeAudioSetting)
	ON_CBN_SELCHANGE(IDC_SAMPLESIZE, OnChangeAudioSetting)
	ON_CBN_SELCHANGE(IDC_MONOSTEREO, OnChangeAudioSetting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CAudioProps 

BOOL CAudioProps::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
   DWORD dwIds[] = {IDC_GROUP_SAMPLE_OPTIONS,
                    IDC_LABEL_SAMPLERATE,
                    IDC_LABEL_SAMPLE_WIDTH,
                    IDC_LABEL_SPS,
                    IDC_LABEL_BPS,
                    -1};
   MfcUtils::Localize(this, dwIds);
	// TODO: Zusätzliche Initialisierung hier einfügen
   m_pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();

   ReadDefaultSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CAudioProps::ReadDefaultSettings()
{
   if (!m_pDoc->project.m_isAudioFormatInitialized)
      m_pDoc->project.InitMediaTypes();

   AM_MEDIA_TYPE *pMT = &m_pDoc->project.m_mtAudioFormat;

   // Now check if we have an acceptable format
   if (!(pMT->majortype == MEDIATYPE_Audio &&
         pMT->subtype   == MEDIASUBTYPE_PCM &&
         pMT->formattype == FORMAT_WaveFormatEx))
   {
      // Warn user, then set to defaults
      CString msg;
      msg.LoadString(IDS_MSG_INVALIDAUDIOTYPE);
      CString cap;
      cap.LoadString(IDS_WARNING_E);
      MessageBox(msg, cap, MB_ICONEXCLAMATION | MB_OK);

      FreeMediaType(*pMT);
      ZeroMemory(pMT, sizeof AM_MEDIA_TYPE);
      pMT->bFixedSizeSamples = TRUE;
      pMT->bTemporalCompression = FALSE;
      pMT->majortype = MEDIATYPE_Audio;
      pMT->subtype = MEDIASUBTYPE_PCM;
      pMT->lSampleSize = 2;
      pMT->pUnk = NULL;
      pMT->formattype = FORMAT_WaveFormatEx;

      WAVEFORMATEX *pWfe = (WAVEFORMATEX *) CoTaskMemAlloc(sizeof WAVEFORMATEX);
      if (!pWfe)
      {
         CString msg;
         msg.LoadString(IDS_MSG_MEMORY);
         CString cap;
         cap.LoadString(IDS_ERROR_E);
         MessageBox(msg, cap, MB_ICONERROR | MB_OK);

         EnableWindow(FALSE);
         m_pDoc->project.m_isAudioFormatInitialized = false;
         return;
      }

      pWfe->cbSize = 0;
      pWfe->nSamplesPerSec = 22050;
      pWfe->nChannels = 1;
      pWfe->wFormatTag = WAVE_FORMAT_PCM;
      pWfe->nBlockAlign = 2;
      pWfe->wBitsPerSample = 16;
      pWfe->nAvgBytesPerSec = 44100;

      pMT->cbFormat = sizeof WAVEFORMATEX;
      pMT->pbFormat = (BYTE *) pWfe;

      m_pDoc->project.m_isAudioFormatInitialized = true;
   }

   WAVEFORMATEX *pWfe = (WAVEFORMATEX *) pMT->pbFormat;
   int sampleRate = pWfe->nSamplesPerSec;
   int sampleSize = pWfe->wBitsPerSample;
   bool stereo    = pWfe->nChannels == 2 ? true : false;

   // -----

   int sampleRateIndex = 3;
   bool sampleRateFound = false;
   for (int i=0; i<m_sampleRateCombo.GetCount() && !sampleRateFound; ++i)
   {
      CString sampleRateText;
      m_sampleRateCombo.GetLBText(i, sampleRateText);
      int sr = _ttoi(sampleRateText);
      if (sr == sampleRate)
      {
         sampleRateIndex = i;
         sampleRateFound = true;
      }
   }

   m_sampleRateCombo.SetCurSel(sampleRateIndex);

   // -----

   int sampleSizeIndex = 1;
   bool sampleSizeFound = false;
   for (i=0; i<m_sampleSizeCombo.GetCount() && !sampleSizeFound; ++i)
   {
      CString sampleSizeText;
      m_sampleSizeCombo.GetLBText(i, sampleSizeText);
      int ss = _ttoi(sampleSizeText);
      if (ss == sampleSize)
      {
         sampleSizeIndex = i;
         sampleSizeFound = true;
      }
   }

   m_sampleSizeCombo.SetCurSel(sampleSizeIndex);

   // -----

   if (stereo)
      m_monoStereoCombo.SetCurSel(1); // "Stereo"
   else
      m_monoStereoCombo.SetCurSel(0); // "Mono"
}

void CAudioProps::OnOK() 
{
	// Now we only have to transfer the new settings
   // to the AM_MEDIA_TYPE variable for the audio.
   AM_MEDIA_TYPE *pMT = &m_pDoc->project.m_mtAudioFormat;
   WAVEFORMATEX *pWfe = (WAVEFORMATEX *) pMT->pbFormat;

   // Sample rate first
   int sampleRateIndex = m_sampleRateCombo.GetCurSel();
   CString sampleRateText;
   m_sampleRateCombo.GetLBText(sampleRateIndex, sampleRateText);
   int sampleRate = _ttoi(sampleRateText);

   int sampleSizeIndex = m_sampleSizeCombo.GetCurSel();
   CString sampleSizeText;
   m_sampleSizeCombo.GetLBText(sampleSizeIndex, sampleSizeText);
   int sampleSize = _ttoi(sampleSizeText);

   bool stereo = m_monoStereoCombo.GetCurSel() == 1 ? true : false;

   pWfe->nSamplesPerSec  = sampleRate;
   pWfe->wBitsPerSample  = sampleSize;
   pWfe->nChannels       = stereo ? 2 : 1;
   pWfe->nBlockAlign     = pWfe->nChannels * pWfe->wBitsPerSample / 8;
   pWfe->nAvgBytesPerSec = pWfe->nSamplesPerSec * pWfe->nBlockAlign;

   pMT->lSampleSize      = pWfe->nBlockAlign;

   // The document has changed, as the audio export format
   // has changed. 
   m_pDoc->SetModifiedFlag();

   // Rebuild the preview after the dialog has closed
   m_pDoc->SetPreviewRebuildNeeded();

	CPropertyPage::OnOK();
}

void CAudioProps::OnChangeAudioSetting() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen

   SetModified();
}

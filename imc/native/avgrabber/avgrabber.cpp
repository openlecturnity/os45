#define __AVGRABBER_CPP__

#pragma warning(disable:4786)
#include "avgrabber.h"
#include "global.h"
#include "resource.h"
#include "LiveContext.h"

#include "LanguageSupport.h"

using namespace std;

static int s_iLogMode = -1;

void AvGrabberDebugMsg(char *szMessage, bool bPrefix, int iValue) {
    static FILE *fileDebug = NULL;
    static bool bLogCreateErrorShown = false;

    if (s_iLogMode == -1) {
        DWORD dwLength = ::GetEnvironmentVariableA("IMC_AV_DEBUG", NULL, 0);       
        s_iLogMode = dwLength > 0 ? 1 : 0;
    }

    bool bShouldDebug = s_iLogMode > 0;

    //if (sCommand.Find(_T("rundll32")) > -1)
    //    bShouldDebug = true;

    if (!bShouldDebug)
        return;
    
    if (bLogCreateErrorShown)
        return;

    if (fileDebug == NULL) {
        char szLogPath[MAX_PATH];
        szLogPath[0] = 0;
        DWORD dwLength = ::GetEnvironmentVariableA("IMC_AV_DEBUG", szLogPath, MAX_PATH);

        fileDebug = fopen(szLogPath, "w");

        if (fileDebug == NULL) {
            ::MessageBoxA(NULL, "Log file could not be created.", "Error", MB_OK | MB_ICONERROR);

            bLogCreateErrorShown = true;
            return;
        }
    }

    if (bPrefix)
        fprintf(fileDebug, "%05d - Thread 0x%08x: ", ::timeGetTime()%100000, GetCurrentThreadId());

    fprintf(fileDebug, szMessage);

    if (iValue != 0)
        fprintf(fileDebug, "Value: %d\n", iValue);

    fflush(fileDebug);
}
void AvGrabberDebugMsg(char *szMessage, bool bPrefix) {
    AvGrabberDebugMsg(szMessage, bPrefix, 0);
}

void AvGrabberDebugMsg(char *szMessage) {
    AvGrabberDebugMsg(szMessage, true, 0);
}


// Global variable, see "global.h"
HMODULE g_hDllInstance = NULL;

BEGIN_MESSAGE_MAP(CAvgrabberDllApp, CWinApp)
END_MESSAGE_MAP()

CAvgrabberDllApp theApp;

CAvgrabberDllApp::CAvgrabberDllApp( )
{
}

BOOL CAvgrabberDllApp::InitInstance( )
{
	CWinApp::InitInstance();

    CLanguageSupport::SetThreadLanguage();

    g_hDllInstance = m_hInstance;
    
    return TRUE;
}

/* ================================================== */
/* ================================================== */
/* ================================================== */

// Use this method to retrieve the currently installed DirectX
// version. AvGrabber likes to have DirectX >= 8.
extern DWORD GetDXVersion();

/* ================================================== */
/* ================================================== */
/* ================================================== */

void AVGRABBER::HotKey::Copy(AVGRABBER::HotKey &hotKey) 
{
    hotkeyName = hotKey.hotkeyName;
    hasCtrl = hotKey.hasCtrl;
    hasShift = hotKey.hasShift;
    hasAlt = hotKey.hasAlt;
    vKey = hotKey.vKey;
    vKeyString = hotKey.vKeyString ;
    id = 0;
}

/* ================================================== */
/* ================================================== */
/* ================================================== */

AvGrabber::AvGrabber(CLiveContext *pLiveContext)
{

    AvGrabberDebugMsg("AvGrabber Startup\n");

    audio_       = NULL;
    audioMixer_  = NULL;
    video_       = NULL;
    wdm_         = NULL;
    screen_      = NULL;
    cpuMeter_    = NULL;

    cpuMeter_    = new CPUUsageReader();

    inProgress_  = false;
    enableVideo_ = false;
    isPaused_    = true;
    displayMonitor_ = false;
    dryCapture_  = false;

    enableMixer_ = true;

    if (s_iLogMode > 0) {
        if ((_access("C:\\nomixer.txt", 0)) != -1)
            enableMixer_ = false;
    }

    dwLastAccelerationLevel_ = 0x0; // full acceleration
    disableHwAcceleration_   = false; // don't turn off video accel by default


    nSgVideoMode_ = AV_NULL_ON_SG;

    initScreenCapturer(pLiveContext);
}

AvGrabber::~AvGrabber()
{

    AvGrabberDebugMsg("~AvGrabber Shutdown\n");


    if (audioMixer_ != NULL)
        delete audioMixer_;
    audioMixer_ = NULL;
    if (audio_ != NULL)
    {
        if (dryCapture_)
        {
            audio_->stop();
            dryCapture_ = false;
        }

        delete audio_;
        audio_ = NULL;
    }
    if (video_ != NULL)
        delete video_;
    video_ = NULL;
    if (wdm_ != NULL)
        delete wdm_;
    wdm_ = NULL;
    if (screen_ != NULL)
        delete screen_;
    screen_ = NULL;

    if (cpuMeter_ != NULL)
        delete cpuMeter_;
    cpuMeter_ = NULL;
}

/* ================================================== */
/* ================================================== */
/* ================================================== */

long AvGrabber::getRecordingTime()
{

  AvGrabberDebugMsg("getRecordingTime()\n");

   if (audio_)
	  return audio_->getWaveInTimeMs();
   else
       return 0;
  
}

void AvGrabber::start(_TCHAR* tszFileName, char* szLadInfo)
{

  AvGrabberDebugMsg("start()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (audio_ == NULL)
		throw AVGrabberException(Local::localizeString(AV_NOAUDIO));

	_tcscpy(szFileName_, tszFileName);
   if (szLadInfo != NULL)
      strcpy(szLadInfo_, szLadInfo);

	_TCHAR wavFile[256];
	_TCHAR aviFile[256];

	_tcscpy(wavFile, tszFileName);
	_tcscpy(aviFile, tszFileName);
   if (szLadInfo == NULL)
   	_tcscat(wavFile, _T(".wav"));
   else
      _tcscat(wavFile, _T(".lad"));
	_tcscat(aviFile, _T(".avi"));

   // if we are in dry capture mode; stop that first
   if (dryCapture_)
   {
      audio_->stop();
      dryCapture_ = false;
   }

   if (disableHwAcceleration_)
   {
      dwLastAccelerationLevel_ = hwAccelManager_.GetAccelerationLevel();
      hwAccelManager_.SetAccelerationLevel(0x5);
   }

   try
   {
      // First, start video or wdm capture process
	   if (video_ != NULL && enableVideo_)
		   video_->start(aviFile, audio_);
      else if (wdm_ != NULL && enableVideo_)
         wdm_->start(aviFile, audio_);
   }
   catch (exception &)
   {
      // If something goes wrong, reenable hardware acceleration first
      // before bailing out.
      if (disableHwAcceleration_)
      {
         hwAccelManager_.SetAccelerationLevel(dwLastAccelerationLevel_);
      }

      throw;
   }

   try
   {
      // Then, start audio capture.
      if (szLadInfo != NULL)
         audio_->useLadFormat(szLadInfo_);
      else
         audio_->useLadFormat(NULL);

	   audio_->start(wavFile);
   }
   catch (exception &e)
   {
      cout << "--- Exception [AvGrabber::start()]: " << e.what() << endl;
      // if something went wrong here, we need to stop the video 
      // process (if available)!!
      if (video_ != NULL && enableVideo_)
         video_->stop();
      else if (wdm_ != NULL && enableVideo_)
         wdm_->stop();

      // If something goes wrong, reenable hardware acceleration first
      // before bailing out.
      if (disableHwAcceleration_)
      {
         hwAccelManager_.SetAccelerationLevel(dwLastAccelerationLevel_);
      }
      throw;
   }

	inProgress_ = true;
	isPaused_   = false;
}

_TCHAR* AvGrabber::getFileNamePrefix()
{

  AvGrabberDebugMsg("getFileNamePrefix()\n");

	return szFileName_;
}

void AvGrabber::stop()
{

  AvGrabberDebugMsg("stop()\n");

   if (!inProgress_)
      return;

   if (audio_ == NULL)
	  throw AVGrabberException(Local::localizeString(AV_NOAUDIO));

	if (video_ != NULL && enableVideo_)
		video_->finalizeSync();
   else if (wdm_ != NULL && enableVideo_)
      wdm_->finalizeSync();

   if (screen_ != NULL && isGrabbingScreen_)
      stopScreenGrabbingClip();

	audio_->stop();

	if (video_ != NULL && enableVideo_)
		video_->stop();
   else if (wdm_ != NULL && enableVideo_)
      wdm_->stop();

	inProgress_ = false;
	isPaused_   = true;

   if (disableHwAcceleration_)
   {
      hwAccelManager_.SetAccelerationLevel(dwLastAccelerationLevel_);
   }

   if (audio_ != NULL)
   {
      dryCapture_ = true;
      audio_->start(NULL);
   }
}

void AvGrabber::WaitClickElementInfo(CSingleProgressDialog *pProgress) {
    if (screen_ != NULL)
        screen_->WaitClickElementInfo(pProgress);
}

void AvGrabber::pause()
{

  AvGrabberDebugMsg("pause()\n");

	if (isPaused_)
	{
		cerr << "AvGrabber: Can't pause, already paused." << endl;
		return;
	}

	if (!inProgress_)
	{
		cerr << "AvGrabber: Can't pause, not in progress." << endl;
		return;
	}

	if (audio_ == NULL)
		throw AVGrabberException(Local::localizeString(AV_NOAUDIO));

	if (video_ != NULL && enableVideo_)
		video_->pause();
   else if (wdm_ != NULL && enableVideo_)
      wdm_->pause();

   if (screen_ != NULL && isGrabbingScreen_)
      screen_->pause();
	audio_->pause();

	isPaused_ = true;
}

void AvGrabber::unPause()
{

  AvGrabberDebugMsg("unPause()\n");

	if (!isPaused_)
		throw AVGrabberException(Local::localizeString(AV_NOT_PAUSED));

	if (audio_ == NULL)
		throw AVGrabberException(Local::localizeString(AV_NOAUDIO));

	if (video_ != NULL && enableVideo_)
		video_->unPause();
   else if (wdm_ != NULL && enableVideo_)
      wdm_->unPause();

   if (screen_ != NULL && isGrabbingScreen_)
      screen_->unPause();
	audio_->unPause();

	isPaused_ = false;
}

int AvGrabber::getCpuUsage()
{

  AvGrabberDebugMsg("getCpuUsage()\n");

   if (cpuMeter_ != NULL)
      return cpuMeter_->read();

   return -1;
}

// ////////////////////////////////
//
//    A U D I O   M E T H O D S
//
// ////////////////////////////////

AudioMixer* AvGrabber::setAudioDevice(int nDeviceID, 
                                      int sampleRate, 
                                      int bitsPerSample,
                                      MIXERCALLBACK mixerCallback)
{

    AvGrabberDebugMsg("setAudioDevice()\n", true, nDeviceID);

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (audio_ != NULL)
   {
      if (dryCapture_)
      {
         audio_->stop();
         dryCapture_ = false;
      }
		delete audio_;
		audio_ = NULL;
   }

	if (audioMixer_ != NULL)
		delete audioMixer_;
   audioMixer_ = NULL;

	Sleep(100);

   audioDataRate_ = sampleRate * bitsPerSample / 8; // always mono


	audio_ = new AudioCapturer(nDeviceID, sampleRate, bitsPerSample, 1);

   if (enableMixer_)
	   audioMixer_ = new AudioMixer(nDeviceID, mixerCallback);

   // start the dry capture mode:
   if (audio_)
      audio_->start(NULL);
   dryCapture_ = true;

   AvGrabberDebugMsg("setAudioDevice() Return\n", true, (int)audioMixer_);

	return audioMixer_;
}

int AvGrabber::getAudioDeviceCount()
{

   AvGrabberDebugMsg("getAudioDeviceCount()\n");

   
   return AudioCapturer::getDeviceCount();
}

void AvGrabber::getAudioDevices(AudioDevice* deviceList[])
{

  AvGrabberDebugMsg("getAudioDevices()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	int devCount = getAudioDeviceCount();

   // Note 8 and 16 khz are not offered
   // 16 khz lead to some (now unknown) problems earlier
   // and both are not supported by FLash Video
	int sampleRateCount = 3;
	int sampleRates[] = {11025, 22050, 44100};

	int bitsPerSampleCount = 2;
	int bitsPerSamples[] = {8, 16};

   // init the variables in deviceList:
   for (int i=0; i<devCount; ++i)
   {
      deviceList[i]->nBitRates = 0;
      deviceList[i]->nSampleRates = 0;
   }

	WAVEINCAPS** waveCaps = new WAVEINCAPS*[devCount];
	for (i=0; i<devCount; ++i)
		waveCaps[i] = new WAVEINCAPS;

	AudioCapturer::getDeviceList(waveCaps);

	for (i=0; i<devCount; i++)
	{
		// copy the name of the device:
		_tcscpy(deviceList[i]->tszName, waveCaps[i]->szPname);

		// query device:
		WAVEFORMATEX waveFEx;

		waveFEx.cbSize          = sizeof(WAVEFORMATEX);
		waveFEx.wFormatTag      = WAVE_FORMAT_PCM;
		waveFEx.nSamplesPerSec  = 22050;
		waveFEx.nChannels       = 1; // always mono
		waveFEx.wBitsPerSample  = 16; // for now
		waveFEx.nBlockAlign     = 2; // == 16 * 1 / 8
		waveFEx.nAvgBytesPerSec = waveFEx.nSamplesPerSec * waveFEx.nBlockAlign;

		// first, check bits per sample with 22050 hz
		for (int j=0; j<bitsPerSampleCount; ++j)
		{
			waveFEx.wBitsPerSample = bitsPerSamples[j];
			waveFEx.nBlockAlign    = bitsPerSamples[j] / 8; // either 1 or 2

			MMRESULT res = waveInOpen(NULL, i, &waveFEx, CALLBACK_NULL, NULL, WAVE_FORMAT_QUERY);
			if (res == MMSYSERR_NOERROR)
         {
				deviceList[i]->bitRates[deviceList[i]->nBitRates++] = bitsPerSamples[j];
         }
		}

		// now set bits per sample to 16
		waveFEx.wBitsPerSample = 16;
		waveFEx.nBlockAlign    = 2; // for 16 bits

		// check the sample rates:
		for (j=0; j<sampleRateCount; ++j)
		{
			waveFEx.nSamplesPerSec  = sampleRates[j];
			waveFEx.nAvgBytesPerSec = sampleRates[j] * waveFEx.nBlockAlign;

			MMRESULT res = waveInOpen(NULL, i, &waveFEx, CALLBACK_NULL, NULL, WAVE_FORMAT_QUERY);
			if (res == MMSYSERR_NOERROR)
         {
				deviceList[i]->sampleRates[deviceList[i]->nSampleRates++] = sampleRates[j];
         }
		}
	}

   for (i=0; i<devCount; ++i)
   {
      delete[] waveCaps[i];
      waveCaps[i] = NULL;
   }
   delete[] waveCaps;
   waveCaps = NULL;
}

int AvGrabber::getAudioLevel()
{

  AvGrabberDebugMsg("getAudioLevel()\n");

   if (audio_ != NULL)
      return audio_->getMaxAudioLevel();
   else
      return 0;
}

long AvGrabber::getAudioDataRate() 
{ 

  AvGrabberDebugMsg("getAudioDataRate()\n");

   return (audio_ != NULL) ? audioDataRate_ : 0; 
}

int AvGrabber::getAudioState()
{ 

  AvGrabberDebugMsg("getAudioState()\n");

   return (audio_ != NULL)? audio_->getState() : -1; 
}

void AvGrabber::setAudioSilenceTimeout(int millis) 
{ 

  AvGrabberDebugMsg("setAudioSilenceTimeout()\n");

   AudioCapturer::setSilenceTimeout(millis); 
}

int AvGrabber::getAudioSilenceTimeout()
{

  AvGrabberDebugMsg("getAudioSilenceTimeout()\n");

   return AudioCapturer::getSilenceTimeout();
}

void AvGrabber::setEnableMixer(bool b) 
{ 

  AvGrabberDebugMsg("setEnableMixer()\n");

   enableMixer_ = b; 
}

// ////////////////////////////////
//
//    V I D E O   M E T H O D S
//
// ////////////////////////////////

int AvGrabber::getVideoDeviceCount()
{

   AvGrabberDebugMsg("getVideoDeviceCount()\n");

   
   return VideoCapturer::getDeviceListSize();
}

void AvGrabber::getVideoDeviceList(_TCHAR **atszList, int maxLength)
{

  AvGrabberDebugMsg("getVideoDeviceList()\n");

  
  VideoCapturer::getDeviceList(atszList, maxLength);
}

void AvGrabber::getVideoDeviceParameters(VideoDevice &videoDevice)
{

  AvGrabberDebugMsg("getVideoDeviceParameters()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL)
	{
		CAPDRIVERCAPS driverCaps;
      video_->getDriverCaps(&driverCaps);

		videoDevice.nDeviceID     = -1;
		_tcscpy(videoDevice.tszName, _T("<Diesen String sollte man nicht sehen!>"));
		videoDevice.bHasFormatDlg  = (driverCaps.fHasDlgVideoFormat == TRUE);
		videoDevice.bHasSourceDlg  = (driverCaps.fHasDlgVideoSource == TRUE);
		videoDevice.bHasDisplayDlg = (driverCaps.fHasDlgVideoDisplay == TRUE);
		videoDevice.isEnabled	   = true;
   }
   else
      throw VideoException(Local::localizeString(AV_NOVIDEO));
}

/*
void AvGrabber::getVideoDevices(VideoDevice* deviceList[])
{
	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	vector<string> deviceNames;
	VideoCapturer::getDeviceList(deviceNames);

	for (int i=0; i<deviceNames.size(); ++i)
	{
#ifdef DEBUG
		cout << "-+-+-+-+-+-+-+-+-+-+-+- checking device " << deviceNames.at(i) << endl;
#endif

		VideoDevice* vd = deviceList[i];
		VideoCapturer* vc = NULL;

		try
		{
			vc = new VideoCapturer(i, false);

			CAPDRIVERCAPS driverCaps;
			vc->getDriverCaps(&driverCaps);

			vd->nDeviceID     = i;
			strcpy(vd->szName, deviceNames.at(i).c_str());
			vd->bHasFormatDlg  = (driverCaps.fHasDlgVideoFormat == TRUE);
			vd->bHasSourceDlg  = (driverCaps.fHasDlgVideoSource == TRUE);
			vd->bHasDisplayDlg = (driverCaps.fHasDlgVideoDisplay == TRUE);
			vd->isEnabled	   = true;

			delete vc;
		}
		catch (exception e)
		{
			if (vc != NULL)
				delete vc;

			vd->nDeviceID	   = i;
			strcpy(vd->szName, Local::localizeString(VIDEO_DEV_ERROR));
			vd->bHasDisplayDlg = false;
			vd->bHasFormatDlg  = false;
			vd->bHasSourceDlg  = false;
         vd->isEnabled      = false;

			cerr << "Exception: " << e.what() << endl;
		}
	}
}
*/

void AvGrabber::setVideoDevice(int nDeviceID)
{

  AvGrabberDebugMsg("setVideoDevice()\n", true, nDeviceID);

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

   /*
   if (screen_ != NULL)
   {
      // TODO: Something better...
      MessageBox(NULL, "Cannot set video device if Screen Capturer exists.",
                 "Error", MB_OK | MB_ICONEXCLAMATION);
      throw AVGrabberException(Local::localizeString(VIDEO_DEV_NOCONNECT));
   }
   */

	if (video_ != NULL)
		delete video_;
	video_ = NULL;

	video_ = new VideoCapturer(nDeviceID, displayMonitor_);
}

void AvGrabber::deleteVideoDevice()
{

  AvGrabberDebugMsg("deleteVideoDevice()\n");

   if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

   if (video_ != NULL)
      delete video_;
   video_ = NULL;
}

///////////////////////////////////
//
//   W D M   M E T H O D S
//
///////////////////////////////////

bool AvGrabber::isWdmEnabled()
{

  AvGrabberDebugMsg("isWdmEnabled()\n");

   DWORD dxVer = GetDXVersion();

   if (dxVer >= 0x801)
   {
      return true;
   }

   return false;
}

int AvGrabber::getWdmDeviceCount()
{

  AvGrabberDebugMsg("getWdmDeviceCount()\n");

   HRESULT hr;

   int nDeviceCount = 0;

   hr = WdmCapturer::GetWdmSourcesCount(&nDeviceCount);

   if (SUCCEEDED(hr))
   {
      return nDeviceCount;
   }
   else
   {
      _TCHAR t[256];
      _stprintf(t, Local::localizeString(WDM_ERR_DEVICES), hr);
      throw AVGrabberException(t);
   }
}

void AvGrabber::getWdmDeviceNames(_TCHAR **paDevices, int maxLen)
{

  AvGrabberDebugMsg("getWdmDeviceNames()\n");

   HRESULT hr;

   hr = WdmCapturer::GetWdmSources(paDevices, maxLen);

   if (FAILED(hr))
   {
      _TCHAR t[256];
      _stprintf(t, Local::localizeString(WDM_ERR_DEVICES), hr);
      throw AVGrabberException(t);
   }
}

void AvGrabber::setWdmVideoDevice(_TCHAR *tszDeviceName)
{

  AvGrabberDebugMsg("setWdmVideoDevice()\n", true, _tcslen(tszDeviceName));

   if (video_ != NULL)
      throw AVGrabberException(Local::localizeString(WDM_VFW_EXISTS));
	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

   if (wdm_ != NULL)
      delete wdm_;
   wdm_ = NULL;
   wdm_ = new WdmCapturer(tszDeviceName);
}

void AvGrabber::deleteWdmDevice()
{

  AvGrabberDebugMsg("deleteWdmDevice()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));
   if (wdm_ == NULL)
      return;

   delete wdm_;
   wdm_ = NULL;
}

bool AvGrabber::isWdmDvDevice()
{

  AvGrabberDebugMsg("isWdmDvDevice()\n");

   if (wdm_ == NULL)
      return false;

   return wdm_->IsDvSource();
}

int AvGrabber::getWdmInterfaceCount()
{

  AvGrabberDebugMsg("getWdmInterfaceCount()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));
   if (wdm_ == NULL)
      throw AVGrabberException(Local::localizeString(WDM_ERR_NODEVICE));

   return wdm_->GetInterfaceCount();
}

void AvGrabber::getWdmInterfaces(WDM_INTERFACE *paWdmInterfaces)
{

  AvGrabberDebugMsg("getWdmInterfaces()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));
   if (wdm_ == NULL)
      throw AVGrabberException(Local::localizeString(WDM_ERR_NODEVICE));

   wdm_->GetInterfaces(paWdmInterfaces);
}

void AvGrabber::displayWdmConfigurationDialog(WDM_INTERFACE_ID wdmId, HWND hWnd)
{

  AvGrabberDebugMsg("displayWdmConfigurationDialog()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));
   if (wdm_ == NULL)
      throw AVGrabberException(Local::localizeString(WDM_ERR_NODEVICE));

   wdm_->DisplayConfigurationDialog(wdmId, hWnd);
}

void AvGrabber::setWdmDefaultDvParameters(DVRES dvRes, bool bRgb)
{

  AvGrabberDebugMsg("setWdmDefaultDvParameters()\n");

   WdmCapturer::SetDefaultDvParameters(dvRes, bRgb);
}

void AvGrabber::setWdmConvertDvDataToRgb(bool bRgb)
{

  AvGrabberDebugMsg("setWdmConvertDvDataToRgb()\n");

   if (wdm_ == NULL)
      return;

   wdm_->SetConvertDvToRgb(bRgb);
}

bool AvGrabber::getWdmConvertDvDataToRgb()
{

  AvGrabberDebugMsg("getWdmConvertDvDataToRgb()\n");

   if (wdm_ == NULL)
      return false;

   return wdm_->GetConvertDvToRgb();
}

void AvGrabber::setWdmDvResolution(DVRES dvRes)
{

  AvGrabberDebugMsg("setWdmDvResolution()\n");

   if (wdm_ == NULL)
      return;

   wdm_->SetDvResolution(dvRes);
}

DVRES AvGrabber::getWdmDvResolution()
{

  AvGrabberDebugMsg("getWdmDvResolution()\n");

   if (wdm_ == NULL)
      return DVRES_HALF;

   return wdm_->GetDvResolution();
}

void AvGrabber::setWdmStreamConfigData(char *pAmMediaType, int cbAmMedia,
                                       char *pVideoHeader, int cbVideoHeader)
{

  AvGrabberDebugMsg("setWdmStreamConfigData()\n");

   if (wdm_ == NULL)
      return;

   wdm_->SetStreamConfigData(pAmMediaType, cbAmMedia,
                             pVideoHeader, cbVideoHeader);
}

void AvGrabber::getWdmStreamConfigData(char *pAmMediaType, int cbAmMedia,
                                       char *pVideoHeader, int cbVideoHeader)
{

  AvGrabberDebugMsg("getWdmStreamConfigData()\n");

   if (wdm_ == NULL)
      return;

   wdm_->GetStreamConfigData(pAmMediaType, cbAmMedia,
                             pVideoHeader, cbVideoHeader);
}

int  AvGrabber::getWdmStreamConfigMediaTypeSize()
{

  AvGrabberDebugMsg("getWdmStreamConfigMediaTypeSize()\n");

   if (wdm_ == NULL)
      return -1;

   return wdm_->GetStreamConfigMediaTypeSize();
}

int  AvGrabber::getWdmStreamConfigVideoHeaderSize()
{

  AvGrabberDebugMsg("getWdmStreamConfigVideoHeaderSize()\n");

   if (wdm_ == NULL)
      return -1;

   return wdm_->GetStreamConfigVideoHeaderSize();
}

///////////////////////////////////
//
//   GENERAL VIDEO METHODS
//
///////////////////////////////////

void AvGrabber::setEnableVideo(bool bEnable)
{

    AvGrabberDebugMsg("setEnableVideo()\n", true, bEnable ? 1 : -1);

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	enableVideo_ = bEnable;
}

bool AvGrabber::getEnableVideo() 
{ 

  AvGrabberDebugMsg("getEnableVideo()\n");

   return enableVideo_; 
}

void AvGrabber::setVideoMonitor(bool bDisplay)
{

  AvGrabberDebugMsg("setVideoMonitor()\n");

	displayMonitor_ = bDisplay;
	if (video_ != NULL)
		video_->displayMonitor(bDisplay);
   if (wdm_ != NULL)
      wdm_->displayMonitor(bDisplay);
}

void AvGrabber::displayVideoDisplayDialog()
{

  AvGrabberDebugMsg("displayVideoDisplayDialog()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL)
	{
		if (video_->hasDisplayDialog())
			video_->displayDisplayDialog();
	}
}

void AvGrabber::getVideoSettings(VideoSettings &videoSettings)
{

  AvGrabberDebugMsg("getVideoSettings()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL || wdm_ != NULL)
	{
      // Note: The getVideoFormat() method only retrieves the first
      // part of the BITMAPINFO structure. Any possible RGBQUAD
      // entries will _not_ be copied into the parameter.
		BITMAPINFO videoFormat;
      if (video_ != NULL)
   		video_->getVideoFormat(&videoFormat);
      else
         wdm_->getVideoFormat(&videoFormat);

		videoSettings.colorDepth = videoFormat.bmiHeader.biBitCount;
		videoSettings.width      = videoFormat.bmiHeader.biWidth;
		videoSettings.height     = videoFormat.bmiHeader.biHeight;
	}
   else
	{
		videoSettings.colorDepth = -1;
		videoSettings.height     = -1;
		videoSettings.width      = -1;
	}
}

long AvGrabber::setVideoFrameRate(float reqFrameRate)
{

  AvGrabberDebugMsg("setVideoFrameRate()\n", true, (int)reqFrameRate);

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

   if (video_ != NULL)
		return video_->setFrameRate(reqFrameRate);
   else if (wdm_ != NULL)
      return wdm_->setFrameRate(reqFrameRate);

	return -1;
}

long AvGrabber::getVideoUSecsPerFrame()
{

  AvGrabberDebugMsg("getVideoUSecsPerFrame()\n");

	if (video_ != NULL)
		return video_->getUSecsPerFrame();
   else if (wdm_ != NULL)
      return wdm_->getUSecsPerFrame();
   //if (screen_ != NULL)
   //   return screen_->getUSecsPerFrame();
	
	return -1;
}

long AvGrabber::getVideoOffset()
{

  AvGrabberDebugMsg("getVideoOffset()\n");

	if (video_ != NULL)
		return video_->getVideoAudioOffset();
   else if (wdm_ != NULL)
      return wdm_->getVideoAudioOffset();
   //if (screen_ != NULL)
   //   return screen_->getVideoAudioOffset();

	return 0;
}

void AvGrabber::setUseFixedAudioVideoOffset(bool bFixedOffset, long nOffsetMs)
{

   AvGrabberDebugMsg("setUseFixedAudioVideoOffset()\n", true, (int)nOffsetMs);

   if (video_ != NULL)
      video_->setUseFixedAudioVideoOffset(bFixedOffset, nOffsetMs);
   else if (wdm_ != NULL)
      wdm_->setUseFixedAudioVideoOffset(bFixedOffset, nOffsetMs);
}

void AvGrabber::displayVideoFormatDialog()
{

  AvGrabberDebugMsg("displayVideoFormatDialog()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL)
	{
		if (video_->hasFormatDialog())
			video_->displayFormatDialog();
	}
}

void AvGrabber::displayVideoSourceDialog()
{

  AvGrabberDebugMsg("displayVideoSourceDialog()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL)
	{
		if (video_->hasSourceDialog())
			video_->displaySourceDialog();
	}
}

long AvGrabber::getVideoEstimatedDataRate()
{

  AvGrabberDebugMsg("getVideoEstimatedDataRate()\n");

   if ((video_ != NULL || wdm_ != NULL) && enableVideo_)
   {
      long dr;
      if (video_ != NULL)
         dr = video_->getEstimatedDataRate();
      else if (wdm_ != NULL)
         dr = wdm_->getEstimatedDataRate();

      return dr;
   }
   else
      return 0;
}

long AvGrabber::getVideoAverageDataRate()
{

  AvGrabberDebugMsg("getVideoAverageDataRate()\n");

  if (!inProgress_)
      return 0;

   if (video_ != NULL && enableVideo_)
      return video_->getAverageDataRate();
   else if (wdm_ != NULL && enableVideo_)
      return wdm_->getAverageDataRate();
   
   return 0;
}

int AvGrabber::getVideoDroppedFramesCount()
{

  AvGrabberDebugMsg("getVideoDroppedFramesCount()\n");

   if (!inProgress_)
      return 0;
   if ((video_ == NULL && wdm_ == NULL) || !enableVideo_)
      return 0;

   if (video_ != NULL)
      return video_->getDroppedFramesCount();
   else // (wdm_ != NULL)
      return wdm_->getDroppedFramesCount();
}

unsigned long AvGrabber::getVideoFileSize()
{

  AvGrabberDebugMsg("getVideoFileSize()\n");

   if (!inProgress_)
      return 0;
   if ((video_ == NULL && wdm_ == NULL) || !enableVideo_)
      return 0;

   if (video_ != NULL)
      return video_->getFileSize();
   else // (wdm_ != NULL)
      return wdm_->getFileSize();
}

unsigned long AvGrabber::getVideoLengthMs()
{

  AvGrabberDebugMsg("getVideoLengthMs()\n");

   if (!inProgress_)
      return 0;
   if ((video_ == NULL && wdm_ == NULL) || !enableVideo_)
   {
      return 0;
   }

   if (video_ != NULL)
      return video_->getVideoLengthMs();
   else // (wdm_ != NULL)
      return wdm_->getVideoLengthMs();
}

void AvGrabber::setEnableVideoThumbnail(bool b)
{

  AvGrabberDebugMsg("setEnableVideoThumbnail()\n");

   if (video_ != NULL)
      video_->setEnableThumbnails(b);
   else if (wdm_ != NULL)
      wdm_->setEnableThumbnails(b);
}

bool AvGrabber::getEnableVideoThumbnail()
{

  AvGrabberDebugMsg("getEnableVideoThumbnail()\n");

   if (video_ != NULL)
      return video_->isThumbnailEnabled();
   else if (wdm_ != NULL)
      return wdm_->isThumbnailEnabled();
   return false;
}

void AvGrabber::getVideoThumbnailData(ThumbnailData* thumb)
{

  AvGrabberDebugMsg("getVideoThumbnailData()\n");

   if (video_ != NULL)
      video_->getThumbnailData(thumb);
   else if (wdm_ != NULL)
      wdm_->getThumbnailData(thumb);
   else
   {
      thumb->data = NULL;
      thumb->info = NULL;
   }
}

void AvGrabber::setUseAdvancedAudioSync(bool bAdvancedSync)
{

    AvGrabberDebugMsg("setUseAdvancedAudioSync()", true, bAdvancedSync ? 1 : -1);

   if (video_ != NULL)
      video_->SetUseAdvancedAudioSync(bAdvancedSync);
   else if (wdm_ != NULL)
      wdm_->SetUseAdvancedAudioSync(bAdvancedSync);
}

void AvGrabber::notifyVideoSgClipStart()
{

  AvGrabberDebugMsg("notifyVideoSgClipStart()\n");

   if (video_ == NULL && wdm_ == NULL)
      return;

   switch (nSgVideoMode_)
   {
   case AV_CONTINUE_ON_SG:
      // do nothing
      break;

   case AV_NULL_ON_SG:
   default:
      if (video_ != NULL)
         video_->setNullFrameMode(true);
      else if (wdm_ != NULL)
         wdm_->setNullFrameMode(true);
      break;
   }
}

void AvGrabber::notifyVideoSgClipStop()
{

  AvGrabberDebugMsg("notifyVideoSgClipStop()\n");

   if (video_ == NULL && wdm_ == NULL)
      return;

   switch (nSgVideoMode_)
   {
   case AV_CONTINUE_ON_SG:
      // do nothing
      break;

   case AV_NULL_ON_SG:
   default:
      if (video_ != NULL)
         video_->setNullFrameMode(false);
      else if (wdm_ != NULL)
         wdm_->setNullFrameMode(false);
      break;
   }
}

bool AvGrabber::loadSgClickNoise(_TCHAR *tszWavFileName)
{

  AvGrabberDebugMsg("loadSgClickNoise()\n");

   if (audio_ == NULL)
      return false;

   return audio_->loadClickNoise(tszWavFileName);
}


// ////////////////////////////////////////////////
//
//    S C R E E N   G R A B B E R   M E T H O D S
//
// ////////////////////////////////////////////////

void AvGrabber::initScreenCapturer(CLiveContext *pLiveContext)
{

  AvGrabberDebugMsg("initScreenCapturer()\n");

   /*
   if (video_ != NULL)
   {
      // TODO: Do something better here:
      MessageBox(NULL, "Cannot create Screen Capturer if Video Capturer exists.", 
                 "Error", MB_OK | MB_ICONEXCLAMATION);
      return;
   }
   */

   if (screen_ != NULL)
      return;

   screen_ = new ScreenCapturer(NULL, NULL, pLiveContext);
   screen_->setUseAutoFrameRate(true, 640, 480, 5.0, 10.0, 1.0);
   isGrabbingScreen_ = false;
}

void AvGrabber::destroyScreenCapturer()
{

  AvGrabberDebugMsg("destroyScreenCapturer()\n");

   if (screen_ == NULL)
      return;

   if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

   delete screen_;
   screen_ = NULL;
   isGrabbingScreen_ = false;
}

bool AvGrabber::chooseSgCaptureArea(bool isStandAlone)
{

  AvGrabberDebugMsg("chooseSgCaptureArea()\n");

   if (screen_ == NULL)
   {
      // TODO: ...
      MessageBox(NULL, _T("Cannot choose screen capture area.\nNo Screen Capturer available."),
                 _T("Error"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
      return false;
   }

   if (isGrabbingScreen_)
   {
      MessageBox(NULL, _T("Cannot choose new capture area while already\ngrabbing screen."),
                 _T("Error"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
      return false;
   }

   //if (isPaused_ && !isStandAlone)
   //{
   //   char t[256];
   //   sprintf(t, "%s", Local::localizeString(SG_NOSTART_WHILE_PAUSED));
   //   MessageBox(NULL, t, "Warning", MB_OK | MB_ICONWARNING);
   //   return false;
   //}

   return screen_->chooseCaptureRect();
}

bool AvGrabber::setSgCaptureRect(RECT *pRect)
{

  AvGrabberDebugMsg("setSgCaptureRect()\n");

   if (screen_)
      return screen_->setScreenCaptureRect(pRect);
   return false;
}

bool AvGrabber::getSgCaptureRect(RECT *pRect)
{

  AvGrabberDebugMsg("getSgCaptureRect()\n");

   if (screen_)
      return screen_->getScreenCaptureRect(pRect);
   return false;
}

void AvGrabber::setSgQuickCaptureMode(bool b)
{

  AvGrabberDebugMsg("setSgQuickCaptureMode()\n");

   if (screen_)
      screen_->setQuickCaptureMode(b);
}

long AvGrabber::setSgFrameRate(float reqFrameRate)
{

  AvGrabberDebugMsg("setSgFrameRate()\n");

	if (screen_ != NULL)
		return screen_->setFrameRate(reqFrameRate);

	return -1;
}

void AvGrabber::setSgAutoFrameRate(bool bEnable, int nWidth, 
                                   int nHeight, float fRate, 
                                   float fMaxRate, float fMinRate)
{

  AvGrabberDebugMsg("setSgAutoFrameRate()\n");

   if (screen_)
      screen_->setUseAutoFrameRate(bEnable, nWidth, nHeight, fRate, fMaxRate, fMinRate);
}

unsigned long AvGrabber::getSgClipLengthMs()
{

  AvGrabberDebugMsg("getSgClipLengthMs()\n");

   //if (!inProgress_)
   //   return 0;

   // The Assistant previously had to read out
   // the SG clip length _before_ the capturing
   // had ended. This sometimes resulted in wrong clip lengths
   // in the LMD file. This is no longer the case.
   if (screen_ != NULL)// && isGrabbingScreen_)
      return screen_->getVideoLengthMs();
   return 0;
}

void AvGrabber::startScreenGrabbingClip(_TCHAR *tszClipName, bool bCaptureActions, const _TCHAR *tszAudioName, const char *szLadInfo)
{

  AvGrabberDebugMsg("startScreenGrabbingClip()\n");

   if (screen_ == NULL)
   {
      // TODO: ...
      MessageBox(NULL, _T("Cannot start screen grabbing clip.\nNo Screen Capturer available."),
                 _T("Error"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
      return;
   }

   if (isGrabbingScreen_)
   {
      // TODO: ...
      MessageBox(NULL, _T("Stop the current screen grabbing recording\nbefore starting a new one."),
                 _T("Error"), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
      return;
   }

   if (!inProgress_ && (tszAudioName == NULL))
   {
      MessageBox(NULL, _T("Must have started capture process with start()\nbefore starting a screen capture clip."),
         _T("Error"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
      return;
   }

   //if (isPaused_ && szAudioName == NULL)
   //{
   //   MessageBox(NULL, "Capture process is currently paused!\nMust have started capture process with start()\nbefore starting a screen capture clip.",
   //      "Error", MB_OK | MB_ICONEXCLAMATION);
   //   return;
   //}

   notifyVideoSgClipStart();

   try
   {
      if (tszAudioName != NULL)
      {
         start((_TCHAR *) tszAudioName, (char *) szLadInfo);
      }

      screen_->start(tszClipName, audio_, isPaused_, bCaptureActions);
      isGrabbingScreen_ = true;
   }
   catch (exception)
   {
      if (isGrabbingScreen_)
         screen_->stop();
      notifyVideoSgClipStop();

      throw;
   }
}

void AvGrabber::stopScreenGrabbingClip()
{

  AvGrabberDebugMsg("stopScreenGrabbingClip()\n");

   if (!isGrabbingScreen_)
      return;

   isGrabbingScreen_ = false;

   if (screen_ == NULL)
      return;

   __try
   {
      screen_->finalizeSync();
      screen_->stop();
   }
   __finally
   {
      notifyVideoSgClipStop();
   }
}

long AvGrabber::getScreenGrabbingClipStartTime()
{

  AvGrabberDebugMsg("getScreenGrabbingClipStartTime()\n");

   if (screen_ == NULL)
   {
      // TODO: Not so good with these MessageBox'es...
      MessageBox(NULL, _T("Cannot retrieve start time of Screen Grabbing\nclip if no Screen Capturer exists."),
                 _T("Error"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
      return -1;
   }

   return screen_->getVideoAudioOffset();
}

long AvGrabber::getSgAverageDataRate()
{

  AvGrabberDebugMsg("getSgAverageDataRate()\n");

   if (!inProgress_)
      return 0;

   if (screen_ != NULL && isGrabbingScreen_)
      return screen_->getAverageDataRate();
   
   return 0;
}

int AvGrabber::getSgDroppedFramesCount()
{

  AvGrabberDebugMsg("getSgDroppedFramesCount()\n");

   // See comment in getSgClipLengthMs(),
   // Now the Assistant reads out this value
   // after calling stopSgClip().
   if (screen_ == NULL)// || !inProgress_ || !isGrabbingScreen_)
      return 0;

   return screen_->getDroppedFramesCount();
}

int AvGrabber::getSgTotalFramesCount()
{

  AvGrabberDebugMsg("getSgTotalFramesCount()\n");

   // See comment in getSgClipLengthMs(),
   // Now the Assistant reads out this value
   // after calling stopSgClip().
   if (screen_ == NULL)// || !inProgress_ || !isGrabbingScreen_)
      return 0;

   return screen_->getTotalFramesCount();
}

void AvGrabber::setSgVideoMode(int nMode)
{

  AvGrabberDebugMsg("setSgVideoMode()\n");

   nSgVideoMode_ = nMode;
}

void AvGrabber::setSgAnnotationMode()
{

  AvGrabberDebugMsg("setSgAnnotationMode()\n");

   if (!inProgress_)
      return;

   if (screen_ != NULL && isGrabbingScreen_)
      screen_->setAnnotationMode();
}

void AvGrabber::setSgInteractionMode()
{

  AvGrabberDebugMsg("setSgInteractionMode()\n");

   if (!inProgress_)
      return;

   if (screen_ != NULL && isGrabbingScreen_)
      screen_->setInteractionMode();
}

void AvGrabber::setSgAnnotationTool(int nTool)
{

  AvGrabberDebugMsg("setSgAnnotationTool()\n");

   if (screen_)
      screen_->setAnnotationTool(nTool);
}

void AvGrabber::setSgAnnotationPenColor(Gdiplus::ARGB color)
{

  AvGrabberDebugMsg("setSgAnnotationPenColor()\n");

   if (screen_)
      screen_->setAnnotationPenColor(color);
}

void AvGrabber::setSgAnnotationFillColor(Gdiplus::ARGB color)
{

  AvGrabberDebugMsg("setSgAnnotationFillColor()\n");

   if (screen_)
      screen_->setAnnotationFillColor(color);
}

void AvGrabber::setSgAnnotationLineWidth(int lineWidth)
{

  AvGrabberDebugMsg("setSgAnnotationLineWidth()\n");

   if (screen_)
      screen_->setAnnotationLineWidth(lineWidth);
}

void AvGrabber::setSgAnnotationLineStyle(int lineStyle)
{

  AvGrabberDebugMsg("setSgAnnotationLineStyle()\n");

   if (screen_)
      screen_->setAnnotationLineStyle(lineStyle);
}

void AvGrabber::setSgAnnotationArrowStyle(int arrowStyle)
{

  AvGrabberDebugMsg("setSgAnnotationArrowStyle()\n");

   if (screen_)
      screen_->setAnnotationArrowStyle(arrowStyle);
}

void AvGrabber::setSgAnnotationArrowConfig(int arrowConfig)
{

  AvGrabberDebugMsg("setSgAnnotationArrowConfig()\n");

   if (screen_)
      screen_->setAnnotationArrowConfig(arrowConfig);
}

void AvGrabber::setSgAnnotationFont(LOGFONT *pLogFont)
{

  AvGrabberDebugMsg("setSgAnnotationFont()\n");

   if (screen_)
      screen_->setAnnotationFont(pLogFont);
}

void AvGrabber::setSgAnnotationIsFilled(bool b)
{

  AvGrabberDebugMsg("setSgAnnotationIsFilled()\n");

   if (screen_)
      screen_->setAnnotationIsFilled(b);
}

void AvGrabber::setSgAnnotationIsClosed(bool b)
{

  AvGrabberDebugMsg("setSgAnnotationIsClosed()\n");

   if (screen_)
      screen_->setAnnotationIsClosed(b);
}

void AvGrabber::setSgHideWhilePanning(bool b)
{

  AvGrabberDebugMsg("setSgHideWhilePanning()\n");

   if (screen_)
      screen_->setHideWhilePanning(b);
}

void AvGrabber::setSgDeleteAllOnInteractionMode(bool b)
{

  AvGrabberDebugMsg("setSgDeleteAllOnInteractionMode()\n");

   if (screen_)
      screen_->setDeleteAllOnInteractionMode(b);
}

void AvGrabber::setSgClickEffectFrames(int frames, int radiusInc)
{

  AvGrabberDebugMsg("setSgClickEffectFrames()\n");

   if (screen_)
      screen_->setClickEffectFrames(frames, radiusInc);
}

void AvGrabber::setSgEnableClickEffects(bool audioEffects, 
                                        bool videoEffects)                                       
{

  AvGrabberDebugMsg("setSgEnableClickEffects()\n");

   if (screen_)
      screen_->setEnableClickEffects(audioEffects, videoEffects);
}

void AvGrabber::sgAnnotationUndo()
{

  AvGrabberDebugMsg("sgAnnotationUndo()\n");

   if (screen_)
      screen_->annotationUndo();
}

long AvGrabber::getSgAnnotationLastUndoTimeStamp()
{

  AvGrabberDebugMsg("getSgAnnotationLastUndoTimeStamp()\n");

   if (screen_)
      return screen_->getAnnotationLastUndoTimeStamp();
   return -1;
}

void AvGrabber::sgAnnotationRedo()
{

  AvGrabberDebugMsg("sgAnnotationRedo()\n");

   if (screen_)
      screen_->annotationRedo();
}

long AvGrabber::getSgAnnotationNextRedoTimeStamp()
{

  AvGrabberDebugMsg("getSgAnnotationNextRedoTimeStamp()\n");

   if (screen_)
      return screen_->getAnnotationNextRedoTimeStamp();
   return -1;
}

void AvGrabber::setSgDisableVideoAcceleration(bool b)
{

  AvGrabberDebugMsg("setSgDisableVideoAcceleration()\n");

   disableHwAcceleration_ = b;
}

bool AvGrabber::getSgDisableVideoAcceleration()
{

  AvGrabberDebugMsg("getSgDisableVideoAcceleration()\n");

   return disableHwAcceleration_;
}

// ////////////////////////////////////////
//
//    D A T A   R A T E   M E T H O D S
//
// ////////////////////////////////////////

long AvGrabber::getEstimatedDataRate()
{

  AvGrabberDebugMsg("getEstimatedDataRate()\n");

   if (inProgress_)
      return getAverageDataRate();

   return getVideoEstimatedDataRate() + getAudioDataRate();
}

long AvGrabber::getAverageDataRate()
{

  AvGrabberDebugMsg("getAverageDataRate()\n");

   if (!inProgress_)
      return 0;

   return getVideoAverageDataRate() + getAudioDataRate() + getSgAverageDataRate();
}

// ////////////////////////////////
//
//    C O D E C   M E T H O D S
//
// ////////////////////////////////

int AvGrabber::getCodecCount()
{

  AvGrabberDebugMsg("getCodecCount()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL)
	{
		return video_->getCodecListSize();
	}
   else if (wdm_ != NULL)
   {
      return wdm_->getCodecListSize();
   }
	else
		return 0;
}

int AvGrabber::getSgCodecCount()
{

  AvGrabberDebugMsg("getSgCodecCount()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (screen_ != NULL)
	{
		return screen_->getCodecListSize();
	}
	else
		return 0;
}

void AvGrabber::getCodecs(CodecInfo* codecList[])
{

  AvGrabberDebugMsg("getCodecs()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL)
		video_->getCodecList(codecList);
   else if (wdm_ != NULL)
      wdm_->getCodecList(codecList);
}

void AvGrabber::getSgCodecs(CodecInfo* codecList[])
{

  AvGrabberDebugMsg("getSgCodecs()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (screen_ != NULL)
		screen_->getCodecList(codecList);
}

void AvGrabber::setVideoCodec(CodecInfo* codec)
{

  AvGrabberDebugMsg("setVideoCodec()\n", true, codec->fccHandler);

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL)
		video_->setCodec(codec);
   else if (wdm_ != NULL)
      wdm_->setCodec(codec);
}

void AvGrabber::setScreenGrabberCodec(CodecInfo* codec)
{

  AvGrabberDebugMsg("setScreenGrabberCodec()\n", true, codec->fccHandler);

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (screen_ != NULL)
      screen_->setCodec(codec);
}

void AvGrabber::displayCodecAboutDialog(HWND hParent)
{

  AvGrabberDebugMsg("displayCodecAboutDialog()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL)
		video_->aboutCodec(hParent);
   else if (wdm_ != NULL)
      wdm_->aboutCodec(hParent);
   //if (screen_ != NULL)
   //   screen_->aboutCodec(hParent);
}

void AvGrabber::displaySgCodecAboutDialog(HWND hParent)
{

  AvGrabberDebugMsg("displaySgCodecAboutDialog()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

   if (screen_ != NULL)
      screen_->aboutCodec(hParent);
}

void AvGrabber::displayCodecConfigureDialog(HWND hParent)
{

  AvGrabberDebugMsg("displayCodecConfigureDialog()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

	if (video_ != NULL)
		video_->configureCodec(hParent);
   else if (wdm_ != NULL)
      wdm_->configureCodec(hParent);
   //if (screen_ != NULL)
   //   screen_->configureCodec(hParent);
}

void AvGrabber::displaySgCodecConfigureDialog(HWND hParent)
{

  AvGrabberDebugMsg("displaySgCodecConfigureDialog()\n");

	if (inProgress_)
		throw AVGrabberException(Local::localizeString(AV_IN_PROGRESS));

   if (screen_ != NULL)
      screen_->configureCodec(hParent);
}

void AvGrabber::setAnnotationMode()
{

  AvGrabberDebugMsg("setAnnotationMode()\n");

   if (!inProgress_)
      return;

   if (screen_ == NULL)
      return;

   screen_->setAnnotationMode();
}

void AvGrabber::setInteractionMode()
{

  AvGrabberDebugMsg("setInteractionMode()\n");

   if (!inProgress_)
      return;

   if (screen_ == NULL)
      return;

   screen_->setInteractionMode();
}

void AvGrabber::setAnnotationTool(int nTool)
{

  AvGrabberDebugMsg("setAnnotationTool()\n");

   if (screen_ == NULL)
      return;

   screen_->setAnnotationTool(nTool);
}

void AvGrabber::setDeleteAllOnInteractionMode(bool b)
{

  AvGrabberDebugMsg("setDeleteAllOnInteractionMode()\n");

   if (screen_ == NULL)
      return;

   screen_->setDeleteAllOnInteractionMode(b);
}

int AvGrabber::staticGetVideoLength(_TCHAR *tszVideoFile)
{

  AvGrabberDebugMsg("staticGetVideoLength()\n");

   AVIFileInit();

   PAVIFILE pFile     = NULL;
   PAVISTREAM pStream = NULL;
   int streamLengthFrames = -1;
   int streamLengthMs     = -1;  

   HRESULT hr = AVIFileOpen(&pFile, tszVideoFile, OF_READ|OF_SHARE_DENY_WRITE, NULL);
   if (SUCCEEDED(hr))
      hr = AVIFileGetStream(pFile, &pStream, streamtypeVIDEO, 0);
   if (SUCCEEDED(hr))
      streamLengthFrames = AVIStreamLength(pStream);
   if (SUCCEEDED(hr) && streamLengthFrames != -1)
      streamLengthMs = AVIStreamSampleToTime(pStream, streamLengthFrames);

   if (pStream)
      AVIStreamRelease(pStream);
   pStream = NULL;
   if (pFile)
      AVIFileRelease(pFile);
   pFile = NULL;

   AVIFileExit();

   return streamLengthMs;
}

//#include "AudioMixer.h"
#include "global.h"
#include "resource.h"
#include "LanguageSupport.h"

using namespace std;

void AvGrabberDebugMsg(char *szFormat);
void AvGrabberDebugMsg(char *szFormat, bool bPrefix, int iValue);

AudioMixer::AudioMixer(int waveInID, MIXERCALLBACK mixerCallback)
{
	mixer_            = NULL;
	mixerIsOpen_      = false;
	mixerInitialized_ = false;
   isBusy_           = false;

	sourceActive_  = NULL;
	sourceVolumes_ = NULL;
	sourceLabels_  = NULL;
   sourceDetails_ = NULL;
   audioSources_  = NULL;
   muxDetails_    = NULL;

   instance = this;
   closeEvent_    = NULL;
   createEvent_   = NULL;
   mixerEvent_    = NULL;

   lpMixerCallback_ = mixerCallback;

   mixerHwnd_     = NULL;

   if (mixerCallback != NULL)
   {
      // (*mixerCallback)((HWND) 1, 2, 3, 4);
      mixerHwnd_ = createMixerHwnd();
   }

	UINT mixerId;

	MMRESULT res = mixerGetID((HMIXEROBJ) waveInID, &mixerId, MIXER_OBJECTF_WAVEIN);

	if (mixerId == -1 || res != MMSYSERR_NOERROR)
	{
		mixerIsOpen_ = false;
		return;
	}

	res = mixerOpen(&mixer_, mixerId, (DWORD) mixerHwnd_, 
		(DWORD) (LPVOID) this, (mixerHwnd_ != NULL)? CALLBACK_WINDOW : 0);

    AvGrabberDebugMsg("AudioMixer::AudioMixer() Return\n", true, res);

	if (res != MMSYSERR_NOERROR)
	{
		mixerIsOpen_ = false;
		return;
	}

	mixerIsOpen_ = true;

	initSources();
}

AudioMixer::~AudioMixer()
{
	if (mixerIsOpen_)
	{
      //cout << "~AudioMixer::mixerClose()" << endl;
		mixerClose(mixer_);
	}

	if (sourceActive_ != NULL)
   {
      //cout << "~AudioMixer::free(sourceActive_)" << endl;
		free(sourceActive_);
   }
	sourceActive_ = NULL;

	if (sourceLabels_ != NULL)
   {
      //cout << "~AudioMixer::free(sourceLabels_)" << endl;
		free(sourceLabels_);
   }
	sourceLabels_ = NULL;

   if (sourceDetails_ != NULL)
   {
      //cout << "~AudioMixer::free(sourceDetails_)" << endl;
      free(sourceDetails_);
   }
   sourceDetails_ = NULL;

   if (audioSources_ != NULL)
   {
      //cout << "~AudioMixer::free(audioSources_)" << endl;
      free(audioSources_);
   }
   audioSources_ = NULL;

   if (muxDetails_ != NULL)
   {
      //cout << "~AudioMixer::free(muxDetails_)" << endl;
      free(muxDetails_);
   }
   muxDetails_ = NULL;

	if (sourceVolumes_ != NULL)
   {
      //cout << "~AudioMixer::free(sourceVolumes_ == " << sourceVolumes_ << ")" << endl;
		free(sourceVolumes_);
   }
	sourceVolumes_ = NULL;

   if (mixerHwnd_ != NULL)
   {
      //cout << "~AudioMixer::destroyMixerHwnd()" << endl;
      destroyMixerHwnd();
   }
   mixerHwnd_ = NULL;

   instance = NULL;
}

void AudioMixer::initSources()
{
    AvGrabberDebugMsg("AudioMixer::initSources()\n");

	// (1) Find out how many lines our mixer has
	MIXERCAPS mixerCaps;
	MMRESULT res = mixerGetDevCaps((UINT) mixer_, &mixerCaps, sizeof MIXERCAPS);
	if (res != MMSYSERR_NOERROR)
		return;

	int lines = mixerCaps.cDestinations;

	// (2) Find the waveIn line
	MIXERLINE recordLine;
	recordLine.cbStruct = sizeof MIXERLINE;
	recordLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	res = mixerGetLineInfo((HMIXEROBJ) mixer_, &recordLine, 
		MIXER_GETLINEINFOF_COMPONENTTYPE | MIXER_OBJECTF_HMIXER);
	if (res != MMSYSERR_NOERROR)
	{
		cerr << "mixerGetLineInfo() failed: error == " << res << endl;
		return;
	}
	else
		cerr << "mixerGetLineInfo() succeeded." << endl;

	// (3) Check for a MUX control on the wavein line
	MIXERLINECONTROLS mixerLineControls;
	mixerLineControls.cbStruct = sizeof MIXERLINECONTROLS;
	mixerLineControls.dwLineID = recordLine.dwDestination;
	mixerLineControls.cbmxctrl = sizeof MIXERCONTROL;

	bool muxFound = false;

	for (int controlId=0; controlId<recordLine.cControls; ++controlId)
	{
		MIXERCONTROL mixerControl;

		mixerControl.cbStruct = sizeof MIXERCONTROL;
		mixerLineControls.dwControlID = controlId;
		mixerLineControls.pamxctrl = &mixerControl;

		res = mixerGetLineControls((HMIXEROBJ) mixer_, &mixerLineControls,
									MIXER_GETLINECONTROLSF_ONEBYID | MIXER_OBJECTF_HMIXER);

		if ((mixerControl.dwControlType & MIXERCONTROL_CONTROLTYPE_MUX) &&
			(mixerControl.fdwControl & MIXERCONTROL_CONTROLF_MULTIPLE) &&
			!(mixerControl.fdwControl & MIXERCONTROL_CONTROLF_DISABLED))
		{
			// hooray for boobies, we found a waveIn MUX

			// note how many sources we have:
			sourceCount_ = mixerControl.cMultipleItems;

            // Note (June 2010) the below != may happen on some hardware (Conexant)
            // However this doesn't lead to real problems? -> Only the crash in "getSource()"
            // is prohibited at the moment...
            //
            //if (recordLine.cConnections != mixerControl.cMultipleItems)

         muxDetails_ = (MIXERCONTROLDETAILS*) malloc(sizeof(MIXERCONTROLDETAILS));
			// MIXERCONTROLDETAILS details;

			// We'll start and retrieve the active state of the sources
			sourceActive_ = (MIXERCONTROLDETAILS_BOOLEAN*)
				malloc(sourceCount_ * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

			muxDetails_->cbStruct       = sizeof MIXERCONTROLDETAILS;
			muxDetails_->dwControlID    = mixerControl.dwControlID;
			muxDetails_->paDetails      = sourceActive_;
			muxDetails_->cbDetails      = sizeof MIXERCONTROLDETAILS_BOOLEAN;
			muxDetails_->cMultipleItems = mixerControl.cMultipleItems;
			muxDetails_->cChannels      = 1;

			res = mixerGetControlDetails((HMIXEROBJ) mixer_, muxDetails_, 
										 MIXER_GETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER);
			if (res != MMSYSERR_NOERROR)
			{
				cerr << "*** mixerGetControlDetails(VALUE) failed: " << res << endl;
				return;
			}

			// now we'll retrieve the labels for the sources:
			sourceLabels_ = (MIXERCONTROLDETAILS_LISTTEXT*)
				malloc(sourceCount_ * sizeof(MIXERCONTROLDETAILS_LISTTEXT));
			
			muxDetails_->cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
			muxDetails_->paDetails = sourceLabels_;

			res = mixerGetControlDetails((HMIXEROBJ) mixer_, muxDetails_,
										 MIXER_GETCONTROLDETAILSF_LISTTEXT | MIXER_OBJECTF_HMIXER);
         if (res != MMSYSERR_NOERROR)
			{
				cerr << "*** mixerGetControlDetails(LISTTEXT) failed: " << res << endl;

            // we also need the information from sourceLabels_ later on
            // so it is better to fail (return) and thus deactivate the mixer
            return;
			}

         // now set back the details to active state fetch mode
			muxDetails_->paDetails      = sourceActive_;
			muxDetails_->cbDetails      = sizeof MIXERCONTROLDETAILS_BOOLEAN;

         muxFound = true;
			break;
		}
	}

	if (muxFound)
	{
      // We have found a MUX, now let's get the volume controls of the source lines.
      MIXERLINE line;
      memcpy(&line, &recordLine, sizeof MIXERLINE);

      // allocate some memory for the controls:


      // NOTE: we allocate memory for one extra line. This seems to be due to a problem
      // in the Soundblaster PCI Live sound card; it overwrites some data here. Weird.
      sourceVolumes_ = (MIXERCONTROLDETAILS_UNSIGNED*)
         malloc((recordLine.cConnections + 1) * sizeof(MIXERCONTROLDETAILS_UNSIGNED));
      
      //cout << "sourceVolumes_ == " << sourceVolumes_ << ", sizeof UNSIGNED == " << sizeof(MIXERCONTROLDETAILS_UNSIGNED) << endl;

      sourceDetails_ = (MIXERCONTROLDETAILS*)
         malloc(recordLine.cConnections * sizeof(MIXERCONTROLDETAILS));

      // allocate some memory for the other data:
      audioSources_ = (AudioSource*)
         malloc(recordLine.cConnections * sizeof(AudioSource));

      // did we get the memory?
      if (sourceVolumes_ == NULL ||
          sourceDetails_ == NULL ||
          audioSources_  == NULL)
      {
         cerr << "*** memory for mixer structures missing." << endl;
         return;
      }

      AvGrabberDebugMsg("AudioMixer::initSources() Connections\n", true, recordLine.cConnections);

      for (int connId=0; connId<recordLine.cConnections; ++connId)
      {
         line.dwSource = connId;

         res = mixerGetLineInfo((HMIXEROBJ) mixer_, &line, 
                                MIXER_GETLINEINFOF_SOURCE | MIXER_OBJECTF_HMIXER);
         if (res != MMSYSERR_NOERROR)
         {
            cerr << "mixerGetLineInfo(dwSource==" << connId << ") failed." << endl;
            continue;
         }

         int index = -1;
         for (int k=0; k<recordLine.cConnections; ++k)
         {
            if (k < sourceCount_ && line.dwLineID == sourceLabels_[k].dwParam1)
            {
               index = k;
               cout << "Index of dwLineID " << line.dwLineID << " is " << index << endl;
            }
         }

         // initalize the audioSources_ structure with default values:
         // strcpy(audioSources_[connId].szName, sourceLabels_[connId].szName);
         _tcscpy(audioSources_[connId].tszName, line.szName);
         audioSources_[connId].isEnabled        = false;
         audioSources_[connId].srcID            = connId;
         audioSources_[connId].hasVolumeControl = false;
         audioSources_[connId].dwMinVolume      = 0;
         audioSources_[connId].dwMaxVolume      = 65535;
         audioSources_[connId].dwSteps          = 1;

         if (index == -1)
         {
             AvGrabberDebugMsg("AudioMixer::initSources() Could not find label for id\n", true, line.dwLineID);
            cerr << "could not find label for line ID " << line.dwLineID << endl;
            continue;
         }

         audioSources_[connId].isActive = (sourceActive_[index].fValue != 0);
         if (audioSources_[connId].isActive) {
            activeSource_ = connId;
            AvGrabberDebugMsg("AudioMixer::initSources() Active source is\n", true, activeSource_);
         }

         audioSources_[connId].srcIndex = index;

         // the line is ok, so we can set the enabled flag:
         audioSources_[connId].isEnabled = true;

         // ok, let's see if we can find a volume control here...
			MIXERLINECONTROLS mixerControls;
			MIXERCONTROL volumeControl;
			volumeControl.cbStruct = sizeof MIXERCONTROL;

			mixerControls.cbStruct      = sizeof MIXERLINECONTROLS;
			mixerControls.dwLineID      = line.dwLineID;
			// mixerControls.dwControlID   = 1;
			mixerControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mixerControls.cControls     = 1; // retrieve 1 control
			mixerControls.cbmxctrl      = sizeof MIXERCONTROL;
			mixerControls.pamxctrl      = &volumeControl;

         res = mixerGetLineControls((HMIXEROBJ) mixer_, &mixerControls, 
				MIXER_GETLINECONTROLSF_ONEBYTYPE | MIXER_OBJECTF_HMIXER);

         // cout << "mixerControls.dwLineID == " << mixerControls.dwLineID << endl;

         if (res != MMSYSERR_NOERROR)
         {
            cerr << "mixerLine with dwSource==" << connId << " does not have a volume control." << endl;
            continue;
         }

         printf("line.cChannels == %d\n", line.cChannels);
         printf("volumeControl.dwControlID == 0x%X\n", volumeControl.dwControlID);
         printf("volumeControl.dwControlType == 0x%X\n", volumeControl.dwControlType);
         printf("volumeControl.fdwControl == 0x%X\n", volumeControl.fdwControl);
         printf("volumeControl.cMultipleItems == 0x%X\n", volumeControl.cMultipleItems);
         printf("volumeControl.szName == %s\n", volumeControl.szName);

         // note the min and max values:
         audioSources_[connId].dwMinVolume = volumeControl.Bounds.dwMinimum;
         audioSources_[connId].dwMaxVolume = volumeControl.Bounds.dwMaximum;
         audioSources_[connId].dwSteps     = volumeControl.Metrics.cSteps;

         // retrieve further details on the control:
         MIXERCONTROLDETAILS* details = &sourceDetails_[connId];
         details->cbStruct       = sizeof MIXERCONTROLDETAILS;
         details->dwControlID    = volumeControl.dwControlID;
         details->cMultipleItems = 0;
         details->cbDetails      = sizeof MIXERCONTROLDETAILS_UNSIGNED;
         details->cChannels      = 1; // 1 == UNIFORM?? //line.cChannels; <-- ???!
         details->paDetails      = &sourceVolumes_[connId];

         res = mixerGetControlDetails((HMIXEROBJ) mixer_, details, 
                                      MIXER_GETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER);
         if (res != MMSYSERR_NOERROR)
         {
            cerr << "mixerGetControlDetails(dwControlId==" << volumeControl.dwControlID << ", " <<
               "connId==" << connId << ") failed: " << res << endl;
            continue;
         }

         cout << "Volume #" << connId << ": " << sourceVolumes_[connId].dwValue << endl;
         printf("details->dwControlId == 0x%X\n", details->dwControlID);
         printf("details->cMultipleItems = %d\n", details->cMultipleItems);
         printf("details->cChannels == %d\n", details->cChannels);

         audioSources_[connId].hasVolumeControl = true;
      }
	}
	else
	{
		cerr << "*** no MUX found!" << endl;
		return;
	}

   /*
   for (int i=0; i<sourceCount_; ++i)
   {
      cout << "Audio source #" << audioSources_[i].srcID << ":" << endl;
      cout << "  Name: " << audioSources_[i].szName << endl;
      cout << "  isEnabled: " << audioSources_[i].isEnabled << endl;
      cout << "  isActive: " << audioSources_[i].isActive << endl;
      cout << "  hasVolumeControl: " << audioSources_[i].hasVolumeControl << endl;
      if (audioSources_[i].hasVolumeControl)
         cout << "    Volume: " << sourceVolumes_[i].dwValue << endl;
   }
   */

   AvGrabberDebugMsg("AudioMixer::initSources() Return Active Source is\n", true, activeSource_);

	// Lastly: We're ready:
	mixerInitialized_ = true;
}

HWND AudioMixer::createMixerHwnd()
{
   //createEvent_ = CreateEvent(NULL, TRUE, FALSE, "lecturnity mixer create event");
   //mixerEvent_  = CreateEvent(NULL, TRUE, FALSE, "Lecturnity Mixer event");
   createEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);
   mixerEvent_  = CreateEvent(NULL, TRUE, FALSE, NULL);
   eventThreadQuitRequest_ = false;

   if (createEvent_ == NULL || mixerEvent_ == NULL)
   {
      if (createEvent_)
         CloseHandle(createEvent_);
      createEvent_ = NULL;
      if (mixerEvent_)
         CloseHandle(mixerEvent_);
      mixerEvent_ = NULL;
      cerr << "-+-+-+- could not create mixer window creation event!" << endl;
      return NULL;
   }
   ResetEvent(createEvent_);

   //DWORD threadId, threadId2;
   //HANDLE handle1 = CreateThread(NULL, 0, startWindowLoop, this, 0, &threadId);
   //HANDLE handle2 = CreateThread(NULL, 0, mixerEventThread, this, 0, &threadId2);
   unsigned int thread1 =
      _beginthread(startWindowLoop, 0, this);
   unsigned int thread2 =
      _beginthread(mixerEventThread, 0, this);

   WaitForSingleObject(createEvent_, 30000);

   CloseHandle(createEvent_);
   createEvent_ = NULL;

   return mixerHwnd_;
}

//DWORD WINAPI AudioMixer::startWindowLoop(LPVOID param)
void __cdecl AudioMixer::startWindowLoop(LPVOID param)
{
   CLanguageSupport::SetThreadLanguage();

   static int nr = 0;
   nr++;

   cout << "AudioMixer::startWindowLoop()" << endl;

   AudioMixer* mixer = (AudioMixer*) param;

   // mixer->closeEvent_ = CreateEvent(NULL, TRUE, FALSE, "lecturnity mixer close event");
   // cout << "AudioMixer::startWindowLoop(): CreateEvent()" << endl;
   mixer->closeEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);
   // cout << " ok" << endl;
   if (mixer->closeEvent_ == NULL)
   {
      cerr << "-+-+-+- could not create mixer window close event" << endl;
      mixer->mixerHwnd_ = NULL;
      return; // 1;
   }

   _TCHAR className[128];
   _stprintf(className, _T("Lecturnity Mixer window #%d"), nr);

   _TCHAR windowName[128];
   _tcscpy(windowName, className);
   // cout << "class name: " << className << endl;
   // cout << "window name: " << windowName << endl;

   HINSTANCE hInstance = g_hDllInstance;
   
   WNDCLASSEX wndClass;
   wndClass.cbSize      = sizeof WNDCLASSEX;
   wndClass.style       = 0; // CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE; // 0;
   wndClass.lpfnWndProc = (WNDPROC) dummyWindowProc;
   wndClass.cbClsExtra  = 0;
   wndClass.cbWndExtra  = 0;
   wndClass.hInstance   = hInstance; // hInstance;
   wndClass.hIcon       = NULL;
   wndClass.hIconSm     = NULL;
   wndClass.hCursor     = LoadCursor(NULL, IDC_ARROW); // NULL;
   wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); //NULL;
   wndClass.lpszMenuName  = NULL;
   wndClass.lpszClassName = className;

   // cout << "AudioMixer::startWindowLoop(): RegisterClassEx()";
   ATOM atom = RegisterClassEx(&wndClass);
   // cout << " ok (atom == " << atom << ")" << endl;

   if (atom == 0)
   {
      cerr << "-+-+-+- could not register window class: " << className << endl;
      mixer->mixerHwnd_ = NULL;
      return; // 1;
   }

   //cout << "&wndClass == " << (DWORD) (&wndClass) << endl;
   printf("&wndClass == %x\n", (DWORD) (&wndClass));
   //cout << "GetModuleHandle(NULL) == " << GetModuleHandle(NULL) << endl;
   printf("g_hDllInstance == %x\n", hInstance /*GetModuleHandle(NULL)*/);

   // Sleep(2000);

   // cout << "AudioMixer::startWindowLoop(): CreateWindowEx()";
   HWND hWnd = CreateWindowEx(
         0, // WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 
         className, 
         windowName, 
         0, //WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, // 0, 
         0, 0, 128, 128, 
         NULL, 
         NULL, 
         hInstance, // g_hDllInstance
         NULL);
   // cout << " ok" << endl;

   if (hWnd == NULL)
   {
      cerr << "-+-+-+- could not CreateWindow for mixer control" << endl;
      UnregisterClass(className, g_hDllInstance);
      mixer->mixerHwnd_ = NULL;
      return; // 1;
   }

   mixer->mixerHwnd_ = hWnd;

   SetEvent(mixer->createEvent_);
   ResetEvent(mixer->closeEvent_);

   cout << "AudioMixer::Starting message loop" << endl;

   MSG msg;
   while (GetMessage(&msg, mixer->mixerHwnd_, 0, 0) > 0)
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   mixer->eventThreadQuitRequest_ = true;
   SetEvent(mixer->mixerEvent_);
   SetEvent(mixer->closeEvent_);

   cout << "+-+-+-+-+-+-+-+-+-+ elvis has left the building." << endl;

   return; // 0;
}

LRESULT CALLBACK AudioMixer::dummyWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   //cout << "dummyWindowProc(" << hWnd << ", " << msg << ", " << wParam << ", " << lParam << ")" << endl;
   //printf("dummyWindowProc(%x, %x, %x, %x)\n", hWnd, msg, wParam, lParam);
   switch (msg)
   {
   case WM_COMMAND:
      cout << "AudioMixer::WM_COMMAND" << endl;
      DestroyWindow(hWnd);
      break;

   case WM_DESTROY:
      cout << "AudioMixer::WM_DESTROY" << endl;
      PostQuitMessage(0);
      break;

   case MM_MIXM_CONTROL_CHANGE:
   case MM_MIXM_LINE_CHANGE:
      // cout << "AudioMixer::MIXER_SCHEIß" << endl;
      SetEvent(instance->mixerEvent_);
      break;
   }

   return DefWindowProc(hWnd, msg, wParam, lParam);
}

//DWORD WINAPI AudioMixer::mixerEventThread(LPVOID param)
void __cdecl AudioMixer::mixerEventThread(LPVOID param)
{
   CLanguageSupport::SetThreadLanguage();

   AudioMixer* mixer = (AudioMixer*) param;
   mixer->mixerEventThreadExited_ = false;

   while (!mixer->eventThreadQuitRequest_)
   {
      WaitForSingleObject(mixer->mixerEvent_, INFINITE);
      ResetEvent(mixer->mixerEvent_);

      if (!mixer->eventThreadQuitRequest_ && mixer->isEnabled())
         (*(mixer->lpMixerCallback_)) (mixer->mixerHwnd_, 0, 0, 0);
   }

   mixer->mixerEventThreadExited_ = true;

   cout << "+-+-+-+-+-+-+-+-+-+ priscilla has followed" << endl;
   return; // 0;
}

void AudioMixer::destroyMixerHwnd()
{
   PostMessage(mixerHwnd_, WM_COMMAND, 0, 0);

   BOOL res = WaitForSingleObject(closeEvent_, 10000);

   while (!mixerEventThreadExited_)
   {
      cout << "Waiting for mixer event thread..." << endl;
      Sleep(100);
   }

   if (closeEvent_ != NULL)
      CloseHandle(closeEvent_);
   closeEvent_ = NULL;

   if (mixerEvent_ != NULL)
      CloseHandle(mixerEvent_);
   mixerEvent_ = NULL;
}

bool AudioMixer::isEnabled()
{
    //AvGrabberDebugMsg("AudioMixer::isEnabled()\n");

	return (mixerIsOpen_ && mixerInitialized_);
}

int AudioMixer::getSourceCount()
{
    AvGrabberDebugMsg("AudioMixer::getSourceCount()\n");

   if (!isEnabled())
      return 0;
   return sourceCount_;
}

void AudioMixer::getSources(AudioSource* audioSources[])
{
    AvGrabberDebugMsg("AudioMixer::getSources()\n");

   if (!isEnabled())
      return;

   int iSourceCount = getSourceCount();

   for (int i=0; i<iSourceCount; ++i)
   {
      audioSources[i]->dwMaxVolume = audioSources_[i].dwMaxVolume;
      audioSources[i]->dwMinVolume = audioSources_[i].dwMinVolume;
      audioSources[i]->dwSteps     = audioSources_[i].dwSteps;
      audioSources[i]->hasVolumeControl = audioSources_[i].hasVolumeControl;
      audioSources[i]->isActive    = audioSources_[i].isActive;
      audioSources[i]->isEnabled   = audioSources_[i].isEnabled;
      audioSources[i]->srcID       = audioSources_[i].srcID;
      _tcscpy(audioSources[i]->tszName, audioSources_[i].tszName);
   }

   // memcpy(audioSources, &audioSources_, getSourceCount() * sizeof AudioSource);
}

int AudioMixer::getSource()
{
    AvGrabberDebugMsg("AudioMixer::getSource()\n");

   if (!isEnabled())
      return -1;

   MMRESULT res = mixerGetControlDetails((HMIXEROBJ) mixer_, muxDetails_, 
	                                      MIXER_GETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER);
   if (res != MMSYSERR_NOERROR)
   {
      cerr << "-- getSource(): mixerGetControlDetails failed: " << res << endl;
      return activeSource_; // not correct, but what can i do?
   }

   int iSourceCount = getSourceCount();

   AvGrabberDebugMsg("AudioMixer::getSource() Count\n", true, iSourceCount);

   for (int i=0; i<iSourceCount; ++i)
   {
       int iSrcIndex = audioSources_[i].srcIndex;

      
      audioSources_[i].isActive = false;
      
      if (iSrcIndex >= 0 && iSrcIndex < iSourceCount)
          audioSources_[i].isActive = (sourceActive_[iSrcIndex].fValue != 0);
      else
          AvGrabberDebugMsg("AudioMixer::getSource() No active determination\n", true, iSrcIndex);

      if (audioSources_[i].isActive) {
          AvGrabberDebugMsg("AudioMixer::getSource() Active found\n", true, i);
          activeSource_ = i;
      }
   }

   return activeSource_;
}

void AudioMixer::setSource(int srcID)
{
    AvGrabberDebugMsg("AudioMixer::setSource()\n", true, srcID);

   if (!isEnabled())
      return;

   if (srcID >= sourceCount_)
      return;
   if (srcID < 0)
      return;

   if (isBusy_)
   {
      MessageBox(NULL, _T("setSource(): Mixer is busy!"), _T("Error"), MB_OK | MB_TOPMOST);
      return;
   }

   isBusy_ = true;

   int count = 0;
   while (srcID != getSource() && count < 2)
   {
      cout << "AudioMixer::setSource(): Try #" << (++count) << endl;

      for (int i=0; i<sourceCount_; ++i)
      {
          int iSrcIndex = audioSources_[i].srcIndex;

         if (i == srcID)
         {
            audioSources_[i].isActive = true;

            if (iSrcIndex >= 0 && iSrcIndex < sourceCount_)
                sourceActive_[iSrcIndex].fValue   = 1;
            else
                AvGrabberDebugMsg("AudioMixer::setSource() Not setting active\n", true, i);
         }
         else
         {
            audioSources_[i].isActive = false;

            if (iSrcIndex >= 0 && iSrcIndex < sourceCount_)
                sourceActive_[iSrcIndex].fValue   = 0;
            else
                AvGrabberDebugMsg("AudioMixer::setSource() Not setting inactive\n", true, i);
         }
      }

      // now set the values in the mux Control:
      muxDetails_->cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
      muxDetails_->paDetails = sourceActive_;

      MMRESULT res = mixerSetControlDetails((HMIXEROBJ) mixer_, muxDetails_,
                                            MIXER_SETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER);

      if (res != MMSYSERR_NOERROR)
      {
         cerr << "*** Could not set active source to " << srcID << ". Error: " << res << endl;
         return;
      }
   }

   activeSource_ = getSource();

   if (srcID != getSource())
   {
      _TCHAR msg[256];
      _TCHAR cap[256];
      _stprintf(msg, Local::localizeString(MIXER_SETSOURCE_FAILED), audioSources_[activeSource_].tszName);
      _tcscpy(cap, Local::localizeString(MSGBOX_WARNING));
      MessageBox(NULL, msg, cap, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
   }

   isBusy_ = false;
}

int AudioMixer::getVolume()
{
    AvGrabberDebugMsg("AudioMixer::getVolume()\n");

   if (!isEnabled())
      return -1;

   if (!audioSources_[activeSource_].hasVolumeControl)
      return -1;

   // retrieve the current volume:
   MMRESULT res = mixerGetControlDetails((HMIXEROBJ) mixer_, &sourceDetails_[activeSource_],
                                         MIXER_GETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER);

   if (res != MMSYSERR_NOERROR)
   {
      cerr << "mixerGetControlDetails(getVolume[" << activeSource_ << "]) failed: " << res << endl;
      return -1;
   }

   int vol = sourceVolumes_[activeSource_].dwValue;

   // normalize the volume:
   if (vol < 0)
      vol = 0;
   if (vol > audioSources_[activeSource_].dwMaxVolume)
      vol = audioSources_[activeSource_].dwMaxVolume;

   return (vol * 100 / audioSources_[activeSource_].dwMaxVolume);
}

void AudioMixer::setVolume(int volume)
{
   if (!isEnabled())
      return;
   if (!audioSources_[activeSource_].hasVolumeControl)
      return;
   if (isBusy_)
      MessageBox(NULL, _T("setVolume(): Mixer is busy!"), _T("Error"), MB_OK | MB_TOPMOST);

   isBusy_ = true;

   if (volume > 100)
      volume = 100;
   if (volume < 0)
      volume = 0;

   DWORD oneStep = (audioSources_[activeSource_].dwMaxVolume - 
      audioSources_[activeSource_].dwMinVolume) / audioSources_[activeSource_].dwSteps;
   int myStep = (int) ((((float) volume) / 100.0) * audioSources_[activeSource_].dwSteps); 
   DWORD vol = oneStep * myStep;

   int realVolume = vol * 100 / audioSources_[activeSource_].dwMaxVolume;

   int count = 0;
   while (getVolume() != realVolume && (++count) < 5)
   {
      cout << "AudioMixer::setVolume(" << volume << " ~= " << vol << "), Try #" << count << endl;

      sourceVolumes_[activeSource_].dwValue = vol;

      MMRESULT res = mixerSetControlDetails((HMIXEROBJ) mixer_, &sourceDetails_[activeSource_],
                                            MIXER_SETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER);
      if (res != MMSYSERR_NOERROR)
      {
         cerr << "*** mixerSetControlDetails(volume == " << volume << ") failed: " << res << endl;
      }
   }

   if (realVolume != getVolume())
   {
      _TCHAR msg[256];
      _tcscpy(msg, Local::localizeString(MIXER_SETVOLUME_FAILED));
      _TCHAR cap[256];
      _tcscpy(cap, Local::localizeString(MSGBOX_WARNING));
      MessageBox(NULL, msg, cap, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
   }

   isBusy_ = false;
}

AudioMixer* AudioMixer::instance = NULL;

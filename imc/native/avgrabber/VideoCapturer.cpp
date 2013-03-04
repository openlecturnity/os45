#include "global.h"
#include "resource.h"

#include "LanguageSupport.h"

#pragma warning(disable:4786)
//#include "VideoCapturer.h"
//#include <stdio.h>

//#define _DEBUGFILE

#ifdef _DEBUG
#define DEBUG
#define _DEBUGFILE
#endif

#define MONITOR_WINDOW_CAPTION _T("Lecturnity Video Tool - Monitor")

#ifdef _DEBUGFILE
FILE *debugFile = fopen("C:\\vidcap_debug.txt", "w");
#endif

// initialize static variables:
int VideoCapturer::globalnID_          = 0x00022100;
static map<HWND, VideoCapturer *> instanceMap;

/*
 * Constructor
 */
VideoCapturer::VideoCapturer(UINT driverId, bool preview)
{
#ifdef DEBUG
	printf("VideoCapturer(%d)\n", driverId);
#endif

#ifdef _DEBUGFILE
   fprintf(debugFile, "VideoCapturer()\n");
	fflush(debugFile);
#endif

   driverId_ = driverId;

   // for the ScreenCapturer and the WdmCapturer, driverId equals -1
   if (driverId == -1)
      return;

   // initialize all variables:
   initVariables();

   // register the monitor window class:
   registerMonitorWndClass();

   // now for the VfW stuff...
	preview_           = preview;

   // This event is used to signalize when capturing is done
	captureDoneEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);

   // This event is used to signalize when the initialization
   // process of the capture device has finished
	captureInitEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);

   // This event is used to signalize when the preroll phase of the
   // capture driver has finished
	prerollEvent_     = CreateEvent(NULL, TRUE, FALSE, NULL);

   // Did we get handles for all the needed events?
	if (captureDoneEvent_ == NULL || captureInitEvent_ == NULL ||
       prerollEvent_ == NULL)
	{
      // No, bail out
		destroy();
		throw VideoException(Local::localizeString(VIDEO_NOEVENTS));
	}

   // Everything looks fine, launch the initialization thread.
   // This has to be a thread as all the actions on the capture
   // window have to be done from the same thread and we want
   // control to return to the calling thread (the thread which
   // instanciated this VideoCapture instance).
	launchInitThread();

	// now make sure the constructor does not return before
	// the driver is ready for capture! The event is set from
   // the captureWindowLoop() function after the window
   // has been created, set up and is ready for start.
	DWORD evRes = WaitForSingleObject(captureInitEvent_, 30000); // wait max. 30 secs

   // Did an exception occur while initializing the capture
   // window?
	if (hasExceptionOccurred_ != false)
	{
      // Yes! Destroy this object and release all
      // resources:
		destroy();

      // Reset the instance variable:
		// instance = NULL;

      // unregister the instance variable, if necessary:
      unregisterInstance();

      // Propagate the exception error message;
      // the message is copied in the captureWindowLoop()
		throw VideoException(exceptionMsg_);
	}

   // No exception occurred during creation, we may proceed.
   // Now check if we received the initDoneEvent_:
	switch (evRes)
	{
	case WAIT_OBJECT_0:
		// everythings fine, just
		break;

	case WAIT_ABANDONED:
      // The wait call was abandoned. Whatever that means.
		throw VideoException(Local::localizeString(VIDEO_INIT_ERROR));

	case WAIT_TIMEOUT:
      // We waited very long, but the capture window could not
      // be initialized. Throw an exception to notify the caller
		throw VideoException(Local::localizeString(VIDEO_INIT_TIMEOUT));
	}
}

void VideoCapturer::initVariables()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "initVariables\n");
  fflush(debugFile);
#endif

   // Initialize the instance variable (static variable)
   // with this instance of VideoCapturer:
	// instance = this;

   // Check for a performance counter:
   performanceFrequency_.QuadPart = 0i64;
   BOOL hasRtCounter = QueryPerformanceFrequency(&performanceFrequency_);
   if (hasRtCounter == FALSE)
   {
      hasPerformanceCounter_ = false;
      // Ok, there is not real performance counter to use. Then try
      // to set the time exactness to 1 ms:
      MMRESULT res = timeBeginPeriod((UINT) 1);
      if (res != TIMERR_NOERROR)
      {
         printf("*** Note: Timer is very inexact on this system.\n");
         hasGoodTimer_ = false;
      }
      else
         hasGoodTimer_ = true;
	}
   else
   {
      hasPerformanceCounter_ = true;
      printf("Performance counter frequency: %d\n", performanceFrequency_.QuadPart);
   }

	hWnd_               = NULL;
	captureHWnd_        = NULL;
	videoFormat_        = NULL;
   videoFormatSize_    = 0;
	captureDoneEvent_   = NULL;
	monitorDisplayed_   = false;
	driverConnected_    = false;
	captureInProgress_  = false;
   captureStartSuccess_= false;
	isPaused_           = false;
	wasPaused_          = true;
	lastStartTime_      = 0;
	firstTime_          = -1;
   totalSize_          = 0;
   frameCounter_       = 0;
   aviFileFrameCounter_= 0;

   captureDoneEvent_   = NULL;
   captureInitEvent_   = NULL;
   prerollEvent_       = NULL;

   hasCapErrorOccurred_ = false;

	audio_              = NULL;

   // By default do not use
   // the advanced video/audio synchronization mode.
   useAdvancedAudioSyncMode_ = false;
	audioSync_          = !useAdvancedAudioSyncMode_;
	audioVideoOffset_   = 0;
   useFixedAudioVideoOffset_ = false;
   fixedAudioVideoOffset_ = 0;
   lInternalAvOffset_ = 0;
	
	usecsPerFrame_     = 40000; // 25 fps
   frameRate_         = 25.0;
	preview_           = false;

   // thumbnail things
   decompressForThumbnail_ = false;
   thumbnailPossible_      = false;
   enableThumbnails_       = true;
   thumbnailInfo_          = NULL;
   thumbnailData_          = NULL;

   // start in normal (non-nullframe mode):
   bNullFramesMode_        = false;
}

/*
 * Destructor
 */
VideoCapturer::~VideoCapturer()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "~VideoCapturer\n");
  fflush(debugFile);
#endif
   // for the ScreenCapturer, driverId equals -1
   if (driverId_ == -1)
   {
      if (thumbnailData_ != NULL)
         free(thumbnailData_);
      thumbnailData_ = NULL;

      if (thumbnailInfo_ != NULL)
         free(thumbnailInfo_);
      thumbnailInfo_ = NULL;
      return;
   }

	destroy();

   if (!hasPerformanceCounter_)
   {
      // Reset the timer settings:
      MMRESULT res = timeEndPeriod((UINT) 1);
   }

   // Release any eventual event handlers:
   if (captureDoneEvent_)
   	CloseHandle(captureDoneEvent_);
   captureDoneEvent_ = NULL;
   if (captureInitEvent_)
	   CloseHandle(captureInitEvent_);
   captureInitEvent_ = NULL;
   if (prerollEvent_)
   	CloseHandle(prerollEvent_);
   prerollEvent_ = NULL;

   //instance = NULL;
   unregisterInstance();
}

void VideoCapturer::destroy()
{
#ifdef _DEBUGFILE
   fprintf(debugFile, "destroy\n");
   fflush(debugFile);
#endif

   // Are we still capturing?
	if (captureInProgress_) // if yes, stop that first!
		stop();

   // Then destroy the capture window:
	destroyCaptureWindow();

   // And free the memory:
	freeMem();
}

void VideoCapturer::registerInstance(HWND hWnd)
{
   registeredHwnd_   = hWnd;
   instanceMap[hWnd] = this;
}

void VideoCapturer::unregisterInstance()
{
   instanceMap.erase(registeredHwnd_);
   registeredHwnd_ = NULL;
}

VideoCapturer * VideoCapturer::retrieveInstance(HWND hWnd)
{
   VideoCapturer *pVc = instanceMap[hWnd];
   return pVc;
}

bool VideoCapturer::hasGoodTimer()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "hasGoodTimer\n");
  fflush(debugFile);
#endif

   return hasGoodTimer_ || hasPerformanceCounter_;
}

void VideoCapturer::fourCCToString(FOURCC fcc, char* fccs)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "fourCCToString\n");
  fflush(debugFile);
#endif

   fccs[0] = (char) (fcc & 0x000000ff);
   fccs[1] = (char) (fcc >> 8 & 0x000000ff);
   fccs[2] = (char) (fcc >> 16 & 0x000000ff);
   fccs[3] = (char) (fcc >> 24 & 0x000000ff);
   fccs[4] = (char) 0;
}

void VideoCapturer::freeMem()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "freeMem\n");
  fflush(debugFile);
#endif

   if (thumbnailData_ != NULL)
      free(thumbnailData_);
   thumbnailData_ = NULL;

   if (thumbnailInfo_ != NULL)
      free(thumbnailInfo_);
   thumbnailInfo_ = NULL;

   if (videoFormat_ != NULL)
   {
      free(videoFormat_);
      videoFormat_ = NULL;
      videoFormatSize_ = 0;
   }
   
   // Unregister the class for the monitor window:
   // MessageBox(NULL, "UnregisterClass", NULL, MB_OK);
   UnregisterClass(MONITOR_WINDOW_CAPTION, g_hDllInstance);
}

void VideoCapturer::launchInitThread()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "launchInitThread\n");
  fflush(debugFile);
#endif

#ifdef DEBUG
	printf("VideoCapturer::launchInitThread()\n");
#endif

   unsigned long thread =
      _beginthread(captureWindowLoop, 0, this);
}

void __cdecl VideoCapturer::captureWindowLoop(LPVOID param)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "captureWindowLoop\n");
  fflush(debugFile);
#endif

#ifdef DEBUG
	printf("VideoCapturer::captureWindowLoop()\n");
#endif

   CLanguageSupport::SetThreadLanguage();

	MSG msg;
	VideoCapturer *vidCap = (VideoCapturer *) param;

	try
	{
		vidCap->hasExceptionOccurred_ = false;

		vidCap->initCaptureWindow();
		vidCap->initDriverCaps();
		vidCap->updateVideoFormat();
		vidCap->displayMonitor(vidCap->preview_);

		// we're done: set event for the constructor (see there)
		SetEvent(vidCap->captureInitEvent_);

		while (GetMessage(&msg, vidCap->hWnd_, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return;
	}
	catch (exception &e)
	{
		printf("%%%%%%%% EXCEPTION: %d\n", e.what());

		vidCap->hasExceptionOccurred_ = true;
#ifdef _UNICODE
      ::MultiByteToWideChar(CP_ACP, 0, e.what(), strlen(e.what()), vidCap->exceptionMsg_, 512);
#else
		strcpy(vidCap->exceptionMsg_, e.what());
#endif

		// we're done: set event for the constructor (see there)
		SetEvent(vidCap->captureInitEvent_);
		return;
	}
}

void VideoCapturer::registerMonitorWndClass()
{
   WNDCLASSEX  wc;
   HINSTANCE hInstance = g_hDllInstance;
   _TCHAR tszAppName[100];

   _stprintf(tszAppName, MONITOR_WINDOW_CAPTION);

	// Fill in window class structure with parameters that describe
   // the main window.
   memset(&wc, 0, sizeof(wc));
   
   wc.cbSize        = sizeof(WNDCLASSEX);
   wc.style         = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
   wc.lpfnWndProc   = (WNDPROC) MonitorWindowProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance; // g_hDllInstance;
   wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = tszAppName;
   wc.hIcon         = NULL; // LoadIcon(hInstance, MAKEINTRESOURCE(ASSISTANT_ICON));
   wc.hIconSm       = NULL; // LoadIcon(hInstance, MAKEINTRESOURCE(ASSISTANT_ICON));

   // MessageBox(NULL, "RegisterClassEx()", NULL, MB_OK);
   if (RegisterClassEx(&wc) == NULL)
   {
      // we'll output a note in debug note, ignore otherwise
#ifdef _DEBUG
      printf("RegisterClassEx(&wc) failed.\n");
#endif
   }
}

/*
 * Initializes a capture window
 */
void VideoCapturer::initCaptureWindow()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "initCaptureWindow\n");
  fflush(debugFile);
#endif

#ifdef DEBUG
	printf("VideoCapturer::initCaptureWindow()\n");
#endif
   HINSTANCE hInstance = g_hDllInstance;
   _TCHAR tszAppName[100];

   _stprintf(tszAppName, MONITOR_WINDOW_CAPTION);

   hWnd_ = CreateWindowEx(
      WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
      tszAppName,
      tszAppName,
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
      0, 0, 5, 5,
      NULL, NULL, hInstance, NULL);

   // Did that work?
   if (!hWnd_) 
   {
      // Nope, bail out.
      destroy();
      throw VideoException(Local::localizeString(VIDEO_NOWINDOW));
   }

   // Do not make the window visible first:
   ShowWindow(hWnd_, SW_HIDE);
   UpdateWindow(hWnd_);

   // Now, create the real capture window:
   captureHWnd_ = capCreateCaptureWindow
      (_T("lecturnity"), WS_CHILD | WS_VISIBLE,
      0, 0, 320, 240, hWnd_, globalnID_++);
   
   if (captureHWnd_ == NULL)
   {
      printf("*** Capture window could not be created!\n");
      destroy();
      throw VideoException(Local::localizeString(VIDEO_NOCAPWIN));
   }

   // register the window with this VideoCapturer instance:
   registerInstance(captureHWnd_);
   
   SetWindowPos(captureHWnd_, NULL, 0, 0, 320, 240, 0);
}

void VideoCapturer::destroyCaptureWindow()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "destroyCaptureWindow\n");
  fflush(debugFile);
#endif

#ifdef DEBUG
	printf("VideoCapturer::destroyCaptureWindow()\n");
#endif

	if (driverConnected_)
	{
#ifdef _DEBUGFILE
  fprintf(debugFile, "  capDriverDisconnect\n");
  fflush(debugFile);
#endif
		BOOL res = capDriverDisconnect(captureHWnd_);
#ifdef DEBUG
		if (!res)
		{
			printf("capDriverDisconnect() failed!\n");
		}
#endif
	}

	if (captureHWnd_ != NULL)
   {
#ifdef _DEBUGFILE
  fprintf(debugFile, "  DestroyWindow(captureHWnd_)\n");
  fflush(debugFile);
#endif
		DestroyWindow(captureHWnd_);
   }
	captureHWnd_ = NULL;

	if (hWnd_ != NULL)
   {
#ifdef _DEBUGFILE
  fprintf(debugFile, "  PostMessage(hWnd_, WM_COMMAND, 0, 0)\n");
  fflush(debugFile);
#endif
      PostMessage(hWnd_, WM_COMMAND, 0, 0);
   }
	hWnd_ = NULL;
}

LRESULT CALLBACK VideoCapturer::MonitorWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//#ifdef _DEBUGFILE
//  fprintf(debugFile, "MonitorWindowProc\n");
//  fflush(debugFile);
//#endif

   switch (uMsg)
   {
   case WM_COMMAND:
      DestroyWindow(hWnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;
   }
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/*
 * Displays or hides the preview monitor
 */
void VideoCapturer::displayMonitor(bool b)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "displayMonitor\n");
  fflush(debugFile);
#endif

#ifdef DEBUG
	printf("VideoCapturer::displayMonitor(%d)\n", b);
#endif

   // Has a state change occurred?
	if (b && !monitorDisplayed_)
	{
		// Display monitor!
		if (driverCaps_.fHasOverlay)
			capOverlay(captureHWnd_, TRUE);
		else
		{
			capPreview(captureHWnd_, TRUE);
			capPreviewRate(captureHWnd_, 100); // 10 fps
		}

		ShowWindow(hWnd_, SW_SHOWNORMAL);

		monitorDisplayed_ = true;
	}
	else if (!b && monitorDisplayed_)
	{
      // Hide monitor.
		ShowWindow(hWnd_, SW_HIDE);

		if (driverCaps_.fHasOverlay)
			capOverlay(captureHWnd_, FALSE);
		else
			capPreview(captureHWnd_, FALSE);

		monitorDisplayed_ = false;
	}

	// Find out size of capture window:
	RECT rectCapture;
	GetWindowRect(captureHWnd_, &rectCapture);

	int width = rectCapture.right - rectCapture.left;
	int height = rectCapture.bottom - rectCapture.top;

   RECT rectClient;
	RECT rectWindow;

	if (width > 0 && height > 0) 
	{
		GetClientRect(hWnd_, &rectClient);
		GetWindowRect(hWnd_, &rectWindow);
		width += rectWindow.right - rectWindow.left - rectClient.right;
		height += rectWindow.bottom - rectWindow.top - rectClient.bottom;
		SetWindowPos(hWnd_, HWND_TOP,
                   0, 0, width, height, SWP_NOMOVE);
	}

	UpdateWindow(hWnd_);
}

void VideoCapturer::initDriverCaps()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "initDriverCaps\n");
  fflush(debugFile);
#endif

	BOOL res;

	res = capDriverConnect(captureHWnd_, driverId_);

	if (res == FALSE)
	{
		destroy();
		throw VideoException(Local::localizeString(VIDEO_DEV_NOCONNECT));
	}

	driverConnected_ = true;

   // Get the driver capabilities:
   res = capDriverGetCaps(captureHWnd_, &driverCaps_, sizeof(CAPDRIVERCAPS));

	if (res == FALSE)
	{
      printf("*** capDriverGetCaps() failed!\n");
		destroy();
		throw VideoException(Local::localizeString(VIDEO_DEV_NOCAPS));
	}

	// wait until the capture driver is ready
	while (driverCaps_.fCaptureInitialized == FALSE)
	{
#ifdef DEBUG
		printf("Capture driver not yet ready...\n");
#endif
		capDriverGetCaps(captureHWnd_, &driverCaps_, sizeof(CAPDRIVERCAPS));
		Sleep(100);
	}

#ifdef DEBUG
	printf("Driver caps:\n");
	printf("  wDeviceIndex       : %d\n", driverCaps_.wDeviceIndex);
	printf("  fHasOverlay        : %d\n", driverCaps_.fHasOverlay);
	printf("  fHasDlgVideoSource : %d\n", driverCaps_.fHasDlgVideoSource);
	printf("  fHasDlgVideoFormat : %d\n", driverCaps_.fHasDlgVideoFormat);
	printf("  fHasDlgVideoDisplay: %d\n", driverCaps_.fHasDlgVideoDisplay);
	printf("  fCaptureInitialized: %d\n", driverCaps_.fCaptureInitialized);
	printf("  fDriverSuppliesPalettes: %d\n", driverCaps_.fDriverSuppliesPalettes);
#endif

   // Read out the capture parameters set in the current
   // capture driver:
	res = capCaptureGetSetup(captureHWnd_, &captureParms_, sizeof(CAPTUREPARMS));

	if (res == FALSE)
	{
		destroy();
		throw VideoException(Local::localizeString(VIDEO_DEV_NOSETTINGS));
	}

#ifdef DEBUG
	dumpCaptureParms();
#endif

   // Now set the desired parameters:
   // Do not abort on left mouse button:
	captureParms_.fAbortLeftMouse           = FALSE;
   // And not on right mouse button:
	captureParms_.fAbortRightMouse          = FALSE;
   // We do not want the user to press "OK" before capture starts
	captureParms_.fMakeUserHitOKToCapture   = FALSE;
   // We do not want a time limit for the capture
	captureParms_.wTimeLimit                = 0;
   // This is the requested frame rate
	captureParms_.dwRequestMicroSecPerFrame = (DWORD) usecsPerFrame_;
   // Set the number of video buffers:
	captureParms_.wNumVideoRequested        = VIDEO_BUFFERS;
   // Do not capture audio, we'll do that by ourselves
	captureParms_.fCaptureAudio             = FALSE;
   // Do not define any hotkeys for capture abortion
	captureParms_.vKeyAbort                 = 0;
   // Yield must be set to true, otherwise we will not receíve any callbacks
	captureParms_.fYield                    = TRUE;

	res = capCaptureSetSetup(captureHWnd_, &captureParms_, sizeof(CAPTUREPARMS));
	if (res == FALSE)
	{
		destroy();
		throw VideoException(Local::localizeString(VIDEO_DEV_NOSETSETTINGS));
	}

	res = capCaptureGetSetup(captureHWnd_, &captureParms_, sizeof(CAPTUREPARMS));

#ifdef DEBUG
	printf("=== After capCaptureSetSetup()\n");
	dumpCaptureParms();
#endif
}

bool VideoCapturer::hasFormatDialog()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "hasFormatDialog\n");
  fflush(debugFile);
#endif

	return (driverCaps_.fHasDlgVideoFormat ? true : false);
}

void VideoCapturer::displayFormatDialog()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "displayFormatDialog\n");
  fflush(debugFile);
#endif

   if (!hasFormatDialog())
      return;

   bool backupDisplay = monitorDisplayed_;
   if (!backupDisplay)
      displayMonitor(true);
	capDlgVideoFormat(captureHWnd_);
   if (!backupDisplay)
      displayMonitor(false);

   // this cannot hurt: set the capture parms again
   setCaptureParms();

	updateVideoFormat();
}

bool VideoCapturer::hasSourceDialog()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "hasSourceDialog\n");
  fflush(debugFile);
#endif

	return (driverCaps_.fHasDlgVideoSource ? true : false);
}

void VideoCapturer::displaySourceDialog()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "displaySourceDialog\n");
  fflush(debugFile);
#endif

   if (!hasSourceDialog())
      return;

   bool backupDisplay = monitorDisplayed_;
   if (!backupDisplay)
      displayMonitor(true);
	capDlgVideoSource(captureHWnd_);
   if (!backupDisplay)
      displayMonitor(false);

   // this cannot hurt: set the capture parms again
   setCaptureParms();

	updateVideoFormat();
}

bool VideoCapturer::hasDisplayDialog()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "hasDisplayDialog\n");
  fflush(debugFile);
#endif

	return (driverCaps_.fHasDlgVideoDisplay ? true : false);
}

void VideoCapturer::displayDisplayDialog()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "displayDisplayDialog\n");
  fflush(debugFile);
#endif

   if (!hasDisplayDialog())
      return;

   bool backupDisplay = monitorDisplayed_;
   if (!backupDisplay)
      displayMonitor(true);
	capDlgVideoDisplay(captureHWnd_);
   if (!backupDisplay)
      displayMonitor(false);

   // this cannot hurt: set the capture parms again
   setCaptureParms();

   updateVideoFormat();
}

void VideoCapturer::dumpCaptureParms()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "dumpCaptureParms\n");
  fflush(debugFile);
#endif

	printf("Capture params:\n");
	printf("  dwRequestMicroSecPerFrame: %d\n", captureParms_.dwRequestMicroSecPerFrame);
	printf("  fMakeUseHitOKToCapture   : %d\n", captureParms_.fMakeUserHitOKToCapture);
	printf("  wPercentDropForError     : %d\n", captureParms_.wPercentDropForError);
	printf("  fYield                   : %d\n", captureParms_.fYield);
	printf("  dwIndexSize              : %d\n", captureParms_.dwIndexSize);
	printf("  wChunkGranularity        : %d\n", captureParms_.wChunkGranularity);
	printf("  wNumVideoRequested       : %d\n", captureParms_.wNumVideoRequested);
	printf("  vKeyAbort                : %d\n", captureParms_.vKeyAbort);
	printf("  fAbortLeftMouse          : %d\n", captureParms_.fAbortLeftMouse);
	printf("  fAbortRightMouse         : %d\n", captureParms_.fAbortRightMouse);
	printf("  fLimitEnabled            : %d\n", captureParms_.fLimitEnabled);
	printf("  fMCIControl              : %d\n", captureParms_.fMCIControl);
	printf("  fCaptureAudio            : %d\n", captureParms_.fCaptureAudio);
}

void VideoCapturer::setCaptureParms()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "setCaptureParms\n");
  fflush(debugFile);
#endif

	capCaptureSetSetup(captureHWnd_, &captureParms_, sizeof(CAPTUREPARMS));
}

string getCompression(DWORD c)
{
	switch (c)
	{
	case BI_RGB: return "BI_RGB";
	case BI_RLE8: return "BI_RLE8";
	case BI_RLE4: return "BI_RLE4";
	case BI_BITFIELDS: return "BI_BITFIELDS";
	}

	string tmp = "[unknown]";

	return tmp;
}

void VideoCapturer::updateVideoFormat()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "updateVideoFormat\n");
  fflush(debugFile);
#endif

   DWORD dwFormatSize = capGetVideoFormatSize(captureHWnd_);

   if (dwFormatSize == 0)
   {
      destroy();
      throw VideoException(Local::localizeString(VIDEO_FORMAT_SIZE));
   }

	if (videoFormat_ == NULL || dwFormatSize != videoFormatSize_)
	{
      videoFormatSize_ = dwFormatSize;

      if (videoFormat_ == NULL)
      {
         free(videoFormat_);
         videoFormat_ = NULL;
      }

      videoFormat_ = (BITMAPINFO*) malloc(videoFormatSize_);

		if (videoFormat_ == NULL)
		{
			destroy();
			throw MemoryVideoException(Local::localizeString(VIDEO_NOMEM_BITMAP));
		}
	}

	HRESULT res = capGetVideoFormat(captureHWnd_, videoFormat_, videoFormatSize_);
   if (res == 0)
   {
      destroy();
      throw VideoException(Local::localizeString(VIDEO_FORMAT_SIZE));
   }

	SetWindowPos(captureHWnd_, NULL, 0, 0, videoFormat_->bmiHeader.biWidth,
                videoFormat_->bmiHeader.biHeight, 0);

	displayMonitor(monitorDisplayed_);

#ifdef DEBUG
	BITMAPINFOHEADER* bmih = &(videoFormat_->bmiHeader);

	printf("Video: \n");
	printf("  biSize == %d\n", bmih->biSize);
	printf("  biWidth == %d\n", bmih->biWidth);
	printf("  biHeight == %d\n", bmih->biHeight);
	printf("  biPlanes == %d\n", bmih->biPlanes);
	printf("  biBitCount == %d\n", bmih->biBitCount);
	printf("  biCompression == %s (%08x)\n", getCompression(bmih->biCompression).c_str(), bmih->biCompression);
	printf("  biSizeImage == %d\n", bmih->biSizeImage);
	printf("  biXPelsPerMeter == %d\n", bmih->biXPelsPerMeter);
	printf("  biYPelsPerMeter == %d\n", bmih->biYPelsPerMeter);
	printf("  biClrUsed == %d\n", bmih->biClrUsed);
	printf("  biClrImportant == %d\n", bmih->biClrImportant);
#endif
}

/*
 * Copies the BITMAPINFO structure of the video format into the 
 * videoFormat variable. Note: Any eventual palette is _not_
 * copied by this method. Only sizeof(BITMAPINFO) bytes are
 * copied. The AvGrabber class relies on this to be like
 * that!
 */
void VideoCapturer::getVideoFormat(BITMAPINFO* videoFormat)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getVideoFormat\n");
  fflush(debugFile);
#endif

	memcpy(videoFormat, videoFormat_, sizeof BITMAPINFO);
}

unsigned long VideoCapturer::getVideoLengthMs()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getVideoLengthMs\n");
  fflush(debugFile);
#endif

   if (aviHeader_)
      return (unsigned long) (0.001 * ((double) (usecsPerFrame_ * aviOutputFile_->getCurrentFrame())));
   else
      return (unsigned long) (0.001 * ((double) (usecsPerFrame_ * aviFileFrameCounter_)));
   //return (unsigned long) (0.001 * ((double) (usecsPerFrame_ * frameCounter_)));
}

void VideoCapturer::getDriverCaps(CAPDRIVERCAPS* driverCaps)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getDriverCaps\n");
  fflush(debugFile);
#endif

	memcpy(driverCaps, &driverCaps_, sizeof(CAPDRIVERCAPS));
}

void VideoCapturer::initStartVariables(_TCHAR* tszFileName, AudioCapturer* audio)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "initStartVariables\n");
  fflush(debugFile);
#endif

   // copy the file name:
	_tcscpy(szFileName_, tszFileName);

   ulFirstControlTime_    = 0;
   bFirstControlDone_     = false;
   if (audio != NULL)
	{
		audio_              = audio;
		audioSync_          = true;
#ifdef _DEBUG
      printf("Audio<->Video sync.\n");
#endif
      bAudioControlDone_  = false;
	}
	else
	{
		audioSync_ = false;
	}

   isPaused_           = false;
	wasPaused_          = true;
	lastStartTime_      = 0;
	firstTime_          = -1;
   totalSize_          = 0;

	// reset the frame counters:
	frameCounter_       = 0;
	dropCounter_        = 0;
	dropCounterAudio_   = 0;
	prerollDone_        = false;
   lastThumbnailFrame_ = -100; // to make sure the first frame is copied

   // reset the last returned reference time:
   lastReturnedRefTime_ = 0;

   hasCapErrorOccurred_ = false;
   captureStartSuccess_ = false;

   if (codecInfo_.bSupportsDatarate && codecInfo_.bUseDatarate)
   {
      // set the accumulated frame byte quota to zero to start with:
      accFrameByteQuota_ = 0;

      // Calculate the mean byte quota for a single frame:
      double framesPerSec = 1000000.0 / ((double) usecsPerFrame_);
      frameByteQuota_     = (DWORD) (((double) codecInfo_.iDatarate * 1024.0) / framesPerSec);

#ifdef _DEBUG
      printf("frameByteQuota_ == %d\n", frameByteQuota_);
      printf("framesPerSec    == %d\n", framesPerSec);
      printf("frameRate_      == %f\n", frameRate_);
#endif
   }
   else
   {
      // Don't use frame crunching
      accFrameByteQuota_ = 0xffffff; // maximum
      frameByteQuota_    = 0;
   }

#ifdef _DEBUGFILE
      fprintf(debugFile, "frameByteQuota_ == %d\n", frameByteQuota_);
//      fprintf(debugFile, "framesPerSec    == %d\n", framesPerSec);
      fprintf(debugFile, "frameRate_      == %f\n", frameRate_);
      fflush(debugFile);
#endif

   // init audio/video factor to 1, can't be that wrong...
	audioVideoFactor_ = 1.0;

   // start in non-audiosync-mode? if useAdvancedAudioSyncMode_ is
   // false, then audioSyncMode_ is true, and then the advanced
   // sync mode is never used, as normal synchronization is done
   // from the start.
   audioSyncMode_    = !useAdvancedAudioSyncMode_; 

	checkSyncInterval_ = (int) (1000000.0 / usecsPerFrame_);
	printf("Checking sync every %d frames.\n", checkSyncInterval_);

	// set default media threshold half a frame, plus marginal of 2ms
	mediaThreshold_ = usecsPerFrame_ / 2i64 + 2000i64; //  / 4i64 * 3i64;
#ifdef _DEBUG
   printf("Media threshold: %d\n", (long) mediaThreshold_);
#endif

	mostRecentFrameWritten_ = 0;
	mostRecentTimeCaptured_ = -1; // uninitialized
	pausedTime_             = 0;

	// open the output file. note: if something goes wrong, an exception
	// is thrown
	initAviFile();

   // start in normal (non-nullframe mode):
   bNullFramesMode_        = false;
}

void VideoCapturer::start(_TCHAR* tszFileName, AudioCapturer* audio)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "### start\n");
  fflush(debugFile);
#endif

	printf("VideoCapturer::start()\n");

   // Initialize all the variables needed for starting the
   // recording...
   initStartVariables(tszFileName, audio);

   // The rest of the commands are VfW specific!

	// Register the callbacks:
   capSetCallbackOnError(captureHWnd_, errorCallback);
	capSetCallbackOnVideoStream(captureHWnd_, videoStreamCallback);
	capSetCallbackOnStatus(captureHWnd_, statusCallback);
	capSetCallbackOnCapControl(captureHWnd_, controlCallback);

	printf("==> VideoCapturer::starting capture sequence!\n");

	captureInProgress_ = true;

   // Now, really start the capture process:
	BOOL success = capCaptureSequenceNoFile(captureHWnd_);

   // Did it work?
   if (success != TRUE)
   {
      // No, throw an exception
      printf("capCaptureSequenceNoFile() failed!\n");
      captureStartSuccess_ = false;
      destroy();
      throw VideoException(Local::localizeString(VIDEO_START_FAILED2));
   }
   else
   {
      // Yes; at stop(), capCaptureAbort() has to be called to
      // really stop the capture process:
      captureStartSuccess_ = true;
   }

	// Wait for capture preroll to finish:
	DWORD res = WaitForSingleObject(prerollEvent_, 10000);

   // Did the preroll event occur?
   switch (res)
   {
   case WAIT_ABANDONED:
   case WAIT_TIMEOUT:
      // No, something went wrong, or we waited too long
      stop();
      if (!hasCapErrorOccurred_)
      {
         hasCapErrorOccurred_ = true;
         _tcscpy(capErrorMsg_, Local::localizeString(VIDEO_START_FAILED));
      }
   }

   if (hasCapErrorOccurred_)
   {
      if (captureInProgress_)
         stop();

      _TCHAR tmp[512];
      _stprintf(tmp, Local::localizeString(VIDEO_CAP_ERROR), capErrorMsg_);
      throw VideoIOException(tmp);
   }

	// Delay it so that capture has definitely started.
   do
   {
      Sleep(50);
   } 
   while (!bFirstControlDone_);

   //	printf("VideoCapturer()::start() returns.\n");
}

void VideoCapturer::finalizeSync()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "finalizeSync\n");
  fflush(debugFile);
#endif

	// if not in audio sync mode, just return
	if (!audioSync_)
		return;

	// calculate the new frame rate:
   TIME_US newUsecsPerFrame_ = (TIME_US) (((double) usecsPerFrame_) * audioVideoFactor_);
	//usecsPerFrame_ = (TIME_US) (((double) usecsPerFrame_) * audioVideoFactor_);
   //frameRate_     = 1000000.0 / usecsPerFrame_;

	printf("New frame rate is: %d us/frame.\n", (long) newUsecsPerFrame_);
	//printf("Video offset     : %d ms\n", getVideoAudioOffset());

	aviOutputFile_->setFrameRate(newUsecsPerFrame_);
}

void VideoCapturer::stop()
{
#ifdef _DEBUGFILE
   fprintf(debugFile, "stop\n");
   fflush(debugFile);
#endif

#ifdef _DEBUG
	printf("VideoCapturer::stop()\n");
#endif
	if (!captureInProgress_)
		return;

   // Did the capCaptureSequenceNoFile succeed?
   if (captureStartSuccess_)
   {
      // Yes, then we have to abort the capture process:
      //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      BOOL res = capCaptureAbort(captureHWnd_);
      //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      if (res == FALSE)
      {
         // What could I do if this fails?
#ifdef _DEBUG
         printf("---> VideoCapturer::stop() capCaptureAbort() failed!\n");
#endif
      }
      
      // Wait for capture to finish:
      // This is not so nice, but what the heck:
      bool inProgress = true;
      CAPSTATUS capStatus;
      
      while (inProgress)
      {
         Sleep(50);
         capGetStatus(captureHWnd_, &capStatus, sizeof(CAPSTATUS));
         inProgress = (capStatus.fCapturingNow == TRUE);
      }
   }

	// close the output file:
	closeAviFile();

	captureInProgress_ = false;

	// Unregister the callbacks:
	capSetCallbackOnVideoStream(captureHWnd_, NULL);
	capSetCallbackOnStatus(captureHWnd_, NULL);
	capSetCallbackOnCapControl(captureHWnd_, NULL);
   capSetCallbackOnError(captureHWnd_, NULL);

   // check if we need to close a decompressor
   if (isThumbnailEnabled() && decompressForThumbnail_)
   {
      ICDecompressEnd(thumbnailHic_);
      ICClose(thumbnailHic_);
      thumbnailPossible_ = false;
   }

#ifdef DEBUG
	printf("Frame counter : %d\n", frameCounter_);
	printf("Dropped frames: %d\n", dropCounter_);
#endif
}

void VideoCapturer::pause()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "pause\n");
  fflush(debugFile);
#endif

#ifdef _DEBUG
	printf("VideoCapturer::pause()\n");
#endif
	// if we're in audio sync mode, we'll have to enter the strict sync mode now
	if (audioSync_)
		enterAudioSyncMode();

	isPaused_ = true;
}

void VideoCapturer::unPause()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "unPause\n");
  fflush(debugFile);
#endif

	isPaused_ = false;
	wasPaused_ = true;
}

int VideoCapturer::getDroppedFramesCount()
{ 
   return dropCounter_; 
}

TIME_US VideoCapturer::setFrameRate(double frameRate)
{
#ifdef _DEBUGFILE
   fprintf(debugFile, "setFrameRate\n");
   fflush(debugFile);
#endif

   // setFrameRate accepts a double frame rate; we need to
   // calculate the frame rate in microsecs per frame:
	usecsPerFrame_ = (TIME_US) (1000000.0 / frameRate);
   if (usecsPerFrame_ < 2000i64)
      usecsPerFrame_ = 2000i64; // otherwise calculations for sleep and frame times do not work
   frameRate_     = 1000000.0 / usecsPerFrame_;

	captureParms_.dwRequestMicroSecPerFrame = (DWORD) usecsPerFrame_;

	return usecsPerFrame_;
}

TIME_US VideoCapturer::getUSecsPerFrame()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getUSecsPerFrame\n");
  fflush(debugFile);
#endif

	return usecsPerFrame_;
}

long VideoCapturer::getVideoAudioOffset()
{
#ifdef _DEBUGFILE
   fprintf(debugFile, "getVideoAudioOffset\n");
   fflush(debugFile);
#endif

   if (!useFixedAudioVideoOffset_)
	   return audioVideoOffset_;
   else
      return fixedAudioVideoOffset_;
}

void VideoCapturer::setUseFixedAudioVideoOffset(bool bFixedOffset, long nOffsetMs)
{
   useFixedAudioVideoOffset_ = bFixedOffset;
   fixedAudioVideoOffset_ = nOffsetMs;
}

void VideoCapturer::initAviFile()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "initAviFile\n");
  fflush(debugFile);
#endif

	aviOutputFile_ = new VideoFile(szFileName_);

	aviHeader_ = (MainAVIHeader*) malloc(sizeof(MainAVIHeader));

	if (aviHeader_ == NULL)
		throw MemoryVideoException(Local::localizeString(VIDEO_NOMEM_HEADER));

	aviStreamHeader_ = (LecturnityAVIStreamHeader*) malloc(sizeof(LecturnityAVIStreamHeader));
	if (aviStreamHeader_ == NULL)
		throw MemoryVideoException(Local::localizeString(VIDEO_NOMEM_HEADER));

	memset(aviHeader_, 0, sizeof(MainAVIHeader));
	aviHeader_->dwMicroSecPerFrame    = usecsPerFrame_; // captureParms_.dwRequestMicroSecPerFrame;
	aviHeader_->dwMaxBytesPerSec      = 0; // 10 MB/s?
	aviHeader_->dwPaddingGranularity  = 0;
	aviHeader_->dwFlags               = AVIF_HASINDEX | AVIF_ISINTERLEAVED; // flag for HASINDEX here
	aviHeader_->dwTotalFrames         = 0; // up to now, will change
	aviHeader_->dwInitialFrames       = 0;
	aviHeader_->dwStreams             = 1; // only video
	aviHeader_->dwSuggestedBufferSize = 0; // videoFormat_->bmiHeader.biSizeImage; // 1 MB?
	aviHeader_->dwWidth               = videoFormat_->bmiHeader.biWidth;
	aviHeader_->dwHeight              = videoFormat_->bmiHeader.biHeight;
	// aviHeader_->dwReserved            = 0; // reserved

	aviOutputFile_->setMainAviHeader(aviHeader_);

	// 'strh' Stream header settings:
	memset(aviStreamHeader_, 0, sizeof(LecturnityAVIStreamHeader));
	aviStreamHeader_->fccType               = mmioStringToFOURCC(_T("vids"), 0);
	aviStreamHeader_->dwFlags               = 0; // leave
	aviStreamHeader_->dwInitialFrames       = 0;
	aviStreamHeader_->dwScale               = usecsPerFrame_; // captureParms_.dwRequestMicroSecPerFrame;  // == dwMicroSecPerFrame above
	aviStreamHeader_->dwRate                = 1000000; // frame rate =  rate / scale
	aviStreamHeader_->dwStart               = 0;
	aviStreamHeader_->dwLength              = 0; // in frames!
	aviStreamHeader_->dwSuggestedBufferSize = videoFormat_->bmiHeader.biSizeImage; // for now
	aviStreamHeader_->dwQuality             = codecInfo_.bSupportsQuality ? codecInfo_.iQuality : -1; // dwQuality
	// if (codecInfo_.fccHandler == BI_RGB)
	aviStreamHeader_->dwSampleSize          = 0; // aviHeader_->dwWidth*aviHeader_->dwHeight*3; // 0 means: can vary in size
	aviStreamHeader_->left					= 0;
	aviStreamHeader_->top					= 0;
	aviStreamHeader_->right					= videoFormat_->bmiHeader.biWidth;
	aviStreamHeader_->bottom				= videoFormat_->bmiHeader.biHeight;

	// give the stream header to the file handler:
	aviOutputFile_->setAVIStreamHeader(aviStreamHeader_, sizeof(LecturnityAVIStreamHeader));
	// 'strf' format tag settings:
	aviOutputFile_->setBitmapInfoHeader(videoFormat_, videoFormatSize_);
	// set the codec
	aviOutputFile_->setCodec(&codecInfo_);
#ifdef _DEBUGFILE
   fprintf(debugFile, "codec quality: %d\n", codecInfo_.iQuality);
   fflush(debugFile);
#endif

	// init codec and its (eventual) buffers
	aviOutputFile_->initCodec();

	// write the headers...
	aviOutputFile_->writeAviHeaders();

	aviStreamHeader_->dwLength = 0;
	aviHeader_->dwTotalFrames  = 0;
	frameCounter_              = 0;
	dropCounter_               = 0;
	dropCounterAudio_          = 0;
	lastStartTime_             = 0;

   // now check the input format for the thumbnails
   if (enableThumbnails_)
      initThumbnailMaker();

	// we're done. here we go.
}

bool VideoCapturer::closeAviFile()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "closeAviFile\n");
  fflush(debugFile);
#endif

	if (aviOutputFile_ != NULL)
	{
      // Remember the number of frames written to the file
      aviFileFrameCounter_ = aviOutputFile_->getCurrentFrame();

		// close "AVI " RIFF chunk
		aviOutputFile_->finalizeAviFile();
	
		// close physical file:
		aviOutputFile_->close();
	
		delete aviOutputFile_;
		aviOutputFile_ = NULL;
	}

	if (aviHeader_ != NULL)
	{
		free(aviHeader_);
		aviHeader_ = NULL;
	}

	if (aviStreamHeader_ != NULL)
	{
		free(aviStreamHeader_);
		aviStreamHeader_ = NULL;
	}

	return true;
}

void VideoCapturer::initThumbnailMaker()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "initThumbnailMaker\n");
  fflush(debugFile);
#endif

   printf("VideoCapturer::initThumbnailMaker()\n");

   // init to false:
   thumbnailPossible_ = false;

   // init the thumbnail buffer
   if (thumbnailInfo_ != NULL)
   {
      free(thumbnailInfo_);
      thumbnailInfo_ = NULL;
   }
   if (thumbnailData_ != NULL)
   {
      free(thumbnailData_);
      thumbnailData_ = NULL;
   }


   // the BI_RGB format is ok, we can handle that without
   // using a codec.
   if (videoFormat_->bmiHeader.biCompression == BI_RGB)
   {
#ifdef _DEBUG
      printf("--- BI_RGB type, no decompression for thumbnail\n");
#endif

      decompressForThumbnail_ = false;
      thumbnailPossible_      = true;

      // we'll just create a copy of the video format and
      // create a buffer for the image
      thumbnailInfo_ = (BITMAPINFO*) malloc(videoFormatSize_);
      if (thumbnailInfo_ == NULL)
      {
         thumbnailPossible_ = false;
         return;
      }
      memcpy(thumbnailInfo_, videoFormat_, videoFormatSize_);

      thumbnailData_ = (char*) malloc(videoFormat_->bmiHeader.biSizeImage);
      if (thumbnailData_ == NULL)
      {
         thumbnailPossible_ = false;
         free(thumbnailInfo_);
         thumbnailInfo_ = NULL;
      }
      return;
   }

   // the compression of the pixel data from the video device
   // is not pure RGB data. We have to do something about that.
   decompressForThumbnail_ = true;

   DWORD width  = videoFormat_->bmiHeader.biHeight;
   DWORD height = videoFormat_->bmiHeader.biWidth;

   // let's have a look at the decompressors:
   ICINFO icInfo;

   FOURCC fccHandler = 0;
   bool handlerFound = false;
   int i = 0;
   
   while (ICInfo(ICTYPE_VIDEO, i++, &icInfo) && !handlerFound)
   {
      // Open the IC for query
      HIC hic = ICOpen(ICTYPE_VIDEO, icInfo.fccHandler, ICMODE_QUERY);
      if (hic != NULL)
      {
         // Can you handle this format at all?
         // if (ICDecompressQuery(hic, videoFormat_, NULL) != ICERR_BADFORMAT)
         if (ICDecompressQuery(hic, videoFormat_, NULL) == ICERR_OK)
         {
            // So, you think you can. What do you do with it?
            
            // How much space does the format data need?
            DWORD formatSize = ICDecompressGetFormatSize(hic, videoFormat_);
            if (formatSize <= 1024*1024)
            {
               // Check further only if size is reasonable
               thumbnailInfo_ = (BITMAPINFO*) malloc(formatSize);
               
               if (thumbnailInfo_ != NULL)
               {
                  // Ok, we have memory, now give me the format data
                  if (ICDecompressGetFormat(hic, videoFormat_, thumbnailInfo_) == ICERR_OK)
                  {
                     // Is it an RGB format?
                     if (thumbnailInfo_->bmiHeader.biCompression == BI_RGB)
                     {
                        // Yes? Great! That's what we need! You're hired!
                        fccHandler = icInfo.fccHandler;
                        handlerFound = true;
                     }
                     else
                     {
                        // If you don't do RGB of, just piss off...
                        free(thumbnailInfo_);
                        thumbnailInfo_ = NULL;
                     }
                  }
                  else
                  {
                     // Something went terribly wrong here...
                     free(thumbnailInfo_);
                     thumbnailInfo_ = NULL;
                  }
               }
            }
         }
         
         // Bye, bye!
         ICClose(hic);
      }
   }
   
   if (handlerFound)
   {
      thumbnailHic_ = ICOpen(ICTYPE_VIDEO, fccHandler, ICMODE_DECOMPRESS);
      if (thumbnailHic_ == NULL)
      {
         thumbnailPossible_ = false;
#ifdef _DEBUG
         printf("thumb: ICOpen failed.\n");
#endif
         return;
      }

      // allocate memory for buffer
      thumbnailData_ = (char*) malloc(thumbnailInfo_->bmiHeader.biSizeImage);
      if (thumbnailData_ == NULL)
      {
#ifdef _DEBUG
         printf("Note: Not enough memory for thumbnail decompress buffer.\n");
#endif
         thumbnailPossible_ = false;
         return;
      }

      if (ICDecompressBegin(thumbnailHic_, videoFormat_, thumbnailInfo_) != ICERR_OK)
      {
         printf("thumb: ICDecompressBegin() failed.\n");
         ICClose(thumbnailHic_);
         thumbnailPossible_ = false;
         return;
      }

      thumbnailPossible_ = true;
   }

#ifdef _DEBUG
   printf("--- end of initThumbnailMaker()\n");
#endif
}

void VideoCapturer::setEnableThumbnails(bool b)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "setEnableThumbnails\n");
  fflush(debugFile);
#endif

   enableThumbnails_ = b;
}

bool VideoCapturer::isThumbnailEnabled()
{
#ifdef _DEBUGFILE
//  fprintf(debugFile, "isThumbnailEnabled\n");
//  fflush(debugFile);
#endif

   return enableThumbnails_ && thumbnailPossible_ && captureInProgress_;
}

void VideoCapturer::getThumbnailData(ThumbnailData* thumb)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getThumbnailData\n");
  fflush(debugFile);
#endif

   if (isThumbnailEnabled())
   {
      printf("thumbnails are enabled!\n");
      printf("image size: %d\n", thumbnailInfo_->bmiHeader.biSizeImage);
      printf("video format image size: %d\n", videoFormat_->bmiHeader.biSizeImage);
      thumb->info = thumbnailInfo_;
      thumb->data = thumbnailData_;
   }
   else
   {
      printf("thumbnails are disabled.\n");
      thumb->info = NULL;
      thumb->data = NULL;
   }
}

void VideoCapturer::setNullFrameMode(bool b) 
{ 
   bNullFramesMode_ = b; 
}

void VideoCapturer::setCodec(CodecInfo* codecInfo)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "setCodec\n");
  fflush(debugFile);
#endif

	memcpy(&codecInfo_, codecInfo, sizeof CodecInfo);
}

void VideoCapturer::configureCodec(HWND hWnd)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "configureCodec\n");
  fflush(debugFile);
#endif

	if (codecInfo_.fccHandler == BI_RGB)
		return;

	HIC hic = ICOpen(ICTYPE_VIDEO, codecInfo_.fccHandler, ICMODE_QUERY);
	if (hic == 0)
		return;

	ICConfigure(hic, hWnd);

	ICClose(hic);
}

void VideoCapturer::aboutCodec(HWND hWnd)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "aboutCodec\n");
  fflush(debugFile);
#endif

	if (codecInfo_.fccHandler == BI_RGB)
		return;

	HIC hic = ICOpen(ICTYPE_VIDEO, codecInfo_.fccHandler, ICMODE_QUERY);
	if (hic == 0)
		return;

	ICAbout(hic, hWnd);

	ICClose(hic);
}

int VideoCapturer::checkSettings()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "checkSettings\n");
  fflush(debugFile);
#endif

	if (codecInfo_.fccHandler == BI_RGB)
		return VIDEOCAP_OK;

	BITMAPINFO* outputFormat;

	HIC hic = ICOpen(ICTYPE_VIDEO, codecInfo_.fccHandler, ICMODE_QUERY);
	if (hic == 0)
		return VIDEOCAP_CODEC_NOT_FOUND;

	// we want: input format is output format. check selected codec:
	DWORD res = ICCompressQuery(hic, videoFormat_, NULL);
	if (res != ICERR_OK)
	{
		ICClose(hic);
		return VIDEOCAP_WRONG_INPUT;
	}

	// get a matching output format:
	DWORD formatSize = ICCompressGetFormatSize(hic, videoFormat_);
	outputFormat = (BITMAPINFO*) malloc(formatSize);
	if (outputFormat == NULL)
	{
		ICClose(hic);
		return VIDEOCAP_NO_MEMORY;
	}
	res = ICCompressGetFormat(hic, videoFormat_, outputFormat);

	res = ICCompressQuery(hic, videoFormat_, outputFormat);
	if (res != ICERR_OK)
	{
		ICClose(hic);
		free(outputFormat);
		return VIDEOCAP_WRONG_OUTPUT;
	}

   // everything's fine, let's try to get a coarse size
   // estimate on the frame size:
   maxFrameSize_ = ICCompressGetSize(hic, videoFormat_, outputFormat);

	free(outputFormat);
	ICClose(hic);

	return VIDEOCAP_OK;
}


void VideoCapturer::getCodecList(CodecInfo** codecList)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getCodecList\n");
  fflush(debugFile);
#endif

	ICINFO icInfo;
	int i = 0; 
	int current = 1; // CodecInfo[0] is always RGB / not recompressed
	FOURCC vidc = mmioStringToFOURCC(_T("vidc"), 0);

	// start with the RGB uncompressed format.
	codecList[0]->bHasConfigDialog   = false;
	codecList[0]->bHasInfoDialog     = false;
	codecList[0]->bSupportsDatarate  = false;
	codecList[0]->bSupportsKeyframes = false;
	codecList[0]->bSupportsQuality   = false;
	codecList[0]->bUseDatarate       = false;
	codecList[0]->bUseKeyframes      = false;
	codecList[0]->dwFlags            = 0;
   if (videoFormat_->bmiHeader.biCompression == BI_RGB)
   {
	   codecList[0]->fccHandler         = BI_RGB;
   	_tcscpy(codecList[0]->fccString, _T("DIB "));

   	const _TCHAR* tmp = Local::localizeString(VIDEO_RGB_CODECNAME);
   	// cout << "Localized: " << tmp << endl;

	   _tcscpy(codecList[0]->tszName, tmp);
   	_tcscpy(codecList[0]->tszDescription, Local::localizeString(VIDEO_RGB_CODECDESC));
   }
   else
   {
      // the video format is already compressed
      codecList[0]->fccHandler = videoFormat_->bmiHeader.biCompression;

		_TCHAR* t = codecList[0]->fccString;
		t[0] = (_TCHAR) (codecList[0]->fccHandler & 0x000000ff);
		t[1] = (_TCHAR) (codecList[0]->fccHandler >> 8 & 0x000000ff);
		t[2] = (_TCHAR) (codecList[0]->fccHandler >> 16 & 0x000000ff);
		t[3] = (_TCHAR) (codecList[0]->fccHandler >> 24 & 0x000000ff);
		t[4] = (_TCHAR) 0;

      _tcscpy(codecList[0]->tszName, Local::localizeString(VIDEO_NO_RECOMPRESS));
      _tcscpy(codecList[0]->tszDescription, Local::localizeString(VIDEO_NO_RECOMPRESSDESC));
   }

	codecList[0]->iDatarate            = 0;
	codecList[0]->iDefaultKeyframeRate = 0;
	codecList[0]->iKeyframeRate        = 1;
	codecList[0]->iQuality             = 10000;

	while (ICInfo(vidc, i++, &icInfo))
	{
		ICINFO moreInfo;
		memset(&moreInfo, 0, sizeof ICINFO);

		moreInfo.dwSize = sizeof ICINFO;

		HIC hic = ICOpen(ICTYPE_VIDEO, icInfo.fccHandler, ICMODE_QUERY);

		if (hic == 0)
		{
			// that codec couldn't be opened...
			continue;
		}

		// check input format:
		bool formatOk = (ICCompressQuery(hic, videoFormat_, NULL) == ICERR_OK); // any output format
		if (!formatOk)
		{
			// the codec could not handle input format, take next.
			ICClose(hic);
			continue;
		}

		LRESULT res = ICGetInfo(hic, &moreInfo, sizeof ICINFO);

		if (res == 0)
		{
			_TCHAR* tmp = _T("<error>\000");
			_tcscpy(codecList[current]->tszName, tmp);
			_tcscpy(codecList[current]->tszDescription, tmp);
			codecList[current]->fccHandler = icInfo.fccHandler;
		}
		else
		{
         // Note: The doc says: returns ICERR_OK if everything is
         // ok. This is not true: Either FALSE or TRUE is returned.
         DWORD queryConfigure = ICQueryConfigure(hic);
         DWORD queryAbout     = ICQueryAbout(hic);

			codecList[current]->dwFlags          = moreInfo.dwFlags;
			codecList[current]->fccHandler       = icInfo.fccHandler;
			codecList[current]->bHasConfigDialog = (queryConfigure != FALSE);
			codecList[current]->bHasInfoDialog   = (queryAbout != FALSE);

			DWORD defFrameRate = ICGetDefaultKeyFrameRate(hic);

			codecList[current]->iDefaultKeyframeRate = defFrameRate;
			codecList[current]->bUseKeyframes        = true;

			codecList[current]->bSupportsDatarate  = ((moreInfo.dwFlags & VIDCF_CRUNCH) != 0);
			codecList[current]->bSupportsQuality   = ((moreInfo.dwFlags & VIDCF_QUALITY) != 0);
			codecList[current]->bSupportsKeyframes = ((moreInfo.dwFlags & VIDCF_TEMPORAL) != 0);
	
			codecList[current]->iDatarate          = 500; // KB/s

#ifdef _UNICODE
         _tcscpy(codecList[current]->tszName, moreInfo.szName);
         _tcscpy(codecList[current]->tszDescription, moreInfo.szDescription);
#else
         ::WideCharToMultiByte(CP_ACP, 0, moreInfo.szName, -1, codecList[current]->tszName, 128, NULL, NULL);
         ::WideCharToMultiByte(CP_ACP, 0, moreInfo.szDescription, -1, codecList[current]->tszDescription, 128, NULL, NULL);
#endif
         /*
			char tempString[128];
			LRESULT lres = WideCharToMultiByte
				(CP_ACP, 0, moreInfo.szName, -1, tempString, 128, NULL, NULL);
			strcpy(codecList[current]->tszName, tempString);
			lres = WideCharToMultiByte
				(CP_ACP, 0, moreInfo.szDescription, -1, tempString, 128, NULL, NULL);
			strcpy(codecList[current]->tszDescription, tempString);
         */
		}

		res = ICClose(hic);

		_TCHAR* t = codecList[current]->fccString;
		t[0] = (_TCHAR) (icInfo.fccHandler & 0x000000ff);
		t[1] = (_TCHAR) (icInfo.fccHandler >> 8 & 0x000000ff);
		t[2] = (_TCHAR) (icInfo.fccHandler >> 16 & 0x000000ff);
		t[3] = (_TCHAR) (icInfo.fccHandler >> 24 & 0x000000ff);
		t[4] = (_TCHAR) 0;

		// next codec:
		current++;
	}

	// return true;
}

int VideoCapturer::getCodecListSize()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getCodecListSize\n");
  fflush(debugFile);
#endif

// #define DEBUG
#ifdef _DEBUG
	BITMAPINFOHEADER* bmih = &(videoFormat_->bmiHeader);

	printf("Video: \n");
	printf("  biSize == %d\n", bmih->biSize);
	printf("  biWidth == %d\n", bmih->biWidth);
	printf("  biHeight == %d\n", bmih->biHeight);
	printf("  biPlanes == %d\n", bmih->biPlanes);
	printf("  biBitCount == %d\n", bmih->biBitCount);
	printf("  biCompression == %s (%08x)\n", getCompression(bmih->biCompression).c_str(), bmih->biCompression);
	printf("  biSizeImage == %d\n", bmih->biSizeImage);
	printf("  biXPelsPerMeter == %d\n", bmih->biXPelsPerMeter);
	printf("  biYPelsPerMeter == %d\n", bmih->biYPelsPerMeter);
	printf("  biClrUsed == %d\n", bmih->biClrUsed);
	printf("  biClrImportant == %d\n", bmih->biClrImportant);
#endif

	ICINFO icInfo;
	int i = 0;
	int icCount = 1; // RGB uncompressed always matches!
	FOURCC vidc = mmioStringToFOURCC(_T("vidc"), 0);

	while (ICInfo(vidc, i++, &icInfo))
	{
#ifdef DEBUG
		char tmp[5];
		tmp[0] = (char) (icInfo.fccHandler & 0x000000ff);
		tmp[1] = (char) (icInfo.fccHandler >> 8 & 0x000000ff);
		tmp[2] = (char) (icInfo.fccHandler >> 16 & 0x000000ff);
		tmp[3] = (char) (icInfo.fccHandler >> 24 & 0x000000ff);
		tmp[4] = (char) 0;

		//cout << "FOURCC of #" << i << ": " << tmp;
#endif

		HIC hic = ICOpen(ICTYPE_VIDEO, icInfo.fccHandler, ICMODE_QUERY);
		if (hic != 0)
		{
			DWORD res = ICCompressQuery(hic, videoFormat_, NULL);
			if (res == ICERR_OK)
			{
				icCount++;
#ifdef DEBUG
				//cout << " (ok)" << endl;
#endif
			}
#ifdef DEBUG
			//else
         //{
				//cout << " (format failed)" << endl;
         //}
#endif

			ICClose(hic);
		}
#ifdef DEBUG
		//else
			//cout << " (failed)" << endl;
#endif
	}

#ifdef DEBUG
	printf("Found %d codecs.\n", icCount);
#endif

	return icCount;
#undef DEBUG
}

/*
In Audio Sync Mode, the audio/video time factor is no longer updated,
i.e., it is fixed for the rest of the capture session. If the audio sync
mode is entered too soon, the resulting video will probably contain a 
large amount of dropped or forward dropped frames.
*/
void VideoCapturer::enterAudioSyncMode()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "enterAudioSyncMode\n");
  fflush(debugFile);
#endif

#ifdef _DEBUG
	printf("+-+-+-+-+  A U D I O  S Y N C  M O D E  +-+-+-+-+\n");
#endif
	audioSyncMode_ = true;
}

/* ==================== *
 *
 *    STATIC METHODS
 *
 * ==================== */

int VideoCapturer::getDeviceListSize()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "VideoCapturer::getDeviceListSize()\n");
  fflush(debugFile);
#endif

#ifdef DEBUG
	printf("VideoCapturer::getDeviceListSize()\n");
#endif

   int iCount = 0;
   _TCHAR tszDriverName[256];
	_TCHAR tszDriverDesc[256];
 
	for (int i=0; i<10; ++i)
	{
		BOOL res;
		res = capGetDriverDescription(i, tszDriverName, 256, tszDriverDesc, 256);

      if (res == TRUE)
         ++iCount;
      else
         break;
   }

   return iCount;
}

void VideoCapturer::getDeviceList(_TCHAR **atszDevices, int iMaxLength)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getDeviceList hupps\n");
  fflush(debugFile);
#endif

#ifdef DEBUG
	printf("VideoCapturer::getDeviceList()\n");
#endif
	
	_TCHAR tszDriverName[256];
	_TCHAR tszDriverDesc[256];
   _TCHAR tszDriverIdent[600];

	for (int i=0; i<10; ++i)
	{
		BOOL res;
		res = capGetDriverDescription(i, tszDriverName, 256, tszDriverDesc, 256);

#ifdef _DEBUGFILE
      fprintf(debugFile, " * Trying Device #%d.\n", i);
      fflush(debugFile);
#endif
		if (res == TRUE)
		{
         tszDriverIdent[0] = 0;
         _tcscat(tszDriverIdent, tszDriverName);
         _tcscat(tszDriverIdent, _T(" ["));
         _tcscat(tszDriverIdent, tszDriverDesc);
         _tcscat(tszDriverIdent, _T("]"));

         int iIdentLength = _tcslen(tszDriverIdent);
         int iCopyLength = iIdentLength > 599 ? 599 : iIdentLength;
         _tcsncpy(atszDevices[i], tszDriverIdent, iCopyLength);
         atszDevices[i][iCopyLength] = 0;

			//nameList.push_back(string(szDriverName) + " [" + string(szDriverDesc) + "]");
#ifdef DEBUG
         
         _tprintf(_T("Driver #%d: %s\n"), i, atszDevices[i])

			//printf("Driver #%d: %s\n", i, nameList.at(i));
#endif
#ifdef _DEBUGFILE
         
         _ftprintf(debugFile, _T("  - Device %d: %s\n"), i, atszDevices[i]);

        //fprintf(debugFile, "  - Device %d: %s\n", i, nameList.at(i).c_str());
        fflush(debugFile);
#endif
		}
		else
			break;
	}
}

DWORD VideoCapturer::getFileSize()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getFileSize\n");
  fflush(debugFile);
#endif

   return totalSize_;
}

long VideoCapturer::getEstimatedDataRate()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getEstimatedDataRate\n");
  fflush(debugFile);
#endif

   if (codecInfo_.fccHandler == BI_RGB)
   {
      return (long) (frameRate_ * videoFormat_->bmiHeader.biSizeImage);
   }

   if (checkSettings() == VIDEOCAP_OK)
   {
      return (long) (frameRate_ * (maxFrameSize_ + 24));
   }
   else
      return -1;
}

long VideoCapturer::getAverageDataRate()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "getAverageDataRate\n");
  fflush(debugFile);
#endif

   if (frameCounter_ <= 0)
      return 0;

   return (long) ((frameRate_ * totalSize_) / ((double) frameCounter_));
}


void VideoCapturer::writeThumbnail(void* data, int cbSize, BITMAPINFO* pHeader, char* fileName)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "writeThumbnail\n");
  fflush(debugFile);
#endif

   /*
   int THUMB_XSIZE = 32;
   int THUMB_YSIZE = 24;
   */

   /*
   BITMAPINFOHEADER tmpHdr;
   tmpHdr.
   */

   BITMAPFILEHEADER bmpHeader;
   bmpHeader.bfType      = 'BM';
   bmpHeader.bfReserved1 = 0;
   bmpHeader.bfReserved2 = 0;
   bmpHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
   bmpHeader.bfSize      = bmpHeader.bfOffBits + pHeader->bmiHeader.biSizeImage;

   FILE* file = fopen("C:\\imagecap.bmp\000", "wb");
   fwrite(&bmpHeader, sizeof(char), sizeof BITMAPFILEHEADER, file);
   fwrite(&pHeader->bmiHeader, sizeof(char), sizeof BITMAPINFOHEADER, file);
   fwrite(data, sizeof(char), pHeader->bmiHeader.biSizeImage, file);
   fclose(file);
}

long VideoCapturer::getRefTime()
{
   // Double code: getVideoTime()
   static CAPSTATUS capStatus;
   // static long      lastVideoTimeMs = 0;

   BOOL success = capGetStatus(captureHWnd_, &capStatus, sizeof CAPSTATUS);
   if (success)
   {
      // This should not be necessary, still the devil is
      // a squirrel. If the current value should happen
      // to be smaller than the last one, keep the last.
      if (lastReturnedRefTime_ <= capStatus.dwCurrentTimeElapsedMS)
         lastReturnedRefTime_ = capStatus.dwCurrentTimeElapsedMS;
#ifdef _DEBUGFILE
      fprintf(debugFile, "getRefTime() == %d\n", lastReturnedRefTime_);
      fflush(debugFile);
#endif
      return lastReturnedRefTime_;
   }
   else
   {
#ifdef _DEBUGFILE
      fprintf(debugFile, "*** getRefTime(): capGetStatus failed\n", capStatus.dwCurrentTimeElapsedMS);
      fflush(debugFile);
#endif
   }

   if (hasPerformanceCounter_)
   {
      BOOL success = ::QueryPerformanceCounter(&performanceCounter_);
	  if (success != FALSE)
	      return (long) ((1000i64 * performanceCounter_.QuadPart) / performanceFrequency_.QuadPart);
   }

   return timeGetTime();
}

long VideoCapturer::getVideoTime()
{
   // Double code: getRefTime()
   static CAPSTATUS capStatus;

   BOOL success = capGetStatus(captureHWnd_, &capStatus, sizeof CAPSTATUS);
   if (success)
   {
      // This should not be necessary, still the devil is
      // a squirrel. If the current value should happen
      // to be smaller than the last one, keep the last.
      if (lastReturnedRefTime_ <= capStatus.dwCurrentTimeElapsedMS)
         lastReturnedRefTime_ = capStatus.dwCurrentTimeElapsedMS;
      return lastReturnedRefTime_;
   }

   // capGetStatus failed!
#ifdef _DEBUGFILE
   fprintf(debugFile, "*** getVideoTime(): capGetStatus failed\n", capStatus.dwCurrentTimeElapsedMS);
   fflush(debugFile);
#endif
   return lastReturnedRefTime_;
}

void VideoCapturer::SetUseAdvancedAudioSync(bool bAdvancedSync)
{
   useAdvancedAudioSyncMode_ = bAdvancedSync;
}

LRESULT CALLBACK VideoCapturer::videoStreamCallback(HWND hwnd, LPVIDEOHDR lpVHdr)
{
   // retrieve the fitting VideoCapturer instance:
   VideoCapturer *instance = retrieveInstance(hwnd);
   if (!instance)
   {
      MessageBox(NULL, _T("Lost Video Capturer instance!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
      return TRUE;
   }

   if (!instance->useAdvancedAudioSyncMode_)
      return instance->VideoCallback(hwnd, lpVHdr);

#ifdef _DEBUGFILE
   fprintf(debugFile, "videoStreamCallback %dms (Time: %d, File time: %d)\n", lpVHdr->dwTimeCaptured, (long) (instance->frameCounter_ * instance->usecsPerFrame_ / 1000i64), 
                     (long) (instance->aviOutputFile_->getCurrentFrame() * instance->usecsPerFrame_ / 1000i64));
   fflush(debugFile);
#endif

   // Query for the current reference time: This may start at
   // an arbitrary time, but must increase steadily. It must
   // correspond to the video time, i.e. the video time
   // runs exactly as fast as the reference time. The
   // returned value is in milliseconds.
	long realTime = instance->getRefTime(); //  = timeGetTime();

   // If audio is available, retrieve the current audio sampling
   // position, in milliseconds.
	long waveTime = 0;
	if (instance->audioSync_)
		waveTime = instance->audio_->getWaveInTimeMs();

   // Is this the first we enter this method?
	if (instance->firstTime_ == -1)
	{
      // Yes, store the time for delta calculations
		instance->firstTime_ = realTime;

#ifdef _DEBUG
		printf("firstTime_ == %d\n", instance->firstTime_);
		dumpVideoHeader(lpVHdr);
#endif
#ifdef _DEBUGFILE
      fprintf(debugFile, "firstTime_ == %d\n", instance->firstTime_);
#endif
	}

	if (instance->isPaused_)
   {
      // In case we're paused, just throw away
      // this frame.
#ifdef _DEBUGFILE
      fprintf(debugFile, "Paused.\n");
      fflush(debugFile);
#endif
		return TRUE;
   }

   // Calculate the current stream time for use throughout
   // this method call. Note that this is not in all cases
   // the real current stream time, but may deviate from
   // the stream time by a certain offset. This offset equals
   // getVideoTime() - currentStreamTime.
   long currentStreamTime = realTime - instance->firstTime_;

   // Should we check for the audio/video offset now?
   // Check only once (bAudioControlDone_) and only if
   // audio is available and has started capturing.
   if (!instance->bAudioControlDone_ && instance->audioSync_ && waveTime > 0)
   {
      long realVideoTime = instance->getVideoTime();
      // Here we calculate the real audio/video offset for use during
      // playback (e.g. in the Lecturnity Player). 
      // We cannot use this value for the other sync calculations
      // as it does not take the video latency into account.
      // This value is usually saved into an LRD file.
      instance->audioVideoOffset_ = waveTime - realVideoTime; // was: (realTime - instance->firstTime_);

      // Instead we have to use this variable. This is the offset
      // between the real time, relative to the first call of
      // videoStreamCallback, and the wave time.
      instance->lInternalAvOffset_ = waveTime - currentStreamTime;
//      char t[256];
//      sprintf(t, "waveTime == %d, videoTime == %d, internalAvOffset == %d", waveTime, realVideoTime, instance->lInternalAvOffset_);
//      MessageBox(NULL, t, NULL, MB_OK);
#ifdef _DEBUGFILE
      fprintf(debugFile, "-----> Audio/video offset: %d (realTime: %d, firstTime: %d, waveTime: %d, realVideoTime: %d, internalOffset: %d)\n", instance->audioVideoOffset_, realTime, instance->firstTime_, waveTime, realVideoTime, instance->lInternalAvOffset_);
      fprintf(debugFile, "   skew preroll to this frame: %d\n", realTime - instance->ulPrerollTime_);
      fflush(debugFile);
#endif

      instance->bAudioControlDone_ = true;
   }

   // Should we try to create preview thumbnails at all?
   if (instance->isThumbnailEnabled())
   {
      // Yes. Is it time for a new preview thumbnail?
      if (instance->frameCounter_ - instance->lastThumbnailFrame_ > instance->checkSyncInterval_)
      {
         if (instance->decompressForThumbnail_)
         {
            // we need the decompressor to make a 24 RGB DIB of this
            if (lpVHdr->dwFlags & VHDR_KEYFRAME)
            {
               // we can only decompress keyframes, as we only do it once in a while
               ICDecompress(instance->thumbnailHic_,
                  0, // no flags
                  &instance->videoFormat_->bmiHeader,
                  lpVHdr->lpData,
                  &instance->thumbnailInfo_->bmiHeader,
                  instance->thumbnailData_);

               instance->lastThumbnailFrame_ = instance->frameCounter_;
            }
         }
         else
         {
            // the video data is RGB data, so just copy it
            memcpy(instance->thumbnailData_, lpVHdr->lpData, lpVHdr->dwBytesUsed);

            instance->lastThumbnailFrame_ = instance->frameCounter_;
         }
      }
   }

   // Is this the first time this method is called
   // after a pause? Note: The variable wasPaused_
   // is also true at start, i.e. the next block
   // is executed the first time this method is
   // called at all.
	if (instance->wasPaused_)
	{
      // At a start (or restart) of the video capture,
      // we need a keyframe from the video device. This
      // is always the case with RGB and YUV formats, but
      // some MJPG and/or MPG formats also have other
      // types of frames which are not keyframes. In that
      // case, act like we were still paused.
		if ((lpVHdr->dwFlags & VHDR_KEYFRAME) == 0)
		{
#ifdef _DEBUG
			printf("Cannot start with non-keyframe...\n");
#endif
			return TRUE;
		}
		instance->lastStartTime_    = 1000i64 * lpVHdr->dwTimeCaptured;
      // First time ever?
      if (instance->mostRecentTimeCaptured_ == -1)
         instance->mostRecentTimeCaptured_ = 1000i64 * lpVHdr->dwTimeCaptured;
		instance->lastOurStartTime_ = instance->usecsPerFrame_ * instance->frameCounter_;

		// update the video pause time offset, is last dwTimeCaptured deducted from
		// the current dwTimeCaptured. From that, we have to subtract one frame. 
      // Rationale: The time stamp in mostRecentTimeCaptured is the time stamp at the
      // beginning of the validity span of that frame, but it is valid for another
      // frame time (e.g. 40ms at 25 fps).
		instance->pausedTime_ += 
         (1000i64 * lpVHdr->dwTimeCaptured - instance->mostRecentTimeCaptured_) - instance->usecsPerFrame_;

		instance->wasPaused_        = false;
   }

	// First, we'll check for any hardware-dropped frames:
	// We check that by comparing the frame counter times
	// the frame time (us/frame) with the "captured time"
	// given in the VIDEOHDR structure. If these two values
	// deviate too much (can only be in one direction), we 
	// have to drop frames, as they were dropped by the
	// hardware.
	TIME_US hardwareDelta = (((TIME_US) (1000i64 * lpVHdr->dwTimeCaptured)) - instance->lastStartTime_)
							- (instance->usecsPerFrame_ * instance->frameCounter_ - instance->lastOurStartTime_);

	int hardwareDropFrames = 0;
   if (abs((int)hardwareDelta) > instance->mediaThreshold_)
   {
#ifdef _DEBUGFILE
      fprintf(debugFile, "hardware delta: %d\n", hardwareDelta);
      fflush(debugFile);
#endif
      if (hardwareDelta > 0) // probably hardware drop
	      hardwareDropFrames = (int) ((hardwareDelta + instance->usecsPerFrame_ / 2i64) / instance->usecsPerFrame_);
      else // this must be some crappy capture driver
      {
   	   hardwareDropFrames = (int) ((hardwareDelta - instance->usecsPerFrame_ / 2i64) / instance->usecsPerFrame_);
      }
   }

	// Now: All in all, we have to make sure that the frame counter
	// is increased by hardwareDropFrames + 1. That assures that we
	// are in sync with the hardware on this level.

	// We will fill this variable with a suitable value eventually.
	// It is not used if the audio sync is not applied. Then it is
	// always zero. This value may also be below zero. If it is,
	// frames are "forward dropped".
	int audioSyncDropFrames = 0;

#ifdef _DEBUG
   if (hardwareDropFrames != 0)
		printf("hardware drop: %d (delta: %d)\n", hardwareDropFrames, (long) hardwareDelta);
#endif

	if (!instance->audioSync_)
	{
		// No audio to sync to; then the hardwareDropFrames counter
		// is all we need; no need to do any further actions here now.
	}
	else
	{
		// We want to sync the video to a (master) audio stream.
		// First, we'll check if we're already in the tight audio
		// sync mode, which means that we have already calculated
		// an audio/video time factor for the streams. We need this
		// factor due to the fact that most sound cards' oscillators
		// are inherently inexact: The stream lengths of video and 
		// audio normally differ, sometimes as much as a percent (1%)
		// in length, which results in seconds of unsynchronity with
		// a large recording (and is thus noticeable).
      //
		// Note: If audio has not yet started, it's pointless
		// to try to do a synchronization with it (bAudioControlDone_).
      if (instance->audioSyncMode_ && instance->bAudioControlDone_)
		{
			// We are in the audio tight sync mode; the time sync is
			// done over the pausedTime_ variable. The hardware sync
			// does primarily not have anything to do with the audio
			// time sync. It is just a question of manipulating the
			// pausedTime_ counter.

         // Calculate the current video time. See above for an
         // explanation of the lInternalAvOffset_ variable.
         TIME_US videoTime = 1000i64 * (currentStreamTime + instance->lInternalAvOffset_);
         // Estimate the audio time from the current video time
			TIME_US alpha = (TIME_US) 
				(instance->audioVideoFactor_ * (videoTime - instance->pausedTime_));

         // This is the real audio time (in us)
			TIME_US beta = 1000i64 * waveTime;
#ifdef _DEBUGFILE
         fprintf(debugFile, "* sync: alpha == %d, beta == %d\n", (long) (alpha / 1000i64), (long) (beta / 1000i64));
         fflush(debugFile);
#endif

         // This is the estimated time for one frame (in us)
         TIME_US gamma = (TIME_US) (instance->audioVideoFactor_ * instance->usecsPerFrame_);

         // Calculate the difference between the estimated audio
         // time (alpha) and the real audio time (beta).
			TIME_US delta = alpha - beta;

			if (abs((int)delta) > instance->mediaThreshold_)
			{
				// Now, we have a large deviation estimated audio time
				// (real time times audio-video factor) vs. real audio
				// time here. So we'll check what kind of resync we'll
				// have to do.
#ifdef _DEBUG
				printf("est. audio - real audio == %d\n", (long) delta);
#endif

				if (delta > 0)
				{
					// Video is faster than audio; video has to wait
					// for audio: A "forward drop".

					audioSyncDropFrames = (int) (( -delta - gamma / 2 ) / gamma);
#ifdef _DEBUG
					printf("audio says: forward drop: %d frames.\n", audioSyncDropFrames);
#endif
				}
				else
				{
					// Audio is faster than video; we need to insert
					// a null frame or two into the video file to
					// catch up.

					audioSyncDropFrames = (int) (( -delta + gamma / 2 ) / gamma);
#ifdef _DEBUG
					printf("audio says: backward drop: %d frames.\n", audioSyncDropFrames);
#endif
				}
			}

		}
		else
		{
			// We are not yet in the audio sync mode, i.e. everything
			// has gone quite fine up to now. There is no need to try
			// to sync the video with the audio yet; we just let them
			// run parallelly and measure the time factor between the
			// audio and the video stream once in a while. During that
			// check, we also check if any audio data has been lost;
			// if that is the case, we must enter the tight audio sync
			// mode, using the calculated audio/video factor.

         // Only do all that if audio has started; otherwise do
         // not yet change the audio/video factor:
         if (waveTime > 0)
         {
            // In order to calculate the audio/video factor more
            // often, this if-clause has been removed. Problem was:
            // In case of frequent hardware drops, the condition
            // was very seldom true.
			   // if (instance->frameCounter_ % instance->checkSyncInterval_ == instance->checkSyncInterval_ - 1)
			   // {
				if (instance->audio_->hasDataBeenLost())
				{
#ifdef _DEBUG
					printf("*** AUDIO DATA LOSS!\n");
#endif
#ifdef _DEBUGFILE
               fprintf(debugFile, "*** AUDIO DATA LOSS!\n");
               fflush(debugFile);
#endif
					// data has been lost during audio capturing; enter audio sync mode.
					instance->enterAudioSyncMode();
				}
				else
				{
					// calculate the new audio/video factor
					double dWaveTime = waveTime;
					double videoTime = currentStreamTime + instance->lInternalAvOffset_;
					instance->audioVideoFactor_ = (instance->audioVideoFactor_ + dWaveTime / videoTime) / 2.0;
#ifdef _DEBUG
					printf("Audio/Video factor: %f\n", instance->audioVideoFactor_);
#endif
#ifdef _DEBUGFILE
               fprintf(debugFile, "Audio/Video factor: %f, wav time - time: %d\n", instance->audioVideoFactor_, waveTime - (long) (currentStreamTime + instance->lInternalAvOffset_));
               fflush(debugFile);
#endif
				}
            // } // time to check sync?
         } // waveTime > 0
		}
	}

	// Add the two drop counters and we get the number of frames
	// to drop:

	int dropAction = hardwareDropFrames + audioSyncDropFrames;

	// In any case: update the frameCounter to match the hardware:
   instance->frameCounter_ += (hardwareDropFrames < -1 ? -1 : hardwareDropFrames) + 1;

	// Note: It is not very probable that we have a
	if (dropAction < 0)
	{
		// the sum is negative; we really need a forward drop.
		// Update the pausedTime_: Add one frame length.
	
		if (audioSyncDropFrames < 0)
			instance->pausedTime_ += instance->usecsPerFrame_;

      // As this is also a drop, add one to the drop counter:
      instance->dropCounter_++;

#ifdef _DEBUG
		printf("** forward drop: pausedTime_ == %d\n", (long) instance->pausedTime_);
#endif
#ifdef _DEBUGFILE
      fprintf(debugFile, "++ forward drop, 1 frame\n");
      fflush(debugFile);
#endif
		return TRUE;
	}
   else
   {
      // Drop action is not negative, but perhaps we still
      // have to adjust the pausedTime_? (Note: audioSyncDropFrames
      // is negative, thus minus!)
      if (audioSyncDropFrames < 0)
      {
         instance->pausedTime_ -= audioSyncDropFrames * instance->usecsPerFrame_;
#ifdef _DEBUG
         printf("++ updating paused time, but no forward drop: %d frames, pausedTime == %d\n", audioSyncDropFrames, (TIME_US) instance->pausedTime_);
#endif
#ifdef _DEBUGFILE
         fprintf(debugFile, "++ updating paused time, but no forward drop: %d frames, pausedTime == %d\n", audioSyncDropFrames, (TIME_US) instance->pausedTime_);
         fflush(debugFile);
#endif
      }
   }

	// Now we know that we do not have to forward drop anything
	// Update the pausedTime_, if necessary, by subtracting the
	// needed amount of frame lengths (in us).
	if (audioSyncDropFrames > 0)
	{
		instance->pausedTime_ -= instance->usecsPerFrame_ * audioSyncDropFrames;
#ifdef _DEBUG
		printf("** syncing audio paused time: pausedTime_ == %d\n", (long) instance->pausedTime_);
#endif
#ifdef _DEBUGFILE
      fprintf(debugFile, "++ audio sync drop: %d frames, pausedTime == %d\n", audioSyncDropFrames, (TIME_US) instance->pausedTime_);
      fflush(debugFile);
#endif
	}

	if (dropAction > 0)
	{
		// We need to insert some null frames into the AVI
		// file for some reason
#ifdef _DEBUGFILE
   fprintf(debugFile, "dropping %d frames. hw drop: %d, audio drop: %d, real time: %d, wave time: %d, hw delta: %d\n", 
	   dropAction, hardwareDropFrames, audioSyncDropFrames, realTime, waveTime, hardwareDelta);
   fflush(debugFile);
#endif

		instance->dropCounter_ += dropAction;

		for (int j=0; j<dropAction; ++j)
      {
         // update the frame byte quota:
         if (instance->codecInfo_.bSupportsDatarate && instance->codecInfo_.bUseDatarate)
            instance->accFrameByteQuota_ += instance->frameByteQuota_ - 24;

			instance->aviOutputFile_->writeNullFrame();
      }
	}

	// The captured frame will be written in any case: So update
	// the most recent frame time variable:
	instance->mostRecentTimeCaptured_ = 1000i64 * lpVHdr->dwTimeCaptured;

   instance->accFrameByteQuota_ += instance->frameByteQuota_;
   // Check for too small quotas: If the accumulated frame quota is smaller than
   // half the quota for a single frame, set to half the frame quota.
   if (instance->accFrameByteQuota_ < instance->frameByteQuota_/2)
      instance->accFrameByteQuota_ = instance->frameByteQuota_/2;

	// write some data perhaps:
   DWORD size;
   if (!instance->bNullFramesMode_)
   {
      if (instance->codecInfo_.bUseDatarate)
      {
   	   size = instance->aviOutputFile_->writeFrame(lpVHdr->lpData, lpVHdr->dwBytesUsed, 
                                                     instance->accFrameByteQuota_, lpVHdr->dwFlags,
                                                     lpVHdr->dwUser, (QCDATA*)lpVHdr->dwReserved[0], 
                                                     (SrMouseData*)lpVHdr->dwReserved[1],
                                                     (SrClickData*)lpVHdr->dwReserved[2],
                                                     lpVHdr->dwTimeCaptured);
      }
      else
      {
   	   size = instance->aviOutputFile_->writeFrame(lpVHdr->lpData, 
                                                     lpVHdr->dwBytesUsed, 
                                                     (instance->frameCounter_ == 1) ? 0xffffff : 0, //instance->accFrameByteQuota_, 
                                                     lpVHdr->dwFlags,
                                                     lpVHdr->dwUser, (QCDATA*)lpVHdr->dwReserved[0], 
                                                     (SrMouseData*)lpVHdr->dwReserved[1],
                                                     (SrClickData*)lpVHdr->dwReserved[2],
                                                     lpVHdr->dwTimeCaptured);
      }
   }
   else
   {
      // we force NULL frames, even if it's not necessary.
      instance->aviOutputFile_->writeNullFrame();
      size = 24; // null frame has 8 bytes, index has 16 bytes for frame.
   }

   instance->totalSize_ += size;

   if (instance->codecInfo_.bSupportsDatarate && instance->codecInfo_.bUseDatarate)
      instance->accFrameByteQuota_ -= size;

   /*
#ifdef DEBUG
    long _realTime = instance->getRefTime() - firstTime_;
	printf("VideoCapturer::videoStreamCallback(): " << lpVHdr->dwTimeCaptured << 
		"(" << (getRefTime() - firstTime_) << " ms, delta " << (_realTime - (long) lpVHdr->dwTimeCaptured) << 
		")" << endl;
#endif
      */

	return TRUE;
}

LRESULT VideoCapturer::VideoCallback(HWND hwnd, LPVIDEOHDR lpVHdr)
{
#ifdef _DEBUG
   if (!audioSyncMode_)
      MessageBox(NULL, _T("Audio sync mode ist aus!!!"), _T("Fehler"), MB_OK);
#endif

#ifdef _DEBUGFILE
   long hwTime = (long) (frameCounter_ * usecsPerFrame_ / 1000i64);
   long fileTime = (long) (aviOutputFile_->getCurrentFrame() * usecsPerFrame_ / 1000i64);
   fprintf(debugFile, "videoStreamCallback %dms (Time: %d, File time: %d)\n", lpVHdr->dwTimeCaptured, hwTime, fileTime);
   long interpFileTime = hwTime - ((long) (pausedTime_ / 1000i64));
   if (abs(interpFileTime - fileTime) > (usecsPerFrame_ / 2000i64))
   {
      fprintf(debugFile, "%%%%% delta(hwTime - pausedTime, fileTime) == %d\n", (interpFileTime - fileTime));
   }
   fflush(debugFile);
#endif

   // Query for the current reference time: This may start at
   // an arbitrary time, but must increase steadily. It must
   // correspond to the video time, i.e. the video time
   // runs exactly as fast as the reference time. The
   // returned value is in milliseconds.
	long realTime = getRefTime(); //  = timeGetTime();

   // If audio is available, retrieve the current audio sampling
   // position, in milliseconds.
	long waveTime = 0;
	if (audioSync_)
		waveTime = audio_->getWaveInTimeMs();

   // Is this the first we enter this method?
	if (firstTime_ == -1)
	{
      // Yes, store the time for delta calculations
		firstTime_ = realTime;

#ifdef _DEBUG
		printf("firstTime_ == %d\n", firstTime_);
		dumpVideoHeader(lpVHdr);
#endif
#ifdef _DEBUGFILE
      fprintf(debugFile, "firstTime_ == %d\n", firstTime_);
#endif
	}

	if (isPaused_)
   {
      // In case we're paused, just throw away
      // this frame.
#ifdef _DEBUGFILE
      fprintf(debugFile, "Paused.\n");
      fflush(debugFile);
#endif
		return TRUE;
   }

   // Calculate the current stream time for use throughout
   // this method call. Note that this is not in all cases
   // the real current stream time, but may deviate from
   // the stream time by a certain offset. This offset equals
   // getVideoTime() - currentStreamTime.
   long currentStreamTime = realTime - firstTime_;

   // Should we check for the audio/video offset now?
   // Check only once (bAudioControlDone_) and only if
   // audio is available and has started capturing.
   if (!bAudioControlDone_ && audioSync_ && waveTime > 0)
   {
      long realVideoTime = getVideoTime();
      // Here we calculate the real audio/video offset for use during
      // playback (e.g. in the Lecturnity Player). 
      // We cannot use this value for the other sync calculations
      // as it does not take the video latency into account.
      // This value is usually saved into an LRD file.
      audioVideoOffset_ = waveTime - realVideoTime; // was: (realTime - firstTime_);

      // Instead we have to use this variable. This is the offset
      // between the real time, relative to the first call of
      // videoStreamCallback, and the wave time.
      lInternalAvOffset_ = waveTime - currentStreamTime;
//      char t[256];
//      sprintf(t, "waveTime == %d, videoTime == %d, internalAvOffset == %d", waveTime, realVideoTime, lInternalAvOffset_);
//      MessageBox(NULL, t, NULL, MB_OK);
#ifdef _DEBUGFILE
      fprintf(debugFile, "-----> Audio/video offset: %d (realTime: %d, firstTime: %d, waveTime: %d, realVideoTime: %d, internalOffset: %d)\n", audioVideoOffset_, realTime, firstTime_, waveTime, realVideoTime, lInternalAvOffset_);
      fprintf(debugFile, "   skew preroll to this frame: %d\n", realTime - ulPrerollTime_);
      fflush(debugFile);
#endif

      bAudioControlDone_ = true;
   }

   // Should we try to create preview thumbnails at all?
   if (isThumbnailEnabled())
   {
      // Yes. Is it time for a new preview thumbnail?
      if (frameCounter_ - lastThumbnailFrame_ > checkSyncInterval_)
      {
         if (decompressForThumbnail_)
         {
            // we need the decompressor to make a 24 RGB DIB of this
            if (lpVHdr->dwFlags & VHDR_KEYFRAME)
            {
               // we can only decompress keyframes, as we only do it once in a while
               ICDecompress(thumbnailHic_,
                  0, // no flags
                  &videoFormat_->bmiHeader,
                  lpVHdr->lpData,
                  &thumbnailInfo_->bmiHeader,
                  thumbnailData_);

               lastThumbnailFrame_ = frameCounter_;
            }
         }
         else
         {
            // the video data is RGB data, so just copy it
            memcpy(thumbnailData_, lpVHdr->lpData, lpVHdr->dwBytesUsed);

            lastThumbnailFrame_ = frameCounter_;
         }
      }
   }

   // Is this the first time this method is called
   // after a pause? Note: The variable wasPaused_
   // is also true at start, i.e. the next block
   // is executed the first time this method is
   // called at all.
	if (wasPaused_)
	{
      // At a start (or restart) of the video capture,
      // we need a keyframe from the video device. This
      // is always the case with RGB and YUV formats, but
      // some MJPG and/or MPG formats also have other
      // types of frames which are not keyframes. In that
      // case, act like we were still paused.
		if ((lpVHdr->dwFlags & VHDR_KEYFRAME) == 0)
		{
#ifdef _DEBUG
			printf("Cannot start with non-keyframe...\n");
#endif
			return TRUE;
		}
		lastStartTime_    = 1000i64 * lpVHdr->dwTimeCaptured;
      // First time ever?
      bool firstTimeEver = false;
      if (mostRecentTimeCaptured_ == -1)
      {
         firstTimeEver = true;
         mostRecentTimeCaptured_ = 1000i64 * lpVHdr->dwTimeCaptured;
      }
		lastOurStartTime_ = usecsPerFrame_ * frameCounter_;
		wasPaused_        = false;

		// update the video pause time offset, is last dwTimeCaptured deducted from
		// the current dwTimeCaptured. From that, we have to subtract one frame. 
      // Rationale: The time stamp in mostRecentTimeCaptured is the time stamp at the
      // beginning of the validity span of that frame, but it is valid for another
      // frame time (e.g. 40ms at 25 fps).
      if (!firstTimeEver)
		   pausedTime_ += 
            (1000i64 * lpVHdr->dwTimeCaptured - mostRecentTimeCaptured_) - usecsPerFrame_;
      else
         pausedTime_ = 0;
	}

	// First, we'll check for any hardware-dropped frames:
	// We check that by comparing the frame counter times
	// the frame time (us/frame) with the "captured time"
	// given in the VIDEOHDR structure. If these two values
	// deviate too much (can only be in one direction), we 
	// have to drop frames, as they were dropped by the
	// hardware.
	TIME_US hardwareDelta = (((TIME_US) (1000i64 * lpVHdr->dwTimeCaptured)) - lastStartTime_)
							- (usecsPerFrame_ * frameCounter_ - lastOurStartTime_);

   int framesToWrite = 1;

	int hardwareDropFrames = 0;
   if (abs((int)hardwareDelta) > mediaThreshold_)
   {
#ifdef _DEBUGFILE
      fprintf(debugFile, "hardware delta: %d\n", hardwareDelta);
      fflush(debugFile);
#endif
      if (hardwareDelta > 0) // probably hardware drop
	      hardwareDropFrames = (int) ((hardwareDelta + usecsPerFrame_ / 2i64) / usecsPerFrame_);
      else // this must be some crappy capture driver
      {
   	   hardwareDropFrames = (int) ((hardwareDelta - usecsPerFrame_ / 2i64) / usecsPerFrame_);
#ifdef _DEBUGFILE
         fprintf(debugFile, "+++ hardware forward drop.\n");
         fflush(debugFile);
#endif
      }

      framesToWrite += hardwareDropFrames;
   }

	// Now: All in all, we have to make sure that the frame counter
	// is increased by hardwareDropFrames + 1. That assures that we
	// are in sync with the hardware on this level.

	// We will fill this variable with a suitable value eventually.
	// It is not used if the audio sync is not applied. Then it is
	// always zero. This value may also be below zero. If it is,
	// frames are "forward dropped".
	int audioSyncDropFrames = 0;

#ifdef _DEBUG
   if (hardwareDropFrames != 0)
		printf("hardware drop: %d (delta: %d)\n", hardwareDropFrames, (long) hardwareDelta);
#endif

	if (!audioSync_)
	{
		// No audio to sync to; then the hardwareDropFrames counter
		// is all we need; no need to do any further actions here now.
	}
	else
	{
		// We want to sync the video to a (master) audio stream.
		// First, we'll check if we're already in the tight audio
		// sync mode, which means that we have already calculated
		// an audio/video time factor for the streams. We need this
		// factor due to the fact that most sound cards' oscillators
		// are inherently inexact: The stream lengths of video and 
		// audio normally differ, sometimes as much as a percent (1%)
		// in length, which results in seconds of unsynchronity with
		// a large recording (and is thus noticeable).
      //
		// Note: If audio has not yet started, it's pointless
		// to try to do a synchronization with it (bAudioControlDone_).
      if (audioSyncMode_ && bAudioControlDone_)
		{
			// We are in the audio tight sync mode; the time sync is
			// done over the pausedTime_ variable. The hardware sync
			// does primarily not have anything to do with the audio
			// time sync. It is just a question of manipulating the
			// pausedTime_ counter.

         // Calculate the current video time. See above for an
         // explanation of the lInternalAvOffset_ variable.
         TIME_US videoTime = 1000i64 * (currentStreamTime + lInternalAvOffset_);
         // Estimate the audio time from the current video time
			TIME_US alpha = (TIME_US) 
				(audioVideoFactor_ * (videoTime - pausedTime_));

         // This is the real audio time (in us)
			TIME_US beta = 1000i64 * waveTime;
#ifdef _DEBUGFILE
         fprintf(debugFile, "* sync: alpha == %d, beta == %d\n", (long) (alpha / 1000i64), (long) (beta / 1000i64));
         fflush(debugFile);
#endif

         // This is the estimated time for one frame (in us)
         TIME_US gamma = (TIME_US) (audioVideoFactor_ * usecsPerFrame_);

         // Calculate the difference between the estimated audio
         // time (alpha) and the real audio time (beta).
			TIME_US delta = alpha - beta;

			if (abs((int)delta) > mediaThreshold_)
			{
				// Now, we have a large deviation estimated audio time
				// (real time times audio-video factor) vs. real audio
				// time here. So we'll check what kind of resync we'll
				// have to do.
#ifdef _DEBUG
				printf("est. audio - real audio == %d\n", (long) delta);
#endif

				if (delta > 0)
				{
					// Video is faster than audio; video has to wait
					// for audio: A "forward drop".

					audioSyncDropFrames = (int) (( -delta - gamma / 2 ) / gamma);
#ifdef _DEBUG
					printf("audio says: forward drop: %d frames.\n", audioSyncDropFrames);
#endif
				}
				else
				{
					// Audio is faster than video; we need to insert
					// a null frame or two into the video file to
					// catch up.

					audioSyncDropFrames = (int) (( -delta + gamma / 2 ) / gamma);
#ifdef _DEBUG
					printf("audio says: backward drop: %d frames.\n", audioSyncDropFrames);
#endif
				}
			}

		}
	}

   if (hardwareDropFrames >= 0)
   {
      frameCounter_ += hardwareDropFrames + 1;

      // NOT TRUE: The captured frame will be written in any case: So update
	   // the most recent frame time variable:
	   mostRecentTimeCaptured_ = 1000i64 * lpVHdr->dwTimeCaptured;
   }
   // else: don't update the frame counter, we expect another
   // frame for this slot.

   if (audioSyncDropFrames != 0)
   {
#ifdef _DEBUGFILE
      fprintf(debugFile, "### audioSyncDropFrames == %d, hardwareDropFrames == %d, framesToWrite == %d\n", audioSyncDropFrames, hardwareDropFrames, framesToWrite);
      fflush(debugFile);
#endif
      bool updatedPausedTime = false;
      if (hardwareDropFrames == 0)
      {
         if (audioSyncDropFrames < 0)
         {
            pausedTime_ += usecsPerFrame_;
            framesToWrite = 0;
            updatedPausedTime = true;
         }
         else if (audioSyncDropFrames > 0)
         {
            pausedTime_ -= usecsPerFrame_ * audioSyncDropFrames;
            framesToWrite += audioSyncDropFrames;
            updatedPausedTime = true;
         }
         // else audioSyncDropFrames == 0 ==> Do nothing for A/V sync
      }
      else if (hardwareDropFrames > 0)
      {
         // We have hardware drops and we need to sync A/V at the
         // same time. Tricky.
         long framesOffSync = abs(audioSyncDropFrames);
         if (audioSyncDropFrames < 0)
         {
            // Hardware backward drop and A/V forward drop. Now do the right thing.
            if (framesToWrite > framesOffSync)
            {
               pausedTime_ += usecsPerFrame_ * framesOffSync;
               framesToWrite -= framesOffSync;
               updatedPausedTime = true;
            }
            else if (framesToWrite == framesOffSync)
            {
               pausedTime_ += usecsPerFrame_ * framesOffSync;
               framesToWrite = 0; // == framesToWrite - framesOffSync
               updatedPausedTime = true;
            }
            else // framesToWrite < framesOffSync
            {
               pausedTime_ += usecsPerFrame_ * framesToWrite;
               framesToWrite = 0; // as framesToWrite < framesOffSync
               updatedPausedTime = true;
            }
         }
         else if (audioSyncDropFrames == 0)
         {
            // Nothing has to be done
         }
         else // audioSyncDropFrames > 0
         {
            pausedTime_ -= usecsPerFrame_ * framesOffSync;
            framesToWrite += framesOffSync;
            updatedPausedTime = true;
         }
      }
      else // (hardwareDropFrames < 0)
      {
         if (audioSyncDropFrames <= 0)
         {
            // Do nothing, just forward drop. This frame is not supposed
            // to be here anyway, so we must drop it in each case, and
            // we must not use it in order to correct A/V sync problems.
         }
         else // audioSyncDropFrames > 0
         {
            // This frame is not supposed to be here, but as we need to
            // make an A/V sync anyway, we may use it for that purpose
            long framesOffSync = audioSyncDropFrames;
            pausedTime_ -= usecsPerFrame_ * framesOffSync;
            framesToWrite += framesOffSync;
         }
      }
#ifdef _DEBUGFILE
      if (updatedPausedTime)
      {
         fprintf(debugFile, "++ updated paused time: %d, audio drop: %d\n", (long) pausedTime_, audioSyncDropFrames);
         fflush(debugFile);
      }
#endif
   }

   dropCounterAudio_ += audioSyncDropFrames;

   // ==>

   //framesToWrite += audioSyncDropFrames;
   // <==

   if (framesToWrite <= 0)
   {
#ifdef _DEBUGFILE
      fprintf(debugFile, "~~~ performing forward drop.\n");
      fflush(debugFile);
#endif
      return TRUE;
   }


   // Note: Here hardwareDropFrames is >= 0.

	// In any case: update the frameCounter to match the hardware:
   //frameCounter_ += (hardwareDropFrames < -1 ? -1 : hardwareDropFrames) + 1;
   //frameCounter_ += hardwareDropFrames + 1;

   if (framesToWrite > 1)
   {
      int dropAction = framesToWrite - 1;

		// We need to insert some null frames into the AVI
		// file for some reason
#ifdef _DEBUGFILE
   fprintf(debugFile, "dropping %d frames. hw drop: %d, audio drop: %d, real time: %d, wave time: %d, hw delta: %d\n", 
	   dropAction, hardwareDropFrames, audioSyncDropFrames, realTime, waveTime, hardwareDelta);
   fflush(debugFile);
#endif

		dropCounter_ += dropAction;

		for (int j=0; j<dropAction; ++j)
      {
         // update the frame byte quota:
         if (codecInfo_.bSupportsDatarate && codecInfo_.bUseDatarate)
            accFrameByteQuota_ += frameByteQuota_ - 24;

			aviOutputFile_->writeNullFrame();
      }
   }

   accFrameByteQuota_ += frameByteQuota_;
   // Check for too small quotas: If the accumulated frame quota is smaller than
   // half the quota for a single frame, set to half the frame quota.
   if (accFrameByteQuota_ < frameByteQuota_/2)
      accFrameByteQuota_ = frameByteQuota_/2;

	// write some data perhaps:
   DWORD size;
   if (!bNullFramesMode_)
   {
      if (codecInfo_.bUseDatarate)
      {
   	   size = aviOutputFile_->writeFrame(lpVHdr->lpData, lpVHdr->dwBytesUsed, 
                                                     accFrameByteQuota_, lpVHdr->dwFlags,
                                                     lpVHdr->dwUser, (QCDATA*)lpVHdr->dwReserved[0], 
                                                     (SrMouseData*)lpVHdr->dwReserved[1],
                                                     (SrClickData*)lpVHdr->dwReserved[2],
                                                     lpVHdr->dwTimeCaptured);
      }
      else
      {
   	   size = aviOutputFile_->writeFrame(lpVHdr->lpData, 
                                                     lpVHdr->dwBytesUsed, 
                                                     (frameCounter_ == 1) ? 0xffffff : 0, //accFrameByteQuota_, 
                                                     lpVHdr->dwFlags,
                                                     lpVHdr->dwUser, (QCDATA*)lpVHdr->dwReserved[0], 
                                                     (SrMouseData*)lpVHdr->dwReserved[1],
                                                     (SrClickData*)lpVHdr->dwReserved[2],
                                                     lpVHdr->dwTimeCaptured);
      }
   }
   else
   {
      // we force NULL frames, even if it's not necessary.
      aviOutputFile_->writeNullFrame();
      size = 24; // null frame has 8 bytes, index has 16 bytes for frame.
   }

   totalSize_ += size;

   if (codecInfo_.bSupportsDatarate && codecInfo_.bUseDatarate)
      accFrameByteQuota_ -= size;

   /*
#ifdef DEBUG
    long _realTime = getRefTime() - firstTime_;
	printf("VideoCapturer::videoStreamCallback(): " << lpVHdr->dwTimeCaptured << 
		"(" << (getRefTime() - firstTime_) << " ms, delta " << (_realTime - (long) lpVHdr->dwTimeCaptured) << 
		")" << endl;
#endif
      */

	return TRUE;
}

void VideoCapturer::dumpVideoHeader(LPVIDEOHDR lpVHdr)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "dumpVideoHeader\n");
  fflush(debugFile);
#endif

	printf("lpVHdr->\n");
	printf("  dwBufferLength == %d\n", lpVHdr->dwBufferLength);
	printf("  dwBytesUsed == %d\n", lpVHdr->dwBytesUsed);
	printf("  dwFlags == %d\n", lpVHdr->dwFlags);

}

LRESULT CALLBACK VideoCapturer::controlCallback(HWND hWnd, int nState)
{
#ifdef _DEBUGFILE
//   fprintf(debugFile, "controlCallback(hWnd=0x%08x, nState=%d), PREROLL==%d\n", hWnd, nState, CONTROLCALLBACK_PREROLL);
//   fflush(debugFile);
#endif
   VideoCapturer *instance = retrieveInstance(hWnd);

   // DWORD time = timeGetTime();
   DWORD time = instance->getRefTime();

   if (nState == CONTROLCALLBACK_PREROLL)
	{
#ifdef DEBUG
		printf("Preroll done.\n");
#endif
#ifdef _DEBUGFILE
      if (instance->audio_)
      {
         fprintf(debugFile, "controlCallback: CONTROLCALLBACK_PREROLL\n", hWnd, nState);
         fflush(debugFile);
      }
#endif
		instance->prerollDone_ = true;

      
      instance->ulPrerollTime_ = time;

         
      if (instance->prerollEvent_)
   		SetEvent(instance->prerollEvent_);
	}

   if (instance->audioSync_ && nState == CONTROLCALLBACK_CAPTURING)
   {
      if (!instance->bFirstControlDone_)
      {
#ifdef _DEBUGFILE
         fprintf(debugFile, "FIRST TIME IN CAPTURING CONTROL CALLBACK: %d\n", time);
         fflush(debugFile);
#endif
         instance->ulFirstControlTime_ = time;
         instance->bFirstControlDone_  = true;
      }

      if (!instance->bAudioControlDone_)
      {
         DWORD lAudioTime = instance->audio_->getWaveInTimeMs();
         if (lAudioTime > 0)
         {
/*
#ifdef _DEBUGFILE
            fprintf(debugFile, "Offset: %d\n", (time - instance->ulFirstControlTime_ - lAudioTime));
            fflush(debugFile);
#endif
            instance->bAudioControlDone_ = true;
            instance->audioVideoOffset_ = (time - instance->ulFirstControlTime_ - lAudioTime);
*/
         }
      }
   }

   return TRUE;
}

LRESULT CALLBACK VideoCapturer::errorCallback(HWND hWnd, int nId, LPCTSTR lpsz)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "errorCallback(HWND=0x%08x, nId=%d, msg=\"%s\")\n", hWnd, nId, lpsz);
  fflush(debugFile);
#endif
   VideoCapturer *instance = retrieveInstance(hWnd);

   if (!nId)
      return 0;

   if (!instance->prerollDone_)
   {
      // the preroll is not yet done and we have encountered an error!
      _stprintf(instance->capErrorMsg_, _T("%d: %s"), nId, lpsz);
      instance->hasCapErrorOccurred_ = true;
      SetEvent(instance->prerollEvent_);
   }
   _tprintf(_T("***** errorCallback: #%d, %s\n"), nId, lpsz);
   return TRUE;
}


LRESULT CALLBACK VideoCapturer::statusCallback(HWND hwnd, int nID, LPCTSTR lpsz)
{
#ifdef _DEBUGFILE
  _ftprintf(debugFile, _T("statusCallback(hWnd=0x%08x, nID=%d, msg=\"%s\")\n"), hwnd, nID, lpsz);
  fflush(debugFile);
#endif
   VideoCapturer *instance = retrieveInstance(hwnd);

#ifdef DEBUG
	tprintf(_T("VideoCapturer::statusCallback(): %s\n"), lpsz);
#endif
	return TRUE;
}

void VideoCapturer::dumpCodecList()
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "dumpCodecList\n");
  fflush(debugFile);
#endif

	ICINFO icInfo;
	icInfo.dwSize = sizeof(ICINFO);
	BOOL res = TRUE;
	int i = 0;
	while (res)
	{
		res = ICInfo(ICTYPE_VIDEO, i++, &icInfo);
		if (res)
		{
			ICINFO moreInfo;
			moreInfo.dwSize = sizeof ICINFO;
			HIC hic = ICOpen(ICTYPE_VIDEO, icInfo.fccHandler, ICMODE_QUERY);
			LRESULT res2 = ICGetInfo(hic, &moreInfo, sizeof ICINFO);
			ICClose(hic);

			char* temp = (char*) malloc(128);
			int res = WideCharToMultiByte(CP_ACP, 0, moreInfo.szName, -1, temp, 128, NULL, NULL);
			printf("Codec #%d: %s\n", i, temp);
			printf("  Flags: %d\n", moreInfo.dwFlags);
			if (moreInfo.dwFlags & VIDCF_COMPRESSFRAMES)
				printf("  VIDCF_COMPRESSFRAMES\n");
			if (moreInfo.dwFlags & VIDCF_CRUNCH)
				printf("  VIDCF_CRUNCH\n");
			if (moreInfo.dwFlags & VIDCF_DRAW)
				printf("  VIDCF_DRAW\n");
			if (moreInfo.dwFlags & VIDCF_FASTTEMPORALC)
				printf("  VIDCF_FASTTEMPORALC\n");
			if (moreInfo.dwFlags & VIDCF_FASTTEMPORALD)
				printf("  VIDCF_FASTTEMPORALD\n");
			if (moreInfo.dwFlags & VIDCF_QUALITY)
				printf("  VIDCF_QUALITY\n");
			if (moreInfo.dwFlags & VIDCF_TEMPORAL)
				printf("  VIDCF_TEMPORAL\n");
			res = WideCharToMultiByte(CP_ACP, 0, moreInfo.szDescription, -1, temp, 128, NULL, NULL);
			printf("  Description: %s\n", temp);

			string fcc;
			fcc += (char) (moreInfo.fccHandler & 0x000000ff);
			fcc += (char) ((moreInfo.fccHandler >> 8) & 0x000000ff);
			fcc += (char) ((moreInfo.fccHandler >> 16) & 0x000000ff);
			fcc += (char) ((moreInfo.fccHandler >> 24) & 0x000000ff);

			printf("  fccHandler: %s (%08x)\n", fcc.c_str(), moreInfo.fccHandler);

			free(temp);
		}
		else
			printf("----> ICInfo() failed!\n");
	}
}

void VideoCapturer::throwVideoException(DWORD errorCode)
{
#ifdef _DEBUGFILE
  fprintf(debugFile, "throwVideoException\n");
  fflush(debugFile);
#endif

   switch (errorCode)
   {
   case ICERR_UNSUPPORTED:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_UNSUPPORTED));
   case ICERR_BADFORMAT:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_BADFORMAT));
   case ICERR_MEMORY:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_MEMORY));
   case ICERR_INTERNAL:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_INTERNAL));
   case ICERR_BADFLAGS:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_BADFLAGS));
   case ICERR_BADPARAM:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_BADPARAM));
   case ICERR_BADSIZE:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_BADSIZE));
   case ICERR_BADHANDLE:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_BADHANDLE));
   case ICERR_CANTUPDATE:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_CANTUPDATE));
   case ICERR_ABORT:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_ABORT));
   case ICERR_ERROR:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_ERROR));
   case ICERR_BADBITDEPTH:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_BADBITDEPTH));
   case ICERR_BADIMAGESIZE:
      throw VideoIOException(Local::localizeString(VIDEO_CODEC_BADIMAGESIZE));
   default:
      {
         _TCHAR tmp[256];
         _stprintf(tmp, Local::localizeString(VIDEO_CODEC_UNKNOWN), errorCode);
         throw VideoIOException(tmp);
      }
   }
}

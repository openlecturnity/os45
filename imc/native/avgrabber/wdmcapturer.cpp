#include "global.h"
#include "resource.h"
#include <crtdbg.h>

#include "LanguageSupport.h"

//#include "wdmcapturer.h"
//#include "exceptions.h"
//#include <map>
//#include <vector>

//#define _DEBUG
//#define _DEBUGFILE

#ifdef _DEBUG
#define _DEBUGFILE
#endif

enum
{
   WM_WDM_QUIT = WM_USER,
      WM_WDM_STARTPREVIEW,
      WM_WDM_STOPPREVIEW,
      WM_WDM_STARTCAPTURE,
      WM_WDM_STOPCAPTURE,
      WM_WDM_READ_VIDEOSETTINGS,
      WM_WDM_SETFRAMERATE,
      WM_WDM_SETSTREAMCONFIGDATA,
      WM_WDM_GETAMMEDIATYPESIZE,
      WM_WDM_GETVIDEOHEADERSIZE,
      WM_WDM_GETSTREAMCONFIGDATA
};

typedef struct
{
   bool          bConvertToRgb;
   _DVRESOLUTION dvRes;
} DVDECODERPARAMS;

UINT GetShowCmd(HWND hWnd)
{
   WINDOWPLACEMENT wp;
   wp.length = sizeof WINDOWPLACEMENT;

   BOOL success = ::GetWindowPlacement(hWnd, &wp);

   if (success)
      return wp.showCmd;
   return SW_SHOW;
}

#ifdef _DEBUG
   #define ASSERT_THREAD() \
   { \
      DWORD dwId = ::GetCurrentThreadId(); \
      _ASSERT(dwId == dwThreadId_); \
   }
#else
   #define ASSERT_THREAD()
#endif

#define HAS_PROPERTIES(x) \
   { \
      ISpecifyPropertyPages *pProp = NULL; \
      hr = x->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp); \
      if (SUCCEEDED(hr)) \
      { \
         CAUUID caGUID; \
         pProp->GetPages(&caGUID); \
         pProp->Release(); \
         hasPages = (caGUID.cElems > 0); \
         ::CoTaskMemFree(caGUID.pElems); \
      } \
   }

#define INTERFACE_LIST (*((std::vector<WDM_INTERFACE_ID> *) pInterfaces_))


#ifdef _DEBUG
   DWORD g_dwRegister;

   HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
   {
      IMoniker * pMoniker;
      IRunningObjectTable *pROT;
      if (FAILED(GetRunningObjectTable(0, &pROT))) {
         return E_FAIL;
      }
      WCHAR wsz[256];
      wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
      HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
      if (SUCCEEDED(hr)) {
         hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
         pMoniker->Release();
      }
      pROT->Release();
      return hr;
   }

   void RemoveFromRot(DWORD pdwRegister)
   {
      IRunningObjectTable *pROT;
      if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
         pROT->Revoke(pdwRegister);
         pROT->Release();
      }
   }
#endif

#ifdef _DEBUGFILE
   void DebugMsg(char *szFormat)
   {
      static FILE *debugFile = NULL;

      if (!debugFile)
         debugFile = fopen("C:\\wdm_debug.txt", "w");
      fprintf(debugFile, "%05d - Thread 0x%08x: ", ::timeGetTime()%100000, GetCurrentThreadId());
      fprintf(debugFile, szFormat);
      fflush(debugFile);
   }
#endif


// static variables:
int           WdmCapturer::instanceCount_        = 0;
//void*         WdmCapturer::pWdmDevices_          = NULL;
_TCHAR      **WdmCapturer::s_atszDevices         = NULL;
int           WdmCapturer::s_iDeviceListSize     = 0;
_DVRESOLUTION WdmCapturer::dvResolutionDefault_  = DVRESOLUTION_HALF;
bool          WdmCapturer::convertDvDataDefault_ = true;

map<HWND, WdmCapturer *> instanceMap_;

WdmCapturer::WdmCapturer(_TCHAR *tszDeviceName) : VideoCapturer(-1)
{
#ifdef _DEBUGFILE
   DebugMsg("WdmCapturer()\n");
#endif
   if (instanceCount_ == 0)
      RegisterWdmMonitorClass();

   _tcsncpy(szDeviceName_, tszDeviceName, 128);

   pBuilder_    = NULL;
   pGraph_      = NULL;
   pSrc_        = NULL;
   pRefClock_   = NULL;
   pVideo_      = NULL;
   pInterfaces_ = NULL;

   hInitEvent_  = NULL;
   hExitEvent_  = NULL;
   hInitResult_ = S_OK;

   hMonitor_    = NULL;

   isGraphBuilt_ = false;
   isPreviewing_ = false;
   isDvSource_   = false;
   isVfwSource_  = false;
   isCapturing_  = false;
   captureInProgress_ = false;

   streamConfigData_.pAmMediaType = NULL;
   streamConfigData_.pVideoHeader = NULL;
   streamConfigData_.cbVideoHeader = 0;
   streamConfigData_.cbAmMediaType = 0;

   m_pStreamMediaType = NULL;

   streamConfigDataValid_ = false;

   dvResolution_  = dvResolutionDefault_;
   convertDvData_ = convertDvDataDefault_;

   initVariables();

   pSampleCallback_ = new CSampleCallback();
   pSampleCallback_->AddRef();
   pSampleCallback_->SetWdmCapturer((void *) this);

   instanceCount_++;

   hInitEvent_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);

   _beginthread(WindowLauncher, 0, this);

   DWORD res = ::WaitForSingleObject(hInitEvent_, INFINITE);
   ::CloseHandle(hInitEvent_);
   hInitEvent_ = NULL;

   // Now, did everything work out fine?
   if (FAILED(hInitResult_))
   {
      // No, an error occurred. Throw a suiting exception:
      _TCHAR t[MAX_ERROR_TEXT_LEN + 256];
      _TCHAR t2[MAX_ERROR_TEXT_LEN];
      // Retrieve the error message for the error code:
      DWORD res = ::AMGetErrorText(hInitResult_, t2, MAX_ERROR_TEXT_LEN);
      _stprintf(t, Local::localizeString(WDM_ERR_INIT), hInitResult_, t2);

      // Release all resources before bailing out
      ReleaseResources();

      // And off you go
      throw WdmException(t);
   }
}

WdmCapturer::~WdmCapturer()
{
#ifdef _DEBUGFILE
   DebugMsg("~WdmCapturer()\n");
#endif
   ReleaseResources();
}

void WdmCapturer::ReleaseResources()
{
#ifdef _DEBUGFILE
   DebugMsg("ReleaseResources()\n");
#endif
   instanceCount_--;

   if (hMonitor_)
   {
      // As a side effect, the WDM filter graph is torn down
      // in the WM_DESTROY window message:
      hExitEvent_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
      ::PostMessage(hMonitor_, WM_WDM_QUIT, 0, 0);

      DWORD res = ::WaitForSingleObject(hExitEvent_, 30000);
      ::CloseHandle(hExitEvent_);

      instanceMap_[hMonitor_] = NULL;
      unregisterInstance();
   }

   if (pInterfaces_)
      delete ((std::vector<WDM_INTERFACE_ID> *) pInterfaces_);

   if (pSampleCallback_)
   {
      pSampleCallback_->Release();
      pSampleCallback_ = NULL;
   }

   if (instanceCount_ <= 0)
   {
      instanceCount_ = 0;
      UnregisterWdmMonitorClass();
   }

   if (streamConfigData_.pAmMediaType != NULL)
      delete[] streamConfigData_.pAmMediaType;
   streamConfigData_.pAmMediaType = NULL;
   if (streamConfigData_.pVideoHeader != NULL)
      delete[] streamConfigData_.pVideoHeader;
   streamConfigData_.pVideoHeader = NULL;

   if (s_atszDevices != NULL)
   {
      for (int i=0; i<s_iDeviceListSize; ++i)
            delete[] s_atszDevices[i];
      delete[] s_atszDevices;
   }
   s_atszDevices = NULL;
   s_iDeviceListSize = 0;

   if (videoFormat_ != NULL)
       delete videoFormat_;
   videoFormat_ = NULL;

   /* TODO some objects (memory leak) remain. They are created in MonitorProc::WM_CREATE
   std::map<HWND, WdmCapturer *>::iterator iter2;
   for (iter2 = instanceMap_.begin(); iter2 != instanceMap_.end(); ++iter2)
   {
      delete (*iter2);
   }
   */
   instanceMap_.clear();
}

void WdmCapturer::RegisterWdmMonitorClass()
{
#ifdef _DEBUGFILE
   DebugMsg("RegisterWdmMonitorClass()\n");
#endif
   WNDCLASSEX wnd;
   memset(&wnd, 0, sizeof WNDCLASSEX);

   wnd.cbSize        = sizeof WNDCLASSEX;
   wnd.style         = CS_NOCLOSE;
   wnd.lpfnWndProc   = MonitorProc;
   wnd.cbClsExtra    = 0;
   wnd.cbWndExtra    = 0;
   wnd.hInstance     = g_hDllInstance;
   wnd.hIcon         = NULL;
   wnd.hIconSm       = NULL;
   wnd.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
   wnd.hbrBackground = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
   wnd.lpszMenuName  = NULL;
   wnd.lpszClassName = _T("WdmMonitorWindow");

   ATOM atom = ::RegisterClassEx(&wnd);
}

void WdmCapturer::UnregisterWdmMonitorClass()
{
#ifdef _DEBUGFILE
   DebugMsg("UnregisterWdmMonitorClass()\n");
#endif
   ::UnregisterClass(_T("WdmMonitorWindow"), g_hDllInstance);
}

void __cdecl WdmCapturer::WindowLauncher(void *pData)
{
#ifdef _DEBUGFILE
   DebugMsg("WindowLauncher()\n");
#endif
   
   CLanguageSupport::SetThreadLanguage();

   WdmCapturer *pWdm = (WdmCapturer *) pData;
   pWdm->WindowLoop();
}

void WdmCapturer::WindowLoop()
{
#ifdef _DEBUGFILE
   DebugMsg("WindowLoop()\n");
#endif
   ::CoInitialize(NULL);

   dwThreadId_ = ::GetCurrentThreadId();

   hInitResult_ = InitFilterGraph();

   // Note: Now we know whether we have a DV source
   // or a "regular" source. This can be read out
   // using the IsDvSource() method.

   if (SUCCEEDED(hInitResult_))
   {
      // Initialize the stream config data,
      // i.e. the media type and the stream
      // subtype data members.
      /* HRESULT hr = */ 
      InitWdmStreamConfigData();

      // Now try to set some sensible parameters
      // for the video device, like 320x240@25fps
      // and RGB data model.
      SetWdmDefaultParameters();

      isGraphBuilt_ = true;
   }
   else
   {
      // Yikes, this is not good at all.
      isGraphBuilt_ = false;

      // Logic: hInitResult_ is set to something
      // which is FAILED; in that case, the constructur
      // will throw an exception.
      if (hInitEvent_)
         ::SetEvent(hInitEvent_);

      return;
   }

   MakeWindow();

   if (hInitEvent_)
      ::SetEvent(hInitEvent_);

   MSG msg;
   while (::GetMessage(&msg, hMonitor_, 0, 0) > 0)
   {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
   }

   if (hExitEvent_)
      ::SetEvent(hExitEvent_);

   ::CoUninitialize();
}

void WdmCapturer::MakeWindow()
{
#ifdef _DEBUGFILE
   DebugMsg("MakeWindow()\n");
#endif
   hMonitor_ = ::CreateWindowEx(
      WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
      _T("WdmMonitorWindow"),
      _T("Lecturnity Video Tool - WDM Monitor"),
      WS_OVERLAPPEDWINDOW,
      100, 100,
      320, 240,
      NULL, NULL,
      g_hDllInstance,
      this);

   ::ShowWindow(hMonitor_, SW_HIDE);
   ::UpdateWindow(hMonitor_);
}

LRESULT CALLBACK WdmCapturer::MonitorProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUGFILE
//   DebugMsg("MonitorProc()\n");
#endif
   switch (uMsg)
   {
   case WM_CREATE:
      {
         LPCREATESTRUCT lpCS = (LPCREATESTRUCT) lParam;
         instanceMap_[hWnd] = (WdmCapturer *) lpCS->lpCreateParams;
         instanceMap_[hWnd]->registerInstance(hWnd);
      }
      break;

   case WM_DESTROY:
      instanceMap_[hWnd]->DestroyWdmCapturer();
      break;

   case WM_WDM_QUIT:
      DestroyWindow(hWnd);
      return 0;

   case WM_WDM_STARTPREVIEW:
      return instanceMap_[hWnd]->StartPreview();

   case WM_WDM_STOPPREVIEW:
      return instanceMap_[hWnd]->StopPreview();

   case WM_WDM_STARTCAPTURE:
      return instanceMap_[hWnd]->StartCapture();

   case WM_WDM_STOPCAPTURE:
      return instanceMap_[hWnd]->StopCapture();

   case WM_WDM_READ_VIDEOSETTINGS:
      return instanceMap_[hWnd]->RetrieveVideoSettings();

   case WM_WDM_SETFRAMERATE:
      return instanceMap_[hWnd]->SetWdmFrameRate((REFERENCE_TIME *) lParam);

   case WM_WDM_GETAMMEDIATYPESIZE:
      return instanceMap_[hWnd]->GetWdmStreamConfigMediaTypeSize((DWORD *) lParam);

   case WM_WDM_GETVIDEOHEADERSIZE:
      return instanceMap_[hWnd]->GetWdmStreamConfigVideoHeaderSize((DWORD *) lParam);

   case WM_WDM_SETSTREAMCONFIGDATA:
      return instanceMap_[hWnd]->SetWdmStreamConfigData();

   case WM_WDM_GETSTREAMCONFIGDATA:
      return instanceMap_[hWnd]->GetWdmStreamConfigData();

   case WM_COMMAND:
      instanceMap_[hWnd]->streamConfigDataValid_ = false;

      switch (LOWORD(lParam))
      {
      case ID_DV_DECODER:
         instanceMap_[hWnd]->DisplayDvDecoderDialog((HWND) wParam);
         break;
      case ID_CAPTUREFILTER:
         instanceMap_[hWnd]->DisplayCaptureFilterDialog((HWND) wParam);
         break;
      case ID_ANALOGVIDEODECODER:
         instanceMap_[hWnd]->DisplayAnalogVideoDecoderDialog((HWND) wParam);
         break;
      case ID_STREAMCONFIG:
         instanceMap_[hWnd]->DisplayStreamConfigDialog((HWND) wParam);
         break;
      case ID_CROSSBAR:
         instanceMap_[hWnd]->DisplayCrossbarDialog((HWND) wParam);
         break;
      case ID_VFW_SOURCE:
         instanceMap_[hWnd]->DisplayVfwSourceDialog((HWND) wParam);
         break;
      case ID_VFW_FORMAT:
         instanceMap_[hWnd]->DisplayVfwFormatDialog((HWND) wParam);
         break;
      case ID_VFW_DISPLAY:
         instanceMap_[hWnd]->DisplayVfwDisplayDialog((HWND) wParam);
         break;
      }
      break;
   }

   return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// --------------------------------- //
// VideoCapturer Overridden methods
// --------------------------------- //
void WdmCapturer::displayMonitor(bool b)
{
#ifdef _DEBUGFILE
   DebugMsg("displayMonitor()\n");
#endif
   if (!isGraphBuilt_)
      return;
   if (isCapturing_)
      return;

   if (b && !isPreviewing_)
   {
      // Start previewing
      ::SendMessage(hMonitor_, WM_WDM_STARTPREVIEW, 0, 0);
   }
   else if (!b && isPreviewing_)
   {
      // Stop previewing
      ::SendMessage(hMonitor_, WM_WDM_STOPPREVIEW, 0, 0);
   }
}

TIME_US WdmCapturer::setFrameRate(double frameRate)
{
#ifdef _DEBUGFILE
   DebugMsg("setFrameRate()\n");
#endif
   REFERENCE_TIME timePerFrame = (REFERENCE_TIME) (10000000.0 / frameRate);

   HRESULT hr = ::SendMessage(hMonitor_, WM_WDM_SETFRAMERATE, 0, (LPARAM) &timePerFrame);

   return getUSecsPerFrame();
}

void WdmCapturer::start(_TCHAR *tszFileName, AudioCapturer *audio)
{
#ifdef _DEBUGFILE
   DebugMsg("start()\n");
#endif
   HRESULT hr;

   initStartVariables(tszFileName, audio);

   hr = (HRESULT) ::SendMessage(hMonitor_, WM_WDM_STARTCAPTURE, 0, 0);

   if (FAILED(hr))
   {
      closeAviFile();
      _TCHAR t[MAX_ERROR_TEXT_LEN + 256];
      _TCHAR t2[MAX_ERROR_TEXT_LEN];
      DWORD res = ::AMGetErrorText(hr, t2, MAX_ERROR_TEXT_LEN);
      _stprintf(t, Local::localizeString(WDM_ERR_START), debugPosition_, hr, t2);
      throw WdmException(t);
   }
}

void WdmCapturer::stop()
{
#ifdef _DEBUGFILE
   DebugMsg("stop()\n");
#endif
   HRESULT hr;

   hr = (HRESULT) ::SendMessage(hMonitor_, WM_WDM_STOPCAPTURE, 0, 0);

   closeAviFile();

   // TODO: Exception if error?
}

long WdmCapturer::getRefTime()
{
#ifdef _DEBUGFILE
   DebugMsg("getRefTime()\n");
#endif
   static REFERENCE_TIME time;
   static HRESULT        hr;
   if (pRefClock_)
   {
      hr = pRefClock_->GetTime(&time);
      if (SUCCEEDED(hr))
         return (long) (time / 10000i64); // convert 100ns -> ms
   }

   return timeGetTime();
}

long WdmCapturer::getVideoTime()
{
#ifdef _DEBUGFILE
   DebugMsg("getVideoTime()\n");
#endif
   HRESULT hr = S_OK;
   IMediaSeeking *pMS = NULL;
   LONGLONG llCurrent = 0i64;

   hr = pGraph_->QueryInterface(IID_IMediaSeeking, (void **) &pMS);
   if (SUCCEEDED(hr))
   {
      hr = pMS->GetCurrentPosition(&llCurrent);
      pMS->Release();
      pMS = NULL;
   }

   if (SUCCEEDED(hr))
   {
      return (long) (llCurrent / 10000i64); // 100 ns -> ms
   }

   return 0L;
}

// ---------------------------------------- //
// Public methods
// ---------------------------------------- //

bool WdmCapturer::IsDvSource()
{
#ifdef _DEBUGFILE
   DebugMsg("IsDvSource()\n");
#endif
   return isDvSource_;
}

bool WdmCapturer::IsVfwSource()
{
#ifdef _DEBUGFILE
   DebugMsg("IsVfwSource()\n");
#endif
   return isVfwSource_;
}

int WdmCapturer::GetInterfaceCount()
{
#ifdef _DEBUGFILE
   DebugMsg("GetInterfaceCount()\n");
#endif
   return INTERFACE_LIST.size();
}

void WdmCapturer::GetInterfaces(WDM_INTERFACE *pInterfaces)
{
#ifdef _DEBUGFILE
   DebugMsg("GetInterfaces()\n");
#endif
   int c = INTERFACE_LIST.size();

   for (int i=0; i<c; ++i)
   {
      pInterfaces[i].widId = INTERFACE_LIST[i];

      int lid = 0;
      switch (pInterfaces[i].widId)
      {
      case ID_CAPTUREFILTER:
         lid = WDM_ID_CAPTUREFILTER;
         break;
      case ID_ANALOGVIDEODECODER:
         lid = WDM_ID_ANALOGVIDEODECODER;
         break;
      case ID_STREAMCONFIG:
         lid = WDM_ID_STREAMCONFIG;
         break;
      case ID_CROSSBAR:
         lid = WDM_ID_CROSSBAR;
         break;
      case ID_TVTUNER:
         lid = WDM_ID_TVTUNER;
         break;
      case ID_DV_DECODER:
         lid = WDM_ID_DV_DECODER;
         break;
      case ID_VFW_SOURCE:
         lid = WDM_ID_VFW_SOURCE;
         break;
      case ID_VFW_FORMAT:
         lid = WDM_ID_VFW_FORMAT;
         break;
      case ID_VFW_DISPLAY:
         lid = WDM_ID_VFW_DISPLAY;
         break;
      default:
         lid = WDM_ID_UNKNOWN;
         break;
      }

      _tcscpy(pInterfaces[i].tszDescription, Local::localizeString(lid));
   }
}

// ---------------------------------------- //

HRESULT WdmCapturer::GetWdmSourcesCount(int *pnDevCount)
{
#ifdef _DEBUGFILE
   DebugMsg("GetWdmSourcesCount()\n");
#endif
   if (pnDevCount == NULL)
      return E_INVALIDARG;

   HRESULT hr;   

   hr = ::CoInitialize(NULL);

   if (SUCCEEDED(hr))
   {
      if (s_atszDevices != NULL)
      {
         for (int i=0; i<s_iDeviceListSize; ++i)
            delete[] s_atszDevices[i];
         delete[] s_atszDevices;
      }
      s_atszDevices = NULL;
      s_iDeviceListSize = 0;

      ICreateDevEnum *pSysDevEnum = NULL;
      IEnumMoniker   *pEnumCat    = NULL;

#ifdef _DEBUGFILE
      DebugMsg("GetWdmSourcesCount: pSysDevEnum\n");
#endif

      if (SUCCEEDED(hr))
      {
         // Create the System Device Enumerator.
         hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, 
                                 NULL, 
                                 CLSCTX_ALL, 
                                 IID_ICreateDevEnum, 
                                 (void **) &pSysDevEnum);
      }

#ifdef _DEBUGFILE
      DebugMsg("GetWdmSourcesCount: pEnumCat\n");
#endif

      if (SUCCEEDED(hr) && pSysDevEnum)
      {
         // Obtain a class enumerator for the video input device category.
         hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, 
                                                 &pEnumCat, 
                                                 0);
      }

      // Enumerate the monikers.
      IMoniker *pMoniker    = NULL;
      ULONG     cFetched    = 0;

      int       nCount      = 0;
      bool      foundDevice = false;

      // Note: hr may be S_FALSE here; then pEnumCat is NULL because there
      // do not exist any devices in the requested category.
      if (SUCCEEDED(hr) && pEnumCat)
      {
         int iDeviceCount = 0;

         while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
         {
            ++iDeviceCount;

            pMoniker->Release();
            pMoniker = NULL;
         }

         hr = pEnumCat->Reset();

         if (SUCCEEDED(hr))
         {
            s_atszDevices = new _TCHAR*[iDeviceCount];
            s_iDeviceListSize = iDeviceCount;
         }

#ifdef _DEBUGFILE
         DebugMsg("GetWdmSourcesCount: Before while-loop\n");
#endif
         int iIndex = 0;
         while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK && SUCCEEDED(hr))
         {
#ifdef _DEBUGFILE
            DebugMsg("GetWdmSourcesCount: In while-loop\n");
#endif
            IPropertyBag *pProp;
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pProp);
            if (SUCCEEDED(hr) && pProp)
            {
               VARIANT varName;
#ifdef _DEBUGFILE
               DebugMsg("GetWdmSourcesCount:    VariantInit\n");
#endif
               VariantInit(&varName); // Try to match the friendly name.
               hr = pProp->Read(L"FriendlyName", &varName, 0); 
               if (SUCCEEDED(hr))
               {
#ifdef _DEBUGFILE
                  DebugMsg("GetWdmSourcesCount:    Convert bstr -> char*\n");
#endif
                  _bstr_t tmp(varName);

                  s_atszDevices[iIndex] = new _TCHAR[tmp.length() + 1];
                  ZeroMemory(s_atszDevices[iIndex], (tmp.length() + 1) * sizeof _TCHAR);

                  _tcsncpy(s_atszDevices[iIndex], (_TCHAR *)tmp, tmp.length());

                  ++iIndex;
               }
#ifdef _DEBUGFILE
                  DebugMsg("GetWdmSourcesCount:    VariantClear()\n");
#endif
               VariantClear(&varName);
               pProp->Release();
               pProp = NULL;
            }
#ifdef _DEBUGFILE
            DebugMsg("GetWdmSourcesCount:    Release Moniker\n");
#endif
            pMoniker->Release();
            pMoniker = NULL; // Release for the next loop.
#ifdef _DEBUGFILE
            DebugMsg("GetWdmSourcesCount:    End of loop\n");
#endif
         }
#ifdef _DEBUGFILE
         DebugMsg("GetWdmSourcesCount: while-loop finished\n");
#endif
      }


      if (pMoniker)
         pMoniker->Release();
      pMoniker = NULL;

      if (pEnumCat)
         pEnumCat->Release();
      pEnumCat = NULL;

      if (pSysDevEnum)
         pSysDevEnum->Release();
      pSysDevEnum = NULL;

   }

   if (FAILED(hr))
   {
      _TCHAR t[512];
      _TCHAR dxerr[MAX_ERROR_TEXT_LEN];
      // Retrieve the error message for the error code:
      DWORD res = ::AMGetErrorText(hr, dxerr, MAX_ERROR_TEXT_LEN);

      _stprintf(t, _T("Fehler aufgetreten: %08x,\nDirectX sagt: \"%s\""), hr, dxerr);

      ::MessageBox(NULL, t, _T("Mööp"), MB_OK | MB_TOPMOST);
   }

   ::CoUninitialize();

   *pnDevCount = s_iDeviceListSize;//((std::vector<char *> *) pWdmDevices_)->size();

   return hr;
}

HRESULT WdmCapturer::GetWdmSources(_TCHAR **paDevices, int maxLen)
{
#ifdef _DEBUGFILE
   DebugMsg("GetWdmSources()\n");
#endif
   int count = 0;
   HRESULT hr;
   if (s_atszDevices == NULL)
   {
      hr = GetWdmSourcesCount(&count);
   }
   else
   {
      count = s_iDeviceListSize; //((std::vector<char *> *) pWdmDevices_)->size();
      hr = S_FALSE;
   }

   if (SUCCEEDED(hr))
   {
      for (int i=0; i<count; ++i)
      {
         _tcsncpy(paDevices[i], s_atszDevices[i], maxLen);//((std::vector<char *> *) pWdmDevices_)->at(i), maxLen);
      }
   }

   return hr;
}

char MakeHex(char c)
{
   if (c < 10)
      return (char) (c + 0x30);
   else
      return (char) (c + 0x57);
}

char MakeInt(char c)
{
   if (c > 0x39)
      return (char) (c - 0x57);
   else
      return (char) (c - 0x30);
}

char MakeChar(char c1, char c2)
{
   return (MakeInt(c1) << 4) | MakeInt(c2);
}

void Make4Bit(char *pSrc, char *pDest, int length)
{
   int destIndex = 0;
   for (int i=0; i<length; ++i)
   {
      pDest[destIndex++] = MakeHex((pSrc[i] & 0xf0) >> 4);
      pDest[destIndex++] = MakeHex(pSrc[i] & 0x0f);
   }
}

void Make8Bit(char *pSrc, char *pDest, int length)
{
   int byteLen = length / 2;
   int srcIndex = 0;
   for (int i=0; i<byteLen; ++i)
   {
      char c1 = pSrc[srcIndex++];
      char c2 = pSrc[srcIndex++];
      pDest[i] = MakeChar(c1, c2);
   }
}

void WdmCapturer::SetStreamConfigData(char *pAmMediaType, int cbAmMedia,
                                      char *pVideoHeader, int cbVideoHeader)
{
   if (streamConfigData_.pAmMediaType)
      delete [] streamConfigData_.pAmMediaType;
   if (streamConfigData_.pVideoHeader)
      delete [] streamConfigData_.pVideoHeader;

   streamConfigData_.pAmMediaType = new char[cbAmMedia/2 + 1];
   streamConfigData_.pVideoHeader = new char[cbVideoHeader/2 + 1];

   Make8Bit(pAmMediaType, streamConfigData_.pAmMediaType, cbAmMedia);
   Make8Bit(pVideoHeader, streamConfigData_.pVideoHeader, cbVideoHeader);

   VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)streamConfigData_.pVideoHeader;
   //_RPT1(_CRT_WARN, "Setting and storing streamConfigData_: comp %d\n", pVih->bmiHeader.biCompression);
   AM_MEDIA_TYPE *pMt = (AM_MEDIA_TYPE *)streamConfigData_.pAmMediaType;

   HRESULT hr = ::SendMessage(hMonitor_, WM_WDM_SETSTREAMCONFIGDATA, NULL, NULL);

   if (FAILED(hr))
   {
      throw WdmException(_T("Setting WDM video type failed!"));
   }
}

int  WdmCapturer::GetStreamConfigMediaTypeSize()
{
   DWORD dwSize;

   HRESULT hr = ::SendMessage(hMonitor_, WM_WDM_GETAMMEDIATYPESIZE, NULL, (LPARAM) &dwSize);

   if (FAILED(hr))
      return -1;

   return (int) (2*dwSize);
}

int  WdmCapturer::GetStreamConfigVideoHeaderSize()
{
   DWORD dwSize;

   HRESULT hr = ::SendMessage(hMonitor_, WM_WDM_GETVIDEOHEADERSIZE, NULL, (LPARAM) &dwSize);

   if (FAILED(hr))
      return -1;

   return (int) (2*dwSize);
}

void WdmCapturer::GetStreamConfigData(char *pAmMediaType, int cbAmMedia,
                                      char *pVideoHeader, int cbVideoHeader)
{
   HRESULT hr = ::SendMessage(hMonitor_, WM_WDM_GETSTREAMCONFIGDATA, NULL, NULL);

   VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)streamConfigData_.pVideoHeader;
   //_RPT1(_CRT_WARN, "Getting streamConfigData_: comp %d\n", pVih->bmiHeader.biCompression);

   AM_MEDIA_TYPE *pMt = (AM_MEDIA_TYPE *)streamConfigData_.pAmMediaType;


   if (SUCCEEDED(hr))
   {
      Make4Bit(streamConfigData_.pAmMediaType, pAmMediaType, streamConfigData_.cbAmMediaType);
      Make4Bit(streamConfigData_.pVideoHeader, pVideoHeader, streamConfigData_.cbVideoHeader);
   }
   else
   {
      throw WdmException(_T("Could not retrieve stream config!"));
   }
}


// ---------------------------------------- //

HRESULT WdmCapturer::DestroyWdmCapturer()
{
#ifdef _DEBUGFILE
   DebugMsg("DestroyWdmCapturer()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr = S_OK;

   if (isPreviewing_)
      hr = StopPreview();
   if (isGraphBuilt_)
      hr = ReleaseFilterGraph();

   return hr;
}

//
// Initialize the Filter Graph: Create a filter graph and a
// capture graph builder; further instanciate a source filter
// for the capture device.
//
HRESULT WdmCapturer::InitFilterGraph()
{
#ifdef _DEBUGFILE
   DebugMsg("InitFilterGraph()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;
   debugPosition_ = 0;

   hr = ::CoCreateInstance(CLSID_FilterGraph, 
                           NULL, 
                           CLSCTX_ALL, 
                           IID_IGraphBuilder, 
                           (void **) &pGraph_);

#ifdef _DEBUG
   if (SUCCEEDED(hr))
   {
      hr = AddToRot(pGraph_, &g_dwRegister);
   }
#endif

   if (SUCCEEDED(hr) && pGraph_)
   {
      debugPosition_ = 10;
      hr = ::CoCreateInstance(CLSID_CaptureGraphBuilder2,
                              NULL,
                              CLSCTX_ALL,
                              IID_ICaptureGraphBuilder2,
                              (void **) &pBuilder_);
   }

   if (SUCCEEDED(hr) && pBuilder_)
   {
      debugPosition_ = 20;

      hr = InitCaptureSource(szDeviceName_, &pSrc_);
   }

   if (SUCCEEDED(hr) && pBuilder_)
   {
      debugPosition_ = 30;

      hr = pBuilder_->SetFiltergraph(pGraph_);
   }


   bool srcAdded = false;
   if (SUCCEEDED(hr) && pSrc_)
   {
      debugPosition_ = 30;

      hr = pGraph_->AddFilter(pSrc_, L"Source");
   }


   if (SUCCEEDED(hr))
   {
      debugPosition_ = 40;

      srcAdded = true;
      hr = RetrieveVideoSettings();
   }

   if (SUCCEEDED(hr))
   {
      debugPosition_ = 50;

      FindVideoInterfaces();
   }

   if (FAILED(hr))
   {
      if (srcAdded)
         pGraph_->RemoveFilter(pSrc_);

      if (pSrc_)
         pSrc_->Release();
      pSrc_ = NULL;

      if (pBuilder_)
         pBuilder_->Release();
      pBuilder_ = NULL;

      if (pGraph_)
         pGraph_->Release();
      pGraph_ = NULL;
   }

   return hr;
}

//
// Checks for available interfaces on the video stream
// and/or other settings.
//
HRESULT WdmCapturer::FindVideoInterfaces()
{
#ifdef _DEBUGFILE
   DebugMsg("FindVideoInterfaces()\n");
#endif
   HRESULT hr;

   if (pInterfaces_)
      delete ((std::vector<WDM_INTERFACE_ID> *) pInterfaces_);
   pInterfaces_ = (void *) new std::vector<WDM_INTERFACE_ID>;
   INTERFACE_LIST.clear();

   // Video capture filter itself.
   {
      bool hasPages;
      HAS_PROPERTIES(pSrc_);
      if (hasPages)
      {
         // AppendSetting(ID_CAPTUREFILTER, _T("IBaseFilter"));
         INTERFACE_LIST.push_back(ID_CAPTUREFILTER);
      }
   }

   // The analog video decoder. Probably the same as
   // the video capture filter.
   IAMAnalogVideoDecoder *pAMAVD = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMAnalogVideoDecoder,
      (void **) &pAMAVD);
   if (pAMAVD)
   {
      bool hasPages = false;
      HAS_PROPERTIES(pAMAVD);
      if (hasPages)
      {
         //AppendSetting(ID_ANALOGVIDEODECODER, _T("IAMAnalogVideoDecoder"));
         INTERFACE_LIST.push_back(ID_ANALOGVIDEODECODER);
      }
      pAMAVD->Release();
      pAMAVD = NULL;
   }

   // Do we have a DV stream? In that case, add our custom DV
   // configuration dialog, too:
   if (isDvSource_)
   {
      INTERFACE_LIST.push_back(ID_DV_DECODER);
   }

   // Stream config: Size/Frame rate.
   IAMStreamConfig *pAMSC = NULL;
   hr = pBuilder_->FindInterface(&PIN_CATEGORY_CAPTURE, 
                                &MEDIATYPE_Video,
                                pSrc_,
                                IID_IAMStreamConfig,
                                (void **) &pAMSC);
   if (pAMSC)
   {
      bool hasPages;
      HAS_PROPERTIES(pAMSC);
      if (hasPages)
      {
         //AppendSetting(ID_STREAMCONFIG, _T("IAMStreamConfig"));
         INTERFACE_LIST.push_back(ID_STREAMCONFIG);
      }
      pAMSC->Release();
      pAMSC = NULL;
   }

   // The video crossbar: Selecting input device,
   // e.g. SVHS or Composite
   IAMCrossbar *pAMCB = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMCrossbar,
      (void **) &pAMCB);
   if (pAMCB)
   {
      bool hasPages = false;
      HAS_PROPERTIES(pAMCB);
      if (hasPages)
         INTERFACE_LIST.push_back(ID_CROSSBAR);
         // AppendSetting(ID_CROSSBAR, _T("IAMCrossBar"));
      pAMCB->Release();
      pAMCB = NULL;
   }

   IAMTVTuner *pAMTVT = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMTVTuner,
      (void **) &pAMTVT);
   if (SUCCEEDED(hr) && pAMTVT)
   {
      bool hasPages = false;
      HAS_PROPERTIES(pAMTVT);
      if (hasPages)
         INTERFACE_LIST.push_back(ID_TVTUNER);
         // AppendSetting(ID_TVTUNER, _T("IAMTVTuner"));
      pAMTVT->Release();
      pAMTVT = NULL;
   }

   IAMVfwCaptureDialogs *pAMVfw = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMVfwCaptureDialogs,
      (void **) &pAMVfw);
   if (SUCCEEDED(hr) && pAMVfw)
   {
      isVfwSource_ = true;
      if (pAMVfw->HasDialog(VfwCaptureDialog_Source) == S_OK)
         INTERFACE_LIST.push_back(ID_VFW_SOURCE);
      if (pAMVfw->HasDialog(VfwCaptureDialog_Format) == S_OK)
         INTERFACE_LIST.push_back(ID_VFW_FORMAT);
      if (pAMVfw->HasDialog(VfwCaptureDialog_Display) == S_OK)
         INTERFACE_LIST.push_back(ID_VFW_DISPLAY);

      pAMVfw->Release();
      pAMVfw = NULL;
   }

   return S_OK;
}

//
// Release the filter graph, the capture graph builder
// and the capture source filter.
//
HRESULT WdmCapturer::ReleaseFilterGraph()
{
#ifdef _DEBUGFILE
   DebugMsg("ReleaseFilterGraph()\n");
#endif
   ASSERT_THREAD();

   if (!isGraphBuilt_)
      return S_FALSE;

   HRESULT hr;
   ULONG rc;

   if (pSrc_ && pGraph_)
   {
      hr = pGraph_->RemoveFilter(pSrc_);

      pSrc_->Release();
      pSrc_ = NULL;
   }

   if (pBuilder_)
      rc = pBuilder_->Release();
   pBuilder_ = NULL;

#ifdef _DEBUG
   RemoveFromRot(g_dwRegister);
#endif

   if (pGraph_)
      rc = pGraph_->Release();
   pGraph_ = NULL;

   isGraphBuilt_ = false;

   return hr;
}


HRESULT WdmCapturer::InitCaptureSource(_TCHAR *tszDeviceName, IBaseFilter **pSrc)
{
#ifdef _DEBUGFILE
   DebugMsg("InitCaptureSource()\n");
#endif
   ASSERT_THREAD();

   ULONG rc;

   HRESULT         hr          = E_FAIL;

   IBaseFilter    *pFilter     = NULL;
   ICreateDevEnum *pSysDevEnum = NULL;
   IEnumMoniker   *pEnumCat    = NULL;
   
   // Create the System Device Enumerator.
   hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, 
                           NULL, 
                           CLSCTX_ALL, 
                           IID_ICreateDevEnum, 
                           (void **) &pSysDevEnum);

   if (SUCCEEDED(hr) && pSysDevEnum)
   {
      // Obtain a class enumerator for the video input device category.
      hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, 
                                              &pEnumCat, 
                                              0);
   }

   // Enumerate the monikers.
   IMoniker *pMoniker;
   ULONG     cFetched;

   int       nCount = 0;
   bool      foundDevice = false;

   if (SUCCEEDED(hr) && pEnumCat)
   {

      while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
      {
         IPropertyBag *pProp;
         pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pProp);
         VARIANT varName;
         VariantInit(&varName); // Try to match the friendly name.
         hr = pProp->Read(L"FriendlyName", &varName, 0); 
         if (SUCCEEDED(hr))
         {
            //_TCHAR tszThisDevice[256];
            //_tcsncpy(tszThisDevice, (_TCHAR *) _bstr_t(varName), 256);
            if (_tcsncmp((_TCHAR *)_bstr_t(varName), tszDeviceName, 128) == 0)
            {
               // This HRESULT result is returned:
               hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pFilter);
//               if (SUCCEEDED(hr))
//                  pFilter->AddRef();
               rc = pProp->Release();
               pProp = NULL;
               break;
            }
         }
         VariantClear(&varName);
         rc = pProp->Release();
         pProp = NULL;
         rc = pMoniker->Release();
         pMoniker = NULL; // Release for the next loop.
      }

      if (pMoniker)
         rc = pMoniker->Release();
      pMoniker = NULL;

      if (pEnumCat)
         rc = pEnumCat->Release();
      pEnumCat = NULL;

      if (pSysDevEnum)
         rc = pSysDevEnum->Release();
      pSysDevEnum = NULL;

      *pSrc = pFilter;
   // (*pSrc)->AddRef();  // Add ref on the way out. // No! Why?
   }

   if (pFilter == NULL)
   {
      // The device was not found, return a matching
      // error code.
      hr = VFW_E_NO_CAPTURE_HARDWARE;
   }

   return hr;
}

HRESULT WdmCapturer::CopyPreviewSettings()
{
#ifdef _DEBUGFILE
   DebugMsg("CopyPreviewSettings()\n");
#endif
   ASSERT_THREAD();

   return S_OK;
   // Commented out, not needed at the moment.
/*
   HRESULT hr;

   if (pSrc_ == NULL || pBuilder_ == NULL)
      return E_INVALIDARG;

   IAMStreamConfig *pAMSCPreview;
   IAMStreamConfig *pAMSCCapture;

   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_PREVIEW, 
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMStreamConfig,
      (void **) &pAMSCPreview);
   if (SUCCEEDED(hr) && pAMSCPreview)
   {
      hr = pBuilder_->FindInterface(
         &PIN_CATEGORY_CAPTURE, 
         &MEDIATYPE_Video,
         pSrc_,
         IID_IAMStreamConfig,
         (void **) &pAMSCCapture);
      if (SUCCEEDED(hr) && pAMSCCapture)
      {
         AM_MEDIA_TYPE *pMT = NULL;

         hr = pAMSCCapture->GetFormat(&pMT);
         if (SUCCEEDED(hr))
         {
            hr = pAMSCPreview->SetFormat(pMT);
            if (FAILED(hr))
            {
//               ::MessageBox(
//                  hMonitor_, "Could not copy capture to preview format.",
//                  "Information", MB_OK);
            }
            
            ::DeleteMediaType(pMT);
            pMT = NULL;
         }

         pAMSCCapture->Release();
         pAMSCCapture = NULL;
      }

      pAMSCPreview->Release();
      pAMSCPreview = NULL;
   }
   else
   {
//      ::MessageBox(hMonitor_, _T("Note: This device does not have PREVIEW settings."),
//                   _T("Information"), MB_OK | MB_ICONINFORMATION);
   }

   return hr;
*/
}

HRESULT WdmCapturer::CopyBitmapInfoHeader(AM_MEDIA_TYPE *pMt)
{
#ifdef _DEBUGFILE
   DebugMsg("CopyBitmapInfoHeader()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr = E_FAIL;

   if (pMt->majortype == MEDIATYPE_Video)
   {
//      ::MessageBox(hMonitor_, "MEDIATYPE_Video", "Information", MB_OK);

      if (pMt->subtype == MEDIASUBTYPE_dvsd)
      {
//         ::MessageBox(hMonitor_, "DV Data (MEDIASUBTYPE_dvsd)", _T("Information"), MB_OK);
         isDvSource_ = true;
      }
      else
      {
         isDvSource_ = false;
      }
      
      bool copiedBMI = false;

      if (!isDvSource_ || (isDvSource_ && !convertDvData_))
      {
         if (pMt->formattype == FORMAT_MPEGVideo ||
             pMt->formattype == FORMAT_VideoInfo)
         {
            // Starts with a VIDEOINFOHEADER
            VIDEOINFOHEADER *pVIH =
               reinterpret_cast<VIDEOINFOHEADER *> (pMt->pbFormat);

            double frameRate = 10000000.0 / pVIH->AvgTimePerFrame;
            VideoCapturer::setFrameRate(frameRate);
         
            // Copy the BITMAPINFOHEADER
            bmiVideoStream_.bmiHeader = pVIH->bmiHeader;
            copiedBMI = true;
         }
         else if (pMt->formattype == FORMAT_MPEG2Video ||
                  pMt->formattype == FORMAT_VideoInfo2)
         {
            // Starts with a VIDEOINFOHEADER2
            VIDEOINFOHEADER2 *pVIH2 =
               reinterpret_cast<VIDEOINFOHEADER2 *> (pMt->pbFormat);
            
            double frameRate = 10000000.0 / pVIH2->AvgTimePerFrame;
            VideoCapturer::setFrameRate(frameRate);

            // Copy the BITMAPINFOHEADER
            bmiVideoStream_.bmiHeader = pVIH2->bmiHeader;
            copiedBMI = true;
         }
      }
      else
      {
         // We have DV data and we want to convert it to RGB
         // Build a DV capture graph
         hr = BuildDVCaptureGraph();
         // By side-effect, the media type has been set
         // But also isDvSource_, set to true again
         isDvSource_ = true;

         if (SUCCEEDED(hr))
         {
            hr = DestroyDVCaptureGraph();
            copiedBMI = true;
         }
      }
      
      if (copiedBMI)
      {
         if (videoFormat_)
            free(videoFormat_);

         // NOTE: This line may look verrry naughty, but is necessary:
         // In the VideoCapturer, videoFormat_ always contains a BITMAPINFO
         // structure that is given by the capture device. In case that
         // such a BITMAPINFO structure does not contain a palette
         // (biClrImportant == 0), then the BITMAPINFO structure only consists
         // of the BITMAPINFOHEADER member; the palette entry is not present.
         // Thus: The structure (which should be a BITMAPINFO structure) has
         // the size of a BITMAPINFOHEADER structure. In our case, we never
         // have a palette (minimum 16 Bit), so we need a BITMAPINFOHEADER
         // sized videoFormat_.
         // This kind of code can be found in the ScreenCapturer also.
         // Let's hope we do not have a palette.
         videoFormat_            = (BITMAPINFO *) malloc(sizeof BITMAPINFOHEADER);
         videoFormat_->bmiHeader = bmiVideoStream_.bmiHeader;
         videoFormatSize_        = sizeof BITMAPINFOHEADER;
         // Bug 5293: If RGB24 is used there are a lot of codecs are available.
         // In "BuildCaptureGraph" conversion to RGB24 is added, so the video
         // header has to be adapted.
         videoFormat_->bmiHeader.biBitCount = 24;
         videoFormat_->bmiHeader.biCompression = BI_RGB;
         videoFormat_->bmiHeader.biSizeImage = videoFormat_->bmiHeader.biWidth * videoFormat_->bmiHeader.biHeight * videoFormat_->bmiHeader.biBitCount;

         //_RPT1(_CRT_WARN, "CopyBitmapInfoHeader() comp %d\n", bmiVideoStream_.bmiHeader.biCompression);


         hr = S_OK;
      }
   }
   else
   {
//      ::MessageBox(hMonitor_, "Invalid MEDIATYPE", "Error", MB_OK);
      hr = E_INVALIDARG;
   }

   return hr;
}

HRESULT WdmCapturer::RetrieveVideoSettings()
{
#ifdef _DEBUGFILE
   DebugMsg("RetrieveVideoSettings()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;

   if (pSrc_ == NULL)
      return E_INVALIDARG;

   IAMStreamConfig *pAMSC;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE, 
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMStreamConfig,
      (void **) &pAMSC);
   if (SUCCEEDED(hr) && pAMSC)
   {
      AM_MEDIA_TYPE *pMT;
      hr = pAMSC->GetFormat(&pMT);

      if (SUCCEEDED(hr))
      {
         hr = CopyBitmapInfoHeader(pMT);
         
         ::DeleteMediaType(pMT);
      }
      else
      {
//         ::MessageBox(hMonitor_, "Could not retrieve video format.", "Error", MB_OK);
      }

      pAMSC->Release();
      pAMSC = NULL;
   }

//   TCHAR msg[512];
//   _stprintf(msg, _T("Video size: %d x %d, %d bits per pixel."),
//             bmihVideoStream_.biWidth, 
//             bmihVideoStream_.biHeight,
//             bmihVideoStream_.biBitCount);
//   ::MessageBox(hMonitor_, msg, "Information", MB_OK);

   return hr;
}

HRESULT WdmCapturer::SetWdmFrameRate(REFERENCE_TIME *pAvgTimePerFrame)
{
#ifdef _DEBUGFILE
   DebugMsg("SetWdmFrameRate()\n");
#endif
   HRESULT hr = E_FAIL;

   if (isPreviewing_ || isCapturing_)
      return E_FAIL;

   IAMStreamConfig *pAMSC = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE, 
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMStreamConfig,
      (void **) &pAMSC);
   if (SUCCEEDED(hr) && pAMSC)
   {
      AM_MEDIA_TYPE *pMT = NULL;
      hr = pAMSC->GetFormat(&pMT);

      if (SUCCEEDED(hr) && pMT)
      {
         if (pMT->formattype == FORMAT_MPEGVideo ||
             pMT->formattype == FORMAT_VideoInfo ||
             pMT->formattype == FORMAT_VideoInfo2)
         {
            // Starts with a VIDEOINFOHEADER(2)
            VIDEOINFOHEADER *pVIH =
               reinterpret_cast<VIDEOINFOHEADER *> (pMT->pbFormat);

            // Set the desired frame rate
            pVIH->AvgTimePerFrame = *pAvgTimePerFrame;

            hr = pAMSC->SetFormat(pMT);
            if (SUCCEEDED(hr))
            {
               double frameRate = 10000000.0 / pVIH->AvgTimePerFrame;
               VideoCapturer::setFrameRate(frameRate);
            }
         }

         ::DeleteMediaType(pMT);
         pMT = NULL;
      }

      pAMSC->Release();
      pAMSC = NULL;
   }

   return hr;
}

HRESULT WdmCapturer::StartPreview()
{
#ifdef _DEBUGFILE
   DebugMsg("StartPreview()\n");
#endif
   ASSERT_THREAD();

   if (isPreviewing_)
      return S_FALSE;

   HRESULT hr;

   ::ShowWindow(hMonitor_, SW_SHOW);

   hr = BuildPreviewGraph();

   if (SUCCEEDED(hr))
      isPreviewing_ = true;

   return hr;
}

HRESULT WdmCapturer::StopPreview()
{
#ifdef _DEBUGFILE
   DebugMsg("StopPreview()\n");
#endif
   ASSERT_THREAD();

   if (!isPreviewing_)
      return S_FALSE;

   HRESULT hr;

   hr = StopPreviewGraph();

   if (SUCCEEDED(hr))
      isPreviewing_ = false;

   ::ShowWindow(hMonitor_, SW_HIDE);

   return hr;
}

HRESULT WdmCapturer::FindReferenceClock()
{
#ifdef _DEBUGFILE
   DebugMsg("FindReferenceClock()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;

   pRefClock_ = NULL;

   // Retrieve a IMediaFilter interface for the filter graph manager
   IMediaFilter *pMF = NULL;
   hr = pGraph_->QueryInterface(IID_IMediaFilter, (void **) &pMF);
   if (SUCCEEDED(hr) && pMF)
   {
      // Do we have a reference clock?
      hr = pMF->GetSyncSource(&pRefClock_);

//      if (SUCCEEDED(hr) && pRefClock_)
//      {
//         ::MessageBox(hMonitor_, "Found a reference clock!", "Info", MB_OK);
//         REFERENCE_TIME time;
//         HRESULT hr3 = pRefClock_->GetTime(&time);
//         if (SUCCEEDED(hr3))
//            dwFirstTime_ = (DWORD) (time / 10000i64);
//         dwFirstRealTime_ = timeGetTime();
//      }
//      else
//      {
//         TCHAR t[256];
//         _stprintf(t, _T("IReferenceClock not found: hr == %08x"), hr);
//         ::MessageBox(NULL, t, NULL, MB_OK);
//      }

      pMF->Release();
      pMF = NULL;
   }

   return hr;
}

HRESULT WdmCapturer::SetDvParameters()
{
#ifdef _DEBUGFILE
   DebugMsg("SetDvParameters()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;

   IIPDVDec *pDVDec;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IIPDVDec, 
      (void **) &pDVDec);

   if (SUCCEEDED(hr) && pDVDec)
   {
      hr = pDVDec->put_IPDisplay(dvResolution_);
      if (FAILED(hr))
      {
         ::MessageBox(NULL, _T("Could not specify DV resolution!"), _T("Error"), MB_OK | MB_TOPMOST);
      }
      pDVDec->Release();
      pDVDec = NULL;
   }

   return hr;
}

HRESULT WdmCapturer::BuildPreviewGraph()
{
#ifdef _DEBUGFILE
   DebugMsg("BuildPreviewGraph()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;

   hr = CopyPreviewSettings();
   hr = RetrieveVideoSettings();

   hr = pBuilder_->RenderStream(
      &PIN_CATEGORY_PREVIEW,
      &MEDIATYPE_Video,
      pSrc_,
      NULL,
      NULL);

   if (isDvSource_)
   {
      SetDvParameters();
   }

   IMediaControl *pControl = NULL;
   hr = pGraph_->QueryInterface(IID_IMediaControl, (void **) &pControl);

   if (SUCCEEDED(hr))
   {
      // Try to get the IVideoWindow from the Filter Graph:
      hr = pGraph_->QueryInterface(IID_IVideoWindow, (void **) &pVideo_);
      if (SUCCEEDED(hr))
      {
         pVideo_->put_Owner((OAHWND) hMonitor_);
         pVideo_->put_WindowStyle(WS_CHILD);

         long lMinIdealX = 320;
         long lMinIdealY = 240;

//         IBasicVideo *pBV = NULL;
//         HRESULT hr3 = pVideo_->QueryInterface(IID_IBasicVideo, (void **) &pBV);
//         if (SUCCEEDED(hr3) && pBV)
//         {
//            hr3 = pBV->get_VideoWidth(&lMinIdealX);
//            hr3 = pBV->get_VideoHeight(&lMinIdealY);
//            pBV->Release();
//            pBV = NULL;
//         }
         lMinIdealX = bmiVideoStream_.bmiHeader.biWidth;
         lMinIdealY = bmiVideoStream_.bmiHeader.biHeight;

         RECT cRect, wRect;
         ::GetClientRect(hMonitor_, &cRect);
         ::GetClientRect(hMonitor_, &wRect);
         int bw = wRect.right - wRect.left - (cRect.right - cRect.left);
         int bh = wRect.bottom - wRect.top - (cRect.bottom - cRect.top);

         ::SetWindowPos(hMonitor_, NULL, 0, 0, lMinIdealX + bw, lMinIdealY + bh, 
                        SWP_NOMOVE | SWP_NOZORDER);

         pVideo_->SetWindowPosition(0, 0, lMinIdealX, lMinIdealY);
         pVideo_->put_Visible(OATRUE);
      }
      
      pControl->Run();
      pControl->Release();
      pControl = NULL;
   }

   if (FAILED(hr))
   {
      ::MessageBox(hMonitor_, _T("Preview failed."), _T("Error"), MB_OK | MB_TOPMOST);
      TearDownGraph(pGraph_, pSrc_);
   }

   return hr;
}

HRESULT WdmCapturer::StopPreviewGraph()
{
#ifdef _DEBUGFILE
   DebugMsg("StopPreviewGraph()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;

   //::KillTimer(hMonitor_, 0);

   IMediaControl *pControl = NULL;
   hr = pGraph_->QueryInterface(IID_IMediaControl, (void **) &pControl);

   if (SUCCEEDED(hr))
   {
      if (pRefClock_)
         pRefClock_->Release();
      pRefClock_ = NULL;

      pControl->Stop();
      pControl->Release();
      pControl = NULL;

      pVideo_->put_Visible(OAFALSE);
      pVideo_->put_Owner(NULL);
      pVideo_->put_WindowStyle(0);

      TearDownGraph(pGraph_, pSrc_);

      pVideo_->Release();
      pVideo_ = NULL;
   }
   else
   {
      ::MessageBox(hMonitor_, _T("Could not stop preview!"), _T("Error"), MB_OK | MB_TOPMOST);
   }

   return hr;
}

HRESULT WdmCapturer::StartCapture()
{
#ifdef _DEBUGFILE
   DebugMsg("StartCapture()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr = E_FAIL;
   debugPosition_ = 1000;

   if (isPreviewing_)
      hr = StopPreview();

   IMediaControl *pMC = NULL;

   hr = BuildCaptureGraph();

   if (SUCCEEDED(hr))
   {
      debugPosition_ = 1010;

      hr = pGraph_->QueryInterface(IID_IMediaControl, (void **) &pMC);
   }

   if (SUCCEEDED(hr) && pMC)
   {
      debugPosition_ = 1020;

      hr = pMC->Run();
      pMC->Release();
      pMC = NULL;
   }

   if (SUCCEEDED(hr))
   {
      debugPosition_ = 1030;

      isCapturing_ = true;
      captureInProgress_ = true; // for thumbnails!
      FindReferenceClock();
   }
   else
   {
      DestroyCaptureGraph();
   }

   return hr;
}

HRESULT WdmCapturer::StopCapture()
{
#ifdef _DEBUGFILE
   DebugMsg("StopCapture()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr = E_FAIL;

   IMediaControl *pMC = NULL;

   hr = pGraph_->QueryInterface(IID_IMediaControl, (void **) &pMC);

   if (SUCCEEDED(hr) && pMC)
   {
      hr = pMC->Stop();
      pMC->Release();
      pMC = NULL;
   }

   if (SUCCEEDED(hr))
   {
      isCapturing_ = false;
      captureInProgress_ = false;
      DestroyCaptureGraph();
   }

   return hr;
}

HRESULT WdmCapturer::BuildCaptureGraph()
{
#ifdef _DEBUGFILE
   DebugMsg("BuildCaptureGraph()\n");
#endif
   ASSERT_THREAD();

   if (isDvSource_ && convertDvData_)
      return BuildDVCaptureGraph();

   HRESULT hr = E_FAIL;

   ISampleGrabber *pSG         = NULL;
   IBaseFilter    *pSGFilter   = NULL;
   IBaseFilter    *pNullFilter = NULL;

   debugPosition_ = 1001;

   hr = ::CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_ALL,
                           IID_ISampleGrabber, (void **) &pSG);

   if (SUCCEEDED(hr) && pSG)
   {
      // Specify desired RGB format for sample grabber here:
      AM_MEDIA_TYPE mt;
      ZeroMemory(&mt, sizeof AM_MEDIA_TYPE);
      mt.majortype  = MEDIATYPE_Video;
      mt.subtype    = MEDIASUBTYPE_RGB24;
      mt.formattype = GUID_NULL;

      hr = pSG->SetMediaType(&mt);
   }

   if (SUCCEEDED(hr) && pSG)
   {
      debugPosition_ = 1002;
      hr = pSG->QueryInterface(IID_IBaseFilter, (void **) &pSGFilter);
   }

   if (SUCCEEDED(hr))
   {
      debugPosition_ = 1003;
      // Set a "Sample callback" (vs. "Buffer Callback")
      hr = pSG->SetCallback((ISampleGrabberCB *) pSampleCallback_, 0);
   }

   if (SUCCEEDED(hr) && pSGFilter)
   {
      debugPosition_ = 1004;
      hr = pGraph_->AddFilter(pSGFilter, L"Sample Grabber");
   }

   if (SUCCEEDED(hr))
   {
      debugPosition_ = 1005;
      // Connect the source filter with the sample grabber
      hr = pBuilder_->RenderStream(
         &PIN_CATEGORY_CAPTURE,
         &MEDIATYPE_Video,
         pSrc_,
         NULL,
         pSGFilter);
   }

   if (SUCCEEDED(hr))
   {
      debugPosition_ = 1006;
      hr = ::CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_ALL,
                              IID_IBaseFilter, (void **) &pNullFilter);
   }

   if (SUCCEEDED(hr) && pNullFilter)
   {
      debugPosition_ = 1007;
      hr = pGraph_->AddFilter(pNullFilter, L"Null Renderer");
   }

   if (SUCCEEDED(hr))
   {
      debugPosition_ = 1008;
      hr = pBuilder_->RenderStream(
         NULL,
         &MEDIATYPE_Video,
         pSGFilter,
         NULL,
         pNullFilter);
   }

   if (FAILED(hr))
   {
      TearDownGraph(pGraph_, pSrc_);
   }

   ULONG rc = 0;

   // We don't need these interfaces anymore: Either they have
   // been added to the filter graph, or building the capture
   // graph did not succeed. In any case: We don't need the
   // references anymore.
   if (pNullFilter)
      rc = pNullFilter->Release();
   pNullFilter = NULL;

   if (pSGFilter)
      rc = pSGFilter->Release();
   pSGFilter = NULL;

   if (pSG)
      rc = pSG->Release();
   pSG = NULL;

   return hr;
}

HRESULT WdmCapturer::DestroyCaptureGraph()
{
#ifdef _DEBUGFILE
   DebugMsg("DestroyCaptureGraph()\n");
#endif
   ASSERT_THREAD();

   if (isDvSource_ && convertDvData_)
      return DestroyDVCaptureGraph();

   HRESULT hr = S_OK;

   if (pRefClock_)
      pRefClock_->Release();
   pRefClock_ = NULL;

   // It's enough to just tear down the graph.
   // We'll leave the source filter in the graph
   // for later.
   TearDownGraph(pGraph_, pSrc_);

   return hr;
}

HRESULT WdmCapturer::BuildDVCaptureGraph()
{
#ifdef _DEBUGFILE
   DebugMsg("BuildDVCaptureGraph()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr = E_FAIL;

   // DV Decoder Filter
   IBaseFilter    *pDVDec      = NULL;

   // Sample grabber filters
   ISampleGrabber *pSG         = NULL;
   IBaseFilter    *pSGFilter   = NULL;
   IBaseFilter    *pNullFilter = NULL;
   
   hr = ::CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_ALL,
                           IID_ISampleGrabber, (void **) &pSG);

   if (SUCCEEDED(hr) && pSG)
   {
      hr = ::CoCreateInstance(CLSID_DVVideoCodec, NULL, CLSCTX_ALL, 
                              IID_IBaseFilter, (void **) &pDVDec);
   }


   if (SUCCEEDED(hr) && pDVDec)
   {
      hr = pGraph_->AddFilter(pDVDec, L"DV Video Decoder");
   }

   if (SUCCEEDED(hr))
   {
      hr = pBuilder_->RenderStream(0, &MEDIATYPE_Video, pSrc_, NULL, pDVDec);
   }

   if (SUCCEEDED(hr))
   {
      // Set the DV parameters (decoder resolution):
      hr = SetDvParameters();
   }

   if (SUCCEEDED(hr) && pSG)
   {
      // Specify desired RGB format for sample grabber here:
      AM_MEDIA_TYPE mt;
      ZeroMemory(&mt, sizeof AM_MEDIA_TYPE);
      mt.majortype  = MEDIATYPE_Video;
      mt.subtype    = MEDIASUBTYPE_RGB24;
      mt.formattype = GUID_NULL;

      hr = pSG->SetMediaType(&mt);
   }

   if (SUCCEEDED(hr) && pSG)
   {
      hr = pSG->QueryInterface(IID_IBaseFilter, (void **) &pSGFilter);
   }

   if (SUCCEEDED(hr))
   {
      // Set a "Sample callback" (vs. "Buffer Callback")
      hr = pSG->SetCallback((ISampleGrabberCB *) pSampleCallback_, 0);
      // hr = pSG->SetCallback((ISampleGrabberCB *) pSampleCallback_, 1);
   }

   if (SUCCEEDED(hr) && pSGFilter)
   {
      hr = pGraph_->AddFilter(pSGFilter, L"Sample Grabber");
   }

   if (SUCCEEDED(hr))
   {
      hr = pBuilder_->RenderStream(
         NULL,
         &MEDIATYPE_Video,
         pDVDec,
         NULL,
         pSGFilter);
   }

   if (SUCCEEDED(hr))
   {
      // We have successfully connected the DV filter
      // to the sample grabber. What's the media type?
      AM_MEDIA_TYPE mt;
      hr = pSG->GetConnectedMediaType(&mt);
      if (SUCCEEDED(hr))
      {
         // Note: By side-effect, this call sets "isDvSource_" to
         // false, so we need to recover that setting after the
         // call; this method only applies to DV sources.
         hr = CopyBitmapInfoHeader(&mt);
         isDvSource_ = true;

         ::FreeMediaType(mt);
      }
   }

   if (SUCCEEDED(hr))
   {
      hr = ::CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_ALL,
                              IID_IBaseFilter, (void **) &pNullFilter);
   }

   if (SUCCEEDED(hr) && pNullFilter)
   {
      hr = pGraph_->AddFilter(pNullFilter, L"Null Renderer");
   }

   if (SUCCEEDED(hr))
   {
      hr = pBuilder_->RenderStream(
         NULL,
         &MEDIATYPE_Video,
         pSGFilter,
         NULL,
         pNullFilter);
   }

   // We don't need the NullFilter pointers anymore:
   if (pNullFilter)
      pNullFilter->Release();
   pNullFilter = NULL;

   // Neither the SG IBaseFilter:
   if (pSGFilter)
      pSGFilter->Release();
   pSGFilter = NULL;

   // Nor any of the other ones:
   if (pSG)
      pSG->Release();
   pSG = NULL;
   if (pDVDec)
      pDVDec->Release();
   pDVDec = NULL;

   if (FAILED(hr))
   {
      DestroyDVCaptureGraph();
   }

   return hr;
}

HRESULT WdmCapturer::DestroyDVCaptureGraph()
{
#ifdef _DEBUGFILE
   DebugMsg("DestroyDVCaptureGraph()\n");
#endif
   ASSERT_THREAD();

   if (pRefClock_)
      pRefClock_->Release();
   pRefClock_ = NULL;

   TearDownGraph(pGraph_, pSrc_);

   return S_OK;
}

void WdmCapturer::HandleSample(double sampleTime, IMediaSample *pSample)
{
#ifdef _DEBUGFILE
   DebugMsg("HandleSample()\n");
#endif
   static VIDEOHDR  videoHdr;
   static BYTE     *pBuffer;
   static HRESULT   hr;

   hr = pSample->GetPointer(&pBuffer);
   videoHdr.lpData         = reinterpret_cast<unsigned char *> (pBuffer);
   videoHdr.dwBufferLength = pSample->GetSize();
   videoHdr.dwBytesUsed    = pSample->GetActualDataLength();
   videoHdr.dwTimeCaptured = (DWORD) (1000.0 * sampleTime);
   videoHdr.dwUser         = 0;
   videoHdr.dwFlags        = 0;
   if (pSample->IsSyncPoint() == S_OK)
      videoHdr.dwFlags    |= VHDR_KEYFRAME;

   controlCallback(hMonitor_, CONTROLCALLBACK_CAPTURING);
   videoStreamCallback(hMonitor_, &videoHdr);
}

// Tears down a graph downstream from a base filter (source
// filter). The base filter itself is not removed from the
// the filter graph.
void WdmCapturer::TearDownGraph(IFilterGraph *pFg, IBaseFilter *pFilter)
{
#ifdef _DEBUGFILE
   DebugMsg("TearDownGraph()\n");
#endif
   ASSERT_THREAD();

   if (pFilter == NULL)
      return;

   IEnumPins *pEnumPins;
   pFilter->EnumPins(&pEnumPins);
   pEnumPins->Reset();

   IPin *pPin, *pTo;
   ULONG cFetched;
   HRESULT hr;
   while (pEnumPins->Next(1, &pPin, &cFetched) == S_OK)
   {
      if (pPin)
      {
         hr = pPin->ConnectedTo(&pTo);
         if (SUCCEEDED(hr) && pTo)
         {
            PIN_INFO pinInfo;
            hr = pTo->QueryPinInfo(&pinInfo);
            if (SUCCEEDED(hr))
            {
               if(pinInfo.dir == PINDIR_INPUT)
               {
                  TearDownGraph(pFg, pinInfo.pFilter);
                  pFg->Disconnect(pPin);
                  pFg->Disconnect(pTo);
                  pFg->RemoveFilter(pinInfo.pFilter);
               }
               // why here?!
               pinInfo.pFilter->Release();
            }
            pTo->Release();
         }
         pPin->Release();
      }
   }

   if (pEnumPins)
      pEnumPins->Release();
}

bool WdmCapturer::DisplayConfigurationDialog(WDM_INTERFACE_ID widId, HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayConfigurationDialog()\n");
#endif
   if (hWnd == NULL)
      hWnd = hMonitor_;
   bool wasPreviewing = isPreviewing_;
   if (isPreviewing_)
      displayMonitor(false);
   LRESULT res = ::SendMessage(hMonitor_, WM_COMMAND, (WPARAM) hWnd, (LPARAM) widId);
   ::SendMessage(hMonitor_, WM_WDM_READ_VIDEOSETTINGS, 0, 0);

   if (wasPreviewing)
      displayMonitor(true);

   return (res == TRUE);
}

void WdmCapturer::SetConvertDvToRgb(bool bRgb)
{
#ifdef _DEBUGFILE
   DebugMsg("SetConvertDvToRgb()\n");
#endif
   convertDvData_ = bRgb;
}

bool WdmCapturer::GetConvertDvToRgb()
{
#ifdef _DEBUGFILE
   DebugMsg("GetConvertDvToRgb()\n");
#endif
   return convertDvData_;
}

void WdmCapturer::SetDvResolution(DVRES dvRes)
{
#ifdef _DEBUGFILE
   DebugMsg("SetDvResolution()\n");
#endif
   dvResolution_ = (_DVRESOLUTION) dvRes;
}

DVRES WdmCapturer::GetDvResolution()
{
#ifdef _DEBUGFILE
   DebugMsg("GetDvResolution()\n");
#endif
   return (DVRES) dvResolution_;
}

void WdmCapturer::SetDefaultDvParameters(DVRES dvRes, bool bRgb)
{
#ifdef _DEBUGFILE
   DebugMsg("SetDefaultDvParameters()\n");
#endif
   dvResolutionDefault_  = (_DVRESOLUTION) dvRes;
   convertDvDataDefault_ = bRgb;
}

bool WdmCapturer::DisplayDvDecoderDialog(HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayDvDecoderDialog()\n");
#endif
   ASSERT_THREAD();

   if (hWnd == NULL)
      hWnd = hMonitor_;

   DVDECODERPARAMS dvParams;
   dvParams.bConvertToRgb = convertDvData_;
   dvParams.dvRes         = dvResolution_;

   int res = ::DialogBoxParam(g_hDllInstance, MAKEINTRESOURCE(IDD_DV_DECODER), 
                              hWnd, DvDialogProc, (LPARAM) (void *) &dvParams);

   if (res == TRUE)
   {
      bool wasPreviewing = false;
      if (isPreviewing_)
      {
         StopPreview();
         wasPreviewing = true;
      }

      dvResolution_  = dvParams.dvRes;
      convertDvData_ = dvParams.bConvertToRgb;

      if (wasPreviewing)
         StartPreview();

      return true;
   }

   return false;
}

BOOL CALLBACK WdmCapturer::DvDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUGFILE
//   DebugMsg("DvDialogProc()\n");
#endif
   static DVDECODERPARAMS *pDvParams = NULL;

   switch (msg)
   {
   case WM_INITDIALOG:
      // Initialize the dialog
      pDvParams = (DVDECODERPARAMS *) lParam;
      ::SetDlgItemText(hWnd, IDC_DV_CONVERT, Local::localizeString(DVDEC_CONVERT));
      ::SetDlgItemText(hWnd, IDC_DV_RESOLUTION, Local::localizeString(DVDEC_RESOLUTION));
      ::SetDlgItemText(hWnd, IDC_DV_FULL, Local::localizeString(DVDEC_FULL));
      ::SetDlgItemText(hWnd, IDC_DV_HALF, Local::localizeString(DVDEC_HALF));
      ::SetDlgItemText(hWnd, IDC_DV_QUARTER, Local::localizeString(DVDEC_QUARTER));
      ::SetDlgItemText(hWnd, IDC_DV_STAMP, Local::localizeString(DVDEC_DC));
      ::CheckDlgButton(hWnd, IDC_DV_CONVERT, 
                       pDvParams->bConvertToRgb ? BST_CHECKED : BST_UNCHECKED);
      switch (pDvParams->dvRes)
      {
      case DVRESOLUTION_FULL:
         ::CheckDlgButton(hWnd, IDC_DV_FULL, BST_CHECKED);
         break;
      case DVRESOLUTION_QUARTER:
         ::CheckDlgButton(hWnd, IDC_DV_QUARTER, BST_CHECKED);
         break;
      case DVRESOLUTION_DC:
         ::CheckDlgButton(hWnd, IDC_DV_STAMP, BST_CHECKED);
         break;
      default:
      case DVRESOLUTION_HALF:
         ::CheckDlgButton(hWnd, IDC_DV_HALF, BST_CHECKED);
         break;
      }
      return TRUE;

   case WM_CLOSE:
      // Close works like "Cancel"
      ::EndDialog(hWnd, FALSE);
      return TRUE;

   case WM_COMMAND:
      {
         HWND hwndControl = (HWND) lParam;
         if (hwndControl == ::GetDlgItem(hWnd, IDOK))
         {
            if (::IsDlgButtonChecked(hWnd, IDC_DV_FULL))
               pDvParams->dvRes = DVRESOLUTION_FULL;
            else if (::IsDlgButtonChecked(hWnd, IDC_DV_HALF))
               pDvParams->dvRes = DVRESOLUTION_HALF;
            else if (::IsDlgButtonChecked(hWnd, IDC_DV_QUARTER))
               pDvParams->dvRes = DVRESOLUTION_QUARTER;
            else
               pDvParams->dvRes = DVRESOLUTION_DC;

            pDvParams->bConvertToRgb = (::IsDlgButtonChecked(hWnd, IDC_DV_CONVERT) == TRUE);

            ::EndDialog(hWnd, TRUE);
            return TRUE;
         }
         else if (hwndControl == ::GetDlgItem(hWnd, IDCANCEL))
         {
            ::EndDialog(hWnd, FALSE);
            return TRUE;
         }
      }
      break;
   }

   return FALSE;
}

// Configuration Dialogs
HRESULT WdmCapturer::ShowPropertyPages(IUnknown *pFilter, HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("ShowPropertyPages()\n");
#endif
   ASSERT_THREAD();

   ISpecifyPropertyPages *pProp;
   HRESULT hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
   if (SUCCEEDED(hr)) 
   {
      // Show the page. 
      CAUUID caGUID;
      pProp->GetPages(&caGUID);
      pProp->Release();
      hr = OleCreatePropertyFrame(
         hWnd,                   // Parent window
         0, 0,                   // (Reserved)
         NULL,     // Caption for the dialog box
         1,                      // Number of objects (just the filter)
         &pFilter,            // Array of object pointers. 
         caGUID.cElems,          // Number of property pages
         caGUID.pElems,          // Array of property page CLSIDs
         0,                      // Locale identifier
         0, NULL                 // Reserved
         );
      
      // Clean up.
      CoTaskMemFree(caGUID.pElems);
   }
   return hr;
}

bool WdmCapturer::DisplayStreamConfigDialog(HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayStreamConfigDialog()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;
   IAMStreamConfig *pAMSC;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE, 
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMStreamConfig,
      (void **) &pAMSC);
   if (SUCCEEDED(hr) && pAMSC)
   {
      hr = ShowPropertyPages(pAMSC, hWnd);

      pAMSC->Release();
      pAMSC = NULL;
   }

   return (SUCCEEDED(hr) == TRUE);
}

bool WdmCapturer::DisplayAnalogVideoDecoderDialog(HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayAnalogVideoDecoderDialog()\n");
#endif
   HRESULT hr;
   IAMAnalogVideoDecoder *pAMAVD;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMAnalogVideoDecoder,
      (void **) &pAMAVD);
   if (SUCCEEDED(hr) && pAMAVD)
   {
      hr = ShowPropertyPages(pAMAVD, hWnd);

      pAMAVD->Release();
      pAMAVD = NULL;
   }

   return (SUCCEEDED(hr) == TRUE);
}

bool WdmCapturer::DisplayCrossbarDialog(HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayCrossbarDialog()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;
   IAMCrossbar *pAMCB = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMCrossbar,
      (void **) &pAMCB);
   if (SUCCEEDED(hr) && pAMCB)
   {
      hr = ShowPropertyPages(pAMCB, hWnd);
      pAMCB->Release();
      pAMCB = NULL;
   }

   return (SUCCEEDED(hr) == TRUE);
}

bool WdmCapturer::DisplayTvTunerDialog(HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayTvTunerDialog()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;
   IAMTVTuner *pAMTVT = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMTVTuner,
      (void **) &pAMTVT);
   if (SUCCEEDED(hr) && pAMTVT)
   {
      hr = ShowPropertyPages(pAMTVT, hWnd);
      pAMTVT->Release();
      pAMTVT = NULL;
   }

   return (SUCCEEDED(hr) == TRUE);
}

bool WdmCapturer::DisplayCaptureFilterDialog(HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayCaptureFilterDialog()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr = ShowPropertyPages(pSrc_, hWnd);

   return (SUCCEEDED(hr) == TRUE);
}

bool WdmCapturer::DisplayVfwDisplayDialog(HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayVfwDisplayDialog()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;

   IAMVfwCaptureDialogs *pAMVfw = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMVfwCaptureDialogs,
      (void **) &pAMVfw);
   if (SUCCEEDED(hr) && pAMVfw)
   {
      if (pAMVfw->HasDialog(VfwCaptureDialog_Source) == S_OK)
         pAMVfw->ShowDialog(VfwCaptureDialog_Source, hWnd);

      pAMVfw->Release();
      pAMVfw = NULL;
   }

   return false;
}

bool WdmCapturer::DisplayVfwFormatDialog(HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayVfwFormatDialog()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;

   IAMVfwCaptureDialogs *pAMVfw = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMVfwCaptureDialogs,
      (void **) &pAMVfw);
   if (SUCCEEDED(hr) && pAMVfw)
   {
      if (pAMVfw->HasDialog(VfwCaptureDialog_Format) == S_OK)
      {
         pAMVfw->ShowDialog(VfwCaptureDialog_Format, hWnd);

      }

      pAMVfw->Release();
      pAMVfw = NULL;
   }

   return false;
}

bool WdmCapturer::DisplayVfwSourceDialog(HWND hWnd)
{
#ifdef _DEBUGFILE
   DebugMsg("DisplayVfwSourceDialog()\n");
#endif
   ASSERT_THREAD();

   HRESULT hr;

   IAMVfwCaptureDialogs *pAMVfw = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMVfwCaptureDialogs,
      (void **) &pAMVfw);
   if (SUCCEEDED(hr) && pAMVfw)
   {
      if (pAMVfw->HasDialog(VfwCaptureDialog_Display) == S_OK)
         pAMVfw->ShowDialog(VfwCaptureDialog_Display, hWnd);

      pAMVfw->Release();
      pAMVfw = NULL;
   }

   return false;
}

HRESULT WdmCapturer::InitWdmStreamConfigData()
{
   ASSERT_THREAD();

   HRESULT hr = E_FAIL;

   if (streamConfigData_.pAmMediaType)
      delete[] streamConfigData_.pAmMediaType;
   streamConfigData_.pAmMediaType = NULL;
   streamConfigData_.cbAmMediaType = 0;
   if (streamConfigData_.pVideoHeader)
      delete[] streamConfigData_.pVideoHeader;
   streamConfigData_.pVideoHeader = NULL;
   streamConfigData_.cbVideoHeader = 0;

   if (isPreviewing_ || isCapturing_)
      return E_FAIL;

   IAMStreamConfig *pAMSC = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE, 
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMStreamConfig,
      (void **) &pAMSC);
   if (SUCCEEDED(hr) && pAMSC)
   {
      AM_MEDIA_TYPE *pMT = NULL;
      hr = pAMSC->GetFormat(&pMT);

      if (SUCCEEDED(hr) && pMT)
      {
         streamConfigData_.cbAmMediaType = sizeof AM_MEDIA_TYPE;
         streamConfigData_.pAmMediaType = new char[sizeof AM_MEDIA_TYPE];
         memcpy(streamConfigData_.pAmMediaType, (const void *) pMT, sizeof AM_MEDIA_TYPE);
         
         streamConfigData_.cbVideoHeader = pMT->cbFormat;
         if (pMT->cbFormat > 0)
         {
            streamConfigData_.pVideoHeader = new char[pMT->cbFormat];
            memcpy(streamConfigData_.pVideoHeader, (const void *) pMT->pbFormat, pMT->cbFormat);
         }
         else
         {
            streamConfigData_.pVideoHeader = NULL;
         }

         streamConfigDataValid_ = true;

         ::DeleteMediaType(pMT);
      }

      pAMSC->Release();
      pAMSC = NULL;
   }

   return hr;
}

HRESULT WdmCapturer::SetWdmDefaultParameters()
{
   HRESULT hr = S_OK;

   // If we have a DV source, we know the parameters
   // are quite ok. We don't have to do anything for
   // that. If we have a VfW source, setting parameters
   // does not have any impact; that is done in the 
   // VfW format dialog.
   if (!IsDvSource() && !IsVfwSource())
   {
      IAMStreamConfig *pAMSC = NULL;
      hr = pBuilder_->FindInterface(
         &PIN_CATEGORY_CAPTURE, 
         &MEDIATYPE_Video,
         pSrc_,
         IID_IAMStreamConfig,
         (void **) &pAMSC);
      if (SUCCEEDED(hr) && pAMSC)
      {
         AM_MEDIA_TYPE mtOutFormat;
         ZeroMemory(&mtOutFormat, sizeof AM_MEDIA_TYPE);
		 // nBestMatch will contain the score of the
		 // best video stream configuration. The score
		 // is defined by us, according to our preferences.
         int nBestMatch = -1;

         int iCount = 0;
         int iSize  = 0;
         hr = pAMSC->GetNumberOfCapabilities(&iCount, &iSize);
         VIDEO_STREAM_CONFIG_CAPS *pSCC = NULL;
         if (SUCCEEDED(hr))
         {
            pSCC = (VIDEO_STREAM_CONFIG_CAPS *) (new BYTE[iSize]);

            for (int i=0; i<iCount && SUCCEEDED(hr); ++i)
            {
               AM_MEDIA_TYPE *pMt = NULL;

               hr = pAMSC->GetStreamCaps(i, &pMt, (BYTE *) pSCC);
               if (SUCCEEDED(hr))
               {
                  // Check major type and format type.
                  // Has to be normal video.
                  if (pMt->majortype == MEDIATYPE_Video &&
                     (pMt->formattype == FORMAT_VideoInfo ||
                      pMt->formattype == FORMAT_VideoInfo2))
                  {
                     // Check what we got here
                     int nMatch = -1;

                     // Is it an RGB format?
                     if (pMt->subtype == MEDIASUBTYPE_RGB555)
                        nMatch += 3; // RGB555 is good
                     else if (pMt->subtype == MEDIASUBTYPE_RGB24)
                        nMatch += 4; // RGB24 is better
                     else if (pMt->subtype == MEDIASUBTYPE_RGB32)
                        nMatch += 2; // RGB32 is not bad either
                     else if (pMt->subtype == MEDIASUBTYPE_YUY2)
                        nMatch += 1; // at least better than nothing (ie XVID supports this)
                     else if (pMt->subtype == MEDIASUBTYPE_UYVY)
                        nMatch += 1; // at least better than nothing (ie XVID supports this)

                     
                     // Now check the frame rate.
                     REFERENCE_TIME avgTimePerFrame = 
                        ((VIDEOINFOHEADER *) pMt->pbFormat)->AvgTimePerFrame;
                     if (avgTimePerFrame >= 399000i64 && avgTimePerFrame < 401000i64)
                        nMatch += 3; // 25 fps is really good
                     else if (avgTimePerFrame >= 333000i64 && avgTimePerFrame < 334000i64)
                        nMatch += 3; // 30 fps is not bad either
                     else if (avgTimePerFrame >= 666000i64 && avgTimePerFrame < 668000i64)
                        nMatch += 1; // 15 fps is acceptable, too

                     BITMAPINFOHEADER *pBH = NULL;
                     if (pMt->formattype == FORMAT_VideoInfo)
                        pBH = &((VIDEOINFOHEADER *) pMt->pbFormat)->bmiHeader;
                     else
                        pBH = &((VIDEOINFOHEADER2 *) pMt->pbFormat)->bmiHeader;

                     int nW = abs(pBH->biWidth);
                     int nH = abs(pBH->biHeight);
                     if (nW == 320 && nH == 240)
                        nMatch += 3; // Prefer 320x240
                     else if (nW == 384 && nH == 288)
                        nMatch += 2; // 1/4 PAL
                     else if (nW == 352 && nH == 288)
                        nMatch += 2; // CIF
                     else if (nW == 240 && nH == 180)
                        nMatch += 2; // 240x180 is good, too
                     else if (nW == 160 && nH == 120)
                        nMatch += 1; // 160x120 is acceptable, not more
                     else if (nW == 176 && nH == 144)
                        nMatch += 1; // QCIF
                     else if (nW == 192 && nH == 144)
                        nMatch += 1; // 1/16 PAL

                     //_RPT4(_CRT_WARN, "%d: %dx%d comp %d", i, nW, nH, pBH->biCompression);
                     //_RPT2(_CRT_WARN, " bits %d match %d\n", pBH->biBitCount, nMatch);

                     if (nMatch > nBestMatch)
                     {
                        // Did we have a previous match?
                        if (nBestMatch != -1)
                           FreeMediaType(mtOutFormat);
                        
                        CopyMediaType(&mtOutFormat, pMt);
                        nBestMatch = nMatch;
                     }
                  }

                  // Free the media type
                  DeleteMediaType(pMt);
                  pMt = NULL;
               }
            }

            // Now, did we have a good match?
            if (nBestMatch >= 0)
            {
               hr = pAMSC->SetFormat(&mtOutFormat);
               FreeMediaType(mtOutFormat);
            }	
			// else use the camera default
         }

         if (pSCC)
            delete[] ((BYTE *) pSCC);
         pSCC = NULL;
         pAMSC->Release();
         pAMSC = NULL;
      }
   }

   RetrieveVideoSettings();

   return hr;
}

HRESULT WdmCapturer::SetWdmStreamConfigData()
{
   ASSERT_THREAD();

   HRESULT hr;

   if (isPreviewing_ || isCapturing_)
   {
      ::MessageBox(NULL, _T("isPreviewing || isCapturing == true"), NULL, MB_OK | MB_TOPMOST);
      return E_FAIL;
   }
#ifdef _DEBUGFILE
   DebugMsg("SetWdmStreamConfigData()\n");
#endif
   IAMStreamConfig *pAMSC = NULL;
   hr = pBuilder_->FindInterface(
      &PIN_CATEGORY_CAPTURE, 
      &MEDIATYPE_Video,
      pSrc_,
      IID_IAMStreamConfig,
      (void **) &pAMSC);
   if (SUCCEEDED(hr) && pAMSC)
   {
#ifdef _DEBUGFILE
      DebugMsg("  - Found IAMStreamConfig interface\n");
#endif
      AM_MEDIA_TYPE *pMT = (AM_MEDIA_TYPE *) streamConfigData_.pAmMediaType;
      pMT->cbFormat = streamConfigData_.cbVideoHeader;
      pMT->pbFormat = (BYTE *) streamConfigData_.pVideoHeader;

      VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)streamConfigData_.pVideoHeader;

      //_RPT1(_CRT_WARN, "Setting format to device: comp %d\n", pVih->bmiHeader.biCompression);

      hr = pAMSC->SetFormat(pMT);
#ifdef _DEBUGFILE
      if (SUCCEEDED(hr))
      {
         DebugMsg("  - Setting format succeeded.\n");
      }
      else
      {
         char t[256];
         sprintf(t, "  - Setting format failed: 0x%08x\n", hr);
         DebugMsg(t);
      }
#endif
      pAMSC->Release();
      pAMSC = NULL;
   }
#ifdef _DEBUGFILE
   else
   {
      DebugMsg("  - Could not find IAMStreamConfig interface\n");
   }
#endif

   // If we were able to set the stream format successfully,
   // then try to retrieve the format again so that the
   // application knows the actual stream settings, too.
   if (SUCCEEDED(hr))
   {
      hr = RetrieveVideoSettings();
   }

   return hr;
}

HRESULT WdmCapturer::GetWdmStreamConfigData()
{
   ASSERT_THREAD();

   if (!streamConfigDataValid_)
      InitWdmStreamConfigData();
   if (!streamConfigDataValid_)
      return E_FAIL;

   return S_OK;
}

HRESULT WdmCapturer::GetWdmStreamConfigMediaTypeSize(DWORD *dwSize)
{
   ASSERT_THREAD();

   if (!streamConfigDataValid_)
      InitWdmStreamConfigData();
   if (!streamConfigDataValid_)
      return E_FAIL;

   *dwSize = (DWORD) streamConfigData_.cbAmMediaType;

   return S_OK;
}

HRESULT WdmCapturer::GetWdmStreamConfigVideoHeaderSize(DWORD *dwSize)
{
   ASSERT_THREAD();

   if (!streamConfigDataValid_)
      InitWdmStreamConfigData();
   if (!streamConfigDataValid_)
      return E_FAIL;

   *dwSize = (DWORD) streamConfigData_.cbVideoHeader;

   return S_OK;
}

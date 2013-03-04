#include "stdafx.h"
#include "avedit.h"
#include "avinfo.h" // DoSomeTests()
#include "wavdest.h" // ILadMux
#include "../lresizer/lresizer.h"
#include "lutils.h" // LIo::GetDiskSpace
#include "LanguageSupport.h"

#include <shlobj.h>
void DebugMsg(_TCHAR *tszMessage)
{
   /*
   _TCHAR tszLogPath[MAX_PATH];
   HRESULT hr = ::SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, tszLogPath);
   if (SUCCEEDED(hr) && hr != S_FALSE)
   {
      _tcscat(tszLogPath, _T("\\editor.log.txt"));

      FILE *f = _tfopen(tszLogPath, _T("a+"));
      if (f != NULL && tszMessage != NULL)
      {
         _ftprintf(f, tszMessage);
         fflush(f);
         fclose(f);
      }
   }
   */
}

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
   // Aufrufe von DllMain sind asynchron zu anderen 
   // Threads und haben nichts mit dem Aufruf von Exports
   // der Dll zu tun (scheinen zufällig).


   //static int s_nInstanceCounter = 0;
   // das geht so nicht so einfach
   // (3x Attach, 1x (PROCESS) Detach)
   // Multizugriff??

   //_CrtSetBreakAlloc(44);

   switch (ul_reason_for_call)
   {
   case DLL_THREAD_ATTACH:
      //printf("Process attach: %x\n", hModule);
      break;
   case DLL_PROCESS_ATTACH:
      //printf("Thread attach: %x\n", hModule);
      
      break;
   case DLL_THREAD_DETACH:
      //printf("Thread detach: %x\n", hModule);
      
      break;
   case DLL_PROCESS_DETACH:
      //printf("Process detach: %x\n", hModule);
         
      break;
   }
   return TRUE;
}

HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    CComPtr< IMoniker > pMoniker;
    CComPtr< IRunningObjectTable > pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT)))
        return E_FAIL;
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr))
        hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
    return hr;
}

void RemoveFromRot(DWORD pdwRegister)
{
    CComPtr< IRunningObjectTable > pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT)))
        pROT->Revoke(pdwRegister);
}


void PromptForGraphInspection(IGraphBuilder *pGraph, bool addRemove)
{
#ifdef _DEBUG
     
   DWORD dwRegister;
   if (addRemove)
   {
      AddToRot(pGraph, &dwRegister);
   }


   MessageBox(NULL, _T("Inspect it!"), _T("NOW"), MB_OK);
   
   if (addRemove)
   {
      RemoveFromRot(dwRegister);
   }
#endif
}

void InspectGraph(IGraphBuilder *pGraph)
{
   HRESULT hr = S_OK;
   CComPtr< IEnumFilters > pEnum;
   hr = pGraph->EnumFilters(&pEnum);

   if (SUCCEEDED(hr))
   {
      printf("### Filter listing for %x:\n", pGraph);

      //CComPtr< IBaseFilter > pFilter;
      IBaseFilter *pFilter = NULL;
      while(pEnum->Next(1, &pFilter, NULL) == S_OK)
      {
         FILTER_INFO info;
         pFilter->QueryFilterInfo(&info);

         wprintf(L"%s\n", info.achName);

         //pFilter.Detach();
         pFilter->Release();
      }

      //std::cin.get();
   }
}


// static
HRESULT AVEdit::AddVolumeEffect(IAMTimelineObj *pObject, IAMTimeline *pTimeline, 
                                int nVolume, int nStartMs, int nEndMs)
{
   HRESULT hr = S_OK;

   if (pObject == NULL || pTimeline == NULL)
      return E_POINTER;

   if (nVolume < 0 || nStartMs < 0 || nEndMs < 0 || nEndMs < nStartMs)
      return E_INVALIDARG;

   CComPtr<IAMTimelineObj> pVolumeObj;
   if (SUCCEEDED(hr))
      hr = pTimeline->CreateEmptyNode(&pVolumeObj, TIMELINE_MAJOR_TYPE_EFFECT);
   if (SUCCEEDED(hr))
      hr = pVolumeObj->SetSubObjectGUID(CLSID_AudMixer);
   if (SUCCEEDED(hr))
   {
      hr = pVolumeObj->SetStartStop(nStartMs*10000i64, nEndMs*10000i64);
      // start and top times of effects are relative to their parent
   }
   CComPtr<IPropertySetter> pPropSetter;
   if (SUCCEEDED(hr))
      hr = ::CoCreateInstance(CLSID_PropertySetter, NULL, CLSCTX_ALL, IID_IPropertySetter, (void **) &pPropSetter);
   
   if (SUCCEEDED(hr))
      hr = pPropSetter->ClearProps();
   if (SUCCEEDED(hr))
   {
      DEXTER_PARAM dxPar;
      dxPar.Name = _bstr_t("Vol");
      dxPar.nValues = 1;
      DEXTER_VALUE dxVal;
      dxVal.v.vt = VT_R8;
      dxVal.v.dblVal = (double)nVolume / 100.0;
      dxVal.rt = 0i64;
      dxVal.dwInterp = DEXTERF_JUMP;
      
      hr = pPropSetter->AddProp(dxPar, &dxVal);
   }
   if (SUCCEEDED(hr))
      hr = pVolumeObj->SetPropertySetter(pPropSetter);
   
   CComPtr<IAMTimelineEffectable> pEffectTarget;
   if (SUCCEEDED(hr))
      hr = pObject->QueryInterface(IID_IAMTimelineEffectable, (void **)&pEffectTarget);
   if (SUCCEEDED(hr))
      hr = pEffectTarget->EffectInsBefore(pVolumeObj, 0);

   return hr;
}



AVEdit::AVEdit()
{
   // muss existieren, da es sonst beim Einbinden dieser Dll
   // "nicht aufgeloeste Symbole" gibt
   // Vulgo: C++ hat/macht keine Standardkonstruktoren?!

   ::CoInitialize(NULL);

   // TODO: Sowas wird leider nicht gefunden:
   //malloc(217);

   m_paRenderEngines = NULL;
   m_paTargets = NULL;
   m_paTimelines = NULL;
   m_paStreamControls = NULL;
   m_nNumStreamControls = 0;
   m_hWndNotify = NULL;
   m_hVideoWnd = NULL;
   m_nVideoStream = -1;
   m_isControlThreadRunning   = false;
   m_controlThreadStopRequest = false;
   m_isPreviewThreadRunning   = false;
   m_previewThreadStopRequest = false;
   m_tszDirPrefix = NULL;
   m_bIsStartFillClip = false;

   m_pLock = new CCritSec();
}

AVEdit::~AVEdit()
{
   StopControlThreads();
   ReleaseStreamControls();
   FreeResources(false);

   if (m_pLock)
      delete m_pLock;
   if (m_tszDirPrefix)
      delete[] m_tszDirPrefix;

   ::CoUninitialize();
}

/*
class CMyErrorLog : public IAMErrorLog
{
public:
   CMyErrorLog()
   {
      //
   }

   // IUnknown
   virtual ULONG STDMETHODCALLTYPE AddRef()
   {
      return 1L;
   }

   virtual ULONG STDMETHODCALLTYPE Release()
   {
      return 1L;
   }

   STDMETHODIMP QueryInterface(REFIID riid, LPVOID *pUnk)
   {
      return E_NOINTERFACE;
   }

   // IAMErrorLog

   STDMETHODIMP LogError(
       LONG Severity,          // Reserved. Do not use.
       BSTR ErrorString,       // Description.
       LONG ErrorCode,         // Error code.
       HRESULT hresult,        // HRESULT that caused the error.
       VARIANT *pExtraInfo)   // Extra information about the error.
   {
      MessageBox(NULL, _T("LogError"), _T("Info"), MB_OK);
      return S_OK;
   }
};
*/

UINT AVEdit::GetAudioPreviewLength()
{
   if (m_nNumStreamControls > 0)
   {
      LONGLONG lDuration = 0i64;
      HRESULT hr = m_paStreamControls[0]->pSeeking->GetDuration(&lDuration);
   
      if (SUCCEEDED(hr))
      {
         UINT nDurationMs = (UINT)(lDuration / 10000i64);
         if (lDuration % 10000i64 != 0)
            ++nDurationMs;

         return nDurationMs;
      }
   }

   return 0;
}

HRESULT AVEdit::InitializeTimelines(const AVTarget **paTargets, const int nNumTargets, bool bForPreview, 
                                    AVProgressListener *pListener)
{
   // falls schon etwas von einem vorherigen Aufruf vorhanden ist:
   StopControlThreads();
   ReleaseStreamControls();
   FreeResources(false);

   m_nNumTargets = nNumTargets;
   m_paTargets = new AVTarget*[m_nNumTargets];
   int i = 0;
   for (i=0; i<m_nNumTargets; ++i)
      m_paTargets[i] = paTargets[i]->DeepClone();


   HRESULT hr = S_OK;

   m_paTimelines = new IAMTimeline*[m_nNumTargets];
   for (i=0; i<m_nNumTargets; ++i) m_paTimelines[i] = NULL;

   for (i=0; i<m_nNumTargets && SUCCEEDED(hr); ++i)
   {
      hr = ::CoCreateInstance(CLSID_AMTimeline, NULL, CLSCTX_INPROC_SERVER, 
         IID_IAMTimeline, (void **) &m_paTimelines[i]);

      if (SUCCEEDED(hr))
      {
         hr = m_paTargets[i]->AddTo(m_paTimelines[i], bForPreview);
         if (m_paTargets[i]->IsStartFillClip())
             m_bIsStartFillClip = true;
      }
   }

   return hr;
}

HRESULT AVEdit::InitializeExport(const _TCHAR *tszDirPrefix, HWND hWndNotify)
{
   _ASSERT(tszDirPrefix);
   _ASSERT(m_paTimelines);

   m_bCancelRequested = false;

   if (m_tszDirPrefix)
      delete[] m_tszDirPrefix;
   m_tszDirPrefix = _tcsdup(tszDirPrefix);
   
   if (m_paRenderEngines)
      FreeResources(true);

   m_hWndNotify = hWndNotify;

   HRESULT hr = CreateRenderEngines(false);
   
   for (int i=0; i<m_nNumTargets && SUCCEEDED(hr); ++i)
   {
      CComPtr< IGraphBuilder > pGraph;
      
      if (SUCCEEDED(hr))
      {
         hr = m_paRenderEngines[i]->GetFilterGraph(&pGraph);
      }
      
      
      CComPtr< ICaptureGraphBuilder2 > pBuilder;
      
      if (SUCCEEDED(hr))
      {
         hr = ::CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
            IID_ICaptureGraphBuilder2, (void **) &pBuilder);
      }
      
      if (SUCCEEDED(hr))
      {
         hr = pBuilder->SetFiltergraph(pGraph);
      }
      
#ifdef _DEBUG
//      DWORD dwRegister;
//      AddToRot(pGraph, &dwRegister);
#endif
      
      if (SUCCEEDED(hr))
      {
         // TODO: is the group number obsolete -> AviMux ?
         int nGroupNumber = 0;
         hr = m_paTargets[i]->SetFile(
            m_paRenderEngines[i], pBuilder, nGroupNumber, tszDirPrefix);
      }

            
      //PromptForGraphInspection(pGraph, false);

      // InspectGraph(pGraph);

      
#ifdef _DEBUG
//      RemoveFromRot(dwRegister);
#endif

  }

   return hr;
}


HRESULT AVEdit::InitializePreview(//const HWND **paWindows, const nNumWindows,
                                  const HWND hVideoWnd,
                                  AVProgressListener *pListener, HWND hWndNotify)
                                  // Wird hier eigentlich noch ein Listener gebraucht?
                                  // Ja, denn StartPreview() muss nicht aufgerufen werden
                                  // (zB nur JumpPreview()).
{
   _ASSERT(m_paTimelines);

   m_hWndNotify = hWndNotify;
   m_hVideoWnd  = hVideoWnd;
   
   if (m_paRenderEngines)
      FreeResources(true);
   
   HRESULT hr = CreateRenderEngines(true);
   
   for(int i=0; i<m_nNumTargets && SUCCEEDED(hr); ++i)
   {
      if (SUCCEEDED(hr))
      {
         hr = m_paRenderEngines[i]->RenderOutputPins( );
      }
   }

   return hr;

}


HRESULT AVEdit::StartExport(AVProgressListener *pListener)
{
   _ASSERT(m_paRenderEngines);

   HRESULT hr = S_OK;

   for(int i=0; i<m_nNumTargets && SUCCEEDED(hr) && !m_bCancelRequested; ++i)
   {
      if (m_hWndNotify)
      {
         if (m_paTargets[i]->m_pFormatDefinition->majortype == MEDIATYPE_Video)
         {
            ::PostMessage(m_hWndNotify, WM_AVEDIT, AVEDIT_NEXTVIDEO, 0);
         }
      }

      CComPtr< IGraphBuilder > pGraph;
      
      if (SUCCEEDED(hr))
      {
         hr = m_paRenderEngines[i]->GetFilterGraph(&pGraph);
      }

      CComQIPtr<IMediaFilter> pFilter(pGraph);

      if (!pGraph || !pFilter)
         hr = VFW_E_NO_INTERFACE;

      if (SUCCEEDED(hr))
         hr = pFilter->SetSyncSource(NULL);

      CComQIPtr<IMediaControl> pControl(pGraph);
      CComQIPtr<IMediaEvent> pEvent(pGraph);
      CComQIPtr<IMediaSeeking> pSeeking(pGraph);
      CComPtr<ILadMux> pLadMux;

      if (!pEvent || !pSeeking || !pControl)
         hr = VFW_E_NO_INTERFACE;

      CComQIPtr<IFilterGraph> pFilterGraph(pGraph);
      if (SUCCEEDED(hr) && pFilterGraph != NULL)
      {
         DWORD dwCaps = AM_SEEKING_CanGetCurrentPos;
         HRESULT hrPos = pSeeking->CheckCapabilities(&dwCaps);

         if (hrPos != S_OK)
         {
            CComPtr<IBaseFilter> pWavDest;
            hrPos = pFilterGraph->FindFilterByName(L"Wav Dest", &pWavDest);
      
            if (SUCCEEDED(hrPos))
               hrPos = pWavDest->QueryInterface(IID_ILadMux, (void **)&pLadMux);
         }
      }

      //if (SUCCEEDED(hr))
      //   hr = pSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
      if (SUCCEEDED(hr))
         hr = pControl->Pause();
      if (SUCCEEDED(hr))
      {
         OAFilterState state = State_Stopped;
         hr = pControl->GetState(250, &state);
         while (SUCCEEDED(hr) && state != State_Paused);
         {
            Sleep(50);
            hr = pControl->GetState(250, &state);
         }
      }
     int inCaseOfErrorPercentage = 0;

      if (SUCCEEDED(hr))
         hr = pControl->Run();


      bool stopRequested = false;
      if (SUCCEEDED(hr))
      {
         int iDuration = m_paTargets[i]->GetMasterEnd();
         
         long evCode = 0, param1, param2;
         int  currentPercent;
         LONGLONG rtCurrent;
		 
         // rtLast and nNoProgressCounter are used to detect
         // if there isn't any progress in the export process.
         LONGLONG rtLast = 123456789i64;
         int nNoProgressCounter = 0;
         bool bNoProgressAbort = false;

         DWORD dwLoopStart = ::timeGetTime();

         while (evCode != EC_COMPLETE && evCode != EC_USERABORT && evCode != EC_ERRORABORT && !stopRequested)
         {
            HRESULT hrEvent = pEvent->GetEvent(&evCode, &param1, &param2, 250);
            if (hrEvent == E_ABORT)
               hrEvent = S_OK; // time out expired

            // Note: events are hopefully not too frequent. So normally the above timeout 
            // of 250ms will occur.
            
            // We don't care about the events, actually
            pEvent->FreeEventParams(evCode, param1, param2);


            if (SUCCEEDED(hrEvent))
            {
               hrEvent = pSeeking->GetCurrentPosition(&rtCurrent);
               // Note: This always fails for audio output.

               if (FAILED(hrEvent) && pLadMux != NULL)
                  hrEvent = pLadMux->GetCurrentPosition(&rtCurrent);
            }
            
            // So, now, where are we
            if (SUCCEEDED(hrEvent) && m_hWndNotify)
            {
               // See above, non-progress detection
               if (rtCurrent == rtLast)
                  nNoProgressCounter++;
               else
                  nNoProgressCounter = 0;
												   
               // '120' means about 30 seconds (250 ms per loop)
               // Only check if there has been any progress at all,
               // at the beginning, it may take quite a long time
               // before there is any progress.
               if (nNoProgressCounter > 120 && rtCurrent > 0i64)
               {											   
                  // No progress has occurred for a while.
                  // Abort!
                  evCode = EC_ERRORABORT;
                  bNoProgressAbort = true;
                  m_nNoProgressAbortTimeMs = (int) (rtCurrent / 10000i64);
               }

               rtLast = rtCurrent;

               currentPercent = (int) (100.0 * ((double) rtCurrent) / 10000.0 / ((double) iDuration) + .5);
               ::PostMessage(m_hWndNotify, WM_AVEDIT, AVEDIT_EXPORTPROGRESS, currentPercent);
            }
            else if (FAILED(hrEvent) && m_hWndNotify)
            {
#ifdef _DEBUG
               //if (inCaseOfErrorPercentage == 0)
               //   OutputDebugString(_T("GetPositions() failed.\n"));
#endif

               inCaseOfErrorPercentage += 1;
               if (inCaseOfErrorPercentage > 100)
                  inCaseOfErrorPercentage = 100;
               ::PostMessage(m_hWndNotify, WM_AVEDIT, AVEDIT_EXPORTPROGRESS, inCaseOfErrorPercentage);
            }

            if (m_bCancelRequested && !stopRequested)
            {
               pControl->Stop();
               stopRequested = true;
            }
         }
			
         // The export process has stopped and/or finished,
         // check for errors.
         if (!stopRequested && evCode != EC_COMPLETE)
         {
            bool bFoundError = false;

            if (!bFoundError && bNoProgressAbort)
            {
               // No progress was detected and the conversion
               // has been cancelled due to this.
               // Set the hr value to RPC_E_TIMEOUT
               hr = RPC_E_TIMEOUT;
               bFoundError = true;
            }

            if (!bFoundError)
            {
               // Check for full disk
               __int64 i64DiskFree = LIo::GetDiskSpace(m_tszDirPrefix);
               if (i64DiskFree != -1i64)
               {
                  if (i64DiskFree < 1048576i64) // 1 MB
                  {
                     hr = E_OUTOFMEMORY;
                     bFoundError = true;
                  }
               }
            }

            if (!bFoundError)
            {
               if (pControl != NULL)
                  pControl->Stop();

               // Some error occurred, stop was not requested.
               // Set hr to "help me! something went wrong!"
               hr = E_FAIL;
            }
         }
         
         DWORD dwLoopEnd = ::timeGetTime();
      }

   }

   FreeResources(false);

//   return S_OK;
   return hr;
}

int AVEdit::GetNoProgressAbortTimeMs()
{
   return m_nNoProgressAbortTimeMs;
}

HRESULT AVEdit::AbortExport()
{
   m_bCancelRequested = true;
   return S_OK;
}

HRESULT AVEdit::ResizeVideoWindow(RECT *pRect)
{
   if (m_nVideoStream > 0)
      return FitVideoWindow(m_nVideoStream, pRect);
   return E_FAIL;
}

HRESULT AVEdit::GetPreviewVideoSize(SIZE *pSize)
{
   int nStream = -1;
   if (m_nVideoStream > 0)
      nStream = m_nVideoStream;
   else
      return E_FAIL;

   if (m_hVideoWnd == NULL || m_paRenderEngines == NULL || 
       nStream >= m_nNumTargets || pSize == NULL)
      return E_POINTER;

   CComPtr<IGraphBuilder> pGraph;
   HRESULT hr = m_paRenderEngines[nStream]->GetFilterGraph(&pGraph);

   if (FAILED(hr) || !pGraph)
      return hr;

   CComQIPtr<IBasicVideo>  pBV(pGraph);

   if (pBV && SUCCEEDED(hr))
   {
      long width = 0;
      long height = 0;

      hr = pBV->get_VideoWidth(&width);
      if (SUCCEEDED(hr))
         hr = pBV->get_VideoHeight(&height);

      if (SUCCEEDED(hr))
      {
         pSize->cx = width;
         pSize->cy = height;
      }
   }
   return hr;
}

HRESULT AVEdit::FitVideoWindow(int nStream, RECT *pRect)
{
   if (m_hVideoWnd == NULL || m_paRenderEngines == NULL || nStream >= m_nNumTargets)
      return E_POINTER;
   CComPtr<IGraphBuilder> pGraph;
   HRESULT hr = m_paRenderEngines[nStream]->GetFilterGraph(&pGraph);

   if (FAILED(hr) || !pGraph)
      return hr;

   RECT clientRect;
   ::GetClientRect(m_hVideoWnd, &clientRect);

   CComQIPtr<IVideoWindow> pVW(pGraph);
   //CComQIPtr<IBasicVideo>  pBV(pGraph);

   if (pVW/* && pBV*/)
   {
      /*
      long vw  = 0;
      long vh = 0;
      hr = pBV->get_VideoWidth(&vw);
      if (SUCCEEDED(hr))
         hr = pBV->get_VideoHeight(&vh);
      int cw = clientRect.right - clientRect.left;
      int ch = clientRect.bottom - clientRect.top;

      double xf = ((double) cw) / ((double) vw);
      double yf = ((double) ch) / ((double) vh);

      double f = xf < yf ? xf : yf;
      long width = (long) (f * vw);
      long height = (long) (f * vh);

      long left = (cw - width) / 2;
      long top = (ch - height) / 2;

      if (SUCCEEDED(hr))
         hr = pVW->SetWindowPosition(left, top, width, height);
      if (SUCCEEDED(hr))
         hr = pVW->SetWindowForeground(OATRUE);
      if (SUCCEEDED(hr) && pRect)
      {
         pRect->left = left;
         pRect->top  = top;
         pRect->right = left + width;
         pRect->bottom = top + height;
      }
      */

      if (SUCCEEDED(hr))
         hr = pVW->SetWindowPosition(clientRect.left, clientRect.top,
            clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
      if (SUCCEEDED(hr))
         hr = pVW->SetWindowForeground(OATRUE);
   }

   return hr;
}

HRESULT AVEdit::BuildPreview()
{
   // The preview should be running, but
   // checking that will not hurt
   // StopPreview();

   ASSERT(!m_isControlThreadRunning && !m_isPreviewThreadRunning);
   // StopControlThreads();

   //
   // start rendering (this may take some time)
   // TODO: make it async
   //
   HRESULT hr = S_OK;

   // Release any previously allocated stream controls
   ReleaseStreamControls();

   m_pLock->Lock();
   m_paStreamControls = new AVStreamControl*[m_nNumTargets];
   m_nNumStreamControls = m_nNumTargets;

   int i = 0;
   for(i=0; i<m_nNumTargets && SUCCEEDED(hr); ++i)
   {
      CComPtr< IGraphBuilder > pGraph;
      
      if (SUCCEEDED(hr))
      {
         hr = m_paRenderEngines[i]->GetFilterGraph(&pGraph);
      }
      
      m_paStreamControls[i] = new AVStreamControl(pGraph);
      
      // Pause the audio graph so that we can
      // retrieve the reference clock from there;
      // all other graphs must remain stopped until 
      // we have replaced their reference clocks
      // with our own clocks.
      if (SUCCEEDED(hr) && m_paStreamControls[i]->pControl && i==0)
      {
         hr = m_paStreamControls[i]->pControl->Pause();
      }
   }

   // Now use the stream clock of the first stream for
   // all other streams, too. We assume that stream number
   // one is audio.
   CSkewReferenceClock **paClocks = new CSkewReferenceClock*[m_nNumTargets - 1];
   CComPtr<IGraphBuilder> pGraph;
   if (SUCCEEDED(hr))
      hr = m_paRenderEngines[0]->GetFilterGraph(&pGraph);
   CComQIPtr<IMediaFilter> pAudioFilter(pGraph);
   if (SUCCEEDED(hr) && pAudioFilter)
   {
      CComPtr<IReferenceClock> pAudioRefClock;
      hr = pAudioFilter->GetSyncSource(&pAudioRefClock);
      if (SUCCEEDED(hr) && pAudioRefClock)
      {
         for (int i=1; i<m_nNumTargets; ++i)
         {
            CComPtr<IGraphBuilder> pTempGraph;
            hr = m_paRenderEngines[i]->GetFilterGraph(&pTempGraph);
            if (SUCCEEDED(hr) && pTempGraph)
            {
               CComQIPtr<IMediaFilter> pTempMediaFilter(pTempGraph);
               if (pTempMediaFilter)
               {
                  m_paStreamControls[i]->pClock = 
                     new CSkewReferenceClock(pAudioRefClock, 0i64);
                  hr = pTempMediaFilter->SetSyncSource(m_paStreamControls[i]->pClock);
                  ASSERT(SUCCEEDED(hr));
               }
            }
         }
      }
   }

   // Now we can unlock again; now we're sure that everything
   // has been created properly (if that is so)
   m_pLock->Unlock();

   // Do we have a video to put somewhere?
   if (SUCCEEDED(hr) && m_nNumTargets > 1 && m_hVideoWnd)
   {
      bool videoWindowFound = false;
      for (i=1; i<m_nNumTargets && !videoWindowFound && SUCCEEDED(hr); ++i)
      {
         CComPtr<IGraphBuilder> pVideoGraph;
         // hr = 
         m_paRenderEngines[i]->GetFilterGraph(&pVideoGraph);
         if (SUCCEEDED(hr) && pVideoGraph)
         {
            CComQIPtr<IVideoWindow> pVW(pVideoGraph);
            CComQIPtr<IBasicVideo>  pBV(pVideoGraph);
            if (pVW && pBV)
            {
               pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
               pVW->put_Owner((OAHWND) m_hVideoWnd);

               FitVideoWindow(i);

               videoWindowFound = true;
               m_nVideoStream = i;
            }
         }
      }

      if (!videoWindowFound)
         m_nVideoStream = -1;
   }
//   m_nVideoStream = -1;

   // Ok, now pause all other graphs
   if (SUCCEEDED(hr))
   {
      for (i=1; i<m_nNumTargets && SUCCEEDED(hr); ++i)
         hr = m_paStreamControls[i]->pControl->Pause();
   }

   LONGLONG lDuration = 0i64;
   if (SUCCEEDED(hr))
      hr = m_paStreamControls[0]->pSeeking->GetDuration(&lDuration);
   
   DWORD dwDurationMs = 0;
   if (SUCCEEDED(hr))
      dwDurationMs = (DWORD) (lDuration / 10000i64);

   //_RPT1(_CRT_WARN, "duration of 0 %d\n", dwDurationMs);

   // Now we may launch the thread which contains the event
   // loop of the graph:
   if (SUCCEEDED(hr))
      _beginthread(LaunchPreviewThread, 0, this);

   // Now the preview thread has either started or
   // it has failed. In case everything is ok, start
   // the control thread, too.
   if (SUCCEEDED(hr))
      _beginthread(LaunchControlThread, 0, this);


   // Bugfix #2206 (Absturz nach erfolgreichem Auswahl-Export)
   // Wait for the two threads to have started, otherwise 
   // this method returns to early and it might
   // happen that the StopPreview() is called (a second time) before
   // these thread have even started.
   while (!m_isControlThreadRunning || !m_isPreviewThreadRunning)
   {
      Sleep(50);
   }

   return hr;
}

HRESULT AVEdit::StartPreview(int startMs, int endMs)
{
   HRESULT hr = S_OK;

   if (SUCCEEDED(hr) && m_paStreamControls != NULL)
   {
       m_pLock->Lock();
      if (startMs != -1 && endMs != -1)
      {
         for (int i=0; i<m_nNumStreamControls && SUCCEEDED(hr); ++i)
         {
            LONGLONG rtStart = 10000i64 * startMs;
            LONGLONG rtEnd   = 10000i64 * endMs;
            hr = m_paStreamControls[i]->pSeeking->SetPositions(
               &rtStart, AM_SEEKING_AbsolutePositioning,
               &rtEnd, AM_SEEKING_AbsolutePositioning);
         }
      }
      /*
      else
      {
         for (int i=0; i<m_nNumStreamControls && SUCCEEDED(hr); ++i)
         {
            LONGLONG rtEnd = MAX_TIME;
            hr = m_paStreamControls[i]->pSeeking->SetPositions(
               NULL, AM_SEEKING_NoPositioning,
               &rtEnd, AM_SEEKING_AbsolutePositioning);
         }
      }
      */

      for (int i=0; i<m_nNumStreamControls && SUCCEEDED(hr); ++i)
      {
         hr = m_paStreamControls[i]->pControl->Run();
      }
      m_pLock->Unlock();
   }
   else
      hr = E_RENDER_ENGINE_IS_BROKEN;

   return hr;
}

void AVEdit::LaunchPreviewThread(void *pInstance)
{
   HRESULT hr;
   
   hr = CLanguageSupport::SetThreadLanguage();

   ((AVEdit *) pInstance)->PreviewThread();
}

void AVEdit::PreviewThread()
{
   m_isPreviewThreadRunning = true;

   HRESULT hr = S_OK;

   long evCode, param1, param2;
   while (!m_previewThreadStopRequest)
   {
      hr = m_paStreamControls[0]->pEvent->GetEvent(&evCode, &param1, &param2, 250);
      if (SUCCEEDED(hr))
      {
         switch(evCode) 
         { 
            // Call application-defined functions for each 
            // type of event that you want to handle.
         case EC_COMPLETE:
            PausePreview();
            if (m_hWndNotify)
            {
               
               LONGLONG currentTime = 0i64;
               hr = m_paStreamControls[0]->pSeeking->GetCurrentPosition(&currentTime);
               
               DWORD dwCurrentMs = 0;
               if (SUCCEEDED(hr))
                  dwCurrentMs = (DWORD) (currentTime / 10000i64);
               
               ::PostMessage(m_hWndNotify, WM_AVEDIT, AVEDIT_STREAM_FINISHED, dwCurrentMs);
            }
            break;
         }
         hr = m_paStreamControls[0]->pEvent->FreeEventParams(evCode, param1, param2);
      }
      else if (hr == E_ABORT)
         hr = S_OK;
   }

   m_isPreviewThreadRunning = false;
}

HRESULT AVEdit::PausePreview()
{
   HRESULT hr = S_OK;
   m_pLock->Lock();
   if (m_paStreamControls)
   {
      int i = 0;
      for (i=0; i<m_nNumStreamControls; ++i)
         hr = m_paStreamControls[i]->pControl->Pause();

      OAFilterState filterState = State_Running;
      bool allFiltersPaused = false;
      while (!allFiltersPaused)
      {
         allFiltersPaused = true;
         for (i=0; i<m_nNumStreamControls; ++i)
         {
            hr = m_paStreamControls[i]->pControl->GetState(100, &filterState);
            if (filterState != State_Paused)
               allFiltersPaused = false;
         }
         Sleep(50);
      }

      if (filterState == State_Paused)
         hr = S_OK;
   }
   else
      hr = S_FALSE;
   m_pLock->Unlock();

   return hr;
}

HRESULT AVEdit::StopPreview()
{
  
   HRESULT hr = S_OK;
   if (m_paStreamControls)
   {
      m_pLock->Lock();
      for (int i=0; i<m_nNumStreamControls; ++i)
         hr = m_paStreamControls[i]->pControl->Stop();
      m_pLock->Unlock();
      // TIME: needs 1s

      // Shut down control threads
      StopControlThreads();
      // TIME: needs 0.2s

      // Then release all stream controls
      ReleaseStreamControls();
      // TIME: needs 0.5s or 2s if Stop() was not called above

      // ... and free all resources
      FreeResources(false);
   }
   else
      hr = S_FALSE;

   return hr;
}

HRESULT AVEdit::JumpPreview(int nMillis, bool allStreams)
{
   HRESULT hr = S_OK;
   m_pLock->Lock();
   if (m_paStreamControls)
   {
      OAFilterState filterState;
      hr = m_paStreamControls[0]->pControl->GetState(INFINITE, &filterState);

      if (filterState == State_Paused)
      {
         LONGLONG position = 10000i64 * nMillis;
         LONGLONG rtEnd    = MAX_TIME;

         // We do not always want to seek in the 
         // audio stream, too.
         int startAt = allStreams ? 0 : 1;

         for (int i=startAt; i<m_nNumStreamControls && SUCCEEDED(hr); ++i)
         {
            hr = m_paStreamControls[i]->pSeeking->SetPositions(
               &position,
               AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame,
               &rtEnd,
               AM_SEEKING_AbsolutePositioning //AM_SEEKING_NoPositioning
               );
         }
      }
      else
         hr = VFW_E_WRONG_STATE;
   }
   else
      hr = S_FALSE;
   m_pLock->Unlock();

   return hr;
}

HRESULT AVEdit::CreateRenderEngines(bool bForPreview)
{
   _ASSERT(m_paTimelines);

   // TODO: Check if there are targets pointing to the same file (avi)
   m_paRenderEngines = new IRenderEngine*[m_nNumTargets];
   int i = 0;
   for (i=0; i<m_nNumTargets; ++i) m_paRenderEngines[i] = NULL;

   HRESULT hr = S_OK;
   
   for(i=0; i<m_nNumTargets && SUCCEEDED(hr); ++i)
   {
      
      if (SUCCEEDED(hr))
      {
         // SmartRenderEngine soll nicht für Preview eingesetzt werden
         // es müssen sonst die (verfrüht) komprimierten Daten wieder
         // dekomprimiert werden
         bool useSmart = !bForPreview && m_paTargets[i]->m_useSmartRecompression;

         hr = ::CoCreateInstance(
            useSmart ? CLSID_SmartRenderEngine: CLSID_RenderEngine,
            NULL, CLSCTX_INPROC_SERVER,
            IID_IRenderEngine, (void **) &m_paRenderEngines[i]);
      }
      
      if (SUCCEEDED(hr))
      {
         hr = m_paRenderEngines[i]->SetTimelineObject(m_paTimelines[i]);
      }
      
      // The && !bForPreview makes it faster, but creates wrong colors...
      if (SUCCEEDED(hr)) // && !bForPreview)
      {
         // Set our custom resizer GUID
         CComQIPtr<IRenderEngine2> pRender2(m_paRenderEngines[i]);
         if (pRender2)
         {
            hr = pRender2->SetResizerGUID(CLSID_StretchDIBitsResizer);
         }
      }
      
      if (SUCCEEDED(hr))
      {
         hr = m_paRenderEngines[i]->ConnectFrontEnd();

         // Is this video? And not preview? And should we try to use
         // smart recompression?
         if (SUCCEEDED(hr) &&
             m_paTargets[i]->m_pFormatDefinition->formattype == FORMAT_VideoInfo && 
             !bForPreview && 
             m_paTargets[i]->m_useSmartRecompression)
         {
            // Do we need a compressor?
            VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER *) m_paTargets[i]->m_pFormatDefinition->pbFormat;
            if (pVIH->bmiHeader.biCompression != BI_RGB)
            {
               // Now check if we got a compressor
               CComQIPtr<ISmartRenderEngine> pSmart(m_paRenderEngines[i]);
               if (pSmart)
               {
                  // If we're not in smart mode, that's bad
                  CComPtr<IBaseFilter> pCompressor = NULL;
                  hr = pSmart->GetGroupCompressor(0, &pCompressor);
                  if (pCompressor == NULL)
                  {
                     hr = AVInfo::StaticCreateCompressor(
                        m_paTargets[i]->m_pFormatDefinition, 
                        m_paTargets[i]->m_dwFcc,
                        &pCompressor,
                        m_paTargets[i]->m_nKeyframeRate,
                        m_paTargets[i]->m_nQuality
                        );
                     if (SUCCEEDED(hr) && pCompressor)
                        hr = pSmart->SetGroupCompressor(0, pCompressor);
                     if (SUCCEEDED(hr))
                        hr = m_paRenderEngines[i]->ConnectFrontEnd();
                     if (SUCCEEDED(hr))
                     {
                        CComPtr<IBaseFilter> pGroupC = NULL;
                        HRESULT hr2 = pSmart->GetGroupCompressor(0, &pGroupC);
                        if (FAILED(hr2) || !pGroupC)
                           hr = E_RENDER_ENGINE_IS_BROKEN;
                     }
                  }
               }
            }
         }
      }
   }
   
   return hr;
}

void AVEdit::FreeResources(bool bRenderEnginesOnly)
{
   // See if we need to release any video windows
   if (m_nVideoStream > 0 && m_hVideoWnd)
   {
      CComPtr<IGraphBuilder> pGraph;
      HRESULT hr = m_paRenderEngines[m_nVideoStream]->GetFilterGraph(&pGraph);
      if (SUCCEEDED(hr) && pGraph)
      {
         CComQIPtr<IVideoWindow> pVW(pGraph);
         if (pVW)
         {
            hr = pVW->put_Visible(OAFALSE);
            if (SUCCEEDED(hr))
               hr = pVW->put_Owner(NULL);
            if (SUCCEEDED(hr))
               hr = pVW->put_WindowStyle(0);
         }
      }

      m_nVideoStream = -1;
      m_hVideoWnd    = NULL;
   }

   if (!bRenderEnginesOnly)
   {
      if (m_paTargets)
      {
         for (int i=0; i<m_nNumTargets; ++i)
            delete m_paTargets[i];
         delete[] m_paTargets;
         m_paTargets = NULL;
      }
      
      
      if (m_paTimelines)
      {
         for (int i=0; i<m_nNumTargets; ++i)
         {  
            if (m_paTimelines[i])
               m_paTimelines[i]->Release();
         }
         delete[] m_paTimelines;
         m_paTimelines = NULL;
      }
   }
   
   if (m_paRenderEngines)
   {
      for (int i=0; i<m_nNumTargets; ++i)
      {
         if (m_paRenderEngines[i])
            m_paRenderEngines[i]->Release();
      }
      delete[] m_paRenderEngines;
      m_paRenderEngines = NULL;
   }

   // TODO: resetting different (other) variables??
   m_bIsStartFillClip = false;
}

void AVEdit::ReleaseStreamControls()
{
   if (m_paStreamControls)
   {
      for (int i=0; i<m_nNumStreamControls; ++i)
         delete m_paStreamControls[i];
      delete[] m_paStreamControls;
      m_paStreamControls = NULL;
      m_nNumStreamControls = 0;
   }
}

void AVEdit::LaunchControlThread(void *pInstance)
{
   HRESULT hr;
   
   hr = CLanguageSupport::SetThreadLanguage();

   ((AVEdit *) pInstance)->ControlThread();
}

void AVEdit::ControlThread()
{
   m_isControlThreadRunning = true;
//   MessageBeep(0);

   LONGLONG currentTime = 0i64;
   OAFilterState filterState = State_Stopped;
   HRESULT hr = S_OK;
   int loopCounter = 0;

   LONGLONG audioTime;
   LONGLONG otherTime;
   LONGLONG deviationTime;

   while (!m_controlThreadStopRequest)
   {
      Sleep(100);

      ++loopCounter;

      if (!m_controlThreadStopRequest)
      {
         m_pLock->Lock();
         if (m_paStreamControls && m_hWndNotify)
         {
            /* Replaced by a loop/timer in CMainFrame and the method GetPreviewPosition() here.

            hr = m_paStreamControls[0]->pControl->GetState(100, &filterState);
            if (SUCCEEDED(hr) && filterState == State_Running)
            {
               hr = m_paStreamControls[0]->pSeeking->GetCurrentPosition(&currentTime);
               if (SUCCEEDED(hr))
                  ::PostMessage(m_hWndNotify, WM_AVEDIT, AVEDIT_STREAMPOSITION, 
                     (DWORD) (currentTime / 10000i64));
            }
            */

            if (loopCounter % 10 == 0)
            {
               hr = m_paStreamControls[0]->pSeeking->GetPositions(&audioTime, NULL);
               if (SUCCEEDED(hr))
               {
                  for (int i=1; i<m_nNumStreamControls; ++i)
                  {
                     hr = m_paStreamControls[i]->pSeeking->GetPositions(&otherTime, NULL);
                     if (SUCCEEDED(hr))
                     {
                        deviationTime = otherTime - audioTime;
                        m_paStreamControls[i]->pClock->SetSkew(deviationTime);
#ifdef _DEBUG
//                        char t[64];
//                        sprintf(t, "New Skew #%d: %dms\n", i, (int) (deviationTime / 10000i64));
//                        DbgOutString(t);
#endif
                     }
                  }
               }
            }
         } // if (m_paStreamControls)
         m_pLock->Unlock();
      }
   }

   m_isControlThreadRunning = false;
}

void AVEdit::StopControlThreads()
{
   if (m_isControlThreadRunning || m_isPreviewThreadRunning)
   {
      m_controlThreadStopRequest = true;
      m_previewThreadStopRequest = true;
      while (m_isControlThreadRunning || m_isPreviewThreadRunning)
      {
         Sleep(50);
      }

      ASSERT(!m_isControlThreadRunning && !m_isPreviewThreadRunning);

      m_controlThreadStopRequest = false;
      m_previewThreadStopRequest = false;
   }
}

bool AVEdit::IsPreviewPaused()
{
   if (m_paStreamControls)
   {
      OAFilterState filterState;
      HRESULT hr = S_OK;

      hr = m_paStreamControls[0]->pControl->GetState(INFINITE, &filterState);

      if (SUCCEEDED(hr))
      {
         return (filterState == State_Paused);
      }
   }

   return false;
}

bool AVEdit::IsPreviewRunning()
{
   if (m_paStreamControls)
   {
      OAFilterState filterState;
      HRESULT hr = S_OK;

      hr = m_paStreamControls[0]->pControl->GetState(INFINITE, &filterState);

      if (SUCCEEDED(hr))
      {
         return (filterState == State_Running);
      }
   }

   return false;
}

UINT AVEdit::GetPreviewPosition()
{
   UINT nPositionMs = 0;

   // This method may not block! It blocks the message loop then.
   // Locking is also not necessary for simple query methods.
   //m_pLock->Lock();

   if (m_paStreamControls != NULL)
   {
      HRESULT hr = S_OK;

      if (SUCCEEDED(hr))
      {
         LONGLONG currentTime = 0i64;
         hr = m_paStreamControls[0]->pSeeking->GetCurrentPosition(&currentTime);

         if (SUCCEEDED(hr))
            nPositionMs = (UINT)(currentTime / 10000i64);
      }
   }

   // See above
   //m_pLock->Unlock();        

   return nPositionMs;
}
   
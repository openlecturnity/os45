#include "stdafx.h"
#include "DsHelper.h"
#include "streams.h" // ie CMediaType

#include <initguid.h> // otherwise linker errors
#include "ilameaudioprops.h"

DEFINE_GUID(CLSID_LameMp3,
            0xb8d27088, 0xff5f, 0x4b7c, 0x98, 0xdc, 0x0e, 0x91, 0xa1, 0x69, 0x62, 0x86);


// static
HRESULT CDsHelper::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
   HRESULT hr = S_OK;
#ifdef _DEBUG
   IMoniker * pMoniker;
   IRunningObjectTable *pROT;
   if (FAILED(GetRunningObjectTable(0, &pROT))) {
      return E_FAIL;
   }
   WCHAR wsz[256];
   wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
   hr = CreateItemMoniker(L"!", wsz, &pMoniker);
   if (SUCCEEDED(hr)) {
      hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
      pMoniker->Release();
   }
   pROT->Release();
#endif
   return hr;
}

// static
void CDsHelper::RemoveFromRot(DWORD pdwRegister)
{
#ifdef _DEBUG
   IRunningObjectTable *pROT;
   if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
      pROT->Revoke(pdwRegister);
      pROT->Release();
   }
#endif
}


// static 
HRESULT CDsHelper::GetPin(CComPtr<IPin>& pPin, IBaseFilter *pFilter, PIN_DIRECTION PinDir, DWORD dwPinIndex) {
    if (pFilter == NULL)
        return E_POINTER;

    pPin.Release();
	CComPtr<IEnumPins> pEnum;
	DWORD dwPinsCounter = 0;

	// enumerate pins
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if(FAILED(hr)) {
		return hr;
	}

	// try to find the specific pin
	PIN_DIRECTION PinDirThis;
	while(pEnum->Next(1, &pPin, NULL) == S_OK) {
		hr = pPin->QueryDirection(&PinDirThis);
		if (SUCCEEDED(hr) && PinDir == PinDirThis) {
			if(dwPinsCounter == dwPinIndex) {
				return S_OK;
			} else {
				dwPinsCounter++;
			}
		}
		pPin.Release();
	}

	return E_FAIL;
}


// static 
HRESULT CDsHelper::GetFirstEmptyPin(CComPtr<IPin>& pPin, IBaseFilter *pFilter, PIN_DIRECTION PinDir) {
    HRESULT hr = S_OK;

    DWORD dwPinIndex = 0;
    while (SUCCEEDED(hr)) {
        hr = GetPin(pPin, pFilter, PinDir, dwPinIndex);
        if (SUCCEEDED(hr)) {
            CComPtr<IPin> pTestPin;
            if (pPin->ConnectedTo(&pTestPin) == S_OK) {
                // is not empty; try next
                pPin.Release();
                ++dwPinIndex;
            }
            else
                break;
        }
        
    }

    return hr;
}

// static
HRESULT CDsHelper::ConnectFiltersSimple(IGraphBuilder *pGraph, IBaseFilter *pFilter1, IBaseFilter *pFilter2) {
    HRESULT hr = S_OK;

    if (pGraph == NULL || pFilter1 == NULL || pFilter2 == NULL)
        return E_POINTER;

    CComPtr<IPin> pFilter1OutPin;
    if (SUCCEEDED(hr))
        hr = GetPin(pFilter1OutPin, pFilter1, PINDIR_OUTPUT, 0);

    CComPtr<IPin> pFilter2InPin;
    if (SUCCEEDED(hr))
        hr = GetPin(pFilter2InPin, pFilter2, PINDIR_INPUT, 0);

    if (SUCCEEDED(hr))
        hr = pGraph->Connect(pFilter1OutPin, pFilter2InPin);

    return hr;
}

//static 
HRESULT CDsHelper::InsertTeeAndConnect(IGraphBuilder *pGraph, IPin *pPinLeft, IPin *pPinRight, CComPtr<IPin> &pPinTee2) {
    HRESULT hr = S_OK;

    if (pGraph == NULL || pPinLeft == NULL || pPinRight == NULL)
        return E_POINTER;

    CComPtr<IBaseFilter> pTeeFilter;
    if (SUCCEEDED(hr))
        hr = CoCreateInstance(CLSID_InfTee, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pTeeFilter);

    if (SUCCEEDED(hr))
        hr = pGraph->AddFilter(pTeeFilter, L"Multiplier Tee");


    // This also (or especially) works if the pins are already connected; tee is inserted
    CComPtr<IPin> pPinLeftConnected;
    if (SUCCEEDED(hr)) {
        HRESULT hrDisconnect = pPinLeft->ConnectedTo(&pPinLeftConnected);
        if (SUCCEEDED(hrDisconnect)) {
            pGraph->Disconnect(pPinLeft);
            pGraph->Disconnect(pPinRight);
        }
    }

    
    CComPtr<IPin> pTeeFilterOut1;
    if (SUCCEEDED(hr))
        hr = GetPin(pTeeFilterOut1, pTeeFilter, PINDIR_OUTPUT, 0);


    CComPtr<IPin> pTeeFilterIn;
    if (SUCCEEDED(hr))
        hr = GetPin(pTeeFilterIn, pTeeFilter, PINDIR_INPUT, 0);

    if (SUCCEEDED(hr))
        hr = pGraph->Connect(pPinLeft, pTeeFilterIn);
    
    if (SUCCEEDED(hr))
        hr = pGraph->Connect(pTeeFilterOut1, pPinRight);


    if (SUCCEEDED(hr))
        hr = GetPin(pPinTee2, pTeeFilter, PINDIR_OUTPUT, 1);


    return hr;
}

//static 
HRESULT CDsHelper::ConnectEmptyAviMux(IGraphBuilder *pGraph, IPin *pPinToDiscard) {
    HRESULT hr = S_OK;
   
    if (pGraph == NULL || pPinToDiscard == NULL)
        return E_POINTER;

    CComPtr<IBaseFilter> pAviMuxFilter;
    if (SUCCEEDED(hr))
        hr = CoCreateInstance(CLSID_AviDest, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pAviMuxFilter);

    if (SUCCEEDED(hr))
        hr = pGraph->AddFilter(pAviMuxFilter, L"Avi Empty");

    CComPtr<IPin> pAviMuxIn;
    if (SUCCEEDED(hr))
        hr = GetPin(pAviMuxIn, pAviMuxFilter, PINDIR_INPUT, 0);

    CComPtr<IBaseFilter> pNullRenderer;
    if (SUCCEEDED(hr))
        hr = CoCreateInstance(CLSID_NullRenderer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNullRenderer);

    if (SUCCEEDED(hr))
        hr = pGraph->AddFilter(pNullRenderer, L"Null Empty");

    if (SUCCEEDED(hr))
        hr = ConnectFiltersSimple(pGraph, pAviMuxFilter, pNullRenderer);

    if (SUCCEEDED(hr))
        hr = pGraph->Connect(pPinToDiscard, pAviMuxIn);

    return hr;
}

// static 
HRESULT CDsHelper::RemoveRenderersSaveConnections(IGraphBuilder *pGraph, IPin **ppAudioPin, IPin **ppVideoPin) {
    if (pGraph == NULL)
        return E_POINTER;

    HRESULT hr = S_OK;

    CComPtr<IEnumFilters> pEnum;
    CComPtr<IBaseFilter> pFilter;

    // enumerate filters
    if (SUCCEEDED(hr))
        hr = pGraph->EnumFilters(&pEnum);

    // Remove all renderer(s) from the graph, obtain their pin connection
    while(SUCCEEDED(hr) && pEnum && (pEnum->Next(1, &pFilter, NULL) == S_OK)) {
        CComPtr<IPin> pTestOutputPin;
        hr = GetPin(pTestOutputPin, pFilter, PINDIR_OUTPUT, 0);
        if(SUCCEEDED(hr)) {
            pFilter.Release();
            continue;
        } else
            hr = S_OK; // it has no output pin and is thus a renderer

        CComPtr<IPin> pRndInputPin;
        if (SUCCEEDED(hr))
            hr = GetPin(pRndInputPin, pFilter, PINDIR_INPUT, 0); // Get the first input pin of the renderer.
        if(SUCCEEDED(hr))
        {
            CMediaType mt;
            HRESULT hrConnection = pRndInputPin->ConnectionMediaType(&mt);
            if(SUCCEEDED(hrConnection))
            {
                if(mt.majortype == MEDIATYPE_Audio) {
                    if (ppAudioPin != NULL && *ppAudioPin == NULL) { // given but not set
                        if (mt.subtype == MEDIASUBTYPE_PCM && mt.formattype == FORMAT_WaveFormatEx) {
                            hr = pRndInputPin->ConnectedTo(ppAudioPin);
                        } else {
                            ::MessageBox(NULL, _T("Unexpected: Unrecognized audio format on renderer."), NULL, MB_OK);
                        }
                    }
                } else if(mt.majortype	== MEDIATYPE_Video) {
                    if (ppVideoPin != NULL && *ppVideoPin == NULL) { // given but not set
                        if (mt.formattype == FORMAT_VideoInfo) {
                            hr = pRndInputPin->ConnectedTo(ppVideoPin);
                        } else if (mt.formattype == FORMAT_VideoInfo2) {
                            ::MessageBox(NULL, _T("Unexpected: VideoInfo2 not supported on video renderer."), NULL, MB_OK);
                        } else {
                            ::MessageBox(NULL, _T("Unexpected: Unrecognized video format on renderer."), NULL, MB_OK);
                        }
                    }
                }
            }
            // else maybe an unused renderer

            // Remove this filter from the graph.
            pGraph->RemoveFilter(pFilter);
            pFilter.Release();

            // Restart enumerator.
            pEnum->Reset();		
        }
        pFilter.Release();
    }

    return hr;
}

// static 
HRESULT CDsHelper::CreateLameMp3Filter(CComPtr<IBaseFilter>& pMp3Encoder, int iAudioKBit, DWORD nSamplesPerSec) {
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr))
        hr = CoCreateInstance(CLSID_LameMp3, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pMp3Encoder);

    CComPtr<IAudioEncoderProperties> pMp3Properties;

    // trying to configure mp3 output; this is not fatal
    HRESULT hrLameConfigure = S_OK;
    if (SUCCEEDED(hr))
        hrLameConfigure = pMp3Encoder->QueryInterface(IID_IAudioEncoderProperties, (void **)&pMp3Properties);

    if (SUCCEEDED(hrLameConfigure))
    {
        hrLameConfigure = pMp3Properties->set_Bitrate(iAudioKBit);
        //pMp3Properties->set_ChannelMode(0 = stereo, 1 = jstereo, 3 = mono); 
        // According to documentation (lame.h in CVS of lame project) this is picked automatically.
        if (SUCCEEDED(hrLameConfigure) && nSamplesPerSec > 0)
            hrLameConfigure = pMp3Properties->set_SampleRate(nSamplesPerSec);
        // Otherwise it might have a sample rate (like 22khz) configured which is then the maximum
        // used regardless of the format of the input pin.
    }

    if (FAILED(hrLameConfigure))
    {
        ::MessageBox(NULL, _T("Unexpected: Audio could not be configured. Using standard. Continuing..."), NULL, MB_OK);
    }

    return hr;
}

// static
HRESULT CDsHelper::WaitForFinishSimple(CComPtr<IGraphBuilder>& pGraph) {
    HRESULT hr = S_OK;

   CComPtr<IMediaControl> pControl;
   CComPtr<IMediaEvent>   pEvent;
   CComPtr<IMediaSeeking> pSeeking;
   CComPtr<IMediaFilter>  pFilterGeneric;
   if (SUCCEEDED(hr))
       hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
   if (SUCCEEDED(hr))
       hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
   if (SUCCEEDED(hr))
       hr = pGraph->QueryInterface(IID_IMediaSeeking, (void **)&pSeeking);
   if (SUCCEEDED(hr))
       hr = pGraph->QueryInterface(IID_IMediaFilter, (void **)&pFilterGeneric);

   // should be default; just to make sure
   if (SUCCEEDED(hr))
       hr = pSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME); 

   if (SUCCEEDED(hr))
       pFilterGeneric->SetSyncSource(NULL); 
   // otherwise you'll get the system/running time from GetCurrentPosition() below

   if (SUCCEEDED(hr))
       hr = pControl->Run();

   // Length and proper event code see Lrd2VideoConverter.cpp
   // Here no filter can deliver a current position/duration: just wait for the end
   if (SUCCEEDED(hr)) {
       long lEventCode;
       pEvent->WaitForCompletion(6000000, &lEventCode); // 100 minutes at maximum

       if (lEventCode != EC_COMPLETE)
           hr = E_UNEXPECTED;
   }

   HRESULT hrIgnore = pControl->Stop(); // finish the file

   return hr;
}